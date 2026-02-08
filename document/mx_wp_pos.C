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
 * MODULE/CLASS : mx_wp_pos
 *
 * AUTHOR : Andrew Haisley
 *
 *
 *
 * DESCRIPTION:
 *
 * Positions within a document from a WP point of view.
 *
 *
 *
 */

#include "mx_op_pos.h"
#include "mx_ta_pos.h"
#include "mx_txt_pos.h"
#include "mx_wp_pos.h"
#include "mx_wp_sh_pos.h"
#include <mx.h>
#include <mx_break_w.h>
#include <mx_nlist.h>
#include <mx_paragraph.h>
#include <mx_style.h>
#include <mx_wp_doc.h>

mx_wp_doc_pos& mx_wp_doc_pos::operator=(const mx_wp_doc_pos& p)
{
    if (&p != this) {
        sheet_num = p.sheet_num;
        sheet_pos = p.sheet_pos;
        stored_pos = p.stored_pos;
    }
    return *this;
}

mx_wp_doc_pos::mx_wp_doc_pos(const mx_wp_doc_pos& other)
    : stored_pos(other.stored_pos)
{
    sheet_num = other.sheet_num;
    sheet_pos = other.sheet_pos;
}

// compare logical positions
bool mx_wp_doc_pos::operator<(const mx_wp_doc_pos& p)
{
    if (sheet_num != p.sheet_num) {
        return sheet_num < p.sheet_num;
    } else {
        return sheet_pos < p.sheet_pos;
    }
}

bool mx_wp_doc_pos::operator>(const mx_wp_doc_pos& p)
{
    if (sheet_num != p.sheet_num) {
        return sheet_num > p.sheet_num;
    } else {
        return sheet_pos > p.sheet_pos;
    }
}

bool mx_wp_doc_pos::operator==(const mx_wp_doc_pos& p)
{
    return sheet_num == p.sheet_num && sheet_pos == p.sheet_pos;
}

bool mx_wp_doc_pos::operator!=(const mx_wp_doc_pos& p)
{
    return sheet_num != p.sheet_num || sheet_pos != p.sheet_pos;
}

bool mx_wp_doc_pos::operator<=(const mx_wp_doc_pos& p)
{
    if (sheet_num < p.sheet_num) {
        return TRUE;
    } else {
        if (sheet_num == p.sheet_num) {
            return sheet_pos <= p.sheet_pos;
        } else {
            return FALSE;
        }
    }
}

bool mx_wp_doc_pos::operator>=(const mx_wp_doc_pos& p)
{
    if (sheet_num > p.sheet_num) {
        return TRUE;
    } else {
        if (sheet_num == p.sheet_num) {
            return sheet_pos >= p.sheet_pos;
        } else {
            return FALSE;
        }
    }
}

void mx_wp_doc_pos::up(int& err, mx_wp_document* d)
{
    mx_sheet* s = d->sheet(err, sheet_num);
    MX_ERROR_CHECK(err);

    sheet_pos.up(err, s);
    if (err == MX_POSITION_RANGE_ERROR) {
        if (sheet_num == 0) {
            MX_ERROR_CHECK(err);
        } else {
            MX_ERROR_CLEAR(err);

            s = d->sheet(err, --sheet_num);
            MX_ERROR_CHECK(err);

            sheet_pos.moveto_end(err, s);
            MX_ERROR_CHECK(err);
        }
    } else {
        MX_ERROR_CHECK(err);
    }

abort:;
}

void mx_wp_doc_pos::down(int& err, mx_wp_document* d)
{
    int ns;

    mx_sheet* s = d->sheet(err, sheet_num);
    MX_ERROR_CHECK(err);

    ns = d->get_num_sheets(err);
    MX_ERROR_CHECK(err);

    sheet_pos.down(err, s);
    if (err == MX_POSITION_RANGE_ERROR) {
        if (sheet_num == (ns - 1)) {
            MX_ERROR_CHECK(err);
        } else {
            MX_ERROR_CLEAR(err);

            s = d->sheet(err, ++sheet_num);
            MX_ERROR_CHECK(err);

            sheet_pos.moveto_start(err, s);
            MX_ERROR_CHECK(err);
        }
    } else {
        MX_ERROR_CHECK(err);
    }

abort:;
}

void mx_wp_doc_pos::left(int& err, mx_wp_document* d)
{
    mx_point p;

    mx_sheet* s = d->sheet(err, sheet_num);
    MX_ERROR_CHECK(err);

    sheet_pos.left(err, s);
    if (err == MX_POSITION_RANGE_ERROR) {
        if (sheet_num == 0) {
            MX_ERROR_CHECK(err);
        } else {
            MX_ERROR_CLEAR(err);

            sheet_num--;

            s = d->sheet(err, sheet_num);
            MX_ERROR_CHECK(err);

            sheet_pos.moveto_end(err, s);
            MX_ERROR_CHECK(err);
        }
    } else {
        MX_ERROR_CHECK(err);
    }

abort:;
}

void mx_wp_doc_pos::right(int& err, mx_wp_document* d)
{
    mx_point p;
    int ns;

    mx_sheet* s = d->sheet(err, sheet_num);
    MX_ERROR_CHECK(err);

    ns = d->get_num_sheets(err);
    MX_ERROR_CHECK(err);

    sheet_pos.right(err, s);
    if (err == MX_POSITION_RANGE_ERROR) {
        if (sheet_num == (ns - 1)) {
            MX_ERROR_CHECK(err);
        } else {
            MX_ERROR_CLEAR(err);

            sheet_num++;

            s = d->sheet(err, sheet_num);
            MX_ERROR_CHECK(err);

            sheet_pos.moveto_start(err, s);
            MX_ERROR_CHECK(err);
        }
    } else {
        MX_ERROR_CHECK(err);
    }

abort:;
}

// where is the selection within the area - use rtti to figure
// out class of result
mx_wp_sheet_pos* mx_wp_doc_pos::get_sheet_pos()
{
    return &sheet_pos;
}

