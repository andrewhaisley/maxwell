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
#ifndef MX_WP_POS
#define MX_WP_POS
/*
 * MODULE/CLASS : mx_wp_pos.h
 *
 * AUTHOR : Andrew Haisley
 *
 *
 *
 * DESCRIPTION:
 *
 * Positions within a document from a WP point of view.
 *
 *
 *
 */

#include <mx.h>
#include <mx_wp_sh_pos.h>

class mx_para_pos;
class mx_paragraph;
class mx_paragraph_style;
class mx_wp_document;
class mx_doc_coord_t;
class mx_point;

class mx_wp_doc_pos : public mx_rtti {
    MX_RTTI(mx_wp_pos_class_e)

public:
    mx_wp_doc_pos();
    mx_wp_doc_pos(const mx_wp_doc_pos& other);
    virtual ~mx_wp_doc_pos();

    mx_wp_doc_pos& operator=(const mx_wp_doc_pos& p);

    // compare logical positions
    bool operator<(const mx_wp_doc_pos& p);
    bool operator>(const mx_wp_doc_pos& p);
    bool operator==(const mx_wp_doc_pos& p);
    bool operator!=(const mx_wp_doc_pos& p);
    bool operator<=(const mx_wp_doc_pos& p);
    bool operator>=(const mx_wp_doc_pos& p);

    // logical navigation around the document
    void up(int& err, mx_wp_document* d);
    void down(int& err, mx_wp_document* d);
    void left(int& err, mx_wp_document* d);
    void right(int& err, mx_wp_document* d);

    void moveto(int& err, mx_wp_document* d, mx_doc_coord_t& c);

    void moveto_start(int& err, mx_wp_document* d);
    void moveto_end(int& err, mx_wp_document* d);
    void moveto_start_sheet(int& err, mx_wp_document* d, int sheet);

    // is it the end/start of the doc?
    // is it the first line/paragraph of the doc?
    bool is_end(int& err, mx_wp_document* doc);
    bool is_start();
    bool is_start_line();
    bool is_start_para();

    // which sheet is the current position in
    int get_sheet() const;

    // position within the sheet
    mx_wp_sheet_pos* get_sheet_pos();

    // back one sheet
    void back_sheet(int& err);

    // back one area
    void back_area(int& err, mx_wp_document* doc);

    // forward one sheet
    void forward_sheet(int& err, mx_wp_document* d);

    // end of sheet
    void moveto_end_sheet(int& err, mx_wp_document* d);

    // start of sheet
    void moveto_start_sheet(int& err, mx_wp_document* d);

    // where are we?
    mx_doc_coord_t position(int& err, mx_wp_document* d);

    // where are we offset in lines from current position
    mx_doc_coord_t position(int& err, mx_wp_document* d, int line_offset);

    // reset in the event of a change in the underlying document
    void reset(int& err, mx_wp_document* d);

    // is the position in paragraph?
    bool is_paragraph(int& err);

    // is the position in table?
    bool is_table(int& err);

    // get the paragraph pos if the postion is in one
    mx_para_pos* get_para_pos(int& err);

    // get the paragraph if the postion is in one
    mx_paragraph* get_para(int& err, mx_wp_document* doc);

    // get the paragraph style at the current location - could be an
    // area or a paragraph
    mx_paragraph_style* get_para_style(int& err, mx_wp_document* doc);

    // get the area position the position is in
    mx_area_pos* get_area_pos(int& err);

    // get the area the position is in
    mx_area* get_area(int& err, mx_wp_document* doc);

    // is the position at the start of its area
    bool is_area_start(int& err, mx_wp_document* doc);

    // is the position at the end of its area
    bool is_area_end(int& err, mx_wp_document* doc);

    // moveto next sheet
    void next_sheet(int& err, mx_wp_document* doc);

    // moveto previous sheet
    void prev_sheet(int& err, mx_wp_document* doc);

    // moveto next area
    void next_area(int& err, mx_wp_document* doc);

    // moveto previous area
    void prev_area(int& err, mx_wp_document* doc);

    // moveto start of next or end of previous cell in table. Or do left/right
    // if at the start/end of current table area, or not in a table area
    void next_cell(int& err, mx_wp_document* doc);
    void prev_cell(int& err, mx_wp_document* doc);

    // moveto start of next or end of previous row in table. Or do up/down if
    // at the start/end of current table area, or not in a table area
    void next_row(int& err, mx_wp_document* doc);
    void prev_row(int& err, mx_wp_document* doc);

