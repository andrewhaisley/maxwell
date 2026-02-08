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
 * MODULE/CLASS : mx_table_d
 *
 * AUTHOR : Andrew Haisley
 *
 * This file
 *
 * DESCRIPTION:
 *
 *  An insert table dialog.
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

#include "mx_table_d.h"

static void button_cb(
    Widget widget,
    XtPointer client_data,
    XtPointer call_data)
{
    mx_table_d* d = (mx_table_d*)client_data;

    if (widget == d->ok_button) {
        d->validate_data();
    } else {
        if (widget == d->auto_toggle) {
            if (XmToggleButtonGetState(d->auto_toggle)) {
                XtVaSetValues(d->column_width_text, XmNsensitive, False, NULL);
            } else {
                XtVaSetValues(d->column_width_text, XmNsensitive, True, NULL);
            }
        } else {
            d->modal_result = cancel_e;
        }
    }
}

static void text_changed(
    Widget widget,
    XtPointer client_data,
    XtPointer call_data)
{
    mx_table_d* d = (mx_table_d*)client_data;
    char *s1, *s2, *s3;

    s1 = XmTextGetString(d->rows_text);
    s2 = XmTextGetString(d->columns_text);
    s3 = XmTextGetString(d->column_width_text);

    if (XmToggleButtonGetState(d->auto_toggle)) {
        XtVaSetValues(d->ok_button,
            XmNsensitive, mx_is_blank(s1) || mx_is_blank(s2) ? False : True,
            NULL);
    } else {
        XtVaSetValues(d->ok_button,
            XmNsensitive, mx_is_blank(s1) || mx_is_blank(s2) || mx_is_blank(s3) ? False : True,
            NULL);
    }
    XtFree(s1);
    XtFree(s2);
    XtFree(s3);
}

static void text_return(
    Widget widget,
    XtPointer client_data,
    XtPointer call_data)
{
    mx_table_d* d = (mx_table_d*)client_data;

    d->validate_data();
}

mx_table_d::mx_table_d(Widget parent, float width_mm, mx_unit_e display_units)
    : mx_dialog("table", parent, TRUE, FALSE)
{
    Widget form1, form2, label1, label2;

    mx_table_d::width_mm = width_mm;
    mx_table_d::display_units = display_units;

    invalid_rows_d = NULL;
    invalid_columns_d = NULL;
    invalid_width_d = NULL;

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
        "rowsLabel",
        xmLabelGadgetClass,
        form1,
        XmNtopAttachment, XmATTACH_FORM,
        XmNleftAttachment, XmATTACH_FORM,
        NULL);

    label2 = XtVaCreateManagedWidget(
        "columnsLabel",
        xmLabelGadgetClass,
        form1,
        XmNtopAttachment, XmATTACH_WIDGET,
        XmNtopWidget, label1,
        XmNleftAttachment, XmATTACH_FORM,
        NULL);

    (void)XtVaCreateManagedWidget(
        "widthLabel",
        xmLabelGadgetClass,
        form1,
        XmNtopAttachment, XmATTACH_WIDGET,
        XmNtopWidget, label2,
        XmNleftAttachment, XmATTACH_FORM,
        NULL);

    rows_text = XtVaCreateManagedWidget("rowsText",
        xmTextWidgetClass, form2,
        XmNleftAttachment, XmATTACH_FORM,
        XmNtopAttachment, XmATTACH_FORM,
        NULL);

    columns_text = XtVaCreateManagedWidget("columnsText",
        xmTextWidgetClass, form2,
        XmNleftAttachment, XmATTACH_FORM,
        XmNtopAttachment, XmATTACH_WIDGET,
        XmNtopWidget, rows_text,
        NULL);

    column_width_text = XtVaCreateManagedWidget("columnWidthText",
        xmTextWidgetClass, form2,
        XmNleftAttachment, XmATTACH_FORM,
        XmNtopAttachment, XmATTACH_WIDGET,
        XmNtopWidget, columns_text,
        XmNsensitive, False,
        NULL);

    XtAddCallback(rows_text, XmNactivateCallback, text_return, this);
    XtAddCallback(columns_text, XmNactivateCallback, text_return, this);
    XtAddCallback(column_width_text, XmNactivateCallback, text_return, this);
    XtAddCallback(rows_text, XmNvalueChangedCallback, text_changed, this);
    XtAddCallback(columns_text, XmNvalueChangedCallback, text_changed, this);
    XtAddCallback(column_width_text, XmNvalueChangedCallback, text_changed, this);

    auto_toggle = XtVaCreateManagedWidget(
        "autoToggle",
        xmToggleButtonGadgetClass, form2,
        XmNleftAttachment, XmATTACH_WIDGET,
        XmNleftWidget, column_width_text,
        XmNtopAttachment, XmATTACH_WIDGET,
        XmNtopWidget, columns_text,
        XmNset, True,
        NULL);

    XtAddCallback(
        auto_toggle,
        XmNvalueChangedCallback,
        button_cb,
        (XtPointer)this);

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

    XtManageChild(form1);
    XtManageChild(form2);
    XtManageChild(action_area);
    XtManageChild(control_area);
    XtManageChild(pane);
}

