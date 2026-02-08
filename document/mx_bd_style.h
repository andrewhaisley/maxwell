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
#ifndef MX_BORDER_STYLE_H
#define MX_BORDER_STYLE_H
/*
 * MODULE/CLASS : mx_border_style & mx_border_style_mod
 *
 * AUTHOR : Andrew Haisley
 *
 *
 *
 * DESCRIPTION:
 *
 * Border style
 *
 *
 *
 */

#include <mx_db.h>

#include "mx_fill_style.h"
#include "mx_line_style.h"

class mx_border_style;
class mx_border_style_mod;

class mx_border_style : public mx_serialisable_object {
    MX_RTTI(mx_border_style_class_e)

    friend class mx_border_style_mod;

public:
    mx_line_style top_style;
    mx_line_style bottom_style;
    mx_line_style left_style;
    mx_line_style right_style;
    mx_fill_style fill_style;

    float distance_from_contents; // in MM
    int32 border_id;

    mx_border_style();
    virtual ~mx_border_style();

    uint32 get_serialised_size(int& err);
    void serialise(int& err, uint8** buffer);
    void unserialise(int& err, uint8** buffer);

    // copy constructor
    mx_border_style(const mx_border_style& rvalue);

    // assignment operator
    mx_border_style& operator=(const mx_border_style& rvalue);

    // == operator
    friend bool operator==(const mx_border_style& s1, const mx_border_style& s2);
    friend bool operator!=(const mx_border_style& s1, const mx_border_style& s2);

    // apply a border style modification
    mx_border_style& operator+=(const mx_border_style_mod& m);
};

class mx_border_style_mod : public mx_serialisable_object {
    friend class mx_border_style;

    MX_RTTI(mx_border_style_mod_class_e)

public:
    mx_border_style_mod();
    virtual ~mx_border_style_mod();
    mx_border_style_mod(const mx_border_style& s1, const mx_border_style& s2);
    mx_border_style_mod(const mx_border_style_mod& s);

    // set all mods to TRUE
    mx_border_style_mod(const mx_border_style& s);

    mx_border_style_mod& operator=(const mx_border_style_mod& other);
    mx_border_style_mod& operator+=(const mx_border_style_mod& other);

    void serialise(int& err, uint8** buffer);
    void unserialise(int& err, uint8** buffer);
    uint32 get_serialised_size(int& err);

    // is this a null mod?
    bool is_null() const;

    // does this mod modify a given border style
    bool modifies(const mx_border_style& bs) const;

    // clears any mod flags in this mod which are set in the other_mod
    void clear_mods_in(const mx_border_style_mod& other_mod);

    // get/set/clear individual mods

    bool get_top_style_mod(mx_line_style& new_ts) const;
    bool get_bottom_style_mod(mx_line_style& new_bs) const;
    bool get_left_style_mod(mx_line_style& new_ls) const;
    bool get_right_style_mod(mx_line_style& new_rs) const;
    bool get_fill_style_mod(mx_fill_style& new_fs) const;
    bool get_horizontal_style_mod(mx_line_style& new_hs) const;
    bool get_vertical_style_mod(mx_line_style& new_vs) const;
    bool get_distance_from_contents_mod(float& new_dfc) const;

    void set_top_style_mod(const mx_line_style& new_ts);
    void set_bottom_style_mod(const mx_line_style& new_bs);
    void set_left_style_mod(const mx_line_style& new_ls);
    void set_right_style_mod(const mx_line_style& new_rs);
    void set_fill_style_mod(const mx_fill_style& new_fs);
    void set_horizontal_style_mod(const mx_line_style& new_hs);
    void set_vertical_style_mod(const mx_line_style& new_vs);
    void set_distance_from_contents_mod(float new_dfc);

    void clear_top_style_mod();
    void clear_bottom_style_mod();
    void clear_left_style_mod();
    void clear_right_style_mod();
    void clear_fill_style_mod();
    void clear_horizontal_style_mod();
    void clear_vertical_style_mod();
    void clear_distance_from_contents_mod();

private:
    bool top_style_mod : 1;
    bool bottom_style_mod : 1;
    bool left_style_mod : 1;
    bool right_style_mod : 1;
    bool fill_style_mod : 1;
    bool horizontal_style_mod : 1;
    bool vertical_style_mod : 1;
    bool distance_from_contents_mod : 1;
    mx_line_style top_style;
    mx_line_style bottom_style;
    mx_line_style left_style;
    mx_line_style right_style;
    mx_fill_style fill_style;
    mx_line_style horizontal_style;
    mx_line_style vertical_style;
    float distance_from_contents;
};

#endif
