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
#include <mx_frame.h>
#include <mx_mod_iter.h>
#include <mx_op_pos.h>
#include <mx_paragraph.h>
#include <mx_rtti.h>
#include <mx_ta_pos.h>
#include <mx_txt_pos.h>
#include <mx_undo.h>
#include <mx_wp_area_iter.h>
#include <mx_wp_doc.h>
#include <mx_wp_lay.h>
#include <mx_wp_ob_buff.h>
#include <mx_wp_pos.h>
#include <mx_wp_style_iter.h>

mx_paragraph* mx_wp_cursor::insert_paragraph(int& err)
{
    // If the current position is a text area or a text area within a table,
    // the paragraph is inserted there.
    // If the position is directly before or after a text area a paragraph
    // is added to the start or end of the adjacent area.
    // If the position is not next to a text area, a new text area is inserted
    // and a pointer to it's first paragraph returned.
    mx_sheet* s;
    mx_area* a;

    mx_area_pos* ap;
    mx_paragraph* res;

    s = document->sheet(err, position.get_sheet());
    MX_ERROR_CHECK(err);

    a = position.get_sheet_pos()->area(err, s);
    MX_ERROR_CHECK(err);

    ap = position.get_sheet_pos()->get_area_pos(err);
    MX_ERROR_CHECK(err);

    if (ap->rtti_class() == mx_text_area_pos_class_e) {
        res = insert_paragraph_into_text_area(err, (mx_text_area_pos*)ap, (mx_text_area*)a);
        MX_ERROR_CHECK(err);
    } else {
        if (ap->rtti_class() == mx_table_area_pos_class_e) {
            res = insert_paragraph_into_table_area(err, (mx_table_area_pos*)ap, (mx_table_area*)a);
            MX_ERROR_CHECK(err);
        } else {
            res = insert_paragraph_into_opaque_area(
                err,
                *(position.get_sheet_pos()),
                s,
                a);
            MX_ERROR_CHECK(err);
        }
    }

    return res;
abort:
    return NULL;
}

mx_paragraph* mx_wp_cursor::insert_paragraph_after(int& err)
{
    mx_paragraph* para;
    mx_text_area_pos* tap;
    mx_text_area* ta;
    mx_table_area* tab_a = NULL;
    mx_table_area_pos* tab_ap = NULL;
    bool is_paragraph;

    is_paragraph = position.is_paragraph(err);
    MX_ERROR_CHECK(err);

    if (is_paragraph) {
        para = position.get_para(err, document);
        MX_ERROR_CHECK(err);

        if (para->total_num_words() <= 1) {
            tap = (mx_text_area_pos*)position.get_area_pos(err);
            MX_ERROR_CHECK(err);

            switch (tap->rtti_class()) {
            case mx_table_area_pos_class_e:
                tab_ap = (mx_table_area_pos*)tap;
                tap = &tab_ap->text_pos;

                tab_a = (mx_table_area*)position.get_area(err, document);
                MX_ERROR_CHECK(err);

                ta = (mx_text_area*)tab_a->get_cell(err, tab_ap->row, tab_ap->column);
                MX_ERROR_CHECK(err);

                break;
            case mx_text_area_pos_class_e:
                ta = (mx_text_area*)position.get_area(err, document);
                MX_ERROR_CHECK(err);
                break;
            default:
                MX_ERROR_THROW(err, MX_ERROR_UNDEFINED);
                break;
            }

            tap->paragraph_index++;

            para = ta->insert_new_paragraph(err, *tap);
            MX_ERROR_CHECK(err);

            return para;
        }
    }

    para = insert_paragraph(err);
    MX_ERROR_CHECK(err);

    return para;

abort:
    return NULL;
}

mx_paragraph* mx_wp_cursor::insert_paragraph_into_text_area(
    int& err,
    mx_text_area_pos* ap,
    mx_text_area* a)
{
    mx_paragraph* res;

    res = a->insert_new_paragraph(err, *ap);
    MX_ERROR_CHECK(err);

    ap->para_pos.moveto_start();

    return res;
abort:
    return NULL;
}

