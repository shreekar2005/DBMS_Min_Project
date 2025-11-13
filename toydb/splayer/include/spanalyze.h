/**
 * @file spanalyze.h
 * @brief Defines the helper function for database analysis.
 */

#ifndef SPANALYZE_H
#define SPANALYZE_H

/**
 * @brief Internal helper function to scan and analyze a .tdb file.
 * @param tdb_fd File descriptor for the open .tdb file.
 * @return SPE_OK on success, or a PF error code.
 */
int SPanalyzeDb(int tdb_fd);

#endif // SPANALYZE_H