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
 * MODULE : mx_rtfe_lu.C
 *
 * AUTHOR : Tom Newton
 *
 * This file
 *
 * DESCRIPTION: a load of lookup table classes for outputting maxwell objects
 * into an rtf file
 *
 *
 *
 *
 */

#include "mx_rtfe_conv.h"
#include "mx_rtfe_lu.h"
#include <mx_colour.h>
#include <mx_mod_iter.h>
#include <mx_para_pos.h>
#include <mx_paragraph.h>
#include <mx_table_area.h>
#include <mx_text_area.h>
#include <mx_wp_doc.h>
#include <mx_wp_pos.h>

mx_rtfe_lookups::mx_rtfe_lookups()
{
    c = new mx_rtfe_colour_lookup;
    f = new mx_rtfe_font_lookup;
    s = new mx_rtfe_style_lookup;
}

mx_rtfe_lookups::~mx_rtfe_lookups()
{
    delete c;
    delete f;
    delete s;
}

// disable copy constructor
// mx_rtfe_lookups::mx_rtfe_lookups(const mx_rtfe_lookups &l);

void mx_rtfe_lookups::initialise_lookups(int& err, mx_wp_document* doc)
{
    f->add_standard_fonts(err);
    MX_ERROR_CHECK(err);

    c->add_all_colours(err, doc);
    MX_ERROR_CHECK(err);

    s->add_all_styles(err, this, doc);
    MX_ERROR_CHECK(err);
abort:;
}

mx_rtfe_colour_lookup::mx_rtfe_colour_lookup()
{
    num_colours = 0;
}

mx_rtfe_colour_lookup::~mx_rtfe_colour_lookup()
{
}

static void add_char_style_mod_colours(int& err, const mx_char_style_mod* mod,
    mx_rtfe_colour_lookup* cl)
{
    mx_colour c;
    if (((mx_char_style_mod*)mod)->get_colour_mod(c)) {
        cl->add_colour(err, c);
        MX_ERROR_CHECK(err);
    }
abort:;
}

static void add_para_style_colours(int& err, mx_paragraph_style* ps,
    mx_rtfe_colour_lookup* cl)
{
    mx_border_style* bs = ps->get_border_style();

    cl->add_colour(err, ps->get_char_style()->colour);
    MX_ERROR_CHECK(err);

    if (bs->fill_style.type == mx_fill_colour) {
        cl->add_colour(err, bs->fill_style.colour);
        MX_ERROR_CHECK(err);
    }

    if (bs->top_style.line_type != mx_no_line) {
        cl->add_colour(err, bs->top_style.colour);
        MX_ERROR_CHECK(err);
    }

    if (bs->bottom_style.line_type != mx_no_line) {
        cl->add_colour(err, bs->bottom_style.colour);
        MX_ERROR_CHECK(err);
    }

    if (bs->left_style.line_type != mx_no_line) {
        cl->add_colour(err, bs->left_style.colour);
        MX_ERROR_CHECK(err);
    }

    if (bs->right_style.line_type != mx_no_line) {
        cl->add_colour(err, bs->right_style.colour);
        MX_ERROR_CHECK(err);
    }
abort:;
}

static void add_paragraph_colours(int& err, mx_paragraph* para,
    mx_rtfe_colour_lookup* cl)
{
    mx_para_pos start, end;

    add_para_style_colours(err, para->get_paragraph_style(), cl);
    MX_ERROR_CHECK(err);

    start.moveto_start();
    end.moveto_end(err, para);
    MX_ERROR_CHECK(err);

    {
        mx_style_mod_iterator it(para, start, end);

        while (it.more()) {
            mx_char_style_mod* m = it.data();
            add_char_style_mod_colours(err, m, cl);
            MX_ERROR_CHECK(err);
        }
    }
abort:;
}

static void add_text_colours(int& err, mx_text_area* txta,
    mx_rtfe_colour_lookup* cl)
{
    uint32 p, num_paras;
    mx_paragraph* para;

    add_para_style_colours(err, txta->get_paragraph_style(), cl);
    MX_ERROR_CHECK(err);

    num_paras = txta->num_paragraphs(err);
    MX_ERROR_CHECK(err);

    for (p = 0; p < num_paras; p++) {
        para = txta->get(err, p);
        MX_ERROR_CHECK(err);

        add_paragraph_colours(err, para, cl);
        MX_ERROR_CHECK(err);
    }

abort:;
}

static void add_table_colours(int& err, mx_table_area* table,
    mx_rtfe_colour_lookup* cl)
{
    uint32 row, col, num_rows, num_columns;
    mx_text_area* txta;

    num_rows = table->get_num_rows(err);
    MX_ERROR_CHECK(err);

    num_columns = table->get_num_columns(err);
    MX_ERROR_CHECK(err);

    for (row = 0; row < num_rows; row++) {
        for (col = 0; col < num_columns; col++) {
            txta = table->get_cell(err, row, col);
            MX_ERROR_CHECK(err);

            add_text_colours(err, txta, cl);
            MX_ERROR_CHECK(err);
        }
    }
abort:;
}

