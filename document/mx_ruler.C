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
 * MODULE/CLASS : mx_ruler
 *
 * AUTHOR : Andrew Haisley
 *
 *
 *
 * DESCRIPTION:
 *
 * rulers hold tabs
 *
 *
 *
 *
 */
#include <mx.h>
#include <mx_db.h>

#include "mx_ruler.h"
#include <mx_list_iter.h>

// all tabs are the same size when serialised
#define SLS_TABSTOP (SLS_ENUM + SLS_ENUM + SLS_FLOAT)

static int num_ruler_instances = 0;

mx_tabstop::mx_tabstop(mx_tabstop_type_t t, float p,
    mx_tabstop_leader_type_t l)
{
    type = t;
    position = p;
    leader_type = l;
}

uint32 mx_tabstop::get_serialised_size(int& err)
{
    return SLS_TABSTOP;
}

void mx_tabstop::serialise(int& err, uint8** buffer)
{
    serialise_enum(type, buffer);
    serialise_enum(leader_type, buffer);
    serialise_float(position, buffer);
}

void mx_tabstop::unserialise(int& err, uint8** buffer)
{
    uint16 e;

    unserialise_enum(e, buffer);
    type = (mx_tabstop_type_t)e;
    unserialise_enum(e, buffer);
    leader_type = (mx_tabstop_leader_type_t)e;
    unserialise_float(position, buffer);
}

int mx_ruler::get_num_user_tabs()
{
    return user_tabs.get_num_items();
}

void mx_ruler::clear_user_tabs()
{
    mx_list_iterator i(user_tabs);

    while (i.more()) {
        delete (mx_tabstop*)i.data();
    }
    user_tabs = mx_list();
}

mx_ruler::mx_ruler()
{
    default_tabstop_space = MX_RULER_DEFAULT_TABSTOP;
    left_indent = right_indent = first_line_indent = 0.0;

    num_ruler_instances++;
}

// Copy constructor
mx_ruler::mx_ruler(const mx_ruler& rvalue)
{
    mx_tabstop* src_tab;
    mx_list* src_tabs;
    int err = MX_ERROR_OK;

    default_tabstop_space = rvalue.default_tabstop_space;
    left_indent = rvalue.left_indent;
    right_indent = rvalue.right_indent;
    first_line_indent = rvalue.first_line_indent;

    src_tabs = (mx_list*)&rvalue.user_tabs;
    src_tabs->iterate_start();

    while ((src_tab = (mx_tabstop*)src_tabs->iterate_next()) != NULL) {
        add_tab(err, *src_tab);
        MX_ERROR_CHECK(err);
    }
    num_ruler_instances++;
    return;
abort:
    global_error_trace->print();
    MX_ERROR_CLEAR(err);
}

// Assignment operator
void mx_ruler::operator=(const mx_ruler& rvalue)
{
    int err = MX_ERROR_OK;
    if (this != &rvalue) {
        mx_list_iterator iter(*((mx_list*)&rvalue.user_tabs));
        mx_tabstop* tmp_tab;

        default_tabstop_space = rvalue.default_tabstop_space;
        left_indent = rvalue.left_indent;
        right_indent = rvalue.right_indent;
        first_line_indent = rvalue.first_line_indent;

        // Make a new list of new tabstops, removing all the old ones 1st
        while (user_tabs.get_num_items() > 0) {
            tmp_tab = (mx_tabstop*)user_tabs.remove(err, 0);
            MX_ERROR_CHECK(err);

            delete tmp_tab;
        }

        iter.rewind();
        while (iter.more()) {
            add_tab(err, *(mx_tabstop*)iter.data());
            MX_ERROR_CHECK(err);
        }
    }
    return;

abort:
    global_error_trace->print();
    MX_ERROR_CLEAR(err);
}

mx_ruler::~mx_ruler()
{
    mx_tabstop* tab;

    // Delete the tabs in the user tab list
    user_tabs.iterate_start();

    while ((tab = (mx_tabstop*)user_tabs.iterate_next()) != NULL) {
        delete tab;
    }

    num_ruler_instances--;
}

bool operator!=(const mx_ruler& r1, const mx_ruler& r2)
{
    return !(r1 == r2);
}

