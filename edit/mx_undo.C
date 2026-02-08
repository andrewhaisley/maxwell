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
 * MODULE/CLASS : mx_undo
 *
 * AUTHOR : Andrew Haisley
 *
 *
 *
 * DESCRIPTION:
 *
 * Undo and redo support.
 *
 *
 *
 *
 */
#include <mx.h>
#include <mx_area_pos.h>
#include <mx_bd_style.h>
#include <mx_cw_t.h>
#include <mx_list.h>
#include <mx_list_iter.h>
#include <mx_para_style.h>
#include <mx_paragraph.h>
#include <mx_ta_pos.h>
#include <mx_txt_pos.h>
#include <mx_undo.h>
#include <mx_wp_doc.h>
#include <mx_wp_ob_buff.h>
#include <mx_wp_pos.h>

void mx_ur_item::init(mx_ur_item_type_t t)
{
    type = t;
    buffer_doc = NULL;
    buffer = NULL;
    para_list = NULL;
    style_list = NULL;
    border_styles = NULL;
    area_styles = NULL;
    complex_word_item = NULL;
    num_rows = 0;
    num_columns = 0;
    table = NULL;
    text_area = NULL;
    base_style_name = NULL;
    actual_style = NULL;
    character_style = NULL;
    remove_text_area_to_undo_insert = FALSE;
    area_size = NULL;
    crop_rect = NULL;
    num_chars_to_undo = 1;
    deleted_chars = NULL;
    offset_into_para = 0;
}

mx_ur_item::mx_ur_item(
    mx_ur_item_type_t t,
    mx_wp_document* doc,
    mx_wp_doc_pos& pos,
    mx_table_area* a)
{
    int err = MX_ERROR_OK;

    init(t);

    position = pos;

    position.save(err, doc);
    MX_ERROR_CHECK(err);

    table = new mx_table_area(*a);
    return;
abort:
    global_error_trace->print();
    MX_ERROR_CLEAR(err);
}

mx_ur_item::mx_ur_item(
    mx_ur_item_type_t t,
    mx_wp_document* doc,
    mx_paragraph_style* styles[],
    int n)
{
    init(t);

    style_list = new mx_list;

    for (int i = 0; i < n; i++) {
        style_list->append(new mx_paragraph_style(*(styles[i])));
    }
}

mx_ur_item::mx_ur_item(
    mx_ur_item_type_t t,
    mx_wp_document* doc,
    float width,
    float height,
    float top,
    float bottom,
    float left,
    float right,
    bool is_landscape,
    mx_wp_doc_pos& pos)
{
    int err = MX_ERROR_OK;

    init(t);

    mx_ur_item::width = width;
    mx_ur_item::height = height;
    mx_ur_item::top = top;
    mx_ur_item::bottom = bottom;
    mx_ur_item::left = left;
    mx_ur_item::right = right;
    mx_ur_item::is_landscape = is_landscape;

    position = pos;

    position.save(err, doc);
    MX_ERROR_CHECK(err);
    return;
abort:
    global_error_trace->print();
    MX_ERROR_CLEAR(err);
}

// typing a character of some kind
mx_ur_item::mx_ur_item(mx_ur_item_type_t t,
    mx_wp_doc_pos& pos)
{
    init(t);
    position = end_position = pos;
    num_chars_to_undo = 1;
}

mx_ur_item::mx_ur_item(
    int& err,
    mx_ur_item_type_t t,
    mx_wp_document* doc,
    mx_wp_doc_pos& pos,
    int num_rows,
    int num_columns,
    float column_width)
{
    mx_wp_sheet_pos* sp;

    init(t);

    sp = pos.get_sheet_pos();

    (void)doc->sheet(err, pos.get_sheet());
    MX_ERROR_CHECK(err);

    is_sheet_start = sp->is_start();
    position = pos;

    position.save(err, doc);
    MX_ERROR_CHECK(err);

    mx_ur_item::num_rows = num_rows;
    mx_ur_item::num_columns = num_columns;
    mx_ur_item::column_width = column_width;

    text_area = (mx_text_area*)position.get_area(err, doc);
    MX_ERROR_CHECK(err);
    MX_ERROR_ASSERT(err, text_area->is_a(mx_text_area_class_e));

    text_area = new mx_text_area(*text_area);

abort:;
}

// undo a delete or backspace keypress
mx_ur_item::mx_ur_item(mx_ur_item_type_t t,
    mx_cw_t* cwi,
    mx_char_style* cs,
    mx_wp_doc_pos& pos)
{
    init(t);
    character_style = cs;
    complex_word_item = cwi;
    position = end_position = pos;
    num_chars_to_undo = 1;
}

mx_ur_item::mx_ur_item(mx_ur_item_type_t t,
    mx_wp_document* doc,
    const char* base_name,
    const mx_paragraph_style& the_style,
    mx_wp_doc_pos& pos)
{
    int err = MX_ERROR_OK;

    init(t);

    position = pos;
    base_style_name = mx_string_copy(base_name);
    actual_style = new mx_paragraph_style(the_style);

    position.save(err, doc);
    MX_ERROR_CHECK(err);
    return;
abort:
    global_error_trace->print();
    MX_ERROR_CLEAR(err);
}

mx_ur_item::mx_ur_item(
    mx_ur_item_type_t t,
    mx_wp_document* doc,
    mx_wp_doc_pos& start,
    mx_wp_doc_pos& end,
    mx_list* styles,
    int rows,
    int columns)
{
    int err = MX_ERROR_OK;

    init(t);

    position = start;
    end_position = end;

    border_styles = styles;
    num_rows = rows;
    num_columns = columns;

    position.save(err, doc);
    MX_ERROR_CHECK(err);

    end_position.save(err, doc);
    MX_ERROR_CHECK(err);
    return;
abort:
    global_error_trace->print();
    MX_ERROR_CLEAR(err);
}

mx_ur_item::mx_ur_item(
    mx_ur_item_type_t t,
    mx_wp_document* doc,
    mx_wp_doc_pos& start,
    mx_wp_doc_pos& end,
    mx_list* styles,
    int dummy)
{
    int err = MX_ERROR_OK;

    init(t);

    position = start;
    end_position = end;

    border_styles = styles;

    position.save(err, doc);
    MX_ERROR_CHECK(err);

    end_position.save(err, doc);
    MX_ERROR_CHECK(err);
    return;
abort:
    global_error_trace->print();
    MX_ERROR_CLEAR(err);
}

mx_ur_item::mx_ur_item(
    mx_ur_item_type_t t,
    mx_wp_document* doc,
    mx_wp_object_buffer* buff,
    mx_wp_doc_pos& start,
    mx_wp_doc_pos& end)
{
    int err = MX_ERROR_OK;
    init(t);

    position = start;
    end_position = end;

    // copy the buffer
    buffer = new mx_wp_object_buffer(*buff);

    // make the buffer be associated with the buffer document.

    buffer_doc = mx_db_client_open_temporary_wp_doc(err);
    MX_ERROR_CHECK(err);

    buffer->set_document(err, buffer_doc);
    MX_ERROR_CHECK(err);

abort:;
}

