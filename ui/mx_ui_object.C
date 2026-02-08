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
 * MODULE/CLASS : mx_ui_object
 *
 * AUTHOR : Tom Newton/Andrew Haisley
 *
 * This file
 *
 * DESCRIPTION:
 *
 *  This is the implementation for the mx_ui_object class
 *
 *
 *
 */

#include <X11/Intrinsic.h>
#include <X11/cursorfont.h>
#include <Xm/Xm.h>

#include <mx.h>
#include <mx_ui_object.h>

mx_ui_object::mx_ui_object()
{
    widget = NULL;
    current_cursor_type = MX_NO_CURSOR;
}

mx_ui_object::mx_ui_object(const char* objectName,
    WidgetClass objectClass,
    Widget objectParent,
    int n_args,
    Arg* args)
{
    if (n_args == 0) {
        widget = XtVaCreateWidget(objectName, objectClass, objectParent, NULL);
    } else {
        widget = XtCreateWidget(objectName, objectClass, objectParent, args, n_args);
    }
}

mx_ui_object::~mx_ui_object()
{
}

bool mx_ui_object::is_managed()
{
    if (widget != NULL) {
        return XtIsManaged(widget);
    } else {
        return False;
    }
}

void mx_ui_object::manage()
{
    if (widget != NULL) {
        XtManageChild(widget);
    }
}

void mx_ui_object::unmanage()
{
    if (widget != NULL) {
        XtUnmanageChild(widget);
    }
}

const Widget mx_ui_object::get_widget()
{
    return widget;
}

const char* mx_ui_object::get_name()
{
    return XtName(widget);
}

void mx_ui_object::destroy_widget()
{
    XtDestroyWidget(widget);
}

void mx_ui_object::XtAppNextEvent(XtAppContext app_context, XEvent* event_return)
{
    ::XtAppNextEvent(app_context, event_return);
}

void mx_ui_object::set_cursor(mx_cursor_type_t icursor_type, Widget w)
{
    XSetWindowAttributes attrs;
    unsigned int shape = XC_num_glyphs;
    Cursor cursor;

    if (w == NULL) {
        if (icursor_type == current_cursor_type)
            return;
        w = widget;
    }

    switch (icursor_type) {
    case MX_WATCH_CURSOR:
        shape = XC_watch;
        break;
    case MX_CROSSHAIR_CURSOR:
        shape = XC_crosshair;
        break;
    case MX_PENCIL_CURSOR:
        shape = XC_pencil;
        break;
    default:
        attrs.cursor = None;
        break;
    }

    current_cursor_type = icursor_type;

    if (shape != XC_num_glyphs) {
        cursor = XCreateFontCursor(XtDisplay(w), shape);
        attrs.cursor = cursor;
    }

    XChangeWindowAttributes(
        XtDisplay(w),
        XtWindow(w),
        CWCursor,
        &attrs);
    XFlush(XtDisplay(w));
}

void mx_ui_object::set_busy(Widget w)
{
    set_cursor(MX_WATCH_CURSOR, w);
}

void mx_ui_object::clear_busy(Widget w)
{
    set_cursor(MX_NO_CURSOR, w);
}

void mx_ui_object::set_parent_busy()
{
    if (widget != NULL) {
        Widget parent = XtParent(widget);
        if (parent != NULL) {
            set_busy(parent);
        }
    }
}

void mx_ui_object::clear_parent_busy()
{
    if (widget != NULL) {
        Widget parent = XtParent(widget);
        if (parent != NULL) {
            clear_busy(parent);
        }
    }
}

void mx_ui_object::bell()
{
    XBell(XtDisplay(widget), 100);
}

bool mx_ui_object::get_resource(
    int& err,
    const char* in_value_name,
    String out_value_type,
    XrmValue* out_value_ptr)
{
    static char resource_string[100];
    static XrmDatabase db = NULL;

    char* str_type[20];
    XrmValue in_value;

    err = MX_ERROR_OK;

    if (db == NULL) {
        db = XtDatabase(XtDisplay(widget));

        if (db == NULL) {
            MX_ERROR_THROW(err, MX_NO_XTRESOURCE_DATABASE);
        } else {
            strcpy(resource_string, "Maxwell.");
        }
    }

    out_value_ptr->size = 0;
    out_value_ptr->addr = NULL;

    strcpy(resource_string + 8, in_value_name);

    if (!XrmGetResource(db, resource_string, "Maxwell", str_type, &in_value)) {
        return FALSE;
    }

    if (!XtConvertAndStore(
            widget,
            str_type[0],
            &in_value,
            out_value_type,
            out_value_ptr)) {
        return FALSE;
    }

    return TRUE;

abort:
    return FALSE;
}

void mx_ui_object::take_focus()
{
    XmProcessTraversal(widget, XmTRAVERSE_CURRENT);
}
