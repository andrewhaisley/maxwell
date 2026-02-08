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
 * MODULE/CLASS : mx_border_style
 *
 * AUTHOR : Andrew Haisley
 *
 *
 *
 * DESCRIPTION:
 *
 * Styles for borders
 *
 *
 *
 *
 */

#include "mx_bd_style.h"

#include <mx.h>
#include <mx_db.h>

mx_border_style::mx_border_style()
    : top_style(mx_no_line, 0.0f)
    , bottom_style(mx_no_line, 0.0f)
    , left_style(mx_no_line, 0.0f)
    , right_style(mx_no_line, 0.0f)
    , fill_style(mx_fill_transparent)
    , distance_from_contents(0.0f)
{
    border_id = -1;
}

mx_border_style::~mx_border_style()
{
}

mx_border_style::mx_border_style(const mx_border_style& rvalue)
    : top_style(rvalue.top_style)
    , bottom_style(rvalue.bottom_style)
    , left_style(rvalue.left_style)
    , right_style(rvalue.right_style)
    , fill_style(rvalue.fill_style)
    , distance_from_contents(rvalue.distance_from_contents)
{
    border_id = rvalue.border_id;
}

uint32 mx_border_style::get_serialised_size(int& err)
{
    uint32 res = SLS_FLOAT + SLS_INT32;

    res += top_style.get_serialised_size(err);
    MX_ERROR_CHECK(err);

    res += bottom_style.get_serialised_size(err);
    MX_ERROR_CHECK(err);

    res += left_style.get_serialised_size(err);
    MX_ERROR_CHECK(err);

    res += right_style.get_serialised_size(err);
    MX_ERROR_CHECK(err);

    res += fill_style.get_serialised_size(err);
    MX_ERROR_CHECK(err);

    return res;

abort:
    return 0;
}

mx_border_style& mx_border_style::operator=(const mx_border_style& rvalue)
{
    top_style = rvalue.top_style;
    bottom_style = rvalue.bottom_style;
    left_style = rvalue.left_style;
    right_style = rvalue.right_style;
    fill_style = rvalue.fill_style;
    distance_from_contents = rvalue.distance_from_contents;
    border_id = rvalue.border_id;

    return *this;
}

void mx_border_style::serialise(int& err, unsigned char** buffer)
{
    err = MX_ERROR_OK;

    serialise_float(distance_from_contents, buffer);
    serialise_int32(border_id, buffer);

    top_style.serialise(err, buffer);
    MX_ERROR_CHECK(err);

    bottom_style.serialise(err, buffer);
    MX_ERROR_CHECK(err);

    left_style.serialise(err, buffer);
    MX_ERROR_CHECK(err);

    right_style.serialise(err, buffer);
    MX_ERROR_CHECK(err);

    fill_style.serialise(err, buffer);
    MX_ERROR_CHECK(err);

abort:;
}

void mx_border_style::unserialise(int& err, unsigned char** buffer)
{
    err = MX_ERROR_OK;

    unserialise_float(distance_from_contents, buffer);
    unserialise_int32(border_id, buffer);

    top_style.unserialise(err, buffer);
    MX_ERROR_CHECK(err);

    bottom_style.unserialise(err, buffer);
    MX_ERROR_CHECK(err);

    left_style.unserialise(err, buffer);
    MX_ERROR_CHECK(err);

    right_style.unserialise(err, buffer);
    MX_ERROR_CHECK(err);

    fill_style.unserialise(err, buffer);
    MX_ERROR_CHECK(err);

abort:;
}

bool operator==(const mx_border_style& s1, const mx_border_style& s2)
{
    return (MX_FLOAT_EQ(s1.distance_from_contents, s2.distance_from_contents)) && (s1.top_style == s2.top_style) && (s1.bottom_style == s2.bottom_style) && (s1.left_style == s2.left_style) && (s1.right_style == s2.right_style) && (s1.fill_style == s2.fill_style);
}

bool operator!=(const mx_border_style& s1, const mx_border_style& s2)
{
    return !(s1 == s2);
}

mx_border_style& mx_border_style::operator+=(const mx_border_style_mod& o)
{
    if (o.top_style_mod) {
        top_style = o.top_style;
    }

    if (o.bottom_style_mod) {
        bottom_style = o.bottom_style;
    }

    if (o.left_style_mod) {
        left_style = o.left_style;
    }

    if (o.right_style_mod) {
        right_style = o.right_style;
    }

    if (o.fill_style_mod) {
        fill_style = o.fill_style;
    }

    if (o.distance_from_contents_mod) {
        distance_from_contents = o.distance_from_contents;
    }
    return *this;
}

