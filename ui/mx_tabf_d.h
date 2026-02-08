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
#ifndef MX_TABF_D
#define MX_TABF_D
/*
 * MODULE/CLASS : mx_tabf_d
 *
 * AUTHOR : Andrew Haisley
 *
 *
 *
 * DESCRIPTION:
 *
 *  A table paragraph style dialog
 *
 *
 *
 */

#include "mx_dialog.h"
#include <Xm/Xm.h>
#include <mx_colour.h>
#include <mx_ui_object.h>

class mx_char_d;
class mx_border_d;
class mx_tab_d;
class mx_paragraph_style;
class mx_paragraph_style_mod;

class mx_tabf_d : public mx_dialog {
public:
    mx_tabf_d(Widget parent, bool has_apply = TRUE);
    ~mx_tabf_d();

    void activate_d(mx_paragraph_style* s[], int num_styles, mx_unit_e u,
        mx_paragraph_style_mod& result);

    int run(mx_paragraph_style* s[], int num_styles, mx_unit_e u,
        mx_paragraph_style_mod& result);

    Widget ok_button, apply_button, cancel_button;
    void handle_button(Widget w);

    void figure_options();
    void set_options();

private:
    int num_styles;
    mx_paragraph_style** selected_styles;
    mx_paragraph_style_mod* result;

    mx_paragraph_style* base_style;
    mx_paragraph_style_mod* style_mod;

    void fill_list(Widget w);
    void fill_space_list(Widget w);

    Widget left_justify_toggle;
    Widget right_justify_toggle;
    Widget full_justify_toggle;
    Widget centre_justify_toggle;

    Widget keep_together_toggle;
    Widget keep_with_next_toggle;

    Widget space_before_text;
    Widget space_after_text;

    Widget left_indent_text;
    Widget right_indent_text;

    Widget break_before_toggle;

    Widget justification_rc;

    mx_unit_e units;
};

#endif
