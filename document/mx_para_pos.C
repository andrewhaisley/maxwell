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

#include "mx_para_pos.h"
#include <mx_break_w.h>
#include <mx_geometry.h>
#include <mx_nlist.h>
#include <mx_paragraph.h>
#include <mx_space_w.h>
#include <mx_word.h>

mx_para_pos::mx_para_pos()
{
    line_index = 0;
    word_index = 0;
    letter_index = 0;
}

mx_para_pos::~mx_para_pos()
{
}

mx_para_pos::mx_para_pos(int line_index, int word_index, int letter_index)
{
    mx_para_pos::line_index = line_index;
    mx_para_pos::word_index = word_index;
    mx_para_pos::letter_index = letter_index;
}

mx_para_pos::mx_para_pos(const mx_para_pos& p)
    : mx_position((const mx_position&)p)
{
    line_index = p.line_index;
    word_index = p.word_index;
    letter_index = p.letter_index;
}

void mx_para_pos::up(int& err, mx_paragraph* para)
{
    if (line_index == 0) {
        MX_ERROR_THROW(err, MX_POSITION_RANGE_ERROR);
    } else {
        line_index--;
        word_index = 0;
        letter_index = 0;
    }
abort:;
}

void mx_para_pos::position_on_line(int& err, mx_paragraph* para, int line, float x)
{
    int i, n;
    mx_word *w, *next;

    n = para->words.get_num_items(err, line);
    MX_ERROR_CHECK(err);

    x -= para->get_paragraph_style()->get_ruler()->left_indent;

    w = (mx_word*)para->words.item(err, line, 0);
    MX_ERROR_CHECK(err);

    if (x <= w->x) {
        // is the position given before the start of the line?
        word_index = 0;
        letter_index = 0;
        return;
    }

    for (i = 0; i < (n - 1); i++) {
        w = (mx_word*)para->words.item(err, line, i);
        MX_ERROR_CHECK(err);

        next = (mx_word*)para->words.item(err, line, i + 1);
        MX_ERROR_CHECK(err);

        if (x >= w->x && x < next->x) {
            // it's in this word somewhere
            word_index = i;
            position_in_word(w, x, para->get_paragraph_style()->get_ruler());
            return;
        }
    }

    // the position is at the end of the line
    word_index = (n - 1);
    w = (mx_word*)para->words.item(err, line, word_index);
    MX_ERROR_CHECK(err);
    letter_index = w->get_num_visible_items();
    if (letter_index > 0)
        letter_index--;
abort:;
}

void mx_para_pos::down(int& err, mx_paragraph* para)
{
    int n = para->num_lines() - 1;

    if (line_index == n) {
        MX_ERROR_THROW(err, MX_POSITION_RANGE_ERROR);
    } else {
        line_index++;
        word_index = 0;
        letter_index = 0;
    }
abort:;
}

void mx_para_pos::left(int& err, mx_paragraph* para)
{
    mx_word* w;

    if (letter_index == 0) {
        if (word_index == 0) {
            if (line_index == 0) {
                MX_ERROR_THROW(err, MX_POSITION_RANGE_ERROR);
            } else {
                line_index--;

                word_index = para->words.get_num_items(err, line_index) - 1;
                MX_ERROR_CHECK(err);

                w = (mx_word*)para->words.item(err, line_index, word_index);
                MX_ERROR_CHECK(err);

                letter_index = w->get_num_visible_items();
                if (letter_index > 0) {
                    letter_index--;
                }
            }
        } else {
            word_index--;

            w = (mx_word*)para->words.item(err, line_index, word_index);
            MX_ERROR_CHECK(err);

            letter_index = w->get_num_visible_items();
            if (letter_index > 0) {
                letter_index--;
            }
        }
    } else {
        letter_index--;
    }
abort:;
}

void mx_para_pos::right(int& err, mx_paragraph* para)
{
    mx_word* w;

    int num_words, num_lines;

    w = (mx_word*)para->words.item(err, line_index, word_index);
    MX_ERROR_CHECK(err);

    num_words = para->words.get_num_items(err, line_index);
    MX_ERROR_CHECK(err);

    num_lines = para->num_lines();

    if (w->get_num_visible_items() == 0 || letter_index == (w->get_num_visible_items() - 1)) {
        if (word_index == (num_words - 1)) {
            if (line_index == (num_lines - 1)) {
                MX_ERROR_THROW(err, MX_POSITION_RANGE_ERROR);
            } else {
                line_index++;
                word_index = letter_index = 0;
            }
        } else {
            word_index++;
            letter_index = 0;
        }
    } else {
        letter_index++;
    }
abort:;
}

