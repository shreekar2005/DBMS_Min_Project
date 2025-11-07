/**
 * @file pf.c
 * @brief Paged File Interface Routines and support routines.
 */

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <fcntl.h>
#include <sys/file.h>
#include "pf.h"
#include "pftypes.h"

/* To keep system V and PC users happy */
#ifndef L_SET
#define L_SET 0
#endif

int PFerrno = PFE_OK; /**< last error message */

static PFftab_ele PFftab[PF_FTAB_SIZE]; /**< table of opened files */

/**
 * @brief Check if a file descriptor is invalid.
 * @param fd The file descriptor to check.
 */
#define PFinvalidFd(fd) ((fd) < 0 || (fd) >= PF_FTAB_SIZE || PFftab[fd].fname == NULL)

/**
 * @brief Check if a page number is invalid for a given file.
 * @param fd The file descriptor.
 * @param pagenum The page number to check.
 */
#define PFinvalidPagenum(fd, pagenum) ((pagenum) < 0 || (pagenum) >= \
															PFftab[fd].hdr.numpages)

/****************** Internal Support Functions *****************************/
static char *savestr(const char *str);
static int PFtabFindFname(const char *fname);
static int PFftabFindFree(void);
static int PFreadfcn(int fd, int pagenum, PFfpage *buf);
static int PFwritefcn(int fd, int pagenum, PFfpage *buf);

/**
 * @brief Allocate memory and save a string.
 * @param str String to be saved.
 * @return A pointer to the saved string, or NULL if no memory.
 */
static char *savestr(const char *str)
{
	char *s;

	if ((s = (char *)malloc(strlen(str) + 1)) != NULL)
		strcpy(s, str);
	return (s);
}

/**
 * @brief Find the index to PFftab[] entry whose "fname" field is the same as "fname".
 * @param fname File name to find.
 * @return The desired index, or -1 if not found.
 */
static int PFtabFindFname(const char *fname)
{
	int i;

	for (i = 0; i < PF_FTAB_SIZE; i++)
	{
		if (PFftab[i].fname != NULL && strcmp(PFftab[i].fname, fname) == 0)
			/* found it */
			return (i);
	}
	return (-1);
}

/**
 * @brief Find a free entry in the open file table "PFtab".
 * @return If >=0, the index of the free entry. Otherwise, none can be found.
 */
static int PFftabFindFree(void)
{
	int i;

	for (i = 0; i < PF_FTAB_SIZE; i++)
		if (PFftab[i].fname == NULL)
			return (i);
	return (-1);
}

/**
 * @brief Read the paged numbered "pagenum" from the file indexed by "fd" into the page buffer "buf".
 * @param fd File descriptor.
 * @param pagenum Page number.
 * @param buf Buffer to read into.
 * @return PFE_OK if ok, PF error code if not OK.
 */
static int PFreadfcn(int fd, int pagenum, PFfpage *buf)
{
	int error;

	/* seek to the appropriate place */
	if ((error = lseek(PFftab[fd].unixfd, pagenum * sizeof(PFfpage) + PF_HDR_SIZE,
					   L_SET)) == -1)
	{
		PFerrno = PFE_UNIX;
		return (PFerrno);
	}

	/* read the data */
	if ((error = read(PFftab[fd].unixfd, (char *)buf, sizeof(PFfpage))) != sizeof(PFfpage))
	{
		if (error < 0)
			PFerrno = PFE_UNIX;
		else
			PFerrno = PFE_INCOMPLETEREAD;
		return (PFerrno);
	}

	return (PFE_OK);
}

/**
 * @brief Write the page numbered "pagenum" from the buffer indexed by "buf" into the file indexed by "fd".
 * @param fd File descriptor.
 * @param pagenum Page to write.
 * @param buf Buffer where to write the page from.
 * @return PFE_OK if ok, PF error code if not OK.
 */
