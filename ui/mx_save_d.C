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
 * MODULE/CLASS : mx_save_d
 *
 * AUTHOR : Andrew Haisley
 *
 * This file
 *
 * DESCRIPTION:
 *
 *  A file browsing/saving dialog
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
#include "mx_finfo_d.h"
#include "mx_inform_d.h"
#include "mx_save_d.h"
#include "mx_yes_no_d.h"
#include <mx_file_type.h>

static void perm_button_cb(
    Widget widget,
    XtPointer client_data,
    XtPointer call_data)
{
    mx_save_d* d = (mx_save_d*)client_data;
    char temp[MAX_PATH_LEN * 2];
    struct stat fs;
    char* s = XmTextGetString(d->file_text);

    if (d->save_file_type == mx_template_e) {
        sprintf(temp, "%s/%s", d->template_dir, s);
    } else {
        strcpy(temp, s);
    }
    XtFree(s);

    if (stat(temp, &fs) < 0) {
        if (d->no_exist_d == NULL) {
            d->no_exist_d = new mx_inform_d("noExist", d->dialog, error_e);
        }
        (void)d->no_exist_d->centre();
        (void)d->no_exist_d->run_modal();
        (void)d->no_exist_d->deactivate();
    } else {
        if (d->finfo_d == NULL) {
            d->finfo_d = new mx_finfo_d("openFilePerm", d->dialog);
        }
        d->finfo_d->centre();
        d->finfo_d->activate_d(temp);
        (void)d->finfo_d->run_modal();
        d->finfo_d->deactivate();
    }
}

static void type_change(
    Widget widget,
    XtPointer client_data,
    XtPointer call_data)
{
    save_file_type_callback_t* t = (save_file_type_callback_t*)client_data;

    if (t->t != t->d->save_file_type) {
        Boolean sens = (t->t == mx_template_e) ? False : True;
        Widget label = XmOptionButtonGadget(t->d->path_menu);

        XtVaSetValues(label, XmNsensitive, sens, NULL);
        XtVaSetValues(t->d->path_menu, XmNsensitive, sens, NULL);

        bool change_dir = (t->t == mx_template_e || t->d->save_file_type == mx_template_e);

        t->d->save_file_type = t->t;
        if (change_dir)
            t->d->load_directory();
    }
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

static bool file_openable(char* s, mx_save_d* d)
{
    struct stat fs;
    FILE* f;

    strcpy(d->selected_file_name, s);

    if (stat(s, &fs) < 0) {
        f = fopen(s, "w");
        if (f != NULL) {
            fclose(f);
            unlink(s);
            return TRUE;
        } else {
            if (d->no_create_d == NULL) {
                d->no_create_d = new mx_inform_d("noCreate", d->dialog, error_e);
            }
            (void)d->no_create_d->centre();
            (void)d->no_create_d->run_modal();
            d->no_create_d->deactivate();
            return FALSE;
        }
    } else {
        f = fopen(s, "r+");
        fclose(f);
        if (f == NULL) {
            // tell them the bad news
            if (d->no_write_d == NULL) {
                d->no_write_d = new mx_inform_d("noWriteSave", d->dialog, error_e);
            }
            d->no_write_d->centre();
            (void)d->no_write_d->run_modal();
            d->no_write_d->deactivate();
            return FALSE;
        } else {
            // ask if they want to overwrite it
            if (d->overwrite_d == NULL) {
                d->overwrite_d = new mx_yes_no_d("overwriteSave", d->dialog, FALSE, yes_e);
            }
            d->overwrite_d->centre();
            if (d->overwrite_d->run_modal() == yes_e) {
                d->overwrite_d->deactivate();
                return TRUE;
            } else {
                d->overwrite_d->deactivate();
                return FALSE;
            }
        }
    }
}

static void handle_file_selection(mx_save_d* d)
{
    char* file_name;
    uid_t uid;
    gid_t gid;

    bool exec_perm;
    char s[MAX_PATH_LEN * 2];
    char temp[MAX_PATH_LEN];

    struct stat fs;

    file_name = XmTextGetString(d->file_text);
    fix_file_name(file_name, temp);
    XtFree(file_name);
    if (d->save_file_type == mx_template_e) {
        sprintf(s, "%s/%s", d->template_dir, temp);
    } else {
        strcpy(s, temp);
    }

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
        XmTextSetString(d->file_text, const_cast<char*>(""));
        d->load_directory();

        XtUnmanageChild(d->path_sub_menu);
        d->path_sub_menu = XmCreatePulldownMenu(d->control_area, const_cast<char*>("pathPulldown"), NULL, 0);
        d->set_path_menu(d->path_sub_menu);

        XtVaSetValues(d->path_menu, XmNsubMenuId, d->path_sub_menu, NULL);
    } else {
        if (file_openable(s, d)) {
            d->modal_result = yes_e;
        }
    }
}

