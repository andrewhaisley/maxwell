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
 * MODULE/CLASS : mx_about_d
 *
 * AUTHOR : Andrew Haisley
 *
 * This file
 *
 * DESCRIPTION:
 *
 *  An about box dialog.
 *
 *
 *
 */

#include "mx_about_d.h"
#include <Xm/Xm.h>

#include <Xm/Label.h>
#include <Xm/LabelG.h>
#include <Xm/List.h>
#include <Xm/Protocols.h>
#include <Xm/PushBG.h>
#include <Xm/RowColumn.h>
#include <Xm/Separator.h>
#include <Xm/Text.h>
#include <Xm/Xm.h>

static void button_cb(
    Widget widget,
    XtPointer client_data,
    XtPointer call_data)
{
    mx_about_d* d = (mx_about_d*)client_data;

    if (widget == d->confirm_button) {
        d->modal_result = yes_e;
    }
}

mx_about_d::mx_about_d(Widget parent)
    : mx_dialog("about", parent, TRUE, FALSE)
{
    Widget label1, label2, label3, label5, label6, label7;
    XmString str;

    const char* sn = "none";
    const char* owner = "bob";
    const char* org = "An Org";

    XtVaSetValues(action_area, XmNfractionBase, 3, NULL);

    label1 = XtVaCreateManagedWidget(
        "text1",
        xmLabelGadgetClass,
        control_area,
        XmNtopAttachment, XmATTACH_FORM,
        XmNleftAttachment, XmATTACH_FORM,
        NULL);

    label2 = XtVaCreateManagedWidget(
        "text2",
        xmLabelGadgetClass,
        control_area,
        XmNtopAttachment, XmATTACH_WIDGET,
        XmNtopWidget, label1,
        XmNleftAttachment, XmATTACH_FORM,
        NULL);

    label3 = XtVaCreateManagedWidget(
        "text3",
        xmLabelGadgetClass,
        control_area,
        XmNtopAttachment, XmATTACH_WIDGET,
        XmNtopWidget, label2,
        XmNleftAttachment, XmATTACH_FORM,
        NULL);

    (void)XtVaCreateManagedWidget(
        "text4",
        xmLabelGadgetClass,
        control_area,
        XmNtopAttachment, XmATTACH_WIDGET,
        XmNtopWidget, label3,
        XmNleftAttachment, XmATTACH_FORM,
        NULL);

    label5 = XtVaCreateManagedWidget(
        "text5",
        xmLabelGadgetClass,
        control_area,
        XmNtopAttachment, XmATTACH_WIDGET,
        XmNtopWidget, label1,
        XmNleftAttachment, XmATTACH_WIDGET,
        XmNleftWidget, label2,
        NULL);

    label6 = XtVaCreateManagedWidget(
        "text6",
        xmLabelGadgetClass,
        control_area,
        XmNtopAttachment, XmATTACH_WIDGET,
        XmNtopWidget, label5,
        XmNleftAttachment, XmATTACH_WIDGET,
        XmNleftWidget, label2,
        NULL);

    label7 = XtVaCreateManagedWidget(
        "text6",
        xmLabelGadgetClass,
        control_area,
        XmNtopAttachment, XmATTACH_WIDGET,
        XmNtopWidget, label6,
        XmNleftAttachment, XmATTACH_WIDGET,
        XmNleftWidget, label2,
        NULL);

    str = XmStringCreate(const_cast<char*>(owner), XmFONTLIST_DEFAULT_TAG);
    XtVaSetValues(label5, XmNlabelString, str, NULL);
    XmStringFree(str);

    str = XmStringCreate(const_cast<char*>(org), XmFONTLIST_DEFAULT_TAG);
    XtVaSetValues(label6, XmNlabelString, str, NULL);
    XmStringFree(str);

    str = XmStringCreate(const_cast<char*>(sn), XmFONTLIST_DEFAULT_TAG);
    XtVaSetValues(label7, XmNlabelString, str, NULL);
    XmStringFree(str);

    // now, the action buttons
    confirm_button = XtVaCreateManagedWidget(
        "confirm",
        xmPushButtonGadgetClass, action_area,
        XmNshowAsDefault, True,
        XmNdefaultButtonShadowThickness, 1,
        XmNleftAttachment, XmATTACH_POSITION,
        XmNleftPosition, 1,
        XmNrightAttachment, XmATTACH_POSITION,
        XmNrightPosition, 2,
        NULL);

    XtAddCallback(
        confirm_button,
        XmNactivateCallback,
        button_cb,
        (XtPointer)this);

    XtManageChild(action_area);
    XtManageChild(control_area);
    XtManageChild(pane);
}
