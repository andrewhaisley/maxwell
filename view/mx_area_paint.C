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

#include <mx_area_paint.h>
#include <mx_device.h>
#include <mx_frame.h>
#include <mx_sc_device.h>

void mx_area_painter::draw_area_border(int& err, mx_area* a)
{
    mx_border_style* bs;

    mx_point p1, p2;
    float right, bottom;

    bs = a->get_area_style()->get_border_style();

    right = a->get_width(err);
    MX_ERROR_CHECK(err);

    bottom = a->get_height(err);
    MX_ERROR_CHECK(err);

    if (bs->top_style.line_type != mx_no_line) {
        p1.x = 0;
        p1.y = bs->top_style.width / 2;
        p2.x = right;
        p2.y = p1.y;

        grid_paint.add(err, bs->top_style, p1, p2);
        MX_ERROR_CHECK(err);
    }

    if (bs->right_style.line_type != mx_no_line) {
        p1.x = right - bs->right_style.width / 2;
        p1.y = 0;
        p2.x = p1.x;
        p2.y = bottom;

        grid_paint.add(err, bs->right_style, p1, p2);
        MX_ERROR_CHECK(err);
    }

    if (bs->bottom_style.line_type != mx_no_line) {
        p1.y = bottom - bs->bottom_style.width / 2;
        p1.x = right;
        p2.y = p1.y;
        p2.x = 0;

        grid_paint.add(err, bs->bottom_style, p1, p2);
        MX_ERROR_CHECK(err);
    }

    if (bs->left_style.line_type != mx_no_line) {
        p1.x = bs->left_style.width / 2;
        p1.y = bottom;
        p2.x = p1.x;
        p2.y = 0;

        grid_paint.add(err, bs->left_style, p1, p2);
        MX_ERROR_CHECK(err);
    }
abort:;
}

void mx_area_painter::draw_area_background(int& err, mx_area* a)
{
    mx_doc_coord_t tl(0, 0, 0);
    mx_doc_coord_t br(0, 0, 0);

    mx_border_style* style = a->get_paragraph_style()->get_border_style();

    if (style->fill_style.type != mx_fill_transparent) {
        br.p.x = a->get_width(err);
        MX_ERROR_CHECK(err);

        br.p.y = a->get_height(err);
        MX_ERROR_CHECK(err);

        device->fillRect(err, tl, br, defaultAngle, &(style->fill_style));
        MX_ERROR_CHECK(err);
    }
abort:;
}

void mx_area_painter::add_border_to_false_origin(int& err, mx_area* a)
{
    mx_doc_coord_t falseOrigin;
    mx_border_style* bs;

    bs = a->get_paragraph_style()->get_border_style();

    falseOrigin = device->getFrame()->getDocFalseOrigin(err);
    MX_ERROR_CHECK(err);

    if (bs->top_style.line_type != mx_no_line) {
        falseOrigin.p.y += bs->top_style.width;
    }
    if (bs->left_style.line_type != mx_no_line) {
        falseOrigin.p.x += bs->left_style.width;
    }
    falseOrigin.p.x += bs->distance_from_contents;
    falseOrigin.p.y += bs->distance_from_contents;

    device->getFrame()->setFalseOrigin(err, falseOrigin);
    MX_ERROR_CHECK(err);

abort:;
}

void mx_area_painter::set_area_clip(int& err,
    mx_area* a,
    mx_point& topLeft,
    mx_point& displaySize,
    bool clipTL,
    bool clipBR)
{
    mx_doc_coord_t ctopLeft;
    mx_doc_coord_t cbottomRight;
    double framePixelSize;
    mx_doc_rect_t r;
    mx_point origin;
    mx_point areaSize;

    areaSize.x = a->get_width(err);
    MX_ERROR_CHECK(err);

    areaSize.y = a->get_height(err);
    MX_ERROR_CHECK(err);

    if (topLeft.x > 0)
        ctopLeft.p.x = topLeft.x;
    if (topLeft.y > 0)
        ctopLeft.p.y = topLeft.y;

    if (topLeft.x + displaySize.x > areaSize.x) {
        cbottomRight.p.x = areaSize.x;
    } else {
        cbottomRight.p.x = topLeft.x + displaySize.x;
    }

    if (topLeft.y + displaySize.y > areaSize.y) {
        cbottomRight.p.y = areaSize.y;
    } else {
        cbottomRight.p.y = topLeft.y + displaySize.y;
    }

    // get the size of one display pixel
    framePixelSize = 1.0 / (device->frameResolution());

    // clip area in one display pixel to stop boundary
    // drawing - particularly for raster

    if (clipTL) {
        ctopLeft.p.x += framePixelSize;
        ctopLeft.p.y += framePixelSize;
    }

    if (clipBR) {
        cbottomRight.p.x -= framePixelSize;
        cbottomRight.p.y -= framePixelSize;
    }

    get_device()->pushClipRect(err, ctopLeft, cbottomRight,
        defaultAngle, TRUE);
    MX_ERROR_CHECK(err);

abort:;
}

void mx_area_painter::unset_area_clip(int& err)
{
    get_device()->popClipMask(err);
    MX_ERROR_CHECK(err);

abort:;
}

mx_area_painter::mx_area_painter()
    : mx_painter()
{
}

void mx_area_painter::highlight_from_start_of_area(int& err, mx_area* a, mx_area_pos* end)
{
}

void mx_area_painter::highlight_whole_area(int& err, mx_area* s)
{
}

void mx_area_painter::highlight_in_area(int& err, mx_area* s, mx_area_pos* start, mx_area_pos* end)
{
}

void mx_area_painter::highlight_to_end_of_area(int& err, mx_area* s, mx_area_pos* start)
{
}
