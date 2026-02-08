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
 * MODULE/CLASS : mx_text_area_painter
 *
 * AUTHOR : Andrew Haisley
 *
 *
 *
 * DESCRIPTION:
 *
 * A class to draw text areas onto devices
 *
 *
 *
 *
 *
 */
#ifndef MX_TXT_PAINTER_H
#define MX_TXT_PAINTER_H

#include <mx.h>
#include <mx_area_paint.h>
#include <mx_break_w.h>
#include <mx_complex_w.h>
#include <mx_doc_coord.h>
#include <mx_doc_rect.h>
#include <mx_paintable.h>
#include <mx_painter.h>
#include <mx_paragraph.h>
#include <mx_space_w.h>
#include <mx_spline.h>
#include <mx_text_area.h>
#include <mx_word.h>

class mx_device;

class mx_text_area_painter : public mx_area_painter {
    MX_RTTI(mx_text_area_painter_class_e)

public:
    // these routines are needed by the document to perform drawing

    // use this to get a painter. There is in fact only one
    // instance in the system.....
    static mx_text_area_painter* painter();

    // draw a bit of the document
    virtual void draw(int& err, mx_doc_rect_t& r, mx_paintable& o);

    virtual ~mx_text_area_painter();

    virtual void highlight_from_start_of_area(int& err, mx_area* a, mx_area_pos* end);
    virtual void highlight_whole_area(int& err, mx_area* s);
    virtual void highlight_in_area(int& err, mx_area* s, mx_area_pos* start, mx_area_pos* end);
    virtual void highlight_to_end_of_area(int& err, mx_area* s, mx_area_pos* start);

private:
    // the constructor is private to stop instances being created. use the
    // painter() routine to get an instance to use.
    mx_text_area_painter();

    static mx_text_area_painter* the_painter;

    static void init();

    void draw_border(int& err, mx_paragraph* para, const mx_rect& r, float y_offset);
    void draw_simple_word(int& err, float x_offset, float y_offset,
        mx_word* w);
    void draw_complex_word(int& err, const mx_rect& r, float x_offset,
        float y_offset, mx_complex_word* w);
    void draw_break_word(int& err, float x_offset, float y_offset,
        mx_break_word* w);
    void draw_space_word(int& err, float x_offset, float y_offset,
        mx_space_word* w, mx_word* next, float left_gap);
    void draw_paragraph(int& err, mx_paragraph* para, const mx_rect& r, float y_offset);
    void draw_paragraph_line(int& err, mx_paragraph* para, const mx_rect& r, float y_offset,
        int line);
    void draw_paragraph_break(int& err, float x_offset, float y_offset,
        mx_break_word* w);
    void draw_background(int& err, mx_paragraph* para, float y_offset);
    mx_line_style get_line_style_for_edge(mx_border_style* s, int i);
    void get_coords_for_edge(int& err,
        mx_line_style& s,
        int i,
        mx_point& p1,
        mx_point& p2);
    void draw_line_break(int& err, float x, float y);
    void draw_space_mark(int& err, float x, float y, mx_char_style& s);
    void draw_tab_leader(int& err, float left_x, float right_x, float y,
        mx_tabstop_leader_type_t l, mx_char_style& s);
    void draw_tab_mark(int& err, float left_x, float right_x,
        float y, mx_char_style& s);

    float underline_start_x;
    void draw_underline(int& err, const mx_doc_coord_t& p2,
        mx_char_style& before);

    mx_text_area* current_area;
    mx_paragraph* current_para;

    void highlight_from_start_of_para(
        int& err,
        mx_text_area* a,
        int i,
        mx_para_pos& pos);

    void highlight_in_para(
        int& err,
        mx_text_area* a,
        int i,
        mx_para_pos& start,
        mx_para_pos& end);

    void highlight_whole_para(
        int& err,
        mx_text_area* a,
        int i);

    void highlight_to_end_of_para(
        int& err,
        mx_text_area* a,
        int i,
        mx_para_pos& pos);

    void highlight_from_start_of_line(
        int& err,
        float y,
        mx_paragraph* p,
        mx_para_pos& end);

    void highlight_to_end_of_line(
        int& err,
        float y,
        mx_paragraph* p,
        mx_para_pos& start);

    void highlight_line(
        int& err,
        float y,
        mx_paragraph* p,
        int line);

    float y_offset_for_para(int& err, mx_text_area* a, int i);

    void highlight_height_for_line(
        int& err,
        mx_paragraph* p,
        int line_index,
        float& height,
        float& descender);
};

#endif
