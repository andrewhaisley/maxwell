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
 * MODULE/CLASS :  mx_text_area_pos
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

#include <mx_paragraph.h>
#include <mx_text_area.h>
#include <mx_txt_pos.h>

mx_text_area_pos::mx_text_area_pos()
{
    paragraph_index = 0;
}

mx_text_area_pos::mx_text_area_pos(int iparagraph_index, mx_para_pos& pp)
{
    paragraph_index = iparagraph_index;
    para_pos = pp;
}

mx_text_area_pos::mx_text_area_pos(
    int iparagraph_index,
    int line_index,
    int word_index,
    int letter_index)
{
    paragraph_index = iparagraph_index;
    para_pos.line_index = line_index;
    para_pos.word_index = word_index;
    para_pos.letter_index = letter_index;
}

void mx_text_area_pos::operator=(const mx_text_area_pos& p)
{
    paragraph_index = p.paragraph_index;
    para_pos = p.para_pos;
}

void mx_text_area_pos::up(int& err, mx_area* a)
{
    mx_text_area* ta = (mx_text_area*)a;
    mx_paragraph* p;

    MX_ERROR_ASSERT(err, a->is_a(mx_text_area_class_e));

    p = ta->get(err, paragraph_index);
    MX_ERROR_CHECK(err);

    para_pos.up(err, p);
    if (err == MX_POSITION_RANGE_ERROR) {
        if (paragraph_index == 0) {
            MX_ERROR_CHECK(err);
        } else {
            MX_ERROR_CLEAR(err);

            p = ta->get(err, --paragraph_index);
            MX_ERROR_CHECK(err);

            para_pos.moveto_end(err, p);
            MX_ERROR_CHECK(err);
        }
    } else {
        MX_ERROR_CHECK(err);
    }
abort:;
}

void mx_text_area_pos::down(int& err, mx_area* a)
{
    mx_text_area* ta = (mx_text_area*)a;
    mx_paragraph* p;
    int np;

    MX_ERROR_ASSERT(err, a->is_a(mx_text_area_class_e));

    np = ta->num_paragraphs(err);
    MX_ERROR_CHECK(err);

    p = ta->get(err, paragraph_index);
    MX_ERROR_CHECK(err);

    para_pos.down(err, p);
    if (err == MX_POSITION_RANGE_ERROR) {
        if (paragraph_index == (np - 1)) {
            MX_ERROR_CHECK(err);
        } else {
            MX_ERROR_CLEAR(err);

            p = ta->get(err, ++paragraph_index);
            MX_ERROR_CHECK(err);

            para_pos.moveto_start();
        }
    } else {
        MX_ERROR_CHECK(err);
    }
abort:;
}

void mx_text_area_pos::left(int& err, mx_area* a)
{
    mx_text_area* ta = (mx_text_area*)a;
    mx_paragraph* p;

    MX_ERROR_ASSERT(err, a->is_a(mx_text_area_class_e));

    p = ta->get(err, paragraph_index);
    MX_ERROR_CHECK(err);

    para_pos.left(err, p);
    if (err == MX_POSITION_RANGE_ERROR) {
        if (paragraph_index == 0) {
            MX_ERROR_CHECK(err);
        } else {
            MX_ERROR_CLEAR(err);

            paragraph_index--;

            p = ta->get(err, paragraph_index);
            MX_ERROR_CHECK(err);

            para_pos.moveto_end(err, p);
            MX_ERROR_CHECK(err);
        }
    } else {
        MX_ERROR_CHECK(err);
    }
abort:;
}

void mx_text_area_pos::right(int& err, mx_area* a)
{
    mx_text_area* ta = (mx_text_area*)a;
    mx_paragraph* p;
    int np;

    MX_ERROR_ASSERT(err, a->is_a(mx_text_area_class_e));

    np = ta->num_paragraphs(err);
    MX_ERROR_CHECK(err);

    p = ta->get(err, paragraph_index);
    MX_ERROR_CHECK(err);

    para_pos.right(err, p);
    if (err == MX_POSITION_RANGE_ERROR) {
        if (paragraph_index == (np - 1)) {
            MX_ERROR_CHECK(err);
        } else {
            MX_ERROR_CLEAR(err);

            paragraph_index++;
            para_pos.moveto_start();
        }
    } else {
        MX_ERROR_CHECK(err);
    }
abort:;
}

