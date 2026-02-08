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
 * MODULE/CLASS :  mx_area_para_iter
 *
 * AUTHOR : Andrew Haisley
 *
 *
 *
 * DESCRIPTION:
 *  a class that iterates over the paragraphs in text areas and whole areas
 *  for other classes
 *
 *

 *
 */
#include <mx.h>
#include <mx_ap_iter.h>
#include <mx_paragraph.h>
#include <mx_rtti.h>
#include <mx_ta_pos.h>
#include <mx_txt_pos.h>
#include <mx_wp_doc.h>

mx_area_para_iterator::mx_area_para_iterator(
    mx_wp_document* doc,
    mx_wp_doc_pos& s,
    mx_wp_doc_pos& e)
    : mx_wp_iterator(doc, s, e)
{
}

bool mx_area_para_iterator::more(int& err)
{
    bool b;
    mx_paragraph *para1, *para2;
    mx_area* a;

    if (position.get_sheet() == end.get_sheet()) {
        if (position.get_sheet_pos()->get_area_index() == end.get_sheet_pos()->get_area_index()) {
            a = position.get_area(err, document);
            MX_ERROR_CHECK(err);

            b = position.is_paragraph(err);
            MX_ERROR_CHECK(err);

            if (b && (a->rtti_class() == mx_text_area_class_e)) {
                para1 = position.get_para(err, document);
                MX_ERROR_CHECK(err);

                para2 = end.get_para(err, document);
                MX_ERROR_CHECK(err);

                if (para1 == para2) {
                    return FALSE;
                } else {
                    increment(err);
                    MX_ERROR_CHECK(err);

                    return TRUE;
                }
            } else {
                increment(err);
                MX_ERROR_CHECK(err);

                return TRUE;
            }
        } else {
            increment(err);
            MX_ERROR_CHECK(err);

            return TRUE;
        }
    } else {
        increment(err);
        MX_ERROR_CHECK(err);

        return TRUE;
    }
abort:
    MX_ERROR_CLEAR(err);
    return FALSE;
}

void mx_area_para_iterator::increment(int& err)
{
    mx_area_pos* ap;
    mx_area* a;

    ap = position.get_area_pos(err);
    MX_ERROR_CHECK(err);

    a = position.get_area(err, document);
    MX_ERROR_CHECK(err);

    if (ap->rtti_class() == mx_text_area_pos_class_e) {
        ((mx_text_area_pos*)ap)->next_para(err, (mx_text_area*)a);
        if (err == MX_POSITION_RANGE_ERROR) {
            MX_ERROR_CLEAR(err);

            increment_area(err);
            MX_ERROR_CHECK(err);
        } else {
            MX_ERROR_CHECK(err);
        }
    } else {
        increment_area(err);
        MX_ERROR_CHECK(err);
    }

abort:;
}

mx_rtti* mx_area_para_iterator::data(int& err)
{
    bool b;
    mx_paragraph* para;
    mx_area* a;

    b = position.is_paragraph(err);
    MX_ERROR_CHECK(err);

    a = position.get_area(err, document);
    MX_ERROR_CHECK(err);

    if (b && (a->rtti_class() == mx_text_area_class_e)) {
        para = position.get_para(err, document);
        MX_ERROR_CHECK(err);

        return para;
    } else {
        return a;
    }

abort:
    return NULL;
}

int mx_area_para_iterator::sheet_num()
{
    return position.get_sheet();
}

mx_area* mx_area_para_iterator::area()
{
    mx_area* a;
    int err = MX_ERROR_OK;

    a = position.get_area(err, document);
    MX_ERROR_CHECK(err);

    return a;
abort:
    MX_ERROR_CLEAR(err);
    return NULL;
}

int mx_area_para_iterator::para_num()
{
    mx_text_area_pos* ap;
    int err = MX_ERROR_OK;

    ap = (mx_text_area_pos*)position.get_area_pos(err);
    MX_ERROR_CHECK(err);

    return ap->paragraph_index;

abort:
    MX_ERROR_CLEAR(err);
    return -1;
}

int mx_area_para_iterator::area_index()
{
    return position.get_sheet_pos()->get_area_index();
}

mx_sheet* mx_area_para_iterator::sheet()
{
    int sn;
    int err = MX_ERROR_OK;

    mx_sheet* res;

    sn = position.get_sheet();

    res = document->sheet(err, sn);
    MX_ERROR_CHECK(err);

    return res;

abort:
    MX_ERROR_CLEAR(err);
    return NULL;
}

mx_paragraph* mx_area_para_iterator::para()
{
    int pn;
    int err = MX_ERROR_OK;

    mx_text_area* ta;
    mx_paragraph* res;

    pn = para_num();

    ta = (mx_text_area*)area();

    res = ta->get(err, pn);
    MX_ERROR_CHECK(err);

    return res;

abort:
    MX_ERROR_CLEAR(err);
    return NULL;
}
