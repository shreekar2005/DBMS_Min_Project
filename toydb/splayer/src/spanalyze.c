/**
 * @file spanalyze.c
 * @brief Implementation of the database analysis helper.
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "../../pflayer/include/pf.h"
#include "../include/sp.h"
#include "../include/spanalyze.h"

/**
 * @brief Helper to print one row of the results table
 */
static void print_table_row(const char* method, long dataBytes, long totalBytes, int pages) {
    double utilization = 0.0;
    if (totalBytes > 0) {
        utilization = ((double)dataBytes / (double)totalBytes) * 100.0;
    }
    printf("| %-20s | %12ld | %10d | %15.2f%% |\n", method, totalBytes, pages, utilization);
}


/**
 * @brief Internal helper to scan and analyze a .tdb file.
 */
int SPanalyzeDb(int tdb_fd) {
    int currentPageNum = -1;
    char *pagePtr;
    int err;

    long total_records = 0;
    long total_data_bytes = 0;
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
        return err; 
    }

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
    
    print_table_row("Slotted Pages", total_data_bytes, slotted_total_bytes, total_pages_used);

    int static_lengths[] = {50, 100, 250};
    int i;
    for (i = 0; i < sizeof(static_lengths)/sizeof(int); i++) {
        int static_len = static_lengths[i];
        if (static_len == 0) continue;
        
        int records_per_page = PF_PAGE_SIZE / static_len;
        if (records_per_page == 0) continue;
        
        long static_pages = (total_records + records_per_page - 1) / records_per_page;
        long static_total_bytes = static_pages * PF_PAGE_SIZE;
        
        char method_name[30];
        sprintf(method_name, "Static (%d bytes)", static_len);
        print_table_row(method_name, total_data_bytes, static_total_bytes, static_pages);
    }
    printf("|----------------------|--------------|-------------|---------------|\n");

    return SPE_OK;
}