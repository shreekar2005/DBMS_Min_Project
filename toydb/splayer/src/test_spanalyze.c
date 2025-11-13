/**
 * @file test_spanalyze.c
 * @brief Tool to analyze a .tdb file and report on space utilization.
 *
 * This is the main executable that calls the SP_AnalyzeDb function.
 */

#include <stdio.h>
#include <stdlib.h>

// Include paths are relative to this file's location (splayer/src/)
#include "../../pflayer/include/pf.h"
#include "../include/sp.h"
#include "../../pflayer/include/pfbuf.h" // For STRATEGY defines

void print_usage(const char *progName) {
    fprintf(stderr, "Usage: %s <num_buffers> <strategy>\n", progName);
    fprintf(stderr, "  <strategy>: 0 for LRU, 1 for MRU\n");
}


int main(int argc, char *argv[]) {
    // --- 1. Argument Parsing ---
    if (argc != 3) {
        print_usage(argv[0]);
        return 1;
    }

    const char *tdbFile = "testdata.tdb";
    int numBuffers = atoi(argv[1]);
    int strategy = atoi(argv[2]);

    if (numBuffers <= 0) {
        fprintf(stderr, "Error: Number of buffers must be greater than 0.\n");
        return 1;
    }
    if (strategy != STRATEGY_LRU && strategy != STRATEGY_MRU) {
        fprintf(stderr, "Error: Invalid strategy. Use 0 for LRU or 1 for MRU.\n");
        return 1;
    }

    printf("--- Initializing PF Layer for Analysis ---\n");
    printf("Database file: %s\n", tdbFile);
    printf("Buffer size:   %d pages\n", numBuffers);
    printf("Strategy:      %s\n", (strategy == STRATEGY_LRU) ? "LRU" : "MRU");
    printf("------------------------------------------\n");

    // --- 2. Initialization ---
    PF_Init(numBuffers, strategy);

    int tdb_fd = PF_OpenFile(tdbFile);
    if (tdb_fd < 0) {
        PF_PrintError("Error: PF_OpenFile");
        return 1;
    }

    // --- 3. Call the high-level analysis function ---
    int err = SP_AnalyzeDb(tdb_fd);
    if (err != SPE_OK) {
        // SPanalyzeDb already prints errors via PF_PrintError
        fprintf(stderr, "Analysis failed.\n");
    }

    // --- 4. Cleanup ---
    if (PF_CloseFile(tdb_fd) != PFE_OK) {
        PF_PrintError("Error: PF_CloseFile");
    }

    // Print statistics before quitting
    PF_PrintStats();
    
    return (err == SPE_OK) ? 0 : 1;
}