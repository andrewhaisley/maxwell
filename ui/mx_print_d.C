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
 * MODULE/CLASS : mx_print_d
 *
 * AUTHOR : Andrew Haisley
 *
 * This file
 *
 * DESCRIPTION:
 *
 *  A print dialog.
 *
 *
 *
 *
 */

#include <ctype.h>
#include <dirent.h>
#include <errno.h>
#include <pwd.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>
#include <cups/cups.h>

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

#include "mx_print_d.h"

static void range_cb(
    Widget widget,
    XtPointer client_data,
    XtPointer call_data)
{
    mx_print_d* d = (mx_print_d*)client_data;

    if (XmToggleButtonGetState(d->all_button) || XmToggleButtonGetState(d->current_button)) {
        XtVaSetValues(d->start_text, XmNsensitive, False, NULL);
        XtVaSetValues(d->end_text, XmNsensitive, False, NULL);
        XtVaSetValues(d->list_text, XmNsensitive, False, NULL);
    }

    if (XmToggleButtonGetState(d->range_button)) {
        XtVaSetValues(d->start_text, XmNsensitive, True, NULL);
        XtVaSetValues(d->end_text, XmNsensitive, True, NULL);
        XtVaSetValues(d->list_text, XmNsensitive, False, NULL);
    }

    if (XmToggleButtonGetState(d->list_button)) {
        XtVaSetValues(d->start_text, XmNsensitive, False, NULL);
        XtVaSetValues(d->end_text, XmNsensitive, False, NULL);
        XtVaSetValues(d->list_text, XmNsensitive, True, NULL);
        XmToggleButtonSetState(d->parity_all_button, True, False);
        XmToggleButtonSetState(d->parity_odd_button, False, False);
        XmToggleButtonSetState(d->parity_even_button, False, False);
    }

    if (XmToggleButtonGetState(d->current_button)) {
        XmToggleButtonSetState(d->parity_all_button, True, False);
        XmToggleButtonSetState(d->parity_odd_button, False, False);
        XmToggleButtonSetState(d->parity_even_button, False, False);
    }

    if (XmToggleButtonGetState(d->list_button) || XmToggleButtonGetState(d->current_button)) {
        XtVaSetValues(d->parity_radio, XmNsensitive, False, NULL);
    } else {
        XtVaSetValues(d->parity_radio, XmNsensitive, True, NULL);
    }
}

static void button_cb(Widget widget, XtPointer client_data, XtPointer call_data)
{
    mx_print_d* d = (mx_print_d*)client_data;

    if (widget == d->print_button) {
        if (d->find_options()) {
            if (d->to_file) {
                if (d->get_output_file()) {
                    d->modal_result = yes_e;
                }
            } else {
                d->modal_result = yes_e;
            }
        } else {
            return;
        }
    } else {
        d->modal_result = cancel_e;
    }
}

void mx_print_d::list_cb(Widget list, XtPointer client_data, XtPointer call_data)
{
    int err;
    mx_print_d* d = (mx_print_d*)client_data;

    char* choice;

    XmListCallbackStruct* cbs = (XmListCallbackStruct*)call_data;

    XmStringGetLtoR(cbs->item, XmFONTLIST_DEFAULT_TAG, &choice);
    d->selected_printer = choice;
    d->m_config->set_string(err, "printer", choice);
    XtFree(choice);
}

