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
 * MODULE/CLASS : mx_para_style
 *
 * AUTHOR : Andrew Haisley
 *
 *
 *
 * DESCRIPTION:
 *
 * Styles for paragraphs
 *
 *
 *
 *
 */
#include <mx.h>
#include <mx_db.h>

#include "mx_para_style.h"
#include <mx_font.h>

static int num_para_style_instances = 0;

mx_paragraph_style::mx_paragraph_style()
{
    strcpy(name, "");

    justification = mx_left_aligned;
    keep_lines_together = FALSE;
    keep_with_next = FALSE;
    space_before = 0.0;
    space_after = 0.0;
    line_spacing = 1.0f;
    is_heading = is_toc = FALSE;
    heading_level = 0;
    page_break_before = FALSE;
    next_style_name = NULL;

    num_para_style_instances++;
}

mx_paragraph_style::mx_paragraph_style(const mx_paragraph_style& para)
{
    default_char_style = para.default_char_style;
    default_border_style = para.default_border_style;
    ruler = para.ruler;
    next_style_name = NULL;

    set_name(para.name);
    keep_lines_together = para.keep_lines_together;
    keep_with_next = para.keep_with_next;
    space_before = para.space_before;
    space_after = para.space_after;
    line_spacing = para.line_spacing;
    is_heading = para.is_heading;
    is_toc = para.is_toc;
    heading_level = para.heading_level;
    page_break_before = para.page_break_before;
    justification = para.justification;
    set_next_style_name(para.next_style_name);
    num_para_style_instances++;
}

// Assignment operator
mx_paragraph_style& mx_paragraph_style::operator=(const mx_paragraph_style& para)
{
    if (this == &para) {
        return *this;
    }

    // assignment operators in the other classes should take care of this
    default_char_style = para.default_char_style;
    default_border_style = para.default_border_style;
    ruler = para.ruler;

    set_next_style_name(para.next_style_name);
    set_name(para.name);

    keep_lines_together = para.keep_lines_together;
    keep_with_next = para.keep_with_next;
    space_before = para.space_before;
    space_after = para.space_after;
    line_spacing = para.line_spacing;
    is_heading = para.is_heading;
    is_toc = para.is_toc;
    heading_level = para.heading_level;
    page_break_before = para.page_break_before;
    justification = para.justification;

    return *this;
}

mx_paragraph_style::~mx_paragraph_style()
{
    if (next_style_name != NULL) {
        delete[] next_style_name;
    }
    num_para_style_instances--;
}

void mx_paragraph_style::set_next_style_name(const char* name)
{
    if (next_style_name != NULL) {
        delete[] next_style_name;
        next_style_name = NULL;
    }

    if (name != NULL) {
        next_style_name = mx_string_copy(name);
    }
}

void mx_paragraph_style::set_char_style(const mx_char_style& st)
{
    default_char_style = st;
}

void mx_paragraph_style::set_border_style(const mx_border_style& st)
{
    default_border_style = st;
}

void mx_paragraph_style::set_ruler(const mx_ruler& rl)
{
    ruler = rl;
}

void mx_paragraph_style::set_name(const char* nm)
{
    mx_strncpy(name, nm, MX_PARA_STYLE_MAX_NAME);
    name[MX_PARA_STYLE_MAX_NAME - 1] = '\0';
}

uint32 mx_paragraph_style::get_serialised_size(int& err)
{
    uint32 result;

    err = MX_ERROR_OK;
    result = 4 * SLS_BOOL + 3 * SLS_FLOAT + SLS_INT8 + SLS_ENUM;

    result += default_char_style.get_serialised_size(err);
    MX_ERROR_CHECK(err);

    result += ruler.get_serialised_size(err);
    MX_ERROR_CHECK(err);

    result += default_border_style.get_serialised_size(err);
    MX_ERROR_CHECK(err);

    result += SLS_STRING(name);
    result += SLS_STRING(next_style_name);

    return result;
abort:
    return 0;
}

