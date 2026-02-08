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
 * MODULE/CLASS :  mx_grid_paint
 *
 * AUTHOR : Andrew Haisley
 *
 *
 *
 * DESCRIPTION:
 *
 * A class for drawing grids of horizontal and vertical lines. This is harder
 * than it sounds due to having to get the corners right.
 *
 *
 *
 *
 *
 */
#include <math.h>

#include <mx.h>
#include <mx_device.h>
#include <mx_doc_coord.h>
#include <mx_frame.h>
#include <mx_grid_paint.h>
#include <mx_rtti.h>

mx_grid_paint::mx_grid_paint()
{
}

mx_grid_paint::~mx_grid_paint()
{
    int err = MX_ERROR_OK;

    reset(err);
    MX_ERROR_CLEAR(err);
}

void mx_grid_paint::reset(int& err)
{
    mx_grid_paint_line_t* t;

    while (vertical_lines.get_num_items() != 0) {
        t = (mx_grid_paint_line_t*)vertical_lines.remove(err, 0);
        MX_ERROR_CHECK(err);

        delete t;
    }

    while (horizontal_lines.get_num_items() != 0) {
        t = (mx_grid_paint_line_t*)horizontal_lines.remove(err, 0);
        MX_ERROR_CHECK(err);

        delete t;
    }

abort:;
}

void mx_grid_paint::add(int& err, mx_line_style& ls, mx_point& p1, mx_point& p2)
{
    mx_point offset;

    offset = device->getFrame()->getDocFalseOrigin(err).p;
    MX_ERROR_CHECK(err);

    p1 += offset;
    p2 += offset;

    if (MX_FLOAT_EQ(p1.x, p2.x)) {
        add_vertical(err, ls, p1, p2);
        MX_ERROR_CHECK(err);
    } else {
        add_horizontal(err, ls, p1, p2);
        MX_ERROR_CHECK(err);
    }
abort:;
}

void mx_grid_paint::draw(int& err, mx_device& drawable)
{
    while (horizontal_lines.get_num_items() > 0 || vertical_lines.get_num_items() > 0) {
        draw_chain(err, drawable);
        MX_ERROR_CHECK(err);
    }
abort:;
}

mx_grid_paint_line_t* mx_grid_paint::get_joining_line(mx_grid_paint_line_t* t)
{
    if (MX_FLOAT_EQ(t->p1.x, t->p2.x)) {
        // line is vertical -> joining line must be horizontal
        return get_joining_line(t, horizontal_lines);
    } else {
        // etc
        return get_joining_line(t, vertical_lines);
    }
}

mx_grid_paint_line_t* mx_grid_paint::get_joining_line(mx_grid_paint_line_t* t1, mx_list& lines)
{
    int err = MX_ERROR_OK, i;
    int n = lines.get_num_items();
    float vt, ht;

    mx_grid_paint_line_t* t2;

    for (i = 0; i < n; i++) {
        t2 = (mx_grid_paint_line_t*)lines.get(err, i);
        MX_ERROR_CHECK(err);

        // check style is same...
        if (t1->s.line_type == t2->s.line_type && MX_FLOAT_EQ(t1->s.width, t2->s.width) && t1->s.colour == t2->s.colour) {
            find_tolerance(t1, t2, ht, vt);

            if (points_match(t1->p1, t2->p1, vt, ht) || points_match(t1->p1, t2->p2, vt, ht) || points_match(t1->p2, t2->p1, vt, ht) || points_match(t1->p2, t2->p2, vt, ht)) {
                (void)lines.remove(err, i);
                MX_ERROR_CHECK(err);

                return t2;
            }
        }
    }
    return NULL;
abort:;
    MX_ERROR_CLEAR(err);
    return NULL;
}

void mx_grid_paint::find_tolerance(
    mx_grid_paint_line_t* t1,
    mx_grid_paint_line_t* t2,
    float& ht,
    float& vt)
{
    if (MX_FLOAT_EQ(t1->p1.x, t1->p2.x)) {
        ht = t1->s.width / 2;
        vt = t2->s.width / 2;
    } else {
        ht = t2->s.width / 2;
        vt = t1->s.width / 2;
    }
    ht *= 1.1;
    vt *= 1.1;
}

