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
 * MODULE/CLASS :  mx_table_area_pos
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

#include <mx_area_pos.h>
#include <mx_doc_coord.h>
#include <mx_doc_rect.h>
#include <mx_para_pos.h>
#include <mx_position.h>
#include <mx_spline.h>
#include <mx_ta_pos.h>
#include <mx_table_area.h>
#include <mx_text_area.h>
#include <mx_txt_pos.h>

mx_table_area_pos::mx_table_area_pos()
{
    row = column = 0;
}

mx_table_area_pos::mx_table_area_pos(
    int irow,
    int icolumn,
    const mx_text_area_pos& itext_pos)
    : row(irow)
    , column(icolumn)
    , text_pos(itext_pos)
{
}

void mx_table_area_pos::operator=(const mx_table_area_pos& p)
{
    row = p.row;
    column = p.column;
    text_pos = p.text_pos;
}

void mx_table_area_pos::up(int& err, mx_area* area)
{
    mx_table_area* ta = (mx_table_area*)area;
    mx_text_area* a;

    a = ta->get_cell(err, row, column);
    MX_ERROR_CHECK(err);

    text_pos.up(err, a);
    if (err == MX_POSITION_RANGE_ERROR) {
        if (row == 0) {
            MX_ERROR_CHECK(err);
        } else {
            MX_ERROR_CLEAR(err);

            a = ta->get_cell(err, --row, column);
            MX_ERROR_CHECK(err);

            text_pos.moveto_end(err, a);
            MX_ERROR_CHECK(err);
        }
    } else {
        MX_ERROR_CHECK(err);
    }

abort:;
}

void mx_table_area_pos::down(int& err, mx_area* area)
{
    mx_table_area* ta = (mx_table_area*)area;
    mx_text_area* a;
    int nr;

    nr = ta->get_num_rows(err);
    MX_ERROR_CHECK(err);

    a = ta->get_cell(err, row, column);
    MX_ERROR_CHECK(err);

    text_pos.down(err, a);
    if (err == MX_POSITION_RANGE_ERROR) {
        if (row == (nr - 1)) {
            MX_ERROR_CHECK(err);
        } else {
            MX_ERROR_CLEAR(err);

            a = ta->get_cell(err, ++row, column);
            MX_ERROR_CHECK(err);

            text_pos.moveto_start(err, a);
            MX_ERROR_CHECK(err);
        }
    } else {
        MX_ERROR_CHECK(err);
    }

abort:;
}

void mx_table_area_pos::left(int& err, mx_area* area)
{
    mx_table_area* ta = (mx_table_area*)area;
    mx_text_area* a;

    a = ta->get_cell(err, row, column);
    MX_ERROR_CHECK(err);

    text_pos.left(err, a);
    if (err == MX_POSITION_RANGE_ERROR) {
        if (row == 0 && column == 0) {
            MX_ERROR_CHECK(err);
        } else {
            MX_ERROR_CLEAR(err);

            if (column == 0) {
                column = ta->get_num_columns(err) - 1;
                MX_ERROR_CHECK(err);

                row--;
            } else {
                column--;
            }

            a = ta->get_cell(err, row, column);
            MX_ERROR_CHECK(err);

            text_pos.moveto_end(err, a);
            MX_ERROR_CHECK(err);
        }
    } else {
        MX_ERROR_CHECK(err);
    }

abort:;
}

void mx_table_area_pos::right(int& err, mx_area* area)
{
    mx_table_area* ta = (mx_table_area*)area;
    mx_text_area* a;
    int nc, nr;

    a = ta->get_cell(err, row, column);
    MX_ERROR_CHECK(err);

    nc = ta->get_num_columns(err);
    MX_ERROR_CHECK(err);

    nr = ta->get_num_rows(err);
    MX_ERROR_CHECK(err);

    text_pos.right(err, a);
    if (err == MX_POSITION_RANGE_ERROR) {
        if (row == (nr - 1) && column == (nc - 1)) {
            MX_ERROR_CHECK(err);
        } else {
            MX_ERROR_CLEAR(err);

            if (column == (nc - 1)) {
                column = 0;
                row++;
            } else {
                column++;
            }

            a = ta->get_cell(err, row, column);
            MX_ERROR_CHECK(err);

            text_pos.moveto_start(err, a);
            MX_ERROR_CHECK(err);
        }
    } else {
        MX_ERROR_CHECK(err);
    }

abort:;
}

