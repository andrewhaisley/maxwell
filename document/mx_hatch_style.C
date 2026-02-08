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
 * MODULE : mx_hatch_style.C
 *
 * AUTHOR : David Miller
 *
 * This file
 *
 * DESCRIPTION:
 * Module mx_hatch_style.C
 *
 *
 * Revision 1.1  1996/10/06 13:15:45  andrew
 * Initial revision
 *
 *
 */

#include "mx_hatch_style.h"

void mx_hatch_style::getHatchValues(int ihatch,
    float& oangle,
    float& ospacing,
    int& olineStyleId,
    int& ocolourId)
{
    oangle = angle[ihatch];
    ospacing = spacing[ihatch];
    olineStyleId = line_style[ihatch].line_id;
    ocolourId = line_style[ihatch].colour_id;
}

void mx_hatch_style::init()
{
    spacing[0] = 0;
    spacing[1] = 0;
    angle[0] = 0;
    angle[1] = PI2;
    hatch_id = -1;
}

mx_hatch_style::~mx_hatch_style()
{
}

mx_hatch_style::mx_hatch_style()
{
    init();
}

uint32 mx_hatch_style::get_serialised_size(int& err)
{
    uint32 res;

    if (spacing[0] == 0) {
        res = SLS_FLOAT;
    } else {
        res = 4 * SLS_FLOAT + SLS_INT32;

        res += anchor.get_serialised_size(err);
        MX_ERROR_CHECK(err);

        res += line_style[0].get_serialised_size(err);
        MX_ERROR_CHECK(err);

        res += line_style[1].get_serialised_size(err);
        MX_ERROR_CHECK(err);
    }

abort:
    return res;
}

void mx_hatch_style::serialise(int& err, uint8** buffer)
{
    err = MX_ERROR_OK;

    serialise_float(spacing[0], buffer);

    if (spacing[0] != 0) {
        serialise_float(spacing[1], buffer);
        serialise_float(angle[0], buffer);
        serialise_float(angle[1], buffer);
        serialise_int32(hatch_id, buffer);

        anchor.serialise(err, buffer);
        MX_ERROR_CHECK(err);

        line_style[0].serialise(err, buffer);
        MX_ERROR_CHECK(err);

        line_style[1].serialise(err, buffer);
        MX_ERROR_CHECK(err);
    }

abort:;
}

void mx_hatch_style::unserialise(int& err, uint8** buffer)
{
    unserialise_float(spacing[0], buffer);

    if (spacing[0] != 0) {
        unserialise_float(spacing[1], buffer);
        unserialise_float(angle[0], buffer);
        unserialise_float(angle[1], buffer);

        unserialise_int32(hatch_id, buffer);

        anchor.unserialise(err, buffer);
        MX_ERROR_CHECK(err);

        line_style[0].unserialise(err, buffer);
        MX_ERROR_CHECK(err);

        line_style[1].unserialise(err, buffer);
        MX_ERROR_CHECK(err);
    } else {
        init();
    }

abort:;
}

bool operator==(const mx_hatch_style& s1, const mx_hatch_style& s2)
{
    if (s1.spacing[0] == s2.spacing[1]) {
        if (s1.spacing[0] == 0) {
            return TRUE;
        } else {
            return s1.spacing[1] == s2.spacing[1] && s1.angle[0] == s2.angle[0] && s1.angle[1] == s2.angle[1] && s1.anchor == s2.anchor && s1.line_style[0] == s2.line_style[0] && s1.line_style[1] == s2.line_style[1];
        }
    } else {
        return FALSE;
    }
}

bool operator!=(const mx_hatch_style& s1, const mx_hatch_style& s2)
{
    return !(s1 == s2);
}
