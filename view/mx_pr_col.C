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
 * MODULE : mx_pr_col.C
 *
 * AUTHOR : David Miller
 *
 * This file
 *
 * DESCRIPTION:
 * Module mx_pr_col.C
 *
 */

#include <mx_pr_device.h>

/*-------------------------------------------------
 * FUNCTION: mx_print_device::registerColour
 *
 * DESCRIPTION:
 *
 *
 */

void mx_print_device::registerColour(mx_colour* colour, LUT_VALUE* pixel)
{
}

/*-------------------------------------------------
 * FUNCTION: setForegroundColour
 *
 * DESCRIPTION:
 *
 *
 */

void mx_print_device::setForegroundColour(int& err, LUT_VALUE pixel)
{
}

/*-------------------------------------------------
 * FUNCTION: setBackgroundColour
 *
 * DESCRIPTION:
 *
 *
 */

void mx_print_device::setBackgroundColour(int& err, LUT_VALUE pixel)
{
}

void mx_print_device::setForegroundColour(int& err, mx_colour& c)
{
}

void mx_print_device::setBackgroundColour(int& err, mx_colour& c)
{
}
