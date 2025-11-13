/**
 * @file spconvert.h
 * @brief Internal helper function for converting text to .tdb
 */

#ifndef SPCONVERT_H
#define SPCONVERT_H

/**
 * @brief Internal logic for creating a .tdb file from a text file.
 * @param inputTxtFile Path to the source .txt file.
 * @param outputTdbFile Path to the destination .tdb file to create.
 * @return SPE_OK on success, or a PF error code on failure.
 */
int SPconvertTxtToTdb(const char *inputTxtFile, const char *outputTdbFile);

#endif // SPCONVERT_H