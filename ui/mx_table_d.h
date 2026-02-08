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
#ifndef MX_TABLE_D
#define MX_TABLE_D
/*
 * MODULE/CLASS : mx_table_d
 *
 * AUTHOR : Andrew Haisley
 *
 * This file
 *
 * DESCRIPTION:
 *
 *  An insert table dialog.
 *
 *
 *
 *
 */

#include "mx_dialog.h"
#include <Xm/Xm.h>
#include <mx_ui_object.h>

class mx_inform_d;

class mx_table_d : public mx_dialog {

public:
    mx_table_d(Widget parent, float width_mm, mx_unit_e display_units);

    int num_rows, num_columns;
    float column_width_mm;

    Widget ok_button, cancel_button, auto_toggle;
    Widget rows_text, columns_text, column_width_text;

    void validate_data();

    virtual void activate_d(float width_mm, mx_unit_e units);

    int run(float width_mm, mx_unit_e units);

private:
    float width_mm;
    mx_unit_e display_units;

    mx_inform_d* invalid_rows_d;
    mx_inform_d* invalid_columns_d;
    mx_inform_d* invalid_width_d;
};

#endif