bool mx_grid_paint::points_match(
    mx_point& p1,
    mx_point& p2,
    float vertical_tolerance,
    float horizontal_tolerance)
{
    return (fabs(p1.x - p2.x) < horizontal_tolerance) && (fabs(p1.y - p2.y) < vertical_tolerance);
}

void mx_grid_paint::draw_chain(int& err, mx_device& drawable)
{
    mx_list lines;

    mx_grid_paint_line_t* t;

    if (horizontal_lines.get_num_items() == 0) {
        t = (mx_grid_paint_line_t*)vertical_lines.remove(err, 0);
        MX_ERROR_CHECK(err);
    } else {
        t = (mx_grid_paint_line_t*)horizontal_lines.remove(err, 0);
        MX_ERROR_CHECK(err);
    }

    lines.append(t);

    while ((t = get_joining_line(t)) != NULL) {
        lines.append(t);
    }

    // now the same from the start of the list
    t = (mx_grid_paint_line_t*)lines.get(err, 0);
    MX_ERROR_CHECK(err);

    while ((t = get_joining_line(t)) != NULL) {
        lines.insert(err, 0, t);
        MX_ERROR_CHECK(err);
    }

    draw_line_list(err, drawable, lines);
    MX_ERROR_CHECK(err);

abort:;
}

void mx_grid_paint::fix_line_list(int& err, mx_list& lines)
{
    int i, n;

    mx_grid_paint_line_t* t1;
    mx_grid_paint_line_t* t2;
    mx_point temp;

    float ht, vt;

    n = lines.get_num_items();

    if (n <= 1) {
        return;
    }

    // fix the first line - make sure that p1 is not the point that
    // matches the start of the next line
    t1 = (mx_grid_paint_line_t*)lines.get(err, 0);
    MX_ERROR_CHECK(err);

    t2 = (mx_grid_paint_line_t*)lines.get(err, 1);
    MX_ERROR_CHECK(err);

    find_tolerance(t1, t2, ht, vt);

    if (points_match(t1->p1, t2->p1, vt, ht) || points_match(t1->p1, t2->p2, vt, ht)) {
        temp = t1->p1;
        t1->p1 = t1->p2;
        t1->p2 = temp;
    }

    for (i = 0; i < (n - 1); i++) {
        t1 = (mx_grid_paint_line_t*)lines.get(err, i);
        MX_ERROR_CHECK(err);

        t2 = (mx_grid_paint_line_t*)lines.get(err, i + 1);
        MX_ERROR_CHECK(err);

        // assume that t1 is pointed in the right direction - swap t2 around
        // if necessary
        find_tolerance(t1, t2, ht, vt);

        if (!points_match(t1->p2, t2->p1, vt, ht)) {
            temp = t2->p1;
            t2->p1 = t2->p2;
            t2->p2 = temp;
        }

        // now, all we have to do is fix the joint between t1->p2 and
        // t2->p1
        if (MX_FLOAT_EQ(t1->p1.x, t1->p2.x)) {
            // t1 is vertical
            t2->p1.x = t1->p2.x;
            t1->p2.y = t2->p1.y;
        } else {
            // t1 is horizontal
            t2->p1.y = t1->p2.y;
            t1->p2.x = t2->p1.x;
        }
    }

    // finally, fix first and last points if they are coincident
    t1 = (mx_grid_paint_line_t*)lines.get(err, n - 1);
    MX_ERROR_CHECK(err);

    t2 = (mx_grid_paint_line_t*)lines.get(err, 0);
    MX_ERROR_CHECK(err);

    find_tolerance(t1, t2, ht, vt);

    if (points_match(t1->p2, t2->p1, vt, ht)) {
        if (MX_FLOAT_EQ(t1->p1.x, t1->p2.x)) {
            // t1 is vertical
            t2->p1.x = t1->p2.x;
            t1->p2.y = t2->p1.y;
        } else {
            // t1 is horizontal
            t2->p1.y = t1->p2.y;
            t1->p2.x = t2->p1.x;
        }
    }

abort:;
}

