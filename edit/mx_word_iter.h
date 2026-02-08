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
 *  a class that iterates over printable words in a document
 *
 *
 *
 *
 */
#ifndef MX_WORD_ITER_H
#define MX_WORD_ITER_H

#include <mx.h>
#include <mx_rtti.h>
#include <mx_wp_pos.h>

class mx_word;

class mx_wp_document;

class mx_word_iterator //: public mx_rtti
{
    // MX_RTTI(mx_word_iterator_class_e)

public:
    mx_word_iterator(
        mx_wp_document* doc,
        mx_wp_doc_pos& start,
        mx_wp_doc_pos& end,
        bool include_breaks = FALSE);

    mx_word_iterator(const mx_word_iterator& other);

    bool more(int& err);
    mx_word* data(int& err);

    void rewind();
    void moveto(mx_wp_doc_pos& pos) { position = pos; };

    mx_wp_doc_pos start;
    mx_wp_doc_pos end;

    mx_wp_doc_pos word_start;
    mx_wp_doc_pos word_end;

    mx_wp_doc_pos position;

private:
    mx_wp_document* document;

    void back_to_start(int& err);
    void forward_to_end(int& err);

    void increment(int& err);
    void forward_to_word(int& err);

    bool not_used_yet;
    bool reverse;
    bool finished;

    bool include_breaks;
};

#endif