mx_paragraph* mx_wp_cursor::insert_paragraph_into_table_area(
    int& err,
    mx_table_area_pos* ap,
    mx_table_area* a)
{
    mx_text_area* ta;
    mx_paragraph* res;

    ta = a->get_cell(err, ap->row, ap->column);
    MX_ERROR_CHECK(err);

    res = insert_paragraph_into_text_area(err, &(ap->text_pos), ta);
    MX_ERROR_CHECK(err);

    return res;

abort:
    return NULL;
}

mx_paragraph* mx_wp_cursor::insert_paragraph_into_opaque_area(
    int& err,
    mx_wp_sheet_pos& sp,
    mx_sheet* s,
    mx_area* a)
{
    mx_opaque_area_pos* ap;
    mx_paragraph* p = NULL;

    ap = (mx_opaque_area_pos*)sp.get_area_pos(err);
    MX_ERROR_CHECK(err);

    if (ap->is_start()) {
        p = insert_paragraph_left_of_opaque_area(err, sp, s);
        MX_ERROR_CHECK(err);
    } else {
        p = insert_paragraph_right_of_opaque_area(err, sp, s);
        MX_ERROR_CHECK(err);
    }

    return p;

abort:
    return p;
}

mx_paragraph* mx_wp_cursor::insert_paragraph_left_of_opaque_area(
    int& err,
    mx_wp_sheet_pos& sp,
    mx_sheet* s)
{
    mx_paragraph* p = NULL;
    mx_text_area* ta;
    mx_area* prev;

    // get area to left
    sp.left(err, s);
    if (err == MX_POSITION_RANGE_ERROR) {
        MX_ERROR_CLEAR(err);

        // insert a new text area into the start of the sheet
        ta = add_text_area(err, s, 0);
        MX_ERROR_CHECK(err);

        p = ta->get(err, 0);
        MX_ERROR_CHECK(err);

        sp.moveto_start(err, s);
        MX_ERROR_CHECK(err);
    } else {
        MX_ERROR_CHECK(err);

        // now sp points to end of previous area. what type of area?
        prev = sp.area(err, s);
        MX_ERROR_CHECK(err);

        if (prev->rtti_class() == mx_text_area_class_e) {
            // add new paragraph
            p = insert_paragraph(err);
            MX_ERROR_CHECK(err);
        } else {
            // insert new text area
            sp.right(err, s);
            MX_ERROR_CHECK(err);

            ta = add_text_area(err, s, sp.get_area_index());
            MX_ERROR_CHECK(err);

            sp.reset(err, s);
            MX_ERROR_CHECK(err);
        }
    }

abort:
    return p;
}

mx_paragraph* mx_wp_cursor::insert_paragraph_right_of_opaque_area(
    int& err,
    mx_wp_sheet_pos& sp,
    mx_sheet* s)
{
    mx_paragraph* p = NULL;
    mx_text_area* ta;
    mx_area* next;

    // get area to right
    sp.right(err, s);
    if (err == MX_POSITION_RANGE_ERROR) {
        MX_ERROR_CLEAR(err);

        // insert a new text area into the end of the sheet
        ta = add_text_area(err, s, s->get_num_areas());
        MX_ERROR_CHECK(err);

        sp.right(err, s);
        MX_ERROR_CHECK(err);

        p = ta->get(err, 0);
        MX_ERROR_CHECK(err);
    } else {
        MX_ERROR_CHECK(err);

        // now sp points to start of next area. what type of area?
        next = sp.area(err, s);
        MX_ERROR_CHECK(err);

        if (next->rtti_class() == mx_text_area_class_e) {
            // add new paragraph
            p = insert_paragraph(err);
            MX_ERROR_CHECK(err);
        } else {
            // insert new text area
            ta = add_text_area(err, s, sp.get_area_index());
            MX_ERROR_CHECK(err);

            sp.reset(err, s);
            MX_ERROR_CHECK(err);

            p = ta->get(err, 0);
            MX_ERROR_CHECK(err);
        }
    }

abort:
    return p;
}

