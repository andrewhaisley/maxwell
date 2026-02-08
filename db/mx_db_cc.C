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
 * MODULE/CLASS : mx_db_client_cache
 *
 * AUTHOR : Andrew Haisley
 *
 *
 *
 *
 *
 */

#include <stdlib.h>
#include <string.h>
#include <map>
#include <memory>

#include <mx.h>
#include <mx_db_cc.h>
#include <mx_db_object.h>
#include <mx_doc.h>
#include <mx_error.h>
#include <mx_error_codes.h>
#include <mx_file.h>

class blob_cache_t {
public:
    blob_cache_t() {
        size = 0;
        new_blob = true;
    }
    uint32 size;
    std::shared_ptr<unsigned char[]> data;
    bool new_blob;
};

class obj_cache_t {
public:
    uint32 id;
    mx_db_object *obj;
    std::map<std::string, std::shared_ptr<mx_attribute>> attrs; 
    std::map<std::string, std::shared_ptr<blob_cache_t>> blobs;

    obj_cache_t()
    {
        id = 0;
        obj = nullptr;
    }
};

// a map of maps:
//
// [docid1] -> hash table of objects by oid (each is * obj_cache_t)
// [docid2] -> ......
//

static std::map<uint32, std::map<uint32, std::shared_ptr<obj_cache_t>>> m_docs;
static std::map<uint32, std::shared_ptr<mx_file>> m_files;

static unsigned char blob_readonly[MX_DB_OBJECT_MAX_READONLY_BLOB_SIZE];

std::shared_ptr<obj_cache_t> get_obj(int& err, uint32 docid, uint32 oid)
{
    if (m_docs.find(docid) == m_docs.end()) {
        MX_ERROR_THROW(err, MX_DB_CLIENT_CACHE_NO_SUCH_DOC);
    }
    if (m_docs[docid].find(oid) == m_docs[docid].end()) {
        MX_ERROR_THROW(err, MX_DB_CLIENT_CACHE_NO_SUCH_OID);
    }

    return m_docs[docid][oid];

abort:;
    return nullptr;
}

uint32 mx_db_cc_add_oid(int& err, uint32 docid, mx_db_object* o)
{
    {
        if (m_files.find(docid) == m_files.end()) {
            MX_ERROR_THROW(err, MX_DB_CLIENT_CACHE_NO_SUCH_DOC);
        }

        if (m_docs.find(docid) == m_docs.end()) {
            m_docs[docid] = std::map<uint32, std::shared_ptr<obj_cache_t>>();
        }

        obj_cache_t* item = new obj_cache_t;

        item->obj = o;
        item->id = m_files[docid]->get_next_id();

        m_docs[docid][item->id].reset(item);

        return item->id;
    }

abort:
    return 0;
}

static void mx_db_cc_delete_oid_from_cache(int& err, uint32 docid, uint32 oid)
{
    auto item = m_docs[docid][oid];

    // free the attributes
    for (auto i : item->attrs)
    {
        i.second->free_value();
    }

    // delete the object entry from the object hash
    m_docs[docid].erase(oid);
}

void mx_db_cc_delete_oid(int& err, uint32 docid, uint32 oid)
{
    err = MX_ERROR_OK;

    // drop it from the cache - if it's there
    mx_db_cc_delete_oid_from_cache(err, docid, oid);
    if (err == MX_HASH_NOT_FOUND) {
        MX_ERROR_CLEAR(err);

        return;
    }
    MX_ERROR_CHECK(err);

    return;

abort:;
}

mx_db_object* mx_db_cc_get_obj_from_oid(int& err, uint32 docid, uint32 oid)
{
    if (m_docs.find(docid) == m_docs.end() || m_docs[docid].find(oid) == m_docs[docid].end()) {
        err = MX_DB_CLIENT_CACHE_NO_SUCH_OID;
        return nullptr;
    } else {
        if (m_docs[docid][oid]->obj == nullptr)
        {
            MX_ERROR_THROW(err, MX_DB_CLIENT_CACHE_NO_SUCH_OID);
        }
        return m_docs[docid][oid]->obj;
    }
abort:
    return nullptr;
}

