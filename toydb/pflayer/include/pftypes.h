/**
 * @file pftypes.h
 * @brief Declarations for the Paged File interface.
 */

#ifndef PFTYPES_H
#define PFTYPES_H

#include "../include/pf.h"

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

#define PF_HASH_TBL_SIZE 20 /**< size of PF hash table */

/**
 * @brief Hash function for hash table.
 * @param fd File descriptor.
 * @param page Page number.
 */
#define PFhash(fd, page) (((fd) + (page)) % PF_HASH_TBL_SIZE)

#endif /* PFTYPES_H */
