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
 * MODULE/CLASS : mx_stdio
 *
 * AUTHOR : Andrew Haisley
 *
 *
 *
 *
 *
 */
#include <algorithm>
#include <climits>
#include <functional>
#include <iostream>
#include <random>
#include <sstream>
#include <string>
#include <vector>

#include <ctype.h>
#include <errno.h>
#include <mx.h>
#include <stdarg.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <unistd.h>

void mx_copy_file(int& err, char* in, char* out)
/*
 * PARAMS  :
 *   OUT   - err  what went wrong
 *   IN    - s       the file to copy from
 *   IN    - t       the file to copy to
 *
 * ACTION  : copies the file
 * RETURNS : none
 *
 *
 */
{
    static char s[1024];
    FILE *f1, *f2;
    uint32 i;

    f1 = fopen(in, "r");
    if (f1 == NULL) {
        err = mx_translate_file_error(errno);
        return;
    }

    f2 = fopen(out, "w");
    if (f2 == NULL) {
        err = mx_translate_file_error(errno);
        fclose(f1);
        return;
    }

    do {
        i = fread(s, 1, 1024, f1);
        if (fwrite(s, 1, i, f2) != i) {
            err = mx_translate_file_error(errno);
            fclose(f1);
            fclose(f2);
            return;
        }
    } while (i == 1024);
    fclose(f1);
    fclose(f2);
    err = MX_ERROR_OK;
    return;
}

int mx_translate_file_error(int e)
{
    switch (e) {
    case EEXIST:
        return MX_FILE_EEXIST;
    case EISDIR:
        return MX_FILE_EISDIR;
    case ETXTBSY:
        return MX_FILE_ETXTBSY;
    case EFAULT:
        return MX_FILE_EFAULT;
    case EACCES:
        return MX_FILE_EACCES;
    case ENAMETOOLONG:
        return MX_FILE_ENAMETOOLONG;
    case ENOENT:
        return MX_FILE_ENOENT;
    case ENOTDIR:
        return MX_FILE_ENOTDIR;
    case EMFILE:
        return MX_FILE_EMFILE;
    case ENFILE:
        return MX_FILE_ENFILE;
    case ENOMEM:
        return MX_FILE_ENOMEM;
    case EROFS:
        return MX_FILE_EROFS;
    case ELOOP:
        return MX_FILE_ELOOP;
    case ENOSPC:
        return MX_FILE_ENOSPC;
    default:
        return MX_FILE_UNKNOWN;
    }
}

uint32 mx_file_size(int& err, const char* file_name)
{
    struct stat info;

    if (stat(file_name, &info)) {
        err = mx_translate_file_error(errno);
        MX_ERROR_CHECK(err);
    }

    return info.st_size;
abort:
    return 0;
}

void mx_printf_warning(const char* format, ...)
{
    va_list ap;

    char real_format[300] = "WARNING: ";

    va_start(ap, format);
    strcat(real_format, format);
    strcat(real_format, "\n");

    vfprintf(stderr, real_format, ap);
}

FILE* mx_fopen(int& err, const char* file_name, const char* mode)
{
    FILE* res;

    res = fopen(file_name, mode);
    if (res == NULL) {
        MX_ERROR_THROW(err, mx_translate_file_error(errno));
    } else {
        return res;
    }

abort:
    return NULL;
}

int mx_fprintf(int& err, FILE* f, const char* format, ...)
{
    va_list ap;
    int res;

    va_start(ap, format);
    res = vfprintf(f, format, ap);
    if (errno == 0) {
        return res;
    } else {
        MX_ERROR_THROW(err, mx_translate_file_error(errno));
    }

abort:
    return -1;
}

unsigned char random_char()
{
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<> dis(0, 255);
    return static_cast<unsigned char>(dis(gen));
}

std::string generate_hex(int len)
{
    std::stringstream ss;
    for (int i = 0; i < len; i++) {
        auto rc = random_char();
        std::stringstream hexstream;
        hexstream << std::hex << int(rc);
        auto hex = hexstream.str();
        ss << (hex.length() < 2 ? '0' + hex : hex);
    }
    return ss.str();
}

static char tmpfilename[1000];

const char* mx_tmpnam(char* res)
{
    if (res == nullptr) {
        sprintf(tmpfilename, "/tmp/maxwell-%s", generate_hex(20).c_str());
        return tmpfilename;
    } else {
        sprintf(res, "/tmp/maxwell-%s", generate_hex(20).c_str());
        return res;
    }
}
