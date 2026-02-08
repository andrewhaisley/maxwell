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
 * MODULE/CLASS : mx_word_6
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
#include <errno.h>
#include <mx.h>
#include <mx_ole.h>
#include <mx_word_6.h>

#include <mx_prog_d.h>
#include <mx_wp_cursor.h>
#include <mx_wp_doc.h>

mx_word_6::mx_word_6(int& err, char* name, bool plain_file)
{
    mx_word_6::plain_file = plain_file;

    if (plain_file) {
        file = fopen(name, "r");
        if (file == NULL) {
            MX_ERROR_THROW(err, mx_translate_file_error(errno));
        }
    } else {
        ole_file = new mx_ole(err, name, "r");
        MX_ERROR_CHECK(err);

        ole_file->open(err, "WordDocument");
        MX_ERROR_CHECK(err);
    }

    read_header(err);
    MX_ERROR_CHECK(err);

    if (header.encrypted) {
        MX_ERROR_THROW(err, MX_WORD_ENCRYPTED);
    }

abort:;
}

mx_word_6::~mx_word_6()
{
    if (plain_file) {
        fclose(file);
    } else {
        delete ole_file;
    }
}

void mx_word_6::dump_words(int& err)
{
    uint32 p;
    uint8 c;

    // seek to start of text
    seek(err, header.start_of_text);
    MX_ERROR_CHECK(err);

    while (TRUE) {
        p = tell(err);
        MX_ERROR_CHECK(err);

        if (p >= header.end_of_text) {
            break;
        }

        if (plain_file) {
            auto x = fread(&c, 1, 1, file);
            if (x != 1) {
                MX_ERROR_THROW(err, errno);
            }
        } else {
            ole_file->read(err, &c, 1);
            MX_ERROR_CHECK(err);
        }

        switch (c) {
        case 13:
            printf("\npara end\n");
            break;
        case 11:
            printf("\nhard line break\n");
            break;
        case 31:
            printf("\nnon-req hyphen\n");
            break;
        case 30:
            printf("\nnon-break hyphen\n");
            break;
        case 160:
            printf("\nnon-break space\n");
            break;
        case 12:
            printf("\npage or section break\n");
            break;
        case 14:
            printf("\ncolumn break\n");
            break;
        case 9:
            printf("\ntab\n");
            break;
        case 19:
            printf("\nfield begin\n");
            break;
        case 20:
            printf("\nfield sep\n");
            break;
        case 21:
            printf("\nfield end\n");
            break;
        case 7:
            printf("\ncell or row end\n");
            break;
        default:
            printf("%c", c);
            break;
        }
    }
abort:;
}

void mx_word_6::read_header(int& err)
{
    uint8 temp;

    header.magic = read_uint16(err);
    MX_ERROR_CHECK(err);

    (void)read_uint32(err);
    MX_ERROR_CHECK(err);
    (void)read_uint32(err);
    MX_ERROR_CHECK(err);

    temp = read_uint8(err);
    MX_ERROR_CHECK(err);
    temp = read_uint8(err);
    MX_ERROR_CHECK(err);

    header.encrypted = temp & 0x1;
    header.readonly = (temp >> 1) & 0x1;

    (void)read_uint32(err);
    MX_ERROR_CHECK(err);
    (void)read_uint32(err);
    MX_ERROR_CHECK(err);
    (void)read_uint32(err);
    MX_ERROR_CHECK(err);

    header.start_of_text = read_uint32(err);
    MX_ERROR_CHECK(err);
    header.end_of_text = read_uint32(err);
    MX_ERROR_CHECK(err);
abort:;
}

uint32 mx_word_6::read_uint32(int& err)
{
    uint8 buf[4];
    uint32 res;

    if (plain_file) {
        if (fread(buf, 4, 1, file) != 1) {
            MX_ERROR_THROW(err, mx_translate_file_error(errno));
        }
    } else {
        ole_file->read(err, buf, 4);
        MX_ERROR_CHECK(err);
    }

    res = buf[0];
    res |= buf[1] << 8;
    res |= buf[2] << 16;
    res |= buf[3] << 24;

    return res;

abort:
    return 0;
}

uint16 mx_word_6::read_uint16(int& err)
{
    uint8 buf[2];
    uint16 res;

    if (plain_file) {
        if (fread(buf, 2, 1, file) != 1) {
            MX_ERROR_THROW(err, mx_translate_file_error(errno));
        }
    } else {
        ole_file->read(err, buf, 2);
        MX_ERROR_CHECK(err);
    }

    res = buf[0];
    res |= buf[1] << 8;

    return res;

abort:
    return 0;
}

uint8 mx_word_6::read_uint8(int& err)
{
    uint8 res;

    if (plain_file) {
        if (fread(&res, 1, 1, file) != 1) {
            MX_ERROR_THROW(err, mx_translate_file_error(errno));
        }
    } else {
        ole_file->read(err, &res, 1);
        MX_ERROR_CHECK(err);
    }

    return res;
abort:
    return 0;
}

