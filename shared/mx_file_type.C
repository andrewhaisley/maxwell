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
 */

#include <ctype.h>
#include <netinet/in.h>
#include <string.h>
#include <xpm.h>

#include <mx.h>
#include <mx_file.h>
#include <mx_file_type.h>
extern "C" {
#include <png.h>
}

static bool is_document_file(unsigned char* s, int i)
{
    uint32 sig;

    memcpy(&sig, s, 4);
    return (i >= 100) && (ntohl(sig) == MX_SIMPLE_FILE_SIGNATURE);
}

static bool is_rtf_file(unsigned char* s, int i)
{
    return (i >= 6) && (strncmp((char*)s, "{\\rtf1}", 6) == 0);
}

static bool is_word_2_file(unsigned char* s, int i)
{
    return FALSE;
}

static bool is_word_5_file(unsigned char* s, int i)
{
    return FALSE;
}

static bool is_word_6_file(unsigned char* s, int i)
{
    return i >= 512 && s[0] == 0xd0 && s[1] == 0xcf && s[2] == 0x11 && s[3] == 0xe0 && s[4] == 0xa1 && s[5] == 0xb1 && s[6] == 0x1a && s[7] == 0xe1;
}

static bool is_word_perfect_file(unsigned char* s, int i)
{
    return FALSE;
}

static bool is_ascii_file(unsigned char* s, int i)
{
    int j;

    for (j = 0; j < i; j++) {
        if ((s[j] != 13) && (s[j] != 10) && (s[j] != 9) && (s[j] != '') && (!isprint(s[j]))) {
            return FALSE;
        }
    }
    return TRUE;
}

static bool is_tiff_file(unsigned char* s, int i)
{
    return (i >= 3) && (s[2] == 42);
}

static bool is_png_file(unsigned char* s, int i)
{
    return (i >= 4) && (strncmp((char*)(s + 1), "PNG", 3) == 0);
}

static bool is_xpm_file(unsigned char* s, int i)
{
    return (i >= 5) && (strncmp((char*)s, "! XPM", 5) == 0);
}

static bool is_xbm_file(const char* filename)
{
    unsigned int width, height;
    int t1, t2;
    unsigned char* buffer = NULL;
    int retValue;

    retValue = XReadBitmapFileData(filename,
        &width,
        &height,
        &buffer,
        &t1, &t2);
    XFree(buffer);

    return retValue == BitmapSuccess;
}

static bool is_jpg_file(unsigned char* s, int i)
{
    return (i >= 10) && (strncmp((char*)(s + 6), "JFIF", 4) == 0);
}

static bool is_ps_file(unsigned char* s, int i)
{
    return (i >= 4) && (strncmp((char*)s, "%!PS", 4) == 0);
}

// checks if a file is of a given type
bool mx_is_file_type(const char* file_name, mx_file_type_t t)
{
    FILE* f;
    unsigned char b[1024];

    int i;

    f = fopen(file_name, "r");
    if (f == NULL) {
        return FALSE;
    }

    i = fread(b, 1, 1024, f);
    fclose(f);

    if (i <= 0) {
        return FALSE;
    }

    switch (t) {
    case mx_document_file_e:
        return is_document_file(b, i);
    case mx_rtf_file_e:
        return is_rtf_file(b, i);
    case mx_word_2_file_e:
        return is_word_2_file(b, i);
    case mx_word_5_file_e:
        return is_word_5_file(b, i);
    case mx_word_6_file_e:
        return is_word_6_file(b, i);
    case mx_word_perfect_file_e:
        return is_word_perfect_file(b, i);
    case mx_ascii_file_e:
        return is_ascii_file(b, i) && !is_rtf_file(b, i) && !is_ps_file(b, i) && !is_xbm_file(file_name) && !is_xpm_file(b, i);
    case mx_tiff_file_e:
        return is_tiff_file(b, i);
    case mx_png_file_e:
        return is_png_file(b, i);
    case mx_xpm_file_e:
        return is_xpm_file(b, i);
    case mx_xbm_file_e:
        return is_xbm_file(file_name);
    case mx_jpg_file_e:
        return is_jpg_file(b, i);
    case mx_ps_file_e:
        return is_ps_file(b, i);
    case mx_all_file_e:
        return TRUE;
    case mx_other_file_e:
        return !is_document_file(b, i) && !is_rtf_file(b, i) && !is_ascii_file(b, i) && !is_tiff_file(b, i) && !is_png_file(b, i) && !is_jpg_file(b, i) && !is_xpm_file(b, i) && !is_xbm_file(file_name) && !is_ps_file(b, i);
    default:
        return FALSE;
    }

    return FALSE;
}

