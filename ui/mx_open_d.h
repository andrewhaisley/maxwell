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
#ifndef MX_OPEN_D
#define MX_OPEN_D
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

#include "mx_dialog.h"
#include <Xm/Xm.h>
#include <mx.h>
#include <mx_file_type.h>
#include <mx_ui_object.h>

class mx_inform_d;
class mx_yes_no_d;
class mx_finfo_d;

#define MX_FILE_OPEN_MAX_DIR_DEPTH 100

class mx_open_d;

typedef struct
{
    mx_file_type_t t;
    mx_open_d* d;
} open_file_type_callback_t;

class mx_open_d : public mx_dialog {

public:
    ~mx_open_d();

    mx_open_d(
        const char* name,
        Widget parent,
        mx_file_type_t default_type = mx_document_file_e,
        mx_file_class_t c = mx_all_file_class_e,
        bool want_write_permission = TRUE);

    // call this after run_modal to find the name of the
    // file selected. The file will have at least read
    // permission
    char* file_name(bool& writable);

    virtual void activate();
    virtual void activate_d(mx_file_class_t c, mx_file_type_t default_type);

    virtual int run(mx_file_class_t c, mx_file_type_t default_type,
        bool we_want_write);

    Widget open_button, cancel_button, file_list;
    Widget type_menu, path_menu, path_sub_menu, file_text, perm_button;

    bool want_write;

    mx_yes_no_d* no_write_d;
    mx_inform_d* no_read_d;
    mx_inform_d* no_exist_d;
    mx_inform_d* no_exec_d;
    mx_finfo_d* finfo_d;

    bool dir_loaded;
    void load_directory();

    char current_dir[MAX_PATH_LEN];
    char selected_file_name[MAX_PATH_LEN];
    bool selected_write_perm;

    mx_file_type_t default_file_type;

    Widget path_buttons[MX_FILE_OPEN_MAX_DIR_DEPTH];
    int path_indexes[MX_FILE_OPEN_MAX_DIR_DEPTH];
    int num_path_buttons;

    void set_path_menu(Widget w);

private:
    void add_file_types();

    int num_files;
    mx_file_class_t file_class;
    Widget file_type_buttons[MX_FILE_TYPE_NUM_TYPES];
    int num_file_type_buttons;
    Widget type_sub_menu;

    int num_file_types;
    open_file_type_callback_t* file_type_ptr[MX_FILE_TYPE_NUM_TYPES];
};

#endif