void mx_wp_doc_pos::moveto(int& err, mx_wp_document* d, mx_doc_coord_t& c)
{
    sheet_num = c.sheet_number;

    mx_sheet* s = d->sheet(err, sheet_num);
    if (err == MX_DOCUMENT_SHEET_INDEX) {
        MX_ERROR_CLEAR(err);

        this->moveto_end(err, d);
        MX_ERROR_CHECK(err);

        MX_ERROR_THROW(err, MX_POSITION_RANGE_ERROR);
    }
    MX_ERROR_CHECK(err);

    sheet_pos.moveto(err, s, c.p);
    MX_ERROR_CHECK(err);

abort:;
}

void mx_wp_doc_pos::moveto_start(int& err, mx_wp_document* d)
{
    moveto_start_sheet(err, d, 0);
}

void mx_wp_doc_pos::moveto_end(int& err, mx_wp_document* d)
{
    int num_sheets = d->get_num_sheets(err);

    mx_sheet* s = d->sheet(err, num_sheets - 1);
    MX_ERROR_CHECK(err);

    sheet_num = num_sheets - 1;

    sheet_pos.moveto_end(err, s);
    MX_ERROR_CHECK(err);

abort:;
}

// which sheet is the current position in
int mx_wp_doc_pos::get_sheet() const
{
    return sheet_num;
}

mx_wp_doc_pos::mx_wp_doc_pos()
    : stored_pos()
{
    sheet_num = 0;
}

mx_wp_doc_pos::~mx_wp_doc_pos()
{
}

void mx_wp_doc_pos::moveto_start_sheet(int& err, mx_wp_document* d, int sheet)
{
    sheet_num = sheet;

    mx_sheet* s = d->sheet(err, sheet);
    MX_ERROR_CHECK(err);

    sheet_pos.moveto_start(err, s);
    MX_ERROR_CHECK(err);

abort:;
}

void mx_wp_doc_pos::back_sheet(int& err)
{
    if (sheet_num <= 0) {
        MX_ERROR_THROW(err, MX_POSITION_RANGE_ERROR);
    } else {
        sheet_num--;
    }
abort:;
}

void mx_wp_doc_pos::forward_sheet(int& err, mx_wp_document* d)
{
    int n;

    n = d->get_num_sheets(err);
    MX_ERROR_CHECK(err);

    if (sheet_num >= (n - 1)) {
        MX_ERROR_THROW(err, MX_POSITION_RANGE_ERROR);
    } else {
        sheet_num++;
    }
abort:;
}

mx_doc_coord_t mx_wp_doc_pos::position(int& err, mx_wp_document* d)
{
    mx_doc_coord_t res;

    mx_sheet* s = d->sheet(err, sheet_num);
    MX_ERROR_CHECK(err);

    res.sheet_number = sheet_num;

    res.p = sheet_pos.position(err, s);
    MX_ERROR_CHECK(err);

abort:
    return res;
}

mx_doc_coord_t mx_wp_doc_pos::position(int& err, mx_wp_document* d, int line_offset)
{
    mx_doc_coord_t res;

    mx_sheet* s = d->sheet(err, sheet_num);
    MX_ERROR_CHECK(err);

    res.sheet_number = sheet_num;

    res.p = sheet_pos.position(err, s, line_offset);
    MX_ERROR_CHECK(err);

abort:
    return res;
}

void mx_wp_doc_pos::reset(int& err, mx_wp_document* d)
{
    mx_sheet* s;

    s = d->sheet(err, sheet_num);
    MX_ERROR_CHECK(err);

    sheet_pos.make_valid(err, s);
    MX_ERROR_CHECK(err);

    sheet_pos.reset(err, s);
    MX_ERROR_CHECK(err);

abort:;
}

bool mx_wp_doc_pos::is_paragraph(int& err)
{
    mx_area_pos* ap = sheet_pos.get_area_pos(err);
    MX_ERROR_CHECK(err);

    return ap->rtti_class() == mx_text_area_pos_class_e || ap->rtti_class() == mx_table_area_pos_class_e;

abort:
    return FALSE;
}

bool mx_wp_doc_pos::is_table(int& err)
{
    mx_area_pos* ap = sheet_pos.get_area_pos(err);
    MX_ERROR_CHECK(err);

    return ap->is_a(mx_table_area_pos_class_e);
abort:
    return FALSE;
}

mx_para_pos* mx_wp_doc_pos::get_para_pos(int& err)
{
    mx_area_pos* ap;

    ap = sheet_pos.get_area_pos(err);
    MX_ERROR_CHECK(err);

    if (ap->rtti_class() == mx_text_area_pos_class_e) {
        mx_text_area_pos* tap = (mx_text_area_pos*)ap;

        return &(tap->para_pos);
    } else {
        if (ap->rtti_class() == mx_table_area_pos_class_e) {
            mx_table_area_pos* tap = (mx_table_area_pos*)ap;

            return &(tap->text_pos.para_pos);
        } else {
            MX_ERROR_THROW(err, MX_POSITION_RANGE_ERROR);
        }
    }

abort:
    return NULL;
}

mx_paragraph* mx_wp_doc_pos::get_para(int& err, mx_wp_document* doc)
{
    mx_area_pos* ap;
    mx_area* a;
    mx_paragraph* res = NULL;

    ap = this->get_area_pos(err);
    MX_ERROR_CHECK(err);

    a = this->get_area(err, doc);
    MX_ERROR_CHECK(err);

    if (ap->rtti_class() == mx_text_area_pos_class_e) {
        mx_text_area_pos* tap = (mx_text_area_pos*)ap;
        mx_text_area* ta = (mx_text_area*)a;

        MX_ERROR_ASSERT(err, ta->is_a(mx_text_area_class_e));

        res = ta->get(err, tap->paragraph_index);
        MX_ERROR_CHECK(err);
    } else {
        if (ap->rtti_class() == mx_table_area_pos_class_e) {
            mx_table_area_pos* tap = (mx_table_area_pos*)ap;
            mx_table_area* table = (mx_table_area*)a;
            mx_text_area* ta;

            MX_ERROR_ASSERT(err, table->is_a(mx_table_area_class_e));

            ta = table->get_cell(err, tap->row, tap->column);
            MX_ERROR_CHECK(err);

            res = ta->get(err, tap->text_pos.paragraph_index);
            MX_ERROR_CHECK(err);
        } else {
            MX_ERROR_THROW(err, MX_POSITION_RANGE_ERROR);
        }
    }

abort:
    return res;
}

