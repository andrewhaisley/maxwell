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
 * MODULE/CLASS :  mx_wp_cursor
 *
 * AUTHOR : Andrew Haisley
 *
 *
 *
 * DESCRIPTION:
 *
 * Cursor for WP documents
 *
 *
 *
 */

#include "mx_aa.h"
#include "mx_wp_cursor.h"
#include <mx.h>
#include <mx_cw_t.h>
#include <mx_event.h>
#include <mx_mod_iter.h>
#include <mx_op_pos.h>
#include <mx_paragraph.h>
#include <mx_rtti.h>
#include <mx_sc_device.h>
#include <mx_ta_pos.h>
#include <mx_txt_pos.h>
#include <mx_undo.h>
#include <mx_wp_area_iter.h>
#include <mx_wp_doc.h>
#include <mx_wp_lay.h>
#include <mx_wp_ob_buff.h>
#include <mx_wp_pos.h>
#include <mx_wp_style_iter.h>
#include <mx_xframe.h>

void mx_wp_cursor::reformat_and_restore_position(int& err,
    mx_wp_doc_pos& start_pos,
    mx_wp_doc_pos& end_pos,
    bool sheets_deleted)
{
    mx_sheet_size sheet_size;
    bool end_equals_pos = FALSE;

    if (is_selection) {
        start.save(err, document, start.get_sheet());
        MX_ERROR_CHECK(err);

        end_equals_pos = (end == position);
        if (!end_equals_pos) {
            end.save(err, document, end.get_sheet());
            MX_ERROR_CHECK(err);
        }
    }

    position.save(err, document, position.get_sheet());
    MX_ERROR_CHECK(err);

    if (start_pos < end_pos) {
        document->reformat(err, start_pos, end_pos);
    } else {
        document->reformat(err, end_pos, start_pos);
    }
    MX_ERROR_CHECK(err);

    if (sheets_deleted) {
        recalculate_layout(err);
        MX_ERROR_CHECK(err);

        frame->resetSheetLayout(err);
        MX_ERROR_CHECK(err);
    } else {
        sheet_size = document->get_sheet_visible_size(err, position.get_sheet(), current_view_mode);
        MX_ERROR_CHECK(err);

        layout->changeSheets(err, position.get_sheet(), 1, &sheet_size);
        MX_ERROR_CHECK(err);
    }

    position.restore(err, document);
    MX_ERROR_CHECK(err);

    if (is_selection) {
        start.restore(err, document);
        MX_ERROR_CHECK(err);

        if (end_equals_pos) {
            end = position;
        } else {
            end.restore(err, document);
            MX_ERROR_CHECK(err);
        }
    }

abort:;
}

void mx_wp_cursor::update_selection(int& err, bool is_extend)
{
    if (is_extend) {
        // start selection if necessary
        if (!is_selection) {
            is_selection = TRUE;
            frame->externalSelectionAvailable();
            start = end = position;
        }
    } else {
        if (is_selection) {
            // redraw whole thing
            is_selection = FALSE;

            frame->refresh(err);
            MX_ERROR_CHECK(err);
        }
    }
abort:;
}

void mx_wp_cursor::update_caret(int& err)
{
    if (active_area->is_active()) {
        device->switchCaretOff();

        active_area->draw_active_handles(err);
        MX_ERROR_CHECK(err);
    } else {
        mx_doc_coord_t p1;
        float height, depth;

        active_area->undraw_active_handles(err);
        MX_ERROR_CHECK(err);

        position.make_valid(err, document);
        MX_ERROR_CHECK(err);

        p1 = position.position(err, document);
        MX_ERROR_CHECK(err);

        calculate_view_correction(err, p1.sheet_number);
        MX_ERROR_CHECK(err);

        p1.p -= view_correction;

        // height and pos of line depends on what sort of thing the cursor is
        // pointing at.
        get_caret_offsets(err, height, depth);
        MX_ERROR_CHECK(err);

        p1.p.y -= depth;

        device->caretPosition(err, p1, height - depth);
        MX_ERROR_CHECK(err);

        update_selection_contents_flags(err);
        MX_ERROR_CHECK(err);

        if (is_selection) {
            draw_selection(err);
            MX_ERROR_CHECK(err);

            is_selection = (position != start);
            if (is_selection) {
                device->switchCaretOff();
            } else {
                device->switchCaretOn();
            }
        } else {
            device->switchCaretOn();
        }
    }

abort:;
}

