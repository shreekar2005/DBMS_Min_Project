/**
 * @file test_amcreateindex.c
 * @brief OBJECTIVE 3 : TASK 1 : Build index from an existing unsorted data file.
 */

#include <stdio.h>
#include <stdlib.h>
#include "pf.h"
#include "sp.h"
#include "am.h"

#define DATA_FILE "testdata_unsorted.tdb"
#define INDEX_BASE_NAME "index_test_create"
#define INDEX_FILE_NAME "index_test_create.0"
#define INDEX_NO 0
#define ATTR_TYPE 'i'
#define ATTR_LEN sizeof(int)

int main()
{
    int sp_fd, am_fd;
    int key;
    int int_recId;
    int rootPageNum;
    char *pageBuf;
    int pf_err;
    int pageNum = -1;
    char *pageBuf_sp; // Buffer for splayer pages
    RecIdType recId;  // This is just an int, per testam.h

    PF_Init(20,0); // 20 buffers, LRU strategy

    printf("TASK : Create Index from Existing File\n");
    printf("Using Data File: %s\n", DATA_FILE);
    printf("Creating Index File: %s\n\n", INDEX_FILE_NAME);

    // Create the index file ("index_test_create.0")
    if (AM_CreateIndex((char *)INDEX_BASE_NAME, INDEX_NO, ATTR_TYPE, ATTR_LEN) != AME_OK)
    {
        AM_PrintError("Error creating index");
        exit(1);
    }

    am_fd = PF_OpenFile((char *)INDEX_FILE_NAME);
    if (am_fd < 0)
    {
        PF_PrintError("Error opening index file");
        exit(1);
    }

    sp_fd = PF_OpenFile((char *)DATA_FILE);
    if (sp_fd < 0)
    {
        PF_PrintError("Error opening data file");
        PF_CloseFile(am_fd);
        exit(1);
    }

    printf("Scanning data file and inserting into index...\n");

    pf_err = PF_GetFirstPage(sp_fd, &pageNum, &pageBuf_sp);
    if (pf_err != PFE_OK && pf_err != PFE_EOF) {
        PF_PrintError("Error getting first page of data file");
    }

    while (pf_err == PFE_OK) {
        int slotID = -1;
        char *record;
        int recLen;

        // Iterate through all records on this page
        while (SP_GetNextRecord(pageBuf_sp, &slotID, &record, &recLen) == SPE_OK) {
            key = *(int *)record; // Assuming integer key is at the start
            
            recId = (pageNum << 16) | (slotID & 0xFFFF);
            
            int_recId = RecIdToInt(recId); 

            printf("Inserting key: %d, RECID: %d (Page %d, Slot %d)\n", key, int_recId, pageNum, slotID);
            if (AM_InsertEntry(am_fd, ATTR_TYPE, ATTR_LEN, (char *)&key, int_recId) != AME_OK)
            {
                AM_PrintError("Error inserting entry");
            }
        }
        
        pf_err = PF_UnfixPage(sp_fd, pageNum, FALSE);
        if (pf_err != PFE_OK) {
             PF_PrintError("Error unfixing data page");
             break;
        }

        pf_err = PF_GetNextPage(sp_fd, &pageNum, &pageBuf_sp);
    }
    
    if (pf_err != PFE_EOF) {
         PF_PrintError("Error getting next data page");
    }
    
    printf("...Scan complete.\n\n");

    // Close files
    PF_CloseFile(sp_fd);
    PF_CloseFile(am_fd);

    // return 0;



    // Print the index to verify
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
    printf("Create Index from Existing File : Verification Complete\n");
    return 0;
}