mx_table_area* mx_wp_cursor::insert_table(int& err)
{
    mx_sheet* s;
    mx_table_area* ta;
    int i;

    // deactivate the active area
    active_area->deactivate();

    i = document->split(err, position);
    MX_ERROR_CHECK(err);

    s = document->sheet(err, position.get_sheet());
    MX_ERROR_CHECK(err);

    ta = s->add_table_area(err, i);
    MX_ERROR_CHECK(err);

    position.get_sheet_pos()->moveto_start(err, s, i);
    MX_ERROR_CHECK(err);

    return ta;

abort:
    return NULL;
}

void mx_wp_cursor::insert_cw_item(int& err, mx_cw_t* item)
{
    mx_wp_doc_pos reformat_from = position;

    remove_selection(err);
    MX_ERROR_CHECK(err);

    document->insert_cw_item(err, item, position);
    MX_ERROR_CHECK(err);

    position.right(err, document);
    MX_ERROR_CHECK(err);

    reformat_and_restore_position(err, reformat_from, position, FALSE);
    MX_ERROR_CHECK(err);
abort:;
}

mx_text_area* mx_wp_cursor::add_text_area(int& err, mx_sheet* s, int i)
{
    mx_text_area* res;
    float left, right, top, bottom, width;

    s->get_margins(err, left, right, top, bottom);
    MX_ERROR_CHECK(err);

    width = s->get_width(err);
    MX_ERROR_CHECK(err);

    res = s->add_text_area(err, i);
    MX_ERROR_CHECK(err);

    res->set_outline(err, width - (left + right), 1.0);
    MX_ERROR_CHECK(err);

    return res;
abort:
    return NULL;
}

void mx_wp_cursor::set_selected_column_width(int& err, float width)
{
    mx_wp_doc_pos* start_pos = selection_start();
    mx_wp_doc_pos* end_pos = selection_end();
    mx_ur_item* ur_item;
    int i;

    active_area->deactivate();

    // if range is entirely within one area things work differently
    if (start_pos->is_same_area_as(end_pos)) {
        mx_table_area* ta;
        mx_table_area_pos *sap, *eap;

        ta = (mx_table_area*)start_pos->get_area(err, document);
        MX_ERROR_CHECK(err);

        ur_item = new mx_ur_item(
            mx_undo_table_change_e,
            document,
            position,
            ta);

        ur_buffer->insert(err, ur_item);
        MX_ERROR_CHECK(err);

        sap = (mx_table_area_pos*)start_pos->get_area_pos(err);
        MX_ERROR_CHECK(err);

        eap = (mx_table_area_pos*)end_pos->get_area_pos(err);
        MX_ERROR_CHECK(err);

        for (i = sap->column; i <= eap->column; i++) {
            ta->set_column_width(err, i, width);
            MX_ERROR_CHECK(err);
        }
    } else {
        // find all the table areas/columns in the current range
        mx_wp_area_iterator iter(document, *start_pos, *end_pos);
        int nc;

        do {
            MX_ERROR_CHECK(err);
            mx_area* a = (mx_area*)iter.data(err);
            MX_ERROR_CHECK(err);

            if (a->rtti_class() == mx_table_area_class_e) {
                mx_table_area* ta = (mx_table_area*)a;

                ur_item = new mx_ur_item(mx_undo_table_change_e,
                    document,
                    position,
                    ta);

                ur_buffer->insert(err, ur_item);
                MX_ERROR_CHECK(err);

                nc = ta->get_num_columns(err);
                MX_ERROR_CHECK(err);

                for (i = 0; i < nc; i++) {
                    ta->set_column_width(err, i, width);
                    MX_ERROR_CHECK(err);
                }
            }
        } while (iter.more(err));
        MX_ERROR_CHECK(err);
    }

    {
        mx_wp_doc_pos start_of_sh_pos = *start_pos;
        mx_wp_doc_pos end_of_sh_pos = *end_pos;

        start_of_sh_pos.moveto_start_sheet(err, document);
        MX_ERROR_CHECK(err);

        end_of_sh_pos.moveto_end_sheet(err, document);
        MX_ERROR_CHECK(err);

        reformat_and_restore_position(err, start_of_sh_pos, end_of_sh_pos, TRUE);
        MX_ERROR_CHECK(err);

        update_caret(err);
        MX_ERROR_CHECK(err);

        frame->refresh(err);
        MX_ERROR_CHECK(err);
    }
abort:;
}

