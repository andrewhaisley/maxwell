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
 *
 */
#ifndef MX_NLIST_H
#define MX_NLIST_H

#include <mx.h>
#include <mx_list.h>
#include <mx_rtti.h>

class mx_nlist : public mx_rtti {
    MX_RTTI(mx_nlist_class_e)

public:
    mx_nlist();
    mx_nlist(const mx_nlist& l);
    virtual ~mx_nlist();

    // get a row - returns the actual list, so you can do things
    // to it with the list access methods
    mx_list* row(int& err, int index);

    // get the item out of a row at a given index
    void* item(int& err, int row, int index);

    // get the next item out of a row at a given index
    void* next(int& err, int row, int index);

    // get the index for the next/previous nth item. Returns an error if the
    // next nth is not presxent in the nlist
    void next(int& err, int offset, int& row, int& index);

    // insert a new row at the given index - no copy is made
    // of the list
    void insert(int& err, mx_list* l, int row = 0);

    // insert a new empty row at the given index - no copy is made
    // of the list
    mx_list* insert(int& err, int row = 0);

    // insert a new item - if the row is one greater than the current size,
    // a new row is added
    void insert(int& err, void* i, int row = 0, int index = 0);

    // overwrite the value of an item
    void* overwrite(int& err, void* i, int row = 0, int index = 0);

    // append a new row
    void append(mx_list* l);

    // append a new empty row
    mx_list* append();

    // append an item right at the end
    void append_item(void* i);

    // delete a row
    mx_list* delete_row(int& err, int row = 0);

    // delete an item
    void* delete_item(int& err, int row = 0, int index = 0);

    // move the last n items of one row into the start of the next - a new
    // row is created if necessary and the old row is removed if it becomes
    // empty as a result
    void move_items_out_end(int& err, int row, int n);

    // move the first n items of the next row into the end of this - the next
    // row is deleted if it becomes empty. It is an error to move more items
    // than are contained in the next row.
    void move_items_in_end(int& err, int row, int n);

    // move the first n items of this row into the end of the previous - a new
    // row is created if necessary (i.e. if you do it on the first row)
    void move_items_out_start(int& err, int row, int n);

    // move the last n items of the previous row into the start of this - it is
    // an error to do this on the first row
    void move_items_in_start(int& err, int row, int n);

    // move the last n rows from this nlist into the start of
    // another nlist
    void move_rows_out_end(int& err, int n, mx_nlist& other);

    // move the first n rows of another nlist into the end of
    // this one
    void move_rows_in_end(int& err, int n, mx_nlist& other);

    // move the first n rows from this nlist into the end of
    // another nlist
    void move_rows_out_start(int& err, int n, mx_nlist& other);

    // move the last n rows of another nlist into the start of
    // this one
    void move_rows_in_start(int& err, int n, mx_nlist& other);

    // append an nlist onto this one. this is the same as moving
    // all the rows into the end - no copying is made and the
    // second nlist is left empty
    void append_remove(mx_nlist& other);

    // append by copying everything except whatever the individual lists
    // point to
    void append(mx_nlist& other);

    // get the total number of items in the nlist
    int get_num_items();

    // get the number of items in a row
    int get_num_items(int& err, int row);

    // get the number of rows
    int get_num_rows() { return list.get_num_items(); };

    // this is public so you can do things to it with the list class methods
    mx_list list;
};

#endif