mx_ur_item::mx_ur_item(
    mx_ur_item_type_t t,
    mx_wp_document* doc,
    mx_wp_doc_pos& start,
    mx_wp_doc_pos& end,
    mx_list* paras)
{
    int err = MX_ERROR_OK;

    init(t);

    position = start;
    end_position = end;

    if (t == mx_undo_area_style_change_e) {
        area_styles = paras;
    } else {
        para_list = paras;
    }

    position.save(err, doc);
    MX_ERROR_CHECK(err);

    end_position.save(err, doc);
    MX_ERROR_CHECK(err);
    return;
abort:
    global_error_trace->print();
    MX_ERROR_CLEAR(err);
}

mx_ur_item::mx_ur_item(
    mx_ur_item_type_t t,
    mx_wp_document* doc,
    mx_wp_doc_pos& start,
    mx_wp_doc_pos& end,
    bool need_to_delete_text_area_to_undo)
{
    int err = MX_ERROR_OK;

    init(t);

    remove_text_area_to_undo_insert = need_to_delete_text_area_to_undo;

    position = start;
    end_position = end;

    position.save(err, doc);
    MX_ERROR_CHECK(err);

    end_position.save(err, doc);
    MX_ERROR_CHECK(err);
    return;
abort:
    global_error_trace->print();
    MX_ERROR_CLEAR(err);
}

// scale an embedded area
mx_ur_item::mx_ur_item(mx_ur_item_type_t t,
    mx_wp_document* doc,
    const mx_wp_doc_pos& pos,
    const mx_point& old_size,
    const mx_point& new_size)
{
    int err = MX_ERROR_OK;

    init(t);

    area_size = new mx_point(old_size);
    crop_rect = new mx_rect(mx_point(0, 0), new_size);

    position = pos;

    position.save(err, doc);
    MX_ERROR_CHECK(err);

    return;
abort:
    global_error_trace->print();
    MX_ERROR_CLEAR(err);
}

// crop an embedded area
mx_ur_item::mx_ur_item(mx_ur_item_type_t t,
    mx_wp_document* doc,
    const mx_wp_doc_pos& pos,
    const mx_point& old_size,
    const mx_rect& new_crop_rect)
{
    int err = MX_ERROR_OK;

    init(t);

    area_size = new mx_point(old_size);
    crop_rect = new mx_rect(new_crop_rect);

    position = pos;

    position.save(err, doc);
    MX_ERROR_CHECK(err);

    return;
abort:
    global_error_trace->print();
    MX_ERROR_CLEAR(err);
}

mx_ur_item::~mx_ur_item()
{
    if (buffer != NULL) {
        delete buffer;
    }

    if (buffer_doc != NULL) {
        int err = MX_ERROR_OK;
        mx_db_client_close_wp_doc(err, buffer_doc);
        MX_ERROR_CLEAR(err);
    }

    if (para_list != NULL) {
        mx_list_iterator iter(*para_list);
        mx_paragraph* p;

        while (iter.more()) {
            p = (mx_paragraph*)iter.data();
            delete p;
        }

        delete para_list;
    }

    if (style_list != NULL) {
        mx_list_iterator iter(*style_list);
        mx_paragraph_style* s;

        while (iter.more()) {
            s = (mx_paragraph_style*)iter.data();
            delete s;
        }

        delete style_list;
    }

    if (area_styles != NULL) {
        mx_list_iterator iter(*area_styles);
        mx_paragraph_style* s;

        while (iter.more()) {
            s = (mx_paragraph_style*)iter.data();
            delete s;
        }

        delete area_styles;
    }

    if (border_styles != NULL) {
        mx_list_iterator iter(*border_styles);
        mx_border_style* s;

        while (iter.more()) {
            s = (mx_border_style*)iter.data();
            delete s;
        }

        delete border_styles;
    }

    if (complex_word_item != NULL) {
        delete complex_word_item;
    }

    if (table != NULL) {
        delete table;
    }

    if (text_area != NULL) {
        delete text_area;
    }

    if (base_style_name != NULL)
        delete[] base_style_name;
    if (actual_style != NULL)
        delete actual_style;
    if (character_style != NULL)
        delete character_style;

    if (area_size != NULL) {
        delete area_size;
    }

    if (crop_rect != NULL) {
        delete crop_rect;
    }
}

static void move_positions_for_reformat(int& err, mx_wp_document* doc,
    mx_wp_doc_pos& p1,
    mx_wp_doc_pos& p2)
{
    p1.moveto_start_sheet(err, doc);
    MX_ERROR_CHECK(err);

    p2.moveto_end_sheet(err, doc);
    MX_ERROR_CHECK(err);

abort:;
}

static void reformat_at_position(int& err, mx_wp_document* doc,
    const mx_wp_doc_pos& pos)
{
    mx_wp_doc_pos start = pos, end = pos;

    move_positions_for_reformat(err, doc, start, end);
    MX_ERROR_CHECK(err);

    doc->reformat(err, start, end);
    MX_ERROR_CHECK(err);
abort:;
}

void mx_ur_item::redo_typing(int& err, mx_wp_document* doc, mx_wp_doc_pos* pos)
{
    mx_wp_doc_pos st_pos, end_pos;
    bool need_to_delete_text_area_to_undo;
    position.restore(err, doc);
    MX_ERROR_CHECK(err);

    MX_ERROR_ASSERT(err, buffer != NULL && buffer_doc != NULL);

    st_pos = end_pos = position;

    buffer->insert(err, *doc, end_pos, need_to_delete_text_area_to_undo);
    MX_ERROR_CHECK(err);
    MX_ERROR_ASSERT(err, !need_to_delete_text_area_to_undo);

    st_pos.restore(err, doc);
    MX_ERROR_CHECK(err);

    move_positions_for_reformat(err, doc, st_pos, end_pos);
    MX_ERROR_CHECK(err);

    doc->reformat(err, st_pos, end_pos);
    MX_ERROR_CHECK(err);

    position.restore(err, doc);
    MX_ERROR_CHECK(err);

    *pos = position;
abort:;
}

void mx_ur_item::redo_cut(int& err, mx_wp_document* doc, mx_wp_doc_pos* pos)
{
    mx_wp_object_buffer buf(NULL);

    mx_wp_doc_pos p1;
    mx_wp_doc_pos p2;

    position.restore(err, doc);
    MX_ERROR_CHECK(err);

    end_position.restore(err, doc);
    MX_ERROR_CHECK(err);

    p1 = position;
    p2 = end_position;

    buf.remove(err, *doc, p1, p2);
    MX_ERROR_CHECK(err);

    move_positions_for_reformat(err, doc, p1, p2);
    MX_ERROR_CHECK(err);

    doc->reformat(err, p1, p2);
    MX_ERROR_CHECK(err);

    *pos = position;

    pos->restore(err, doc);
    MX_ERROR_CHECK(err);
abort:;
}

void mx_ur_item::redo_paste(int& err, mx_wp_document* doc, mx_wp_doc_pos* pos)
{
    mx_wp_doc_pos start_p, end_p;

    position.restore(err, doc);
    MX_ERROR_CHECK(err);

    start_p = end_p = position;

    buffer->insert(err, *doc, end_p, remove_text_area_to_undo_insert);
    MX_ERROR_CHECK(err);

    position.restore(err, doc);
    MX_ERROR_CHECK(err);

    end_position = end_p;
    end_position.save(err, doc);
    MX_ERROR_CHECK(err);

    start_p = position;
    end_p = end_position;

    move_positions_for_reformat(err, doc, start_p, end_p);
    MX_ERROR_CHECK(err);

    doc->reformat(err, start_p, end_p);
    MX_ERROR_CHECK(err);

    *pos = position;

    pos->restore(err, doc);
    MX_ERROR_CHECK(err);
abort:;
}

