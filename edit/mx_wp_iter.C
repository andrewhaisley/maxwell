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
#include <mx_wp_doc.h>
#include <mx_wp_iter.h>

mx_wp_iterator::mx_wp_iterator(
    mx_wp_document* doc,
    mx_wp_doc_pos& s,
    mx_wp_doc_pos& e)
{
    int err = MX_ERROR_OK;

    if (s < e) {
        end = e;
        start = s;
    } else {
        end = s;
        start = e;
    }

    document = doc;

    back_to_start(err);
    MX_ERROR_CLEAR(err);

    forward_to_end(err);
    MX_ERROR_CLEAR(err);

    position = start;
}

void mx_wp_iterator::rewind()
{
    position = start;
}

void mx_wp_iterator::increment_area(int& err)
{
    mx_sheet* s;

    s = document->sheet(err, position.get_sheet());
    MX_ERROR_CHECK(err);

    position.get_sheet_pos()->next_area(err, s);
    if (err == MX_POSITION_RANGE_ERROR) {
        MX_ERROR_CLEAR(err);

        position.next_sheet(err, document);
        MX_ERROR_CHECK(err);
    } else {
        MX_ERROR_CHECK(err);
    }
abort:;
}

void mx_wp_iterator::back_to_start(int& err)
{
    bool b;
    mx_paragraph* p;

    while (TRUE) {
        b = start.is_paragraph(err);
        MX_ERROR_CHECK(err);

        if (b) {
            p = start.get_para(err, document);
            MX_ERROR_CHECK(err);

            if (p->get_type() == mx_paragraph::mx_paragraph_middle_e || p->get_type() == mx_paragraph::mx_paragraph_end_e) {
                start.prev_para(err, document);
                MX_ERROR_CHECK(err);
            } else {
                break;
            }
        } else {
            break;
        }
    }
abort:;
}

void mx_wp_iterator::forward_to_end(int& err)
{
    bool b;
    mx_paragraph* p;

    while (TRUE) {
        b = end.is_paragraph(err);
        MX_ERROR_CHECK(err);

        if (b) {
            p = end.get_para(err, document);
            MX_ERROR_CHECK(err);

            if (p->get_type() == mx_paragraph::mx_paragraph_middle_e || p->get_type() == mx_paragraph::mx_paragraph_start_e) {
                end.next_para(err, document);
                MX_ERROR_CHECK(err);
            } else {
                break;
            }
        } else {
            break;
        }
    }
abort:;
}
