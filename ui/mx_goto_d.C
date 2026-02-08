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
 * MODULE/CLASS : mx_goto_d
 *
 * AUTHOR : Andrew Haisley
 *
 * This file
 *
 * DESCRIPTION:
 *
 *  A goto page dialog
 *
 *
 *
 *
 */

#include <dirent.h>
#include <errno.h>
#include <pwd.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>

#include "mx_dialog.h"
#include "mx_inform_d.h"
#include <Xm/Label.h>
#include <Xm/LabelG.h>
#include <Xm/List.h>
#include <Xm/Protocols.h>
#include <Xm/PushBG.h>
#include <Xm/RowColumn.h>
#include <Xm/Separator.h>
#include <Xm/Text.h>
#include <Xm/Xm.h>
#include <mx.h>
#include <mx_file_type.h>
#include <mx_std.h>
#include <mx_ui_object.h>

#include "mx_goto_d.h"

static void goto_text_return(
    Widget widget,
    XtPointer client_data,
    XtPointer call_data)
{
    mx_goto_d* d = (mx_goto_d*)client_data;
    char* s = XmTextGetString(d->goto_text);
    d->page_num = atoi(s);
    XtFree(s);
    if (d->page_num < 1) {
        if (d->bad_number_d == NULL) {
            d->bad_number_d = new mx_inform_d("badPageNumber", d->dialog, error_e);
        }
        d->bad_number_d->centre();
        d->bad_number_d->run_modal();
        d->bad_number_d->deactivate();
    } else {
        d->modal_result = yes_e;
    }
}

static void button_cb(
    Widget widget,
    XtPointer client_data,
    XtPointer call_data)
{
    mx_goto_d* d = (mx_goto_d*)client_data;

    if (widget == d->goto_button) {
        goto_text_return(widget, client_data, call_data);
    } else {
        d->modal_result = cancel_e;
    }
}

static void goto_text_changed(
    Widget widget,
    XtPointer client_data,
    XtPointer call_data)
{
    mx_goto_d* d = (mx_goto_d*)client_data;
    char* s = XmTextGetString(d->goto_text);

    XtVaSetValues(d->goto_button,
        XmNsensitive, mx_is_blank(s) ? False : True,
        NULL);
    XtFree(s);
}

mx_goto_d::~mx_goto_d()
{
    if (bad_number_d != NULL) {
        delete bad_number_d;
    }
}

mx_goto_d::mx_goto_d(Widget parent)
    : mx_dialog("goto", parent, TRUE, FALSE)
{
    Widget label;

    XtVaSetValues(action_area, XmNfractionBase, 5, NULL);

    bad_number_d = NULL;

    label = XtVaCreateManagedWidget(
        "gotoLabel",
        xmLabelGadgetClass,
        control_area,
        XmNtopAttachment, XmATTACH_FORM,
        XmNleftAttachment, XmATTACH_FORM,
        NULL);

    goto_text = XtVaCreateManagedWidget("gotoText",
        xmTextWidgetClass, control_area,
        XmNleftAttachment, XmATTACH_WIDGET,
        XmNleftWidget, label,
        XmNtopAttachment, XmATTACH_FORM,
        NULL);

    XtAddCallback(goto_text, XmNvalueChangedCallback, goto_text_changed, this);
    XtAddCallback(goto_text, XmNactivateCallback, goto_text_return, this);

    // now, the action buttons
    goto_button = XtVaCreateManagedWidget(
        "goto",
        xmPushButtonGadgetClass, action_area,
        XmNshowAsDefault, True,
        XmNdefaultButtonShadowThickness, 1,
        XmNleftAttachment, XmATTACH_POSITION,
        XmNleftPosition, 1,
        XmNrightAttachment, XmATTACH_POSITION,
        XmNrightPosition, 2,
        XmNsensitive, False,
        NULL);

    XtAddCallback(
        goto_button,
        XmNactivateCallback,
        button_cb,
        (XtPointer)this);

    cancel_button = XtVaCreateManagedWidget(
        "cancel",
        xmPushButtonGadgetClass, action_area,
        XmNshowAsDefault, False,
        XmNdefaultButtonShadowThickness, 1,
        XmNleftAttachment, XmATTACH_POSITION,
        XmNleftPosition, 3,
        XmNrightAttachment, XmATTACH_POSITION,
        XmNrightPosition, 4,
        NULL);

    XtAddCallback(
        cancel_button,
        XmNactivateCallback,
        button_cb,
        (XtPointer)this);

    XtManageChild(action_area);
    XtManageChild(control_area);
    XtManageChild(pane);
}
