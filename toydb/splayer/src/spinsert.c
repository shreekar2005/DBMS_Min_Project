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

    // 1. Find the last page in the file
    // We iterate through all pages. The last one we get from
    // PF_GetNextPage will be the last page in the file.
    while ((err = PF_GetNextPage(tdb_fd, &pageNum, &pagePtr)) == PFE_OK) {
        if (lastPagePtr != NULL) {
            // Unfix the *previous* last page, it's not the one we want.
            PF_UnfixPage(tdb_fd, lastPageNum, FALSE);
        }
        lastPageNum = pageNum;
        lastPagePtr = pagePtr;
    }

    if (err != PFE_EOF) {
        // An actual error occurred during page scan
        PF_PrintError("SPinsertRecordByContent: Error scanning for last page");
        return err;
    }

    // --- At this point, two cases are possible ---

    // 2. Case 1: The file was empty (PF_GetNextPage returned EOF immediately)
    if (lastPagePtr == NULL) {
        // Allocate a brand new page
        err = PF_AllocPage(tdb_fd, &pageNum, &pagePtr);
        if (err != PFE_OK) {
            PF_PrintError("SPinsertRecordByContent: PF_AllocPage (empty file)");
            return err;
        }

        // Initialize it as a new slotted page
        SP_InitPage(pagePtr);
    }
    // 3. Case 2: The file was not empty.
    else {
        // lastPageNum and lastPagePtr point to the *pinned* last page
        pageNum = lastPageNum;
        pagePtr = lastPagePtr;
    }

    // 4. Try to insert the record into the chosen page (either new or last)
    err = SP_InsertRecord(pagePtr, record, recLen);

    if (err >= 0) { // <-- FIX: Check for any non-negative slotID
        // Success! Unfix the page, marking it dirty.
        PF_UnfixPage(tdb_fd, pageNum, TRUE);
        return SPE_OK;
    }
    else if (err == SPE_PAGE_FULL) {
// ... existing code ...
        // Initialize and insert (this shouldn't fail unless record is too big)
        SP_InitPage(pagePtr);
        err = SP_InsertRecord(pagePtr, record, recLen);

        if (err >= 0) { // <-- FIX: Also check here
            // Success on the new page. Unfix dirty.
            PF_UnfixPage(tdb_fd, pageNum, TRUE);
            return SPE_OK;
        }
        else {
            // Failed (e.g., SPE_RECORD_TOO_BIG)
            PF_UnfixPage(tdb_fd, pageNum, FALSE); // Unfix clean
            return err;
        }
    }
    else {
        // Another error (e.g., SPE_RECORD_TOO_BIG on the first try)
        PF_UnfixPage(tdb_fd, pageNum, FALSE); // Unfix clean
        return err;
    }
}