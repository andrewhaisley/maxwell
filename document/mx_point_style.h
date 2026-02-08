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
 * MODULE/CLASS : mx_point_style
 *
 * AUTHOR : Andrew Haisley
 *
 *
 *
 * DESCRIPTION:
 *
 * styles for point
 *
 *
 *
 */
#ifndef MX_POINT_STYLE_H
#define MX_POINT_STYLE_H

#include <mx.h>
#include <mx_bd_style.h>
#include <mx_db.h>
#include <mx_fill_style.h>
#include <mx_hatch_style.h>
#include <mx_rtti.h>

typedef enum {
    mx_point_style_none,
    mx_point_style_pixel,
    mx_point_style_circle,
    mx_point_style_box,
    mx_point_style_cross
} mx_point_style_t;

class mx_point_style : public mx_serialisable_object {
public:
    mx_point_style();
    ~mx_point_style();

    int32 point_id;
    mx_point box;
    mx_point_style_t type;

    friend bool operator==(const mx_point_style& s1, const mx_point_style& s2);
    friend bool operator!=(const mx_point_style& s1, const mx_point_style& s2);

    uint32 get_serialised_size(int& err);
    void serialise(int& err, uint8** buffer);
    void unserialise(int& err, uint8** buffer);

    void init();
};
#endif
