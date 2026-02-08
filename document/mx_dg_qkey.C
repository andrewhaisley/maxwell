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
 * MODULE : mx_dg_qkey.C
 *
 * AUTHOR : David Miller
 *
 * This file
 *
 * DESCRIPTION:
 * Module mx_dg_qkey.C
 *
 *
 * Revision 1.1  1996/10/06 13:15:45  andrew
 * Initial revision
 *
 *
 */

#include "mx_dg_area.h"

const int mx_dg_quad_key::maxDepth = 8;

mx_dg_quad_key::mx_dg_quad_key()
{
    depth = quadId = 0;
    offset = -1;
}

mx_dg_quad_key::mx_dg_quad_key(uint8 id,
    uint16 iq,
    int32 io)
{
    depth = id;
    quadId = iq;
    offset = io;
}

void mx_dg_quad_key::next(int iquad, mx_dg_quad_key& key)
{
    key.quadId = quadId + (iquad << (depth * 2));
    key.depth = depth + 1;
}

uint32 mx_dg_quad_key::compress()
{
    return (depth << 16) + quadId;
}

int mx_dg_quad_key::depthQuad(int atDepth) const
{
    if (atDepth > depth) {
        return -1;
    } else {
        return (quadId >> (2 * (atDepth))) & ((uint32)3);
    }
}

/*-------------------------------------------------
 * FUNCTION: mx_dg_quad_key_element::get_serialised_size
 *
 * DESCRIPTION:
 *
 *
 */

uint32 mx_dg_quad_key::get_serialised_size(int& err)
{
    err = MX_ERROR_OK;

    return size();
}

/*-------------------------------------------------
 * FUNCTION: mx_dg_quad_key::serialise
 *
 * DESCRIPTION:
 *
 *
 */

void mx_dg_quad_key::serialise(int& err,
    uint8** buffer)
{
    err = MX_ERROR_OK;

    serialise_uint8(depth, buffer);
    serialise_uint16(quadId, buffer);
    serialise_int32(offset, buffer);
}

/*-------------------------------------------------
 * FUNCTION: mx_dg_quad_key::unserialise
 *
 * DESCRIPTION:
 *
 *
 */

void mx_dg_quad_key::unserialise(int& err,
    uint8** buffer)
{
    err = MX_ERROR_OK;

    unserialise_uint8(depth, buffer);
    unserialise_uint16(quadId, buffer);
    unserialise_int32(offset, buffer);
}
