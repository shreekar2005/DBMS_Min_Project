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
    fprintf(stderr, "Usage: %s <tdb_file> \"<record_to_find>\" <num_buffers> <strategy>\n", progName);
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

    printf("--- Finding record in '%s' ---\n", tdbFile);
    printf("Record: \"%s\"\n", recordToFind);

    PF_Init(numBuffers, strategy);

    int tdb_fd = PF_OpenFile(tdbFile);
    if (tdb_fd < 0) {
        PF_PrintError("Error: PF_OpenFile");
        return 1;
    }

    int pageNum, slotID;
    int err = SP_FindRecord(tdb_fd, recordToFind, &pageNum, &slotID);

    if (err == SPE_OK) {
        printf("\n*** Record Found! ***\n");
        printf("  Location: Page %d, Slot %d\n\n", pageNum, slotID);
    } else if (err == SPE_RECORD_NOT_FOUND) {
        printf("\n--- Record not found ---\n");
    } else {
        fprintf(stderr, "\n--- Error during find ---\n");
    }

    PF_CloseFile(tdb_fd);
    PF_PrintStats();

    return (err == SPE_OK) ? 0 : 1;
}