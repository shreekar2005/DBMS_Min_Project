/**
 * @file sp.h
 * @brief Defines the layout and functions for a slotted page.
 *
 * This file defines the layout of a slotted page. This layout is used
 * on a raw page buffer (char*) provided by the PF layer.
 */

#ifndef SP_H
#define SP_H

/**
 * @brief Each record is pointed to by a slot.
 */
typedef struct
{
    int offset; /**< Byte offset from the *start* of the page where the record begins */
    int length; /**< Length of the record in bytes */
} Slot;

/**
 * @brief The PageHeader is at the very beginning of the page.
 * The array of slots (Slot[]) will follow *immediately* after it.
 */
typedef struct
{
    int numSlots;     /**< Number of slots currently in use */
    int freeSpacePtr; /**< Offset from the start of the page, pointing to
                           the *beginning* of the free space (which is also
                           the end of the packed record data). */
} PageHeader;

// Error codes for the slotted page layer
#define SPE_OK 0               /**< Success */
#define SPE_PAGE_FULL -21      /**< Page is full */
#define SPE_INVALID_SLOT -22   /**< Invalid slot ID */
#define SPE_RECORD_TOO_BIG -23 /**< Record is too big to fit */

/****************************************************************************
                Slotted Page Level Interface
****************************************************************************/

/**
 * @brief Format a new, blank page as an empty slotted page.
 * @param pagePtr Pointer to the page buffer.
 */
void SP_InitPage(char *pagePtr);

/**
 * @brief Tries to insert a new record onto the page.
 * @param pagePtr Pointer to the page buffer.
 * @param record Pointer to the record data to insert.
 * @param recLen Length of the record.
 * @return The slotID if successful, or an error code otherwise.
 */
int SP_InsertRecord(char *pagePtr, char *record, int recLen);

/**
 * @brief Deletes a record from a given slot.
 * @param pagePtr Pointer to the page buffer.
 * @param slotID The ID of the slot to delete.
 * @return SPE_OK on success, or an error code.
 */
int SP_DeleteRecord(char *pagePtr, int slotID);

/**
 * @brief Gets a *pointer* to the record data in the given slot.
 * @param pagePtr Pointer to the page buffer.
 * @param slotID The ID of the slot to retrieve.
 * @param record Output: Pointer to the record data within the page buffer.
 * @param recLen Output: Length of the record.
 * @return SPE_OK on success, or an error code.
 */
int SP_GetRecord(char *pagePtr, int slotID, char **record, int *recLen);

/**
 * @brief Finds the next valid slotID *after* the one given. Used for scanning.
 * @param pagePtr Pointer to the page buffer.
 * @param slotID Input: The current slot ID. Call with *slotID = -1 to start.
 *               Output: The next valid slot ID.
 * @param record Output: Pointer to the record data.
 * @param recLen Output: Length of the record.
 * @return SPE_OK on success, SPE_INVALID_SLOT if no more records.
 */
int SP_GetNextRecord(char *pagePtr, int *slotID, char **record, int *recLen);

/**
 * @brief Returns the amount of free space left on the page.
 * @param pagePtr Pointer to the page buffer.
 * @return The amount of free space in bytes.
 */
int SP_GetFreeSpace(char *pagePtr);

#endif // SP_H
