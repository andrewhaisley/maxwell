#pragma once

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

#include <map>
#include <string>
#include <vector>
#include <memory>

#include <mx.h>
#include <mx_font.h>
#include <mx_font_family.h>

class mx_font_metrics;

// This class is a storage mechanism for keeping track of font families. It is
// basically a wrapper for a map of font families, keyed by the family
// name.
class mx_font_metrics_store {
public:
    mx_font_metrics_store();
    ~mx_font_metrics_store();

    // gets the named font family. Returns nullptr if the named font family does
    // not exist
    std::shared_ptr<mx_font_family> get_font_family(const std::string &font_family_name);

    // gets a family whose named contains the given string (case insensitive)
    std::shared_ptr<mx_font_family> get_nearest_font_family(std::string name);

    // gets the metrics for a particular family and style
    mx_font_metrics* get_font_metrics(int& err, const std::string &font_family_name, mx_font_style_t style);

    // returns the number of font families stored here
    uint32 get_num_font_families();

    // gets a list of font family names
    std::vector<std::string> get_font_family_names();

    // gets the default roman font family name
    std::string get_default_roman_font_family_name() { return m_default_roman_font_family_name; }

    // gets the default sans serif font family name
    std::string get_default_symbol_font_family_name() { return m_default_symbol_font_family_name; }

private:
    void add_fm(mx_font_metrics* fm, const std::string &family_name);

    void init_xft_fm(int& err);

    void init_font_config(int& err);

    void load_everything(int &err);

    std::string get_default_from_name(int& err, const std::string &name);

    void set_default(int& err);

    mx_font_style_t translate_style(int weight, int slant);

    // a map of font_familys. Each family contains up to four pointers to
    // mx_font_metrics instances, according to the bold/italic options. The
    // pointers are nullptr if a particular bold/italic style does not exist.
    static std::map<std::string, std::shared_ptr<mx_font_family>> m_font_families;

    static std::string m_default_roman_font_family_name;
    static std::string m_default_symbol_font_family_name;

    static FT_Library m_ft;
};