void mx_paragraph_style::serialise(int& err, unsigned char** buffer)
{
    err = MX_ERROR_OK;

    serialise_bool(keep_lines_together, buffer);
    serialise_bool(keep_with_next, buffer);
    serialise_bool(is_heading, buffer);
    serialise_bool(page_break_before, buffer);
    serialise_float(space_before, buffer);
    serialise_float(space_after, buffer);
    serialise_float(line_spacing, buffer);
    serialise_uint8((uint8)heading_level, buffer);
    serialise_enum(justification, buffer);

    default_char_style.serialise(err, buffer);
    MX_ERROR_CHECK(err);

    ruler.serialise(err, buffer);
    MX_ERROR_CHECK(err);

    default_border_style.serialise(err, buffer);
    MX_ERROR_CHECK(err);

    serialise_string(name, buffer);
    serialise_string(next_style_name, buffer);

    return;
abort:
    return;
}

void mx_paragraph_style::unserialise(int& err, unsigned char** buffer)
{
    err = MX_ERROR_OK;
    uint16 e;

    char temp_name[MX_PARA_STYLE_MAX_NAME];

    unserialise_bool(keep_lines_together, buffer);
    unserialise_bool(keep_with_next, buffer);
    unserialise_bool(is_heading, buffer);
    unserialise_bool(page_break_before, buffer);
    unserialise_float(space_before, buffer);
    unserialise_float(space_after, buffer);
    unserialise_float(line_spacing, buffer);
    unserialise_uint8(heading_level, buffer);
    unserialise_enum(e, buffer);
    justification = (mx_justification_t)e;

    default_char_style.unserialise(err, buffer);
    MX_ERROR_CHECK(err);

    ruler.unserialise(err, buffer);
    MX_ERROR_CHECK(err);

    default_border_style.unserialise(err, buffer);
    MX_ERROR_CHECK(err);

    unserialise_string(name, buffer);
    unserialise_string(temp_name, buffer);
    if (temp_name[0] != 0) {
        set_next_style_name(temp_name);
    }

    return;
abort:
    return;
}

void mx_paragraph_style::operator+=(const mx_paragraph_style_mod& m)
{
#define MX_PARA_STYLE_PE(xxx) \
    if (m.xxx##_mod) {        \
        xxx = m.xxx;          \
    }

    MX_PARA_STYLE_PE(justification)
    MX_PARA_STYLE_PE(keep_lines_together)
    MX_PARA_STYLE_PE(keep_with_next)
    MX_PARA_STYLE_PE(space_before)
    MX_PARA_STYLE_PE(space_after)
    MX_PARA_STYLE_PE(is_heading)
    MX_PARA_STYLE_PE(is_toc)
    MX_PARA_STYLE_PE(heading_level)
    MX_PARA_STYLE_PE(page_break_before)
    MX_PARA_STYLE_PE(line_spacing)
    MX_PARA_STYLE_PE(ruler)

    default_char_style += m.char_style_mod;
    default_border_style += m.border_style_mod;
}

bool operator==(const mx_paragraph_style& s1, const mx_paragraph_style& s2)
{
    if (&s1 == &s2) {
        return TRUE;
    }

    if (s1.is_heading != s2.is_heading) {
        return FALSE;
    }

    if (s1.is_heading && (s1.heading_level != s2.heading_level)) {
        return FALSE;
    }

    return ((s1.justification == s2.justification) && (s1.keep_lines_together == s2.keep_lines_together) && (s1.keep_with_next == s2.keep_with_next) && (s1.space_before == s2.space_before) && (s1.space_after == s2.space_after) && (s1.is_heading == s2.is_heading) && (s1.page_break_before == s2.page_break_before) && (s1.line_spacing == s2.line_spacing) && (s1.default_char_style == s2.default_char_style) && (s1.default_border_style == s2.default_border_style) && (s1.ruler == s2.ruler));
}

