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
 * MODULE/CLASS :  mx_text_area_pos
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
#ifndef MX_TEXT_AREA_POS_H
#define MX_TEXT_AREA_POS_H

#include <mx.h>
#include <mx_rtti.h>

#include <mx_area_pos.h>
#include <mx_doc_coord.h>
#include <mx_doc_rect.h>
#include <mx_para_pos.h>
#include <mx_position.h>
#include <mx_spline.h>

class mx_text_area;

class mx_text_area_pos : public mx_area_pos {
    MX_RTTI(mx_text_area_pos_class_e)

public:
    mx_text_area_pos();

    mx_text_area_pos(
        int paragraph_index,
        int line_index,
        int word_index,
        int letter_index);

    mx_text_area_pos(
        int paragraph_index,
        mx_para_pos& pp);

    void operator=(const mx_text_area_pos& p);

    virtual void up(int& err, mx_area* a);
    virtual void down(int& err, mx_area* a);
    virtual void left(int& err, mx_area* a);
    virtual void right(int& err, mx_area* a);

    void left_word(int& err, mx_text_area* ta);
    void right_word(int& err, mx_text_area* ta);

    virtual mx_area_pos* duplicate();

    // move to the end of given text area
    virtual void moveto_end(int& err, mx_area* area);

    // move to the start
    virtual void moveto_start(int& err, mx_area* area);

    // moveto the end of the current paragraph
    void moveto_para_end(int& err, mx_text_area* area);

    // moveto the start of the current paragraph
    void moveto_para_start(int& err, mx_text_area* area);

    // forwards one paragraph
    void next_para(int& err, mx_text_area* area);

    // backwards one paragraph
    void prev_para(int& err, mx_text_area* area);

    // forward one word
    void next_word(int& err, mx_text_area* area);

    // backward one word
    void prev_word(int& err, mx_text_area* area);

    virtual void moveto(int& err, mx_area* a, mx_point& p);

    virtual void next_para(int& err, mx_area* a);
    virtual void prev_para(int& err, mx_area* a);

    virtual bool is_end(int& err, mx_area* area);
    virtual bool is_start();
    virtual bool is_start_line();
    virtual bool is_start_para();

    virtual mx_point position(int& err, mx_area* a);
    virtual mx_point position(int& err, mx_area* a, int line_offset);

    // compare logical positions
    virtual bool operator<(const mx_area_pos& p);
    virtual bool operator>(const mx_area_pos& p);
    virtual bool operator==(const mx_area_pos& p);
    virtual bool operator!=(const mx_area_pos& p);
    virtual bool operator<=(const mx_area_pos& p);
    virtual bool operator>=(const mx_area_pos& p);
    virtual bool is_same_para_as(mx_area_pos& p);

    int paragraph_index;
    mx_para_pos para_pos;

    virtual mx_char_style* get_char_style(int& err, mx_area* a);
};

#endif
