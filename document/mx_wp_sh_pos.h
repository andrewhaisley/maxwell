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
#ifndef MX_WP_SHEET_POS
#define MX_WP_SHEET_POS
/*
 * MODULE/CLASS : mx_wp_sheet_pos.h
 *
 * AUTHOR : Andrew Haisley
 *
 *
 *
 * DESCRIPTION:
 *
 * Positions within a sheet from a WP point of view.
 *
 *
 *
 */

#include <mx_position.h>

class mx_sheet;
class mx_area;
class mx_area_pos;
class mx_point;

class mx_wp_sheet_pos : public mx_position {
    MX_RTTI(mx_wp_sheet_pos_class_e)

public:
    mx_wp_sheet_pos();
    mx_wp_sheet_pos(const mx_wp_sheet_pos& other);
    virtual ~mx_wp_sheet_pos();

    mx_wp_sheet_pos& operator=(const mx_wp_sheet_pos& p);

    // compare logical positions
    bool operator<(const mx_wp_sheet_pos& p);
    bool operator>(const mx_wp_sheet_pos& p);
    bool operator==(const mx_wp_sheet_pos& p);
    bool operator!=(const mx_wp_sheet_pos& p);
    bool operator<=(const mx_wp_sheet_pos& p);
    bool operator>=(const mx_wp_sheet_pos& p);

    // logical navigation around the document
    void up(int& err, mx_sheet* s);
    void down(int& err, mx_sheet* s);
    void left(int& err, mx_sheet* s);
    void right(int& err, mx_sheet* s);

    void back_area(int& err);
    void forward_area(int& err);

    bool is_end(int& err, mx_sheet* s);
    bool is_start();
    bool is_start_line();
    bool is_start_para();

    void moveto(int& err, mx_sheet* a, mx_point& p);

    void moveto_start(int& err, mx_sheet* s);
    void moveto_start(int& err, mx_sheet* s, int area_index);
    void moveto_end(int& err, mx_sheet* s);

    // next/prev word, paragraph, and area
    void next_word(int& err, mx_sheet* s);
    void prev_word(int& err, mx_sheet* s);
    void next_para(int& err, mx_sheet* s);
    void prev_para(int& err, mx_sheet* s);
    void next_area(int& err, mx_sheet* s);
    void prev_area(int& err, mx_sheet* s);

    // which area
    int get_area_index() const;

    // goto start of area
    void set_area_index(int& err, int i, mx_sheet* s);

    // which area is the position in - use rtti to figure out class
    // of result
    virtual mx_area* area(int& err, mx_sheet* s);

    // where is the position within the area - use rtti to figure
    // out class of result
    virtual mx_area_pos* get_area_pos(int& err);

    // where within the sheet is the position
    mx_point position(int& err, mx_sheet* s);

    // where within the sheet is the position offset in lines
    mx_point position(int& err, mx_sheet* s, int line_offset);

    // reset area position in event of underlying document change
    void reset(int& err, mx_sheet* s);

    // if the position is off the end of the sheet, move it to the end
    void make_valid(int& err, mx_sheet* s);

private:
    int index;
    mx_area_pos* area_pos;

    void new_area_pos(mx_area* a);
};

#endif
