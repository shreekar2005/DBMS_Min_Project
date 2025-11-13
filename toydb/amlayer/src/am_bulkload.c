/**
 * @file am_bulkload.c
 * @brief Stub implementation for B+ Tree bulk-loading.
 */

#include <stdio.h>
#include "pf.h" // For PF_GetFirstPage, etc.
#include "sp.h" // For SP_GetNextRecord
#include "am.h" // For AME_OK, am.h includes testam.h for RecIdType

/**
 * @brief Efficiently builds an index from a pre-sorted data file.
 *
 * NOTE: This is a STUBBED implementation. A real implementation would
 * perform a bottom-up build of the B+ Tree.
 *
 * This stub simply scans the file to prove the linkage works, using the
 * low-level PF and SP functions.
 */
int AM_BulkLoad(int amFileDesc, int spFileDesc,
                char attrType, int attrLength)
{
    int pf_err;
    int pageNum = -1;
    char *pageBuf;
    
    printf("******************************************************************\n");
    printf("* AM_BulkLoad: Called successfully.\n");
    printf("* NOTE: This is a STUBBED function.\n");
    printf("******************************************************************\n");

    // --- Scan the splayer file using PF and SP functions ---

    // 1. Get the first page of the splayer file
    pf_err = PF_GetFirstPage(spFileDesc, &pageNum, &pageBuf);
    if (pf_err != PFE_OK && pf_err != PFE_EOF) {
        PF_PrintError("AM_BulkLoad: Error getting first page");
        return AME_PF;
    }

    // 2. Iterate through all pages in the splayer file
    while (pf_err == PFE_OK) {
        int slotID = -1;
        char *record;
        int recLen;

        // 3. Iterate through all records on this page
        while (SP_GetNextRecord(pageBuf, &slotID, &record, &recLen) == SPE_OK) {
            // A real implementation would build the index here
            
            // char* keyPtr = record;
            // RecIdType recId;
            // recId.pagenum = pageNum;
            // recId.slotnum = slotID;
            // int int_recId = RecIdToInt(recId);
            
            // Add_To_Leaf_Page_Buffer(keyPtr, int_recId);
        }
        
        // 4. Unfix the current page
        pf_err = PF_UnfixPage(spFileDesc, pageNum, FALSE);
        if (pf_err != PFE_OK) {
             PF_PrintError("AM_BulkLoad: Error unfixing page");
             return AME_PF;
        }

        // 5. Get the next page
        pf_err = PF_GetNextPage(spFileDesc, &pageNum, &pageBuf);
    }
    
    if (pf_err != PFE_EOF) {
        PF_PrintError("AM_BulkLoad: Error getting next page");
        return AME_PF;
    }

    return AME_OK;
}