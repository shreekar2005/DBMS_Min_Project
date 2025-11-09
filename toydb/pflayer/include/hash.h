#include "../include/pftypes.h"

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


void PFhashInit(void);
PFbpage *PFhashFind(int fd, int page);
int PFhashInsert(int fd, int page, PFbpage *bpage);
int PFhashDelete(int fd, int page);
void PFhashPrint(void);