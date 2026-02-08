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
 * MODULE/CLASS : mx_text_buffer
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
#ifndef MX_TEXT_BUFFER_H
#define MX_TEXT_BUFFER_H

#include "mx_buffer.h"
#include <mx.h>
#include <mx_rtti.h>

class mx_text_area;
class mx_text_area_pos;
class mx_list;
class mx_paragraph;
class mx_para_pos;
class mx_word;
class mx_char_style_mod;

class mx_text_buffer : public mx_buffer {
    MX_RTTI(mx_text_buffer_class_e)
public:
    mx_text_buffer(mx_wp_document*);
    mx_text_buffer(const mx_text_buffer&);
    virtual ~mx_text_buffer();

    void cut(int& err,
        mx_text_area& a,
        mx_text_area_pos p1,
        mx_text_area_pos p2,
        mx_edit_op cut_or_copy = mx_cut_e);

    void insert(int& err, mx_text_area& a, mx_text_area_pos& p);

    bool contains_words();

private:
    void clear_old_contents();
    mx_list* contents;

    void cut_from_paragraph(int& err,
        mx_paragraph* para,
        mx_para_pos p1,
        mx_para_pos p2,
        mx_edit_op cut_or_copy);
    void cut_paragraph(int& err, mx_text_area& a, int index,
        mx_edit_op cut_or_copy);
    void cut_word(int& err, mx_paragraph* para, mx_para_pos& pos, int& line,
        int& word, mx_edit_op cut_or_copy);
    void cut_from_word(int& err,
        mx_paragraph* para,
        int line,
        int word,
        int start_letter,
        int end_letter,
        mx_edit_op cut_or_copy);

    bool covers_whole_word(int& err,
        mx_paragraph* para,
        int line,
        int word,
        int start_letter,
        int end_letter);

    void insert_word(int& err, mx_text_area& a, mx_word* w,
        mx_text_area_pos& p);
    void insert_mod(int& err, mx_text_area& a, mx_char_style_mod* m,
        mx_text_area_pos& p);

    void fix_break(int& err, mx_paragraph* para);

    virtual void move_to_document(int& err, mx_wp_document* d);
};

#endif
