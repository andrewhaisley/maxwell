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
#ifndef MX_FILL_STYLE_H
#define MX_FILL_STYLE_H
/*
 * MODULE/CLASS : mx_fill_style
 *
 * AUTHOR : Andrew Haisley
 *
 *
 *
 * DESCRIPTION:
 *
 * styles for filling areas
 *
 *
 *
 *
 *
 */

#include <mx.h>
#include <mx_colour.h>
#include <mx_db.h>
#include <mx_rtti.h>

enum mx_fill_type_t {
    mx_fill_transparent,
    mx_fill_colour,
    mx_use_fstyle
};

class mx_fill_style : public mx_serialisable_object {
    // MX_RTTI(mx_fill_style_class_e)
public:
    mx_fill_style();
    mx_fill_style(mx_fill_type_t t);
    mx_fill_style(const char* colour_name);
    mx_fill_style(const mx_fill_style& s);
    virtual ~mx_fill_style();

    mx_fill_type_t type;
    mx_colour colour;
    int32 colour_id;
    int32 fill_id;

    void operator=(const mx_fill_style& other);

    friend bool operator==(const mx_fill_style& s1, const mx_fill_style& s2);
    friend bool operator!=(const mx_fill_style& s1, const mx_fill_style& s2);

    void setStyleChanges(bool setNew,
        mx_fill_style& newStyle,
        bool& setFill);

    uint32 get_serialised_size(int& err);
    void serialise(int& err, uint8** buffer);
    void unserialise(int& err, uint8** buffer);
};
#endif