void mx_ur_item::redo_format(int& err, mx_wp_document* doc, mx_wp_doc_pos* pos)
{
    undo_format(err, doc, pos);
    MX_ERROR_CHECK(err);
abort:;
}

void mx_ur_item::redo_table_change(int& err, mx_wp_document* doc, mx_wp_doc_pos* pos)
{
    undo_table_change(err, doc, pos);
    MX_ERROR_CHECK(err);
abort:;
}

void mx_ur_item::redo_page_setup(int& err, mx_wp_document* doc, mx_wp_doc_pos* pos)
{
    undo_page_setup(err, doc, pos);
    MX_ERROR_CHECK(err);
abort:;
}

void mx_ur_item::undo_typing(int& err, mx_wp_document* doc, mx_wp_doc_pos* pos)
{
    mx_wp_doc_pos st_pos, end_pos;

    position.restore(err, doc);
    MX_ERROR_CHECK(err);

    st_pos = end_pos = position;

    for (int i = 0; i < num_chars_to_undo; i++) {
        end_pos.move_right_ignoring_false_breaks(err, doc);
        MX_ERROR_CHECK(err);
    }

    // create new buffers if they haven't been created before
    if (buffer_doc == NULL) {
        // create a new buffer document
        buffer_doc = mx_db_client_open_temporary_wp_doc(err);
        MX_ERROR_CHECK(err);

        // create a new buffer
        buffer = new mx_wp_object_buffer(buffer_doc);
    }

    // cut the characters we've just typed - p1 is saved during the cut, so we
    // can use it to restore after the reformat
    buffer->cut(err, *doc, st_pos, end_pos);
    MX_ERROR_CHECK(err);

    reformat_at_position(err, doc, end_pos);
    MX_ERROR_CHECK(err);

    st_pos.restore(err, doc);
    MX_ERROR_CHECK(err);

    *pos = st_pos;
abort:;
}

void mx_ur_item::undo_cut(int& err, mx_wp_document* doc, mx_wp_doc_pos* pos)
{
    mx_wp_doc_pos p1, p2;
    bool b;

    position.restore(err, doc);
    MX_ERROR_CHECK(err);

    end_position = position;

    buffer->insert(err, *doc, end_position, b);
    MX_ERROR_CHECK(err);

    MX_ERROR_ASSERT(err, !b);

    position.restore(err, doc);
    MX_ERROR_CHECK(err);

    end_position.save(err, doc);
    MX_ERROR_CHECK(err);

    p1 = position;
    p2 = end_position;

    move_positions_for_reformat(err, doc, p1, p2);
    MX_ERROR_CHECK(err);

    doc->reformat(err, p1, p2);
    MX_ERROR_CHECK(err);

    end_position.restore(err, doc);
    MX_ERROR_CHECK(err);

    *pos = end_position;
abort:;
}

void mx_ur_item::undo_paste(int& err, mx_wp_document* doc, mx_wp_doc_pos* pos)
{
    mx_wp_doc_pos p1;
    mx_wp_doc_pos p2;

    position.restore(err, doc);
    MX_ERROR_CHECK(err);

    end_position.restore(err, doc);
    MX_ERROR_CHECK(err);

    p1 = position;
    p2 = end_position;

    if (buffer_doc == NULL) {
        buffer_doc = mx_db_client_open_temporary_wp_doc(err);
        MX_ERROR_CHECK(err);

        buffer = new mx_wp_object_buffer(buffer_doc);
    }

    buffer->cut(err, *doc, p1, p2);
    MX_ERROR_CHECK(err);

    if (remove_text_area_to_undo_insert) {
        mx_sheet* s;
        mx_area* a;
        int area_num = p2.get_sheet_pos()->get_area_index();

        s = doc->sheet(err, p2.get_sheet());
        MX_ERROR_CHECK(err);

        a = s->remove_area(err, area_num);
        MX_ERROR_CHECK(err);

        delete a;

        p2.get_sheet_pos()->reset(err, s);
        MX_ERROR_CHECK(err);

        if (area_num > 0) {
            s->join_areas(err, area_num - 1);
            MX_ERROR_CHECK(err);
        }
    }

    position.restore(err, doc);
    MX_ERROR_CHECK(err);

    p1 = p2 = position;

    move_positions_for_reformat(err, doc, p1, p2);
    MX_ERROR_CHECK(err);

    doc->reformat(err, p1, p2);
    MX_ERROR_CHECK(err);

    *pos = position;

    pos->restore(err, doc);
    MX_ERROR_CHECK(err);
abort:;
}

void mx_ur_item::undo_named_style_change(int& err, mx_wp_document* doc, mx_wp_doc_pos* pos)
{
    mx_paragraph_style* styles[MX_DOCUMENT_MAX_STYLES];
    mx_paragraph_style* tmp;
    mx_list_iterator iter(*style_list);

    mx_list* save_styles = new mx_list();

    int n;

    pos->save(err, doc);
    MX_ERROR_CHECK(err);

    n = doc->get_num_styles(err);
    MX_ERROR_CHECK(err);

    // make a copy of the current style list
    for (int i = 0; i < n; i++) {
        tmp = doc->get_style(err, i);
        MX_ERROR_CHECK(err);

        save_styles->append(new mx_paragraph_style(*tmp));
    }

    n = 0;

    while (iter.more()) {
        styles[n++] = (mx_paragraph_style*)iter.data();
    }

    doc->set_style_list(err, styles, n);
    MX_ERROR_CHECK(err);

    iter.rewind();

    while (iter.more()) {
        delete (mx_paragraph_style*)iter.data();
    }

    delete style_list;
    style_list = save_styles;

    doc->reformat(err);
    MX_ERROR_CHECK(err);

    pos->restore(err, doc);
    MX_ERROR_CHECK(err);

abort:;
}

void mx_ur_item::redo_named_style_change(int& err, mx_wp_document* doc, mx_wp_doc_pos* pos)
{
    undo_named_style_change(err, doc, pos);
    MX_ERROR_CHECK(err);
abort:;
}

void mx_ur_item::undo_format(int& err, mx_wp_document* doc, mx_wp_doc_pos* end_pos)
{
    mx_paragraph* p;
    mx_wp_doc_pos pos = position;

    mx_area_pos* ap;
    mx_text_area* text_area;
    mx_table_area* table_area;

    int i, n = para_list->get_num_items();

    pos.restore(err, doc);
    MX_ERROR_CHECK(err);

    for (i = 0; i < n; i++) {
        p = (mx_paragraph*)para_list->get(err, i);
        MX_ERROR_CHECK(err);

        ap = pos.get_area_pos(err);
        MX_ERROR_CHECK(err);

        if (ap->rtti_class() == mx_text_area_pos_class_e) {
            text_area = (mx_text_area*)pos.get_area(err, doc);
            MX_ERROR_CHECK(err);

            p = replace_para(err, text_area, (mx_text_area_pos*)ap, p);
            MX_ERROR_CHECK(err);

            (void)para_list->overwrite(err, i, p);
            MX_ERROR_CHECK(err);
        } else {
            if (ap->rtti_class() == mx_table_area_pos_class_e) {
                table_area = (mx_table_area*)pos.get_area(err, doc);
                MX_ERROR_CHECK(err);

                p = replace_para(err, table_area, (mx_table_area_pos*)ap, p);
                MX_ERROR_CHECK(err);

                (void)para_list->overwrite(err, i, p);
                MX_ERROR_CHECK(err);
            }
        }

        if (i != n - 1) {
            // only move to next paragraph if its not the last one, since the
            // last one may be at the end of the document
            pos.next_para(err, doc);
            MX_ERROR_CHECK(err);
        }
    }

    doc->reformat(err, position, end_position);
    MX_ERROR_CHECK(err);

    *end_pos = end_position;

    end_pos->restore(err, doc);
    MX_ERROR_CHECK(err);

abort:;
}