void mx_db_cc_set_attribute(int& err, uint32 docid, uint32 oid, mx_attribute* a)
{
    std::shared_ptr<obj_cache_t> cached_object;
    std::shared_ptr<mx_attribute> cached_attr;

    // is it too big?
    if (a->size(err) > MX_ATTRIBUTE_MAX_SIZE) {
        MX_ERROR_THROW(err, MX_DB_CLIENT_CACHE_ATTR_TOO_BIG);
    }

    // get the cached object record
    cached_object = get_obj(err, docid, oid);
    MX_ERROR_CHECK(err);

    // does the object already have the attribute?
    if (cached_object->attrs.find(a->name) == cached_object->attrs.end()) {
        // no, create it
        cached_attr = std::make_shared<mx_attribute>();
        cached_object->attrs[a->name] = cached_attr;
    } else {
        // yes, get rid of the old value
        cached_attr = cached_object->attrs[a->name];
        cached_attr->free_value();
    }

    // now copy the new value into place
    *cached_attr = *a;
   
abort:;
}

const mx_attribute* mx_db_cc_get_attribute_ro(int& err, uint32 docid, uint32 oid, const char* name)
{
    if (m_docs.find(docid) == m_docs.end()) {
        MX_ERROR_THROW(err, MX_DB_CLIENT_CACHE_NO_SUCH_DOC);
    }

    if (m_docs[docid].find(oid) == m_docs[docid].end()) {
        MX_ERROR_THROW(err, MX_DB_CLIENT_CACHE_NO_SUCH_ATTR);
    }

    {
        // get the object record
        auto o = get_obj(err, docid, oid);
        MX_ERROR_CHECK(err);

        if (o->attrs.find(name) == o->attrs.end()) {
            MX_ERROR_THROW(err, MX_DB_CLIENT_CACHE_NO_SUCH_ATTR);
        } else {
            return o->attrs[name].get();
        }
    
    }

abort:;
    return nullptr;
}

void mx_db_cc_del_attribute(int& err, uint32 docid, uint32 oid, const char* name)
{
    // get the object record
    auto o = get_obj(err, docid, oid);
    MX_ERROR_CHECK(err);

    if (o->attrs.find(name) == o->attrs.end()) {
        MX_ERROR_THROW(err, MX_DB_CLIENT_CACHE_NO_SUCH_ATTR);
    } else {
        o->attrs[name]->free_value();
        o->attrs.erase(name);
    }

abort:;
}

void mx_db_cc_init(int& err)
{
}

void mx_db_cc_set_blob_data(int& err, uint32 docid, uint32 oid, const char* name, uint32 index, uint32 n_bytes, unsigned char* data)
{
    std::shared_ptr<obj_cache_t> item;
    std::shared_ptr<blob_cache_t> blob;

    // get the object record
    item = get_obj(err, docid, oid);
    MX_ERROR_CHECK(err);

    if (item->blobs.find(name) == item->blobs.end()) {
        MX_ERROR_THROW(err, MX_DB_OBJECT_NO_SUCH_BLOB);
    } else {
        blob = item->blobs[name];
    }
    
    // check size
    if ((index + n_bytes) > blob->size) {
        MX_ERROR_THROW(err, MX_DB_OBJECT_BLOB_INDEX_RANGE);
    }

    memcpy(blob->data.get() + index, data, n_bytes);

abort:;
}

const unsigned char* mx_db_cc_get_blob_readonly(int& err, uint32 docid, uint32 oid, const char* name, uint32 index, uint32 n_bytes)
{
    if (n_bytes > MX_DB_OBJECT_MAX_READONLY_BLOB_SIZE) {
        MX_ERROR_THROW(err, MX_DB_OBJECT_BLOB_SLICE_SIZE);
    } else {
        mx_db_cc_get_blob_data(err, docid, oid, name, index, n_bytes, blob_readonly);
        MX_ERROR_CHECK(err);
    }
    return blob_readonly;
abort:;
    return nullptr;
}

void mx_db_cc_create_blob(int& err, uint32 docid, uint32 oid, const char* name)
{
    auto item = get_obj(err, docid, oid);
    MX_ERROR_CHECK(err);

    if (item->blobs.find(name) != item->blobs.end()) {
        MX_ERROR_THROW(err, MX_DB_CLIENT_CACHE_DUPLICATE_BLOB);
    }

    item->blobs[name] = std::make_shared<blob_cache_t>();
abort:;
}

void mx_db_cc_delete_blob(int& err, uint32 docid, uint32 oid, const char* name)
{
    auto item = get_obj(err, docid, oid);
    MX_ERROR_CHECK(err);

    item->blobs.erase(name);

abort:;
}