static int PFwritefcn(int fd, int pagenum, PFfpage *buf)
{
	int error;

	/* seek to the right place */
	if ((error = lseek(PFftab[fd].unixfd, pagenum * sizeof(PFfpage) + PF_HDR_SIZE,
					   L_SET)) == -1)
	{
		PFerrno = PFE_UNIX;
		return (PFerrno);
	}

	/* write out the page */
	if ((error = write(PFftab[fd].unixfd, (char *)buf, sizeof(PFfpage))) != sizeof(PFfpage))
	{
		if (error < 0)
			PFerrno = PFE_UNIX;
		else
			PFerrno = PFE_INCOMPLETEWRITE;
		return (PFerrno);
	}

	return (PFE_OK);
}

/************************* Interface Routines ****************************/

void PF_Init(void)
{
	int i;
	/* init the hash table */
	PFhashInit();

	/* init the file table to be not used*/
	for (i = 0; i < PF_FTAB_SIZE; i++)
	{
		PFftab[i].fname = NULL;
	}
}


int PF_CreateFile(const char *fname)
{
	int fd;		   /* unix file descripotr */
	PFhdr_str hdr; /* file header */
	int error;

	/* create file for exclusive use */
	if ((fd = open(fname, O_CREAT | O_EXCL | O_WRONLY, 0664)) < 0)
	{
		/* unix error on open */
		PFerrno = PFE_UNIX;
		return (PFE_UNIX);
	}

	/* write out the file header */
	hdr.firstfree = PF_PAGE_LIST_END; /* no free pag yet */
	hdr.numpages = 0;
	if ((error = write(fd, (char *)&hdr, sizeof(hdr))) != sizeof(hdr))
	{
		/* error while writing. Abort everything. */
		if (error < 0)
			PFerrno = PFE_UNIX;
		else
			PFerrno = PFE_HDRWRITE;
		close(fd);
		unlink(fname);
		return (PFerrno);
	}

	if ((error = close(fd)) == -1)
	{
		PFerrno = PFE_UNIX;
		return (PFerrno);
	}

	return (PFE_OK);
}


int PF_DestroyFile(const char *fname)
{
	int error;

	if (PFtabFindFname(fname) != -1)
	{
		/* file is open */
		PFerrno = PFE_FILEOPEN;
		return (PFerrno);
	}

	if ((error = unlink(fname)) != 0)
	{
		/* unix error */
		PFerrno = PFE_UNIX;
		return (PFerrno);
	}

	/* success */
	return (PFE_OK);
}


int PF_OpenFile(const char *fname)
{
	int count; /* # of bytes in read */
	int fd;	   /* file descriptor */

	/* find a free entry in the file table */
	if ((fd = PFftabFindFree()) < 0)
	{
		/* file table full */
		PFerrno = PFE_FTABFULL;
		return (PFerrno);
	}

	/* open the file */
	if ((PFftab[fd].unixfd = open(fname, O_RDWR)) < 0)
	{
		/* can't open the file */
		PFerrno = PFE_UNIX;
		return (PFerrno);
	}

	/* Read the file header */
	if ((count = read(PFftab[fd].unixfd, (char *)&PFftab[fd].hdr, PF_HDR_SIZE)) != PF_HDR_SIZE)
	{
		if (count < 0)
			/* unix error */
			PFerrno = PFE_UNIX;
		else /* not enough bytes in file */
			PFerrno = PFE_HDRREAD;
		close(PFftab[fd].unixfd);
		return (PFerrno);
	}
	/* set file header to be not changed */
	PFftab[fd].hdrchanged = FALSE;

	/* save the file name */
	if ((PFftab[fd].fname = savestr(fname)) == NULL)
	{
		/* no memory */
		close(PFftab[fd].unixfd);
		PFerrno = PFE_NOMEM;
		return (PFerrno);
	}

	return (fd);
}