void mx_text_area_pos::moveto_end(int& err, mx_area* ga)
{
    int np;
    mx_paragraph* p;

    mx_text_area* a = (mx_text_area*)ga;

    MX_ERROR_ASSERT(err, a->is_a(mx_text_area_class_e));

    np = a->num_paragraphs(err);
    MX_ERROR_CHECK(err);

    paragraph_index = np - 1;

    p = a->get(err, np - 1);
    MX_ERROR_CHECK(err);

    para_pos.moveto_end(err, p);
    MX_ERROR_CHECK(err);
abort:;
}

void mx_text_area_pos::moveto_start(int& err, mx_area* ga)
{
    paragraph_index = 0;
    para_pos.moveto_start();
}

mx_area_pos* mx_text_area_pos::duplicate()
{
    mx_text_area_pos* res;

    res = new mx_text_area_pos;
    *res = *this;
    return res;
}

void mx_text_area_pos::left_word(int& err, mx_text_area* ta)
{
    mx_paragraph* p;

    p = ta->get(err, paragraph_index);
    MX_ERROR_CHECK(err);

    para_pos.left_word(err, p);
    if (err == MX_POSITION_RANGE_ERROR) {
        if (paragraph_index == 0) {
            MX_ERROR_CHECK(err);
        } else {
            MX_ERROR_CLEAR(err);

            paragraph_index--;

            p = ta->get(err, paragraph_index);
            MX_ERROR_CHECK(err);

            para_pos.moveto_end(err, p);
            MX_ERROR_CHECK(err);
        }
    } else {
        MX_ERROR_CHECK(err);
    }
abort:;
}

void mx_text_area_pos::right_word(int& err, mx_text_area* ta)
{
    mx_paragraph* p;
    int n;

    p = ta->get(err, paragraph_index);
    MX_ERROR_CHECK(err);

    n = ta->num_paragraphs(err);
    MX_ERROR_CHECK(err);

    para_pos.right_word(err, p);
    if (err == MX_POSITION_RANGE_ERROR) {
        if (paragraph_index == (n - 1)) {
            MX_ERROR_CHECK(err);
        } else {
            MX_ERROR_CLEAR(err);

            paragraph_index++;

            p = ta->get(err, paragraph_index);
            MX_ERROR_CHECK(err);

            para_pos.moveto_start();
        }
    } else {
        MX_ERROR_CHECK(err);
    }
abort:;
}

bool mx_text_area_pos::is_end(int& err, mx_area* ga)
{
    int np;
    bool res;
    mx_text_area* a = (mx_text_area*)ga;

    mx_paragraph* p;

    MX_ERROR_ASSERT(err, a->is_a(mx_text_area_class_e));

    np = a->num_paragraphs(err);
    MX_ERROR_CHECK(err);

    if (paragraph_index == (np - 1)) {
        p = a->get(err, np - 1);
        MX_ERROR_CHECK(err);

        res = para_pos.is_end(err, p);
        MX_ERROR_CHECK(err);

        return res;
    } else {
        return FALSE;
    }

abort:
    return FALSE;
}

bool mx_text_area_pos::is_start()
{
    return paragraph_index == 0 && para_pos.is_start();
}

bool mx_text_area_pos::is_start_line()
{
    return paragraph_index == 0 && para_pos.line_index == 0;
}

bool mx_text_area_pos::is_start_para()
{
    return paragraph_index == 0;
}

mx_point mx_text_area_pos::position(int& err, mx_area* area)
{
    int i;
    mx_text_area* ta = (mx_text_area*)area;
    mx_border_style* bs;

    mx_point res;
    float y_offset = 0.0;
    mx_paragraph* p;

    MX_ERROR_ASSERT(err, area->is_a(mx_text_area_class_e));

    bs = area->get_paragraph_style()->get_border_style();

    for (i = 0; i < paragraph_index; i++) {
        p = ta->get(err, i);
        MX_ERROR_CHECK(err);

        y_offset += p->get_height();
    }

    p = ta->get(err, paragraph_index);
    MX_ERROR_CHECK(err);

    res = para_pos.position(err, p);
    MX_ERROR_CHECK(err);

    res.y += y_offset;

    if (bs->left_style.line_type != mx_no_line) {
        res.x += bs->left_style.width;
    }
    res.x += bs->distance_from_contents;

    if (bs->top_style.line_type != mx_no_line) {
        res.y += bs->top_style.width;
    }
    res.y += bs->distance_from_contents;

abort:
    return res;
}

