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
 * MODULE/CLASS : mx_error.h
 *
 * AUTHOR : Andrew Haisley
 *
 *
 *
 *
 *
 */
#ifndef MX_ERROR_H
#define MX_ERROR_H

#include <mx_error_codes.h>
#include <mx_error_stack.h>
#include <mx_std.h>
#include <setjmp.h>
#include <stdio.h>

extern mx_error_stack* global_error_trace;
extern int global_error_code;

#define MX_ERROR_CHECK(e)                                               \
    if (e != MX_ERROR_OK) {                                             \
        mx_error_stack* n;                                              \
        n = new mx_error_stack(e, (char*)"", __LINE__, __FILE__);       \
        if (n == NULL) {                                                \
            fprintf(stderr, "HELP! MALLOC FAILED IN MX_ERROR_CHECK\n"); \
            exit(-1);                                                   \
        }                                                               \
        n->push(global_error_trace);                                    \
        global_error_trace = n;                                         \
        goto abort;                                                     \
    }

#define MX_ERROR_CLEAR(err)                                             \
    {                                                                   \
        delete global_error_trace;                                      \
        global_error_trace = new mx_error_stack(MX_ERROR_TRACEBACK,     \
            "top level",                                                \
            __LINE__,                                                   \
            __FILE__);                                                  \
        if (global_error_trace == NULL) {                               \
            fprintf(stderr, "HELP! MALLOC FAILED IN MX_ERROR_CLEAR\n"); \
            exit(-1);                                                   \
        }                                                               \
        err = MX_ERROR_OK;                                              \
    }

#define MX_ERROR_THROW(err, e) \
    {                          \
        err = e;               \
        MX_ERROR_CHECK(err);   \
    }

#define MX_ERROR_ASSERT(err, assertion)                     \
    {                                                       \
        if (!(assertion)) {                                 \
            MX_ERROR_THROW(err, MX_ERROR_ASSERTION_FAILED); \
        }                                                   \
    }

#endif // MX_ERROR_H
