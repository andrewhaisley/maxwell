#pragma once 

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
 * MODULE/CLASS : mx_window.h
 *
 * AUTHOR : Tom Newton/Andrew Haisley
 *
 * This file
 *
 * DESCRIPTION:
 *
 * Editor window. One of these is used to hold whatever bits and pieces
 * an editor wants - e.g. toolbar, menu work area etc.
 *
 * Things in windows are tiled vertically.
 *
 *
 *
 *
 */

#include <vector>

#include <Xm/Xm.h>
#include <mx.h>
#include <mx_bar.h>
#include <mx_list.h>
#include <mx_ui_object.h>

class mx_editor;

class mx_window : public mx_ui_object {
public:
    mx_window(mx_editor* parent, const char* name = NULL);

    virtual ~mx_window();

    // Change the windows title..
    void set_title(const char* string);

    // Get this window's owning editor.
    mx_editor* get_editor();

    // iconise the window
    void iconsise();

    // de-iconise the window
    void deiconise();

    // gets hold of the form widget - needed by objects that you
    // are going to add in resize etc.
    Widget get_form() { return form; };

    // tell the window that you have added a ui object - this allows the
    // window to tile everything horizontally in the correct order
    void add_ui_object(mx_ui_object* o);

    // call this once everything has been constructed to realize the widget
    // tree
    void activate(int& err);

    // give the initial focus to the given ui_object
    void initial_focus(mx_ui_object* o);

    // called when the window is about to be forceably closed
    virtual void forced_close(mx_editor *e);

    // get the editor for a given window
    static mx_editor *get_editor_for_window(Window w);

private:

    // his calls mx_editor::window_is_closing()
    static void close_window_cb(Widget, XtPointer, XtPointer);

    mx_editor* parent;

    Widget form;

    std::vector<mx_ui_object *> m_objects;
};
