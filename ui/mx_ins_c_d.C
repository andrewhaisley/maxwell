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
 * MODULE/CLASS : mx_ins_c_d
 *
 * AUTHOR : Andrew Haisley
 *
 * This file
 *
 * DESCRIPTION:
 *
 *  Inserting columns into a table
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
#include <Xm/ComboBox.h>
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

#include "mx_ins_c_d.h"

static void button_cb(
    Widget widget,
    XtPointer client_data,
    XtPointer call_data)
{
    mx_ins_c_d* d = (mx_ins_c_d*)client_data;

    if (widget == d->ok_left_button) {
        if (d->figure_options()) {
            d->insert_to_left = TRUE;
            d->modal_result = yes_e;
        }
    } else {
        if (widget == d->ok_right_button) {
            if (d->figure_options()) {
                d->insert_to_left = FALSE;
                d->modal_result = yes_e;
            }
        } else {
            d->modal_result = cancel_e;
        }
    }
}

static void text_return(
    Widget widget,
    XtPointer client_data,
    XtPointer call_data)
{
    mx_ins_c_d* d = (mx_ins_c_d*)client_data;

    if (d->figure_options()) {
        d->modal_result = yes_e;
    }
}

static void text_changed(
    Widget widget,
    XtPointer client_data,
    XtPointer call_data)
{
    mx_ins_c_d* d = (mx_ins_c_d*)client_data;
    char* s = XmTextGetString(d->num_columns_text);

    XtVaSetValues(d->ok_left_button,
        XmNsensitive, mx_is_blank(s) ? False : True,
        NULL);

    XtVaSetValues(d->ok_right_button,
        XmNsensitive, mx_is_blank(s) ? False : True,
        NULL);
    XtFree(s);
}

mx_ins_c_d::~mx_ins_c_d()
{
    if (bad_number_d != NULL) {
        delete bad_number_d;
    }
}

mx_ins_c_d::mx_ins_c_d(Widget parent)
    : mx_dialog("insertColumns", parent, TRUE, FALSE)
{
    Widget label;
    Widget combo, list;

    XtVaSetValues(action_area, XmNfractionBase, 7, NULL);

    bad_number_d = NULL;

    label = XtVaCreateManagedWidget(
        "numColumnsLabel",
        xmLabelGadgetClass,
        control_area,
        XmNtopAttachment, XmATTACH_FORM,
        XmNleftAttachment, XmATTACH_FORM,
        NULL);

    combo = XmCreateDropDownComboBox(control_area, const_cast<char*>("numColumnsCombo"), NULL, 0);

    XtVaSetValues(combo,
        XmNleftAttachment, XmATTACH_WIDGET,
        XmNleftWidget, label,
        XmNtopAttachment, XmATTACH_FORM,
        NULL);

    num_columns_text = XtNameToWidget(combo, "*Text");
    list = XtNameToWidget(combo, "*List");

    XtVaSetValues(list, XmNvisibleItemCount, 5, NULL);

    fill_list(list);

    XtAddCallback(num_columns_text, XmNactivateCallback, text_return, this);
    XtAddCallback(num_columns_text, XmNvalueChangedCallback, text_changed, this);

    // now, the action buttons
    ok_left_button = XtVaCreateManagedWidget(
        "okLeft",
        xmPushButtonGadgetClass, action_area,
        XmNshowAsDefault, True,
        XmNleftAttachment, XmATTACH_FORM,
        XmNtopAttachment, XmATTACH_FORM,
        XmNdefaultButtonShadowThickness, 1,
        XmNsensitive, False,
        NULL);

    XtAddCallback(
        ok_left_button,
        XmNactivateCallback,
        button_cb,
        (XtPointer)this);

    ok_right_button = XtVaCreateManagedWidget(
        "okRight",
        xmPushButtonGadgetClass, action_area,
        XmNshowAsDefault, False,
        XmNdefaultButtonShadowThickness, 1,
        XmNleftAttachment, XmATTACH_WIDGET,
        XmNleftWidget, ok_left_button,
        XmNtopAttachment, XmATTACH_FORM,
        XmNsensitive, False,
        NULL);

    XtAddCallback(
        ok_right_button,
        XmNactivateCallback,
        button_cb,
        (XtPointer)this);

    cancel_button = XtVaCreateManagedWidget(
        "cancel",
        xmPushButtonGadgetClass, action_area,
        XmNshowAsDefault, False,
        XmNdefaultButtonShadowThickness, 1,
        XmNleftAttachment, XmATTACH_WIDGET,
        XmNleftWidget, ok_right_button,
        XmNtopAttachment, XmATTACH_FORM,
        NULL);

    XtAddCallback(
        cancel_button,
        XmNactivateCallback,
        button_cb,
        (XtPointer)this);

    XtManageChild(combo);
    XtManageChild(action_area);
    XtManageChild(control_area);
    XtManageChild(pane);
}

bool mx_ins_c_d::figure_options()
{
    char* s = XmTextGetString(num_columns_text);
    num_columns = atoi(s);
    XtFree(s);

    if (num_columns < 1 || num_columns > 100) {
        if (bad_number_d == NULL) {
            bad_number_d = new mx_inform_d("badInsColumns", dialog, error_e);
        }
        bad_number_d->centre();
        bad_number_d->run_modal();
        bad_number_d->deactivate();
        return FALSE;
    } else {
        return TRUE;
    }
}

void mx_ins_c_d::fill_list(Widget w)
{
    XmString str[5];
    char s[5];

    int i;

    for (i = 0; i < 5; i++) {
        sprintf(s, "%d", i + 1);
        str[i] = XmStringCreate(s, XmFONTLIST_DEFAULT_TAG);
    }

    XtVaSetValues(w,
        XmNitemCount, 5,
        XmNitems, str,
        NULL);

    for (i = 0; i < 5; i++) {
        XmStringFree(str[i]);
    }

    return;
}
