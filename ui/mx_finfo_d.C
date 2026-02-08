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
 * MODULE/CLASS : mx_finfo_d
 *
 * AUTHOR : Andrew Haisley
 *
 * This file
 *
 * DESCRIPTION:
 *
 *  A file permissions edit dialog
 *
 *
 *
 *
 */

#include <dirent.h>
#include <errno.h>
#include <grp.h>
#include <pwd.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <time.h>
#include <unistd.h>

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
#include "mx_dialog.h"
#include "mx_finfo_d.h"
#include <maxwell.h>
#include <mx_file_type.h>

static void button_cb(
    Widget widget,
    XtPointer client_data,
    XtPointer call_data)
{
    mx_finfo_d* d = (mx_finfo_d*)client_data;

    d->modal_result = cancel_e;
}

static void chmod_cb(
    Widget widget,
    XtPointer client_data,
    XtPointer call_data)
{
    mx_finfo_d* d = (mx_finfo_d*)client_data;
    struct stat st;
    mode_t m;

    stat(d->current_file_name, &st);

    m = st.st_mode & (S_ISUID | S_ISGID | S_ISVTX | S_IXUSR | S_IXGRP | S_IXOTH);

    if (XmToggleButtonGetState(d->ureadToggle)) {
        m |= S_IRUSR;
    }
    if (XmToggleButtonGetState(d->uwriteToggle)) {
        m |= S_IWUSR;
    }
    if (XmToggleButtonGetState(d->greadToggle)) {
        m |= S_IRGRP;
    }
    if (XmToggleButtonGetState(d->gwriteToggle)) {
        m |= S_IWGRP;
    }
    if (XmToggleButtonGetState(d->oreadToggle)) {
        m |= S_IROTH;
    }
    if (XmToggleButtonGetState(d->owriteToggle)) {
        m |= S_IWOTH;
    }
    chmod(d->current_file_name, m);
}

