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
 * MODULE/CLASS : mx_word
 *
 * AUTHOR : Andrew Haisley
 *
 *
 *
 * DESCRIPTION:
 *
 *
 *
 */

#include "mx_word.h"
#include <mx_char_style.h>

static int num_word_instances = 0;
bool mx_word::operations_to_left_of_mods = TRUE;

void mx_word::print_debug()
{
}

mx_word::mx_word()
{
    x = y = -100000.0f;
    style = NULL;
    num_word_instances++;
}

mx_word::mx_word(mx_char_style* s)
{
    x = y = -100000.0f;
    style = s;
    num_word_instances++;
}

mx_word::~mx_word()
{
    num_word_instances--;
}

void mx_word::set_style(mx_char_style* s)
{
    style = s;
}

// float mx_word::width()
//{
// return 0.0;
//}

// float mx_word::width(int index)
//{
// return 0.0;
//}

void mx_word::metrics(
    int& err,
    float& spacing,
    float& ascender,
    float& descender)
{
    if (style == NULL) {
        MX_ERROR_THROW(err, MX_WORD_NO_STYLE_SET);
    } else {
        spacing = style->line_space;
        ascender = style->get_ascender();
        descender = style->get_descender();
    }
    return;

abort:;
    spacing = 0.0;
    ascender = 0.0;
    descender = 0.0;
}

void mx_word::get_end_style(mx_char_style& s)
{
}

void mx_word::get_index_style(int& err, int index, mx_char_style& s)
{
    MX_ERROR_ASSERT(err, style != NULL);
    s = *style;
abort:;
}

mx_char_style* mx_word::get_style()
{
    return style;
}

mx_word* mx_word::move_last_in_start(int& err, int n, mx_word* w)
{
    MX_ERROR_THROW(err, MX_WORD_OP_NOT_AVAILABLE_ON_SUBCLASS);
abort:
    return this;
}

mx_word* mx_word::move_first_in_end(int& err, int n, mx_word* w)
{
    MX_ERROR_THROW(err, MX_WORD_OP_NOT_AVAILABLE_ON_SUBCLASS);
abort:
    return this;
}

void mx_word::insert(int& err, mx_char_style_mod& s, int index)
{
    MX_ERROR_THROW(err, MX_WORD_OP_NOT_AVAILABLE_ON_SUBCLASS);
abort:;
}

void mx_word::move_style_mod_out_end(int& err, mx_word* w) { }

bool mx_word::has_style_mod()
{
    return FALSE;
}

mx_char_style_mod* mx_word::get_style_mod(int& err, int character_index)
{
    return NULL;
}

bool mx_word::is_empty()
{
    return TRUE;
}

mx_word* mx_word::split(int& err, int index)
{
    MX_ERROR_THROW(err, MX_WORD_CANT_SPLIT_WORD);

abort:
    return NULL;
}

mx_word* mx_word::split(int& err, float f)
{
    MX_ERROR_THROW(err, MX_WORD_CANT_SPLIT_WORD);

abort:
    return NULL;
}

const char* mx_word::cstring()
{
    return "";
}

mx_word* mx_word::cut(int& err, int start, int end)
{
    MX_ERROR_THROW(err, MX_WORD_CANT_SPLIT_WORD);

abort:
    return NULL;
}

int mx_word::get_num_visible_items()
{
    return 0;
}

bool mx_word::can_be_part_of_long_word()
{
    return FALSE;
}

mx_cw_t* mx_word::remove(int& err, int start, int end)
{
    MX_ERROR_THROW(err, MX_WORD_CANT_SPLIT_WORD);

abort:
    return NULL;
}

void mx_word::move_x(float x_offset)
{
    x += x_offset;
}

void mx_word::set_operations_to_left_of_mods()
{
    operations_to_left_of_mods = TRUE;
}

void mx_word::set_operations_to_right_of_mods()
{
    operations_to_left_of_mods = FALSE;
}

bool mx_word::get_operations_to_left_of_mods()
{
    return operations_to_left_of_mods;
}

bool mx_word::get_operations_to_right_of_mods()
{
    return operations_to_left_of_mods;
}
