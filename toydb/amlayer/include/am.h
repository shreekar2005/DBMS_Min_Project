/**
 * @file am.h
 * @brief Header file for the Access Method (AM) layer.
 */

#ifndef AM_H
#define AM_H

#include <string.h>
#include <stdlib.h>
#include "testam.h"

/**
 * @brief Header for a leaf page.
 */
typedef struct am_leafheader
{
    char pageType;
    int nextLeafPage;
    short recIdPtr;
    short keyPtr;
    short freeListPtr;
    short numinfreeList;
    short attrLength;
    short numKeys;
    short maxKeys;
} AM_LEAFHEADER;

/**
 * @brief Header for an internal node.
 */
typedef struct am_intheader
{
    char pageType;
    short numKeys;
    short maxKeys;
    short attrLength;
} AM_INTHEADER;

/* Globals */
extern int AM_RootPageNum; /* The page number of the root */
extern int AM_LeftPageNum; /* The page Number of the leftmost leaf */
extern int AM_Errno;       /* last error in AM layer */

/* Utility Macros */
#define AM_Check if (errVal != PFE_OK) { AM_Errno = AME_PF; return (AME_PF); }
#define AM_Check_Void if (errVal != PFE_OK) { AM_Errno = AME_PF; return; }
#define AM_si sizeof(int)
#define AM_ss sizeof(short)
#define AM_sl sizeof(AM_LEAFHEADER)
#define AM_sint sizeof(AM_INTHEADER)
#define AM_sc sizeof(char)
#define AM_sf sizeof(float)

/* Constants */
#define AM_NOT_FOUND 0 /* Key is not in tree */
#define AM_FOUND 1     /* Key is in tree */
#define AM_NULL 0      /* Null pointer for lists in a page */
#define AM_MAX_FNAME_LENGTH 80
#define AM_NULL_PAGE -1
#define FREE 0 /* Free is chosen to be zero because C initialises all variables to zero and we require that our scan table be initialised */
#define FIRST 1
#define BUSY 2
#define LAST 3
#define OVER 4
#define ALL 0
#define EQUAL 1
#define LESS_THAN 2
#define GREATER_THAN 3
#define LESS_THAN_EQUAL 4
#define GREATER_THAN_EQUAL 5
#define NOT_EQUAL 6
#define MAXSCANS 20
#define AM_MAXATTRLENGTH 256

/* Error codes */
#define AME_OK 0
#define AME_INVALIDATTRLENGTH -1
#define AME_NOTFOUND -2
#define AME_PF -3
#define AME_INTERROR -4
#define AME_INVALID_SCANDESC -5
#define AME_INVALID_OP_TO_SCAN -6
#define AME_EOF -7
#define AME_SCAN_TAB_FULL -8
#define AME_INVALIDATTRTYPE -9
#define AME_FD -10
#define AME_INVALIDVALUE -11
#define AME_BULKLOADFAILED -12
#define AME_NOMEM -12


/* am.c */

/**
 * @brief Splits a leaf node.
 * @param fileDesc File descriptor.
 * @param pageBuf Pointer to buffer.
 * @param pageNum Pagenumber of new leaf created (output).
 * @param attrLength Length of the attribute.
 * @param recId Record ID for insert.
 * @param value Attribute value for insert.
 * @param status Whether key was found or not in the tree.
 * @param index Place where key is to be inserted.
 * @param key Returns the key to be filled in the parent (output).
 * @return TRUE if a key needs to be added to the parent, FALSE otherwise.
 */
int AM_SplitLeaf(int fileDesc, char *pageBuf, int *pageNum, int attrLength, int recId, char *value, int status, int index, char *key);

/**
 * @brief Adds a key and page number to a parent internal node.
 * @param fileDesc File descriptor.
 * @param pageNum Page Number to be added to parent.
 * @param value Pointer to attribute value to be added.
 * @param attrLength Length of the attribute.
 * @return AME_OK on success, or an error code.
 */
int AM_AddtoParent(int fileDesc, int pageNum, char *value, int attrLength);

/**
 * @brief Adds a key to an internal node.
 * @param pageBuf Buffer for the internal node page.
 * @param value Value to be added to the node.
 * @param pageNum Page number of child to be inserted.
 * @param header Pointer to the page header.
 * @param offset Place where key is to be inserted.
 */
