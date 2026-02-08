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
 * MODULE/CLASS : mx_replace_d
 *
 * AUTHOR : Andrew Haisley
 *
 * This file
 *
 * DESCRIPTION:
 *
 *  A search dialog.
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
#include <Xm/ComboBox.h>
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

#include "mx_replace_d.h"

static void button_cb(
    Widget widget,
    XtPointer client_data,
    XtPointer call_data)
{
    mx_replace_d* d = (mx_replace_d*)client_data;

    if (widget == d->prev_button) {
        d->replace_all = FALSE;
        d->modal_result = yes_e;
        d->add_pattern();
        d->figure_options();
        d->direction = mx_search_backward_e;
    } else {
        if (widget == d->next_button) {
            d->modal_result = yes_e;
            d->replace_all = FALSE;
            d->add_pattern();
            d->figure_options();
            d->direction = mx_search_forward_e;
        } else {
            if (widget == d->all_button) {
                d->modal_result = yes_e;
                d->replace_all = TRUE;
                d->add_pattern();
                d->figure_options();
            } else {
                d->modal_result = cancel_e;
            }
        }
    }
}

static void pattern_return(
    Widget widget,
    XtPointer client_data,
    XtPointer call_data)
{
    mx_replace_d* d = (mx_replace_d*)client_data;

    d->modal_result = yes_e;
    d->add_pattern();
    d->figure_options();
}

static void pattern_changed(
    Widget widget,
    XtPointer client_data,
    XtPointer call_data)
{
    mx_replace_d* d = (mx_replace_d*)client_data;
    char* s = XmTextGetString(d->pattern_text);

    if (mx_is_blank(s)) {
        XtVaSetValues(d->next_button, XmNsensitive, False, NULL);
        XtVaSetValues(d->prev_button, XmNsensitive, False, NULL);
        XtVaSetValues(d->all_button, XmNsensitive, False, NULL);
    } else {
        XtVaSetValues(d->next_button, XmNsensitive, True, NULL);
        XtVaSetValues(d->prev_button, XmNsensitive, True, NULL);
        XtVaSetValues(d->all_button, XmNsensitive, True, NULL);
    }
    XtFree(s);
}

