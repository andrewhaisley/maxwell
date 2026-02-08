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
 * MODULE/CLASS : mx_support_d
 *
 * AUTHOR : Andrew Haisley
 *
 * This file
 *
 * DESCRIPTION:
 *
 *  An support info dialog.
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
#include <mx_std.h>
#include <mx_ui_object.h>
#include <mx_file_type.h>

#include "mx_support_d.h"

static void button_cb(
    Widget widget,
    XtPointer client_data,
    XtPointer call_data)
{
    mx_support_d* d = (mx_support_d*)client_data;

    if (widget == d->ok_button) {
        d->modal_result = yes_e;
    }
}

mx_support_d::mx_support_d(Widget parent)
    : mx_dialog("support", parent, TRUE, FALSE)
{
    XtVaSetValues(action_area, XmNfractionBase, 3, NULL);

    info_text = XtVaCreateManagedWidget("infoText",
        xmTextWidgetClass, control_area,
        XmNleftAttachment, XmATTACH_FORM,
        XmNrightAttachment, XmATTACH_FORM,
        XmNtopAttachment, XmATTACH_FORM,
        XmNbottomAttachment, XmATTACH_FORM,
        XmNeditMode, XmMULTI_LINE_EDIT,
        XmNeditable, False,
        XmNcursorPositionVisible, False,
        XmNnavigationType, XmNONE,
        // XmNcolumns, 40,
        NULL);

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

    XtManageChild(action_area);
    XtManageChild(control_area);
    XtManageChild(pane);
}

void mx_support_d::activate_d()
{
    const char* text = 

R"(Maxwell is copyright (c) 1996 to 1998 of:
Andrew Haisley, Dave Miller and Tom Newton, with
significant contributions by Katherine Brown.

This program is free software and is provided under 
the terms of the GNU General Public License, version
2 or later.  There is no WARRANTY for this software.

See the COPYRIGHT file distributed with this
application for full details.)";

    XmTextSetString(info_text, const_cast<char *>(text));

    mx_dialog::activate();
}

void mx_support_d::run()
{
    centre();
    activate_d();
    (void)run_modal();
    deactivate();
}
