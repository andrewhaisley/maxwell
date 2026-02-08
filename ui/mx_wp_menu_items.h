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

#ifndef MX_WP_MENU_ITEMS_H
#define MX_WP_MENU_ITEMS_H

// these are codes for menu items whose visibility may be changed
typedef enum {
    mx_file_new_e,
    mx_file_open_e,
    mx_file_close_e,
    mx_file_save_e,
    mx_file_save_as_e,
    mx_file_print_e,
    mx_file_print_one_e,
    mx_edit_undo_e,
    mx_edit_redo_e,
    mx_edit_cut_e,
    mx_edit_copy_e,
    mx_edit_paste_e,
    mx_edit_select_all_e,
    mx_edit_find_e,
    mx_edit_find_again_e,
    mx_edit_replace_e,
    mx_edit_goto_e,
    mx_edit_goto_start_e,
    mx_edit_goto_end_e,
    mx_table_delete_rows_e,
    mx_table_delete_columns_e,
    mx_table_rows_e,
    mx_table_columns_e,
    mx_view_outline_e,
    mx_view_header_e,
    mx_view_footer_e,
    mx_view_styles_e,
    mx_view_table_guides_e,
    mx_view_tools_e,
    mx_view_ruler_e,
    mx_view_zoom_e,
    mx_insert_break_e,
    mx_insert_page_number_e,
    mx_insert_date_time_e,
    mx_insert_symbol_e,
    mx_insert_bullets_e,
    mx_insert_image_e,
    mx_insert_diagram_e,
    mx_insert_table_e,
    mx_insert_file_e,
    mx_insert_toc_e,
    mx_insert_index_e,
    mx_insert_index_entry_e,
    mx_format_paragraph_e,
    mx_format_character_e,
    mx_format_border_e,
    mx_format_tabs_e,
    mx_format_style_e,
    mx_format_page_setup_e,
    mx_format_columns_e,
    mx_format_table_e,
    mx_format_table_style_e,
    mx_format_make_normal_e,
    mx_format_make_bold_e,
    mx_format_make_italic_e,
    mx_format_make_underline_e,
    mx_tools_spelling_e,
    mx_tools_repaginate_e,
    mx_tools_options_e,
    mx_tools_envelope_e,
    mx_help_help_index_e,
    mx_help_product_support_e,
    mx_help_about_e,

    // this item is used to terminate vararg lists
    mx_end_item_e
} menu_item_name_t;

#endif