void mx_table_area_pos::moveto_end(int& err, mx_area* ga)
{
    mx_text_area* area;
    mx_table_area* ta = (mx_table_area*)ga;

    row = ta->get_num_rows(err) - 1;
    MX_ERROR_CHECK(err);

    column = ta->get_num_columns(err) - 1;
    MX_ERROR_CHECK(err);

    area = ta->get_cell(err, row, column);
    MX_ERROR_CHECK(err);

    text_pos.moveto_end(err, area);
    MX_ERROR_CHECK(err);

abort:;
}

void mx_table_area_pos::moveto_start(int& err, mx_area* ga)
{
    row = 0;
    column = 0;
    text_pos.moveto_start(err, ga);
    MX_ERROR_CHECK(err);

abort:;
}

mx_area_pos* mx_table_area_pos::duplicate()
{
    mx_table_area_pos* res;

    res = new mx_table_area_pos;
    *res = *this;
    return res;
}

bool mx_table_area_pos::is_end(int& err, mx_area* area)
{
    mx_table_area* a = (mx_table_area*)area;
    mx_text_area* ta;
    bool b;
    int nc, nr;

    nc = a->get_num_columns(err);
    MX_ERROR_CHECK(err);

    nr = a->get_num_rows(err);
    MX_ERROR_CHECK(err);

    ta = a->get_cell(err, row, column);
    MX_ERROR_CHECK(err);

    b = text_pos.is_end(err, ta);
    MX_ERROR_CHECK(err);

    return (row == (nr - 1)) && (column == (nc - 1)) && b;

abort:
    return FALSE;
}

bool mx_table_area_pos::is_start()
{
    return row == 0 && column == 0 && text_pos.is_start();
}

bool mx_table_area_pos::is_start_line()
{
    return row == 0;
}

bool mx_table_area_pos::is_start_para()
{
    return row == 0;
}

mx_point mx_table_area_pos::position(int& err, mx_area* area)
{
    mx_table_area* ta = (mx_table_area*)area;
    mx_text_area* a;
    mx_point res;

    a = ta->get_cell(err, row, column);
    MX_ERROR_CHECK(err);

    res = text_pos.position(err, a);
    MX_ERROR_CHECK(err);

    res += a->get_origin(err);
    MX_ERROR_CHECK(err);

abort:
    return res;
}

void mx_table_area_pos::moveto(int& err, mx_area* area, mx_point& p)
{
    mx_table_area* ta = (mx_table_area*)area;
    mx_text_area *a, *nearest = NULL;
    float d, dist = 100000000.0;
    mx_point o;

    int r, c, nr, nc, nearest_row = 0, nearest_column = 0;
    mx_rect rect;

    nc = ta->get_num_columns(err);
    MX_ERROR_CHECK(err);

    nr = ta->get_num_rows(err);
    MX_ERROR_CHECK(err);

    // first find right row
    for (r = 0; r < nr; r++) {
        a = ta->get_cell(err, r, 0);
        MX_ERROR_CHECK(err);

        rect = a->bounding_box(err);
        MX_ERROR_CHECK(err);

        o = a->get_origin(err);
        MX_ERROR_CHECK(err);

        rect += o;
        if (p.y <= GMAX(rect.yt, rect.yb) && p.y >= GMIN(rect.yt, rect.yb)) {
            nearest_row = r;
            break;
        } else if (fabs(o.y - p.y) < dist) {
            dist = fabs(o.y - p.y);
            nearest_row = r;
        }
    }

    dist = 100000000.0;

    for (c = 0; c < nc; c++) {
        a = ta->get_cell(err, nearest_row, c);
        MX_ERROR_CHECK(err);

        rect = a->bounding_box(err);
        MX_ERROR_CHECK(err);

        o = a->get_origin(err);
        MX_ERROR_CHECK(err);

        rect += o;

        if (rect.pointIn(p)) {
            nearest = a;
            nearest_column = c;
            break;
        }
        d = fabs(o.x - p.x);
        if (d < dist) {
            dist = d;
            nearest = a;
            nearest_column = c;
        }
    }

    // move to nearest
    row = nearest_row;
    column = nearest_column;

    p -= nearest->get_origin(err);
    MX_ERROR_CHECK(err);

    text_pos.moveto(err, nearest, p);
    MX_ERROR_CHECK(err);

abort:;
}

void mx_table_area_pos::next_para(int& err, mx_area* a)
{
    mx_text_area* txta;
    mx_table_area* taba = (mx_table_area*)a;
    MX_ERROR_ASSERT(err, a->is_a(mx_table_area_class_e));

    txta = taba->get_cell(err, row, column);
    MX_ERROR_CHECK(err);

    text_pos.next_para(err, txta);
    if (err == MX_POSITION_RANGE_ERROR) {
        MX_ERROR_CLEAR(err);

        this->next_cell(err, taba);
        MX_ERROR_CHECK(err);
    } else {
        MX_ERROR_CHECK(err);
    }

abort:;
}

