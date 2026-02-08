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
 * MODULE/CLASS : mx_printcap
 *
 * AUTHOR : Andrew Haisley
 *
 * This file
 *
 * DESCRIPTION:
 *
 *  Class to access a printcap file
 *
 *
 *
 *
 */

#include <ctype.h>
#include <errno.h>
#include <pwd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include "mx_printcap.h"
#include <mx_std.h>

char* mx_printcap::get_next_printer(FILE* f)
{
    static char line[500], *s;
    int i;

    line[0] = 0;

    s = get_next_line(f);
    if (s[0] == 0) {
        return line;
    } else {
        strcpy(line, s);
    }

    // get rid of any extension lines
    while (s[0] != 0) {
        i = strlen(s);
        if (s[i - 1] == '\\') {
            s = get_next_line(f);
        } else {
            break;
        }
    }

    i = 0;
    while (line[i] != ':' && line[i] != 0) {
        if (line[i] == '|') {
            line[i] = 0;
            break;
        }
        i++;
    }
    if (line[i] == ':') {
        line[i] = 0;
    }
    return line;
}

char* mx_printcap::get_next_line(FILE* f)
{
    static char line[500];
    int i, c;

    do {
        i = 0;
        while (TRUE) {
            c = fgetc(f);
            if (c == EOF || c == 10) {
                break;
            }
            line[i++] = c;
        }
        line[i] = 0;
    } while ((i == 0 && c != EOF) || (i > 0 && line[0] == '#'));

    return line;
}
