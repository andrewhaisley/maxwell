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
 * MODULE/CLASS :  mx_position
 *
 * AUTHOR : Andrew Haisley
 *
 *
 *
 * DESCRIPTION:
 *
 * Things which represent logical positions within bits of a document - or
 * a whole document - must do at least these things.
 *
 *
 *
 *
 *
 */

#include "mx_position.h"

mx_position::mx_position()
{
    set_inside();
}

mx_position::~mx_position() { }

void mx_position::set_before_start()
{
    after_end = FALSE;
    before_start = TRUE;
}

void mx_position::set_after_end()
{
    after_end = TRUE;
    before_start = FALSE;
}

void mx_position::set_inside()
{
    after_end = before_start = FALSE;
}

// get the state of this position
bool mx_position::is_before_start()
{
    return before_start;
}

bool mx_position::is_after_end()
{
    return after_end;
}

bool mx_position::is_inside()
{
    return !before_start && !after_end;
}
