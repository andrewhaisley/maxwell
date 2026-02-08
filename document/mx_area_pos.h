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
 * MODULE/CLASS :  mx_area_pos
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
#ifndef MX_AREA_POS_H
#define MX_AREA_POS_H

#include "mx_position.h"
#include <mx.h>
#include <mx_rtti.h>

class mx_area;
class mx_point;
class mx_char_style;

class mx_area_pos : public mx_position {
    MX_RTTI(mx_area_pos_class_e);

public:
    mx_area_pos();
    mx_area_pos(const mx_area_pos& p);
    virtual ~mx_area_pos();

    // makes a copy of the instance
    virtual mx_area_pos* duplicate() = 0;

    virtual void moveto_start(int& err, mx_area* a) = 0;
    virtual void moveto_end(int& err, mx_area* a) = 0;
    virtual bool is_start() = 0;
    virtual bool is_start_line() = 0;
    virtual bool is_start_para() = 0;
    virtual bool is_end(int& err, mx_area* a) = 0;

    virtual void up(int& err, mx_area* a) = 0;
    virtual void down(int& err, mx_area* a) = 0;
    virtual void left(int& err, mx_area* a) = 0;
    virtual void right(int& err, mx_area* a) = 0;

    virtual mx_point position(int& err, mx_area* a) = 0;
    virtual mx_point position(int& err, mx_area* a, int line_offset);

    virtual void moveto(int& err, mx_area* a, mx_point& p) = 0;

    virtual void next_para(int& err, mx_area* a) = 0;
    virtual void prev_para(int& err, mx_area* a) = 0;

    // compare logical positions
    virtual bool operator<(const mx_area_pos& p) = 0;
    virtual bool operator>(const mx_area_pos& p) = 0;
    virtual bool operator==(const mx_area_pos& p) = 0;
    virtual bool operator!=(const mx_area_pos& p) = 0;
    virtual bool operator<=(const mx_area_pos& p) = 0;
    virtual bool operator>=(const mx_area_pos& p) = 0;
    virtual bool is_same_para_as(mx_area_pos& p) = 0;
    virtual bool is_same_cell_as(mx_area_pos& p);
    virtual bool is_same_row_as(mx_area_pos& p);
    virtual bool is_same_column_as(mx_area_pos& p);

    virtual mx_char_style* get_char_style(int& err, mx_area* a);
};

#endif
