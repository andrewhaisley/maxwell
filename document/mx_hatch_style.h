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
#ifndef MX_HATCH_STYLE
#define MX_HATCH_STYLE

/*
 * MODULE : mx_hatch_style.h
 *
 * AUTHOR : David Miller
 *
 * This file
 *
 * DESCRIPTION:
 * Module mx_hatch_style.h
 *
 *
 * Revision 1.1  1996/10/06 13:15:45  andrew
 * Initial revision
 *
 *
 */

#include <mx_db.h>

#include <mx_colour.h>
#include <mx_doc_coord.h>
#include <mx_doc_rect.h>
#include <mx_geometry.h>
#include <mx_line_style.h>
#include <mx_spline.h>

class mx_hatch_style : public mx_serialisable_object {
public:
    mx_hatch_style();
    ~mx_hatch_style();

    // anchor for hatching
    mx_point anchor;

    // hatching
    float angle[2];
    float spacing[2];
    mx_line_style line_style[2];
    int32 hatch_id;

    uint32 get_serialised_size(int& err);
    void serialise(int& err, uint8** buffer);
    void unserialise(int& err, uint8** buffer);

    inline mx_point getAnchor() { return anchor; };

    void init();

    void getHatchValues(int ihatch,
        float& angle,
        float& spacing,
        int& lineStyleId,
        int& colourId);

    friend bool operator==(const mx_hatch_style& s1, const mx_hatch_style& s2);
    friend bool operator!=(const mx_hatch_style& s1, const mx_hatch_style& s2);
};

#endif
