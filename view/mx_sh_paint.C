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
 * MODULE/CLASS : mx_sheet_painter
 *
 * AUTHOR : Andrew Haisley
 *
 *
 *
 * DESCRIPTION:
 *
 * A class to draw sheets onto devices
 *
 *
 *
 *
 *
 */

#include <mx.h>
#include <mx_area.h>
#include <mx_device.h>
#include <mx_frame.h>
#include <mx_list_iter.h>
#include <mx_nlist.h>
#include <mx_sh_paint.h>
#include <mx_sheet.h>
#include <mx_text_area.h>
#include <mx_wp_doc.h>

mx_sheet_painter* mx_sheet_painter::the_painter = NULL;

mx_sheet_painter* mx_sheet_painter::painter()
{
    init();
    return the_painter;
}

void mx_sheet_painter::init()
{
    if (the_painter == NULL) {
        the_painter = new mx_sheet_painter();
    }
}

void mx_sheet_painter::draw(int& err, mx_doc_rect_t& r, mx_paintable& o)
{
    mx_sheet* s = (mx_sheet*)&o;
    mx_rect draw_rect = r.r;
    mx_point origin(0, 0), header_offset;

    mx_area* a;
    mx_rect rect;
    mx_doc_rect_t area_rect;

    float left, right, top, bottom, temp, sheet_height;

    mx_list_iterator* iter = NULL;

    mx_wp_document* doc = (mx_wp_document*)s->get_document(err);
    MX_ERROR_CHECK(err);

    sheet_height = s->get_height(err);
    MX_ERROR_CHECK(err);

    iter = new mx_list_iterator(*(s->get_area_index(err)));
    MX_ERROR_CHECK(err);

    figure_origin_offset(err, s);
    MX_ERROR_CHECK(err);

    draw_rect += origin_offset;

    old_false_origin = device->getFrame()->getDocFalseOrigin(err);
    MX_ERROR_CHECK(err);

    store_false_origin(err);
    MX_ERROR_CHECK(err);

    grid_paint.reset(err);
    MX_ERROR_CHECK(err);

    grid_paint.set_device(device);

    a = doc->header(err);
    MX_ERROR_CHECK(err);

    if (a != NULL) {
        temp = a->get_height(err);

        if (temp > (sheet_height * MX_WP_DOC_MAX_HEADER)) {
            header_offset.y = sheet_height * MX_WP_DOC_MAX_HEADER;
        } else {
            header_offset.y = a->get_height(err);
            MX_ERROR_CHECK(err);
        }
    }

    while (iter->more()) {
        a = (mx_area*)iter->data();

        // does the area intersect the redraw rect?
        rect = a->bounding_box(err) + a->get_origin(err);
        MX_ERROR_CHECK(err);

        if (rect.intersects(draw_rect)) {
            origin = a->get_origin(err);
            MX_ERROR_CHECK(err);

            origin -= origin_offset;

            if (draw_mode == mx_paint_wp_full_e) {
                origin += header_offset;
            }

            // set false origin on device
            new_false_origin = old_false_origin;
            new_false_origin += origin;

            device->getFrame()->setFalseOrigin(err, new_false_origin);
            MX_ERROR_CHECK(err);

            // area can now draw itself in a zero based coordinate system
            area_rect.sheet_number = r.sheet_number;
            area_rect.r = draw_rect;

            area_rect.r -= a->get_origin(err);
            MX_ERROR_CHECK(err);

            a->redraw(err, area_rect);
            MX_ERROR_CHECK(err);
        }
    }

    // finally, draw header and footer
    if (draw_mode == mx_paint_wp_full_e) {
        a = doc->header(err);
        MX_ERROR_CHECK(err);

        if (a != NULL) {
            s->get_margins(err, left, right, top, bottom);
            MX_ERROR_CHECK(err);

            temp = a->get_height(err);

            if (temp > (sheet_height * MX_WP_DOC_MAX_HEADER)) {
                origin.y = top - (temp - (sheet_height * MX_WP_DOC_MAX_HEADER));
                origin.x = left;
            } else {
                origin.y = top;
                origin.x = left;
            }

            // set false origin on device
            new_false_origin = old_false_origin;
            new_false_origin += origin;

            device->getFrame()->setFalseOrigin(err, new_false_origin);
            MX_ERROR_CHECK(err);

            a->redraw(err, r);
            MX_ERROR_CHECK(err);
        }

        a = doc->footer(err);
        MX_ERROR_CHECK(err);

        if (a != NULL) {
            s->get_margins(err, left, right, top, bottom);
            MX_ERROR_CHECK(err);

            origin.x = left;

            temp = a->get_height(err);

            if (temp > (sheet_height * MX_WP_DOC_MAX_HEADER)) {
                origin.y = s->get_height(err) - bottom;
                MX_ERROR_CHECK(err);

                origin.y -= sheet_height * MX_WP_DOC_MAX_HEADER;
            } else {
                origin.y = s->get_height(err) - bottom;
                MX_ERROR_CHECK(err);

                origin.y -= a->get_height(err);
                MX_ERROR_CHECK(err);
            }

            // set false origin on device
            new_false_origin = old_false_origin;
            new_false_origin += origin;

            device->getFrame()->setFalseOrigin(err, new_false_origin);
            MX_ERROR_CHECK(err);

            a->redraw(err, r);
            MX_ERROR_CHECK(err);
        }
    }

    // reset the false origin for the grid painter
    device->getFrame()->setFalseOrigin(err, old_false_origin);
    MX_ERROR_CHECK(err);

    // draw any grid lines - these are from zero origin as the false
    // origin is added to each as it is stored
    grid_paint.draw(err, *device);
    MX_ERROR_CHECK(err);

    draw_page_break(err, s);
    MX_ERROR_CHECK(err);

abort:;
    if (iter != NULL) {
        delete iter;
    }
}

