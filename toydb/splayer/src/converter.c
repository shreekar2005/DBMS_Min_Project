/**
 * @file converter.c
 * @brief Converts a text file (e.g., from /data) into a .tdb file
 * using the slotted page layout.
 *
 * This program demonstrates the use of the PF and SP layers to build
 * a database file from a raw text file.
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// Include paths are relative to this file's location (splayer/src/)
#include "../../pflayer/include/pf.h"
#include "../include/sp.h"
#include "../../pflayer/include/pfbuf.h"

#define MAX_LINE_LENGTH 1024 // Max characters per line in .txt file

/**
 * @brief Prints usage instructions and exits.
 * @param progName The name of the program (argv[0]).
 */
void print_usage(const char *progName) {
    fprintf(stderr, "Usage: %s <input_txt_file> <output_tdb_file> <num_buffers> <strategy>\n", progName);
    fprintf(stderr, "  <strategy>: 0 for LRU, 1 for MRU\n");
}

int main(int argc, char *argv[]) {
    // --- 1. Argument Parsing ---
    if (argc != 5) {
        print_usage(argv[0]);
        return 1;
    }

    const char *inputTxtFile = argv[1];
    const char *outputTdbFile = argv[2];
    int numBuffers = atoi(argv[3]);
    int strategy = atoi(argv[4]);

    if (numBuffers <= 0) {
        fprintf(stderr, "Error: Number of buffers must be greater than 0.\n");
        return 1;
    }
    if (strategy != STRATEGY_LRU && strategy != STRATEGY_MRU) {
        fprintf(stderr, "Error: Invalid strategy. Use 0 for LRU or 1 for MRU.\n");
        return 1;
    }

    printf("--- Starting Conversion ---\n");
    printf("Input file:   %s\n", inputTxtFile);
    printf("Output file:  %s\n", outputTdbFile);
    printf("Buffer size:  %d pages\n", numBuffers);
    printf("Strategy:     %s\n", (strategy == STRATEGY_LRU) ? "LRU" : "MRU");
    printf("---------------------------\n");

    // --- 2. Initialization ---
    // This call integrates Objective 1!
    PF_Init(numBuffers, strategy);

    FILE *txtFile = fopen(inputTxtFile, "r");
    if (txtFile == NULL) {
        perror("Error opening input text file");
        return 1;
    }

    // Create and open the new database file
    int err = PF_CreateFile(outputTdbFile);
    if (err != PFE_OK) {
        PF_PrintError("Error: PF_CreateFile");
        fclose(txtFile);
        return 1;
    }

    int tdb_fd = PF_OpenFile(outputTdbFile);
    if (tdb_fd < 0) {
        PF_PrintError("Error: PF_OpenFile");
        fclose(txtFile);
        return 1;
    }

    // --- 3. Core Conversion Loop ---
    char lineBuffer[MAX_LINE_LENGTH];
    char *currentPagePtr = NULL;
    int currentPageNum = -1;
    int recordsInserted = 0;
    int pagesUsed = 0;

    // Get the very first page
    err = PF_AllocPage(tdb_fd, &currentPageNum, &currentPagePtr);
    if (err != PFE_OK) {
        PF_PrintError("Error: PF_AllocPage (first)");
        return 1;
    }
    SP_InitPage(currentPagePtr); // Initialize it as a slotted page
    pagesUsed++;

    // Read input file line by line
    while (fgets(lineBuffer, sizeof(lineBuffer), txtFile) != NULL) {
        // Remove trailing newline character
        lineBuffer[strcspn(lineBuffer, "\n")] = 0;
        int recLen = strlen(lineBuffer);

        if (recLen == 0) continue; // Skip empty lines

        // Try to insert the record. If the page is full, get a new one.
        while (SP_InsertRecord(currentPagePtr, lineBuffer, recLen) == SPE_PAGE_FULL) {
            
            // Current page is full. Unfix it (marking it dirty).
            err = PF_UnfixPage(tdb_fd, currentPageNum, TRUE);
            if (err != PFE_OK) {
                PF_PrintError("Error: PF_UnfixPage");
                return 1;
            }

            // Allocate a new page
            err = PF_AllocPage(tdb_fd, &currentPageNum, &currentPagePtr);
            if (err != PFE_OK) {
                PF_PrintError("Error: PF_AllocPage (new)");
                return 1;
            }
            SP_InitPage(currentPagePtr); // Initialize the new page
            pagesUsed++;
        }
        
        // At this point, the record was successfully inserted
        recordsInserted++;
    }

    // --- 4. Cleanup ---
    printf("\n--- Conversion Complete ---\n");
    printf("Total Records Inserted: %d\n", recordsInserted);
    printf("Total Pages Used:       %d\n", pagesUsed);
    printf("---------------------------\n");

    // Unfix the last page we were working on
    if (currentPagePtr != NULL) {
        err = PF_UnfixPage(tdb_fd, currentPageNum, TRUE);
        if (err != PFE_OK) {
            PF_PrintError("Error: PF_UnfixPage (last)");
        }
    }

    // Close files
    err = PF_CloseFile(tdb_fd);
    if (err != PFE_OK) {
        PF_PrintError("Error: PF_CloseFile");
    }

    fclose(txtFile);

    // Print the buffer statistics!
    PF_PrintStats();

    return 0;
}