mx_para_pos& mx_para_pos::operator=(const mx_para_pos& other)
{
    line_index = other.line_index;
    word_index = other.word_index;
    letter_index = other.letter_index;
    return *this;
}

void mx_para_pos::moveto_end(int& err, mx_paragraph* para)
{
    line_index = para->num_lines() - 1;

    if (line_index >= 0) {
        word_index = para->num_words(err, line_index) - 1;
        MX_ERROR_CHECK(err);
    } else {
        word_index = 0;
    }

    // the last word is always a break word which can only have one
    // thing in it.
    letter_index = 0;

abort:;
}

void mx_para_pos::moveto_start()
{
    line_index = 0;
    word_index = 0;
    letter_index = 0;
}

bool mx_para_pos::is_start()
{
    return line_index == 0 && word_index == 0 && letter_index == 0;
}

bool mx_para_pos::is_end(int& err, mx_paragraph* para)
{
    int n;

    if (line_index == para->num_lines() - 1) {
        n = para->num_words(err, line_index);
        MX_ERROR_CHECK(err);

        if (word_index == (n - 1)) {
            return letter_index == 0;
        } else {
            return FALSE;
        }
    } else {
        return FALSE;
    }

abort:
    return FALSE;
}

mx_point mx_para_pos::position(int& err, mx_paragraph* para)
{
    mx_point res;
    mx_word* w;

    w = (mx_word*)para->words.item(err, line_index, word_index);
    MX_ERROR_CHECK(err);

    res.y = w->y;

    if (w->is_a(mx_space_word_class_e)) {
        mx_space_word* sw = (mx_space_word*)w;
        res.x = sw->x + sw->calculated_width(letter_index);
    } else {
        res.x = w->x + w->width(letter_index);
    }

    res.x += para->get_paragraph_style()->get_ruler()->left_indent;

abort:
    return res;
}

void mx_para_pos::position_in_word(mx_word* w, float x, mx_ruler* r)
{
    int i, n;
    float dist = 10000000.0;
    float f, tmp_f;
    bool got_space_word = w->is_a(mx_space_word_class_e);
    mx_space_word* sw = (mx_space_word*)w;

    letter_index = 0;
    n = w->get_num_visible_items();
    for (i = 0; i <= n; i++) {
        if (got_space_word) {
            f = sw->calculated_width(i) + sw->x;
        } else {
            f = w->width(i) + w->x;
        }

        tmp_f = fabs(x - f);
        if (tmp_f < dist) {
            dist = tmp_f;
            letter_index = i;
        } else {
            if (letter_index == n) {
                word_index++;
                letter_index = 0;
            }
            return;
        }
    }

    if (letter_index >= n) {
        word_index++;
        letter_index = 0;
    }
}

void mx_para_pos::left_word(int& err, mx_paragraph* para)
{
    mx_word* w;

    if (word_index == 0) {
        if (line_index == 0) {
            MX_ERROR_THROW(err, MX_POSITION_RANGE_ERROR);
        } else {
            line_index--;

            word_index = para->words.get_num_items(err, line_index) - 1;
            MX_ERROR_CHECK(err);

            w = (mx_word*)para->words.item(err, line_index, word_index);
            MX_ERROR_CHECK(err);

            letter_index = w->get_num_visible_items();
            if (letter_index > 0) {
                letter_index--;
            }
        }
    } else {
        word_index--;

        w = (mx_word*)para->words.item(err, line_index, word_index);
        MX_ERROR_CHECK(err);

        letter_index = w->get_num_visible_items();
        if (letter_index > 0) {
            letter_index--;
        }
    }
abort:;
}

void mx_para_pos::right_word(int& err, mx_paragraph* para)
{
    int num_words, num_lines;

    (void)para->words.item(err, line_index, word_index);
    MX_ERROR_CHECK(err);

    num_words = para->words.get_num_items(err, line_index);
    MX_ERROR_CHECK(err);

    num_lines = para->num_lines();

    if (word_index == (num_words - 1)) {
        if (line_index == (num_lines - 1)) {
            MX_ERROR_THROW(err, MX_POSITION_RANGE_ERROR);
        } else {
            line_index++;
            word_index = letter_index = 0;
        }
    } else {
        word_index++;
        letter_index = 0;
    }
abort:;
}

