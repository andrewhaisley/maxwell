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
 * MODULE/CLASS : mx_word6
 *
 * AUTHOR : Andrew Haisley
 *
 *
 *
 * Support for reading and writing Word6 document files.
 *
 *
 *
 */
#ifndef MX_WORD_6_H
#define MX_WORD_6_H

#include <mx.h>
#include <mx_ole.h>
#include <mx_para_style.h>

class mx_wp_document;
class mx_wp_cursor;
class mx_prog_d;

#define MX_WORD_BUFFER_SIZE 500

typedef struct
{
    uint16 magic;
    uint8 encrypted;
    uint8 readonly;
    uint32 start_of_text;
    uint32 end_of_text;
} mx_word_6_header;

class mx_word_6 {
public:
    mx_word_6(int& err, char* name, bool plain_file = FALSE);
    ~mx_word_6();
    void dump_words(int& err);

    void import_into_document(
        int& err,
        mx_wp_document* doc,
        mx_wp_cursor* doc_cursor,
        mx_prog_d* pd = NULL);

    mx_ole* ole_file;
    FILE* file;

private:
    void read_header(int& err);

    uint32 read_uint32(int& err);
    uint16 read_uint16(int& err);
    uint8 read_uint8(int& err);

    uint16 read_uint16_from_buffer(uint8* buffer, int& length);
    int16 read_int16_from_buffer(uint8* buffer, int& length);
    uint32 read_uint32_from_buffer(uint8* buffer, int& length);

    mx_word_6_header header;

    bool plain_file;

    void seek(int& err, int pos);
    int tell(int& err);

    char buffer[MX_WORD_BUFFER_SIZE + 1];
    int buffer_pos;

    void add_char_to_buffer(int& err, uint8 c, mx_wp_cursor* cursor);
    bool update_progress();

    mx_prog_d* pd;
    int stream_length;
};

#endif
