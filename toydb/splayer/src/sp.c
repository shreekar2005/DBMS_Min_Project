/**
 * @file sp.c
 * @brief Implementation of slotted page functions.
 */

#include <string.h>
#include <stdio.h>
#include "../../pflayer/include/pf.h" // We need PF_PAGE_SIZE
#include "../include/sp.h" // Our new header

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

// --- Implementation of our functions ---


void SP_InitPage(char *pagePtr)
{
    PageHeader *header = getHeader(pagePtr);
    header->numSlots = 0;
    header->freeSpacePtr = PF_PAGE_SIZE; // Free space starts at the very end
}


int SP_GetFreeSpace(char *pagePtr)
{
    PageHeader *header = getHeader(pagePtr);

    // Start of the free space "middle"
    int freeSpaceStart = sizeof(PageHeader) + (header->numSlots * sizeof(Slot));

    // End of the free space "middle"
    int freeSpaceEnd = header->freeSpacePtr;

    return (freeSpaceEnd - freeSpaceStart);
}


int SP_InsertRecord(char *pagePtr, char *record, int recLen)
{
    PageHeader *header = getHeader(pagePtr);
    Slot *slots = getSlots(pagePtr);

    // 1. Calculate required space
    // We need space for the record data AND a new slot
    int spaceNeeded = recLen + sizeof(Slot);

    if (SP_GetFreeSpace(pagePtr) < spaceNeeded)
    {
        return SPE_PAGE_FULL;
    }

    // 2. Find a slot for this record.
    // For simplicity, we'll just add to the end.
    // A better way would be to find an empty slot (from a deleted record)
    int newSlotID = header->numSlots;

    // 3. Add the record data
    // Calculate new start position for data (at the end of the page)
    int dataOffset = header->freeSpacePtr - recLen;
    memcpy(pagePtr + dataOffset, record, recLen);

    // 4. Update the slot
    slots[newSlotID].offset = dataOffset;
    slots[newSlotID].length = recLen;

    // 5. Update the header
    header->numSlots++;
    header->freeSpacePtr = dataOffset; // Move the free space pointer

    return newSlotID; // Return the new slot number
}


int SP_DeleteRecord(char *pagePtr, int slotID)
{
    PageHeader *header = getHeader(pagePtr);
    Slot *slots = getSlots(pagePtr);

    if (slotID < 0 || slotID >= header->numSlots)
    {
        return SPE_INVALID_SLOT;
    }

    // "Deletion" is simple: just mark the slot as empty.
    // The data is still there, but it's now un-referenced "garbage".
    // This is simple but doesn't reclaim space.
    // Real databases would do "compaction" later.
    slots[slotID].offset = -1; // -1 can mean "empty"
    slots[slotID].length = 0;

    // Note: We don't decrement numSlots because that
    // would mess up the slotIDs for all subsequent records.
    // The slot is just "empty" now.

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
        // This slot was deleted
        return SPE_INVALID_SLOT;
    }

    // Set the output pointers
    *recLen = slot->length;
    *record = pagePtr + slot->offset; // Pointer *into* the page buffer

    return SPE_OK;
}


int SP_GetNextRecord(char *pagePtr, int *slotID, char **record, int *recLen)
{
    PageHeader *header = getHeader(pagePtr);
    Slot *slots = getSlots(pagePtr);

    int currentSlot = *slotID;
    int nextSlot = currentSlot + 1;

    // Loop from the next slot to the end, looking for a *valid* one
    while (nextSlot < header->numSlots)
    {
        if (slots[nextSlot].offset != -1)
        { // -1 means deleted
            // Found a valid record!
            *slotID = nextSlot;
            return SP_GetRecord(pagePtr, nextSlot, record, recLen);
        }
        nextSlot++;
    }

    // No more valid records found
    return SPE_INVALID_SLOT; // Or a different "EOF" code
}
