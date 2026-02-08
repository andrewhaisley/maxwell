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
#ifndef MX_STDIO_H
#define MX_STDIO_H

#include <ctype.h>
#include <errno.h>
#include <mx.h>
#include <stdlib.h>
#include <string.h>

#define MAX_PATH_LEN 1024
#define MAX_FILE_NAME_LEN 1024
#define MAX_DIR_FILES 512

const char* mx_tmpnam(char* res);

void mx_copy_file(int& err, char* in, char* out);
/*
 * PARAMS  :
 *   OUT   - err  what went wrong
 *   IN    - s    the file to copy from
 *   IN    - t    the file to copy to
 *
 * ACTION  : copies the file
 * RETURNS : none
 *
 *
 */

int mx_translate_file_error(int e);
/*
 * PARAMS  :
 *   IN    - e  the error code from errno
 *
 * ACTION  : translates errno value for file error into maxwell code
 * RETURNS : the maxwell code
 *
 *
 */

uint32 mx_file_size(int& err, const char* file_name);
/*
 * PARAMS  :
 *   IN    - file_name the name of the file
 *
 * ACTION  : gets the size of a file
 * RETURNS : the size in bytes of the file
 *
 * NOTE: if the file specified is open and being used with the stdio routines,
 *       then the value returned here may be incorrect.
 *
 */

FILE* mx_fopen(int& err, const char* file_name, const char* mode);
/*
 * PARAMS  :
 *   IN    - file_name the name of the file
 *   IN    - mode stdio open mode
 *
 * ACTION  : opens the file
 * RETURNS : the file pointer
 *
 */

int mx_fprintf(int& err, FILE* f, const char* format, ...);
/*
 * PARAMS  :
 *   IN    - same kinds of stuff as printf...
 *
 * ACTION  : sends stuff to a file....
 * RETURNS : same as fprintf
 *
 *
 */

void mx_printf_warning(const char* format, ...);
/*
 * PARAMS  :
 *   IN    - same kinds of stuff as printf...
 *
 * ACTION  : prints a warning message. Prepends the string "WARNING: " to the
 *           string passed to it and appends a newline character.
 * RETURNS : nothing
 *
 *
 */

#endif
