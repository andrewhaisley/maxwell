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
 * MODULE/CLASS : mx_dg_menubar
 *
 * AUTHOR : David Miller
 *
 * This file
 *
 * DESCRIPTION:
 *
 *
 */

#include <Xm/Xm.h>
#include <stdarg.h>

#include <mx.h>
#include <mx_dg_editor.h>
#include <mx_dg_menubar.h>

mx_dg_menubar::mx_dg_menubar(mx_window* w)
    : mx_menubar("dgMenuBar", w)
{
    mx_menu* menu;

    menu = add_sub_menu("fileItem");
    menu->add_item(MX_DG_UPDATE, &mx_dg_menubar::text_cb, this);
    menu->add_item(MX_DG_QUIT, &mx_dg_menubar::text_cb, this);
    menu->add_item(MX_DG_RETURN, &mx_dg_menubar::text_cb, this);

    menu = add_sub_menu("editItem");
    menu->add_item(MX_DG_UNDO, &mx_dg_menubar::text_cb, this);
    menu->add_item(MX_DG_REDO, &mx_dg_menubar::text_cb, this);
    menu->add_separator();
    menu->add_item(MX_DG_CUT, &mx_dg_menubar::text_cb, this);
    menu->add_item(MX_DG_PASTE, &mx_dg_menubar::text_cb, this);
    menu->add_item(MX_DG_SELECT_ALL, &mx_dg_menubar::text_cb, this);
    menu->add_separator();
    menu->add_item(MX_DG_ROTATE, &mx_dg_menubar::text_cb, this);
    menu->add_item(MX_DG_SCALE, &mx_dg_menubar::text_cb, this);
    menu->add_item(MX_DG_ALIGN, &mx_dg_menubar::text_cb, this);
    menu->add_item(MX_DG_FLIP, &mx_dg_menubar::text_cb, this);
    menu->add_item(MX_DG_GROUP, &mx_dg_menubar::text_cb, this);
    menu->add_item(MX_DG_UNGROUP, &mx_dg_menubar::text_cb, this);

    menu = add_sub_menu("levelItem");
    menu->add_item(MX_DG_UP_LEVEL, &mx_dg_menubar::text_cb, this);
    menu->add_item(MX_DG_DOWN_LEVEL, &mx_dg_menubar::text_cb, this);
    menu->add_item(MX_DG_CHOOSE_LEVEL, &mx_dg_menubar::text_cb, this);
    menu->add_item(MX_DG_CURRENT_LEVEL, &mx_dg_menubar::text_cb, this);

    menu = add_sub_menu("styleItem");
    menu->add_item(MX_DG_LINE_STYLE, &mx_dg_menubar::text_cb, this);
    menu->add_item(MX_DG_AREA_STYLE, &mx_dg_menubar::text_cb, this);
    menu->add_item(MX_DG_TEXT_STYLE, &mx_dg_menubar::text_cb, this);
    menu->add_item(MX_DG_COLOUR_STYLE, &mx_dg_menubar::text_cb, this);

    menu = add_sub_menu("positionItem");
    menu->add_item(MX_DG_GRID, &mx_dg_menubar::text_cb, this);
    menu->add_item(MX_DG_SNAP, &mx_dg_menubar::text_cb, this);
    menu->add_item(MX_DG_GUIDE, &mx_dg_menubar::text_cb, this);

    menu = add_sub_menu("objectItem");
    menu->add_item(MX_DG_SPAN, &mx_dg_menubar::text_cb, this);
    menu->add_item(MX_DG_ARC, &mx_dg_menubar::text_cb, this);
    menu->add_item(MX_DG_PLINE, &mx_dg_menubar::text_cb, this);
    menu->add_item(MX_DG_RECT, &mx_dg_menubar::text_cb, this);
    menu->add_item(MX_DG_SNAP, &mx_dg_menubar::text_cb, this);
    menu->add_item(MX_DG_SELECT, &mx_dg_menubar::text_cb, this);
}

void mx_dg_menubar::text_cb(Widget w,
    XtPointer cd,
    XtPointer call_data)
{
    mx_dg_menubar* bar = (mx_dg_menubar*)cd;
    mx_window* window = bar->get_window();
    char* name = XtName(w);
    mx_dg_editor* ed = (mx_dg_editor*)window->get_editor();
    mx_text_event te(name);

    int err = MX_ERROR_OK;

    window->set_busy();

    ed->textEntry(err, te);

    window->clear_busy();
    MX_ERROR_CHECK(err);
    return;

abort:
    global_error_trace->print();
}