mx_print_d::mx_print_d(Widget parent)
    : mx_dialog("print", parent, true, false)
{
    Widget label1, label2, label3, label4, sep;
    Widget label5, label6;

    int n;
    Arg args[10];

    save_d = NULL;
    XtVaSetValues(action_area, XmNfractionBase, 5, NULL);

    bad_copies_d = NULL;
    bad_first_page_d = NULL;
    bad_last_page_d = NULL;

    list = XmCreateScrolledList(control_area, const_cast<char*>("printerList"), NULL, 0);

    XtVaSetValues(list,
        XmNselectionPolicy, XmBROWSE_SELECT,
        NULL);

    XtVaSetValues(XtParent(list),
        XmNtopAttachment, XmATTACH_FORM,
        XmNleftAttachment, XmATTACH_FORM,
        XmNleftOffset, 5,
        XmNtopOffset, 5,
        XmNwidth, 150,
        NULL);

    XtAddCallback(list, XmNbrowseSelectionCallback, list_cb, this);

    fill_list(list);

    sep = XtVaCreateManagedWidget(
        "separator",
        xmSeparatorWidgetClass,
        control_area,
        XmNleftAttachment, XmATTACH_FORM,
        XmNrightAttachment, XmATTACH_FORM,
        XmNtopAttachment, XmATTACH_WIDGET,
        XmNtopWidget, XtParent(list),
        XmNbottomOffset, 2,
        XmNtopOffset, 5,
        XmNleftOffset, 0,
        XmNrightOffset, 0,
        XmNseparatorType, XmSHADOW_ETCHED_IN,
        NULL);

    label1 = XtVaCreateManagedWidget(
        "rangeLabel",
        xmLabelGadgetClass,
        control_area,
        XmNtopAttachment, XmATTACH_WIDGET,
        XmNtopWidget, sep,
        XmNleftAttachment, XmATTACH_FORM,
        XmNleftOffset, 5,
        XmNtopOffset, 10,
        NULL);

    n = 0;
    XtSetArg(args[n], XmNrightAttachment, XmATTACH_FORM);
    n++;
    XtSetArg(args[n], XmNleftAttachment, XmATTACH_WIDGET);
    n++;
    XtSetArg(args[n], XmNleftWidget, label1);
    n++;
    XtSetArg(args[n], XmNtopAttachment, XmATTACH_WIDGET);
    n++;
    XtSetArg(args[n], XmNtopWidget, sep);
    n++;
    XtSetArg(args[n], XmNorientation, XmHORIZONTAL);
    n++;
    XtSetArg(args[n], XmNleftOffset, 5);
    n++;
    XtSetArg(args[n], XmNpacking, XmPACK_TIGHT);
    n++;
    XtSetArg(args[n], XmNradioAlwaysOne, True);
    n++;
    print_range_radio = XmCreateRadioBox(control_area, const_cast<char*>("printRangeRadiobox"), args, n);

    all_button = XtVaCreateManagedWidget("allButton", xmToggleButtonGadgetClass,
        print_range_radio,
        XmNset, True,
        NULL);

    XtAddCallback(all_button, XmNvalueChangedCallback, range_cb, (XtPointer)this);

    current_button = XtVaCreateManagedWidget("currentButton", xmToggleButtonGadgetClass,
        print_range_radio,
        NULL);

    XtAddCallback(current_button, XmNvalueChangedCallback, range_cb, (XtPointer)this);

    range_button = XtVaCreateManagedWidget("rangeButton", xmToggleButtonGadgetClass,
        print_range_radio,
        NULL);

    XtAddCallback(range_button, XmNvalueChangedCallback, range_cb, (XtPointer)this);

    list_button = XtVaCreateManagedWidget("listButton", xmToggleButtonGadgetClass,
        print_range_radio,
        NULL);

    XtAddCallback(list_button, XmNvalueChangedCallback, range_cb, (XtPointer)this);

    label2 = XtVaCreateManagedWidget(
        "startTextLabel",
        xmLabelGadgetClass,
        control_area,
        XmNtopAttachment, XmATTACH_WIDGET,
        XmNtopWidget, print_range_radio,
        XmNleftAttachment, XmATTACH_FORM,
        XmNleftOffset, 5,
        XmNtopOffset, 5,
        NULL);

    start_text = XtVaCreateManagedWidget("startText",
        xmTextWidgetClass, control_area,
        XmNleftAttachment, XmATTACH_WIDGET,
        XmNleftWidget, label2,
        XmNtopAttachment, XmATTACH_WIDGET,
        XmNtopWidget, print_range_radio,
        XmNsensitive, False,
        NULL);

    label3 = XtVaCreateManagedWidget(
        "endTextLabel",
        xmLabelGadgetClass,
        control_area,
        XmNtopAttachment, XmATTACH_WIDGET,
        XmNtopWidget, print_range_radio,
        XmNleftAttachment, XmATTACH_WIDGET,
        XmNleftWidget, start_text,
        XmNtopOffset, 5,
        NULL);

    end_text = XtVaCreateManagedWidget("endText",
        xmTextWidgetClass, control_area,
        XmNleftAttachment, XmATTACH_WIDGET,
        XmNleftWidget, label3,
        XmNtopAttachment, XmATTACH_WIDGET,
        XmNtopWidget, print_range_radio,
        XmNsensitive, False,
        NULL);

    label4 = XtVaCreateManagedWidget(
        "listTextLabel",
        xmLabelGadgetClass,
        control_area,
        XmNtopAttachment, XmATTACH_WIDGET,
        XmNtopWidget, print_range_radio,
        XmNleftAttachment, XmATTACH_WIDGET,
        XmNleftWidget, end_text,
        XmNtopOffset, 5,
        NULL);

    list_text = XtVaCreateManagedWidget("listText",
        xmTextWidgetClass, control_area,
        XmNleftAttachment, XmATTACH_WIDGET,
        XmNleftWidget, label4,
        XmNtopAttachment, XmATTACH_WIDGET,
        XmNtopWidget, print_range_radio,
        XmNsensitive, False,
        XmNrightAttachment, XmATTACH_FORM,
        NULL);

    label5 = XtVaCreateManagedWidget(
        "numCopiesLabel",
        xmLabelGadgetClass,
        control_area,
        XmNtopAttachment, XmATTACH_FORM,
        XmNleftAttachment, XmATTACH_WIDGET,
        XmNleftWidget, XtParent(list),
        NULL);

    num_copies_text = XtVaCreateManagedWidget("numCopiesText",
        xmTextWidgetClass, control_area,
        XmNleftAttachment, XmATTACH_WIDGET,
        XmNleftWidget, label5,
        XmNtopAttachment, XmATTACH_FORM,
        NULL);

    label6 = XtVaCreateManagedWidget(
        "parityLabel",
        xmLabelGadgetClass,
        control_area,
        XmNtopAttachment, XmATTACH_WIDGET,
        XmNtopWidget, label5,
        XmNleftAttachment, XmATTACH_WIDGET,
        XmNleftWidget, XtParent(list),
        NULL);

    n = 0;
    XtSetArg(args[n], XmNleftAttachment, XmATTACH_WIDGET);
    n++;
    XtSetArg(args[n], XmNleftWidget, label6);
    n++;
    XtSetArg(args[n], XmNtopAttachment, XmATTACH_WIDGET);
    n++;
    XtSetArg(args[n], XmNtopWidget, num_copies_text);
    n++;
    XtSetArg(args[n], XmNorientation, XmHORIZONTAL);
    n++;
    XtSetArg(args[n], XmNpacking, XmPACK_TIGHT);
    n++;
    XtSetArg(args[n], XmNradioAlwaysOne, True);
    n++;
    parity_radio = XmCreateRadioBox(control_area, const_cast<char*>("parityRadiobox"), args, n);

    parity_all_button = XtVaCreateManagedWidget("parityAllButton", xmToggleButtonGadgetClass,
        parity_radio,
        XmNset, True,
        NULL);

    parity_odd_button = XtVaCreateManagedWidget("parityOddButton", xmToggleButtonGadgetClass,
        parity_radio,
        NULL);

    parity_even_button = XtVaCreateManagedWidget("parityEvenButton", xmToggleButtonGadgetClass,
        parity_radio,
        NULL);

    collate_button = XtVaCreateManagedWidget("collateButton", xmToggleButtonGadgetClass,
        control_area,
        XmNtopAttachment, XmATTACH_WIDGET,
        XmNtopWidget, parity_radio,
        XmNleftAttachment, XmATTACH_WIDGET,
        XmNleftWidget, XtParent(list),
        NULL);

    // now, the action buttons
    print_button = XtVaCreateManagedWidget(
        "print",
        xmPushButtonGadgetClass, action_area,
        XmNshowAsDefault, True,
        XmNdefaultButtonShadowThickness, 1,
        XmNleftAttachment, XmATTACH_POSITION,
        XmNleftPosition, 1,
        XmNrightAttachment, XmATTACH_POSITION,
        XmNrightPosition, 2,
        NULL);

    XtAddCallback(
        print_button,
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
    XtManageChild(print_range_radio);
    XtManageChild(parity_radio);
    XtManageChild(action_area);
    XtManageChild(control_area);
    XtManageChild(pane);
}

void mx_print_d::fill_list(Widget w)
{
    XmString str[MAX_DIR_FILES];
    cups_dest_t *printers;

    int n, i = 0, j, d = 0;

    str[i++] = XmStringCreate(const_cast<char *>(MX_PRINT_TO_FILE), XmFONTLIST_DEFAULT_TAG);

    n = cupsGetDests(&printers);

    for (j = 0; j < n; j++) {
        str[i++] = XmStringCreate(printers[j].name, XmFONTLIST_DEFAULT_TAG);
        if (printers[j].is_default) {
            d = j;
        }
    }

    cupsFreeDests(n, printers);
    
    XtVaSetValues(w, XmNitemCount, i, XmNselectedPosition, d, XmNitems, str, NULL);

    for (j = 0; j < i; j++) {
        XmStringFree(str[j]);
    }
}

void mx_print_d::activate_d(int num_pages, bool options_sensitive)
{
    char s[20];
    XmString str;
    int i, err;
    Widget w[13] = {
        print_range_radio, all_button, current_button,
        range_button, list_button,
        start_text, end_text, list_text,
        collate_button,
        parity_radio, parity_all_button,
        parity_odd_button, parity_even_button,
    };

    for (i = 0; i < 13; i++) {
        XtVaSetValues(w[i], XmNsensitive, options_sensitive ? True : False, NULL);
    }

    XmTextSetString(num_copies_text, const_cast<char*>("1"));
    XmTextSetString(start_text, const_cast<char*>("1"));
    sprintf(s, "%d", num_pages);
    XmTextSetString(end_text, s);
    XmTextSetString(list_text, const_cast<char*>(""));

    selected_printer = m_config->get_default_string(err, "printer", MX_PRINT_TO_FILE);

    str = XmStringCreate(const_cast<char *>(selected_printer.c_str()), XmFONTLIST_DEFAULT_TAG);
    XmListSelectItem(list, str, False);

    XmStringFree(str);
}

bool mx_print_d::find_options()
{
    char *s, temp[20];

    first_page = last_page = num_pages = 0;
    strcpy(selected_file, "");
    if (XmToggleButtonGetState(all_button)) {
        range = print_all_e;
    }
    if (XmToggleButtonGetState(current_button)) {
        range = print_current_e;
    }
    if (XmToggleButtonGetState(range_button)) {
        range = print_range_e;
        s = XmTextGetString(start_text);
        first_page = atoi(s);
        XtFree(s);
        if (mx_is_blank(s) || (first_page < 1)) {
            if (bad_first_page_d == NULL) {
                bad_first_page_d = new mx_inform_d("badFirstPage", dialog, error_e);
            }
            bad_first_page_d->centre();
            bad_first_page_d->run_modal();
            bad_first_page_d->deactivate();
            return false;
        }

        sprintf(temp, "%d", first_page);
        XmTextSetString(start_text, temp);

        s = XmTextGetString(end_text);
        last_page = atoi(s);
        XtFree(s);

        if (mx_is_blank(s) || (last_page < 1) || (last_page < first_page)) {
            if (bad_last_page_d == NULL) {
                bad_last_page_d = new mx_inform_d("badLastPage", dialog, error_e);
            }
            bad_last_page_d->centre();
            bad_last_page_d->run_modal();
            bad_last_page_d->deactivate();
            return false;
        }

        sprintf(temp, "%d", last_page);
        XmTextSetString(end_text, temp);
    }

    if (XmToggleButtonGetState(list_button)) {
        s = XmTextGetString(list_text);
        get_print_list(s);
        XtFree(s);
        range = print_list_e;
    }

    s = XmTextGetString(num_copies_text);
    num_copies = atoi(s);
    XtFree(s);
    if (mx_is_blank(s) || (num_copies < 1)) {
        if (bad_copies_d == NULL) {
            bad_copies_d = new mx_inform_d("badCopies", dialog, error_e);
        }
        bad_copies_d->centre();
        bad_copies_d->run_modal();
        bad_copies_d->deactivate();
        return false;
    } else {
        sprintf(temp, "%d", num_copies);
        XmTextSetString(num_copies_text, temp);
    }

    to_file = selected_printer == MX_PRINT_TO_FILE;
    collate = XmToggleButtonGetState(collate_button);

    if (XmToggleButtonGetState(parity_all_button)) {
        parity = print_both_e;
    }
    if (XmToggleButtonGetState(parity_odd_button)) {
        parity = print_odd_e;
    }
    if (XmToggleButtonGetState(parity_even_button)) {
        parity = print_even_e;
    }
    return true;
}

bool mx_print_d::get_output_file()
{
    if (save_d == NULL) {
        save_d = new mx_save_d("save", dialog, mx_ps_file_e, mx_ps_file_class_e, ".");
    }
    save_d->centre();
    save_d->activate();

    if (save_d->run_modal() == yes_e) {
        save_d->deactivate();
        strcpy(selected_file, save_d->selected_file_name);
        return true;
    } else {
        save_d->deactivate();
        return false;
    }
}

void mx_print_d::get_print_list(char* s)
{
    int i = 0, n = 0;
    int l = strlen(s);

    while (i < l) {
        if (isdigit(s[i])) {
            pages_to_print[n++] = atoi(s + i);
            while (isdigit(s[i++]))
                ;
        }
    }
    num_pages = n;
}

mx_print_d::~mx_print_d()
{
    if (save_d != NULL) {
        delete save_d;
    }
    if (bad_copies_d != NULL) {
        delete bad_copies_d;
    }
    if (bad_first_page_d != NULL) {
        delete bad_first_page_d;
    }
    if (bad_last_page_d != NULL) {
        delete bad_last_page_d;
    }
}

void mx_print_d::fill_res_list(Widget w)
{
    XmString str[5];
    int i;
    char s[20];

    int res[5] = { 200, 300, 600, 1200, 2400 };

    XtVaSetValues(w, XmNvisibleItemCount, 5, NULL);

    for (i = 0; i < 5; i++) {
        sprintf(s, "%d dpi", res[i]);
        str[i] = XmStringCreate(s, XmFONTLIST_DEFAULT_TAG);
    }

    XtVaSetValues(w,
        XmNitemCount, 5,
        XmNitems, str,
        NULL);

    for (i = 0; i < 5; i++) {
        XmStringFree(str[i]);
    }
}

int mx_print_d::run(int num_pages, mx_config *config, bool options_sensitive)
{
    int res;

    m_config = config;

    centre();
    activate_d(num_pages, options_sensitive);
    res = run_modal();
    deactivate();

    return res;
}
