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

#include <mx.h>
#include <mx_aa.h>
#include <mx_break_w.h>
#include <mx_cw_t.h>
#include <mx_event.h>
#include <mx_mod_iter.h>
#include <mx_op_pos.h>
#include <mx_paragraph.h>
#include <mx_rtti.h>
#include <mx_space_w.h>
#include <mx_ta_pos.h>
#include <mx_txt_pos.h>
#include <mx_undo.h>
#include <mx_word_iter.h>
#include <mx_list_iter.h>
#include <mx_wp_area_iter.h>
#include <mx_wp_cursor.h>
#include <mx_wp_doc.h>
#include <mx_wp_lay.h>
#include <mx_wp_ob_buff.h>
#include <mx_wp_pos.h>
#include <mx_wp_style_iter.h>

mx_wp_document* mx_wp_cursor::buffer_doc = NULL;
mx_wp_object_buffer* mx_wp_cursor::buffer = NULL;

void mx_wp_cursor::cut_into_buffer(int& err, mx_wp_object_buffer& buffer)
{
    store_sheet_dimensions(err);
    MX_ERROR_CHECK(err);

    if (start == position) {
        return;
    }

    if (is_selection) {
        // make sure position is always the first in the range
        sort_range();

        buffer.cut(err, *document, start, position);
        MX_ERROR_CHECK(err);

        check_for_empty_document(err);
        MX_ERROR_CHECK(err);

        is_selection = FALSE;
    } else {
        MX_ERROR_THROW(err, MX_CURSOR_NO_SELECTION);
    }
abort:;
}

void mx_wp_cursor::cut(int& err)
{
    mx_char_style* cs;

    if (is_selection) {
        sort_range();

        mx_wp_doc_pos p1 = start;
        mx_wp_doc_pos p2 = position;
        mx_ur_item* item;

        p1.save(err, document);
        MX_ERROR_CHECK(err);

        p2.save(err, document);
        MX_ERROR_CHECK(err);

        cut_into_buffer(err, *buffer);
        MX_ERROR_CHECK(err);

        item = new mx_ur_item(mx_undo_cut_e, document, buffer, p1, p2);

        ur_buffer->insert(err, item);
        MX_ERROR_CHECK(err);
    }

    cs = this->char_style_at_position(err);
    MX_ERROR_CHECK(err);

    set_current_style(*cs);
abort:;
}

void mx_wp_cursor::copy(int& err)
{
    if (is_selection) {
        // make sure position is always the first in the range
        sort_range();

        buffer->copy(err, *document, start, position);
        MX_ERROR_CHECK(err);
    } else {
        MX_ERROR_THROW(err, MX_CURSOR_NO_SELECTION);
    }
abort:;
}

void mx_wp_cursor::paste(int& err)
{
    if (buffer->is_empty()) {
        MX_ERROR_THROW(err, MX_CURSOR_NO_SELECTION);
    } else {
        mx_char_style* cs;
        mx_wp_doc_pos p1, p2;
        mx_ur_item* item;
        bool need_to_delete_area_for_undo;

        remove_selection(err);
        MX_ERROR_CHECK(err);

        p1 = position;
        p1.save(err, document, p1.get_sheet());
        MX_ERROR_CHECK(err);

        buffer->insert(err, *document, position, need_to_delete_area_for_undo);
        MX_ERROR_CHECK(err);

        p1.restore(err, document);
        MX_ERROR_CHECK(err);
        p2 = position;

        item = new mx_ur_item(mx_undo_paste_e, document, p1, p2,
            need_to_delete_area_for_undo);

        ur_buffer->insert(err, item);
        MX_ERROR_CHECK(err);

        cs = this->char_style_at_position(err);
        MX_ERROR_CHECK(err);

        set_current_style(*cs);
    }
abort:;
}

void mx_wp_cursor::select_all(int& err)
{
    mx_wp_doc_pos s, e;

    s.moveto_start(err, document);
    MX_ERROR_CHECK(err);

    e.moveto_end(err, document);
    MX_ERROR_CHECK(err);

    set_selection(err, s, e);
    MX_ERROR_CHECK(err);

abort:;
}