uint32 mx_db_cc_get_blob_size(int& err, uint32 docid, uint32 oid, const char* name)
{
    auto item = get_obj(err, docid, oid);
    MX_ERROR_CHECK(err);

    if (item->blobs.find(name) == item->blobs.end()) {
        MX_ERROR_THROW(err, MX_DB_OBJECT_NO_SUCH_BLOB);
    } else {
        return item->blobs[name]->size;
    }

abort:
    return 0;
}

void mx_db_cc_set_blob_size(int& err, uint32 docid, uint32 oid, const char* name, uint32 size)
{
    blob_cache_t* blob;

    auto item = get_obj(err, docid, oid);
    MX_ERROR_CHECK(err);

    if (item->blobs.find(name) == item->blobs.end()) {
        MX_ERROR_THROW(err, MX_DB_OBJECT_NO_SUCH_BLOB);
    } else {
        blob = item->blobs[name].get();
    }

    if (size == blob->size) {
        return;
    }

    {
        auto old_data = blob->data;

        blob->data.reset(new unsigned char[size]);

        if (size > blob->size) {
            memcpy(blob->data.get(), old_data.get(), blob->size);
        } else {
            memcpy(blob->data.get(), old_data.get(), size);
        }

        blob->size = size;
    }

abort:;
}

void mx_db_cc_get_blob_data(int& err, uint32 docid, uint32 oid, const char* name, uint32 index, uint32 n_bytes, unsigned char* data)
{
    std::shared_ptr<blob_cache_t> blob;

    auto item = get_obj(err, docid, oid);
    MX_ERROR_CHECK(err);

    if (item->blobs.find(name) == item->blobs.end()) {
        MX_ERROR_THROW(err, MX_DB_OBJECT_NO_SUCH_BLOB);
    } else {
        blob = item->blobs[name];
    }

    // check size
    if ((index + n_bytes) > blob->size) {
        MX_ERROR_THROW(err, MX_DB_OBJECT_BLOB_INDEX_RANGE);
    }

    memcpy(data, blob->data.get() + index, n_bytes);

abort:;
}

static void create_object(int& err, int docid, int oid)
{
    if (m_docs.find(docid) == m_docs.end()) {
        m_docs[docid] = std::map<uint32, std::shared_ptr<obj_cache_t>>();
    }

    if (m_docs[docid].find(oid) == m_docs[docid].end()) {
        std::shared_ptr<obj_cache_t> item = std::make_shared<obj_cache_t>();

        item->obj = nullptr;
        item->id = oid;

        m_docs[docid][oid] = item;
    }
}

static void read_file(int& err, int docid, std::shared_ptr<mx_file> &file)
{
    mx_sfile_type_t t;

    int oid, length, segnum;
    mx_attribute* a;

    char name[100];

    while (true) {
        t = file->next_type(err);
        MX_ERROR_CHECK(err);

        switch (t) {
        case mx_sfile_attr_e:

            a = file->next_attribute(err, oid);
            MX_ERROR_CHECK(err);

            create_object(err, docid, oid);
            MX_ERROR_CHECK(err);

            mx_db_cc_set_attribute(err, docid, oid, a);
            MX_ERROR_CHECK(err);

            break;

        case mx_sfile_blob_e:

            file->next_blob(err, oid, name, length);
            MX_ERROR_CHECK(err);

            create_object(err, docid, oid);
            MX_ERROR_CHECK(err);

            mx_db_cc_create_blob(err, docid, oid, name);
            MX_ERROR_CHECK(err);

            mx_db_cc_set_blob_size(err, docid, oid, name, length);
            MX_ERROR_CHECK(err);

            break;

        case mx_sfile_blob_seg_e:

            {
                auto data = file->next_blob_segment(err, oid, segnum, name, length);
                MX_ERROR_CHECK(err);

                create_object(err, docid, oid);
                MX_ERROR_CHECK(err);

                mx_db_cc_set_blob_data(err, docid, oid, name, 0, length, data.get());
                MX_ERROR_CHECK(err);
            }

            break;

        case mx_sfile_none_e:

            return;
        }
    }
abort:;
}

