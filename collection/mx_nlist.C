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
 * MODULE/CLASS :  mx_nlist
 *
 * AUTHOR : Andrew Haisley
 *
 *
 *
 * DESCRIPTION:
 *
 * Nested lists - lists of lists (only nested to one level). Intended for
 * for storing words in paragraphs and areas in sheets but no doubt other
 * uses could be found.
 *
 *
 *
 */
#include <mx_list_iter.h>
#include <mx_nlist.h>

mx_nlist::mx_nlist()
{
}

mx_nlist::mx_nlist(const mx_nlist& l)
{
    mx_list_iterator iter(l.list);

    while (iter.more()) {
        mx_list* second = (mx_list*)iter.data();

        list.append(new mx_list(*second));
    }
}

mx_nlist::~mx_nlist()
{
    mx_list_iterator iter(list);

    while (iter.more()) {
        mx_list* second = (mx_list*)iter.data();

        delete second;
    }
}

// get a row - returns the actual list, so you can do things
// to it with the list access methods
mx_list* mx_nlist::row(int& err, int index)
{
    mx_list* res = (mx_list*)list.get(err, index);
    MX_ERROR_CHECK(err);

    return res;
abort:
    return NULL;
}

// get the item out of a row at a given index
void* mx_nlist::item(int& err, int r, int index)
{
    mx_list* the_row;
    void* res;

    the_row = row(err, r);
    MX_ERROR_CHECK(err);

    res = the_row->get(err, index);
    MX_ERROR_CHECK(err);

    return res;
abort:
    return NULL;
}

// insert a new row at the given index - no copy is made
// of the list
void mx_nlist::insert(int& err, mx_list* l, int row)
{
    list.insert(err, row, l);
    MX_ERROR_CHECK(err);
abort:;
}

// insert a new empty row
mx_list* mx_nlist::insert(int& err, int row)
{
    mx_list* new_row = new mx_list;

    list.insert(err, row, new_row);
    MX_ERROR_CHECK(err);

    return new_row;
abort:;
    return NULL;
}

// insert a new item
void mx_nlist::insert(int& err, void* i, int row, int index)
{
    mx_list* second;

    if (row == list.get_num_items()) {
        second = append();
    } else {
        second = (mx_list*)list.get(err, row);
        MX_ERROR_CHECK(err);
    }

    second->insert(err, index, i);
    MX_ERROR_CHECK(err);

abort:;
}

// append a new row
void mx_nlist::append(mx_list* l)
{
    list.append(l);
}

// delete a row
mx_list* mx_nlist::delete_row(int& err, int row)
{
    mx_list* res;

    res = (mx_list*)list.remove(err, row);
    MX_ERROR_CHECK(err);

    return res;

abort:;
    return NULL;
}

// delete an item
void* mx_nlist::delete_item(int& err, int r, int index)
{
    mx_list* second;
    void* res;

    second = (mx_list*)row(err, r);
    MX_ERROR_CHECK(err);

    res = second->remove(err, index);
    MX_ERROR_CHECK(err);

    return res;
abort:;
    return NULL;
}

// move the last n items of one row into the start of the next - a new
// row is created if necessary and the old row is removed if it becomes
// empty as a result
void mx_nlist::move_items_out_end(int& err, int row, int n)
{
    mx_list* l;

    int n_rows = list.get_num_items();

    mx_list *this_row, *next_row;
    int old_n_items;

    if (row >= n_rows || row < 0) {
        MX_ERROR_THROW(err, MX_NLIST_ROW_RANGE);
    }

    this_row = (mx_list*)list.get(err, row);
    MX_ERROR_CHECK(err);

    old_n_items = this_row->get_num_items();
    if (n > old_n_items) {
        MX_ERROR_THROW(err, MX_NLIST_NITEMS_RANGE);
    }

    if (row == (n_rows - 1)) {
        next_row = new mx_list();

        append(next_row);
    } else {
        next_row = (mx_list*)list.get(err, row + 1);
        MX_ERROR_CHECK(err);
    }

    this_row->move_items_out_end(err, n, *next_row);
    MX_ERROR_CHECK(err);

    if (this_row->get_num_items() == 0) {
        l = delete_row(err, row);
        MX_ERROR_CHECK(err);

        delete l;
    }

abort:;
}

// move the first n items of the next row into the end of this - the next
// row is deleted if it becomes empty. It is an error to move more items
// than are contained in the next row.
void mx_nlist::move_items_in_end(int& err, int row, int n)
{
    mx_list* l;

    int n_rows = list.get_num_items();

    mx_list *this_row, *next_row;
    int old_n_items;

    if (row >= (n_rows - 1) || row < 0) {
        MX_ERROR_THROW(err, MX_NLIST_ROW_RANGE);
    }

    this_row = (mx_list*)list.get(err, row);
    MX_ERROR_CHECK(err);

    next_row = (mx_list*)list.get(err, row + 1);
    MX_ERROR_CHECK(err);

    old_n_items = next_row->get_num_items();
    if (n > old_n_items) {
        MX_ERROR_THROW(err, MX_NLIST_NITEMS_RANGE);
    }

    this_row->move_items_in_end(err, n, *next_row);
    MX_ERROR_CHECK(err);

    if (next_row->get_num_items() == 0) {
        l = delete_row(err, row + 1);
        MX_ERROR_CHECK(err);

        delete l;
    }

abort:;
}

