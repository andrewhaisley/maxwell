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
 * MODULE/CLASS : mx_db_cc
 *
 * AUTHOR : Andrew Haisley
 *
 *
 *
 * DESCRIPTION:
 *
 *
 *
 */

#ifndef MX_DB_CLIENT_CACHE_H
#define MX_DB_CLIENT_CACHE_H

#include <mx.h>
#include <mx_file.h>

class mx_db_object;
class mx_attribute;

#define MX_DB_OBJECT_DOCUMENT_ID 1

// a cache of objects/object IDs
uint32 mx_db_cc_add_oid(int& err, uint32 docid, mx_db_object* o);
void mx_db_cc_add_known_oid(int& err, uint32 docid, mx_db_object* o, uint32 oid);
void mx_db_cc_delete_oid(int& err, uint32 docid, uint32 oid);
mx_db_object* mx_db_cc_get_obj_from_oid(int& err, uint32 docid, uint32 oid);

// a cache of attributes
void mx_db_cc_set_attribute(int& err, uint32 docid, uint32 oid, mx_attribute* a);
const mx_attribute* mx_db_cc_get_attribute_ro(int& err, uint32 docid, uint32 oid, const char* name);
void mx_db_cc_del_attribute(int& err, uint32 docid, uint32 oid, const char* name);

// blobs
void mx_db_cc_create_blob(int& err, uint32 docid, uint32 oid, const char* name);
void mx_db_cc_delete_blob(int& err, uint32 docid, uint32 oid, const char* name);
uint32 mx_db_cc_get_blob_size(int& err, uint32 docid, uint32 oid, const char* name);
void mx_db_cc_set_blob_size(int& err, uint32 docid, uint32 oid, const char* name, uint32 size);
void mx_db_cc_get_blob_data(int& err, uint32 docid, uint32 oid, const char* name, uint32 index, uint32 n_bytes, unsigned char* data);
void mx_db_cc_set_blob_data(int& err, uint32 docid, uint32 oid, const char* name, uint32 index, uint32 n_bytes, unsigned char* data);
const unsigned char* mx_db_cc_get_blob_readonly(int& err, uint32 docid, uint32 oid, const char* name, uint32 index, uint32 n_bytes);

// initialisation
void mx_db_cc_init(int& err);

// database type things - open, close, commit etc
void mx_db_cc_open_doc(int& err, uint32 docid, const char* file_name, bool recover, mx_file_create_type_t t = mx_file_simple_e);
void mx_db_cc_close_doc(int& err, uint32 docid);
void mx_db_cc_commit(int& err, uint32 docid);
void mx_db_cc_commit_as(int& err, uint32 docid, char* file_name);

// size
uint32 mx_db_cc_size(int& err);

// document description info
void mx_db_cc_set_info(int& err, uint32 docid, char* description, char* author, char* version);
const char* mx_db_cc_get_description(uint32 docid);
const char* mx_db_cc_get_author(uint32 docid);
const char* mx_db_cc_get_version(uint32 docid);

// modification test
bool mx_db_cc_doc_modified(int& err, uint32 docid);

// move an object between document - moves blobs and attributes.
// the oid will change as a result. This function will only work properly if
// all attributes and blobs are at least partly in the cache.
void mx_db_cc_move_object(int& err, mx_db_object* o, int source_doc, int dest_doc, int& oid);

#endif
