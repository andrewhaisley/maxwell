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
 * MODULE : mx_rtfe_conv.C
 *
 * AUTHOR : Tom Newton
 *
 * This file
 *
 * DESCRIPTION:
 *
 * classes for converting maxwell styles and mods into rtf style control words
 *
 *
 *
 *
 */

#include "mx_rtfe_conv.h"
#include "mx_rtfe_lu.h"
#include <mx_bd_style.h>
#include <mx_char_style.h>
#include <mx_list_iter.h>
#include <mx_para_style.h>

static mx_paragraph_style* default_rtf_ps = NULL;
static mx_char_style* default_rtf_cs = NULL;
static mx_border_style* default_rtf_bs = NULL;

static void setup_default_rtf_styles()
{
    int err = MX_ERROR_OK;
    if (default_rtf_ps == NULL) {
        mx_paragraph_style* ps = default_rtf_ps = new mx_paragraph_style;
        mx_char_style* cs = default_rtf_cs = ps->get_char_style();
        (void)ps->get_border_style();

        cs->set_width_adjust = 0;
        cs->colour = mx_colour(0, 0, 0, "black");
        cs->effects = mx_no_effects;
        cs->alignment = mx_normal_align;

        // 6 half-points is default
        //        cs->alignment_offset = 3.0f;

        cs->line_space = 0.0f;

        // 24 half points is default
        cs->get_font()->set_size(12.0f);
        cs->get_font()->set_style(err, mx_normal);
        MX_ERROR_CHECK(err);

        cs->get_font()->set_family(err, mx_font::get_default_roman_font());
        MX_ERROR_CHECK(err);
    }
    return;
abort:
    global_error_trace->print();
    MX_ERROR_CLEAR(err);
}

mx_rtfe_style_conv::mx_rtfe_style_conv(mx_rtfe_lookups* lu)
    : lookups(lu)
    , rtf_string(NULL)
{
    setup_default_rtf_styles();
}

mx_rtfe_style_conv::~mx_rtfe_style_conv()
{
    if (rtf_string != NULL) {
        delete[] rtf_string;
    }
}

void mx_rtfe_style_conv::output_rtf(int& err, FILE* file)
{
    MX_ERROR_ASSERT(err, get_rtf_string() != NULL);
    fputs(get_rtf_string(), file);
abort:;
}

const char* mx_rtfe_style_conv::get_rtf_string()
{
    return rtf_string ? rtf_string : "";
}

void mx_rtfe_style_conv::set_rtf_string(const char* str)
{
    if (rtf_string != NULL) {
        delete[] rtf_string;
        rtf_string = NULL;
    }
    if (str != NULL)
        rtf_string = mx_string_copy(str);
}

mx_rtfe_lookups* mx_rtfe_style_conv::get_lookups()
{
    return lookups;
}

mx_rtfe_char_style_conv::mx_rtfe_char_style_conv(mx_rtfe_lookups* lu)
    : mx_rtfe_style_conv(lu)
{
}

mx_rtfe_char_style_conv::~mx_rtfe_char_style_conv() { }

void mx_rtfe_char_style_conv::convert_from(int& err, const mx_char_style& cs)
{
    convert_from(err, mx_char_style_mod(*default_rtf_cs, cs));
}

