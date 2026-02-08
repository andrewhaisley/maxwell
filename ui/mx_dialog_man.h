#pragma once

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

#include <map>
#include <string>

#include <mx.h>
#include <mx_inform_d.h>
#include <mx_ui_object.h>

// forward definitions
class mx_font;
class mx_dialog;
class mx_about_d;
class mx_open_d;
class mx_save_d;
class mx_quit_d;
class mx_yes_no_d;
class mx_new_d;
class mx_print_d;
class mx_search_d;
class mx_replace_d;
class mx_goto_d;
class mx_zoom_d;
class mx_symbol_d;
class mx_table_d;
class mx_toc_d;
class mx_index_d;
class mx_char_d;
class mx_para_d;
class mx_tabf_d;
class mx_border_d;
class mx_tab_d;
class mx_style_d;
class mx_date_d;
class mx_page_d;
class mx_column_d;
class mx_ins_c_d;
class mx_ins_r_d;
class mx_cell_d;
class mx_spell_d;
class mx_env_d;
class mx_opt_d;
class mx_support_d;
class mx_hsearch_d;
class mx_prog_d;
class mx_locked_d;

typedef enum {
    mx_none_e = 0,
    mx_confirm_yes_e = 1,
    mx_confirm_no_e = 2
} mx_confirm_t;

class mx_dialog_manager : public mx_ui_object {

public:
    mx_dialog_manager();
    ~mx_dialog_manager();

    // set the parent widget to be used for dialogs.
    void set_parent(Widget w);

    // some dialogs aren't parameterised, these first
    mx_quit_d* get_quit_d();
    mx_about_d* get_about_d();

    // information dialogs, have a type and some text
    mx_inform_d* get_inform_d(mx_inform_d_t type, const char* name);

    // run an information dialog now - return when user finished
    void run_inform_d(mx_inform_d_t type, const char* name);

    // yes/no dialogs have text
    mx_yes_no_d* get_yes_no_d(
        const char* name,
        bool cancel_button,
        mx_dialog_result_t default_button);

    // run a yes/no dialog now
    mx_dialog_result_t run_yes_no_d(
        const char* name,
        bool cancel_button,
        mx_dialog_result_t default_button);

    // file open dialog
    mx_open_d* get_open_d();

    // file save dialog
    mx_save_d* get_save_d(const char* template_dir);

    // new document dialog
    mx_new_d* get_new_d(const char* template_dir);

    // print dialog
    mx_print_d* get_print_d();

    // search dialog
    mx_search_d* get_search_d();

    // search/replace dialog
    mx_replace_d* get_replace_d();

    // goto page dialog
    mx_goto_d* get_goto_d();

    // zoom dialog
    mx_zoom_d* get_zoom_d();

    // symbol dialog
    mx_symbol_d* get_symbol_d(mx_font* f);

    // insert table dialog
    mx_table_d* get_table_d(float width_mm, mx_unit_e display_units);

    // table of contents insert dialog
    mx_toc_d* get_toc_d();

    // index insert dialog
    mx_index_d* get_index_d();

    // character format dialog
    mx_char_d* get_char_d();

    // paragraph format dialog
    mx_para_d* get_para_d();

    // table paragraph format dialog
    mx_tabf_d* get_tabf_d();

    // border format dialog
    mx_border_d* get_border_d();

    // tab format dialog
    mx_tab_d* get_tab_d();

    // style dialog
    mx_style_d* get_style_d();

    // date/time dialog
    mx_date_d* get_date_d();

    // page setup
    mx_page_d* get_page_d();

    // columns of text
    mx_column_d* get_column_d();

    // insert columns into a table
    mx_ins_c_d* get_ins_c_d();

    // delete columns from a table
    mx_ins_r_d* get_ins_r_d();

    // format table cells
    mx_cell_d* get_cell_d();

    // spell checker
    mx_spell_d* get_spell_d(const char* language);

    // envelope formatter
    mx_env_d* get_env_d();

    // options
    mx_opt_d* get_opt_d();

    // support info
    mx_support_d* get_support_d();

    // progress dialog with no bar
    mx_prog_d* get_prog_d();

    // progress dialog with bar
    mx_prog_d* get_prog_bar_d();

    // reset the progress bar dialogs
    void reset_prog_d();

    // run an immediate information dialog with dynamic text
    void run_inform_now(mx_inform_d_t type, const char* message);

    // run an immediate confirm dialog with dynamic text
    mx_dialog_result_t run_confirm_now(const char* message);

    // run an immediate file locked dialog with dynamic text
    void run_locked_now(const char* file, const char* locked_by, const char* locked_host);

private:
    void centre_widget(Widget w);

    static void button_cb(Widget widget,
        XtPointer client_data,
        XtPointer call_data);

    static mx_dialog_result_t mess_result;

    mx_open_d* open_d;
    mx_save_d* save_d;
    mx_quit_d* quit_d;
    mx_about_d* about_d;
    mx_new_d* new_d;
    mx_print_d* print_d;
    mx_search_d* search_d;
    mx_replace_d* replace_d;
    mx_goto_d* goto_d;
    mx_zoom_d* zoom_d;
    mx_symbol_d* symbol_d;
    mx_table_d* table_d;
    mx_toc_d* toc_d;
    mx_index_d* index_d;
    mx_char_d* char_d;
    mx_para_d* para_d;
    mx_tabf_d* tabf_d;
    mx_border_d* border_d;
    mx_tab_d* tab_d;
    mx_style_d* style_d;
    mx_date_d* date_d;
    mx_page_d* page_d;
    mx_column_d* column_d;
    mx_ins_c_d* ins_c_d;
    mx_ins_r_d* ins_r_d;
    mx_cell_d* cell_d;
    mx_spell_d* spell_d;
    mx_env_d* env_d;
    mx_opt_d* opt_d;
    mx_support_d* support_d;
    mx_prog_d* prog_d;
    mx_prog_d* prog_bar_d;
    mx_locked_d* locked_d;

    std::map<std::string, mx_yes_no_d *> m_yes_no_d_hash;
    std::map<std::string, mx_inform_d *> m_inform_d_hash;

    Widget parent;
};
