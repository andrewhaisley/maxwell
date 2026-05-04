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
 * MODULE : mx_pr_ras.C
 *
 * AUTHOR : David Miller
 *
 * This file
 *
 * DESCRIPTION:
 * Module mx_pr_ras.C
 *
 *
 * Revision 1.1  1996/10/06 13:15:45  andrew
 * Initial revision
 *
 *
 */

#include <mx_pr_device.h>
#include <raster.h>

/*-------------------------------------------------
 * FUNCTION: mx_pr_device::pdrawTile
 *
 * DESCRIPTION:
 *
 *
 */

void mx_print_device::pdrawTile(int& err,
    MX_DEVICE_RASTER tileType,
    int nsigbits,
    int nunpackedBits,
    mx_ipoint& tileSize,
    mx_ipoint& topLeft,
    void* tile,
    mx_irect* clipRect)
{
}

/*-------------------------------------------------
 * FUNCTION: mx_pr_device::outputImageHeader
 *
 * DESCRIPTION:
 *
 *
 */

void mx_print_device::outputImageHeader(int& err,
    mx_point& topLeftOffset,
    mx_point& realSize,
    MX_DEVICE_RASTER deviceType,
    int nbits,
    int nplanes,
    int ncolours,
    mx_colour* colours,
    mx_ipoint& imageSize,
    bool clipSet)
{
}

/*-------------------------------------------------
 * FUNCTION: mx_pr_device::outputImageTrailer
 *
 * DESCRIPTION:
 *
 *
 */

void mx_print_device::outputImageTrailer(int& err, bool clipSet)
{
}
