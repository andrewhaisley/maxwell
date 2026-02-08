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
#ifndef MX_POSITION_H
#define MX_POSITION_H
/*
 * MODULE/CLASS :  mx_position
 *
 * AUTHOR : Andrew Haisley
 *
 *
 *
 * DESCRIPTION:
 *
 * Things which represent logical positions within bits of a document - or
 * a whole document - must do at least these things.
 *
 *
 *
 *
 *
 */

#include <mx.h>
#include <mx_rtti.h>

class mx_doc_coord_t;

class mx_position : public mx_rtti {
    MX_RTTI(mx_position_class_e);

public:
    mx_position();
    virtual ~mx_position();

    // indicate that this position is before the start of, after the end of,
    // or inside (the usual state) the associated object
    void set_before_start();
    void set_after_end();
    void set_inside();

    // get the state of this position
    bool is_before_start();
    bool is_after_end();
    bool is_inside();

private:
    bool before_start : 1;
    bool after_end : 1;
};

#endif
