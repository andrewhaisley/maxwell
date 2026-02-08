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
 *  table menu routines for wp editor
 *
 *
 *
 *
 */
#include "mx_wp_editor.h"

#include <mx_wp_menubar.h>
#include <mx_wp_toolbar.h>

#include <mx_ins_c_d.h>
#include <mx_ins_r_d.h>
#include <mx_undo.h>

void mx_wp_editor::table_cut_rows(int& err)
{
    cursor.cut_table_rows(err);
    MX_ERROR_CHECK(err);

    cleanup_after_cursor(err);
    MX_ERROR_CHECK(err);
abort:;
}

void mx_wp_editor::table_cut_columns(int& err)
{
    cursor.cut_table_columns(err);
    MX_ERROR_CHECK(err);

    cleanup_after_cursor(err);
    MX_ERROR_CHECK(err);

abort:;
}

void mx_wp_editor::table_insert_rows(int& err)
{
    mx_ins_r_d* d = dialog_man.get_ins_r_d();

    if (d->run() == yes_e) {
        if (d->insert_above) {
            cursor.insert_table_rows_before(err, d->num_rows);
            MX_ERROR_CHECK(err);
        } else {
            cursor.insert_table_rows_after(err, d->num_rows);
            MX_ERROR_CHECK(err);
        }
    }

    cleanup_after_cursor(err);
    MX_ERROR_CHECK(err);

abort:;
}

void mx_wp_editor::table_insert_columns(int& err)
{
    mx_ins_c_d* d = dialog_man.get_ins_c_d();

    if (d->run() == yes_e) {
        if (d->insert_to_left) {
            cursor.insert_table_columns_before(err, d->num_columns);
            MX_ERROR_CHECK(err);
        } else {
            cursor.insert_table_columns_after(err, d->num_columns);
            MX_ERROR_CHECK(err);
        }
    }

    cleanup_after_cursor(err);
    MX_ERROR_CHECK(err);

abort:;
}

void mx_wp_editor::update_table_sensitivities(int& err)
{
    bool b;

    if (current_edit_component != mx_wp_editor_body_e) {
        menu->set_invisible(
            mx_table_delete_rows_e,
            mx_table_delete_columns_e,
            mx_table_rows_e,
            mx_table_columns_e,
            mx_insert_table_e,
            mx_end_item_e);
        toolbar->set_invisible(
            mx_table_delete_rows_e,
            mx_table_delete_columns_e,
            mx_table_rows_e,
            mx_table_columns_e,
            mx_insert_table_e,
            mx_end_item_e);
    } else {
        menu->set_visible(
            mx_insert_table_e,
            mx_end_item_e);
        toolbar->set_visible(
            mx_insert_table_e,
            mx_end_item_e);
    }

    b = cursor.selection_within_area(err);
    MX_ERROR_CHECK(err);

    if (b) {
        if (cursor.selection_has_table()) {
            menu->set_visible(
                mx_table_delete_rows_e,
                mx_table_delete_columns_e,
                mx_table_rows_e,
                mx_table_columns_e,
                mx_end_item_e);
            toolbar->set_visible(
                mx_table_delete_rows_e,
                mx_table_delete_columns_e,
                mx_table_rows_e,
                mx_table_columns_e,
                mx_end_item_e);
            return;
        }
    }

    menu->set_invisible(
        mx_table_delete_rows_e,
        mx_table_delete_columns_e,
        mx_table_rows_e,
        mx_table_columns_e,
        mx_end_item_e);
    toolbar->set_invisible(
        mx_table_delete_rows_e,
        mx_table_delete_columns_e,
        mx_table_rows_e,
        mx_table_columns_e,
        mx_end_item_e);
abort:;
}
