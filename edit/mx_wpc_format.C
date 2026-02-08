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
#include <mx_cw_t.h>
#include <mx_frame.h>
#include <mx_mod_iter.h>
#include <mx_para_pos.h>
#include <mx_paragraph.h>
#include <mx_sc_device.h>
#include <mx_ta_pos.h>
#include <mx_txt_pos.h>
#include <mx_undo.h>
#include <mx_wp_doc.h>
#include <mx_wp_pos.h>
#include <mx_wp_style_iter.h>

static void apply_mods_in_selection(int& err,
    mx_wp_document* document,
    mx_wp_doc_pos& start_pos,
    mx_wp_doc_pos& end_pos,
    const mx_paragraph_style_mod& mod_to_apply)
{
    bool para_mod_is_null, char_mod_is_null;
    mx_paragraph *start_para = NULL, *end_para = NULL;
    mx_para_pos *start_para_pos = NULL, *end_para_pos = NULL;
    mx_paragraph_style_mod psm = mod_to_apply;
    mx_char_style_mod csm = *psm.get_char_style_mod();
    mx_paragraph* para;
    mx_wp_style_iterator para_iter(document, start_pos, end_pos);

    // make sure the paragraph style mod has no component character or border
    // style mod
    *psm.get_char_style_mod() = mx_char_style_mod();
    *psm.get_border_style_mod() = mx_border_style_mod();

    MX_ERROR_ASSERT(err, end_pos >= start_pos);

    // get start and end paragraphs, and positions

    start_para = start_pos.get_para(err, document);
    MX_ERROR_CHECK(err);

    end_para = end_pos.get_para(err, document);
    MX_ERROR_CHECK(err);

    start_para_pos = start_pos.get_para_pos(err);
    MX_ERROR_CHECK(err);

    end_para_pos = end_pos.get_para_pos(err);
    MX_ERROR_CHECK(err);

    para_mod_is_null = psm.is_null();
    char_mod_is_null = csm.is_null();

    do {
        MX_ERROR_CHECK(err);
        para = (mx_paragraph*)para_iter.data(err);
        MX_ERROR_CHECK(err);

        if (para->is_a(mx_paragraph_class_e)) {
            mx_para_pos start_pp, end_pp;

            start_pp.moveto_start();
            end_pp.moveto_end(err, para);
            MX_ERROR_CHECK(err);

            if (para == start_para)
                start_pp = *start_para_pos;
            if (para == end_para)
                end_pp = *end_para_pos;

            if (!para_mod_is_null)
                para->modify_paragraph_style(psm);

            if (!char_mod_is_null) {
                // do the character style mods
                mx_style_mod_iterator mod_iter(para, start_pp, end_pp);
                while (mod_iter.more())
                    mod_iter.data()->clear_mods_in(csm);

                mx_word::set_operations_to_right_of_mods();
                para->aka_insert_mod(err, csm, start_pp);
                MX_ERROR_CHECK(err);
            }
        }
    } while (para_iter.more(err));
    MX_ERROR_CHECK(err);
abort:;
}