void mx_wp_cursor::cut_table_rows(int& err)
{
    mx_table_area* ta;
    mx_table_area_pos *start_tap, *end_tap;
    int n, m;

    ta = (mx_table_area*)get_start_area(err);
    MX_ERROR_CHECK(err);

    n = ta->get_num_rows(err);
    MX_ERROR_CHECK(err);

    if (n == 1) {
        // remove whole table
        remove_whole_table(err);
        MX_ERROR_CHECK(err);
    } else {
        if (is_selection) {
            // delete the rows in the selection
            start_tap = (mx_table_area_pos*)get_start_area_pos(err);
            MX_ERROR_CHECK(err);

            end_tap = (mx_table_area_pos*)get_end_area_pos(err);
            MX_ERROR_CHECK(err);

            m = end_tap->row - start_tap->row + 1;

            if (m == n) {
                // remove whole table
                remove_whole_table(err);
                MX_ERROR_CHECK(err);
            } else {
                ta->remove_rows(err, start_tap->row, m);
                MX_ERROR_CHECK(err);

                if (end_tap->row == (n - 1)) {
                    start_tap->row--;
                }

                start_tap->text_pos.moveto_start(err, NULL);
                MX_ERROR_CHECK(err);
            }
            is_selection = FALSE;
        } else {
            // delete the current row
            start_tap = (mx_table_area_pos*)get_start_area_pos(err);
            MX_ERROR_CHECK(err);

            ta->remove_rows(err, start_tap->row, 1);
            MX_ERROR_CHECK(err);

            if (start_tap->row == (n - 1)) {
                start_tap->row--;
            }

            start_tap->text_pos.moveto_start(err, NULL);
            MX_ERROR_CHECK(err);
        }
    }

abort:;
}

void mx_wp_cursor::cut_table_columns(int& err)
{
    mx_table_area* ta;
    mx_table_area_pos *start_tap, *end_tap;
    int n, m;

    ta = (mx_table_area*)get_start_area(err);
    MX_ERROR_CHECK(err);

    n = ta->get_num_columns(err);
    MX_ERROR_CHECK(err);

    if (n == 1) {
        // remove whole table
        remove_whole_table(err);
        MX_ERROR_CHECK(err);
    } else {
        if (is_selection) {
            // delete the columns in the selection
            start_tap = (mx_table_area_pos*)get_start_area_pos(err);
            MX_ERROR_CHECK(err);

            end_tap = (mx_table_area_pos*)get_end_area_pos(err);
            MX_ERROR_CHECK(err);

            m = end_tap->column - start_tap->column + 1;

            if (m == n) {
                // remove whole table
                remove_whole_table(err);
                MX_ERROR_CHECK(err);
            } else {
                ta->remove_columns(err, start_tap->column, m);
                MX_ERROR_CHECK(err);

                if (end_tap->column == (n - 1)) {
                    start_tap->column--;
                }

                start_tap->text_pos.moveto_start(err, NULL);
                MX_ERROR_CHECK(err);
            }
            is_selection = FALSE;
        } else {
            // delete the current row
            start_tap = (mx_table_area_pos*)get_start_area_pos(err);
            MX_ERROR_CHECK(err);

            ta->remove_columns(err, start_tap->column, 1);
            MX_ERROR_CHECK(err);

            if (start_tap->column == (n - 1)) {
                start_tap->column--;
            }

            start_tap->text_pos.moveto_start(err, NULL);
            MX_ERROR_CHECK(err);
        }
    }

abort:;
}

