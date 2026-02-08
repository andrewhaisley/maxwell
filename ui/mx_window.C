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
 * MODULE/CLASS : mx_window.C
 *
 * AUTHOR : Tom Newton
 *
 * This file
 *
 * DESCRIPTION:
 *
 *  This is the implementation for the mx_window class.
 *
 *
 *
 *
 */

#include <maxwell.h>
#include <mx.h>
#include <mx_editor.h>
#include <mx_window.h>

#include <X11/Shell.h>
#include <X11/cursorfont.h>
#include <Xm/Xm.h>

#include <Xm/Form.h>

mx_window::mx_window(mx_editor* parent, const char* name) : mx_ui_object()
{
    widget = XtVaAppCreateShell(
        name == NULL ? const_cast<char*>("maxwell") : const_cast<char*>(name),
        "maxwell",
        topLevelShellWidgetClass,
        XtDisplay(global_top_level),
        XmNallowResize, True,
        NULL);

    form = XtVaCreateManagedWidget(
        name == NULL ? "form" : name,
        xmFormWidgetClass, widget,
        NULL);

    XtAddCallback(widget, XmNdestroyCallback,
        (XtCallbackProc)&mx_window::close_window_cb,
        (XtPointer)this);

    mx_window::parent = parent;
}

mx_window::~mx_window()
{
    // Remove all our destroy callbacks
    XtRemoveAllCallbacks(widget, XmNdestroyCallback);

    // This should destroy all widgets in the Motif widget tree.
    mx_ui_object::destroy_widget();
}

// additional window behaviour..
void mx_window::set_title(const char* string)
{
    XtVaSetValues(widget, XmNtitle, string, NULL);
}

void mx_window::close_window_cb(Widget widget, XtPointer client_data, XtPointer call_data)
{
    mx_window* w;

    w = (mx_window*)client_data;

    w->forced_close(w->parent);

    if (w->parent != NULL) {
        w->parent->window_is_closing();
    }
}

mx_editor* mx_window::get_editor()
{
    return parent;
}

void mx_window::iconsise()
{
    XIconifyWindow(XtDisplay(widget), XtWindow(widget), 0);
}

void mx_window::deiconise()
{
    XMapRaised(XtDisplay(widget), XtWindow(widget));
}

void mx_window::activate(int& err)
{
    // go through the objects tiling them as we go

    if (m_objects.size() == 0) {
        XtRealizeWidget(widget);
    } else if (m_objects.size() == 1) {
        XtVaSetValues(m_objects[0]->get_widget(),
            XmNleftAttachment, XmATTACH_FORM,
            XmNrightAttachment, XmATTACH_FORM,
            XmNtopAttachment, XmATTACH_FORM,
            XmNbottomAttachment, XmATTACH_FORM,
            NULL);
    } else {
        auto n = m_objects.size();
        for (size_t i = 0; i < n; i++) {
            Widget w = m_objects[i]->get_widget();

            if (i == 0) {
                XtVaSetValues(w,
                    XmNleftAttachment, XmATTACH_FORM,
                    XmNrightAttachment, XmATTACH_FORM,
                    XmNtopAttachment, XmATTACH_FORM,
                    NULL);
            } else {
                if (i == (n - 1)) {
                    XtVaSetValues(w,
                        XmNleftAttachment, XmATTACH_FORM,
                        XmNrightAttachment, XmATTACH_FORM,
                        XmNbottomAttachment, XmATTACH_FORM,
                        NULL);
                } else {
                    Widget prev_w = m_objects[i - 1]->get_widget();

                    if (i == (n - 2)) {
                        Widget next_w = m_objects[i + 1]->get_widget();

                        XtVaSetValues(w,
                            XmNleftAttachment, XmATTACH_FORM,
                            XmNrightAttachment, XmATTACH_FORM,
                            XmNtopAttachment, XmATTACH_WIDGET,
                            XmNtopWidget, prev_w,
                            XmNbottomAttachment, XmATTACH_WIDGET,
                            XmNbottomWidget, next_w,
                            NULL);
                    } else {
                        XtVaSetValues(w,
                            XmNleftAttachment, XmATTACH_FORM,
                            XmNrightAttachment, XmATTACH_FORM,
                            XmNtopAttachment, XmATTACH_WIDGET,
                            XmNtopWidget, prev_w,
                            NULL);
                    }
                }
            }
        }

        XtRealizeWidget(widget);
    }
}

void mx_window::add_ui_object(mx_ui_object* o)
{
    m_objects.push_back(o);
}

void mx_window::initial_focus(mx_ui_object* o)
{
    XtVaSetValues(form, XmNinitialFocus, o->get_widget(), NULL);
}

void mx_window::forced_close(mx_editor *e)
{
}
