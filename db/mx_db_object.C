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
 * MODULE/CLASS : mx_db_object
 *
 * AUTHOR : Andrew Haisley
 *
 *
 *
 * DESCRIPTION: sort of persistant objects
 *
 *
 *
 */
#include <mx.h>
#include <mx_db_cc.h>
#include <mx_db_object.h>
#include <mx_string.h>
#include <stdio.h>
#include <stdlib.h>

mx_db_object::mx_db_object(uint32 docid, bool is_controlled)
{
    int err = MX_ERROR_OK;

    db_id = mx_db_cc_add_oid(err, docid, this);
    error = err;
    MX_ERROR_CLEAR(err);
    mem_state = mx_in_mem_e;

    controlled = is_controlled;
    doc_id = docid;
    return;
}

mx_db_object::mx_db_object(const mx_db_object& o)
{
    int err = MX_ERROR_OK;

    db_id = mx_db_cc_add_oid(err, o.get_docid(), this);
    error = err;
    MX_ERROR_CLEAR(err);

    mem_state = mx_in_mem_and_modified_e;
    controlled = o.controlled;
    doc_id = o.get_docid();
    return;
}

mx_db_object::mx_db_object(uint32 docid, uint32 oid, bool is_controlled)
{
    int err = MX_ERROR_OK;

    db_id = oid;
    mx_db_cc_add_known_oid(err, docid, this, oid);
    error = err;
    controlled = is_controlled;
    mem_state = mx_on_disk_e;

    MX_ERROR_CLEAR(err);

    doc_id = docid;
    return;
}

mx_db_object::~mx_db_object()
{
    int err;

    mx_db_cc_delete_oid(err, doc_id, db_id);
}

void mx_db_object::add_attribute(int& err, mx_attribute* initial)
{
    err = MX_ERROR_OK;

    mx_db_cc_set_attribute(err, doc_id, db_id, initial);
    MX_ERROR_CHECK(err);

abort:;
}

void mx_db_object::delete_attribute(int& err, const char* name)
{
    err = MX_ERROR_OK;

    mx_db_cc_del_attribute(err, doc_id, db_id, name);
    MX_ERROR_CHECK(err);

abort:;
}

mx_attribute* mx_db_object::get(int& err, const char* name)
{
    mx_attribute* copy;
    const mx_attribute* a;

    err = MX_ERROR_OK;

    a = mx_db_cc_get_attribute_ro(err, doc_id, db_id, name);
    MX_ERROR_CHECK(err);

    copy = new mx_attribute();

    *copy = *a;
    err = MX_ERROR_OK;
    return copy;

abort:
    return NULL;
}

const mx_attribute* mx_db_object::get_readonly(int& err, const char* name)
{
    const mx_attribute* a;

    err = MX_ERROR_OK;

    a = mx_db_cc_get_attribute_ro(err, doc_id, db_id, name);
    MX_ERROR_CHECK(err);

    return a;
abort:
    return NULL;
}

void mx_db_object::set(int& err, mx_attribute* a)
{
    err = MX_ERROR_OK;

    mx_db_cc_set_attribute(err, doc_id, db_id, a);
    MX_ERROR_CHECK(err);

abort:;
}

void mx_db_object::create_blob(int& err, const char* name)
{
    err = MX_ERROR_OK;

    mx_db_cc_create_blob(err, doc_id, db_id, name);
    MX_ERROR_CHECK(err);

abort:;
}

void mx_db_object::delete_blob(int& err, const char* name)
{
    err = MX_ERROR_OK;

    mx_db_cc_delete_blob(err, doc_id, db_id, name);
    MX_ERROR_CHECK(err);

abort:;
}

uint32 mx_db_object::get_blob_size(int& err, const char* name)
{
    uint32 res;

    err = MX_ERROR_OK;

    res = mx_db_cc_get_blob_size(err, doc_id, db_id, name);
    MX_ERROR_CHECK(err);
    return res;

abort:;
    return 0;
}

