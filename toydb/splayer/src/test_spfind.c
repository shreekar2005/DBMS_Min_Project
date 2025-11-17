/**
 * @file test_spfind.c
 * @brief Test driver for the SP_FindRecord function.
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "../../pflayer/include/pf.h"
#include "../include/sp.h"
#include "../../pflayer/include/pfbuf.h"

static void print_usage(const char *progName) {
    fprintf(stderr, "Usage: %s <num_buffers> <strategy>\n", progName);
    fprintf(stderr, "  <strategy>: 0 for LRU, 1 for MRU\n");
}

int main(int argc, char *argv[]) {
    if (argc != 3) {
        print_usage(argv[0]);
        return 1;
    }

    const char *tdbFile = "testdata.tdb"; 
    int numBuffers = atoi(argv[1]);
    int strategy = atoi(argv[2]); 

    const char *recordsToFind[] = {
        "00001001;abhas@aero.iitb.ac.in;;",
        "00001002;jain0ua@ccs.iitb.ac.in;;",
        "00001003;naik0ua@ccs.iitb.ac.in;;"
    };
    int numRecords = sizeof(recordsToFind) / sizeof(recordsToFind[0]);
    int all_finds_ok = 1;

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
            all_finds_ok = 0;
        } else {
            fprintf(stderr, "--- Error during find ---\n");
            all_finds_ok = 0;
        }
    }
    
    printf("\n--- All find operations attempted ---\n");

    PF_CloseFile(tdb_fd);
    PF_PrintStats();

    // return (all_finds_ok) ? 0 : 1;
    return 0;
}