bool operator==(const mx_ruler& tr1, const mx_ruler& tr2)
{
    mx_ruler& r1 = (mx_ruler&)tr1;
    mx_ruler& r2 = (mx_ruler&)tr2;

    int i, n_tabs, err = MX_ERROR_OK;

    mx_tabstop *t1, *t2;

    if (!MX_FLOAT_EQ(r1.default_tabstop_space, r2.default_tabstop_space) || !MX_FLOAT_EQ(r1.left_indent, r2.left_indent) || !MX_FLOAT_EQ(r1.right_indent, r2.right_indent) || !MX_FLOAT_EQ(r1.first_line_indent, r2.first_line_indent)) {
        return FALSE;
    }

    n_tabs = r1.user_tabs.get_num_items();

    if (r2.user_tabs.get_num_items() != n_tabs) {
        return FALSE;
    }

    for (i = 0; i < n_tabs; i++) {
        t1 = (mx_tabstop*)r1.user_tabs.get(err, i);
        MX_ERROR_CHECK(err);

        t2 = (mx_tabstop*)r2.user_tabs.get(err, i);
        MX_ERROR_CHECK(err);

        if (t1->type != t2->type || !MX_FLOAT_EQ(t1->position, t2->position)) {
            return FALSE;
        }
    }
    return TRUE;

abort:
    global_error_trace->print();
    MX_ERROR_CLEAR(err);
    return FALSE;
}

void mx_ruler::add_tab(int& err, mx_tabstop& tab)
{
    mx_tabstop* tmp_tab;
    bool found;
    int i = 0;

    // Add a tab at the specified position
    found = FALSE;

    // Find the 1st tab to the right of the new one

    for (i = 0; i < user_tabs.get_num_items(); i++) {
        tmp_tab = (mx_tabstop*)user_tabs.get(err, i);
        MX_ERROR_CHECK(err);

        found = (tmp_tab->position >= tab.position);
        if (found)
            break;
    }

    if (!found || !MX_FLOAT_EQ(tmp_tab->position, tab.position)) {
        tmp_tab = new mx_tabstop(tab);

        user_tabs.insert(err, i, tmp_tab);
        MX_ERROR_CHECK(err);
    } else // Already a tab here
    {
        MX_ERROR_THROW(err, MX_DUPLICATE_TABSTOP);
    }

abort:;
}

void mx_ruler::remove_tab(int& err, float position)
{
    mx_tabstop* tmp_tab;
    int i;

    // Find the tab at this position
    for (i = 0; i < user_tabs.get_num_items(); i++) {
        tmp_tab = (mx_tabstop*)user_tabs.get(err, i);
        MX_ERROR_CHECK(err);

        if (MX_FLOAT_EQ(tmp_tab->position, position)) {
            // Remove from list
            user_tabs.remove(err, i);
            MX_ERROR_CHECK(err);
            delete tmp_tab;
            return;
        }
    }
    MX_ERROR_THROW(err, MX_NO_SUCH_TABSTOP);

abort:;
}

// The tabs returned from this function are copies and so
// should be deleted.

void mx_ruler::get_left_tab(int& err, float position, mx_tabstop& result) const
{
    mx_tabstop* tmp_tab = NULL;
    bool found = FALSE;
    int i, pos;
    float default_position;
    mx_list* the_user_tabs = (mx_list*)&user_tabs;

    // If there are no tabs then use the default spacing

    if (the_user_tabs->get_num_items() == 0) {
        // Create a new tabstop to the left or at the requested position
        default_position = ((int)(position / default_tabstop_space)) * default_tabstop_space;

        if (default_position < left_indent && position > left_indent) {
            default_position = left_indent;
        }

        result = mx_tabstop(mx_automatic, default_position,
            mx_leader_space_e);
        return;
    }

    // Find the 1st tab to the right of this position
    for (i = 0; i < the_user_tabs->get_num_items() && !found; i++) {
        tmp_tab = (mx_tabstop*)the_user_tabs->get(err, i);
        MX_ERROR_CHECK(err);

        found = (tmp_tab->position > position) || MX_FLOAT_EQ(tmp_tab->position, position);
    }

    if (found) {
        if (MX_FLOAT_EQ(tmp_tab->position, position)) {
            pos = i - 1;
        } else {
            if (i > 1) {
                pos = i - 2;
            } else {
                MX_ERROR_THROW(err, MX_NO_SUCH_TABSTOP);
            }
        }
    } else {
        MX_ERROR_THROW(err, MX_NO_SUCH_TABSTOP);
    }

    tmp_tab = (mx_tabstop*)the_user_tabs->get(err, pos);
    MX_ERROR_CHECK(err);

    result = *tmp_tab;
abort:;
}

