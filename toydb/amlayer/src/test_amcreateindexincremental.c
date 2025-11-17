/**
 * @file test_amcreateindexincremental.c
 * @brief OBJECTIVE 3 : TASK 2 : Build index incrementally from empty files.
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h> // Added for timing
#include "pf.h"
#include "sp.h"
#include "am.h"

#define DATA_BASE_NAME "data_test_inc.tdb"
#define INDEX_BASE_NAME "index_test_inc"
#define INDEX_FILE_NAME "index_test_inc.0"
#define INDEX_NO 0
#define ATTR_TYPE 'i'
#define ATTR_LEN sizeof(int)
#define NUM_RECORDS 50
#define REC_SIZE 20

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

    // Timing and User Input variables
    clock_t start_t, end_t;
    double total_t;
    char verify_choice;

    PF_Init(20,0);

    printf("TASK : Create Index Incrementally\n");
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
    
    start_t = clock(); // Start Timer
    
    // Insert records (in reverse order to test tree splits)
    for (i = NUM_RECORDS - 1; i >= 0; i--) {
        key = i;
        memset(record, 0, REC_SIZE);
        memcpy(record, &key, sizeof(int));
        sprintf(record + sizeof(int), "rec-%d", key);
        if (sp_pageBuf == NULL) {
            if (PF_AllocPage(sp_fd, &sp_pageNum, &sp_pageBuf) != PFE_OK) {
                PF_PrintError("Error allocating first data page");
                break;
            }
            SP_InitPage(sp_pageBuf); 
        }
        int slotID = SP_InsertRecord(sp_pageBuf, record, REC_SIZE);
        
        if (slotID < 0) {
            PF_UnfixPage(sp_fd, sp_pageNum, TRUE);
            
            if (PF_AllocPage(sp_fd, &sp_pageNum, &sp_pageBuf) != PFE_OK) {
                PF_PrintError("Error allocating new data page");
                break;
            }
            SP_InitPage(sp_pageBuf);
            
            slotID = SP_InsertRecord(sp_pageBuf, record, REC_SIZE);
            if (slotID < 0) {
                printf("Error: Record too big for a new page.\n");
                break;
            }
        }
        
        recId = (sp_pageNum << 16) | (slotID & 0xFFFF);
        
        int_recId = RecIdToInt(recId); 

        printf("Inserting key: %d, RECID: %d (Page %d, Slot %d)\n", key, int_recId, sp_pageNum, slotID);
        if (AM_InsertEntry(am_fd, ATTR_TYPE, ATTR_LEN, (char *)&key, int_recId) != AME_OK) {
            AM_PrintError("Error inserting entry into index");
        }
    }
    
    end_t = clock(); // End Timer
    
    printf("...Insertions complete.\n");

    // Calculate and print time
    total_t = (double)(end_t - start_t) / CLOCKS_PER_SEC;
    printf("Total time to insert all entries: %f seconds\n\n", total_t);

    if (sp_pageBuf != NULL) {
        PF_UnfixPage(sp_fd, sp_pageNum, TRUE);
    }

    PF_CloseFile(sp_fd);
    PF_CloseFile(am_fd);

    // User Prompt for Verification
    printf("Print indices to verify insertions? (y/n): ");
    scanf(" %c", &verify_choice);

    if (verify_choice == 'y' || verify_choice == 'Y')
    {
        printf("Printing Index Structure (from %s)\n", INDEX_FILE_NAME);
        am_fd = PF_OpenFile((char *)INDEX_FILE_NAME);
        if (am_fd < 0)
        {
            PF_PrintError("Error opening index file for printing");
            exit(1);
        }

        if (PF_GetFirstPage(am_fd, &rootPageNum, &pageBuf) != PFE_OK)
        {
            PF_PrintError("Error getting root page");
            PF_CloseFile(am_fd);
            exit(1);
        }
        PF_UnfixPage(am_fd, rootPageNum, FALSE);

        AM_PrintTree(am_fd, rootPageNum, ATTR_TYPE);
        PF_CloseFile(am_fd);
        printf("Create Index Incrementally : Verification Complete\n");
    }

    return 0;
}