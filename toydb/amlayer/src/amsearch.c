/**
 * @file amsearch.c
 * @brief Functions for searching in the B+ tree.
 */

#include <stdio.h>
#include "../include/am.h"
#include "../../pflayer/include/pf.h"

int AM_Search(int fileDesc, char attrType, int attrLength, char *value, int *pageNum, char **pageBuf, int *indexPtr)
{
	int errVal;
	int nextPage;				   /* next page to be followed on the path from root to leaf*/
	AM_LEAFHEADER lhead, *lheader; /* local pointer to leaf header */
	AM_INTHEADER ihead, *iheader;  /* local pointer to internal node header */

	/* initialise the headers */
	lheader = &lhead;
	iheader = &ihead;

	/* get the root of the B+ tree */
	errVal = PF_GetFirstPage(fileDesc, pageNum, pageBuf);
	AM_Check;

	if (**pageBuf == 'l')
	{
		/* if root is a leaf page */
		AM_bcopy(*pageBuf, (char *)lheader, AM_sl);
		if (lheader->attrLength != attrLength)
			return (AME_INVALIDATTRLENGTH);
	}
	else /* root is not a leaf */
	{
		AM_bcopy(*pageBuf, (char *)iheader, AM_sint);
		if (iheader->attrLength != attrLength)
			return (AME_INVALIDATTRLENGTH);
	}

	/* find the leaf at which key is present or can be inserted */
	while ((**pageBuf) != 'l')
	{
		/* find the next page to be followed */
		nextPage = AM_BinSearch(*pageBuf, attrType, attrLength, value, indexPtr, iheader);

		/* push onto stack for backtracking and splitting nodes if needed later */
		AM_PushStack(*pageNum, *indexPtr);

		errVal = PF_UnfixPage(fileDesc, *pageNum, FALSE);
		AM_Check;

		/* set pageNum to the next page to be followed */
		*pageNum = nextPage;

		/* Get the next page to be followed */
		errVal = PF_GetThisPage(fileDesc, *pageNum, pageBuf);
		AM_Check;

		if (**pageBuf == 'l')
		{
			/* if next page is a leaf */
			AM_bcopy(*pageBuf, (char *)lheader, AM_sl);
			if (lheader->attrLength != attrLength)
				return (AME_INVALIDATTRLENGTH);
		}
		else
		{
			/* if next page is an internal node */
			AM_bcopy(*pageBuf, (char *)iheader, AM_sint);
			if (iheader->attrLength != attrLength)
				return (AME_INVALIDATTRLENGTH);
		}
	}
	/* find whether key is in leaf or not */
	return (AM_SearchLeaf(*pageBuf, attrType, attrLength, value, indexPtr, lheader));
}

int AM_BinSearch(char *pageBuf, char attrType, int attrLength, char *value, int *indexPtr, AM_INTHEADER *header)
{
	int low, high, mid; /* for binary search */
	int compareVal;		/* result of comparison of key with value */
	int recSize;		/* size in bytes of a key,ptr pair */
	int pageNum;		/* page number of node to be followed along the B+ tree */

	recSize = AM_si + attrLength;
	low = 1;
	high = header->numKeys;

	while ((high - low) > 1)
	{
		/* get the middle key */
		mid = (low + high) / 2;

		/* compare the value with the middle key */
		compareVal = AM_Compare(pageBuf + AM_sint + AM_si + (mid - 1) * recSize, attrType, attrLength, value);

		if (compareVal < 0)
			high = mid - 1;
		else if (compareVal > 0)
			low = mid + 1;
		else
		{
			/* value = middle key */
			AM_bcopy(pageBuf + AM_sint + mid * recSize, (char *)&pageNum, AM_si);
			*indexPtr = mid;
			return (pageNum);
		}
	}

	/* check the border cases */
	if ((high - low) == 0)
	{
		if (AM_Compare(pageBuf + AM_sint + AM_si + (low - 1) * recSize, attrType, attrLength, value) < 0)
		{
			AM_bcopy(pageBuf + AM_sint + (low - 1) * recSize, (char *)&pageNum, AM_si);
			*indexPtr = low - 1;
			return (pageNum);
		}
		else
		{
			AM_bcopy(pageBuf + AM_sint + low * recSize, (char *)&pageNum, AM_si);
			*indexPtr = low;
			return (pageNum);
		}
	}

	if ((high - low) == 1)
	{
		if (AM_Compare(pageBuf + AM_sint + AM_si + (low - 1) * recSize, attrType, attrLength, value) < 0)
		{
			AM_bcopy(pageBuf + AM_sint + (low - 1) * recSize, (char *)&pageNum, AM_si);
			*indexPtr = low - 1;
			return (pageNum);
		}
		else
		{
			if (AM_Compare(pageBuf + AM_sint + AM_si + low * recSize, attrType, attrLength, value) < 0)
			{
				AM_bcopy(pageBuf + AM_sint + low * recSize, (char *)&pageNum, AM_si);
				*indexPtr = low;
				return (pageNum);
			}
			else
			{
				AM_bcopy(pageBuf + AM_sint + (low + 1) * recSize, (char *)&pageNum, AM_si);
				*indexPtr = low + 1;
				return (pageNum);
			}
		}
	}
	return 0; /* Should not happen */
}

