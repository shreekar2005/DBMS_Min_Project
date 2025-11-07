/**
 * @file buf.c
 * @brief Buffer management routines.
 *
 * Interface routines are: PFbufGet(), PFbufUnfix(), PFbufAlloc(),
 * PFbufReleaseFile(), PFbufUsed() and PFbufPrint().
 */

#include <stdio.h>
#include <stdlib.h>
#include "../include/pf.h"
#include "../include/pftypes.h"

static int PFnumbpage = 0;			 /**< # of buffer pages in memory */
static PFbpage *PFfirstbpage = NULL; /**< ptr to first buffer page, or NULL */
static PFbpage *PFlastbpage = NULL;	 /**< ptr to last buffer page, or NULL */
static PFbpage *PFfreebpage = NULL;	 /**< list of free buffer pages */

/* Static function prototypes */
static void PFbufInsertFree(PFbpage *bpage);
static void PFbufLinkHead(PFbpage *bpage);
static void PFbufUnlink(PFbpage *bpage);
static int PFbufInternalAlloc(PFbpage **bpage, int (*writefcn)(int, int, PFfpage *));

/**
 * @brief Insert the buffer page pointed by "bpage" into the free list.
 * @param bpage The buffer page to insert.
 */
static void PFbufInsertFree(PFbpage *bpage)
{
	bpage->nextpage = PFfreebpage;
	PFfreebpage = bpage;
}

/**
 * @brief Link the buffer page pointed by "bpage" as the head of the used buffer list.
 * @param bpage Pointer to buffer page to be linked.
 */
static void PFbufLinkHead(PFbpage *bpage)
{
	bpage->nextpage = PFfirstbpage;
	bpage->prevpage = NULL;
	if (PFfirstbpage != NULL)
		PFfirstbpage->prevpage = bpage;
	PFfirstbpage = bpage;
	if (PFlastbpage == NULL)
		PFlastbpage = bpage;
}

/**
 * @brief Unlink the page pointed by bpage from the buffer list.
 *
 * Assume that bpage is a valid pointer. Set the "prevpage" and "nextpage"
 * fields to NULL. The caller is responsible to either place
 * the unlinked page into the free list, or insert it back
 * into the used list.
 * @param bpage Buffer page to be unlinked from the used list.
 */
static void PFbufUnlink(PFbpage *bpage)
{
	if (PFfirstbpage == bpage)
		PFfirstbpage = bpage->nextpage;

	if (PFlastbpage == bpage)
		PFlastbpage = bpage->prevpage;

	if (bpage->nextpage != NULL)
		bpage->nextpage->prevpage = bpage->prevpage;

	if (bpage->prevpage != NULL)
		bpage->prevpage->nextpage = bpage->nextpage;

	bpage->prevpage = bpage->nextpage = NULL;
}

/**
 * @brief Allocate a buffer page.
 *
 * Allocate a buffer page and set *bpage to point to it. *bpage
 * is set to NULL if one can not be allocated.
 * The "nextpage" and "prevpage" fields of *bpage are linked as
 * the head of the list of used buffers. All the other fields are undefined.
 *
 * @param bpage Pointer to pointer to buffer bpage to be allocated.
 * @param writefcn Function to write pages.
 * @return PFE_OK if no error, PFE_NOMEM if no memory, PFE_NOBUF if no buffer space.
 */