void AM_AddtoIntPage(char *pageBuf, char *value, int pageNum, AM_INTHEADER *header, int offset);

/**
 * @brief Fills the header and inserts a key into a new root.
 * @param pageBuf Buffer to new root.
 * @param pageNum1 Page number of the first child.
 * @param pageNum2 Page number of the second child.
 * @param value Attribute value to be inserted.
 * @param attrLength Attribute length.
 * @param maxKeys Maximum keys in a node.
 */
void AM_FillRootPage(char *pageBuf, int pageNum1, int pageNum2, char *value, short attrLength, short maxKeys);

/**
 * @brief Splits an internal node.
 * @param pageBuf Internal node to be split.
 * @param pbuf1 Buffer for the first half.
 * @param pbuf2 Buffer for the second half.
 * @param header Pointer to the page header.
 * @param value Pointer to key to be added and to be returned to parent.
 * @param pageNum Page number of the child.
 * @param offset Offset where key is to be inserted.
 */
void AM_SplitIntNode(char *pageBuf, char *pbuf1, char *pbuf2, AM_INTHEADER *header, char *value, int pageNum, int offset);

/**
 * @brief A non-standard memory copy function.
 * @param s1 Source buffer.
 * @param s2 Destination buffer.
 * @param nbytes Number of bytes to copy.
 */
void AM_bcopy(char *s1, char *s2, int nbytes);

/* amfns.c */

/**
 * @brief Creates a secondary index file called fileName.indexNo.
 * @param fileName Name of indexed file.
 * @param indexNo Number of this index for file.
 * @param attrType 'c' for char, 'i' for int, 'f' for float.
 * @param attrLength 4 for 'i' or 'f', 1-255 for 'c'.
 * @return AME_OK on success, or an error code.
 */
int AM_CreateIndex(char *fileName, int indexNo, char attrType, int attrLength);

/**
 * @brief Destroys the index fileName.indexNo.
 * @param fileName Name of indexed file.
 * @param indexNo Number of this index for file.
 * @return AME_OK on success, or an error code.
 */
int AM_DestroyIndex(char *fileName, int indexNo);

/**
 * @brief Deletes the recId from the list for value and deletes value if list becomes empty.
 * @param fileDesc File Descriptor.
 * @param attrType 'c', 'i' or 'f'.
 * @param attrLength 4 for 'i' or 'f', 1-255 for 'c'.
 * @param value Value of key whose corr recId is to be deleted.
 * @param recId ID of the record to delete.
 * @return AME_OK on success, or an error code.
 */
int AM_DeleteEntry(int fileDesc, char attrType, int attrLength, char *value, int recId);

/**
 * @brief Inserts a value,recId pair into the tree.
 * @param fileDesc File Descriptor.
 * @param attrType 'i' or 'c' or 'f'.
 * @param attrLength 4 for 'i' or 'f', 1-255 for 'c'.
 * @param value Value to be inserted.
 * @param recId recId to be inserted.
 * @return AME_OK on success, or an error code.
 */
int AM_InsertEntry(int fileDesc, char attrType, int attrLength, char *value, int recId);

/**
 * @brief Prints the AM layer error message.
 * @param s String to print before the error message.
 */
void AM_PrintError(const char *s);

/* aminsert.c */

/**
 * @brief Inserts a key into a leaf node.
 * @param pageBuf Buffer where the leaf page resides.
 * @param attrLength Length of the attribute.
 * @param value Attribute value to be inserted.
 * @param recId Recid of the attribute to be inserted.
 * @param index Index where key is to be inserted.
 * @param status Whether key is a new key or an old key.
 * @return TRUE if inserted, FALSE if split is needed, or an error code.
 */
int AM_InsertintoLeaf(char *pageBuf, int attrLength, char *value, int recId, int index, int status);

/**
 * @brief Insert into leaf given the fact that the key is old.
 * @param pageBuf Buffer for the leaf page.
 * @param recId Record ID to insert.
 * @param index Index where key is located.
 * @param header Pointer to the page header.
 */
void AM_InsertToLeafFound(char *pageBuf, int recId, int index, AM_LEAFHEADER *header);

