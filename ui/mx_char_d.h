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
#ifndef MX_CHAR_D
#define MX_CHAR_D
/*
 * MODULE/CLASS : mx_char_d
 *
 * AUTHOR : Andrew Haisley
 *
 * This file
 *
 * DESCRIPTION:
 *
 *  A character style dialog
 *
 *
 *
 */

#include "mx_dialog.h"
#include <Xm/Xm.h>
#include <mx_style.h>
#include <mx_ui_object.h>

class mx_char_d : public mx_dialog {
public:
    mx_char_d(Widget parent, bool has_apply = TRUE);

    int run(mx_char_style* initial_styles[], int num_styles, mx_char_style_mod& result_mod);
    void activate_d(mx_char_style* initial_styles[], int num_styles, mx_char_style_mod& result_mod);

private:
    void set_font(char* name);
    void set_size();
    void set_style();
    void set_spacing();
    void draw_sample();
    void set_colour();
    void set_align();
    void set_alignment_offset();

    static void drawing_area_cb(Widget, XtPointer, XtPointer);
    static void alignment_offset_return_cb(Widget, XtPointer, XtPointer);
    static void size_return_cb(Widget, XtPointer, XtPointer);
    static void spacing_return_cb(Widget, XtPointer, XtPointer);
    static void style_change_cb(Widget, XtPointer, XtPointer);
    static void align_change_cb(Widget, XtPointer, XtPointer);
    static void colour_cb(Widget, XtPointer, XtPointer);
    static void button_cb(Widget, XtPointer, XtPointer);
    static void list_cb(Widget, XtPointer, XtPointer);

    void fill_list(Widget w);
    void set_combo_lists();
    void set_options(mx_char_style_mod& diffs);

    Widget ok_button, apply_button, cancel_button;
    Widget bold_toggle, italic_toggle, underline_toggle;
    Widget normal_toggle, superscript_toggle, subscript_toggle;
    Widget size_text, line_spacing_text, char_spacing_text;
    Widget colour_buttons[MX_NUM_COLOURS];
    Widget drawing_area, font_list;
    Widget size_list, line_spacing_list, char_spacing_list;
    Widget colour_rc, align_radio_box;
    Widget alignment_offset_text, alignment_offset_list;

    GC gc;

    mx_char_style* base_style;
    mx_char_style_mod* base_diffs;
    mx_char_style_mod* result;
};

#endif
