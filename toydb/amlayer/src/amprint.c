/**
 * @file amprint.c
 * @brief Functions for printing B+ tree nodes for debugging.
 */

#include <stdio.h>
#include <stdlib.h>
#include "../include/am.h"
#include "../../pflayer/include/pf.h"

void AM_PrintIntNode(char *pageBuf, char attrType)
{
	int tempPageint;
	int i;
	int recSize;
	AM_INTHEADER *header;

	header = (AM_INTHEADER *)calloc(1, AM_sint);
	AM_bcopy(pageBuf, (char *)header, AM_sint);
	recSize = header->attrLength + AM_si;
	printf("PAGETYPE %c\n", header->pageType);
	printf("NUMKEYS %d\n", header->numKeys);
	printf("MAXKEYS %d\n", header->maxKeys);
	printf("ATTRLENGTH %d\n", header->attrLength);
	AM_bcopy(pageBuf + AM_sint, (char *)&tempPageint, AM_si);
	printf("FIRSTPAGE is %d\n", tempPageint);
	for (i = 1; i <= (header->numKeys); i++)
	{
		AM_PrintAttr(pageBuf + (i - 1) * recSize + AM_sint + AM_si, attrType, header->attrLength);
		AM_bcopy(pageBuf + i * recSize + AM_sint, (char *)&tempPageint, AM_si);
		printf("NEXTPAGE is %d\n", tempPageint);
	}
	free(header);
}

void AM_PrintLeafNode(char *pageBuf, char attrType)
{
	short nextRec;
	int i;
	int recSize;
	int recId;
	int offset1;
	AM_LEAFHEADER *header;

	header = (AM_LEAFHEADER *)calloc(1, AM_sl);
	AM_bcopy(pageBuf, (char *)header, AM_sl);
	recSize = header->attrLength + AM_ss;
	printf("PAGETYPE %c\n", header->pageType);
	printf("NEXTLEAFPAGE %d\n", header->nextLeafPage);
	printf("NUMKEYS %d\n", header->numKeys);
	for (i = 1; i <= header->numKeys; i++)
	{
		offset1 = (i - 1) * recSize + AM_sl;
		AM_PrintAttr(pageBuf + AM_sl + (i - 1) * recSize, attrType, header->attrLength);
		AM_bcopy(pageBuf + offset1 + header->attrLength, (char *)&nextRec, AM_ss);
		while (nextRec != 0)
		{
			AM_bcopy(pageBuf + nextRec, (char *)&recId, AM_si);
			printf("RECID is %d\n", recId);
			AM_bcopy(pageBuf + nextRec + AM_si, (char *)&nextRec, AM_ss);
		}
		printf("\n\n");
	}
	free(header);
}

void AM_DumpLeafPages(int fileDesc, int min, char attrType, int attrLength)
{
	int pageNum;
	char *pageBuf;
	int errVal;
	AM_LEAFHEADER *header;

	printf("%d PAGE \n", AM_LeftPageNum);
	PF_GetThisPage(fileDesc, AM_LeftPageNum, &pageBuf);
	header = (AM_LEAFHEADER *)calloc(1, AM_sl);
	AM_bcopy(pageBuf, (char *)header, AM_sl);
	pageNum = AM_LeftPageNum;
	while (header->nextLeafPage != -1)
	{
		printf("PAGENUMBER = %d\n", pageNum);
		AM_PrintLeafKeys(pageBuf, attrType);
		errVal = PF_UnfixPage(fileDesc, pageNum, FALSE);
		AM_Check_Void;
		pageNum = header->nextLeafPage;
		errVal = PF_GetThisPage(fileDesc, pageNum, &pageBuf);
		AM_Check_Void;
		AM_bcopy(pageBuf, (char *)header, AM_sl);
	}
	printf("PAGENUMBER = %d\n", pageNum);
	AM_PrintLeafKeys(pageBuf, attrType);
	errVal = PF_UnfixPage(fileDesc, pageNum, FALSE);
	AM_Check_Void;
	free(header);
}

void AM_PrintLeafKeys(char *pageBuf, char attrType)
{
	short nextRec;
	int i;
	int recSize;
	int recId;
	int offset1;
	AM_LEAFHEADER *header;

	header = (AM_LEAFHEADER *)calloc(1, AM_sl);
	AM_bcopy(pageBuf, (char *)header, AM_sl);
	recSize = header->attrLength + AM_ss;
	for (i = 1; i <= header->numKeys; i++)
	{
		offset1 = (i - 1) * recSize + AM_sl;
		AM_PrintAttr(pageBuf + AM_sl + (i - 1) * recSize, attrType, header->attrLength);
		AM_bcopy(pageBuf + offset1 + header->attrLength, (char *)&nextRec, AM_ss);
		while (nextRec != 0)
		{
			AM_bcopy(pageBuf + nextRec, (char *)&recId, AM_si);
			printf("RECID is %d\n", recId);
			AM_bcopy(pageBuf + nextRec + AM_si, (char *)&nextRec, AM_ss);
		}
	}
	free(header);
}

void AM_PrintAttr(char *bufPtr, char attrType, int attrLength)
{
	int bufint;
	float buffloat;
	char *bufstr;

	switch (attrType)
	{
	case 'i':
	{
		AM_bcopy(bufPtr, (char *)&bufint, AM_si);
		printf("ATTRIBUTE is %d\n", bufint);
		break;
	}
	case 'f':
	{
		AM_bcopy(bufPtr, (char *)&buffloat, AM_sf);
		printf("ATTRIBUTE is %f\n", buffloat);
		break;
	}
	case 'c':
	{
		bufstr = malloc((unsigned)(attrLength + 1));
		AM_bcopy(bufPtr, bufstr, attrLength);
		bufstr[attrLength] = '\0';
		printf("ATTRIBUTE is %s\n", bufstr);
		free(bufstr);
		break;
	}
	}
}

void AM_PrintTree(int fileDesc, int pageNum, char attrType)
{
	int nextPage;
	int errVal;
	AM_INTHEADER *header;
	char *tempPage;
	char *pageBuf;
	int recSize;
	int i;

	printf("GETTING PAGE = %d\n", pageNum);
	errVal = PF_GetThisPage(fileDesc, pageNum, &pageBuf);
	AM_Check_Void;
	tempPage = malloc(PF_PAGE_SIZE);
	AM_bcopy(pageBuf, tempPage, PF_PAGE_SIZE);
	errVal = PF_UnfixPage(fileDesc, pageNum, FALSE);
	AM_Check_Void;
	if (*tempPage == 'l')
	{
		printf("PAGENUM = %d\n", pageNum);
		AM_PrintLeafKeys(tempPage, attrType);
		free(tempPage);
		return;
	}
	header = (AM_INTHEADER *)calloc(1, AM_sint);
	AM_bcopy(tempPage, (char *)header, AM_sint);
	recSize = header->attrLength + AM_si;
	for (i = 1; i <= (header->numKeys + 1); i++)
	{
		AM_bcopy(tempPage + AM_sint + (i - 1) * recSize, (char *)&nextPage, AM_si);
		AM_PrintTree(fileDesc, nextPage, attrType);
	}
	printf("PAGENUM = %d", pageNum);
	AM_PrintIntNode(tempPage, attrType);
	free(header);
	free(tempPage);
}