void mx_wp_cursor::update_selection_contents_flags(int& err)
{
    mx_area* a;
    mx_wp_doc_pos* start_pos = is_selection ? &start : &position;
    mx_wp_area_iterator iter(document, *start_pos, position);

    text_in_selection = table_in_selection = multi_column_in_selection = diagram_in_selection = image_in_selection = FALSE;

    do {
        a = (mx_area*)iter.data(err);
        MX_ERROR_CHECK(err);

        switch (a->rtti_class()) {
        case mx_table_area_class_e:
            table_in_selection = TRUE;
            break;
        case mx_text_area_class_e:
            text_in_selection = TRUE;
            break;
        default:
            break;
        }

        if (table_in_selection && text_in_selection) {
            break;
        }
    } while (iter.more(err));
    MX_ERROR_CHECK(err);

    if (table_in_selection && text_in_selection) {
        text_only_selection = table_only_selection = FALSE;
    } else if (table_in_selection) {
        if (position.is_same_area_as(start_pos)) {
            text_only_selection = position.is_same_cell_as(err, start_pos);
            MX_ERROR_CHECK(err);

            table_only_selection = !text_only_selection;
        } else {
            text_only_selection = table_only_selection = FALSE;
        }
    } else {
        text_only_selection = text_in_selection;
        table_only_selection = FALSE;
    }

abort:;
}

bool mx_wp_cursor::selection_within_area(int& err)
{
    if (!is_selection)
        return TRUE;
    return position.is_same_area_as(&start);
}

void mx_wp_cursor::remove_selection(int& err)
{
    if (is_selection) {
        mx_wp_doc_pos cut_start, cut_end;

        if (temp_cut_buffer == NULL) {
            temp_cut_document = mx_db_client_open_temporary_wp_doc(err);
            MX_ERROR_CHECK(err);

            temp_cut_buffer = new mx_wp_object_buffer(temp_cut_document);
            MX_ERROR_CHECK(err);
        }

        sort_range();

        mx_ur_item* item;

        cut_start = start;
        cut_end = position;

        cut_start.save(err, document);
        MX_ERROR_CHECK(err);

        cut_end.save(err, document);
        MX_ERROR_CHECK(err);

        temp_cut_buffer->cut(err, *document, start, position);
        MX_ERROR_CHECK(err);

        item = new mx_ur_item(mx_undo_cut_e, document, temp_cut_buffer,
            cut_start, cut_end);

        ur_buffer->insert(err, item);
        MX_ERROR_CHECK(err);

        is_selection = FALSE;

        {
            // now reformat to ensure stuff is repositioned correctly
            mx_wp_doc_pos start_pos = position, end_pos = position;

            start_pos.moveto_start_para(err, document);
            MX_ERROR_CHECK(err);

            end_pos.moveto_end_sheet(err, document);
            MX_ERROR_CHECK(err);

            reformat_and_restore_position(err, start_pos, end_pos, TRUE);
            MX_ERROR_CHECK(err);
        }
        mx_char_style* cs;

        cs = this->char_style_at_position(err);
        MX_ERROR_CHECK(err);

        set_current_style(*cs);
    }
abort:;
}

void mx_wp_cursor::delete_buffer()
{
    if (buffer)
        delete buffer;
    if (buffer_doc) {
        int err = MX_ERROR_OK;
        mx_db_client_close_wp_doc(err, buffer_doc);
        MX_ERROR_CLEAR(err);
    }
}

static void get_styles_in_selection(int& err,
    mx_wp_document* document,
    mx_wp_doc_pos& start_pos,
    mx_wp_doc_pos& end_pos,
    mx_list& style_list)
{
    bool b;
    mx_paragraph *start_para = NULL, *end_para = NULL;

    MX_ERROR_ASSERT(err, end_pos >= start_pos);

    b = start_pos.is_paragraph(err);
    if (b) {
        MX_ERROR_CHECK(err);
        start_para = start_pos.get_para(err, document);
    }
    MX_ERROR_CHECK(err);

    b = end_pos.is_paragraph(err);
    if (b) {
        MX_ERROR_CHECK(err);
        end_para = end_pos.get_para(err, document);
    }
    MX_ERROR_CHECK(err);

    {
        static const uint32 num_paras_limit = 1000;
        uint32 num_paras_considered = 0;
        mx_paragraph* para;
        mx_wp_style_iterator para_iter(document, start_pos, end_pos);

        do {
            MX_ERROR_CHECK(err);
            para = (mx_paragraph*)para_iter.data(err);
            MX_ERROR_CHECK(err);

            if (para->is_a(mx_paragraph_class_e)) {
                mx_paragraph_style current_ps;
                mx_char_style *cs, current_cs;
                mx_para_pos start_pp, end_pp;

                start_pp.moveto_start();
                end_pp.moveto_end(err, para);
                MX_ERROR_CHECK(err);

                if (para == start_para) {
                    // we have the start paragraph
                    mx_para_pos* pp = start_pos.get_para_pos(err);
                    MX_ERROR_CHECK(err);
                    start_pp = *pp;
                }

                if (para == end_para) {
                    // we have the end paragraph
                    mx_para_pos* pp = end_pos.get_para_pos(err);
                    MX_ERROR_CHECK(err);
                    end_pp = *pp;
                }

                mx_word::set_operations_to_right_of_mods();

                current_ps = *para->get_paragraph_style();
                cs = para->aka_get_style(err, start_pp);
                MX_ERROR_CHECK(err);
                current_ps.set_char_style(*cs);
                cs = current_ps.get_char_style();

                style_list.append(new mx_paragraph_style(current_ps));

                {
                    mx_style_mod_iterator mod_iter(para, start_pp, end_pp);
                    while (mod_iter.more()) {
                        *cs += *mod_iter.data();
                        style_list.append(new mx_paragraph_style(current_ps));
                    }
                }
            }
        } while (para_iter.more(err) && num_paras_considered++ < num_paras_limit);
        MX_ERROR_CHECK(err);
    }
abort:;
}