bool operator!=(const mx_paragraph_style& s1, const mx_paragraph_style& s2)
{
    return !(s1 == s2);
}

mx_paragraph_style_mod::mx_paragraph_style_mod()
{
    justification_mod = keep_lines_together_mod = keep_with_next_mod = space_before_mod = space_after_mod = is_heading_mod = is_toc_mod = heading_level_mod = page_break_before_mod = line_spacing_mod = ruler_mod = FALSE;
}

mx_paragraph_style_mod::~mx_paragraph_style_mod()
{
}

mx_paragraph_style_mod::mx_paragraph_style_mod(const mx_paragraph_style& s1, const mx_paragraph_style& s2)
    : char_style_mod(s1.default_char_style, s2.default_char_style)
    , border_style_mod(s1.default_border_style, s2.default_border_style)
{
    justification_mod = keep_lines_together_mod = keep_with_next_mod = space_before_mod = space_after_mod = is_heading_mod = is_toc_mod = heading_level_mod = page_break_before_mod = line_spacing_mod = ruler_mod = FALSE;

#define MX_PARA_STYLE_INIT_MOD(xxx) \
    if (s1.xxx != s2.xxx) {         \
        xxx##_mod = TRUE;           \
        xxx = s2.xxx;               \
    }

    MX_PARA_STYLE_INIT_MOD(justification);
    MX_PARA_STYLE_INIT_MOD(keep_lines_together);
    MX_PARA_STYLE_INIT_MOD(keep_with_next);
    MX_PARA_STYLE_INIT_MOD(space_before);
    MX_PARA_STYLE_INIT_MOD(space_after);
    MX_PARA_STYLE_INIT_MOD(is_heading);
    MX_PARA_STYLE_INIT_MOD(is_toc);
    MX_PARA_STYLE_INIT_MOD(heading_level);
    MX_PARA_STYLE_INIT_MOD(page_break_before);
    MX_PARA_STYLE_INIT_MOD(line_spacing);
    MX_PARA_STYLE_INIT_MOD(ruler);
}

mx_paragraph_style_mod::mx_paragraph_style_mod(const mx_paragraph_style_mod& s)
{
    justification_mod = s.justification_mod;
    justification = s.justification;
    keep_lines_together_mod = s.keep_lines_together_mod;
    keep_lines_together = s.keep_lines_together;
    keep_with_next_mod = s.keep_with_next_mod;
    keep_with_next = s.keep_with_next;
    space_before_mod = s.space_before_mod;
    space_before = s.space_before;
    space_after_mod = s.space_after_mod;
    space_after = s.space_after;
    is_heading_mod = s.is_heading_mod;
    is_heading = s.is_heading;
    is_toc_mod = s.is_toc_mod;
    is_toc = s.is_toc;
    heading_level_mod = s.heading_level_mod;
    heading_level = s.heading_level;
    page_break_before_mod = s.page_break_before_mod;
    page_break_before = s.page_break_before;
    line_spacing_mod = s.line_spacing_mod;
    line_spacing = s.line_spacing;
    ruler_mod = s.ruler_mod;
    ruler = s.ruler;
    char_style_mod = s.char_style_mod;
    border_style_mod = s.border_style_mod;
}

mx_paragraph_style_mod::mx_paragraph_style_mod(const mx_paragraph_style& ps)
    : ruler(ps.ruler)
    , char_style_mod(*((mx_paragraph_style&)ps).get_char_style())
    , border_style_mod(*((mx_paragraph_style&)ps).get_border_style())
{
    justification_mod = keep_lines_together_mod = keep_with_next_mod = space_before_mod = space_after_mod = is_heading_mod = is_toc_mod = heading_level_mod = page_break_before_mod = line_spacing_mod = ruler_mod = TRUE;

    justification = ps.justification;
    keep_lines_together = ps.keep_lines_together;
    keep_with_next = ps.keep_with_next;
    space_before = ps.space_before;
    space_after = ps.space_after;
    is_heading = ps.is_heading;
    is_toc = ps.is_toc;
    heading_level = ps.heading_level;
    page_break_before = ps.page_break_before;
    line_spacing = ps.line_spacing;
}

