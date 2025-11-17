/**
 * @file test_ambulkload.c
 * @brief OBJECTIVE 3: TASK 3 : Build index using efficient bulk-loading.
 */

#include <stdio.h>
#include <stdlib.h>
#include <time.h> // Added for timing
#include "pf.h"
#include "sp.h"
#include "am.h"
#define DATA_FILE "testdata_sorted.tdb"
#define INDEX_BASE_NAME "index_test_bulk"
#define INDEX_FILE_NAME "index_test_bulk.0"
#define INDEX_NO 0
#define ATTR_TYPE 'i'
#define ATTR_LEN sizeof(int)

int main()
{
    int sp_fd, am_fd;
    int rootPageNum;
    char *pageBuf;
    
    // Timing variables
    clock_t start_t, end_t;
    double total_t;
    char verify_choice;

    PF_Init(20,0); // 20 buffers, LRU strategy

    printf("TASK : Bulk-Load Index from Sorted File\n");
    printf("Using Sorted Data File: %s\n", DATA_FILE);
    printf("Creating Index File: %s\n\n", INDEX_FILE_NAME);

    // Create the index file
    if (AM_CreateIndex((char *)INDEX_BASE_NAME, INDEX_NO, ATTR_TYPE, ATTR_LEN) != AME_OK)
    {
        AM_PrintError("Error creating index");
        exit(1);
    }

    // Open the index file
    am_fd = PF_OpenFile((char *)INDEX_FILE_NAME);
    if (am_fd < 0)
    {
        PF_PrintError("Error opening index file");
        exit(1);
    }

    // Open the existing sorted data file
    sp_fd = PF_OpenFile((char *)DATA_FILE);
    if (sp_fd < 0)
    {
        PF_PrintError("Error opening data file");
        PF_CloseFile(am_fd);
        exit(1);
    }

    // Call the bulk-load function with timing
    printf("Calling AM_BulkLoad...\n");
    
    start_t = clock(); // Start Timer
    if (AM_BulkLoad(am_fd, sp_fd, ATTR_TYPE, ATTR_LEN) != AME_OK)
    {
        AM_PrintError("Error during bulk-load");
        PF_CloseFile(sp_fd);
        PF_CloseFile(am_fd);
        exit(1);
    }
    end_t = clock(); // End Timer
    
    printf("...Bulk-load function returned.\n");

    // Calculate and print time
    total_t = (double)(end_t - start_t) / CLOCKS_PER_SEC;
    printf("Total time to bulk-load entries: %f seconds\n\n", total_t);

    // Close files
    PF_CloseFile(sp_fd);
    PF_CloseFile(am_fd);

    // User Prompt for Verification
    printf("Print indices to verify insertions? (y/n): ");
    scanf(" %c", &verify_choice);

    if (verify_choice == 'y' || verify_choice == 'Y')
    {
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
        printf("Bulk-Load Index from Sorted File : Verification Complete\n");
    }
    
    return 0;
}