void mx_wp_cursor::apply_mod_to_current_style(int& err,
    const mx_paragraph_style_mod& mod)
{
    mx_wp_doc_pos* start_pos = selection_start();
    mx_wp_doc_pos* end_pos = selection_end();
    mx_paragraph_style_mod psm = mod;
    bool need_to_reformat = TRUE;

    // deactivate any active area
    active_area->deactivate();

    if (!is_selection) {
        // just change the current style ready for the next insertion
        bool b;

        current_style += *psm.get_char_style_mod();
        *psm.get_char_style_mod() = mx_char_style_mod();

        b = position.is_paragraph(err);
        MX_ERROR_CHECK(err);

        if (!b || psm.is_null()) {
            need_to_reformat = FALSE;
        } else {
            mx_paragraph* para = position.get_para(err, document);
            MX_ERROR_CHECK(err);
            para->modify_paragraph_style(psm);
        }
    } else {
        // apply the mod immediately to the selection
        mx_paragraph_style* ps;
        mx_char_style start_cs, end_cs;

        need_to_reformat = TRUE;

        // save the old stuff for undo
        save_selection_for_undo(err);
        MX_ERROR_CHECK(err);

        ps = document->get_para_style(err, start_pos);
        MX_ERROR_CHECK(err);
        start_cs = *ps->get_char_style();

        ps = document->get_para_style(err, end_pos);
        MX_ERROR_CHECK(err);
        end_cs = *ps->get_char_style();

        if (!psm.get_border_style_mod()->is_null()) {
            apply_mod_to_current_style(err, *psm.get_border_style_mod());
            MX_ERROR_CHECK(err);
        }

        apply_mods_in_selection(err, document, *start_pos, *end_pos, mod);
        MX_ERROR_CHECK(err);

        start_cs += *(((mx_paragraph_style_mod&)mod).get_char_style_mod());

        document->insert_cw_item(err, new mx_style_mod_cw_t(mx_char_style_mod(start_cs, end_cs)),
            *end_pos);
        MX_ERROR_CHECK(err);
    }

    if (need_to_reformat) {
        mx_wp_doc_pos start_of_para_pos = *start_pos, end_of_doc_pos = *end_pos;

        start_of_para_pos.moveto_start_para(err, document);
        MX_ERROR_CHECK(err);

        end_of_doc_pos.moveto_end_sheet(err, document);
        MX_ERROR_CHECK(err);

        reformat_and_restore_position(err, start_of_para_pos, end_of_doc_pos,
            TRUE);
        MX_ERROR_CHECK(err);

        update_caret(err);
        MX_ERROR_CHECK(err);

        frame->refresh(err);
        MX_ERROR_CHECK(err);
    }

    device->take_focus();
abort:;
}

// split a border style into its component parts. Start should be applied to
// the first paragraph in a selection, middle applied to all paragraphs between
// start and end, end applied just to the last one.
static void get_start_middle_end_border_mods(const mx_border_style_mod& original,
    mx_border_style_mod& start,
    mx_border_style_mod& middle,
    mx_border_style_mod& end)
{
    mx_line_style no_line_ls(mx_no_line, 0.0f);
    bool b;
    mx_line_style middle_ls;

    start = middle = end = original;

    b = original.get_horizontal_style_mod(middle_ls);

    if (b) {
        start.set_bottom_style_mod(no_line_ls);

        middle.set_top_style_mod(middle_ls);
        middle.set_bottom_style_mod(no_line_ls);

        end.set_top_style_mod(middle_ls);
    } else {
        start.clear_bottom_style_mod();

        middle.clear_top_style_mod();
        middle.clear_bottom_style_mod();

        end.clear_top_style_mod();
    }
}

static void apply_border_mods_in_text_selection(
    int& err, mx_wp_document* document,
    mx_wp_doc_pos& start_pos, mx_wp_doc_pos& end_pos,
    const mx_border_style_mod& mod_to_apply)
{
    mx_border_style_mod start_bsm, middle_bsm, end_bsm;
    mx_paragraph *start_para = NULL, *end_para = NULL, *para;
    mx_paragraph_style_mod psm;
    mx_wp_style_iterator para_iter(document, start_pos, end_pos);

    get_start_middle_end_border_mods(mod_to_apply, start_bsm,
        middle_bsm, end_bsm);

    MX_ERROR_ASSERT(err, end_pos >= start_pos);

    // get start/end paragraphs

    start_para = start_pos.get_para(err, document);
    MX_ERROR_CHECK(err);

    end_para = end_pos.get_para(err, document);
    MX_ERROR_CHECK(err);

    // just apply the original mod if we are only in one paragraph
    if (start_para == end_para) {
        *psm.get_border_style_mod() = mod_to_apply;
        start_para->modify_paragraph_style(psm);
        return;
    }

    // apply border at start and end positions

    *psm.get_border_style_mod() = start_bsm;
    start_para->modify_paragraph_style(psm);

    *psm.get_border_style_mod() = end_bsm;
    end_para->modify_paragraph_style(psm);

    // apply middle border at all positions

    *psm.get_border_style_mod() = middle_bsm;
    if (psm.is_null())
        return;

    do {
        MX_ERROR_CHECK(err);
        para = (mx_paragraph*)para_iter.data(err);
        MX_ERROR_CHECK(err);

        if (para->is_a(mx_paragraph_class_e) && para != start_para && para != end_para) {
            para->modify_paragraph_style(psm);
        }
    } while (para_iter.more(err));
    MX_ERROR_CHECK(err);
abort:;
}

