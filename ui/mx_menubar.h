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
#ifndef MX_MENUBAR_H
#define MX_MENUBAR_H
/*
 * MODULE/CLASS : mx_menubar.h
 *
 * AUTHOR : Tom Newton
 *
 * This file
 *
 * DESCRIPTION:
 *
 *  This class is a derived base class for menu bars. It encapsulates
 *  the idea of a menu bar and the menus within it.
 *
 *
 *
 *
 *
 */

#include <map>
#include <string>

#include <Xm/Xm.h>
#include <mx.h>
#include <mx_bar.h>
#include <mx_ui_object.h>
#include <mx_window.h>

// forward declarations
class mx_menu_item;
class mx_menu;
class mx_menubar;

class mx_menu_item : public mx_ui_object {
    MX_RTTI(mx_menu_item_class_e)

public:
    // A Function to change an items sensitivity (whether its greyed out or
    // not).
    bool is_sensitized(); // returns an items sensitive status
    bool toggle_sensitivity(); // returns the new sensitivity
    void set_sensitivity(bool s);

    bool is_checked(); // returns FALSE for non-checkables
    void set_checked(bool s); // sets the check status

private:
    friend class mx_menu;

    // create a separator item
    mx_menu_item(mx_menu* parent);

    // create a normal item
    mx_menu_item(
        const char* name,
        mx_menu* parent,
        XtCallbackProc callback = NULL,
        XtPointer cd = NULL);

    // create a checkable item with an initial state
    mx_menu_item(
        const char* name,
        mx_menu* parent,
        bool is_item_checked,
        XtCallbackProc callback = NULL,
        XtPointer cd = NULL);

    ~mx_menu_item();

    enum mx_menu_item_type_t { normal_type,
        checkable_type,
        separator_type };

    mx_menu_item_type_t item_type;
};

class mx_menu : public mx_ui_object {
    MX_RTTI(mx_menu_class_e)

public:
    mx_menu_item* add_separator();

    mx_menu_item* add_item(
        const char* name,
        XtCallbackProc cb = NULL,
        XtPointer cd = NULL);

    mx_menu_item* add_checkable_item(
        const char* name,
        bool is_checked,
        XtCallbackProc cb = NULL,
        XtPointer cd = NULL);

    mx_menu* add_sub_menu(const char* name);

    mx_menu_item* get_item(const char* name);

    mx_menu* get_sub_menu(const char* name);

    friend class mx_menubar;

    mx_menu(const char* name, mx_menubar* parent, bool is_help = FALSE);
    mx_menu(const char* name, mx_menu* parent);
    ~mx_menu();

private:
    Widget button;
    std::map<std::string, mx_menu_item *> m_items;
    std::map<std::string, mx_menu *> m_sub_menus;
};

class mx_menubar : public mx_bar {
    MX_RTTI(mx_menubar_class_e)

public:
    mx_menubar(const char* name, mx_window* window);
    ~mx_menubar();

    mx_menu* add_sub_menu(const char* name, bool is_help = FALSE);

    mx_menu* get_sub_menu(const char* name);

    void set_item_visible(const char* menu_name, const char* item_name, bool s);

    bool get_item_checked(const char* menu_name, const char* item_name);

    void set_item_checked(const char* menu_name, const char* item_name, bool s);

    mx_menu_item* get_item(const char* menu_name, const char* item_name);

    bool is_checked(Widget w);

private:
    std::map<std::string, mx_menu *> m_sub_menus;
};

#endif // MX_MENUBAR_H