/**
 * @brief Insert to a leaf given that the key is new.
 * @param pageBuf Buffer for the leaf page.
 * @param value New attribute value.
 * @param recId Record ID to insert.
 * @param index Index where key should be inserted.
 * @param header Pointer to the page header.
 */
void AM_InsertToLeafNotFound(char *pageBuf, char *value, int recId, int index, AM_LEAFHEADER *header);

/**
 * @brief Compacts all the recid's to the right so that there is enough space in the middle.
 * @param low Lower bound of keys to compact.
 * @param high Upper bound of keys to compact.
 * @param pageBuf Source page buffer.
 * @param tempPage Destination temporary page buffer.
 * @param header Pointer to the source page header.
 */
void AM_Compact(int low, int high, char *pageBuf, char *tempPage, AM_LEAFHEADER *header);

/* amprint.c */

/**
 * @brief Prints the contents of an internal node.
 * @param pageBuf Buffer containing the internal node page.
 * @param attrType Attribute type ('c', 'i', 'f').
 */
void AM_PrintIntNode(char *pageBuf, char attrType);

/**
 * @brief Prints the contents of a leaf node.
 * @param pageBuf Buffer containing the leaf node page.
 * @param attrType Attribute type ('c', 'i', 'f').
 */
void AM_PrintLeafNode(char *pageBuf, char attrType);

/**
 * @brief Dumps all leaf pages starting from a minimum value.
 * @param fileDesc File descriptor.
 * @param min Minimum value to start from (not fully implemented in original).
 * @param attrType Attribute type.
 * @param attrLength Attribute length.
 */
void AM_DumpLeafPages(int fileDesc, int min, char attrType, int attrLength);

/**
 * @brief Prints the keys from a leaf page.
 * @param pageBuf Buffer containing the leaf page.
 * @param attrType Attribute type.
 */
void AM_PrintLeafKeys(char *pageBuf, char attrType);

/**
 * @brief Prints an attribute value based on its type.
 * @param bufPtr Pointer to the attribute data.
 * @param attrType Attribute type.
 * @param attrLength Attribute length.
 */
void AM_PrintAttr(char *bufPtr, char attrType, int attrLength);

/**
 * @brief Recursively prints the B+ tree structure.
 * @param fileDesc File descriptor.
 * @param pageNum Page number of the current node to print.
 * @param attrType Attribute type.
 */
void AM_PrintTree(int fileDesc, int pageNum, char attrType);

/* amscan.c */

/**
 * @brief Opens an index scan.
 * @param fileDesc File Descriptor.
 * @param attrType 'i' or 'c' or 'f'.
 * @param attrLength 4 for 'i' or 'f', 1-255 for 'c'.
 * @param op Operator for comparison.
 * @param value Value for comparison.
 * @return A scan descriptor on success, or an error code.
 */
int AM_OpenIndexScan(int fileDesc, char attrType, int attrLength, int op, char *value);

/**
 * @brief Returns the record id of the next record that satisfies the conditions.
 * @param scanDesc Index scan descriptor.
 * @return The record ID, or an error code.
 */
int AM_FindNextEntry(int scanDesc);

/**
 * @brief Terminates an index scan.
 * @param scanDesc Scan Descriptor.
 * @return AME_OK on success, or an error code.
 */
int AM_CloseIndexScan(int scanDesc);

/**
 * @brief Gets the page number of the leftmost leaf.
 * @param fileDesc File descriptor.
 * @return The page number of the leftmost leaf.
 */
int GetLeftPageNum(int fileDesc);

/* amsearch.c */

/**
 * @brief Searches for a key in a B+ tree.
 *
 * Returns FOUND or NOTFOUND and returns the pagenumber and the offset
 * where key is present or could be inserted.
 *
 * @param fileDesc File descriptor.
 * @param attrType Attribute type.
 * @param attrLength Attribute length.
 * @param value Value to search for.
 * @param pageNum Output: page number of page where key is present or can be inserted.
 * @param pageBuf Output: pointer to buffer in memory where leaf page can be found.
 * @param indexPtr Output: pointer to index in leaf where key is present or can be inserted.
 * @return AM_FOUND or AM_NOT_FOUND, or an error code.
 */
int AM_Search(int fileDesc, char attrType, int attrLength, char *value, int *pageNum, char **pageBuf, int *indexPtr);

