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
 * MODULE/CLASS : mx_menubar.C
 *
 * AUTHOR : Tom Newton
 *
 * This file
 *
 * DESCRIPTION:
 *
    This implements the mx_menu_item, mx_menu, and mx_menubar classes
 *
 *
 *
 */

#include <X11/Intrinsic.h>
#include <Xm/CascadeB.h>
#include <Xm/PushB.h>
#include <Xm/RowColumn.h>
#include <Xm/Separator.h>
#include <Xm/ToggleB.h>
#include <Xm/Xm.h>
#include <mx.h>
#include <mx_menubar.h>

///////////////////////////////////// mx_menu_item stuff

mx_menu_item::mx_menu_item(mx_menu* parent)
    : mx_ui_object()
{
    static int sep_num = 0;

    char char_num[30];

    sprintf(char_num, "separator%d", sep_num++);

    item_type = separator_type;

    widget = XtVaCreateManagedWidget(
        char_num,
        xmSeparatorWidgetClass,
        parent->get_widget(),
        NULL);
}

mx_menu_item::mx_menu_item(
    const char* name,
    mx_menu* parent,
    XtCallbackProc cb,
    XtPointer cd)
    : mx_ui_object()
{
    item_type = normal_type;

    widget = XtVaCreateManagedWidget(
        const_cast<char*>(name),
        xmPushButtonWidgetClass,
        parent->get_widget(),
        NULL);

    if ((cb != NULL) && (cd != NULL)) {
        XtAddCallback(widget, XmNactivateCallback, cb, cd);
    }
}

mx_menu_item::mx_menu_item(
    const char* name,
    mx_menu* parent,
    bool checked,
    XtCallbackProc cb,
    XtPointer cd)
    : mx_ui_object()
{
    item_type = checkable_type;

    widget = XtVaCreateManagedWidget(
        const_cast<char*>(name),
        xmToggleButtonWidgetClass,
        parent->get_widget(),
        XmNtoggleMode, XmTOGGLE_BOOLEAN,
        XmNindicatorType, XmN_OF_MANY,
        XmNindicatorOn, XmINDICATOR_CHECK,
        NULL);

    XtVaSetValues(widget, XmNset, checked ? XmSET : XmUNSET, NULL);

    if ((cb != NULL) && (cd != NULL)) {
        XtAddCallback(widget, XmNvalueChangedCallback, cb, cd);
    }
}

mx_menu_item::~mx_menu_item()
{
}

bool mx_menu_item::is_sensitized()
{
    return XtIsSensitive(widget);
}

bool mx_menu_item::toggle_sensitivity()
{
    bool value = !is_sensitized();

    XtSetSensitive(widget, value ? True : False);
    return value;
}

void mx_menu_item::set_sensitivity(bool s)
{
    XtSetSensitive(widget, s ? True : False);
}

bool mx_menu_item::is_checked()
{
    Boolean is_set;

    if (item_type == checkable_type) {
        XtVaGetValues(widget, XmNset, &is_set, NULL);

        if (is_set == XmSET) {
            return TRUE;
        }
    }
    return FALSE;
}

void mx_menu_item::set_checked(bool s)
{
    if (item_type == checkable_type) {
        XtVaSetValues(widget, XmNset, s ? XmSET : XmUNSET, NULL);
    }
}

///////////////////////////////////// mx_menu stuff

#define MENU_POSTFIX "Menu"

mx_menu::mx_menu(const char* name, mx_menubar* parent, bool is_help)
    : mx_ui_object()
{
    char menu_name[200];

    strcpy(menu_name, name);
    strcat(menu_name, MENU_POSTFIX);

    widget = XmCreatePulldownMenu(parent->get_widget(), const_cast<char*>(name), NULL, 0);

    button = XtVaCreateManagedWidget(
        name,
        xmCascadeButtonWidgetClass,
        parent->get_widget(),
        XmNsubMenuId, widget,
        NULL);

    // special case for help menu which goes on the right
    if (is_help) {
        XtVaSetValues(
            parent->get_widget(),
            XmNmenuHelpWidget, button,
            NULL);
    }
}

