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
 * MODULE/CLASS : mx_fill_style
 *
 * AUTHOR : Andrew Haisley
 *
 *
 *
 * DESCRIPTION:
 *
 * styles for filling areas
 *
 *
 *
 *
 *
 */

#include <mx_fill_style.h>

mx_fill_style::mx_fill_style()
{
    type = mx_fill_colour;
    fill_id = -1;
    colour_id = -1;
}

mx_fill_style::mx_fill_style(mx_fill_type_t t)
{
    type = t;
    fill_id = -1;
    colour_id = -1;
}

mx_fill_style::mx_fill_style(const char* colour_name)
{
    type = mx_fill_colour;
    colour.name = colour_name;
    fill_id = -1;
    colour_id = -1;
}

mx_fill_style::mx_fill_style(const mx_fill_style& s)
{
    type = s.type;
    colour = s.colour;
    fill_id = s.fill_id;
    colour_id = s.colour_id;
}

mx_fill_style::~mx_fill_style()
{
}

bool operator==(const mx_fill_style& s1, const mx_fill_style& s2)
{
    return s1.type == s2.type && s1.colour == s2.colour;
}

bool operator!=(const mx_fill_style& s1, const mx_fill_style& s2)
{
    return s1.type != s2.type || s1.colour != s2.colour;
}

uint32 mx_fill_style::get_serialised_size(int& err)
{
    uint32 res = SLS_ENUM + SLS_INT32;

    if (type == mx_fill_colour) {
        res += colour.get_serialised_size(err);
        MX_ERROR_CHECK(err);
    }

abort:
    return res;
}

void mx_fill_style::serialise(int& err, uint8** buffer)
{
    serialise_enum(type, buffer);
    serialise_int32(fill_id, buffer);

    if (type == mx_fill_colour) {
        colour.serialise(err, buffer);
        MX_ERROR_CHECK(err);
    }
abort:;
}

void mx_fill_style::unserialise(int& err, uint8** buffer)
{
    uint16 e;

    unserialise_enum(e, buffer);
    unserialise_int32(fill_id, buffer);
    type = (mx_fill_type_t)e;
    if (type == mx_fill_colour) {
        colour.unserialise(err, buffer);
        MX_ERROR_CHECK(err);
    }
abort:;
}

void mx_fill_style::operator=(const mx_fill_style& other)
{
    type = other.type;
    colour = other.colour;
    fill_id = other.fill_id;
    colour_id = other.colour_id;
}

/*-------------------------------------------------
 * FUNCTION: mx_fill_style::setStyleChanges
 *
 * DESCRIPTION:
 *
 *
 */

void mx_fill_style::setStyleChanges(bool setNew,
    mx_fill_style& newStyle,
    bool& setFill)
{
    if (setNew) {
        setFill = TRUE;
        *this = newStyle;
    } else {
        setFill = (type == newStyle.type);
        type = newStyle.type;
    }
}
