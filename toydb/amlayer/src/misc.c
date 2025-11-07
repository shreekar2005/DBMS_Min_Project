/**
 * @file misc.c
 * @brief Miscellaneous utility and wrapper functions for testing the AM layer.
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "../../pflayer/include/pf.h"
#include "testam.h"
#include "am.h"

void padstring(char *str, int length)
{
	int i;

	for (i = strlen(str); i < length; i++)
		str[i] = '\0';
}

int xAM_CreateIndex(char *fname, int indexno, char attrtype, int attrlen)
{
	int errval;

	if ((errval = AM_CreateIndex(fname, indexno, attrtype, attrlen)) != AME_OK)
	{
		printf("AM_CreateIndex(%s,%d,'%c',%d) failed: %d\n",
			   fname, indexno, attrtype, attrlen, errval);
        if (errval == AME_PF) {
            PF_PrintError("PF Error");
        }
		exit(1);
	}
	return (errval);
}

int xAM_DestroyIndex(char *fname, int indexno)
{
	int errval;

	if ((errval = AM_DestroyIndex(fname, indexno)) != AME_OK)
	{
		printf("AM_DestroyIndex(%s,%d) failed: %d\n", fname, indexno, errval);
		exit(1);
	}
	return (errval);
}

int xAM_InsertEntry(int fd, char attrtype, int attrlen, char *val, RecIdType recid)
{
	int errval;

	if ((errval = AM_InsertEntry(fd, attrtype, attrlen, val, recid)) != AME_OK)
	{
		printf("AM_InsertEntry(%d,'%c',%d,val,%d) failed: %d\n",
			   fd, attrtype, attrlen, RecIdToInt(recid), errval);
		exit(1);
	}
	return (errval);
}

int xAM_DeleteEntry(int fd, char attrtype, int attrlen, char *val, RecIdType recid)
{
	int errval;

	if ((errval = AM_DeleteEntry(fd, attrtype, attrlen, val, recid)) != AME_OK)
	{
		printf("AM_DeleteEntry(%d,'%c',%d,val,%d) failed: %d\n",
			   fd, attrtype, attrlen, RecIdToInt(recid), errval);
		exit(1);
	}
	return (errval);
}

int xAM_OpenIndexScan(int fd, char attrtype, int attrlen, int op, char *val)
{
	int sd;

	if ((sd = AM_OpenIndexScan(fd, attrtype, attrlen, op, val)) < 0)
	{
		printf("AM_OpenIndexScan(%d,'%c',%d,%d,rec) failed: %d\n",
			   fd, attrtype, attrlen, op, sd);
		exit(1);
	}
	return (sd);
}

RecIdType xAM_FindNextEntry(int sd)
{
	int errval;
	RecIdType recid;

	recid = AM_FindNextEntry(sd);
	if ((errval = RecIdToInt(recid)) < AME_OK && errval != AME_EOF)
	{
		printf("AM_FindNextEntry(%d) failed: %d\n", sd, errval);
		exit(1);
	}
	return (recid);
}

int xAM_CloseIndexScan(int sd)
{
	int errval;

	if ((errval = AM_CloseIndexScan(sd)) != AME_OK)
	{
		printf("AM_CloseIndexScan(%d) failed:%d\n", sd, errval);
		exit(1);
	}
	return (errval);
}

int xPF_OpenFile(char *fname)
{
	int errval;

	if ((errval = PF_OpenFile(fname)) < 0)
	{
		PF_PrintError("PF_OpenFile failed");
		exit(1);
	}
	return (errval);
}

int xPF_CloseFile(int fd)
{
	int errval;

	if ((errval = PF_CloseFile(fd)) != PFE_OK)
	{
		PF_PrintError("PF_CloseFile failed");
		exit(1);
	}
	return (errval);
}