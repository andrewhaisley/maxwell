/*
 *  This file is part of the Maxwell Word Processor application.
 *  Copyright (C) 1996, 1997, 1998 Andrew Haisley, David Miller, Tom Newton
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program; if not, write to the Free Software
 *  Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
 */
/*
 * MODULE : rascache.C
 *
 * AUTHOR : David Miller
 *
 * This file
 *
 * DESCRIPTION:
 * Module rascache.C
 *
 *
 *
 *
 * $id$
 * $log$
 *
 */

#include "raster.h"

/*-------------------------------------------------
 * FUNCTION: rascache::newFreeList
 *
 * DESCRIPTION:
 *
 *
 */

void rascache::newFreeList(int& err)
{
    int iel;

    err = MX_ERROR_OK;

    freeList = new rasItem[initialSize];

    // Set the free list of elements
    for (iel = 0; iel < initialSize; iel++)
        freeList[iel].next = freeList + iel + 1;

    // Set last element of free list to point to NULL next ;
    freeList[initialSize - 1].next = NULL;
}

/*-------------------------------------------------
 * FUNCTION: rascache::rascache
 *
 * DESCRIPTION: Constructor. Create with a size of the
 * number of elements to put in the free list. The
 * default global cache is created with 100 elements.
 */

rascache::rascache(int32 size)
{
    // Allocate free list
    error = MX_ERROR_OK;
    initialSize = size;

    newFreeList(error);
    MX_ERROR_CHECK(error);

    head = NULL;
    tail = NULL;

    return;
abort:
    return;
}

/*-------------------------------------------------
 * FUNCTION: rascache::splice
 *
 * DESCRIPTION:
 *
 *
 */

inline void rascache::splice(rasItem* thisRasItem)
{
    // Splice out node from its current postion
    // Get the existing previous element pointing
    // to the next

    if (thisRasItem == head) {
        head = thisRasItem->next;
        if (head != NULL)
            head->prev = NULL;
    } else {
        thisRasItem->prev->next = thisRasItem->next;

        if (thisRasItem == tail) {
            // If it is the tail (thisRasItem->next == NULL) - reset the tail
            tail = thisRasItem->prev;
        } else {
            thisRasItem->next->prev = thisRasItem->prev;
        }
    }
}

/*-------------------------------------------------
 * FUNCTION: rascache::move
 *
 * DESCRIPTION: Move a rascache item to the head of
 * the list
 *
 */

void rascache::move(rasItem* thisRasItem)
{
    // If it is the head return now
    if (thisRasItem == head)
        return;

    splice(thisRasItem);

    // Add to head

    head->prev = thisRasItem;
    thisRasItem->next = head;
    thisRasItem->prev = NULL;
    head = thisRasItem;
}

/*-------------------------------------------------
 * FUNCTION: rascache::getFromFreeList
 *
 * DESCRIPTION: Get the next element from the free list
 * The freelist cannot be NULL when this is called.
 *
 */

inline void rascache::getFromFreeList(rasItem** thisRasItem)
{
    *thisRasItem = freeList;
    freeList = freeList->next;
}

/*-------------------------------------------------
 * FUNCTION: rascache::returnToFreeList
 *
 * DESCRIPTION: Return an item to the free list
 *
 *
 */

inline void rascache::returnToFreeList(rasItem* thisRasItem)
{
    splice(thisRasItem);

    thisRasItem->next = freeList;
    freeList = thisRasItem;
}

/*-------------------------------------------------
 * FUNCTION: rascache::add
 *
 * DESCRIPTION: Add a new element to the free list
 *
 *
 */

void rascache::add(int& err, raster* thisRaster, int32 thisTileIndex,
    unsigned char* buffer, rasItem** index)
{
    bool found;
    rasItem* test;

    // The item is not in the raster dictionary
    // and hence is not in the global linked list

    found = FALSE;

    if (freeList == NULL) {
        // The freelist is all used up

        // Remove the first non-locked element from the raster tree

        test = tail;
        found = FALSE;

        while (test != NULL) {
            if (!RISLOCKED(test)) {
                test->iraster->removeTile(err, tail->ibuffer, tail->itileIndex,
                    RISEDITED(test));
                MX_ERROR_CHECK(err);

                // Reset the tail parameters and then move to head
                test->iraster = thisRaster;
                test->itileIndex = thisTileIndex;
                test->ibuffer = buffer;
                test->flag = 0;

                // And move to head
                move(test);
                found = TRUE;
                break;
            }
            test = test->prev;
        }

        if (!found) {
            // all the elements are locked
            // Need to extend the free list
            newFreeList(err);
            MX_ERROR_CHECK(err);
        }
    }

    // Get item from free list
    if (!found) {
        rasItem* newRasItem;

        // Get the next  free element
        getFromFreeList(&newRasItem);

        newRasItem->iraster = thisRaster;
        newRasItem->itileIndex = thisTileIndex;
        newRasItem->ibuffer = buffer;
        newRasItem->flag = 0;

        // Add to head
        newRasItem->prev = NULL;
        newRasItem->next = head;
        if (head != NULL)
            head->prev = newRasItem;

        head = newRasItem;
        if (tail == NULL)
            tail = newRasItem;
    }

    // Return the element in the free list to the caller
    *index = head;

    return;
abort:
    *index = NULL;
    return;
}

/*-------------------------------------------------
 * FUNCTION: rascache::del
 *
 * DESCRIPTION: Delete all items connect with a
 * raster from the cache
 *
 */

void rascache::del(int& err, raster* thisRaster)
{
    rasItem *nextItem, *thisRasItem;

    thisRasItem = head;

    while (thisRasItem != NULL) {
        nextItem = thisRasItem->next;
        if (thisRasItem->iraster == thisRaster) {
            // Remove the element from the raster tree
            thisRaster->removeTile(err, thisRasItem->ibuffer,
                thisRasItem->itileIndex,
                RISEDITED(thisRasItem));
            MX_ERROR_CHECK(err);

            returnToFreeList(thisRasItem);
        }
        thisRasItem = nextItem;
    }

    return;
abort:
    return;
}