void mx_db_object::set_blob_size(int& err, const char* name, uint32 size)
{
    err = MX_ERROR_OK;

    mx_db_cc_set_blob_size(err, doc_id, db_id, name, size);
    MX_ERROR_CHECK(err);

abort:;
}

void mx_db_object::set_blob_data(int& err, const char* name,
    uint32 start_index,
    uint32 n_bytes,
    unsigned char* data)
{
    err = MX_ERROR_OK;

    mx_db_cc_set_blob_data(err, doc_id, db_id, name, start_index, n_bytes, data);
    MX_ERROR_CHECK(err);

abort:;
}

void mx_db_object::get_blob_data(int& err, const char* name,
    uint32 start_index,
    uint32 n_bytes,
    unsigned char* data)
{
    err = MX_ERROR_OK;

    mx_db_cc_get_blob_data(err, doc_id, db_id, name, start_index, n_bytes, data);
    MX_ERROR_CHECK(err);
abort:;
}

const unsigned char* mx_db_object::get_blob_readonly(int& err, const char* name,
    uint32 start_index,
    uint32 n_bytes)
{
    const unsigned char* res;

    err = MX_ERROR_OK;

    res = mx_db_cc_get_blob_readonly(err, doc_id, db_id, name, start_index, n_bytes);
    MX_ERROR_CHECK(err);

    return res;
abort:;
    return NULL;
}

void mx_db_object::copy_blob(int& err, bool create_new, const char* name, mx_db_object* in_object, const char* inname)
{
    int blob_size;

    err = MX_ERROR_OK;

    // copy the level data
    if (create_new) {
        create_blob(err, name);
        MX_ERROR_CHECK(err);
    }

    blob_size = in_object->get_blob_size(err, inname);
    MX_ERROR_CHECK(err);

    set_blob_size(err, name, blob_size);
    MX_ERROR_CHECK(err);

    {
        std::unique_ptr<unsigned char []> buffer(new unsigned char[blob_size]);

        in_object->get_blob_data(err, inname, 0, blob_size, buffer.get());
        MX_ERROR_CHECK(err);

        set_blob_data(err, name, 0, blob_size, buffer.get());
        MX_ERROR_CHECK(err);
    }

abort:;
}

mx_db_object* mx_db_object::get_owner_object(int& err)
{
    uint32 id;

    err = MX_ERROR_OK;

    id = get_default_id(err, MX_DB_OBJECT_OWNER_ID, 0);
    MX_ERROR_CHECK(err);

    if (id != 0) {
        return mx_db_cc_get_obj_from_oid(err, doc_id, id);
    } else {
        return NULL;
    }
abort:
    return NULL;
}

void mx_db_object::set_owner_object(int& err, mx_db_object* owner)
{
    uint32 id;
    const mx_attribute* a;

    err = MX_ERROR_OK;

    if (owner != NULL) {
        id = owner->get_db_id(err);
    } else {
        id = 0;
    }

    a = get_readonly(err, MX_DB_OBJECT_OWNER_ID);

    if (err == MX_DB_CLIENT_CACHE_NO_SUCH_ATTR) {
        MX_ERROR_CLEAR(err);

        set_id(err, MX_DB_OBJECT_OWNER_ID, id);
        MX_ERROR_CHECK(err);
    } else {
        MX_ERROR_CHECK(err);

        if (a->value.id != id) {
            set_id(err, MX_DB_OBJECT_OWNER_ID, id);
            MX_ERROR_CHECK(err);
        }
    }

    return;

abort:;
}

int mx_db_object::get_db_id(int& err)
{
    err = MX_ERROR_OK;
    return db_id;
}

