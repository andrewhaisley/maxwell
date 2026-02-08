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
 *  view menu routines for wp editor
 *
 *
 *
 *
 */
#include "mx_wp_editor.h"

#include <mx_wp_stsbar.h>
#include <mx_wp_toolbar.h>
#include <mx_xframe.h>
#include <mx_zoom_d.h>

void mx_wp_editor::view_outline(int& err, bool s)
{
    current_view_mode = s ? mx_paint_wp_normal_e : mx_paint_wp_codes_e;

    frame->refresh(err);
    MX_ERROR_CHECK(err);

    cursor.set_view_mode(current_view_mode);

abort:;
}

void mx_wp_editor::view_zoom(int& err)
{
    mx_zoom_d* d = dialog_man.get_zoom_d();
    mx_doc_coord_t f;

    if (d->run((int)(current_zoom * 100)) == yes_e) {
        current_zoom = d->zoom_percent;
        current_zoom /= 100.0;

        frame->setZoom(err, current_zoom);
        MX_ERROR_CHECK(err);

        cursor.make_visible(err);
        MX_ERROR_CHECK(err);

        cursor.update_caret(err);
        MX_ERROR_CHECK(err);

        toolbar->get_ruler_bar()->display(err, current_zoom, current_units);
        MX_ERROR_CHECK(err);

        statusbar->update_b(
            err,
            *(cursor.get_position()),
            document,
            current_units,
            FALSE,
            current_zoom * 100.0);
        MX_ERROR_CHECK(err);
    }
abort:;
}

void mx_wp_editor::view_header(int& err)
{
    set_edit_component(err, mx_wp_editor_header_e);
    MX_ERROR_CHECK(err);

abort:;
}

void mx_wp_editor::view_document(int& err)
{
    set_edit_component(err, mx_wp_editor_body_e);
    MX_ERROR_CHECK(err);

abort:;
}

void mx_wp_editor::view_footer(int& err)
{
    set_edit_component(err, mx_wp_editor_footer_e);
    MX_ERROR_CHECK(err);

abort:;
}

void mx_wp_editor::view_actions(int& err, bool s)
{
    toolbar->view_actions(s);
}

void mx_wp_editor::view_styles(int& err, bool s)
{
    toolbar->view_styles(s);
}

void mx_wp_editor::view_ruler(int& err, bool s)
{
    toolbar->view_ruler(s);
}

void mx_wp_editor::view_table_guides(int& err, bool s)
{
    draw_table_guides = !s;

    frame->refresh(err);
    MX_ERROR_CHECK(err);
abort:;
}
