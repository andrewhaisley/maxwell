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

#include <mx_font.h>
#include <mx_font_mod.h>

mx_font_mod::mx_font_mod()
{
    typeface_family_mod = false;
    typeface_size_mod = false;
    typeface_bold_mod = false;
    typeface_italic_mod = false;

    typeface_family_mod_rev = false;
    typeface_size_mod_rev = false;
    typeface_bold_mod_rev = false;
    typeface_italic_mod_rev = false;

    typeface_size = 0.0;

    typeface_bold = false;
    typeface_italic = false;
}

mx_font_mod::~mx_font_mod()
{
}

mx_font_mod::mx_font_mod(const mx_font& s1, const mx_font& s2)
{
    typeface_family_mod = false;
    typeface_size_mod = false;
    typeface_bold_mod = false;
    typeface_italic_mod = false;

    typeface_family_mod_rev = false;
    typeface_size_mod_rev = false;
    typeface_bold_mod_rev = false;
    typeface_italic_mod_rev = false;

    typeface_size = 0.0;

    typeface_bold = false;
    typeface_italic = false;

    // if either typeface family is blank, the font has not been
    // initialised properly, so we can't do very much
    if (s1.m_typeface_family == "" || s2.m_typeface_family == "") {
        mx_printf_warning("FATAL ERROR: typeface family unset in mx_font_mod::mx_font_mod.....");
        exit(-1);
    }

    if (s1.m_typeface_family != s2.m_typeface_family) {
        typeface_family_mod = TRUE;
        set_family(s2.m_typeface_family);
    }

    if (!MX_FLOAT_EQ(s1.typeface_size, s2.typeface_size)) {
        typeface_size_mod = TRUE;
        typeface_size = s2.typeface_size;
    }

    if (s1.typeface_style != s2.typeface_style) {
        if (s1.get_bold() && !s2.get_bold()) {
            typeface_bold_mod = TRUE;
            typeface_bold = FALSE;
        } else if (!s1.get_bold() && s2.get_bold()) {
            typeface_bold_mod = TRUE;
            typeface_bold = TRUE;
        }

        if (s1.get_italic() && !s2.get_italic()) {
            typeface_italic_mod = TRUE;
            typeface_italic = FALSE;
        } else if (!s1.get_italic() && s2.get_italic()) {
            typeface_italic_mod = TRUE;
            typeface_italic = TRUE;
        }
    }
}

mx_font_mod::mx_font_mod(const mx_font_mod& o)
{
    m_typeface_family = "";
    *this = o;
    m_typeface_family = "";
    set_family(o.m_typeface_family);
}

mx_font_mod::mx_font_mod(const mx_font& font)
{
    mx_font f = font;
    m_typeface_family = "";
    set_family_mod(f.get_family());
    set_size_mod(f.get_size());
    set_bold_mod(f.get_bold());
    set_italic_mod(f.get_italic());
}

void mx_font_mod::operator=(const mx_font_mod& o)
{
    set_family("");

    typeface_family_mod = o.typeface_family_mod;
    typeface_size_mod = o.typeface_size_mod;
    typeface_bold_mod = o.typeface_bold_mod;
    typeface_italic_mod = o.typeface_italic_mod;
    typeface_family_mod_rev = o.typeface_family_mod_rev;
    typeface_size_mod_rev = o.typeface_size_mod_rev;
    typeface_bold_mod_rev = o.typeface_bold_mod_rev;
    typeface_italic_mod_rev = o.typeface_italic_mod_rev;
    m_typeface_family = o.m_typeface_family;
    typeface_size = o.typeface_size;
    typeface_bold = o.typeface_bold;
    typeface_italic = o.typeface_italic;

    m_typeface_family = "";
    set_family(o.m_typeface_family);
}

mx_font_mod& mx_font_mod::operator+=(const mx_font_mod& o)
{
    if (o.typeface_family_mod) {
        typeface_family_mod = TRUE;
        typeface_family_mod_rev = o.typeface_family_mod_rev;
        set_family(o.m_typeface_family);
    }

    if (o.typeface_size_mod) {
        typeface_size_mod = TRUE;
        typeface_size_mod_rev = o.typeface_size_mod_rev;
        typeface_size = o.typeface_size;
    }

    if (o.typeface_bold_mod) {
        typeface_bold_mod = TRUE;
        typeface_bold_mod_rev = o.typeface_bold_mod_rev;
        typeface_bold = o.typeface_bold;
    }

    if (o.typeface_italic_mod) {
        typeface_italic_mod = TRUE;
        typeface_italic_mod_rev = o.typeface_italic_mod_rev;
        typeface_italic = o.typeface_italic;
    }
    return *this;
}

void mx_font_mod::set_family(const std::string &s)
{
    m_typeface_family = s;
}

void mx_font_mod::serialise(int& err, uint8** buffer)
{
    serialise_bool(typeface_family_mod, buffer);
    if (typeface_family_mod) {
        serialise_string(m_typeface_family.c_str(), buffer);
    }

    serialise_bool(typeface_size_mod, buffer);
    if (typeface_size_mod) {
        serialise_float(typeface_size, buffer);
    }

    serialise_bool(typeface_bold_mod, buffer);
    if (typeface_bold_mod) {
        serialise_bool(typeface_bold, buffer);
    }

    serialise_bool(typeface_italic_mod, buffer);
    if (typeface_italic_mod) {
        serialise_bool(typeface_italic, buffer);
    }
}