mx_paragraph_style_mod& mx_paragraph_style_mod::operator=(const mx_paragraph_style_mod& s)
{
    if (this == &s) {
        return *this;
    }

    justification_mod = s.justification_mod;
    justification = s.justification;
    keep_lines_together_mod = s.keep_lines_together_mod;
    keep_lines_together = s.keep_lines_together;
    keep_with_next_mod = s.keep_with_next_mod;
    keep_with_next = s.keep_with_next;
    space_before_mod = s.space_before_mod;
    space_before = s.space_before;
    space_after_mod = s.space_after_mod;
    space_after = s.space_after;
    is_heading_mod = s.is_heading_mod;
    is_heading = s.is_heading;
    is_toc_mod = s.is_toc_mod;
    is_toc = s.is_toc;
    heading_level_mod = s.heading_level_mod;
    heading_level = s.heading_level;
    page_break_before_mod = s.page_break_before_mod;
    page_break_before = s.page_break_before;
    line_spacing_mod = s.line_spacing_mod;
    line_spacing = s.line_spacing;
    ruler_mod = s.ruler_mod;
    ruler = s.ruler;
    char_style_mod = s.char_style_mod;
    border_style_mod = s.border_style_mod;
    return *this;
}

mx_paragraph_style_mod& mx_paragraph_style_mod::operator+=(const mx_paragraph_style_mod& o)
{
#define MX_PARA_STYLE_MOD_COMPONENT(xxx) \
    if (o.xxx##_mod) {                   \
        xxx##_mod = TRUE;                \
        xxx = o.xxx;                     \
    }

    MX_PARA_STYLE_MOD_COMPONENT(justification);
    MX_PARA_STYLE_MOD_COMPONENT(keep_lines_together);
    MX_PARA_STYLE_MOD_COMPONENT(keep_with_next);
    MX_PARA_STYLE_MOD_COMPONENT(space_before);
    MX_PARA_STYLE_MOD_COMPONENT(space_after);
    MX_PARA_STYLE_MOD_COMPONENT(is_heading);
    MX_PARA_STYLE_MOD_COMPONENT(is_toc);
    MX_PARA_STYLE_MOD_COMPONENT(heading_level);
    MX_PARA_STYLE_MOD_COMPONENT(page_break_before);
    MX_PARA_STYLE_MOD_COMPONENT(line_spacing);
    MX_PARA_STYLE_MOD_COMPONENT(ruler);

    char_style_mod += o.char_style_mod;
    border_style_mod += o.border_style_mod;
    return *this;
}

void mx_paragraph_style_mod::serialise(int& err, uint8** buffer)
{
    serialise_bool(justification_mod, buffer);
    if (justification_mod) {
        serialise_enum(justification, buffer);
    }

    serialise_bool(keep_lines_together_mod, buffer);
    if (keep_lines_together_mod) {
        serialise_bool(keep_lines_together, buffer);
    }

    serialise_bool(keep_with_next_mod, buffer);
    if (keep_with_next_mod) {
        serialise_bool(keep_with_next, buffer);
    }

    serialise_bool(space_before_mod, buffer);
    if (space_before_mod) {
        serialise_float(space_before, buffer);
    }

    serialise_bool(space_after_mod, buffer);
    if (space_after_mod) {
        serialise_float(space_after, buffer);
    }

    serialise_bool(is_heading_mod, buffer);
    if (is_heading_mod) {
        serialise_bool(is_heading, buffer);
    }

    serialise_bool(is_toc_mod, buffer);
    if (is_toc_mod) {
        serialise_bool(is_toc, buffer);
    }

    serialise_bool(heading_level_mod, buffer);
    if (heading_level_mod) {
        serialise_uint8(heading_level, buffer);
    }

    serialise_bool(page_break_before_mod, buffer);
    if (page_break_before_mod) {
        serialise_bool(page_break_before, buffer);
    }

    serialise_bool(line_spacing_mod, buffer);
    if (line_spacing_mod) {
        serialise_float(line_spacing, buffer);
    }

    serialise_bool(ruler_mod, buffer);
    if (ruler_mod) {
        ruler.serialise(err, buffer);
        MX_ERROR_CHECK(err);
    }

    char_style_mod.serialise(err, buffer);
    MX_ERROR_CHECK(err);

    border_style_mod.serialise(err, buffer);
    MX_ERROR_CHECK(err);
abort:;
}