mx_area* mx_wp_doc_pos::get_area(int& err, mx_wp_document* doc)
{
    mx_area* res;

    mx_sheet* s = doc->sheet(err, sheet_num);
    MX_ERROR_CHECK(err);

    res = sheet_pos.area(err, s);
    MX_ERROR_CHECK(err);

    return res;

abort:
    return NULL;
}

mx_area_pos* mx_wp_doc_pos::get_area_pos(int& err)
{
    mx_area_pos* ap = sheet_pos.get_area_pos(err);
    MX_ERROR_CHECK(err);

    return ap;
abort:
    return NULL;
}

void mx_wp_doc_pos::prev_sheet(int& err, mx_wp_document* doc)
{
    mx_sheet* s;

    if (sheet_num == 0) {
        MX_ERROR_THROW(err, MX_POSITION_RANGE_ERROR);
    } else {
        sheet_num--;
    }

    s = doc->sheet(err, sheet_num);
    MX_ERROR_CHECK(err);

    sheet_pos.moveto_start(err, s);
    MX_ERROR_CHECK(err);
abort:;
}

void mx_wp_doc_pos::next_sheet(int& err, mx_wp_document* doc)
{
    mx_sheet* s;
    int ns;

    ns = doc->get_num_sheets(err);
    MX_ERROR_CHECK(err);

    if (sheet_num == (ns - 1)) {
        MX_ERROR_THROW(err, MX_POSITION_RANGE_ERROR);
    } else {
        sheet_num++;
    }

    s = doc->sheet(err, sheet_num);
    MX_ERROR_CHECK(err);

    sheet_pos.moveto_start(err, s);
    MX_ERROR_CHECK(err);
abort:;
}

void mx_wp_doc_pos::next_area(int& err, mx_wp_document* doc)
{
    mx_sheet* s = doc->sheet(err, sheet_num);
    MX_ERROR_CHECK(err);

    sheet_pos.next_area(err, s);
    if (err == MX_POSITION_RANGE_ERROR) {
        MX_ERROR_CLEAR(err);

        this->next_sheet(err, doc);
        MX_ERROR_CHECK(err);
    } else {
        MX_ERROR_CHECK(err);
    }
abort:;
}

void mx_wp_doc_pos::prev_area(int& err, mx_wp_document* doc)
{
    mx_sheet* s = doc->sheet(err, sheet_num);
    MX_ERROR_CHECK(err);

    sheet_pos.prev_area(err, s);
    if (err == MX_POSITION_RANGE_ERROR) {
        MX_ERROR_CLEAR(err);

        this->prev_sheet(err, doc);
        MX_ERROR_CHECK(err);

        this->moveto_end_sheet(err, doc);
        MX_ERROR_CHECK(err);
    } else {
        MX_ERROR_CHECK(err);
    }
abort:;
}

void mx_wp_doc_pos::next_cell(int& err, mx_wp_document* doc)
{
    bool b = this->is_table(err);
    MX_ERROR_CHECK(err);

    if (b) {
        this->moveto_end_cell(err, doc);
        MX_ERROR_CHECK(err);
    }

    this->right(err, doc);
    MX_ERROR_CHECK(err);
abort:;
}

void mx_wp_doc_pos::prev_cell(int& err, mx_wp_document* doc)
{
    bool b = this->is_table(err);
    MX_ERROR_CHECK(err);

    if (b) {
        this->moveto_start_cell(err, doc);
        MX_ERROR_CHECK(err);
    }

    this->left(err, doc);
    MX_ERROR_CHECK(err);
abort:;
}

void mx_wp_doc_pos::next_row(int& err, mx_wp_document* doc)
{
    bool b = this->is_table(err);
    MX_ERROR_CHECK(err);

    if (b) {
        this->moveto_end_row(err, doc);
        MX_ERROR_CHECK(err);

        this->right(err, doc);
        MX_ERROR_CHECK(err);
    } else {
        this->down(err, doc);
        MX_ERROR_CHECK(err);
    }
abort:;
}

void mx_wp_doc_pos::prev_row(int& err, mx_wp_document* doc)
{
    bool b = this->is_table(err);
    MX_ERROR_CHECK(err);

    if (b) {
        this->moveto_start_row(err, doc);
        MX_ERROR_CHECK(err);

        this->left(err, doc);
        MX_ERROR_CHECK(err);
    } else {
        this->up(err, doc);
        MX_ERROR_CHECK(err);
    }
abort:;
}

void mx_wp_doc_pos::moveto_start_area(int& err, mx_wp_document* doc)
{
    mx_area* a;
    mx_area_pos* ap;

    a = this->get_area(err, doc);
    MX_ERROR_CHECK(err);

    ap = this->get_area_pos(err);
    MX_ERROR_CHECK(err);

    ap->moveto_start(err, a);
    MX_ERROR_CHECK(err);

abort:;
}

void mx_wp_doc_pos::moveto_end_area(int& err, mx_wp_document* doc)
{
    mx_area* a;
    mx_area_pos* ap;

    a = this->get_area(err, doc);
    MX_ERROR_CHECK(err);

    ap = this->get_area_pos(err);
    MX_ERROR_CHECK(err);

    ap->moveto_end(err, a);
    MX_ERROR_CHECK(err);

abort:;
}

