/**
 * @file sp.c
 * @brief Implementation of slotted page functions.
 *
 * This file contains the low-level page management functions and
 * high-level wrapper functions that call internal helpers.
 */

#include <string.h>
#include <stdio.h>
#include <stdlib.h> 
#include "../../pflayer/include/pf.h" 
#include "../include/sp.h" 

// Include all internal helper headers
#include "../include/spconvert.h"
#include "../include/spscan.h"
#include "../include/spfind.h"
#include "../include/spdelete.h"
#include "../include/spanalyze.h"
#include "../include/spinsert.h"


/* Static function prototypes */
static PageHeader *getHeader(char *pagePtr);
static Slot *getSlots(char *pagePtr);

/**
 * @brief Helper function to get the header from a page.
 * @param pagePtr Pointer to the page buffer.
 * @return Pointer to the PageHeader.
 */
static PageHeader *getHeader(char *pagePtr)
{
    return (PageHeader *)pagePtr;
}

/**
 * @brief Helper function to get the slot array from a page.
 * @param pagePtr Pointer to the page buffer.
 * @return Pointer to the first Slot in the slot array.
 */
static Slot *getSlots(char *pagePtr)
{
    // The slot array starts *right after* the PageHeader
    return (Slot *)(pagePtr + sizeof(PageHeader));
}

/* --- Low-Level Page Functions (Unchanged) --- */

void SP_InitPage(char *pagePtr)
{
    PageHeader *header = getHeader(pagePtr);
    header->numSlots = 0;
    header->freeSpacePtr = PF_PAGE_SIZE; 
}


int SP_GetFreeSpace(char *pagePtr)
{
    PageHeader *header = getHeader(pagePtr);
    int freeSpaceStart = sizeof(PageHeader) + (header->numSlots * sizeof(Slot));
    int freeSpaceEnd = header->freeSpacePtr;
    return (freeSpaceEnd - freeSpaceStart);
}


int SP_InsertRecord(char *pagePtr, const char *record, int recLen)
{
    PageHeader *header = getHeader(pagePtr);
    Slot *slots = getSlots(pagePtr);

    int spaceNeeded = recLen + sizeof(Slot);
    if (SP_GetFreeSpace(pagePtr) < spaceNeeded)
    {
        return SPE_PAGE_FULL;
    }

    int newSlotID = header->numSlots;
    int dataOffset = header->freeSpacePtr - recLen;
    memcpy(pagePtr + dataOffset, record, recLen);

    slots[newSlotID].offset = dataOffset;
    slots[newSlotID].length = recLen;

    header->numSlots++;
    header->freeSpacePtr = dataOffset; 

    return newSlotID; 
}


int SP_DeleteRecord(char *pagePtr, int slotID)
{
    PageHeader *header = getHeader(pagePtr);
    Slot *slots = getSlots(pagePtr);

    if (slotID < 0 || slotID >= header->numSlots)
    {
        return SPE_INVALID_SLOT;
    }

    slots[slotID].offset = -1; 
    slots[slotID].length = 0;
    num_logical_writes++; // added by shreekar :)
    return SPE_OK;
}


int SP_GetRecord(char *pagePtr, int slotID, char **record, int *recLen)
{
    PageHeader *header = getHeader(pagePtr);
    Slot *slots = getSlots(pagePtr);

    if (slotID < 0 || slotID >= header->numSlots)
    {
        return SPE_INVALID_SLOT;
    }

    Slot *slot = &slots[slotID];

    if (slot->offset == -1)
    {
        return SPE_INVALID_SLOT;
    }

    *recLen = slot->length;
    *record = pagePtr + slot->offset; 

    return SPE_OK;
}


int SP_GetNextRecord(char *pagePtr, int *slotID, char **record, int *recLen)
{
    PageHeader *header = getHeader(pagePtr);
    Slot *slots = getSlots(pagePtr);

    int currentSlot = *slotID;
    int nextSlot = currentSlot + 1;

    while (nextSlot < header->numSlots)
    {
        if (slots[nextSlot].offset != -1)
        { 
            *slotID = nextSlot;
            return SP_GetRecord(pagePtr, nextSlot, record, recLen);
        }
        nextSlot++;
    }

    return SPE_INVALID_SLOT; 
}


/* --- High-Level Database Wrapper Functions --- */

int SP_ConvertTxtToTdb(const char *inputTxtFile, const char *outputTdbFile)
{
    // Call the internal helper function
    return SPconvertTxtToTdb(inputTxtFile, outputTdbFile);
}


int SP_ScanDb(int tdb_fd)
{
    // Call the internal helper function
    return SPscanDb(tdb_fd);
}


int SP_FindRecord(int tdb_fd, const char *recordToFind, int *outPageNumPtr, int *outSlotID, char** outPagePtrPtr)
{
    // Call the internal helper function
    int status;
    status = SPfindRecord(tdb_fd, recordToFind, outPageNumPtr, outSlotID, outPagePtrPtr); 
    if(status==SPE_OK) PF_UnfixPage(tdb_fd, *outPageNumPtr, FALSE); 
    return status;
}


int SP_DeleteRecordByContent(int tdb_fd, const char *recordToFind)
{
    // Call the internal helper function
    return SPdeleteRecordByContent(tdb_fd, recordToFind);
}

int SP_InsertRecordByContent(int tdb_fd, const char *record, int recLen)
{
    // Call the internal helper function from spinsert.c
    return SPinsertRecordByContent(tdb_fd, record, recLen);
}

int SP_AnalyzeDb(int tdb_fd)
{
    // Call the internal helper function
    return SPanalyzeDb(tdb_fd);
}