void mx_wp_cursor::insert_table_rows_before(int& err, int n)
{
    mx_table_area* a;
    mx_table_area_pos* ap;

    mx_ur_item* ur_item;

    ap = (mx_table_area_pos*)position.get_area_pos(err);
    MX_ERROR_CHECK(err);

    a = (mx_table_area*)position.get_area(err, document);
    MX_ERROR_CHECK(err);

    ur_item = new mx_ur_item(
        mx_undo_table_change_e,
        document,
        position,
        a);

    ur_buffer->insert(err, ur_item);
    MX_ERROR_CHECK(err);

    a->insert_rows(err, ap->row, n);
    MX_ERROR_CHECK(err);

    position.reset(err, document);
    MX_ERROR_CHECK(err);

abort:;
}

void mx_wp_cursor::insert_table_rows_after(int& err, int n)
{
    mx_table_area* a;
    mx_table_area_pos* ap;

    mx_ur_item* ur_item;

    ap = (mx_table_area_pos*)position.get_area_pos(err);
    MX_ERROR_CHECK(err);

    a = (mx_table_area*)position.get_area(err, document);
    MX_ERROR_CHECK(err);

    ur_item = new mx_ur_item(
        mx_undo_table_change_e,
        document,
        position,
        a);

    ur_buffer->insert(err, ur_item);
    MX_ERROR_CHECK(err);

    a->insert_rows(err, ap->row + 1, n);
    MX_ERROR_CHECK(err);

    position.reset(err, document);
    MX_ERROR_CHECK(err);

abort:;
}

void mx_wp_cursor::insert_table_columns_before(int& err, int n)
{
    mx_table_area* a;
    mx_table_area_pos* ap;

    mx_ur_item* ur_item;

    ap = (mx_table_area_pos*)position.get_area_pos(err);
    MX_ERROR_CHECK(err);

    a = (mx_table_area*)position.get_area(err, document);
    MX_ERROR_CHECK(err);

    ur_item = new mx_ur_item(
        mx_undo_table_change_e,
        document,
        position,
        a);

    ur_buffer->insert(err, ur_item);
    MX_ERROR_CHECK(err);

    a->insert_columns(err, ap->column, n);
    MX_ERROR_CHECK(err);

    position.reset(err, document);
    MX_ERROR_CHECK(err);

abort:;
}

void mx_wp_cursor::insert_table_columns_after(int& err, int n)
{
    mx_table_area* a;
    mx_table_area_pos* ap;

    mx_ur_item* ur_item;

    ap = (mx_table_area_pos*)position.get_area_pos(err);
    MX_ERROR_CHECK(err);

    a = (mx_table_area*)position.get_area(err, document);
    MX_ERROR_CHECK(err);

    ur_item = new mx_ur_item(
        mx_undo_table_change_e,
        document,
        position,
        a);

    ur_buffer->insert(err, ur_item);
    MX_ERROR_CHECK(err);

    a->insert_columns(err, ap->column + 1, n);
    MX_ERROR_CHECK(err);

    position.reset(err, document);
    MX_ERROR_CHECK(err);

abort:;
}

void mx_wp_cursor::insert_page_break(int& err)
{
    mx_para_pos* pp;
    mx_paragraph* p;

    mx_paragraph_style style;
    mx_table_area_pos* tap;

    int w, l;

    mx_area_pos* ap = position.get_area_pos(err);
    MX_ERROR_CHECK(err);

    // are we in a table ?
    if (ap->rtti_class() == mx_table_area_pos_class_e) {
        tap = (mx_table_area_pos*)ap;

        if (tap->row != 0) {
            (void)split_table(err);
            MX_ERROR_CHECK(err);

            insert_text_area_before_table(err);
            MX_ERROR_CHECK(err);

            repaginate(err);
            MX_ERROR_CHECK(err);
        } else {
            insert_text_area_before_table(err);
            MX_ERROR_CHECK(err);

            (void)position.get_area(err, document);
            MX_ERROR_CHECK(err);

            repaginate(err);
            MX_ERROR_CHECK(err);
        }
    } else {
        pp = position.get_para_pos(err);
        MX_ERROR_CHECK(err);

        w = pp->word_index;
        l = pp->letter_index;

        pp->word_index = 0;
        pp->letter_index = 0;

        if (!pp->is_start()) {
            handle_return(err, FALSE);
            MX_ERROR_CHECK(err);
        }

        p = position.get_para(err, document);
        MX_ERROR_CHECK(err);

        style = *(p->get_paragraph_style());
        style.page_break_before = TRUE;
        p->modify_paragraph_style(style);

        pp->word_index = w;
        pp->letter_index = l;

        repaginate(err);
        MX_ERROR_CHECK(err);
    }

abort:;
}

