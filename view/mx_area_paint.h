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
 * MODULE/CLASS :   mx_area_painter
 *
 * AUTHOR : Andrew Haisley
 *
 *
 *
 * DESCRIPTION:
 *
 * some things that all area painters might need to do
 *
 *
 *
 *
 *
 */
#ifndef MX_AREA_PAINT_H
#define MX_AREA_PAINT_H

#include <mx.h>
#include <mx_area.h>
#include <mx_painter.h>
#include <mx_rtti.h>

class mx_area_painter : public mx_painter {
public:
    void draw_area_border(int& err, mx_area* a);
    void draw_area_background(int& err, mx_area* a);
    void add_border_to_false_origin(int& err, mx_area* a);

    void unset_area_clip(int& err);

    void set_area_clip(int& err,
        mx_area* a,
        mx_point& topLeft,
        mx_point& displaySize,
        bool clipTL,
        bool clipBR);

    virtual void highlight_from_start_of_area(int& err, mx_area* a, mx_area_pos* end);
    virtual void highlight_whole_area(int& err, mx_area* s);
    virtual void highlight_in_area(int& err, mx_area* s, mx_area_pos* start, mx_area_pos* end);
    virtual void highlight_to_end_of_area(int& err, mx_area* s, mx_area_pos* start);

protected:
    mx_area_painter();
};

#endif