void mx_table_area_pos::prev_para(int& err, mx_area* a)
{
    mx_text_area* txta;
    mx_table_area* taba = (mx_table_area*)a;
    MX_ERROR_ASSERT(err, a->is_a(mx_table_area_class_e));

    txta = taba->get_cell(err, row, column);
    MX_ERROR_CHECK(err);

    text_pos.prev_para(err, txta);
    if (err == MX_POSITION_RANGE_ERROR) {
        MX_ERROR_CLEAR(err);

        this->prev_cell(err, taba);
        MX_ERROR_CHECK(err);
    } else {
        MX_ERROR_CHECK(err);
    }

abort:;
}

bool mx_table_area_pos::operator<(const mx_area_pos& p)
{
    mx_table_area_pos* ap = (mx_table_area_pos*)(&p);

    if (p.rtti_class() != this->rtti_class()) {
        return FALSE;
    }

    if (row < ap->row) {
        return TRUE;
    } else {
        if (row == ap->row) {
            if (column < ap->column) {
                return TRUE;
            } else {
                if (column == ap->column) {
                    return text_pos < ap->text_pos;
                } else {
                    return FALSE;
                }
            }
        } else {
            return FALSE;
        }
    }
}

bool mx_table_area_pos::operator>(const mx_area_pos& p)
{
    mx_table_area_pos* ap = (mx_table_area_pos*)(&p);

    if (p.rtti_class() != this->rtti_class()) {
        return FALSE;
    }

    if (row > ap->row) {
        return TRUE;
    } else {
        if (row == ap->row) {
            if (column > ap->column) {
                return TRUE;
            } else {
                if (column == ap->column) {
                    return text_pos > ap->text_pos;
                } else {
                    return FALSE;
                }
            }
        } else {
            return FALSE;
        }
    }
}

bool mx_table_area_pos::operator==(const mx_area_pos& p)
{
    mx_table_area_pos* ap = (mx_table_area_pos*)(&p);

    if (p.rtti_class() != this->rtti_class()) {
        return FALSE;
    }
    return row == ap->row && column == ap->column && text_pos == ap->text_pos;
}

bool mx_table_area_pos::operator!=(const mx_area_pos& p)
{
    return !(*this == p);
}

bool mx_table_area_pos::operator<=(const mx_area_pos& p)
{
    return !(*this > p);
}

bool mx_table_area_pos::operator>=(const mx_area_pos& p)
{
    return !(*this < p);
}

bool mx_table_area_pos::is_same_para_as(mx_area_pos& p)
{
    if (!p.is_a(mx_table_area_pos_class_e))
        return FALSE;
    mx_table_area_pos& tp = (mx_table_area_pos&)p;
    return this->text_pos.is_same_para_as(tp.text_pos);
}

bool mx_table_area_pos::is_same_cell_as(mx_area_pos& p)
{
    if (!p.is_a(mx_table_area_pos_class_e))
        return FALSE;
    mx_table_area_pos& tp = (mx_table_area_pos&)p;
    return this->row == tp.row && this->column == tp.column;
}

bool mx_table_area_pos::is_same_row_as(mx_area_pos& p)
{
    if (!p.is_a(mx_table_area_pos_class_e))
        return FALSE;
    mx_table_area_pos& tp = (mx_table_area_pos&)p;
    return this->row == tp.row;
}

bool mx_table_area_pos::is_same_column_as(mx_area_pos& p)
{
    if (!p.is_a(mx_table_area_pos_class_e))
        return FALSE;
    mx_table_area_pos& tp = (mx_table_area_pos&)p;
    return this->column == tp.column;
}

void mx_table_area_pos::moveto_para_end(int& err, mx_table_area* ta)
{
    mx_text_area* a;

    a = ta->get_cell(err, row, column);
    MX_ERROR_CHECK(err);

    text_pos.moveto_para_end(err, a);
    MX_ERROR_CHECK(err);

abort:;
}

void mx_table_area_pos::moveto_para_start(int& err, mx_table_area* ta)
{
    mx_text_area* a;

    a = ta->get_cell(err, row, column);
    MX_ERROR_CHECK(err);

    text_pos.moveto_para_start(err, a);
    MX_ERROR_CHECK(err);

abort:;
}

