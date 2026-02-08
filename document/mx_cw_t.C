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
 * MODULE/CLASS :  mx_cw_t
 *
 * AUTHOR : Tom Newton
 *
 *
 *
 * DESCRIPTION:
 *
 * Items of different sorts in a complex word.
 *
 *
 *
 *
 */

#include "mx_cw_t.h"
#include <mx_area.h>
#include <mx_char_style.h>
#include <mx_dg_area.h>
#include <mx_field_w.h>
#include <mx_image_area.h>

void mx_cw_t::mod_style(mx_char_style& s) { }

float mx_cw_t::spacing(const mx_char_style& s)
{
    return s.line_space;
}

float mx_cw_t::ascender(const mx_char_style& s)
{
    return s.get_ascender();
}

float mx_cw_t::descender(const mx_char_style& s)
{
    return s.get_descender();
}

bool mx_cw_t::line_difference()
{
    return FALSE;
}

mx_character_cw_t::mx_character_cw_t(char nc)
{
    c = nc;
}

mx_cw_t* mx_character_cw_t::deep_copy()
{
    return new mx_character_cw_t(*this);
}

float mx_character_cw_t::width(const mx_char_style& s)
{
    return s.width(c);
}

uint32 mx_character_cw_t::get_serialised_size(int& err)
{
    return SLS_INT8;
}

void mx_character_cw_t::serialise(int& err, uint8** buffer)
{
    serialise_int8(c, buffer);
}

void mx_character_cw_t::unserialise(int& err, uint8** buffer, uint32 docid)
{
    unserialise_char(c, buffer);
}

mx_style_mod_cw_t::mx_style_mod_cw_t(const mx_char_style_mod& nm)
    : m(nm)
{
}

mx_cw_t* mx_style_mod_cw_t::deep_copy()
{
    return new mx_style_mod_cw_t(*this);
}

void mx_style_mod_cw_t::mod_style(mx_char_style& s)
{
    s += m;
}

bool mx_style_mod_cw_t::line_difference()
{
    return TRUE;
}

float mx_style_mod_cw_t::width(const mx_char_style& s)
{
    return 0.0;
}

uint32 mx_style_mod_cw_t::get_serialised_size(int& err)
{
    return m.get_serialised_size(err);
}

void mx_style_mod_cw_t::serialise(int& err, uint8** buffer)
{
    m.serialise(err, buffer);
    MX_ERROR_CHECK(err);
abort:;
}

void mx_style_mod_cw_t::unserialise(int& err, uint8** buffer, uint32 docid)
{
    m.unserialise(err, buffer);
    MX_ERROR_CHECK(err);
abort:;
}

mx_field_word_cw_t::mx_field_word_cw_t(const mx_field_word& nw)
    : w(nw)
{
}

mx_cw_t* mx_field_word_cw_t::deep_copy()
{
    return new mx_field_word_cw_t(*this);
}

float mx_field_word_cw_t::width(const mx_char_style& s)
{
    return s.width(w.cstring());
}

uint32 mx_field_word_cw_t::get_serialised_size(int& err)
{
    return w.get_serialised_size(err);
}

void mx_field_word_cw_t::serialise(int& err, uint8** buffer)
{
    w.serialise(err, buffer);
    MX_ERROR_CHECK(err);
abort:;
}

void mx_field_word_cw_t::unserialise(int& err, uint8** buffer, uint32 docid)
{
    w.unserialise(err, buffer, docid);
    MX_ERROR_CHECK(err);
abort:;
}

mx_area_cw_t::mx_area_cw_t(mx_area* na)
{
    a = na;
}

mx_area_cw_t::~mx_area_cw_t()
{
    delete a;
}

mx_cw_t* mx_area_cw_t::deep_copy()
{
    return new mx_area_cw_t(a->duplicate());
}

void mx_area_cw_t::move_to_document(int& err, mx_document* doc)
{
    a->move_to_document(err, doc);
    MX_ERROR_CHECK(err);
abort:;
}

float mx_area_cw_t::width(const mx_char_style& s)
{
    int err = MX_ERROR_OK;
    float res = a->get_width(err);
    MX_ERROR_CHECK(err);
    return res;
abort:
    global_error_trace->print();
    MX_ERROR_CLEAR(err);
    return 0;
}

float mx_area_cw_t::height(const mx_char_style& s)
{
    int err = MX_ERROR_OK;
    float res = a->get_height(err);
    MX_ERROR_CHECK(err);
    return res;
abort:
    global_error_trace->print();
    MX_ERROR_CLEAR(err);
    return 0;
}

float mx_area_cw_t::ascender(const mx_char_style& s)
{
    return height(s);
}

float mx_area_cw_t::descender(const mx_char_style& s)
{
    return 0.0;
}

bool mx_area_cw_t::line_difference()
{
    return TRUE;
}

uint32 mx_area_cw_t::get_serialised_size(int& err)
{
    return SLS_ENUM + SLS_UINT32;
}

void mx_area_cw_t::serialise(int& err, uint8** buffer)
{
    uint32 oid = a->get_db_id(err);
    MX_ERROR_CHECK(err);

    serialise_enum(a->rtti_class(), buffer);
    serialise_uint32(oid, buffer);
abort:;
}

void mx_area_cw_t::unserialise(int& err, uint8** buffer, uint32 docid)
{
    uint32 oid = 0;
    uint16 t;

    unserialise_enum(t, buffer);
    unserialise_uint32(oid, buffer);

    switch ((mx_rtti_class_e)t) {
    case mx_image_area_class_e:
        a = new mx_image_area(docid, oid);
        break;
    case mx_diagram_area_class_e:
        a = new mx_diagram_area(docid, oid);
        break;
    default:
        // should not be here
        MX_ERROR_THROW(err, MX_ERROR_UNDEFINED);
    }
abort:;
}
