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
 * MODULE/CLASS : mx_wp_sheet_pos
 *
 * AUTHOR : Andrew Haisley
 *
 * DESCRIPTION:
 *
 * Positions within a sheet from a WP point of view.
 *
 *
 */

#include <mx.h>
#include <mx_nlist.h>
#include <mx_list_iter.h>
#include <mx_op_pos.h>
#include <mx_ta_pos.h>
#include <mx_txt_pos.h>
#include <mx_wp_doc.h>
#include <mx_wp_sh_pos.h>

mx_wp_sheet_pos::mx_wp_sheet_pos(const mx_wp_sheet_pos& o)
{
    index = o.index;
    area_pos = NULL;

    if (o.area_pos != NULL) {
        area_pos = o.area_pos->duplicate();
    }
}

mx_wp_sheet_pos& mx_wp_sheet_pos::operator=(const mx_wp_sheet_pos& p)
{
    if (&p == this) {
        return *this;
    }

    index = p.index;

    if (p.area_pos != NULL) {
        if (area_pos != NULL) {
            delete area_pos;
        }
        area_pos = p.area_pos->duplicate();
    }

    return *this;
}

// compare logical positions
bool mx_wp_sheet_pos::operator<(const mx_wp_sheet_pos& p)
{
    if (index == p.index) {
        return *area_pos < *(p.area_pos);
    } else {
        return index < p.index;
    }
}

bool mx_wp_sheet_pos::operator>(const mx_wp_sheet_pos& p)
{
    if (index == p.index) {
        return *area_pos > *(p.area_pos);
    } else {
        return index > p.index;
    }
}

bool mx_wp_sheet_pos::operator==(const mx_wp_sheet_pos& p)
{
    return index == p.index && *area_pos == *(p.area_pos);
}

bool mx_wp_sheet_pos::operator!=(const mx_wp_sheet_pos& p)
{
    return index != p.index || *area_pos != *(p.area_pos);
}

bool mx_wp_sheet_pos::operator<=(const mx_wp_sheet_pos& p)
{
    return !(*this > p);
}

bool mx_wp_sheet_pos::operator>=(const mx_wp_sheet_pos& p)
{
    return !(*this < p);
}

void mx_wp_sheet_pos::up(int& err, mx_sheet* s)
{
    mx_area* a = s->get_area(err, index);
    MX_ERROR_CHECK(err);

    area_pos->up(err, a);
    if (err == MX_POSITION_RANGE_ERROR) {
        if (index == 0) {
            MX_ERROR_CHECK(err);
        } else {
            MX_ERROR_CLEAR(err);

            index--;

            a = s->get_area(err, index);
            MX_ERROR_CHECK(err);

            new_area_pos(a);

            area_pos->moveto_end(err, a);
            MX_ERROR_CHECK(err);
        }
    } else {
        MX_ERROR_CHECK(err);
    }

abort:;
}

void mx_wp_sheet_pos::down(int& err, mx_sheet* s)
{
    mx_area* a;
    mx_point p;

    a = s->get_area(err, index);
    MX_ERROR_CHECK(err);

    area_pos->down(err, a);
    if (err == MX_POSITION_RANGE_ERROR) {
        if (index == (s->get_num_areas() - 1)) {
            MX_ERROR_CHECK(err);
        } else {
            MX_ERROR_CLEAR(err);

            index++;

            a = s->get_area(err, index);
            MX_ERROR_CHECK(err);

            new_area_pos(a);

            area_pos->moveto_start(err, a);
            MX_ERROR_CHECK(err);
        }
    } else {
        MX_ERROR_CHECK(err);
    }

abort:;
}

void mx_wp_sheet_pos::left(int& err, mx_sheet* s)
{
    mx_area* a;

    a = s->get_area(err, index);
    MX_ERROR_CHECK(err);

    area_pos->left(err, a);
    if (err == MX_POSITION_RANGE_ERROR) {
        if (index == 0) {
            MX_ERROR_CHECK(err);
        } else {
            MX_ERROR_CLEAR(err);

            index--;

            a = s->get_area(err, index);
            MX_ERROR_CHECK(err);

            new_area_pos(a);

            area_pos->moveto_end(err, a);
        }
    } else {
        MX_ERROR_CHECK(err);
    }

abort:;
}

