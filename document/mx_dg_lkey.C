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
 * MODULE : mx_dg_lkey.C
 *
 * AUTHOR : David Miller
 *
 * This file
 *
 * DESCRIPTION:
 * Module mx_dg_lkey.C
 *
 *
 * Revision 1.1  1996/10/06 13:15:45  andrew
 * Initial revision
 *
 *
 */

#include "mx_dg_area.h"

/*-------------------------------------------------
 * FUNCTION: mx_dg_level_key::mx_dg_level_key
 *
 * DESCRIPTION:
 *
 *
 */

mx_dg_level_key::mx_dg_level_key(uint8 ilevelId,
    int32 ioffset)
{
    levelId = ilevelId;
    offset = ioffset;
}

/*-------------------------------------------------
 * FUNCTION: mx_dg_level_key::get_serialised_size
 *
 * DESCRIPTION:
 *
 *
 */

uint32 mx_dg_level_key::get_serialised_size(int& err)
{
    err = MX_ERROR_OK;

    return mx_dg_level_key::size();
}

/*-------------------------------------------------
 * FUNCTION: mx_dg_level_key::serialise
 *
 * DESCRIPTION:
 *
 *
 */

void mx_dg_level_key::serialise(int& err,
    uint8** buffer)
{
    err = MX_ERROR_OK;

    serialise_uint32(compress(), buffer);
}

/*-------------------------------------------------
 * FUNCTION: mx_dg_level_key::unserialise
 *
 * DESCRIPTION:
 *
 *
 */

void mx_dg_level_key::unserialise(int& err,
    uint8** buffer)
{
    uint32 uncomp;

    err = MX_ERROR_OK;

    unserialise_uint32(uncomp, buffer);

    uncompress(uncomp);
}
