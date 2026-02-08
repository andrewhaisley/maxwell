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
 * MODULE/CLASS :  mx_doc_coord_t
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

#include "mx_doc_coord.h"

bool operator==(const mx_doc_coord_t& c1, const mx_doc_coord_t& c2)
{
    return (c1.p == c2.p) && (c1.sheet_number == c2.sheet_number);
}

bool operator!=(const mx_doc_coord_t& c1, const mx_doc_coord_t& c2)
{
    return (c1.p != c2.p) || (c1.sheet_number != c2.sheet_number);
}

mx_doc_coord_t::mx_doc_coord_t(float x, float y, int sheet)
{
    p.x = x;
    p.y = y;
    sheet_number = sheet;
}

mx_doc_coord_t::mx_doc_coord_t(const mx_point& ip)
{
    p = ip;
    sheet_number = 0;
}

mx_doc_coord_t::mx_doc_coord_t(const mx_ipoint& ip)
{
    p = ip;
    sheet_number = 0;
}

mx_doc_coord_t::mx_doc_coord_t()
{
    p.x = 0;
    p.y = 0;
    sheet_number = 0;
}

mx_doc_coord_t::mx_doc_coord_t(float x, float y)
{
    p.x = x;
    p.y = y;
    sheet_number = 0;
}

void mx_doc_coord_t::operator+=(const mx_point& ip)
{
    p += ip;
}