void mx_wp_doc_pos::moveto_end_para(int& err, mx_wp_document* doc)
{
    bool b;

    mx_paragraph* para;
    mx_para_pos* pp;
    mx_area_pos* ap;
    mx_area* a;

    b = is_paragraph(err);
    MX_ERROR_CHECK(err);

    if (b) {
        para = get_para(err, doc);
        MX_ERROR_CHECK(err);

        pp = get_para_pos(err);
        MX_ERROR_CHECK(err);

        pp->moveto_end(err, para);
        MX_ERROR_CHECK(err);
    } else {
        ap = sheet_pos.get_area_pos(err);
        MX_ERROR_CHECK(err);

        a = get_area(err, doc);
        MX_ERROR_CHECK(err);

        ap->moveto_end(err, a);
        MX_ERROR_CHECK(err);
    }
abort:;
}

void mx_wp_doc_pos::moveto_start_para(int& err, mx_wp_document* doc)
{
    bool b;

    mx_para_pos* pp;
    mx_area_pos* ap;
    mx_area* a;

    b = is_paragraph(err);
    MX_ERROR_CHECK(err);

    if (b) {
        pp = get_para_pos(err);
        MX_ERROR_CHECK(err);

        pp->moveto_start();
    } else {
        ap = sheet_pos.get_area_pos(err);
        MX_ERROR_CHECK(err);

        a = get_area(err, doc);
        MX_ERROR_CHECK(err);

        ap->moveto_start(err, a);
        MX_ERROR_CHECK(err);
    }
abort:;
}

void mx_wp_doc_pos::moveto_start_row(int& err, mx_wp_document* doc)
{
    mx_table_area* taba;
    mx_table_area_pos* tabp;

    taba = (mx_table_area*)get_area(err, doc);
    MX_ERROR_CHECK(err);

    tabp = (mx_table_area_pos*)get_area_pos(err);
    MX_ERROR_CHECK(err);

    MX_ERROR_ASSERT(err, taba->is_a(mx_table_area_class_e));
    MX_ERROR_ASSERT(err, tabp->is_a(mx_table_area_pos_class_e));

    tabp->moveto_row_start(err, taba);
    MX_ERROR_CHECK(err);
abort:;
}

void mx_wp_doc_pos::moveto_end_row(int& err, mx_wp_document* doc)
{
    mx_table_area* taba;
    mx_table_area_pos* tabp;

    taba = (mx_table_area*)get_area(err, doc);
    MX_ERROR_CHECK(err);

    tabp = (mx_table_area_pos*)get_area_pos(err);
    MX_ERROR_CHECK(err);

    MX_ERROR_ASSERT(err, taba->is_a(mx_table_area_class_e));
    MX_ERROR_ASSERT(err, tabp->is_a(mx_table_area_pos_class_e));

    tabp->moveto_row_end(err, taba);
    MX_ERROR_CHECK(err);
abort:;
}

void mx_wp_doc_pos::moveto_start_cell(int& err, mx_wp_document* doc)
{
    mx_table_area* taba;
    mx_table_area_pos* tabp;

    taba = (mx_table_area*)get_area(err, doc);
    MX_ERROR_CHECK(err);

    tabp = (mx_table_area_pos*)get_area_pos(err);
    MX_ERROR_CHECK(err);

    MX_ERROR_ASSERT(err, taba->is_a(mx_table_area_class_e));
    MX_ERROR_ASSERT(err, tabp->is_a(mx_table_area_pos_class_e));

    tabp->moveto_cell_start(err, taba);
    MX_ERROR_CHECK(err);
abort:;
}

void mx_wp_doc_pos::moveto_end_cell(int& err, mx_wp_document* doc)
{
    mx_table_area* taba;
    mx_table_area_pos* tabp;

    taba = (mx_table_area*)get_area(err, doc);
    MX_ERROR_CHECK(err);

    tabp = (mx_table_area_pos*)get_area_pos(err);
    MX_ERROR_CHECK(err);

    MX_ERROR_ASSERT(err, taba->is_a(mx_table_area_class_e));
    MX_ERROR_ASSERT(err, tabp->is_a(mx_table_area_pos_class_e));

    tabp->moveto_cell_end(err, taba);
    MX_ERROR_CHECK(err);
abort:;
}

void mx_wp_doc_pos::moveto_start_word(int& err, mx_wp_document* doc)
{
    mx_para_pos* para_pos = this->get_para_pos(err);
    MX_ERROR_CHECK(err);

    para_pos->moveto_start_word();
abort:;
}

void mx_wp_doc_pos::moveto_end_word(int& err, mx_wp_document* doc)
{
    mx_paragraph* para;
    mx_para_pos* para_pos;

    para_pos = this->get_para_pos(err);
    MX_ERROR_CHECK(err);

    para = this->get_para(err, doc);
    MX_ERROR_CHECK(err);

    para_pos->moveto_end_word(err, para);
    MX_ERROR_CHECK(err);
abort:;
}

void mx_wp_doc_pos::moveto_start_logical_word(int& err, mx_wp_document* doc)
{
    this->prev_logical_word(err, doc);
    if (err == MX_POSITION_RANGE_ERROR) {
        MX_ERROR_CLEAR(err);
    } else {
        MX_ERROR_CHECK(err);

        // move the position back into the word

        this->next_logical_word(err, doc);
        MX_ERROR_CHECK(err);
    }

    this->moveto_start_word(err, doc);
    MX_ERROR_CHECK(err);
abort:;
}

void mx_wp_doc_pos::moveto_end_logical_word(int& err, mx_wp_document* doc)
{
    this->next_logical_word(err, doc);
    if (err == MX_POSITION_RANGE_ERROR) {
        MX_ERROR_CLEAR(err);
    } else {
        MX_ERROR_CHECK(err);

        // move the position back into the word

        this->prev_logical_word(err, doc);
        MX_ERROR_CHECK(err);
    }

    this->moveto_end_word(err, doc);
    MX_ERROR_CHECK(err);
abort:;
}

