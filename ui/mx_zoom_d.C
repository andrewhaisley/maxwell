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
 * MODULE/CLASS : mx_zoom_d
 *
 * AUTHOR : Andrew Haisley
 *
 * This file
 *
 * DESCRIPTION:
 *
 *  A zoom dialog.
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

#include "mx_zoom_d.h"

static void button_cb(
    Widget widget,
    XtPointer client_data,
    XtPointer call_data)
{
    mx_zoom_d* d = (mx_zoom_d*)client_data;

    if (widget == d->ok_button) {
        if (d->figure_options()) {
            d->modal_result = yes_e;
        }
    } else {
        d->modal_result = no_e;
    }
}

static void zoom_return(
    Widget widget,
    XtPointer client_data,
    XtPointer call_data)
{
    mx_zoom_d* d = (mx_zoom_d*)client_data;

    if (d->figure_options()) {
        d->modal_result = yes_e;
    }
}

static void text_changed(
    Widget widget,
    XtPointer client_data,
    XtPointer call_data)
{
    mx_zoom_d* d = (mx_zoom_d*)client_data;
    char* s = XmTextGetString(d->zoom_text);

    XtVaSetValues(d->ok_button,
        XmNsensitive, mx_is_blank(s) ? False : True,
        NULL);
    XtFree(s);
}

mx_zoom_d::~mx_zoom_d()
{
    if (bad_zoom_d != NULL) {
        delete bad_zoom_d;
    }
}

mx_zoom_d::mx_zoom_d(Widget parent)
    : mx_dialog("zoom", parent, TRUE, FALSE)
{
    Widget combo, label;
    Arg args[20];
    int n;

    XtVaSetValues(action_area, XmNfractionBase, 5, NULL);
    zoom_percent = 100;
    bad_zoom_d = NULL;

    label = XtVaCreateManagedWidget(
        "zoomLabel",
        xmLabelGadgetClass,
        control_area,
        XmNtopAttachment, XmATTACH_FORM,
        XmNleftAttachment, XmATTACH_FORM,
        NULL);

    n = 0;
    combo = XmCreateDropDownComboBox(control_area, const_cast<char*>("combo"), args, n);
    XtVaSetValues(combo,
        XmNleftAttachment, XmATTACH_WIDGET,
        XmNleftWidget, label,
        XmNtopAttachment, XmATTACH_FORM,
        NULL);

    zoom_text = XtNameToWidget(combo, "*Text");
    zoom_list = XtNameToWidget(combo, "*List");

    XtAddCallback(zoom_text, XmNactivateCallback, zoom_return, this);
    XtAddCallback(zoom_text, XmNvalueChangedCallback, text_changed, this);

    XtVaSetValues(zoom_text,
        XmNeditable, True,
        XmNcursorPosition, True,
        NULL);

    XtVaSetValues(zoom_list,
        XmNvisibleItemCount, 7,
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
        XmNshowAsDefault, True,
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

    add_levels();

    XtManageChild(combo);
    XtManageChild(action_area);
    XtManageChild(control_area);
    XtManageChild(pane);
}

void mx_zoom_d::add_zoom(int z)
{
    char s[20];
    ;
    XmString item;
    int i;

    sprintf(s, "%d%%", z);
    item = XmStringCreateLocalized(s);

    if (!XmListItemExists(zoom_list, item)) {
        XmListAddItem(zoom_list, item, 1);

        XtVaGetValues(zoom_list,
            XmNvisibleItemCount, &i,
            NULL);
        i++;
        XtVaSetValues(zoom_list,
            XmNvisibleItemCount, i < 25 ? i : 25,
            NULL);
    }

    XmStringFree(item);
}

void mx_zoom_d::add_levels()
{
    const char* s[7] = { "25%", "50%", "100%", "133%", "150%", "200%", "400%" };
    XmString item;
    int i;

    for (i = 0; i < 7; i++) {
        item = XmStringCreateLocalized(const_cast<char*>(s[i]));
        XmListAddItem(zoom_list, item, 0);
        XmStringFree(item);
    }
}

void mx_zoom_d::activate_d(int zoom_percent)
{
    char s[20];

    sprintf(s, "%d %%", zoom_percent);
    XmTextSetString(zoom_text, s);
}

bool mx_zoom_d::figure_options()
{
    char* s = XmTextGetString(zoom_text);
    zoom_percent = atoi(s);
    XtFree(s);
    if (zoom_percent > 10) {
        add_zoom(zoom_percent);
        return TRUE;
    } else {
        if (bad_zoom_d == NULL) {
            bad_zoom_d = new mx_inform_d("badZoomNumber", dialog, error_e);
        }
        bad_zoom_d->centre();
        bad_zoom_d->run_modal();
        bad_zoom_d->deactivate();
        return FALSE;
    }
}

int mx_zoom_d::run(int z)
{
    int res;

    centre();
    activate_d(z);
    res = run_modal();
    deactivate();

    return res;
}