void mx_rtfe_colour_lookup::add_all_colours(int& err, mx_wp_document* doc)
{
    mx_wp_doc_pos pos;

    add_standard_colours(err);
    MX_ERROR_CHECK(err);

    pos.moveto_start(err, doc);
    MX_ERROR_CHECK(err);

    while (err == MX_ERROR_OK) {
        mx_area* a = pos.get_area(err, doc);
        MX_ERROR_CHECK(err);

        if (a->is_a(mx_table_area_class_e)) {
            add_table_colours(err, (mx_table_area*)a, this);
        } else {
            MX_ERROR_ASSERT(err, a->is_a(mx_text_area_class_e));
            add_text_colours(err, (mx_text_area*)a, this);
        }
        MX_ERROR_CHECK(err);

        pos.next_area(err, doc);
    }

    if (err == MX_POSITION_RANGE_ERROR) {
        MX_ERROR_CLEAR(err);
    } else {
        MX_ERROR_CHECK(err);
    }

abort:;
}

void mx_rtfe_colour_lookup::add_standard_colours(int& err)
{
    for (int i = 0; i < MX_NUM_COLOURS; i++) {
        add_colour(err, mx_colour(mx_colour_values[i].r, mx_colour_values[i].g, mx_colour_values[i].b, mx_colour_names[i]));
        MX_ERROR_CHECK(err);
    }
abort:;
}

void mx_rtfe_colour_lookup::add_colour(int& err, const mx_colour& c)
{
    uint32_t colval = (((uint32)c.red << 16) | ((uint32)c.green << 8) | (uint32)c.blue);
    uint32_t colnum = num_colours;

    MX_ERROR_ASSERT(err, sizeof(uint32) <= sizeof(void*));

    colours_to_numbers[colval] = -colnum;
    if (err == MX_HASH_DUPLICATE) {
        MX_ERROR_CLEAR(err);
    } else {
        MX_ERROR_CHECK(err);

        numbers_to_colours[colnum] = colval;
        MX_ERROR_CHECK(err);

        if (c.name != NULL) {
            // add the colour name too
            names_to_numbers[c.name] = colnum;
        }
        num_colours++;
    }
abort:;
}

uint32 mx_rtfe_colour_lookup::get_colour_number(int& err, const mx_colour& c)
{
    uint64_t colnum = 0;

    if (c.name == NULL) {
        uint32 colval = (((uint32)c.red << 16) | ((uint32)c.green << 8) | (uint32)c.blue);

        colnum = colours_to_numbers[colval];
    } else {
        colnum = names_to_numbers[c.name];
    }
    return colnum;
}

void mx_rtfe_colour_lookup::get_colour(int& err, uint32 number,
    mx_colour& result)
{
    uint64_t colval = numbers_to_colours[number];

    result.name = NULL;
    result.blue = colval & 0x000000ff;
    colval >>= 8;
    result.green = colval & 0x000000ff;
    colval >>= 8;
    result.red = colval & 0x000000ff;
}

void mx_rtfe_colour_lookup::output_colour_table_rtf(int& err, FILE* file)
{
    mx_colour c;

    fprintf(file, "\n{\\colortbl ");
    for (uint32 i = 0; i < num_colours; i++) {
        get_colour(err, i, c);
        MX_ERROR_CHECK(err);

        fprintf(file, "\n\\red%d\\green%d\\blue%d;",
            (int)c.red, (int)c.green, (int)c.blue);
    }
    fprintf(file, "}");
abort:;
}

mx_rtfe_font_lookup::mx_rtfe_font_lookup()
{
    num_fonts = 0;
}

mx_rtfe_font_lookup::~mx_rtfe_font_lookup()
{
}

void mx_rtfe_font_lookup::add_standard_fonts(int& err)
{
    mx_font f;

    for (auto i : f.get_font_family_names()) {
        add_font(err, i.c_str());
        MX_ERROR_CHECK(err);
    }

abort:;
}

static const char* remove_foundry(const char* font_name)
{
    while (*font_name != '-' && *font_name != '\0')
        font_name++;
    if (*font_name == '-')
        font_name++;
    return font_name;
}

void mx_rtfe_font_lookup::add_font(int& err, const char* font_name)
{
    font_name = remove_foundry(font_name);

    if (fonts_to_numbers.find(font_name) == fonts_to_numbers.end()) {
        fonts_to_numbers[font_name] = num_fonts;
        numbers_to_fonts[num_fonts] = font_name;
        num_fonts++;
    }
}

const char* mx_rtfe_font_lookup::get_font_name(int& err, uint32 number)
{
    if (numbers_to_fonts.find(number) != numbers_to_fonts.end()) {
        return numbers_to_fonts[number].c_str();
    } else {
        return nullptr;
    }
}