mx_menu::mx_menu(const char* name, mx_menu* parent)
    : mx_ui_object()
{
    char menu_name[200];

    strcpy(menu_name, name);
    strcat(menu_name, MENU_POSTFIX);

    widget = XmCreatePulldownMenu(parent->get_widget(), const_cast<char*>(name), NULL, 0);

    button = XtVaCreateManagedWidget(
        name,
        xmCascadeButtonWidgetClass,
        parent->get_widget(),
        XmNsubMenuId, widget,
        NULL);
}

mx_menu::~mx_menu()
{
    for (auto i : m_sub_menus) {
        delete i.second;
    }

    for (auto i : m_items) {
        delete i.second;
    }
}

///////////////////////////////////// mx_menubar stuff

mx_menubar::mx_menubar(const char* name, mx_window* window)
    : mx_bar(window)
{
    // this widget goes into the form of the window, not the window
    // itself
    Arg args[1];

    XtSetArg(args[0], XmNtearOffModel, XmTEAR_OFF_ENABLED);
    widget = XmCreateMenuBar(window->get_form(), const_cast<char*>(name), args, 1);

    XtManageChild(widget);
}

mx_menubar::~mx_menubar()
{
    for (auto i : m_sub_menus) {
        delete i.second;
    }
}

void mx_menubar::set_item_visible(const char* menu_name, const char* item_name, bool s)
{
    // get the menu
    if (m_sub_menus.find(menu_name) != m_sub_menus.end()) {
        auto item = m_sub_menus[menu_name]->get_item(item_name);
        if (item != nullptr) {
            item->set_sensitivity(s);
        }
    }
}

mx_menu_item* mx_menubar::get_item(const char* menu_name, const char* item_name)
{
    // get the menu
    if (m_sub_menus.find(menu_name) == m_sub_menus.end()) {
        return nullptr;
    } else {
        return m_sub_menus[menu_name]->get_item(item_name);
    }
}

bool mx_menubar::get_item_checked(const char* menu_name, const char* item_name)
{
    mx_menu_item* item = get_item(menu_name, item_name);

    if (item == nullptr) {
        return false;
    } else {
        return item->is_checked();
    }
}

void mx_menubar::set_item_checked(const char* menu_name, const char* item_name, bool s)
{
    mx_menu_item* item = get_item(menu_name, item_name);

    if (item != nullptr) {
        item->set_checked(s);
    }
}

mx_menu_item* mx_menu::add_separator()
{
    mx_menu_item* item = new mx_menu_item(this);
    m_items[item->get_name()] = item;

    return item;
}

mx_menu_item* mx_menu::add_item(const char* name, XtCallbackProc cb, XtPointer cd)
{
    mx_menu_item* item = new mx_menu_item(name, this, cb, cd);
    m_items[name] = item;

    return item;
}

mx_menu_item* mx_menu::add_checkable_item(const char* name, bool is_checked, XtCallbackProc cb, XtPointer cd)
{
    mx_menu_item* item = new mx_menu_item(name, this, is_checked, cb, cd);
    m_items[item->get_name()] = item;
    return item;
}

mx_menu_item* mx_menu::get_item(const char* name)
{
    if (m_items.find(name) == m_items.end()) {
        return nullptr;
    } else {
        return m_items[name];
    }
}

mx_menu* mx_menu::add_sub_menu(const char* name)
{
    mx_menu* menu = new mx_menu(name, this);
    m_sub_menus[name] = menu;
    return menu;
}

mx_menu* mx_menu::get_sub_menu(const char* name)
{
    return m_sub_menus[name];
}

mx_menu* mx_menubar::add_sub_menu(const char* name, bool is_help)
{
    mx_menu* menu = new mx_menu(name, this, is_help);
    m_sub_menus[menu->get_name()] = menu;
    return menu;
}

mx_menu* mx_menubar::get_sub_menu(const char* name)
{
    if (m_sub_menus.find(name) == m_sub_menus.end()) {
        return nullptr;
    } else {
        return m_sub_menus[name];
    }
}

bool mx_menubar::is_checked(Widget w)
{
    Boolean is_set;

    XtVaGetValues(w, XmNset, &is_set, NULL);
    return is_set == XmSET;
}
