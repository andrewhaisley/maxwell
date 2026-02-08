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
#ifndef MX_SEARCH_D
#define MX_SEARCH_D
/*
 * MODULE/CLASS : mx_search_d
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
 */

#include "mx_dialog.h"
#include <Xm/Xm.h>
#include <mx_ui_object.h>

#define MAX_SEARCH_PATTERN 100

typedef enum {
    mx_search_forward_e,
    mx_search_backward_e
} mx_search_dir_t;

class mx_search_d : public mx_dialog {

public:
    mx_search_d(Widget parent);

    char search_pattern[MAX_SEARCH_PATTERN];
    bool match_case;

    mx_search_dir_t direction;

    Widget prev_button, next_button, cancel_button, pattern_text;
    Widget case_toggle;

    void add_pattern();
    void figure_options();

    int num_patterns;

private:
    Widget pattern_list;
};

#endif