mx_paragraph* mx_ur_item::replace_para(
    int& err,
    mx_table_area* area,
    mx_table_area_pos* pos,
    mx_paragraph* p)
{
    mx_text_area* text_area;
    mx_paragraph* res = nullptr;

    text_area = area->get_cell(err, pos->row, pos->column);
    MX_ERROR_CHECK(err);

    res = replace_para(err, text_area, &(pos->text_pos), p);
    MX_ERROR_CHECK(err);

abort:
    return res;
}

mx_paragraph* mx_ur_item::replace_para(
    int& err,
    mx_text_area* area,
    mx_text_area_pos* pos,
    mx_paragraph* p)
{
    mx_paragraph* res = NULL;

    res = area->swap(err, pos->paragraph_index, p);
    MX_ERROR_CHECK(err);

abort:
    return res;
}

void mx_ur_item::undo_table_change(int& err, mx_wp_document* doc, mx_wp_doc_pos* pos)
{
    mx_wp_sheet_pos* sp;
    mx_table_area* ta;
    mx_sheet* s;
    int i;

    position.restore(err, doc);
    MX_ERROR_CHECK(err);

    sp = position.get_sheet_pos();
    i = sp->get_area_index();

    s = doc->sheet(err, position.get_sheet());
    MX_ERROR_CHECK(err);

    ta = (mx_table_area*)s->remove_area(err, i);
    MX_ERROR_CHECK(err);

    s->add_area(err, table, i);
    MX_ERROR_CHECK(err);

    table = ta;

    reformat_at_position(err, doc, position);
    MX_ERROR_CHECK(err);

    position.restore(err, doc);
    MX_ERROR_CHECK(err);

    *pos = position;

abort:;
}

void mx_ur_item::undo_insert_table(int& err, mx_wp_document* doc, mx_wp_doc_pos* pos)
{
    mx_wp_sheet_pos* sp;
    mx_sheet* s;
    mx_area* a;
    mx_table_area* ta;
    int i, n, ta_num_rows;

    MX_ERROR_ASSERT(err, text_area != NULL);

    position.restore(err, doc);
    MX_ERROR_CHECK(err);

    sp = position.get_sheet_pos();

    i = sp->get_area_index();

    s = doc->sheet(err, position.get_sheet());
    MX_ERROR_CHECK(err);

    a = s->get_area(err, i);
    MX_ERROR_CHECK(err);

    // make sure the position is at a table area as it should be.
    MX_ERROR_ASSERT(err, a->is_a(mx_table_area_class_e));

    ta = (mx_table_area*)a;
    ta_num_rows = ta->get_num_rows(err);
    MX_ERROR_CHECK(err);

    if (ta_num_rows != this->num_rows) {
        // the table has been joined with another one due to a cut or a
        // paste. So split it up again.

        i = doc->split(err, position);
        MX_ERROR_CHECK(err);
    }

    // remove the area previous to the table area if the area is not at the
    // start of the sheet and if the previous area is not a table area
    if (i > 0) {
        a = s->get_area(err, i - 1);
        MX_ERROR_CHECK(err);

        if (!a->is_a(mx_table_area_class_e)) {
            i--;

            a = s->remove_area(err, i);
            MX_ERROR_CHECK(err);

            delete a;
        }
    }

    // remove the table area and the text area that follows
    for (n = 0; n < 2; n++) {
        a = s->remove_area(err, i);
        MX_ERROR_CHECK(err);

        delete a;
    }

    s->add_area(err, text_area, i);
    MX_ERROR_CHECK(err);

    text_area = NULL;

    if (i < s->get_num_areas() - 1) {
        s->join_areas(err, i);
        MX_ERROR_CHECK(err);
    }

    if (i > 0) {
        s->join_areas(err, i - 1);
        MX_ERROR_CHECK(err);
    }

    reformat_at_position(err, doc, position);
    MX_ERROR_CHECK(err);

    position.restore(err, doc);
    MX_ERROR_CHECK(err);

    *pos = position;
abort:;
}

void mx_ur_item::redo_insert_table(int& err, mx_wp_document* doc, mx_wp_doc_pos* pos)
{
    mx_sheet* s;
    mx_table_area* ta;

    mx_wp_doc_pos start, end;

    int i;

    position.restore(err, doc);
    MX_ERROR_CHECK(err);

    if (text_area != NULL) {
        delete text_area;
    }

    text_area = (mx_text_area*)position.get_area(err, doc);
    MX_ERROR_CHECK(err);
    MX_ERROR_ASSERT(err, text_area->is_a(mx_text_area_class_e));

    text_area = new mx_text_area(*text_area);

    i = doc->split(err, position);
    MX_ERROR_CHECK(err);

    s = doc->sheet(err, position.get_sheet());
    MX_ERROR_CHECK(err);

    ta = s->add_table_area(err, i);
    MX_ERROR_CHECK(err);

    ta->set_num_rows(err, num_rows);
    MX_ERROR_CHECK(err);

    ta->set_num_columns(err, num_columns);
    MX_ERROR_CHECK(err);

    for (i = 0; i < num_columns; i++) {
        ta->set_column_width(err, i, column_width);
        MX_ERROR_CHECK(err);
    }

    reformat_at_position(err, doc, position);
    MX_ERROR_CHECK(err);

    position.restore(err, doc);
    MX_ERROR_CHECK(err);

    *pos = position;

abort:;
}

void mx_ur_item::undo_page_setup(int& err, mx_wp_document* doc, mx_wp_doc_pos* pos)
{
    float old_width;
    float old_height;
    float old_top;
    float old_bottom;
    float old_left;
    float old_right;
    bool old_is_landscape;

    pos->save(err, doc);
    MX_ERROR_CHECK(err);

    doc->get_page_size(err, old_width, old_height, old_is_landscape);
    MX_ERROR_CHECK(err);

    doc->set_page_size(err, width, height, is_landscape);
    MX_ERROR_CHECK(err);

    doc->get_page_margins(err, old_left, old_right, old_top, old_bottom);
    MX_ERROR_CHECK(err);

    doc->set_page_margins(err, left, right, top, bottom);
    MX_ERROR_CHECK(err);

    width = old_width;
    height = old_height;
    top = old_top;
    bottom = old_bottom;
    left = old_left;
    right = old_right;
    is_landscape = old_is_landscape;

    doc->reformat(err);
    MX_ERROR_CHECK(err);

    pos->restore(err, doc);
    MX_ERROR_CHECK(err);

abort:;
}

void mx_ur_item::undo_format_borders(int& err, mx_wp_document* doc, mx_wp_doc_pos* pos)
{
    if (num_rows == 0) {
        // paragraph border undo
        undo_format_para_borders(err, doc, pos);
        MX_ERROR_CHECK(err);
    } else {
        // it's a table border undo
        undo_format_table_borders(err, doc, pos);
        MX_ERROR_CHECK(err);
    }

abort:;
}

