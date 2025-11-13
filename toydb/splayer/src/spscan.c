/**
 * @file spscan.c
 * @brief Implementation of the database scan helper.
 */

#include <stdio.h>
#include "../../pflayer/include/pf.h"
#include "../include/sp.h"
#include "../include/spscan.h"

int SPscanDb(int tdb_fd)
{
    int currentPageNum = -1;
    char *pagePtr;
    int err;
    int recordsFound = 0;

    while ((err = PF_GetNextPage(tdb_fd, &currentPageNum, &pagePtr)) == PFE_OK) {
        int currentSlot = -1;
        char *record;
        int recLen;

        while (SP_GetNextRecord(pagePtr, &currentSlot, &record, &recLen) == SPE_OK) {
            printf("  [Page: %d, Slot: %d]: '%.*s'\n", currentPageNum, currentSlot, recLen, record);
            recordsFound++;
        }

        err = PF_UnfixPage(tdb_fd, currentPageNum, FALSE); // Not dirty
        if (err != PFE_OK) {
            PF_PrintError("SPscanDb: PF_UnfixPage");
            return err;
        }
    }

    if (err != PFE_EOF) {
        PF_PrintError("SPscanDb: Error during scan");
        return err;
    }

    return recordsFound;
}