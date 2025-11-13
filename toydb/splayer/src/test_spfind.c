/**
 * @file test_spfind.c
 * @brief Test driver for the SP_FindRecord function.
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "../../pflayer/include/pf.h"
#include "../include/sp.h"
#include "../../pflayer/include/pfbuf.h" // For STRATEGY defines

void print_usage(const char *progName) {
    fprintf(stderr, "Usage: %s <num_buffers> <strategy>\n", progName);
    fprintf(stderr, "  <strategy>: 0 for LRU, 1 for MRU\n");
    // fprintf(stderr, "  Note: Enclose the record in quotes.\n"); // Commented out
}

int main(int argc, char *argv[]) {
    if (argc != 3) { // Changed from 5 to 3
        print_usage(argv[0]);
        return 1;
    }

    const char *tdbFile = "testout.tdb"; // Hardcoded
    // const char *recordToFind = argv[2]; // Removed
    int numBuffers = atoi(argv[1]); // Changed from argv[3]
    int strategy = atoi(argv[2]); // Changed from argv[4]

    // Array of records to find
    const char *recordsToFind[] = {
        "00001001;abhas@aero.iitb.ac.in;;",
        "00001002;jain0ua@ccs.iitb.ac.in;;",
        "00001003;naik0ua@ccs.iitb.ac.in;;"
    };
    int numRecords = sizeof(recordsToFind) / sizeof(recordsToFind[0]);
    int all_finds_ok = 1; // Flag to track success

    printf("--- Finding records in '%s' ---\n", tdbFile);
    
    PF_Init(numBuffers, strategy);

    int tdb_fd = PF_OpenFile(tdbFile);
    if (tdb_fd < 0) {
        PF_PrintError("Error: PF_OpenFile");
        return 1;
    }

    for (int i = 0; i < numRecords; i++) {
        const char *recordToFind = recordsToFind[i];
        printf("\nRecord: \"%s\"\n", recordToFind);

        int pageNum, slotID;
        char* pagePtr;
        int err = SP_FindRecord(tdb_fd, recordToFind, &pageNum, &slotID, &pagePtr);

        if (err == SPE_OK) {
            printf("*** Record Found! ***\n");
            printf("  Location: Page %d, Slot %d\n", pageNum, slotID);
        } else if (err == SPE_RECORD_NOT_FOUND) {
            printf("--- Record not found ---\n");
            all_finds_ok = 0; // Mark as failed
        } else {
            fprintf(stderr, "--- Error during find ---\n");
            all_finds_ok = 0; // Mark as failed
        }
    }
    
    printf("\n--- All find operations attempted ---\n");

    PF_CloseFile(tdb_fd);
    PF_PrintStats();

    return (all_finds_ok) ? 0 : 1;
}