void mx_wp_sheet_pos::right(int& err, mx_sheet* s)
{
    mx_area* a;

    a = s->get_area(err, index);
    MX_ERROR_CHECK(err);

    area_pos->right(err, a);
    if (err == MX_POSITION_RANGE_ERROR) {
        if (index == (s->get_num_areas() - 1)) {
            MX_ERROR_CHECK(err);
        } else {
            MX_ERROR_CLEAR(err);

            index++;

            a = s->get_area(err, index);
            MX_ERROR_CHECK(err);

            new_area_pos(a);
            area_pos->moveto_start(err, a);
        }
    } else {
        MX_ERROR_CHECK(err);
    }

abort:;
}

// where is the selection within the area - use rtti to figure
// out class of result
mx_area_pos* mx_wp_sheet_pos::get_area_pos(int& err)
{
    return area_pos;
}

void mx_wp_sheet_pos::moveto_start(int& err, mx_sheet* s)
{
    mx_list* areas = s->get_area_index(err);
    mx_area* a;

    index = 0;

    a = (mx_area*)areas->get(err, 0);
    MX_ERROR_CHECK(err);

    new_area_pos(a);

abort:;
}

void mx_wp_sheet_pos::moveto_end(int& err, mx_sheet* s)
{
    mx_list* areas = s->get_area_index(err);
    mx_area* a;

    index = areas->get_num_items() - 1;

    a = (mx_area*)areas->get(err, index);
    MX_ERROR_CHECK(err);

    new_area_pos(a);

    area_pos->moveto_end(err, a);
    MX_ERROR_CHECK(err);

abort:;
}

// which area is the position in - use rtti to figure out class
// of result
mx_area* mx_wp_sheet_pos::area(int& err, mx_sheet* s)
{
    mx_area* a = NULL;

    mx_list* areas = s->get_area_index(err);
    a = (mx_area*)areas->get(err, index);
    MX_ERROR_CHECK(err);

abort:;
    return a;
}

mx_wp_sheet_pos::mx_wp_sheet_pos()
{
    index = 0;
    area_pos = new mx_opaque_area_pos;
}

mx_wp_sheet_pos::~mx_wp_sheet_pos()
{
    if (area_pos != NULL) {
        delete area_pos;
    }
}

int mx_wp_sheet_pos::get_area_index() const
{
    return index;
}

void mx_wp_sheet_pos::back_area(int& err)
{
    if (index <= 0) {
        MX_ERROR_THROW(err, MX_POSITION_RANGE_ERROR);
    } else {
        index--;
    }
abort:;
}

void mx_wp_sheet_pos::forward_area(int& err)
{
    index++;
}

mx_point mx_wp_sheet_pos::position(int& err, mx_sheet* s)
{
    mx_area* a;
    mx_point res;

    a = s->get_area(err, index);
    MX_ERROR_CHECK(err);

    res = a->get_origin(err);
    MX_ERROR_CHECK(err);

    res += area_pos->position(err, a);
    MX_ERROR_CHECK(err);

abort:
    return res;
}

mx_point mx_wp_sheet_pos::position(int& err, mx_sheet* s, int line_offset)
{
    mx_area* a;
    mx_point res;

    a = s->get_area(err, index);
    MX_ERROR_CHECK(err);

    res = a->get_origin(err);
    MX_ERROR_CHECK(err);

    res += area_pos->position(err, a, line_offset);
    MX_ERROR_CHECK(err);

abort:
    return res;
}

