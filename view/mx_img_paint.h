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
 * MODULE/CLASS : mx_image_area_painter
 *
 * AUTHOR : Andrew Haisley
 *
 *
 *
 * DESCRIPTION:
 *
 * A class to draw image areas onto devices
 *
 *
 *
 *
 *
 */
#ifndef MX_IMG_PAINTER_H
#define MX_IMG_PAINTER_H

#include <mx.h>
#include <mx_area_paint.h>
#include <mx_device.h>
#include <mx_doc_coord.h>
#include <mx_doc_rect.h>
#include <mx_image_area.h>
#include <mx_paintable.h>
#include <mx_painter.h>
#include <mx_spline.h>

class mx_image_area_painter : public mx_area_painter {
    MX_RTTI(mx_image_area_painter_class_e)

public:
    // these routines are needed by the document to perform drawing

    // use this to get a painter. There is in fact only one
    // instance in the system.....
    static mx_image_area_painter* painter();

    // draw a bit of the document
    virtual void draw(int& err, mx_doc_rect_t& r, mx_paintable& o);

    virtual ~mx_image_area_painter();

private:
    // the constructor is private to stop instances being created. use the
    // painter() routine to get an instance to use.
    mx_image_area_painter();

    void draw_to_screen(int& err, mx_doc_rect_t& r, mx_paintable& o);
    void draw_to_printer(int& err, mx_doc_rect_t& r, mx_paintable& o);

    void drawTiles(int& err,
        raster* iraster,
        mx_doc_rect_t& ir,
        double displayResolution,
        mx_point& topLeft,
        mx_irect& pixelRect);

    void registerColours(int& err,
        mx_image_area* ia,
        LUT_VALUE* lut,
        int* lutSize,
        int* ndisplayColours,
        RAST_DATA* data,
        RAST_PACK* pack,
        int* ncolours,
        mx_colour** colours,
        bool newRaster);

    void setForeAndBack(int& err,
        mx_colour& setColour1,
        mx_colour& setColour2);

    void drawPixels(int& err,
        raster* iraster,
        mx_doc_rect_t& ir,
        mx_point& displayResolution,
        mx_point& topLeft,
        mx_irect& pixelRect);

    static mx_image_area_painter* the_painter;

    static void init();

    void setup(int& err,
        raster* ras);

    MX_DEVICE_RASTER colourDeviceType;
    int maxSigbits;
    int unpackedBits;
    int numColours;
    int numPlanes;
    mx_colour* colours;
};

#endif
