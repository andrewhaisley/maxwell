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
 * MODULE/CLASS :  mx_wp_style_iter
 *
 * AUTHOR : Andrew Haisley
 *
 *
 *
 * DESCRIPTION:
 *  a class that iterates over things that have styles in a wp document
 *
 *
 *
 *
 */
#include <mx.h>
#include <mx_paragraph.h>
#include <mx_rtti.h>
#include <mx_ta_pos.h>
#include <mx_txt_pos.h>
#include <mx_wp_doc.h>
#include <mx_wp_style_iter.h>

mx_wp_style_iterator::mx_wp_style_iterator(
    mx_wp_document* doc,
    mx_wp_doc_pos& s,
    mx_wp_doc_pos& e)
    : mx_wp_iterator(doc, s, e)
{
    int err = MX_ERROR_OK;

    iterating_over_table_area = s.is_table(err);
    MX_ERROR_CHECK(err);

    iterating_over_table_area = (iterating_over_table_area && e.is_table(err));
    MX_ERROR_CHECK(err);

    iterating_over_table_area = (iterating_over_table_area && s.is_same_area_as(&e));
    return;
abort:
    global_error_trace->print();
    MX_ERROR_CLEAR(err);
}

bool mx_wp_style_iterator::more(int& err)
{
    bool return_val = TRUE, pos_is_para, end_is_para;
    mx_rtti *thing1, *thing2;

    pos_is_para = position.is_paragraph(err);
    MX_ERROR_CHECK(err);

    end_is_para = end.is_paragraph(err);
    MX_ERROR_CHECK(err);

    if (pos_is_para && end_is_para) {
        thing1 = position.get_para(err, document);
        MX_ERROR_CHECK(err);

        thing2 = end.get_para(err, document);
        MX_ERROR_CHECK(err);

        return_val = (thing1 != thing2);
    } else if (!pos_is_para && !end_is_para) {
        thing1 = position.get_area(err, document);
        MX_ERROR_CHECK(err);

        thing2 = end.get_area(err, document);
        MX_ERROR_CHECK(err);

        return_val = (thing1 != thing2);
    }

    if (return_val) {
        increment(err);
        MX_ERROR_CHECK(err);
    }

    return return_val;
abort:
    return FALSE;
}

void mx_wp_style_iterator::increment(int& err)
{
    position.next_para(err, document);
    MX_ERROR_CHECK(err);

    if (iterating_over_table_area) {
        int32 start_column, end_column;

        mx_table_area* taba;
        mx_table_area_pos *tabp, *start_p, *end_p;

        taba = (mx_table_area*)position.get_area(err, document);
        MX_ERROR_CHECK(err);

        tabp = (mx_table_area_pos*)position.get_area_pos(err);
        MX_ERROR_CHECK(err);

        start_p = (mx_table_area_pos*)start.get_area_pos(err);
        MX_ERROR_CHECK(err);

        end_p = (mx_table_area_pos*)end.get_area_pos(err);
        MX_ERROR_CHECK(err);

        MX_ERROR_ASSERT(err, taba->is_a(mx_table_area_class_e));
        MX_ERROR_ASSERT(err, tabp->is_a(mx_table_area_pos_class_e));
        MX_ERROR_ASSERT(err, start_p->is_a(mx_table_area_pos_class_e));
        MX_ERROR_ASSERT(err, end_p->is_a(mx_table_area_pos_class_e));
        MX_ERROR_ASSERT(err, *start_p <= *end_p);

        start_column = start_p->column < end_p->column ? start_p->column : end_p->column;
        end_column = start_p->column > end_p->column ? start_p->column : end_p->column;

        if (tabp->column > end_column) {
            tabp->row++;
            tabp->column = start_column;

            tabp->moveto_cell_start(err, taba);
            MX_ERROR_CHECK(err);
        } else if (tabp->column < start_column) {
            tabp->column = start_column;

            tabp->moveto_cell_start(err, taba);
            MX_ERROR_CHECK(err);
        }
    }
abort:;
}

mx_rtti* mx_wp_style_iterator::data(int& err)
{
    mx_rtti* thing = NULL;
    bool b = position.is_paragraph(err);
    MX_ERROR_CHECK(err);

    if (b) {
        thing = position.get_para(err, document);
    } else {
        thing = position.get_area(err, document);
    }
    MX_ERROR_CHECK(err);

abort:
    return thing;
}
