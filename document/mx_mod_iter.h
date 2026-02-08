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
#ifndef MX_MOD_ITER_H
#define MX_MOD_ITER_H
/*
 * MODULE/CLASS :  mx_style_mod_iterator
 *
 * AUTHOR : Tom Newton
 *
 *
 *
 * DESCRIPTION:
 *  a class that iterates over character style mods in a paragraph
 *
 *
 *
 *
 */

#include <mx_para_pos.h>

class mx_paragraph;
class mx_char_style_mod;

class mx_style_mod_iterator {
public:
    mx_style_mod_iterator(mx_paragraph* para,
        const mx_para_pos& from,
        const mx_para_pos& to);
    ~mx_style_mod_iterator();

    // are there more mods in the range to get data from
    bool more();

    // get the data
    mx_char_style_mod* data();

    // returns the current position the iterator is at
    const mx_para_pos& key();

private:
    uint32 item_index;
    mx_para_pos from_pos, to_pos, current_pos;
    mx_paragraph* paragraph;
    mx_char_style_mod* current_data;
};

#endif // MX_MOD_ITER_H
