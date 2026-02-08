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
#ifndef MX_REPLACE_D
#define MX_REPLACE_D
/*
 * MODULE/CLASS : mx_replace_d
 *
 * AUTHOR : Andrew Haisley
 *
 * This file
 *
 * DESCRIPTION:
 *
 *  An search dialog.
 *
 *
 *
 *
 */

#include "mx_dialog.h"
#include "mx_search_d.h"
#include <Xm/Xm.h>
#include <mx_ui_object.h>

#define MAX_REPLACE_PATTERN 100

class mx_replace_d : public mx_dialog {

public:
    mx_replace_d(Widget parent);

    char search_pattern[MAX_REPLACE_PATTERN];
    char replace_string[MAX_REPLACE_PATTERN];
    bool match_case;

    mx_search_dir_t direction;
    bool replace_all;

    Widget prev_button, next_button, cancel_button, pattern_text;
    Widget case_toggle, replace_text, all_button;

    void add_pattern();
    void figure_options();

    int num_patterns;

private:
    Widget pattern_list;
};

#endif
