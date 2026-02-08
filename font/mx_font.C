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
 * MODULE/CLASS : mx_font
 *
 * AUTHOR : Tom Newton
 *
 *
 *
 * DESCRIPTION:
 *
 * This class is a font class and includes methods for querying a font about
 * its metrics and such like..
 *
 *
 *
 */

#include <mx_font.h>
#include <Intrinsic.h>
#include <mx.h>
#include <mx_font_metrics.h>
#include <mx_font_metrics_store.h>
#include <mx_font_mod.h>


mx_font_metrics_store* mx_font::m_font_metrics_store = nullptr;
std::map<mx_font_key_t, XftFont *> mx_font::m_font_cache;
FcConfig *mx_font::m_config = nullptr;

extern char* global_maxhome;
extern Widget global_top_level;

mx_font::mx_font()
{
    int err = MX_ERROR_OK;

    init_font();

    set_family(err, get_default_roman_font());
    MX_ERROR_CHECK(err);

    return;
abort:
    global_error_trace->print();
    MX_ERROR_CLEAR(err);
    mx_printf_warning("FATAL ERROR: Cannot find default font. Exiting.....");
    exit(EXIT_FAILURE);
    return;
}

std::string mx_font::get_default_roman_font()
{
    return m_font_metrics_store->get_default_roman_font_family_name().c_str();
}

std::string mx_font::get_default_symbol_font()
{
    return m_font_metrics_store->get_default_symbol_font_family_name().c_str();
}

std::vector<std::string> mx_font::get_font_family_names()
{
    return m_font_metrics_store->get_font_family_names();
}

mx_font::mx_font(int& err, const std::string &family_name, float new_size, mx_font_style_t new_style)
{
    init_font();

    if (family_name == "")
    {
        set_family(err, get_default_roman_font());
        MX_ERROR_CHECK(err);
    }
    else
    {
        set_family(err, family_name);
        MX_ERROR_CHECK(err);
    }

    set_style(err, new_style);
    MX_ERROR_CHECK(err);

    set_size(new_size);

    return;
abort:
    return;
}

mx_font::~mx_font()
{
}

void mx_font::init_font()
{
    if (m_font_metrics_store == nullptr) {
        m_font_metrics_store = new mx_font_metrics_store;
        m_config = FcInitLoadConfigAndFonts();
    }

    typeface_size = 12.0;
    typeface_style = mx_normal;
}

bool mx_font::set_family(int& err, const std::string &new_family_name)
{
    std::shared_ptr<mx_font_family> ff;
    mx_font_metrics* fm;

    if (m_typeface_family == "" || new_family_name != m_typeface_family) {

        ff = m_font_metrics_store->get_font_family(new_family_name);

        if (!ff) {
            MX_ERROR_THROW(err, MX_FONT_STYLE_NOT_AVAILABLE);
        }

        fm = ff->get_font_metrics(get_style());

        if (fm == nullptr) {
            MX_ERROR_THROW(err, MX_FONT_STYLE_NOT_AVAILABLE);
        }

        m_typeface_family = fm->get_family_name();
        m_font_family = ff;
    }

    return true;
abort:
    return false;
}

bool mx_font::set_nearest_family(int& err, const std::string &new_family_name)
{
    auto ff = m_font_metrics_store->get_nearest_font_family(new_family_name);

    if (!ff)
    {
        return false;
    }

    set_family(err, ff->get_name());
    if (err == MX_FONT_STYLE_NOT_AVAILABLE) {
        MX_ERROR_CLEAR(err);
        set_style(err, mx_normal);
    }
    MX_ERROR_CHECK(err);

    return true;
abort:
    return false;
}

bool mx_font::set_size(float new_size)
{
    if (!MX_FLOAT_EQ(typeface_size, new_size)) {
        typeface_size = new_size;
        return true;
    }
    return false;
}

bool mx_font::set_style(int& err, mx_font_style_t new_style)
{
    if (new_style != typeface_style) {
        if (m_font_family->get_font_metrics(new_style) == nullptr) {
            MX_ERROR_THROW(err, MX_FONT_STYLE_NOT_AVAILABLE);
        }
        typeface_style = new_style;
    }
    return true;
abort:
    return false;
}

uint32 mx_font::em_width(char character) const
{
    return m_font_family->get_font_metrics(typeface_style)->get_em_width((uint8)character);
}

