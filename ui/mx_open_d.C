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
 * MODULE/CLASS : mx_open_d
 *
 * AUTHOR : Andrew Haisley
 *
 * This file
 *
 * DESCRIPTION:
 *
 *  A file browsing/opening dialog
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
#include "mx_finfo_d.h"
#include "mx_inform_d.h"
#include "mx_open_d.h"
#include "mx_yes_no_d.h"
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
#include <mx_file_type.h>
#include <mx_std.h>
#include <mx_ui_object.h>

static void type_change(
    Widget widget,
    XtPointer client_data,
    XtPointer call_data)
{
    open_file_type_callback_t* t = (open_file_type_callback_t*)client_data;

    t->d->default_file_type = t->t;
    t->d->dir_loaded = FALSE;
    t->d->load_directory();
}

static void fix_file_name(char* in, char* out)
{
    int i = 0, j;
    struct passwd* pw;

    while (in[i] == ' ') {
        i++;
    }

    if (in[i] == '~' && in[i + 1] == '/') {
        i++;
        pw = getpwuid(getuid());

        strcpy(out, "~");
        strcat(out, pw->pw_name);
        strcat(out, in + i);
    } else {
        strcpy(out, in + i);
    }

    j = strlen(out) - 1;
    while (j > 0 && out[j] == ' ') {
        j--;
    }

    out[j + 1] = 0;
}

static bool file_openable(bool want_write, char* s, mx_open_d* d)
{
    struct stat fs;
    uid_t uid;
    gid_t gid;

    bool read_perm, write_perm;

    uid = getuid();
    gid = getgid();

    if (stat(s, &fs) < 0) {
        if (d->no_exist_d == NULL) {
            d->no_exist_d = new mx_inform_d("noExist", d->dialog, error_e);
        }
        (void)d->no_exist_d->centre();
        (void)d->no_exist_d->run_modal();
        d->no_exist_d->deactivate();
        return FALSE;
    }

    // is the user the owner of the file ?
    if (fs.st_uid == uid) {
        read_perm = S_IREAD & fs.st_mode;
        write_perm = S_IWRITE & fs.st_mode;
    } else {
        // is the user in the same group ?
        if (fs.st_gid == gid) {
            read_perm = (S_IRGRP & fs.st_mode) || (S_IROTH & fs.st_mode);
            write_perm = (S_IWGRP & fs.st_mode) || (S_IWOTH & fs.st_mode);
        } else {
            read_perm = S_IROTH & fs.st_mode;
            write_perm = S_IWOTH & fs.st_mode;
        }
    }

    d->selected_write_perm = write_perm;
    strcpy(d->selected_file_name, s);

    //    if (want_write && !write_perm)
    if (want_write && (write_perm == 0)) {
        // see if they want to open it read only
        if (d->no_write_d == NULL) {
            d->no_write_d = new mx_yes_no_d("noWrite", d->dialog, FALSE, yes_e);
        }
        d->no_write_d->centre();
        if (d->no_write_d->run_modal() == yes_e) {
            d->no_write_d->deactivate();
            return TRUE;
        } else {
            d->no_write_d->deactivate();
            return FALSE;
        }
    }

    // oh dear, no read permission
    //    if (!read_perm)
    if (read_perm == 0) {
        if (d->no_read_d == NULL) {
            d->no_read_d = new mx_inform_d("noRead", d->dialog, error_e);
        }
        (void)d->no_read_d->centre();
        (void)d->no_read_d->run_modal();
        d->no_read_d->deactivate();
        return FALSE;
    }
    //    return FALSE;
    return TRUE;
}

