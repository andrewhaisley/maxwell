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
 * MODULE/CLASS : mx_list
 *
 * AUTHOR : Tom Newton
 *
 *
 *
 *
 *
 */

#include "mx_list.h"
#include <mx.h>
#include <mx_error.h>

/* These provide a good way of checking whether the previous element stuff is
 * working correctly. Insert the macro at various places in the code, and put a
 * breakpoint on the break_func function in the debugger, so you can see where
 * the duff previous element is being generated. Should not be needed from now
 * on, but you can never tell

static void break_func()
{
    // do nothing but provide a breaking place
}

#define CHECK_PREV_ELEM                         \
{                                               \
    if (prev_list_elem != NULL)                 \
    {                                           \
        bool found_prev_elem = FALSE;           \
        elem_t *elem = first_list_elem;         \
                                                \
        while (elem != NULL)                    \
        {                                       \
            if (elem == prev_list_elem)         \
            {                                   \
                found_prev_elem = TRUE;         \
            }                                   \
            elem = elem->next;                  \
        }                                       \
                                                \
        if (!found_prev_elem)                   \
        {                                       \
            break_func();                       \
        }                                       \
    }                                           \
} */

static mx_list::elem_t* mx_list_free_node_list = NULL;
static int mx_list_num_free_nodes = 0;

void* mx_list::elem_t::operator new(size_t s)
{
    if (mx_list_free_node_list) {
        elem_t* result = mx_list_free_node_list;
        mx_list_free_node_list = result->next;
        mx_list_num_free_nodes--;
        return result;
    }
    return ::operator new(s);
}

void mx_list::elem_t::operator delete(void* node)
{
    elem_t* n = (elem_t*)node;
    n->next = mx_list_free_node_list;
    mx_list_free_node_list = n;
    mx_list_num_free_nodes++;
}

//////////////////////////// Constructor
mx_list::mx_list()
{
    num_items = current_index = 0;
    first_list_elem = prev_list_elem = current_list_elem = last_list_elem = NULL;
}

//////////////////////////// Destructor
mx_list::~mx_list()
{
    delete_list(first_list_elem);
}

//////////////////////////// Copy Constructor
mx_list::mx_list(const mx_list& list)
{
    first_list_elem = NULL;

    this->equate(list);
}

//////////////////////////// equals operator

mx_list& mx_list::equate(const mx_list& rvalue)
{
    delete_list(first_list_elem);

    num_items = current_index = 0;
    first_list_elem = prev_list_elem = current_list_elem = last_list_elem = NULL;

    first_list_elem = copy_list(rvalue.first_list_elem, (mx_list&)rvalue);
    return *this;
}

// inserts an item before the item specified in the index.
void mx_list::insert(int& err, int32 index, void* item)
{
    elem_t* rest_list = first_list_elem;
    elem_t* new_list_elem;

    if ((index == -1) || (index == num_items)) {
        append(item);
    } else {
        new_list_elem = new elem_t;

        range_check(err, index);
        MX_ERROR_CHECK(err);

        if (index == 0) {
            first_list_elem = new_list_elem;
            current_index++;
        } else {
            goto_index(err, index - 1);
            MX_ERROR_CHECK(err);

            rest_list = current_list_elem->next;
            current_list_elem->next = new_list_elem;
        }

        new_list_elem->item = item;
        new_list_elem->next = rest_list;
        num_items++;
    }
abort:
    return;
}

void mx_list::append(void* item)
{
    elem_t* new_list_elem;

    new_list_elem = new elem_t;

    if (current_index == num_items) {
        prev_list_elem = current_list_elem;
        current_list_elem = new_list_elem;
    }

    if (num_items == 0) {
        first_list_elem = new_list_elem;
    } else {
        last_list_elem->next = new_list_elem;
    }

    new_list_elem->item = item;
    new_list_elem->next = NULL;
    last_list_elem = new_list_elem;
    num_items++;
}

void* mx_list::remove(int& err, int32 index)
{
    elem_t *item_to_delete = first_list_elem,
           *rest,
           *tail = first_list_elem;
    void* removed_item;

    range_check(err, index);
    MX_ERROR_CHECK(err);

    if (index == 0) {
        item_to_delete = first_list_elem;
        first_list_elem = first_list_elem->next;

        if (current_index == 0) {
            current_list_elem = first_list_elem;
        } else {
            current_index--;
        }

        if (num_items == 1) {
            current_index = 0;
            first_list_elem = current_list_elem = prev_list_elem = last_list_elem = NULL;
        }
    } else {
        goto_index(err, index - 1);
        MX_ERROR_CHECK(err);

        tail = current_list_elem;
        item_to_delete = tail->next;
        rest = item_to_delete->next;
        tail->next = rest;

        if (rest == NULL) {
            last_list_elem = tail;
        }
    }

    removed_item = item_to_delete->item;
    item_to_delete->next = NULL;

    delete_list(item_to_delete);

    --num_items;
    return removed_item;
abort:
    return NULL;
}

