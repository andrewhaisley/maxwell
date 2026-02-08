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
 * MODULE/CLASS :  mx_wp_area_iter
 *
 * AUTHOR : Andrew Haisley
 *
 *
 *
 * DESCRIPTION:
 *  a class that iterates over areas in a wp document
 *
 *
 *
 *
 */
#include <mx_wp_area_iter.h>
#include <mx_wp_doc.h>

mx_wp_area_iterator::mx_wp_area_iterator(
    mx_wp_document* doc,
    mx_wp_doc_pos& start,
    mx_wp_doc_pos& end)
    : mx_wp_iterator(doc, start, end)
{
}

bool mx_wp_area_iterator::more(int& err)
{
    if (position.get_sheet() == end.get_sheet()) {
        if (position.get_sheet_pos()->get_area_index() == end.get_sheet_pos()->get_area_index()) {
            return FALSE;
        } else {
            increment_area(err);
            MX_ERROR_CHECK(err);

            return TRUE;
        }
    } else {
        increment_area(err);
        MX_ERROR_CHECK(err);

        return TRUE;
    }
abort:
    MX_ERROR_CLEAR(err);
    return FALSE;
}

mx_rtti* mx_wp_area_iterator::data(int& err)
{
    mx_area* a;
    mx_sheet* s;

    s = document->sheet(err, position.get_sheet());
    MX_ERROR_CHECK(err);

    a = position.get_sheet_pos()->area(err, s);
    MX_ERROR_CHECK(err);

    return (mx_rtti*)a;

abort:
    return NULL;
}
