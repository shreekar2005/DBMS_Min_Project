/**
 * @file spdelete.h
 * @brief Internal helper function for deleting a record by content.
 */

#ifndef SPDELETE_H
#define SPDELETE_H

/**
 * @brief Internal logic for finding and deleting a record.
 * @param tdb_fd File descriptor for the open .tdb file.
 * @param recordToFind The byte content of the record to delete.
 * @return SPE_OK if found and deleted, SPE_RECORD_NOT_FOUND if not found, or an error code.
 */
int SPdeleteRecordByContent(int tdb_fd, const char *recordToFind);

#endif // SPDELETE_H