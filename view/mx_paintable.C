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
 * MODULE/CLASS : mx_paintable
 *
 * AUTHOR : Andrew Haisley
 *
 *
 *
 * DESCRIPTION:
 *
 *   Things which can be painted by painters.
 *
 *
 *
 *
 */

#include <mx.h>
#include <mx_paintable.h>
#include <mx_painter.h>

mx_paintable::mx_paintable(uint32 docid, bool is_controlled)
    : mx_db_object(docid, is_controlled)
{
    painter = NULL;
}

mx_paintable::mx_paintable(uint32 docid, uint32 oid, bool is_controlled)
    : mx_db_object(docid, oid, is_controlled)
{
    painter = NULL;
}

mx_paintable::mx_paintable(const mx_paintable& o)
    : mx_db_object(o)
{
    painter = o.get_painter();
}

void mx_paintable::redraw(int& err, mx_doc_rect_t& r)
{
    if (painter == NULL) {
        err = MX_PAINTABLE_OBJECT_NO_PAINTER;
    } else {
        painter->draw(err, r, *this);
        MX_ERROR_CHECK(err);
    }
abort:;
}

mx_painter* mx_paintable::get_painter() const
{
    return painter;
}