void mx_word_6::seek(int& err, int pos)
{
    if (plain_file) {
        if (fseek(file, pos, SEEK_SET) != 0) {
            MX_ERROR_THROW(err, mx_translate_file_error(errno));
        }
    } else {
        ole_file->seek(err, pos);
        MX_ERROR_CHECK(err);
    }
abort:;
}

int mx_word_6::tell(int& err)
{
    if (plain_file) {
        return ftell(file);
    } else {
        return ole_file->tell(err);
    }
}

void mx_word_6::import_into_document(
    int& err,
    mx_wp_document* doc,
    mx_wp_cursor* doc_cursor,
    mx_prog_d* pd)
{
    uint8 c;
    int n = 0;
    uint32 p;

    buffer_pos = 0;

    mx_wp_doc_pos pos;

    mx_word_6::pd = pd;
    stream_length = ole_file->size(err, "WordDocument");

    // seek to start of text
    seek(err, header.start_of_text);
    MX_ERROR_CHECK(err);

    pos.moveto_start(err, doc);
    MX_ERROR_CHECK(err);

    while (TRUE) {
        p = tell(err);
        MX_ERROR_CHECK(err);

        if (p >= header.end_of_text) {
            buffer[buffer_pos] = 0;

            doc_cursor->insert_text(err, buffer);
            MX_ERROR_CHECK(err);

            break;
        }

        if (plain_file) {
            auto x = fread(&c, 1, 1, file);
            if (x != 1) {
                MX_ERROR_THROW(err, errno);
            }
        } else {
            ole_file->read(err, &c, 1);
            MX_ERROR_CHECK(err);
        }

        switch (c) {
        case 13:
            buffer[buffer_pos] = 0;

            doc_cursor->insert_text(err, buffer);
            MX_ERROR_CHECK(err);

            buffer_pos = 0;

            doc_cursor->insert_para_break(err);
            MX_ERROR_CHECK(err);
            break;

        case 11:
            // printf("\nhard line break\n");
            break;

        case 30:
        case 31:
            add_char_to_buffer(err, c, doc_cursor);
            MX_ERROR_CHECK(err);
            break;

        case 160:
            add_char_to_buffer(err, ' ', doc_cursor);
            MX_ERROR_CHECK(err);
            break;

        case 12:
        case 14:
            buffer[buffer_pos] = 0;

            doc_cursor->insert_text(err, buffer);
            MX_ERROR_CHECK(err);

            doc_cursor->insert_para_break(err);
            MX_ERROR_CHECK(err);

            buffer_pos = 0;
            break;

        case 9:
            add_char_to_buffer(err, '\t', doc_cursor);
            MX_ERROR_CHECK(err);
            break;

        case 19:
            // printf("\nfield begin\n");
            break;

        case 20:
            // printf("\nfield sep\n");
            break;

        case 21:
            // printf("\nfield end\n");
            break;

        case 7:
            // printf("\ncell or row end\n");
            break;

        default:
            if (isprint(c)) {
                add_char_to_buffer(err, c, doc_cursor);
                MX_ERROR_CHECK(err);
            }
            break;
        }

        if (++n == 100) {
            if (update_progress()) {
                MX_ERROR_THROW(err, MX_USER_CANCEL);
            }
            n = 0;
        }
    }
abort:;
}

void mx_word_6::add_char_to_buffer(int& err, uint8 c, mx_wp_cursor* cursor)
{
    if (buffer_pos == MX_WORD_BUFFER_SIZE) {
        buffer[buffer_pos] = 0;

        cursor->insert_text(err, buffer);
        MX_ERROR_CHECK(err);

        buffer_pos = 0;
    }
    buffer[buffer_pos++] = c;

abort:;
}

bool mx_word_6::update_progress()
{
    float percent;
    int file_offset, err = MX_ERROR_OK;

    file_offset = ole_file->tell(err);
    MX_ERROR_CLEAR(err);

    percent = 100.0 * (float)file_offset / (float)stream_length;

    if (pd == NULL) {
        return FALSE;
    } else {
        pd->set_progress((int)percent);
        return pd->peek_cancel();
    }
}

int16 mx_word_6::read_int16_from_buffer(uint8* buffer, int& length)
{
    int16 result;

    result = buffer[length++] * 256;
    result += buffer[length++];

    return result;
}

uint16 mx_word_6::read_uint16_from_buffer(uint8* buffer, int& length)
{
    uint16 result;

    result = buffer[length++] * 256;
    result += buffer[length++];

    return result;
}

uint32 mx_word_6::read_uint32_from_buffer(uint8* buffer, int& length)
{
    uint32 result;

    result = buffer[length++] * 256 * 256 * 256;
    result += buffer[length++] * 256 * 256;
    result += buffer[length++] * 256;
    result += buffer[length++];

    return result;
}