mx_char_style* mx_table_area_pos::get_char_style(int& err, mx_area* ta)
{
    mx_text_area* a;
    mx_char_style* res;

    a = ((mx_table_area*)ta)->get_cell(err, row, column);
    MX_ERROR_CHECK(err);

    res = text_pos.get_char_style(err, a);
    MX_ERROR_CHECK(err);

    return res;
abort:
    return NULL;
}

void mx_table_area_pos::moveto_cell_start(int& err, mx_table_area* area)
{
    mx_text_area* a;

    a = area->get_cell(err, row, column);
    MX_ERROR_CHECK(err);

    text_pos.moveto_start(err, a);
    MX_ERROR_CHECK(err);
abort:;
}

void mx_table_area_pos::moveto_cell_end(int& err, mx_table_area* area)
{
    mx_text_area* a;

    a = area->get_cell(err, row, column);
    MX_ERROR_CHECK(err);

    text_pos.moveto_end(err, a);
    MX_ERROR_CHECK(err);
abort:;
}

void mx_table_area_pos::moveto_row_start(int& err, mx_table_area* area)
{
    column = 0;

    moveto_cell_start(err, area);
    MX_ERROR_CHECK(err);
abort:;
}

void mx_table_area_pos::moveto_row_end(int& err, mx_table_area* area)
{
    int num_columns = area->get_num_columns(err);
    MX_ERROR_CHECK(err);

    column = num_columns - 1;
    MX_ERROR_CHECK(err);

    moveto_cell_end(err, area);
    MX_ERROR_CHECK(err);
abort:;
}

void mx_table_area_pos::next_cell(int& err, mx_table_area* area)
{
    int nc, nr;

    nr = area->get_num_rows(err);
    MX_ERROR_CHECK(err);

    nc = area->get_num_columns(err);
    MX_ERROR_CHECK(err);

    if (column == nc - 1) {
        if (row == (nr - 1)) {
            MX_ERROR_THROW(err, MX_POSITION_RANGE_ERROR);
        } else {
            column = 0;
            row++;
        }
    } else {
        column++;
    }

    moveto_cell_start(err, area);
    MX_ERROR_CHECK(err);

abort:;
}

void mx_table_area_pos::prev_cell(int& err, mx_table_area* area)
{
    int nc;

    nc = area->get_num_columns(err);
    MX_ERROR_CHECK(err);

    if (column == 0) {
        if (row == 0) {
            MX_ERROR_THROW(err, MX_POSITION_RANGE_ERROR);
        } else {
            row--;
            column = nc - 1;
        }
    } else {
        column--;
    }

    moveto_cell_start(err, area);
    MX_ERROR_CHECK(err);

abort:;
}

mx_point mx_table_area_pos::position(int& err, mx_area* area, int line_offset)
{
    mx_table_area* ta = (mx_table_area*)area;
    mx_text_area* a;
    mx_point res;

    a = ta->get_cell(err, row, column);
    MX_ERROR_CHECK(err);

    res = text_pos.position(err, a, line_offset);
    MX_ERROR_CHECK(err);

    res += a->get_origin(err);
    MX_ERROR_CHECK(err);

abort:
    return res;
}

void mx_table_area_pos::next_word(int& err, mx_table_area* area)
{
    mx_text_area* a;

    a = area->get_cell(err, row, column);
    MX_ERROR_CHECK(err);

    text_pos.next_word(err, a);
    if (err == MX_POSITION_RANGE_ERROR) {
        MX_ERROR_CLEAR(err);

        next_cell(err, area);
        MX_ERROR_CHECK(err);
    } else {
        MX_ERROR_CHECK(err);
    }
abort:;
}

void mx_table_area_pos::prev_word(int& err, mx_table_area* area)
{
    mx_text_area* a;

    a = area->get_cell(err, row, column);
    MX_ERROR_CHECK(err);

    text_pos.prev_word(err, a);
    if (err == MX_POSITION_RANGE_ERROR) {
        MX_ERROR_CLEAR(err);

        prev_cell(err, area);
        MX_ERROR_CHECK(err);
    } else {
        MX_ERROR_CHECK(err);
    }
abort:;
}

void mx_table_area_pos::moveto_cell(int& err, mx_table_area* area, int r, int c)
{
    mx_table_area* ta = (mx_table_area*)area;
    mx_text_area* a;

    row = r;
    column = c;

    a = ta->get_cell(err, row, column);
    MX_ERROR_CHECK(err);

    text_pos.moveto_start(err, a);
    MX_ERROR_CHECK(err);
abort:;
}
