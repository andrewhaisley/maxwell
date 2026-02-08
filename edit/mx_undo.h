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
#ifndef MX_UNDO_H
#define MX_UNDO_H
/*
 * MODULE/CLASS : mx_undo
 *
 * AUTHOR : Andrew Haisley
 *
 *
 *
 * DESCRIPTION:
 *
 * Undo and redo support.
 *
 *
 *
 *
 */
#include <mx.h>
#include <mx_list.h>
#include <mx_wp_pos.h>

class mx_wp_document;
class mx_wp_object_buffer;
class mx_table_area;
class mx_table_area_pos;
class mx_paragraph;
class mx_text_area;
class mx_text_area_pos;
class mx_cw_t;
class mx_border_style;
class mx_paragraph_style;
class mx_char_style;
class mx_rect;

enum mx_ur_item_type_t {
    mx_undo_typing_e,
    mx_undo_insert_table_e,
    mx_undo_delete_e,
    mx_undo_delete_para_break_e,
    mx_undo_cut_e,
    mx_undo_paste_e,
    mx_undo_format_e,
    mx_undo_format_borders_e,
    mx_undo_table_change_e,
    mx_undo_page_setup_e,
    mx_undo_named_style_change_e,
    mx_undo_area_style_change_e,
    mx_undo_area_scale_change_e,
    mx_undo_area_crop_change_e
};

class mx_ur_item //: public mx_rtti
{
    // MX_RTTI(mx_ur_item);

public:
    // page setup
    mx_ur_item(
        mx_ur_item_type_t t,
        mx_wp_document* doc,
        float width,
        float height,
        float top,
        float bottom,
        float left,
        float right,
        bool is_landscape,
        mx_wp_doc_pos& pos);

    // change one of the named styles
    mx_ur_item(
        mx_ur_item_type_t t,
        mx_wp_document* doc,
        mx_paragraph_style* styles[],
        int n);

    // insert character, cw_item, cr or shift/cr
    mx_ur_item(
        mx_ur_item_type_t t,
        mx_wp_doc_pos& pos);

    // insert table
    mx_ur_item(
        int& err,
        mx_ur_item_type_t t,
        mx_wp_document* doc,
        mx_wp_doc_pos& pos,
        int num_rows,
        int num_columns,
        float column_width);

    // delete something
    mx_ur_item(
        mx_ur_item_type_t t,
        mx_cw_t* cwi,
        mx_char_style* deleted_style,
        mx_wp_doc_pos& pos);

    // delete paragraph break
    mx_ur_item(
        mx_ur_item_type_t t,
        mx_wp_document* doc,
        const char* base_name,
        const mx_paragraph_style& the_style,
        mx_wp_doc_pos& pos);

    // cut something. If cut cannot be undone by paste pass NULL as the object
    // buffer, and the whole document is saved
    mx_ur_item(
        mx_ur_item_type_t t,
        mx_wp_document* doc,
        mx_wp_object_buffer* buff,
        mx_wp_doc_pos& start,
        mx_wp_doc_pos& end);

    // paste something
    mx_ur_item(
        mx_ur_item_type_t t,
        mx_wp_document* doc,
        mx_wp_doc_pos& start,
        mx_wp_doc_pos& end,
        bool need_to_delete_text_area_to_undo);

    // change some formatting
    // or change some area styles
    mx_ur_item(
        mx_ur_item_type_t t,
        mx_wp_document* doc,
        mx_wp_doc_pos& start,
        mx_wp_doc_pos& end,
        mx_list* paras);

    // change some borders in some paragraphs
    mx_ur_item(
        mx_ur_item_type_t t,
        mx_wp_document* doc,
        mx_wp_doc_pos& start,
        mx_wp_doc_pos& end,
        mx_list* styles,
        int dummy);

    // change some borders in a table
    mx_ur_item(
        mx_ur_item_type_t t,
        mx_wp_document* doc,
        mx_wp_doc_pos& start,
        mx_wp_doc_pos& end,
        mx_list* styles,
        int rows,
        int columns);

    // change a table in some way
    mx_ur_item(
        mx_ur_item_type_t t,
        mx_wp_document* doc,
        mx_wp_doc_pos& pos,
        mx_table_area* a);

    // scale an embedded area
    mx_ur_item(mx_ur_item_type_t t,
        mx_wp_document* doc,
        const mx_wp_doc_pos& pos,
        const mx_point& old_size,
        const mx_point& new_size);

    // crop an embedded area
    mx_ur_item(mx_ur_item_type_t t,
        mx_wp_document* doc,
        const mx_wp_doc_pos& pos,
        const mx_point& old_size,
        const mx_rect& new_crop_rect); // relative to origin of area

    // undo the thing
    void undo(int& err, mx_wp_document* doc, mx_wp_doc_pos* pos);

    // redo the thing
    void redo(int& err, mx_wp_document* doc, mx_wp_doc_pos* pos);

    virtual ~mx_ur_item();

    mx_ur_item_type_t type;

    // methods for typing items
    void setup_first_item(int& err, mx_wp_document* doc);
    void setup_first_typing_item(int& err, mx_wp_document* doc);
    void setup_first_delete_item(int& err, mx_wp_document* doc);

    // returns whether the next_item was succesfully combined with the first
    bool combine_items(int& err, mx_ur_item* next_item, mx_wp_document* doc);
    bool combine_typing_items(int& err, mx_ur_item* next_item, mx_wp_document* doc);
    bool combine_delete_items(int& err, mx_ur_item* next_item, mx_wp_document* doc);

private:
    int num_chars_to_undo;
    mx_wp_doc_pos position;
    mx_wp_doc_pos end_position;