void mx_paragraph_style_mod::unserialise(int& err, uint8** buffer)
{
    uint16 e;
    bool b;

    unserialise_bool(b, buffer);
    justification_mod = b;
    if (justification_mod) {
        unserialise_enum(e, buffer);
        justification = (mx_justification_t)e;
    }

    unserialise_bool(b, buffer);
    keep_lines_together_mod = b;
    if (keep_lines_together_mod) {
        unserialise_bool(b, buffer);
        keep_lines_together = b;
    }

    unserialise_bool(b, buffer);
    keep_with_next_mod = b;
    if (keep_with_next_mod) {
        unserialise_bool(b, buffer);
        keep_with_next = b;
    }

    unserialise_bool(b, buffer);
    space_before_mod = b;
    if (space_before_mod) {
        unserialise_float(space_before, buffer);
    }

    unserialise_bool(b, buffer);
    space_after_mod = b;
    if (space_after_mod) {
        unserialise_float(space_after, buffer);
    }

    unserialise_bool(b, buffer);
    is_heading_mod = b;
    if (is_heading_mod) {
        unserialise_bool(b, buffer);
        is_heading = b;
    }

    unserialise_bool(b, buffer);
    is_toc_mod = b;
    if (is_toc_mod) {
        unserialise_bool(b, buffer);
        is_toc = b;
    }

    unserialise_bool(b, buffer);
    heading_level_mod = b;
    if (heading_level_mod) {
        unserialise_uint8(heading_level, buffer);
    }

    unserialise_bool(b, buffer);
    page_break_before_mod = b;
    if (page_break_before_mod) {
        unserialise_bool(b, buffer);
        page_break_before = b;
    }

    unserialise_bool(b, buffer);
    line_spacing_mod = b;
    if (line_spacing_mod) {
        unserialise_float(line_spacing, buffer);
    }

    unserialise_bool(b, buffer);
    ruler_mod = b;
    if (ruler_mod) {
        ruler.unserialise(err, buffer);
        MX_ERROR_CHECK(err);
    }

    char_style_mod.unserialise(err, buffer);
    MX_ERROR_CHECK(err);

    border_style_mod.unserialise(err, buffer);
    MX_ERROR_CHECK(err);
abort:;
}

uint32 mx_paragraph_style_mod::get_serialised_size(int& err)
{
    uint32 res = SLS_BOOL * 11;

    if (justification_mod) {
        res += SLS_ENUM;
    }

    if (keep_lines_together_mod) {
        res += SLS_BOOL;
    }

    if (keep_with_next_mod) {
        res += SLS_BOOL;
    }

    if (space_before_mod) {
        res += SLS_FLOAT;
    }

    if (space_after_mod) {
        res += SLS_FLOAT;
    }

    if (is_heading_mod) {
        res += SLS_BOOL;
    }

    if (is_toc_mod) {
        res += SLS_BOOL;
    }

    if (heading_level_mod) {
        res += SLS_UINT8;
    }

    if (page_break_before_mod) {
        res += SLS_BOOL;
    }

    if (line_spacing_mod) {
        res += SLS_FLOAT;
    }

    if (ruler_mod) {
        res += ruler.get_serialised_size(err);
        MX_ERROR_CHECK(err);
    }

    res += char_style_mod.get_serialised_size(err);
    MX_ERROR_CHECK(err);

    res += border_style_mod.get_serialised_size(err);
    MX_ERROR_CHECK(err);

    return res;
abort:;
    return 0;
}