static void apply_border_mods_in_table_selection(
    int& err, mx_wp_document* document,
    mx_wp_doc_pos& start_pos, mx_wp_doc_pos& end_pos,
    const mx_border_style_mod& mod_to_apply)
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

    taba1->apply_border_mod_to_selection(err, *tabp1, *tabp2, mod_to_apply);
    MX_ERROR_CHECK(err);
abort:;
}

void mx_wp_cursor::apply_mod_to_current_style(int& err,
    const mx_border_style_mod& mod)
{
    // deactivate any active area
    active_area->deactivate();

    if (!selection()) {
        mx_paragraph_style_mod m;
        mx_paragraph* para = position.get_para(err, document);
        MX_ERROR_CHECK(err);

        save_para_borders_for_undo(err);
        MX_ERROR_CHECK(err);

        *m.get_border_style_mod() = mod;
        para->modify_paragraph_style(m);
    } else if (selection_only_covers_text()) {
        save_para_borders_for_undo(err);
        MX_ERROR_CHECK(err);

        apply_border_mods_in_text_selection(err, document, *selection_start(),
            *selection_end(), mod);
        MX_ERROR_CHECK(err);
    } else if (selection_only_covers_table()) {
        save_table_borders_for_undo(err);
        MX_ERROR_CHECK(err);

        apply_border_mods_in_table_selection(err, document, *selection_start(),
            *selection_end(), mod);
        MX_ERROR_CHECK(err);
    } else {
        // shouldn't be here
        MX_ERROR_THROW(err, MX_ERROR_UNDEFINED);
    }
abort:;
}

static void apply_tab_change_to_para(int& err, mx_paragraph* para,
    mx_wp_cursor::mx_tab_change op,
    const mx_tabstop& tab,
    float tolerance)
{
    mx_paragraph_style ps = *para->get_paragraph_style();

    if (op == mx_wp_cursor::mx_add_tab_e) {
        mx_tabstop t = tab;
        ps.get_ruler()->add_tab(err, t);
    } else {
        ps.get_ruler()->remove_tab_tolerance(err, tab.position, tolerance);
    }
    MX_ERROR_CLEAR(err);

    para->modify_paragraph_style(ps);
}

static void apply_tab_change_to_selection(
    int& err, mx_wp_document* document,
    mx_wp_doc_pos& start_pos, mx_wp_doc_pos& end_pos,
    mx_wp_cursor::mx_tab_change op, const mx_tabstop& tab, float tolerance)
{
    mx_wp_style_iterator para_iter(document, start_pos, end_pos);

    do {
        MX_ERROR_CHECK(err);
        mx_paragraph* para = (mx_paragraph*)para_iter.data(err);
        MX_ERROR_CHECK(err);

        if (para->is_a(mx_paragraph_class_e)) {
            apply_tab_change_to_para(err, para, op, tab, tolerance);
            MX_ERROR_CHECK(err);
        }
    } while (para_iter.more(err));
    MX_ERROR_CHECK(err);
abort:;
}

void mx_wp_cursor::apply_tab_change_to_current_style(
    int& err, mx_tab_change op, const mx_tabstop& tab, float tolerance)
{
    save_selection_for_undo(err);
    MX_ERROR_CHECK(err);

    // deactivate any active area
    active_area->deactivate();

    if (selection()) {
        apply_tab_change_to_selection(err, document, *selection_start(),
            *selection_end(), op, tab, tolerance);
        MX_ERROR_CHECK(err);
    } else {
        mx_paragraph* para = position.get_para(err, document);
        MX_ERROR_CHECK(err);

        apply_tab_change_to_para(err, para, op, tab, tolerance);
        MX_ERROR_CHECK(err);
    }

    {
        mx_wp_doc_pos start_of_para_pos = *selection_start();
        ;
        mx_wp_doc_pos end_of_doc_pos = *selection_end();

        start_of_para_pos.moveto_start_para(err, document);
        MX_ERROR_CHECK(err);

        end_of_doc_pos.moveto_end_sheet(err, document);
        MX_ERROR_CHECK(err);

        reformat_and_restore_position(err, start_of_para_pos, end_of_doc_pos, TRUE);
        MX_ERROR_CHECK(err);

        update_caret(err);
        MX_ERROR_CHECK(err);

        frame->refresh(err);
        MX_ERROR_CHECK(err);
    }

    device->take_focus();
abort:;
}

