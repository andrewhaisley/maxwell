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
 *  tools menu routines for wp editor
 *
 *
 *
 *
 */

#include "mx_wp_editor.h"
#include <mx_space_w.h>
#include <mx_word_iter.h>
#include <mx_wp_doc.h>

#include <mx_wp_menubar.h>
#include <mx_wp_toolbar.h>
#include <mx_xframe.h>

#include <mx_break_w.h>
#include <mx_env_d.h>
#include <mx_opt_d.h>
#include <mx_paragraph.h>
#include <mx_spell_d.h>
#include <mx_undo.h>

static bool is_valid_chunk_char(uint8 character)
{
    return isalpha((uint16)character) || character == '\'' || character > 126;
}

inline static bool string_has_more_chunks(char* string,
    uint32 string_length,
    uint32 chunk_start_index)
{
    while (chunk_start_index < string_length) {
        if (is_valid_chunk_char(string[chunk_start_index++])) {
            return TRUE;
        }
    }
    return FALSE;
}

inline static void string_next_chunk(int& err,
    char* string,
    uint32 string_length,
    uint32& chunk_start_index,
    uint32& chunk_end_index,
    mx_wp_doc_pos& chunk_start,
    mx_wp_doc_pos& chunk_end,
    mx_wp_document* document)
{
    chunk_start = chunk_end;
    chunk_start_index = chunk_end_index;
    while (!is_valid_chunk_char(string[chunk_start_index]) && chunk_start_index < string_length) {
        chunk_start_index++;
        chunk_start.move_right_ignoring_false_breaks(err, document);
        MX_ERROR_CHECK(err);
    }
abort:;
}

inline static void string_get_chunk(int& err,
    char* string,
    uint32 string_length,
    uint32& chunk_start_index,
    uint32& chunk_end_index,
    mx_wp_doc_pos& chunk_start,
    mx_wp_doc_pos& chunk_end,
    mx_wp_document* document)
{
    while (!is_valid_chunk_char(string[chunk_start_index]) && chunk_start_index < string_length) {
        chunk_start_index++;
        chunk_start.move_right_ignoring_false_breaks(err, document);
        MX_ERROR_CHECK(err);
    }
    chunk_end_index = chunk_start_index;
    chunk_end = chunk_start;

    while (is_valid_chunk_char(string[chunk_end_index]) && chunk_end_index < string_length) {
        chunk_end_index++;
        chunk_end.move_right_ignoring_false_breaks(err, document);
        MX_ERROR_CHECK(err);
    }
abort:;
}

int mx_wp_editor::tools_spell_check_range(
    int& err,
    mx_spell_d* d,
    mx_wp_doc_pos& start,
    mx_wp_doc_pos& end)
{
    mx_word_iterator iter(document, start, end);
    mx_wp_doc_pos last_error = start;
    mx_wp_doc_pos chunk_doc_start, chunk_doc_end;

    int sn = start.get_sheet();
    mx_word* w;
    const char* s;

    static const uint32 temp_buf_length = 1000;
    char str[temp_buf_length + 1];
    uint32 chunk_str_start, chunk_str_end, str_length;

    bool replaced_something = FALSE;

    while (iter.more(err)) {
        MX_ERROR_CHECK(err);

    check_word:

        w = iter.data(err);
        MX_ERROR_CHECK(err);

        s = w->cstring();
        strncpy(str, s, temp_buf_length);
        str[temp_buf_length] = 0;
        str_length = strlen(str);

        chunk_str_start = chunk_str_end = 0;
        chunk_doc_start = chunk_doc_end = iter.word_start;

        while (string_has_more_chunks(str, str_length, chunk_str_start)) {
            char* chunk_str;

            string_get_chunk(err, str, str_length, chunk_str_start,
                chunk_str_end, chunk_doc_start, chunk_doc_end,
                document);
            MX_ERROR_CHECK(err);

            str[chunk_str_end] = 0;
            chunk_str = str + chunk_str_start;

            if (d->wrong_spelling(global_user_config, chunk_str)) {
                if (!d->is_active()) {
                    d->centre();
                    d->activate();
                }

                cursor.set_selection(err, chunk_doc_start, chunk_doc_end);
                MX_ERROR_CHECK(err);

                cursor.make_visible(err);
                MX_ERROR_CHECK(err);

                cursor.update_caret(err);
                MX_ERROR_CHECK(err);

                switch (d->run_modal(global_user_config, chunk_str)) {
                case cancel_e:
                    return cancel_e;
                case replace_e:
                    replace_text(err, chunk_doc_start, chunk_doc_end, d->replacement.c_str());
                    MX_ERROR_CHECK(err);

                    replaced_something = TRUE;
                    break;
                case back_e:
                    iter.moveto(last_error);
                    goto check_word;
                default:
                case ignore_e:
                    last_error = chunk_doc_start;
                    break;
                }
            }

            string_next_chunk(err, str, str_length, chunk_str_start,
                chunk_str_end, chunk_doc_start, chunk_doc_end,
                document);
            MX_ERROR_CHECK(err);
        }
    }
    MX_ERROR_CHECK(err);

    if (replaced_something) {
        cursor.moveto_sheet(err, sn);
        MX_ERROR_CHECK(err);

        ur_buffer->mark_cant_undo(err);
        MX_ERROR_CHECK(err);
    }

    return yes_e;
abort:
    return cancel_e;
}