    // move to the start/end of the area.
    void moveto_start_area(int& err, mx_wp_document* doc);
    void moveto_end_area(int& err, mx_wp_document* doc);

    // moveto start of next/prev paragraph if in a text area, next/prev area
    // otherwise
    void next_para(int& err, mx_wp_document* doc);
    void prev_para(int& err, mx_wp_document* doc);

    // next/prev word
    void next_word(int& err, mx_wp_document* doc);
    void prev_word(int& err, mx_wp_document* doc);

    // next/prev logical word, ie, counting words which have been split across
    // lines as one.
    void next_logical_word(int& err, mx_wp_document* doc);
    void prev_logical_word(int& err, mx_wp_document* doc);

    // if the position is a paragraph, moveto the start/end of it. Otherwise
    // move to the start/end of the area.
    void moveto_start_para(int& err, mx_wp_document* doc);
    void moveto_end_para(int& err, mx_wp_document* doc);

    // moveto the start/end of the table row. Position must be in a table area
    void moveto_start_row(int& err, mx_wp_document* doc);
    void moveto_end_row(int& err, mx_wp_document* doc);

    // moveto the start/end of the table cell. Position must be in a table area
    void moveto_start_cell(int& err, mx_wp_document* doc);
    void moveto_end_cell(int& err, mx_wp_document* doc);

    // move to the start or end of physical or logical words
    void moveto_start_word(int& err, mx_wp_document* doc);
    void moveto_end_word(int& err, mx_wp_document* doc);
    void moveto_start_logical_word(int& err, mx_wp_document* doc);
    void moveto_end_logical_word(int& err, mx_wp_document* doc);

    // move left or right ignoring "false" break words. False break words are
    // those break words which are merely markers, ie Long word breaks and
    // paragraph breaks for split paragraphs (after repagination).
    void move_left_ignoring_false_breaks(int& err, mx_wp_document* doc);
    void move_right_ignoring_false_breaks(int& err, mx_wp_document* doc);

    // try and make sure the position is valid - if it is off the end of the
    // sheet, move it to the end
    void make_valid(int& err, mx_wp_document* doc);

    // is the position in the same area, paragraph, table cell, table row, or
    // table column as the other?
    bool is_same_area_as(mx_wp_doc_pos* p);
    bool is_same_para_as(int& err, mx_wp_doc_pos* p);
    bool is_same_cell_as(int& err, mx_wp_doc_pos* p);
    bool is_same_row_as(int& err, mx_wp_doc_pos* p);
    bool is_same_column_as(int& err, mx_wp_doc_pos* p);

    // is the position at the start of a paragraph
    bool is_at_start_of_para(int& err, mx_wp_document* doc);
    bool is_at_end_of_para(int& err, mx_wp_document* doc);

    // is the position within a table or paragraph which has been split due
    // to repagination
    bool is_in_split_text_container(int& err, mx_wp_document* doc);

    // remember this position from the start of a given sheet in units of
    // paragraphs/table areas and character offsets into those objects
    void save(int& err, mx_wp_document* doc, uint32 sheet_no = 0);

    // restore this position from the start of a given sheet in units of
    // paragraphs/table areas and character offsets into those objects
    void restore(int& err, mx_wp_document* doc);

    // tells whether this position and the other position are part of the same
    // logical text container (paragraph/table)
    bool is_part_of_same_text_container(int& err, mx_wp_doc_pos& other,
        mx_wp_document* doc);

    // moves to the next logical paragraph
    void next_logical_para(int& err, mx_wp_document* doc);

private:
    int sheet_num;
    mx_wp_sheet_pos sheet_pos;

    // stuff for "remembering" positions
    struct stored_pos_t {
        stored_pos_t();
        uint32 sheet_index;
        uint32 paragraph_index;
        uint32 char_index;
    };

    stored_pos_t stored_pos;

    // returns the text container (table area or paragraph) at the current
    // position
    mx_rtti* get_text_container(int& err, mx_wp_document* doc);

    // is the given text container the end, or a whole one, ie not split by the
    // repagination
    bool is_whole_or_end_of_text_container(int& err, mx_rtti* tc);

    // sets the stored pos to be the offset from the start of the current
    // logical paragraph
    void get_offset_from_start_of_logical_paragraph(int& err,
        mx_wp_document* doc);

    // sets the physical pos to be the stored logical pos from the start of the
    // current logical paragraph
    void set_offset_from_start_of_logical_paragraph(int& err,
        mx_wp_document* doc);
};

#endif // MX_WP_POS