// move the first n items of this row into the end of the previous - a new
// row is created if necessary (i.e. if you do it on the first row)
void mx_nlist::move_items_out_start(int& err, int row, int n)
{
    move_items_in_end(err, row - 1, n);
    MX_ERROR_CHECK(err);
abort:;
}

// move the last n items of the previous row into the start of this - it is
// an error to do this on the first row
void mx_nlist::move_items_in_start(int& err, int row, int n)
{
    move_items_out_end(err, row - 1, n);
    MX_ERROR_CHECK(err);
abort:;
}

// move the last n rows from this nlist into the start of another nlist
void mx_nlist::move_rows_out_end(int& err, int n, mx_nlist& other)
{
    int n_rows = list.get_num_items();

    if (n >= n_rows || n < 1) {
        MX_ERROR_THROW(err, MX_NLIST_ROW_RANGE);
    }

    list.move_items_out_end(err, n, other.list);
    MX_ERROR_CHECK(err);

abort:;
}

// move the first n rows of another nlist into the end of this one
void mx_nlist::move_rows_in_end(int& err, int n, mx_nlist& other)
{
    int n_rows = other.list.get_num_items();

    if (n > n_rows || n < 1) {
        MX_ERROR_THROW(err, MX_NLIST_ROW_RANGE);
    }

    list.move_items_in_end(err, n, other.list);
    MX_ERROR_CHECK(err);

abort:;
}

// move the first n rows from this nlist into the end of another nlist
void mx_nlist::move_rows_out_start(int& err, int n, mx_nlist& other)
{
    other.move_rows_in_end(err, n, *this);
}

// move the last n rows of another nlist into the start of this one
void mx_nlist::move_rows_in_start(int& err, int n, mx_nlist& other)
{
    other.move_rows_out_end(err, n, *this);
}

// append an nlist onto this one. this is the same as moving
// all the rows into the end - no copying is made and the
// second nlist is left empty
void mx_nlist::append_remove(mx_nlist& other)
{
    int err = MX_ERROR_OK;

    move_rows_in_end(err, other.get_num_rows(), other);
    MX_ERROR_CLEAR(err);
}

// append a new empty row
mx_list* mx_nlist::append()
{
    mx_list* l = new mx_list;

    append(l);
    return l;
}

void mx_nlist::append_item(void* i)
{
    mx_list* row;

    if (list.get_num_items() == 0) {
        row = append();
    } else {
        int err = MX_ERROR_OK;
        row = (mx_list*)list.get(err, list.get_num_items() - 1);
        MX_ERROR_CLEAR(err);
    }
    row->append(i);
}

int mx_nlist::get_num_items()
{
    mx_list_iterator iter(list);
    int res = 0;

    while (iter.more()) {
        mx_list* tmp = (mx_list*)iter.data();
        res += tmp->get_num_items();
    }
    return res;
}

int mx_nlist::get_num_items(int& err, int n)
{
    mx_list* r = (mx_list*)row(err, n);
    MX_ERROR_CHECK(err);

    return r->get_num_items();
abort:;
    return -1;
}

void* mx_nlist::overwrite(int& err, void* i, int row, int index)
{
    mx_list* second;
    void* res;

    second = (mx_list*)list.get(err, row);
    MX_ERROR_CHECK(err);

    res = second->overwrite(err, index, i);
    MX_ERROR_CHECK(err);

    return res;
abort:;
    return NULL;
}

void* mx_nlist::next(int& err, int row, int index)
{
    int n;
    void* res;

    n = get_num_items(err, row);
    MX_ERROR_CHECK(err);

    if (index < (n - 1)) {
        res = item(err, row, index + 1);
    } else {
        res = item(err, row + 1, 0);
    }
    MX_ERROR_CHECK(err);

    return res;
abort:;
    return NULL;
}

void mx_nlist::next(int& err, int offset, int& row, int& index)
{
    int num_items;
    index += offset;

    num_items = get_num_items(err, row);
    MX_ERROR_CHECK(err);

    while (TRUE) {
        if (index >= num_items) {
            index -= num_items;
            row++;

            num_items = get_num_items(err, row);
            MX_ERROR_CHECK(err);
        } else if (index < 0) {
            row--;
            num_items = get_num_items(err, row);
            MX_ERROR_CHECK(err);

            index += num_items;
        } else {
            return;
        }
    }

abort:;
}
