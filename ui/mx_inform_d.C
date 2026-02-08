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
 * MODULE/CLASS : mx_inform_d
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
 */

#include "mx_inform_d.h"
#include <Xm/LabelG.h>
#include <Xm/PushBG.h>
#include <Xm/Xm.h>
#include <mx_ui_object.h>

static void confirm_cb(Widget widget, XtPointer client_data, XtPointer call_data)
{
    mx_inform_d* d = (mx_inform_d*)client_data;

    d->modal_result = yes_e;
}

mx_inform_d::mx_inform_d(const char* name, Widget parent, mx_inform_d_t type)
    : mx_dialog(name, parent, TRUE, FALSE)
{
    Pixmap pixmap;
    Pixel fg, bg;
    Widget label, button;

    XtVaSetValues(action_area, XmNfractionBase, 3, NULL);

    // at this point, the basic dialog structure all exists, just
    // have to add the bits and pieces in.
    XtVaGetValues(
        control_area,
        XmNforeground, &fg,
        XmNbackground, &bg,
        NULL);

    switch (type) {
    case inform_e:
        pixmap = XmGetPixmap(
            XtScreen(control_area),
            const_cast<char*>("mx_inform.xpm"),
            fg, bg);
        break;
    case warn_e:
        pixmap = XmGetPixmap(
            XtScreen(control_area),
            const_cast<char*>("mx_warn.xpm"),
            fg, bg);
        break;
    default:
    case error_e:
        pixmap = XmGetPixmap(
            XtScreen(control_area),
            const_cast<char*>("mx_error.xpm"),
            fg, bg);
        break;
    }

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

    // finally the confirm button
    button = XtVaCreateManagedWidget(
        "confirm",
        xmPushButtonGadgetClass, action_area,
        XmNleftAttachment, XmATTACH_POSITION,
        XmNleftPosition, 1,
        XmNtopAttachment, XmATTACH_FORM,
        XmNbottomAttachment, XmATTACH_FORM,
        XmNshowAsDefault, True,
        XmNdefaultButtonShadowThickness, 1,
        NULL);

    XtAddCallback(
        button,
        XmNactivateCallback,
        confirm_cb,
        (XtPointer)this);

    XtManageChild(action_area);
    XtManageChild(control_area);
    XtManageChild(pane);
}
