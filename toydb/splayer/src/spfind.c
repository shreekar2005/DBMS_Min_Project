/**
 * @file spfind.c
 * @brief Implementation of the record find helper.
 */

#include <stdio.h>
#include <string.h>
#include "../../pflayer/include/pf.h"
#include "../include/sp.h"
#include "../include/spfind.h"

int SPfindRecord(int tdb_fd, const char *recordToFind, int *outPageNum, int *outSlotID)
{
    int lenToFind = strlen(recordToFind);
    int currentPageNum = -1;
    char *pagePtr;
    int err;

    while ((err = PF_GetNextPage(tdb_fd, &currentPageNum, &pagePtr)) == PFE_OK) {
        int currentSlot = -1;
        char *record;
        int recLen;

        while (SP_GetNextRecord(pagePtr, &currentSlot, &record, &recLen) == SPE_OK) {
            if (recLen == lenToFind && strncmp(record, recordToFind, recLen) == 0) {
                *outPageNum = currentPageNum;
                *outSlotID = currentSlot;
                
                PF_UnfixPage(tdb_fd, currentPageNum, FALSE);
                return SPE_OK;
            }
        }
        
        PF_UnfixPage(tdb_fd, currentPageNum, FALSE);
    }

    if (err == PFE_EOF) {
        return SPE_RECORD_NOT_FOUND;
    }

    PF_PrintError("SPfindRecord: Error during scan");
    return err;
}