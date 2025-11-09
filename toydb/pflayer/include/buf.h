#include "../include/pftypes.h"

#define STRATEGY_LRU 0 /**LRU for Buffer manager */
#define STRATEGY_MRU 1 /**MRU for Buffer manager */
#define DEFAULT_MAX_PAGE 20 /**Number of max page in one buffer */

extern int PFmaxbpage;

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
int PFbufGet(int fd, int pagenum, PFfpage **fpage, int (*readfcn)(int, int, PFfpage*), int (*writefcn)(int, int, PFfpage*));

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
int PFbufUnfix(int fd, int pagenum, int dirty);

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
int PFbufAlloc(int fd, int pagenum, PFfpage **fpage, int (*writefcn)(int, int, PFfpage*));

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
int PFbufReleaseFile(int fd, int (*writefcn)(int, int, PFfpage*));

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
int PFbufUsed(int fd, int pagenum);

/**
 * @brief Print the current page buffers.
 */
void PFbufPrint(void);

/**
 * @brief Set the maximum number of buffer pages.
 * @param num_bufs The maximum number of buffer pages.
 */
void PFbufSetNumPages(int num_bufs);

/**
 * @brief Set the page replacement strategy.
 * @param strategy The strategy (STRATEGY_LRU or STRATEGY_MRU).
 */
void PFbufSetStrategy(int strategy);