void mx_wp_cursor::styles_in_selection(int& err, mx_list& style_list)
{
    style_list = mx_list();

    if (!selection()) {
        mx_paragraph_style* ps;

        ps = document->get_para_style(err, &position);
        MX_ERROR_CHECK(err);

        style_list.append(new mx_paragraph_style(*ps));
    } else {
        get_styles_in_selection(err, document, *selection_start(), *selection_end(),
            style_list);
        MX_ERROR_CHECK(err);
    }

abort:;
}

static void get_border_styles_in_table_selection(int& err,
    mx_wp_document* document,
    mx_wp_doc_pos& start_pos,
    mx_wp_doc_pos& end_pos,
    mx_nlist& style_array)
{
    mx_table_area *taba1, *taba2;
    mx_table_area_pos *tabp1, *tabp2;

    taba1 = (mx_table_area*)start_pos.get_area(err, document);
    MX_ERROR_CHECK(err);

    taba2 = (mx_table_area*)end_pos.get_area(err, document);
    MX_ERROR_CHECK(err);

    MX_ERROR_ASSERT(err, taba1 == taba2);
    MX_ERROR_ASSERT(err, taba1->is_a(mx_table_area_class_e));

    tabp1 = (mx_table_area_pos*)start_pos.get_area_pos(err);
    MX_ERROR_CHECK(err);

    tabp2 = (mx_table_area_pos*)end_pos.get_area_pos(err);
    MX_ERROR_CHECK(err);

    MX_ERROR_ASSERT(err, tabp1->is_a(mx_table_area_pos_class_e));
    MX_ERROR_ASSERT(err, tabp2->is_a(mx_table_area_pos_class_e));

    taba1->get_cell_borders_for_selection(err, *tabp1, *tabp2, style_array);
    MX_ERROR_CHECK(err);
abort:;
}

void mx_wp_cursor::border_styles_in_selection(int& err, mx_nlist& style_array)
{
    style_array = mx_nlist();

    if (!selection()) {
        mx_paragraph_style* ps;

        ps = document->get_para_style(err, &position);
        MX_ERROR_CHECK(err);

        style_array.append_item(new mx_border_style(*ps->get_border_style()));
    } else {
        if (selection_only_covers_text()) {
            mx_list ps_list;
            this->styles_in_selection(err, ps_list);
            MX_ERROR_CHECK(err);
            {
                mx_list_iterator ps_iter(ps_list);
                while (ps_iter.more()) {
                    mx_paragraph_style* ps = (mx_paragraph_style*)ps_iter.data();
                    style_array.append();
                    style_array.append_item(
                        new mx_border_style(*ps->get_border_style()));
                    delete ps;
                }
            }
        } else if (selection_only_covers_table()) {
            get_border_styles_in_table_selection(
                err, document, *selection_start(), *selection_end(), style_array);
            MX_ERROR_CHECK(err);
        } else {
            // shouldn't be here
            MX_ERROR_THROW(err, MX_ERROR_UNDEFINED);
        }
    }
abort:;
}

