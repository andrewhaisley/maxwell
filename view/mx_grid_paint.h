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
 * MODULE/CLASS :  mx_grid_paint
 *
 * AUTHOR : Andrew Haisley
 *
 *
 *
 * DESCRIPTION:
 *
 * A class for drawing grids of horizontal and vertical lines. This is harder
 * than it sounds due to having to get the corners right.
 *
 *
 *
 *
 *
 */
#ifndef MX_GRID_PAINT_H
#define MX_GRID_PAINT_H

#include <mx.h>
#include <mx_geometry.h>
#include <mx_rtti.h>
#include <mx_style.h>

class mx_point;
class mx_device;

typedef struct
{
    mx_point p1;
    mx_point p2;
    mx_line_style s;
} mx_grid_paint_line_t;

class mx_grid_paint //: public mx_rtti
{
    // MX_RTTI(mx_grid_paint_class_e)
public:
    mx_grid_paint();
    virtual ~mx_grid_paint();

    // set the device
    void set_device(mx_device* d) { device = d; };

    // empty the old contents out
    void reset(int& err);

    // add a line to be drawn - must be vertical or horizontal
    void add(int& err, mx_line_style& ls, mx_point& p1, mx_point& p2);

    void draw(int& err, mx_device& drawable);

private:
    void draw_chain(int& err, mx_device& drawable);
    void draw_line_list(int& err,
        mx_device& drawable,
        mx_list& lines);
    void find_tolerance(mx_grid_paint_line_t* t1,
        mx_grid_paint_line_t* t2,
        float& ht,
        float& vt);
    void fix_line_list(int& err, mx_list& lines);
    mx_grid_paint_line_t* get_joining_line(mx_grid_paint_line_t* t);
    mx_grid_paint_line_t* get_joining_line(mx_grid_paint_line_t* t1, mx_list& lines);
    bool points_match(mx_point& p1,
        mx_point& p2,
        float vertical_tolerance,
        float horizontal_tolerance);

    mx_list vertical_lines;
    mx_list horizontal_lines;

    void add_horizontal(int& err, mx_line_style& ls, mx_point& p1, mx_point& p2);
    void add_vertical(int& err, mx_line_style& ls, mx_point& p1, mx_point& p2);

    mx_device* device;
};

#endif