mx_finfo_d::mx_finfo_d(const char* name, Widget parent)
    : mx_dialog(name, parent, TRUE, FALSE)
{
    Widget row_column1, row_column2;
    Widget row_column3, row_column4;
    Widget row_column5, row_column6;
    Widget row_column7, sep;

    Widget close_button;

    row_column1 = XtVaCreateManagedWidget(
        "rowColumn1",
        xmRowColumnWidgetClass,
        control_area,
        XmNtopOffset, 10,
        XmNtopAttachment, XmATTACH_FORM,
        XmNleftAttachment, XmATTACH_FORM,
        NULL);

    (void)XtVaCreateManagedWidget(
        "readPermText",
        xmLabelGadgetClass,
        row_column1,
        NULL);

    (void)XtVaCreateManagedWidget(
        "writePermText",
        xmLabelGadgetClass,
        row_column1,
        NULL);

    row_column2 = XtVaCreateManagedWidget(
        "rowColumn2",
        xmRowColumnWidgetClass,
        control_area,
        XmNtopAttachment, XmATTACH_FORM,
        XmNrightAttachment, XmATTACH_FORM,
        XmNleftAttachment, XmATTACH_WIDGET,
        XmNleftWidget, row_column1,
        NULL);

    row_column3 = XtVaCreateManagedWidget(
        "rowColumn3",
        xmRowColumnWidgetClass,
        row_column2,
        XmNorientation, XmHORIZONTAL,
        NULL);

    row_column4 = XtVaCreateManagedWidget(
        "rowColumn3",
        xmRowColumnWidgetClass,
        row_column2,
        XmNorientation, XmHORIZONTAL,
        NULL);

    ureadToggle = XtVaCreateManagedWidget(
        "ureadToggle",
        xmToggleButtonGadgetClass,
        row_column3,
        NULL);

    greadToggle = XtVaCreateManagedWidget(
        "greadToggle",
        xmToggleButtonGadgetClass,
        row_column3,
        NULL);

    oreadToggle = XtVaCreateManagedWidget(
        "oreadToggle",
        xmToggleButtonGadgetClass,
        row_column3,
        NULL);

    uwriteToggle = XtVaCreateManagedWidget(
        "uwriteToggle",
        xmToggleButtonGadgetClass,
        row_column4,
        NULL);

    gwriteToggle = XtVaCreateManagedWidget(
        "gwriteToggle",
        xmToggleButtonGadgetClass,
        row_column4,
        NULL);

    owriteToggle = XtVaCreateManagedWidget(
        "owriteToggle",
        xmToggleButtonGadgetClass,
        row_column4,
        NULL);

    XtAddCallback(ureadToggle, XmNvalueChangedCallback, chmod_cb, (XtPointer)this);
    XtAddCallback(greadToggle, XmNvalueChangedCallback, chmod_cb, (XtPointer)this);
    XtAddCallback(oreadToggle, XmNvalueChangedCallback, chmod_cb, (XtPointer)this);
    XtAddCallback(uwriteToggle, XmNvalueChangedCallback, chmod_cb, (XtPointer)this);
    XtAddCallback(gwriteToggle, XmNvalueChangedCallback, chmod_cb, (XtPointer)this);
    XtAddCallback(owriteToggle, XmNvalueChangedCallback, chmod_cb, (XtPointer)this);

    sep = XtVaCreateManagedWidget(
        "separator",
        xmSeparatorWidgetClass,
        control_area,
        XmNleftAttachment, XmATTACH_FORM,
        XmNrightAttachment, XmATTACH_FORM,
        XmNtopAttachment, XmATTACH_WIDGET,
        XmNtopWidget, row_column2,
        XmNtopOffset, 4,
        XmNseparatorType, XmSHADOW_ETCHED_IN,
        NULL);

    row_column5 = XtVaCreateManagedWidget(
        "rowColumn5",
        xmRowColumnWidgetClass,
        control_area,
        XmNtopAttachment, XmATTACH_WIDGET,
        XmNtopWidget, sep,
        XmNtopOffset, 4,
        XmNleftAttachment, XmATTACH_FORM,
        XmNbottomAttachment, XmATTACH_FORM,
        XmNorientation, XmHORIZONTAL,
        NULL);

    row_column6 = XtVaCreateManagedWidget(
        "rowColumn6",
        xmRowColumnWidgetClass,
        row_column5,
        NULL);

    row_column7 = XtVaCreateManagedWidget(
        "rowColumn7",
        xmRowColumnWidgetClass,
        row_column5,
        NULL);

    (void)XtVaCreateManagedWidget(
        "nameText",
        xmLabelGadgetClass,
        row_column6,
        NULL);

    (void)XtVaCreateManagedWidget(
        "typeText",
        xmLabelGadgetClass,
        row_column6,
        NULL);

    (void)XtVaCreateManagedWidget(
        "ownerText",
        xmLabelGadgetClass,
        row_column6,
        NULL);

    (void)XtVaCreateManagedWidget(
        "groupText",
        xmLabelGadgetClass,
        row_column6,
        NULL);

    (void)XtVaCreateManagedWidget(
        "modifiedText",
        xmLabelGadgetClass,
        row_column6,
        NULL);

    nameWidget = XtVaCreateManagedWidget(
        "wwwwwwwwwwwwwwwwwwwwwwfileNameText",
        xmLabelGadgetClass,
        row_column7,
        NULL);

    typeWidget = XtVaCreateManagedWidget(
        "wwwwwwwwwwwwwwwwwwwwwwfileTypeText",
        xmLabelGadgetClass,
        row_column7,
        NULL);

    ownerWidget = XtVaCreateManagedWidget(
        "wwwwwwwwwwwwwwwwwwwwwwfileOwnerText",
        xmLabelGadgetClass,
        row_column7,
        NULL);

    groupWidget = XtVaCreateManagedWidget(
        "wwwwwwwwwwwwwwwwwwwwwwfileGroupText",
        xmLabelGadgetClass,
        row_column7,
        NULL);

    dateWidget = XtVaCreateManagedWidget(
        "wwwwwwwwwwwwwwwwwwwwwwfileDateText",
        xmLabelGadgetClass,
        row_column7,
        NULL);

    // now, the action buttons
    XtVaSetValues(action_area, XmNfractionBase, 3, NULL);

    close_button = XtVaCreateManagedWidget(
        "closePermButton",
        xmPushButtonGadgetClass, action_area,
        XmNshowAsDefault, True,
        XmNdefaultButtonShadowThickness, 1,
        XmNleftAttachment, XmATTACH_POSITION,
        XmNleftPosition, 1,
        XmNrightAttachment, XmATTACH_POSITION,
        XmNrightPosition, 2,
        NULL);

    XtAddCallback(
        close_button,
        XmNactivateCallback,
        button_cb,
        (XtPointer)this);

    XtManageChild(action_area);
    XtManageChild(control_area);
    XtManageChild(pane);
}

