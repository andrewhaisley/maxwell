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
#ifndef MX_PAINTABLE_H
#define MX_PAINTABLE_H
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
#include <mx_db.h>

class mx_painter;
class mx_doc_rect_t;

class mx_paintable : public mx_db_object {
    MX_RTTI(mx_paintable_class_e)

public:
    mx_paintable(uint32 docid, bool is_controlled = FALSE);
    mx_paintable(uint32 docid, uint32 oid, bool is_controlled = FALSE);
    mx_paintable(const mx_paintable& o);

    // redraw a bit of if - always draws from zero origin & assumes that
    // any transforms and translations have already been set on the
    // output device.
    virtual void redraw(int& err, mx_doc_rect_t& r);

    mx_painter* get_painter() const;

protected:
    mx_painter* painter;
};

#endif