void mx_rtfe_char_style_conv::convert_from(int& err, const mx_char_style_mod& csm)
{
    char buf[2000] = "";
    char* strptr = buf;
    const mx_font_mod* fm = csm.get_font_mod();

    int8 new_swa;
    mx_colour new_colour;
    mx_char_effects_t new_effects;
    mx_align_t new_alignment;
    float new_offset;
    float new_line_space;
    std::string new_family;
    float new_size;
    bool bold_on, italic_on;

    if (csm.get_set_width_adjust_mod(new_swa)) {
        // do an expansion ratio against the default font if there is no font
        // mod
        float font_size = default_rtf_cs->get_font()->get_size();
        float new_set_width = (float)new_swa;
        fm->get_size_mod(font_size);

        int exp = (int)(0.5f + new_set_width * font_size * 20.0f / 100.0f);
        strptr += sprintf(strptr, "\\expnd%d\\expndtw%d ", (exp / 5), exp);
    }

    if (csm.get_colour_mod(new_colour)) {
        // output new foreground colour
        int colournum = get_lookups()->colours()->get_colour_number(err, new_colour);
        MX_ERROR_CHECK(err);
        strptr += sprintf(strptr, "\\cf%d ", colournum);
    }

    if (csm.get_effects_mod(new_effects)) {
        int underline_on = (new_effects == mx_underline);
        strptr += sprintf(strptr, "\\ul%d ", underline_on);
    }

    if (csm.get_alignment_mod(new_alignment)) {
        switch (new_alignment) {
        case mx_superscript:
            strptr += sprintf(strptr, "\\super ");
            break;
        case mx_subscript:
            strptr += sprintf(strptr, "\\sub ");
            break;
        default:
            strptr += sprintf(strptr, "\\nosupersub ");
            break;
        }
    }

    if (csm.get_alignment_offset_mod(new_offset)) {
        // rtf offsets are in half points
        int hp_offset = (int)(new_offset * 2.0f + 0.5f);
        strptr += sprintf(strptr, "\\up%d\\dn%d ", hp_offset, hp_offset);
    }

    if (csm.get_line_space_mod(new_line_space)) {
        // rtf has no line space character style
    }

    if (fm->get_family_mod(new_family)) {
        int font_num = get_lookups()->fonts()->get_font_number(err, new_family.c_str());
        MX_ERROR_CHECK(err);

        strptr += sprintf(strptr, "\\f%d ", font_num);
    }

    if (fm->get_size_mod(new_size)) {
        // rtf font size is in half points
        int fs = (int)(0.5f + new_size * 2.0f);
        strptr += sprintf(strptr, "\\fs%d ", fs);
    }

    if (fm->get_bold_mod(bold_on)) {
        const char* bold_str = bold_on ? "\\b " : "\\b0 ";
        strcat(strptr, bold_str);
        strptr += strlen(strptr);
    }

    if (fm->get_italic_mod(italic_on)) {
        const char* italic_str = italic_on ? "\\i " : "\\i0 ";
        strcat(strptr, italic_str);
        strptr += strlen(strptr);
    }

    set_rtf_string(buf);
abort:;
}

mx_rtfe_border_style_conv::mx_rtfe_border_style_conv(mx_rtfe_lookups* lu)
    : mx_rtfe_style_conv(lu)
{
}

mx_rtfe_border_style_conv::~mx_rtfe_border_style_conv() { }

void mx_rtfe_border_style_conv::convert_from(int& err,
    const mx_border_style& bs)
{
    convert_from(err, mx_border_style_mod(*default_rtf_bs, bs));
}

static void convert_line_style(int& err, const mx_line_style& ls,
    float distance_from_contents, char*& str,
    mx_rtfe_lookups* lu)
{
    const char* type_str;
    int colnum = lu->colours()->get_colour_number(err, ls.colour);
    MX_ERROR_CHECK(err);

    switch (ls.line_type) {
    case mx_dotted:
        type_str = "\\brdrdot ";
        break;
    case mx_dashed:
        type_str = "\\brdrdash ";
        break;
    case mx_solid:
        type_str = "\\brdrs ";
        break;
    default:
        return;
    }

    str += sprintf(str, "%s\\brdrw%d\\brdrcf%d ",
        type_str, (int)(0.5f + MX_MM_TO_TWIPS(ls.width)), colnum);

    if (distance_from_contents >= 0.0f) {
        str += sprintf(str, "\\brsp%d ",
            (int)(0.5f + MX_MM_TO_TWIPS(distance_from_contents)));
    }

abort:;
}

void mx_rtfe_border_style_conv::convert_from(int& err,
    const mx_border_style_mod& bsm)
{
    float new_dfc = -1.0f;
    mx_line_style ls;
    mx_fill_style fs;
    char buf[2000] = "";
    char* strptr = buf;

    bsm.get_distance_from_contents_mod(new_dfc);

    if (bsm.get_top_style_mod(ls)) {
        strptr += sprintf(strptr, "%s", get_top_style_rtf_string());
        convert_line_style(err, ls, new_dfc, strptr, get_lookups());
        MX_ERROR_CHECK(err);
    }

    if (bsm.get_bottom_style_mod(ls)) {
        strptr += sprintf(strptr, "%s", get_bottom_style_rtf_string());
        convert_line_style(err, ls, new_dfc, strptr, get_lookups());
        MX_ERROR_CHECK(err);
    }

    if (bsm.get_left_style_mod(ls)) {
        strptr += sprintf(strptr, "%s", get_left_style_rtf_string());
        convert_line_style(err, ls, new_dfc, strptr, get_lookups());
        MX_ERROR_CHECK(err);
    }

    if (bsm.get_right_style_mod(ls)) {
        strptr += sprintf(strptr, "%s", get_right_style_rtf_string());
        convert_line_style(err, ls, new_dfc, strptr, get_lookups());
        MX_ERROR_CHECK(err);
    }

    if (bsm.get_fill_style_mod(fs) && fs.type == mx_fill_colour) {
        int colnum = get_lookups()->colours()->get_colour_number(err, fs.colour);
        MX_ERROR_CHECK(err);

        strptr += sprintf(strptr, "%s%d%s10000 ", get_bg_colour_rtf_string(),
            colnum, get_shading_rtf_string());
    }

    set_rtf_string(buf);
abort:;
}