void mx_wp_cursor::apply_style_change_to_current_style(
    int& err, const char* new_style_name)
{
    mx_wp_style_iterator para_iter(document, *selection_start(),
        *selection_end());
    mx_paragraph_style* named_style;

    // deactivate any active area
    active_area->deactivate();

    named_style = document->get_named_style(err, (char*)new_style_name);
    MX_ERROR_CHECK(err);

    if (strcmp(new_style_name, named_style->get_name()) != 0)
        return;

    do {
        MX_ERROR_CHECK(err);
        mx_paragraph* para = (mx_paragraph*)para_iter.data(err);
        MX_ERROR_CHECK(err);

        para->set_paragraph_style(named_style);
    } while (para_iter.more(err));

    MX_ERROR_CHECK(err);

    {
        mx_paragraph_style_mod mod(*named_style);
        *mod.get_border_style_mod() = mx_border_style_mod();

        this->apply_mod_to_current_style(err, mod);
        MX_ERROR_CHECK(err);
    }

abort:;
}

void mx_wp_cursor::save_selection_for_undo(int& err)
{
    mx_wp_doc_pos start_pos = *selection_start();
    mx_wp_doc_pos end_pos = *selection_end();

    mx_wp_style_iterator para_iter(document, start_pos, end_pos);

    mx_list* paras = new mx_list();
    mx_ur_item* item;

    mx_paragraph* copy;

    do {
        copy = (mx_paragraph*)para_iter.data(err);
        MX_ERROR_CHECK(err);

        paras->append(new mx_paragraph(*copy));
    } while (para_iter.more(err));
    MX_ERROR_CHECK(err);

    item = new mx_ur_item(mx_undo_format_e, document, start_pos, end_pos, paras);

    ur_buffer->insert(err, item);
    MX_ERROR_CHECK(err);
abort:;
}

void mx_wp_cursor::save_para_borders_for_undo(int& err)
{
    mx_wp_doc_pos start_pos = *selection_start();
    mx_wp_doc_pos end_pos = *selection_end();

    mx_wp_style_iterator para_iter(document, start_pos, end_pos);

    mx_list* paras = new mx_list();
    mx_ur_item* item;

    mx_border_style* copy;
    mx_paragraph* para;

    do {
        para = (mx_paragraph*)para_iter.data(err);
        MX_ERROR_CHECK(err);

        copy = para->get_paragraph_style()->get_border_style();

        paras->append(new mx_border_style(*copy));
    } while (para_iter.more(err));
    MX_ERROR_CHECK(err);

    item = new mx_ur_item(mx_undo_format_borders_e, document, start_pos, end_pos, paras, 0);

    ur_buffer->insert(err, item);
    MX_ERROR_CHECK(err);
abort:;
}

void mx_wp_cursor::save_table_borders_for_undo(int& err)
{
    int row, column;

    mx_wp_doc_pos start_pos = *selection_start();
    mx_wp_doc_pos end_pos = *selection_end();

    mx_table_area_pos* tap1;
    mx_table_area_pos* tap2;

    mx_table_area* table;
    mx_text_area* text;

    mx_border_style* copy;

    mx_list* styles = new mx_list();
    mx_ur_item* item;

    tap1 = (mx_table_area_pos*)start_pos.get_area_pos(err);
    MX_ERROR_CHECK(err);

    tap2 = (mx_table_area_pos*)end_pos.get_area_pos(err);
    MX_ERROR_CHECK(err);

    table = (mx_table_area*)start_pos.get_area(err, document);
    MX_ERROR_CHECK(err);

    MX_ERROR_ASSERT(err, tap1->rtti_class() == mx_table_area_pos_class_e);
    MX_ERROR_ASSERT(err, tap2->rtti_class() == mx_table_area_pos_class_e);

    for (row = tap1->row; row <= tap2->row; row++) {
        for (column = tap1->column; column <= tap2->column; column++) {
            text = table->get_cell(err, row, column);
            MX_ERROR_CHECK(err);

            copy = text->get_paragraph_style()->get_border_style();

            styles->append(new mx_border_style(*copy));
        }
    }

    item = new mx_ur_item(
        mx_undo_format_borders_e,
        document,
        start_pos,
        end_pos,
        styles,
        tap2->row - tap1->row + 1,
        tap2->column - tap1->column + 1);

    ur_buffer->insert(err, item);
    MX_ERROR_CHECK(err);

abort:;
}