void mx_db_object::set_int(int& err, const char* name, int32 i)
{
    mx_attribute attrib;

    err = MX_ERROR_OK;

    attrib.type = mx_int;
    attrib.value.i = i;
    attrib.length = 0;
    strcpy(attrib.name, name);

    this->set(err, &attrib);
    MX_ERROR_CHECK(err);
abort:;
}

void mx_db_object::set_float(int& err, const char* name, float f)
{
    mx_attribute attrib;

    err = MX_ERROR_OK;

    attrib.type = mx_float;
    attrib.value.f = f;
    strcpy(attrib.name, name);

    this->set(err, &attrib);
}

void mx_db_object::set_string(int& err, const char* name, const char* s)
{
    mx_attribute* a;

    err = MX_ERROR_OK;

    a = new mx_attribute(name, mx_string);
    a->value.ca = mx_string_copy(s);
    a->length = strlen(s);

    this->set(err, a);
    a->value.ca = NULL;
    delete a;
}

void mx_db_object::set_int_array(int& err, const char* name, int32* a, int length)
{
    mx_attribute attrib;

    err = MX_ERROR_OK;

    attrib.type = mx_int_array;
    attrib.value.ia = a;
    attrib.length = length;
    strcpy(attrib.name, name);

    this->set(err, &attrib);
}

void mx_db_object::set_id_array(int& err, const char* name, uint32* a, int length)
{
    mx_attribute attrib;

    err = MX_ERROR_OK;

    attrib.type = mx_int_array;
    attrib.value.ida = a;
    attrib.length = length;
    strcpy(attrib.name, name);

    this->set(err, &attrib);
}

void mx_db_object::set_float_array(int& err, const char* name, float* a, int length)
{
    mx_attribute attrib;

    err = MX_ERROR_OK;

    attrib.type = mx_float_array;
    attrib.value.fa = a;
    strcpy(attrib.name, name);
    attrib.length = length;

    this->set(err, &attrib);
}

int32 mx_db_object::get_int(int& err, const char* name)
{
    const mx_attribute* a;

    err = MX_ERROR_OK;

    a = this->get_readonly(err, name);
    MX_ERROR_CHECK(err);

    return a->value.i;
abort:
    return 0;
}

float mx_db_object::get_float(int& err, const char* name)
{
    const mx_attribute* a;

    err = MX_ERROR_OK;

    a = this->get_readonly(err, name);
    MX_ERROR_CHECK(err);

    return a->value.f;

abort:
    return 0.0;
}

const char* mx_db_object::get_string(int& err, const char* name)
{
    const mx_attribute* a;

    err = MX_ERROR_OK;

    a = this->get_readonly(err, name);
    MX_ERROR_CHECK(err);

    return a->value.s;
abort:
    return NULL;
}

int32 mx_db_object::get_default_int(int& err, const char* name, int32 def)
{
    const mx_attribute* a;

    err = MX_ERROR_OK;

    a = this->get_readonly(err, name);

    if (err == MX_DB_CLIENT_CACHE_NO_SUCH_ATTR) {
        MX_ERROR_CLEAR(err);
        return def;
    } else {
        MX_ERROR_CHECK(err);
        return a->value.i;
    }
abort:
    return def;
}

float mx_db_object::get_default_float(int& err, const char* name, float def)
{
    const mx_attribute* a;

    err = MX_ERROR_OK;

    a = this->get_readonly(err, name);

    if (err == MX_DB_CLIENT_CACHE_NO_SUCH_ATTR) {
        MX_ERROR_CLEAR(err);
        return def;
    } else {
        MX_ERROR_CHECK(err);
        return a->value.f;
    }
abort:
    return def;
}

const char* mx_db_object::get_default_string(int& err, const char* name, const char* def)
{
    const mx_attribute* a;

    err = MX_ERROR_OK;

    a = this->get_readonly(err, name);

    if (err == MX_DB_CLIENT_CACHE_NO_SUCH_ATTR) {
        MX_ERROR_CLEAR(err);
        return def;
    } else {
        MX_ERROR_CHECK(err);
        return a->value.s;
    }
abort:
    return def;
}

