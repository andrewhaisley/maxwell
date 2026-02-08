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
 * MODULE/CLASS : mx_doc_file
 *
 * AUTHOR : Andrew Haisley
 *
 *
 *
 * DESCRIPTION:
 * base class for document file access
 *
 *
 *
 *
 */
#ifndef MX_DOC_FILE_H
#define MX_DOC_FILE_H

#include <mx.h>
#include <mx_attribute.h>

typedef enum {
    mx_file_simple_e,
    mx_file_db_e
} mx_file_create_type_t;

class mx_doc_file : public mx_rtti {
public:
    mx_doc_file();
    virtual ~mx_doc_file();

    // close file - calls commit also
    virtual void close(int& err) = 0;

    // start a transaction
    virtual void start_transaction(int& err) = 0;

    // commits changes to disk
    virtual void commit(int& err) = 0;

    virtual const char* get_description() = 0;
    virtual const char* get_author() = 0;
    virtual const char* get_version() = 0;

    virtual void set_info(int& err, char* description, char* author, char* version) = 0;

    // get an object id
    virtual uint32 get_next_id() = 0;

    virtual void duplicate(int& err, char* new_file_name);

};

#endif
