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
#ifndef MX_STYLE_D
#define MX_STYLE_D
/*
 * MODULE/CLASS : mx_style_d
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
#include <Xm/Xm.h>
#include <mx_style.h>
#include <mx_ui_object.h>

class mx_inform_d;
class mx_yes_no_d;
class mx_para_d;

class mx_style_d : public mx_dialog {

public:
    mx_style_d(Widget parent);
    ~mx_style_d();
    void activate_d(mx_paragraph_style* styles[], int* num_styles, int max_styles, mx_unit_e u);
    int run(mx_paragraph_style* styles[], int* num_styles, int max_styles, mx_unit_e u);

    Widget ok_button, cancel_button, style_name_text;
    Widget add_button, delete_button, edit_button;
    void handle_button(Widget w);
    void edit_style();
    void delete_style();
    void add_style();
    void handle_change();

    // was the list of styles edited in some way?
    bool edited;

private:
    Widget style_list;
    void fill_list(mx_paragraph_style* styles[], int n);
    mx_paragraph_style** styles;

    mx_para_d* para_d;
    mx_inform_d* too_many_d;
    mx_yes_no_d* sure_d;
    mx_unit_e units;
    int *num_styles, max_styles;
};

#endif