void mx_text_area_pos::moveto(int& err, mx_area* area, mx_point& point)
{
    int i, np;
    mx_text_area* ta = (mx_text_area*)area;

    mx_point res, temp = point;
    float y_offset = 0.0, height;
    mx_paragraph* p;
    mx_border_style* bs;

    MX_ERROR_ASSERT(err, area->is_a(mx_text_area_class_e));

    bs = area->get_paragraph_style()->get_border_style();

    if (bs->left_style.line_type != mx_no_line)
        temp.x -= bs->left_style.width;
    temp.x -= bs->distance_from_contents;

    if (bs->top_style.line_type != mx_no_line)
        temp.y -= bs->top_style.width;
    temp.y -= bs->distance_from_contents;

    np = ta->num_paragraphs(err);
    MX_ERROR_CHECK(err);

    for (i = 0; i < np; i++) {
        p = ta->get(err, i);
        MX_ERROR_CHECK(err);

        height = p->get_height();

        if (temp.y >= y_offset && temp.y <= (y_offset + height)) {
            paragraph_index = i;
            temp.y -= y_offset;

            para_pos.moveto(err, p, temp);
            MX_ERROR_CHECK(err);

            return;
        } else {
            y_offset += height;
        }
    }
abort:;
}

void mx_text_area_pos::next_para(int& err, mx_area* a)
{
    int n;
    mx_text_area* txta = (mx_text_area*)a;
    MX_ERROR_ASSERT(err, a->is_a(mx_text_area_class_e));

    n = txta->num_paragraphs(err);
    MX_ERROR_CHECK(err);

    paragraph_index++;
    if (paragraph_index >= n) {
        --paragraph_index;
        MX_ERROR_THROW(err, MX_POSITION_RANGE_ERROR);
    }

    para_pos.moveto_start();
abort:;
}

void mx_text_area_pos::prev_para(int& err, mx_area* a)
{
    if (paragraph_index <= 0) {
        MX_ERROR_THROW(err, MX_POSITION_RANGE_ERROR);
    } else {
        paragraph_index--;
    }
    para_pos.moveto_start();
abort:;
}

bool mx_text_area_pos::operator<(const mx_area_pos& p)
{
    mx_text_area_pos* ap = (mx_text_area_pos*)(&p);

    if (p.rtti_class() != this->rtti_class()) {
        return FALSE;
    }

    if (paragraph_index < ap->paragraph_index) {
        return TRUE;
    } else {
        if (paragraph_index == ap->paragraph_index) {
            return para_pos < ap->para_pos;
        } else {
            return FALSE;
        }
    }
}

bool mx_text_area_pos::operator>(const mx_area_pos& p)
{
    mx_text_area_pos* ap = (mx_text_area_pos*)(&p);

    if (p.rtti_class() != this->rtti_class()) {
        return FALSE;
    }

    if (paragraph_index > ap->paragraph_index) {
        return TRUE;
    } else {
        if (paragraph_index == ap->paragraph_index) {
            return para_pos > ap->para_pos;
        } else {
            return FALSE;
        }
    }
}

bool mx_text_area_pos::operator==(const mx_area_pos& p)
{
    mx_text_area_pos* ap = (mx_text_area_pos*)(&p);

    if (p.rtti_class() != this->rtti_class()) {
        return FALSE;
    }

    return paragraph_index == ap->paragraph_index && para_pos == ap->para_pos;
}

bool mx_text_area_pos::operator!=(const mx_area_pos& p)
{
    mx_text_area_pos* ap = (mx_text_area_pos*)(&p);

    if (p.rtti_class() != this->rtti_class()) {
        return FALSE;
    }

    return paragraph_index != ap->paragraph_index || para_pos != ap->para_pos;
}

bool mx_text_area_pos::operator<=(const mx_area_pos& p)
{
    mx_text_area_pos* ap = (mx_text_area_pos*)(&p);

    if (p.rtti_class() != this->rtti_class()) {
        return FALSE;
    }

    return !(*this > *ap);
}

bool mx_text_area_pos::operator>=(const mx_area_pos& p)
{
    mx_text_area_pos* ap = (mx_text_area_pos*)(&p);

    if (p.rtti_class() != this->rtti_class()) {
        return FALSE;
    }

    return !(*this < *ap);
}