void mx_para_pos::moveto(int& err, mx_paragraph* para, mx_point& p)
{
    // find the line index
    int n = para->num_lines();
    float y;

    line_index = -1;

    do {
        y = para->line_ypos(err, ++line_index);
        MX_ERROR_CHECK(err);
    } while (line_index < (n - 1) && p.y > y);

    position_on_line(err, para, line_index, p.x);
    MX_ERROR_CHECK(err);
abort:;
}

bool mx_para_pos::operator<(const mx_para_pos& p) const
{
    if (line_index != p.line_index) {
        return line_index < p.line_index;
    } else {
        if (word_index != p.word_index) {
            return word_index < p.word_index;
        } else {
            return letter_index < p.letter_index;
        }
    }
}

bool mx_para_pos::operator>(const mx_para_pos& p) const
{
    if (line_index != p.line_index) {
        return line_index > p.line_index;
    } else {
        if (word_index != p.word_index) {
            return word_index > p.word_index;
        } else {
            return letter_index > p.letter_index;
        }
    }
}

bool mx_para_pos::operator==(const mx_para_pos& p) const
{
    return line_index == p.line_index && word_index == p.word_index && letter_index == p.letter_index;
}

bool mx_para_pos::operator!=(const mx_para_pos& p) const
{
    return line_index != p.line_index || word_index != p.word_index || letter_index != p.letter_index;
}

bool mx_para_pos::operator<=(const mx_para_pos& p) const
{
    return !(*this > p);
}

bool mx_para_pos::operator>=(const mx_para_pos& p) const
{
    return !(*this < p);
}

bool mx_para_pos::is_same_word_as(const mx_para_pos& p) const
{
    return (line_index == p.line_index) && (word_index == p.word_index);
}

bool mx_para_pos::is_same_line_as(const mx_para_pos& p) const
{
    return line_index == p.line_index;
}

void mx_para_pos::get_distance_from_start(int& err, mx_paragraph* para, int& letter)
{
    int line = 0, word = 0, nw;

    mx_word* w;
    mx_para_pos pp;

    letter = 0;

    if (line_index == 0 && word_index == 0) {
        letter = letter_index;
        return;
    }

    nw = para->num_words(err, 0);
    MX_ERROR_CHECK(err);

    while (line != line_index || word != word_index) {
        pp.line_index = line;
        pp.word_index = word;

        w = para->get_word(err, pp);
        if (err == MX_LIST_INDEX) {
            MX_ERROR_CLEAR(err);
            return;
        }
        MX_ERROR_CHECK(err);

        letter += w->get_num_visible_items();

        if (w->rtti_class() == mx_break_word_class_e && ((mx_break_word*)w)->type() == mx_line_break_e) {
            letter++;
        }

        word++;
        if (word == nw) {
            if (line == line_index) {
                break;
            } else {
                word = 0;
                line++;

                nw = para->num_words(err, line);
                if (err == MX_LIST_INDEX) {
                    MX_ERROR_CLEAR(err);
                    return;
                }
                MX_ERROR_CHECK(err);
            }
        }
    }

    letter += letter_index;

abort:;
}

void mx_para_pos::set_distance_from_start(int& err, mx_paragraph* para, int letter)
{
    int line, word, nw, nl, num_items;
    mx_word* w;
    mx_para_pos pp;

    word_index = 0;
    letter_index = 0;
    line_index = 0;

    if (letter == 0) {
        return;
    }

    nl = para->num_lines();

    for (line = 0; line < nl; line++) {
        nw = para->num_words(err, line);
        MX_ERROR_CHECK(err);

        for (word = 0; word < nw; word++) {
            pp.line_index = line;
            pp.word_index = word;

            w = para->get_word(err, pp);
            MX_ERROR_CHECK(err);

            num_items = w->get_num_visible_items();

            if (w->rtti_class() == mx_break_word_class_e && ((mx_break_word*)w)->type() == mx_line_break_e) {
                num_items = 1;
            }

            if ((letter - num_items) < 0) {
                word_index = word;
                line_index = line;
                letter_index = letter;
                return;
            } else {
                letter -= num_items;
            }
        }
    }

    moveto_end(err, para);
    MX_ERROR_CHECK(err);

abort:;
}

