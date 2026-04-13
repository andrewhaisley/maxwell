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
 * MODULE/CLASS : mx_opt_d
 *
 * AUTHOR : Andrew Haisley
 *
 * This file
 *
 * DESCRIPTION:
 *
 *  Options dialog
 *
 *
 *
 *
 */

#include <cups/cups.h>

#include <ctype.h>
#include <dirent.h>
#include <errno.h>
#include <pwd.h>
#include <signal.h>
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
#include <mx_file_type.h>
#include <mx_sizes.h>

#include "mx_opt_d.h"

static void button_cb(
    Widget widget,
    XtPointer client_data,
    XtPointer call_data)
{
    mx_opt_d* d = (mx_opt_d*)client_data;

    if (widget == d->ok_button) {
        d->modal_result = yes_e;
        d->figure_options();
    } else {
        d->modal_result = cancel_e;
    }
}

mx_opt_d::mx_opt_d(Widget parent)
    : mx_dialog("opt", parent, TRUE, FALSE)
{
    Widget label1, label2, label3, label4, label5, label6;
    Arg args[15];
    int n;

    XtVaSetValues(action_area, XmNfractionBase, 3, NULL);

    label1 = XtVaCreateManagedWidget(
        "pageLabel",
        xmLabelGadgetClass,
        control_area,
        XmNtopAttachment, XmATTACH_FORM,
        XmNleftAttachment, XmATTACH_FORM,
        NULL);

    label2 = XtVaCreateManagedWidget(
        "envelopeLabel",
        xmLabelGadgetClass,
        control_area,
        XmNtopAttachment, XmATTACH_WIDGET,
        XmNtopWidget, label1,
        XmNleftAttachment, XmATTACH_FORM,
        NULL);

    label3 = XtVaCreateManagedWidget(
        "unitsLabel",
        xmLabelGadgetClass,
        control_area,
        XmNtopAttachment, XmATTACH_WIDGET,
        XmNtopWidget, label2,
        XmNleftAttachment, XmATTACH_FORM,
        NULL);

    label4 = XtVaCreateManagedWidget(
        "printerLabel",
        xmLabelGadgetClass,
        control_area,
        XmNtopAttachment, XmATTACH_WIDGET,
        XmNtopWidget, label3,
        XmNleftAttachment, XmATTACH_FORM,
        NULL);

    label5 = XtVaCreateManagedWidget(
        "xresLabel",
        xmLabelGadgetClass,
        control_area,
        XmNtopAttachment, XmATTACH_WIDGET,
        XmNtopWidget, label4,
        XmNleftAttachment, XmATTACH_FORM,
        NULL);

    label6 = XtVaCreateManagedWidget(
        "yresLabel",
        xmLabelGadgetClass,
        control_area,
        XmNtopAttachment, XmATTACH_WIDGET,
        XmNtopWidget, label5,
        XmNleftAttachment, XmATTACH_FORM,
        NULL);

    (void)XtVaCreateManagedWidget(
        "languageLabel",
        xmLabelGadgetClass,
        control_area,
        XmNtopAttachment, XmATTACH_WIDGET,
        XmNtopWidget, label6,
        XmNleftAttachment, XmATTACH_FORM,
        NULL);

    // page size
    page_sub_menu = XmCreatePulldownMenu(control_area, const_cast<char*>("pagePulldown"), NULL, 0);

    n = 0;
    XtSetArg(args[n], XmNsubMenuId, page_sub_menu);
    n++;
    XtSetArg(args[n], XmNtopAttachment, XmATTACH_FORM);
    n++;
    XtSetArg(args[n], XmNleftAttachment, XmATTACH_WIDGET);
    n++;
    XtSetArg(args[n], XmNleftWidget, label2);
    n++;

    page_menu = XmCreateOptionMenu(control_area, const_cast<char*>("pageMenu"), args, n);

    // envelope size
    envelope_sub_menu = XmCreatePulldownMenu(control_area, const_cast<char*>("envelopePulldown"), NULL, 0);

    n = 0;
    XtSetArg(args[n], XmNsubMenuId, envelope_sub_menu);
    n++;
    XtSetArg(args[n], XmNtopAttachment, XmATTACH_WIDGET);
    n++;
    XtSetArg(args[n], XmNtopWidget, page_menu);
    n++;
    XtSetArg(args[n], XmNleftAttachment, XmATTACH_WIDGET);
    n++;
    XtSetArg(args[n], XmNleftWidget, label2);
    n++;

    envelope_menu = XmCreateOptionMenu(control_area, const_cast<char*>("envelopeMenu"), args, n);

    // units
    units_sub_menu = XmCreatePulldownMenu(control_area, const_cast<char*>("unitsPulldown"), NULL, 0);

    n = 0;
    XtSetArg(args[n], XmNsubMenuId, units_sub_menu);
    n++;
    XtSetArg(args[n], XmNtopAttachment, XmATTACH_WIDGET);
    n++;
    XtSetArg(args[n], XmNtopWidget, envelope_menu);
    n++;
    XtSetArg(args[n], XmNleftAttachment, XmATTACH_WIDGET);
    n++;
    XtSetArg(args[n], XmNleftWidget, label2);
    n++;

    units_menu = XmCreateOptionMenu(control_area, const_cast<char*>("unitsMenu"), args, n);

    // printer
    printer_sub_menu = XmCreatePulldownMenu(control_area, const_cast<char*>("printerPulldown"), NULL, 0);

    n = 0;
    XtSetArg(args[n], XmNsubMenuId, printer_sub_menu);
    n++;
    XtSetArg(args[n], XmNtopAttachment, XmATTACH_WIDGET);
    n++;
    XtSetArg(args[n], XmNtopWidget, units_menu);
    n++;
    XtSetArg(args[n], XmNleftAttachment, XmATTACH_WIDGET);
    n++;
    XtSetArg(args[n], XmNleftWidget, label2);
    n++;

    printer_menu = XmCreateOptionMenu(control_area, const_cast<char*>("printerMenu"), args, n);

    x_res_text = XtVaCreateManagedWidget("xresText",
        xmTextWidgetClass, control_area,
        XmNleftAttachment, XmATTACH_WIDGET,
        XmNleftWidget, label2,
        XmNtopAttachment, XmATTACH_WIDGET,
        XmNtopWidget, printer_menu,
        NULL);

    y_res_text = XtVaCreateManagedWidget("yresText",
        xmTextWidgetClass, control_area,
        XmNleftAttachment, XmATTACH_WIDGET,
        XmNleftWidget, label2,
        XmNtopAttachment, XmATTACH_WIDGET,
        XmNtopWidget, x_res_text,
        NULL);

    // language
    language_sub_menu = XmCreatePulldownMenu(control_area, const_cast<char*>("languagePulldown"), NULL, 0);

    n = 0;
    XtSetArg(args[n], XmNsubMenuId, language_sub_menu);
    n++;
    XtSetArg(args[n], XmNtopAttachment, XmATTACH_WIDGET);
    n++;
    XtSetArg(args[n], XmNtopWidget, y_res_text);
    n++;
    XtSetArg(args[n], XmNleftAttachment, XmATTACH_WIDGET);
    n++;
    XtSetArg(args[n], XmNleftWidget, label2);
    n++;

    language_menu = XmCreateOptionMenu(control_area, const_cast<char*>("languageMenu"), args, n);

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

    create_menus();

    XtManageChild(page_menu);
    XtManageChild(envelope_menu);
    XtManageChild(units_menu);
    XtManageChild(printer_menu);
    XtManageChild(language_menu);
    XtManageChild(action_area);
    XtManageChild(control_area);
    XtManageChild(pane);
}

