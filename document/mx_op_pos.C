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
 * MODULE/CLASS :  mx_opaque_area_pos
 *
 * AUTHOR : Andrew Haisley
 *
 *
 *
 * DESCRIPTION:
 *
 *
 *
 *
 *
 */
#include <mx.h>
#include <mx_rtti.h>

#include <mx_area.h>
#include <mx_area_pos.h>
#include <mx_doc_rect.h>
#include <mx_op_pos.h>
#include <mx_position.h>

mx_opaque_area_pos::mx_opaque_area_pos()
{
    is_left = TRUE;
}

mx_opaque_area_pos::~mx_opaque_area_pos()
{
}

void mx_opaque_area_pos::operator=(const mx_opaque_area_pos& p)
{
    is_left = p.is_left;
}

void mx_opaque_area_pos::up(int& err, mx_area* a)
{
    MX_ERROR_THROW(err, MX_POSITION_RANGE_ERROR);
abort:;
}

void mx_opaque_area_pos::down(int& err, mx_area* a)
{
    MX_ERROR_THROW(err, MX_POSITION_RANGE_ERROR);
abort:;
}

void mx_opaque_area_pos::left(int& err, mx_area* a)
{
    if (is_left) {
        MX_ERROR_THROW(err, MX_POSITION_RANGE_ERROR);
    } else {
        is_left = TRUE;
    }
abort:;
}

void mx_opaque_area_pos::right(int& err, mx_area* a)
{
    if (is_left) {
        is_left = FALSE;
    } else {
        MX_ERROR_THROW(err, MX_POSITION_RANGE_ERROR);
    }
abort:;
}

void mx_opaque_area_pos::moveto_end(int& err, mx_area* area)
{
    is_left = FALSE;
}

void mx_opaque_area_pos::moveto_start(int& err, mx_area* area)
{
    is_left = TRUE;
}

mx_area_pos* mx_opaque_area_pos::duplicate()
{
    mx_opaque_area_pos* res;

    res = new mx_opaque_area_pos;
    *res = *this;
    return res;
}

mx_point mx_opaque_area_pos::position(int& err, mx_area* a)
{
    mx_point res;

    res.y += a->get_height(err);
    MX_ERROR_CHECK(err);

    if (!is_left) {
        res.x += a->get_width(err);
        MX_ERROR_CHECK(err);
    }

abort:
    return res;
}

void mx_opaque_area_pos::moveto(int& err, mx_area* a, mx_point& p)
{
    float width;

    width = a->get_width(err);
    MX_ERROR_CHECK(err);

    is_left = (p.x <= (width / 2));

abort:;
}

void mx_opaque_area_pos::next_para(int& err, mx_area* a)
{
    MX_ERROR_THROW(err, MX_POSITION_RANGE_ERROR);
abort:;
}

void mx_opaque_area_pos::prev_para(int& err, mx_area* a)
{
    MX_ERROR_THROW(err, MX_POSITION_RANGE_ERROR);
abort:;
}

bool mx_opaque_area_pos::operator<(const mx_area_pos& p)
{
    mx_opaque_area_pos* ap = (mx_opaque_area_pos*)(&p);

    if (p.rtti_class() != this->rtti_class()) {
        return FALSE;
    }

    return is_left && !ap->is_left;
}

bool mx_opaque_area_pos::operator>(const mx_area_pos& p)
{
    mx_opaque_area_pos* ap = (mx_opaque_area_pos*)(&p);

    if (p.rtti_class() != this->rtti_class()) {
        return FALSE;
    }

    return !is_left && ap->is_left;
}

bool mx_opaque_area_pos::operator==(const mx_area_pos& p)
{
    mx_opaque_area_pos* ap = (mx_opaque_area_pos*)(&p);

    if (p.rtti_class() != this->rtti_class()) {
        return FALSE;
    }

    return (is_left && ap->is_left) || (!is_left && !ap->is_left);
}

bool mx_opaque_area_pos::operator!=(const mx_area_pos& p)
{
    mx_opaque_area_pos* ap = (mx_opaque_area_pos*)(&p);

    if (p.rtti_class() != this->rtti_class()) {
        return FALSE;
    }

    return (is_left && !ap->is_left) || (!is_left && ap->is_left);
}

bool mx_opaque_area_pos::operator<=(const mx_area_pos& p)
{
    mx_opaque_area_pos* ap = (mx_opaque_area_pos*)(&p);

    if (p.rtti_class() != this->rtti_class()) {
        return FALSE;
    }

    return is_left || (!is_left && !ap->is_left);
}

bool mx_opaque_area_pos::operator>=(const mx_area_pos& p)
{
    mx_opaque_area_pos* ap = (mx_opaque_area_pos*)(&p);

    if (p.rtti_class() != this->rtti_class()) {
        return FALSE;
    }

    return !is_left || (is_left && ap->is_left);
}
bool mx_opaque_area_pos::is_same_para_as(mx_area_pos& p)
{
    return p.is_a(mx_opaque_area_pos_class_e);
}
