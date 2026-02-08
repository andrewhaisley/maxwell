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
 * MODULE/CLASS :  mx_break_word
 *
 * AUTHOR : Andrew Haisley
 *
 *
 *
 * DESCRIPTION:
 *
 * Words that represent breaks and similar things.
 *
 *
 *
 *
 *
 */
#include "mx_break_w.h"
#include "mx_complex_w.h"
#include <mx.h>
#include <mx_rtti.h>

mx_break_word::mx_break_word()
{
    word_type = mx_paragraph_break_e;
}

mx_break_word::mx_break_word(mx_break_word_type_t t)
    : mx_word()
    , word_type(t)
{
}

mx_break_word::mx_break_word(const mx_break_word& s)
    : mx_word()
    , mod(s.mod)
    , word_type(s.word_type)
{
    x = s.x;
    y = s.y;
}

mx_break_word::~mx_break_word()
{
}

mx_break_word_type_t mx_break_word::type()
{
    return word_type;
}

void mx_break_word::serialise(int& err, uint8** buffer)
{
    serialise_enum(word_type, buffer);
    mod.serialise(err, buffer);
    MX_ERROR_CHECK(err);
abort:;
}

void mx_break_word::unserialise(int& err, uint8** buffer, uint32 docid)
{
    uint16 e;

    unserialise_enum(e, buffer);
    word_type = (mx_break_word_type_t)e;
    mod.unserialise(err, buffer);
    MX_ERROR_CHECK(err);
abort:;
}

uint32 mx_break_word::get_serialised_size(int& err)
{
    uint32 res = mod.get_serialised_size(err);
    MX_ERROR_CHECK(err);

    res += SLS_ENUM;
abort:
    return res;
}

const char* mx_break_word::cstring()
{
    return (word_type == mx_long_word_break_e) ? "" : "\n";
}

void mx_break_word::metrics(int& err, float& spacing, float& ascender,
    float& descender)
{
    mx_word::metrics(err, spacing, ascender, descender);
    MX_ERROR_CHECK(err);

    if (this->type() != mx_paragraph_break_e) {
        float temp;
        mx_char_style temp_style = *style;
        temp_style += mod;

        temp = temp_style.line_space;
        if (temp > spacing)
            spacing = temp;

        temp = temp_style.get_ascender();
        if (temp > ascender)
            ascender = temp;

        temp = temp_style.get_descender();
        if (temp < descender)
            descender = temp;
    }
abort:;
}

bool mx_break_word::can_be_part_of_long_word()
{
    return (word_type == mx_long_word_break_e);
}

void mx_break_word::insert(int& err, mx_char_style_mod& s, int index)
{
    if (index != 0)
        MX_ERROR_THROW(err, MX_WORD_CHAR_INDEX_RANGE);

    if (operations_to_left_of_mods) {
        s += mod;
        mod = s;
    } else {
        mod += s;
    }

abort:;
}

void mx_break_word::move_style_mod_out_end(int& err, mx_word* w)
{
    if (type() == mx_long_word_break_e && !mod.is_null()) {
        w->insert(err, mod, 0);
        MX_ERROR_CHECK(err);

        mod = mx_char_style_mod();
    }
abort:;
}

bool mx_break_word::has_style_mod()
{
    return !mod.is_null();
}

mx_char_style_mod* mx_break_word::get_style_mod(int& err, int index)
{
    if (index != 0)
        MX_ERROR_THROW(err, MX_WORD_CHAR_INDEX_RANGE);
    if (!mod.is_null()) {
        return &mod;
    }
abort:
    return NULL;
}

void mx_break_word::minimise_mods()
{
    if (style != NULL && !mod.modifies(*style))
        mod = mx_char_style_mod();
}

void mx_break_word::replace_base_style(int& err, mx_char_style& cs)
{
    mod.set_new_default_cs(cs);
}

void mx_break_word::set_style_mod_revert_flags(int& err, mx_char_style& cs)
{
    mod.set_revert_to_default_flags(cs);
}

mx_word* mx_break_word::split(int& err, int index)
{
    MX_ERROR_THROW(err, MX_WORD_CANT_SPLIT_WORD);
abort:
    return NULL;
}

void mx_break_word::get_end_style(mx_char_style& s)
{
    s += mod;
}

void mx_break_word::get_index_style(int& err, int index, mx_char_style& s)
{
    if (index > 1)
        MX_ERROR_THROW(err, MX_WORD_CHAR_INDEX_RANGE);

    MX_ERROR_ASSERT(err, style != NULL);
    s = *style;

    if (index < 1 && operations_to_left_of_mods) {
        return;
    } else {
        s += mod;
    }
abort:;
}
