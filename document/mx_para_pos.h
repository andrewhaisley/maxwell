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
 * MODULE/CLASS :  mx_para_pos
 *
 * AUTHOR : Andrew Haisley
 *
 *
 *
 * DESCRIPTION:
 *
 * Positions within a paragraph
 *
 *
 *
 *
 *
 */
#ifndef MX_PARA_POSITION_H
#define MX_PARA_POSITION_H

#include <mx.h>
#include <mx_position.h>
#include <mx_rtti.h>

class mx_doc_coord_t;
class mx_paragraph;
class mx_point;
class mx_word;
class mx_ruler;

class mx_para_pos : public mx_position {
    MX_RTTI(mx_para_pos_class_e)

public:
    mx_para_pos();
    mx_para_pos(int line_index, int word_index, int letter_index);
    mx_para_pos(const mx_para_pos& p);

    mx_para_pos& operator=(const mx_para_pos& other);

    virtual ~mx_para_pos();

    void up(int& err, mx_paragraph* para);
    void down(int& err, mx_paragraph* para);
    void left(int& err, mx_paragraph* para);
    void right(int& err, mx_paragraph* para);
    void left_word(int& err, mx_paragraph* para);
    void right_word(int& err, mx_paragraph* para);

    // compare logical positions
    bool operator<(const mx_para_pos& p) const;
    bool operator>(const mx_para_pos& p) const;
    bool operator==(const mx_para_pos& p) const;
    bool operator!=(const mx_para_pos& p) const;
    bool operator<=(const mx_para_pos& p) const;
    bool operator>=(const mx_para_pos& p) const;

    bool is_same_word_as(const mx_para_pos& p) const;
    bool is_same_line_as(const mx_para_pos& p) const;

    // move to the end of given paragraph
    void moveto_end(int& err, mx_paragraph* para);

    // move to the end of the given line
    void moveto_end(int& err, mx_paragraph* para, int line);

    // move to the start of a paragraph
    void moveto_start();

    // move to start/end of the word
    void moveto_start_word();
    void moveto_end_word(int& err, mx_paragraph* para);

    // move to position
    void moveto(int& err, mx_paragraph* para, mx_point& p);

    // move to next/previous word - may give range error
    void next_word(int& err, mx_paragraph* para);
    void prev_word(int& err, mx_paragraph* para);

    // move to next logical word. Sets the boolean reference to TRUE if the
    // word is continued in the next paragraph. Throws an error if there are no
    // more words after the current one in this paragraph.
    void next_logical_word(int& err, mx_paragraph* para,
        bool& word_continues_on_next_para);

    // move to previous logical word. Sets the boolean reference to TRUE if the
    // previous paragraph needs to be checked to see if the first word in this
    // paragraph is a continuation of a long word which started in the previous
    // paragraph. Throws an error if there are no more words before the current
    // one in this paragraph.
    void prev_logical_word(int& err, mx_paragraph* para,
        bool& check_prev_para_for_word_continuation);

    bool is_end(int& err, mx_paragraph* para);
    bool is_start();

    mx_point position(int& err, mx_paragraph* para);
    mx_point position(int& err, mx_paragraph* para, int line_offset);

    int line_index;
    int word_index;
    int letter_index;

    void get_distance_from_start(int& err, mx_paragraph* para, int& letter_index);
    void set_distance_from_start(int& err, mx_paragraph* para, int letter_index);

private:
    void position_on_line(int& err, mx_paragraph* para, int n, float x);
    void position_in_word(mx_word* w, float x, mx_ruler* r);
};

#endif