std::string mx_font::get_filename() const
{
    return m_font_family->get_font_metrics(get_style())->get_file_name();
}

extern Widget global_top_level;

uint32 mx_font::get_serialised_size(int& err)
{
    return SLS_STRING(m_typeface_family.c_str()) + SLS_FLOAT + SLS_ENUM;
}

void mx_font::serialise(int& err, uint8** buffer)
{
    serialise_string(m_typeface_family.c_str(), buffer);
    serialise_float(typeface_size, buffer);
    serialise_enum(typeface_style, buffer);
}

void mx_font::unserialise(int& err, uint8** buffer)
{
    uint16 e;

    unserialise_string((char*)temp_buffer, buffer);
    set_family(err, (char*)temp_buffer);
    MX_ERROR_CHECK(err);
    unserialise_float(typeface_size, buffer);
    unserialise_enum(e, buffer);
    typeface_style = (mx_font_style_t)e;
abort:;
}

void mx_font::set_bold(int& err, bool flag)
{
    bool is_bold = get_bold();

    if ((flag && is_bold) || (!flag && !is_bold))
        return;

    if (flag) {
        if (get_italic()) {
            set_style(err, mx_bold_italic);
        } else {
            set_style(err, mx_bold);
        }
    } else {
        if (get_italic()) {
            set_style(err, mx_italic);
        } else {
            set_style(err, mx_normal);
        }
    }
    MX_ERROR_CHECK(err);
    return;
abort:
    return;
}

void mx_font::set_italic(int& err, bool flag)
{
    bool is_italic = get_italic();

    if ((flag && is_italic) || (!flag && !is_italic))
        return;

    if (flag) {
        if (get_bold()) {
            set_style(err, mx_bold_italic);
        } else {
            set_style(err, mx_italic);
        }
    } else {
        if (get_bold()) {
            set_style(err, mx_bold);
        } else {
            set_style(err, mx_normal);
        }
    }
    MX_ERROR_CHECK(err);
    return;
abort:
    return;
}

// height in MM above the baseline of the tallest character in this font
float mx_font::get_ascender() const
{
    return MX_POINTS_TO_MM(
        (m_font_family->get_font_metrics(get_style())->get_em_ascender() * typeface_size) / 1000.0);
}

// depth in MM below the baseline of the lowest character in this font
float mx_font::get_descender() const
{
    return MX_POINTS_TO_MM(
        (m_font_family->get_font_metrics(get_style())->get_em_descender() * typeface_size) / 1000.0);
}

bool mx_font::has_latin_encoding(int& err)
{
    return true;
    // TODO: get this from FcFont stuff
}

bool operator==(const mx_font& f1, const mx_font& f2)
{
    return (
        (f1.typeface_size == f2.typeface_size) && 
        (f1.typeface_style == f2.typeface_style) && 
        (f1.m_typeface_family == f2.m_typeface_family));
}

mx_font& mx_font::operator+=(const mx_font_mod& m)
{
    int err = MX_ERROR_OK;
    if (m.typeface_family_mod) {
        set_family(err, m.m_typeface_family);
        if (err != MX_ERROR_OK) {
            MX_ERROR_CLEAR(err);
            set_nearest_family(err, m.m_typeface_family);
            MX_ERROR_CHECK(err);
        }
    }

    if (m.typeface_size_mod) {
        set_size(m.typeface_size);
    }

    if (m.typeface_bold_mod) {
        set_bold(err, m.typeface_bold);
        MX_ERROR_CHECK(err);
    }

    if (m.typeface_italic_mod) {
        set_italic(err, m.typeface_italic);
        MX_ERROR_CHECK(err);
    }

    return *this;
abort:
    global_error_trace->print();
    MX_ERROR_CLEAR(err);
    return *this;
}

std::string mx_font::get_family() const
{
    return m_typeface_family;
}

float mx_font::get_size() const
{
    return typeface_size;
}

mx_font_style_t mx_font::get_style() const
{
    return typeface_style;
}

float mx_font::em_to_maxwell(uint32 em_width) const
{
    // This width is the range 0 - 1000  - convert to
    // Maxwell units (floating mm) for this point size
    return MX_POINTS_TO_MM(((float)(em_width * get_size())) / 1000.0);
}

float mx_font::width(const std::string &s) const
{
    uint32 res = 0;

    for (auto c : s) {
        res += em_width(c);
    }

    return em_to_maxwell(res);
}

