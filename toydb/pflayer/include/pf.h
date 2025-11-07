/**
 * @file pf.h
 * @brief Externs and error codes for the Paged File Interface.
 */

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

/* page size */
#define PF_PAGE_SIZE 4096

/* externs from the PF layer */
extern int PFerrno; /**< error number of last error */

/****************************************************************************
                Page File Level Interface
****************************************************************************/

/**
 * @brief Initialize the PF interface.
 */
void PF_Init(void);

/**
 * @brief Create a paged file called "fname".
 * @param fname Name of file to create.
 */
int PF_CreateFile(const char *fname);

/**
 * @brief Destroy the paged file whose name is "fname".
 * @param fname File name to destroy.
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
 */
int PF_CloseFile(int fd);

/**
 * @brief Read the first page into memory.
 * @param fd File descriptor.
 * @param pagenum Page number of first page (output).
 * @param pagebuf Pointer to the pointer to buffer (output).
 */
int PF_GetFirstPage(int fd, int *pagenum, char **pagebuf);

/**
 * @brief Read the next valid page after *pagenum.
 * @param fd File descriptor of the file.
 * @param pagenum Old page number on input, new page number on output.
 * @param pagebuf Pointer to pointer to buffer of page data (output).
 */
int PF_GetNextPage(int fd, int *pagenum, char **pagebuf);

/**
 * @brief Read the page specified by "pagenum".
 * @param fd File descriptor.
 * @param pagenum Page number to read.
 * @param pagebuf Pointer to pointer to page data (output).
 */
int PF_GetThisPage(int fd, int pagenum, char **pagebuf);

/**
 * @brief Allocate a new, empty page for file "fd".
 * @param fd File descriptor.
 * @param pagenum New page number (output).
 * @param pagebuf Pointer to pointer to page buffer (output).
 */
int PF_AllocPage(int fd, int *pagenum, char **pagebuf);

/**
 * @brief Dispose the page numbered "pagenum" of the file "fd".
 * @param fd File descriptor.
 * @param pagenum Page number.
 */
int PF_DisposePage(int fd, int pagenum);

/**
 * @brief Unfix a page in the buffer.
 * @param fd File descriptor.
 * @param pagenum Page number.
 * @param dirty TRUE if page has been modified.
 */
int PF_UnfixPage(int fd, int pagenum, int dirty);

/**
 * @brief Print the last PF error message.
 * @param s String to print before the error message.
 */
void PF_PrintError(const char *s);

#endif /* PF_H */