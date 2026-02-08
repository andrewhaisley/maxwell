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
 * MODULE/CLASS :  mx_area_pos
 *
 * AUTHOR : Andrew Haisley
 *
 *
 *
 * DESCRIPTION:
 *
 * Positions within an area
 *
 *
 *
 *
 *
 */

#include <math.h>
#include <mx.h>
#include <mx_area.h>
#include <mx_area_pos.h>
#include <mx_char_style.h>
#include <mx_doc_coord.h>
#include <mx_doc_rect.h>
#include <mx_geometry.h>
#include <mx_para_pos.h>
#include <mx_paragraph.h>
#include <mx_rtti.h>
#include <mx_spline.h>

mx_area_pos::mx_area_pos()
    : mx_position()
{
}
mx_area_pos::mx_area_pos(const mx_area_pos& p)
    : mx_position((const mx_position&)p)
{
}
mx_area_pos::~mx_area_pos() { };

bool mx_area_pos::is_same_cell_as(mx_area_pos& p)
{
    return FALSE;
}

bool mx_area_pos::is_same_row_as(mx_area_pos& p)
{
    return FALSE;
}

bool mx_area_pos::is_same_column_as(mx_area_pos& p)
{
    return FALSE;
}

mx_char_style* mx_area_pos::get_char_style(int& err, mx_area* a)
{
    return a->get_paragraph_style()->get_char_style();
}

mx_point mx_area_pos::position(int& err, mx_area* a, int line_offset)
{
    return position(err, a);
}
