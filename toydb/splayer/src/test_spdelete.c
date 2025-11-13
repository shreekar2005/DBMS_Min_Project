/**
 * @file test_spdelete.c
 * @brief Test driver for the SP_DeleteRecordByContent function.
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "../../pflayer/include/pf.h"
#include "../include/sp.h"
#include "../../pflayer/include/pfbuf.h" // For STRATEGY defines

/**
 * @brief Prints the correct command-line usage instructions.
 * @param progName The name of the program (argv[0]).
 */
void print_usage(const char *progName) {
    fprintf(stderr, "Usage: %s <tdb_file> \"<record_to_delete>\" <num_buffers> <strategy>\n", progName);
    fprintf(stderr, "  <strategy>: 0 for LRU, 1 for MRU\n");
    fprintf(stderr, "  Note: Enclose the record in quotes.\n");
}

int main(int argc, char *argv[]) {
    if (argc != 5) {
        print_usage(argv[0]);
        return 1;
    }

    const char *tdbFile = argv[1];
    const char *recordToFind = argv[2];
    int numBuffers = atoi(argv[3]);
    int strategy = atoi(argv[4]);

    printf("--- Find and Delete Record in '%s' ---\n", tdbFile);
    printf("Record to delete: \"%s\"\n", recordToFind);

    PF_Init(numBuffers, strategy);

    int tdb_fd = PF_OpenFile(tdbFile);
    if (tdb_fd < 0) {
        PF_PrintError("Error: PF_OpenFile");
        return 1;
    }

    int err = SP_DeleteRecordByContent(tdb_fd, recordToFind);

    if (err == SPE_OK) {
        printf("--- Deletion complete ---\n");
    } else if (err == SPE_RECORD_NOT_FOUND) {
        printf("\n--- Record not found, no deletion performed ---\n");
    } else {
        fprintf(stderr, "\n--- Error during deletion ---\n");
    }
    
    if (PF_CloseFile(tdb_fd) != PFE_OK) {
        PF_PrintError("Error: PF_CloseFile");
        return 1;
    }

    PF_PrintStats();

    return (err == SPE_OK) ? 0 : 1;
}