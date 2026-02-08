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
 * MODULE/CLASS :  mx_field_word
 *
 * AUTHOR : Andrew Haisley
 *
 *
 *
 * DESCRIPTION:
 *
 * Fields represent something like a date/time or a page number. No
 * sub-classes here but may need to review that if lots more complicated
 * fields come along in the future.
 *
 * A current restriction is that the whole of the field must appear in
 * a character style.
 *
 *
 *
 *
 *
 */

#include "mx_field_w.h"
#include <mx_char_style.h>

int mx_field_word::page_num = 1;

mx_field_word::mx_field_word()
    : mx_word()
{
    word_type = mx_page_num_field_e;
}

mx_field_word::mx_field_word(mx_field_word_type_t t)
    : mx_word()
{
    word_type = t;
    h_format = dt_hh24_mm_e;
    d_format = dt_dd_mm_yyyy_e;
}

mx_field_word::mx_field_word(mx_field_word_type_t t,
    mx_field_dt_format_t h,
    mx_field_dt_format_t d)
{
    word_type = t;
    h_format = h;
    d_format = d;
}

mx_field_word::mx_field_word(const mx_field_word& s)
{
    word_type = s.word_type;
    h_format = s.h_format;
    d_format = s.d_format;
    x = s.x;
    y = s.y;
}

mx_field_word::~mx_field_word()
{
}

float mx_field_word::width(mx_char_style& s)
{
    return s.width((char*)cstring());
}

float mx_field_word::spacing(mx_char_style& s)
{
    return s.line_space;
}

// get the word as a C string
const char* mx_field_word::cstring()
{
    if (word_type == mx_date_time_field_e) {
        return dt_cstring();
    } else {
        return page_cstring();
    }
}

mx_field_word::mx_field_word_type_t mx_field_word::type()
{
    return word_type;
}

// not relevant for page numbers
mx_field_word::mx_field_dt_format_t mx_field_word::hour_format()
{
    return h_format;
}

mx_field_word::mx_field_dt_format_t mx_field_word::date_format()
{
    return d_format;
}

// not relvant for dates
void mx_field_word::set_page_number(int n)
{
    page_num = n;
}

void mx_field_word::serialise(int& err, uint8** buffer)
{
    serialise_enum(word_type, buffer);

    if (word_type == mx_date_time_field_e) {
        serialise_enum(d_format, buffer);
        serialise_enum(h_format, buffer);
    }
}

void mx_field_word::unserialise(int& err, uint8** buffer, uint32 docid)
{
    uint16 e;

    unserialise_enum(e, buffer);
    word_type = (mx_field_word_type_t)e;

    if (word_type == mx_date_time_field_e) {
        unserialise_enum(e, buffer);
        h_format = (mx_field_dt_format_t)e;

        unserialise_enum(e, buffer);
        d_format = (mx_field_dt_format_t)e;
    }
}

uint32 mx_field_word::get_serialised_size(int& err)
{
    if (word_type == mx_date_time_field_e) {
        return SLS_ENUM * 3;
    } else {
        return SLS_ENUM;
    }
}

const char* mx_field_word::dt_cstring()
{
    time_t t;
    struct tm* tp;
    const char* format;

    char temp[MX_FIELD_WORD_MAX_STATIC_LENGTH + 1];

    t = time(&t);
    tp = localtime(&t);

    temp[0] = word_array[0] = '\0';

    switch (h_format) {
    case dt_hh24_mm_ss_e:
        format = "%H:%M:%S";
        break;
    case dt_hh_mm_ss_e:
        format = "%I:%M:%S";
        break;
    case dt_hh24_mm_e:
        format = "%H:%M";
        break;
    case dt_none_e:
    default:
        format = NULL;
    }

    if (format != NULL) {
        strftime(word_array, MX_FIELD_WORD_MAX_STATIC_LENGTH, format, tp);
    }

    switch (d_format) {
    case dt_dd_mm_yyyy_e:
        format = "%d/%m/%Y";
        break;
    case dt_mm_dd_yyyy_e:
        format = "%m/%d/%Y";
        break;
    case dt_d_mon_yyyy_e:
        format = "%d %b %Y";
        break;
    case dt_none_e:
    default:
        format = NULL;
    }

    if (format != NULL) {
        strftime(temp, MX_FIELD_WORD_MAX_STATIC_LENGTH, format, tp);
    }

    if (temp[0] != '\0' && word_array[0] != '\0') {
        strcat(word_array, " ");
    }

    strcat(word_array, temp);

    return word_array;
}

const char* mx_field_word::page_cstring()
{
    sprintf(word_array, "%d", page_num);
    return word_array;
}