mx_paragraph_style* mx_wp_doc_pos::get_para_style(int& err, mx_wp_document* doc)
{
    bool b;

    mx_paragraph* para;
    mx_area* a;

    b = is_paragraph(err);
    MX_ERROR_CHECK(err);

    if (b) {
        para = get_para(err, doc);
        MX_ERROR_CHECK(err);

        return para->get_paragraph_style();
    } else {
        a = get_area(err, doc);
        MX_ERROR_CHECK(err);

        return a->get_paragraph_style();
    }
abort:
    return NULL;
}

void mx_wp_doc_pos::next_para(int& err, mx_wp_document* doc)
{
    mx_sheet* s = doc->sheet(err, sheet_num);
    MX_ERROR_CHECK(err);

    sheet_pos.next_para(err, s);
    if (err == MX_POSITION_RANGE_ERROR) {
        MX_ERROR_CLEAR(err);

        this->next_sheet(err, doc);
        MX_ERROR_CHECK(err);
    } else {
        MX_ERROR_CHECK(err);
    }
abort:;
}

void mx_wp_doc_pos::prev_para(int& err, mx_wp_document* doc)
{
    mx_sheet* s = doc->sheet(err, sheet_num);
    MX_ERROR_CHECK(err);

    sheet_pos.prev_para(err, s);
    if (err == MX_POSITION_RANGE_ERROR) {
        MX_ERROR_CLEAR(err);

        this->prev_sheet(err, doc);
        MX_ERROR_CHECK(err);

        this->moveto_end_sheet(err, doc);
        MX_ERROR_CHECK(err);
    } else {
        MX_ERROR_CHECK(err);
    }
abort:;
}

bool mx_wp_doc_pos::is_area_start(int& err, mx_wp_document* doc)
{
    mx_area_pos* ap = sheet_pos.get_area_pos(err);
    MX_ERROR_CHECK(err);

    return ap->is_start();
abort:
    return FALSE;
}

bool mx_wp_doc_pos::is_area_end(int& err, mx_wp_document* doc)
{
    mx_area_pos* ap;
    mx_area* a;
    bool b;

    ap = sheet_pos.get_area_pos(err);
    MX_ERROR_CHECK(err);

    a = get_area(err, doc);
    MX_ERROR_CHECK(err);

    b = ap->is_end(err, a);
    MX_ERROR_CHECK(err);

    return b;

abort:
    return FALSE;
}

void mx_wp_doc_pos::back_area(int& err, mx_wp_document* doc)
{
    mx_sheet* s;
    mx_area_pos* ap;
    mx_area* a;

    sheet_pos.back_area(err);
    if (err == MX_POSITION_RANGE_ERROR) {
        if (sheet_num > 0) {
            MX_ERROR_CLEAR(err);
            sheet_num--;

            s = doc->sheet(err, sheet_num);
            MX_ERROR_CHECK(err);

            sheet_pos.moveto_end(err, s);
            MX_ERROR_CHECK(err);
        } else {
            MX_ERROR_CHECK(err);
        }
    } else {
        MX_ERROR_CHECK(err);

        s = doc->sheet(err, sheet_num);
        MX_ERROR_CHECK(err);

        sheet_pos.reset(err, s);
        MX_ERROR_CHECK(err);
    }

    ap = sheet_pos.get_area_pos(err);
    MX_ERROR_CHECK(err);

    s = doc->sheet(err, sheet_num);
    MX_ERROR_CHECK(err);

    a = sheet_pos.area(err, s);
    MX_ERROR_CHECK(err);

    ap->moveto_end(err, a);
    MX_ERROR_CHECK(err);

abort:;
}

void mx_wp_doc_pos::make_valid(int& err, mx_wp_document* doc)
{
    int num_sheets = doc->get_num_sheets(err);
    MX_ERROR_CHECK(err);

    if (sheet_num >= num_sheets) {
        moveto_end(err, doc);
        MX_ERROR_CHECK(err);
    } else {
        mx_sheet* s = doc->sheet(err, sheet_num);
        MX_ERROR_CHECK(err);

        sheet_pos.make_valid(err, s);
        MX_ERROR_CHECK(err);
    }

abort:;
}

bool mx_wp_doc_pos::is_same_area_as(mx_wp_doc_pos* p)
{
    return ((this->sheet_num == p->sheet_num) && (this->sheet_pos.get_area_index() == p->sheet_pos.get_area_index()));
}

static void get_area_positions(int& err,
    mx_wp_doc_pos* dp1,
    mx_wp_doc_pos* dp2,
    mx_area_pos*& a1,
    mx_area_pos*& a2)
{
    a1 = dp1->get_area_pos(err);
    MX_ERROR_CHECK(err);

    a2 = dp2->get_area_pos(err);
    MX_ERROR_CHECK(err);
abort:;
}

bool mx_wp_doc_pos::is_same_para_as(int& err, mx_wp_doc_pos* p)
{
    if (this->is_same_area_as(p)) {
        mx_area_pos *p1, *p2;
        get_area_positions(err, this, p, p1, p2);
        MX_ERROR_CHECK(err);

        return p1->is_same_para_as(*p2);
    }
abort:
    return FALSE;
}

bool mx_wp_doc_pos::is_same_cell_as(int& err, mx_wp_doc_pos* p)
{
    if (this->is_same_area_as(p)) {
        mx_area_pos *p1, *p2;
        get_area_positions(err, this, p, p1, p2);
        MX_ERROR_CHECK(err);

        return p1->is_same_cell_as(*p2);
    }
abort:
    return FALSE;
}

bool mx_wp_doc_pos::is_same_row_as(int& err, mx_wp_doc_pos* p)
{
    if (this->is_same_area_as(p)) {
        mx_area_pos *p1, *p2;
        get_area_positions(err, this, p, p1, p2);
        MX_ERROR_CHECK(err);

        return p1->is_same_row_as(*p2);
    }
abort:
    return FALSE;
}

