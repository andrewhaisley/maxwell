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
 * MODULE/CLASS : mx_style_d
 *
 * AUTHOR : Andrew Haisley
 *
 * This file
 *
 * DESCRIPTION:
 *
 *  A style editing dialog
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
#include "mx_para_d.h"
#include "mx_yes_no_d.h"
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
#include <mx_std.h>
#include <mx_ui_object.h>

#include "mx_style_d.h"
#if 0
#include <mx_document.h>
#endif

static void button_cb(
    Widget widget,
    XtPointer client_data,
    XtPointer call_data)
{
    mx_style_d* d = (mx_style_d*)client_data;

    if (widget == d->ok_button) {
        d->modal_result = yes_e;
    } else {
        if (widget == d->cancel_button) {
            d->modal_result = cancel_e;
        } else {
            d->handle_button(widget);
        }
    }
}

static void changed_cb(
    Widget widget,
    XtPointer client_data,
    XtPointer call_data)
{
    mx_style_d* d = (mx_style_d*)client_data;

    d->handle_change();
}

static void text_return(
    Widget widget,
    XtPointer client_data,
    XtPointer call_data)
{
    mx_style_d* d = (mx_style_d*)client_data;
    int i;

    XtVaGetValues(d->add_button, XmNsensitive, &i, NULL);

    if (i) {
        d->handle_button(d->add_button);
    }
}

static void list_cb(Widget list, XtPointer client_data, XtPointer call_data)
{
    mx_style_d* d = (mx_style_d*)client_data;

    char* choice;

    XmListCallbackStruct* cbs = (XmListCallbackStruct*)call_data;

    XmStringGetLtoR(cbs->item, XmFONTLIST_DEFAULT_TAG, &choice);
    XmTextSetString(d->style_name_text, choice);
    XtFree(choice);

    XtVaSetValues(d->edit_button, XmNsensitive, True, NULL);
    XtVaSetValues(d->delete_button, XmNsensitive, True, NULL);

    d->handle_change();

    if (cbs->reason == XmCR_BROWSE_SELECT) {
        // single clicked on a style
        return;
    }

    if (cbs->reason == XmCR_DEFAULT_ACTION) {
        // double clicked on a style
        d->edit_style();
    }
}