uint32 mx_db_object::get_id_array_item(int& err, const char* name, int32 index)
{
    const mx_attribute* a;

    err = MX_ERROR_OK;

    a = get_readonly(err, name);
    MX_ERROR_CHECK(err);

    return a->value.ida[index];

abort:
    return 0;
}

void mx_db_object::set_id_array_item(int& err, const char* name, int32 index, uint32 id)
{
    mx_attribute* a;

    err = MX_ERROR_OK;

    a = get(err, name);
    MX_ERROR_CHECK(err);

    a->value.ida[index] = id;
    set(err, a);
    MX_ERROR_CHECK(err);

    a->free_value();
    delete a;

abort:;
}

void mx_db_object::add_id_array_item(int& err, const char* name, uint32 id)
{
    mx_attribute* a;
    mx_attribute_value v;

    err = MX_ERROR_OK;

    a = get(err, name);
    if (err == MX_DB_CLIENT_CACHE_NO_SUCH_ATTR) {
        MX_ERROR_CLEAR(err);
        a = new mx_attribute(name, mx_id_array);
    } else {
        MX_ERROR_CHECK(err);
    }

    v.ida = new uint32[a->length + 1];

    memcpy(v.ida, a->value.ida, 4 * a->length);
    v.ida[a->length] = id;

    // free the old value
    a->free_value();

    a->type = mx_id_array;
    a->value = v;
    a->length++;

    set(err, a);
    MX_ERROR_CHECK(err);

    // free the new value as well - the cache makes a copy
    a->free_value();
    delete a;

abort:;
}

void mx_db_object::del_id_array_item(int& err, const char* name, int32 index)
{
    mx_attribute* a;
    mx_attribute_value v;
    uint32 i;

    err = MX_ERROR_OK;

    a = get(err, name);
    MX_ERROR_CHECK(err);

    v = a->value;

    for (i = index; i < (a->length - 1); i++) {
        v.ida[i] = v.ida[i + 1];
    }

    a->length--;

    set(err, a);
    MX_ERROR_CHECK(err);

    a->free_value();
    delete a;

abort:;
}

int32 mx_db_object::get_id_array_length(int& err, const char* name)
{
    const mx_attribute* a;

    err = MX_ERROR_OK;

    a = get_readonly(err, name);

    if (err == MX_DB_CLIENT_CACHE_NO_SUCH_ATTR) {
        MX_ERROR_CLEAR(err);
        return 0;
    } else {
        MX_ERROR_CHECK(err);
        return a->length;
    }

abort:
    return -1;
}

const uint32* mx_db_object::get_id_array_ro(int& err, const char* name)
{
    const mx_attribute* a;

    err = MX_ERROR_OK;

    a = get_readonly(err, name);
    MX_ERROR_CHECK(err);

    return a->value.ida;

abort:
    return NULL;
}

float mx_db_object::get_float_array_item(int& err, const char* name, int32 index)
{
    const mx_attribute* a;

    err = MX_ERROR_OK;

    a = get_readonly(err, name);
    MX_ERROR_CHECK(err);

    return a->value.fa[index];

abort:
    return 0.0;
}

void mx_db_object::set_float_array_item(int& err, const char* name, int32 index, float f)
{
    mx_attribute* a;

    err = MX_ERROR_OK;

    a = get(err, name);
    MX_ERROR_CHECK(err);

    a->value.fa[index] = f;
    set(err, a);
    MX_ERROR_CHECK(err);

    a->free_value();
    delete a;

abort:;
}