bool mx_text_area_pos::is_same_para_as(mx_area_pos& p)
{
    if (!p.is_a(mx_text_area_pos_class_e))
        return FALSE;

    mx_text_area_pos& tp = (mx_text_area_pos&)p;

    return this->paragraph_index == tp.paragraph_index;
}

void mx_text_area_pos::moveto_para_end(int& err, mx_text_area* ta)
{
    mx_paragraph* p;

    p = ta->get(err, paragraph_index);
    MX_ERROR_CHECK(err);

    para_pos.moveto_end(err, p);
    MX_ERROR_CHECK(err);

abort:;
}

void mx_text_area_pos::moveto_para_start(int& err, mx_text_area* area)
{
    para_pos.moveto_start();
}

mx_char_style* mx_text_area_pos::get_char_style(int& err, mx_area* ta)
{
    mx_paragraph* p;
    mx_char_style* res;

    MX_ERROR_ASSERT(err, ta->is_a(mx_text_area_class_e));

    p = ((mx_text_area*)ta)->get(err, paragraph_index);
    MX_ERROR_CHECK(err);

    res = p->get_style(err, para_pos);
    MX_ERROR_CHECK(err);

    return res;
abort:
    return NULL;
}

void mx_text_area_pos::next_para(int& err, mx_text_area* area)
{
    int np;

    np = area->num_paragraphs(err);
    MX_ERROR_CHECK(err);

    if (paragraph_index == (np - 1)) {
        MX_ERROR_THROW(err, MX_POSITION_RANGE_ERROR);
    } else {
        paragraph_index++;
    }

    moveto_para_start(err, area);
    MX_ERROR_CHECK(err);

abort:;
}

void mx_text_area_pos::prev_para(int& err, mx_text_area* area)
{
    if (paragraph_index == 0) {
        MX_ERROR_THROW(err, MX_POSITION_RANGE_ERROR);
    } else {
        paragraph_index--;
    }

    moveto_para_start(err, area);
    MX_ERROR_CHECK(err);

abort:;
}

mx_point mx_text_area_pos::position(int& err, mx_area* area, int line_offset)
{
    int i;
    mx_text_area* ta = (mx_text_area*)area;
    mx_border_style* bs;

    mx_point res;
    float y_offset = 0.0;
    mx_paragraph* p;

    MX_ERROR_ASSERT(err, area->is_a(mx_text_area_class_e));

    bs = area->get_paragraph_style()->get_border_style();

    for (i = 0; i < paragraph_index; i++) {
        p = ta->get(err, i);
        MX_ERROR_CHECK(err);

        y_offset += p->get_height();
    }

    p = ta->get(err, paragraph_index);
    MX_ERROR_CHECK(err);

    res = para_pos.position(err, p, line_offset);
    MX_ERROR_CHECK(err);

    res.y += y_offset;

    if (bs->left_style.line_type != mx_no_line) {
        res.x += bs->left_style.width;
    }
    res.x += bs->distance_from_contents;

    if (bs->top_style.line_type != mx_no_line) {
        res.y += bs->top_style.width;
    }
    res.y += bs->distance_from_contents;

abort:
    return res;
}

void mx_text_area_pos::next_word(int& err, mx_text_area* ta)
{
    mx_paragraph* p;
    int np;

    p = ta->get(err, paragraph_index);
    MX_ERROR_CHECK(err);

    np = ta->num_paragraphs(err);
    MX_ERROR_CHECK(err);

    para_pos.next_word(err, p);
    if (err == MX_POSITION_RANGE_ERROR) {
        paragraph_index++;

        if (paragraph_index == np) {
            MX_ERROR_CHECK(err);
        } else {
            MX_ERROR_CLEAR(err);
            para_pos.moveto_start();
        }
    } else {
        MX_ERROR_CHECK(err);
    }

abort:;
}

void mx_text_area_pos::prev_word(int& err, mx_text_area* ta)
{
    mx_paragraph* p;

    p = ta->get(err, paragraph_index);
    MX_ERROR_CHECK(err);

    para_pos.prev_word(err, p);
    if (err == MX_POSITION_RANGE_ERROR) {
        if (paragraph_index == 0) {
            MX_ERROR_CHECK(err);
        } else {
            MX_ERROR_CLEAR(err);
            paragraph_index--;

            p = ta->get(err, paragraph_index);
            MX_ERROR_CHECK(err);

            para_pos.moveto_end(err, p);
            MX_ERROR_CHECK(err);
        }
    } else {
        MX_ERROR_CHECK(err);
    }

abort:;
}
