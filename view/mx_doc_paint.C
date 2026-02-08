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
 * MODULE/CLASS : mx_document_painter
 *
 * AUTHOR : Andrew Haisley
 *
 *
 *
 * DESCRIPTION:
 *
 * A class to draw documents onto devices
 *
 *
 *
 *
 *
 */

#include <mx.h>
#include <mx_device.h>
#include <mx_doc.h>
#include <mx_doc_paint.h>
#include <mx_frame.h>
#include <mx_sheet.h>

mx_document_painter* mx_document_painter::the_painter = NULL;

mx_document_painter* mx_document_painter::painter()
{
    init();
    return the_painter;
}

void mx_document_painter::init()
{
    if (the_painter == NULL) {
        the_painter = new mx_document_painter();
    }
}

void mx_document_painter::draw(int& err, mx_doc_rect_t& r, mx_paintable& o)
{
    mx_document* doc = (mx_document*)&o;
    mx_doc_coord_t origin(0, 0, r.sheet_number);
    mx_sheet* sheet;

    device->getFrame()->setFalseOrigin(err, origin);
    MX_ERROR_CHECK(err);

    sheet = doc->sheet(err, r.sheet_number);
    MX_ERROR_CHECK(err);

    sheet->redraw(err, r);
    MX_ERROR_CHECK(err);

    if (draw_mode != mx_paint_wp_full_e) {
        draw_highlight(err, r.sheet_number, sheet);
        MX_ERROR_CHECK(err);
    }

    device->getFrame()->clearFalseOrigin();
abort:;
}

mx_document_painter::mx_document_painter()
    : mx_painter()
{
    is_highlight = FALSE;
}

mx_document_painter::~mx_document_painter()
{
}

void mx_document_painter::set_highlight_range(mx_wp_doc_pos& start, mx_wp_doc_pos& end)
{
    highlight_start = start;
    highlight_end = end;
    is_highlight = TRUE;
}

void mx_document_painter::clear_highlight()
{
    is_highlight = FALSE;
}

void mx_document_painter::draw_highlight(int& err, int sheet_num, mx_sheet* s)
{
    if (is_highlight) {
        if (highlight_start.get_sheet() < sheet_num) {
            if (highlight_end.get_sheet() < sheet_num) {
                // highlight is before this sheet
                return;
            } else {
                if (highlight_end.get_sheet() == sheet_num) {
                    // hightlight starts in previous sheet and extends into
                    // this one
                    s->highlight_from_start_of_sheet(err, *(highlight_end.get_sheet_pos()));
                    MX_ERROR_CHECK(err);
                } else {
                    // highlight covers whole sheet
                    s->highlight_whole_sheet(err);
                    MX_ERROR_CHECK(err);
                }
            }
        } else {
            if (highlight_start.get_sheet() == sheet_num) {
                if (highlight_end.get_sheet() == sheet_num) {
                    // hightlight is contained wholly within this sheet
                    s->highlight_in_sheet(
                        err,
                        *(highlight_start.get_sheet_pos()),
                        *(highlight_end.get_sheet_pos()));
                    MX_ERROR_CHECK(err);
                } else {
                    // highlight starts in this sheet and extends into
                    // the next one
                    s->highlight_to_end_of_sheet(err, *(highlight_start.get_sheet_pos()));
                    MX_ERROR_CHECK(err);
                }
            } else {
                // highlight is after this sheet
                return;
            }
        }
    }
abort:;
}
