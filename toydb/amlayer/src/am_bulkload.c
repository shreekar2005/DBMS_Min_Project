/**
 * @file am_bulkload.c
 * @brief Full implementation for B+ Tree bulk-loading.
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "pf.h"
#include "sp.h"
#include "am.h"

/* Return codes for helper functions */
#define PAGE_FULL 1
#define PAGE_OK 0

/**
 * @brief Holds a (key, pageNum) pair to be promoted to a parent node.
 */
typedef struct ParentEntry
{
    char key[AM_MAXATTRLENGTH];
    int pageNum;
} ParentEntry;

static void InitLeafHeader(char *pageBuf, short attrLength, short maxKeys);
static int PackLeafPage(char *pageBuf, AM_LEAFHEADER *header, char *key, int recId, short attrLength);
static void InitInternalHeader(char *pageBuf, short attrLength, short maxKeys);
static int PackInternalPage(char *pageBuf, AM_INTHEADER *header, char *key, int pageNum, short attrLength);
static int BuildInternalLevel(int amFileDesc, ParentEntry *levelEntries, int numEntries, short attrLength, short maxKeys, int leftmostChildPage, int *newRootPageNum);


int AM_BulkLoad(int amFileDesc, int spFileDesc,
                char attrType, int attrLength)
{
    int pf_err;
    int sp_pageNum = -1; // Page number in splayer file
    char *sp_pageBuf;    // Buffer for splayer page
    
    int am_pageNum = -1; // Page number in amlayer file
    char *am_pageBuf;    // Buffer for amlayer page
    AM_LEAFHEADER leafHeader;
    
    int prevLeafPageNum = AM_NULL_PAGE;
    short maxKeys; // Max keys for internal nodes, read from root
    
    ParentEntry *parentEntries = NULL;
    int maxParentEntries = 1024; // Initial size
    int numParentEntries = 0;
    
    printf("AM_BulkLoad: Starting efficient bottom-up B+ Tree construction.\n");

    parentEntries = (ParentEntry *)malloc(sizeof(ParentEntry) * maxParentEntries);
    if (parentEntries == NULL) return AME_NOMEM;

    if (PF_GetThisPage(amFileDesc, 0, &am_pageBuf) != PFE_OK) {
        PF_PrintError("AM_BulkLoad: Could not get page 0");
        free(parentEntries);
        return AME_PF;
    }
    AM_bcopy(am_pageBuf, (char *)&leafHeader, AM_sl);
    maxKeys = leafHeader.maxKeys;
    
    if (PF_UnfixPage(amFileDesc, 0, FALSE) != PFE_OK) {
        PF_PrintError("AM_BulkLoad: Could not unfix page 0");
        free(parentEntries);
        return AME_PF;
    }

    if (PF_AllocPage(amFileDesc, &am_pageNum, &am_pageBuf) != PFE_OK) {
        PF_PrintError("AM_BulkLoad: Error allocating first leaf page");
        free(parentEntries);
        return AME_PF;
    }
    
    AM_LeftPageNum = am_pageNum;
    InitLeafHeader(am_pageBuf, (short)attrLength, maxKeys);
    AM_bcopy(am_pageBuf, (char *)&leafHeader, AM_sl);


    // Scan splayer file and build all leaf pages

    pf_err = PF_GetFirstPage(spFileDesc, &sp_pageNum, &sp_pageBuf);
    if (pf_err != PFE_OK && pf_err != PFE_EOF) {
        PF_PrintError("AM_BulkLoad: Error getting first data page");
        free(parentEntries);
        return AME_PF;
    }
    while (pf_err == PFE_OK) {
        int slotID = -1;
        char *record;
        int recLen;
        while (SP_GetNextRecord(sp_pageBuf, &slotID, &record, &recLen) == SPE_OK) {
            char *key = record;
            int recId = (sp_pageNum << 16) | (slotID & 0xFFFF);
            if (PackLeafPage(am_pageBuf, &leafHeader, key, recId, (short)attrLength) == PAGE_FULL)
            {
                leafHeader.nextLeafPage = AM_NULL_PAGE;
                AM_bcopy((char *)&leafHeader, am_pageBuf, AM_sl);
                if (PF_UnfixPage(amFileDesc, am_pageNum, TRUE) != PFE_OK) {
                     PF_PrintError("AM_BulkLoad: Error unfixing full leaf");
                     free(parentEntries);
                     return AME_PF;
                }
                
                prevLeafPageNum = am_pageNum;
                if (PF_AllocPage(amFileDesc, &am_pageNum, &am_pageBuf) != PFE_OK) {
                    PF_PrintError("AM_BulkLoad: Error allocating new leaf");
                    free(parentEntries);
                    return AME_PF;
                }
                InitLeafHeader(am_pageBuf, (short)attrLength, maxKeys);
                AM_bcopy(am_pageBuf, (char *)&leafHeader, AM_sl);
                char *prevPageBuf;
                AM_LEAFHEADER prevHeader;
                if (PF_GetThisPage(amFileDesc, prevLeafPageNum, &prevPageBuf) != PFE_OK) {
                    PF_PrintError("AM_BulkLoad: Error getting prev leaf to link");
                    free(parentEntries);
                    return AME_PF;
                }
                AM_bcopy(prevPageBuf, (char *)&prevHeader, AM_sl);
                prevHeader.nextLeafPage = am_pageNum;
                AM_bcopy((char *)&prevHeader, prevPageBuf, AM_sl);
                if (PF_UnfixPage(amFileDesc, prevLeafPageNum, TRUE) != PFE_OK) {
                    PF_PrintError("AM_BulkLoad: Error unfixing prev leaf after link");
                    free(parentEntries);
                    return AME_PF;
                }
                if (numParentEntries >= maxParentEntries) {
                    maxParentEntries *= 2;
                    parentEntries = (ParentEntry *)realloc(parentEntries, sizeof(ParentEntry) * maxParentEntries);
                    if (parentEntries == NULL) return AME_NOMEM;
                }
                AM_bcopy(key, parentEntries[numParentEntries].key, attrLength);
                parentEntries[numParentEntries].pageNum = am_pageNum;
                numParentEntries++;
                PackLeafPage(am_pageBuf, &leafHeader, key, recId, (short)attrLength);
            }
        }
        pf_err = PF_UnfixPage(spFileDesc, sp_pageNum, FALSE);
        if (pf_err != PFE_OK) {
             PF_PrintError("AM_BulkLoad: Error unfixing data page");
             free(parentEntries);
             return AME_PF;
        }
        pf_err = PF_GetNextPage(spFileDesc, &sp_pageNum, &sp_pageBuf);
    }
    
    if (pf_err != PFE_EOF) {
        PF_PrintError("AM_BulkLoad: Error getting next data page");
        free(parentEntries);
        return AME_PF;
    }
    leafHeader.nextLeafPage = AM_NULL_PAGE;
    AM_bcopy((char *)&leafHeader, am_pageBuf, AM_sl);
    if (PF_UnfixPage(amFileDesc, am_pageNum, TRUE) != PFE_OK) {
        PF_PrintError("AM_BulkLoad: Error unfixing final leaf");
        free(parentEntries);
        return AME_PF;
    }

    printf("... Leaf level (Level 0) complete. %d parent entries created.\n", numParentEntries);

    // Build Internal Levels
    
    int newRootPageNum = -1;
    if (numParentEntries == 0) {
        char *leafBuf;
        char *page0Buf;
        if (PF_GetThisPage(amFileDesc, AM_LeftPageNum, &leafBuf) != PFE_OK) { 
            PF_PrintError("AM_BulkLoad: Error getting single leaf");
            free(parentEntries); 
            return AME_PF; 
        }
        if (PF_GetThisPage(amFileDesc, 0, &page0Buf) != PFE_OK) { 
            PF_PrintError("AM_BulkLoad: Error getting page 0");
            free(parentEntries); 
            return AME_PF; 
        }
        AM_bcopy(leafBuf, page0Buf, PF_PAGE_SIZE);

        if (PF_UnfixPage(amFileDesc, 0, TRUE) != PFE_OK) { free(parentEntries); return AME_PF; }
        if (PF_UnfixPage(amFileDesc, AM_LeftPageNum, FALSE) != PFE_OK) { free(parentEntries); return AME_PF; }
        
        if (PF_DisposePage(amFileDesc, AM_LeftPageNum) != PFE_OK) { free(parentEntries); return AME_PF; }

        AM_RootPageNum = 0; // The root IS the leaf, now at page 0
        AM_LeftPageNum = 0; // The leftmost leaf is now page 0
        printf("... Tree consists of a single leaf node. Moved to page 0.\n");
    } else {
        // Build internal levels, passing AM_LeftPageNum as the first child pointer
        int err = BuildInternalLevel(amFileDesc, parentEntries, numParentEntries, 
                                     (short)attrLength, maxKeys, AM_LeftPageNum, &newRootPageNum);
        if (err != AME_OK) {
            free(parentEntries);
            return err;
        }
        
        if (newRootPageNum != 0) {
             printf("AM_BulkLoad: FATAL: BuildInternalLevel did not return page 0 as root.\n");
             free(parentEntries);
             return AME_INTERROR;
        }
        
        AM_RootPageNum = 0;
        printf("... Internal levels built. New root is at page 0.\n");
    }

    free(parentEntries);
    printf("AM_BulkLoad: B+ Tree construction complete.\n");
    return AME_OK;
}