mx_border_style_mod::mx_border_style_mod()
{
    top_style_mod = bottom_style_mod = left_style_mod = right_style_mod = horizontal_style_mod = vertical_style_mod = fill_style_mod = distance_from_contents_mod = FALSE;
}

mx_border_style_mod::~mx_border_style_mod()
{
}

mx_border_style_mod::mx_border_style_mod(const mx_border_style& s1, const mx_border_style& s2)
{
    top_style_mod = bottom_style_mod = left_style_mod = right_style_mod = horizontal_style_mod = vertical_style_mod = fill_style_mod = distance_from_contents_mod = FALSE;

    if (s1.top_style != s2.top_style) {
        top_style_mod = TRUE;
        top_style = s2.top_style;
    }

    if (s1.bottom_style != s2.bottom_style) {
        bottom_style_mod = TRUE;
        bottom_style = s2.bottom_style;
    }

    if (s1.left_style != s2.left_style) {
        left_style_mod = TRUE;
        left_style = s2.left_style;
    }

    if (s1.right_style != s2.right_style) {
        right_style_mod = TRUE;
        right_style = s2.right_style;
    }

    if (s1.fill_style != s2.fill_style) {
        fill_style_mod = TRUE;
        fill_style = s2.fill_style;
    }

    if (!MX_FLOAT_EQ(s1.distance_from_contents, s2.distance_from_contents)) {
        distance_from_contents_mod = TRUE;
        distance_from_contents = s2.distance_from_contents;
    }
}

mx_border_style_mod::mx_border_style_mod(const mx_border_style_mod& o)
{
    top_style_mod = o.top_style_mod;
    top_style = o.top_style;
    bottom_style_mod = o.bottom_style_mod;
    bottom_style = o.bottom_style;
    left_style_mod = o.left_style_mod;
    left_style = o.left_style;
    right_style_mod = o.right_style_mod;
    right_style = o.right_style;
    fill_style_mod = o.fill_style_mod;
    fill_style = o.fill_style;
    vertical_style_mod = o.vertical_style_mod;
    vertical_style = o.vertical_style;
    horizontal_style_mod = o.horizontal_style_mod;
    horizontal_style = o.horizontal_style;
    distance_from_contents_mod = o.distance_from_contents_mod;
    distance_from_contents = o.distance_from_contents;
}

mx_border_style_mod::mx_border_style_mod(const mx_border_style& o)
    : horizontal_style()
    , vertical_style()
{
    vertical_style_mod = horizontal_style_mod = FALSE;

    set_top_style_mod(o.top_style);
    set_bottom_style_mod(o.bottom_style);
    set_left_style_mod(o.left_style);
    set_right_style_mod(o.right_style);
    set_fill_style_mod(o.fill_style);
    set_distance_from_contents_mod(o.distance_from_contents);
}

mx_border_style_mod& mx_border_style_mod::operator=(const mx_border_style_mod& o)
{
    top_style_mod = o.top_style_mod;
    top_style = o.top_style;
    bottom_style_mod = o.bottom_style_mod;
    bottom_style = o.bottom_style;
    left_style_mod = o.left_style_mod;
    left_style = o.left_style;
    right_style_mod = o.right_style_mod;
    right_style = o.right_style;
    fill_style_mod = o.fill_style_mod;
    fill_style = o.fill_style;
    horizontal_style_mod = o.horizontal_style_mod;
    horizontal_style = o.horizontal_style;
    vertical_style_mod = o.vertical_style_mod;
    vertical_style = o.vertical_style;
    distance_from_contents_mod = o.distance_from_contents_mod;
    distance_from_contents = o.distance_from_contents;
    return *this;
}

mx_border_style_mod& mx_border_style_mod::operator+=(const mx_border_style_mod& o)
{
    if (o.top_style_mod) {
        top_style_mod = TRUE;
        top_style = o.top_style;
    }

    if (o.bottom_style_mod) {
        bottom_style_mod = TRUE;
        bottom_style = o.bottom_style;
    }

    if (o.left_style_mod) {
        left_style_mod = TRUE;
        left_style = o.left_style;
    }

    if (o.right_style_mod) {
        right_style_mod = TRUE;
        right_style = o.right_style;
    }

    if (o.fill_style_mod) {
        fill_style_mod = TRUE;
        fill_style = o.fill_style;
    }

    if (o.horizontal_style_mod) {
        horizontal_style_mod = TRUE;
        horizontal_style = o.horizontal_style;
    }

    if (o.vertical_style_mod) {
        vertical_style_mod = TRUE;
        vertical_style = o.vertical_style;
    }

    if (o.distance_from_contents_mod) {
        distance_from_contents_mod = TRUE;
        distance_from_contents = o.distance_from_contents;
    }
    return *this;
}