void mx_wp_cursor::apply_mod_to_current_table_style(
    int& err,
    const mx_paragraph_style_mod& mod)
{
    mx_wp_doc_pos start_pos = *selection_start();
    mx_wp_doc_pos end_pos = *selection_end();
    mx_wp_doc_pos p;

    mx_paragraph_style ps;

    save_area_styles_for_undo(err);
    MX_ERROR_CHECK(err);

    // deactivate any active area
    active_area->deactivate();

    find_table_bounds(err, start_pos, end_pos);
    MX_ERROR_CHECK(err);

    p = start_pos;

    while (p <= end_pos) {
        mx_area* a;

        a = p.get_area(err, document);
        MX_ERROR_CHECK(err);

        ps = *(a->get_paragraph_style());
        ps += mod;
        a->modify_paragraph_style(ps);

        p.next_area(err, document);
        MX_ERROR_CHECK(err);
    }

    {
        mx_wp_doc_pos start_of_sh_pos = *selection_start();
        ;
        mx_wp_doc_pos end_of_sh_pos = *selection_end();

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

void mx_wp_cursor::find_table_bounds(
    int& err,
    mx_wp_doc_pos& start_pos,
    mx_wp_doc_pos& end_pos)
{
    mx_wp_doc_pos temp_pos;
    mx_area* a;

    while (TRUE) {
        temp_pos = start_pos;

        temp_pos.prev_area(err, document);
        if (err == MX_POSITION_RANGE_ERROR) {
            MX_ERROR_CLEAR(err);
            break;
        } else {
            MX_ERROR_CHECK(err);
        }

        a = temp_pos.get_area(err, document);
        MX_ERROR_CHECK(err);

        if (a->rtti_class() == mx_table_area_class_e) {
            start_pos = temp_pos;
        } else {
            break;
        }
    }

    while (TRUE) {
        temp_pos = end_pos;

        temp_pos.next_area(err, document);
        if (err == MX_POSITION_RANGE_ERROR) {
            MX_ERROR_CLEAR(err);
            break;
        } else {
            MX_ERROR_CHECK(err);
        }

        a = temp_pos.get_area(err, document);
        MX_ERROR_CHECK(err);

        if (a->rtti_class() == mx_table_area_class_e) {
            end_pos = temp_pos;
        } else {
            break;
        }
    }
abort:;
}

void mx_wp_cursor::save_area_styles_for_undo(int& err)
{
    mx_wp_doc_pos start_pos = *selection_start();
    mx_wp_doc_pos end_pos = *selection_end();
    mx_wp_doc_pos pos = start_pos;

    mx_list* styles = new mx_list();
    mx_ur_item* item;

    mx_area* a;
    mx_paragraph_style* ps;

    find_table_bounds(err, start_pos, end_pos);
    MX_ERROR_CHECK(err);

    while (pos <= end_pos) {
        a = pos.get_area(err, document);
        MX_ERROR_CHECK(err);

        ps = a->get_paragraph_style();

        styles->append(new mx_paragraph_style(*ps));

        pos.next_area(err, document);
        MX_ERROR_CHECK(err);
    }

    item = new mx_ur_item(mx_undo_area_style_change_e, document, start_pos, end_pos, styles);

    ur_buffer->insert(err, item);
    MX_ERROR_CHECK(err);
abort:;
}

mx_char_style* mx_wp_cursor::get_current_style()
{
    return &current_style;
}

void mx_wp_cursor::set_current_style(const mx_char_style& cs)
{
    current_style = cs;
}