void mx_ur_item::undo_area_style_change(int& err, mx_wp_document* doc, mx_wp_doc_pos* end_pos)
{
    mx_wp_doc_pos pos = position;
    mx_paragraph_style *temp, save, *para_style;

    mx_area* a;

    int i;

    for (i = 0; i < area_styles->get_num_items(); i++) {
        a = pos.get_area(err, doc);
        MX_ERROR_CHECK(err);

        para_style = a->get_paragraph_style();

        temp = (mx_paragraph_style*)area_styles->get(err, i);
        MX_ERROR_CHECK(err);

        save = *temp;
        *temp = *para_style;

        a->modify_paragraph_style(save);

        pos.next_area(err, doc);
        MX_ERROR_CHECK(err);
    }

    doc->reformat(err, position, end_position);
    MX_ERROR_CHECK(err);

    *end_pos = end_position;

abort:;
}

void mx_ur_item::redo_area_style_change(int& err, mx_wp_document* doc, mx_wp_doc_pos* pos)
{
    undo_area_style_change(err, doc, pos);
    MX_ERROR_CHECK(err);
abort:;
}

void mx_ur_item::undo_format_para_borders(int& err, mx_wp_document* doc, mx_wp_doc_pos* end_pos)
{
    mx_paragraph* p;
    mx_wp_doc_pos pos = position;

    mx_border_style border_style, *temp, *save_border;
    mx_paragraph_style para_style;

    int i;

    for (i = 0; i < border_styles->get_num_items(); i++) {
        p = pos.get_para(err, doc);
        MX_ERROR_CHECK(err);

        para_style = *(p->get_paragraph_style());
        border_style = *(para_style.get_border_style());

        temp = (mx_border_style*)border_styles->get(err, i);
        MX_ERROR_CHECK(err);

        para_style.set_border_style(*temp);

        p->modify_paragraph_style(para_style);

        save_border = new mx_border_style(border_style);

        (void)border_styles->overwrite(err, i, save_border);
        MX_ERROR_CHECK(err);

        delete temp;

        pos.next_para(err, doc);
        MX_ERROR_CHECK(err);
    }

    doc->reformat(err, position, end_position);
    MX_ERROR_CHECK(err);

    *end_pos = end_position;
abort:;
}

void mx_ur_item::undo_format_table_borders(
    int& err,
    mx_wp_document* doc,
    mx_wp_doc_pos* end_pos)
{
    mx_wp_doc_pos pos = position;

    mx_border_style border_style, *temp, *save_border;
    mx_paragraph_style para_style;

    mx_table_area_pos* tap1;
    mx_table_area_pos* tap2;
    mx_table_area* table;
    mx_text_area* text;

    int i = 0, row, column;

    table = (mx_table_area*)position.get_area(err, doc);
    MX_ERROR_CHECK(err);

    tap1 = (mx_table_area_pos*)position.get_area_pos(err);
    MX_ERROR_CHECK(err);

    tap2 = (mx_table_area_pos*)end_position.get_area_pos(err);
    MX_ERROR_CHECK(err);

    MX_ERROR_ASSERT(err, tap1->rtti_class() == mx_table_area_pos_class_e);
    MX_ERROR_ASSERT(err, tap2->rtti_class() == mx_table_area_pos_class_e);

    for (row = tap1->row; row <= tap2->row; row++) {
        for (column = tap1->column; column <= tap2->column; column++) {
            text = table->get_cell(err, row, column);
            MX_ERROR_CHECK(err);

            para_style = *(text->get_paragraph_style());
            border_style = *(para_style.get_border_style());

            temp = (mx_border_style*)border_styles->get(err, i);
            MX_ERROR_CHECK(err);

            para_style.set_border_style(*temp);

            text->modify_paragraph_style(para_style);

            save_border = new mx_border_style(border_style);

            (void)border_styles->overwrite(err, i, save_border);
            MX_ERROR_CHECK(err);

            i++;

            delete temp;
        }
    }

    doc->reformat(err, position, end_position);
    MX_ERROR_CHECK(err);

    *end_pos = end_position;
abort:;
}

static void insert_undo_cwi(int& err, mx_cw_t* complex_word_item,
    mx_paragraph* para, mx_para_pos* para_pos)
{
    if (complex_word_item->rtti_class() == mx_character_cw_t_class_e) {
        mx_character_cw_t* cwi = (mx_character_cw_t*)complex_word_item;

        switch (cwi->c) {
        case MX_CW_T_PARA_BREAK_CHAR:
            // these aren't used any more
            MX_ERROR_THROW(err, MX_ERROR_UNDEFINED);
            break;
        case MX_CW_T_LINE_BREAK_CHAR:
            para->insert_line_break(err, *para_pos);
            MX_ERROR_CHECK(err);
            break;
        case MX_CW_T_TAB_CHAR:
            para->insert_char(err, '\t', *para_pos);
            MX_ERROR_CHECK(err);
            break;
        case MX_CW_T_SPACE_CHAR:
            para->insert_char(err, ' ', *para_pos);
            MX_ERROR_CHECK(err);
            break;
        default:
            para->insert_char(err, cwi->c, *para_pos);
            MX_ERROR_CHECK(err);
            break;
        }
    } else {
        complex_word_item = complex_word_item->deep_copy();

        para->insert_cw_item(err, complex_word_item, *para_pos);
        MX_ERROR_CHECK(err);
    }
abort:;
}

void mx_ur_item::undo_delete(int& err, mx_wp_document* doc, mx_wp_doc_pos* pos)
{
    mx_cw_t *mod_on = NULL, *mod_off = NULL;
    mx_char_style *cs, *position_cs;
    mx_cw_t* cwi;
    mx_paragraph* para;
    mx_para_pos* para_pos;
    int i, n = deleted_chars->get_num_items();
    mx_wp_doc_pos doc_pos = position;

    doc_pos.restore(err, doc);
    MX_ERROR_CHECK(err);

    para = doc_pos.get_para(err, doc);
    MX_ERROR_CHECK(err);

    para_pos = doc_pos.get_para_pos(err);
    MX_ERROR_CHECK(err);

    para_pos->set_distance_from_start(err, para, offset_into_para);
    MX_ERROR_CHECK(err);

    for (i = 0; i < n; i += 2) {
        cs = (mx_char_style*)deleted_chars->get(err, i);
        MX_ERROR_CHECK(err);

        cwi = (mx_cw_t*)deleted_chars->get(err, i + 1);
        MX_ERROR_CHECK(err);

        insert_undo_cwi(err, cwi, para, para_pos);
        MX_ERROR_CHECK(err);

        mod_on = mod_off = NULL;
        if (cs != NULL) {
            MX_ERROR_ASSERT(err, cs->is_a(mx_char_style_class_e));

            position_cs = para->get_style(err, *para_pos);
            MX_ERROR_CHECK(err);

            if (*position_cs != *cs) {
                // need to insert character style mods around the character
                mx_char_style_mod new_cs_on(*position_cs, *cs);
                mx_char_style_mod new_cs_off(*cs, *position_cs);

                mod_on = new mx_style_mod_cw_t(new_cs_on);
                mod_off = new mx_style_mod_cw_t(new_cs_off);
            }
        }

        para_pos->right(err, para);
        MX_ERROR_CHECK(err);

        if (mod_off != NULL) {
            para->insert_cw_item(err, mod_off, *para_pos);
            MX_ERROR_CHECK(err);

            para_pos->left(err, para);
            MX_ERROR_CHECK(err);

            para->insert_cw_item(err, mod_on, *para_pos);
            MX_ERROR_CHECK(err);

            para_pos->right(err, para);
            MX_ERROR_CHECK(err);

            para->reformat_styles(err);
            MX_ERROR_CHECK(err);
        }
    }

    reformat_at_position(err, doc, doc_pos);
    MX_ERROR_CHECK(err);

    position.restore(err, doc);
    MX_ERROR_CHECK(err);

    *pos = position;
abort:;
}