bool mx_wp_doc_pos::is_same_column_as(int& err, mx_wp_doc_pos* p)
{
    if (this->is_same_area_as(p)) {
        mx_area_pos *p1, *p2;
        get_area_positions(err, this, p, p1, p2);
        MX_ERROR_CHECK(err);

        return p1->is_same_column_as(*p2);
    }
abort:
    return FALSE;
}

bool mx_wp_doc_pos::is_at_start_of_para(int& err, mx_wp_document* doc)
{
    bool b = this->is_paragraph(err);
    MX_ERROR_CHECK(err);

    if (b) {
        mx_para_pos* p = this->get_para_pos(err);
        MX_ERROR_CHECK(err);

        return p->is_start();
    }

abort:
    return FALSE;
}

bool mx_wp_doc_pos::is_at_end_of_para(int& err, mx_wp_document* doc)
{
    bool b = this->is_paragraph(err);
    MX_ERROR_CHECK(err);

    if (b) {
        mx_para_pos* p;
        mx_paragraph* para;

        p = this->get_para_pos(err);
        MX_ERROR_CHECK(err);

        para = this->get_para(err, doc);
        MX_ERROR_CHECK(err);

        b = p->is_end(err, para);
        MX_ERROR_CHECK(err);

        return b;
    }

abort:
    return FALSE;
}

bool mx_wp_doc_pos::is_in_split_text_container(int& err,
    mx_wp_document* doc)
{
    mx_area_pos* ap = sheet_pos.get_area_pos(err);
    MX_ERROR_CHECK(err);

    if (ap->is_a(mx_text_area_pos_class_e)) {
        mx_paragraph* p = get_para(err, doc);
        MX_ERROR_CHECK(err);

        return p->get_type() != mx_paragraph::mx_paragraph_whole_e;
    } else if (ap->is_a(mx_table_area_pos_class_e)) {
        mx_table_area::mx_table_type_t tab_type;
        mx_table_area* a = (mx_table_area*)get_area(err, doc);
        MX_ERROR_CHECK(err);
        MX_ERROR_ASSERT(err, a->is_a(mx_table_area_class_e));

        tab_type = a->get_type(err);
        MX_ERROR_CHECK(err);

        return tab_type != mx_table_area::mx_table_whole_e;
    }

abort:
    return FALSE;
}

// moves the position forward, either to the next paragraph, if we are in a
// paragraph, or to the next table area, if we are in a table
static void next_text_container_unit(int& err, mx_wp_doc_pos* pos,
    mx_wp_document* doc)
{
    bool b = pos->is_table(err);
    MX_ERROR_CHECK(err);

    if (b) {
        pos->next_area(err, doc);
        MX_ERROR_CHECK(err);
    } else {
        pos->next_para(err, doc);
        MX_ERROR_CHECK(err);
    }
abort:;
}

bool mx_wp_doc_pos::is_part_of_same_text_container(
    int& err, mx_wp_doc_pos& other, mx_wp_document* doc)
{
    mx_rtti *this_tc, *other_tc;
    mx_wp_doc_pos p2;
    bool b = FALSE;

    if (*this < other) {
        return other.is_part_of_same_text_container(err, *this, doc);
    }

    p2 = other;

    this_tc = this->get_text_container(err, doc);
    MX_ERROR_CHECK(err);

    other_tc = p2.get_text_container(err, doc);
    MX_ERROR_CHECK(err);

    b = is_whole_or_end_of_text_container(err, other_tc);
    MX_ERROR_CHECK(err);

    while (!b && this_tc != other_tc) {
        next_text_container_unit(err, &p2, doc);
        MX_ERROR_CHECK(err);

        other_tc = p2.get_text_container(err, doc);
        MX_ERROR_CHECK(err);

        b = is_whole_or_end_of_text_container(err, other_tc);
        MX_ERROR_CHECK(err);
    }

    return this_tc == other_tc;
abort:
    return FALSE;
}

mx_rtti* mx_wp_doc_pos::get_text_container(int& err, mx_wp_document* doc)
{
    mx_rtti* res = NULL;
    bool b = this->is_table(err);
    MX_ERROR_CHECK(err);

    if (b) {
        res = this->get_area(err, doc);
        MX_ERROR_CHECK(err);
    } else {
        res = this->get_para(err, doc);
        MX_ERROR_CHECK(err);
    }
abort:
    return res;
}

bool mx_wp_doc_pos::is_whole_or_end_of_text_container(int& err, mx_rtti* tc)
{
    mx_rtti_class_e the_class = tc->rtti_class();

    switch (the_class) {
    case mx_table_area_class_e: {
        mx_table_area* taba = (mx_table_area*)tc;
        mx_table_area::mx_table_type_t the_type = taba->get_type(err);
        MX_ERROR_CHECK(err);

        return (the_type == mx_table_area::mx_table_end_e || the_type == mx_table_area::mx_table_whole_e);
    }
    case mx_paragraph_class_e: {
        mx_paragraph* para = (mx_paragraph*)tc;
        mx_paragraph::mx_paragraph_type_t the_type = para->get_type();
        return (the_type == mx_paragraph::mx_paragraph_end_e || the_type == mx_paragraph::mx_paragraph_whole_e);
    }
    default:
        MX_ERROR_THROW(err, MX_ERROR_UNDEFINED);
    }
abort:
    return TRUE;
}