void mx_font_mod::unserialise(int& err, uint8** buffer)
{
    bool b;

    unserialise_bool(b, buffer);
    typeface_family_mod = b;
    if (typeface_family_mod) {
        unserialise_string((char*)temp_buffer, buffer);
        set_family((char*)temp_buffer);
    }

    unserialise_bool(b, buffer);
    typeface_size_mod = b;
    if (typeface_size_mod) {
        unserialise_float(typeface_size, buffer);
    }

    unserialise_bool(b, buffer);
    typeface_bold_mod = b;
    if (typeface_bold_mod) {
        unserialise_bool(b, buffer);
        typeface_bold = b;
    }

    unserialise_bool(b, buffer);
    typeface_italic_mod = b;
    if (typeface_italic_mod) {
        unserialise_bool(b, buffer);
        typeface_italic = b;
    }
}

uint32 mx_font_mod::get_serialised_size(int& err)
{
    int res;

    res = 4 * SLS_BOOL;

    if (typeface_family_mod) {
        res += SLS_STRING(m_typeface_family.c_str());
    }

    if (typeface_size_mod) {
        res += SLS_FLOAT;
    }

    if (typeface_bold_mod) {
        res += SLS_BOOL;
    }

    if (typeface_italic_mod) {
        res += SLS_BOOL;
    }

    return res;
}

bool mx_font_mod::is_null() const
{
    return !typeface_family_mod && !typeface_size_mod && !typeface_bold_mod && !typeface_italic_mod;
}

void mx_font_mod::set_revert_to_default_flags(const mx_font& f)
{
    typeface_family_mod_rev = (typeface_family_mod && (m_typeface_family == f.m_typeface_family));

    typeface_size_mod_rev = (typeface_size_mod && MX_FLOAT_EQ(typeface_size, f.typeface_size));

    typeface_bold_mod_rev = (typeface_bold_mod && ((typeface_bold && f.get_bold()) || (!typeface_bold && !f.get_bold())));

    typeface_italic_mod_rev = (((typeface_italic_mod && (typeface_italic && f.get_italic())) || (!typeface_italic && !f.get_italic())));
}

void mx_font_mod::set_new_default_cs(const mx_font& f)
{
    if (typeface_family_mod && typeface_family_mod_rev) {
        set_family(f.get_family());
    }

    if (typeface_size_mod && typeface_size_mod_rev) {
        typeface_size = f.typeface_size;
    }

    if (typeface_bold_mod && typeface_bold_mod_rev) {
        typeface_bold = f.get_bold();
    }

    if (typeface_italic_mod && typeface_italic_mod_rev) {
        typeface_italic = f.get_italic();
    }
}

void mx_font_mod::clear_revert_to_default_flags()
{
    typeface_family_mod_rev = typeface_size_mod_rev = typeface_bold_mod_rev = typeface_italic_mod_rev = FALSE;
}

void mx_font_mod::clear_mods_in(const mx_font_mod& o)
{
    if (o.typeface_family_mod)
        clear_family_mod();
    if (o.typeface_size_mod)
        clear_size_mod();
    if (o.typeface_bold_mod)
        clear_bold_mod();
    if (o.typeface_italic_mod)
        clear_italic_mod();
}

bool mx_font_mod::get_family_mod(std::string &new_family) const
{
    if (typeface_family_mod)
        new_family = m_typeface_family;
    return typeface_family_mod;
}

bool mx_font_mod::get_size_mod(float& new_size) const
{
    if (typeface_size_mod)
        new_size = typeface_size;
    return typeface_size_mod;
}

bool mx_font_mod::get_bold_mod(bool& bold_on) const
{
    if (typeface_bold_mod)
        bold_on = typeface_bold;
    return typeface_bold_mod;
}

bool mx_font_mod::get_italic_mod(bool& italic_on) const
{
    if (typeface_italic_mod)
        italic_on = typeface_italic;
    return typeface_italic_mod;
}

void mx_font_mod::set_family_mod(const std::string &new_family)
{
    typeface_family_mod = TRUE;
    typeface_family_mod_rev = FALSE;
    set_family(new_family);
}

void mx_font_mod::set_size_mod(float new_size)
{
    typeface_size_mod = TRUE;
    typeface_size_mod_rev = FALSE;
    typeface_size = new_size;
}

void mx_font_mod::set_bold_mod(bool bold_on)
{
    typeface_bold_mod = TRUE;
    typeface_bold_mod_rev = FALSE;
    typeface_bold = bold_on;
}

void mx_font_mod::set_italic_mod(bool italic_on)
{
    typeface_italic_mod = TRUE;
    typeface_italic_mod_rev = FALSE;
    typeface_italic = italic_on;
}

void mx_font_mod::clear_family_mod()
{
    set_family("");
    typeface_family_mod = typeface_family_mod_rev = FALSE;
}

void mx_font_mod::clear_size_mod()
{
    typeface_size = 0.0f;
    typeface_size_mod = typeface_size_mod_rev = FALSE;
}

void mx_font_mod::clear_bold_mod()
{
    typeface_bold = typeface_bold_mod = typeface_bold_mod_rev = FALSE;
}

void mx_font_mod::clear_italic_mod()
{
    typeface_italic = typeface_italic_mod = typeface_italic_mod_rev = FALSE;
}

