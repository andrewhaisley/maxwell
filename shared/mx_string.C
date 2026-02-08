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
 * MODULE/CLASS : mx_string
 *
 * AUTHOR : Andrew Haisley
 *
 *
 *
 *
 *
 */

#include <boost/algorithm/string.hpp>

#include <ctype.h>
#include <errno.h>
#include <stdlib.h>
#include <string.h>

#include <mx.h>
#include <mx_error.h>
#include <mx_string.h>

char* mx_string_copy(const char* s)
/*
 * PARAMS  :
 *   IN    - s     the string to copy
 *   OUT   - err  what went wrong
 *
 * ACTION  : copies the string allocating new memory
 * RETURNS : the copy on success, NULL otherwise
 *
 * ERRORS : MX_MALLOC_FAILED
 *
 */
{
    char* r = new char[strlen(s) + 1];

    strcpy(r, s);
    return r;
}

void mx_char_string_to_uint16(const char* s1, uint16* s2)
/*
 * PARAMS  :
 *   IN  - s1
 *   OUT - s2
 *
 * ACTION : turns an 8 bit string into a 16 bit one
 *
 */
{
    int i = 0;

    while (s1[i] != 0) {
        s2[i] = s1[i];
        i++;
    }
    s2[i] = 0;
}

bool mx_is_blank(const char* s)
{
    int i = 0;

    while (s[i] != 0) {
        if (!isspace(s[i++])) {
            return FALSE;
        }
    }
    return TRUE;
}

char mx_last_nonspace(const char* s)
/*
 * PARAMS  :
 *   IN  - s
 *
 * ACTION : returns the last non-space character in a string or ' ' if there
 *        : are non
 *
 */
{
    int i;

    i = strlen(s) - 1;
    while (i >= 0) {
        if (!isspace(s[i])) {
            return s[i];
        }
    }
    return ' ';
}

uint16 mx_strlen(const uint16* string)
{
    uint16 result;

    result = 0;

    while (string[result] != 0) {
        result++;
    }

    return result;
}

bool mx_str_find(const char* haystack, const char* needle, int& start, bool match_case)
{
    int i = 0, j;
    start = 0;

    while (haystack[i] != 0) {
        j = 0;

        if (match_case) {
            while (
                (needle[j] == haystack[i]) && (needle[j] != 0) && (haystack[i] != 0)) {
                i++;
                j++;
            }
        } else {
            while (
                (tolower(needle[j]) == tolower(haystack[i])) && (needle[j] != 0) && (haystack[i] != 0)) {
                i++;
                j++;
            }
        }

        if (needle[j] == 0) {
            start = i - j;
            return TRUE;
        } else {
            i++;
        }
    }
    return FALSE;
}

std::vector<std::string> split(std::string s, char sep)
{
    std::vector<std::string> res;

    size_t i = 0;

    while (i < s.size()) {
        if (s[i] == sep) {
            res.push_back(s.substr(0, i));
            s = s.substr(i + 1, s.size() - i - 1);
            i = 0;
        }
        i++;
    }

    if (s.size() > 0) {
        res.push_back(s);
    }

    return res;
}

void mx_strncpy(char* dest, const char* src, size_t max)
{
    for (size_t i = 0; i < max; i++) {
        dest[i] = src[i];
        if (dest[i] == '\0') {
            return;
        }
    }
}
