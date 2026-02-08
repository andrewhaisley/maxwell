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
 *
 */

#include "mx_aa.h"
#include "mx_wp_cursor.h"
#include <mx.h>
#include <mx_cw_t.h>
#include <mx_event.h>
#include <mx_frame.h>
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

mx_wp_cursor::mx_wp_cursor()
{
    int err = MX_ERROR_OK;
    is_selection = FALSE;
    document = NULL;
    frame = NULL;
    layout = NULL;
    last_corrected_sheet_number = -1;
    table_in_selection = multi_column_in_selection = diagram_in_selection = image_in_selection = FALSE;
    current_view_mode = mx_paint_wp_normal_e;
    active_area = new mx_active_area(this);
    saved_x_pos = 0.0f;
    save_x_pos = TRUE;
    temp_cut_buffer = NULL;
    key_event_was_cursor_movement = FALSE;

    if (buffer == NULL) {
        if (buffer_doc == NULL) {
            buffer_doc = mx_db_client_open_temporary_wp_doc(err);
            MX_ERROR_CHECK(err);
        }

        buffer = new mx_wp_object_buffer(buffer_doc);
    }

    return;
abort:
    global_error_trace->print();
    MX_ERROR_CLEAR(err);
}

mx_wp_cursor::mx_wp_cursor(mx_wp_document* doc, mx_frame* f)
{
    int err = MX_ERROR_OK;

    is_selection = FALSE;
    position.moveto_start(err, doc);
    MX_ERROR_CLEAR(err);

    document = doc;
    frame = f;
    last_corrected_sheet_number = -1;
    table_in_selection = multi_column_in_selection = diagram_in_selection = image_in_selection = FALSE;
    current_view_mode = mx_paint_wp_normal_e;
    active_area = new mx_active_area(this);
    saved_x_pos = 0.0f;
    save_x_pos = TRUE;
    temp_cut_buffer = NULL;
    key_event_was_cursor_movement = FALSE;

    if (buffer == NULL) {
        if (buffer_doc == NULL) {
            buffer_doc = mx_db_client_open_temporary_wp_doc(err);
            MX_ERROR_CHECK(err);
        }

        buffer = new mx_wp_object_buffer(buffer_doc);
    }
    return;
abort:
    global_error_trace->print();
    MX_ERROR_CLEAR(err);
}

mx_wp_cursor::~mx_wp_cursor()
{
    int err = MX_ERROR_OK;

    delete active_area;

    if (temp_cut_buffer != NULL) {
        delete temp_cut_buffer;

        mx_db_client_close_wp_doc(err, temp_cut_document);
        MX_ERROR_CLEAR(err);
    }
}

void mx_wp_cursor::set_layout(mx_wp_layout* l)
{
    layout = l;
}

void mx_wp_cursor::set_document(mx_wp_document* doc)
{
    int err = MX_ERROR_OK;

    active_area->deactivate();

    // invalidate any view_correction
    last_corrected_sheet_number = -1;
    document = doc;
    position.moveto_start(err, doc);
    MX_ERROR_CLEAR(err);

    if (frame != NULL) {
        update_caret(err);
        MX_ERROR_CLEAR(err);

        device->switchCaretOn();
    }
}

void mx_wp_cursor::set_frame(mx_frame* f)
{
    active_area->deactivate();

    frame = f;
    device = (mx_screen_device*)(frame->getDevice());
    int err = MX_ERROR_OK;

    if (document != NULL) {
        update_caret(err);
        MX_ERROR_CLEAR(err);

        device->switchCaretOn();
    }
}

void mx_wp_cursor::set_view_mode(mx_painter_mode_t m)
{
    current_view_mode = m;
}

mx_wp_object_buffer* mx_wp_cursor::get_buffer()
{
    return buffer;
}

void mx_wp_cursor::set_ur_buffer(mx_undo_redo_buffer* buf)
{
    ur_buffer = buf;
}

mx_frame* mx_wp_cursor::get_frame()
{
    return frame;
}

mx_device* mx_wp_cursor::get_device()
{
    return device;
}

mx_paragraph_style* mx_wp_cursor::style_at_position(int& err)
{
    bool b;

    b = position.is_paragraph(err);
    MX_ERROR_CHECK(err);

    if (b) {
        mx_paragraph* p = position.get_para(err, document);
        MX_ERROR_CHECK(err);

        return p->get_base_paragraph_style();
    } else {
        mx_area* a = position.get_area(err, document);
        MX_ERROR_CHECK(err);

        return a->get_base_paragraph_style();
    }
abort:
    return NULL;
}

void mx_wp_cursor::calculate_view_correction(int& err, int sheet_number)
{
    mx_point p;

    if (sheet_number == last_corrected_sheet_number) {
        return;
    } else {
        last_corrected_sheet_number = sheet_number;
    }

    if (current_view_mode != mx_paint_wp_full_e) {
        mx_sheet* s;
        float left, right, top, bottom;

        s = document->sheet(err, sheet_number);
        MX_ERROR_CHECK(err);

        s->get_margins(err, left, right, top, bottom);
        MX_ERROR_CHECK(err);

        p.x += left;
        p.y += top;
    }

    view_correction = p;

abort:;
}

float mx_wp_cursor::get_sheet_width(int& err)
{
    mx_sheet* s;
    float res;
    float t, b, l, r;

    s = document->sheet(err, position.get_sheet());
    MX_ERROR_CHECK(err);

    res = s->get_width(err);
    MX_ERROR_CHECK(err);

    s->get_margins(err, l, r, t, b);
    MX_ERROR_CHECK(err);

    return res - (l + r);

abort:
    return 0.0;
}

