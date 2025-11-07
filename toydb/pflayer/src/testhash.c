/**
 * @file testhash.c
 * @brief Tests the hash table functions.
 */

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include "pf.h"
#include "pftypes.h"

/**
 * @brief Main test function for the hash table.
 */
int main(void)
{
	int i;
	long j;
	PFbpage *bpage;

	PFhashInit();
	/* insert a few entries */
	for (i = 1; i < 11; i++)
		for (j = 1; j < 11; j++)
		{
			if (PFhashInsert(i, j, (PFbpage *)(intptr_t)(i + j)) != PFE_OK)
			{
				printf("PFhashInsert failed\n");
				exit(1);
			}
		}

	/* Now, find all the entries */
	for (i = 1; i < 11; i++)
		for (j = 1; j < 11; j++)
		{
			bpage = PFhashFind(i, j);
			if (bpage == NULL)
			{
				printf("PFhashFind failed at %d %ld\n", i, j);
				exit(1);
			}
			else if ((intptr_t)bpage != (i + j))
			{
				printf("PFhashFind returned wrong value at %d %ld\n", i, j);
				exit(1);
			}
			else
			{
				printf("found %d, %ld\n", i, j);
			}
		}

	/* Now, delete them in reverse */
	for (j = 10; j > 0; j--)
		for (i = 10; i > 0; i--)
			if (PFhashDelete(i, j) != PFE_OK)
			{
				printf("PFhashDelete failed at %d %ld", i, j);
				exit(1);
			}

	/* print the hash table out */
	PFhashPrint();

	return 0;
}