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
 * file menu routines for wp editor
 *
 *
 *
 *
 *
 */

#include "mx_wp_editor.h"
#include <mx_app.h>
#include <mx_ascii_exporter.h>
#include <mx_field_w.h>
#include <mx_print_target.h>
#include <mx_rtf_exporter.h>

#include <mx_new_d.h>
#include <mx_open_d.h>
#include <mx_undo.h>
#include <mx_window.h>
#include <mx_wp_menubar.h>
#include <mx_wp_toolbar.h>
#include <mx_xframe.h>
#include <mx_yes_no_d.h>

void mx_wp_editor::file_new(int& err)
{
    mx_new_d* nd;
    char s[MAX_PATH_LEN * 2];
    char t[MAX_PATH_LEN * 2];

    mx_document* doc;

    set_edit_component(err, mx_wp_editor_body_e);
    MX_ERROR_CHECK(err);

    sprintf(s, "%s/template", global_maxhome);

    nd = dialog_man.get_new_d(s);

    if (nd->run() == yes_e) {
        sprintf(s, "%s/template/%s", global_maxhome, nd->template_name);
        mx_tmpnam(t);

        mx_copy_file(err, s, t);
        MX_ERROR_CHECK(err);

        doc = mx_editor::open_document(err, t);
        MX_ERROR_CHECK(err);

        if (doc != nullptr) {
            app->new_wp_editor(err, (mx_wp_document*)doc);
            MX_ERROR_CHECK(err);
        }
    }
abort:;
}

void mx_wp_editor::file_open(int& err)
{
    static mx_file_type_t file_type = mx_document_file_e;
    mx_document* doc = get_document();
    mx_open_d* d;
    bool b;

    set_edit_component(err, mx_wp_editor_body_e);
    MX_ERROR_CHECK(err);

    d = dialog_man.get_open_d();

    if (d->run(mx_document_file_class_e, file_type, false) == yes_e) {
        doc = mx_editor::open_document(err, d->file_name(b));
        MX_ERROR_CHECK(err);

        file_type = mx_guess_file_type(d->file_name(b));

        if (doc == nullptr) {
            return;
        }

        // two cases - if the current document is temporary and unmodified,
        // open in this window, otherwise, open in a new one
        if (document->get_temp_flag() && !document->is_modified()) {
            mx_db_client_close_wp_doc(err, document);
            MX_ERROR_CHECK(err);

            if (footer_document != nullptr) {
                mx_db_client_close_wp_doc(err, footer_document);
                MX_ERROR_CHECK(err);
            }

            if (header_document != nullptr) {
                mx_db_client_close_wp_doc(err, header_document);
                MX_ERROR_CHECK(err);
            }

            header_document = nullptr;
            footer_document = nullptr;
            body_document = nullptr;

            body_document = document = (mx_wp_document*)doc;

            (void)set_sheet_layout(err, document);
            MX_ERROR_CHECK(err);

            frame->resetSheetLayout(err);
            MX_ERROR_CHECK(err);

            cursor.set_document(document);
            cursor.set_frame(frame);
            cursor.set_layout(layout);

            body_ur_buffer->mark_cant_undo(err);
            MX_ERROR_CHECK(err);

            header_ur_buffer->mark_cant_undo(err);
            MX_ERROR_CHECK(err);

            footer_ur_buffer->mark_cant_undo(err);
            MX_ERROR_CHECK(err);

            window->set_title(document->get_file_name());
            toolbar->set_style_pulldown(document);

            update_all_sensitivities(err);
            MX_ERROR_CHECK(err);

            mx_char_style* cs;
            cs = cursor.char_style_at_position(err);
            MX_ERROR_CHECK(err);
            cursor.set_current_style(*cs);

            update_toolbar(err);
            MX_ERROR_CHECK(err);

            frame->refresh(err);
            MX_ERROR_CHECK(err);
        } else {
            app->new_wp_editor(err, (mx_wp_document*)doc);
            MX_ERROR_CHECK(err);
        }
    }
abort:;
}

void mx_wp_editor::file_save(int& err)
{
    body_ur_buffer->mark_cant_undo(err);
    MX_ERROR_CHECK(err);

    header_ur_buffer->mark_cant_undo(err);
    MX_ERROR_CHECK(err);

    footer_ur_buffer->mark_cant_undo(err);
    MX_ERROR_CHECK(err);

    set_edit_component(err, mx_wp_editor_body_e);
    MX_ERROR_CHECK(err);

    mx_editor::file_save(err);
    MX_ERROR_CHECK(err);

    // file may have changed
    window->set_title(document->get_file_name());

    update_file_sensitivities(err);
    MX_ERROR_CHECK(err);

abort:;
}

void mx_wp_editor::file_save_as(int& err)
{
    body_ur_buffer->mark_cant_undo(err);
    MX_ERROR_CHECK(err);

    header_ur_buffer->mark_cant_undo(err);
    MX_ERROR_CHECK(err);

    footer_ur_buffer->mark_cant_undo(err);
    MX_ERROR_CHECK(err);

    set_edit_component(err, mx_wp_editor_body_e);
    MX_ERROR_CHECK(err);

    mx_editor::file_save_as(err);
    MX_ERROR_CHECK(err);

    window->set_title(document->get_file_name());

    update_file_sensitivities(err);
    MX_ERROR_CHECK(err);

abort:;
}

