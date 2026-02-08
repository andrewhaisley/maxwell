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
 */

#ifndef MX_ERROR_STACK_H
#define MX_ERROR_STACK_H

#include <X11/Intrinsic.h>
#include <time.h>

class mx_error_stack {

    mx_error_stack* next; /* next one down - for tracebacks */

public:
    int code; /* should always be an error */
    const char* d; /* a general description */
    int line; /* line it happened at */
    const char* file; /* file it happened in */

    mx_error_stack(int code, int line, const char* file);
    mx_error_stack(int code, const char* d, int line, const char* file);

    ~mx_error_stack();

    void push(mx_error_stack* n);
    void print();

    static void init_exit_proc(void (*f)());
    static const char* get_text_from_code(int code);
};

#endif
