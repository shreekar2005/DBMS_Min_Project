/**
 * @file spdelete.c
 * @brief Implementation of the record delete helper.
 */

#include <stdio.h>
#include "../../pflayer/include/pf.h"
#include "../include/sp.h"
#include "../include/spfind.h" // Depends on the find helper
#include "../include/spdelete.h"

int SPdeleteRecordByContent(int tdb_fd, const char *recordToFind)
{
    int pageNum, slotID;
    int err;

    // 1. Find the record using the helper
    err = SPfindRecord(tdb_fd, recordToFind, &pageNum, &slotID);

    if (err == SPE_RECORD_NOT_FOUND) {
        return SPE_RECORD_NOT_FOUND;
    }
    if (err != SPE_OK) {
        return err; 
    }

    // 2. Get that specific page
    char *pagePtr;
    err = PF_GetThisPage(tdb_fd, pageNum, &pagePtr);
    if (err != PFE_OK) {
        PF_PrintError("SPdeleteRecordByContent: PF_GetThisPage");
        return err;
    }

    // 3. Delete the record from the page
    err = SP_DeleteRecord(pagePtr, slotID);
    if (err != SPE_OK) {
        PF_UnfixPage(tdb_fd, pageNum, FALSE); // Unfix clean
        return err;
    }
    
    printf("\n*** Record Found at [Page: %d, Slot: %d]. Deleting... ***\n", pageNum, slotID);

    // 4. Unfix the page, MARKING IT DIRTY
    err = PF_UnfixPage(tdb_fd, pageNum, TRUE);
    if (err != PFE_OK) {
        PF_PrintError("SPdeleteRecordByContent: PF_UnfixPage (dirty)");
        return err;
    }

    return SPE_OK;
}