float mx_wp_cursor::get_sheet_height(int& err)
{
    mx_sheet* s;
    float res;
    float t, b, l, r;

    s = document->sheet(err, position.get_sheet());
    MX_ERROR_CHECK(err);

    res = s->get_height(err);
    MX_ERROR_CHECK(err);

    s->get_margins(err, l, r, t, b);
    MX_ERROR_CHECK(err);

    return res - (t + b);

abort:
    return 0.0;
}

void mx_wp_cursor::sheet_size_changed(int& err)
{
    last_corrected_sheet_number = -1;

    calculate_view_correction(err, position.get_sheet());
    MX_ERROR_CHECK(err);

abort:;
}

void mx_wp_cursor::store_sheet_dimensions(int& err)
{
    mx_sheet* s;

    s = document->sheet(err, position.get_sheet());
    MX_ERROR_CHECK(err);

    cut_sheet_width = s->get_width(err);
    MX_ERROR_CHECK(err);

    cut_sheet_height = s->get_width(err);
    MX_ERROR_CHECK(err);

    s->get_margins(err,
        cut_sheet_left_margin,
        cut_sheet_right_margin,
        cut_sheet_top_margin,
        cut_sheet_bottom_margin);
    MX_ERROR_CHECK(err);

abort:;
}

const mx_point& mx_wp_cursor::get_view_correction()
{
    return view_correction;
}

void mx_wp_cursor::recalculate_layout(int& err)
{
    mx_sheet_size* sheets;
    int i, n;

    n = layout->getNumberSheets();

    layout->removeSheets(err, 0, n);
    MX_ERROR_CHECK(err);

    n = document->get_num_sheets(err);
    MX_ERROR_CHECK(err);

    sheets = new mx_sheet_size[n];

    for (i = 0; i < n; i++) {
        sheets[i] = document->get_sheet_visible_size(err, i, current_view_mode);
        MX_ERROR_CHECK(err);
    }

    layout->setSnapTolerance(0.00001);

    layout->addSheets(err, 0, n, sheets);
    MX_ERROR_CHECK(err);

    delete[] sheets;

abort:;
}

void mx_wp_cursor::repaginate(int& err)
{
    if (is_selection) {
        start.save(err, document);
        MX_ERROR_CHECK(err);
    }

    ur_buffer->mark_cant_undo(err);
    MX_ERROR_CHECK(err);

    position.save(err, document);
    MX_ERROR_CHECK(err);

    document->repaginate(err, 0);
    MX_ERROR_CHECK(err);

    recalculate_layout(err);
    MX_ERROR_CHECK(err);

    if (is_selection) {
        start.restore(err, document);
        MX_ERROR_CHECK(err);
    }

    position.restore(err, document);
    MX_ERROR_CHECK(err);

    frame->refresh(err);
    MX_ERROR_CHECK(err);

    update_caret(err);
    MX_ERROR_CHECK(err);
abort:;
}

mx_table_area* mx_wp_cursor::split_table(int& err)
{
    mx_table_area_pos* tap;
    mx_table_area *ta, *new_ta;
    int c;

    mx_text_area_pos tp;

    // what type of area is the position in?
    tap = (mx_table_area_pos*)position.get_area_pos(err);
    MX_ERROR_CHECK(err);

    if (tap->is_a(mx_table_area_pos_class_e)) {
        ta = (mx_table_area*)position.get_area(err, document);
        MX_ERROR_CHECK(err);

        new_ta = ta->normal_split(err, *tap);
        MX_ERROR_CHECK(err);

        tp = tap->text_pos;
        c = tap->column;

        position.next_area(err, document);
        MX_ERROR_CHECK(err);

        tap->row = 0;
        tap->column = c;
        tap->text_pos = tp;

        return new_ta;
    } else {
        MX_ERROR_THROW(err, MX_POSITION_RANGE_ERROR);
    }

abort:
    return NULL;
}

void mx_wp_cursor::scale_active_area(int& err,
    mx_area* active_area,
    const mx_point& ns)
{
    mx_point old_size, new_size = ns;

    old_size = active_area->bounding_box(err).size();
    MX_ERROR_CHECK(err);

    active_area->scale(err, new_size);
    MX_ERROR_CHECK(err);

    active_area->set_outline(err, new_size.x, new_size.y);
    MX_ERROR_CHECK(err);

    mx_ur_item* ur_item;
    ur_item = new mx_ur_item(mx_undo_area_scale_change_e, document,
        *get_position(), old_size, new_size);

    ur_buffer->insert(err, ur_item);
    MX_ERROR_CHECK(err);
abort:;
}

void mx_wp_cursor::crop_active_area(int& err,
    mx_area* active_area,
    const mx_rect& cr)
{
    mx_rect crop_rectangle = cr;
    mx_point old_size, new_size = crop_rectangle.size();

    old_size = active_area->bounding_box(err).size();
    MX_ERROR_CHECK(err);

    // to do crop, area must have a valid origin
    active_area->set_origin(err, mx_point(0, 0));
    MX_ERROR_CHECK(err);

    active_area->crop(err, cr);
    MX_ERROR_CHECK(err);

    active_area->set_outline(err, new_size.x, new_size.y);
    MX_ERROR_CHECK(err);

    mx_ur_item* ur_item;
    ur_item = new mx_ur_item(mx_undo_area_crop_change_e, document,
        *get_position(), old_size, cr);

    ur_buffer->insert(err, ur_item);
    MX_ERROR_CHECK(err);
abort:;
}
