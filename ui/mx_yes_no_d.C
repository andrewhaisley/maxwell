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
 * MODULE/CLASS : mx_yes_no_d
 *
 * AUTHOR : Andrew Haisley
 *
 * This file
 *
 * DESCRIPTION:
 *
 *  An information dialog.
 *
 *
 *
 *
 */

#include "mx_dialog.h"
#include "mx_yes_no_d.h"
#include <Xm/LabelG.h>
#include <Xm/PushBG.h>
#include <Xm/Xm.h>
#include <mx.h>
#include <mx_ui_object.h>

static void button_cb(
    Widget widget,
    XtPointer client_data,
    XtPointer call_data)
{
    mx_yes_no_d* d = (mx_yes_no_d*)client_data;

    if (widget == d->yes_button) {
        d->modal_result = yes_e;
    } else {
        if (widget == d->no_button) {
            d->modal_result = no_e;
        } else {
            d->modal_result = cancel_e;
        }
    }
}

mx_yes_no_d::mx_yes_no_d(
    const char* name,
    Widget parent,
    bool have_cancel_button,
    mx_dialog_result_t default_button)
    : mx_dialog(name, parent, TRUE, FALSE)
{
    Pixmap pixmap;
    Pixel fg, bg;
    Widget label;

    // at this point, the basic dialog structure all exists, just
    // have to add the bits and pieces in.
    if (have_cancel_button) {
        XtVaSetValues(action_area, XmNfractionBase, 7, NULL);
    } else {
        XtVaSetValues(action_area, XmNfractionBase, 5, NULL);
    }

    XtVaGetValues(
        control_area,
        XmNforeground, &fg,
        XmNbackground, &bg,
        NULL);

    pixmap = XmGetPixmap(
        XtScreen(control_area),
        const_cast<char*>("mx_question.xpm"),
        fg, bg);

    label = XtVaCreateManagedWidget(
        "icon",
        xmLabelGadgetClass,
        control_area,
        XmNlabelType, XmPIXMAP,
        XmNlabelPixmap, pixmap,
        XmNleftAttachment, XmATTACH_FORM,
        XmNtopAttachment, XmATTACH_FORM,
        XmNbottomAttachment, XmATTACH_FORM,
        NULL);

    text_label = XtVaCreateManagedWidget(
        "text",
        xmLabelGadgetClass,
        control_area,
        XmNleftAttachment, XmATTACH_WIDGET,
        XmNleftWidget, label,
        XmNtopAttachment, XmATTACH_FORM,
        XmNbottomAttachment, XmATTACH_FORM,
        NULL);

    // now, the buttons
    yes_button = XtVaCreateManagedWidget(
        "yes",
        xmPushButtonGadgetClass, action_area,
        XmNleftAttachment, XmATTACH_POSITION,
        XmNleftPosition, 1,
        XmNrightAttachment, XmATTACH_POSITION,
        XmNrightPosition, 2,
        XmNtopAttachment, XmATTACH_FORM,
        XmNbottomAttachment, XmATTACH_FORM,
        XmNshowAsDefault, default_button == yes_e,
        XmNdefaultButtonShadowThickness, 1,
        NULL);

    XtAddCallback(
        yes_button,
        XmNactivateCallback,
        button_cb,
        (XtPointer)this);

    no_button = XtVaCreateManagedWidget(
        "no",
        xmPushButtonGadgetClass, action_area,
        XmNleftAttachment, XmATTACH_POSITION,
        XmNleftPosition, 3,
        XmNrightAttachment, XmATTACH_POSITION,
        XmNrightPosition, 4,
        XmNtopAttachment, XmATTACH_FORM,
        XmNbottomAttachment, XmATTACH_FORM,
        XmNshowAsDefault, default_button == no_e,
        XmNdefaultButtonShadowThickness, 1,
        NULL);

    XtAddCallback(
        no_button,
        XmNactivateCallback,
        button_cb,
        (XtPointer)this);

    if (have_cancel_button) {
        cancel_button = XtVaCreateManagedWidget(
            "cancel",
            xmPushButtonGadgetClass, action_area,
            XmNleftAttachment, XmATTACH_POSITION,
            XmNleftPosition, 5,
            XmNrightAttachment, XmATTACH_POSITION,
            XmNrightPosition, 6,
            XmNtopAttachment, XmATTACH_FORM,
            XmNbottomAttachment, XmATTACH_FORM,
            XmNshowAsDefault, default_button == cancel_e,
            XmNdefaultButtonShadowThickness, 1,
            NULL);

        XtAddCallback(
            cancel_button,
            XmNactivateCallback,
            button_cb,
            (XtPointer)this);
    }

    XtManageChild(action_area);
    XtManageChild(control_area);
    XtManageChild(pane);
}
