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
#ifndef MX_FIELD_WORD_H
#define MX_FIELD_WORD_H

#include <mx.h>
#include <mx_rtti.h>
#include <mx_word.h>

// this must be big enough to hold the widest field
#define MX_FIELD_WORD_MAX_STATIC_LENGTH 50

class mx_field_word : public mx_word {
    MX_RTTI(mx_field_word_class_e)

public:

    typedef enum {
        mx_date_time_field_e,
        mx_page_num_field_e
    } mx_field_word_type_t;

    typedef enum {
        dt_dd_mm_yyyy_e = 1,
        dt_mm_dd_yyyy_e = 2,
        dt_d_mon_yyyy_e = 3,
        dt_hh24_mm_ss_e = 4,
        dt_hh_mm_ss_e = 5,
        dt_hh24_mm_e = 6,
        dt_none_e = 7
    } mx_field_dt_format_t;

    mx_field_word();
    mx_field_word(mx_field_word_type_t t);

    mx_field_word(
        mx_field_word_type_t t,
        mx_field_dt_format_t h_format,
        mx_field_dt_format_t d_format);

    mx_field_word(const mx_field_word& s);
    virtual ~mx_field_word();

    virtual mx_word* duplicate() { return new mx_field_word(*this); };

    // the width according to the underlying outline font.
    virtual float width(mx_char_style& s);

    // the spacing according to the underlying character style
    virtual float spacing(mx_char_style& s);

    // get the word as a C string
    virtual const char* cstring();

    mx_field_word_type_t type();

    // not relevant for page numbers
    mx_field_dt_format_t hour_format();
    mx_field_dt_format_t date_format();

    // not relvant for dates
    void set_page_number(int n);

    virtual void serialise(int& err, uint8** buffer);
    virtual void unserialise(int& err, uint8** buffer, uint32 docid);
    virtual uint32 get_serialised_size(int& err);

private:
    const char* dt_cstring();
    const char* page_cstring();

    char word_array[MX_FIELD_WORD_MAX_STATIC_LENGTH + 1];

    mx_field_word_type_t word_type;
    mx_field_dt_format_t h_format;
    mx_field_dt_format_t d_format;

    static int page_num;
};

#endif
