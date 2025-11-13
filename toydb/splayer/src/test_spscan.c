/**
 * @file test_spscan.c
 * @brief Test driver for the SP_ScanDb function.
 */

#include <stdio.h>
#include <stdlib.h>
#include "../../pflayer/include/pf.h"
#include "../include/sp.h"
#include "../../pflayer/include/pfbuf.h" // For STRATEGY defines

void print_usage(const char *progName) {
    fprintf(stderr, "Usage: %s <file.tdb> <num_buffers> <strategy>\n", progName);
    fprintf(stderr, "  <strategy>: 0 for LRU, 1 for MRU\n");
}

int main(int argc, char *argv[]) {
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

    PF_Init(numBuffers, strategy);

    int tdb_fd = PF_OpenFile(tdbFile);
    if (tdb_fd < 0) {
        PF_PrintError("Error: PF_OpenFile");
        return 1;
    }

    // Call the library function
    int recordsFound = SP_ScanDb(tdb_fd);
    if (recordsFound < 0) {
        fprintf(stderr, "Error: Scan failed.\n");
    }

    PF_CloseFile(tdb_fd);

    printf("--- Scan Complete: Found %d records ---\n", (recordsFound < 0) ? 0 : recordsFound);
    PF_PrintStats();

    return (recordsFound < 0) ? 1 : 0;
}