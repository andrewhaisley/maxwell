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
 * MODULE/CLASS : mx_env_d
 *
 * AUTHOR : Andrew Haisley
 *
 * This file
 *
 * DESCRIPTION:
 *
 *  An envelope dialog.
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
// #include "mx_yes_no_d.h"
// #include "mx_open_d.h"
#include <mx_file_type.h>

#include "mx_env_d.h"
#include <mx_sizes.h>

static void text_return(
    Widget widget,
    XtPointer client_data,
    XtPointer call_data)
{
    mx_env_d* d = (mx_env_d*)client_data;

    d->figure_options();
    d->modal_result = yes_e;
}

static void button_cb(
    Widget widget,
    XtPointer client_data,
    XtPointer call_data)
{
    mx_env_d* d = (mx_env_d*)client_data;

    if (widget == d->ok_button) {
        d->figure_options();
        d->modal_result = yes_e;
    } else {
        d->modal_result = cancel_e;
    }
}

static void list_cb(Widget list, XtPointer client_data, XtPointer call_data)
{
    mx_env_d* d = (mx_env_d*)client_data;

    char* choice;

    XmListCallbackStruct* cbs = (XmListCallbackStruct*)call_data;

    XmStringGetLtoR(cbs->item, XmFONTLIST_DEFAULT_TAG, &choice);
    d->set_size(choice);
    XtFree(choice);

    if (cbs->reason == XmCR_BROWSE_SELECT) {
        // single clicked on a size
        return;
    }

    if (cbs->reason == XmCR_DEFAULT_ACTION) {
        // double clicked on a size
        d->modal_result = yes_e;
    }
}

