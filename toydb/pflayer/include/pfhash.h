#ifndef PFHASH_H
#define PFHASH_H

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
 * @brief Initialize the hash table entries. Must be called before any of the other hash functions are used.
 */
void PFhashInit(void);

/**
 * @brief Given the file descriptor "fd", and page number "page", find the buffer address of this particular page.
 * @param fd File descriptor.
 * @param page Page number.
 * @return Buffer address if found, NULL otherwise.
 */
PFbpage *PFhashFind(int fd, int page);

/**
 * @brief Insert the file descriptor "fd", page number "page", and the buffer address "bpage" into the hash table.
 * @param fd File descriptor.
 * @param page Page number.
 * @param bpage Buffer address for this page.
 * @return PFE_OK if OK, PFE_NOMEM if no memory, PFE_HASHPAGEEXIST if the page already exists.
 */
int PFhashInsert(int fd, int page, PFbpage *bpage);

/**
 * @brief Delete the entry whose file descriptor is "fd", and whose page number is "page" from the hash table.
 * @param fd File descriptor.
 * @param page Page number.
 * @return PFE_OK if OK, PFE_HASHNOTFOUND if can't find the entry.
 */
int PFhashDelete(int fd, int page);

/**
 * @brief Print the hash table entries.
 */
void PFhashPrint(void);

#endif /*PFHASH_H*/