void mx_sheet_painter::draw_page_break(int& err, mx_sheet* s)
{
    if (draw_mode != mx_paint_wp_full_e) {
        bool b;
        mx_doc_coord_t p1, p2;
        float left, right, top, bottom;
        static mx_line_style soft_line(mx_dotted, 0.0, "grey50");
        static mx_line_style hard_line(mx_solid, 0.6, "black");

        mx_area* a = s->get_area(err, 0);
        MX_ERROR_CHECK(err);

        s->get_margins(err, left, right, top, bottom);
        MX_ERROR_CHECK(err);

        p1.p.x = 0 - left;
        p2.p.x = s->get_width(err) - left;
        MX_ERROR_CHECK(err);

        p1.sheet_number = p2.sheet_number = 0;
        p1.p.y = p2.p.y = 0.0;

        b = a->has_page_break_before(err);
        MX_ERROR_CHECK(err);

        // first draw a hard page break above this sheet if necessary, and then
        // draw a soft one below.

        if (b) {
            device->drawLine(err, p1, p2, &hard_line);
            MX_ERROR_CHECK(err);
        }

        p1.p.y = p2.p.y = s->get_height(err, draw_mode);
        MX_ERROR_CHECK(err);

        device->drawLine(err, p1, p2, &soft_line);
        MX_ERROR_CHECK(err);
    }

abort:;
}

mx_sheet_painter::mx_sheet_painter()
    : mx_painter()
{
}

mx_sheet_painter::~mx_sheet_painter()
{
}

void mx_sheet_painter::highlight_from_start_of_sheet(int& err, mx_sheet* s, mx_wp_sheet_pos& end)
{
    int i;
    mx_area* area;

    mx_area_pos* eap;

    store_false_origin(err);
    MX_ERROR_CHECK(err);

    figure_origin_offset(err, s);
    MX_ERROR_CHECK(err);

    area = s->get_area(err, end.get_area_index());
    MX_ERROR_CHECK(err);

    eap = end.get_area_pos(err);
    MX_ERROR_CHECK(err);

    set_false_origin(err, area);
    MX_ERROR_CHECK(err);

    area->highlight_from_start_of_area(err, eap);
    MX_ERROR_CHECK(err);

    for (i = 0; i < end.get_area_index(); i++) {
        area = s->get_area(err, i);
        MX_ERROR_CHECK(err);

        set_false_origin(err, area);
        MX_ERROR_CHECK(err);

        area->highlight_whole_area(err);
        MX_ERROR_CHECK(err);
    }
abort:;
}

void mx_sheet_painter::highlight_whole_sheet(int& err, mx_sheet* s)
{
    mx_doc_coord_t bl, tr;

    tr.p.x = s->get_width(err, draw_mode);
    MX_ERROR_CHECK(err);

    tr.p.y = s->get_height(err, draw_mode);
    MX_ERROR_CHECK(err);

    xorRect(err, bl, tr);
    MX_ERROR_CHECK(err);

abort:;
}