void mx_ur_item::undo_delete_para_break(int& err, mx_wp_document* doc, mx_wp_doc_pos* pos)
{
    mx_paragraph* para;
    mx_paragraph_style* ps;

    ps = doc->get_named_style(err, base_style_name);
    MX_ERROR_CHECK(err);

    position.restore(err, doc);
    MX_ERROR_CHECK(err);

    doc->insert_para_break(err, position);
    MX_ERROR_CHECK(err);

    position.right(err, doc);
    MX_ERROR_CHECK(err);

    para = position.get_para(err, doc);
    MX_ERROR_CHECK(err);

    para->set_paragraph_style(ps);
    para->modify_paragraph_style(*actual_style);

    delete[] base_style_name;
    delete actual_style;

    base_style_name = NULL;
    actual_style = NULL;

    position.save(err, doc);
    MX_ERROR_CHECK(err);

    reformat_at_position(err, doc, position);
    MX_ERROR_CHECK(err);

    position.restore(err, doc);
    MX_ERROR_CHECK(err);

    *pos = position;
abort:;
}

static mx_area* get_embedded_area(int& err, mx_wp_document* doc, mx_wp_doc_pos& pos)
{
    mx_complex_word* cw;
    mx_cw_t* cw_item;
    mx_para_pos* pp;

    pp = pos.get_para_pos(err);
    MX_ERROR_CHECK(err);

    cw = (mx_complex_word*)doc->get_word(err, pos);
    MX_ERROR_CHECK(err);
    MX_ERROR_ASSERT(err, cw->is_a(mx_complex_word_class_e));

    cw_item = cw->get_item(err, pp->letter_index);
    MX_ERROR_CHECK(err);
    MX_ERROR_ASSERT(err, cw_item->is_a(mx_area_cw_t_class_e));

    return ((mx_area_cw_t*)cw_item)->a;
abort:
    return NULL;
}

static void invert_crop_rectangle(mx_point& as, mx_rect& cr)
{
    mx_point nas = cr.size();
    mx_rect ncr(mx_point(0, 0), as);
    ncr -= cr.topLeft();

    as = nas;
    cr = ncr;
}

void mx_ur_item::undo_scale(int& err, mx_wp_document* doc, mx_wp_doc_pos* pos)
{
    mx_area* area;

    MX_ERROR_ASSERT(err, area_size != NULL);
    MX_ERROR_ASSERT(err, crop_rect != NULL);

    position.restore(err, doc);
    MX_ERROR_CHECK(err);

    area = get_embedded_area(err, doc, position);
    MX_ERROR_CHECK(err);

    area->scale(err, *area_size);
    MX_ERROR_CHECK(err);

    area->set_outline(err, area_size->x, area_size->y);
    MX_ERROR_CHECK(err);

    reformat_at_position(err, doc, position);
    MX_ERROR_CHECK(err);

    position.restore(err, doc);
    MX_ERROR_CHECK(err);

    invert_crop_rectangle(*area_size, *crop_rect);
    *pos = position;
abort:;
}

void mx_ur_item::undo_crop(int& err, mx_wp_document* doc, mx_wp_doc_pos* pos)
{
    mx_area* area;
    mx_point crop_rect_size;

    MX_ERROR_ASSERT(err, area_size != NULL);
    MX_ERROR_ASSERT(err, crop_rect != NULL);

    // swap the crop rectangle to undo
    invert_crop_rectangle(*area_size, *crop_rect);

    position.restore(err, doc);
    MX_ERROR_CHECK(err);

    area = get_embedded_area(err, doc, position);
    MX_ERROR_CHECK(err);

    area->set_origin(err, mx_point(0, 0));
    MX_ERROR_CHECK(err);

    area->crop(err, *crop_rect);
    MX_ERROR_CHECK(err);

    crop_rect_size = crop_rect->size();

    area->set_outline(err, crop_rect_size.x, crop_rect_size.y);
    MX_ERROR_CHECK(err);

    reformat_at_position(err, doc, position);
    MX_ERROR_CHECK(err);

    position.restore(err, doc);
    MX_ERROR_CHECK(err);

    *pos = position;
abort:;
}

void mx_ur_item::redo_format_borders(int& err, mx_wp_document* doc, mx_wp_doc_pos* pos)
{
    undo_format_borders(err, doc, pos);
    MX_ERROR_CHECK(err);
abort:;
}

void mx_ur_item::redo_delete(int& err, mx_wp_document* doc, mx_wp_doc_pos* pos)
{
    mx_cw_t* cwi;
    mx_paragraph* para;
    mx_para_pos* para_pos;
    int i;
    mx_wp_doc_pos doc_pos;

    position.restore(err, doc);
    MX_ERROR_CHECK(err);
    doc_pos = position;

    para = doc_pos.get_para(err, doc);
    MX_ERROR_CHECK(err);

    para_pos = doc_pos.get_para_pos(err);
    MX_ERROR_CHECK(err);

    para_pos->set_distance_from_start(err, para, offset_into_para);
    MX_ERROR_CHECK(err);

    for (i = 0; i < num_chars_to_undo; i++) {
        para_pos->right(err, para);
        MX_ERROR_CHECK(err);
    }

    for (i = 0; i < num_chars_to_undo; i++) {
        cwi = para->delete_to_left(err, para_pos);
        MX_ERROR_CHECK(err);

        para->reformat_styles(err);
        MX_ERROR_CHECK(err);

        if (cwi != NULL) {
            delete cwi;
        }
    }

    reformat_at_position(err, doc, doc_pos);
    MX_ERROR_CHECK(err);

    position.restore(err, doc);
    MX_ERROR_CHECK(err);

    *pos = position;
abort:;
}

void mx_ur_item::redo_delete_para_break(int& err, mx_wp_document* doc, mx_wp_doc_pos* pos)
{
    bool deleted_para;
    mx_paragraph* para;
    mx_cw_t* cwi;

    position.restore(err, doc);
    MX_ERROR_CHECK(err);

    *pos = position;

    para = position.get_para(err, doc);
    MX_ERROR_CHECK(err);

    base_style_name = mx_string_copy(para->get_base_paragraph_style()->get_name());
    actual_style = new mx_paragraph_style(*para->get_paragraph_style());

    cwi = doc->delete_to_left(err, pos, deleted_para);
    MX_ERROR_CHECK(err);
    delete cwi;
    MX_ERROR_ASSERT(err, deleted_para);

    position = *pos;

    reformat_at_position(err, doc, position);
    MX_ERROR_CHECK(err);

    position.save(err, doc);
    MX_ERROR_CHECK(err);
abort:;
}

void mx_ur_item::redo_scale(int& err, mx_wp_document* doc, mx_wp_doc_pos* pos)
{
    undo_scale(err, doc, pos);
    MX_ERROR_CHECK(err);
abort:;
}

void mx_ur_item::redo_crop(int& err, mx_wp_document* doc, mx_wp_doc_pos* pos)
{
    undo_crop(err, doc, pos);
    MX_ERROR_CHECK(err);
abort:;
}

