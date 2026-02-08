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
 * MODULE/CLASS : mx_area_style
 *
 * AUTHOR : Andrew Haisley
 *
 *
 *
 * DESCRIPTION:
 *
 * styles for areas
 *
 *
 *
 */

#include <mx_area_style.h>

mx_area_style::mx_area_style()
{
    area_id = -1;
}

mx_area_style::~mx_area_style()
{
}

bool operator==(const mx_area_style& s1, const mx_area_style& s2)
{
    return (s1.border_style == s2.border_style) && (s1.hatch_style == s2.hatch_style);
}

bool operator!=(const mx_area_style& s1, const mx_area_style& s2)
{
    return !(s1 == s2);
}

uint32 mx_area_style::get_serialised_size(int& err)
{
    uint32 res = SLS_INT32;

    res += border_style.get_serialised_size(err);
    MX_ERROR_CHECK(err);

    res += hatch_style.get_serialised_size(err);
    MX_ERROR_CHECK(err);

abort:
    return res;
}

void mx_area_style::serialise(int& err, uint8** buffer)
{
    serialise_int32(area_id, buffer);

    border_style.serialise(err, buffer);
    MX_ERROR_CHECK(err);

    hatch_style.serialise(err, buffer);
    MX_ERROR_CHECK(err);

abort:;
}

void mx_area_style::unserialise(int& err, uint8** buffer)
{
    unserialise_int32(area_id, buffer);

    border_style.unserialise(err, buffer);
    MX_ERROR_CHECK(err);

    hatch_style.unserialise(err, buffer);
    MX_ERROR_CHECK(err);

abort:;
}