void mx_ruler::get_right_tab(int& err, float position, mx_tabstop& result) const
{
    mx_list* the_user_tabs = (mx_list*)&user_tabs;
    mx_tabstop* tmp_tab = NULL;
    bool found = FALSE;
    int i;

    // if the given position is before the left indent then return the left
    // indent

    if (position < left_indent) {
        result = mx_tabstop(mx_automatic, left_indent, mx_leader_space_e);
        return;
    }

    // Find the 1st tab to the right of this position

    for (i = 0; i < the_user_tabs->get_num_items() && !found; i++) {
        tmp_tab = (mx_tabstop*)the_user_tabs->get(err, i);
        MX_ERROR_CHECK(err);

        found = (tmp_tab->position > position && !(MX_FLOAT_EQ(tmp_tab->position, position)));
    }

    if (found) {
        result = *tmp_tab;
    } else {
        // Get position to left of or AT requested position
        float result_position = ((int)(position / default_tabstop_space)) * default_tabstop_space;

        // If the requested position is between two tab stops then return
        // the position to the right.

        while ((result_position < position) || MX_FLOAT_EQ(result_position, position)) {
            result_position += default_tabstop_space;
        }

        result = mx_tabstop(mx_automatic, result_position,
            mx_leader_space_e);
    }

abort:;
}

uint32 mx_ruler::get_serialised_size(int& err)
{
    return SLS_FLOAT * 4 + SLS_UINT16 + user_tabs.get_num_items() * SLS_TABSTOP;
}

void mx_ruler::serialise(int& err, unsigned char** buffer)
{
    int i, n;
    mx_tabstop* tmp_tab;

    serialise_float(default_tabstop_space, buffer);
    serialise_float(left_indent, buffer);
    serialise_float(right_indent, buffer);
    serialise_float(first_line_indent, buffer);
    serialise_uint16((uint16)user_tabs.get_num_items(), buffer);

    n = user_tabs.get_num_items();

    for (i = 0; i < n; i++) {
        tmp_tab = (mx_tabstop*)user_tabs.get(err, i);
        tmp_tab->serialise(err, buffer);
        MX_ERROR_CHECK(err);
    }
abort:;
}

void mx_ruler::unserialise(int& err, unsigned char** buffer)
{
    uint16 ntabs;
    int i;
    mx_tabstop tmp_tab;

    unserialise_float(default_tabstop_space, buffer);
    unserialise_float(left_indent, buffer);
    unserialise_float(right_indent, buffer);
    unserialise_float(first_line_indent, buffer);
    unserialise_uint16(ntabs, buffer);

    for (i = 0; i < ntabs; i++) {
        tmp_tab.unserialise(err, buffer);
        MX_ERROR_CHECK(err);

        add_tab(err, tmp_tab);
        MX_ERROR_CHECK(err);
    }

abort:;
}

void mx_ruler::remove_tab_tolerance(int& err, float position, float t)
{
    mx_tabstop* tmp_tab;
    int i;

    // Find the tab at this position
    for (i = 0; i < user_tabs.get_num_items(); i++) {
        tmp_tab = (mx_tabstop*)user_tabs.get(err, i);
        MX_ERROR_CHECK(err);

        if (fabs(tmp_tab->position - position) <= t) {
            // Remove from list
            user_tabs.remove(err, i);
            MX_ERROR_CHECK(err);

            delete tmp_tab;
        }
    }

abort:;
}

mx_tabstop* mx_ruler::get_nearest_tab(float position) const
{
    mx_list_iterator iter(*(mx_list*)&user_tabs);
    mx_tabstop *tmp_tab, *res = NULL;

    while (iter.more()) {
        tmp_tab = (mx_tabstop*)iter.data();

        if (res == NULL) {
            res = tmp_tab;
        } else {
            if (fabs(tmp_tab->position - position) < fabs(res->position - position)) {
                res = tmp_tab;
            }
        }
    }

    return res;
}

float mx_ruler::rightmost_user_tab(int& err)
{
    int i;
    mx_tabstop* tmp_tab;

    i = user_tabs.get_num_items();

    if (i == 0) {
        MX_ERROR_THROW(err, MX_NO_SUCH_TABSTOP);
    } else {
        tmp_tab = (mx_tabstop*)user_tabs.get(err, i - 1);
        MX_ERROR_CHECK(err);

        return tmp_tab->position;
    }

abort:
    return 9999;
}
