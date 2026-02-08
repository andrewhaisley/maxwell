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
#ifndef MX_WP_TOOLBAR_H
#define MX_WP_TOOLBAR_H
/*
 * MODULE/CLASS : mx_wp_toolbar
 *
 * AUTHOR : Andrew Haisley
 *
 *
 *
 * DESCRIPTION:
 *   The word process tool bar
 *
 *
 *
 *
 */

#include "mx_bar.h"
#include "mx_ruler_bar.h"
#include <Xm/Xm.h>
#include <mx.h>
#include <mx_style.h>
#include <mx_window.h>
#include <mx_wp_doc.h>
#include <mx_wp_menu_items.h>

class mx_wp_editor;
class mx_paragraph_style;

class mx_wp_toolbar : public mx_bar, public mx_frame_target {
    MX_RTTI(mx_wp_toolbar_class_e)

public:
    mx_wp_toolbar(mx_window* window);
    ~mx_wp_toolbar();

    // handle a button press
    void handle_button(Widget w);

    // handle the arm callback
    void handle_arm(Widget w);

    // handle the disarm callback
    void handle_disarm();

    // change the visibility of the style editing buttons and things
    void view_styles(bool s);

    // visibility of the menu item buttons (e.g. save, load etc)
    void view_actions(bool s);

    // visibility of the ruler
    void view_ruler(bool s);

    void set_style_pulldown(mx_document* d);

    // call this when a selection gets made to set the buttons, pulldowns
    // etc
    void set_current_style(mx_paragraph_style* s[], int ns);

    // list is terminated with end_item_e
    void set_visible(menu_item_name_t item, ...);
    void set_invisible(menu_item_name_t item, ...);

    void set_item_visible(menu_item_name_t item, bool s);

    mx_ruler_bar* get_ruler_bar() { return ruler_bar; };

private:
    mx_ruler_bar* ruler_bar;

    // the current paragraph style being displayed
    mx_paragraph_style* current_ps;

    // whether or not to update all of the style fields
    bool update_ps;

    // set a single paragraph style, updating only those fields that require
    // updating
    void set_current_style(mx_paragraph_style& ps);

    // setting the states of buttons on selection of some text etc
    void set_bold_toggle(Boolean b);
    void set_italic_toggle(Boolean b);
    void set_underline_toggle(Boolean b);

    // family name may be NULL (for indeterminate)
    void set_current_font_family(const char* family_name);

    void set_superscript_toggles(mx_align_t a);

    // size may be 0 (for indeterminate)
    void set_current_font_size(float s);

    // may be NULL for indeterminate
    void set_current_style_name(const char* s);

    // justification
    void set_justification(mx_justification_t j, bool is_indeterminate);

    void fill_font_pulldown(Widget list);
    void fill_size_pulldown(Widget list);

    void font_family_change(int& err, mx_wp_editor* ed);
    void style_change(int& err, mx_wp_editor* ed);
    void font_size_change(int& err, mx_wp_editor* ed);
    void justify_change(int& err, mx_wp_editor* ed, Widget w);
    void align_change(int& err, mx_wp_editor* ed, Widget which);
    void bold_change(int& err, mx_wp_editor* ed);
    void italic_change(int& err, mx_wp_editor* ed);
    void underline_change(int& err, mx_wp_editor* ed);

    int num_style_names;
    char* style_names[MX_DOCUMENT_MAX_STYLES];

    void set_string(Widget w, const char* s);

    Widget quit_button, new_button, open_button, save_button;
    Widget print_button, cut_button, copy_button, paste_button;
    Widget find_button, replace_button, goto_button, header_button;
    Widget insert_image_button, insert_diagram_button, insert_table_button;
    Widget insert_bullets_button, insert_symbol_button;
    Widget insert_page_break_button, format_character_button;
    Widget format_paragraph_button, format_columns_button;
    Widget format_table_button, spelling_button, envelope_button;
    Widget style_form, menu_form, container_form, bold_button;
    Widget italic_button, underline_button, superscript_button;
    Widget subscript_button, justify_button, left_justify_button;
    Widget right_justify_button, centre_justify_button, font_list;
    Widget font_text, size_list, size_text, style_list, style_text;
    Widget para_style_radiobox, footer_button;

    static void activate_cb(Widget, XtPointer, XtPointer);
    static void arm_cb(Widget, XtPointer, XtPointer);
    static void disarm_cb(Widget, XtPointer, XtPointer);
};

#endif // MX_WP_TOOLBAR_H
