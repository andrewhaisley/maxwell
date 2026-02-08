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

#include "mx_mod_iter.h"
#include "mx_paragraph.h"
#include "mx_word.h"

mx_style_mod_iterator::mx_style_mod_iterator(mx_paragraph* para,
    const mx_para_pos& from,
    const mx_para_pos& to)
    : from_pos(from)
    , to_pos(to)
    , current_pos(from)
    , paragraph(para)
    , current_data(NULL)
{
    if (from > to) {
        from_pos = current_pos = to;
        to_pos = from;
    }
}

mx_style_mod_iterator::~mx_style_mod_iterator() { }

bool mx_style_mod_iterator::more()
{
    int err = MX_ERROR_OK;

    if (current_data != NULL) {
        current_data = NULL;
        current_pos.right(err, paragraph);
        MX_ERROR_CHECK(err);
    }

    while (current_data == NULL && current_pos < to_pos) {
        mx_word* w = paragraph->get_word(err, current_pos);
        MX_ERROR_CHECK(err);

        current_data = w->get_style_mod(err, current_pos.letter_index);
        MX_ERROR_CHECK(err);

        if (current_data == NULL) {
            current_pos.right(err, paragraph);
            MX_ERROR_CHECK(err);
        }
    }

    return current_data != NULL;

abort:
    MX_ERROR_CLEAR(err);
    return FALSE;
}

mx_char_style_mod* mx_style_mod_iterator::data()
{
    return current_data;
}

const mx_para_pos& mx_style_mod_iterator::key()
{
    return current_pos;
}
