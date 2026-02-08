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
#ifndef MX_COLUMN_D
#define MX_COLUMN_D
/*
 * MODULE/CLASS : mx_column_d
 *
 * AUTHOR : Andrew Haisley
 *
 * This file
 *
 * DESCRIPTION:
 *
 *  Formatting columns of text
 *
 *
 *
 */

#include "mx_dialog.h"
#include "mx_valid_d.h"
#include <Xm/Xm.h>
#include <mx_ui_object.h>

class mx_column_d : public mx_valid_d {

public:
    mx_column_d(Widget parent);

    void activate_d(int num_columns, float column_space, mx_unit_e u);

    int num_columns;
    float column_space;

    Widget ok_button, apply_button, cancel_button;

    void set_options();
    bool figure_options();
    void fill_list(Widget w);

    Widget num_columns_text;
    Widget space_between_text;

    int run(int num_columns, float column_space, mx_unit_e u);

private:
    mx_unit_e units;
};

#endif
