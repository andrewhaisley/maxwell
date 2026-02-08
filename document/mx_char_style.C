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
 * MODULE/CLASS : mx_char_style
 *
 * AUTHOR : Andrew Haisley
 *
 *
 *
 * DESCRIPTION:
 *
 * Styles for characters
 *
 *
 *
 *
 */
#include <mx.h>
#include <mx_db.h>

#include <mx_char_style.h>
#include <mx_font.h>

static int num_char_style_instances = 0;

mx_char_style::mx_char_style(mx_font& new_font)
{
    font = new_font;

    set_width_adjust = 0;

    colour.red = MX_CHAR_STYLE_DEFAULT_COLOUR_VAL;
    colour.green = MX_CHAR_STYLE_DEFAULT_COLOUR_VAL;
    colour.blue = MX_CHAR_STYLE_DEFAULT_COLOUR_VAL;
    colour.name = mx_colour_names[0];

    effects = mx_no_effects;
    line_space = 0.0;
    alignment = mx_normal_align;
    alignment_offset = 0.0;

    num_char_style_instances++;
}

mx_char_style::mx_char_style(const mx_char_style& style)
{
    copy_it(style);
    num_char_style_instances++;
}

mx_char_style::~mx_char_style()
{
    num_char_style_instances--;
}

mx_char_style::mx_char_style()
{
    set_width_adjust = 0;

    colour.red = MX_CHAR_STYLE_DEFAULT_COLOUR_VAL;
    colour.green = MX_CHAR_STYLE_DEFAULT_COLOUR_VAL;
    colour.blue = MX_CHAR_STYLE_DEFAULT_COLOUR_VAL;
    colour.name = mx_colour_names[0];

    effects = mx_no_effects;
    line_space = 0.0;
    alignment = mx_normal_align;
    alignment_offset = 0.0;
    num_char_style_instances++;
}

void mx_char_style::copy_it(const mx_char_style& style)
{
    font = style.font;

    set_width_adjust = style.set_width_adjust;

    colour = style.colour;
    colour.name = style.colour.name;
    effects = style.effects;
    line_space = style.line_space;
    alignment = style.alignment;
    alignment_offset = style.alignment_offset;

    return;
}

void mx_char_style::set_font(mx_font& new_font)
{
    font = new_font;
}

uint32 mx_char_style::get_serialised_size(int& err)
{
    uint32 res;

    err = MX_ERROR_OK;

    res = font.get_serialised_size(err) + colour.get_serialised_size(err) + SLS_INT8 + 2 * SLS_FLOAT + 2 * SLS_ENUM;

    return res;
}

void mx_char_style::serialise(int& err, unsigned char** buffer)
{
    err = MX_ERROR_OK;

    serialise_int8(set_width_adjust, buffer);
    serialise_float(line_space, buffer);
    serialise_enum(effects, buffer);
    serialise_enum(alignment, buffer);
    serialise_float(alignment_offset, buffer);

    colour.serialise(err, buffer);
    MX_ERROR_CHECK(err);

    font.serialise(err, buffer);
    MX_ERROR_CHECK(err);

abort:
    return;
}

void mx_char_style::unserialise(int& err, unsigned char** buffer)
{
    err = MX_ERROR_OK;
    uint16 e;

    unserialise_int8(set_width_adjust, buffer);
    unserialise_float(line_space, buffer);
    unserialise_enum(e, buffer);
    effects = (mx_char_effects_t)e;
    unserialise_enum(e, buffer);
    alignment = (mx_align_t)e;
    unserialise_float(alignment_offset, buffer);

    colour.unserialise(err, buffer);
    MX_ERROR_CHECK(err);

    font.unserialise(err, buffer);
    MX_ERROR_CHECK(err);

abort:;
}

bool operator==(const mx_char_style& s1, const mx_char_style& s2)
{
    return (&s1 == &s2) || ((s1.set_width_adjust == s2.set_width_adjust) && (s1.effects == s2.effects) && (s1.alignment == s2.alignment) && MX_FLOAT_EQ(s1.alignment_offset, s2.alignment_offset) && MX_FLOAT_EQ(s1.line_space, s2.line_space) && (s1.colour == s2.colour) && (s1.font == s2.font));
}