void mx_opt_d::activate_d(mx_config* conf)
{
    int err, x_res, y_res;
    const char* s;

    config = conf;

    x_res = config->get_default_int(err, "printer_x_res", 300);
    MX_ERROR_CHECK(err);

    y_res = config->get_default_int(err, "printer_y_res", 300);
    MX_ERROR_CHECK(err);

    s = config->get_default_string(err, "printer", "lp");
    MX_ERROR_CHECK(err);
    set_printer_menu((char*)s, x_res, y_res);

    s = config->get_default_string(err, "language", "english");
    MX_ERROR_CHECK(err);
    set_language_menu((char*)s);

    s = config->get_default_string(err, "page", "A4");
    MX_ERROR_CHECK(err);
    set_page_menu((char*)s);

    s = config->get_default_string(err, "envelope", "DL");
    MX_ERROR_CHECK(err);
    set_envelope_menu((char*)s);

    s = config->get_default_string(err, "units", "millimetres");
    MX_ERROR_CHECK(err);
    set_unit_menu((char*)s);

    mx_dialog::activate();

    return;
abort:
    global_error_trace->print();
    MX_ERROR_CLEAR(err);
}

void mx_opt_d::create_menus()
{
    int i, n;

    cups_dest_t *printers;

    for (i = 0; i < MX_NUM_LANGUAGES; i++) {
        language_buttons[i] = XtVaCreateManagedWidget(
            mx_language_names[i],
            xmPushButtonGadgetClass,
            language_sub_menu,
            NULL);
    }

    for (i = 0; i < MX_NUM_PAPER_SIZES; i++) {
        paper_buttons[i] = XtVaCreateManagedWidget(
            mx_paper_size_names[i],
            xmPushButtonGadgetClass,
            page_sub_menu,
            NULL);
    }

    for (i = 0; i < MX_NUM_ENVELOPE_SIZES; i++) {
        envelope_buttons[i] = XtVaCreateManagedWidget(
            mx_envelope_size_names[i],
            xmPushButtonGadgetClass,
            envelope_sub_menu,
            NULL);
    }

    for (i = (int)mx_mm; i <= (int)mx_points; i++) {
        unit_buttons[i] = XtVaCreateManagedWidget(
            mx_full_unit_name((mx_unit_e)i),
            xmPushButtonGadgetClass,
            units_sub_menu,
            NULL);
    }

    n = cupsGetDests(&printers);

    if (n == 0) {
        printer_buttons[0] = XtVaCreateManagedWidget(
            "lp",
            xmPushButtonGadgetClass,
            printer_sub_menu,
            NULL);
        num_printer_buttons = 1;
    } else {
        for (i = 0; i < n; i++) {
            printer_buttons[i] = XtVaCreateManagedWidget(
                printers[i].name,
                xmPushButtonGadgetClass,
                printer_sub_menu,
                NULL);
        }
        num_printer_buttons = n;
    }
}

