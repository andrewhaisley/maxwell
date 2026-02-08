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
 * MODULE/CLASS : mx_date_d
 *
 * AUTHOR : Andrew Haisley
 *
 * This file
 *
 * DESCRIPTION:
 *
 *  A date time field dialog
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
// #include "mx_yes_no_d.h"
// #include "mx_open_d.h"
#include <mx_file_type.h>

#include "mx_date_d.h"

static void button_cb(
    Widget widget,
    XtPointer client_data,
    XtPointer call_data)
{
    mx_date_d* d = (mx_date_d*)client_data;

    if (widget == d->ok_button) {
        d->figure_options();
        d->modal_result = yes_e;
    } else {
        d->modal_result = cancel_e;
    }
}

mx_date_d::mx_date_d(Widget parent)
    : mx_dialog("date", parent, TRUE, FALSE)
{

    XtVaSetValues(action_area, XmNfractionBase, 5, NULL);

    date_list = XmCreateScrolledList(control_area, const_cast<char*>("dateList"), NULL, 0);

    XtVaSetValues(date_list,
        XmNvisibleItemCount, 4,
        XmNselectionPolicy, XmBROWSE_SELECT,
        NULL);

    XtVaSetValues(XtParent(date_list),
        XmNtopAttachment, XmATTACH_FORM,
        XmNleftAttachment, XmATTACH_FORM,
        XmNbottomAttachment, XmATTACH_FORM,
        XmNleftOffset, 5,
        XmNbottomOffset, 5,
        XmNtopOffset, 5,
        NULL);

    time_list = XmCreateScrolledList(control_area, const_cast<char*>("timeList"), NULL, 0);

    XtVaSetValues(time_list,
        XmNvisibleItemCount, 4,
        XmNselectionPolicy, XmBROWSE_SELECT,
        NULL);

    XtVaSetValues(XtParent(time_list),
        XmNtopAttachment, XmATTACH_FORM,
        XmNleftAttachment, XmATTACH_WIDGET,
        XmNleftWidget, XtParent(date_list),
        XmNbottomAttachment, XmATTACH_FORM,
        XmNrightAttachment, XmATTACH_FORM,
        XmNleftOffset, 5,
        XmNbottomOffset, 5,
        XmNrightOffset, 5,
        XmNtopOffset, 5,
        NULL);

    fill_lists();

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

    XtManageChild(date_list);
    XtManageChild(time_list);
    XtManageChild(action_area);
    XtManageChild(control_area);
    XtManageChild(pane);
}

void mx_date_d::fill_lists()
{
    XmString str[4];

    int i;
    char s[50];

    time_t t;
    struct tm* tp;

    t = time(NULL);
    tp = localtime(&t);

    sprintf(s, "%02d/%02d/%04d", tp->tm_mday, tp->tm_mon + 1, tp->tm_year + 1900);
    str[0] = XmStringCreate(s, XmFONTLIST_DEFAULT_TAG);
    sprintf(s, "%02d/%02d/%04d", tp->tm_mon + 1, tp->tm_mday, tp->tm_year + 1900);
    str[1] = XmStringCreate(s, XmFONTLIST_DEFAULT_TAG);
    strftime(s, 50, "%d %b %Y", tp);
    str[2] = XmStringCreate(s, XmFONTLIST_DEFAULT_TAG);
    str[3] = XmStringCreate(const_cast<char*>("none"), XmFONTLIST_DEFAULT_TAG);

    XtVaSetValues(date_list,
        XmNitemCount, 4,
        XmNitems, str,
        NULL);

    for (i = 0; i < 4; i++) {
        XmStringFree(str[i]);
    }

    sprintf(s, "%02d:%02d:%02d", tp->tm_hour, tp->tm_min, tp->tm_sec);
    str[0] = XmStringCreate(s, XmFONTLIST_DEFAULT_TAG);
    sprintf(s, "%02d:%02d:%02d", tp->tm_hour % 12, tp->tm_min, tp->tm_sec);
    str[1] = XmStringCreate(s, XmFONTLIST_DEFAULT_TAG);
    sprintf(s, "%02d:%02d", tp->tm_hour, tp->tm_min);
    str[2] = XmStringCreate(s, XmFONTLIST_DEFAULT_TAG);
    str[3] = XmStringCreate(const_cast<char*>("none"), XmFONTLIST_DEFAULT_TAG);

    XtVaSetValues(time_list,
        XmNitemCount, 4,
        XmNitems, str,
        NULL);

    for (i = 0; i < 4; i++) {
        XmStringFree(str[i]);
    }
}

void mx_date_d::figure_options()
{
    static const mx_field_word::mx_field_dt_format_t d[4] = {
        mx_field_word::dt_dd_mm_yyyy_e,
        mx_field_word::dt_mm_dd_yyyy_e,
        mx_field_word::dt_d_mon_yyyy_e,
        mx_field_word::dt_none_e
    };

    static const mx_field_word::mx_field_dt_format_t t[4] = {
        mx_field_word::dt_hh24_mm_ss_e,
        mx_field_word::dt_hh_mm_ss_e,
        mx_field_word::dt_hh24_mm_e,
        mx_field_word::dt_none_e
    };

    int i;

    time_format = date_format = mx_field_word::dt_none_e;

    for (i = 0; i < 4; i++) {
        if (XmListPosSelected(time_list, i + 1)) {
            time_format = t[i];
            break;
        }
    }

    for (i = 0; i < 4; i++) {
        if (XmListPosSelected(date_list, i + 1)) {
            date_format = d[i];
            break;
        }
    }
}
