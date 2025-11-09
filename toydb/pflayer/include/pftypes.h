/**
 * @file pftypes.h
 * @brief Declarations for the Paged File interface.
 */

#ifndef PFTYPES_H
#define PFTYPES_H

#include "../include/pf.h"

#define STRATEGY_LRU 0 /**LRU for Buffer manager */
#define STRATEGY_MRU 1 /**MRU for Buffer manager */
#define DEFAULT_MAX_PAGE 20

extern int PFmaxbpage;

/**************************** File Page Decls *********************/

/**
 * @brief File header structure.
 *
 * Each file contains a header, which is an integer pointing
 * to the first free page, or -1 if no more free pages in the file.
 * Followed by this header are the file pages as declared in struct PFfpage.
 */
typedef struct PFhdr_str
{
	int firstfree; /**< first free page in the linked list of free pages */
	int numpages;  /**< # of pages in the file */
} PFhdr_str;

#define PF_HDR_SIZE sizeof(PFhdr_str) /**< size of file header */

/* actual page struct to be written onto the file */
#define PF_PAGE_LIST_END -1 /**< end of list of free pages */
#define PF_PAGE_USED -2		/**< page is being used */

/**
 * @brief Structure of a page on disk.
 */
typedef struct PFfpage
{
	/**
	 * page number of next free page in the linked list of free pages,
	 * or PF_PAGE_LIST_END if end of list,
	 * or PF_PAGE_USED if this page is not free
	 */
	int nextfree;
	char pagebuf[PF_PAGE_SIZE]; /**< actual page data */
} PFfpage;

/*************************** Opened File Table **********************/
#define PF_FTAB_SIZE 20 /**< size of open file table */

/**
 * @brief Open file table entry.
 */
typedef struct PFftab_ele
{
	char *fname;	  /**< file name, or NULL if entry not used */
	int unixfd;		  /**< unix file descriptor*/
	PFhdr_str hdr;	  /**< file header */
	short hdrchanged; /**< TRUE if file header has changed */
} PFftab_ele;

/************************** Buffer Page Decls *********************/
#define PF_DEFAULT_BUFS 20 /**< default max # of buffers */

/**
 * @brief Buffer page declaration.
 */
typedef struct PFbpage
{
	struct PFbpage *nextpage; /**< next in the linked list of buffer page */
	struct PFbpage *prevpage; /**< previous in the linked list of buffer pages */
	short dirty : 1;		  /**< TRUE if page is dirty */
	short fixed : 1;		  /**< TRUE if page is fixed in buffer*/
	int page;				  /**< page number of this page */
	int fd;					  /**< file desciptor of this page */
	PFfpage fpage;			  /**< page data from the file */
} PFbpage;

/******************** Hash Table Decls ****************************/
#define PF_HASH_TBL_SIZE 20 /**< size of PF hash table */

/**
 * @brief Hash table bucket entries.
 */
typedef struct PFhash_entry
{
	struct PFhash_entry *nextentry; /**< next hash table element, or NULL */
	struct PFhash_entry *preventry; /**<previous hash table element,or NULL*/
	int fd;							/**< file descriptor */
	int page;						/**< page number */
	struct PFbpage *bpage;			/**< pointer to buffer holding this page */
} PFhash_entry;

/**
 * @brief Hash function for hash table.
 * @param fd File descriptor.
 * @param page Page number.
 */
#define PFhash(fd, page) (((fd) + (page)) % PF_HASH_TBL_SIZE)

/******************* Interface functions from Hash Table ****************/

void PFhashInit(void);
PFbpage *PFhashFind(int fd, int page);
int PFhashInsert(int fd, int page, PFbpage *bpage);
int PFhashDelete(int fd, int page);
void PFhashPrint(void);

/****************** Interface functions from Buffer Manager *************/
/**
 * @brief Get a page from the buffer.
 *
 * Get a page whose number is "pagenum" from the file pointed
 * by "fd". Set *fpage to point to the data for that page.
 *
 * @param fd File descriptor.
 * @param pagenum Page number.
 * @param fpage Pointer to pointer to file page.
 * @param readfcn Function to read a page.
 * @param writefcn Function to write a page.
 * @return PFE_OK if no error, PF error code if error.
 */
int PFbufGet(int fd, int pagenum, PFfpage **fpage, int (*readfcn)(int, int, PFfpage*), int (*writefcn)(int, int, PFfpage*));

/**
 * @brief Unfix a page in the buffer.
 *
 * Unfix the file page whose number is "pagenum" from the buffer.
 * If dirty is TRUE, then mark the buffer as having been modified.
 * Otherwise, the dirty flag is left unchanged.
 *
 * @param fd File descriptor.
 * @param pagenum Page number.
 * @param dirty TRUE if page is dirty.
 * @return PFE_OK if no error, PF error codes if error occurs.
 */
int PFbufUnfix(int fd, int pagenum, int dirty);

/**
 * @brief Allocate a buffer and mark it belonging to a page.
 *
 * Allocate a buffer and mark it belonging to page "pagenum"
 * of file "fd".  Set *fpage to point to the buffer data.
 *
 * @param fd File descriptor.
 * @param pagenum Page number.
 * @param fpage Pointer to file page.
 * @param writefcn Function to write out pages.
 * @return PFE_OK if successful, PF error codes if unsuccessful.
 */
int PFbufAlloc(int fd, int pagenum, PFfpage **fpage, int (*writefcn)(int, int, PFfpage*));

/**
 * @brief Release all pages of a file from the buffer.
 *
 * Release all pages of file "fd" from the buffer and
 * put them into the free list.
 *
 * @param fd File descriptor.
 * @param writefcn Function to write a page of file.
 * @return PFE_OK if no error, PF error code if error.
 */
int PFbufReleaseFile(int fd, int (*writefcn)(int, int, PFfpage*));

/**
 * @brief Mark a page as used.
 *
 * Mark page numbered "pagenum" of file descriptor "fd" as used.
 * The page must be fixed in the buffer. Make this page most
 * recently used.
 *
 * @param fd File descriptor.
 * @param pagenum Page number.
 * @return PF error codes.
 */
int PFbufUsed(int fd, int pagenum);

/**
 * @brief Print the current page buffers.
 */
void PFbufPrint(void);

/**
 * @brief Set the maximum number of buffer pages.
 * @param num_bufs The maximum number of buffer pages.
 */
void PFbufSetNumPages(int num_bufs);

/**
 * @brief Set the page replacement strategy.
 * @param strategy The strategy (STRATEGY_LRU or STRATEGY_MRU).
 */
void PFbufSetStrategy(int strategy);

/**
 * @brief Resest statistics for logical reads, physical reads, number of hits
 */
void PF_ResetStats(void);

/**
 * @brief Print statistics for logical reads, physical reads, number of hits
 */
void PF_PrintStats(void);

#endif /* PFTYPES_H */
