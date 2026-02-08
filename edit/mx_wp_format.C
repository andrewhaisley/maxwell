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
 * MODULE/CLASS :  mx_wp_editor
 *
 * AUTHOR : Andrew Haisley
 *
 *
 *
 * DESCRIPTION:
 *
 * Various format related routines for the wp editor
 *
 *
 *
 *
 */

#include <mx_wp_editor.h>

#include <mx_border_d.h>
#include <mx_cell_d.h>
#include <mx_char_d.h>
#include <mx_page_d.h>
#include <mx_para_d.h>
#include <mx_style_d.h>
#include <mx_tab_d.h>
#include <mx_tabf_d.h>
#include <mx_wp_menubar.h>
#include <mx_wp_toolbar.h>

#include <mx_list_iter.h>
#include <mx_nlist_iter.h>
#include <mx_paragraph.h>
#include <mx_undo.h>
#include <mx_wp_doc.h>
#include <mx_xframe.h>

void mx_wp_editor::format_image(int& err) { }
void mx_wp_editor::format_diagram(int& err) { }
void mx_wp_editor::format_set_underline(int& err, bool s)
{
    set_underline(err, s);
}

void mx_wp_editor::format_set_bold(int& err, bool s)
{
    set_bold(err, s);
}

void mx_wp_editor::format_set_italic(int& err, bool s)
{
    set_italic(err, s);
}

void mx_wp_editor::format_character(int& err)
{
    mx_char_d* d = dialog_man.get_char_d();
    mx_char_style** style_array = NULL;
    mx_paragraph_style_mod mod;
    int n, i;
    mx_list style_list;
    int dialogue_result;

    d->centre();
    do {
        cursor.styles_in_selection(err, style_list);
        MX_ERROR_CHECK(err);

        n = style_list.get_num_items();

        style_array = new mx_char_style*[n];

        for (i = 0; i < n; i++) {
            mx_paragraph_style* ps = (mx_paragraph_style*)style_list.get(err, i);
            MX_ERROR_CHECK(err);
            style_array[i] = ps->get_char_style();
        }

        d->activate_d(style_array, n, *mod.get_char_style_mod());
        dialogue_result = d->run_modal();
        if (dialogue_result != cancel_e) {
            cursor.apply_mod_to_current_style(err, mod);
            MX_ERROR_CHECK(err);
        }

        {
            mx_list_iterator iter(style_list);
            while (iter.more())
                delete (mx_paragraph_style*)iter.data();
            delete[] style_array;
        }
    } while (dialogue_result == apply_e);

    update_all_sensitivities(err);
    MX_ERROR_CHECK(err);

    update_toolbar(err);
    MX_ERROR_CHECK(err);

abort:
    d->deactivate();
}

void mx_wp_editor::format_border(int& err)
{
    mx_border_d* d = dialog_man.get_border_d();
    mx_paragraph_style_mod mod;
    mx_nlist style_array;
    int dialogue_result;

    MX_ERROR_ASSERT(err, (cursor.selection_only_covers_text() || cursor.selection_only_covers_table()));

    d->centre();
    do {
        cursor.border_styles_in_selection(err, style_array);
        MX_ERROR_CHECK(err);

        d->activate_d(style_array, current_units, *mod.get_border_style_mod());
        dialogue_result = d->run_modal();
        if (dialogue_result != cancel_e) {
            cursor.apply_mod_to_current_style(err, mod);
            MX_ERROR_CHECK(err);
        }

        {
            mx_nlist_iterator iter(style_array);
            while (iter.more())
                delete (mx_border_style*)iter.data();
        }
    } while (dialogue_result == apply_e);

    update_all_sensitivities(err);
    MX_ERROR_CHECK(err);

    update_toolbar(err);
    MX_ERROR_CHECK(err);

abort:
    d->deactivate();
}

void mx_wp_editor::format_tabs(int& err)
{
    mx_tab_d* d = dialog_man.get_tab_d();
    mx_paragraph_style_mod mod;
    int dialogue_result;
    bool b;
    mx_wp_doc_pos* sel_s = cursor.selection_start();
    mx_wp_doc_pos* sel_e = cursor.selection_end();

    b = sel_s->is_same_para_as(err, sel_e);
    MX_ERROR_CHECK(err);
    MX_ERROR_ASSERT(err, b);

    d->centre();
    do {
        mx_paragraph* para = sel_s->get_para(err, document);
        MX_ERROR_CHECK(err);

        d->activate_d(*para->get_paragraph_style()->get_ruler(), current_units);
        dialogue_result = d->run_modal();
        if (dialogue_result != cancel_e) {
            mod.set_ruler_mod(d->ruler);
            cursor.apply_mod_to_current_style(err, mod);
            MX_ERROR_CHECK(err);
        }
    } while (dialogue_result == apply_e);

    update_all_sensitivities(err);
    MX_ERROR_CHECK(err);

    update_toolbar(err);
    MX_ERROR_CHECK(err);

abort:
    d->deactivate();
}

