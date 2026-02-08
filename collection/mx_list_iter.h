#pragma once

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

#include <mx.h>
#include <mx_list.h>
#include <mx_rtti.h>
#include <mx_iterator.h>

class mx_list_iterator : public mx_rtti {
    MX_RTTI(mx_list_iterator_class_e)

public:
    mx_list_iterator(const mx_list& l, mx_iter_dir_t d = mx_forwards_e);

    void* data();
    int key();
    void rewind(mx_iter_dir_t d = mx_forwards_e);
    bool more();

private:
    int n_items;
    int index;
    bool forward;
    const mx_list& list;
};
