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
 * MODULE/CLASS : mx_file
 *
 * AUTHOR : Andrew Haisley
 *
 *
 *
 * DESCRIPTION:
 * document file access
 *
 *
 *
 */
#include <errno.h>
#include <fcntl.h>
#include <grp.h>
#include <netinet/in.h>
#include <pwd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>

#include <mx.h>
#include <mx_attribute.h>
#include <mx_file.h>

char mx_file::temp_buf[4096];

mx_file::mx_file()
{
    description[0] = 0;
    author[0] = 0;
    version[0] = 0;
    next_id = 2;
    in_file = out_file = -1;
}

mx_file::~mx_file()
{
    int err = MX_ERROR_OK;

    this->close(err);
    MX_ERROR_CHECK(err);

    return;
abort:
    global_error_trace->print();
    MX_ERROR_CLEAR(err);
}

void mx_file::open(int& err, const char* s)
{
    description[0] = 0;
    author[0] = 0;
    version[0] = 0;
    next_id = 2;

    strcpy(file_name, s);

    in_file = ::open(s, O_RDONLY);
    if (in_file < 0) {
        MX_ERROR_THROW(err, mx_translate_file_error(errno));
    } else {
        char* temp;
        uint32 sig;

        sig = read_uint32(err);
        MX_ERROR_CHECK(err);

        if (sig != MX_SIMPLE_FILE_SIGNATURE) {
            MX_ERROR_THROW(err, MX_BLOCK_NOT_MX_FILE);
        }

        temp = read_string(err);
        MX_ERROR_CHECK(err);

        strcpy(description, temp);

        temp = read_string(err);
        MX_ERROR_CHECK(err);

        strcpy(author, temp);

        temp = read_string(err);
        MX_ERROR_CHECK(err);

        strcpy(version, temp);

        next_id = read_uint32(err);
        MX_ERROR_CHECK(err);
    }
abort:;
}

void mx_file::close(int& err)
{
    if (in_file > 0) {
        if (-1 == ::close(in_file)) {
            MX_ERROR_THROW(err, mx_translate_file_error(errno));
        }
    }

    if (out_file > 0) {
        if (-1 == ::close(out_file)) {
            MX_ERROR_THROW(err, mx_translate_file_error(errno));
        }
    }
abort:
    in_file = out_file = -1;
}

void mx_file::start_transaction(int& err)
{
    printf("mx_file::start_transaction\n");
    char temp[MAX_PATH_LEN + 2];

    sprintf(temp, "%s~", file_name);
    out_file = ::open(temp, O_WRONLY | O_CREAT | O_TRUNC, 0666);
    if (out_file < 0) {
        MX_ERROR_THROW(err, mx_translate_file_error(errno));
    }

    write_uint32(err, MX_SIMPLE_FILE_SIGNATURE);
    MX_ERROR_CHECK(err);

    write_string(err, description);
    MX_ERROR_CHECK(err);

    write_string(err, author);
    MX_ERROR_CHECK(err);

    write_string(err, version);
    MX_ERROR_CHECK(err);

    write_uint32(err, next_id);
    MX_ERROR_CHECK(err);

abort:;
}

void mx_file::commit(int& err)
{
    printf("mx_file::commit\n");
    char temp[MAX_PATH_LEN + 2];

    write_uint8(err, mx_sfile_none_e);
    MX_ERROR_CHECK(err);

    sprintf(temp, "%s~", file_name);
    if (-1 == ::close(out_file)) {
        MX_ERROR_THROW(err, mx_translate_file_error(errno));
    }
    out_file = -1;

    if (rename(temp, file_name) != 0) {
        MX_ERROR_THROW(err, mx_translate_file_error(errno));
    }
abort:;
}

mx_sfile_type_t mx_file::next_type(int& err)
{
    uint8 i;

    i = read_uint8(err);
    MX_ERROR_CHECK(err);

    return (mx_sfile_type_t)i;

abort:
    return mx_sfile_none_e;
}

mx_attribute* mx_file::next_attribute(int& err, int& oid)
{
    mx_attribute* res;
    uint32 len;

    oid = read_uint32(err);
    MX_ERROR_CHECK(err);

    len = read_uint32(err);
    MX_ERROR_CHECK(err);

    if ((uint32)read(in_file, temp_buf, len) != len) {
        MX_ERROR_THROW(err, mx_translate_file_error(errno));
    } else {
        char* temp = temp_buf;

        res = new mx_attribute();

        res->unserialise(err, (uint8**)&temp);
        MX_ERROR_CHECK(err);

        return res;
    }
abort:
    return NULL;
}