void mx_finfo_d::activate_d(char* file_name)
{
    XmString str;
    struct passwd* pw;
    struct group* gr;
    struct stat st;
    struct tm* tp;

    FILE* f;
    char temp[MAX_PATH_LEN];
    int i;

    strcpy(current_file_name, file_name);

    // set the variable parts
    str = XmStringCreateLocalized(file_name);
    XtVaSetValues(nameWidget, XmNlabelString, str, NULL);

    stat(file_name, &st);

    pw = getpwuid(st.st_uid);
    if (pw != NULL) {
        str = XmStringCreateLocalized(pw->pw_name);
    } else {
        char uid_num[20];
        sprintf(uid_num, "%d", (uint32)st.st_uid);
        str = XmStringCreateLocalized(uid_num);
    }
    XtVaSetValues(ownerWidget, XmNlabelString, str, NULL);

    gr = getgrgid(st.st_gid);
    if (gr != NULL) {
        str = XmStringCreateLocalized(gr->gr_name);
    } else {
        char gid_num[20];
        sprintf(gid_num, "%d", (uint32)st.st_gid);
        str = XmStringCreateLocalized(gid_num);
    }
    XtVaSetValues(groupWidget, XmNlabelString, str, NULL);

    if (mx_is_file_type(file_name, mx_document_file_e)) {
        str = XmStringCreateLocalized((char*)mx_file_type_full_name(mx_document_file_e));
    } else {
        if (mx_is_file_type(file_name, mx_rtf_file_e)) {
            str = XmStringCreateLocalized((char*)mx_file_type_full_name(mx_rtf_file_e));
        } else {
            sprintf(temp, "%s/bin/file %s", global_maxhome, file_name);
            f = popen(temp, "r");

            if (f == NULL) {
                str = XmStringCreateLocalized((char*)mx_file_type_full_name(mx_guess_file_type(file_name)));
            } else {
                i = fread(temp, 1, MAX_PATH_LEN - 1, f);
                temp[i] = 0;

                str = XmStringCreateLocalized(temp + strlen(file_name) + 2);
                pclose(f);
            }
        }
    }
    XtVaSetValues(typeWidget, XmNlabelString, str, NULL);

    tp = localtime(&st.st_mtime);
    (void)strftime(temp, MAX_PATH_LEN - 1, "%c", tp);
    str = XmStringCreateLocalized(temp);
    XtVaSetValues(dateWidget, XmNlabelString, str, NULL);

    XmToggleButtonSetState(ureadToggle, (S_IRUSR & st.st_mode) != 0, FALSE);
    XmToggleButtonSetState(uwriteToggle, (S_IWUSR & st.st_mode) != 0, FALSE);
    XmToggleButtonSetState(greadToggle, (S_IRGRP & st.st_mode) != 0, FALSE);
    XmToggleButtonSetState(gwriteToggle, (S_IWGRP & st.st_mode) != 0, FALSE);
    XmToggleButtonSetState(oreadToggle, (S_IROTH & st.st_mode) != 0, FALSE);
    XmToggleButtonSetState(owriteToggle, (S_IWOTH & st.st_mode) != 0, FALSE);

    mx_dialog::activate();
}
