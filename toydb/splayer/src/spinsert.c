/**
 * @file spinsert.c
 * @brief Implementation of the record insert helper.
 *
 * Handles finding the last page and inserting a new record.
 */

#include <stdio.h>
#include <string.h>
#include "../../pflayer/include/pf.h"
#include "../include/sp.h"
#include "../include/spinsert.h"

int SPinsertRecordByContent(int tdb_fd, const char *record, int recLen)
{
    int pageNum = -1;
    char *pagePtr = NULL;
    int lastPageNum = -1;
    char *lastPagePtr = NULL;
    int err;
    while ((err = PF_GetNextPage(tdb_fd, &pageNum, &pagePtr)) == PFE_OK) {
        if (lastPagePtr != NULL) {
            PF_UnfixPage(tdb_fd, lastPageNum, FALSE);
        }
        lastPageNum = pageNum;
        lastPagePtr = pagePtr;
    }

    if (err != PFE_EOF) {
        PF_PrintError("SPinsertRecordByContent: Error scanning for last page");
        return err;
    }
    if (lastPagePtr == NULL) {
        err = PF_AllocPage(tdb_fd, &pageNum, &pagePtr);
        if (err != PFE_OK) {
            PF_PrintError("SPinsertRecordByContent: PF_AllocPage (empty file)");
            return err;
        }
        SP_InitPage(pagePtr);
    }
    else {
        pageNum = lastPageNum;
        pagePtr = lastPagePtr;
    }
    err = SP_InsertRecord(pagePtr, record, recLen);

    if (err >= 0) {
        PF_UnfixPage(tdb_fd, pageNum, TRUE);
        return SPE_OK;
    }
    else if (err == SPE_PAGE_FULL) {
        SP_InitPage(pagePtr);
        err = SP_InsertRecord(pagePtr, record, recLen);

        if (err >= 0) {
            PF_UnfixPage(tdb_fd, pageNum, TRUE);
            return SPE_OK;
        }
        else {
            PF_UnfixPage(tdb_fd, pageNum, FALSE);
            return err;
        }
    }
    else {
        PF_UnfixPage(tdb_fd, pageNum, FALSE); 
        return err;
    }
}