mx_point mx_para_pos::position(int& err, mx_paragraph* para, int line_offset)
{
    mx_point res;
    mx_word* w;
    int li;

    li = line_index + line_offset;

    if (li < 0) {
        res.y = 0.0;
    } else {
        w = (mx_word*)para->words.item(err, li, 0);
        MX_ERROR_CHECK(err);

        res.y = w->y;
    }

    res.x = para->get_paragraph_style()->get_ruler()->left_indent;

abort:
    return res;
}

void mx_para_pos::moveto_end(int& err, mx_paragraph* para, int line)
{
    mx_word* w;
    int ni;

    ni = para->words.get_num_items(err, line);
    MX_ERROR_CHECK(err);

    w = (mx_word*)para->words.item(err, line, ni - 1);
    MX_ERROR_CHECK(err);

    line_index = line;
    word_index = ni - 1;
    letter_index = w->get_num_visible_items();
    if (letter_index > 0) {
        letter_index--;
    }
abort:;
}

void mx_para_pos::moveto_start_word()
{
    letter_index = 0;
}

void mx_para_pos::moveto_end_word(int& err, mx_paragraph* para)
{
    mx_word* w;

    w = para->get_word(err, *this);
    MX_ERROR_CHECK(err);

    letter_index = w->get_num_visible_items();

abort:;
}

void mx_para_pos::next_word(int& err, mx_paragraph* para)
{
    right_word(err, para);
    MX_ERROR_CHECK(err);
abort:;
}

void mx_para_pos::prev_word(int& err, mx_paragraph* para)
{
    left_word(err, para);
    MX_ERROR_CHECK(err);
abort:;
}

void mx_para_pos::next_logical_word(int& err, mx_paragraph* para,
    bool& word_continues_on_next_para)
{
    bool reached_end;
    word_continues_on_next_para = FALSE;

    mx_word* word = para->get_word(err, *this);
    MX_ERROR_CHECK(err);

    if (word->can_be_part_of_long_word()) {
        do {
            this->right_word(err, para);
            MX_ERROR_CHECK(err);

            word = para->get_word(err, *this);
            MX_ERROR_CHECK(err);
        } while (word->can_be_part_of_long_word());
    } else {
        this->right_word(err, para);
        MX_ERROR_CHECK(err);
    }

    reached_end = this->is_end(err, para);
    MX_ERROR_CHECK(err);

    if (reached_end) {
        word_continues_on_next_para = para->has_split_long_word_at_end(err);
        MX_ERROR_CHECK(err);

        if (!word_continues_on_next_para && (para->get_type() == mx_paragraph::mx_paragraph_start_e || para->get_type() == mx_paragraph::mx_paragraph_middle_e)) {
            // we don't want to count the break words for paragraphs which have
            // been split..
            MX_ERROR_THROW(err, MX_POSITION_RANGE_ERROR);
        }
    }
abort:;
}

void mx_para_pos::prev_logical_word(int& err, mx_paragraph* para,
    bool& check_prev_para_for_word_continuation)
{
    check_prev_para_for_word_continuation = FALSE;

    mx_word* word = para->get_word(err, *this);
    MX_ERROR_CHECK(err);

    if (word->can_be_part_of_long_word()) {
        mx_para_pos pp = *this;
        pp.moveto_start_word();

        while (!pp.is_start() && word->can_be_part_of_long_word()) {
            pp.left_word(err, para);
            MX_ERROR_CHECK(err);

            word = para->get_word(err, pp);
            MX_ERROR_CHECK(err);
            pp.moveto_start_word();
        }
        *this = pp;

        if (word->can_be_part_of_long_word()) {
            // in this case we've finished looping because we've come to the
            // start of the paragraph

            check_prev_para_for_word_continuation = (para->get_type() == mx_paragraph::mx_paragraph_middle_e || para->get_type() == mx_paragraph::mx_paragraph_end_e);

            if (!check_prev_para_for_word_continuation) {
                MX_ERROR_THROW(err, MX_POSITION_RANGE_ERROR);
            }
        }
    } else {
        this->left_word(err, para);
        MX_ERROR_CHECK(err);
    }
abort:;
}