const char* mx_file_type_name(mx_file_type_t t)
{
    switch (t) {
    case mx_document_file_e:
        return "Maxwell";
    case mx_rtf_file_e:
        return "RTF";
    case mx_word_2_file_e:
        return "Word 2";
    case mx_word_5_file_e:
        return "Word 5";
    case mx_word_6_file_e:
        return "Word 6";
    case mx_word_perfect_file_e:
        return "Word Perfect";
    case mx_ascii_file_e:
        return "ASCII";
    case mx_tiff_file_e:
        return "TIFF";
    case mx_png_file_e:
        return "PNG";
    case mx_jpg_file_e:
        return "JPG";
    case mx_xpm_file_e:
        return "XPM";
    case mx_xbm_file_e:
        return "XBM";
    case mx_ps_file_e:
        return "PS";
    case mx_all_file_e:
        return "all";
    case mx_template_e:
        return "template";
    case mx_other_file_e:
    default:
        return "other";
    }
}

// gets a fuller name of a file type
const char* mx_file_type_full_name(mx_file_type_t t)
{
    switch (t) {
    case mx_document_file_e:
        return "Maxwell Document";
    case mx_rtf_file_e:
        return "Rich Text Format (RTF)";
    case mx_word_2_file_e:
        return "Microsoft Word Version 2 (PC)";
    case mx_word_5_file_e:
        return "Microsoft Word Version 5 (Mac)";
    case mx_word_6_file_e:
        return "Microsoft Word Version 7 (PC/Mac)";
    case mx_word_perfect_file_e:
        return "Word Perfect";
    case mx_ascii_file_e:
        return "ASCII";
    case mx_tiff_file_e:
        return "TIFF";
    case mx_png_file_e:
        return "PNG";
    case mx_jpg_file_e:
        return "JPG";
    case mx_xpm_file_e:
        return "XPM";
    case mx_xbm_file_e:
        return "XBM";
    case mx_ps_file_e:
        return "PostScript";
    case mx_all_file_e:
        return "All";
    case mx_template_e:
        return "Maxwell document template";
    case mx_other_file_e:
    default:
        return "Other";
    }
}

mx_file_type_t mx_guess_file_type(const char* file_name)
{
    int t;

    for (t = (int)mx_document_file_e; t < (int)mx_other_file_e; t++) {
        if (mx_is_file_type(file_name, (mx_file_type_t)t)) {
            return (mx_file_type_t)t;
        }
    }
    return mx_other_file_e;
}

// does a file type belong to a given class?
bool is_type_in_class(mx_file_type_t t, mx_file_class_t c)
{
    switch (c) {
    case mx_document_file_class_e:
        return t == mx_document_file_e || t == mx_rtf_file_e || t == mx_word_6_file_e || t == mx_ascii_file_e || t == mx_other_file_e || t == mx_all_file_e;
    case mx_image_file_class_e:
        return t == mx_tiff_file_e || t == mx_png_file_e || t == mx_xbm_file_e || t == mx_xpm_file_e || t == mx_jpg_file_e || t == mx_other_file_e || t == mx_all_file_e || t == mx_ps_file_e;
    case mx_save_file_class_e:
        return t == mx_document_file_e || t == mx_rtf_file_e || t == mx_ascii_file_e || t == mx_template_e;
    case mx_ps_file_class_e:
        return t == mx_ps_file_e;
    case mx_all_file_class_e:
    default:
        return TRUE;
    }
}
