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
 * MODULE/CLASS : mx_table_area_painter
 *
 * AUTHOR : Andrew Haisley
 *
 *
 *
 * DESCRIPTION:
 *
 * A class to draw table areas onto devices
 *
 *
 *
 *
 *
 */
#ifndef MX_TAB_PAINTER_H
#define MX_TAB_PAINTER_H

#include <mx.h>
#include <mx_area_paint.h>
#include <mx_doc_coord.h>
#include <mx_doc_rect.h>
#include <mx_paintable.h>
#include <mx_painter.h>
#include <mx_spline.h>

class mx_device;
class mx_table_area;

class mx_table_area_painter : public mx_area_painter {
    MX_RTTI(mx_table_area_painter_class_e)

public:
    // these routines are needed by the document to perform drawing

    // use this to get a painter. There is in fact only one
    // instance in the system.....
    static mx_table_area_painter* painter();

    // draw a bit of the document
    virtual void draw(int& err, mx_doc_rect_t& r, mx_paintable& o);

    virtual ~mx_table_area_painter();

    // turn table guide line drawing on or off
    static void set_guide_draw(bool on);

    virtual void highlight_from_start_of_area(int& err, mx_area* a, mx_area_pos* end);
    virtual void highlight_whole_area(int& err, mx_area* s);
    virtual void highlight_in_area(int& err, mx_area* s, mx_area_pos* start, mx_area_pos* end);
    virtual void highlight_to_end_of_area(int& err, mx_area* s, mx_area_pos* start);

private:
    // the constructor is private to stop instances being created. use the
    // painter() routine to get an instance to use.
    mx_table_area_painter();

    static mx_table_area_painter* the_painter;

    static void init();

    void draw_skeleton(int& err, mx_table_area* a, mx_doc_rect_t& r);
    void draw_cells(int& err, mx_table_area* a, mx_doc_rect_t& r);

    static bool guide_draw;

    void highlight_cells(int& err, mx_table_area* a,
        uint32 from_row, uint32 from_column,
        uint32 to_row, uint32 to_column);
};

#endif