void mx_wp_editor::tools_spelling(int& err)
{
    mx_spell_d* d;

    mx_wp_doc_pos start, end, save_pos;
    bool was_selection = FALSE;

    int res;

    d = dialog_man.get_spell_d();

    if (cursor.selection()) {
        start = *cursor.selection_start();
        end = *cursor.selection_end();

        was_selection = TRUE;
    } else {
        save_pos = start = *cursor.get_position();

        end.moveto_end(err, document);
        MX_ERROR_CHECK(err);
    }

    res = tools_spell_check_range(err, d, start, end);
    MX_ERROR_CHECK(err);

    if (!was_selection && (res != cancel_e) && !start.is_start()) {
        if (dialog_man.run_yes_no_d("startFromTop", FALSE, yes_e) == yes_e) {
            end = start;

            start.moveto_start(err, document);
            MX_ERROR_CHECK(err);

            res = tools_spell_check_range(err, d, start, end);
            MX_ERROR_CHECK(err);
        }
    }

    if (was_selection) {
        cursor.set_selection(err, start, end);
        MX_ERROR_CHECK(err);
    } else {
        cursor.set_position(err, save_pos);
        MX_ERROR_CHECK(err);
    }

    if (res != cancel_e) {
        dialog_man.run_inform_d(inform_e, "allChecked");
    }

    update_all_sensitivities(err);
    MX_ERROR_CHECK(err);

abort:
    d->deactivate();
}

void mx_wp_editor::tools_repaginate(int& err)
{
    cursor.repaginate(err);
    MX_ERROR_CHECK(err);

    ur_buffer->mark_cant_undo(err);
    MX_ERROR_CHECK(err);

    update_all_sensitivities(err);
    MX_ERROR_CHECK(err);
abort:;
}

void mx_wp_editor::tools_options(int& err)
{
    mx_editor::tools_options(err);
    MX_ERROR_CHECK(err);
abort:;
}

void mx_wp_editor::tools_envelope(int& err)
{
    mx_env_d* d;

    mx_wp_document* temp_doc;
    mx_paragraph_style *env_style, *temp_style;

    char address[MX_ENV_MAX_TEXT + 1];

    float width = 110, height = 220;
    int i;

    set_edit_component(err, mx_wp_editor_body_e);
    MX_ERROR_CHECK(err);

    get_address(err, address);

    for (i = 0; i < MX_NUM_ENVELOPE_SIZES; i++) {
        if (strcmp(current_envelope_size, mx_envelope_size_names[i]) == 0) {
            width = mx_envelope_widths[i];
            height = mx_envelope_heights[i];
            break;
        }
    }

    d = dialog_man.get_env_d();
    d->centre();
    d->activate_d(width, height, TRUE, address, current_units);

    if (d->run_modal() == yes_e) {
        temp_doc = mx_db_client_open_temporary_wp_doc(err);
        MX_ERROR_CHECK(err);

        document = temp_doc;

        temp_doc->set_page_size(err,
            d->is_landscape ? d->height : d->width,
            d->is_landscape ? d->width : d->height,
            d->is_landscape);
        MX_ERROR_CHECK(err);

        // copy the default envelope style across
        env_style = body_document->get_named_style(err, "default envelope");
        MX_ERROR_CHECK(err);

        temp_style = temp_doc->get_default_style(err);
        MX_ERROR_CHECK(err);
        *temp_style = *env_style;

        insert_address_text(err, temp_doc, d->address);
        MX_ERROR_CHECK(err);

        init_print_frame(err);
        MX_ERROR_CHECK(err);

        mx_editor::file_print(err, 0, print_frame);
        MX_ERROR_CHECK(err);

        mx_db_client_close_wp_doc(err, temp_doc);
        MX_ERROR_CHECK(err);

        document = body_document;
    }

    d->deactivate();
abort:;
}