/**
 * @brief Initializes a new leaf page header.
 */
static void InitLeafHeader(char *pageBuf, short attrLength, short maxKeys)
{
    AM_LEAFHEADER header;
    memset(&header, 0, sizeof(AM_LEAFHEADER));
    header.pageType = 'l';
    header.nextLeafPage = AM_NULL_PAGE;
    header.recIdPtr = PF_PAGE_SIZE;
    header.keyPtr = AM_sl;
    header.freeListPtr = AM_NULL;
    header.numinfreeList = 0;
    header.attrLength = attrLength;
    header.numKeys = 0;
    header.maxKeys = maxKeys; // This is max *internal* keys, but we store it here
    AM_bcopy((char *)&header, pageBuf, AM_sl);
}

/**
 * @brief Manually packs a (key, recId) pair into a leaf page.
 */
static int PackLeafPage(char *pageBuf, AM_LEAFHEADER *header, char *key, int recId, short attrLength)
{
    short recSize = attrLength + AM_ss;
    short needed = recSize + AM_si + AM_ss; // key, list_head, recId_data, recId_next
    
    if (header->recIdPtr - header->keyPtr < needed) {
        return PAGE_FULL;
    }

    short key_offset = header->keyPtr;
    short recid_data_offset = header->recIdPtr - (AM_si + AM_ss);
    
    AM_bcopy(key, pageBuf + key_offset, attrLength);
    AM_bcopy((char *)&recid_data_offset, pageBuf + key_offset + attrLength, AM_ss);
    AM_bcopy((char *)&recId, pageBuf + recid_data_offset, AM_si);
    
    short null_ptr = AM_NULL;
    AM_bcopy((char *)&null_ptr, pageBuf + recid_data_offset + AM_si, AM_ss);
    
    header->keyPtr += recSize;
    header->recIdPtr = recid_data_offset;
    header->numKeys++;
    
    return PAGE_OK;
}