mx_rtfe_para_bs_conv::mx_rtfe_para_bs_conv(mx_rtfe_lookups* lu)
    : mx_rtfe_border_style_conv(lu)
{
}

mx_rtfe_para_bs_conv::~mx_rtfe_para_bs_conv() { }

const char* mx_rtfe_para_bs_conv::get_top_style_rtf_string() const
{
    return "\\brdrt";
}

const char* mx_rtfe_para_bs_conv::get_bottom_style_rtf_string() const
{
    return "\\brdrb";
}

const char* mx_rtfe_para_bs_conv::get_left_style_rtf_string() const
{
    return "\\brdrl";
}

const char* mx_rtfe_para_bs_conv::get_right_style_rtf_string() const
{
    return "\\brdrr";
}

const char* mx_rtfe_para_bs_conv::get_shading_rtf_string() const
{
    return "\\shading";
}

const char* mx_rtfe_para_bs_conv::get_bg_colour_rtf_string() const
{
    return "\\cfpat";
}

mx_rtfe_table_bs_conv::mx_rtfe_table_bs_conv(mx_rtfe_lookups* lu)
    : mx_rtfe_border_style_conv(lu)
{
}

mx_rtfe_table_bs_conv::~mx_rtfe_table_bs_conv() { }

const char* mx_rtfe_table_bs_conv::get_top_style_rtf_string() const
{
    return "\\clbrdrt";
}

const char* mx_rtfe_table_bs_conv::get_bottom_style_rtf_string() const
{
    return "\\clbrdrb";
}

const char* mx_rtfe_table_bs_conv::get_left_style_rtf_string() const
{
    return "\\clbrdrl";
}

const char* mx_rtfe_table_bs_conv::get_right_style_rtf_string() const
{
    return "\\clbrdrr";
}

const char* mx_rtfe_table_bs_conv::get_shading_rtf_string() const
{
    return "\\clshdng";
}

const char* mx_rtfe_table_bs_conv::get_bg_colour_rtf_string() const
{
    return "\\clcfpat";
}

mx_rtfe_para_style_conv::mx_rtfe_para_style_conv(mx_rtfe_lookups* lu)
    : mx_rtfe_style_conv(lu)
{
}

mx_rtfe_para_style_conv::~mx_rtfe_para_style_conv() { }

void mx_rtfe_para_style_conv::convert_from(int& err,
    const mx_paragraph_style& ps)
{
    convert_from(err, mx_paragraph_style_mod(*default_rtf_ps, ps));
}

static void convert_tab(mx_tabstop& t, char*& str)
{
    const char *type_str, *lead_type_str;

    switch (t.type) {
    case mx_right:
        type_str = "\\tqr ";
        break;
    case mx_centre:
        type_str = "\\tqc ";
        break;
    case mx_centre_on_dp:
        type_str = "\\tqdec ";
        break;
    default:
        type_str = "";
        break;
    }

    switch (t.leader_type) {
    case mx_leader_dot_e:
        lead_type_str = "\\tldot ";
        break;
    case mx_leader_hyphen_e:
        lead_type_str = "\\tlhyph ";
        break;
    case mx_leader_underline_e:
        lead_type_str = "\\tlul ";
        break;
    case mx_leader_thickline_e:
        lead_type_str = "\\tlth ";
        break;
    case mx_leader_equals_e:
        lead_type_str = "\\tleq ";
        break;
    default:
        lead_type_str = "";
        break;
    }

    str += sprintf(str, "%s%s\\tx%d ", type_str, lead_type_str,
        (int)(0.5f + MX_MM_TO_TWIPS(t.position)));
}