void mx_db_cc_open_doc(int& err, uint32 docid, const char* file_name, bool recover, mx_file_create_type_t t)
{
    std::shared_ptr<mx_file> doc_file = std::make_shared<mx_file>();

    m_files[docid] = doc_file;
    MX_ERROR_CHECK(err);

    doc_file->open(err, file_name);

    if (err == MX_FILE_ENOENT) {
        MX_ERROR_CLEAR(err);
    } else {
        MX_ERROR_CHECK(err);

        // read the attributes etc
        read_file(err, docid, doc_file);
        MX_ERROR_CHECK(err);
    }

abort:;
}

static void mx_db_cc_commit_to_file(int& err, std::shared_ptr<mx_file> &file, int docid)
{
    printf("enter mx_db_cc_commit_to_file\n");
    file->start_transaction(err);
    MX_ERROR_CHECK(err);

    if (m_docs.find(docid) == m_docs.end()) {
        MX_ERROR_THROW(err, MX_DB_CLIENT_CACHE_NO_SUCH_DOC);
    }

    for (auto i : m_docs[docid]) {

        auto o = i.second;

        // make sure the attributes are all up to date
        if (o->obj != nullptr) {
            o->obj->cc_serialise_attributes(err);
            MX_ERROR_CHECK(err);
        }

        for (auto ai : o->attrs) {
            file->write_attribute(err, o->id, ai.second.get());
            MX_ERROR_CHECK(err);
        }

        for (auto bi : o->blobs) {

            auto b = bi.second;

            file->write_blob(err, o->id, bi.first.c_str(), b->size);
            MX_ERROR_CHECK(err);

            file->write_blob_segment(err, o->id, 0, b->size, bi.first.c_str(), b->data.get());
            MX_ERROR_CHECK(err);
        }

        if (o->obj) {
            printf("calling set_mem_state in mem for doc %d obj %d\n", docid, o->id);
            o->obj->set_mem_state(mx_in_mem_e);
        }
    }

    file->commit(err);
    MX_ERROR_CHECK(err);

abort:;
    printf("return mx_db_cc_commit_to_file\n");
}

void mx_db_cc_commit(int& err, uint32 docid)
{
    if (m_files.find(docid) == m_files.end()) {
        MX_ERROR_THROW(err, MX_DB_CLIENT_CACHE_NO_SUCH_DOC);
    }

    mx_db_cc_commit_to_file(err, m_files[docid], docid);
    MX_ERROR_CHECK(err);

abort:;
}

static void delete_cache(int& err, int docid)
{
    if (m_docs.find(docid) == m_docs.end()) {
        MX_ERROR_THROW(err, MX_DB_CLIENT_CACHE_NO_SUCH_DOC);
    }

    m_docs.erase(docid);

abort:;
}

void mx_db_cc_close_doc(int& err, uint32 docid)
{
    std::shared_ptr<mx_file> file;
    mx_document* doc;
    bool need_to_delete;
    char filename[MAX_PATH_LEN + MAX_FILE_NAME_LEN + 1];

    if (m_docs.find(docid) == m_docs.end()) {
        MX_ERROR_THROW(err, MX_DB_CLIENT_CACHE_NO_SUCH_DOC);
    } else {
        file = m_files[docid];
    }

    doc = (mx_document*)mx_db_cc_get_obj_from_oid(err, docid, MX_DB_OBJECT_DOCUMENT_ID);
    MX_ERROR_CHECK(err);

    MX_ERROR_ASSERT(err, (doc->is_a(mx_document_class_e) || doc->is_a(mx_wp_document_class_e)));

    need_to_delete = doc->get_temp_flag();
    strcpy(filename, doc->get_file_name());

    file->close(err);
    MX_ERROR_CHECK(err);

    m_files.erase(docid);

    delete_cache(err, docid);
    MX_ERROR_CHECK(err);

    if (need_to_delete) {
        if (-1 == unlink(filename)) {
            MX_ERROR_THROW(err, mx_translate_file_error(errno));
        }
    }

abort:;
}

void mx_db_cc_add_known_oid(int& err, uint32 docid, mx_db_object* o, uint32 oid)
{
    if (m_docs.find(docid) == m_docs.end()) {
        m_docs[docid] = std::map<uint32, std::shared_ptr<obj_cache_t>>();
    }

    if (m_docs[docid].find(oid) == m_docs[docid].end()) {
        std::shared_ptr<obj_cache_t> item =  std::make_shared<obj_cache_t>();

        item->obj = o;
        item->id = oid;

        m_docs[docid][oid] = item;
    } else {
        m_docs[docid][oid]->obj = o;
    }
}

