/**
 * @file scan_db.c
 * @brief Performs a full sequential scan of a .tdb file and prints all records.
 *
 * This tool demonstrates the "sequential scanning" capability of the slotted
 * page system.
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

int main(int argc, char *argv[]) {
    // --- 1. Argument Parsing ---
    if (argc != 4) {
        print_usage(argv[0]);
        return 1;
    }

    const char *tdbFile = argv[1];
    int numBuffers = atoi(argv[2]);
    int strategy = atoi(argv[3]);

    if (numBuffers <= 0) {
        fprintf(stderr, "Error: Number of buffers must be greater than 0.\n");
        return 1;
    }

    printf("--- Sequential Scan of '%s' ---\n", tdbFile);

    // --- 2. Initialization ---
    PF_Init(numBuffers, strategy);

    int tdb_fd = PF_OpenFile(tdbFile);
    if (tdb_fd < 0) {
        PF_PrintError("Error: PF_OpenFile");
        return 1;
    }

    // --- 3. Sequential Scan Loop ---
    int currentPageNum = -1; // Start from the beginning
    char *pagePtr;
    int err;
    int recordsFound = 0;

    // PF_GetNextPage is the core of the file scan
    while ((err = PF_GetNextPage(tdb_fd, &currentPageNum, &pagePtr)) == PFE_OK) {
        
        // Now, scan all records *within* this page
        int currentSlot = -1; // Start scan from the beginning of the page
        char *record;
        int recLen;

        while (SP_GetNextRecord(pagePtr, &currentSlot, &record, &recLen) == SPE_OK) {
            // Print the record (using %.*s to limit by length)
            printf("  [Page: %d, Slot: %d]: '%.*s'\n", currentPageNum, currentSlot, recLen, record);
            recordsFound++;
        }

        // We are done with this page, unfix it (not dirty)
        err = PF_UnfixPage(tdb_fd, currentPageNum, FALSE);
        if (err != PFE_OK) {
            PF_PrintError("Error: PF_UnfixPage");
            return 1;
        }
    }

    if (err != PFE_EOF) {
        // If the loop ended for any reason other than End-Of-File
        PF_PrintError("Error during scan");
    }

    // --- 4. Cleanup ---
    PF_CloseFile(tdb_fd);

    printf("--- Scan Complete: Found %d records ---\n", recordsFound);
    PF_PrintStats(); // Show buffer stats for the scan

    return 0;
}