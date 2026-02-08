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

#include <mx.h>
#include <mx_device.h>
#include <mx_frame.h>
#include <mx_ta_pos.h>
#include <mx_tab_paint.h>
#include <mx_table_area.h>
#include <mx_text_area.h>

mx_table_area_painter* mx_table_area_painter::the_painter = NULL;
bool mx_table_area_painter::guide_draw = TRUE;

mx_table_area_painter* mx_table_area_painter::painter()
{
    init();
    return the_painter;
}

void mx_table_area_painter::init()
{
    if (the_painter == NULL) {
        the_painter = new mx_table_area_painter();
    }
}

void mx_table_area_painter::draw(int& err, mx_doc_rect_t& r, mx_paintable& o)
{
    mx_doc_coord_t oldFalseOrigin;
    mx_table_area* a;

    oldFalseOrigin = device->getFrame()->getDocFalseOrigin(err);
    MX_ERROR_CHECK(err);

    a = (mx_table_area*)&o;

    a->cc_unserialise_attributes(err);
    MX_ERROR_CHECK(err);

    draw_cells(err, a, r);
    MX_ERROR_CHECK(err);

    device->getFrame()->setFalseOrigin(err, oldFalseOrigin);
    MX_ERROR_CHECK(err);

    if (guide_draw) {
        draw_skeleton(err, a, r);
        MX_ERROR_CHECK(err);
    }

abort:;
}

void mx_table_area_painter::draw_cells(int& err, mx_table_area* a, mx_doc_rect_t& r)
{
    mx_doc_coord_t oldFalseOrigin;
    mx_doc_coord_t newFalseOrigin;
    mx_text_area* ta;

    mx_doc_rect_t redraw_rect;

    int nr, nc, i, j;
    mx_rect bb;

    nc = a->get_num_columns(err);
    MX_ERROR_CHECK(err);

    nr = a->get_num_rows(err);
    MX_ERROR_CHECK(err);

    oldFalseOrigin = device->getFrame()->getDocFalseOrigin(err);
    MX_ERROR_CHECK(err);

    for (i = 0; i < nr; i++) {
        for (j = 0; j < nc; j++) {
            ta = a->get_cell(err, i, j);
            MX_ERROR_CHECK(err);

            bb = ta->bounding_box(err);
            MX_ERROR_CHECK(err);

            bb += ta->get_origin(err);
            MX_ERROR_CHECK(err);

            if (bb.intersects(r.r)) {
                newFalseOrigin = oldFalseOrigin;

                newFalseOrigin += ta->get_origin(err);
                MX_ERROR_CHECK(err);

                device->getFrame()->setFalseOrigin(err, newFalseOrigin);
                MX_ERROR_CHECK(err);

                redraw_rect = r;
                redraw_rect.r -= ta->get_origin(err);
                MX_ERROR_CHECK(err);

                ta->redraw(err, redraw_rect);
                MX_ERROR_CHECK(err);
            }
        }
    }
abort:;
}

void mx_table_area_painter::draw_skeleton(int& err, mx_table_area* a, mx_doc_rect_t& r)
{
    int nr, nc, i;

    mx_doc_coord_t p1, p2;
    mx_text_area *ta1 = nullptr, *ta2 = nullptr;

    mx_line_style ls(mx_dotted, 0.0, "grey30");

    if (draw_mode == mx_paint_wp_full_e) {
        return;
    }

    device->setDefaultLineStyle(err, ls);
    MX_ERROR_CHECK(err);

    nc = a->get_num_columns(err);
    MX_ERROR_CHECK(err);

    nr = a->get_num_rows(err);
    MX_ERROR_CHECK(err);

    for (i = 0; i < nc; i++) {
        ta1 = a->get_cell(err, 0, i);
        MX_ERROR_CHECK(err);

        ta2 = a->get_cell(err, nr - 1, i);
        MX_ERROR_CHECK(err);

        p1.p = ta1->get_origin(err);
        MX_ERROR_CHECK(err);

        p2.p = ta2->get_origin(err);
        MX_ERROR_CHECK(err);

        p2.p.y += ta2->get_height(err);
        MX_ERROR_CHECK(err);

        device->drawLine(err, p1, p2);
        MX_ERROR_CHECK(err);
    }

    p1.p.x = p2.p.x = ta1->get_origin(err).x + ta1->get_width(err);
    MX_ERROR_CHECK(err);

    device->drawLine(err, p1, p2);
    MX_ERROR_CHECK(err);

    for (i = 0; i < nr; i++) {
        ta1 = a->get_cell(err, i, 0);
        MX_ERROR_CHECK(err);

        ta2 = a->get_cell(err, i, nc - 1);
        MX_ERROR_CHECK(err);

        p1.p = ta1->get_origin(err);
        MX_ERROR_CHECK(err);

        p2.p = ta2->get_origin(err);
        MX_ERROR_CHECK(err);

        p2.p.x += ta2->get_width(err);
        MX_ERROR_CHECK(err);

        device->drawLine(err, p1, p2);
        MX_ERROR_CHECK(err);
    }

    p1.p.y = p2.p.y = ta1->get_origin(err).y + ta1->get_height(err);
    MX_ERROR_CHECK(err);

    device->drawLine(err, p1, p2);
    MX_ERROR_CHECK(err);

abort:;
}

