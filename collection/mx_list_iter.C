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
 * MODULE/CLASS :  mx_list_iter
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
#include <mx_list.h>
#include <mx_list_iter.h>

mx_list_iterator::mx_list_iterator(const mx_list& l, mx_iter_dir_t d)
    : list(l)
{
    forward = (d == mx_forwards_e);
    index = -1;
    n_items = l.get_num_items();
}

void* mx_list_iterator::data()
{
    int err = MX_ERROR_OK;
    void* res = NULL;

    if (!(index > n_items)) {
        res = list.get(err, index);
        MX_ERROR_CHECK(err);
    }
    return res;
abort:
    global_error_trace->print();
    MX_ERROR_CLEAR(err);
    return NULL;
}

int mx_list_iterator::key()
{
    return index;
}

void mx_list_iterator::rewind(mx_iter_dir_t d)
{
    forward = (d == mx_forwards_e);

    n_items = list.get_num_items();
    index = forward ? -1 : n_items;
}

bool mx_list_iterator::more()
{
    if (forward) {
        if (index >= (n_items - 1)) {
            return FALSE;
        } else {
            index++;
            return TRUE;
        }
    } else {
        if (index <= 0) {
            return FALSE;
        } else {
            index--;
            return TRUE;
        }
    }
}
