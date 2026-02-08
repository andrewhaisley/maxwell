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
 * MODULE/CLASS : mx_diagram_area_painter
 *
 * AUTHOR : Andrew Haisley
 *
 *
 *
 * DESCRIPTION:
 *
 * A class to draw diagram areas
 *
 *
 *
 *
 *
 */

#include <mx.h>
#include <mx_device.h>
#include <mx_dg_area.h>
#include <mx_dg_paint.h>

mx_diagram_area_painter* mx_diagram_area_painter::the_painter = NULL;

mx_diagram_area_painter* mx_diagram_area_painter::painter()
{
    init();
    return the_painter;
}

void mx_diagram_area_painter::init()
{
    if (the_painter == NULL) {
        the_painter = new mx_diagram_area_painter();
    }
}

void mx_diagram_area_painter::draw(int& err, mx_doc_rect_t& r, mx_paintable& o)
{
    float width, height;

    mx_doc_coord_t p1, p2;
    mx_diagram_area* da = (mx_diagram_area*)(&o);

    da->draw(err, device, r);
    MX_ERROR_CHECK(err);

    width = da->get_width(err);
    MX_ERROR_CHECK(err);

    height = da->get_height(err);
    MX_ERROR_CHECK(err);

    p1.p.x = 0;
    p1.p.y = 0;

    p2.p.x = width;
    p2.p.y = 0;

    device->drawLine(err, p1, p2);
    MX_ERROR_CHECK(err);

    p1.p.x = width;
    p1.p.y = 0;

    p2.p.x = width;
    p2.p.y = height;

    device->drawLine(err, p1, p2);
    MX_ERROR_CHECK(err);

    p1.p.x = width;
    p1.p.y = height;

    p2.p.x = 0;
    p2.p.y = height;

    device->drawLine(err, p1, p2);
    MX_ERROR_CHECK(err);

    p1.p.x = 0;
    p1.p.y = height;

    p2.p.x = 0;
    p2.p.y = 0;

    device->drawLine(err, p1, p2);
    MX_ERROR_CHECK(err);

abort:;
}

mx_diagram_area_painter::mx_diagram_area_painter()
    : mx_area_painter()
{
}

mx_diagram_area_painter::~mx_diagram_area_painter()
{
}