void* mx_list::overwrite(int& err, int32 index, void* item)
{
    void* overwritten_item;

    goto_index(err, index);
    MX_ERROR_CHECK(err);

    overwritten_item = current_list_elem->item;
    current_list_elem->item = item;

    return overwritten_item;
abort:
    return NULL;
}

void* mx_list::get(int& err, int32 index) const
{
    goto_index(err, index);
    MX_ERROR_CHECK(err);

    return current_list_elem->item;
abort:
    return NULL;
}

void* mx_list::find_smallest(int& err, int (*comp_func)(void* one, void* two))
{
    int32 smallest_index = current_index = 0;
    elem_t* smallest_list_elem = current_list_elem = first_list_elem;

    range_check(err, smallest_index);
    MX_ERROR_CHECK(err);

    if (num_items > 1) {
        while (current_list_elem->next != NULL) {
            if (comp_func(smallest_list_elem->item,
                    current_list_elem->next->item)
                > 0) {
                smallest_list_elem = current_list_elem->next;
                smallest_index = current_index;
            }
            current_list_elem = current_list_elem->next;
            current_index++;
        }
    }
    current_list_elem = smallest_list_elem;
    current_index = smallest_index;
    return current_list_elem->item;
abort:
    return NULL;
}

static int (*the_comp_func)(void* one, void* two);

static inline int comp_reverse(void* one, void* two)
{
    return the_comp_func(two, one);
}

void* mx_list::find_largest(int& err, int (*comp_func)(void* one, void* two))
{
    the_comp_func = comp_func;
    return find_smallest(err, &comp_reverse);
}

void mx_list::iterate(int& err, void (*iter_func)(int& err, uint32 index, void* item))
{
    elem_t *iter_list_elem, *next_iter_list_elem;
    int32 iter_index;

    iter_index = 0;
    iter_list_elem = first_list_elem;

    while (iter_list_elem != NULL) {
        next_iter_list_elem = iter_list_elem->next;

        iter_func(err, iter_index, iter_list_elem->item);
        MX_ERROR_CHECK(err);

        iter_list_elem = next_iter_list_elem;
        iter_index++;
    }
abort:
    return;
}

// These functions can be used to iterate through inserted objects
// you call iterate_start and then repeatedly call iterate_next.
// iterate_next returns NULL when you have completed the iteration.

void mx_list::iterate_start()
{
    current_index = 0;
    current_list_elem = first_list_elem;
}

void* mx_list::iterate_next()
{
    elem_t* temp_elem = current_list_elem;

    if (current_list_elem == NULL) {
        iterate_start();
        return NULL;
    } else {
        current_list_elem = current_list_elem->next;
        current_index++;
        return temp_elem->item;
    }
}

int32 mx_list::find_index(int& err, void* item)
{
    current_index = 0;
    current_list_elem = first_list_elem;

    prev_list_elem = NULL;

    while (current_list_elem != NULL) {
        if (current_list_elem->item == item) {
            return current_index;
        }
        current_list_elem = current_list_elem->next;
        current_index++;
    }
    err = MX_LIST_NO_SUCH_POINTER;
    return 0;
}

void mx_list::goto_index(int& err, int32 index) const
{
    int32 i;

    range_check(err, index);
    MX_ERROR_CHECK(err);

    if (index == 0) {
        current_list_elem = first_list_elem;
        prev_list_elem = NULL;
        current_index = 0;
        return;
    }

    if (current_index > index) {
        if (prev_list_elem != NULL && index == current_index - 1) {
            current_list_elem = prev_list_elem;
        } else {
            current_index = 0;
            current_list_elem = first_list_elem;
        }
        prev_list_elem = NULL;
    }

    for (i = current_index; i < index; i++) {
        prev_list_elem = current_list_elem;
        current_list_elem = current_list_elem->next;
    }
    current_index = index;

abort:
    return;
}

void mx_list::range_check(int& err, int32& index) const
{
    if (num_items == 0) {
        err = MX_LIST_EMPTY;
        goto abort;
    }

    if (index == -1) {
        index = num_items - 1;
    }

    if ((index < 0) || (index >= num_items)) {
        err = MX_LIST_INDEX;
        goto abort;
    }

abort:
    return;
}

void mx_list::delete_list(elem_t* list)
{
    if (list != NULL) {
        delete_list(list->next);
        delete list;
    }
}

