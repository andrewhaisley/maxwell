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
 * MODULE/CLASS : mx_config
 *
 * AUTHOR : Andrew Haisley
 *
 *
 *
 * DESCRIPTION:
 *        reads and writes configuration parameters from a file
 *
 *
 *
 */
#include <mx.h>
#include <string>
#include <map>

#ifndef MX_CONFIG_H
#define MX_CONFIG_H

#define MX_PRINT_TO_FILE "Save to PDF file"

class mx_config {

public:
    mx_config(int& err, const std::string &file_name);
    ~mx_config();

    int get_int(int& err, const std::string &name);
    int get_default_int(int& err, const std::string &name, int def);
    float get_float(int& err, const std::string &name);

    std::string get_string(int& err, const std::string &name);
    std::string get_default_string(int& err, const std::string &name, const std::string &def);

    void set_int(int& err, const std::string &name, int i);
    void set_float(int& err, const std::string &name, float f);
    void set_string(int& err, const std::string &name, const std::string &value);

private:

    std::string m_file_name;
    std::map<std::string, std::string> m_values;

    void read_values();
    void write_values();
};

#endif