void mx_wp_doc_pos::get_offset_from_start_of_logical_paragraph(
    int& err, mx_wp_document* doc)
{
    mx_wp_doc_pos doc_pos = *this;
    mx_paragraph* p;
    mx_para_pos* pp;
    int letter_index;
    bool not_done = TRUE;

    stored_pos.char_index = 0;

    // loop back through bits of split paragraph counting letters until we get
    // to the logical start of the paragraph
    do {
        pp = doc_pos.get_para_pos(err);
        MX_ERROR_CHECK(err);

        p = doc_pos.get_para(err, doc);
        MX_ERROR_CHECK(err);

        not_done = (p->get_type() != mx_paragraph::mx_paragraph_start_e && p->get_type() != mx_paragraph::mx_paragraph_whole_e);

        pp->get_distance_from_start(err, p, letter_index);
        MX_ERROR_CHECK(err);

        stored_pos.char_index += letter_index;

        if (not_done) {
            doc_pos.moveto_start_para(err, doc);
            MX_ERROR_CHECK(err);

            doc_pos.left(err, doc);
            MX_ERROR_CHECK(err);
        }
    } while (not_done);
abort:;
}

void mx_wp_doc_pos::set_offset_from_start_of_logical_paragraph(
    int& err, mx_wp_document* doc)
{
    mx_wp_doc_pos doc_pos = *this;
    mx_paragraph* p;
    mx_para_pos* pp;
    int letter_index, logical_letter_index;
    bool not_done;

    (void)doc_pos.get_area(err, doc);
    MX_ERROR_CHECK(err);

    p = doc_pos.get_para(err, doc);
    MX_ERROR_CHECK(err);

    // rewind to the start of the logical paragraph
    while (p->get_type() != mx_paragraph::mx_paragraph_start_e && p->get_type() != mx_paragraph::mx_paragraph_whole_e) {
        doc_pos.prev_para(err, doc);
        MX_ERROR_CHECK(err);

        p = doc_pos.get_para(err, doc);
        MX_ERROR_CHECK(err);
    }

    // now move forward through the paragraphs by the number of items in
    // the stored position

    logical_letter_index = stored_pos.char_index;
    not_done = TRUE;
    while (not_done) {
        p = doc_pos.get_para(err, doc);
        MX_ERROR_CHECK(err);

        doc_pos.moveto_end_para(err, doc);
        MX_ERROR_CHECK(err);

        pp = doc_pos.get_para_pos(err);
        MX_ERROR_CHECK(err);

        pp->get_distance_from_start(err, p, letter_index);
        MX_ERROR_CHECK(err);

        if (logical_letter_index <= letter_index) {
            not_done = FALSE;
            pp->set_distance_from_start(err, p, logical_letter_index);
            MX_ERROR_CHECK(err);
            *this = doc_pos;
            return;
        } else {
            logical_letter_index -= letter_index;
            not_done = TRUE;

            doc_pos.next_para(err, doc);
            MX_ERROR_CHECK(err);
        }
    }
    // if we successfully found the original position set this to be the
    // restored position.
    *this = doc_pos;

abort:;
}

void mx_wp_doc_pos::save(int& err, mx_wp_document* doc,
    uint32 start_sheet_num)
{
    mx_paragraph *this_para, *other_para;
    mx_wp_doc_pos other_pos;

    stored_pos = stored_pos_t();
    stored_pos.sheet_index = start_sheet_num;

    other_pos.moveto_start_sheet(err, doc, start_sheet_num);
    MX_ERROR_CHECK(err);

    this_para = this->get_para(err, doc);
    MX_ERROR_CHECK(err);

    other_para = other_pos.get_para(err, doc);
    MX_ERROR_CHECK(err);

    while (this_para != other_para) {
        other_pos.next_para(err, doc);
        MX_ERROR_CHECK(err);

        if (other_para->get_type() == mx_paragraph::mx_paragraph_whole_e || other_para->get_type() == mx_paragraph::mx_paragraph_end_e || this_para == other_para) {
            stored_pos.paragraph_index++;
        }

        other_para = other_pos.get_para(err, doc);
        MX_ERROR_CHECK(err);
    }

    this->get_offset_from_start_of_logical_paragraph(err, doc);
    MX_ERROR_CHECK(err);

abort:;
}

void mx_wp_doc_pos::restore(int& err, mx_wp_document* doc)
{
    mx_paragraph* para;

    moveto_start_sheet(err, doc, stored_pos.sheet_index);
    MX_ERROR_CHECK(err);

    for (uint32 i = 0; i < stored_pos.paragraph_index; i++) {
        do {
            para = this->get_para(err, doc);
            MX_ERROR_CHECK(err);

            this->next_para(err, doc);
            MX_ERROR_CHECK(err);
        } while (para->get_type() != mx_paragraph::mx_paragraph_whole_e && para->get_type() != mx_paragraph::mx_paragraph_end_e);
    }

    set_offset_from_start_of_logical_paragraph(err, doc);
    MX_ERROR_CHECK(err);

abort:;
}

mx_wp_doc_pos::stored_pos_t::stored_pos_t()
{
    sheet_index = paragraph_index = char_index = 0;
}

void mx_wp_doc_pos::next_word(int& err, mx_wp_document* doc)
{
    mx_sheet* s = doc->sheet(err, sheet_num);
    MX_ERROR_CHECK(err);

    sheet_pos.next_word(err, s);
    if (err == MX_POSITION_RANGE_ERROR) {
        next_sheet(err, doc);
        MX_ERROR_CHECK(err);
    } else {
        MX_ERROR_CHECK(err);
    }

abort:;
}

void mx_wp_doc_pos::prev_word(int& err, mx_wp_document* doc)
{
    mx_sheet* s = doc->sheet(err, sheet_num);
    MX_ERROR_CHECK(err);

    sheet_pos.prev_word(err, s);
    if (err == MX_POSITION_RANGE_ERROR) {
        prev_sheet(err, doc);
        MX_ERROR_CHECK(err);

        s = doc->sheet(err, sheet_num);
        MX_ERROR_CHECK(err);

        sheet_pos.moveto_end(err, s);
        MX_ERROR_CHECK(err);
    } else {
        MX_ERROR_CHECK(err);
    }

abort:;
}

