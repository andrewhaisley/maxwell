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
 * MODULE/CLASS : mx_ui_object.h
 *
 * AUTHOR : Tom Newton
 *
 * This file
 *
 * DESCRIPTION:
 *
 *  This is a base class for all user interface objects, ie, anything with a
 *  widget (but not a John Smiths).
 *
 *  Essentially just a wrapper for the standard Motif functions.
 *
 *  The destructor does not destroy the Widget, since its parent (in the Motif
 *  Hierarchy) will do this, and multiple calls to XtDestroyWidget are slow.
 *  To destroy a Widget when you need to you call destroyWidget().
 *
 *
 *
 *
 */

#include <X11/Intrinsic.h>
#include <Xm/Xm.h>
#include <mx.h>
#include <mx_rtti.h>

typedef enum {
    MX_NO_CURSOR,
    MX_WATCH_CURSOR,
    MX_CROSSHAIR_CURSOR,
    MX_PENCIL_CURSOR
} mx_cursor_type_t;

class mx_editor;

class mx_ui_object : public mx_rtti {
    MX_RTTI(mx_ui_object_class_e)

public:
    mx_ui_object();
    mx_ui_object(const char* objectName, WidgetClass objectClass, Widget objectParent, int n_args = 0, Arg* args = NULL);

    virtual ~mx_ui_object();

    bool is_managed();
    void manage();
    void unmanage();

    virtual const Widget get_widget();
    virtual const char* get_name();

    void destroy_widget();

    // set or clear a busy cursor - this is hierachical so any children
    // will also be set busy.
    void set_busy(Widget w = NULL);
    void clear_busy(Widget w = NULL);

    // same thing on the widgets parent
    void set_parent_busy();
    void clear_parent_busy();

    // this is a replacement for XtAppNextEvent
    void XtAppNextEvent(XtAppContext app_context, XEvent* event_return);

    // make a beep
    void bell();

    // get a value out of the X resource file
    bool get_resource(
        int& err,
        const char* in_value_name,
        String out_value_type,
        XrmValue* out_value_ptr);

    // take the input focus
    void take_focus();

    void set_cursor(mx_cursor_type_t icursor_type, Widget w = NULL);

    // respond to a forced close of the top level window (if needed)
    virtual void forced_close(mx_editor *e) {};

protected:
    Widget widget;

    mx_cursor_type_t current_cursor_type;
};
