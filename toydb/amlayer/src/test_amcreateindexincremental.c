/**
 * @file test_amcreateindexincremental.c
 * @brief Test Case 2: Build index incrementally from empty files.
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "pf.h"
#include "sp.h"
#include "am.h" // Includes testam.h for RecIdType and RecIdToInt

#define DATA_BASE_NAME "data_test_inc.tdb"
#define INDEX_BASE_NAME "index_test_inc"
#define INDEX_FILE_NAME "index_test_inc.0"
#define INDEX_NO 0
#define ATTR_TYPE 'i'
#define ATTR_LEN sizeof(int)
#define NUM_RECORDS 50  // Number of records to insert
#define REC_SIZE 20     // Must match splayer testconvert

int main()
{
    int sp_fd, am_fd;
    RecIdType recId; // This is just an int, per testam.h
    char record[REC_SIZE];
    int key;
    int int_recId;
    int i;
    int rootPageNum;
    char *pageBuf;
    
    int sp_pageNum = -1; // Current page number for splayer file
    char *sp_pageBuf = NULL; // Buffer for current splayer page

    // Initialize layers
    PF_Init(20,0);

    printf("--- Test Case 2: Create Index Incrementally ---\n");
    printf("Creating Data File: %s\n", DATA_BASE_NAME);
    printf("Creating Index File: %s\n\n", INDEX_FILE_NAME);

    // Create and open the data file
    if (PF_CreateFile((char *)DATA_BASE_NAME) != PFE_OK) {
        PF_PrintError("Error creating data file");
        exit(1);
    }
    sp_fd = PF_OpenFile((char *)DATA_BASE_NAME);
    if (sp_fd < 0) {
        PF_PrintError("Error opening data file");
        exit(1);
    }

    // Create and open the index file
    if (AM_CreateIndex((char *)INDEX_BASE_NAME, INDEX_NO, ATTR_TYPE, ATTR_LEN) != AME_OK) {
        AM_PrintError("Error creating index");
        PF_CloseFile(sp_fd);
        exit(1);
    }
    am_fd = PF_OpenFile((char *)INDEX_FILE_NAME);
    if (am_fd < 0) {
        PF_PrintError("Error opening index file");
        PF_CloseFile(sp_fd);
        exit(1);
    }

    printf("Inserting %d records into data file and index...\n", NUM_RECORDS);
    // Insert records (in reverse order to test tree splits)
    for (i = NUM_RECORDS - 1; i >= 0; i--) {
        key = i;
        // Create a record (key followed by other data)
        memset(record, 0, REC_SIZE);
        memcpy(record, &key, sizeof(int));
        sprintf(record + sizeof(int), "rec-%d", key);

        // --- Logic to insert into splayer file ---
        if (sp_pageBuf == NULL) { // Need to allocate the first page
            if (PF_AllocPage(sp_fd, &sp_pageNum, &sp_pageBuf) != PFE_OK) {
                PF_PrintError("Error allocating first data page");
                break;
            }
            SP_InitPage(sp_pageBuf); // Initialize as slotted page
        }

        // 1. Try to insert record into current splayer page
        int slotID = SP_InsertRecord(sp_pageBuf, record, REC_SIZE);
        
        if (slotID < 0) { // Page is full
            // Unfix the full page
            PF_UnfixPage(sp_fd, sp_pageNum, TRUE);
            
            // Allocate a new page
            if (PF_AllocPage(sp_fd, &sp_pageNum, &sp_pageBuf) != PFE_OK) {
                PF_PrintError("Error allocating new data page");
                break;
            }
            SP_InitPage(sp_pageBuf); // Initialize as slotted page
            
            // Retry insertion on the new page
            slotID = SP_InsertRecord(sp_pageBuf, record, REC_SIZE);
            if (slotID < 0) {
                printf("Error: Record too big for a new page.\n");
                break;
            }
        }
        
        // At this point, record is inserted into sp_pageNum at slotID
        
        // **FIX**: Pack pageNum and slotID into a single int
        recId = (sp_pageNum << 16) | (slotID & 0xFFFF);
        
        // RecIdToInt macro just returns the int (per testam.h)
        int_recId = RecIdToInt(recId); 

        // 2. Insert into index file
        printf("Inserting key: %d, RECID: %d (Page %d, Slot %d)\n", key, int_recId, sp_pageNum, slotID);
        if (AM_InsertEntry(am_fd, ATTR_TYPE, ATTR_LEN, (char *)&key, int_recId) != AME_OK) {
            AM_PrintError("Error inserting entry into index");
        }
    }
    printf("...Insertions complete.\n\n");

    // Unfix the last data page if it's dirty
    if (sp_pageBuf != NULL) {
        PF_UnfixPage(sp_fd, sp_pageNum, TRUE);
    }

    // Close files
    PF_CloseFile(sp_fd);
    PF_CloseFile(am_fd);

    // Print the index to verify
    printf("--- Printing Index Structure (from %s) ---\n", INDEX_FILE_NAME);
    am_fd = PF_OpenFile((char *)INDEX_FILE_NAME);
    if (am_fd < 0)
    {
        PF_PrintError("Error opening index file for printing");
        exit(1);
    }

    // Get the root page number
    if (PF_GetFirstPage(am_fd, &rootPageNum, &pageBuf) != PFE_OK)
    {
        PF_PrintError("Error getting root page");
        PF_CloseFile(am_fd);
        exit(1);
    }
    PF_UnfixPage(am_fd, rootPageNum, FALSE);

    AM_PrintTree(am_fd, rootPageNum, ATTR_TYPE);
    PF_CloseFile(am_fd);
    printf("--- Test Case 2 Complete ---\n");

    return 0;
}