int AM_SearchLeaf(char *pageBuf, char attrType, int attrLength, char *value, int *indexPtr, AM_LEAFHEADER *header)
{
	int low, high, mid; /* for binary search */
	int compareVal;		/* result of comparison of key with value */
	int recSize;		/* size in bytes of a key,ptr pair */

	recSize = AM_ss + attrLength;
	low = 1;
	high = header->numKeys;

	/* The leaf is Empty */
	if (high == 0)
	{
		*indexPtr = 1;
		return (AM_NOT_FOUND);
	}

	while ((high - low) > 1)
	{
		/* get the middle key */
		mid = (low + high) / 2;

		/* compare value with the middle key */
		compareVal = AM_Compare(pageBuf + AM_sl + (mid - 1) * recSize, attrType, attrLength, value);

		if (compareVal < 0)
			high = mid - 1;
		else if (compareVal > 0)
			low = mid + 1;
		else
		{
			*indexPtr = mid;
			return (AM_FOUND);
		}
	}

	if ((high - low) == 0)
	{
		compareVal = AM_Compare(pageBuf + AM_sl + (low - 1) * recSize, attrType, attrLength, value);

		if (compareVal < 0)
		{
			*indexPtr = low;
			return (AM_NOT_FOUND);
		}
		else if (compareVal > 0)
		{
			*indexPtr = low + 1;
			return (AM_NOT_FOUND);
		}
		else
		{
			*indexPtr = low;
			return (AM_FOUND);
		}
	}

	if ((high - low) == 1)
	{
		compareVal = AM_Compare(pageBuf + AM_sl + (low - 1) * recSize, attrType, attrLength, value);

		if (compareVal < 0)
		{
			*indexPtr = low;
			return (AM_NOT_FOUND);
		}
		else if (compareVal == 0)
		{
			*indexPtr = low;
			return (AM_FOUND);
		}
		else
		{
			compareVal = AM_Compare(pageBuf + AM_sl + low * recSize, attrType, attrLength, value);

			if (compareVal < 0)
			{
				*indexPtr = low + 1;
				return (AM_NOT_FOUND);
			}
			else if (compareVal > 0)
			{
				*indexPtr = low + 2;
				return (AM_NOT_FOUND);
			}
			else
			{
				*indexPtr = low + 1;
				return (AM_FOUND);
			}
		}
	}
	return 0; /* Should not happen */
}

int AM_Compare(char *bufPtr, char attrType, int attrLength, char *valPtr)
{
	int bufint, valint;		  /* temporary aligned storage for comparison */
	float buffloat, valfloat; /* temporary aligned storage for comparison */

	switch (attrType)
	{
	case 'i':
	{
		AM_bcopy(bufPtr, (char *)&bufint, AM_si);
		AM_bcopy(valPtr, (char *)&valint, AM_si);
		if (valint < bufint)
			return (-1);
		else if (valint > bufint)
			return (1);
		else
			return (0);
	}
	case 'f':
	{
		AM_bcopy(bufPtr, (char *)&buffloat, AM_sf);
		AM_bcopy(valPtr, (char *)&valfloat, AM_sf);
		if (valfloat < buffloat)
			return (-1);
		else if (valfloat > buffloat)
			return (1);
		else
			return (0);
	}
	case 'c':
	{
		return (strncmp(valPtr, bufPtr, attrLength));
	}
	}
	return 0; /* Should not happen */
}