static void convert_ruler(mx_ruler& r, char*& str)
{
    str += sprintf(str, "\\fi%d\\li%d\\ri%d ",
        (int)(0.5f + MX_MM_TO_TWIPS(r.first_line_indent)),
        (int)(0.5f + MX_MM_TO_TWIPS(r.left_indent)),
        (int)(0.5f + MX_MM_TO_TWIPS(r.right_indent)));

    mx_list_iterator it(r.user_tabs);

    while (it.more()) {
        mx_tabstop* tab = (mx_tabstop*)it.data();
        convert_tab(*tab, str);
    }
}

void mx_rtfe_para_style_conv::convert_from(int& err,
    const mx_paragraph_style_mod& psm)
{
    char buf[4000] = "";
    char* strptr = buf;

    mx_rtfe_char_style_conv cs_conv(get_lookups());
    mx_rtfe_para_bs_conv bs_conv(get_lookups());

    mx_justification_t new_just;
    bool new_klt, new_kwn, new_ih, new_it, new_pbb;
    float new_sb, new_sa, new_ls;
    uint8 new_hl;
    mx_fill_style new_fs;
    mx_ruler new_ruler;

    // get the sub mods

    cs_conv.convert_from(err, *psm.get_char_style_mod());
    MX_ERROR_CHECK(err);

    bs_conv.convert_from(err, *psm.get_border_style_mod());
    MX_ERROR_CHECK(err);

    strptr += sprintf(strptr, "%s%s",
        cs_conv.get_rtf_string(),
        bs_conv.get_rtf_string());

    if (psm.get_justification_mod(new_just)) {
        switch (new_just) {
        case mx_right_aligned:
            strptr += sprintf(strptr, "\\qr ");
            break;
        case mx_justified:
            strptr += sprintf(strptr, "\\qj ");
            break;
        case mx_centred:
            strptr += sprintf(strptr, "\\qc ");
            break;
        default:
            strptr += sprintf(strptr, "\\ql ");
            break;
        }
    }

    if (psm.get_keep_lines_together_mod(new_klt)) {
        strptr += sprintf(strptr, new_klt ? "\\keep " : "\\keep0 ");
    }

    if (psm.get_keep_with_next_mod(new_kwn)) {
        strptr += sprintf(strptr, new_kwn ? "\\keepn " : "\\keepn0 ");
    }

    if (psm.get_space_before_mod(new_sb)) {
        strptr += sprintf(strptr, "\\sb%d ", (int)(0.5f + MX_MM_TO_TWIPS(new_sb)));
    }

    if (psm.get_space_after_mod(new_sa)) {
        strptr += sprintf(strptr, "\\sa%d ", (int)(0.5f + MX_MM_TO_TWIPS(new_sa)));
    }

    if (psm.get_is_heading_mod(new_ih)) {
    }

    if (psm.get_is_toc_mod(new_it)) {
    }

    if (psm.get_heading_level_mod(new_hl)) {
        strptr += sprintf(strptr, "\\level%d ", new_hl);
    }

    if (psm.get_page_break_before_mod(new_pbb)) {
        strptr += sprintf(strptr, new_pbb ? "\\pagebb " : "\\pagebb0 ");
    }

    if (psm.get_line_spacing_mod(new_ls)) {
        strptr += sprintf(strptr, "\\sl%d\\slmult ",
            (int)(0.5f + new_ls * 20.0f));
    }

    if (psm.get_ruler_mod(new_ruler)) {
        convert_ruler(new_ruler, strptr);
    }

    set_rtf_string(buf);
abort:;
}

void mx_rtfe_para_style_conv::convert_table_cell_style_from(
    int& err, const mx_paragraph_style& ps)
{
    convert_table_cell_style_from(
        err, mx_paragraph_style_mod(*default_rtf_ps, ps));
}

void mx_rtfe_para_style_conv::convert_table_cell_style_from(
    int& err, const mx_paragraph_style_mod& psm)
{
    mx_rtfe_table_bs_conv bs_conv(get_lookups());

    // just get the border style

    bs_conv.convert_from(err, *psm.get_border_style_mod());
    MX_ERROR_CHECK(err);

    set_rtf_string(bs_conv.get_rtf_string());
abort:;
}