bool mx_paragraph_style_mod::modifies(const mx_paragraph_style& ps) const
{
    mx_paragraph_style new_ps = ps;
    new_ps += *this;
    return (new_ps != ps);
}

bool mx_paragraph_style_mod::is_null() const
{
    return (char_style_mod.is_null() && border_style_mod.is_null() && !(justification_mod || keep_lines_together_mod || keep_with_next_mod || space_before_mod || space_after_mod || is_heading_mod || is_toc_mod || heading_level_mod || page_break_before_mod || line_spacing_mod || ruler_mod));
}

void mx_paragraph_style_mod::clear_mods_in(const mx_paragraph_style_mod& o)
{
    char_style_mod.clear_mods_in(o.char_style_mod);
    border_style_mod.clear_mods_in(o.border_style_mod);
    if (o.justification_mod)
        clear_justification_mod();
    if (o.keep_lines_together_mod)
        clear_keep_lines_together_mod();
    if (o.keep_with_next_mod)
        clear_keep_with_next_mod();
    if (o.space_before_mod)
        clear_space_before_mod();
    if (o.space_after_mod)
        clear_space_after_mod();
    if (o.is_heading_mod)
        clear_is_heading_mod();
    if (o.is_toc_mod)
        clear_is_toc_mod();
    if (o.heading_level_mod)
        clear_heading_level_mod();
    if (o.page_break_before_mod)
        clear_page_break_before_mod();
    if (o.line_spacing_mod)
        clear_line_spacing_mod();
    if (o.ruler_mod)
        clear_ruler_mod();
}

mx_char_style_mod* mx_paragraph_style_mod::get_char_style_mod() const
{
    return (mx_char_style_mod*)&char_style_mod;
}

mx_border_style_mod* mx_paragraph_style_mod::get_border_style_mod() const
{
    return (mx_border_style_mod*)&border_style_mod;
}

void mx_paragraph_style_mod::set_char_style_mod(const mx_char_style_mod& csm)
{
    char_style_mod = csm;
}

void mx_paragraph_style_mod::set_border_style_mod(const mx_border_style_mod& bsm)
{
    border_style_mod = bsm;
}

bool mx_paragraph_style_mod::get_justification_mod(mx_justification_t& new_just) const
{
    if (justification_mod)
        new_just = justification;
    return justification_mod;
}

bool mx_paragraph_style_mod::get_keep_lines_together_mod(bool& new_klt) const
{
    if (keep_lines_together_mod)
        new_klt = keep_lines_together;
    return keep_lines_together_mod;
}

bool mx_paragraph_style_mod::get_keep_with_next_mod(bool& new_kwn) const
{
    if (keep_with_next_mod)
        new_kwn = keep_with_next;
    return keep_with_next_mod;
}

bool mx_paragraph_style_mod::get_space_before_mod(float& new_sb) const
{
    if (space_before_mod)
        new_sb = space_before;
    return space_before_mod;
}

bool mx_paragraph_style_mod::get_space_after_mod(float& new_sa) const
{
    if (space_after_mod)
        new_sa = space_after;
    return space_after_mod;
}

bool mx_paragraph_style_mod::get_is_heading_mod(bool& new_ih) const
{
    if (is_heading_mod)
        new_ih = is_heading;
    return is_heading_mod;
}

bool mx_paragraph_style_mod::get_is_toc_mod(bool& new_it) const
{
    if (is_toc_mod)
        new_it = is_toc;
    return is_toc_mod;
}

bool mx_paragraph_style_mod::get_heading_level_mod(uint8& new_hl) const
{
    if (heading_level_mod)
        new_hl = heading_level;
    return heading_level_mod;
}

