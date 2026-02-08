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
 * MODULE/CLASS :  mx_word_iter
 *
 * AUTHOR : Andrew Haisley
 *
 *
 *
 * DESCRIPTION:
 *  a class that iterates over printable words
 *
 *
 *
 *
 */
#include <mx.h>
#include <mx_para_pos.h>
#include <mx_paragraph.h>
#include <mx_rtti.h>
#include <mx_word.h>
#include <mx_word_iter.h>
#include <mx_wp_doc.h>

mx_word_iterator::mx_word_iterator(
    mx_wp_document* doc,
    mx_wp_doc_pos& s,
    mx_wp_doc_pos& e,
    bool include_breaks)
{
    start = s;
    end = e;
    reverse = s > e;

    mx_word_iterator::include_breaks = include_breaks;

    document = doc;
    position = start;
    not_used_yet = TRUE;
    finished = FALSE;
}

void mx_word_iterator::rewind()
{
    position = start;
    not_used_yet = TRUE;
    finished = FALSE;
}

void mx_word_iterator::back_to_start(int& err)
{
    if (reverse) {
        end.moveto_start_logical_word(err, document);
        MX_ERROR_CHECK(err);
    } else {
        start.moveto_start_logical_word(err, document);
        MX_ERROR_CHECK(err);
    }

abort:;
}

void mx_word_iterator::forward_to_end(int& err)
{
    if (reverse) {
        start.moveto_end_logical_word(err, document);
        MX_ERROR_CHECK(err);
    } else {
        end.moveto_end_logical_word(err, document);
        MX_ERROR_CHECK(err);
    }

abort:;
}

bool mx_word_iterator::more(int& err)
{
    // may need to wind start position forward to first
    // printable word in the document
    if (not_used_yet) {
        if (start == end) {
            return FALSE;
        }
        not_used_yet = FALSE;
    } else {
        increment(err);
        MX_ERROR_CHECK(err);
    }

    return !finished;

abort:
    return FALSE;
}

mx_word* mx_word_iterator::data(int& err)
{
    mx_word* res = NULL;

    if (reverse) {
        word_start = end;
        word_end = start;
    } else {
        word_start = start;
        word_end = end;
    }

    res = document->get_logical_word_with_limits(err, position, word_start,
        word_end);
    MX_ERROR_CHECK(err);
abort:
    return res;
}

void mx_word_iterator::increment(int& err)
{
    mx_word* w;

    while (TRUE) {
        if (reverse) {
            position.prev_logical_word(err, document);
        } else {
            position.next_logical_word(err, document);
        }

        if (err != MX_ERROR_OK || (reverse && position < end) || (!reverse && position > end)) {
            MX_ERROR_CLEAR(err);
            finished = TRUE;
            return;
        } else {
            w = document->get_word(err, position);
            MX_ERROR_CHECK(err);

            if ((w->get_num_visible_items() > 0) || ((w->is_a(mx_break_word_class_e)) && include_breaks)) {
                return;
            }
        }
    }
abort:;
}

void mx_word_iterator::forward_to_word(int& err)
{
    mx_word* w;

    while (TRUE) {
        w = document->get_word(err, position);
        MX_ERROR_CHECK(err);

        if (w->get_num_visible_items() > 0) {
            return;
        } else {
            if (reverse) {
                position.prev_word(err, document);
            } else {
                position.next_word(err, document);
            }

            if (err != MX_ERROR_OK) {
                MX_ERROR_CLEAR(err);
                finished = TRUE;
            }
        }
    }
abort:;
}

mx_word_iterator::mx_word_iterator(const mx_word_iterator& other)
{
    start = other.start;
    end = other.end;
    word_start = other.word_start;
    word_end = other.word_end;
    position = other.position;
    document = other.document;
    not_used_yet = other.not_used_yet;
    reverse = other.reverse;
    finished = other.finished;
}
