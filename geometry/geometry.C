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
 * MODULE : geometry.C
 *
 * AUTHOR : David Miller
 *
 *
 *
 * DESCRIPTION: Basic geometry descriptions
 * Module geometry.C
 *
 *
 *
 */

#include <geometry.h>
#include <mx.h>

double mx_geom::fac2 = (sqrt(5.0) - 1) * 0.5;
double mx_geom::fac1 = 1.0 - mx_geom::fac2;

/*-------------------------------------------------
 * FUNCTION: mx_geom::mx_geom
 *
 * DESCRIPTION:
 *
 *
 */

mx_geom::mx_geom()
{
    type = MX_GEOM_INVALID;
    styleId = -1;
    colourId = -1;
    selectCount = MX_GEOM_SNAP_NONE;
}

/*-------------------------------------------------
 * FUNCTION: mx_geom::gserialisedSize
 *
 * DESCRIPTION:
 *
 *
 */

uint32 mx_geom::get_serialised_size(int& err)
{
    err = MX_ERROR_OK;

    return (2 * SLS_INT32);
}

/*-------------------------------------------------
 * FUNCTION: mx_geom::serialise
 *
 * DESCRIPTION:
 *
 *
 */

void mx_geom::serialise(int& err, uint8** buffer)
{
    err = MX_ERROR_OK;

    serialise_int32(styleId, buffer);
    serialise_int32(colourId, buffer);
}

/*-------------------------------------------------
 * FUNCTION: mx_geom::unserialise
 *
 * DESCRIPTION:
 *
 *
 */

void mx_geom::unserialise(int& err, uint8** buffer)
{
    err = MX_ERROR_OK;

    unserialise_int32(styleId, buffer);
    unserialise_int32(colourId, buffer);
}

double mx_geom::dist2snap(const mx_point& p,
    mx_snap_info& snapInfo,
    double tolerance) const
{
    return 0.0;
}