mx_replace_d::mx_replace_d(Widget parent)
    : mx_dialog("replace", parent, TRUE, FALSE)
{
    Widget combo, label;

    XtVaSetValues(action_area, XmNfractionBase, 9, NULL);
    direction = mx_search_forward_e;

    num_patterns = 0;

    label = XtVaCreateManagedWidget(
        "findLabel",
        xmLabelGadgetClass,
        control_area,
        XmNtopAttachment, XmATTACH_FORM,
        XmNleftAttachment, XmATTACH_FORM,
        NULL);

    combo = XmCreateDropDownComboBox(control_area, const_cast<char*>("combo"), NULL, 0);
    XtVaSetValues(combo,
        XmNleftAttachment, XmATTACH_WIDGET,
        XmNleftWidget, label,
        XmNtopAttachment, XmATTACH_FORM,
        NULL);

    pattern_text = XtNameToWidget(combo, "*Text");
    pattern_list = XtNameToWidget(combo, "*List");

    XtAddCallback(pattern_text, XmNvalueChangedCallback, pattern_changed, this);
    XtAddCallback(pattern_text, XmNactivateCallback, pattern_return, this);

    XtVaSetValues(pattern_text,
        XmNeditable, True,
        XmNcursorPosition, True,
        NULL);

    XtVaSetValues(pattern_list,
        XmNvisibleItemCount, 0,
        NULL);

    label = XtVaCreateManagedWidget(
        "replaceWith",
        xmLabelGadgetClass,
        control_area,
        XmNtopAttachment, XmATTACH_FORM,
        XmNleftAttachment, XmATTACH_WIDGET,
        XmNleftWidget, combo,
        NULL);

    replace_text = XtVaCreateManagedWidget("replaceText",
        xmTextWidgetClass, control_area,
        XmNleftAttachment, XmATTACH_WIDGET,
        XmNleftWidget, label,
        XmNtopAttachment, XmATTACH_FORM,
        XmNrightAttachment, XmATTACH_FORM,
        NULL);

    XtAddCallback(replace_text, XmNactivateCallback, pattern_return, this);

    label = XtVaCreateManagedWidget(
        "matchCase",
        xmLabelGadgetClass,
        control_area,
        XmNtopAttachment, XmATTACH_WIDGET,
        XmNtopWidget, combo,
        XmNleftAttachment, XmATTACH_FORM,
        NULL);

    case_toggle = XtVaCreateManagedWidget(
        "caseToggle",
        xmToggleButtonGadgetClass,
        control_area,
        XmNleftAttachment, XmATTACH_WIDGET,
        XmNleftWidget, label,
        XmNtopAttachment, XmATTACH_WIDGET,
        XmNtopWidget, combo,
        XmNset, False,
        NULL);

    // now, the action buttons
    prev_button = XtVaCreateManagedWidget(
        "prev",
        xmPushButtonGadgetClass, action_area,
        XmNshowAsDefault, True,
        XmNdefaultButtonShadowThickness, 1,
        XmNleftAttachment, XmATTACH_FORM,
        XmNtopAttachment, XmATTACH_FORM,
        XmNsensitive, False,
        NULL);

    XtAddCallback(
        prev_button,
        XmNactivateCallback,
        button_cb,
        (XtPointer)this);

    next_button = XtVaCreateManagedWidget(
        "next",
        xmPushButtonGadgetClass, action_area,
        XmNshowAsDefault, True,
        XmNdefaultButtonShadowThickness, 1,
        XmNleftAttachment, XmATTACH_WIDGET,
        XmNleftWidget, prev_button,
        XmNtopAttachment, XmATTACH_FORM,
        XmNsensitive, False,
        NULL);

    XtAddCallback(
        next_button,
        XmNactivateCallback,
        button_cb,
        (XtPointer)this);

    all_button = XtVaCreateManagedWidget(
        "all",
        xmPushButtonGadgetClass, action_area,
        XmNshowAsDefault, True,
        XmNdefaultButtonShadowThickness, 1,
        XmNleftAttachment, XmATTACH_WIDGET,
        XmNleftWidget, next_button,
        XmNtopAttachment, XmATTACH_FORM,
        XmNsensitive, False,
        NULL);

    XtAddCallback(
        all_button,
        XmNactivateCallback,
        button_cb,
        (XtPointer)this);

    cancel_button = XtVaCreateManagedWidget(
        "cancel",
        xmPushButtonGadgetClass, action_area,
        XmNshowAsDefault, False,
        XmNdefaultButtonShadowThickness, 1,
        XmNleftAttachment, XmATTACH_WIDGET,
        XmNleftWidget, all_button,
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

void mx_replace_d::add_pattern()
{
    char* s;
    XmString item;

    s = XmTextGetString(pattern_text);
    item = XmStringCreateLocalized(s);
    XtFree(s);

    if (!XmListItemExists(pattern_list, item)) {
        XmListAddItem(pattern_list, item, 1);
        num_patterns++;
        XtVaSetValues(pattern_list,
            XmNvisibleItemCount, num_patterns < 25 ? num_patterns : 25,
            NULL);
    }

    XmStringFree(item);
}

void mx_replace_d::figure_options()
{
    char* s = XmTextGetString(pattern_text);
    char* s2 = XmTextGetString(replace_text);

    // match case ?
    match_case = XmToggleButtonGetState(case_toggle);
    strncpy(search_pattern, s, MAX_SEARCH_PATTERN);
    search_pattern[MAX_SEARCH_PATTERN - 1] = 0;
    strncpy(replace_string, s2, MAX_SEARCH_PATTERN);
    replace_string[MAX_SEARCH_PATTERN - 1] = 0;
    XtFree(s);
    XtFree(s2);
}