static int PFbufInternalAlloc(PFbpage **bpage, int (*writefcn)(int, int, PFfpage *))
{
	PFbpage *tbpage; /* temporary pointer to buffer page */
	int error;		 /* error value returned*/

	/* Set *bpage to the buffer page to be returned */
	if (PFfreebpage != NULL)
	{
		/* Free list not empty, use the one from the free list. */
		*bpage = PFfreebpage;
		PFfreebpage = (*bpage)->nextpage;
	}
	else if (PFnumbpage < PF_MAX_BUFS)
	{
		/* We have not reached max buffer limit, so malloc() a new one */
		if ((*bpage = (PFbpage *)malloc(sizeof(PFbpage))) == NULL)
		{
			/* no mem */
			*bpage = NULL;
			PFerrno = PFE_NOMEM;
			return (PFerrno);
		}
		/* increment # of pages allocated */
		PFnumbpage++;
	}
	else
	{
		/* we have reached max buffer limit */
		/* choose a victim from the buffer*/

		*bpage = NULL; /* set initial return value */

		for (tbpage = PFlastbpage; tbpage != NULL; tbpage = tbpage->prevpage)
		{
			if (!tbpage->fixed)
				/* found a page that can be swapped out */
				break;
		}

		if (tbpage == NULL)
		{
			/* couldn't find a free page */
			PFerrno = PFE_NOBUF;
			return (PFerrno);
		}

		/* write out the dirty page */
		if (tbpage->dirty && ((error = (*writefcn)(tbpage->fd,
												   tbpage->page, &tbpage->fpage)) != PFE_OK))
			return (error);
		tbpage->dirty = FALSE;

		/* unlink from hash table */
		if ((error = PFhashDelete(tbpage->fd, tbpage->page)) != PFE_OK)
			return (error);

		/* unlink from buffer list */
		PFbufUnlink(tbpage);

		*bpage = tbpage;
	}

	/* Link the page as the head of the used list */
	PFbufLinkHead(*bpage);
	return (PFE_OK);
}

