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
#ifndef MX_DIALOG
#define MX_DIALOG
/*
 * MODULE/CLASS : mx_dialog
 *
 * AUTHOR : Andrew Haisley
 *
 * This file
 *
 * DESCRIPTION:
 *
 *
 *  A base class for dialog boxes of various sorts.
 *
 *
 *
 */

#include <Xm/Xm.h>
#include <maxwell.h>
#include <mx.h>
#include <mx_ui_object.h>

typedef enum {
    none_e = 0,
    yes_e = 1,
    no_e = 2,
    cancel_e = 3,
    apply_e = 4,
    replace_e = 5,
    ignore_e = 6,
    correct_e = 7,
    back_e = 8
} mx_dialog_result_t;

class mx_dialog : public mx_ui_object {

public:
    mx_dialog(
        const char* name,
        Widget parent,
        bool doModal = FALSE,
        bool resizable = FALSE);
    virtual ~mx_dialog();

    void centre();
    bool is_active();
    virtual void activate();
    virtual void deactivate();

    // activate the dialog and don't return until one of the
    // buttons has been pressed. Used when you don't want to have
    // to write three different callbacks to handle a simple
    // result.
    int run_modal();

    // activate the dialog, centre it run modal and deactivate it
    int run();

    // set the window title
    void set_title(char* title);

    mx_dialog_result_t modal_result;

    Widget dialog;
    Widget dialog_parent;
    Widget pane;
    Widget control_area;
    Widget action_area;

    // an extra - not provided by Motif for some reason
    XmToggleButtonState XmToggleButtonGetValue(Widget w);

    bool traverse_widget_tree(Widget w, char* mnemonic, XKeyEvent* key_event);
    bool handle_acc(Widget w, XEvent* event);

protected:
private:
    bool active;
    bool m_resizable;
};

#endif