static void handle_file_selection(mx_open_d* d)
{
    char* file_name;
    uid_t uid;
    gid_t gid;

    bool exec_perm;
    char s[MAX_PATH_LEN];

    struct stat fs;

    file_name = XmTextGetString(d->file_text);
    fix_file_name(file_name, s);
    XtFree(file_name);

    stat(s, &fs);

    if (S_ISDIR(fs.st_mode)) {
        uid = getuid();
        gid = getgid();

        // is the user the owner of the dir ?
        if (fs.st_uid == uid) {
            exec_perm = S_IEXEC & fs.st_mode;
        } else {
            // is the user in the same group ?
            if (fs.st_gid == gid) {
                exec_perm = (S_IXGRP & fs.st_mode) || (S_IXOTH & fs.st_mode);
            } else {
                exec_perm = S_IXOTH & fs.st_mode;
            }
        }

        if (!exec_perm) {
            if (d->no_exec_d == NULL) {
                d->no_exec_d = new mx_inform_d("noExec", d->dialog, error_e);
            }
            (void)d->no_exec_d->centre();
            (void)d->no_exec_d->run_modal();
            d->no_exec_d->deactivate();
            return;
        }

        if (chdir(s) != 0) {
            return;
        }
        if (getcwd(d->current_dir, MAX_PATH_LEN) == nullptr) {
            return;
        }
        d->dir_loaded = FALSE;
        XmTextSetString(d->file_text, const_cast<char*>(""));
        XtVaSetValues(d->perm_button, XmNsensitive, False, NULL);
        XtVaSetValues(d->open_button, XmNsensitive, False, NULL);
        d->load_directory();

        XtUnmanageChild(d->path_sub_menu);
        d->path_sub_menu = XmCreatePulldownMenu(d->control_area, const_cast<char*>("pathPulldown"), NULL, 0);
        d->set_path_menu(d->path_sub_menu);

        XtVaSetValues(d->path_menu, XmNsubMenuId, d->path_sub_menu, NULL);
    } else {
        if (file_openable(d->want_write, s, d)) {
            d->modal_result = yes_e;
        }
    }
}

static void button_cb(
    Widget widget,
    XtPointer client_data,
    XtPointer call_data)
{
    mx_open_d* d = (mx_open_d*)client_data;

    if (widget == d->open_button) {
        handle_file_selection(d);
    } else {
        d->modal_result = cancel_e;
    }
}

static void path_change(
    Widget widget,
    XtPointer client_data,
    XtPointer call_data)
{
    mx_open_d* d = (mx_open_d*)client_data;

    int i;

    for (i = 0; i < d->num_path_buttons; i++) {
        if (d->path_buttons[i] == widget) {
            if (i == 0) {
                strcpy(d->current_dir, "/");
            } else {
                d->current_dir[d->path_indexes[i]] = 0;
            }

            if (chdir(d->current_dir) != 0) {
                return;
            }

            d->dir_loaded = FALSE;
            XmTextSetString(d->file_text, const_cast<char*>(""));
            XtVaSetValues(d->perm_button, XmNsensitive, False, NULL);
            XtVaSetValues(d->open_button, XmNsensitive, False, NULL);
            d->load_directory();

            XtUnmanageChild(d->path_sub_menu);
            d->path_sub_menu = XmCreatePulldownMenu(d->control_area, const_cast<char*>("pathPulldown"), NULL, 0);
            d->set_path_menu(d->path_sub_menu);

            XtVaSetValues(d->path_menu, XmNsubMenuId, d->path_sub_menu, NULL);
        }
    }
}

static void refresh_button_cb(
    Widget widget,
    XtPointer client_data,
    XtPointer call_data)
{
    mx_open_d* d = (mx_open_d*)client_data;

    XmTextSetString(d->file_text, const_cast<char*>(""));
    XtVaSetValues(d->perm_button, XmNsensitive, False, NULL);
    XtVaSetValues(d->open_button, XmNsensitive, False, NULL);

    d->dir_loaded = FALSE;
    d->load_directory();
}

