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
#ifndef MX_TAB_D
#define MX_TAB_D
/*
 * MODULE/CLASS : mx_tab_d
 *
 * AUTHOR : Andrew Haisley
 *
 * This file
 *
 * DESCRIPTION:
 *
 *  A paragraph style dialog
 *
 *
 *
 */

#include "mx_dialog.h"
#include <Xm/Xm.h>
#include <mx_style.h>
#include <mx_ui_object.h>

class mx_yes_no_d;

class mx_tab_d : public mx_dialog {
public:
    mx_tab_d(Widget parent, bool has_apply = TRUE);
    ~mx_tab_d();

    void activate_d(mx_ruler& r, mx_unit_e u);

    int run(mx_ruler& r, mx_unit_e u);

    mx_ruler ruler;
    mx_unit_e units;

    Widget ok_button, apply_button, cancel_button;

    void handle_button(Widget w);
    void handle_toggle(Widget w);
    void handle_tab_toggle(Widget w);

    void set_options();
    void figure_options();
    void fill_list();

    void delete_tab();
    void add_tab();

    // update the toggle buttons and text field with a list item number
    void update_toggles_and_position(uint32 tab_num);

    // update all the toggle buttons with a tab position.
    void update_toggles(float f);

    // in addition update the position text field
    void update_toggles_and_position(float f);

    // used by the set_position_text callback to check whether the position
    // text had just been set by the system
    bool position_set;

private:
    // set the current_tab according to a position
    void find_tab(float f);

    // update the toggles according to the current_tab
    void update_toggles();
    void update_toggles_and_position();

    Widget position_text;

    Widget user_space_toggle;
    Widget default_space_toggle;
    Widget default_space_text;
    Widget tab_list;
    Widget left_toggle;
    Widget right_toggle;
    Widget centre_toggle;
    Widget dp_toggle;
    Widget add_button, delete_button;
    Widget type_rc, tab_type_rc, tab_lead_type_rc;

    Widget no_lead_toggle, dot_lead_toggle, ul_lead_toggle;
    Widget eq_lead_toggle, hyp_lead_toggle;

    mx_yes_no_d* confirm_d;

    mx_tabstop new_tab;
    mx_tabstop* current_tab;
};

#endif
