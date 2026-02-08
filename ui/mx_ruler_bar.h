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
/*
 * MODULE/CLASS :  mx_ruler_bar
 *
 * AUTHOR : Andrew Haisley
 *
 *
 *
 * DESCRIPTION:
 *
 * A class for displaying end editing rulers
 *
 *
 *
 *
 *
 */
#ifndef MX_RULER_BAR_H
#define MX_RULER_BAR_H

#include <Intrinsic.h>
#include <mx.h>
#include <mx_ftarget.h>
#include <mx_ruler.h>
#include <mx_unit.h>

class mx_wp_toolbar;
class mx_xframe;
class mx_screen_device;
class mx_font;
class mx_char_style;

#define TAB_TOLERANCE 6.0

#define MX_MAX_NUM_DISPLAY_RULERS 100

class mx_ruler_bar : public mx_frame_target {
    // MX_RTTI(mx_ruler_bar_class_e

public:
    mx_ruler_bar(Widget parent, mx_wp_toolbar* notify);
    virtual ~mx_ruler_bar();

    Widget get_widget();

    void display(int& err, mx_ruler* rulers[], int num_rulers, float resolution, mx_unit_e units, float paper_width);
    void display(int& err, float resolution, mx_unit_e units);

    void set_left_origin(int& err, float l);
    inline float get_left_origin() { return left / resolution; };

    inline mx_xframe* get_frame() { return ruler_frame; };

    void set_sensitive(int& err, bool b);

private:
    // callbacks for the ruler frame
    virtual void draw(int& err, mx_draw_event& event);
    virtual void buttonPress(int& err, mx_button_event& event);

    mx_tabstop_type_t current_tab_type;

    mx_xframe* ruler_frame;
    mx_screen_device* ruler_device;
    float resolution;
    float left;
    mx_unit_e units;
    float paper_width;

    int num_rulers;
    mx_ruler* rulers[MX_MAX_NUM_DISPLAY_RULERS];

    void draw_tab(int& err, mx_tabstop_type_t t, float x, float y);
    void erase_tab(int& err, mx_tabstop_type_t t, float x, float y);

    void draw_basic_ruler(int& err);
    void draw_whole_ruler(int& err);

    void draw_left_indent(int& err, mx_ruler* ruler);
    void draw_right_indent(int& err, mx_ruler* ruler);
    void draw_first_line_indent(int& err, mx_ruler* ruler);
    void draw_indents(int& err);
    void draw_tabs(int& err);
    void draw_ticks(int& err);
    void draw_mm_ticks(int& err);
    void draw_inch_ticks(int& err);
    void draw_point_ticks(int& err);
    void draw_tab_choice(int& err);

    static mx_font* font;
    static mx_char_style* cs;

    mx_tabstop_type_t tab_type;
    mx_wp_toolbar* owner_bar;

    bool sensitive;
};

#endif