mx_list::elem_t* mx_list::copy_list(elem_t* list, mx_list& other)
{
    elem_t* result;

    if (list != NULL) {
        result = new elem_t;

        result->item = list->item;

        result->next = copy_list(list->next, other);

        num_items++;

        if (result->next == NULL) {
            last_list_elem = result;
        }

        if (other.current_list_elem == list) {
            current_index = other.current_index;
            current_list_elem = result;
        }
        return result;
    }
    return NULL;
}

bool mx_list::iterate_more()
{
    return current_list_elem != NULL;
}

void* mx_list::iterate_next_no_increment()
{
    if (current_list_elem == NULL) {
        return NULL;
    } else {
        return current_list_elem->item;
    }
}

void* mx_list::last()
{
    int err = MX_ERROR_OK;
    void* res;

    res = get(err, num_items - 1);
    MX_ERROR_CLEAR(err);

    return res;
}

// move the last n items from this list into the start of another list
void mx_list::move_items_out_end(int& err, int n, mx_list& other)
{
    list_segment_t seg;

    seg.first_elem = nullptr;
    seg.last_elem = nullptr;
    seg.num_elems = 0;

    remove_last_n_elems(err, n, seg);
    MX_ERROR_CHECK(err);

    other.prepend_segment(seg);
abort:;
}

// move the first n items of another list into the end of this one
void mx_list::move_items_in_end(int& err, int n, mx_list& other)
{
    list_segment_t seg;

    seg.first_elem = nullptr;
    seg.last_elem = nullptr;
    seg.num_elems = 0;

    other.remove_first_n_elems(err, n, seg);
    MX_ERROR_CHECK(err);

    append_segment(seg);
abort:;
}

// move the first n items from this list into the end of another list
void mx_list::move_items_out_start(int& err, int n, mx_list& other)
{
    other.move_items_in_end(err, n, *this);
}

// move the last n items of another list into the start of this one
void mx_list::move_items_in_start(int& err, int n, mx_list& other)
{
    other.move_items_out_end(err, n, *this);
    MX_ERROR_CHECK(err);
abort:;
}

void mx_list::remove_all_elems(list_segment_t& seg)
{
    seg.first_elem = first_list_elem;
    seg.last_elem = last_list_elem;
    seg.num_elems = num_items;

    first_list_elem = last_list_elem = prev_list_elem = current_list_elem = NULL;
    num_items = current_index = 0;
}

void mx_list::remove_last_n_elems(int& err, int32 n, list_segment_t& seg)
{
    if (n > num_items)
        MX_ERROR_THROW(err, MX_LIST_INDEX);

    if (n == num_items) {
        remove_all_elems(seg);
    } else {
        uint32 elem_to_change_index = num_items - n - 1;

        goto_index(err, elem_to_change_index);
        MX_ERROR_CHECK(err);

        seg.first_elem = current_list_elem->next;
        seg.last_elem = last_list_elem;
        seg.num_elems = n;

        last_list_elem = current_list_elem;
        current_list_elem->next = NULL;

        num_items -= n;
        current_list_elem = first_list_elem;
        current_index = 0;

        prev_list_elem = NULL;
    }
abort:;
}

void mx_list::remove_first_n_elems(int& err, int32 n, list_segment_t& seg)
{
    if (n > num_items)
        MX_ERROR_THROW(err, MX_LIST_INDEX);

    if (n == num_items) {
        remove_all_elems(seg);
    } else {
        uint32 elem_to_change_index = n - 1;

        goto_index(err, elem_to_change_index);
        MX_ERROR_CHECK(err);

        seg.first_elem = first_list_elem;
        seg.last_elem = current_list_elem;
        seg.num_elems = n;

        first_list_elem = current_list_elem->next;
        current_list_elem->next = NULL;

        num_items -= n;
        current_list_elem = first_list_elem;
        current_index = 0;

        prev_list_elem = NULL;
    }
abort:;
}

void mx_list::prepend_segment(const list_segment_t& seg)
{
    seg.last_elem->next = first_list_elem;
    first_list_elem = seg.first_elem;

    if (last_list_elem == NULL)
        last_list_elem = seg.last_elem;

    num_items += seg.num_elems;
    current_index += seg.num_elems;

    prev_list_elem = NULL;
}

void mx_list::append_segment(const list_segment_t& seg)
{
    if (first_list_elem == NULL) {
        first_list_elem = current_list_elem = seg.first_elem;
        last_list_elem = seg.last_elem;
    } else {
        last_list_elem->next = seg.first_elem;
        last_list_elem = seg.last_elem;
    }

    if (current_index >= num_items) {
        current_index = num_items;
        current_list_elem = seg.first_elem;
    }

    num_items += seg.num_elems;

    prev_list_elem = NULL;
}