void mx_wp_cursor::insert_text_area_before_table(int& err)
{
    mx_sheet* s;
    mx_wp_sheet_pos* sp = position.get_sheet_pos();
    mx_text_area* ta;

    mx_table_area_pos save_pos;

    mx_paragraph_style style;
    mx_table_area_pos* tap;
    mx_paragraph* p;

    bool b;

    tap = (mx_table_area_pos*)position.get_area_pos(err);
    MX_ERROR_CHECK(err);

    save_pos = *tap;

    s = document->sheet(err, position.get_sheet());
    MX_ERROR_CHECK(err);

    ta = s->add_text_area(err, sp->get_area_index());
    MX_ERROR_CHECK(err);

    document->reformat_sheet(err, position.get_sheet(), b);
    MX_ERROR_CHECK(err);

    // get the first paragraph and set the page break before field
    p = ta->get(err, 0);
    MX_ERROR_CHECK(err);

    style = *(p->get_paragraph_style());
    style.page_break_before = TRUE;
    p->modify_paragraph_style(style);

    sp->set_area_index(err, sp->get_area_index() + 1, s);
    MX_ERROR_CHECK(err);

    tap = (mx_table_area_pos*)position.get_area_pos(err);
    MX_ERROR_CHECK(err);

    tap->text_pos = save_pos.text_pos;
    tap->row = save_pos.row;
    tap->column = save_pos.column;

abort:;
}

void mx_wp_cursor::insert_bullets(int& err)
{
    mx_paragraph_style style;
    mx_wp_doc_pos start_para = *selection_start(), end_para = *selection_end();
    mx_wp_style_iterator iter(document, start_para, end_para);

    mx_paragraph* p;

    mx_word* w;
    mx_para_pos pp;
    mx_ruler* ruler;

    const unsigned char* str;

    mx_char_style *style_before, style_after;
    bool got_bullet_style = FALSE;

    // deactivate the active area
    active_area->deactivate();

    save_selection_for_undo(err);
    MX_ERROR_CHECK(err);

    is_selection = FALSE;

    do {
        MX_ERROR_CHECK(err);

        p = (mx_paragraph*)iter.data(err);
        MX_ERROR_CHECK(err);

        if (p->is_a(mx_paragraph_class_e)) {
            pp.moveto_start();

            style_before = p->get_style(err, pp);
            MX_ERROR_CHECK(err);

            if (!got_bullet_style) {
                got_bullet_style = TRUE;

                style_after.get_font()->set_size(style_before->get_font()->get_size());

                style_after.colour = style_before->colour;

                style_after.get_font()->set_style(err, mx_normal);
                MX_ERROR_CHECK(err);

                style_after.get_font()->set_family(err, mx_font::get_default_symbol_font());
                MX_ERROR_CHECK(err);
            }

            // get the first word out
            w = p->get_word(err, pp);
            MX_ERROR_CHECK(err);

            str = (unsigned char*)(w->cstring());
            if ((str != NULL) && (str[0] != 183)) {
                mx_char_style_mod mod_to(*style_before, style_after);
                mx_char_style_mod mod_back(style_after, *style_before);

                p->insert_char(err, 183, pp);
                MX_ERROR_CHECK(err);

                p->insert_mod(err, mod_to, pp);
                MX_ERROR_CHECK(err);

                pp.right(err, p);
                MX_ERROR_CHECK(err);

                p->insert_mod(err, mod_back, pp);
                MX_ERROR_CHECK(err);

                p->insert_char(err, '\t', pp);
                MX_ERROR_CHECK(err);

                style = *(p->get_paragraph_style());
                if (style.justification != mx_left_aligned && style.justification != mx_justified) {
                    style.justification = mx_left_aligned;
                }
                ruler = style.get_ruler();
                ruler->left_indent = MX_RULER_DEFAULT_TABSTOP;
                ruler->first_line_indent = -MX_RULER_DEFAULT_TABSTOP;

                p->modify_paragraph_style(style);
            }
        }
    } while (iter.more(err));
    MX_ERROR_CHECK(err);

    start_para.moveto_start_para(err, document);
    MX_ERROR_CHECK(err);

    end_para.moveto_end_para(err, document);
    MX_ERROR_CHECK(err);

    // make sure the position is valid
    position.moveto_start_para(err, document);
    MX_ERROR_CHECK(err);

    this->reformat_and_restore_position(err, start_para, end_para, TRUE);
    MX_ERROR_CHECK(err);

    frame->refresh(err);
    MX_ERROR_CHECK(err);

    update_caret(err);
    MX_ERROR_CHECK(err);
abort:;
}