void mx_ur_item::undo(int& err, mx_wp_document* doc, mx_wp_doc_pos* pos)
{
    switch (type) {
    case mx_undo_typing_e:
        undo_typing(err, doc, pos);
        MX_ERROR_CHECK(err);
        break;

    case mx_undo_cut_e:
        undo_cut(err, doc, pos);
        MX_ERROR_CHECK(err);
        break;

    case mx_undo_paste_e:
        undo_paste(err, doc, pos);
        MX_ERROR_CHECK(err);
        break;

    case mx_undo_format_e:
        undo_format(err, doc, pos);
        MX_ERROR_CHECK(err);
        break;

    case mx_undo_table_change_e:
        undo_table_change(err, doc, pos);
        MX_ERROR_CHECK(err);
        break;

    case mx_undo_page_setup_e:
        undo_page_setup(err, doc, pos);
        MX_ERROR_CHECK(err);
        break;

    case mx_undo_format_borders_e:
        undo_format_borders(err, doc, pos);
        MX_ERROR_CHECK(err);
        break;

    case mx_undo_delete_e:
        undo_delete(err, doc, pos);
        MX_ERROR_CHECK(err);
        break;

    case mx_undo_delete_para_break_e:
        undo_delete_para_break(err, doc, pos);
        MX_ERROR_CHECK(err);
        break;

    case mx_undo_named_style_change_e:
        undo_named_style_change(err, doc, pos);
        MX_ERROR_CHECK(err);
        break;

    case mx_undo_insert_table_e:
        undo_insert_table(err, doc, pos);
        MX_ERROR_CHECK(err);
        break;

    case mx_undo_area_style_change_e:
        undo_area_style_change(err, doc, pos);
        MX_ERROR_CHECK(err);
        break;

    case mx_undo_area_scale_change_e:
        undo_scale(err, doc, pos);
        MX_ERROR_CHECK(err);
        break;

    case mx_undo_area_crop_change_e:
        undo_crop(err, doc, pos);
        MX_ERROR_CHECK(err);
        break;

    default:
        MX_ERROR_THROW(err, MX_ERROR_UNDEFINED);
        break;
    }
abort:;
}

void mx_ur_item::redo(int& err, mx_wp_document* doc, mx_wp_doc_pos* pos)
{
    switch (type) {
    case mx_undo_typing_e:
        redo_typing(err, doc, pos);
        MX_ERROR_CHECK(err);
        break;

    case mx_undo_cut_e:
        redo_cut(err, doc, pos);
        MX_ERROR_CHECK(err);
        break;

    case mx_undo_paste_e:
        redo_paste(err, doc, pos);
        MX_ERROR_CHECK(err);
        break;

    case mx_undo_format_e:
        redo_format(err, doc, pos);
        MX_ERROR_CHECK(err);
        break;

    case mx_undo_format_borders_e:
        redo_format_borders(err, doc, pos);
        MX_ERROR_CHECK(err);
        break;

    case mx_undo_table_change_e:
        redo_table_change(err, doc, pos);
        MX_ERROR_CHECK(err);
        break;

    case mx_undo_page_setup_e:
        redo_page_setup(err, doc, pos);
        MX_ERROR_CHECK(err);
        break;

    case mx_undo_delete_e:
        redo_delete(err, doc, pos);
        MX_ERROR_CHECK(err);
        break;

    case mx_undo_delete_para_break_e:
        redo_delete_para_break(err, doc, pos);
        MX_ERROR_CHECK(err);
        break;

    case mx_undo_named_style_change_e:
        redo_named_style_change(err, doc, pos);
        MX_ERROR_CHECK(err);
        break;

    case mx_undo_insert_table_e:
        redo_insert_table(err, doc, pos);
        MX_ERROR_CHECK(err);
        break;

    case mx_undo_area_style_change_e:
        redo_area_style_change(err, doc, pos);
        MX_ERROR_CHECK(err);
        break;

    case mx_undo_area_scale_change_e:
        redo_scale(err, doc, pos);
        MX_ERROR_CHECK(err);
        break;

    case mx_undo_area_crop_change_e:
        redo_crop(err, doc, pos);
        MX_ERROR_CHECK(err);
        break;

    default:
        MX_ERROR_THROW(err, MX_ERROR_UNDEFINED);
        break;
    }
abort:;
}

void mx_ur_item::setup_first_item(int& err, mx_wp_document* doc)
{
    switch (type) {
    case mx_undo_typing_e:
        setup_first_typing_item(err, doc);
        MX_ERROR_CHECK(err);
        break;
    case mx_undo_delete_e:
        setup_first_delete_item(err, doc);
        MX_ERROR_CHECK(err);
        break;
    default:
        break;
    }
abort:;
}

void mx_ur_item::setup_first_typing_item(int& err, mx_wp_document* doc)
{
    MX_ERROR_ASSERT(err, type == mx_undo_typing_e);
    MX_ERROR_ASSERT(err, doc != NULL);

    position.save(err, doc);
    MX_ERROR_CHECK(err);

    // save the end position from the start of this sheet, so that we can
    // compare positions when more typing events come in
    end_position.save(err, doc, end_position.get_sheet());
    MX_ERROR_CHECK(err);
abort:;
}

void mx_ur_item::setup_first_delete_item(int& err, mx_wp_document* doc)
{
    mx_paragraph* para;
    mx_para_pos* para_pos;

    MX_ERROR_ASSERT(err, doc != NULL);

    para = end_position.get_para(err, doc);
    MX_ERROR_CHECK(err);

    para_pos = end_position.get_para_pos(err);
    MX_ERROR_CHECK(err);

    para_pos->get_distance_from_start(err, para, offset_into_para);
    MX_ERROR_CHECK(err);

    position.moveto_start_para(err, doc);
    MX_ERROR_CHECK(err);
    end_position = position;

    // save position from start of document
    position.save(err, doc);
    MX_ERROR_CHECK(err);

    // save the end_position just from the start of this sheet so we can
    // compare items we want to combine
    end_position.save(err, doc, end_position.get_sheet());
    MX_ERROR_CHECK(err);

    deleted_chars = new mx_list;
    deleted_chars->append(character_style);
    deleted_chars->append(complex_word_item);
    character_style = NULL;
    complex_word_item = NULL;
abort:;
}

// returns whether the next_item was succesfully combined with the first
bool mx_ur_item::combine_items(int& err, mx_ur_item* next_item,
    mx_wp_document* doc)
{
    bool return_val = FALSE;

    if (this->type != next_item->type) {
        return FALSE;
    }

    switch (type) {
    case mx_undo_typing_e:
        return_val = this->combine_typing_items(err, next_item, doc);
        MX_ERROR_CHECK(err);
        break;
    case mx_undo_delete_e:
        return_val = this->combine_delete_items(err, next_item, doc);
        MX_ERROR_CHECK(err);
        break;
    default:
        break;
    }
abort:
    return return_val;
}

bool mx_ur_item::combine_typing_items(int& err, mx_ur_item* next_item,
    mx_wp_document* doc)
{
    MX_ERROR_ASSERT(err, this->type == mx_undo_typing_e);
    MX_ERROR_ASSERT(err, next_item->type == mx_undo_typing_e);
    MX_ERROR_ASSERT(err, doc != NULL);

    this->end_position.restore(err, doc);
    MX_ERROR_CHECK(err);

    this->end_position.move_right_ignoring_false_breaks(err, doc);
    MX_ERROR_CHECK(err);

    if (this->end_position == next_item->position) {
        // the typing was immediately after the current position, so we can
        // combine them

        end_position.save(err, doc, end_position.get_sheet());
        MX_ERROR_CHECK(err);

        num_chars_to_undo++;
        return TRUE;
    }
abort:
    return FALSE;
}