void mx_grid_paint::draw_line_list(
    int& err,
    mx_device& drawable,
    mx_list& lines)
{
    int i, n;
    mx_grid_paint_line_t* t;

    fix_line_list(err, lines);
    MX_ERROR_CHECK(err);

    n = lines.get_num_items();

    {
        mx_doc_polypoint pp(0, n + 1);

        for (i = 0; i < n; i++) {
            t = (mx_grid_paint_line_t*)lines.get(err, i);
            MX_ERROR_CHECK(err);

            if (i == (n - 1)) {
                pp.pp.setPoint(i, t->p1);
                pp.pp.setPoint(i + 1, t->p2);

                drawable.drawPolypoint(err, pp, &(t->s));
                MX_ERROR_CHECK(err);
            } else {
                pp.pp.setPoint(i, t->p1);
            }

            delete t;
        }
    }

abort:;
}

// if points are within 0.1 of a mm then they are touching
static const double touch_tolerance = 0.1 * 0.1;

void mx_grid_paint::add_horizontal(int& err, mx_line_style& ls, mx_point& p1, mx_point& p2)
{
    int i;
    int n = horizontal_lines.get_num_items();
    bool greater_than_previous = TRUE;

    mx_grid_paint_line_t* t;

    for (i = 0; i < n; i++) {
        t = (mx_grid_paint_line_t*)horizontal_lines.get(err, i);
        MX_ERROR_CHECK(err);

        if (MX_FLOAT_EQ(p1.y, t->p1.y)) {
            // check to see if the width colour and line type are the same.
            if (ls.line_type == t->s.line_type && MX_FLOAT_EQ(ls.width, t->s.width) && ls.colour == t->s.colour) {
                // all the same, maybe able to merge lines
                if ((t->p1.dist2(p1) < touch_tolerance) || (t->p2.dist2(p1) < touch_tolerance) || (t->p1.dist2(p2) < touch_tolerance) || (t->p2.dist2(p2) < touch_tolerance)) {
                    // yes
                    t->p1.x = GMIN(GMIN(p1.x, p2.x), GMIN(t->p1.x, t->p2.x));
                    t->p2.x = GMAX(GMAX(p1.x, p2.x), GMAX(t->p1.x, t->p2.x));
                    return;
                }
            }
        } else {
            if (greater_than_previous && (p1.y < t->p1.y)) {
                break;
            }
            greater_than_previous = (p1.y > t->p1.y);
        }
    }

    t = new mx_grid_paint_line_t;
    t->p1 = p1;
    t->p2 = p2;
    t->s = ls;

    horizontal_lines.insert(err, i, t);
    MX_ERROR_CHECK(err);

abort:;
}

void mx_grid_paint::add_vertical(int& err, mx_line_style& ls, mx_point& p1, mx_point& p2)
{
    int i;
    int n = vertical_lines.get_num_items();
    bool greater_than_previous = TRUE;

    mx_grid_paint_line_t* t;

    for (i = 0; i < n; i++) {
        t = (mx_grid_paint_line_t*)vertical_lines.get(err, i);
        MX_ERROR_CHECK(err);

        if (MX_FLOAT_EQ(p1.x, t->p1.x)) {
            // check to see if the width colour and line type are the same.
            if (ls.line_type == t->s.line_type && MX_FLOAT_EQ(ls.width, t->s.width) && ls.colour == t->s.colour) {
                // all the same, maybe able to merge lines
                if ((t->p1.dist2(p1) < touch_tolerance) || (t->p2.dist2(p1) < touch_tolerance) || (t->p1.dist2(p2) < touch_tolerance) || (t->p2.dist2(p2) < touch_tolerance)) {
                    // yes
                    t->p1.y = GMIN(GMIN(p1.y, p2.y), GMIN(t->p1.y, t->p2.y));
                    t->p2.y = GMAX(GMAX(p1.y, p2.y), GMAX(t->p1.y, t->p2.y));
                    return;
                }
            }
        } else {
            if (greater_than_previous && (p1.x < t->p1.x)) {
                break;
            }
            greater_than_previous = (p1.x > t->p1.x);
        }
    }

    t = new mx_grid_paint_line_t;
    t->p1 = p1;
    t->p2 = p2;
    t->s = ls;

    vertical_lines.insert(err, i, t);
    MX_ERROR_CHECK(err);
abort:;
}