void mx_wp_editor::init_print_frame(int& err)
{
    mx_doc_coord_t top_left;
    mx_point size;

    mx_sheet* s;

    s = document->sheet(err, 0);
    MX_ERROR_CHECK(err);

    size.x = s->get_width(err);
    MX_ERROR_CHECK(err);

    size.y = s->get_height(err);
    MX_ERROR_CHECK(err);

    set_print_layout(err);
    MX_ERROR_CHECK(err);

    if (print_frame == nullptr) {
        print_target = new mx_print_target(err, print_layout, document);
        MX_ERROR_CHECK(err);

        print_frame = new mx_print_frame(
            err,
            top_left,
            size,
            1,
            MX_MM_TO_INCHES(current_printer_x_res),
            *print_target);
        MX_ERROR_CHECK(err);
    }
abort:;
}

void mx_wp_editor::file_print(int& err)
{
    mx_field_word w;

    set_edit_component(err, mx_wp_editor_body_e);
    MX_ERROR_CHECK(err);

    tools_repaginate(err);
    MX_ERROR_CHECK(err);

    init_print_frame(err);
    MX_ERROR_CHECK(err);

    mx_editor::file_print(err, cursor.get_position()->get_sheet(), print_frame);
    MX_ERROR_CHECK(err);

    w.set_page_number(1);

abort:;
}

void mx_wp_editor::file_print_one_copy(int& err)
{
    set_edit_component(err, mx_wp_editor_body_e);
    MX_ERROR_CHECK(err);

    init_print_frame(err);
    MX_ERROR_CHECK(err);

    mx_editor::file_print_one_copy(err, print_frame);
    MX_ERROR_CHECK(err);

abort:;
}

bool mx_wp_editor::file_close(int& err)
{
    mx_yes_no_d* d = dialog_man.get_yes_no_d("exit", false, no_e);

    set_edit_component(err, mx_wp_editor_body_e);
    MX_ERROR_CHECK(err);

    (void)mx_editor::file_close(err);
    MX_ERROR_CHECK(err);

    if (get_document()->is_modified()) {
        if (d->run() == no_e) {
            return false;
        }
    }

    app->wp_editor_is_exiting(err, this);
    MX_ERROR_CHECK(err);

    delete this;

    return true;

abort:
    return false;
}

bool mx_wp_editor::file_force_close(int& err)
{
    set_edit_component(err, mx_wp_editor_body_e);
    MX_ERROR_CHECK(err);

    (void)mx_editor::file_force_close(err);
    MX_ERROR_CHECK(err);

    if (get_document()->is_modified()) {
        if (get_document()->get_temp_flag()) {
            file_save_as(err);
            MX_ERROR_CHECK(err);
        } else {
            mx_yes_no_d *d = dialog_man.get_yes_no_d("forceExit", false, no_e);
            if (d->run() == yes_e)
            {
                document->commit(err);
                MX_ERROR_CHECK(err);
            }
        }
    }

    app->wp_editor_is_exiting(err, this);
    MX_ERROR_CHECK(err);

    delete this;

    return true;

abort:
    return false;
}

void mx_wp_editor::update_file_sensitivities(int& err)
{
    // has the document been modified ?
    if (document->is_modified() && !document->get_temp_flag()) {
        menu->set_visible(mx_file_save_e, mx_end_item_e);
        toolbar->set_visible(mx_file_save_e, mx_end_item_e);
    } else {
        menu->set_invisible(mx_file_save_e, mx_end_item_e);
        toolbar->set_invisible(mx_file_save_e, mx_end_item_e);
    }
}

mx_wp_layout* mx_wp_editor::set_print_layout(int& err)
{
    mx_sheet_size* sheets;
    int i, n;

    if (print_layout == nullptr) {
        print_layout = new mx_wp_layout;
    }

    n = print_layout->getNumberSheets();

    print_layout->removeSheets(err, 0, n);
    MX_ERROR_CHECK(err);

    n = document->get_num_sheets(err);
    MX_ERROR_CHECK(err);

    sheets = new mx_sheet_size[n];

    for (i = 0; i < n; i++) {
        sheets[i] = document->get_sheet_visible_size(err, i, mx_paint_wp_full_e);
        MX_ERROR_CHECK(err);
    }

    print_layout->setSnapTolerance(0.00001);

    print_layout->addSheets(err, 0, n, sheets);
    MX_ERROR_CHECK(err);

    delete[] sheets;

    return print_layout;
abort:
    return nullptr;
}

void mx_wp_editor::print_page(int& err, int i, mx_print_frame* print_frame)
{
    bool b;
    mx_field_word w;

    w.set_page_number(i + 1);

    if (header_document != nullptr) {
        header_document->reformat(err);
        MX_ERROR_CHECK(err);
    }

    if (footer_document != nullptr) {
        footer_document->reformat(err);
        MX_ERROR_CHECK(err);
    }

    document->reformat_sheet(err, i, b);
    MX_ERROR_CHECK(err);

    mx_editor::print_page(err, i, print_frame);
    MX_ERROR_CHECK(err);

abort:;
}

void mx_wp_editor::file_export(int& err, char* file_name, mx_file_type_t type)
{
    mx_exporter* x;
    switch (type) {
    case mx_ascii_file_e:
        x = new mx_ascii_exporter(err, file_name, document, &dialog_man);
        break;
    case mx_rtf_file_e:
        x = new mx_rtf_exporter(err, file_name, document, &dialog_man);
        break;
    default:
        MX_ERROR_THROW(err, MX_ERROR_NOT_IMPLEMENTED);
        break;
    }
    MX_ERROR_CHECK(err);

    x->export_doc(err);
    MX_ERROR_CHECK(err);

    delete x;
abort:;
}
