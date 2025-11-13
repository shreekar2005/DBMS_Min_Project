/**
 * @file spinsert.h
 * @brief Defines the internal helper function for inserting a record.
 *
 * This function handles the logic of finding the last page,
 * trying to insert, and allocating a new page if necessary.
 */

#ifndef SPINSERT_H
#define SPINSERT_H

#include "sp.h"

/**
 * @brief Internal helper to insert a new record into the .tdb file.
 *
 * Tries to insert into the last page. If that page is full, it allocates
 * a new page, initializes it, and inserts the record there.
 *
 * @param tdb_fd File descriptor for the open .tdb file.
 * @param record The byte content of the record to insert.
 * @param recLen The length of the record.
 * @return SPE_OK on success, SPE_RECORD_TOO_BIG, or a PF error code.
 */
int SPinsertRecordByContent(int tdb_fd, const char *record, int recLen);

#endif // SPINSERT_H