void mx_wp_editor::format_style(int& err)
{
    mx_wp_document* doc = (mx_wp_document*)get_document();
    mx_paragraph_style *styles[MX_DOCUMENT_MAX_STYLES], *tmp;

    mx_style_d* d = NULL;
    mx_ur_item* undo_item;

    int n = 0, i = 0;

    n = document->get_num_styles(err);
    MX_ERROR_CHECK(err);

    // make a copy of the style list
    for (i = 0; i < n; i++) {
        tmp = document->get_style(err, i);
        MX_ERROR_CHECK(err);

        styles[i] = new mx_paragraph_style(*tmp);
        MX_ERROR_CHECK(err);
    }

    // make an undo item - in case it's needed
    undo_item = new mx_ur_item(
        mx_undo_named_style_change_e,
        document,
        styles,
        n);

    d = dialog_man.get_style_d();
    if (d->run(styles, &n, MX_DOCUMENT_MAX_STYLES, current_units) == yes_e) {
        ur_buffer->insert(err, undo_item);
        MX_ERROR_CHECK(err);

        document->set_style_list(err, styles, n);
        MX_ERROR_CHECK(err);

        toolbar->set_style_pulldown(doc);

        cursor.get_position()->save(err, doc);
        MX_ERROR_CHECK(err);

        doc->reformat(err);
        MX_ERROR_CHECK(err);

        set_sheet_layout(err, doc);
        MX_ERROR_CHECK(err);

        frame->resetSheetLayout(err);
        MX_ERROR_CHECK(err);

        frame->refresh(err);
        MX_ERROR_CHECK(err);

        cursor.get_position()->restore(err, doc);
        MX_ERROR_CHECK(err);

        cursor.update_caret(err);
        MX_ERROR_CHECK(err);

    } else {
        delete undo_item;
    }

    for (i = 0; i < n; i++)
        delete styles[i];

    update_all_sensitivities(err);
    MX_ERROR_CHECK(err);

    update_toolbar(err);
    MX_ERROR_CHECK(err);
abort:;
}

void mx_wp_editor::format_page_setup(int& err)
{
    mx_page_d* d;

    float width, height;
    float l, r, t, b;
    bool landscape;

    document->get_page_size(err, width, height, landscape);
    MX_ERROR_CHECK(err);

    document->get_page_margins(err, l, r, t, b);
    MX_ERROR_CHECK(err);

    d = dialog_man.get_page_d();

    if (d->run(width, height, l, r, t, b, landscape, current_units) == yes_e) {
        mx_ur_item* item;

        item = new mx_ur_item(
            mx_undo_page_setup_e,
            document,
            width,
            height,
            t, b, l, r,
            landscape,
            *cursor.get_position());

        ur_buffer->insert(err, item);
        MX_ERROR_CHECK(err);

        if (d->is_landscape) {
            document->set_page_size(err, d->height, d->width, d->is_landscape);
        } else {
            document->set_page_size(err, d->width, d->height, d->is_landscape);
        }
        MX_ERROR_CHECK(err);

        document->set_page_margins(err, d->left_margin, d->right_margin,
            d->top_margin, d->bottom_margin);
        MX_ERROR_CHECK(err);

        {
            mx_wp_doc_pos s, e;

            s.moveto_start(err, document);
            MX_ERROR_CHECK(err);

            e.moveto_end(err, document);
            MX_ERROR_CHECK(err);

            cursor.reformat_and_restore_position(err, s, e, TRUE);
            MX_ERROR_CHECK(err);
        }

        frame->refresh(err);
        MX_ERROR_CHECK(err);

        cursor.sheet_size_changed(err);
        MX_ERROR_CHECK(err);

        cursor.update_caret(err);
        MX_ERROR_CHECK(err);

        update_toolbar(err);
        MX_ERROR_CHECK(err);

        update_file_sensitivities(err);
        MX_ERROR_CHECK(err);
    }
abort:;
}

void mx_wp_editor::format_columns(int& err)
{
}

