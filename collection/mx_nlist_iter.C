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
 * An efficient iterator for lists only
 *
 *
 *
 *
 */
#include <mx.h>
#include <mx_iterator.h>
#include <mx_nlist.h>
#include <mx_nlist_iter.h>

mx_nlist_iterator::mx_nlist_iterator(mx_nlist& l, mx_iter_dir_t d)
    : list(l)
{
    init(d);
}

void mx_nlist_iterator::init(mx_iter_dir_t d)
{
    forward = (d == mx_forwards_e);
    if (forward) {
        row_index = 0;
        item_index = -1;
        n_rows = list.list.get_num_items();
        if (n_rows > 0) {
            int err = MX_ERROR_OK;

            n_items = list.row(err, 0)->get_num_items();
            // only possible error is range check & we have already made sure of
            // that....
            MX_ERROR_CLEAR(err);
        }
    } else {
        n_rows = list.list.get_num_items();
        if (n_rows > 0) {
            int err = MX_ERROR_OK;

            row_index = n_rows - 1;

            n_items = list.row(err, row_index)->get_num_items();
            // only possible error is range check & we have already made sure of
            // that....
            MX_ERROR_CLEAR(err);

            item_index = n_items - 1;
        } else {
            row_index = -1;
            item_index = -1;
        }
    }
}

void* mx_nlist_iterator::data()
{
    if (row_index > n_rows) {
        return NULL;
    } else {
        if (item_index > n_items) {
            return NULL;
        } else {
            int err = MX_ERROR_OK;

            mx_list* row = (mx_list*)list.row(err, row_index);
            if (err != MX_ERROR_OK) {
                MX_ERROR_CLEAR(err);
                return NULL;
            }

            void* res = row->get(err, item_index);
            if (err != MX_ERROR_OK) {
                MX_ERROR_CLEAR(err);
                return NULL;
            } else {
                return res;
            }
        }
    }
}

int mx_nlist_iterator::row_key()
{
    return row_index;
}

int mx_nlist_iterator::item_key()
{
    return item_index;
}

void mx_nlist_iterator::rewind(mx_iter_dir_t d)
{
    init(d);
}

bool mx_nlist_iterator::more()
{
    int err = MX_ERROR_OK;
    int offset = forward ? 1 : -1;

    list.next(err, offset, row_index, item_index);
    MX_ERROR_CHECK(err);

    n_items = list.get_num_items(err, row_index);
    MX_ERROR_CHECK(err);

    return TRUE;
abort:
    MX_ERROR_CLEAR(err);
    return FALSE;
}

void mx_nlist_iterator::set_position(int& err, int row, int item)
{
    if (row < 0 || row > list.get_num_rows()) {
        MX_ERROR_THROW(err, MX_NLIST_ROW_RANGE);
    }

    if (item < 0 || item > list.get_num_items(err, row)) {
        MX_ERROR_THROW(err, MX_NLIST_NITEMS_RANGE);
    }
    MX_ERROR_CHECK(err);

    row_index = row;
    item_index = item;
abort:;
}

void* mx_nlist_iterator::next_data()
{
    int temp_row_index = row_index;
    int temp_item_index = item_index;
    int err = MX_ERROR_OK;

    temp_item_index++;
    if (temp_item_index > n_items) {
        temp_row_index++;
        temp_item_index = 0;
    }

    if (temp_row_index > n_rows) {
        return NULL;
    } else {
        mx_list* row = (mx_list*)list.row(err, temp_row_index);
        if (err != MX_ERROR_OK) {
            MX_ERROR_CLEAR(err);
            return NULL;
        }

        void* res = row->get(err, temp_item_index);
        if (err != MX_ERROR_OK) {
            MX_ERROR_CLEAR(err);
            return NULL;
        } else {
            return res;
        }
    }
}