bool operator!=(const mx_char_style& s1, const mx_char_style& s2)
{
    return ((s1.set_width_adjust != s2.set_width_adjust) || (s1.effects != s2.effects) || (s1.alignment != s2.alignment) || (!MX_FLOAT_EQ(s1.alignment_offset, s2.alignment_offset)) || !MX_FLOAT_EQ(s1.line_space, s2.line_space) || (s1.colour != s2.colour) || (s1.font != s2.font));
}

float mx_char_style::nwidth(const char* s, uint32 n) const
{
    float size = 0;
    float f = set_width_adjust;
    uint32 i = 0;

    while (i < n) {
        size += width(s[i++]);
    }

    size *= ((100.0 + f) / 100.0);

    return size;
}

mx_char_style& mx_char_style::operator=(const mx_char_style& o)
{
    copy_it(o);
    return *this;
}

void mx_char_style::operator+=(const mx_char_style_mod& m)
{
    if (m.set_width_adjust_mod) {
        set_width_adjust = m.set_width_adjust;
    }

    if (m.colour_mod) {
        colour = m.colour;
    }

    if (m.effects_mod) {
        effects = m.effects;
    }

    if (m.alignment_mod) {
        alignment = m.alignment;
    }

    if (m.alignment_offset_mod) {
        alignment_offset = m.alignment_offset;
    }

    if (m.line_space_mod) {
        line_space = m.line_space;
    }

    font += m.font_mod;
}

mx_char_style_mod::mx_char_style_mod()
{
    set_width_adjust_mod = colour_mod = effects_mod = alignment_offset_mod = alignment_mod = line_space_mod = FALSE;

    clear_revert_to_default_flags();
}

mx_char_style_mod::~mx_char_style_mod()
{
}

mx_char_style_mod::mx_char_style_mod(const mx_char_style& s1, const mx_char_style& s2)
{
    set_width_adjust_mod = colour_mod = effects_mod = alignment_offset_mod = alignment_mod = line_space_mod = FALSE;

    clear_revert_to_default_flags();

    if (s1.set_width_adjust != s2.set_width_adjust) {
        set_width_adjust_mod = TRUE;
        set_width_adjust = s2.set_width_adjust;
    }

    if (s1.colour != s2.colour) {
        colour_mod = TRUE;
        colour = s2.colour;
    }

    if (s1.effects != s2.effects) {
        effects_mod = TRUE;
        effects = s2.effects;
    }

    if (s1.alignment != s2.alignment) {
        alignment_mod = TRUE;
        alignment = s2.alignment;
    }

    if (!MX_FLOAT_EQ(s1.alignment_offset, s2.alignment_offset)) {
        alignment_offset_mod = TRUE;
        alignment_offset = s2.alignment_offset;
    }

    if (!MX_FLOAT_EQ(s1.line_space, s2.line_space)) {
        line_space_mod = TRUE;
        line_space = s2.line_space;
    }

    font_mod = mx_font_mod(s1.font, s2.font);
}

mx_char_style_mod::mx_char_style_mod(const mx_char_style_mod& o)
{
    set_width_adjust_mod = o.set_width_adjust_mod;
    colour_mod = o.colour_mod;
    effects_mod = o.effects_mod;
    alignment_mod = o.alignment_mod;
    alignment_offset_mod = o.alignment_offset_mod;
    line_space_mod = o.line_space_mod;

    set_width_adjust_mod_rev = o.set_width_adjust_mod_rev;
    colour_mod_rev = o.colour_mod_rev;
    effects_mod_rev = o.effects_mod_rev;
    alignment_mod_rev = o.alignment_mod_rev;
    alignment_offset_mod_rev = o.alignment_offset_mod_rev;
    line_space_mod_rev = o.line_space_mod_rev;

    set_width_adjust = o.set_width_adjust;
    colour = o.colour;
    effects = o.effects;
    alignment = o.alignment;
    alignment_offset = o.alignment_offset;
    line_space = o.line_space;

    font_mod = o.font_mod;
}

