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
#ifndef MX_FONT_METRICS_H
#define MX_FONT_METRICS_H
/*
 * MODULE/CLASS : mx_font_metrics & mx_font_metrics_store
 *
 * AUTHOR : Tom Newton
 *
 *
 *
 * DESCRIPTION:
 *
 * The mx_font_metrics class represents the metrics for a given font. It is
 * used by mx_font to get its characters' widths etc. The mx_font_metrics_store
 * class is used as a nice interface to a static map table for a load of
 * instances of mx_font_metrics.
 *
 *
 *
 *
 */

#include <map>
#include <string>
#include <vector>
#include <memory>

#include <mx.h>
#include <mx_font.h>
#include <mx_font_family.h>

#define MX_FM_CHARSET_SIZE 65536

// This defines a class for storing the metrics information for a given
// font. There is functionality for reading postscript font metrics information
// and also reading/writing the maxwell font metrics files.
class mx_font_metrics {
public:
    mx_font_metrics();
    ~mx_font_metrics();

    std::string get_family_name() const;
    std::string get_file_name() const;
    mx_font_style_t get_style() const;
    int32 get_em_ascender() const;
    int32 get_em_descender() const;

    // This function gets called a lot by mx_font. It returns the wdith (in
    // thousands of an em-square) of a particular character, or zero if the
    // character does not exist for this font.
    uint32 get_em_width(uint16 c) const;

    // set methods to be used by mx_font_metrics_store only
    void set_family_name(const std::string &name);
    void set_file_name(const std::string &filename);
    void set_style(mx_font_style_t st);
    void set_em_ascender(int32 a);
    void set_em_descender(int32 d);
    void set_em_width(uint16 character, uint32 em_width);

    // write to a file
    void write_to_file(const std::string &filename);

    // read from a file
    void read_from_file(const std::string &filename);

private:

    std::string m_family_name;       // name of the font family
    std::string m_file_name;         // The fonts full pathname
    std::string m_metrics_file_name; // The font metrics file full pathname
    mx_font_style_t style;           // the style of the font

    mutable bool m_pending_file_read;

    // an array of character widths
    mutable uint32 widths_arr[MX_FM_CHARSET_SIZE];

    // max char index for which we have a width
    mutable uint16 m_max_char;

    int32 em_ascender;  // height of the ascender
    int32 em_descender; // height of the descender

    // private methods for accessing the map between iso numbers and type1
    // character names
    void build_iso_lookup();

    void aka_read_from_file() const;

    uint16 get_iso(int& err, const std::string &type1_name);
    std::string get_type1_name(int& err, uint16 iso_num);

    // private maps for the iso num to type 1 name map
    static std::map<int, std::string> iso_to_type1;
    static std::map<std::string, int> type1_to_iso;
};

#endif // MX_FONT_METRICS_H
