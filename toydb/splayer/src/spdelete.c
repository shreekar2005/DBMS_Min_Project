/**
 * @file spdelete.c
 * @brief Implementation of the record delete helper.
 */

#include <stdio.h>
#include "../../pflayer/include/pf.h"
#include "../include/sp.h"
#include "../include/spfind.h"
#include "../include/spdelete.h"

int SPdeleteRecordByContent(int tdb_fd, const char *recordToFind)
{
    int pageNum, slotID;
    int err;
    char* pagePtr=NULL; 

    err = SPfindRecord(tdb_fd, recordToFind, &pageNum, &slotID, &pagePtr); 

    if (err == SPE_RECORD_NOT_FOUND) {
        return SPE_RECORD_NOT_FOUND;
    }
    if (err != SPE_OK) {
        return err; 
    }
    err = SP_DeleteRecord(pagePtr, slotID);
    if (err != SPE_OK) {
        PF_UnfixPage(tdb_fd, pageNum, FALSE);
        return err;
    }
    
    printf("\n*** Record Found at [Page: %d, Slot: %d]. Deleting... ***\n", pageNum, slotID);

    err = PF_UnfixPage(tdb_fd, pageNum, TRUE);
    if (err != PFE_OK) {
        PF_PrintError("SPdeleteRecordByContent: PF_UnfixPage (dirty)");
        return err;
    }

    return SPE_OK;
}