static void button_cb(
    Widget widget,
    XtPointer client_data,
    XtPointer call_data)
{
    mx_save_d* d = (mx_save_d*)client_data;

    if (widget == d->save_button) {
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
    mx_save_d* d = (mx_save_d*)client_data;

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
            XmTextSetString(d->file_text, const_cast<char*>(""));
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
    mx_save_d* d = (mx_save_d*)client_data;

    XmTextSetString(d->file_text, const_cast<char*>(""));
    d->load_directory();
}

static void file_name_return(
    Widget widget,
    XtPointer client_data,
    XtPointer call_data)
{
    mx_save_d* d = (mx_save_d*)client_data;

    handle_file_selection(d);
}

static void file_name_changed(
    Widget widget,
    XtPointer client_data,
    XtPointer call_data)
{
    mx_save_d* d = (mx_save_d*)client_data;
    char* s;

    s = XmTextGetString(d->file_text);

    XtVaSetValues(d->perm_button, XmNsensitive,
        mx_is_blank(s) ? False : True,
        NULL);

    XtVaSetValues(d->save_button, XmNsensitive,
        mx_is_blank(s) ? False : True,
        NULL);
    XtFree(s);
}

static void select_file(Widget list, XtPointer client_data, XtPointer call_data)
{
    mx_save_d* d = (mx_save_d*)client_data;

    char* choice;

    XmListCallbackStruct* cbs = (XmListCallbackStruct*)call_data;

    if (!XmStringGetLtoR(cbs->item, const_cast<char*>("FILETAG"), &choice)) {
        if (!XmStringGetLtoR(cbs->item, const_cast<char*>("DIRTAG"), &choice)) {
            choice = nullptr;
        }
    }

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

mx_save_d::mx_save_d(
    const char* name,
    Widget parent,
    mx_file_type_t default_type,
    mx_file_class_t c,
    const char* template_dir)
    : mx_dialog(name, parent, TRUE, FALSE)
{
    Widget label1, label2, label3, refresh_button;
    Widget type_sub_menu, sep;
    Arg args[15];
    int n;

    strcpy(this->template_dir, template_dir);

    num_file_types = 0;
    no_write_d = NULL;
    no_exec_d = NULL;
    no_create_d = NULL;
    overwrite_d = NULL;
    finfo_d = NULL;
    no_exist_d = NULL;

    save_file_type = default_type;
    file_class = c;

    if (getcwd(current_dir, MAX_PATH_LEN) == nullptr) {
        return;
    }

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

    type_sub_menu = XmCreatePulldownMenu(control_area, const_cast<char*>("typePulldown"), NULL, 0);
    add_file_types(type_sub_menu);

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
        perm_button,
        XmNactivateCallback,
        perm_button_cb,
        (XtPointer)this);

    XtAddCallback(
        refresh_button,
        XmNactivateCallback,
        refresh_button_cb,
        (XtPointer)this);

    // now, the action buttons
    save_button = XtVaCreateManagedWidget(
        "save",
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
        save_button,
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

void mx_save_d::activate()
{
    if (chdir(current_dir) != 0) {
        return;
    }
    load_directory();
    mx_dialog::activate();
}

static int aka_strcmp(const void* p1, const void* p2)
{
    return strcmp(reinterpret_cast<const char*>(p1), reinterpret_cast<const char*>(p2));
}

void mx_save_d::load_directory()
{
    struct dirent* d;

    DIR* dir;
    XmString str[MAX_DIR_FILES];
    int i;

    static char file_names[MAX_DIR_FILES][MAX_FILE_NAME_LEN];
    char temp[MAX_PATH_LEN * 2];

    struct stat fs;

    num_files = 0;

    // open the directory
    if (save_file_type == mx_template_e) {
        dir = opendir(template_dir);
    } else {
        dir = opendir(current_dir);
    }

    while (TRUE) {
        d = readdir(dir);
        if (d == NULL) {
            closedir(dir);
            break;
        }

        if (save_file_type == mx_template_e) {
            sprintf(temp, "%s/%s", template_dir, d->d_name);
        } else {
            strcpy(temp, d->d_name);
        }

        if (stat(temp, &fs) < 0) {
            continue;
        }

        if ((!S_ISDIR(fs.st_mode)) && (!S_ISREG(fs.st_mode))) {
            continue;
        }

        if ((save_file_type == mx_template_e) && S_ISDIR(fs.st_mode)) {
            continue;
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
        if (save_file_type == mx_template_e) {
            sprintf(temp, "%s/%s", template_dir, file_names[i]);
            stat(temp, &fs);
        } else {
            stat(file_names[i], &fs);
        }

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

void mx_save_d::add_file_types(Widget w)
{
    int i;
    Widget tb;

    save_file_type_callback_t* t;

    for (i = 0; i < num_file_types; i++) {
        delete file_type_ptr[i];
    }
    num_file_types = 0;

    tb = XtVaCreateManagedWidget(
        mx_file_type_name(save_file_type),
        xmPushButtonGadgetClass,
        w,
        NULL);

    file_type_ptr[num_file_types++] = t = new save_file_type_callback_t;
    t->d = this;
    t->t = save_file_type;

    XtAddCallback(tb, XmNactivateCallback, type_change, t);

    for (i = mx_document_file_e; i <= mx_other_file_e; i++) {
        if ((mx_file_type_t)i != save_file_type) {
            if (is_type_in_class((mx_file_type_t)i, file_class)) {
                tb = XtVaCreateManagedWidget(
                    mx_file_type_name((mx_file_type_t)i),
                    xmPushButtonGadgetClass,
                    w,
                    NULL);

                file_type_ptr[num_file_types++] = t = new save_file_type_callback_t;
                t->d = this;
                t->t = (mx_file_type_t)i;
                XtAddCallback(tb, XmNactivateCallback, type_change, t);
            }
        }
    }
}

void mx_save_d::set_path_menu(Widget w)
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

mx_save_d::~mx_save_d()
{
    if (no_write_d)
        delete no_write_d;
    if (no_exec_d)
        delete no_exec_d;
    if (no_create_d)
        delete no_create_d;
    if (overwrite_d)
        delete overwrite_d;
    if (finfo_d)
        delete finfo_d;
    if (no_exist_d)
        delete no_exist_d;

    for (int i = 0; i < num_file_types; i++)
        delete file_type_ptr[i];
    num_file_types = 0;
}

char* mx_save_d::file_name()
{
    return selected_file_name;
}
