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
 * MODULE/CLASS :  mx_nlist_iter
 *
 * AUTHOR : Andrew Haisley
 *
 *
 *
 * DESCRIPTION:
 *
 * Simple & efficient iteration over the whole contents of an nlist
 *
 * Note: this iterator cannot handle the contents of the nlist changing
 * behind it's back.
 *
 *
 *
 *
 *
 */
#ifndef MX_NLIST_ITERATOR
#define MX_NLIST_ITERATOR

#include <mx.h>
#include <mx_iterator.h>
#include <mx_nlist.h>
#include <mx_rtti.h>

class mx_nlist_iterator : public mx_rtti {
    MX_RTTI(mx_nlist_iterator_class_e)

public:
    mx_nlist_iterator(mx_nlist& l, mx_iter_dir_t d = mx_forwards_e);

    // get the current item
    void* data();

    // returns NULL if the current item is the last
    void* next_data();

    int row_key();
    int item_key();
    void rewind(mx_iter_dir_t d = mx_forwards_e);
    bool more();

    void set_position(int& err, int row, int item);

    void init(mx_iter_dir_t d);

private:
    int n_items;
    int n_rows;
    int row_index;
    int item_index;
    bool forward;
    mx_nlist& list;
};

#endif