void mx_wp_cursor::insert_text(int& err, const char* string)
{
    remove_selection(err);
    MX_ERROR_CHECK(err);

    document->insert_text(err, *get_position(), string);
    MX_ERROR_CHECK(err);
abort:;
}

void mx_wp_cursor::insert_space(int& err)
{
    document->insert_char(err, ' ', position);
    MX_ERROR_CHECK(err);

    position.right(err, document);
    MX_ERROR_CHECK(err);

abort:;
}

void mx_wp_cursor::insert_tab(int& err)
{
    document->insert_char(err, '\t', position);
    MX_ERROR_CHECK(err);

    position.right(err, document);
    MX_ERROR_CHECK(err);

abort:;
}

void mx_wp_cursor::insert_para_break(int& err)
{
    document->insert_para_break(err, position);
    MX_ERROR_CHECK(err);

    position.next_para(err, document);
    MX_ERROR_CHECK(err);

abort:;
}

void mx_wp_cursor::check_for_empty_document(int& err)
{
    int ns;

    mx_sheet* s;
    mx_text_area* a;

    ns = document->get_num_sheets(err);
    MX_ERROR_CHECK(err);

    if (ns == 0) {
        s = document->add_sheet(err);
        MX_ERROR_CHECK(err);

        s->set_width(err, cut_sheet_width);
        MX_ERROR_CHECK(err);

        s->set_width(err, cut_sheet_height);
        MX_ERROR_CHECK(err);

        s->set_margins(err,
            cut_sheet_left_margin,
            cut_sheet_right_margin,
            cut_sheet_top_margin,
            cut_sheet_bottom_margin);
        MX_ERROR_CHECK(err);
    } else {
        s = document->sheet(err, position.get_sheet());
        MX_ERROR_CHECK(err);
    }

    if (s->get_num_areas() == 0) {
        a = s->add_text_area(err);
        MX_ERROR_CHECK(err);

        a->set_outline(err,
            cut_sheet_width - (cut_sheet_left_margin + cut_sheet_right_margin),
            1.0);

        position.reset(err, document);
        MX_ERROR_CHECK(err);
    }
abort:;
}

void mx_wp_cursor::remove_whole_table(int& err)
{
    mx_sheet* s;
    int ns, na;

    s = document->sheet(err, position.get_sheet());
    MX_ERROR_CHECK(err);

    na = s->get_num_areas();

    document->remove_area(err, &position);
    MX_ERROR_CHECK(err);

    // is the sheet empty now?
    if (na == 1) {
        // is the document empty ?
        ns = document->get_num_sheets(err);
        MX_ERROR_CHECK(err);

        if (ns == 0) {
            check_for_empty_document(err);
            MX_ERROR_CHECK(err);
        } else {
            // back to end of last sheet
            position.back_sheet(err);
            MX_ERROR_CHECK(err);

            position.moveto_end_sheet(err, document);
            MX_ERROR_CHECK(err);
        }
    } else {
        position.back_area(err, document);
        MX_ERROR_CHECK(err);

        position.reset(err, document);
        MX_ERROR_CHECK(err);
    }
    is_selection = FALSE;
abort:;
}