/**
 * @brief Finds the place (index) from where the next page to be followed is got.
 * @param pageBuf Buffer where the page is found.
 * @param attrType Attribute type.
 * @param attrLength Attribute length.
 * @param value Attribute value for which search is called.
 * @param indexPtr Output: index of the child pointer to follow.
 * @param header Pointer to the internal page header.
 * @return Page number of the child node to follow.
 */
int AM_BinSearch(char *pageBuf, char attrType, int attrLength, char *value, int *indexPtr, AM_INTHEADER *header);

/**
 * @brief Search a leaf node for the key.
 * @param pageBuf Buffer where the leaf page resides.
 * @param attrType Attribute type.
 * @param attrLength Attribute length.
 * @param value Attribute value to be compared with.
 * @param indexPtr Output: pointer to the index where key is found or can be inserted.
 * @param header Pointer to the leaf page header.
 * @return AM_FOUND or AM_NOT_FOUND.
 */
int AM_SearchLeaf(char *pageBuf, char attrType, int attrLength, char *value, int *indexPtr, AM_LEAFHEADER *header);

/**
 * @brief Compare value in bufPtr with value in valPtr.
 * @param bufPtr Pointer to the first value.
 * @param attrType Attribute type.
 * @param attrLength Attribute length.
 * @param valPtr Pointer to the second value.
 * @return -1, 0, or 1 if valPtr is less than, equal to, or greater than bufPtr.
 */
int AM_Compare(char *bufPtr, char attrType, int attrLength, char *valPtr);

/* amstack.c */

/**
 * @brief Pushes a page number and offset onto the stack.
 * @param pageNum The page number to push.
 * @param offset The offset to push.
 */
void AM_PushStack(int pageNum, int offset);

/**
 * @brief Pops the top element from the stack.
 */
void AM_PopStack(void);

/**
 * @brief Gets the top element of the stack without popping it.
 * @param pageNum Output: The page number from the top of the stack.
 * @param offset Output: The offset from the top of the stack.
 */
void AM_topofStack(int *pageNum, int *offset);

/**
 * @brief Empties the stack.
 */
void AM_EmptyStack(void);

/* am_bulkload.c */

/**
 * @brief Efficiently builds an index from a pre-sorted data file.
 * @param amFileDesc File descriptor for the index.
 * @param spFileDesc File descriptor for the sorted splayer data file.
 * @param attrType 'c' for char, 'i' for int, 'f' for float.
 * @param attrLength 4 for 'i' or 'f', 1-255 for 'c'.
 * @return AME_OK on success, or an error code.
 */
int AM_BulkLoad(int amFileDesc, int spFileDesc, char attrType, int attrLength);


/* misc.c */

/**
 * @brief Pads the end of a string with '\\0' up to a given length.
 * @param str The string to pad.
 * @param length The total desired length.
 */
void padstring(char *str, int length);

/**
 * @brief Wrapper for AM_CreateIndex that exits on failure.
 */
int xAM_CreateIndex(char *fname, int indexno, char attrtype, int attrlen);

/**
 * @brief Wrapper for AM_DestroyIndex that exits on failure.
 */
int xAM_DestroyIndex(char *fname, int indexno);

/**
 * @brief Wrapper for AM_InsertEntry that exits on failure.
 */
int xAM_InsertEntry(int fd, char attrtype, int attrlen, char *val, RecIdType recid);

/**
 * @brief Wrapper for AM_DeleteEntry that exits on failure.
 */
int xAM_DeleteEntry(int fd, char attrtype, int attrlen, char *val, RecIdType recid);

/**
 * @brief Wrapper for AM_OpenIndexScan that exits on failure.
 */
int xAM_OpenIndexScan(int fd, char attrtype, int attrlen, int op, char *val);

/**
 * @brief Wrapper for AM_FindNextEntry that exits on failure.
 */
RecIdType xAM_FindNextEntry(int sd);

/**
 * @brief Wrapper for AM_CloseIndexScan that exits on failure.
 */
int xAM_CloseIndexScan(int sd);

/**
 * @brief Wrapper for PF_OpenFile that exits on failure.
 */
int xPF_OpenFile(char *fname);

/**
 * @brief Wrapper for PF_CloseFile that exits on failure.
 */
int xPF_CloseFile(int fd);

#endif /* AM_H */