void mx_wp_editor::update_tool_sensitivities(int& err)
{
    if (current_edit_component == mx_wp_editor_body_e) {
        menu->set_visible(mx_tools_repaginate_e, mx_end_item_e);
        toolbar->set_visible(mx_tools_repaginate_e, mx_end_item_e);
    } else {
        menu->set_invisible(mx_tools_repaginate_e, mx_end_item_e);
        toolbar->set_invisible(mx_tools_repaginate_e, mx_end_item_e);
    }
}

void mx_wp_editor::insert_address_text(
    int& err,
    mx_wp_document* doc,
    char* address)
{
    mx_wp_doc_pos pos;
    mx_text_area* a;
    mx_sheet* s;

    float width, height;
    float area_height;

    int i = 0;

    pos.moveto_start(err, doc);
    MX_ERROR_CHECK(err);

    while (address[i] != 0) {
        if (address[i] == 10) {
            doc->insert_para_break(err, pos);
            MX_ERROR_CHECK(err);

            pos.next_para(err, document);
            MX_ERROR_CHECK(err);
        } else {
            doc->insert_char(err, address[i], pos);
            MX_ERROR_CHECK(err);

            pos.right(err, doc);
            MX_ERROR_CHECK(err);
        }
        i++;
    }

    doc->reformat(err);
    MX_ERROR_CHECK(err);

    // centre the text
    s = doc->sheet(err, 0);
    MX_ERROR_CHECK(err);

    a = (mx_text_area*)(s->get_area(err, 0));
    MX_ERROR_CHECK(err);

    width = s->get_width(err);
    MX_ERROR_CHECK(err);

    height = s->get_height(err);
    MX_ERROR_CHECK(err);

    area_height = a->get_height(err);
    MX_ERROR_CHECK(err);

    s->set_margins(err,
        width / 3.0,
        0.0,
        (height - area_height) / 2.0,
        0.0);
    MX_ERROR_CHECK(err);

    doc->reformat(err);
    MX_ERROR_CHECK(err);

abort:;
}

void mx_wp_editor::get_address(int& err, char* address)
{
    mx_wp_doc_pos start, end;
    mx_space_word* sw;

    mx_space_word::mx_space_word_item_t it;

    mx_word* w;
    int i;

    strcpy(address, "");

    if (cursor.selection()) {
        start = *cursor.selection_start();
        end = *cursor.selection_end();

        {
            mx_word_iterator iter(document, start, end, TRUE);

            while (iter.more(err)) {
                MX_ERROR_CHECK(err);

                w = iter.data(err);
                MX_ERROR_CHECK(err);

                if (w->rtti_class() == mx_break_word_class_e) {
                    if (((mx_break_word*)w)->type() != mx_long_word_break_e) {
                        strcat(address, "\n");
                    }
                } else {
                    if (w->rtti_class() == mx_space_word_class_e) {
                        sw = (mx_space_word*)w;
                        for (i = 0; i < sw->get_num_items(); i++) {
                            it = sw->item_type(err, i);
                            MX_ERROR_CHECK(err);

                            if (it == mx_space_word::mx_space_word_space_e) {
                                strcat(address, " ");
                            } else {
                                if (it == mx_space_word::mx_space_word_tab_e) {
                                    strcat(address, "\t");
                                }
                            }
                        }
                    } else {
                        if ((strlen(w->cstring()) + strlen(address)) < MX_ENV_MAX_TEXT) {
                            strcat(address, w->cstring());
                        } else {
                            return;
                        }
                    }
                }
            }
            MX_ERROR_CHECK(err);
        }
    }
abort:;
}

void mx_wp_editor::replace_text(
    int& err,
    mx_wp_doc_pos& start,
    mx_wp_doc_pos& end,
    const char* s)
{
    start.save(err, document);
    MX_ERROR_CHECK(err);

    cursor.remove_selection(err);
    MX_ERROR_CHECK(err);

    cursor.insert_text(err, s);
    MX_ERROR_CHECK(err);

    start.restore(err, document);
    MX_ERROR_CHECK(err);

    start.moveto_start_para(err, document);
    MX_ERROR_CHECK(err);

    end = *cursor.get_position();

    cursor.reformat_and_restore_position(err, start, end, TRUE);
    MX_ERROR_CHECK(err);

    end = *cursor.get_position();

    cursor.update_caret(err);
    MX_ERROR_CHECK(err);

    frame->refresh(err);
    MX_ERROR_CHECK(err);

abort:;
}

void mx_wp_editor::replace_highlight_text(int& err, char* s)
{
    if (cursor.selection()) {
        mx_wp_doc_pos start = *cursor.selection_start();
        mx_wp_doc_pos end = *cursor.selection_end();

        replace_text(err, start, end, s);
        MX_ERROR_CHECK(err);
    }

abort:;
}
