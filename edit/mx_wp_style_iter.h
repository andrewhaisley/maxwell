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
 * MODULE/CLASS :  mx_wp_style_iter
 *
 * AUTHOR : Andrew Haisley
 *
 *
 *
 * DESCRIPTION:
 *  a class that iterates over things that have styles in a wp document
 *
 *
 *
 *
 */
#ifndef MX_WP_STYLE_ITER_H
#define MX_WP_STYLE_ITER_H

#include <mx_wp_iter.h>

class mx_wp_style_iterator : public mx_wp_iterator {
    // MX_RTTI(mx_wp_style_iterator_class_e)

public:
    mx_wp_style_iterator(
        mx_wp_document* doc,
        mx_wp_doc_pos& start,
        mx_wp_doc_pos& end);

    // there is always at least one thing. This means that the
    // more() function must be used after the data method rather than
    // before:
    //   do
    //     {
    //     x = i.data(err);
    //   }
    //   while (i.more(err));
    //
    virtual bool more(int& err);

    // this returns a pointer to either an area or a paragraph within an
    // area depending on the class of the area. For a text or table area,
    // it always a paragraph. For an image or diagram area, it is the area
    // itself.
    virtual mx_rtti* data(int& err);

protected:
    bool iterating_over_table_area;

    void increment(int& err);
};

#endif
