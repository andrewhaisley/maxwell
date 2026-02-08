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
 * MODULE/CLASS : mx_column_d
 *
 * AUTHOR : Andrew Haisley
 *
 * This file
 *
 * DESCRIPTION:
 *
 *  Formatting columns of text
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
#include <mx_std.h>
#include <mx_ui_object.h>
// #include "mx_yes_no_d.h"
// #include "mx_open_d.h"
#include <mx_file_type.h>

#include "mx_column_d.h"

static void button_cb(
    Widget widget,
    XtPointer client_data,
    XtPointer call_data)
{
    mx_column_d* d = (mx_column_d*)client_data;

    if (widget == d->ok_button) {
        if (d->figure_options()) {
            d->modal_result = yes_e;
        }
    } else {
        if (widget == d->apply_button) {
            if (d->figure_options()) {
                d->modal_result = apply_e;
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
    mx_column_d* d = (mx_column_d*)client_data;
    char *s1, *s2;

    s1 = XmTextGetString(d->space_between_text);
    s2 = XmTextGetString(d->num_columns_text);

    XtVaSetValues(d->ok_button,
        XmNsensitive, mx_is_blank(s1) || mx_is_blank(s2) ? False : True,
        NULL);

    XtVaSetValues(d->apply_button,
        XmNsensitive, mx_is_blank(s1) || mx_is_blank(s2) ? False : True,
        NULL);
    XtFree(s1);
    XtFree(s2);
}

static void text_return(
    Widget widget,
    XtPointer client_data,
    XtPointer call_data)
{
    mx_column_d* d = (mx_column_d*)client_data;

    if (d->figure_options()) {
        d->modal_result = yes_e;
    }
}

mx_column_d::mx_column_d(Widget parent)
    : mx_valid_d("columns", parent, TRUE, FALSE)
{
    Widget form1, form2, label1;
    Widget combo, list;

    XtVaSetValues(action_area, XmNfractionBase, 7, NULL);

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
        "numColumnsLabel",
        xmLabelGadgetClass,
        form1,
        XmNtopAttachment, XmATTACH_FORM,
        XmNleftAttachment, XmATTACH_FORM,
        NULL);

    (void)XtVaCreateManagedWidget(
        "spaceBetweenLabel",
        xmLabelGadgetClass,
        form1,
        XmNtopAttachment, XmATTACH_WIDGET,
        XmNtopWidget, label1,
        XmNleftAttachment, XmATTACH_FORM,
        NULL);

    combo = XmCreateDropDownComboBox(form2, const_cast<char*>("numColumnsCombo"), NULL, 0);

    XtVaSetValues(combo,
        XmNleftAttachment, XmATTACH_FORM,
        XmNtopAttachment, XmATTACH_FORM,
        NULL);

    num_columns_text = XtNameToWidget(combo, "*Text");
    list = XtNameToWidget(combo, "*List");

    XtVaSetValues(list, XmNvisibleItemCount, 5, NULL);

    fill_list(list);

    space_between_text = XtVaCreateManagedWidget("spaceBetweenText",
        xmTextWidgetClass, form2,
        XmNleftAttachment, XmATTACH_FORM,
        XmNtopAttachment, XmATTACH_WIDGET,
        XmNtopWidget, combo,
        NULL);

    XtAddCallback(space_between_text, XmNactivateCallback, text_return, this);
    XtAddCallback(num_columns_text, XmNactivateCallback, text_return, this);
    XtAddCallback(space_between_text, XmNvalueChangedCallback, text_changed, this);
    XtAddCallback(num_columns_text, XmNvalueChangedCallback, text_changed, this);

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
        NULL);

    XtAddCallback(
        ok_button,
        XmNactivateCallback,
        button_cb,
        (XtPointer)this);

    apply_button = XtVaCreateManagedWidget(
        "apply",
        xmPushButtonGadgetClass, action_area,
        XmNshowAsDefault, True,
        XmNdefaultButtonShadowThickness, 1,
        XmNleftAttachment, XmATTACH_POSITION,
        XmNleftPosition, 3,
        XmNrightAttachment, XmATTACH_POSITION,
        XmNrightPosition, 4,
        NULL);

    XtAddCallback(
        apply_button,
        XmNactivateCallback,
        button_cb,
        (XtPointer)this);

    cancel_button = XtVaCreateManagedWidget(
        "cancel",
        xmPushButtonGadgetClass, action_area,
        XmNshowAsDefault, False,
        XmNdefaultButtonShadowThickness, 1,
        XmNleftAttachment, XmATTACH_POSITION,
        XmNleftPosition, 5,
        XmNrightAttachment, XmATTACH_POSITION,
        XmNrightPosition, 6,
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

void mx_column_d::activate_d(int num_columns, float column_space, mx_unit_e u)
{
    units = u;
    mx_column_d::num_columns = num_columns;
    mx_column_d::column_space = column_space;

    set_options();
    mx_dialog::activate();
}

bool mx_column_d::figure_options()
{
    char* s;
    float f;

    if (!validate_float("invalidSpaceBetween", space_between_text, 0.0, 10000.0))
        return FALSE;
    if (!validate_int("invalidNumColumns", num_columns_text, 1, 20))
        return FALSE;

    s = XmTextGetString(space_between_text);
    sscanf(s, "%f", &f);
    XtFree(s);
    column_space = mx_unit_to_mm(f, units);

    if (column_space < 0.0) {
        column_space = 0.0;
    }

    s = XmTextGetString(num_columns_text);
    num_columns = atoi(s);
    XtFree(s);

    return TRUE;
}

void mx_column_d::set_options()
{
    char s[50];
    float f;

    f = mx_mm_to_unit(column_space, units);
    sprintf(s, "%.2f %s", f, mx_unit_name(units));
    XmTextSetString(space_between_text, s);

    sprintf(s, "%d", num_columns);
    XmTextSetString(num_columns_text, s);
}

void mx_column_d::fill_list(Widget w)
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

int mx_column_d::run(int num_columns, float column_space, mx_unit_e u)
{
    int res;

    centre();
    activate_d(num_columns, column_space, u);
    res = run_modal();
    deactivate();

    return res;
}