mx_char_style_mod::mx_char_style_mod(const mx_char_style& o)
    : font_mod(*((mx_char_style&)o).get_font())
{
    set_set_width_adjust_mod(o.set_width_adjust);
    set_colour_mod(o.colour);
    set_effects_mod(o.effects);
    set_alignment_mod(o.alignment);
    set_alignment_offset_mod(o.alignment_offset);
    set_line_space_mod(o.line_space);
}

mx_char_style_mod& mx_char_style_mod::operator=(const mx_char_style_mod& o)
{
    if (this == &o) {
        return *this;
    }

    set_width_adjust_mod = o.set_width_adjust_mod;
    colour_mod = o.colour_mod;
    effects_mod = o.effects_mod;
    alignment_mod = o.alignment_mod;
    alignment_offset_mod = o.alignment_offset_mod;
    line_space_mod = o.line_space_mod;

    set_width_adjust_mod_rev = o.set_width_adjust_mod_rev;
    colour_mod_rev = o.colour_mod_rev;
    effects_mod_rev = o.effects_mod_rev;
    alignment_mod_rev = o.alignment_mod_rev;
    alignment_offset_mod_rev = o.alignment_offset_mod_rev;
    line_space_mod_rev = o.line_space_mod_rev;

    set_width_adjust = o.set_width_adjust;
    colour = o.colour;
    effects = o.effects;
    alignment = o.alignment;
    alignment_offset = o.alignment_offset;
    line_space = o.line_space;

    font_mod = o.font_mod;
    return *this;
}

void mx_char_style_mod::operator+=(const mx_char_style_mod& o)
{
    if (o.set_width_adjust_mod) {
        set_width_adjust_mod = TRUE;
        set_width_adjust_mod_rev = o.set_width_adjust_mod_rev;
        set_width_adjust = o.set_width_adjust;
    }

    if (o.colour_mod) {
        colour_mod = TRUE;
        colour_mod_rev = o.colour_mod_rev;
        colour = o.colour;
    }

    if (o.effects_mod) {
        effects_mod = TRUE;
        effects_mod_rev = o.effects_mod_rev;
        effects = o.effects;
    }

    if (o.alignment_mod) {
        alignment_mod = TRUE;
        alignment_mod_rev = o.alignment_mod_rev;
        alignment = o.alignment;
    }

    if (o.alignment_offset_mod) {
        alignment_offset_mod = TRUE;
        alignment_offset_mod_rev = o.alignment_offset_mod_rev;
        alignment_offset = o.alignment_offset;
    }

    if (o.line_space_mod) {
        line_space_mod = TRUE;
        line_space_mod_rev = o.line_space_mod_rev;
        line_space = o.line_space;
    }

    font_mod += o.font_mod;
}

void mx_char_style_mod::serialise(int& err, uint8** buffer)
{
    serialise_bool(set_width_adjust_mod, buffer);
    if (set_width_adjust_mod) {
        serialise_int8(set_width_adjust, buffer);
    }

    serialise_bool(colour_mod, buffer);
    if (colour_mod) {
        colour.serialise(err, buffer);
        MX_ERROR_CHECK(err);
    }

    serialise_bool(effects_mod, buffer);
    if (effects_mod) {
        serialise_enum(effects, buffer);
    }

    serialise_bool(alignment_mod, buffer);
    if (alignment_mod) {
        serialise_enum(alignment, buffer);
    }

    serialise_bool(alignment_offset_mod, buffer);
    if (alignment_offset_mod) {
        serialise_float(alignment_offset, buffer);
    }

    serialise_bool(line_space_mod, buffer);
    if (line_space_mod) {
        serialise_float(line_space, buffer);
    }

    font_mod.serialise(err, buffer);
    MX_ERROR_CHECK(err);
abort:;
}

