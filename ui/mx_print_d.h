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
#ifndef MX_PRINT_D
#define MX_PRINT_D
/*
 * MODULE/CLASS : mx_print_d
 *
 * AUTHOR : Andrew Haisley
 *
 * This file
 *
 * DESCRIPTION:
 *
 *  An new document dialog.
 *
 *
 *
 *
 */

#include "mx_dialog.h"
#include "mx_printcap.h"
#include "mx_save_d.h"
#include <Xm/Xm.h>

#define MX_PRINT_MAX_PAGES 100
#define MX_MAX_PRINTERS 50

class mx_print_d : public mx_dialog, public mx_printcap {

public:
    ~mx_print_d();
    mx_print_d(Widget parent);

    // which printer was selected
    char selected_printer[MAX_PATH_LEN];

    // if to file, this is the name
    char selected_file[MAX_PATH_LEN];

    typedef enum {
        print_all_e,
        print_range_e,
        print_current_e,
        print_list_e
    } mx_print_range_t;

    // what type of page selection
    mx_print_range_t range;

    // for range selection, this is first an last inclusive
    int first_page;
    int last_page;

    // for list selection, these are the numbers
    int num_pages;
    int pages_to_print[MX_PRINT_MAX_PAGES];

    typedef enum {
        print_both_e,
        print_odd_e,
        print_even_e
    } mx_print_parity_t;

    // whether to print odd, even or all
    mx_print_parity_t parity;

    int num_copies;

    // true -> print all page 1s together, all pages 2s etc
    bool collate;

    // true -> print to file
    bool to_file;

    // printer resolution
    int x_res;
    int y_res;

    // true->include font files in file/print job
    bool include_fonts;

    void activate_d(
        int num_pages,
        char* default_printer,
        int default_x_res,
        int default_y_res,
        bool options_sensitive = TRUE);

    virtual int run(
        int num_pages,
        char* default_printer,
        int default_x_res,
        int default_y_res,
        bool options_sensitive = TRUE);

    Widget print_button, cancel_button;
    Widget print_range_radio, all_button, current_button;
    Widget range_button, list_button;
    Widget start_text, end_text, list_text;

    Widget to_file_button, collate_button, include_fonts_button;
    Widget parity_radio, num_copies_text;
    Widget parity_all_button, parity_odd_button, parity_even_button;

    bool find_options();
    bool get_output_file();

    mx_save_d* save_d;

    mx_inform_d* bad_copies_d;
    mx_inform_d* bad_first_page_d;
    mx_inform_d* bad_last_page_d;
    mx_inform_d* bad_x_res_d;
    mx_inform_d* bad_y_res_d;

private:
    void fill_list(Widget w);
    void fill_res_list(Widget w);
    void get_print_list(char* s);

    Widget x_res_text, x_res_list;
    Widget y_res_text, y_res_list;
    Widget list;
};

#endif