float mx_wp_cursor::get_selected_column_width(int& err)
{
    mx_area *a, *start_area, *end_area;
    mx_table_area* ta;

    mx_wp_doc_pos *start, *end;

    mx_table_area_pos *sap, *eap;

    mx_wp_area_iterator* iter;

    bool column_found;
    float column_width = -1.0, temp;
    int i, nc;

    start = selection_start();
    end = selection_end();

    start_area = start->get_area(err, document);
    MX_ERROR_CHECK(err);

    end_area = end->get_area(err, document);
    MX_ERROR_CHECK(err);

    // if range is entirely within one area things work differently
    if (start_area == end_area) {
        ta = (mx_table_area*)start_area;

        sap = (mx_table_area_pos*)start->get_area_pos(err);
        MX_ERROR_CHECK(err);

        eap = (mx_table_area_pos*)end->get_area_pos(err);
        MX_ERROR_CHECK(err);

        column_width = ta->get_column_width(err, sap->column);
        MX_ERROR_CHECK(err);

        for (i = sap->column + 1; i <= eap->column; i++) {
            temp = ta->get_column_width(err, i);
            MX_ERROR_CHECK(err);

            if (!MX_FLOAT_EQ(temp, column_width)) {
                return -1.0;
            }
        }
        return column_width;
    }

    // find all the table areas/columns in the current range
    iter = new mx_wp_area_iterator(document, *start, *end);

    column_found = FALSE;

    do {
        MX_ERROR_CHECK(err);
        a = (mx_area*)iter->data(err);
        MX_ERROR_CHECK(err);

        if (a->rtti_class() == mx_table_area_class_e) {
            ta = (mx_table_area*)a;

            nc = ta->get_num_columns(err);
            MX_ERROR_CHECK(err);

            for (i = 0; i < nc; i++) {
                temp = ta->get_column_width(err, i);
                MX_ERROR_CHECK(err);

                if (column_found) {
                    if (!MX_FLOAT_EQ(temp, column_width)) {
                        return -1.0;
                    }
                } else {
                    column_width = temp;
                    column_found = TRUE;
                }
            }
        }
    } while (iter->more(err));
    MX_ERROR_CHECK(err);

    delete iter;

    return column_width;

abort:
    return -1.0;
}

const char* mx_wp_cursor::get_selection_as_text(int& err)
{
    if (is_selection) {
        mx_word* w;
        mx_word_iterator iter(document, *selection_start(), *selection_end(),
            TRUE);
        static const int max_string_len = 10000;
        static char res[max_string_len + 1];
        int i = 0;
        const char* s;

        res[i] = 0;

        while (iter.more(err) && i < max_string_len) {
            MX_ERROR_CHECK(err);

            w = iter.data(err);
            MX_ERROR_CHECK(err);

            if (!w->is_a(mx_break_word_class_e) || iter.word_end != iter.end) {
                s = w->cstring();

                while (*s != 0 && i < max_string_len) {
                    res[i++] = *s++;
                }
            }
        }
        MX_ERROR_CHECK(err);
        res[i] = 0;
        return res;
    }
abort:
    return NULL;
}

static void get_table_styles_in_selection(int& err,
    mx_wp_document* document,
    mx_wp_doc_pos& start_pos,
    mx_wp_doc_pos& end_pos,
    mx_list& style_list)
{
    mx_area* a;
    mx_wp_doc_pos p = start_pos;

    mx_paragraph_style* ps;

    MX_ERROR_ASSERT(err, end_pos >= start_pos);

    while (p <= end_pos) {
        a = p.get_area(err, document);
        MX_ERROR_CHECK(err);

        ps = a->get_paragraph_style();

        style_list.append(new mx_paragraph_style(*ps));

        p.next_area(err, document);
        MX_ERROR_CHECK(err);
    }
abort:;
}

void mx_wp_cursor::table_styles_in_selection(int& err, mx_list& style_list)
{
    style_list = mx_list();

    if (!selection()) {
        mx_paragraph_style* ps;
        mx_area* a;

        a = position.get_area(err, document);
        MX_ERROR_CHECK(err);

        ps = a->get_paragraph_style();

        style_list.append(new mx_paragraph_style(*ps));
    } else {
        get_table_styles_in_selection(
            err,
            document,
            *selection_start(),
            *selection_end(),
            style_list);
        MX_ERROR_CHECK(err);
    }

abort:;
}

void mx_wp_cursor::fix_selection_positions(int& err)
{
    if (is_selection) {
        mx_wp_doc_pos* start_pos = selection_start();
        mx_wp_doc_pos* end_pos = selection_end();

        bool start_in_table, end_in_table;
        MX_ERROR_ASSERT(err, *start_pos <= *end_pos);

        start_in_table = start_pos->is_table(err);
        MX_ERROR_CHECK(err);

        end_in_table = end_pos->is_table(err);
        MX_ERROR_CHECK(err);

        if (start_in_table && end_in_table) {
            bool b = start_pos->is_same_cell_as(err, end_pos);
            MX_ERROR_CHECK(err);

            if (b)
                return;

            if (start_pos->is_same_area_as(end_pos)) {
                start_pos->moveto_start_cell(err, document);
                MX_ERROR_CHECK(err);

                end_pos->moveto_end_cell(err, document);
                MX_ERROR_CHECK(err);

                return;
            }
        }

        if (start_in_table) {
            start_pos->moveto_start_row(err, document);
            MX_ERROR_CHECK(err);
        }

        if (end_in_table) {
            end_pos->moveto_end_row(err, document);
            MX_ERROR_CHECK(err);
        }
    }
abort:;
}