void mx_file::next_blob(int& err, int& oid, char* name, int& length)
{
    char* temp;

    oid = read_uint32(err);
    MX_ERROR_CHECK(err);

    length = read_uint32(err);
    MX_ERROR_CHECK(err);

    temp = read_string(err);
    MX_ERROR_CHECK(err);

    strcpy(name, temp);

abort:;
}

std::shared_ptr<unsigned char[]> mx_file::next_blob_segment(int& err, int& oid, int& seg_num, char* name, int &len)
{
    char* temp;

    oid = read_uint32(err);
    MX_ERROR_CHECK(err);

    len = read_uint32(err);
    MX_ERROR_CHECK(err);

    seg_num = read_uint32(err);
    MX_ERROR_CHECK(err);

    temp = read_string(err);
    MX_ERROR_CHECK(err);

    strcpy(name, temp);

    {
        std::shared_ptr<unsigned char[]> res(new unsigned char[len]);

        if (read(in_file, res.get(), len) != len) {
            MX_ERROR_THROW(err, mx_translate_file_error(errno));
        }

        return res;
    }

abort:;
    return nullptr;
}

void mx_file::write_attribute(int& err, int oid, mx_attribute* a)
{
    uint32 len;
    uint8* temp = (uint8*)temp_buf;

    len = a->get_serialised_size(err);
    MX_ERROR_CHECK(err);

    write_uint8(err, mx_sfile_attr_e);
    MX_ERROR_CHECK(err);

    write_uint32(err, oid);
    MX_ERROR_CHECK(err);

    write_uint32(err, len);
    MX_ERROR_CHECK(err);

    a->serialise(err, &temp);
    MX_ERROR_CHECK(err);

    if ((uint32)write(out_file, temp_buf, len) != len) {
        MX_ERROR_THROW(err, mx_translate_file_error(errno));
    }

abort:;
}

void mx_file::write_blob(int& err, int oid, const char* name, int length)
{
    write_uint8(err, mx_sfile_blob_e);
    MX_ERROR_CHECK(err);

    write_uint32(err, oid);
    MX_ERROR_CHECK(err);

    write_uint32(err, length);
    MX_ERROR_CHECK(err);

    write_string(err, name);
    MX_ERROR_CHECK(err);

abort:;
}

void mx_file::write_blob_segment(int& err, int oid, int seg_num, int length, const char* name, const unsigned char* data)
{
    write_uint8(err, mx_sfile_blob_seg_e);
    MX_ERROR_CHECK(err);

    write_uint32(err, oid);
    MX_ERROR_CHECK(err);

    write_uint32(err, length);
    MX_ERROR_CHECK(err);

    write_uint32(err, seg_num);
    MX_ERROR_CHECK(err);

    write_string(err, name);
    MX_ERROR_CHECK(err);

    if (write(out_file, data, length) != length) {
        MX_ERROR_THROW(err, mx_translate_file_error(errno));
    }

abort:;
}

uint32 mx_file::read_uint32(int& err)
{
    uint32 i = 0;

    if (read(in_file, &i, 4) != 4) {
        MX_ERROR_THROW(err, mx_translate_file_error(errno));
    }
abort:
    return ntohl(i);
}

uint8 mx_file::read_uint8(int& err)
{
    uint8 i = 0;

    if (read(in_file, &i, 1) != 1) {
        MX_ERROR_THROW(err, mx_translate_file_error(errno));
    }
abort:
    return i;
}

void mx_file::write_uint32(int& err, uint32 i)
{
    uint32 x = htonl(i);
    if (write(out_file, &x, 4) != 4) {
        MX_ERROR_THROW(err, mx_translate_file_error(errno));
    }
abort:;
}

void mx_file::write_uint8(int& err, uint8 i)
{
    if (write(out_file, &i, 1) != 1) {
        MX_ERROR_THROW(err, mx_translate_file_error(errno));
    }
abort:;
}

char* mx_file::read_string(int& err)
{
    int i = 0;

    while (TRUE) {
        if (read(in_file, temp_buf + i, 1) != 1) {
            MX_ERROR_THROW(err, mx_translate_file_error(errno));
        }
        if (temp_buf[i++] == 0) {
            return temp_buf;
        }
    }
abort:
    return NULL;
}

void mx_file::write_string(int& err, const char* s)
{
    int n = strlen(s) + 1;

    if (write(out_file, s, n) != n) {
        MX_ERROR_THROW(err, mx_translate_file_error(errno));
    }
abort:;
}

void mx_file::duplicate(int& err, char* name)
{
    strcpy(file_name, name);
}

uint32 mx_file::get_next_id()
{
    return next_id++;
}

const char* mx_file::get_description()
{
    return "not implemented";
}

const char* mx_file::get_author()
{
    return "not implemented";
}

const char* mx_file::get_version()
{
    return "not implemented";
}

void mx_file::set_info(int& err, char* description, char* author, char* version)
{
}