int PF_CloseFile(int fd)
{
	int error;

	if (PFinvalidFd(fd))
	{
		/* invalid file descriptor */
		PFerrno = PFE_FD;
		return (PFerrno);
	}

	/* Flush all buffers for this file */
	if ((error = PFbufReleaseFile(fd, PFwritefcn)) != PFE_OK)
		return (error);

	if (PFftab[fd].hdrchanged)
	{
		/* write the header back to the file */
		/* First seek to the appropriate place */
		if ((error = lseek(PFftab[fd].unixfd, (unsigned)0, L_SET)) == -1)
		{
			/* seek error */
			PFerrno = PFE_UNIX;
			return (PFerrno);
		}

		/* write header*/
		if ((error = write(PFftab[fd].unixfd, (char *)&PFftab[fd].hdr,
						   PF_HDR_SIZE)) != PF_HDR_SIZE)
		{
			if (error < 0)
				PFerrno = PFE_UNIX;
			else
				PFerrno = PFE_HDRWRITE;
			return (PFerrno);
		}
		PFftab[fd].hdrchanged = FALSE;
	}

	/* close the file */
	if ((error = close(PFftab[fd].unixfd)) == -1)
	{
		PFerrno = PFE_UNIX;
		return (PFerrno);
	}

	/* free the file name space */
	free((char *)PFftab[fd].fname);
	PFftab[fd].fname = NULL;

	return (PFE_OK);
}


int PF_GetFirstPage(int fd, int *pagenum, char **pagebuf)
{
	*pagenum = -1;
	return (PF_GetNextPage(fd, pagenum, pagebuf));
}


int PF_GetNextPage(int fd, int *pagenum, char **pagebuf)
{
	int temppage;	/* page number to scan for next valid page */
	int error;		/* error code */
	PFfpage *fpage; /* pointer to file page */

	if (PFinvalidFd(fd))
	{
		PFerrno = PFE_FD;
		return (PFerrno);
	}

	if (*pagenum < -1 || *pagenum >= PFftab[fd].hdr.numpages)
	{
		PFerrno = PFE_INVALIDPAGE;
		return (PFerrno);
	}

	/* scan the file until a valid used page is found */
	for (temppage = *pagenum + 1; temppage < PFftab[fd].hdr.numpages; temppage++)
	{
		if ((error = PFbufGet(fd, temppage, &fpage, PFreadfcn,
							  PFwritefcn)) != PFE_OK)
			return (error);
		else if (fpage->nextfree == PF_PAGE_USED)
		{
			/* found a used page */
			*pagenum = temppage;
			*pagebuf = (char *)fpage->pagebuf;
			return (PFE_OK);
		}

		/* page is free, unfix it */
		if ((error = PFbufUnfix(fd, temppage, FALSE)) != PFE_OK)
			return (error);
	}

	/* No valid used page found */
	PFerrno = PFE_EOF;
	return (PFerrno);
}


int PF_GetThisPage(int fd, int pagenum, char **pagebuf)
{
	int error;
	PFfpage *fpage;

	if (PFinvalidFd(fd))
	{
		PFerrno = PFE_FD;
		return (PFerrno);
	}

	if (PFinvalidPagenum(fd, pagenum))
	{
		PFerrno = PFE_INVALIDPAGE;
		return (PFerrno);
	}

	if ((error = PFbufGet(fd, pagenum, &fpage, PFreadfcn, PFwritefcn)) != PFE_OK)
	{
		if (error == PFE_PAGEFIXED)
			*pagebuf = fpage->pagebuf;
		return (error);
	}

	if (fpage->nextfree == PF_PAGE_USED)
	{
		/* page is used*/
		*pagebuf = (char *)fpage->pagebuf;
		return (PFE_OK);
	}
	else
	{
		/* invalid page */
		if (PFbufUnfix(fd, pagenum, FALSE) != PFE_OK)
		{
			printf("internal error:PFgetThis()\n");
			exit(1);
		}
		PFerrno = PFE_INVALIDPAGE;
		return (PFerrno);
	}
}


