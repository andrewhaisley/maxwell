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
 * DESCRIPTION:
 *        reads and writes configuration parameters from a file
 *
 */

#include <fstream>
#include <mx_config.h>

using namespace std;


mx_config::mx_config(int& err, const string &file_name) : m_file_name(file_name)
{
    read_values();
}

mx_config::~mx_config()
{
}


int mx_config::get_int(int& err, const string &name)
{
    if (m_values.find(name) == m_values.end()) {
        throw runtime_error("Config value not found");
    } else {
        return stoi(m_values[name]);
    }
}

int mx_config::get_default_int(int& err, const string &name, int def)
{
    if (m_values.find(name) == m_values.end()) {
        return def;
    } else {
        return stoi(m_values[name]);
    }
}

float mx_config::get_float(int& err, const string &name)
{
    if (m_values.find(name) == m_values.end()) {
        throw runtime_error("Config value not found");
    } else {
        return stod(m_values[name]);
    }
}


string mx_config::get_string(int& err, const string &name)
{
    if (m_values.find(name) == m_values.end()) {
        throw runtime_error("Config value not found");
    } else {
        return m_values[name];
    }
}

string mx_config::get_default_string(int& err, const string &name, const string &def)
{
    if (m_values.find(name) == m_values.end()) {
        return def;
    } else {
        return m_values[name];
    }
}

void mx_config::set_int(int& err, const string &name, int i)
{
    m_values[name] = to_string(i);
    write_values();
}

void mx_config::set_float(int& err, const string &name, float f)
{
    m_values[name] = to_string(f);
    write_values();
}

void mx_config::set_string(int& err, const string &name, const string &value)
{
    m_values[name] = value;
    write_values();
}

void mx_config::read_values()
{
    ifstream file(m_file_name);

    if (!file)
    {
        throw runtime_error("Failed to open config file");
    }

    string line;

    while (getline(file, line))
    {
        if (line.empty())
        continue;

        size_t p = line.find(':');

        if (p == string::npos)
            continue;

        string name  = line.substr(0, p);
        string value = line.substr(p+ 1);

        m_values[name] = value;
    }
}

void mx_config::write_values()
{
    ofstream file(m_file_name);

    if (!file)
    {
        throw runtime_error("Failed to open config file for writing");
    }

    for (const auto &[name, value] : m_values)
    {
        file << name << ":" << value << '\n';
    }

    if (!file)
    {
        throw runtime_error("Error writing config file");
    }
}