void mx_wp_cursor::draw_selection(int& err)
{
    bool b;

    // now calculate the redraw region

    // don't need to do anything since it is all up to date
    if (position == end)
        return;

    if (position == start) {
        // in this case there should be no selection, so clear the highlight
        // range
        is_selection = FALSE;
    }

    b = (position.get_sheet() != end.get_sheet());

    b = b || !position.is_paragraph(err);
    MX_ERROR_CHECK(err);

    b = b || !end.is_paragraph(err);
    MX_ERROR_CHECK(err);

    if (!b) {
        b = start.is_table(err);
        MX_ERROR_CHECK(err);

        if (b) {
            b = end.is_table(err);
            MX_ERROR_CHECK(err);

            b = b || position.is_table(err);
            MX_ERROR_CHECK(err);

            if (b) {
                b = start.is_part_of_same_text_container(err, end, document);
                MX_ERROR_CHECK(err);

                b = b || start.is_part_of_same_text_container(err, position, document);
                MX_ERROR_CHECK(err);
            }
        }
    }

    if (b) {
        // redraw whole lot if selection is not on same sheet or if the
        // selection is not in paragraphs, or if the selection is, or was
        // entirely within one table
        frame->refresh(err);
        MX_ERROR_CHECK(err);
    } else {
        float height, descender;
        mx_doc_coord_t tl, br;
        mx_paragraph *p1, *p2;
        mx_para_pos *pp1, *pp2;
        mx_table_area_pos* tabp = NULL;
        mx_table_area* taba = NULL;
        mx_text_area* ta = NULL;
        mx_point origin;
        mx_wp_doc_pos redraw_start = (position < end) ? position : end;
        mx_wp_doc_pos redraw_end = (position > end) ? position : end;

        p1 = redraw_start.get_para(err, document);
        MX_ERROR_CHECK(err);

        p2 = redraw_end.get_para(err, document);
        MX_ERROR_CHECK(err);

        pp1 = redraw_start.get_para_pos(err);
        MX_ERROR_CHECK(err);

        pp2 = redraw_end.get_para_pos(err);
        MX_ERROR_CHECK(err);

        // get a basic rectangle

        tl = redraw_start.position(err, document);
        tl.sheet_number = redraw_start.get_sheet();

        br = redraw_end.position(err, document);
        br.sheet_number = redraw_end.get_sheet();

        p1->highlight_height_for_line(err, pp1->line_index, height, descender);
        MX_ERROR_CHECK(err);
        tl.p.y -= height;

        p2->highlight_height_for_line(err, pp2->line_index, height, descender);
        MX_ERROR_CHECK(err);
        br.p.y -= descender;

        if (p1 != p2 || pp1->line_index != pp2->line_index) {
            // if we are not on the same line then set the left/right x
            // update regions to be the left/right edge of the sheet

            mx_sheet* s;

            tl.p.x = 0;
            tl.sheet_number = redraw_start.get_sheet();

            s = document->sheet(err, br.sheet_number);
            MX_ERROR_CHECK(err);

            br.p.x = s->get_width(err);
            MX_ERROR_CHECK(err);
            br.sheet_number = redraw_end.get_sheet();
        }

        // now see if either of the positions are in a table and adjust the
        // y positions appropriately

        tabp = (mx_table_area_pos*)redraw_start.get_area_pos(err);
        MX_ERROR_CHECK(err);

        if (tabp->is_a(mx_table_area_pos_class_e)) {
            taba = (mx_table_area*)redraw_start.get_area(err, document);
            MX_ERROR_CHECK(err);

            ta = taba->get_cell(err, tabp->row, tabp->column);
            MX_ERROR_CHECK(err);

            origin = taba->get_origin(err);
            MX_ERROR_CHECK(err);

            origin += ta->get_origin(err);
            MX_ERROR_CHECK(err);

            tl.p.y = origin.y;
        }

        tabp = (mx_table_area_pos*)redraw_end.get_area_pos(err);
        MX_ERROR_CHECK(err);

        if (tabp->is_a(mx_table_area_pos_class_e)) {
            taba = (mx_table_area*)redraw_end.get_area(err, document);
            MX_ERROR_CHECK(err);

            ta = taba->get_cell(err, tabp->row, tabp->column);
            MX_ERROR_CHECK(err);

            origin = taba->get_origin(err);
            MX_ERROR_CHECK(err);

            origin += ta->get_origin(err);
            MX_ERROR_CHECK(err);

            origin.y += ta->get_height(err);
            MX_ERROR_CHECK(err);

            br.p.y = origin.y;
        }

        tl.p -= view_correction;
        br.p -= view_correction;

        frame->refresh(err, &tl, &br);
        MX_ERROR_CHECK(err);
    }

    end = position;

abort:;
}

