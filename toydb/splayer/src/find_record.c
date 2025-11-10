/**
 * @file find_record.c
 * @brief Scans a .tdb file for an exact record and reports its location.
 *
 * This tool demonstrates finding a specific record by its content.
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// Include paths are relative to this file's location (splayer/src/)
#include "../../pflayer/include/pf.h"
#include "../include/sp.h"

void print_usage(const char *progName) {
    fprintf(stderr, "Usage: %s <tdb_file> \"<record_to_find>\" <num_buffers> <strategy>\n", progName);
    fprintf(stderr, "  <strategy>: 0 for LRU, 1 for MRU\n");
    fprintf(stderr, "  Note: Enclose the record in quotes.\n");
}

int main(int argc, char *argv[]) {
    // --- 1. Argument Parsing ---
    if (argc != 5) {
        print_usage(argv[0]);
        return 1;
    }

    const char *tdbFile = argv[1];
    const char *recordToFind = argv[2];
    int lenToFind = strlen(recordToFind);
    int numBuffers = atoi(argv[3]);
    int strategy = atoi(argv[4]);

    printf("--- Finding record in '%s' ---\n", tdbFile);
    printf("Record: \"%s\"\n", recordToFind);

    // --- 2. Initialization ---
    PF_Init(numBuffers, strategy);

    int tdb_fd = PF_OpenFile(tdbFile);
    if (tdb_fd < 0) {
        PF_PrintError("Error: PF_OpenFile");
        return 1;
    }

    // --- 3. Scan and Find Loop ---
    int currentPageNum = -1;
    char *pagePtr;
    int err;
    int found = 0;

    while ((err = PF_GetNextPage(tdb_fd, &currentPageNum, &pagePtr)) == PFE_OK) {
        int currentSlot = -1;
        char *record;
        int recLen;

        while (SP_GetNextRecord(pagePtr, &currentSlot, &record, &recLen) == SPE_OK) {
            // Check if this is the record we want
            if (recLen == lenToFind && strncmp(record, recordToFind, recLen) == 0) {
                printf("\n*** Record Found! ***\n");
                printf("  Location: Page %d, Slot %d\n\n", currentPageNum, currentSlot);
                found = 1;

                // Unfix this page and break from all loops
                PF_UnfixPage(tdb_fd, currentPageNum, FALSE);
                goto cleanup; // Exit loops
            }
        }

        // Unfix the page (not dirty)
        PF_UnfixPage(tdb_fd, currentPageNum, FALSE);
    }

cleanup:
    if (!found && err == PFE_EOF) {
        printf("\n--- Record not found ---\n");
    } else if (!found) {
        PF_PrintError("Error during scan");
    }

    // --- 4. Cleanup ---
    PF_CloseFile(tdb_fd);
    PF_PrintStats();

    return 0;
}