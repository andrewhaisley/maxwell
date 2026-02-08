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
#ifndef MX_DATE_D
#define MX_DATE_D
/*
 * MODULE/CLASS : mx_date_d
 *
 * AUTHOR : Andrew Haisley
 *
 * This file
 *
 * DESCRIPTION:
 *
 *  A date time field dialog
 *
 *
 *
 */

#include "mx_dialog.h"
#include <Xm/Xm.h>
#include <mx_field_w.h>
#include <mx_ui_object.h>

class mx_date_d : public mx_dialog {

public:
    mx_date_d(Widget parent);

    mx_field_word::mx_field_dt_format_t date_format;
    mx_field_word::mx_field_dt_format_t time_format;

    Widget ok_button, cancel_button;

    void figure_options();

private:
    Widget date_list, time_list;
    void fill_lists();
};

#endif
