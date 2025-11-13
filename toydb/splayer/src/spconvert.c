/**
 * @file spconvert.c
 * @brief Implementation of the Txt-to-Tdb conversion helper.
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "../../pflayer/include/pf.h"
#include "../include/sp.h"
#include "../include/spconvert.h"

#define MAX_LINE_LENGTH 1024 // Max characters per line in .txt file

int SPconvertTxtToTdb(const char *inputTxtFile, const char *outputTdbFile)
{
    FILE *txtFile = fopen(inputTxtFile, "r");
    if (txtFile == NULL) {
        perror("SPconvertTxtToTdb: Error opening input text file");
        return SPE_FILE_NOT_FOUND;
    }

    int err = PF_CreateFile(outputTdbFile);
    if (err != PFE_OK) {
        PF_PrintError("SPconvertTxtToTdb: PF_CreateFile");
        fclose(txtFile);
        return err;
    }

    int tdb_fd = PF_OpenFile(outputTdbFile);
    if (tdb_fd < 0) {
        PF_PrintError("SPconvertTxtToTdb: PF_OpenFile");
        fclose(txtFile);
        return tdb_fd;
    }

    char lineBuffer[MAX_LINE_LENGTH];
    char *currentPagePtr = NULL;
    int currentPageNum = -1;
    int recordsInserted = 0;
    int pagesUsed = 0;

    err = PF_AllocPage(tdb_fd, &currentPageNum, &currentPagePtr);
    if (err != PFE_OK) {
        PF_PrintError("SPconvertTxtToTdb: PF_AllocPage (first)");
        PF_CloseFile(tdb_fd);
        fclose(txtFile);
        return err;
    }
    SP_InitPage(currentPagePtr);
    pagesUsed++;

    while (fgets(lineBuffer, sizeof(lineBuffer), txtFile) != NULL) {
        lineBuffer[strcspn(lineBuffer, "\n")] = 0;
        int recLen = strlen(lineBuffer);

        if (recLen == 0) continue;

        while (SP_InsertRecord(currentPagePtr, lineBuffer, recLen) == SPE_PAGE_FULL) {
            err = PF_UnfixPage(tdb_fd, currentPageNum, TRUE); // Mark dirty
            if (err != PFE_OK) {
                PF_PrintError("SPconvertTxtToTdb: PF_UnfixPage");
                PF_CloseFile(tdb_fd);
                fclose(txtFile);
                return err;
            }

            err = PF_AllocPage(tdb_fd, &currentPageNum, &currentPagePtr);
            if (err != PFE_OK) {
                PF_PrintError("SPconvertTxtToTdb: PF_AllocPage (new)");
                PF_CloseFile(tdb_fd);
                fclose(txtFile);
                return err;
            }
            SP_InitPage(currentPagePtr);
            pagesUsed++;
        }
        recordsInserted++;
    }

    if (currentPagePtr != NULL) {
        err = PF_UnfixPage(tdb_fd, currentPageNum, TRUE);
        if (err != PFE_OK) {
            PF_PrintError("SPconvertTxtToTdb: PF_UnfixPage (last)");
            PF_CloseFile(tdb_fd); 
            fclose(txtFile);
            return err;
        }
    }

    err = PF_CloseFile(tdb_fd);
    if (err != PFE_OK) {
        PF_PrintError("SPconvertTxtToTdb: PF_CloseFile");
        fclose(txtFile); // Still close the text file
        return err;
    }

    fclose(txtFile);
    
    printf("--- Conversion Complete ---\n");
    printf("Total Records Inserted: %d\n", recordsInserted);
    printf("Total Pages Used:       %d\n", pagesUsed);
    printf("---------------------------\n");

    return SPE_OK;
}