bool mx_border_style_mod::is_null() const
{
    return !(top_style_mod || bottom_style_mod || left_style_mod || right_style_mod || fill_style_mod || horizontal_style_mod || vertical_style_mod || distance_from_contents_mod);
}

bool mx_border_style_mod::modifies(const mx_border_style& bs) const
{
    mx_border_style new_bs = bs;
    new_bs += *this;
    return (new_bs != bs);
}

void mx_border_style_mod::clear_mods_in(const mx_border_style_mod& o)
{
    if (o.top_style_mod)
        clear_top_style_mod();
    if (o.bottom_style_mod)
        clear_bottom_style_mod();
    if (o.left_style_mod)
        clear_left_style_mod();
    if (o.right_style_mod)
        clear_right_style_mod();
    if (o.fill_style_mod)
        clear_fill_style_mod();
    if (o.horizontal_style_mod)
        clear_horizontal_style_mod();
    if (o.vertical_style_mod)
        clear_vertical_style_mod();
    if (o.distance_from_contents_mod)
        clear_distance_from_contents_mod();
}

void mx_border_style_mod::serialise(int& err, uint8** buffer)
{
    serialise_bool(top_style_mod, buffer);
    if (top_style_mod) {
        top_style.serialise(err, buffer);
        MX_ERROR_CHECK(err);
    }

    serialise_bool(bottom_style_mod, buffer);
    if (bottom_style_mod) {
        bottom_style.serialise(err, buffer);
        MX_ERROR_CHECK(err);
    }

    serialise_bool(left_style_mod, buffer);
    if (left_style_mod) {
        left_style.serialise(err, buffer);
        MX_ERROR_CHECK(err);
    }

    serialise_bool(right_style_mod, buffer);
    if (right_style_mod) {
        right_style.serialise(err, buffer);
        MX_ERROR_CHECK(err);
    }

    serialise_bool(fill_style_mod, buffer);
    if (fill_style_mod) {
        fill_style.serialise(err, buffer);
        MX_ERROR_CHECK(err);
    }

    // we do not serialise the horizontal and vertical style mods.

    serialise_bool(distance_from_contents_mod, buffer);
    if (distance_from_contents_mod) {
        serialise_float(distance_from_contents, buffer);
    }
abort:;
}

void mx_border_style_mod::unserialise(int& err, uint8** buffer)
{
    bool b;

    unserialise_bool(b, buffer);
    top_style_mod = b;
    if (top_style_mod) {
        top_style.unserialise(err, buffer);
        MX_ERROR_CHECK(err);
    }

    unserialise_bool(b, buffer);
    bottom_style_mod = b;
    if (bottom_style_mod) {
        bottom_style.unserialise(err, buffer);
        MX_ERROR_CHECK(err);
    }

    unserialise_bool(b, buffer);
    left_style_mod = b;
    if (left_style_mod) {
        left_style.unserialise(err, buffer);
        MX_ERROR_CHECK(err);
    }

    unserialise_bool(b, buffer);
    right_style_mod = b;
    if (right_style_mod) {
        right_style.unserialise(err, buffer);
        MX_ERROR_CHECK(err);
    }

    unserialise_bool(b, buffer);
    fill_style_mod = b;
    if (fill_style_mod) {
        fill_style.unserialise(err, buffer);
        MX_ERROR_CHECK(err);
    }

    // we do not serialise the horizontal and vertical style mods.

    unserialise_bool(b, buffer);
    distance_from_contents_mod = b;
    if (distance_from_contents_mod) {
        unserialise_float(distance_from_contents, buffer);
    }
abort:;
}

uint32 mx_border_style_mod::get_serialised_size(int& err)
{
    uint32 res = SLS_BOOL * 6;

    if (top_style_mod) {
        res += top_style.get_serialised_size(err);
        MX_ERROR_CHECK(err);
    }

    if (bottom_style_mod) {
        res += bottom_style.get_serialised_size(err);
        MX_ERROR_CHECK(err);
    }

    if (left_style_mod) {
        res += left_style.get_serialised_size(err);
        MX_ERROR_CHECK(err);
    }

    if (right_style_mod) {
        res += right_style.get_serialised_size(err);
        MX_ERROR_CHECK(err);
    }

    if (fill_style_mod) {
        res += fill_style.get_serialised_size(err);
        MX_ERROR_CHECK(err);
    }

    // we do not serialise the horizontal and vertical style mods.

    if (distance_from_contents_mod) {
        res += SLS_FLOAT;
    }
    return res;
abort:;
    return 0;
}

