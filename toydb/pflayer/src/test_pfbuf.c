/**
 * @file buffertest.c
 * @brief Test program to show differences in buffer strategies (LRU vs. MRU).
 *
 * This test creates a file with two parts:
 * 1. A small "hot set" of pages (like index pages) that are accessed randomly.
 * 2. A large "cold scan" of pages (like table data) that are accessed sequentially.
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "../include/pf.h"
#include "../include/pftypes.h" 
#include "../include/pfbuf.h"

#define TEST_FILE "buffertestfile"

#define BUFFER_SIZE 20
#define HOT_SET_SIZE 5    // Pages 0-4 (e.g., index pages)
#define COLD_SCAN_SIZE 50 // Pages 5-54 (e.g., table scan)
#define TOTAL_PAGES (HOT_SET_SIZE + COLD_SCAN_SIZE)

#define TOTAL_ACCESSES 10000
#define HOT_ACCESS_PERCENTAGE 80 // 80% of accesses are to the hot set

#define STRATEGY_LRU 0
#define STRATEGY_MRU 1

void createFile(void);
void runTest(void);

int main(void)
{
    int error;
    createFile();

    printf("RUNNING TEST WITH LRU\n");
    PF_Init(BUFFER_SIZE, STRATEGY_LRU);
    runTest();
    PF_PrintStats(); // Print stats for LRU

    printf("RUNNING TEST WITH MRU\n");
    PF_Init(BUFFER_SIZE, STRATEGY_MRU); // Re-init with new strategy
    runTest();
    PF_PrintStats(); // Print stats for MRU

    if ((error = PF_DestroyFile(TEST_FILE)) != PFE_OK)
    {
        PF_PrintError("PF_DestroyFile");
        exit(1);
    }

    return 0;
}

/**
 * @brief Fills a new file with dummy data.
 */
void createFile(void)
{
    int fd, error;
    char *buf;
    int pagenum;

    printf("--- Creating Test File ---\n");
    if ((error = PF_CreateFile(TEST_FILE)) != PFE_OK)
    {
        PF_PrintError("PF_CreateFile");
        exit(1);
    }

    if ((fd = PF_OpenFile(TEST_FILE)) < 0)
    {
        PF_PrintError("PF_OpenFile");
        exit(1);
    }

    for (int i = 0; i < TOTAL_PAGES; i++)
    {
        if ((error = PF_AllocPage(fd, &pagenum, &buf)) != PFE_OK)
        {
            PF_PrintError("PF_AllocPage");
            exit(1);
        }
        // Write the page number into the page
        *((int *)buf) = pagenum;

        if ((error = PF_UnfixPage(fd, pagenum, TRUE)) != PFE_OK)
        {
            PF_PrintError("PF_UnfixPage");
            exit(1);
        }
    }

    if ((error = PF_CloseFile(fd)) != PFE_OK)
    {
        PF_PrintError("PF_CloseFile");
        exit(1);
    }
    printf("Created file '%s' with %d pages.\n\n", TEST_FILE, TOTAL_PAGES);
}

/**
 * @brief Runs the mixed workload test.
 */
void runTest(void)
{
    int fd, error;
    char *buf;
    int pagenum;
    int cold_scan_tracker = 0; // Tracks our position in the sequential scan

    if ((fd = PF_OpenFile(TEST_FILE)) < 0)
    {
        PF_PrintError("PF_OpenFile");
        exit(1);
    }

    // Seed the random number generator
    srand(1); // Use a fixed seed for reproducible results

    for (int i = 0; i < TOTAL_ACCESSES; i++)
    {
        if ((rand() % 100) < HOT_ACCESS_PERCENTAGE)
        {
            // --- 80% HOT ACCESS ---
            // Access a random page from the hot set
            pagenum = rand() % HOT_SET_SIZE;
        }
        else
        {
            // --- 20% COLD ACCESS ---
            // Access the next page in the sequential scan
            pagenum = HOT_SET_SIZE + cold_scan_tracker;

            cold_scan_tracker++;
            if (cold_scan_tracker >= COLD_SCAN_SIZE)
            {
                cold_scan_tracker = 0; // Wrap around
            }
        }

        // Now, get the page (this is the Logical Read)
        if ((error = PF_GetThisPage(fd, pagenum, &buf)) != PFE_OK)
        {
            PF_PrintError("PF_GetThisPage");
            fprintf(stderr, "Error on page %d\n", pagenum);
            exit(1);
        }

        // Verify the page content (optional)
        if (*((int *)buf) != pagenum)
        {
            printf("Data corruption on page %d!\n", pagenum);
        }

        // Unfix the page (not dirty)
        if ((error = PF_UnfixPage(fd, pagenum, FALSE)) != PFE_OK)
        {
            PF_PrintError("PF_UnfixPage");
            exit(1);
        }
    }

    if ((error = PF_CloseFile(fd)) != PFE_OK)
    {
        PF_PrintError("PF_CloseFile");
        exit(1);
    }
}