/**
 * @brief Initializes a new internal page header.
 */
static void InitInternalHeader(char *pageBuf, short attrLength, short maxKeys)
{
    AM_INTHEADER header;
    memset(&header, 0, sizeof(AM_INTHEADER));
    header.pageType = 'i';
    header.numKeys = 0;
    header.maxKeys = maxKeys;
    header.attrLength = attrLength;
    AM_bcopy((char *)&header, pageBuf, AM_sint);
}

/**
 * @brief Manually packs a (key, pageNum) pair into an internal page.
 */
static int PackInternalPage(char *pageBuf, AM_INTHEADER *header, char *key, int pageNum, short attrLength)
{
    if (header->numKeys >= header->maxKeys) {
        return PAGE_FULL;
    }
    
    short recSize = attrLength + AM_si;
    short offset = AM_sint + AM_si + (header->numKeys * recSize);
    
    AM_bcopy(key, pageBuf + offset, attrLength);
    AM_bcopy((char *)&pageNum, pageBuf + offset + attrLength, AM_si);
    
    header->numKeys++;
    return PAGE_OK;
}

/**
 * @brief Recursively builds the internal levels of the B+ Tree.
 * @param leftmostChildPage The page number of the *first* child for this level.
 */
static int BuildInternalLevel(int amFileDesc, ParentEntry *levelEntries, int numEntries,
                            short attrLength, short maxKeys, int leftmostChildPage, int *rootPageNum)
{
    ParentEntry *parentEntries = NULL;
    int maxParentEntries = numEntries;
    int numParentEntries = 0;
    
    int currPageNum;
    char *currPageBuf;
    AM_INTHEADER header;
    int firstPageOfLevel = -1; 
    
    if (numEntries == 0) return AME_OK; 

    parentEntries = (ParentEntry *)malloc(sizeof(ParentEntry) * maxParentEntries);
    if (parentEntries == NULL) return AME_NOMEM;
    if (PF_AllocPage(amFileDesc, &currPageNum, &currPageBuf) != PFE_OK) {
        free(parentEntries);
        return AME_PF;
    }
    firstPageOfLevel = currPageNum;
    InitInternalHeader(currPageBuf, attrLength, maxKeys);
    AM_bcopy(currPageBuf, (char *)&header, AM_sint);
    int currentChildPtr = leftmostChildPage; 
    AM_bcopy((char *)&currentChildPtr, currPageBuf + AM_sint, AM_si);

    for (int i = 0; i < numEntries; i++)
    {
        if (PackInternalPage(currPageBuf, &header, levelEntries[i].key, levelEntries[i].pageNum, attrLength) == PAGE_FULL)
        {
            AM_bcopy((char *)&header, currPageBuf, AM_sint);
            if (PF_UnfixPage(amFileDesc, currPageNum, TRUE) != PFE_OK) {
                free(parentEntries);
                return AME_PF;
            }
            if (PF_AllocPage(amFileDesc, &currPageNum, &currPageBuf) != PFE_OK) {
                free(parentEntries);
                return AME_PF;
            }
            InitInternalHeader(currPageBuf, attrLength, maxKeys);
            AM_bcopy(currPageBuf, (char *)&header, AM_sint);
            if (numParentEntries >= maxParentEntries) {
                maxParentEntries *= 2;
                parentEntries = (ParentEntry *)realloc(parentEntries, sizeof(ParentEntry) * maxParentEntries);
                if (parentEntries == NULL) return AME_NOMEM;
            }
            AM_bcopy(levelEntries[i].key, parentEntries[numParentEntries].key, attrLength);
            parentEntries[numParentEntries].pageNum = currPageNum;
            numParentEntries++;
            currentChildPtr = levelEntries[i].pageNum;
            AM_bcopy((char *)&currentChildPtr, currPageBuf + AM_sint, AM_si);
            if (PackInternalPage(currPageBuf, &header, levelEntries[i].key, levelEntries[i].pageNum, attrLength) != PAGE_OK) {
                printf("AM_BulkLoad: FATAL: Pack failed on new internal page\n");
                free(parentEntries);
                return AME_INTERROR;
            }
        }
    }
    AM_bcopy((char *)&header, currPageBuf, AM_sint);
    if (PF_UnfixPage(amFileDesc, currPageNum, TRUE) != PFE_OK) {
        free(parentEntries);
        return AME_PF;
    }
    
    if (numParentEntries == 0) {
        char *newRootBuf;
        char *page0Buf;
        if (PF_GetThisPage(amFileDesc, firstPageOfLevel, &newRootBuf) != PFE_OK) { 
            PF_PrintError("BuildInternalLevel: Error getting new root");
            free(parentEntries); 
            return AME_PF; 
        }
        if (PF_GetThisPage(amFileDesc, 0, &page0Buf) != PFE_OK) { 
            PF_PrintError("BuildInternalLevel: Error getting page 0");
            free(parentEntries); 
            return AME_PF; 
        }
        AM_bcopy(newRootBuf, page0Buf, PF_PAGE_SIZE);

        if (PF_UnfixPage(amFileDesc, 0, TRUE) != PFE_OK) { free(parentEntries); return AME_PF; }
        if (PF_UnfixPage(amFileDesc, firstPageOfLevel, FALSE) != PFE_OK) { free(parentEntries); return AME_PF; }
        
        if (PF_DisposePage(amFileDesc, firstPageOfLevel) != PFE_OK) { free(parentEntries); return AME_PF; }

        *rootPageNum = 0;

    } else {
        // We have a new, smaller list of parent entries.
        // Recurse to build the level above this one.
        // The *new* leftmost child is the *first* page we just built (firstPageOfLevel)
        int newRootPageNumFromRecurse = -1;
        if (BuildInternalLevel(amFileDesc, parentEntries, numParentEntries, 
                               attrLength, maxKeys, firstPageOfLevel, &newRootPageNumFromRecurse) != AME_OK) {
            free(parentEntries);
            return AME_PF;
        }
        *rootPageNum = newRootPageNumFromRecurse;
    }

    free(parentEntries);
    return AME_OK;
}