static void perm_button_cb(
    Widget widget,
    XtPointer client_data,
    XtPointer call_data)
{
    mx_open_d* d = (mx_open_d*)client_data;
    struct stat fs;
    char* s2 = XmTextGetString(d->file_text);

    if (stat(s2, &fs) < 0) {
        if (d->no_exist_d == NULL) {
            d->no_exist_d = new mx_inform_d("noExist", d->dialog, error_e);
        }
        (void)d->no_exist_d->centre();
        (void)d->no_exist_d->run_modal();
        d->no_exist_d->deactivate();
    } else {
        char* s;
        if (d->finfo_d == NULL) {
            d->finfo_d = new mx_finfo_d("openFilePerm", d->dialog);
        }
        d->finfo_d->centre();
        s = XmTextGetString(d->file_text);
        d->finfo_d->activate_d(s);
        XtFree(s);
        (void)d->finfo_d->run_modal();
        d->finfo_d->deactivate();
    }
    XtFree(s2);
}

static void file_name_return(
    Widget widget,
    XtPointer client_data,
    XtPointer call_data)
{
    mx_open_d* d = (mx_open_d*)client_data;

    handle_file_selection(d);
}

static void file_name_changed(
    Widget widget,
    XtPointer client_data,
    XtPointer call_data)
{
    mx_open_d* d = (mx_open_d*)client_data;
    char* s;

    s = XmTextGetString(d->file_text);

    XtVaSetValues(d->perm_button, XmNsensitive,
        mx_is_blank(s) ? False : True,
        NULL);

    XtVaSetValues(d->open_button, XmNsensitive,
        mx_is_blank(s) ? False : True,
        NULL);
    XtFree(s);
}

static void select_file(Widget list, XtPointer client_data, XtPointer call_data)
{
    mx_open_d* d = (mx_open_d*)client_data;

    char* choice;

    XmListCallbackStruct* cbs = (XmListCallbackStruct*)call_data;

    if (!XmStringGetLtoR(cbs->item, const_cast<char*>("FILETAG"), &choice)) {
        if (!XmStringGetLtoR(cbs->item, const_cast<char*>("DIRTAG"), &choice)) {
            choice = nullptr;
        }
    }

    XtVaSetValues(d->perm_button, XmNsensitive, True, NULL);
    XtVaSetValues(d->open_button, XmNsensitive, True, NULL);

    if (cbs->reason == XmCR_BROWSE_SELECT) {
        // single clicked on a file
        if (choice) {
            XmTextSetString(d->file_text, choice);
            XtFree(choice);
        } else {
            XmTextSetString(d->file_text, const_cast<char*>(""));
        }
        return;
    } else {
        if (choice) {
            XtFree(choice);
        }
    }

    if (cbs->reason == XmCR_DEFAULT_ACTION) {
        // double clicked on a file
        handle_file_selection(d);
    }
}

