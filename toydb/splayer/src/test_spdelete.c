/**
 * @file test_spdelete.c
 * @brief Test driver for the SP_DeleteRecordByContent function.
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

    const char *recordsToDelete[] = {
        "00001001;abhas@aero.iitb.ac.in;;",
        "00001002;jain0ua@ccs.iitb.ac.in;;",
        "00001003;naik0ua@ccs.iitb.ac.in;;"
    };
    int numRecords = sizeof(recordsToDelete) / sizeof(recordsToDelete[0]);
    int all_deletions_ok = 1;

    printf("Find and Delete Records in '%s'\n", tdbFile);
    
    PF_Init(numBuffers, strategy);

    int tdb_fd = PF_OpenFile(tdbFile);
    if (tdb_fd < 0) {
        PF_PrintError("Error: PF_OpenFile");
        return 1;
    }

    for (int i = 0; i < numRecords; i++) {
        const char *recordToFind = recordsToDelete[i];
        printf("\nRecord to delete: \"%s\"\n", recordToFind);

        int err = SP_DeleteRecordByContent(tdb_fd, recordToFind);

        if (err == SPE_OK) {
            printf("--- Deletion complete ---\n");
        } else if (err == SPE_RECORD_NOT_FOUND) {
            printf("--- Record not found, no deletion performed ---\n");
            all_deletions_ok = 0;
        } else {
            fprintf(stderr, "--- Error during deletion ---\n");
            all_deletions_ok = 0;
        }
    }
    
    printf("\n--- All deletions attempted ---\n");

    if (PF_CloseFile(tdb_fd) != PFE_OK) {
        PF_PrintError("Error: PF_CloseFile");
        return 1;
    }

    PF_PrintStats();

    // return (all_deletions_ok) ? 0 : 1;
    return 0;
}