void mx_opt_d::set_unit_menu(char* current)
{
    int i;

    for (i = 0; i < MX_NUM_UNIT_TYPES; i++) {
        if (strcmp(current, XtName(unit_buttons[i])) == 0) {
            XtVaSetValues(units_menu, XmNmenuHistory, unit_buttons[i], NULL);
            return;
        }
    }
}

void mx_opt_d::set_language_menu(char* current)
{
    int i;

    for (i = 0; i < MX_NUM_LANGUAGES; i++) {
        if (strcmp(current, XtName(language_buttons[i])) == 0) {
            XtVaSetValues(language_menu, XmNmenuHistory, language_buttons[i], NULL);
            return;
        }
    }
}

void mx_opt_d::set_page_menu(char* current)
{
    int i;

    for (i = 0; i < MX_NUM_PAPER_SIZES; i++) {
        if (strcmp(current, XtName(paper_buttons[i])) == 0) {
            XtVaSetValues(page_menu, XmNmenuHistory, paper_buttons[i], NULL);
            return;
        }
    }
}

void mx_opt_d::set_envelope_menu(char* current)
{
    int i;

    for (i = 0; i < MX_NUM_ENVELOPE_SIZES; i++) {
        if (strcmp(current, XtName(envelope_buttons[i])) == 0) {
            XtVaSetValues(envelope_menu, XmNmenuHistory, envelope_buttons[i], NULL);
            return;
        }
    }
}

void mx_opt_d::set_printer_menu(char* current, int x_res, int y_res)
{
    char s[20];
    int i;

    for (i = 0; i < num_printer_buttons; i++) {
        if (strcmp(current, XtName(printer_buttons[i])) == 0) {
            XtVaSetValues(printer_menu, XmNmenuHistory, printer_buttons[i], NULL);
            break;
        }
    }
    sprintf(s, "%d dpi", x_res);
    XmTextSetString(x_res_text, s);

    sprintf(s, "%d dpi", y_res);
    XmTextSetString(y_res_text, s);
}

void mx_opt_d::figure_options()
{
    char* s;
    Widget w;
    int err = MX_ERROR_OK;

    XtVaGetValues(language_sub_menu, XmNmenuHistory, &w, NULL);
    s = XtName(w);

    config->set_string(err, "language", s);
    MX_ERROR_CHECK(err);

    XtVaGetValues(units_sub_menu, XmNmenuHistory, &w, NULL);
    s = XtName(w);

    config->set_string(err, "units", s);
    MX_ERROR_CHECK(err);

    XtVaGetValues(printer_sub_menu, XmNmenuHistory, &w, NULL);
    s = XtName(w);

    config->set_string(err, "printer", s);
    MX_ERROR_CHECK(err);

    XtVaGetValues(page_sub_menu, XmNmenuHistory, &w, NULL);
    s = XtName(w);

    config->set_string(err, "page", s);
    MX_ERROR_CHECK(err);

    XtVaGetValues(envelope_sub_menu, XmNmenuHistory, &w, NULL);
    s = XtName(w);

    config->set_string(err, "envelope", s);
    MX_ERROR_CHECK(err);

    s = XmTextGetString(x_res_text);
    if (atoi(s) > 50) {
        config->set_int(err, "printer_x_res", atoi(s));
        MX_ERROR_CHECK(err);
    }
    XtFree(s);

    s = XmTextGetString(y_res_text);
    if (atoi(s) > 50) {
        config->set_int(err, "printer_y_res", atoi(s));
        MX_ERROR_CHECK(err);
    }
    XtFree(s);

    return;

abort:
    global_error_trace->print();
    MX_ERROR_CLEAR(err);
}

int mx_opt_d::run(mx_config* conf)
{
    int res;

    centre();
    activate_d(conf);
    res = run_modal();
    deactivate();

    return res;
}
