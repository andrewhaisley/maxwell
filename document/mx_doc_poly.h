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
 * MODULE/CLASS :  mx_doc_poly
 *
 * AUTHOR : Andrew Haisley
 *
 *
 *
 * DESCRIPTION: polypoints with a sheet number
 *
 *
 *
 *
 *
 */
#ifndef MX_DOC_POLY_H
#define MX_DOC_POLY_H

#include <mx.h>
#include <mx_rtti.h>

#include <geometry.h>

class mx_doc_polypoint {
    // MX_RTTI(mx_doc_polypoint_class_e)
public:
    mx_doc_polypoint(int sheet_number, int num_points);
    mx_doc_polypoint();

    mx_polypoint pp;
    int sheet_number;

    mx_doc_polypoint& operator=(const mx_doc_polypoint& pp);
};

#endif