mx_style_d::mx_style_d(Widget parent)
    : mx_dialog("style", parent, TRUE, FALSE)
{
    Widget label;

    XtVaSetValues(action_area, XmNfractionBase, 5, NULL);

    para_d = NULL;
    too_many_d = NULL;
    sure_d = NULL;
    edited = FALSE;

    style_list = XmCreateScrolledList(control_area, const_cast<char*>("styleList"), NULL, 0);

    XtVaSetValues(style_list,
        XmNselectionPolicy, XmBROWSE_SELECT,
        NULL);

    XtVaSetValues(XtParent(style_list),
        XmNtopAttachment, XmATTACH_FORM,
        XmNleftAttachment, XmATTACH_FORM,
        XmNbottomAttachment, XmATTACH_FORM,
        XmNleftOffset, 5,
        XmNbottomOffset, 5,
        XmNtopOffset, 5,
        NULL);

    XtAddCallback(style_list, XmNdefaultActionCallback, list_cb, this);
    XtAddCallback(style_list, XmNbrowseSelectionCallback, list_cb, this);

    label = XtVaCreateManagedWidget(
        "nameLabel",
        xmLabelGadgetClass,
        control_area,
        XmNtopAttachment, XmATTACH_FORM,
        XmNleftAttachment, XmATTACH_WIDGET,
        XmNleftWidget, XtParent(style_list),
        NULL);

    style_name_text = XtVaCreateManagedWidget("styleNameText",
        xmTextWidgetClass, control_area,
        XmNleftAttachment, XmATTACH_WIDGET,
        XmNleftWidget, label,
        XmNtopAttachment, XmATTACH_FORM,
        NULL);

    XtAddCallback(
        style_name_text,
        XmNvalueChangedCallback,
        changed_cb,
        (XtPointer)this);

    XtAddCallback(
        style_name_text,
        XmNactivateCallback,
        text_return,
        (XtPointer)this);

    add_button = XtVaCreateManagedWidget(
        "add",
        xmPushButtonGadgetClass, control_area,
        XmNleftAttachment, XmATTACH_WIDGET,
        XmNleftWidget, XtParent(style_list),
        XmNtopAttachment, XmATTACH_WIDGET,
        XmNsensitive, False,
        XmNtopWidget, label,
        NULL);

    delete_button = XtVaCreateManagedWidget(
        "delete",
        xmPushButtonGadgetClass, control_area,
        XmNsensitive, False,
        XmNtopAttachment, XmATTACH_WIDGET,
        XmNtopWidget, label,
        XmNleftAttachment, XmATTACH_WIDGET,
        XmNleftWidget, add_button,
        NULL);

    edit_button = XtVaCreateManagedWidget(
        "edit",
        xmPushButtonGadgetClass, control_area,
        XmNsensitive, False,
        XmNtopAttachment, XmATTACH_WIDGET,
        XmNtopWidget, label,
        XmNleftAttachment, XmATTACH_WIDGET,
        XmNleftWidget, delete_button,
        NULL);

    XtAddCallback(
        add_button,
        XmNactivateCallback,
        button_cb,
        (XtPointer)this);

    XtAddCallback(
        delete_button,
        XmNactivateCallback,
        button_cb,
        (XtPointer)this);

    XtAddCallback(
        edit_button,
        XmNactivateCallback,
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
        NULL);

    cancel_button = XtVaCreateManagedWidget(
        "cancel",
        xmPushButtonGadgetClass, action_area,
        XmNdefaultButtonShadowThickness, 1,
        XmNleftAttachment, XmATTACH_POSITION,
        XmNleftPosition, 3,
        XmNrightAttachment, XmATTACH_POSITION,
        XmNrightPosition, 4,
        NULL);

    XtAddCallback(
        ok_button,
        XmNactivateCallback,
        button_cb,
        (XtPointer)this);

    XtAddCallback(
        cancel_button,
        XmNactivateCallback,
        button_cb,
        (XtPointer)this);

    XtManageChild(style_list);
    XtManageChild(action_area);
    XtManageChild(control_area);
    XtManageChild(pane);
}

mx_style_d::~mx_style_d()
{
    if (para_d != NULL)
        delete para_d;
    if (too_many_d != NULL)
        delete too_many_d;
    if (sure_d != NULL)
        delete sure_d;
}

void mx_style_d::fill_list(mx_paragraph_style* styles[], int n)
{
    XmString str[100];
    int i;

    for (i = 0; i < n; i++) {
        str[i] = XmStringCreate(styles[i]->get_name(), XmFONTLIST_DEFAULT_TAG);
    }

    XtVaSetValues(style_list,
        XmNitemCount, n,
        XmNitems, str,
        NULL);

    for (i = 0; i < n; i++) {
        XmStringFree(str[i]);
    }
}

void mx_style_d::activate_d(mx_paragraph_style* styles[], int* num_styles,
    int max_styles, mx_unit_e u)
{
    units = u;
    mx_style_d::styles = styles;
    mx_style_d::num_styles = num_styles;
    mx_style_d::max_styles = max_styles;

    fill_list(styles, *(mx_style_d::num_styles));
    mx_dialog::activate();
}

int mx_style_d::run(mx_paragraph_style* styles[], int* num_styles,
    int max_styles, mx_unit_e u)
{
    int res = 0;
    centre();
    activate_d(styles, num_styles, max_styles, u);
    res = run_modal();
    deactivate();
    return res;
}

void mx_style_d::handle_button(Widget w)
{
    if (w == add_button) {
        add_style();
    } else {
        if (w == delete_button) {
            delete_style();
        } else {
            edit_style();
        }
    }
    return;
}