uint32 mx_rtfe_font_lookup::get_font_number(int& err, const char* font_name)
{
    font_name = remove_foundry(font_name);

    if (fonts_to_numbers.find(font_name) != fonts_to_numbers.end()) {
        return fonts_to_numbers[font_name];
    } else {
        return 0;
    }
}

uint32 mx_rtfe_font_lookup::get_default_font_number(int& err)
{
    return get_font_number(err, mx_font::get_default_roman_font().c_str());
}

void mx_rtfe_font_lookup::output_font_table_rtf(int& err, FILE* file)
{
    const char* name;
    int32 deff, def_sym, i;

    deff = this->get_font_number(err, mx_font::get_default_roman_font().c_str());
    if (err != MX_ERROR_OK) {
        MX_ERROR_CLEAR(err);
        deff = -1;
    } else {
        MX_ERROR_CHECK(err);
    }

    def_sym = this->get_font_number(err, mx_font::get_default_symbol_font().c_str());
    if (err != MX_ERROR_OK) {
        MX_ERROR_CLEAR(err);
        def_sym = -1;
    } else {
        MX_ERROR_CHECK(err);
    }

    fprintf(file, "\n{\\fonttbl");
    for (i = 0; i < (int32)num_fonts; i++) {
        name = this->get_font_name(err, i);
        MX_ERROR_CHECK(err);

        if (i == deff) {
            fprintf(file, "\n{\\f%d\\froman %s;}", i, name);
        } else if (i == def_sym) {
            fprintf(file, "\n{\\f%d\\ftech %s;}", i, name);
        } else {
            fprintf(file, "\n{\\f%d\\fnil %s;}", i, name);
        }
    }
    fprintf(file, "}");
abort:;
}

mx_rtfe_style_lookup::mx_rtfe_style_lookup()
{
    num_styles = 0;
}

mx_rtfe_style_lookup::~mx_rtfe_style_lookup()
{
}

void mx_rtfe_style_lookup::add_all_styles(int& err, mx_rtfe_lookups* lookups,
    mx_wp_document* doc)
{
    uint32 i, n;
    mx_paragraph_style *def_ps, *ps;

    def_ps = doc->get_default_style(err);
    MX_ERROR_CHECK(err);

    add_style(err, lookups, def_ps);
    MX_ERROR_CHECK(err);

    n = doc->get_num_styles(err);
    MX_ERROR_CHECK(err);

    for (i = 0; i < n; i++) {
        ps = doc->get_style(err, i);
        MX_ERROR_CHECK(err);

        if (ps != def_ps) {
            add_style(err, lookups, ps);
            MX_ERROR_CHECK(err);
        }
    }
abort:;
}

const char* mx_rtfe_style_lookup::get_para_style_string(int& err, const char* style_name)
{
    const char* str = NULL;
    uint32 num = get_para_style_num(err, style_name);
    MX_ERROR_CHECK(err);

    str = get_para_style_string(err, num);
    MX_ERROR_CHECK(err);
abort:
    return str;
}

const char* mx_rtfe_style_lookup::get_para_style_string(int& err, uint32 style_num)
{
    return numbers_to_style_strings[style_num].c_str();
}

uint32 mx_rtfe_style_lookup::get_para_style_num(int& err, const char* style_name)
{
    return names_to_numbers[style_name];
}

uint32 mx_rtfe_style_lookup::get_num_styles()
{
    return num_styles;
}

void mx_rtfe_style_lookup::output_style_table_rtf(int& err, FILE* file)
{
    uint32 i;
    const char* style_str;
    const char* style_name;

    fprintf(file, "\n{\\stylesheet ");

    for (i = 0; i < num_styles; i++) {
        style_name = numbers_to_names[i].c_str();
        MX_ERROR_CHECK(err);

        style_str = numbers_to_style_strings[i].c_str();
        MX_ERROR_CHECK(err);

        fprintf(file, "\n{%s%s;}", style_str, style_name);
    }
    fputc('}', file);

abort:;
}

void mx_rtfe_style_lookup::add_style(int& err, mx_rtfe_lookups* lookups,
    mx_paragraph_style* ps)
{
    char buf[4000];
    mx_rtfe_para_style_conv psc(lookups);

    psc.convert_from(err, *ps);
    MX_ERROR_CHECK(err);

    sprintf(buf, "\\s%d %s", (int)num_styles, psc.get_rtf_string());

    names_to_numbers[ps->get_name()] = num_styles;
    MX_ERROR_CHECK(err);

    numbers_to_names[num_styles] = ps->get_name();
    MX_ERROR_CHECK(err);

    numbers_to_style_strings[num_styles] = buf;
    MX_ERROR_CHECK(err);

    num_styles++;
abort:;
}
