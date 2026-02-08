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
 * MODULE/CLASS : mx_buffer
 *
 * AUTHOR : Andrew Haisley
 *
 *
 *
 * DESCRIPTION:
 *
 * The base class for buffers of all types. This hold the clipboard
 * temporary document that is used to hold bits and prices that have been
 * cut or copied.
 *
 *
 *
 *
 *
 */
#include "mx_buffer.h"
#include <mx.h>
#include <mx_rtti.h>
#include <mx_wp_doc.h>

mx_buffer::mx_buffer(mx_wp_document* d)
    : doc(d)
{
}
mx_buffer::mx_buffer(const mx_buffer& b)
    : doc(b.doc)
{
}
mx_buffer::~mx_buffer() { }

mx_wp_document* mx_buffer::get_document()
{
    return doc;
}

int mx_buffer::get_docid()
{
    return get_document()->get_docid();
}

void mx_buffer::set_document(int& err, mx_wp_document* d)
{
    move_to_document(err, d);
    MX_ERROR_CHECK(err);

    doc = d;
abort:;
}
