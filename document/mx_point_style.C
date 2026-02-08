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
 * MODULE/CLASS : mx_point_style
 *
 * AUTHOR : Andrew Haisley
 *
 *
 *
 * DESCRIPTION:
 *
 * styles for points
 *
 *
 *
 */

#include <mx_point_style.h>

mx_point_style::~mx_point_style()
{
}

void mx_point_style::init()
{
    point_id = -1;
    type = mx_point_style_none;
}

mx_point_style::mx_point_style()
{
    init();
}

bool operator==(const mx_point_style& s1, const mx_point_style& s2)
{
    if (s1.type == s2.type) {
        if (s1.type == mx_point_style_none) {
            return TRUE;
        } else {
            return (s1.box == s2.box);
        }
    } else {
        return FALSE;
    }
}

bool operator!=(const mx_point_style& s1, const mx_point_style& s2)
{
    return !(s1 == s2);
}

uint32 mx_point_style::get_serialised_size(int& err)
{
    uint32 res = SLS_ENUM;

    if (type != mx_point_style_none) {
        res += SLS_INT32;

        res += box.get_serialised_size(err);
        MX_ERROR_CHECK(err);
    }

abort:
    return res;
}

void mx_point_style::serialise(int& err, uint8** buffer)
{
    serialise_enum(type, buffer);

    if (type != mx_point_style_none) {
        serialise_int32(point_id, buffer);

        box.serialise(err, buffer);
        MX_ERROR_CHECK(err);
    }

abort:;
}

void mx_point_style::unserialise(int& err, uint8** buffer)
{
    uint16 e;

    unserialise_enum(e, buffer);
    type = (mx_point_style_t)e;

    if (type != mx_point_style_none) {
        unserialise_int32(point_id, buffer);

        box.unserialise(err, buffer);
        MX_ERROR_CHECK(err);
    }

abort:;
}