void mx_wp_cursor::get_caret_offsets(int& err, float& height, float& depth)
{
    mx_area_pos* ap;
    mx_sheet* s;
    mx_wp_sheet_pos* sp;
    mx_area* area;

    // what type of area is the position in?
    ap = position.get_sheet_pos()->get_area_pos(err);
    MX_ERROR_CHECK(err);

    s = document->sheet(err, position.get_sheet());
    MX_ERROR_CHECK(err);

    sp = position.get_sheet_pos();

    area = sp->area(err, s);
    MX_ERROR_CHECK(err);

    if (ap->rtti_class() == mx_text_area_pos_class_e) {
        get_caret_height_for_text_area(
            err,
            height,
            depth,
            (mx_text_area*)area,
            (mx_text_area_pos*)ap);
        MX_ERROR_CHECK(err);
    } else {
        if (ap->rtti_class() == mx_table_area_pos_class_e) {
            get_caret_height_for_table_area(
                err,
                height,
                depth,
                (mx_table_area*)area,
                (mx_table_area_pos*)ap);
            MX_ERROR_CHECK(err);
        } else {
            height = area->get_height(err);
            MX_ERROR_CHECK(err);

            depth = 0.0;
        }
    }

abort:;
}

void mx_wp_cursor::get_caret_height_for_text_area(
    int& err,
    float& height,
    float& depth,
    mx_text_area* area,
    mx_text_area_pos* ap)
{
    mx_paragraph* p;
    float f;

    p = area->get(err, ap->paragraph_index);
    MX_ERROR_CHECK(err);

    p->metrics_for_line(err, ap->para_pos.line_index, height, depth, f);
    MX_ERROR_CHECK(err);

abort:;
}

void mx_wp_cursor::get_caret_height_for_table_area(
    int& err,
    float& height,
    float& depth,
    mx_table_area* area,
    mx_table_area_pos* ap)
{
    mx_text_area* text_area;

    text_area = area->get_cell(err, ap->row, ap->column);
    MX_ERROR_CHECK(err);

    get_caret_height_for_text_area(err, height, depth, text_area, &(ap->text_pos));
    MX_ERROR_CHECK(err);

abort:;
}

void mx_wp_cursor::moveto_sheet(int& err, int sheet)
{
    is_selection = FALSE;

    position.moveto_start_sheet(err, document, sheet);
    MX_ERROR_CHECK(err);

    update_caret(err);
    MX_ERROR_CHECK(err);
abort:;
}

void mx_wp_cursor::make_visible(int& err)
{
    float caret_height, caret_depth;
    mx_doc_coord_t p1, p2;

    p1 = position.position(err, document);
    MX_ERROR_CHECK(err);

    calculate_view_correction(err, p1.sheet_number);
    MX_ERROR_CHECK(err);

    p1.p -= view_correction;
    p2 = p1;

    this->get_caret_offsets(err, caret_height, caret_depth);
    MX_ERROR_CHECK(err);

    p1.p.y -= caret_height;
    p2.p.y -= caret_depth;

    ((mx_xframe*)frame)->makeVisible(err, p1, p2, 10.0);
    MX_ERROR_CHECK(err);

abort:;
}

mx_area* mx_wp_cursor::current_area(int& err)
{
    return position.get_area(err, document);
}

mx_wp_doc_pos* mx_wp_cursor::selection_start()
{
    if (is_selection) {
        if (start < position) {
            return &start;
        } else {
            return &position;
        }
    } else {
        return &position;
    }
}

mx_wp_doc_pos* mx_wp_cursor::selection_end()
{
    if (is_selection) {
        if (start > position) {
            return &start;
        } else {
            return &position;
        }
    } else {
        return &position;
    }
}

bool mx_wp_cursor::selection_has_table()
{
    return table_in_selection;
}

bool mx_wp_cursor::selection_has_diagram()
{
    return diagram_in_selection;
}

bool mx_wp_cursor::selection_has_image()
{
    return image_in_selection;
}

bool mx_wp_cursor::selection_has_multi_column()
{
    return multi_column_in_selection;
}

bool mx_wp_cursor::selection_has_text()
{
    return text_in_selection;
}

bool mx_wp_cursor::selection_only_covers_text()
{
    return text_only_selection;
}

