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
 * MODULE/CLASS : mx_table_area
 *
 * AUTHOR : Andrew Haisley
 *
 *
 *
 * DESCRIPTION:
 *
 * Areas specifically for holding tables
 *
 *
 *
 */
#ifndef MX_TABLE_AREA_H
#define MX_TABLE_AREA_H

#include "mx_area.h"

class mx_text_area;
class mx_table_area_pos;
class mx_cw_t;
class mx_nlist;

class mx_table_area : public mx_area {
    MX_RTTI(mx_table_area_class_e)

public:
    // unfortunately, table areas sometimes get split and need to be rejoined
    // later - the type of the area indicates whether the thing is stand
    // alone or part of a bigger area
    enum mx_table_type_t {
        mx_table_start_e,
        mx_table_middle_e,
        mx_table_end_e,
        mx_table_whole_e
    };

    mx_table_area(uint32 docid, mx_paragraph_style* s);
    mx_table_area(uint32 docid, uint32 oid);
    mx_table_area(const mx_table_area& a);

    ~mx_table_area();

    virtual mx_area* duplicate();

    // these delete or create text areas as necessary
    void set_num_rows(int& err, int rows);
    void set_num_columns(int& err, int columns);

    int get_num_columns(int& err);
    int get_num_rows(int& err);

    // sets the width of a column
    void set_column_width(int& err, int column, float width);

    // gets the width of a column
    float get_column_width(int& err, int column);

    // returns the real thing, not a copy
    mx_text_area* get_cell(int& err, int row, int column);

    // set a cell - deletes the old one first
    void set_cell(int& err, int row, int column, mx_text_area* cell);

    // reduces the number of rows - deletes cells as required
    void remove_rows(int& err, int row, int n);

    // reduces the number of columns - deletes cells as required
    void remove_columns(int& err, int column, int n);

    // increases the number of rows. 0 inserts to the left of row 0, 1
    // inserts to the right of row 0
    void insert_rows(int& err, int row, int n);

    // increases the number of columns. 0 inserts to the left of column 0, 1
    // inserts to the right of column 0
    void insert_columns(int& err, int column, int n);

    // insert/remove rows/columns without readjusting cell positions etc to
    // save time for multiple calls to row insert functions
    void insert_rows_no_reposition(int& err, int row, int n);
    void remove_rows_no_reposition(int& err, int row, int n);
    void insert_columns_no_reposition(int& err, int column, int n);
    void remove_columns_no_reposition(int& err, int column, int n);

    // join the paragraph at the position with the next one. throws an error
    // if the next paragraph cannot be joined
    void join_para_with_next(int& err, mx_table_area_pos& p);

    void serialise_attributes(int& err);
    void unserialise_attributes(int& err);

    virtual void uncache(int& err);
    virtual uint32 memory_size(int& err);

    void delete_cells(int& err);

    void serialise(int& err, uint8** buffer);
    void unserialise(int& err, uint8** buffer);
    uint32 get_serialised_size(int& err);

    // move into another document
    virtual void move_to_document(int& err, mx_document* doc);

    // table areas are flowable
    virtual bool is_flowable() { return TRUE; };

    // reformat a given range. This is the smallest range within the area
    // that will be reformatted - changes may filter down as a result of
    // size changes etc.
    virtual void reformat(
        int& err,
        mx_table_area_pos& start,
        mx_table_area_pos& end,
        bool& height_changed);

    // reformat the whole area
    virtual void reformat(
        int& err,
        bool& height_changed);

    // iterates through table cells and replaces any references to the given
    // base style with the new reference. It also calls the superclasses
    // method.
    virtual void replace_style(int& err, mx_paragraph_style* style_to_replace,
        mx_paragraph_style* replacement_style);

    mx_table_type_t get_type(int& err);
    void set_type(int& err, mx_table_type_t t);

    virtual mx_paragraph_style* get_style(int& err, mx_area_pos* p);

    // delete the thing to the left of the position
    mx_cw_t* delete_to_left(
        int& err,
        mx_table_area_pos* p,
        bool& deleted_paragraph);

    // insert a character - may be space or tab as well as printing character
    void insert_char(int& err, char c, mx_table_area_pos* p);

    // insert a complex word item. The item is NOT copied, so do not delete it.
    void insert_cw_item(int& err, mx_cw_t*, mx_table_area_pos* p);

    // insert a line break
    void insert_line_break(int& err, mx_table_area_pos* p);

    // insert a paragraph break
    void insert_para_break(int& err, mx_table_area_pos* p);

    // get/set a horizontal/vertical border segment.
    void get_vertical_border(int& err, int row, int column,
        mx_line_style& border);
    void get_horizontal_border(int& err, int row, int column,
        mx_line_style& border);
    void set_vertical_border(int& err, int row, int column,
        const mx_line_style& new_border);
    void set_horizontal_border(int& err, int row, int column,
        const mx_line_style& new_border);

    // gets an array of pointers to border styles for a given table
    // selection. border_array is an array of pointers to border styles. The
    // border styles in the nlist must be deleted by the caller
    void get_cell_borders_for_selection(int& err,
        const mx_table_area_pos& start,
        const mx_table_area_pos& end,
        mx_nlist& border_array);

    // apply a given border mod to a selection of cells in this table
    void apply_border_mod_to_selection(int& err,
        const mx_table_area_pos& start,
        const mx_table_area_pos& end,
        const mx_border_style_mod& mod);

    // goes over the cells and sets their origins based around the widths
    // and heights
    void reposition_cells(int& err);
    void recalculate_outline(int& err);

    // gets the row number to break at to make the table no more than
    // the specified height. If the table can't be made that small,
    // the nearest row is given instead
    int get_break_row_for_height(int& err, float f);

    float get_height_from_row(int& err, int row);

    // split the table into two for repagination at the given row
    mx_table_area* repaginate_split(int& err, int row_num);

    // split the table into two at the given position
    mx_table_area* normal_split(int& err, const mx_table_area_pos& tap);

    // can this area be joined with the other?
    virtual bool can_join(int& err, mx_area* a);

    // join this area with another. The second area is deleted as part of this
    // process. Throws an error if the areas are incompatible.
    virtual void join(int& err, mx_area* a);

    // print debug information
    virtual void print_debug();

    virtual mx_db_mem_state_t get_mem_state();
    virtual void set_mem_state(mx_db_mem_state_t s);

private:
    mx_text_area** cell_objs;
    uint32* cell_oids;

    void unserialise_cells(int& err, uint8** buffer, int nc, int nr);

    // just splits a table without setting the table type
    mx_table_area* aka_split(int& err, int row_num);

    // set the table types depending on whether the split was a repagination
    // split or a normal split. Pass the bit of the table just split
    void set_split_table_types_normal(int& err, mx_table_area* table2);
    void set_split_table_types_repagination(int& err, mx_table_area* table2);
};

#endif