void mx_char_style_mod::unserialise(int& err, uint8** buffer)
{
    bool b;
    uint16 e;

    unserialise_bool(b, buffer);
    set_width_adjust_mod = b;
    if (set_width_adjust_mod) {
        unserialise_int8(set_width_adjust, buffer);
    }

    unserialise_bool(b, buffer);
    colour_mod = b;
    if (colour_mod) {
        colour.unserialise(err, buffer);
        MX_ERROR_CHECK(err);
    }

    unserialise_bool(b, buffer);
    effects_mod = b;
    if (effects_mod) {
        unserialise_enum(e, buffer);
        effects = (mx_char_effects_t)e;
    }

    unserialise_bool(b, buffer);
    alignment_mod = b;
    if (alignment_mod) {
        unserialise_enum(e, buffer);
        alignment = (mx_align_t)e;
    }

    unserialise_bool(b, buffer);
    alignment_offset_mod = b;
    if (alignment_offset_mod) {
        unserialise_float(alignment_offset, buffer);
    }

    unserialise_bool(b, buffer);
    line_space_mod = b;
    if (line_space_mod) {
        unserialise_float(line_space, buffer);
    }

    font_mod.unserialise(err, buffer);
    MX_ERROR_CHECK(err);
abort:;
}

uint32 mx_char_style_mod::get_serialised_size(int& err)
{
    int res = 6 * SLS_BOOL;

    if (set_width_adjust_mod) {
        res += SLS_INT8;
    }

    if (colour_mod) {
        res += colour.get_serialised_size(err);
        MX_ERROR_CHECK(err);
    }

    if (effects_mod) {
        res += SLS_ENUM;
    }

    if (alignment_mod) {
        res += SLS_ENUM;
    }

    if (alignment_offset_mod) {
        res += SLS_FLOAT;
    }

    if (line_space_mod) {
        res += SLS_FLOAT;
    }

    res += font_mod.get_serialised_size(err);
    MX_ERROR_CHECK(err);

    return res;
abort:;
    return 0;
}

bool mx_char_style_mod::is_null() const
{
    return !set_width_adjust_mod && !colour_mod && !effects_mod && !alignment_offset_mod && !alignment_mod && !line_space_mod && font_mod.is_null();
}

bool mx_char_style_mod::modifies(const mx_char_style& cs) const
{
    mx_char_style a = cs;
    a += *this;

    return (a != cs);
}

void mx_char_style_mod::set_revert_to_default_flags(const mx_char_style& cs)
{
    set_width_adjust_mod_rev = (set_width_adjust_mod && (set_width_adjust == cs.set_width_adjust));

    colour_mod_rev = (colour_mod && colour == cs.colour);

    effects_mod_rev = (effects_mod && effects == cs.effects);

    alignment_mod_rev = (alignment_mod && alignment == cs.alignment);

    alignment_offset_mod_rev = (alignment_offset_mod && alignment_offset == cs.alignment_offset);

    line_space_mod_rev = (line_space_mod && MX_FLOAT_EQ(line_space, cs.line_space));

    font_mod.set_revert_to_default_flags(cs.font);
}

void mx_char_style_mod::set_new_default_cs(const mx_char_style& cs)
{
    if (set_width_adjust_mod && set_width_adjust_mod_rev) {
        set_width_adjust = cs.set_width_adjust;
    }

    if (colour_mod && colour_mod_rev) {
        colour = cs.colour;
    }

    if (effects_mod && effects_mod_rev) {
        effects = cs.effects;
    }

    if (alignment_mod && alignment_mod_rev) {
        alignment = cs.alignment;
    }

    if (alignment_offset_mod && alignment_offset_mod_rev) {
        alignment_offset = cs.alignment_offset;
    }

    if (line_space_mod && line_space_mod_rev) {
        line_space = cs.line_space;
    }

    font_mod.set_new_default_cs(cs.font);
}

void mx_char_style_mod::clear_revert_to_default_flags()
{
    font_mod.clear_revert_to_default_flags();
    set_width_adjust_mod_rev = colour_mod_rev = effects_mod_rev = alignment_mod_rev = alignment_offset_mod_rev = line_space_mod_rev = FALSE;
}