void mx_wp_editor::format_table(int& err)
{
    mx_cell_d* d = dialog_man.get_cell_d();

    if (cursor.selection_has_table()) {
        bool have_set_width = FALSE;
        float width_set = 0.0f;
        int res = yes_e;
        d->centre();

        do {
            float width = cursor.get_selected_column_width(err);
            MX_ERROR_CHECK(err);

            d->activate_d(width, current_units);
            res = d->run_modal();

            if (res != cancel_e && !(have_set_width && MX_FLOAT_EQ(width_set, d->width))) {
                have_set_width = TRUE;
                width_set = d->width;

                cursor.set_selected_column_width(err, d->width);
                MX_ERROR_CHECK(err);

                update_toolbar(err);
                MX_ERROR_CHECK(err);

                update_all_sensitivities(err);
                MX_ERROR_CHECK(err);
            }
        } while (res == apply_e);
    }
abort:
    d->deactivate();
}

void mx_wp_editor::format_table_style(int& err)
{
    mx_tabf_d* d = dialog_man.get_tabf_d();

    mx_paragraph_style** style_array = NULL;
    mx_paragraph_style_mod mod;

    int n, i;
    mx_list style_list;
    int dialogue_result;

    d->centre();
    do {
        cursor.table_styles_in_selection(err, style_list);
        MX_ERROR_CHECK(err);

        n = style_list.get_num_items();

        style_array = new mx_paragraph_style*[n];

        for (i = 0; i < n; i++) {
            style_array[i] = (mx_paragraph_style*)style_list.get(err, i);
            MX_ERROR_CHECK(err);
        }

        d->activate_d(style_array, n, current_units, mod);

        dialogue_result = d->run_modal();
        if (dialogue_result != cancel_e) {
            cursor.apply_mod_to_current_table_style(err, mod);
            MX_ERROR_CHECK(err);

            update_all_sensitivities(err);
            MX_ERROR_CHECK(err);

            update_toolbar(err);
            MX_ERROR_CHECK(err);
        }

        {
            mx_list_iterator iter(style_list);

            while (iter.more()) {
                delete (mx_paragraph_style*)iter.data();
            }
            delete[] style_array;
        }
    } while (dialogue_result == apply_e);

abort:
    d->deactivate();
}

void mx_wp_editor::set_bold(int& err, bool s)
{
    mx_paragraph_style_mod mod;
    mod.get_char_style_mod()->get_font_mod()->set_bold_mod(s);

    cursor.apply_mod_to_current_style(err, mod);
    MX_ERROR_CHECK(err);

    update_all_sensitivities(err);
    MX_ERROR_CHECK(err);

    update_toolbar(err);
    MX_ERROR_CHECK(err);
abort:;
}

void mx_wp_editor::set_italic(int& err, bool s)
{
    mx_paragraph_style_mod mod;
    mod.get_char_style_mod()->get_font_mod()->set_italic_mod(s);

    cursor.apply_mod_to_current_style(err, mod);
    MX_ERROR_CHECK(err);

    update_all_sensitivities(err);
    MX_ERROR_CHECK(err);

    update_toolbar(err);
    MX_ERROR_CHECK(err);
abort:;
}

void mx_wp_editor::set_underline(int& err, bool s)
{
    mx_paragraph_style_mod mod;
    mod.get_char_style_mod()->set_effects_mod(s ? mx_underline : mx_no_effects);

    cursor.apply_mod_to_current_style(err, mod);
    MX_ERROR_CHECK(err);

    update_all_sensitivities(err);
    MX_ERROR_CHECK(err);

    update_toolbar(err);
    MX_ERROR_CHECK(err);
abort:;
}

void mx_wp_editor::set_font_size(int& err, float s)
{
    mx_paragraph_style_mod mod;
    mod.get_char_style_mod()->get_font_mod()->set_size_mod(s);

    cursor.apply_mod_to_current_style(err, mod);
    MX_ERROR_CHECK(err);

    update_all_sensitivities(err);
    MX_ERROR_CHECK(err);

    update_toolbar(err);
    MX_ERROR_CHECK(err);
abort:;
}

void mx_wp_editor::set_font_family(int& err, const char* name)
{
    mx_paragraph_style_mod mod;
    mod.get_char_style_mod()->get_font_mod()->set_family_mod(name);

    cursor.apply_mod_to_current_style(err, mod);
    MX_ERROR_CHECK(err);

    update_all_sensitivities(err);
    MX_ERROR_CHECK(err);

    update_toolbar(err);
    MX_ERROR_CHECK(err);
abort:;
}

void mx_wp_editor::set_justification(int& err, mx_justification_t j)
{
    mx_paragraph_style_mod mod;
    mod.set_justification_mod(j);

    cursor.apply_mod_to_current_style(err, mod);
    MX_ERROR_CHECK(err);

    update_all_sensitivities(err);
    MX_ERROR_CHECK(err);

    update_toolbar(err);
    MX_ERROR_CHECK(err);
abort:;
}

