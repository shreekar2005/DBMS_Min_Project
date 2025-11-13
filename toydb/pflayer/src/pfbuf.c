/**
 * @file pfbuf.c
 * @brief Buffer management routines.
 *
 * Interface routines are: PFbufGet(), PFbufUnfix(), PFbufAlloc(),
 * PFbufReleaseFile(), PFbufUsed() and PFbufPrint().
 */

#include <stdio.h>
#include <stdlib.h>
#include "../include/pf.h"
#include "../include/pftypes.h"
#include "../include/pfbuf.h"
#include "../include/pfhash.h"

int PFmaxbpage=DEFAULT_MAX_PAGE;

static int PFnumbpage = 0;			  /**< # of buffer pages in memory */
static int PFstrategy = STRATEGY_LRU; /**< current buffer strategy */
static PFbpage *PFfirstbpage = NULL;  /**< ptr to first buffer page (head), or NULL */
static PFbpage *PFlastbpage = NULL;	  /**< ptr to last buffer page (tail), or NULL */
static PFbpage *PFfreebpage = NULL;	  /**< list of free buffer pages (tail) */


/**
 * @brief Insert the buffer page pointed by "bpage" into the head of free list.
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
	else if (PFnumbpage < PFmaxbpage)
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

		if (PFstrategy == STRATEGY_LRU)
		{
			// LRU: Scan from tail (Least Recently Used)
			for (tbpage = PFlastbpage; tbpage != NULL; tbpage = tbpage->prevpage)
			{
				if (!tbpage->fixed)
					break; // found victim
			}
		}
		else // else PFstrategy is MRU
		{
			// MRU: Scan from head
			for (tbpage = PFfirstbpage; tbpage != NULL; tbpage = tbpage->nextpage)
			{
				if (!tbpage->fixed)
					break; // found victim
			}
		}

		if (tbpage == NULL)
		{
			/* couldn't find a free page */
			PFerrno = PFE_NOBUF;
			return (PFerrno);
		}

		/* write out the dirty page */
		if (tbpage->dirty)
			num_physical_writes++;
		if (tbpage->dirty && ((error = (*writefcn)(tbpage->fd, tbpage->page, &tbpage->fpage)) != PFE_OK))
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


int PFbufGet(int fd, int pagenum, PFfpage **fpage, int (*readfcn)(int, int, PFfpage *), int (*writefcn)(int, int, PFfpage *))
{
	num_logical_reads++;
	PFbpage *bpage; /* pointer to buffer */
	int error;

	if ((bpage = PFhashFind(fd, pagenum)) == NULL)
	{
		/* page not in buffer. */

		num_physical_reads++;

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
	else
	{
		/* Page IS in the buffer, so it's a hit */
		num_buffer_hits++;

		if (bpage->fixed)
		{
			/* page already in memory, and is fixed, so we can't
			get it again. */
			*fpage = &bpage->fpage;
			PFerrno = PFE_PAGEFIXED;
			return (PFerrno);
		}
		/* If it's a hit and not fixed, we just fall through */
	}

	/* Fix the page in the buffer then return*/
	bpage->fixed = TRUE;
	*fpage = &bpage->fpage;
	return (PFE_OK);
}


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


int PFbufAlloc(int fd, int pagenum, PFfpage **fpage, int (*writefcn)(int, int, PFfpage *))
{
	num_logical_writes++;
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

			if (bpage->dirty)
				num_physical_writes++;
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

void PFbufSetNumPages(int num_bufs)
{
	PFmaxbpage = num_bufs;
}

void PFbufSetStrategy(int strategy)
{
	PFstrategy = strategy;
}