bool mx_wp_cursor::selection_only_covers_table()
{
    return table_only_selection;
}

mx_area* mx_wp_cursor::get_end_area(int& err)
{
    mx_area* res = NULL;

    res = selection_end()->get_area(err, document);
    MX_ERROR_CHECK(err);

abort:
    return res;
}

mx_area* mx_wp_cursor::get_start_area(int& err)
{
    mx_area* res = NULL;

    res = selection_start()->get_area(err, document);
    MX_ERROR_CHECK(err);

abort:
    return res;
}

mx_area_pos* mx_wp_cursor::get_start_area_pos(int& err)
{
    mx_area_pos* res = NULL;

    res = selection_start()->get_area_pos(err);
    MX_ERROR_CHECK(err);

abort:
    return res;
}

mx_area_pos* mx_wp_cursor::get_end_area_pos(int& err)
{
    mx_area_pos* res = NULL;

    res = selection_end()->get_area_pos(err);
    MX_ERROR_CHECK(err);

abort:
    return res;
}

void mx_wp_cursor::sort_range()
{
    if (is_selection) {
        if (start > position) {
            mx_wp_doc_pos temp;

            temp = position;
            position = start;
            start = temp;
        }
    }
}

void mx_wp_cursor::set_selection(int& err, mx_wp_doc_pos& s, mx_wp_doc_pos& e)
{
    start = end = s;
    position = e;

    is_selection = TRUE;

    frame->refresh(err);
    MX_ERROR_CHECK(err);

    frame->externalSelectionAvailable();

    update_caret(err);
    MX_ERROR_CHECK(err);

abort:;
}

void mx_wp_cursor::clear_selection(int& err)
{
    if (is_selection) {
        // redraw whole thing
        is_selection = FALSE;

        frame->refresh(err);
        MX_ERROR_CHECK(err);
    }

abort:;
}

void mx_wp_cursor::set_position(int& err, mx_wp_doc_pos& pos)
{
    clear_selection(err);
    MX_ERROR_CHECK(err);

    position = pos;

    update_caret(err);
    MX_ERROR_CHECK(err);

abort:;
}

void mx_wp_cursor::moveto_para_start(int& err)
{
    mx_area_pos* ap;
    mx_sheet* s;
    mx_area* area;
    mx_wp_sheet_pos* sp;

    sp = position.get_sheet_pos();

    s = document->sheet(err, position.get_sheet());
    MX_ERROR_CHECK(err);

    area = sp->area(err, s);
    MX_ERROR_CHECK(err);

    ap = position.get_sheet_pos()->get_area_pos(err);
    MX_ERROR_CHECK(err);

    if (ap->rtti_class() == mx_text_area_pos_class_e) {
        mx_text_area_pos* tap = (mx_text_area_pos*)ap;

        tap->moveto_para_start(err, (mx_text_area*)area);
        MX_ERROR_CHECK(err);
    } else {
        if (ap->rtti_class() == mx_table_area_pos_class_e) {
            mx_table_area_pos* tap = (mx_table_area_pos*)ap;

            tap->moveto_para_start(err, (mx_table_area*)area);
            MX_ERROR_CHECK(err);
        }
    }

abort:;
}

void mx_wp_cursor::moveto_para_end(int& err)
{
    mx_area_pos* ap;
    mx_sheet* s;
    mx_area* area;
    mx_wp_sheet_pos* sp;

    sp = position.get_sheet_pos();

    s = document->sheet(err, position.get_sheet());
    MX_ERROR_CHECK(err);

    area = sp->area(err, s);
    MX_ERROR_CHECK(err);

    ap = position.get_sheet_pos()->get_area_pos(err);
    MX_ERROR_CHECK(err);

    if (ap->rtti_class() == mx_text_area_pos_class_e) {
        mx_text_area_pos* tap = (mx_text_area_pos*)ap;

        tap->moveto_para_end(err, (mx_text_area*)area);
        MX_ERROR_CHECK(err);
    } else {
        if (ap->rtti_class() == mx_table_area_pos_class_e) {
            mx_table_area_pos* tap = (mx_table_area_pos*)ap;

            tap->moveto_para_end(err, (mx_table_area*)area);
            MX_ERROR_CHECK(err);
        }
    }

abort:;
}

mx_char_style* mx_wp_cursor::char_style_at_position(int& err)
{
    return document->get_char_style(err, position);
    MX_ERROR_CHECK(err);
abort:;
}
