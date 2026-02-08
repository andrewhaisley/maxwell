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

#include <dirent.h>
#include <errno.h>
#include <pwd.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>

#include "mx_dialog.h"
#include <Xm/Form.h>
#include <Xm/Frame.h>
#include <Xm/Label.h>
#include <Xm/LabelG.h>
#include <Xm/List.h>
#include <Xm/Protocols.h>
#include <Xm/PushBG.h>
#include <Xm/RowColumn.h>
#include <Xm/Separator.h>
#include <Xm/Text.h>
#include <Xm/ToggleB.h>
#include <Xm/ToggleBG.h>
#include <Xm/Xm.h>
#include <mx.h>
#include <mx_file_type.h>
#include <mx_std.h>
#include <mx_ui_object.h>

#include "mx_error_d.h"

static void button_cb(
    Widget widget,
    XtPointer client_data,
    XtPointer call_data)
{
    mx_error_d* d = (mx_error_d*)client_data;

    if (widget == d->continue_button) {
        d->modal_result = yes_e;
    } else {
        if (widget == d->exit_button) {
            d->modal_result = no_e;
        }
    }
}

mx_error_d::mx_error_d(Widget parent)
    : mx_dialog("error", parent, TRUE, FALSE)
{
    Widget label, label1;

    XtVaSetValues(action_area, XmNfractionBase, 5, NULL);

    label = XtVaCreateManagedWidget(
        "label",
        xmLabelGadgetClass,
        control_area,
        XmNtopAttachment, XmATTACH_FORM,
        XmNleftAttachment, XmATTACH_FORM,
        XmNrightAttachment, XmATTACH_FORM,
        NULL);

    label1 = XtVaCreateManagedWidget(
        "label1",
        xmLabelGadgetClass,
        control_area,
        XmNtopAttachment, XmATTACH_WIDGET,
        XmNtopWidget, label,
        XmNleftAttachment, XmATTACH_FORM,
        NULL);

    (void)XtVaCreateManagedWidget(
        "label2",
        xmLabelGadgetClass,
        control_area,
        XmNtopAttachment, XmATTACH_WIDGET,
        XmNtopWidget, label1,
        XmNleftAttachment, XmATTACH_FORM,
        NULL);

    error_label = XtVaCreateManagedWidget(
        "errorText",
        xmLabelGadgetClass,
        control_area,
        XmNtopAttachment, XmATTACH_WIDGET,
        XmNtopWidget, label,
        XmNleftAttachment, XmATTACH_WIDGET,
        XmNleftWidget, label1,
        NULL);

    error_code_label = XtVaCreateManagedWidget(
        "errorCodeText",
        xmLabelGadgetClass,
        control_area,
        XmNtopAttachment, XmATTACH_WIDGET,
        XmNtopWidget, error_label,
        XmNleftAttachment, XmATTACH_WIDGET,
        XmNleftWidget, label1,
        NULL);

    // now, the action buttons
    exit_button = XtVaCreateManagedWidget(
        "exit",
        xmPushButtonGadgetClass, action_area,
        XmNshowAsDefault, True,
        XmNdefaultButtonShadowThickness, 1,
        XmNleftAttachment, XmATTACH_POSITION,
        XmNleftPosition, 1,
        XmNrightAttachment, XmATTACH_POSITION,
        XmNrightPosition, 2,
        NULL);

    XtAddCallback(
        exit_button,
        XmNactivateCallback,
        button_cb,
        (XtPointer)this);

    continue_button = XtVaCreateManagedWidget(
        "continue",
        xmPushButtonGadgetClass, action_area,
        XmNshowAsDefault, True,
        XmNdefaultButtonShadowThickness, 1,
        XmNleftAttachment, XmATTACH_POSITION,
        XmNleftPosition, 3,
        XmNrightAttachment, XmATTACH_POSITION,
        XmNrightPosition, 4,
        NULL);

    XtAddCallback(
        continue_button,
        XmNactivateCallback,
        button_cb,
        (XtPointer)this);

    XtManageChild(action_area);
    XtManageChild(control_area);
    XtManageChild(pane);
}

void mx_error_d::activate_d(int error_code)
{
    XmString str;
    char s[20];

    sprintf(s, "%d", error_code);
    str = XmStringCreate(s, XmFONTLIST_DEFAULT_TAG);
    XtVaSetValues(error_code_label, XmNlabelString, str, NULL);
    XmStringFree(str);

    str = XmStringCreate(const_cast<char*>(global_error_trace->get_text_from_code(error_code)), XmFONTLIST_DEFAULT_TAG);
    XtVaSetValues(error_label, XmNlabelString, str, NULL);
    XmStringFree(str);

    mx_dialog::activate();
}

void mx_error_d::centre_on_display()
{
    Position dialog_x, dialog_y;
    Dimension dialog_width, dialog_height;
    Screen* sc;

    sc = XtScreen(dialog);

    XtVaGetValues(
        dialog,
        XmNwidth, &dialog_width,
        XmNheight, &dialog_height,
        NULL);

    dialog_x = (sc->width / 2) - (dialog_width / 2);
    dialog_y = (sc->height / 2) - (dialog_height / 2);

    XtVaSetValues(
        dialog,
        XmNx, dialog_x,
        XmNy, dialog_y,
        NULL);

    XRaiseWindow(XtDisplay(dialog), XtWindow(dialog));
}
