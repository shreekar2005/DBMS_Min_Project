/**
 * @file util_db.c
 * @brief Analyzes a .tdb file and reports on space utilization,
 * comparing slotted pages vs. static record management.
 *
 * This tool directly addresses the performance metrics part of Objective 2.
 */

#include <stdio.h>
#include <stdlib.h>

// Include paths are relative to this file's location (splayer/src/)
#include "../../pflayer/include/pf.h"
#include "../include/sp.h"

void print_usage(const char *progName) {
    fprintf(stderr, "Usage: %s <tdb_file> <num_buffers> <strategy>\n", progName);
    fprintf(stderr, "  <strategy>: 0 for LRU, 1 for MRU\n");
}

/**
 * @brief Helper to print one row of the results table
 */
void print_table_row(const char* method, long dataBytes, long totalBytes, int pages) {
    double utilization = 0.0;
    if (totalBytes > 0) {
        utilization = ((double)dataBytes / (double)totalBytes) * 100.0;
    }
    printf("| %-20s | %12ld | %10d | %15.2f%% |\n", method, totalBytes, pages, utilization);
}

int main(int argc, char *argv[]) {
    // --- 1. Argument Parsing ---
    if (argc != 4) {
        print_usage(argv[0]);
        return 1;
    }

    const char *tdbFile = argv[1];
    int numBuffers = atoi(argv[2]);
    int strategy = atoi(argv[3]);

    printf("--- Analyzing space utilization for '%s' ---\n", tdbFile);

    // --- 2. Initialization ---
    PF_Init(numBuffers, strategy);

    int tdb_fd = PF_OpenFile(tdbFile);
    if (tdb_fd < 0) {
        PF_PrintError("Error: PF_OpenFile");
        return 1;
    }

    // --- 3. Scan and Collect Statistics ---
    int currentPageNum = -1;
    char *pagePtr;
    int err;

    long total_records = 0;
    long total_data_bytes = 0; // Just the size of the records
    long total_pages_used = 0;
    long total_slot_bytes = 0;
    long total_header_bytes = 0;

    while ((err = PF_GetNextPage(tdb_fd, &currentPageNum, &pagePtr)) == PFE_OK) {
        total_pages_used++;
        total_header_bytes += sizeof(PageHeader);
        
        int currentSlot = -1;
        char *record;
        int recLen;

        while (SP_GetNextRecord(pagePtr, &currentSlot, &record, &recLen) == SPE_OK) {
            total_records++;
            total_data_bytes += recLen;
            total_slot_bytes += sizeof(Slot);
        }

        PF_UnfixPage(tdb_fd, currentPageNum, FALSE);
    }

    if (err != PFE_EOF) {
        PF_PrintError("Error during scan");
        PF_CloseFile(tdb_fd);
        return 1;
    }

    PF_CloseFile(tdb_fd);

    // --- 4. Calculate and Print Report ---
    printf("\n--- Slotted Page Statistics ---\n");
    printf("Total Records Found:   %ld\n", total_records);
    printf("Total Pages Used:      %ld\n", total_pages_used);
    printf("Total Data Bytes:      %ld\n", total_data_bytes);
    printf("Total Slot Overhead:   %ld\n", total_slot_bytes);
    printf("Total Header Overhead: %ld\n", total_header_bytes);
    long slotted_total_bytes = total_pages_used * PF_PAGE_SIZE;
    
    printf("\n--- Space Utilization Comparison Table ---\n");
    printf("| Method               | Total Bytes  | Total Pages | Utilization   |\n");
    printf("|----------------------|--------------|-------------|---------------|\n");
    
    // 1. Slotted Page (Our method)
    print_table_row("Slotted Pages", total_data_bytes, slotted_total_bytes, total_pages_used);

    // 2. Static Management (Comparison)
    int static_lengths[] = {50, 100, 250}; // Compare against these static lengths
    int i;
    for (i = 0; i < sizeof(static_lengths)/sizeof(int); i++) {
        int static_len = static_lengths[i];
        if (static_len == 0) continue;
        
        int records_per_page = PF_PAGE_SIZE / static_len;
        if (records_per_page == 0) continue; // Record too big
        
        // Calculate pages needed (use ceiling division)
        long static_pages = (total_records + records_per_page - 1) / records_per_page;
        long static_total_bytes = static_pages * PF_PAGE_SIZE;
        
        char method_name[30];
        sprintf(method_name, "Static (%d bytes)", static_len);
        print_table_row(method_name, total_data_bytes, static_total_bytes, static_pages);
    }
    printf("|----------------------|--------------|-------------|---------------|\n");

    return 0;
}