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
#ifndef MX_STRING_H
#define MX_STRING_H

void mx_strncpy(char* dest, const char* src, size_t max);

char* mx_string_copy(const char* s);
/*
 * PARAMS  :
 *   IN    - s    the string to copy
 *
 * ACTION  : copies the string allocating new memory
 * RETURNS : the copy on success, NULL otherwise
 *
 */

void mx_char_string_to_uint16(const char* s1, uint16* s2);
/*
 * PARAMS  :
 *   IN  - s1
 *   OUT - s2
 *
 * ACTION : turns an 8 bit string into a 16 bit one
 *
 */

bool mx_is_blank(const char* s);
/*
 * PARAMS  :
 *   IN  - s
 *
 * ACTION : returns TRUE if there are any non-whitespace characters in
 *        : the string
 *
 */

char mx_last_nonspace(const char* s);
/*
 * PARAMS  :
 *   IN  - s
 *
 * ACTION : returns the last non-space character in a string or ' ' if there
 *        : are non
 *
 */

uint16 mx_strlen(const uint16* string);
/*
 * PARAMS  :
 *   IN  - string
 *
 * ACTION : strlen for 16 bit strings
 *
 */

bool mx_str_find(const char* haystack, const char* needle, int& start, bool match_case = TRUE);
/*
 * PARAMS  :
 *   IN  - haystack, needle
 *   OUT - start
 *   IN  - match_case
 *
 * ACTION  : tries to find the needle in the haystack. Sets start to the start of the
 *         : find if it does.
 *
 * RETURNS : TRUE if the thing was found, FALSE otherwise
 *
 */

std::vector<std::string> split(std::string s, char sep);

#endif
