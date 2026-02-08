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
 *
 */
#include <ctype.h>
#include <mx.h>
#include <mx_config.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

mx_config::mx_config(int& err, const char* file_name)
{
    strcpy(mx_config::file_name, file_name);

    file = fopen(file_name, "r");
    if (file == NULL) {
        err = MX_CONFIG_CANT_OPEN;
    } else {
        err = MX_ERROR_OK;
    }
}

mx_config::~mx_config()
{
    if (file)
        fclose(file);
}

int mx_config::get_int(int& err, const char* name)
{
    get_string_value(err, name, buffer);
    MX_ERROR_CHECK(err);

    return atoi(buffer);
abort:;
    return 0;
}

int mx_config::get_default_int(int& err, const char* name, int def)
{
    get_string_value(err, name, buffer);
    if (err == MX_CONFIG_NO_SUCH_NAME) {
        MX_ERROR_CLEAR(err);
        return def;
    }
    MX_ERROR_CHECK(err);

    return atoi(buffer);
abort:;
    return 0;
}

float mx_config::get_float(int& err, const char* name)
{
    get_string_value(err, name, buffer);
    MX_ERROR_CHECK(err);

    return atof(buffer);
abort:;
    return 0.0;
}

const char* mx_config::get_string(int& err, const char* name)
{
    get_string_value(err, name, buffer);
    MX_ERROR_CHECK(err);

    return buffer;
abort:;
    return NULL;
}

const char* mx_config::get_default_string(int& err, const char* name, const char* def)
{
    const char* res;

    res = get_string(err, name);
    if (err == MX_CONFIG_NO_SUCH_NAME) {
        MX_ERROR_CLEAR(err);
        return def;
    } else {
        return res;
    }
}

void mx_config::get_string_value(int& err, const char* name, char* buf)
{
    char sname[MX_CONFIG_MAX_NAME_LENGTH];
    char svalue[MX_CONFIG_MAX_NAME_LENGTH];

    fseek(file, 0, SEEK_SET);

    while (!feof(file)) {
        get_line(sname, svalue);
        if (strcmp(sname, name) == 0) {
            memcpy(buf, svalue, MX_CONFIG_MAX_NAME_LENGTH);
            err = MX_ERROR_OK;
            return;
        }
    }
    err = MX_CONFIG_NO_SUCH_NAME;
    return;
}

void mx_config::get_line(char* name, char* value)
{
    char c;
    int k, j, i = 0;
    static char line[MX_CONFIG_MAX_NAME_LENGTH + MX_CONFIG_MAX_VALUE_LENGTH];

    name[0] = value[0] = 0;

    while (TRUE) {
        c = fgetc(file);
        if (c == EOF || c == 10) {
            line[i] = 0;
            break;
        } else {
            line[i++] = c;
        }
    }

    if (line[0] == '#' || line[0] == 0) {
        return;
    }

    j = k = 0;
    while (j < i) {
        if (isspace(line[j]) || line[j] == ':') {
            name[j++] = 0;
            break;
        } else {
            name[j] = line[j];
            j++;
        }
    }
    while (j < i) {
        if (!isspace(line[j])) {
            break;
        }
        j++;
    }

    while (j < i) {
        if (line[j] == 0) {
            value[k] = 0;
            return;
        }
        value[k++] = line[j++];
    }
    value[k] = 0;
}

void mx_config::set_int(int& err, const char* name, int i)
{
    char buffer[20];

    sprintf(buffer, "%d", i);
    set_string(err, name, buffer);
}

void mx_config::set_float(int& err, const char* name, float f)
{
    char buffer[20];

    sprintf(buffer, "%f", f);
    set_string(err, name, buffer);
}

void mx_config::set_string(int& err, const char* name, const char* s)
{
    FILE* tmp;
    char tmp_name[MAX_PATH_LEN];

    char sname[MX_CONFIG_MAX_NAME_LENGTH];
    char svalue[MX_CONFIG_MAX_NAME_LENGTH];

    bool found = FALSE;

    (void)mx_tmpnam(tmp_name);
    tmp = fopen(tmp_name, "w");
    if (tmp == NULL) {
        err = mx_translate_file_error(errno);
        return;
    }

    fseek(file, 0, SEEK_SET);

    while (!feof(file)) {
        get_line(sname, svalue);
        if (sname[0] == 0) {
            break;
        }

        if (strcmp(sname, name) == 0) {
            found = TRUE;

            // write new value out
            fprintf(tmp, "%s:%s\n", name, s);
        } else {
            // write old value out
            fprintf(tmp, "%s:%s\n", sname, svalue);
        }
    }

    if (!found) {
        fprintf(tmp, "%s:%s\n", name, s);
    }

    fclose(tmp);
    fclose(file);

    mx_copy_file(err, tmp_name, file_name);
    file = fopen(file_name, "r");
    MX_ERROR_CHECK(err);

    if (file == NULL) {
        err = mx_translate_file_error(errno);
        return;
    }
    err = MX_ERROR_OK;
abort:;
}

mx_list* mx_config::get_int_list(int& err, const char* name)
{
    err = MX_ERROR_OK;
    return NULL;
}

mx_list* mx_config::get_float_list(int& err, const char* name)
{
    err = MX_ERROR_OK;
    return NULL;
}

mx_list* mx_config::get_string_list(int& err, const char* name)
{
    mx_list* res = NULL;
    int i = 0, j = 0;

    get_string_value(err, name, buffer);
    MX_ERROR_CHECK(err);

    res = new mx_list;

    while (buffer[j] != 0) {
        if (buffer[j] == ',') {
            buffer[j] = 0;
            res->insert(err, 0, buffer + i);
            MX_ERROR_CHECK(err);

            i = j + 1;
        }
        j++;
    }

    if (i != j) {
        res->insert(err, 0, buffer + i);
        MX_ERROR_CHECK(err);
    }
    return res;

abort:
    return NULL;
}

void mx_config::set_int_list(int& err, const char* name, mx_list* l)
{
    err = MX_ERROR_OK;
}

void mx_config::set_float_list(int& err, const char* name, mx_list* l)
{
    err = MX_ERROR_OK;
}

void mx_config::set_string_list(int& err, const char* name, mx_list* l)
{
    err = MX_ERROR_OK;
}
