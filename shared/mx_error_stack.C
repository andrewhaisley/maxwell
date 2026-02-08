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
 * MODULE/CLASS : error return class
 *
 * AUTHOR : Andrew Haisley
 *
 *
 *
 *
 *
 *
 */

#include <mx_error.h>
#include <mx_error_codes.h>
#include <mx_error_stack.h>
#include <mx_error_d.h>
#include <stdio.h>

static void (*exit_func)();

int global_error_code;

typedef struct
{
    int code;
    const char* text;
} mx_error_text;

static mx_error_text mx_error_texts[] = MX_ERROR_TEXTS;

void mx_error_stack::push(mx_error_stack* n)
{
    next = n;
}

mx_error_stack::~mx_error_stack()
{
    if (next != NULL) {
        delete next;
    }
}

mx_error_stack::mx_error_stack(int c, int l, const char* f)
{
    code = c;
    line = l;
    file = f;

    d = "none given";
    next = NULL;
}

mx_error_stack::mx_error_stack(int c, const char* desc, int l, const char* f)
{
    code = c;
    line = l;
    file = f;
    d = desc;
    next = NULL;
}

void mx_error_stack::print()
{
    int i = 0;

    fprintf(stderr, "ERROR: file:%s ", file);
    fprintf(stderr, "line:%d ", line);
    fprintf(stderr, "code:%d ", code);

    while (TRUE) {
        if (mx_error_texts[i].code == MX_ERROR_LAST) {
            break;
        }

        if (mx_error_texts[i].code == code) {
            fprintf(stderr, "type:%s ", mx_error_texts[i].text);
            break;
        }

        i++;
    }

    if ((d != NULL) && (d[0] != 0)) {
        fprintf(stderr, "description:%s\n", d);
    } else {
        fprintf(stderr, "\n");
    }

    if (next != NULL)
        next->print();
}

const char* mx_error_stack::get_text_from_code(int code)
{
    int i;

    i = 0;

    while (TRUE) {
        if (mx_error_texts[i].code == MX_ERROR_LAST) {
            return "no text available";
        }

        if (mx_error_texts[i].code == code) {
            return mx_error_texts[i].text;
        }
        i++;
    }
}

void mx_error_stack::init_exit_proc(void (*f)())
{
    exit_func = f;
}