void mx_style_d::add_style()
{
    int err = MX_ERROR_OK;
    char* s = XmTextGetString(style_name_text);

    if (*num_styles == max_styles) {
        if (too_many_d == NULL) {
            too_many_d = new mx_inform_d("tooManyStyles", dialog, error_e);
        }
        too_many_d->centre();
        too_many_d->activate();
        too_many_d->run_modal();
        too_many_d->deactivate();
        XtFree(s);
        return;
    }
    styles[*num_styles] = new mx_paragraph_style();

    styles[*num_styles]->set_name(s);
    MX_ERROR_CHECK(err);

    (*num_styles)++;
    fill_list(styles, *num_styles);
    XtVaSetValues(edit_button, XmNsensitive, True, NULL);
    XtVaSetValues(delete_button, XmNsensitive, True, NULL);
    XtVaSetValues(add_button, XmNsensitive, False, NULL);
    edited = TRUE;
    XtFree(s);
    return;

abort:
    global_error_trace->print();
    MX_ERROR_CLEAR(err);
}

void mx_style_d::delete_style()
{
    int res, i;
    char* s;

    if (sure_d == NULL) {
        sure_d = new mx_yes_no_d("deleteStyle", dialog, FALSE, no_e);
    }
    s = XmTextGetString(style_name_text);

    for (i = 0; i < *num_styles; i++) {
        if (strcasecmp(styles[i]->get_name(), s) == 0) {
            sure_d->centre();
            sure_d->activate();
            res = sure_d->run_modal();
            sure_d->deactivate();

            if (res == yes_e) {
                edited = TRUE;
                delete styles[i];
                styles[i] = styles[*num_styles - 1];
                styles[*num_styles - 1] = NULL;
                (*num_styles)--;
                fill_list(styles, *num_styles);
                XtVaSetValues(edit_button, XmNsensitive, False, NULL);
                XtVaSetValues(delete_button, XmNsensitive, False, NULL);
                XtVaSetValues(add_button, XmNsensitive, True, NULL);
            }
        }
    }
    XtFree(s);
}

void mx_style_d::edit_style()
{
    int i;
    mx_paragraph_style* tmp_style[1];
    char* s;

    if (para_d == NULL) {
        para_d = new mx_para_d(dialog, FALSE);
    }
    s = XmTextGetString(style_name_text);

    for (i = 0; i < *num_styles; i++) {
        if (strcasecmp(styles[i]->get_name(), s) == 0) {
            mx_paragraph_style_mod mod;
            tmp_style[0] = styles[i];

            if (para_d->run(tmp_style, 1, units, mod) == yes_e) {
                edited = TRUE;
                *styles[i] += mod;
            }
        }
    }
    XtFree(s);
    return;
}

void mx_style_d::handle_change()
{
    int i;
    char* s = XmTextGetString(style_name_text);
    if (mx_is_blank(s)) {
        XtVaSetValues(add_button, XmNsensitive, False, NULL);
        XtVaSetValues(delete_button, XmNsensitive, False, NULL);
        XtVaSetValues(edit_button, XmNsensitive, False, NULL);
        XtFree(s);
        return;
    }

    for (i = 0; i < *num_styles; i++) {
        if (strcasecmp(styles[i]->get_name(), s) == 0) {
            XtVaSetValues(edit_button, XmNsensitive, True, NULL);

            // is it the default style ?
            if (strcasecmp(s, "default") == 0) {
                XtVaSetValues(delete_button, XmNsensitive, False, NULL);
            } else {
                XtVaSetValues(delete_button, XmNsensitive, True, NULL);
            }
            XtVaSetValues(add_button, XmNsensitive, False, NULL);
            XtFree(s);
            return;
        }
    }
    XtFree(s);

    // not existing style, so can't delete or edit
    XtVaSetValues(delete_button, XmNsensitive, False, NULL);
    XtVaSetValues(edit_button, XmNsensitive, False, NULL);
    XtVaSetValues(add_button, XmNsensitive, True, NULL);
}
