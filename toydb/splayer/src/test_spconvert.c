/**
 * @file test_spconvert.c
 * @brief Test driver for the SP_ConvertTxtToTdb function.
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "../../pflayer/include/pf.h"
#include "../include/sp.h"
#include "../../pflayer/include/pfbuf.h" // For STRATEGY defines

/**
 * @brief Prints usage instructions and exits.
 * @param progName The name of the program (argv[0]).
 */
void print_usage(const char *progName)
{
    fprintf(stderr, "Usage: %s <num_buffers> <strategy>\n", progName);
    fprintf(stderr, "  <strategy>: 0 for LRU, 1 for MRU\n");
}

int main(int argc, char *argv[])
{
    if (argc != 3) {
        print_usage(argv[0]);
        return 1;
    }

    const char *inputTxtFile = "testin.txt";
    const char *outputTdbFile = "testout.tdb";
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

    printf("--- Initializing PF Layer ---\n");
    printf("Input file:   %s\n", inputTxtFile);
    printf("Output file:  %s\n", outputTdbFile);
    printf("Buffer size:  %d pages\n", numBuffers);
    printf("Strategy:     %s\n", (strategy == STRATEGY_LRU) ? "LRU" : "MRU");
    printf("---------------------------\n");

    PF_Init(numBuffers, strategy);

    // Call the library function to do the work
    int err = SP_ConvertTxtToTdb(inputTxtFile, outputTdbFile);
    if (err != SPE_OK) {
        fprintf(stderr, "Error: Conversion failed.\n");
    }

    PF_PrintStats();
    return (err == SPE_OK) ? 0 : 1;
}