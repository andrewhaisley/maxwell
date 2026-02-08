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
 * MODULE/CLASS :  mx_wp_iter
 *
 * AUTHOR : Andrew Haisley
 *
 * DESCRIPTION:
 *  a class that iterates over things a wp document
 *
 *
 */
#ifndef MX_WP_ITER_H
#define MX_WP_ITER_H

#include <mx.h>
#include <mx_rtti.h>
#include <mx_wp_pos.h>

class mx_wp_document;

class mx_wp_iterator //: public mx_rtti
{
    // MX_RTTI(mx_wp_iterator_class_e)

public:
    mx_wp_iterator(
        mx_wp_document* doc,
        mx_wp_doc_pos& start,
        mx_wp_doc_pos& end);

    virtual ~mx_wp_iterator() { };

    // there is always at least one thing. This means that the
    // more() function must be used after the data method rather than
    // before:
    //   do
    //     {
    //     x = i.data(err);
    //   }
    //   while (i.more(err));
    //
    virtual bool more(int& err) = 0;
    virtual mx_rtti* data(int& err) = 0;

    void rewind();

    mx_wp_doc_pos start;
    mx_wp_doc_pos end;

    mx_wp_doc_pos position;

protected:
    mx_wp_document* document;

    void increment_area(int& err);

private:
    void back_to_start(int& err);
    void forward_to_end(int& err);
};

#endif