void mx_wp_editor::set_alignment(int& err, mx_align_t a)
{
    mx_paragraph_style_mod mod;
    mod.get_char_style_mod()->set_alignment_mod(a);

    cursor.apply_mod_to_current_style(err, mod);
    MX_ERROR_CHECK(err);

    update_all_sensitivities(err);
    MX_ERROR_CHECK(err);

    update_toolbar(err);
    MX_ERROR_CHECK(err);
abort:;
}

void mx_wp_editor::set_style(int& err, char* name)
{
    cursor.apply_style_change_to_current_style(err, name);
    MX_ERROR_CHECK(err);

    update_all_sensitivities(err);
    MX_ERROR_CHECK(err);

    update_toolbar(err);
    MX_ERROR_CHECK(err);
abort:;
}

void mx_wp_editor::add_tab(int& err, const mx_tabstop& tab,
    float position_tolerance)
{
    cursor.apply_tab_change_to_current_style(err, mx_wp_cursor::mx_add_tab_e,
        tab, position_tolerance);
    MX_ERROR_CHECK(err);

    update_all_sensitivities(err);
    MX_ERROR_CHECK(err);

    update_toolbar(err);
    MX_ERROR_CHECK(err);
abort:;
}

void mx_wp_editor::remove_tab(int& err, float pos, float position_tolerance)
{
    cursor.apply_tab_change_to_current_style(
        err, mx_wp_cursor::mx_remove_tab_e, mx_tabstop(mx_left, pos),
        position_tolerance);
    MX_ERROR_CHECK(err);

    update_all_sensitivities(err);
    MX_ERROR_CHECK(err);

    update_toolbar(err);
    MX_ERROR_CHECK(err);
abort:;
}

void mx_wp_editor::update_format_sensitivities(int& err)
{
    bool one_para_selection;

    one_para_selection = cursor.selection() && cursor.selection_start()->is_same_para_as(err, cursor.selection_end());
    MX_ERROR_CLEAR(err);

    if (!cursor.selection() || one_para_selection) {
        menu->set_visible(mx_format_tabs_e, mx_end_item_e);
        toolbar->set_visible(mx_format_tabs_e, mx_end_item_e);
    } else {
        menu->set_invisible(mx_format_tabs_e, mx_end_item_e);
        toolbar->set_invisible(mx_format_tabs_e, mx_end_item_e);
    }

    if (cursor.selection_has_table() && !cursor.selection_has_text() && (current_edit_component == mx_wp_editor_body_e)) {
        menu->set_visible(mx_format_table_e, mx_end_item_e);
        menu->set_visible(mx_format_table_style_e, mx_end_item_e);
        toolbar->set_visible(mx_format_table_e, mx_end_item_e);
        toolbar->set_visible(mx_format_table_style_e, mx_end_item_e);
    } else {
        menu->set_invisible(mx_format_table_e, mx_end_item_e);
        menu->set_invisible(mx_format_table_style_e, mx_end_item_e);
        toolbar->set_invisible(mx_format_table_e, mx_end_item_e);
        toolbar->set_invisible(mx_format_table_style_e, mx_end_item_e);
    }

    if (cursor.selection_only_covers_text() || cursor.selection_only_covers_table()) {
        menu->set_visible(mx_format_border_e, mx_end_item_e);
        toolbar->set_visible(mx_format_border_e, mx_end_item_e);
    } else {
        menu->set_invisible(mx_format_border_e, mx_end_item_e);
        toolbar->set_invisible(mx_format_border_e, mx_end_item_e);
    }
}

void mx_wp_editor::format_paragraph(int& err)
{
    mx_para_d* d = dialog_man.get_para_d();
    mx_paragraph_style** style_array = NULL;
    mx_paragraph_style_mod mod;
    int n, i;
    mx_list style_list;
    int dialogue_result;

    d->centre();
    do {
        cursor.styles_in_selection(err, style_list);
        MX_ERROR_CHECK(err);

        n = style_list.get_num_items();

        style_array = new mx_paragraph_style*[n];

        for (i = 0; i < n; i++) {
            style_array[i] = (mx_paragraph_style*)style_list.get(err, i);
            MX_ERROR_CHECK(err);
        }

        d->activate_d(style_array, n, current_units, mod);
        dialogue_result = d->run_modal();
        if (dialogue_result != cancel_e) {
            cursor.apply_mod_to_current_style(err, mod);
            MX_ERROR_CHECK(err);
        }

        {
            mx_list_iterator iter(style_list);
            while (iter.more())
                delete (mx_paragraph_style*)iter.data();
            delete[] style_array;
        }
    } while (dialogue_result == apply_e);

    update_all_sensitivities(err);
    MX_ERROR_CHECK(err);

    update_toolbar(err);
    MX_ERROR_CHECK(err);

abort:
    d->deactivate();
}
