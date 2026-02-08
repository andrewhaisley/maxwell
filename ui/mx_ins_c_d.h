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
#ifndef MX_INS_C_D
#define MX_INS_C_D
/*
 * MODULE/CLASS : mx_ins_c_d
 *
 * AUTHOR : Andrew Haisley
 *
 * This file
 *
 * DESCRIPTION:
 *
 *  Inserting columns into a table
 *
 *
 *
 */

#include "mx_dialog.h"
#include <Xm/Xm.h>
#include <mx_ui_object.h>

class mx_inform_d;

class mx_ins_c_d : public mx_dialog {

public:
    mx_ins_c_d(Widget parent);
    ~mx_ins_c_d();

    int num_columns;
    bool insert_to_left; // TRUE->left, FALSE->right

    Widget ok_left_button, ok_right_button, cancel_button;

    bool figure_options();
    void fill_list(Widget w);

    Widget num_columns_text;
    mx_inform_d* bad_number_d;

private:
};

#endif
