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
#ifndef MX_FILE_TYPE_H
#define MX_FILE_TYPE_H
/*
 * MODULE/CLASS : mx_file_type
 *
 * AUTHOR : Andrew Haisley
 *
 * This file
 *
 * DESCRIPTION:
 *
 * Routines for figuring out the type of a file
 *
 *
 *
 *
 */

#include <mx_std.h>

#define MX_FILE_TYPE_NUM_TYPES 10

typedef enum {
    mx_document_file_e = 1,
    mx_rtf_file_e = 2,
    mx_word_2_file_e = 3,
    mx_word_5_file_e = 4,
    mx_word_6_file_e = 5,
    mx_word_perfect_file_e = 6,
    mx_ascii_file_e = 7,
    mx_tiff_file_e = 8,
    mx_png_file_e = 9,
    mx_jpg_file_e = 10,
    mx_xbm_file_e = 11,
    mx_xpm_file_e = 12,
    mx_ps_file_e = 13,
    mx_all_file_e = 14,
    mx_template_e = 15,
    mx_other_file_e = 16
} mx_file_type_t;

typedef enum {
    mx_document_file_class_e = 1,
    mx_save_file_class_e = 2,
    mx_image_file_class_e = 3,
    mx_all_file_class_e = 4,
    mx_ps_file_class_e = 5
} mx_file_class_t;

// does a file type belong to a given class?
bool is_type_in_class(mx_file_type_t t, mx_file_class_t c);

// gets the symbolic name of a file type
const char* mx_file_type_name(mx_file_type_t t);

// gets a fuller name of a file type
const char* mx_file_type_full_name(mx_file_type_t t);

// checks if a file is of a given type
bool mx_is_file_type(const char* file_name, mx_file_type_t t);

// guesses the file type
mx_file_type_t mx_guess_file_type(const char* file_name);

#endif
