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
 * MODULE/CLASS :  mx_wp_repagination_item
 *
 * AUTHOR : Andrew Haisley
 *
 *
 *
 *
 *
 *
 */

#include <mx.h>
#include <mx_rtti.h>
#include <mx_wp_rp_item.h>

#include <mx_ap_iter.h>
#include <mx_area.h>
#include <mx_list_iter.h>
#include <mx_paragraph.h>
#include <mx_style.h>
#include <mx_text_area.h>
#include <mx_wp_doc.h>

typedef struct
{
    bool is_area;

    mx_area* a;
    mx_paragraph* p;
    mx_sheet* s;
    int sheet_index;
} mx_repagination_item_bit_t;

mx_repagination_item::mx_repagination_item(mx_constraint_e c)
{
    constraint = c;
    height = 0.0;
    bits = new mx_list;
}

mx_repagination_item::mx_repagination_item(mx_constraint_e c, float h)
{
    constraint = c;
    height = h;
    bits = new mx_list;
}

mx_repagination_item::~mx_repagination_item()
{
    mx_repagination_item_bit_t* t;
    mx_list_iterator iter(*bits);

    while (iter.more()) {
        t = (mx_repagination_item_bit_t*)iter.data();
        delete t;
    }

    delete bits;
}

void mx_repagination_item::add_area(
    int& err,
    int sheet_num,
    mx_sheet* s,
    mx_area* a)
{
    mx_repagination_item_bit_t* t;

    t = new mx_repagination_item_bit_t;
    t->is_area = TRUE;
    t->a = a;
    t->p = NULL;
    t->s = s;
    t->sheet_index = sheet_num;
    bits->append(t);

    height += a->get_height(err);
    MX_ERROR_CHECK(err);

abort:;
}

void mx_repagination_item::add_para(
    int& err,
    int sheet_num,
    mx_sheet* s,
    mx_text_area* a,
    mx_paragraph* p)
{
    mx_repagination_item_bit_t* t;

    t = new mx_repagination_item_bit_t;
    t->is_area = FALSE;
    t->sheet_index = sheet_num;
    t->a = a;
    t->p = p;
    t->s = s;

    height += p->get_height();

    bits->append(t);
}

int mx_repagination_item::get_num_bits()
{
    return bits->get_num_items();
}

bool mx_repagination_item::is_bit_paragraph(int& err, int i)
{
    mx_repagination_item_bit_t* t;

    t = (mx_repagination_item_bit_t*)bits->get(err, i);
    MX_ERROR_CHECK(err);

    return !(t->is_area);

abort:
    return FALSE;
}

mx_paragraph* mx_repagination_item::get_para(int& err, int i)
{
    mx_repagination_item_bit_t* t;

    t = (mx_repagination_item_bit_t*)bits->get(err, i);
    MX_ERROR_CHECK(err);

    return t->p;

abort:
    return NULL;
}

mx_area* mx_repagination_item::get_area(int& err, int i)
{
    mx_repagination_item_bit_t* t;

    t = (mx_repagination_item_bit_t*)bits->get(err, i);
    MX_ERROR_CHECK(err);

    return t->a;

abort:
    return NULL;
}

mx_sheet* mx_repagination_item::get_sheet(int& err, int i)
{
    mx_repagination_item_bit_t* t;

    t = (mx_repagination_item_bit_t*)bits->get(err, i);
    MX_ERROR_CHECK(err);

    return t->s;

abort:
    return NULL;
}

void mx_repagination_item::print_debug()
{
    mx_list_iterator iter(*bits);
    mx_repagination_item_bit_t* bit;

    fprintf(stderr, "constraint = ");
    switch (constraint) {
    case mx_keep_together_e:
        fprintf(stderr, "together");
        break;
    case mx_keep_with_next_e:
        fprintf(stderr, "with next");
        break;
    case mx_none_e:
        fprintf(stderr, "none");
        break;
    case mx_break_before_e:
        fprintf(stderr, "break before");
        break;
    }
    fprintf(stderr, "\nheight = %.2f\n", height);
    fprintf(stderr, "\ndestination = %d\n", dest_sheet);
    fprintf(stderr, "bits =\n");

    while (iter.more()) {
        bit = (mx_repagination_item_bit_t*)iter.data();
        if (bit->is_area) {
            if (bit->a->rtti_class() == mx_table_area_class_e) {
                fprintf(stderr, "table area from ");
            } else {
                if (bit->a->rtti_class() == mx_text_area_class_e) {
                    fprintf(stderr, "text area from ");
                } else {
                    fprintf(stderr, "area of type %d from ", bit->a->rtti_class());
                }
            }
        } else {
            fprintf(stderr, "paragraph from ");
        }
        fprintf(stderr, "sheet %d\n", bit->sheet_index);
    }
}

bool mx_repagination_item::is_para(int& err)
{
    mx_repagination_item_bit_t* bit;

    bit = (mx_repagination_item_bit_t*)bits->get(err, 0);
    MX_ERROR_CHECK(err);

    return !(bit->is_area);

abort:
    return FALSE;
}

mx_rtti_class_e mx_repagination_item::get_area_class(int& err)
{
    mx_repagination_item_bit_t* bit;

    bit = (mx_repagination_item_bit_t*)bits->get(err, 0);
    MX_ERROR_CHECK(err);

    return bit->a->rtti_class();

abort:
    return mx_area_class_e;
}

void mx_repagination_item::insert_para(
    int& err,
    int bit_num,
    int sheet_num,
    mx_sheet* s,
    mx_text_area* a,
    mx_paragraph* p)
{
    mx_repagination_item_bit_t* bit;

    bit = new mx_repagination_item_bit_t;

    bit->is_area = FALSE;
    bit->a = a;
    bit->p = p;
    bit->s = s;
    bit->sheet_index = sheet_num;

    bits->insert(err, bit_num, bit);
    MX_ERROR_CHECK(err);

abort:;
}

void mx_repagination_item::insert_area(
    int& err,
    int bit_num,
    int sheet_num,
    mx_sheet* s,
    mx_area* a)
{
    mx_repagination_item_bit_t* bit;

    bit = new mx_repagination_item_bit_t;

    bit->is_area = TRUE;
    bit->a = a;
    bit->p = NULL;
    bit->s = s;
    bit->sheet_index = sheet_num;

    bits->insert(err, bit_num, bit);
    MX_ERROR_CHECK(err);

abort:;
}

mx_repagination_item* mx_repagination_item::split(int& err, int i)
{
    mx_repagination_item* new_item;
    int n;

    new_item = new mx_repagination_item(constraint);

    n = bits->get_num_items() - i;

    bits->move_items_out_end(err, n, *(new_item->bits));
    MX_ERROR_CHECK(err);

    recalculate_height(err);
    MX_ERROR_CHECK(err);

    new_item->recalculate_height(err);
    MX_ERROR_CHECK(err);

    return new_item;
abort:
    return NULL;
}

void mx_repagination_item::recalculate_height(int& err)
{
    mx_repagination_item_bit_t* bit;
    mx_list_iterator iter(*bits);

    height = 0.0;

    while (iter.more()) {
        bit = (mx_repagination_item_bit_t*)iter.data();
        if (bit->is_area) {
            height += bit->a->get_height(err);
            MX_ERROR_CHECK(err);
        } else {
            height += bit->p->get_height();
        }
    }
abort:;
}

int mx_repagination_item::get_sheet_index(int& err, int i)
{
    mx_repagination_item_bit_t* t;

    t = (mx_repagination_item_bit_t*)bits->get(err, i);
    MX_ERROR_CHECK(err);

    return t->sheet_index;

abort:
    return -1;
}
