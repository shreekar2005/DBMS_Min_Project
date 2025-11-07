/**
 * @file amstack.c
 * @brief Implementation of a simple stack for B+ tree traversal.
 */

#include <stdio.h>
#include "am.h"
#include "../../pflayer/include/pf.h"

#define AM_MAXSTACK 50

static struct
{
    int pageNumber;
    int offset;
} AM_Stack[AM_MAXSTACK];

static int AM_topofStackPtr = -1;

/**
 * @brief Pushes a page number and offset onto the stack.
 * @param pageNum The page number to push.
 * @param offset The offset to push.
 */
void AM_PushStack(int pageNum, int offset)
{
    AM_topofStackPtr++;
    AM_Stack[AM_topofStackPtr].pageNumber = pageNum;
    AM_Stack[AM_topofStackPtr].offset = offset;
}

/**
 * @brief Pops the top element from the stack.
 */
void AM_PopStack(void)
{
    AM_topofStackPtr--;
}

/**
 * @brief Gets the top element of the stack without popping it.
 * @param pageNum Output: The page number from the top of the stack.
 * @param offset Output: The offset from the top of the stack.
 */
void AM_topofStack(int *pageNum, int *offset)
{
    *pageNum = AM_Stack[AM_topofStackPtr].pageNumber;
    *offset = AM_Stack[AM_topofStackPtr].offset;
}

/**
 * @brief Empties the stack.
 */
void AM_EmptyStack(void)
{
    AM_topofStackPtr = -1;
}