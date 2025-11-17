/**
 * @file pf.h
 * @brief Externs and error codes for the Paged File Interface.
 */
#include "../include/pftypes.h"

#ifndef PF_H
#define PF_H

#ifndef TRUE
#define TRUE 1
#endif
#ifndef FALSE
#define FALSE 0
#endif

/************** Error Codes *********************************/
#define PFE_OK 0               /**< OK */
#define PFE_NOMEM -1           /**< no memory */
#define PFE_NOBUF -2           /**< no buffer space */
#define PFE_PAGEFIXED -3       /**< page already fixed in buffer */
#define PFE_PAGENOTINBUF -4    /**< page to be unfixed is not in the buffer */
#define PFE_UNIX -5            /**< unix error */
#define PFE_INCOMPLETEREAD -6  /**< incomplete read of page from file */
#define PFE_INCOMPLETEWRITE -7 /**< incomplete write of page to file */
#define PFE_HDRREAD -8         /**< incomplete read of header from file */
#define PFE_HDRWRITE -9        /**< incomplete write of header to file */
#define PFE_INVALIDPAGE -10    /**< invalid page number */
#define PFE_FILEOPEN -11       /**< file already open */
#define PFE_FTABFULL -12       /**< file table is full */
#define PFE_FD -13             /**< invalid file descriptor */
#define PFE_EOF -14            /**< end of file */
#define PFE_PAGEFREE -15       /**< page already free */
#define PFE_PAGEUNFIXED -16    /**< page already unfixed */

/* Internal error: please report to the TA */
#define PFE_PAGEINBUF -17     /**< new page to be allocated already in buffer */
#define PFE_HASHNOTFOUND -18  /**< hash table entry not found */
#define PFE_HASHPAGEEXIST -19 /**< page already exist in hash table */

#define PF_PAGE_SIZE 4096 /**< Page size in bytes */
#define PF_FTAB_SIZE 20 /**< size of open file table */

/* externs from the PF layer */
extern int PFerrno; /**< error number of last error */
extern long num_logical_reads;
extern long num_logical_writes;
extern long num_buffer_hits;
extern long num_physical_reads;
extern long num_physical_writes;
extern double hit_rate; //added new

/******************************** Interface for outside PFlayer *******************************/

/**
 * @brief Initialize the PF interface.
 * @param num_buffers Number of pages in buffer
 * @param replacement_strategy Define page replacement strategy; 0:LRU, 1:MRU
 */
void PF_Init(int num_buffers, int replacement_strategy);

/**
 * @brief Create a paged file called "fname". The file should not have already existed before.
 * @param fname Name of file to create.
 * @return PFE_OK if OK, PF error code if error.
 */
int PF_CreateFile(const char *fname);

/**
 * @brief Destroy the paged file whose name is "fname". The file should exist, and should not be already open.
 * @param fname File name to destroy.
 * @return PFE_OK if success, PF error codes if error.
 */
int PF_DestroyFile(const char *fname);

/**
 * @brief Open the paged file whose name is fname.
 * @param fname Name of the file to open.
 * @return File descriptor (>= 0) if no error, else PF error code.
 */
int PF_OpenFile(const char *fname);

/**
 * @brief Close the file indexed by file descriptor fd.
 * @param fd File descriptor to close.
 * @return PFE_OK if OK, PF error code if error.
 */
int PF_CloseFile(int fd);

/**
 * @brief Read the first page into memory and set *pagebuf to point to it.
 * @param fd File descriptor.
 * @param pagenum Page number of first page (output).
 * @param pagebuf Pointer to the pointer to buffer (output).
 * @return PFE_OK if no error, PFE_EOF if end of file reached.
 */
int PF_GetFirstPage(int fd, int *pagenum, char **pagebuf);

/**
 * @brief Read the next valid page after *pagenum.
 * @param fd File descriptor of the file.
 * @param pagenum Old page number on input, new page number on output.
 * @param pagebuf Pointer to pointer to buffer of page data (output).
 * @return PFE_OK if success, PFE_EOF if end of file reached.
 */
int PF_GetNextPage(int fd, int *pagenum, char **pagebuf);

/**
 * @brief Read the page specified by "pagenum".
 * @param fd File descriptor.
 * @param pagenum Page number to read.
 * @param pagebuf Pointer to pointer to page data (output).
 * @return PFE_OK if no error.
 */
int PF_GetThisPage(int fd, int pagenum, char **pagebuf);

/**
 * @brief Allocate a new, empty page for file "fd".
 * @param fd File descriptor.
 * @param pagenum New page number (output).
 * @param pagebuf Pointer to pointer to page buffer (output).
 * @return PFE_OK if ok, PF error codes if not ok.
 */
int PF_AllocPage(int fd, int *pagenum, char **pagebuf);

/**
 * @brief Dispose the page numbered "pagenum" of the file "fd".
 * @param fd File descriptor.
 * @param pagenum Page number.
 * @return PFE_OK if no error, PF error code if error.
 */
int PF_DisposePage(int fd, int pagenum);

/**
 * @brief Unfix a page in the buffer.
 * @param fd File descriptor.
 * @param pagenum Page number.
 * @param dirty TRUE if page has been modified.
 * @return PFE_OK if no error, PF error code if error.
 */
int PF_UnfixPage(int fd, int pagenum, int dirty);

/**
 * @brief Write the string "s" onto stderr, then write the last error message from PF onto stderr.
 * @param s String to write.
 */
void PF_PrintError(const char *s);

/**
 * @brief Resest statistics for logical reads, physical reads, number of hits
 */
void PF_ResetStats(void);

/**
 * @brief Print statistics for logical reads, physical reads, number of hits
 */
void PF_PrintStats(void);

#endif /* PF_H */