/**
 * @file sp.h
 * @brief Defines the layout and functions for a slotted page.
 *
 * This file defines the layout of a slotted page. This layout is used
 * on a raw page buffer (char*) provided by the PF layer.
 */

#ifndef SP_H
#define SP_H

#include "../../pflayer/include/pf.h" // For PF_PAGE_SIZE

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
#define SPE_RECORD_NOT_FOUND -24 /**< Record not found during a search */
#define SPE_FILE_NOT_FOUND -25 /**< Source text file not found */

/* --- Low-Level Page Functions --- */

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
 * Output: The next valid slot ID.
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


/* --- High-Level Database Functions --- */

/**
 * @brief Creates a .tdb file from a text file.
 * @param inputTxtFile Path to the source .txt file.
 * @param outputTdbFile Path to the destination .tdb file to create.
 * @return SPE_OK on success, or a PF error code on failure.
 */
int SP_ConvertTxtToTdb(const char *inputTxtFile, const char *outputTdbFile);

/**
 * @brief Scans an entire .tdb file and prints all records.
 * @param tdb_fd File descriptor for the open .tdb file.
 * @return The total number of records found, or a PF error code.
 */
int SP_ScanDb(int tdb_fd);

/**
 * @brief Finds the first occurrence of a record by its content.
 * @param tdb_fd File descriptor for the open .tdb file.
 * @param recordToFind The byte content of the record to find.
 * @param outPageNum Output: The page number where the record was found.
 * @param outSlotID Output: The slot ID where the record was found.
 * @return SPE_OK if found, SPE_RECORD_NOT_FOUND if not found, or a PF error.
 */
int SP_FindRecord(int tdb_fd, const char *recordToFind, int *outPageNum, int *outSlotID);

/**
 * @brief Finds and deletes the first occurrence of a record by its content.
 * @param tdb_fd File descriptor for the open .tdb file.
 * @param recordToFind The byte content of the record to delete.
 * @return SPE_OK if found and deleted, SPE_RECORD_NOT_FOUND if not found, or an error code.
 */
int SP_DeleteRecordByContent(int tdb_fd, const char *recordToFind);

/**
 * @brief Analyzes space utilization of a .tdb file and prints a report.
 * @param tdb_fd File descriptor for the open .tdb file.
 * @return SPE_OK on success, or a PF error code.
 */
int SP_AnalyzeDb(int tdb_fd);

#endif // SP_H