void mx_sheet_painter::highlight_in_sheet(int& err, mx_sheet* s, mx_wp_sheet_pos& start, mx_wp_sheet_pos& end)
{
    int i;
    mx_area* area;

    mx_area_pos *sap, *eap;

    store_false_origin(err);
    MX_ERROR_CHECK(err);

    figure_origin_offset(err, s);
    MX_ERROR_CHECK(err);

    for (i = start.get_area_index(); i <= end.get_area_index(); i++) {
        area = s->get_area(err, i);
        MX_ERROR_CHECK(err);

        if (i == start.get_area_index()) {
            if (i == end.get_area_index()) {
                // highlight is within this area
                sap = start.get_area_pos(err);
                MX_ERROR_CHECK(err);

                eap = end.get_area_pos(err);
                MX_ERROR_CHECK(err);

                set_false_origin(err, area);
                MX_ERROR_CHECK(err);

                area->highlight_in_area(err, sap, eap);
                MX_ERROR_CHECK(err);
            } else {
                // starts in this area, ends in next
                sap = start.get_area_pos(err);
                MX_ERROR_CHECK(err);

                set_false_origin(err, area);
                MX_ERROR_CHECK(err);

                area->highlight_to_end_of_area(err, sap);
                MX_ERROR_CHECK(err);
            }
        } else {
            if (i > start.get_area_index()) {
                if (i == end.get_area_index()) {
                    // starts in previous area and ends in this
                    eap = end.get_area_pos(err);
                    MX_ERROR_CHECK(err);

                    set_false_origin(err, area);
                    MX_ERROR_CHECK(err);

                    area->highlight_from_start_of_area(err, eap);
                    MX_ERROR_CHECK(err);
                } else {
                    set_false_origin(err, area);
                    MX_ERROR_CHECK(err);

                    // covers whole area
                    area->highlight_whole_area(err);
                    MX_ERROR_CHECK(err);
                }
            }
        }
    }
abort:;
}

void mx_sheet_painter::highlight_to_end_of_sheet(int& err, mx_sheet* s, mx_wp_sheet_pos& start)
{
    int i;
    mx_area* area;
    mx_area_pos* sap;

    store_false_origin(err);
    MX_ERROR_CHECK(err);

    figure_origin_offset(err, s);
    MX_ERROR_CHECK(err);

    area = s->get_area(err, start.get_area_index());
    MX_ERROR_CHECK(err);

    sap = start.get_area_pos(err);
    MX_ERROR_CHECK(err);

    set_false_origin(err, area);
    MX_ERROR_CHECK(err);

    area->highlight_to_end_of_area(err, sap);
    MX_ERROR_CHECK(err);

    for (i = start.get_area_index() + 1; i < s->get_num_areas(); i++) {
        area = s->get_area(err, i);
        MX_ERROR_CHECK(err);

        set_false_origin(err, area);
        MX_ERROR_CHECK(err);

        area->highlight_whole_area(err);
        MX_ERROR_CHECK(err);
    }
abort:;
}

void mx_sheet_painter::figure_origin_offset(int& err, mx_sheet* s)
{
    float left, right, top, bottom;

    // in non-full page mode, need to remove sheet margins and
    // header height
    if (draw_mode != mx_paint_wp_full_e) {
        (void)(mx_wp_document*)s->get_document(err);
        MX_ERROR_CHECK(err);

        s->get_margins(err, left, right, top, bottom);
        MX_ERROR_CHECK(err);

        origin_offset.x = left;
        origin_offset.y = top;
    } else {
        origin_offset.x = 0;
        origin_offset.y = 0;
    }
abort:;
}

void mx_sheet_painter::store_false_origin(int& err)
{
    old_false_origin = device->getFrame()->getDocFalseOrigin(err);
    MX_ERROR_CHECK(err);
abort:;
}

void mx_sheet_painter::set_false_origin(int& err, mx_area* a)
{
    mx_point origin(0, 0);

    origin = a->get_origin(err);
    MX_ERROR_CHECK(err);

    origin -= origin_offset;

    // set false origin on device
    new_false_origin = old_false_origin;
    new_false_origin += origin;

    device->getFrame()->setFalseOrigin(err, new_false_origin);
    MX_ERROR_CHECK(err);

abort:;
}