bool mx_border_style_mod::get_top_style_mod(mx_line_style& new_ts) const
{
    if (top_style_mod)
        new_ts = top_style;
    return top_style_mod;
}

bool mx_border_style_mod::get_bottom_style_mod(mx_line_style& new_bs) const
{
    if (bottom_style_mod)
        new_bs = bottom_style;
    return bottom_style_mod;
}

bool mx_border_style_mod::get_left_style_mod(mx_line_style& new_ls) const
{
    if (left_style_mod)
        new_ls = left_style;
    return left_style_mod;
}

bool mx_border_style_mod::get_right_style_mod(mx_line_style& new_rs) const
{
    if (right_style_mod)
        new_rs = right_style;
    return right_style_mod;
}

bool mx_border_style_mod::get_fill_style_mod(mx_fill_style& new_fs) const
{
    if (fill_style_mod)
        new_fs = fill_style;
    return fill_style_mod;
}

bool mx_border_style_mod::get_horizontal_style_mod(mx_line_style& new_hs) const
{
    if (horizontal_style_mod)
        new_hs = horizontal_style;
    return horizontal_style_mod;
}

bool mx_border_style_mod::get_vertical_style_mod(mx_line_style& new_vs) const
{
    if (vertical_style_mod)
        new_vs = vertical_style;
    return vertical_style_mod;
}

bool mx_border_style_mod::get_distance_from_contents_mod(float& new_dfc) const
{
    if (distance_from_contents_mod)
        new_dfc = distance_from_contents;
    return distance_from_contents_mod;
}

void mx_border_style_mod::set_top_style_mod(const mx_line_style& new_ts)
{
    top_style_mod = TRUE;
    top_style = new_ts;
}

void mx_border_style_mod::set_bottom_style_mod(const mx_line_style& new_bs)
{
    bottom_style_mod = TRUE;
    bottom_style = new_bs;
}

void mx_border_style_mod::set_left_style_mod(const mx_line_style& new_ls)
{
    left_style_mod = TRUE;
    left_style = new_ls;
}

void mx_border_style_mod::set_right_style_mod(const mx_line_style& new_rs)
{
    right_style_mod = TRUE;
    right_style = new_rs;
}

void mx_border_style_mod::set_fill_style_mod(const mx_fill_style& new_fs)
{
    fill_style_mod = TRUE;
    fill_style = new_fs;
}

void mx_border_style_mod::set_horizontal_style_mod(const mx_line_style& new_rs)
{
    horizontal_style_mod = TRUE;
    horizontal_style = new_rs;
}

void mx_border_style_mod::set_vertical_style_mod(const mx_line_style& new_rs)
{
    vertical_style_mod = TRUE;
    vertical_style = new_rs;
}

void mx_border_style_mod::set_distance_from_contents_mod(float new_dfc)
{
    distance_from_contents_mod = TRUE;
    distance_from_contents = new_dfc;
}

void mx_border_style_mod::clear_top_style_mod()
{
    top_style_mod = FALSE;
    top_style = mx_line_style();
}

void mx_border_style_mod::clear_bottom_style_mod()
{
    bottom_style_mod = FALSE;
    bottom_style = mx_line_style();
}

void mx_border_style_mod::clear_left_style_mod()
{
    left_style_mod = FALSE;
    left_style = mx_line_style();
}

void mx_border_style_mod::clear_right_style_mod()
{
    right_style_mod = FALSE;
    right_style = mx_line_style();
}

void mx_border_style_mod::clear_fill_style_mod()
{
    fill_style_mod = FALSE;
    fill_style = mx_fill_style();
}

void mx_border_style_mod::clear_horizontal_style_mod()
{
    horizontal_style_mod = FALSE;
    horizontal_style = mx_line_style();
}

void mx_border_style_mod::clear_vertical_style_mod()
{
    vertical_style_mod = FALSE;
    vertical_style = mx_line_style();
}

void mx_border_style_mod::clear_distance_from_contents_mod()
{
    distance_from_contents_mod = FALSE;
    distance_from_contents = 0.0f;
}
