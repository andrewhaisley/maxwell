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
 * MODULE/CLASS :  mx_table_area_pos
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
#ifndef MX_TABLE_AREA_POS_H
#define MX_TABLE_AREA_POS_H

#include <mx.h>
#include <mx_rtti.h>

#include <mx_area_pos.h>
#include <mx_doc_coord.h>
#include <mx_doc_rect.h>
#include <mx_para_pos.h>
#include <mx_position.h>
#include <mx_spline.h>
#include <mx_txt_pos.h>

class mx_table_area;

class mx_table_area_pos : public mx_area_pos {
    MX_RTTI(mx_table_area_pos_class_e)

public:
    mx_table_area_pos();

    mx_table_area_pos(int row, int column,
        const mx_text_area_pos& text_pos);

    void operator=(const mx_table_area_pos& p);

    virtual void up(int& err, mx_area* a);
    virtual void down(int& err, mx_area* a);
    virtual void left(int& err, mx_area* a);
    virtual void right(int& err, mx_area* a);

    virtual void moveto(int& err, mx_area* a, mx_point& p);

    virtual void next_para(int& err, mx_area* a);
    virtual void prev_para(int& err, mx_area* a);

    virtual mx_area_pos* duplicate();

    void moveto_start(int& err, mx_area* ta);
    void moveto_end(int& err, mx_area* ta);

    virtual bool is_end(int& err, mx_area* area);
    virtual bool is_start();
    virtual bool is_start_line();
    virtual bool is_start_para();

    virtual mx_point position(int& err, mx_area* a);
    virtual mx_point position(int& err, mx_area* a, int line_offset);

    // moveto the end of the current paragraph
    void moveto_para_end(int& err, mx_table_area* area);

    // moveto the start of the current paragraph
    void moveto_para_start(int& err, mx_table_area* area);

    // next word
    void next_word(int& err, mx_table_area* area);

    // prev word
    void prev_word(int& err, mx_table_area* area);

    // moveto start of current cell
    void moveto_cell_start(int& err, mx_table_area* area);

    // moveto end of current cell
    void moveto_cell_end(int& err, mx_table_area* area);

    // moveto start/end of current row
    void moveto_row_start(int& err, mx_table_area* area);
    void moveto_row_end(int& err, mx_table_area* area);

    // moveto next cell
    void next_cell(int& err, mx_table_area* area);

    // moveto cell
    void moveto_cell(int& err, mx_table_area* area, int row, int column);

    // moveto previous cell
    void prev_cell(int& err, mx_table_area* area);

    // compare logical positions
    virtual bool operator<(const mx_area_pos& p);
    virtual bool operator>(const mx_area_pos& p);
    virtual bool operator==(const mx_area_pos& p);
    virtual bool operator!=(const mx_area_pos& p);
    virtual bool operator<=(const mx_area_pos& p);
    virtual bool operator>=(const mx_area_pos& p);
    virtual bool is_same_para_as(mx_area_pos& p);
    virtual bool is_same_cell_as(mx_area_pos& p);
    virtual bool is_same_row_as(mx_area_pos& p);
    virtual bool is_same_column_as(mx_area_pos& p);

    int row;
    int column;
    mx_text_area_pos text_pos;

    virtual mx_char_style* get_char_style(int& err, mx_area* a);
};

#endif
