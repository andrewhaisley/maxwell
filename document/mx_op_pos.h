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
 * MODULE/CLASS :  mx_opaque_area_pos
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
#ifndef MX_OPAQUE_AREA_POS_H
#define MX_OPAQUE_AREA_POS_H

#include <mx.h>
#include <mx_rtti.h>

#include <mx_area_pos.h>
#include <mx_doc_rect.h>
#include <mx_position.h>

class mx_area;

class mx_opaque_area_pos : public mx_area_pos {
    MX_RTTI(mx_opaque_area_pos_class_e)

public:
    mx_opaque_area_pos();
    virtual ~mx_opaque_area_pos();

    void operator=(const mx_opaque_area_pos& p);

    virtual void up(int& err, mx_area* a);
    virtual void down(int& err, mx_area* a);
    virtual void left(int& err, mx_area* a);
    virtual void right(int& err, mx_area* a);

    // move to the end of the area
    void moveto_end(int& err, mx_area* area);

    // moveto the start
    void moveto_start(int& err, mx_area* area);

    // moveto a point
    virtual void moveto(int& err, mx_area* a, mx_point& p);

    virtual void next_para(int& err, mx_area* a);
    virtual void prev_para(int& err, mx_area* a);

    virtual mx_area_pos* duplicate();

    virtual bool is_start() { return is_left; };
    virtual bool is_start_line() { return TRUE; };
    virtual bool is_start_para() { return TRUE; };
    virtual bool is_end(int& err, mx_area* a) { return !is_left; };

    virtual mx_point position(int& err, mx_area* a);

    // compare logical positions
    virtual bool operator<(const mx_area_pos& p);
    virtual bool operator>(const mx_area_pos& p);
    virtual bool operator==(const mx_area_pos& p);
    virtual bool operator!=(const mx_area_pos& p);
    virtual bool operator<=(const mx_area_pos& p);
    virtual bool operator>=(const mx_area_pos& p);
    virtual bool is_same_para_as(mx_area_pos& p);

private:
    // can only be to the left or right of the area
    bool is_left;
};

#endif