mx_env_d::mx_env_d(Widget parent)
    : mx_dialog("env", parent, TRUE, FALSE)
{
    Widget frame1, frame2;
    Widget form1, form2;
    Widget label1, label2;
    Widget orientation_rc;

    XtVaSetValues(action_area, XmNfractionBase, 5, NULL);

    frame1 = XtVaCreateManagedWidget(
        "frame1",
        xmFrameWidgetClass,
        control_area,
        XmNtopAttachment, XmATTACH_FORM,
        XmNleftAttachment, XmATTACH_FORM,
        XmNbottomAttachment, XmATTACH_FORM,
        NULL);

    frame2 = XtVaCreateManagedWidget(
        "frame2",
        xmFrameWidgetClass,
        control_area,
        XmNtopAttachment, XmATTACH_FORM,
        XmNbottomAttachment, XmATTACH_FORM,
        XmNleftAttachment, XmATTACH_WIDGET,
        XmNleftWidget, frame1,
        XmNrightAttachment, XmATTACH_FORM,
        NULL);

    form1 = XtVaCreateWidget(
        "form1",
        xmFormWidgetClass,
        frame1,
        NULL);

    form2 = XtVaCreateWidget(
        "form2",
        xmFormWidgetClass,
        frame2,
        NULL);

    (void)XtVaCreateManagedWidget(
        "frame1Label",
        xmLabelGadgetClass,
        frame1,
        XmNchildType, XmFRAME_TITLE_CHILD,
        NULL);

    (void)XtVaCreateManagedWidget(
        "frame2Label",
        xmLabelGadgetClass,
        frame2,
        XmNchildType, XmFRAME_TITLE_CHILD,
        NULL);

    address_text = XtVaCreateManagedWidget("addressText",
        xmTextWidgetClass, form2,
        XmNleftAttachment, XmATTACH_FORM,
        XmNtopAttachment, XmATTACH_FORM,
        XmNbottomAttachment, XmATTACH_FORM,
        XmNrightAttachment, XmATTACH_FORM,
        XmNeditMode, XmMULTI_LINE_EDIT,
        XmNcolumns, 40,
        NULL);

    list = XmCreateScrolledList(form1, const_cast<char*>("sizeList"), NULL, 0);

    XtVaSetValues(list,
        XmNvisibleItemCount, MX_NUM_ENVELOPE_SIZES,
        XmNselectionPolicy, XmBROWSE_SELECT,
        NULL);

    XtVaSetValues(XtParent(list),
        XmNtopAttachment, XmATTACH_FORM,
        XmNleftAttachment, XmATTACH_FORM,
        XmNbottomAttachment, XmATTACH_FORM,
        XmNleftOffset, 5,
        XmNbottomOffset, 5,
        XmNtopOffset, 5,
        NULL);

    XtAddCallback(list, XmNdefaultActionCallback, list_cb, this);
    XtAddCallback(list, XmNbrowseSelectionCallback, list_cb, this);

    fill_size_list();

    label1 = XtVaCreateManagedWidget(
        "widthLabel",
        xmLabelGadgetClass,
        form1,
        XmNtopAttachment, XmATTACH_FORM,
        XmNleftAttachment, XmATTACH_WIDGET,
        XmNleftWidget, list,
        NULL);

    label2 = XtVaCreateManagedWidget(
        "heightLabel",
        xmLabelGadgetClass,
        form1,
        XmNtopAttachment, XmATTACH_WIDGET,
        XmNtopWidget, label1,
        XmNleftAttachment, XmATTACH_WIDGET,
        XmNleftWidget, list,
        NULL);

    width_text = XtVaCreateManagedWidget("widthText",
        xmTextWidgetClass, form1,
        XmNleftAttachment, XmATTACH_WIDGET,
        XmNleftWidget, label2,
        XmNtopAttachment, XmATTACH_FORM,
        NULL);

    XtAddCallback(
        width_text,
        XmNactivateCallback,
        text_return,
        (XtPointer)this);

    height_text = XtVaCreateManagedWidget("heightText",
        xmTextWidgetClass, form1,
        XmNleftAttachment, XmATTACH_WIDGET,
        XmNleftWidget, label2,
        XmNtopAttachment, XmATTACH_WIDGET,
        XmNtopWidget, width_text,
        NULL);

    XtAddCallback(
        height_text,
        XmNactivateCallback,
        text_return,
        (XtPointer)this);

    orientation_rc = XtVaCreateWidget(
        "orientation",
        xmRowColumnWidgetClass,
        form1,
        XmNorientation, XmHORIZONTAL,
        XmNpacking, XmPACK_TIGHT,
        XmNradioBehavior, True,
        XmNleftAttachment, XmATTACH_WIDGET,
        XmNleftWidget, list,
        XmNtopAttachment, XmATTACH_WIDGET,
        XmNtopWidget, height_text,
        NULL);

    portrait_toggle = XtVaCreateManagedWidget(
        "portraitToggle",
        xmToggleButtonGadgetClass, orientation_rc,
        NULL);

    landscape_toggle = XtVaCreateManagedWidget(
        "landscapeToggle",
        xmToggleButtonGadgetClass, orientation_rc,
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

    XtManageChild(list);
    XtManageChild(orientation_rc);
    XtManageChild(form1);
    XtManageChild(form2);
    XtManageChild(action_area);
    XtManageChild(control_area);
    XtManageChild(pane);
}

void mx_env_d::fill_size_list()
{
    XmString str[MX_NUM_ENVELOPE_SIZES];
    int i;
    float f1, f2;

    char s[50];

    for (i = 0; i < MX_NUM_ENVELOPE_SIZES; i++) {
        f1 = mx_mm_to_unit(mx_envelope_widths[i], units);
        f2 = mx_mm_to_unit(mx_envelope_heights[i], units);
        sprintf(s, "%s - %.2f x %.2f %s",
            mx_envelope_size_names[i],
            f1, f2, mx_unit_name(units));

        str[i] = XmStringCreate(s, XmFONTLIST_DEFAULT_TAG);
    }

    XtVaSetValues(list,
        XmNitemCount, MX_NUM_ENVELOPE_SIZES,
        XmNitems, str,
        NULL);

    for (i = 0; i < MX_NUM_ENVELOPE_SIZES; i++) {
        XmStringFree(str[i]);
    }
}

void mx_env_d::activate_d(
    float width,
    float height,
    bool is_landscape,
    char* address,
    mx_unit_e u)
{
    mx_env_d::width = width;
    mx_env_d::height = height;
    mx_env_d::is_landscape = is_landscape;

    strncpy(mx_env_d::address, address, MX_ENV_MAX_TEXT - 1);
    mx_env_d::address[MX_ENV_MAX_TEXT - 1] = 0;

    units = u;
    set_options();
    mx_dialog::activate();
}

void mx_env_d::figure_options()
{
    char* s;
    float f;

    s = XmTextGetString(width_text);
    sscanf(s, "%f", &f);
    XtFree(s);
    width = mx_unit_to_mm(f, units);

    s = XmTextGetString(height_text);
    sscanf(s, "%f", &f);
    XtFree(s);
    height = mx_unit_to_mm(f, units);

    is_landscape = XmToggleButtonGetState(landscape_toggle);

    s = XmTextGetString(address_text);
    strncpy(address, s, MX_ENV_MAX_TEXT - 1);
    address[MX_ENV_MAX_TEXT - 1] = 0;
    XtFree(s);
}

void mx_env_d::set_options()
{
    char s[50];
    float f;
    int i;

    f = mx_mm_to_unit(width, units);
    sprintf(s, "%.2f %s", f, mx_unit_name(units));
    XmTextSetString(width_text, s);

    f = mx_mm_to_unit(height, units);
    sprintf(s, "%.2f %s", f, mx_unit_name(units));
    XmTextSetString(height_text, s);

    XmTextSetString(address_text, address);

    if (is_landscape) {
        XmToggleButtonSetState(landscape_toggle, True, False);
    } else {
        XmToggleButtonSetState(portrait_toggle, True, False);
    }

    for (i = 0; i < MX_NUM_ENVELOPE_SIZES; i++) {
        if (MX_FLOAT_EQ(mx_envelope_widths[i], width) && MX_FLOAT_EQ(mx_envelope_heights[i], height)) {
            XmListSelectPos(list, i + 1, False);
            return;
        }
    }
}

void mx_env_d::set_size(char* s)
{
    int i;
    char temp[50];
    float f;

    for (i = 0; i < MX_NUM_ENVELOPE_SIZES; i++) {
        if (strncmp(
                mx_envelope_size_names[i],
                s,
                strlen(mx_envelope_size_names[i]))
            == 0) {
            f = mx_mm_to_unit(mx_envelope_widths[i], units);
            sprintf(temp, "%.2f %s", f, mx_unit_name(units));
            XmTextSetString(width_text, temp);

            f = mx_mm_to_unit(mx_envelope_heights[i], units);
            sprintf(temp, "%.2f %s", f, mx_unit_name(units));
            XmTextSetString(height_text, temp);

            return;
        }
    }
}