void mx_wp_sheet_pos::moveto(int& err, mx_sheet* s, mx_point& p)
{
    mx_list* areas;
    mx_list_iterator* iter = NULL;
    mx_area* a;
    mx_point o, tp;
    mx_rect r;
    bool found = FALSE;
    mx_area* nearest = NULL;
    float dist_to_nearest = 100000000.0;
    int i = 0, nearest_index = 0;

    areas = s->get_area_index(err);
    MX_ERROR_CHECK(err);

    iter = new mx_list_iterator(*areas);

    while (iter->more()) {
        a = (mx_area*)iter->data();

        o = a->get_origin(err);
        MX_ERROR_CHECK(err);

        tp = p;
        tp -= o;

        r = a->bounding_box(err);
        MX_ERROR_CHECK(err);

        // get the right area on the y axis
        if (tp.y <= GMAX(r.yt, r.yb) && tp.y >= GMIN(r.yt, r.yb)) {
            new_area_pos(a);

            area_pos->moveto(err, a, tp);
            MX_ERROR_CHECK(err);

            index = i;

            found = TRUE;

            break;
        } else if (p.dist(o) < dist_to_nearest) {
            dist_to_nearest = p.dist(o);
            nearest = a;
            nearest_index = i;
        }

        i++;
    }

    if (!found) {
        // place in nearest area instead
        new_area_pos(nearest);

        index = nearest_index;

        o = nearest->get_origin(err);
        MX_ERROR_CHECK(err);

        tp = p;
        tp -= o;

        area_pos->moveto(err, nearest, tp);
        MX_ERROR_CHECK(err);
    }

abort:
    if (iter != NULL) {
        delete iter;
    }
}

void mx_wp_sheet_pos::new_area_pos(mx_area* a)
{
    if (area_pos != NULL) {
        delete area_pos;
    }

    switch (a->rtti_class()) {
    case mx_text_area_class_e:
        area_pos = new mx_text_area_pos;
        break;
    case mx_table_area_class_e:
        area_pos = new mx_table_area_pos;
        break;
    default:
        area_pos = new mx_opaque_area_pos;
        break;
    }
}

void mx_wp_sheet_pos::reset(int& err, mx_sheet* s)
{
    mx_area* a = s->get_area(err, index);
    MX_ERROR_CHECK(err);

    new_area_pos(a);
abort:;
}

void mx_wp_sheet_pos::moveto_start(int& err, mx_sheet* s, int area_index)
{
    mx_area* a;

    index = area_index;

    a = s->get_area(err, index);
    MX_ERROR_CHECK(err);

    new_area_pos(a);
abort:;
}

bool mx_wp_sheet_pos::is_end(int& err, mx_sheet* s)
{
    bool res;

    if (index == (s->get_num_areas() - 1)) {
        mx_area* a;

        a = s->get_area(err, index);
        MX_ERROR_CHECK(err);

        res = area_pos->is_end(err, a);
        MX_ERROR_CHECK(err);

        return res;
    } else {
        return FALSE;
    }
abort:
    return FALSE;
}

bool mx_wp_sheet_pos::is_start()
{
    return index == 0 && area_pos->is_start();
}

bool mx_wp_sheet_pos::is_start_line()
{
    return index == 0 && area_pos->is_start_line();
}

bool mx_wp_sheet_pos::is_start_para()
{
    return index == 0 && area_pos->is_start_para();
}

void mx_wp_sheet_pos::prev_area(int& err, mx_sheet* s)
{
    mx_area* a;

    if (index <= 0) {
        MX_ERROR_THROW(err, MX_POSITION_RANGE_ERROR);
    } else {
        index--;
    }

    reset(err, s);
    MX_ERROR_CHECK(err);

    a = area(err, s);
    MX_ERROR_CHECK(err);

    area_pos->moveto_end(err, a);
    MX_ERROR_CHECK(err);
abort:;
}

void mx_wp_sheet_pos::next_area(int& err, mx_sheet* s)
{
    if (index >= (s->get_num_areas() - 1)) {
        MX_ERROR_THROW(err, MX_POSITION_RANGE_ERROR);
    } else {
        index++;
    }

    reset(err, s);
    MX_ERROR_CHECK(err);
abort:;
}

