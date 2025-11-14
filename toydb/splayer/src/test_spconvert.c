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


static int replace_ext_to_tdb_stdlib(const char* input, char* output, int max_len) {
    const char* last_dot = strrchr(input, '.');
    int base_len;
    if (last_dot != NULL && last_dot != input) {
        base_len = last_dot - input;
    } else {
        base_len = strlen(input);
    }
    if (base_len + 5 > max_len) {
        if (max_len > 0) output[0] = '\0';
        return -1;
    }
    strncpy(output, input, base_len);
    output[base_len] = '.';
    output[base_len + 1] = 't';
    output[base_len + 2] = 'd';
    output[base_len + 3] = 'b';
    output[base_len + 4] = '\0';
    return 0;
}


static void print_usage(const char *progName)
{
    fprintf(stderr, "Usage: %s <file.txt> <num_buffers> <strategy>\n", progName);
    fprintf(stderr, "  <strategy>: 0 for LRU, 1 for MRU\n");
}

int main(int argc, char *argv[])
{
    if (argc != 4) {
        print_usage(argv[0]);
        return 1;
    }

    const char *inputTxtFile = argv[1];
    
    char outputTdbFilename[256];
    if (replace_ext_to_tdb_stdlib(inputTxtFile, outputTdbFilename, 256) != 0) {
        fprintf(stderr, "Error: Output filename buffer is too small.\n");
        return 1;
    }

    const char *outputTdbFile = outputTdbFilename;
    int numBuffers = atoi(argv[2]);
    int strategy = atoi(argv[3]);

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

    int err = SP_ConvertTxtToTdb(inputTxtFile, outputTdbFile);
    if (err != SPE_OK) {
        fprintf(stderr, "Error: Conversion failed.\n");
    }

    PF_PrintStats();
    return (err == SPE_OK) ? 0 : 1;
}