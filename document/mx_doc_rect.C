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
 *
 * AUTHOR : Andrew Haisley
 *
 *
 *
 * DESCRIPTION:
 *
 *
 *
 *
 *
 */
#include <mx.h>
#include <mx_rtti.h>

#include <mx_doc_coord.h>
#include <mx_doc_rect.h>
#include <mx_geometry.h>

mx_doc_rect_t::mx_doc_rect_t(const mx_doc_coord_t& tl, const mx_doc_coord_t& br)
{
    r = mx_rect(tl.p, br.p);

    sheet_number = tl.sheet_number;
}

mx_doc_rect_t::mx_doc_rect_t(const mx_rect& ir)
{
    r = ir;
    sheet_number = 0;
}

mx_doc_rect_t::mx_doc_rect_t()
{
    r = mx_rect(mx_point(0, 0), mx_point(0, 0));
    sheet_number = 0;
}