void mx_wp_sheet_pos::make_valid(int& err, mx_sheet* s)
{
    int n;

    if (index >= (n = s->get_num_areas())) {
        moveto_end(err, s);
        MX_ERROR_CHECK(err);
    }
abort:;
}

void mx_wp_sheet_pos::next_word(int& err, mx_sheet* s)
{
    switch (area_pos->rtti_class()) {
    case mx_text_area_pos_class_e: {
        mx_text_area_pos* text_pos = (mx_text_area_pos*)area_pos;
        mx_text_area* text_area = (mx_text_area*)s->get_area(err, index);
        MX_ERROR_CHECK(err);
        MX_ERROR_ASSERT(err, text_area->is_a(mx_text_area_class_e));

        text_pos->next_word(err, text_area);
        break;
    }
    case mx_table_area_pos_class_e: {
        mx_table_area_pos* table_pos = (mx_table_area_pos*)area_pos;
        mx_table_area* table_area = (mx_table_area*)s->get_area(err, index);
        MX_ERROR_CHECK(err);
        MX_ERROR_ASSERT(err, table_area->is_a(mx_table_area_class_e));

        table_pos->next_word(err, table_area);
        break;
    }
    default:
        err = MX_POSITION_RANGE_ERROR;
        break;
    }

    if (err == MX_POSITION_RANGE_ERROR) {
        MX_ERROR_CLEAR(err);

        next_area(err, s);
        MX_ERROR_CHECK(err);
    } else {
        MX_ERROR_CHECK(err);
    }
abort:;
}

void mx_wp_sheet_pos::prev_word(int& err, mx_sheet* s)
{
    switch (area_pos->rtti_class()) {
    case mx_text_area_pos_class_e: {
        mx_text_area_pos* text_pos = (mx_text_area_pos*)area_pos;
        mx_text_area* text_area = (mx_text_area*)s->get_area(err, index);
        MX_ERROR_CHECK(err);
        MX_ERROR_ASSERT(err, text_area->is_a(mx_text_area_class_e));

        text_pos->prev_word(err, text_area);
        break;
    }
    case mx_table_area_pos_class_e: {
        mx_table_area_pos* table_pos = (mx_table_area_pos*)area_pos;
        mx_table_area* table_area = (mx_table_area*)s->get_area(err, index);
        MX_ERROR_CHECK(err);
        MX_ERROR_ASSERT(err, table_area->is_a(mx_table_area_class_e));

        table_pos->prev_word(err, table_area);
        break;
    }
    default:
        err = MX_POSITION_RANGE_ERROR;
        break;
    }

    if (err == MX_POSITION_RANGE_ERROR) {
        MX_ERROR_CLEAR(err);

        prev_area(err, s);
        MX_ERROR_CHECK(err);
    } else {
        MX_ERROR_CHECK(err);
    }
abort:;
}

void mx_wp_sheet_pos::next_para(int& err, mx_sheet* s)
{
    mx_area* a = s->get_area(err, index);
    MX_ERROR_CHECK(err);

    area_pos->next_para(err, a);
    if (err == MX_POSITION_RANGE_ERROR) {
        MX_ERROR_CLEAR(err);

        this->next_area(err, s);
        MX_ERROR_CHECK(err);
    } else {
        MX_ERROR_CHECK(err);
    }
abort:;
}

void mx_wp_sheet_pos::prev_para(int& err, mx_sheet* s)
{
    mx_area* a = s->get_area(err, index);
    MX_ERROR_CHECK(err);

    area_pos->prev_para(err, a);
    if (err == MX_POSITION_RANGE_ERROR) {
        MX_ERROR_CLEAR(err);

        this->prev_area(err, s);
        MX_ERROR_CHECK(err);
    } else {
        MX_ERROR_CHECK(err);
    }
abort:;
}

void mx_wp_sheet_pos::set_area_index(int& err, int i, mx_sheet* s)
{
    index = i;

    reset(err, s);
    MX_ERROR_CHECK(err);

abort:;
}