    mx_wp_document* buffer_doc;
    mx_wp_object_buffer* buffer;
    mx_list* border_styles;
    mx_list* area_styles;
    int num_rows, num_columns;

    mx_list* deleted_chars;
    int offset_into_para;

    mx_list* para_list;
    mx_list* style_list;
    mx_cw_t* complex_word_item;

    char* base_style_name;
    mx_paragraph_style* actual_style;
    mx_char_style* character_style;

    mx_table_area* table;
    mx_text_area* text_area;

    float column_width;

    float width, height;
    float left, right, top, bottom;
    bool is_landscape;

    bool is_sheet_start;

    bool remove_text_area_to_undo_insert;

    mx_point* area_size;
    mx_rect* crop_rect;

    void init(mx_ur_item_type_t t);

    void undo_typing(int& err, mx_wp_document* doc, mx_wp_doc_pos* pos);
    void undo_cut(int& err, mx_wp_document* doc, mx_wp_doc_pos* pos);
    void undo_paste(int& err, mx_wp_document* doc, mx_wp_doc_pos* pos);
    void undo_format(int& err, mx_wp_document* doc, mx_wp_doc_pos* pos);
    void undo_table_change(int& err, mx_wp_document* doc, mx_wp_doc_pos* pos);
    void undo_page_setup(int& err, mx_wp_document* doc, mx_wp_doc_pos* pos);
    void undo_named_style_change(int& err, mx_wp_document* doc, mx_wp_doc_pos* pos);
    void undo_format_borders(int& err, mx_wp_document* doc, mx_wp_doc_pos* pos);
    void undo_format_para_borders(int& err, mx_wp_document* doc, mx_wp_doc_pos* pos);
    void undo_format_table_borders(int& err, mx_wp_document* doc, mx_wp_doc_pos* pos);
    void undo_delete(int& err, mx_wp_document* doc, mx_wp_doc_pos* pos);
    void undo_delete_para_break(int& err, mx_wp_document* doc, mx_wp_doc_pos* pos);
    void undo_insert_table(int& err, mx_wp_document* doc, mx_wp_doc_pos* pos);
    void undo_area_style_change(int& err, mx_wp_document* doc, mx_wp_doc_pos* pos);
    void undo_scale(int& err, mx_wp_document* doc, mx_wp_doc_pos* pos);
    void undo_crop(int& err, mx_wp_document* doc, mx_wp_doc_pos* pos);

    void redo_typing(int& err, mx_wp_document* doc, mx_wp_doc_pos* pos);
    void redo_cut(int& err, mx_wp_document* doc, mx_wp_doc_pos* pos);
    void redo_paste(int& err, mx_wp_document* doc, mx_wp_doc_pos* pos);
    void redo_delete(int& err, mx_wp_document* doc, mx_wp_doc_pos* pos);
    void redo_delete_para_break(int& err, mx_wp_document* doc, mx_wp_doc_pos* pos);
    void redo_insert_table(int& err, mx_wp_document* doc, mx_wp_doc_pos* pos);
    void redo_format(int& err, mx_wp_document* doc, mx_wp_doc_pos* pos);
    void redo_format_borders(int& err, mx_wp_document* doc, mx_wp_doc_pos* pos);
    void redo_table_change(int& err, mx_wp_document* doc, mx_wp_doc_pos* pos);
    void redo_page_setup(int& err, mx_wp_document* doc, mx_wp_doc_pos* pos);
    void redo_named_style_change(int& err, mx_wp_document* doc, mx_wp_doc_pos* pos);
    void redo_area_style_change(int& err, mx_wp_document* doc, mx_wp_doc_pos* pos);
    void redo_scale(int& err, mx_wp_document* doc, mx_wp_doc_pos* pos);
    void redo_crop(int& err, mx_wp_document* doc, mx_wp_doc_pos* pos);

    mx_paragraph* replace_para(
        int& err,
        mx_table_area* area,
        mx_table_area_pos* pos,
        mx_paragraph* p);

    mx_paragraph* replace_para(
        int& err,
        mx_text_area* area,
        mx_text_area_pos* pos,
        mx_paragraph* p);
};

class mx_undo_redo_buffer //: public mx_rtti
{
    // MX_RTTI(mx_undo_redo_buffer);

public:
    mx_undo_redo_buffer(int length);
    virtual ~mx_undo_redo_buffer();

    // add something to be undone to the start of the buffer. Only need to pass
    // the doc parameter in the typing and backspace/delete cases
    void insert(int& err, mx_ur_item* i, mx_wp_document* doc = NULL);

    // append something that can't be undone
    void mark_cant_undo(int& err);

    // is there anything to be undone?
    bool anything_to_undo();
    mx_ur_item_type_t type_to_undo(int& err);

    // is there anything to be re-done?
    bool anything_to_redo();
    mx_ur_item_type_t type_to_redo(int& err);

    // gets a string representation of the type to redo/undo
    const char* item_name(mx_ur_item_type_t t);

    // undo something - pos may be changed
    void undo(int& err, mx_wp_document* doc, mx_wp_doc_pos* pos);

    // redo something - pos may be changed
    void redo(int& err, mx_wp_document* doc, mx_wp_doc_pos* pos);

private:
    mx_list items;
    int buffer_length;
    int redo_depth;

    bool combine_items(int& err, mx_ur_item* i, mx_wp_document* doc);
};

#endif