mx_open_d::mx_open_d(
    const char* name,
    Widget parent,
    mx_file_type_t default_type,
    mx_file_class_t c,
    bool want_write_permission)
    : mx_dialog(name, parent, TRUE, FALSE)
{
    Widget label1, label2, label3, refresh_button;
    Widget sep;
    Arg args[15];
    int n;

    num_file_types = 0;
    want_write = want_write_permission;
    no_write_d = NULL;
    no_read_d = NULL;
    no_exist_d = NULL;
    no_exec_d = NULL;
    finfo_d = NULL;
    default_file_type = default_type;
    file_class = c;

    if (getcwd(current_dir, MAX_PATH_LEN) == nullptr) {
        return;
    }
    dir_loaded = FALSE;

    XtVaSetValues(action_area, XmNfractionBase, 5, NULL);
    XtVaSetValues(control_area, XmNfractionBase, 7, NULL);

    label1 = XtVaCreateManagedWidget(
        "pathText",
        xmLabelGadgetClass,
        control_area,
        XmNtopAttachment, XmATTACH_FORM,
        XmNleftAttachment, XmATTACH_FORM,
        XmNleftOffset, 5,
        XmNtopOffset, 10,
        NULL);

    path_sub_menu = XmCreatePulldownMenu(control_area, const_cast<char*>("pathPulldown"), NULL, 0);
    set_path_menu(path_sub_menu);

    // path menu
    n = 0;
    XtSetArg(args[n], XmNsubMenuId, path_sub_menu);
    n++;
    XtSetArg(args[n], XmNtopAttachment, XmATTACH_FORM);
    n++;
    XtSetArg(args[n], XmNleftAttachment, XmATTACH_WIDGET);
    n++;
    XtSetArg(args[n], XmNleftWidget, label1);
    n++;
    XtSetArg(args[n], XmNrightAttachment, XmATTACH_FORM);
    n++;
    XtSetArg(args[n], XmNrightOffset, 2);
    n++;
    XtSetArg(args[n], XmNtopOffset, 2);
    n++;
    XtSetArg(args[n], XmNsubMenuId, path_sub_menu);
    n++;

    path_menu = XmCreateOptionMenu(control_area, const_cast<char*>("pathMenu"), args, n);

    label3 = XtVaCreateManagedWidget(
        "nameText",
        xmLabelGadgetClass,
        control_area,
        XmNbottomAttachment, XmATTACH_FORM,
        XmNleftAttachment, XmATTACH_FORM,
        NULL);

    // file name text exit
    file_text = XtVaCreateManagedWidget("fileName",
        xmTextWidgetClass, control_area,
        XmNleftAttachment, XmATTACH_WIDGET,
        XmNleftWidget, label3,
        XmNbottomAttachment, XmATTACH_FORM,
        XmNrightAttachment, XmATTACH_FORM,
        XmNrightOffset, 5,
        XmNleftOffset, 5,
        XmNbottomOffset, 2,
        NULL);

    XtAddCallback(file_text, XmNvalueChangedCallback, file_name_changed, this);
    XtAddCallback(file_text, XmNactivateCallback, file_name_return, this);

    sep = XtVaCreateManagedWidget(
        "separator",
        xmSeparatorWidgetClass,
        control_area,
        XmNbottomAttachment, XmATTACH_WIDGET,
        XmNleftAttachment, XmATTACH_FORM,
        XmNrightAttachment, XmATTACH_FORM,
        XmNbottomWidget, file_text,
        XmNbottomOffset, 2,
        XmNleftOffset, 0,
        XmNrightOffset, 0,
        XmNmargin, 2,
        XmNseparatorType, XmSHADOW_ETCHED_IN,
        NULL);

    file_list = XmCreateScrolledList(control_area, const_cast<char*>("fileList"), NULL, 0);

    XtVaSetValues(file_list,
        XmNvisibleItemCount, 10,
        XmNselectionPolicy, XmBROWSE_SELECT,
        NULL);

    XtVaSetValues(XtParent(file_list),
        XmNtopAttachment, XmATTACH_WIDGET,
        XmNtopWidget, path_menu,
        XmNleftAttachment, XmATTACH_POSITION,
        XmNleftPosition, 0,
        XmNrightAttachment, XmATTACH_POSITION,
        XmNrightPosition, 4,
        XmNbottomAttachment, XmATTACH_WIDGET,
        XmNbottomWidget, sep,
        XmNleftOffset, 5,
        XmNbottomOffset, 5,
        NULL);

    XtAddCallback(file_list, XmNdefaultActionCallback, select_file, this);
    XtAddCallback(file_list, XmNbrowseSelectionCallback, select_file, this);

    label2 = XtVaCreateManagedWidget(
        "typeText",
        xmLabelGadgetClass,
        control_area,
        XmNtopAttachment, XmATTACH_FORM,
        XmNleftAttachment, XmATTACH_WIDGET,
        XmNleftWidget, file_list,
        XmNleftOffset, 5,
        XmNtopOffset, 46,
        NULL);

    num_file_type_buttons = 0;
    type_sub_menu = XmCreatePulldownMenu(control_area, const_cast<char*>("typePulldown"), NULL, 0);
    add_file_types();

    // type menu
    n = 0;
    XtSetArg(args[n], XmNsubMenuId, type_sub_menu);
    n++;
    XtSetArg(args[n], XmNtopAttachment, XmATTACH_FORM);
    n++;
    XtSetArg(args[n], XmNleftAttachment, XmATTACH_WIDGET);
    n++;
    XtSetArg(args[n], XmNleftWidget, label2);
    n++;
    XtSetArg(args[n], XmNrightAttachment, XmATTACH_FORM);
    n++;
    XtSetArg(args[n], XmNrightOffset, 4);
    n++;
    XtSetArg(args[n], XmNtopOffset, 37);
    n++;
    XtSetArg(args[n], XmNsubMenuId, type_sub_menu);
    n++;

    type_menu = XmCreateOptionMenu(control_area, const_cast<char*>("typeMenu"), args, n);

    perm_button = XtVaCreateManagedWidget(
        "permissions",
        xmPushButtonGadgetClass, control_area,
        XmNdefaultButtonShadowThickness, 1,
        XmNleftAttachment, XmATTACH_WIDGET,
        XmNleftWidget, file_list,
        XmNbottomAttachment, XmATTACH_WIDGET,
        XmNbottomWidget, sep,
        XmNsensitive, False,
        NULL);

    refresh_button = XtVaCreateManagedWidget(
        "refresh",
        xmPushButtonGadgetClass, control_area,
        XmNdefaultButtonShadowThickness, 1,
        XmNleftAttachment, XmATTACH_WIDGET,
        XmNleftWidget, perm_button,
        XmNbottomAttachment, XmATTACH_WIDGET,
        XmNbottomWidget, sep,
        NULL);

    XtAddCallback(
        refresh_button,
        XmNactivateCallback,
        refresh_button_cb,
        (XtPointer)this);

    XtAddCallback(
        perm_button,
        XmNactivateCallback,
        perm_button_cb,
        (XtPointer)this);

    // now, the action buttons
    open_button = XtVaCreateManagedWidget(
        "open",
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
        open_button,
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

    (void)XtParent(dialog);

    XtManageChild(file_list);
    XtManageChild(type_menu);
    XtManageChild(path_menu);
    XtManageChild(action_area);
    XtManageChild(control_area);
    XtManageChild(pane);
}

void mx_open_d::activate()
{
    if (chdir(current_dir) != 0) {
        return;
    }
    dir_loaded = FALSE;
    load_directory();
    mx_dialog::activate();
}

static int aka_strcmp(const void* p1, const void* p2)
{
    return strcmp(reinterpret_cast<const char*>(p1), reinterpret_cast<const char*>(p2));
}

void mx_open_d::load_directory()
{
    struct dirent* d;

    DIR* dir;
    XmString str[MAX_DIR_FILES];
    int i;

    static char file_names[MAX_DIR_FILES][MAX_FILE_NAME_LEN];

    struct stat fs;

    if (dir_loaded) {
        return;
    }

    dir_loaded = TRUE;
    num_files = 0;

    // open the directory
    dir = opendir(current_dir);

    while (TRUE) {
        d = readdir(dir);
        if (d == NULL) {
            closedir(dir);
            break;
        }

        if (stat(d->d_name, &fs) < 0) {
            continue;
        }

        if (S_ISREG(fs.st_mode)) {
            if (!mx_is_file_type(d->d_name, default_file_type)) {
                continue;
            }
        } else {
            if ((!S_ISDIR(fs.st_mode)) && (!S_ISLNK(fs.st_mode))) {
                continue;
            }
        }

        if (strcmp(d->d_name, ".") != 0) {
            strncpy(file_names[num_files++], d->d_name, MAX_FILE_NAME_LEN);

            if (num_files >= MAX_DIR_FILES) {
                break;
            }
        }
    }

    qsort(file_names, num_files, MAX_FILE_NAME_LEN, aka_strcmp);

    for (i = 0; i < num_files; i++) {
        stat(file_names[i], &fs);

        if (S_ISREG(fs.st_mode)) {
            str[i] = XmStringCreate(file_names[i], const_cast<char*>("FILETAG"));
        } else {
            str[i] = XmStringCreate(file_names[i], const_cast<char*>("DIRTAG"));
        }
    }

    XtVaSetValues(file_list,
        XmNitemCount, num_files,
        XmNitems, str,
        NULL);

    for (i = 0; i < num_files; i++) {
        XmStringFree(str[i]);
    }
}

void mx_open_d::add_file_types()
{
    int i, n = 0;
    Widget tb, w = type_sub_menu;

    open_file_type_callback_t* t;

    for (i = 0; i < num_file_types; i++) {
        delete file_type_ptr[i];
    }
    num_file_types = 0;

    file_type_buttons[n++] = tb = XtVaCreateManagedWidget(
        mx_file_type_name(default_file_type),
        xmPushButtonGadgetClass,
        w,
        NULL);

    file_type_ptr[num_file_types++] = t = new open_file_type_callback_t;
    t->d = this;
    t->t = default_file_type;

    XtAddCallback(tb, XmNactivateCallback, type_change, t);

    for (i = mx_document_file_e; i <= mx_other_file_e; i++) {
        if ((mx_file_type_t)i != default_file_type) {
            if (is_type_in_class((mx_file_type_t)i, file_class)) {
                file_type_buttons[n++] = tb = XtVaCreateManagedWidget(
                    mx_file_type_name((mx_file_type_t)i),
                    xmPushButtonGadgetClass,
                    w,
                    NULL);

                file_type_ptr[num_file_types++] = t = new open_file_type_callback_t;
                t->d = this;
                t->t = (mx_file_type_t)i;
                XtAddCallback(tb, XmNactivateCallback, type_change, t);
            }
        }
    }

    num_file_type_buttons = n;
}

void mx_open_d::set_path_menu(Widget w)
{
    int i = 1, j, n = 0;
    char s[MAX_PATH_LEN];

    /* add root whatever */
    path_buttons[n] = XtVaCreateManagedWidget(
        "/",
        xmPushButtonGadgetClass,
        w,
        NULL);
    XtAddCallback(path_buttons[n], XmNactivateCallback, path_change, this);
    path_indexes[n++] = 0;

    if (current_dir[0] == 0) {
        num_path_buttons = n;
        return;
    }

    while (current_dir[i] != 0) {
        j = i + 1;
        while (current_dir[j] != 0 && current_dir[j] != '/') {
            j++;
        }
        strncpy(s, current_dir + i, j - i);
        s[j - i] = 0;

        path_buttons[n] = XtVaCreateManagedWidget(
            s,
            xmPushButtonGadgetClass,
            w,
            XmNpositionIndex, 0,
            NULL);
        XtAddCallback(path_buttons[n], XmNactivateCallback, path_change, this);
        path_indexes[n++] = j;

        if (current_dir[j] == 0) {
            break;
        } else {
            i = j + 1;
        }
    }

    num_path_buttons = n;
}

mx_open_d::~mx_open_d()
{
    if (no_write_d)
        delete no_write_d;
    if (no_read_d)
        delete no_read_d;
    if (no_exist_d)
        delete no_exist_d;
    if (no_exec_d)
        delete no_exec_d;
    if (finfo_d)
        delete finfo_d;

    for (int i = 0; i < num_file_types; i++)
        delete file_type_ptr[i];
    num_file_types = 0;
}

void mx_open_d::activate_d(mx_file_class_t c, mx_file_type_t default_type)
{
    file_class = c;
    default_file_type = default_type;

    XtDestroyWidget(type_sub_menu);
    type_sub_menu = XmCreatePulldownMenu(control_area, const_cast<char*>("typePulldown"), NULL, 0);
    add_file_types();

    XtVaSetValues(type_menu, XmNsubMenuId, type_sub_menu, NULL);

    activate();
}

int mx_open_d::run(mx_file_class_t c, mx_file_type_t default_type,
    bool we_want_write)
{
    int res = 0;
    want_write = we_want_write;
    centre();
    activate_d(c, default_type);
    res = run_modal();
    deactivate();
    return res;
}

char* mx_open_d::file_name(bool& writable)
{
    writable = selected_write_perm;
    return selected_file_name;
}