/************************* Interface to the Outside World ****************/

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
int PFbufGet(int fd, int pagenum, PFfpage **fpage, int (*readfcn)(int, int, PFfpage *), int (*writefcn)(int, int, PFfpage *))
{
	PFbpage *bpage; /* pointer to buffer */
	int error;

	if ((bpage = PFhashFind(fd, pagenum)) == NULL)
	{
		/* page not in buffer. */

		/* allocate an empty page */
		if ((error = PFbufInternalAlloc(&bpage, writefcn)) != PFE_OK)
		{
			/* error */
			*fpage = NULL;
			return (error);
		}

		/* read the page */
		if ((error = (*readfcn)(fd, pagenum, &bpage->fpage)) != PFE_OK)
		{
			/* error reading the page. put buffer back into
			the free list, and return gracefully */
			PFbufUnlink(bpage);
			PFbufInsertFree(bpage);
			*fpage = NULL;
			return (error);
		}

		/* insert new page into hash table */
		if ((error = PFhashInsert(fd, pagenum, bpage)) != PFE_OK)
		{
			/* failed to insert into hash table */
			/* put page into free list */
			PFbufUnlink(bpage);
			PFbufInsertFree(bpage);
			return (error);
		}

		/* set the fields for this page*/
		bpage->fd = fd;
		bpage->page = pagenum;
		bpage->dirty = FALSE;
	}
	else if (bpage->fixed)
	{
		/* page already in memory, and is fixed, so we can't
		get it again. */
		*fpage = &bpage->fpage;
		PFerrno = PFE_PAGEFIXED;
		return (PFerrno);
	}

	/* Fix the page in the buffer then return*/
	bpage->fixed = TRUE;
	*fpage = &bpage->fpage;
	return (PFE_OK);
}

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
int PFbufUnfix(int fd, int pagenum, int dirty)
{
	PFbpage *bpage;

	if ((bpage = PFhashFind(fd, pagenum)) == NULL)
	{
		/* page not in buffer */
		PFerrno = PFE_PAGENOTINBUF;
		return (PFerrno);
	}

	if (!bpage->fixed)
	{
		/* page already unfixed */
		PFerrno = PFE_PAGEUNFIXED;
		return (PFerrno);
	}

	if (dirty)
		/* mark this page dirty */
		bpage->dirty = TRUE;

	/* unfix the page */
	bpage->fixed = FALSE;

	/* unlink this page */
	PFbufUnlink(bpage);

	/* insert it as head of linked list to make it most recently used*/
	PFbufLinkHead(bpage);

	return (PFE_OK);
}

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
int PFbufAlloc(int fd, int pagenum, PFfpage **fpage, int (*writefcn)(int, int, PFfpage *))
{
	PFbpage *bpage;
	int error;

	*fpage = NULL; /* initial value of fpage */

	if ((bpage = PFhashFind(fd, pagenum)) != NULL)
	{
		/* page already in buffer*/
		PFerrno = PFE_PAGEINBUF;
		return (PFerrno);
	}

	if ((error = PFbufInternalAlloc(&bpage, writefcn)) != PFE_OK)
		/* can't get any buffer */
		return (error);

	/* put ourselves into the hash table */
	if ((error = PFhashInsert(fd, pagenum, bpage)) != PFE_OK)
	{
		/* can't insert into the hash table */
		/* unlink bpage, and put it into the free list */
		PFbufUnlink(bpage);
		PFbufInsertFree(bpage);
		return (error);
	}

	/* init the fields of bpage and return */
	bpage->fd = fd;
	bpage->page = pagenum;
	bpage->fixed = TRUE;
	bpage->dirty = FALSE;

	*fpage = &bpage->fpage;
	return (PFE_OK);
}

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
int PFbufReleaseFile(int fd, int (*writefcn)(int, int, PFfpage *))
{
	PFbpage *bpage; /* ptr to buffer pages to search */
	PFbpage *temppage;
	int error; /* error code */

	/* Do linear scan of the buffer to find pages belonging to the file */
	bpage = PFfirstbpage;
	while (bpage != NULL)
	{
		if (bpage->fd == fd)
		{
			/* The file descriptor matches*/
			if (bpage->fixed)
			{
				PFerrno = PFE_PAGEFIXED;
				return (PFerrno);
			}

			/* write out dirty page */
			if (bpage->dirty && ((error = (*writefcn)(fd, bpage->page,
													  &bpage->fpage)) != PFE_OK))
				/* error writing file */
				return (error);
			bpage->dirty = FALSE;

			/* get rid of it from the hash table */
			if ((error = PFhashDelete(fd, bpage->page)) != PFE_OK)
			{
				/* internal error */
				printf("Internal error:PFbufReleaseFile()\n");
				exit(1);
			}

			/* put the page into free list */
			temppage = bpage;
			bpage = bpage->nextpage;
			PFbufUnlink(temppage);
			PFbufInsertFree(temppage);
		}
		else
			bpage = bpage->nextpage;
	}
	return (PFE_OK);
}

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
int PFbufUsed(int fd, int pagenum)
{
	PFbpage *bpage; /* pointer to the bpage we are looking for */

	/* Find page in the buffer */
	if ((bpage = PFhashFind(fd, pagenum)) == NULL)
	{
		/* page not in the buffer */
		PFerrno = PFE_PAGENOTINBUF;
		return (PFerrno);
	}

	if (!(bpage->fixed))
	{
		/* page not fixed */
		PFerrno = PFE_PAGEUNFIXED;
		return (PFerrno);
	}

	/* mark this page dirty */
	bpage->dirty = TRUE;

	/* make this page head of the list of buffers*/
	PFbufUnlink(bpage);
	PFbufLinkHead(bpage);

	return (PFE_OK);
}

/**
 * @brief Print the current page buffers.
 */
void PFbufPrint(void)
{
	PFbpage *bpage;

	printf("buffer content:\n");
	if (PFfirstbpage == NULL)
		printf("empty\n");
	else
	{
		printf("fd\tpage\tfixed\tdirty\tfpage\n");
		for (bpage = PFfirstbpage; bpage != NULL; bpage = bpage->nextpage)
			printf("%d\t%d\t%d\t%d\t%p\n",
				   bpage->fd, bpage->page, (int)bpage->fixed,
				   (int)bpage->dirty, (void *)&bpage->fpage);
	}
}