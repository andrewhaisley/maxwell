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
#ifndef MX_WP_MENUBAR_H
#define MX_WP_MENUBAR_H
/*
 * MODULE/CLASS : mx_wp_menubar
 *
 * AUTHOR : Tom Newton/Andrew Haisley
 *
 *
 *
 * DESCRIPTION:
 *
 * A menubar for a word processing editor
 *
 *
 *
 *
 */

#include <Xm/Xm.h>
#include <mx.h>
#include <mx_menubar.h>
#include <mx_wp_menu_items.h>

class mx_wp_menubar : public mx_menubar {
    MX_RTTI(mx_wp_menubar_class_e)

public:
    mx_wp_menubar(mx_window* parent);

    // set visibility of a single item
    void set_item_visible(menu_item_name_t item, bool s);

    // list is terminated with end_item_e
    void set_visible(menu_item_name_t item, ...);
    void set_invisible(menu_item_name_t item, ...);

    // set the checkmarks on an item
    void set_item_checked(int& err, menu_item_name_t item, bool s);

private:
    static void file_cb(Widget myWidget, XtPointer thisPtr, XtPointer callData);
    static void edit_cb(Widget myWidget, XtPointer thisPtr, XtPointer callData);
    static void table_cb(Widget myWidget, XtPointer thisPtr, XtPointer callData);
    static void view_cb(Widget myWidget, XtPointer thisPtr, XtPointer callData);
    static void help_cb(Widget myWidget, XtPointer thisPtr, XtPointer callData);
    static void insert_cb(Widget myWidget, XtPointer thisPtr, XtPointer callData);
    static void format_cb(Widget myWidget, XtPointer thisPtr, XtPointer callData);
    static void tools_cb(Widget myWidget, XtPointer thisPtr, XtPointer callData);
};

#endif // MX_WP_MENUBAR_H
