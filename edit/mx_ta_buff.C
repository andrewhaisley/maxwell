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
 * MODULE/CLASS : mx_table_buffer
 *
 * AUTHOR : Andrew Haisley
 *
 *
 *
 * DESCRIPTION:
 *
 *
 *
 *
 *
 */
#include "mx_ta_buff.h"
#include <mx.h>
#include <mx_rtti.h>
#include <mx_ta_pos.h>
#include <mx_table_area.h>
#include <mx_text_area.h>
#include <mx_wp_doc.h>

mx_table_buffer::mx_table_buffer(mx_wp_document* d)
    : mx_buffer(d)
{
    areas = NULL;
}

mx_table_buffer::mx_table_buffer(const mx_table_buffer& tb)
    : mx_buffer(tb)
{
    num_rows = tb.num_rows;
    num_columns = tb.num_columns;
    areas = tb.areas;

    if (areas != NULL) {
        int i, num_elems = num_rows * num_columns;
        areas = new mx_text_area*[num_elems];

        for (i = 0; i < num_elems; i++) {
            areas[i] = new mx_text_area(*tb.areas[i]);
        }
    }
}

mx_table_buffer::~mx_table_buffer()
{
    clear_old_contents();
}

void mx_table_buffer::cut(int& err, mx_table_area& a,
    mx_table_area_pos& pos1,
    mx_table_area_pos& pos2,
    mx_edit_op edit_op)
{
    mx_table_area_pos p1 = pos1, p2 = pos2;
    mx_text_area* ta;
    bool b;

    if (!p1.is_inside() || !p2.is_inside()) {
        // the selection extends outside the table so make sure we're
        // cutting/copying rows

        p1.column = 0;
        p2.column = a.get_num_columns(err) - 1;
        MX_ERROR_CHECK(err);

        ta = a.get_cell(err, p1.row, p1.column);
        MX_ERROR_CHECK(err);

        p1.text_pos.moveto_start(err, ta);
        MX_ERROR_CHECK(err);

        ta = a.get_cell(err, p2.row, p2.column);
        MX_ERROR_CHECK(err);

        p2.text_pos.moveto_end(err, ta);
        MX_ERROR_CHECK(err);
    }

    // this class can only be used to cut a range of cells. cuts within a
    // single cell are handled by the text area buffer class.
    if (p1.row == p2.row && p1.column == p2.column) {
        ta = a.get_cell(err, p1.row, p1.column);
        MX_ERROR_CHECK(err);

        if (!p1.text_pos.is_start()) {
            MX_ERROR_THROW(err, MX_TABLE_BUFFER_SINGLE_CELL);
        }

        b = p2.text_pos.is_end(err, ta);
        MX_ERROR_CHECK(err);

        if (!b) {
            MX_ERROR_THROW(err, MX_TABLE_BUFFER_SINGLE_CELL);
        }
    }

    if (edit_op != mx_remove_e)
        clear_old_contents();

    cut_cells(err, a,
        p1.row < p2.row ? p1.row : p2.row,
        p1.column < p2.column ? p1.column : p2.column,
        p1.row > p2.row ? p1.row : p2.row,
        p1.column > p2.column ? p1.column : p2.column,
        edit_op);
    MX_ERROR_CHECK(err);

abort:;
}

void mx_table_buffer::cut_cells(
    int& err,
    mx_table_area& a,
    int top_row,
    int top_column,
    int bottom_row,
    int bottom_column,
    mx_edit_op edit_op)
{
    int row, column, i = 0;
    mx_text_area* ta;
    mx_wp_document* d = this->get_document();

    if (edit_op != mx_remove_e) {
        num_rows = bottom_row - top_row + 1;
        num_columns = bottom_column - top_column + 1;
        areas = new mx_text_area*[num_rows * num_columns];
    }

    for (row = top_row; row <= bottom_row; row++) {
        for (column = top_column; column <= bottom_column; column++) {
            ta = a.get_cell(err, row, column);
            MX_ERROR_CHECK(err);

            if (edit_op != mx_remove_e) {
                areas[i] = new mx_text_area(*ta);

                areas[i]->move_to_document(err, d);
                MX_ERROR_CHECK(err);
            }

            if (edit_op != mx_copy_e) {
                ta->clear_contents(err);
                MX_ERROR_CHECK(err);
            }

            i++;
        }
    }
abort:;
}

void mx_table_buffer::insert(int& err, mx_table_area& a, mx_table_area_pos& p)
{
    // are there enough cells in the table to take the paste at the
    // specified position
    int dest_nc;
    int dest_nr;
    int i = 0;
    int row, column;
    mx_text_area* ta;
    mx_document* d = a.get_document(err);
    MX_ERROR_CHECK(err);

    dest_nc = a.get_num_columns(err);
    MX_ERROR_CHECK(err);

    dest_nr = a.get_num_rows(err);
    MX_ERROR_CHECK(err);

    if ((p.row + num_rows) > dest_nr || (p.column + num_columns) > dest_nc) {
        MX_ERROR_THROW(err, MX_TABLE_BUFFER_NOT_BIG_ENOUGH);
    }

    for (row = 0; row < num_rows; row++) {
        for (column = 0; column < num_columns; column++) {
            ta = new mx_text_area(*areas[i]);

            ta->move_to_document(err, d);
            MX_ERROR_CHECK(err);

            a.set_cell(err, row + p.row, column + p.column, ta);
            MX_ERROR_CHECK(err);

            i++;
        }
    }

    p.row += num_rows - 1;
    p.column += num_columns - 1;
    p.moveto_cell_end(err, &a);
    MX_ERROR_CHECK(err);
abort:;
}

void mx_table_buffer::clear_old_contents()
{
    int i;

    if (areas != NULL) {
        for (i = 0; i < (num_rows * num_columns); i++) {
            delete areas[i];
        }
        delete[] areas;
        areas = NULL;
    }
}

void mx_table_buffer::move_to_document(int& err, mx_wp_document* d)
{
    int i;

    if (areas != NULL) {
        for (i = 0; i < (num_rows * num_columns); i++) {
            areas[i]->move_to_document(err, d);
            MX_ERROR_CHECK(err);
        }
    }
abort:;
}