bool mx_ur_item::combine_delete_items(int& err, mx_ur_item* next_item,
    mx_wp_document* doc)
{
    bool b;
    MX_ERROR_ASSERT(err, this->type == mx_undo_delete_e);
    MX_ERROR_ASSERT(err, next_item->type == mx_undo_delete_e);
    MX_ERROR_ASSERT(err, deleted_chars != NULL);
    MX_ERROR_ASSERT(err, doc != NULL);

    end_position.restore(err, doc);
    MX_ERROR_CHECK(err);

    b = end_position.is_same_para_as(err, &next_item->end_position);
    MX_ERROR_CHECK(err);

    if (b) {
        bool prepend_items;
        mx_paragraph* para;
        mx_para_pos* para_pos;

        para = next_item->end_position.get_para(err, doc);
        MX_ERROR_CHECK(err);

        para_pos = next_item->end_position.get_para_pos(err);
        MX_ERROR_CHECK(err);

        para_pos->get_distance_from_start(err, para, next_item->offset_into_para);
        MX_ERROR_CHECK(err);

        if (this->offset_into_para == next_item->offset_into_para) {
            // this was a delete key pressed
            prepend_items = FALSE;
        } else if (this->offset_into_para == next_item->offset_into_para + 1) {
            // this was a backspace key pressed
            prepend_items = TRUE;
        } else {
            // this is in a different position so the items cannot be combined
            return FALSE;
        }

        this->offset_into_para = next_item->offset_into_para;
        this->num_chars_to_undo++;

        // so depending on whether we did a backspace or delete we prepend or
        // append the items to the list, so that we insert them back into the
        // document correctly when we do the undo

        if (prepend_items) {
            deleted_chars->insert(err, 0, next_item->character_style);
            MX_ERROR_CHECK(err);

            deleted_chars->insert(err, 1, next_item->complex_word_item);
            MX_ERROR_CHECK(err);
        } else {
            deleted_chars->append(next_item->character_style);
            deleted_chars->append(next_item->complex_word_item);
        }
        next_item->character_style = NULL;
        next_item->complex_word_item = NULL;
        return TRUE;
    }
abort:
    return FALSE;
}

mx_undo_redo_buffer::mx_undo_redo_buffer(int length)
{
    buffer_length = length;
    redo_depth = 0;
}

mx_undo_redo_buffer::~mx_undo_redo_buffer()
{
    mx_list_iterator iter(items);
    mx_ur_item* i;

    while (iter.more()) {
        i = (mx_ur_item*)iter.data();
        delete i;
    }
}

void mx_undo_redo_buffer::insert(int& err, mx_ur_item* item, mx_wp_document* doc)
{
    bool combined_items;
    mx_ur_item* i;

    delete item;
    return;

    if (item->type == mx_undo_page_setup_e) {
        mark_cant_undo(err);
        MX_ERROR_CHECK(err);
    }

    while (redo_depth > 0) {
        i = (mx_ur_item*)items.remove(err, 0);
        MX_ERROR_CHECK(err);

        delete i;

        redo_depth--;
    }

    combined_items = this->combine_items(err, item, doc);
    MX_ERROR_CHECK(err);

    if (!combined_items) {
        if (items.get_num_items() == buffer_length) {
            i = (mx_ur_item*)items.remove(err, buffer_length - 1);
            MX_ERROR_CHECK(err);

            delete i;
        }

        items.insert(err, 0, item);
        MX_ERROR_CHECK(err);
    }
abort:;
}

void mx_undo_redo_buffer::mark_cant_undo(int& err)
{
    mx_ur_item* i;

    while (items.get_num_items() > 0) {
        i = (mx_ur_item*)items.remove(err, 0);
        MX_ERROR_CHECK(err);

        delete i;
    }

    redo_depth = 0;
abort:;
}

bool mx_undo_redo_buffer::anything_to_undo()
{
    return (items.get_num_items() - redo_depth) > 0;
}

mx_ur_item_type_t mx_undo_redo_buffer::type_to_undo(int& err)
{
    mx_ur_item* i;

    i = (mx_ur_item*)items.get(err, redo_depth);
    MX_ERROR_CHECK(err);

    return i->type;

abort:
    return mx_undo_typing_e;
}

bool mx_undo_redo_buffer::anything_to_redo()
{
    return redo_depth > 0;
}

mx_ur_item_type_t mx_undo_redo_buffer::type_to_redo(int& err)
{
    mx_ur_item* i;

    i = (mx_ur_item*)items.get(err, redo_depth - 1);
    MX_ERROR_CHECK(err);

    return i->type;

abort:
    return mx_undo_typing_e;
}

void mx_undo_redo_buffer::undo(int& err, mx_wp_document* doc, mx_wp_doc_pos* pos)
{
    mx_ur_item* i;

    if (redo_depth == buffer_length) {
        return;
    }

    i = (mx_ur_item*)items.get(err, redo_depth);
    MX_ERROR_CHECK(err);

    i->undo(err, doc, pos);
    MX_ERROR_CHECK(err);

    if (redo_depth < buffer_length) {
        redo_depth++;
    }
abort:;
}

void mx_undo_redo_buffer::redo(int& err, mx_wp_document* doc, mx_wp_doc_pos* pos)
{
    mx_ur_item* i;

    if (redo_depth > 0) {
        i = (mx_ur_item*)items.get(err, redo_depth - 1);
        MX_ERROR_CHECK(err);

        i->redo(err, doc, pos);
        MX_ERROR_CHECK(err);

        redo_depth--;
    }

abort:;
}

const char* mx_undo_redo_buffer::item_name(mx_ur_item_type_t t)
{
    switch (t) {
    case mx_undo_typing_e:
        return "typing";
    case mx_undo_delete_e:
        return "delete";
    case mx_undo_cut_e:
        return "cut";
    case mx_undo_paste_e:
        return "paste";
    case mx_undo_format_e:
    case mx_undo_format_borders_e:
        return "format";
    case mx_undo_table_change_e:
        return "table modification";
    case mx_undo_page_setup_e:
        return "page setup";
    case mx_undo_named_style_change_e:
        return "edit style";
    case mx_undo_insert_table_e:
        return "insert table";
    case mx_undo_area_style_change_e:
        return "area style change";
    default:
        return "action";
    }
}

bool mx_undo_redo_buffer::combine_items(int& err, mx_ur_item* item,
    mx_wp_document* doc)
{
    bool combined_items = FALSE;

    // redo depth should be zero by the time this is called
    MX_ERROR_ASSERT(err, redo_depth == 0);

    if (items.get_num_items() != 0) {
        mx_ur_item* prev_item = (mx_ur_item*)items.get(err, 0);
        MX_ERROR_CHECK(err);

        combined_items = prev_item->combine_items(err, item, doc);
        MX_ERROR_CHECK(err);
    }

    if (combined_items) {
        delete item;
    } else {
        item->setup_first_item(err, doc);
        MX_ERROR_CHECK(err);
    }

    return combined_items;
abort:
    return FALSE;
}
