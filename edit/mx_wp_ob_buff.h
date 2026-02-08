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
 * MODULE/CLASS : mx_wp_object_buffer.h
 *
 * AUTHOR : Andrew Haisley
 *
 * Copyright (C) 1996 Tangent Data Limited
 *
 * DESCRIPTION:
 *
 * This class is one which stores a series of document parts. Things get put
 * into one of these when you do a cut or copy for example. In conjuction with
 * one of the document sub-classes object buffers can be pasted into a
 * document at a given position.
 *
 *
 *
 *
 */

#ifndef MX_WP_OBJECT_BUFFER
#define MX_WP_OBJECT_BUFFER

#include "mx_buffer.h"
#include <mx.h>
#include <mx_para_style.h>

class mx_area;
class mx_sheet;
class mx_wp_sheet_pos;
class mx_wp_document;
class mx_wp_doc_pos;
class mx_table_area;
class mx_table_area_pos;
class mx_text_area;
class mx_text_area_pos;
class mx_table_buffer;
class mx_text_buffer;
class mx_area_pos;

class mx_wp_object_buffer : public mx_buffer {
    MX_RTTI(mx_wp_object_buffer_class_e)

public:
    mx_wp_object_buffer(mx_wp_document*);
    mx_wp_object_buffer(const mx_wp_object_buffer&);
    virtual ~mx_wp_object_buffer();

    // cuts the range of stuff out of a wp document
    void cut(
        int& err,
        mx_wp_document& doc,
        mx_wp_doc_pos& point1,
        mx_wp_doc_pos& point2,
        mx_edit_op edit_op = mx_cut_e);

    // copies the range of stuff out of a wp document
    void copy(
        int& err,
        mx_wp_document& doc,
        mx_wp_doc_pos& point1,
        mx_wp_doc_pos& point2);

    // removes the range of stuff out of a wp document
    void remove(
        int& err,
        mx_wp_document& doc,
        mx_wp_doc_pos& point1,
        mx_wp_doc_pos& point2);

    // Inserts the contents of the buffer into the document at the position
    // specified. All insert operations can be undone by performing a cut from
    // the start position to the position p. The only proviso is that in some
    // cases the caller needs to delete a text area at the cut position after
    // the cut. Whether this needs to be performed is returned in the boolean
    // reference
    void insert(int& err, mx_wp_document& doc, mx_wp_doc_pos& p,
        bool& need_to_delete_text_area_to_undo);

    // is there anuthing in it?
    bool is_empty();

    // does the buffer contain any table cells?
    bool contains_cells();

    // does it contain table cells only
    bool contains_cells_only();

    // does it contain text
    bool contains_text();

    // does it contain text only
    bool contains_text_only();

    // does it contain any whole table areas
    bool contains_tables();

    // does it only contain whole table areas
    bool contains_tables_only();

    // does it have a table at the start or the end of the buffer
    bool contains_table_at_start();
    bool contains_table_at_end();

    // does it have words at the start or the end of the buffer
    bool contains_words_at_start();
    bool contains_words_at_end();

private:
    mx_list* contents;

    mx_char_style_mod start_mod;
    char* buf_base_style_name;
    mx_paragraph_style_mod buf_base_style_mod;

    void clear_old_contents(mx_wp_document* cutting_from_doc = NULL);

    void cut_sheet(int& err, mx_wp_document& doc, int sn,
        mx_edit_op edit_op);

    void cut_from_sheet(int& err, mx_sheet* s, mx_wp_sheet_pos& start,
        mx_wp_sheet_pos& end, mx_edit_op edit_op);

    void cut_from_area(int& err,
        mx_wp_sheet_pos& p,
        mx_sheet* s,
        int& area_index,
        mx_area* a,
        mx_area_pos* sap,
        mx_area_pos* eap,
        mx_edit_op edit_op);

    void cut_from_table_area(int& err,
        mx_wp_sheet_pos& p,
        mx_sheet* s,
        int& area_index,
        mx_table_area* a,
        mx_table_area_pos* sap,
        mx_table_area_pos* eap,
        mx_edit_op edit_op);

    enum mx_which_rows { top_rows_e,
        bottom_rows_e };
    void cut_table_rows(int& err,
        mx_wp_sheet_pos& p,
        mx_sheet* s,
        int& area_index,
        mx_table_area* a,
        mx_table_area_pos* pos,
        mx_which_rows top_or_bottom,
        mx_edit_op edit_op);

    void cut_from_text_area(int& err,
        mx_text_area* a,
        mx_text_area_pos* sap,
        mx_text_area_pos* eap,
        mx_edit_op edit_op);

    void set_contents_flags(mx_area* a);

    bool buffer_contains_cells;
    bool buffer_contains_text;
    bool buffer_contains_tables;

    void insert_text_buffer(int& err, mx_wp_document& doc, mx_text_buffer* b,
        mx_wp_doc_pos& p);

    void insert_table_buffer(int& err, mx_wp_document& doc, mx_table_buffer* b,
        mx_wp_doc_pos& p);

    void insert_area(int& err, mx_wp_document& doc, mx_area* a,
        mx_wp_doc_pos& p);

    void join_areas(int& err, int i, mx_sheet* s);

    virtual void move_to_document(int& err, mx_wp_document* d);

    void insert_into_text(int& err, mx_wp_document& doc, mx_wp_doc_pos& p);
    void insert_into_table(int& err, mx_wp_document& doc, mx_wp_doc_pos& p,
        bool& need_to_delete_text_area_to_undo);
    void insert_cells_into_table(int& err, mx_wp_document& doc, mx_wp_doc_pos& p);
    void insert_text_and_tables_into_table(int& err, mx_wp_document& doc, mx_wp_doc_pos& p,
        bool& need_to_delete_text_area_to_undo);

    void fix_post_insert_document_structure(int& err, mx_wp_document& doc,
        mx_wp_doc_pos& insert_start,
        mx_wp_doc_pos& insert_end,
        mx_char_style& cs_at_insert_pos);
};

#endif // MX_WP_OBJECT_BUFFER