void mx_wp_doc_pos::next_logical_word(int& err, mx_wp_document* doc)
{
    bool word_continues = TRUE;
    mx_paragraph* para;
    mx_para_pos* para_pos;

    while (word_continues) {
        para = this->get_para(err, doc);
        MX_ERROR_CHECK(err);

        para_pos = this->get_para_pos(err);
        MX_ERROR_CHECK(err);

        para_pos->next_logical_word(err, para, word_continues);
        if (err == MX_POSITION_RANGE_ERROR || word_continues) {
            MX_ERROR_CLEAR(err);

            this->next_para(err, doc);
            MX_ERROR_CHECK(err);
        } else {
            MX_ERROR_CHECK(err);
        }
    }
abort:;
}

void mx_wp_doc_pos::prev_logical_word(int& err, mx_wp_document* doc)
{
    bool check_for_word_continuation = TRUE;
    mx_paragraph* para;
    mx_para_pos* para_pos;

    while (check_for_word_continuation) {
        para = this->get_para(err, doc);
        MX_ERROR_CHECK(err);

        para_pos = this->get_para_pos(err);
        MX_ERROR_CHECK(err);

        para_pos->prev_logical_word(err, para, check_for_word_continuation);
        if (err == MX_POSITION_RANGE_ERROR) {
            MX_ERROR_CLEAR(err);

            this->prev_para(err, doc);
            MX_ERROR_CHECK(err);

            this->moveto_end_para(err, doc);
            MX_ERROR_CHECK(err);
        } else {
            MX_ERROR_CHECK(err);

            if (check_for_word_continuation) {
                if (this->is_start()) {
                    MX_ERROR_THROW(err, MX_POSITION_RANGE_ERROR);
                } else {
                    // need to check the previous paragraph to see if the
                    // current word is a continuation of a long word in the
                    // previous paragraph

                    mx_wp_doc_pos temp_doc_pos = *this;

                    // get the previous paragraph
                    temp_doc_pos.prev_para(err, doc);
                    MX_ERROR_CHECK(err);

                    para = temp_doc_pos.get_para(err, doc);
                    MX_ERROR_CHECK(err);

                    para_pos = temp_doc_pos.get_para_pos(err);
                    MX_ERROR_CHECK(err);

                    check_for_word_continuation = para->has_split_long_word_at_end(err);
                    MX_ERROR_CHECK(err);

                    if (para->get_type() == mx_paragraph::mx_paragraph_start_e || para->get_type() == mx_paragraph::mx_paragraph_middle_e) {
                        para_pos->moveto_end(err, para);
                        MX_ERROR_CHECK(err);

                        para_pos->left_word(err, para);
                        MX_ERROR_CHECK(err);

                        *this = temp_doc_pos;
                    }
                }
            }
        }
    }
abort:;
}

// don't need to implement this at the moment
void mx_wp_doc_pos::move_left_ignoring_false_breaks(int& err, mx_wp_document* doc) { };

static void move_right_past_false_breaks(int& err, mx_wp_doc_pos* pos,
    mx_wp_document* doc)
{
    mx_paragraph* para;
    mx_para_pos* pp;
    mx_break_word* bw;
    bool need_to_move_right = TRUE;

    // go past the false break words
    while (need_to_move_right) {
        para = pos->get_para(err, doc);
        MX_ERROR_CHECK(err);

        pp = pos->get_para_pos(err);
        MX_ERROR_CHECK(err);

        bw = (mx_break_word*)para->get_word(err, *pp);
        MX_ERROR_CHECK(err);

        need_to_move_right = bw->is_a(mx_break_word_class_e);
        if (need_to_move_right) {
            need_to_move_right = (bw->type() == mx_long_word_break_e || (bw->type() == mx_paragraph_break_e && para->has_split_long_word_at_end(err)));
            MX_ERROR_CHECK(err);
        }

        if (need_to_move_right) {
            pos->right(err, doc);
            MX_ERROR_CHECK(err);
        }
    }
abort:;
}

void mx_wp_doc_pos::move_right_ignoring_false_breaks(int& err, mx_wp_document* doc)
{
    // move past any false breaks at the current position
    move_right_past_false_breaks(err, this, doc);
    MX_ERROR_CHECK(err);

    // now do the real right move
    this->right(err, doc);
    MX_ERROR_CHECK(err);

    // move past any false breaks after the the current position
    move_right_past_false_breaks(err, this, doc);
    MX_ERROR_CHECK(err);
abort:;
}

bool mx_wp_doc_pos::is_end(int& err, mx_wp_document* doc)
{
    mx_sheet* s;
    bool res = FALSE;

    int ns = doc->get_num_sheets(err);
    MX_ERROR_CHECK(err);

    if (sheet_num == ns - 1) {
        s = doc->sheet(err, sheet_num);
        MX_ERROR_CHECK(err);

        res = sheet_pos.is_end(err, s);
        MX_ERROR_CHECK(err);
    }
abort:
    return res;
}

bool mx_wp_doc_pos::is_start()
{
    return sheet_num == 0 && sheet_pos.is_start();
}

bool mx_wp_doc_pos::is_start_line()
{
    return sheet_num == 0 && sheet_pos.is_start_line();
}

bool mx_wp_doc_pos::is_start_para()
{
    return sheet_num == 0 && sheet_pos.is_start_para();
}

void mx_wp_doc_pos::moveto_end_sheet(int& err, mx_wp_document* d)
{
    mx_sheet* s = d->sheet(err, sheet_num);
    MX_ERROR_CHECK(err);

    sheet_pos.moveto_end(err, s);
    MX_ERROR_CHECK(err);
abort:;
}

void mx_wp_doc_pos::moveto_start_sheet(int& err, mx_wp_document* d)
{
    mx_sheet* s = d->sheet(err, sheet_num);
    MX_ERROR_CHECK(err);

    sheet_pos.moveto_start(err, s);
    MX_ERROR_CHECK(err);
abort:;
}
