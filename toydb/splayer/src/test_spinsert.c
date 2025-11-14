/**
 * @file test_spinsert.c
 * @brief Test driver for the SP_InsertRecordByContent function.
 *
 * Assumes 'testdata.tdb' already exists (e.g., from 'make testconvert').
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

    const char *newRecord = "99999999;tavi@iitb.ac.in;My New Record;;";
    int recLen = strlen(newRecord);

    printf("--- Insert Record into '%s' ---\n", tdbFile);
    
    PF_Init(numBuffers, strategy);

    int tdb_fd = PF_OpenFile(tdbFile);
    if (tdb_fd < 0) {
        PF_PrintError("Error: PF_OpenFile");
        return 1;
    }

    printf("Inserting record: \"%s\" (length %d)\n", newRecord, recLen);

    int err = SP_InsertRecordByContent(tdb_fd, newRecord, recLen);

    if (err == SPE_OK) {
        printf("--- Insertion complete ---\n");
    } else {
        printf("--- Insertion FAILED (err code: %d) ---\n", err);
    }

    if (PF_CloseFile(tdb_fd) != PFE_OK) {
        PF_PrintError("Error: PF_CloseFile");
    }

    printf("--- Test finished ---\n");
    return (err == SPE_OK) ? 0 : 1;
}