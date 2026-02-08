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
 * MODULE/CLASS : mx_index_d
 *
 * AUTHOR : Andrew Haisley
 *
 * This file
 *
 * DESCRIPTION:
 *
 *  An index insert dialog
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
#include <Xm/Form.h>
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

#include "mx_index_d.h"

static void button_cb(
    Widget widget,
    XtPointer client_data,
    XtPointer call_data)
{
    mx_index_d* d = (mx_index_d*)client_data;

    if (widget == d->ok_button) {
        d->validate_data();
    } else {
        d->modal_result = cancel_e;
    }
}

static void text_return(
    Widget widget,
    XtPointer client_data,
    XtPointer call_data)
{
    mx_index_d* d = (mx_index_d*)client_data;

    d->validate_data();
}

mx_index_d::mx_index_d(Widget parent)
    : mx_dialog("index", parent, TRUE, FALSE)
{
    Widget form1, form2, label1, label2;

    invalid_from_d = NULL;
    invalid_to_d = NULL;

    XtVaSetValues(action_area, XmNfractionBase, 5, NULL);

    form1 = XtVaCreateManagedWidget(
        "form1",
        xmFormWidgetClass, control_area,
        XmNleftAttachment, XmATTACH_FORM,
        XmNtopAttachment, XmATTACH_FORM,
        XmNbottomAttachment, XmATTACH_FORM,
        NULL);

    form2 = XtVaCreateManagedWidget(
        "form2",
        xmFormWidgetClass, control_area,
        XmNleftAttachment, XmATTACH_WIDGET,
        XmNleftWidget, form1,
        XmNtopAttachment, XmATTACH_FORM,
        XmNrightAttachment, XmATTACH_FORM,
        XmNbottomAttachment, XmATTACH_FORM,
        NULL);

    label1 = XtVaCreateManagedWidget(
        "replaceLabel",
        xmLabelGadgetClass,
        form1,
        XmNtopAttachment, XmATTACH_FORM,
        XmNleftAttachment, XmATTACH_FORM,
        NULL);

    label2 = XtVaCreateManagedWidget(
        "startLabel",
        xmLabelGadgetClass,
        form1,
        XmNtopAttachment, XmATTACH_WIDGET,
        XmNtopWidget, label1,
        XmNleftAttachment, XmATTACH_FORM,
        NULL);

    (void)XtVaCreateManagedWidget(
        "endLabel",
        xmLabelGadgetClass,
        form1,
        XmNtopAttachment, XmATTACH_WIDGET,
        XmNtopWidget, label2,
        XmNleftAttachment, XmATTACH_FORM,
        NULL);

    replace_toggle = XtVaCreateManagedWidget(
        "replaceToggle",
        xmToggleButtonGadgetClass, form2,
        XmNleftAttachment, XmATTACH_FORM,
        XmNtopAttachment, XmATTACH_FORM,
        XmNset, True,
        NULL);

    from_text = XtVaCreateManagedWidget("fromText",
        xmTextWidgetClass, form2,
        XmNleftAttachment, XmATTACH_FORM,
        XmNtopAttachment, XmATTACH_WIDGET,
        XmNtopWidget, replace_toggle,
        NULL);

    to_text = XtVaCreateManagedWidget("toText",
        xmTextWidgetClass, form2,
        XmNleftAttachment, XmATTACH_FORM,
        XmNtopAttachment, XmATTACH_WIDGET,
        XmNtopWidget, from_text,
        NULL);

    XtAddCallback(to_text, XmNactivateCallback, text_return, this);
    XtAddCallback(from_text, XmNactivateCallback, text_return, this);
    XtAddCallback(to_text, XmNvalueChangedCallback, text_return, this);
    XtAddCallback(from_text, XmNvalueChangedCallback, text_return, this);

    // now, the action buttons
    ok_button = XtVaCreateManagedWidget(
        "ok",
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
        ok_button,
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

void mx_index_d::activate_d(int num_pages)
{
    char s[20];

    sprintf(s, "%d", num_pages);
    XmTextSetString(from_text, const_cast<char*>("1"));
    XmTextSetString(to_text, s);
}

void mx_index_d::validate_data()
{
    char* s;

    s = XmTextGetString(from_text);
    if (s != NULL) {
        start_page = atoi(s);
        XtFree(s);
    }
    if (s == NULL || start_page <= 0) {
        if (invalid_from_d == NULL) {
            invalid_from_d = new mx_inform_d("invalidFromIndex", dialog, error_e);
        }
        invalid_from_d->centre();
        invalid_from_d->run_modal();
        invalid_from_d->deactivate();
        return;
    }

    s = XmTextGetString(to_text);
    if (s != NULL) {
        end_page = atoi(s);
        XtFree(s);
    }
    if (s == NULL || end_page <= 0) {
        if (invalid_to_d == NULL) {
            invalid_to_d = new mx_inform_d("invalidToIndex", dialog, error_e);
        }
        invalid_to_d->centre();
        invalid_to_d->run_modal();
        invalid_to_d->deactivate();
        return;
    }

    replace_existing = XmToggleButtonGetState(replace_toggle);
    modal_result = yes_e;
}
