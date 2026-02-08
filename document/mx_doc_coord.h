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
#ifndef MX_DOC_COORD_H
#define MX_DOC_COORD_H

#include <mx.h>
#include <mx_geometry.h>

class mx_doc_coord_t {
public:
    mx_doc_coord_t();
    mx_doc_coord_t(float x, float y);
    mx_doc_coord_t(float x, float y, int sheet);
    mx_doc_coord_t(const mx_point& p);
    mx_doc_coord_t(const mx_ipoint& p);

    mx_point p;
    int sheet_number; // not always needed

    void operator+=(const mx_point& p);

    friend bool operator==(const mx_doc_coord_t& c1, const mx_doc_coord_t& c2);
    friend bool operator!=(const mx_doc_coord_t& c1, const mx_doc_coord_t& c2);
};

#endif