bool mx_paragraph_style_mod::get_page_break_before_mod(bool& new_pbb) const
{
    if (page_break_before_mod)
        new_pbb = page_break_before;
    return page_break_before_mod;
}

bool mx_paragraph_style_mod::get_line_spacing_mod(float& new_ls) const
{
    if (line_spacing_mod)
        new_ls = line_spacing;
    return line_spacing_mod;
}

bool mx_paragraph_style_mod::get_ruler_mod(mx_ruler& new_ruler) const
{
    if (ruler_mod)
        new_ruler = ruler;
    return ruler_mod;
}

void mx_paragraph_style_mod::set_justification_mod(mx_justification_t new_just)
{
    justification_mod = TRUE;
    justification = new_just;
}

void mx_paragraph_style_mod::set_keep_lines_together_mod(bool new_klt)
{
    keep_lines_together_mod = TRUE;
    keep_lines_together = new_klt;
}

void mx_paragraph_style_mod::set_keep_with_next_mod(bool new_kwn)
{
    keep_with_next_mod = TRUE;
    keep_with_next = new_kwn;
}

void mx_paragraph_style_mod::set_space_before_mod(float new_sb)
{
    space_before_mod = TRUE;
    space_before = new_sb;
}

void mx_paragraph_style_mod::set_space_after_mod(float new_sa)
{
    space_after_mod = TRUE;
    space_after = new_sa;
}

void mx_paragraph_style_mod::set_is_heading_mod(bool new_ih)
{
    is_heading_mod = TRUE;
    is_heading = new_ih;
}

void mx_paragraph_style_mod::set_is_toc_mod(bool new_it)
{
    is_toc_mod = TRUE;
    is_toc = new_it;
}

void mx_paragraph_style_mod::set_heading_level_mod(uint8 new_hl)
{
    heading_level_mod = TRUE;
    heading_level = new_hl;
}

void mx_paragraph_style_mod::set_page_break_before_mod(bool new_pbb)
{
    page_break_before_mod = TRUE;
    page_break_before = new_pbb;
}

void mx_paragraph_style_mod::set_line_spacing_mod(float new_ls)
{
    line_spacing_mod = TRUE;
    line_spacing = new_ls;
}

void mx_paragraph_style_mod::set_ruler_mod(const mx_ruler& new_ruler)
{
    ruler_mod = TRUE;
    ruler = new_ruler;
}

void mx_paragraph_style_mod::clear_justification_mod()
{
    justification_mod = FALSE;
    justification = mx_left_aligned;
}

void mx_paragraph_style_mod::clear_keep_lines_together_mod()
{
    keep_lines_together_mod = keep_lines_together = FALSE;
}

void mx_paragraph_style_mod::clear_keep_with_next_mod()
{
    keep_with_next_mod = keep_with_next = FALSE;
}

void mx_paragraph_style_mod::clear_space_before_mod()
{
    space_before_mod = FALSE;
    space_before = 0.0f;
}

void mx_paragraph_style_mod::clear_space_after_mod()
{
    space_after_mod = FALSE;
    space_after = 0.0f;
}

void mx_paragraph_style_mod::clear_is_heading_mod()
{
    is_heading_mod = is_heading = FALSE;
}

void mx_paragraph_style_mod::clear_is_toc_mod()
{
    is_toc_mod = is_toc = FALSE;
}

void mx_paragraph_style_mod::clear_heading_level_mod()
{
    heading_level_mod = FALSE;
    heading_level = 0;
}

void mx_paragraph_style_mod::clear_page_break_before_mod()
{
    page_break_before_mod = page_break_before = FALSE;
}

void mx_paragraph_style_mod::clear_line_spacing_mod()
{
    line_spacing_mod = FALSE;
    line_spacing = 0.0f;
}

void mx_paragraph_style_mod::clear_ruler_mod()
{
    ruler_mod = FALSE;
    ruler = mx_ruler();
}