void mx_char_style_mod::clear_mods_in(const mx_char_style_mod& o)
{
    font_mod.clear_mods_in(o.font_mod);
    if (o.set_width_adjust_mod)
        clear_set_width_adjust_mod();
    if (o.colour_mod)
        clear_colour_mod();
    if (o.effects_mod)
        clear_effects_mod();
    if (o.alignment_mod)
        clear_alignment_mod();
    if (o.alignment_offset_mod)
        clear_alignment_offset_mod();
    if (o.line_space_mod)
        clear_line_space_mod();
}

mx_font_mod* mx_char_style_mod::get_font_mod() const
{
    return (mx_font_mod*)&font_mod;
}

bool mx_char_style_mod::get_set_width_adjust_mod(int8& new_swa) const
{
    if (set_width_adjust_mod)
        new_swa = set_width_adjust;
    return set_width_adjust_mod;
}

bool mx_char_style_mod::get_colour_mod(mx_colour& new_colour) const
{
    if (colour_mod)
        new_colour = colour;
    return colour_mod;
}

bool mx_char_style_mod::get_effects_mod(mx_char_effects_t& new_effects) const
{
    if (effects_mod)
        new_effects = effects;
    return effects_mod;
}

bool mx_char_style_mod::get_alignment_mod(mx_align_t& new_alignment) const
{
    if (alignment_mod)
        new_alignment = alignment;
    return alignment_mod;
}

bool mx_char_style_mod::get_alignment_offset_mod(float& new_offset) const
{
    if (alignment_offset_mod)
        new_offset = alignment_offset;
    return alignment_offset_mod;
}

bool mx_char_style_mod::get_line_space_mod(float& new_line_space) const
{
    if (line_space_mod)
        new_line_space = line_space;
    return line_space_mod;
}

void mx_char_style_mod::set_set_width_adjust_mod(int8 new_swa)
{
    set_width_adjust_mod = TRUE;
    set_width_adjust = new_swa;
}

void mx_char_style_mod::set_colour_mod(const mx_colour& new_colour)
{
    colour_mod = TRUE;
    colour = new_colour;
}

void mx_char_style_mod::set_effects_mod(mx_char_effects_t new_effects)
{
    effects_mod = TRUE;
    effects = new_effects;
}

void mx_char_style_mod::set_alignment_mod(mx_align_t new_alignment)
{
    alignment_mod = TRUE;
    alignment = new_alignment;
}

void mx_char_style_mod::set_alignment_offset_mod(float new_offset)
{
    alignment_offset_mod = TRUE;
    alignment_offset = new_offset;
}

void mx_char_style_mod::set_line_space_mod(float new_line_space)
{
    line_space_mod = TRUE;
    line_space = new_line_space;
}

void mx_char_style_mod::clear_set_width_adjust_mod()
{
    set_width_adjust_mod = set_width_adjust_mod_rev = FALSE;
    set_width_adjust = 0;
}

void mx_char_style_mod::clear_colour_mod()
{
    colour_mod = colour_mod_rev = FALSE;
    colour = mx_colour();
}

void mx_char_style_mod::clear_effects_mod()
{
    effects_mod = effects_mod_rev = FALSE;
    effects = mx_no_effects;
}

void mx_char_style_mod::clear_alignment_mod()
{
    alignment_mod = alignment_mod_rev = FALSE;
    alignment = mx_normal_align;
}

void mx_char_style_mod::clear_alignment_offset_mod()
{
    alignment_offset_mod = alignment_offset_mod_rev = FALSE;
    alignment_offset = 0.0;
}

void mx_char_style_mod::clear_line_space_mod()
{
    line_space_mod = line_space_mod_rev = FALSE;
    line_space = 0.0f;
}

float mx_char_style::width(char c) const
{
    if (set_width_adjust) {
        return font.width(c) * ((100.0f + set_width_adjust) / 100.0f);
    } else {
        return font.width(c);
    }
}

float mx_char_style::width(const char* s) const
{
    if (set_width_adjust) {
        return font.width(s) * ((100.0f + set_width_adjust) / 100.0f);
    } else {
        return font.width(s);
    }
}

// ascender for characters in this style
float mx_char_style::get_ascender() const
{
    return font.get_ascender();
}

// descender for characters in this style
float mx_char_style::get_descender() const
{
    return font.get_descender();
}