float mx_font::width(char character) const
{
    return em_to_maxwell(em_width(character));
}

uint32 mx_font::em_to_maxwell_int(uint32 em_width) const
{
    return (int)(MX_POINTS_TO_MM((float)(em_width * get_size()) * 10.0));
}

bool mx_font::get_bold() const
{
    return typeface_style == mx_bold || typeface_style == mx_bold_italic;
}

bool mx_font::get_italic() const
{
    return typeface_style == mx_italic || typeface_style == mx_bold_italic;
}

bool operator!=(const mx_font& f1, const mx_font& f2)
{
    return !(f1 == f2);
}

XftFont *mx_font::get_xft_font(int &err, Display *dpy, float scale) const
{
    mx_font_key_t k;

    k.m_family = get_family();
    k.m_size = typeface_size;
    k.m_style = typeface_style;
    k.m_scale = scale;
    k.m_resolution = get_resolution();

    if (m_font_cache.find(k) != m_font_cache.end())
    {
        return m_font_cache[k];
    }

    FcPattern* pat = FcNameParse((const FcChar8 *)get_family().c_str());

    if (!pat) {
        MX_ERROR_THROW(err, MX_XFT_FAILED);
    }

    FcPatternAddDouble(pat, FC_SIZE, typeface_size * scale);
    FcPatternAddDouble(pat, FC_DPI, get_resolution());
    FcPatternAddBool(pat, FC_ANTIALIAS, FcTrue);
    FcPatternAddBool(pat, FC_OUTLINE, FcTrue);

    switch (typeface_style)
    {
    default:
    case mx_normal:
        FcPatternAddInteger(pat, FC_WEIGHT, FC_WEIGHT_REGULAR);
        FcPatternAddInteger(pat, FC_SLANT, FC_SLANT_ROMAN);
        break;
    case mx_bold:
        FcPatternAddInteger(pat, FC_WEIGHT, FC_WEIGHT_BOLD);
        FcPatternAddInteger(pat, FC_SLANT, FC_SLANT_ROMAN);
        break;
    case mx_italic:
        FcPatternAddInteger(pat, FC_WEIGHT, FC_WEIGHT_REGULAR);
        FcPatternAddInteger(pat, FC_SLANT, FC_SLANT_ITALIC);
        break;
    case mx_bold_italic:
        FcPatternAddInteger(pat, FC_WEIGHT, FC_WEIGHT_BOLD);
        FcPatternAddInteger(pat, FC_SLANT, FC_SLANT_ITALIC);
    }

    {
        FcConfigSubstitute(m_config, pat, FcMatchPattern);
        FcDefaultSubstitute(pat);

        FcResult result;
        FcPattern *match = FcFontMatch(m_config, pat, &result);

        if (match)
        {
            XftFont *xft_font = XftFontOpenPattern(dpy, match);

            if (xft_font) {
                m_font_cache[k] = xft_font;
                return xft_font;
            } 
            else {
                MX_ERROR_THROW(err, MX_XFT_FAILED);
            }
        }
        else
        {
            MX_ERROR_THROW(err, MX_XFT_FAILED);
        }
    }

abort:
    return nullptr;
}

float mx_font::get_resolution() const
{
    Screen *s = XtScreen(global_top_level);
    return (0.5 + ((float)s->width / ((float)s->mwidth / 25.4)));
}

bool mx_font_key_t::operator<(const mx_font_key_t &o) const
{
    if (m_family < o.m_family)
    {
        return true;
    }
    else if (m_family > o.m_family)
    {
        return false;
    }

    if (m_size < o.m_size)
    {
        return true;
    }
    else if (m_size > o.m_size)
    {
        return false;
    }

    if (m_style < o.m_style)
    {
        return true;
    }
    else if (m_style > o.m_style)
    {
        return false;
    }

    if (m_scale < o.m_scale)
    {
        return true;
    }
    else if (m_scale > o.m_scale)
    {
        return false;
    }

    return m_resolution < o.m_resolution;
}

std::shared_ptr<mx_font_family> mx_font::get_font_family() const
{
    return m_font_family;
}

int mx_font::get_xft_width(XftFont *f, Display *display, const char *s, int len)
{
    XGlyphInfo ext;
    XftTextExtentsUtf8(display, f, (const FcChar8*)s, len, &ext);
    return ext.width;
}