mx_table_area_painter::mx_table_area_painter()
    : mx_area_painter()
{
}

mx_table_area_painter::~mx_table_area_painter()
{
}
void mx_table_area_painter::set_guide_draw(bool on)
{
    guide_draw = on;
}

void mx_table_area_painter::highlight_from_start_of_area(int& err, mx_area* a, mx_area_pos* end)
{
    mx_table_area_pos* tend = (mx_table_area_pos*)end;
    mx_table_area* ta = (mx_table_area*)a;
    int nc;

    nc = ta->get_num_columns(err);
    MX_ERROR_CHECK(err);

    highlight_cells(err, ta, 0, 0, tend->row, nc - 1);
    MX_ERROR_CHECK(err);
abort:;
}

void mx_table_area_painter::highlight_whole_area(int& err, mx_area* a)
{
    mx_table_area* ta = (mx_table_area*)a;
    int nr, nc;

    nr = ta->get_num_rows(err);
    MX_ERROR_CHECK(err);

    nc = ta->get_num_columns(err);
    MX_ERROR_CHECK(err);

    highlight_cells(err, ta, 0, 0, nr - 1, nc - 1);
    MX_ERROR_CHECK(err);
abort:;
}

void mx_table_area_painter::highlight_in_area(int& err, mx_area* a, mx_area_pos* start, mx_area_pos* end)
{
    mx_table_area_pos* tstart = (mx_table_area_pos*)start;
    mx_table_area_pos* tend = (mx_table_area_pos*)end;

    mx_doc_coord_t oldFalseOrigin;
    mx_doc_coord_t newFalseOrigin;

    mx_text_area* ta;

    if (tstart->is_same_cell_as(*tend)) {
        ta = ((mx_table_area*)a)->get_cell(err, tstart->row, tstart->column);
        MX_ERROR_CHECK(err);

        oldFalseOrigin = device->getFrame()->getDocFalseOrigin(err);
        MX_ERROR_CHECK(err);

        newFalseOrigin = oldFalseOrigin;

        newFalseOrigin += ta->get_origin(err);
        MX_ERROR_CHECK(err);

        device->getFrame()->setFalseOrigin(err, newFalseOrigin);
        MX_ERROR_CHECK(err);

        ta->highlight_in_area(err, &(tstart->text_pos), &(tend->text_pos));
        MX_ERROR_CHECK(err);

        device->getFrame()->setFalseOrigin(err, oldFalseOrigin);
        MX_ERROR_CHECK(err);
    } else {
        highlight_cells(err, (mx_table_area*)a,
            tstart->row, tstart->column,
            tend->row, tend->column);
        MX_ERROR_CHECK(err);
    }

abort:;
}

void mx_table_area_painter::highlight_to_end_of_area(int& err, mx_area* a, mx_area_pos* start)
{
    mx_table_area_pos* tstart = (mx_table_area_pos*)start;
    mx_table_area* ta = (mx_table_area*)a;

    int nr, nc;

    nr = ta->get_num_rows(err);
    MX_ERROR_CHECK(err);

    nc = ta->get_num_columns(err);
    MX_ERROR_CHECK(err);

    highlight_cells(err, ta, tstart->row, 0, nr - 1, nc - 1);
    MX_ERROR_CHECK(err);
abort:;
}

void mx_table_area_painter::highlight_cells(int& err, mx_table_area* a,
    uint32 from_row,
    uint32 from_column,
    uint32 to_row,
    uint32 to_column)
{
    mx_text_area* ta;
    mx_doc_coord_t tl, br;

    ta = a->get_cell(err, from_row, from_column);
    MX_ERROR_CHECK(err);

    tl.p = ta->get_origin(err);
    MX_ERROR_CHECK(err);

    ta = a->get_cell(err, to_row, to_column);
    MX_ERROR_CHECK(err);

    br.p = ta->get_origin(err);
    MX_ERROR_CHECK(err);

    br.p.x += ta->get_width(err);
    MX_ERROR_CHECK(err);

    br.p.y += ta->get_height(err);
    MX_ERROR_CHECK(err);

    xorRect(err, tl, br);
    MX_ERROR_CHECK(err);

abort:;
}
