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
#ifndef MX_RTFE_CONV_H
#define MX_RTFE_CONV_H
/*
 * MODULE : mx_rtfe_conv.h
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

#include <stdio.h>

// forward definitions
struct mx_rtfe_lookups;
class mx_char_style;
class mx_char_style_mod;
class mx_paragraph_style;
class mx_paragraph_style_mod;
class mx_border_style;
class mx_border_style_mod;

class mx_rtfe_style_conv {
public:
    mx_rtfe_style_conv(mx_rtfe_lookups* lu);
    virtual ~mx_rtfe_style_conv();

    void output_rtf(int& err, FILE* file);
    const char* get_rtf_string();

protected:
    void set_rtf_string(const char* str);
    mx_rtfe_lookups* get_lookups();

private:
    mx_rtfe_lookups* lookups;
    char* rtf_string;
};

class mx_rtfe_char_style_conv : public mx_rtfe_style_conv {
public:
    mx_rtfe_char_style_conv(mx_rtfe_lookups* lu);
    ~mx_rtfe_char_style_conv();

    // only produces the control words that differ from the RTF default
    void convert_from(int& err, const mx_char_style& cs);

    // produces all control words in the mod
    void convert_from(int& err, const mx_char_style_mod& csm);
};

class mx_rtfe_border_style_conv : public mx_rtfe_style_conv {
public:
    mx_rtfe_border_style_conv(mx_rtfe_lookups* lu);
    ~mx_rtfe_border_style_conv();

    // only produces the control words that differ from the RTF default
    void convert_from(int& err, const mx_border_style& bs);

    // produces all control words in the mod
    void convert_from(int& err, const mx_border_style_mod& bsm);

    virtual const char* get_top_style_rtf_string() const = 0;
    virtual const char* get_bottom_style_rtf_string() const = 0;
    virtual const char* get_left_style_rtf_string() const = 0;
    virtual const char* get_right_style_rtf_string() const = 0;
    virtual const char* get_shading_rtf_string() const = 0;
    virtual const char* get_bg_colour_rtf_string() const = 0;
};

class mx_rtfe_para_bs_conv : public mx_rtfe_border_style_conv {
public:
    mx_rtfe_para_bs_conv(mx_rtfe_lookups* lu);
    ~mx_rtfe_para_bs_conv();

    virtual const char* get_top_style_rtf_string() const;
    virtual const char* get_bottom_style_rtf_string() const;
    virtual const char* get_left_style_rtf_string() const;
    virtual const char* get_right_style_rtf_string() const;
    virtual const char* get_shading_rtf_string() const;
    virtual const char* get_bg_colour_rtf_string() const;
};

class mx_rtfe_table_bs_conv : public mx_rtfe_border_style_conv {
public:
    mx_rtfe_table_bs_conv(mx_rtfe_lookups* lu);
    ~mx_rtfe_table_bs_conv();

    virtual const char* get_top_style_rtf_string() const;
    virtual const char* get_bottom_style_rtf_string() const;
    virtual const char* get_left_style_rtf_string() const;
    virtual const char* get_right_style_rtf_string() const;
    virtual const char* get_shading_rtf_string() const;
    virtual const char* get_bg_colour_rtf_string() const;
};

class mx_rtfe_para_style_conv : public mx_rtfe_style_conv {
public:
    mx_rtfe_para_style_conv(mx_rtfe_lookups* lu);
    ~mx_rtfe_para_style_conv();

    // only produces the control words that differ from the RTF default
    void convert_from(int& err, const mx_paragraph_style& ps);

    // produces all control words in the mod
    void convert_from(int& err, const mx_paragraph_style_mod& psm);

    // use this for the paragraph style used in table cells to describe cell
    // borders
    void convert_table_cell_style_from(int& err, const mx_paragraph_style& ps);

private:
    void convert_table_cell_style_from(int& err, const mx_paragraph_style_mod& psm);
};

#endif // MX_RTFE_CONV_H
