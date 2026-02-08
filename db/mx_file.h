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
 *
 */
#ifndef MX_FILE_H
#define MX_FILE_H

#include <memory>
#include <mx.h>
#include <mx_attribute.h>
#include <mx_doc_file.h>

#define MX_SIMPLE_FILE_SIGNATURE 0x3a13ef98

#define MX_BLOCK_DESCRIPTION_LENGTH 200
#define MX_BLOCK_AUTHOR_LENGTH 100
#define MX_BLOCK_VERSION_LENGTH 100

typedef enum {
    mx_sfile_attr_e = 10,
    mx_sfile_blob_e,
    mx_sfile_blob_seg_e,
    mx_sfile_none_e
} mx_sfile_type_t;

class mx_file : public mx_doc_file {
    MX_RTTI(mx_file_class_e)

public:
    mx_file();
    ~mx_file();

    // open database file for reading
    void open(int& err, const char* file_name);

    // close file
    void close(int& err);

    // start a transaction
    void start_transaction(int& err);

    // commits changes to disk
    void commit(int& err);

    // change output file name
    virtual void duplicate(int& err, char* name);

    // what is the next thing available for reading?
    mx_sfile_type_t next_type(int& err);

    // get the next thing as an attribute
    mx_attribute* next_attribute(int& err, int& oid);

    // get the next thing as blob
    void next_blob(int& err, int& oid, char* name, int& length);

    // get the next thing as a blob segment
    std::shared_ptr<unsigned char[]> next_blob_segment(int& err, int& oid, int& seg_num, char* name, int &len);

    // write an attribute
    void write_attribute(int& err, int oid, mx_attribute* a);

    // write a blob
    void write_blob(int& err, int oid, const char* name, int length);

    // write a blob segment
    void write_blob_segment(int& err, int oid, int seg_num, int length, const char* name, const unsigned char* data);

    virtual uint32 get_next_id();
    virtual const char* get_description();
    virtual const char* get_author();
    virtual const char* get_version();

    virtual void set_info(int& err, char* description, char* author, char* version);

private:
    int in_file, out_file;
    uint32 next_id;

    char description[MX_BLOCK_DESCRIPTION_LENGTH];
    char author[MX_BLOCK_AUTHOR_LENGTH];
    char version[MX_BLOCK_VERSION_LENGTH];

    uint32 read_uint32(int& err);
    uint8 read_uint8(int& err);
    char* read_string(int& err);

    void write_uint32(int& err, uint32 i);
    void write_uint8(int& err, uint8 i);
    void write_string(int& err, const char* s);

    char file_name[MAX_PATH_LEN];
    static char temp_buf[4096];
};
#endif
