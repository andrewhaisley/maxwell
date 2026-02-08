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
 * MODULE/CLASS : mx_new_d
 *
 * AUTHOR : Andrew Haisley
 *
 * This file
 *
 * DESCRIPTION:
 *
 *  A new document dialog.
 *
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

#include "mx_new_d.h"

static void button_cb(
    Widget widget,
    XtPointer client_data,
    XtPointer call_data)
{
    mx_new_d* d = (mx_new_d*)client_data;

    if (widget == d->new_button) {
        d->modal_result = yes_e;
    } else {
        d->modal_result = cancel_e;
    }
}

static void list_cb(Widget list, XtPointer client_data, XtPointer call_data)
{
    mx_new_d* d = (mx_new_d*)client_data;

    char* choice;

    XmListCallbackStruct* cbs = (XmListCallbackStruct*)call_data;

    XmStringGetLtoR(cbs->item, XmFONTLIST_DEFAULT_TAG, &choice);
    strcpy(d->template_name, choice);
    XtFree(choice);

    XtVaSetValues(d->new_button, XmNsensitive, True, NULL);

    if (cbs->reason == XmCR_BROWSE_SELECT) {
        // single clicked on a template
        return;
    }

    if (cbs->reason == XmCR_DEFAULT_ACTION) {
        // double clicked on a template
        d->modal_result = yes_e;
    }
}

mx_new_d::mx_new_d(Widget parent, const char* d)
    : mx_dialog("new", parent, TRUE, FALSE)
{

    strcpy(template_dir, d);

    XtVaSetValues(action_area, XmNfractionBase, 5, NULL);

    list = XmCreateScrolledList(control_area, const_cast<char*>("templateList"), NULL, 0);

    XtVaSetValues(list,
        XmNvisibleItemCount, 20,
        XmNselectionPolicy, XmBROWSE_SELECT,
        NULL);

    XtVaSetValues(XtParent(list),
        XmNtopAttachment, XmATTACH_FORM,
        XmNleftAttachment, XmATTACH_FORM,
        XmNrightAttachment, XmATTACH_FORM,
        XmNbottomAttachment, XmATTACH_FORM,
        XmNleftOffset, 5,
        XmNbottomOffset, 5,
        XmNrightOffset, 5,
        XmNtopOffset, 5,
        NULL);

    XtAddCallback(list, XmNdefaultActionCallback, list_cb, this);
    XtAddCallback(list, XmNbrowseSelectionCallback, list_cb, this);

    fill_list();

    // now, the action buttons
    new_button = XtVaCreateManagedWidget(
        "new",
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
        new_button,
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
    XtManageChild(action_area);
    XtManageChild(control_area);
    XtManageChild(pane);
}

static int aka_strcmp(const void* p1, const void* p2)
{
    return strcmp(reinterpret_cast<const char*>(p1), reinterpret_cast<const char*>(p2));
}

void mx_new_d::fill_list()
{
    struct dirent* d;

    DIR* dir;
    XmString str[MAX_DIR_FILES];
    int i;

    static char file_names[MAX_DIR_FILES][MAX_FILE_NAME_LEN];
    struct stat fs;
    char temp[MAX_PATH_LEN * 2];

    int num_files = 0;

    // open the directory
    dir = opendir(template_dir);

    while (TRUE) {
        d = readdir(dir);
        if (d == NULL) {
            closedir(dir);
            break;
        }

        sprintf(temp, "%s/%s", template_dir, d->d_name);
        if (stat(temp, &fs) < 0) {
            continue;
        }

        if (S_ISREG(fs.st_mode)) {
            if (mx_is_file_type(temp, mx_document_file_e)) {
                strncpy(file_names[num_files++], d->d_name, MAX_FILE_NAME_LEN);
            } else {
                continue;
            }
        }
    }

    qsort(file_names, num_files, MAX_FILE_NAME_LEN, aka_strcmp);

    for (i = 0; i < num_files; i++) {
        str[i] = XmStringCreate(file_names[i], XmFONTLIST_DEFAULT_TAG);
    }

    XtVaSetValues(list,
        XmNitemCount, num_files,
        XmNitems, str,
        NULL);

    for (i = 0; i < num_files; i++) {
        XmStringFree(str[i]);
    }
}

void mx_new_d::activate()
{
    fill_list();
    mx_dialog::activate();
}
