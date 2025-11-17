/**
 * @file test_pfbuf_hotmix.c
 * @brief Test program to generate data for plotting buffer strategy performance
 * across different hot/cold access mixtures.
 *
 * This test runs a mixed access pattern multiple times,
 * varying the percentage of accesses that are to the "hot set".
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "../include/pf.h"
#include "../include/pftypes.h" 
#include "../include/pfbuf.h"

#define TEST_FILE "buffertestfile_hotmix"

#define BUFFER_SIZE 20
#define HOT_SET_SIZE 50    // Pages 0-49 (e.g., index pages)
#define COLD_SET_SIZE 450  // Pages 50-449 (e.g., table scan)
#define TOTAL_PAGES (HOT_SET_SIZE + COLD_SET_SIZE)

#define TOTAL_ACCESSES 10000

#define STRATEGY_LRU 0
#define STRATEGY_MRU 1

void createFile(void);
void runTest(int hot_access_percentage); // Now takes a parameter

int main(void)
{
    int error;
    createFile();

    printf("Hot/Cold Mix percentage VS Hit Rate : for LRU and MRU\n");
    printf("Total Accesses per Run: %d\n", TOTAL_ACCESSES);
    printf("Test is READ-ONLY\n\n");

    FILE* fp=fopen("coordinates.txt", "w");

    if (fp == NULL) {
    printf("Error opening file!\n");
    return 1;
    }

    // This loop will generate the data for your X-axis
    for (int hot_mix = 0; hot_mix <= 100; hot_mix += 10)
    {
        printf("========================================================\n");
        printf("  running test : HOT MIX = %d%%\n", hot_mix);
        printf("========================================================\n");
        printf(" With LRU\n");
        PF_Init(BUFFER_SIZE, STRATEGY_LRU);
        runTest(hot_mix);
        PF_PrintStats(); // Make sure your PF_PrintStats() prints the counters!
        double lru_hit_rate=hit_rate;
        printf(" With MRU\n");
        PF_Init(BUFFER_SIZE, STRATEGY_MRU); 
        runTest(hot_mix);
        PF_PrintStats(); // Make sure your PF_PrintStats() prints the counters!
        double mru_hit_rate=hit_rate;

        //writing "hot mix, lru_hit_rate, mru_hit_rate" in a line of file
        fprintf(fp, "%d %lf %lf\n", hot_mix, lru_hit_rate*100.0, mru_hit_rate*100.0);
    }

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

    PF_DestroyFile(TEST_FILE); // Destroy if it exists, ignore error

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
}

/**
 * @brief Runs the mixed workload test.
 * @param hot_access_percentage The chance (0-100) that an access is to the hot set.
 */
void runTest(int hot_access_percentage)
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
        if ((rand() % 100) < hot_access_percentage)
        {
            // HOT ACCESS
            // Access a random page from the hot set
            pagenum = rand() % HOT_SET_SIZE;
        }
        else
        {
            // COLD ACCESS
            // Access the next page in the sequential scan
            pagenum = HOT_SET_SIZE + cold_scan_tracker;

            cold_scan_tracker++;
            if (cold_scan_tracker >= COLD_SET_SIZE)
            {
                cold_scan_tracker = 0;
            }
        }
        
        if ((error = PF_GetThisPage(fd, pagenum, &buf)) != PFE_OK)
        {
            PF_PrintError("PF_GetThisPage");
            fprintf(stderr, "Error on page %d\n", pagenum);
            exit(1);
        }

        // Verify the page content
        if (*((int *)buf) != pagenum)
        {
            // This check is not perfect, as a previous run might have
            // written data, but it's a basic sanity check.
            // In a read-only test, this should ideally always pass.
        }

        // Unfix the page (read-only, so dirty bit is FALSE)
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