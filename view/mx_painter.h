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
 * MODULE/CLASS : mx_painter
 *
 * AUTHOR : Andrew Haisley
 *
 *
 *
 * DESCRIPTION:
 *
 * A class of things that paint other things onto devices
 *
 *
 *
 *
 *
 */
#ifndef MX_PAINTER_H
#define MX_PAINTER_H

#include <mx.h>
#include <mx_doc_coord.h>
#include <mx_doc_rect.h>
#include <mx_grid_paint.h>
#include <mx_paintable.h>
#include <mx_spline.h>

class mx_device;

// painters can paint documents in number of ways.....
typedef enum {
    mx_paint_wp_normal_e, // standard page layout style
    mx_paint_wp_codes_e, // standard page layout style + symbols
    mx_paint_wp_full_e // full page print - preview, page mode and print
} mx_painter_mode_t;

class mx_painter : public mx_rtti {
    MX_RTTI(mx_painter_class_e)

public:
    mx_painter();

    virtual ~mx_painter();

    // get a handle on the device
    static mx_device* get_device() { return device; };

    // set the output device
    static void set_device(mx_device& dev) { device = &dev; };

    // set the draw mode
    static void set_draw_mode(mx_painter_mode_t t) { draw_mode = t; };

    // draw a bit of the associated object - the class of that object
    // is entirely dependant on the derived class.
    virtual void draw(int& err, mx_doc_rect_t& r, mx_paintable& o) = 0;

protected:
    static mx_device* device;
    static mx_painter_mode_t draw_mode;

    static mx_grid_paint grid_paint;

    void xorRect(
        int& err,
        mx_doc_coord_t& tl,
        mx_doc_coord_t& br);
};

#endif