void mx_db_object::add_float_array_item(int& err, const char* name, float f)
{
    mx_attribute* a;
    mx_attribute_value v;

    err = MX_ERROR_OK;

    a = get(err, name);
    if (err == MX_DB_CLIENT_CACHE_NO_SUCH_ATTR) {
        MX_ERROR_CLEAR(err);
        a = new mx_attribute(name, mx_float_array);
    } else {
        MX_ERROR_CHECK(err);
    }

    v.fa = new float[a->length + 1];

    memcpy(v.fa, a->value.fa, 4 * a->length);
    v.fa[a->length] = f;

    a->free_value();
    a->type = mx_float_array;
    a->value = v;
    a->length++;

    set(err, a);
    MX_ERROR_CHECK(err);

    a->free_value();
    delete a;

abort:;
}

void mx_db_object::del_float_array_item(int& err, const char* name, int32 index)
{
    mx_attribute* a;
    mx_attribute_value v;
    uint32 i;

    err = MX_ERROR_OK;

    a = get(err, name);
    MX_ERROR_CHECK(err);

    v = a->value;

    for (i = index; i < (a->length - 1); i++) {
        v.fa[i] = v.fa[i + 1];
    }

    a->length--;

    set(err, a);
    MX_ERROR_CHECK(err);

    a->free_value();
    delete a;

abort:;
}

int32 mx_db_object::get_float_array_length(int& err, const char* name)
{
    const mx_attribute* a;

    err = MX_ERROR_OK;

    a = get_readonly(err, name);

    if (err == MX_DB_CLIENT_CACHE_NO_SUCH_ATTR) {
        MX_ERROR_CLEAR(err);
        return 0;
    } else {
        MX_ERROR_CHECK(err);
        return a->length;
    }

abort:
    return -1;
}

const float* mx_db_object::get_float_array_ro(int& err, const char* name)
{
    const mx_attribute* a;

    err = MX_ERROR_OK;

    a = get_readonly(err, name);
    MX_ERROR_CHECK(err);

    return a->value.fa;

abort:
    return NULL;
}

int mx_db_object::get_docid() const
{
    return doc_id;
}

int32 mx_db_object::get_int_array_item(int& err, const char* name, int32 index)
{
    const mx_attribute* a;

    err = MX_ERROR_OK;

    a = get_readonly(err, name);
    MX_ERROR_CHECK(err);

    return a->value.ia[index];

abort:
    return 0;
}

void mx_db_object::set_int_array_item(int& err, const char* name, int32 index, int32 i)
{
    mx_attribute* a;

    err = MX_ERROR_OK;

    a = get(err, name);
    MX_ERROR_CHECK(err);

    a->value.ia[index] = i;
    set(err, a);
    MX_ERROR_CHECK(err);

    a->free_value();
    delete a;

abort:;
}

void mx_db_object::add_int_array_item(int& err, const char* name, int32 i)
{
    mx_attribute* a;
    mx_attribute_value v;

    err = MX_ERROR_OK;

    a = get(err, name);
    if (err == MX_DB_CLIENT_CACHE_NO_SUCH_ATTR) {
        MX_ERROR_CLEAR(err);

        a = new mx_attribute(name, mx_int_array);
    } else {
        MX_ERROR_CHECK(err);
    }

    v.ia = new int32[a->length + 1];

    memcpy(v.ia, a->value.ia, 4 * a->length);
    v.ia[a->length] = i;

    a->free_value();
    a->type = mx_int_array;
    a->value = v;
    a->length++;

    set(err, a);
    MX_ERROR_CHECK(err);

    a->free_value();
    delete a;

abort:;
}

void mx_db_object::del_int_array_item(int& err, const char* name, int32 index)
{
    mx_attribute* a;
    mx_attribute_value v;
    uint32 i;

    err = MX_ERROR_OK;

    a = get(err, name);
    MX_ERROR_CHECK(err);

    v = a->value;

    for (i = index; i < (a->length - 1); i++) {
        v.ia[i] = v.ia[i + 1];
    }

    a->length--;

    set(err, a);
    MX_ERROR_CHECK(err);

    a->free_value();
    delete a;

abort:;
}

