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
 * MODULE : rasterData.C
 *
 * AUTHOR : David Miller
 *
 * This file
 *
 * DESCRIPTION:
 * Module rasterData.C
 *
 *
 *
 *
 * $id$
 * $log$
 *
 */

#include "raster.h"

/*-------------------------------------------------
 * FUNCTION: rasterBits
 *
 * DESCRIPTION: Get the number of bits in some pixels
 *
 *
 */

int rasterBits(RAST_PACK packtype, int32 npixels, int32 nplanes)
{
    int value = 0;

    switch (packtype) {
    case RPACK_BIT:
        value = 1;
        break;
    case RPACK_BYTE:
        value = 8;
        break;
    case RPACK_SHORT:
        value = 16;
        break;
    case RPACK_INT:
        value = 32;
        break;
    }

    return value * npixels * nplanes;
}

/*-------------------------------------------------
 * FUNCTION: rasterBytes
 *
 * DESCRIPTION: Get the number of bytes in some pixels
 *
 *
 */

int rasterBytes(RAST_PACK packtype, int32 npixels, int32 nplanes)
{
    return (rasterBits(packtype, npixels, nplanes) + 7) / 8;
}
