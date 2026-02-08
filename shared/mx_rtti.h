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
 * MODULE/CLASS : mx_rtti
 *
 * AUTHOR : Andrew Haisley
 *
 *
 *
 * DESCRIPTION:
 *
 * Simple run time type identification
 *
 *
 *
 *
 */
#ifndef MX_RTTI_H
#define MX_RTTI_H

#include <mx.h>

#define MX_RTTI(x)                             \
public:                                        \
    virtual bool is_a(mx_rtti_class_e c) const \
    {                                          \
        return c == x;                         \
    }                                          \
    virtual bool is_a(const mx_rtti& o) const  \
    {                                          \
        return x == o.rtti_class();            \
    }                                          \
    virtual mx_rtti_class_e rtti_class() const \
    {                                          \
        return x;                              \
    }

enum mx_rtti_class_e {
    mx_area_class_e,
    mx_area_cursor_class_e,
    mx_area_cw_t_class_e,
    mx_area_para_iterator_class_e,
    mx_area_pos_class_e,
    mx_ascii_importer_class_e,
    mx_attribute_class_e,
    mx_bar_class_e,
    mx_block_class_e,
    mx_border_style_class_e,
    mx_border_style_mod_class_e,
    mx_break_word_class_e,
    mx_buffer_class_e,
    mx_char_style_class_e,
    mx_char_style_mod_class_e,
    mx_character_cw_t_class_e,
    mx_colour_class_e,
    mx_complex_word_class_e,
    mx_cursor_class_e,
    mx_cw_iter_class_e,
    mx_db_object_class_e,
    mx_device_class_e,
    mx_diagram_area_class_e,
    mx_diagram_area_painter_class_e,
    mx_doc_polypoint_class_e,
    mx_doc_pos_class_e,
    mx_doc_rect_class_e,
    mx_document_class_e,
    mx_document_painter_class_e,
    mx_editor_class_e,
    mx_field_word_class_e,
    mx_field_word_cw_t_class_e,
    mx_file_class_e,
    mx_sfile_class_e,
    mx_fill_style_class_e,
    mx_font_class_e,
    mx_font_mod_class_e,
    mx_grid_paint_class_e,
    mx_image_area_class_e,
    mx_image_area_painter_class_e,
    mx_importer_class_e,
    mx_line_style_class_e,
    mx_list_class_e,
    mx_list_iterator_class_e,
    mx_log_class_e,
    mx_maxwell_importer_class_e,
    mx_menu_class_e,
    mx_menu_item_class_e,
    mx_menubar_class_e,
    mx_nlist_class_e,
    mx_nlist_iterator_class_e,
    mx_opaque_area_cursor_class_e,
    mx_opaque_area_pos_class_e,
    mx_paintable_class_e,
    mx_painter_class_e,
    mx_para_pos_class_e,
    mx_paragraph_class_e,
    mx_paragraph_style_class_e,
    mx_paragraph_style_mod_class_e,
    mx_position_class_e,
    mx_printer_device_class_e,
    mx_rtf_importer_class_e,
    mx_ruler_bar_class_e,
    mx_ruler_class_e,
    mx_screen_device_class_e,
    mx_serialisable_object_class_e,
    mx_sheet_class_e,
    mx_sheet_painter_class_e,
    mx_simple_word_class_e,
    mx_space_word_class_e,
    mx_statusbar_class_e,
    mx_style_mod_cw_t_class_e,
    mx_table_area_class_e,
    mx_table_area_cursor_class_e,
    mx_table_area_painter_class_e,
    mx_table_area_pos_class_e,
    mx_table_buffer_class_e,
    mx_tabstop_class_e,
    mx_text_area_class_e,
    mx_text_area_cursor_class_e,
    mx_text_area_painter_class_e,
    mx_text_area_pos_class_e,
    mx_text_buffer_class_e,
    mx_text_importer_class_e,
    mx_ui_object_class_e,
    mx_unit_class_e,
    mx_word6_importer_class_e,
    mx_word_class_e,
    mx_word_iterator_class_e,
    mx_wp_area_iterator_class_e,
    mx_wp_cursor_class_e,
    mx_wp_document_class_e,
    mx_wp_editor_class_e,
    mx_wp_iterator_class_e,
    mx_wp_menubar_class_e,
    mx_wp_object_buffer_class_e,
    mx_wp_pos_class_e,
    mx_wp_repaginator_class_e,
    mx_wp_sh_buff_class_e,
    mx_wp_sheet_pos_class_e,
    mx_wp_statusbar_class_e,
    mx_wp_style_iterator_class_e,
    mx_wp_toolbar_class_e,
    mx_exporter_class_e,
    mx_rtf_exporter_class_e,
    mx_ascii_exporter_class_e,
    mx_undo_redo_buffer_class_e,
    mx_ur_item_class_e,
    mx_point_style_class_e,
    mx_dash_style_class_e,
    mx_area_style_class_e,
    mx_doc_arc_class_e
};

class mx_rtti {
public:
    virtual bool is_a(mx_rtti_class_e c) const = 0;
    virtual bool is_a(const mx_rtti& o) const = 0;
    virtual mx_rtti_class_e rtti_class() const = 0;

    virtual ~mx_rtti() { };
};

#endif