const char* mx_db_cc_get_description(uint32 docid)
{
    if (m_docs.find(docid) == m_docs.end()) {
        return "<none set>";
    } else {
        return m_files[docid]->get_description();
    }
}

const char* mx_db_cc_get_author(uint32 docid)
{
    if (m_docs.find(docid) == m_docs.end()) {
        return "<none set>";
    } else {
        return m_files[docid]->get_author();
    }
}

const char* mx_db_cc_get_version(uint32 docid)
{
    if (m_docs.find(docid) == m_docs.end()) {
        return "<none set>";
    } else {
        return m_files[docid]->get_version();
    }
}

void mx_db_cc_set_info(int& err, uint32 docid, char* description, char* author, char* version)
{
    if (m_files.find(docid) == m_files.end()) {
        MX_ERROR_THROW(err, MX_DB_CLIENT_CACHE_NO_SUCH_DOC);
    } else {
        m_files[docid]->set_info(err, description, author, version);
        MX_ERROR_CHECK(err);
    }

abort:;
}

bool mx_db_cc_doc_modified(int& err, uint32 docid)
{
    if (m_docs.find(docid) == m_docs.end()) {
        MX_ERROR_THROW(err, MX_DB_CLIENT_CACHE_NO_SUCH_DOC);
    }

    for (auto i : m_docs[docid]) {
        if ((i.second->obj != nullptr) && (i.second->obj->get_mem_state() == mx_in_mem_and_modified_e)) {
            return true;
        }
    }

abort:
    return false;
}

void mx_db_cc_commit_as(int& err, uint32 docid, char* file_name)
{
    std::shared_ptr<mx_file> file;

    if (m_files.find(docid) == m_files.end()) {
        MX_ERROR_THROW(err, MX_DB_CLIENT_CACHE_NO_SUCH_DOC);
    } else {
        file = m_files[docid];
    }

    file->duplicate(err, file_name);
    MX_ERROR_CHECK(err);

    mx_db_cc_commit_to_file(err, file, docid);
    MX_ERROR_CHECK(err);

abort:;
}

static void mx_db_cc_copy_attrs(int& err, std::map<std::string, std::shared_ptr<mx_attribute>> &attrs, int dest_doc, int dest_oid)
{
    for (auto i : attrs) {
        mx_db_cc_set_attribute(err, dest_doc, dest_oid, i.second.get());
        MX_ERROR_CHECK(err);
    }

abort:;
}

static void mx_db_cc_copy_blobs(int& err, std::map<std::string, std::shared_ptr<blob_cache_t>> &blobs, int source_doc, int source_oid, int dest_doc, int dest_oid)
{
    for (auto bi : blobs) {
        auto b = bi.second;
        auto name = bi.first;

        mx_db_cc_create_blob(err, dest_doc, dest_oid, name.c_str());
        MX_ERROR_CHECK(err);

        mx_db_cc_set_blob_size(err, dest_doc, dest_oid, name.c_str(), b->size);
        MX_ERROR_CHECK(err);

        std::unique_ptr<unsigned char []> buffer(new unsigned char[b->size]);

        mx_db_cc_get_blob_data(err, source_doc, source_oid, name.c_str(), 0, b->size, buffer.get());
        MX_ERROR_CHECK(err);

        mx_db_cc_set_blob_data(err, dest_doc, dest_oid, name.c_str(), 0, b->size, buffer.get());
        MX_ERROR_CHECK(err);
    }
abort:;
}

void mx_db_cc_move_object(int& err, mx_db_object* o, int source_doc, int dest_doc, int& oid)
{
    int new_oid;

    auto item = get_obj(err, source_doc, oid);
    MX_ERROR_CHECK(err);

    new_oid = mx_db_cc_add_oid(err, dest_doc, o);
    MX_ERROR_CHECK(err);

    mx_db_cc_copy_attrs(err, item->attrs, dest_doc, new_oid);
    MX_ERROR_CHECK(err);

    mx_db_cc_copy_blobs(err, item->blobs, source_doc, oid, dest_doc, new_oid);
    MX_ERROR_CHECK(err);

    mx_db_cc_delete_oid_from_cache(err, source_doc, oid);
    MX_ERROR_CHECK(err);

    oid = new_oid;
abort:;
}
