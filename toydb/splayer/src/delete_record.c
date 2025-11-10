/**
 * @file delete_record.c
 * @brief Finds a specific record by content, deletes it, and then
 * performs a full scan to verify the deletion.
 *
 * This tool demonstrates the "deletion" capability of the slotted page system.
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// Include paths are relative to this file's location (splayer/src/)
#include "../../pflayer/include/pf.h"
#include "../include/sp.h"

void print_usage(const char *progName) {
    fprintf(stderr, "Usage: %s <tdb_file> \"<record_to_delete>\" <num_buffers> <strategy>\n", progName);
    fprintf(stderr, "  <strategy>: 0 for LRU, 1 for MRU\n");
    fprintf(stderr, "  Note: Enclose the record in quotes.\n");
}

int main(int argc, char *argv[]) {
    // --- 1. Argument Parsing ---
    if (argc != 5) {
        print_usage(argv[0]);
        return 1;
    }

    const char *tdbFile = argv[1];
    const char *recordToFind = argv[2];
    int lenToFind = strlen(recordToFind);
    int numBuffers = atoi(argv[3]);
    int strategy = atoi(argv[4]);

    printf("--- Find and Delete Record in '%s' ---\n", tdbFile);
    printf("Record to delete: \"%s\"\n", recordToFind);

    // --- 2. Initialization ---
    PF_Init(numBuffers, strategy);

    int tdb_fd = PF_OpenFile(tdbFile);
    if (tdb_fd < 0) {
        PF_PrintError("Error: PF_OpenFile");
        return 1;
    }

    // --- 3. Scan and Find Loop ---
    int currentPageNum = -1;
    char *pagePtr;
    int err;
    int found = 0;

    while ((err = PF_GetNextPage(tdb_fd, &currentPageNum, &pagePtr)) == PFE_OK) {
        int currentSlot = -1;
        char *record;
        int recLen;

        while (SP_GetNextRecord(pagePtr, &currentSlot, &record, &recLen) == SPE_OK) {
            // Check if this is the record we want
            if (recLen == lenToFind && strncmp(record, recordToFind, recLen) == 0) {
                printf("\n*** Record Found at [Page: %d, Slot: %d]. Deleting... ***\n", currentPageNum, currentSlot);
                found = 1;

                // Delete the record
                err = SP_DeleteRecord(pagePtr, currentSlot);
                if (err != SPE_OK) {
                     printf("Error calling SP_DeleteRecord!\n");
                }
                
                // Unfix the page, MARKING IT DIRTY
                err = PF_UnfixPage(tdb_fd, currentPageNum, TRUE);
                if (err != PFE_OK) {
                    PF_PrintError("Error: PF_UnfixPage (dirty)");
                }
                goto cleanup_find; // Exit both loops
            }
        }

        // Record not found on this page, unfix (not dirty)
        err = PF_UnfixPage(tdb_fd, currentPageNum, FALSE);
        if (err != PFE_OK) {
            PF_PrintError("Error: PF_UnfixPage (clean)");
            goto cleanup_find;
        }
    }

cleanup_find:
    if (!found && err == PFE_EOF) {
        printf("\n--- Record not found, no deletion performed ---\n");
    } else if (!found) {
        PF_PrintError("Error during scan");
    }

    // Close the file to ensure the dirty page is flushed to disk
    if (PF_CloseFile(tdb_fd) != PFE_OK) {
        PF_PrintError("Error: PF_CloseFile");
        return 1;
    }

    if (!found) {
        return 1; // Exit if we didn't find the record
    }

    // --- 4. Verification Scan ---
    // Re-open the file and scan to prove deletion
    printf("\n--- Verification Scan (record should be missing) ---\n");
    
    tdb_fd = PF_OpenFile(tdbFile);
    if (tdb_fd < 0) {
        PF_PrintError("Error: PF_OpenFile (for verification)");
        return 1;
    }

    currentPageNum = -1; // Start from the beginning
    int recordsFound = 0;

    while ((err = PF_GetNextPage(tdb_fd, &currentPageNum, &pagePtr)) == PFE_OK) {
        int currentSlot = -1;
        char *record;
        int recLen;

        while (SP_GetNextRecord(pagePtr, &currentSlot, &record, &recLen) == SPE_OK) {
            printf("  [Page: %d, Slot: %d]: '%.*s'\n", currentPageNum, currentSlot, recLen, record);
            recordsFound++;
        }
        
        PF_UnfixPage(tdb_fd, currentPageNum, FALSE);
    }

    if (err != PFE_EOF) {
        PF_PrintError("Error during verification scan");
    }

    PF_CloseFile(tdb_fd);
    printf("--- Verification Scan Complete: Found %d records ---\n", recordsFound);
    PF_PrintStats();

    return 0;
}