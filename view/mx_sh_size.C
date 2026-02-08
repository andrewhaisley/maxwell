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
 * MODULE : mx_sh_size.C
 *
 * AUTHOR : David Miller
 *
 * This file
 *
 * DESCRIPTION:
 * Module mx_sh_size.C
 *
 *
 *
 *
 */

#include "mx_sh_lay.h"

/*-------------------------------------------------
 * FUNCTION: mx_sheet_size::mx_sheet_size
 *
 * DESCRIPTION: Create a sheet with a given size
 *
 *
 */

mx_sheet_size::mx_sheet_size(const mx_point& ip)
{
    size = ip;
}

/*-------------------------------------------------
 * FUNCTION: mx_sheet_size::mx_sheet_size
 *
 * DESCRIPTION: Create a sheet with a given size
 * and position
 *
 */

mx_sheet_size::mx_sheet_size(const mx_point& isz, const mx_point& ipos)
{
    size = isz;
    position = ipos;
}

void mx_sheet_size::operator=(const mx_sheet_size& other)
{
    size = other.size;
    position = other.position;
    maxSize = other.maxSize;
}

mx_sheet_size::mx_sheet_size(const mx_sheet_size& other)
{
    size = other.size;
    position = other.position;
    maxSize = other.maxSize;
}
