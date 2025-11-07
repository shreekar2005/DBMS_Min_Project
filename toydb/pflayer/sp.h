// sp.h
#ifndef SP_H
#define SP_H

// This file defines the layout of a slotted page.
// We will use this layout on a raw page buffer (char*)
// given to us by the PF layer.

// Each record is pointed to by a slot.
typedef struct {
    int offset;  // Byte offset from the *start* of the page where the record begins
    int length;  // Length of the record in bytes
} Slot;

// The PageHeader is at the very beginning of the page.
// The array of slots (Slot[]) will follow *immediately* after it.
typedef struct {
    int numSlots;     // Number of slots currently in use
    int freeSpacePtr; // Offset from the start of the page, pointing to 
                      // the *beginning* of the free space (which is also
                      // the end of the packed record data).
} PageHeader;

// Error codes for the slotted page layer
#define SPE_OK          0
#define SPE_PAGE_FULL   -21 // Example error code
#define SPE_INVALID_SLOT -22
#define SPE_RECORD_TOO_BIG -23

// --- Function Prototypes ---
// (We will implement these in sp.c)

// Format a new, blank page as an empty slotted page
void SP_InitPage(char *pagePtr);

// Tries to insert a new record onto the page.
// Returns the slotID if successful, or an error code otherwise.
int SP_InsertRecord(char *pagePtr, char *record, int recLen);

// Deletes a record from a given slot.
// (We will just mark the slot as empty)
int SP_DeleteRecord(char *pagePtr, int slotID);

// Gets a *pointer* to the record data in the given slot.
int SP_GetRecord(char *pagePtr, int slotID, char **record, int *recLen);

// Finds the next valid slotID *after* the one given.
// Used for scanning. Call with *slotID = -1 to start.
int SP_GetNextRecord(char *pagePtr, int *slotID, char **record, int *recLen);

// Returns the amount of free space left on the page
int SP_GetFreeSpace(char *pagePtr);


#endif // SP_H