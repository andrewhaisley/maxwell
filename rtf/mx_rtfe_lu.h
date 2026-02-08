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
#ifndef MX_RTFE_LU_H
#define MX_RTFE_LU_H
/*
 * MODULE : mx_rtfe_lu.h
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

#include <map>
#include <string>

#include <mx_std.h>
#include <stdio.h>

// forward definitions
class mx_rtfe_colour_lookup;
class mx_rtfe_font_lookup;
class mx_rtfe_style_lookup;
class mx_wp_document;
class mx_colour;
class mx_paragraph_style;

class mx_rtfe_lookups {
public:
    mx_rtfe_lookups();
    mx_rtfe_lookups(const mx_rtfe_lookups& l);
    ~mx_rtfe_lookups();

    void initialise_lookups(int& err, mx_wp_document* doc);

    mx_rtfe_colour_lookup* colours() { return c; };
    mx_rtfe_font_lookup* fonts() { return f; };
    mx_rtfe_style_lookup* styles() { return s; };

private:
    mx_rtfe_colour_lookup* c;
    mx_rtfe_font_lookup* f;
    mx_rtfe_style_lookup* s;
};

class mx_rtfe_colour_lookup {
public:
    mx_rtfe_colour_lookup();
    ~mx_rtfe_colour_lookup();
    mx_rtfe_colour_lookup(const mx_rtfe_colour_lookup&);

    void add_all_colours(int& err, mx_wp_document* doc);

    void add_standard_colours(int& err);
    void add_colour(int& err, const mx_colour& c);

    uint32 get_colour_number(int& err, const mx_colour& c);
    void get_colour(int& err, uint32 number, mx_colour& result);

    void output_colour_table_rtf(int& err, FILE* file);

private:
    uint32 num_colours;

    std::map<uint32_t, uint32_t> colours_to_numbers;
    std::map<std::string, uint32_t> names_to_numbers;
    std::map<uint32_t, uint32_t> numbers_to_colours;
};

class mx_rtfe_font_lookup {
public:
    mx_rtfe_font_lookup();
    ~mx_rtfe_font_lookup();
    mx_rtfe_font_lookup(const mx_rtfe_font_lookup&);

    void add_standard_fonts(int& err);
    void add_font(int& err, const char* font_name);

    const char* get_font_name(int& err, uint32 number);
    uint32 get_font_number(int& err, const char* font_name);
    uint32 get_default_font_number(int& err);

    void output_font_table_rtf(int& err, FILE* file);

private:
    uint32 num_fonts;
    std::map<std::string, int> fonts_to_numbers;
    std::map<int, std::string> numbers_to_fonts;
};

class mx_rtfe_style_lookup {
public:
    mx_rtfe_style_lookup();
    ~mx_rtfe_style_lookup();

    void add_all_styles(int& err, mx_rtfe_lookups* lookups,
        mx_wp_document* doc);

    const char* get_para_style_string(int& err, const char* style_name);
    const char* get_para_style_string(int& err, uint32 style_num);

    uint32 get_para_style_num(int& err, const char* style_name);
    uint32 get_num_styles();

    void output_style_table_rtf(int& err, FILE* file);

private:
    void add_style(int& err, mx_rtfe_lookups* lookups,
        mx_paragraph_style* ps);

    uint32 num_styles;
    std::map<int, std::string> numbers_to_style_strings;
    std::map<std::string, int> names_to_numbers;
    std::map<int, std::string> numbers_to_names;
};

#endif // MX_RTFE_LU_H
