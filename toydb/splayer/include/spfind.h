/**
 * @file spfind.h
 * @brief Internal helper function for finding a record.
 */

#ifndef SPFIND_H
#define SPFIND_H

/**
 * @brief Internal logic for finding the first occurrence of a record.
 * @param tdb_fd File descriptor for the open .tdb file.
 * @param recordToFind The byte content of the record to find.
 * @param outPageNum Output: The page number where the record was found.
 * @param outSlotID Output: The slot ID where the record was found.
 * @param outPagePtrPtr Output: The pointer to PagePtr (pointing to page loaded in buffer)
 * @return SPE_OK if found, SPE_RECORD_NOT_FOUND if not found, or a PF error.
 */
int SPfindRecord(int tdb_fd, const char *recordToFind, int *outPageNum, int *outSlotID, char** outPagePtr);

#endif // SPFIND_H