void mx_table_d::validate_data()
{
    char* s;

    s = XmTextGetString(rows_text);
    if (s != NULL) {
        num_rows = atoi(s);
        XtFree(s);
    }
    if (s == NULL || num_rows <= 0) {
        if (invalid_rows_d == NULL) {
            invalid_rows_d = new mx_inform_d("invalidRows", dialog, error_e);
        }
        invalid_rows_d->centre();
        invalid_rows_d->run_modal();
        invalid_rows_d->deactivate();
        return;
    }

    s = XmTextGetString(columns_text);
    if (s != NULL) {
        num_columns = atoi(s);
        XtFree(s);
    }
    if (s == NULL || num_columns <= 0) {
        if (invalid_columns_d == NULL) {
            invalid_columns_d = new mx_inform_d("invalidColumns", dialog, error_e);
        }
        invalid_columns_d->centre();
        invalid_columns_d->run_modal();
        invalid_columns_d->deactivate();
        return;
    }

    if (XmToggleButtonGetState(auto_toggle)) {
        column_width_mm = width_mm / num_columns;
    } else {
        s = XmTextGetString(column_width_text);
        if (s != NULL) {
            column_width_mm = atof(s);
            column_width_mm = mx_unit_to_mm(column_width_mm, display_units);
            XtFree(s);
        }
        if (s == NULL || column_width_mm <= 0.0) {
            if (invalid_width_d == NULL) {
                invalid_width_d = new mx_inform_d("invalidWidth", dialog, error_e);
            }
            invalid_width_d->centre();
            invalid_width_d->run_modal();
            invalid_width_d->deactivate();
            return;
        }
    }
    modal_result = yes_e;
}

void mx_table_d::activate_d(float page_width_mm, mx_unit_e u)
{
    char *s1, *s2, *s3;

    s1 = XmTextGetString(rows_text);
    s2 = XmTextGetString(columns_text);
    s3 = XmTextGetString(column_width_text);

    display_units = u;
    width_mm = page_width_mm;

    if (XmToggleButtonGetState(auto_toggle)) {
        XtVaSetValues(ok_button,
            XmNsensitive, mx_is_blank(s1) || mx_is_blank(s2) ? False : True,
            NULL);
    } else {
        XtVaSetValues(ok_button,
            XmNsensitive, mx_is_blank(s1) || mx_is_blank(s2) || mx_is_blank(s3) ? False : True,
            NULL);
    }
    XtFree(s1);
    XtFree(s2);
    XtFree(s3);
    mx_dialog::activate();
}

int mx_table_d::run(float p, mx_unit_e u)
{
    int res = 0;
    centre();
    activate_d(p, u);
    res = run_modal();
    deactivate();
    return res;
}
