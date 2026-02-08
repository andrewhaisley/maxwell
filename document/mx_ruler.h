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
#ifndef MX_RULER_H
#define MX_RULER_H
/*
 * MODULE/CLASS : mx_ruler
 *
 * AUTHOR : Andrew Haisley
 *
 *
 *
 * DESCRIPTION:
 *
 * A ruler holds tabs
 *
 *
 *
 */

#include <mx.h>
#include <mx_db.h>

#include <mx_list.h>

// an mx_tabstop_type_t describes what sort of tab you have. mx_automatic is
// returned (never set by the client) by the get_tab functions in the ruler
// class so that the ruler bar can distinguish between an "automatic" and
// a left tab stop. Its behaviour is the same as an mx_left tab.

enum mx_tabstop_type_t {
    mx_automatic, // One of the rulers preset tabs
    mx_left, // Left (Normal) tab stop
    mx_right, // Right tab stop
    mx_centre, // Centres the text on the tab stop if possible
    mx_centre_on_dp, // Centres upon a decimal point
};

enum mx_tabstop_leader_type_t {
    mx_leader_space_e, // whitespace (default)
    mx_leader_dot_e,
    mx_leader_hyphen_e,
    mx_leader_underline_e,
    mx_leader_thickline_e,
    mx_leader_equals_e,
};

#define MX_RULER_DEFAULT_TABSTOP 6.0

class mx_tabstop : public mx_serialisable_object {
    MX_RTTI(mx_tabstop_class_e)

public:
    mx_tabstop_type_t type;
    mx_tabstop_leader_type_t leader_type;
    float position; // in mm

    mx_tabstop(mx_tabstop_type_t t = mx_left, float p = 0.0,
        mx_tabstop_leader_type_t = mx_leader_space_e);

    virtual uint32 get_serialised_size(int& err);
    virtual void serialise(int& err, uint8** buffer);
    virtual void unserialise(int& err, uint8** buffer);
};

/*-------------------------------------------------
 * CLASS:
 *
 * DESCRIPTION:
 *
 * Another "light" class where many of the values are stored as public
 * data members. This is a class for a ruler which just specifies indents
 * and tabstops etc..
 *
 */
class mx_ruler : public mx_serialisable_object {
    MX_RTTI(mx_ruler_class_e)

public:
    float default_tabstop_space; // mm
    float left_indent; // mm
    float right_indent; // mm
    float first_line_indent; // mm

    mx_ruler();
    virtual ~mx_ruler();

    // copy constructor
    mx_ruler(const mx_ruler& rvalue);

    // assignment operator
    void operator=(const mx_ruler& rvalue);
    friend bool operator==(const mx_ruler& r1, const mx_ruler& r2);
    friend bool operator!=(const mx_ruler& r1, const mx_ruler& r2);

    // adds and removes tabs at given positions. an error is generated for
    // removing tabs if there is no tab at the position specified.
    void add_tab(int& err, mx_tabstop& tab);
    void remove_tab(int& err, float position);
    void remove_tab_tolerance(int& err, float position, float t);

    // gets nearest tab to the left/right of a given position. generates
    // an error if there are no more tabs in the direction specified.

    void get_left_tab(int& err, float position, mx_tabstop& result) const;
    void get_right_tab(int& err, float position, mx_tabstop& result) const;

    // do not delete the result of this since it points to an actual tab
    mx_tabstop* get_nearest_tab(float position) const;

    // number of user defined tabs
    int get_num_user_tabs();

    // clear all tabs
    void clear_user_tabs();

    float rightmost_user_tab(int& err);

    uint32 get_serialised_size(int& err);
    void serialise(int& err, unsigned char** buffer);
    void unserialise(int& err, unsigned char** buffer);

    mx_list user_tabs;

private:
};

#endif
