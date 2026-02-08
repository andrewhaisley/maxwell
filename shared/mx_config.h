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
#include <mx_list.h>
#include <stdio.h>

#ifndef MX_CONFIG_H
#define MX_CONFIG_H

#define MX_CONFIG_MAX_NAME_LENGTH 128
#define MX_CONFIG_MAX_VALUE_LENGTH 1024

class mx_config {

public:
    mx_config(int& err, const char* file_name);
    ~mx_config();

    int get_int(int& err, const char* name);
    int get_default_int(int& err, const char* name, int def);
    float get_float(int& err, const char* name);

    // returns a statically allocated thingy -> don't free
    const char* get_string(int& err, const char* name);

    // returns a statically allocated thingy -> don't free
    const char* get_default_string(int& err, const char* name, const char* def);

    // lists of things - in each case the pointer points to
    // a statically allocated area
    mx_list* get_int_list(int& err, const char* name);
    mx_list* get_float_list(int& err, const char* name);
    mx_list* get_string_list(int& err, const char* name);

    void set_int(int& err, const char* name, int i);
    void set_float(int& err, const char* name, float f);
    void set_string(int& err, const char* name, const char* s);
    void set_int_list(int& err, const char* name, mx_list* l);
    void set_float_list(int& err, const char* name, mx_list* l);
    void set_string_list(int& err, const char* name, mx_list* l);

private:
    FILE* file;
    void get_string_value(int& err, const char* name, char* buf);
    void get_line(char* name, char* value);

    char buffer[MX_CONFIG_MAX_VALUE_LENGTH];
    int int_buffer[100];
    int float_buffer[100];

    char file_name[MAX_PATH_LEN];
};

#endif
