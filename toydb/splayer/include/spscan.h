/**
 * @file spscan.h
 * @brief Internal helper function for scanning a .tdb file.
 */

#ifndef SPSCAN_H
#define SPSCAN_H

/**
 * @brief Internal logic for scanning an entire .tdb file.
 * @param tdb_fd File descriptor for the open .tdb file.
 * @return The total number of records found, or a PF error code.
 */
int SPscanDb(int tdb_fd);

#endif // SPSCAN_H