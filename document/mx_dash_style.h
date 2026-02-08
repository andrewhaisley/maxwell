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
#ifndef MX_DASH_STYLE_H
#define MX_DASH_STYLE_H

/*
 * MODULE : mx_dash_style.h
 *
 * AUTHOR : David Miller
 *
 * This file
 *
 * DESCRIPTION:
 * Module mx_dash_style.h
 *
 *
 * Revision 1.1  1996/10/06 13:15:45  andrew
 * Initial revision
 *
 *
 */

#include "mx_sl_obj.h"

/*-------------------------------------------------
 * CLASS: mx_dash_style
 *
 * DESCRIPTION: Dash styles
 *
 *
 */

class mx_dash_style : public mx_serialisable_object {
    MX_RTTI(mx_dash_style_class_e)

public:
    mx_dash_style();
    ~mx_dash_style();

    void init();

    uint32 get_serialised_size(int& err);
    void serialise(int& err, uint8** buffer);
    void unserialise(int& err, uint8** buffer);

    void set(int32 ndashes, const float* dashes, float idash_offset);
    inline int32 get_ndashes() { return ndashes; };
    inline float* get_dashes() { return dashes; };
    inline float get_dash_offset() { return dash_offset; };
    inline int32 get_dash_id() { return dash_id; };
    inline void set_dash_id(int32 idash_id) { dash_id = idash_id; };

    friend bool operator==(const mx_dash_style& s1, const mx_dash_style& s2);
    friend bool operator!=(const mx_dash_style& s1, const mx_dash_style& s2);

    mx_dash_style& operator=(const mx_dash_style& dash_style);

    void set(int32 indashes,
        const float* idashes,
        float idash_offset,
        int32 idash_id);

protected:
private:
    int32 dash_id;
    float dash_offset;
    int32 ndashes;
    float* dashes;
};

#endif