int PF_AllocPage(int fd, int *pagenum, char **pagebuf)
{
	PFfpage *fpage; /* pointer to file page */
	int error;

	if (PFinvalidFd(fd))
	{
		PFerrno = PFE_FD;
		return (PFerrno);
	}

	if (PFftab[fd].hdr.firstfree != PF_PAGE_LIST_END)
	{
		/* get a page from the free list */
		*pagenum = PFftab[fd].hdr.firstfree;
		if ((error = PFbufGet(fd, *pagenum, &fpage, PFreadfcn,
							  PFwritefcn)) != PFE_OK)
			/* can't get the page */
			return (error);
		PFftab[fd].hdr.firstfree = fpage->nextfree;
		PFftab[fd].hdrchanged = TRUE;
	}
	else
	{
		/* Free list empty, allocate one more page from the file */
		*pagenum = PFftab[fd].hdr.numpages;
		if ((error = PFbufAlloc(fd, *pagenum, &fpage, PFwritefcn)) != PFE_OK)
			/* can't allocate a page */
			return (error);

		/* increment # of pages for this file */
		PFftab[fd].hdr.numpages++;
		PFftab[fd].hdrchanged = TRUE;

		/* mark this page dirty */
		if ((error = PFbufUsed(fd, *pagenum)) != PFE_OK)
		{
			printf("internal error: PFalloc()\n");
			exit(1);
		}
	}

	/* Mark the new page used */
	fpage->nextfree = PF_PAGE_USED;

	/* set return value */
	*pagebuf = fpage->pagebuf;

	return (PFE_OK);
}


int PF_DisposePage(int fd, int pagenum)
{
	PFfpage *fpage; /* pointer to file page */
	int error;

	if (PFinvalidFd(fd))
	{
		PFerrno = PFE_FD;
		return (PFerrno);
	}

	if (PFinvalidPagenum(fd, pagenum))
	{
		PFerrno = PFE_INVALIDPAGE;
		return (PFerrno);
	}

	if ((error = PFbufGet(fd, pagenum, &fpage, PFreadfcn, PFwritefcn)) != PFE_OK)
		/* can't get this page */
		return (error);

	if (fpage->nextfree != PF_PAGE_USED)
	{
		/* this page already freed */
		if (PFbufUnfix(fd, pagenum, FALSE) != PFE_OK)
		{
			printf("internal error: PFdispose()\n");
			exit(1);
		}
		PFerrno = PFE_PAGEFREE;
		return (PFerrno);
	}

	/* put this page into the free list */
	fpage->nextfree = PFftab[fd].hdr.firstfree;
	PFftab[fd].hdr.firstfree = pagenum;
	PFftab[fd].hdrchanged = TRUE;

	/* unfix this page */
	return (PFbufUnfix(fd, pagenum, TRUE));
}


int PF_UnfixPage(int fd, int pagenum, int dirty)
{
	if (PFinvalidFd(fd))
	{
		PFerrno = PFE_FD;
		return (PFerrno);
	}

	if (PFinvalidPagenum(fd, pagenum))
	{
		PFerrno = PFE_INVALIDPAGE;
		return (PFerrno);
	}

	return (PFbufUnfix(fd, pagenum, dirty));
}

/* error messages */
static char *PFerrormsg[] = {
	"No error",
	"No memory",
	"No buffer space",
	"Page already fixed in buffer",
	"page to be unfixed is not in the buffer",
	"unix error",
	"incomplete read of page from file",
	"incomplete write of page to file",
	"incomplete read of header from file",
	"incomplete write of header from file",
	"invalid page number",
	"file already open",
	"file table full",
	"invalid file descriptor",
	"end of file",
	"page already free",
	"page already unfixed",
	"new page to be allocated already in buffer",
	"hash table entry not found",
	"page already in hash table"};


void PF_PrintError(const char *s)
{
	fprintf(stderr, "%s", s);
	fprintf(stderr, ":%s", PFerrormsg[-1 * PFerrno]);
	if (PFerrno == PFE_UNIX)
		/* print the unix error message */
		perror(" ");
	else
		fprintf(stderr, "\n");
}