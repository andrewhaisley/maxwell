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
 * MODULE/CLASS : mx_font_metrics
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
 *
 */

#include <stdio.h>
#include <sys/stat.h>
#include <unistd.h>

#include <map>
#include <string>
#include <algorithm>
#include <cctype>
#include <iostream>
#include <fstream>

#include <mx_font_metrics.h>
#include <mx.h>
#include <mx_iso2ps.h>

using namespace std;

// global variable
extern char* global_maxhome;

extern char* global_maxhome;

std::map<int, std::string> mx_font_metrics::iso_to_type1;
std::map<std::string, int> mx_font_metrics::type1_to_iso;

mx_font_metrics::mx_font_metrics()
{
    style = mx_normal;
    em_ascender = 0;
    em_descender = 0;
    m_max_char = 0;
    m_pending_file_read = true;

    // using an array for character widths
    for (int i = 0; i < MX_FM_CHARSET_SIZE; i++) {
        widths_arr[i] = 0;
    }

    build_iso_lookup();
}

mx_font_metrics::~mx_font_metrics()
{
}

std::string mx_font_metrics::get_family_name() const
{
    return m_family_name;
}

mx_font_style_t mx_font_metrics::get_style() const
{
    return style;
}

std::string mx_font_metrics::get_file_name() const
{
    return m_file_name;
}

int32 mx_font_metrics::get_em_ascender() const
{
    return em_ascender;
}

int32 mx_font_metrics::get_em_descender() const
{
    return em_descender;
}

uint32 mx_font_metrics::get_em_width(uint16 c) const
{
    aka_read_from_file();
    if (c > 255) {
        return 0;
    }
    return widths_arr[c];
}

void mx_font_metrics::set_family_name(const std::string &new_name)
{
    m_family_name = new_name;
}

void mx_font_metrics::set_style(mx_font_style_t st)
{
    style = st;
}

void mx_font_metrics::set_file_name(const std::string &new_file_name)
{
    m_file_name = new_file_name;
}

void mx_font_metrics::set_em_ascender(int32 a)
{
    em_ascender = a;
}

void mx_font_metrics::set_em_descender(int32 d)
{
    em_descender = d;
}

void mx_font_metrics::set_em_width(uint16 character, uint32 em_width)
{
    int err = MX_ERROR_OK;

    if (character > (MX_FM_CHARSET_SIZE - 1)) {
        MX_ERROR_THROW(err, MX_FM_OUT_OF_RANGE);
    } else {
        widths_arr[character] = em_width;
    }

    if (character > m_max_char)
    {
        m_max_char = character;
    }

    return;
abort:
    global_error_trace->print();
    MX_ERROR_CLEAR(err);
    return;
}

void mx_font_metrics::build_iso_lookup()
{
    if (iso_to_type1.size() != 0) {
        return;
    }

    for (auto i : iso2ps_map)
    {
        iso_to_type1[i.first] = i.second;
        type1_to_iso[i.second] = i.first;
    }
}

uint16 mx_font_metrics::get_iso(int& err, const std::string &type1_name)
{
    if (type1_to_iso.find(type1_name) != type1_to_iso.end()) {
        return type1_to_iso[type1_name];
    } else {
        return type1_to_iso[type1_name];
    }
}

void mx_font_metrics::write_to_file(const std::string &filename)
{
    std::ofstream out_file(filename);

    out_file << m_family_name << std::endl;
    out_file << m_file_name << std::endl;
    out_file << em_ascender << std::endl;
    out_file << em_descender << std::endl;
    out_file << (int)style << std::endl;
    out_file << m_max_char << std::endl;

    for (int i = 0; i < m_max_char; i++)
    {
        out_file << widths_arr[i] << std::endl;
    }

    out_file.close();
}

void mx_font_metrics::aka_read_from_file() const
{
    if (!m_pending_file_read) {
        return;
    }
    
    std::ifstream in_file(m_metrics_file_name);

    std::string s;
    int x;
    std::getline(in_file, s);
    in_file >> s;
    in_file >> x;
    in_file >> x;
    in_file >> x;
    in_file >> m_max_char;

    for (int i = 0; i < m_max_char; i++)
    {
        in_file >> widths_arr[i];
    }

    in_file.close();

    m_pending_file_read = false;
}

void mx_font_metrics::read_from_file(const std::string &filename)
{
    m_metrics_file_name = filename;
    m_pending_file_read = true;
}