int32 mx_db_object::get_int_array_length(int& err, const char* name)
{
    const mx_attribute* a;

    err = MX_ERROR_OK;

    a = get_readonly(err, name);

    if (err == MX_DB_CLIENT_CACHE_NO_SUCH_ATTR) {
        MX_ERROR_CLEAR(err);
        return 0;
    } else {
        MX_ERROR_CHECK(err);
        return a->length;
    }

abort:
    return -1;
}

const int32* mx_db_object::get_int_array_ro(int& err, const char* name)
{
    const mx_attribute* a;

    err = MX_ERROR_OK;

    a = get_readonly(err, name);
    MX_ERROR_CHECK(err);

    return a->value.ia;

abort:
    return NULL;
}

uint32 mx_db_object::get_default_id(int& err, const char* name, uint32 def)
{
    const mx_attribute* a;

    err = MX_ERROR_OK;

    a = this->get_readonly(err, name);

    if (err == MX_DB_CLIENT_CACHE_NO_SUCH_ATTR) {
        MX_ERROR_CLEAR(err);
        return def;
    } else {
        MX_ERROR_CHECK(err);
        return a->value.id;
    }
abort:
    return def;
}

void mx_db_object::set_id(int& err, const char* name, uint32 id)
{
    mx_attribute attrib;

    err = MX_ERROR_OK;

    attrib.type = mx_id;
    attrib.value.id = id;
    attrib.length = 0;
    strcpy(attrib.name, name);

    this->set(err, &attrib);
    MX_ERROR_CHECK(err);
abort:;
}

void mx_db_object::move_to_document(int& err, int dest_docid)
{
    int new_oid = db_id;

    this->serialise_attributes(err);
    MX_ERROR_CHECK(err);

    mx_db_cc_move_object(err, this, doc_id, dest_docid, new_oid);
    MX_ERROR_CHECK(err);

    db_id = new_oid;
    doc_id = dest_docid;
abort:;
}

void mx_db_object::serialise_attributes(int& err)
{
    err = MX_ERROR_OK;
}

mx_db_mem_state_t mx_db_object::get_mem_state()
{
    return mem_state;
}

void mx_db_object::set_mem_state(mx_db_mem_state_t s)
{
    switch (s) {
    case mx_on_disk_e:
        printf("set_memstate %d, %d: on disk\n", doc_id, db_id);
        break;
    case mx_in_mem_e:
        printf("set_memstate %d, %d: in mem\n", doc_id, db_id);
        break;
    case mx_in_mem_and_modified_e:
        printf("set_memstate %d, %d: in mem and modified\n", doc_id, db_id);
        break;
    default:
        printf("set_memstate %d, %d: unknown\n", doc_id, db_id);
        break;
    }
    mem_state = s;
}

bool mx_db_object::is_controlled() const
{
    return controlled;
}

void mx_db_object::cc_serialise_attributes(int& err)
{
    if (controlled || (get_mem_state() != mx_in_mem_and_modified_e)) {
        return;
    } else {
        serialise_attributes(err);
        MX_ERROR_CHECK(err);

        set_mem_state(mx_in_mem_e);
    }
abort:;
}

void mx_db_object::cc_unserialise_attributes(int& err)
{
    if (controlled || (get_mem_state() != mx_on_disk_e)) {
        return;
    } else {
        set_mem_state(mx_in_mem_e);

        unserialise_attributes(err);
        MX_ERROR_CHECK(err);

        set_mem_state(mx_in_mem_e);
    }
abort:;
}

void mx_db_object::cc_uncache(int& err)
{
    uncache(err);
    MX_ERROR_CHECK(err);

    set_mem_state(mx_on_disk_e);
abort:;
}

void mx_db_object::unserialise_attributes(int& err)
{
    err = MX_ERROR_OK;
}

void mx_db_object::uncache(int& err)
{
    err = MX_ERROR_OK;
}
