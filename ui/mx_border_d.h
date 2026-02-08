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
#ifndef MX_BORDER_D
#define MX_BORDER_D
/*
 * MODULE/CLASS : mx_border_d
 *
 * AUTHOR : Andrew Haisley
 *
 * This file
 *
 * DESCRIPTION:
 *
 *  A border style dialog
 *
 *
 *
 *
 */

#include "mx_dialog.h"
#include <Xm/Xm.h>
#include <mx_line_style.h>
#include <mx_ui_object.h>

class mx_border_style;
class mx_border_style_mod;
class mx_nlist;

class mx_border_d : public mx_dialog {
public:
    mx_border_d(Widget parent, bool has_apply = TRUE);
    ~mx_border_d();

    // pass a 2D array of pointers to border_style objects, of size
    // num_style_rows * num_style_columns
    void activate_d(mx_nlist& initial_styles,
        mx_unit_e u, mx_border_style_mod& result);

    int run(mx_nlist& initial_styles,
        mx_unit_e u, mx_border_style_mod& result);

    Widget ok_button, apply_button, cancel_button;
    Widget dashed_button, dotted_button, solid_button;

    void draw_sample();
    void set_line_colour(Widget w);
    void set_bg_colour(Widget w);
    void get_options();
    void get_initial_mod(mx_nlist& border_styles);

    void handle_input(XEvent* e);

    mx_line_type_t line_type;

private:
    mx_border_style_mod* result;

    Widget size_text, size_list;
    Widget bg_colour_buttons[MX_NUM_COLOURS + 1];
    Widget line_colour_buttons[MX_NUM_COLOURS];

    Widget drawing_area;

    Widget distance_text, distance_list;

    void set_options();
    void set_size();
    void set_type();
    void set_type_menu(Widget w);

    void set_line_colour_menu(Widget w);
    void set_bg_colour_menu(Widget w);

    void fill_list(Widget w);
    void draw_line(
        Display* display,
        Drawable drawable,
        mx_line_style style,
        bool ind,
        int x1,
        int y1,
        int x2,
        int y2);

    GC gc;

    int x1, x2, y1, y2;
    int xmid, ymid;
    int last_x, last_y;
    bool has_horizontal, has_vertical;

    int line_pos(int x, int y);
    mx_line_style line_style();
    void toggle_style(mx_line_style& style, mx_line_style new_style, bool ind);
    mx_unit_e units;

    const char* current_colour_name;
};

#endif
