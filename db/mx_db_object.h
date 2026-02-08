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
 * DESCRIPTION:
 *
 * mx_db_object provides database access functions for documents
 * and all objects contained within documents.
 *
 * NOTES:
 *
 * Things are arranged so that you only need worry about your local
 * memory management. Any objects/values that you pass into this
 * object get copied (if necessary) and any that come out are
 * stand-alone copies. As long as you delete your own objects
 * and any that you get back as a result of calling methods on this
 * object you'll be fine.
 *
 * There are also methods that return read-only copies of things -
 * you should use these when accessing large attributes that
 * you don't want to modify (e.g. drawing a raster) as they don't
 * do any copying.
 *
 *
 *
 */
#ifndef MX_DB_OBJECT_H
#define MX_DB_OBJECT_H

#include <mx.h>
#include <mx_attribute.h>
#include <mx_file.h>
#include <mx_sl_obj.h>

#define MX_DB_OBJECT_MAX_READONLY_BLOB_SIZE 32768
#define MX_DB_OBJECT_OWNER_ID "ownerid"

typedef enum {
    mx_on_disk_e,
    mx_in_mem_e,
    mx_in_mem_and_modified_e
} mx_db_mem_state_t;

class mx_db_object : public mx_serialisable_object {
    MX_RTTI(mx_db_object_class_e)

public:
    // constructor
    mx_db_object(uint32 docid, bool is_controlled = FALSE);

    // constructor for when the oid is known
    mx_db_object(uint32 docid, uint32 oid, bool is_controlled = FALSE);

    // copy constructor - assigns new db id and resets serialisation flags
    // etc
    mx_db_object(const mx_db_object& o);

    int get_db_id(int& err);

    // destructor
    virtual ~mx_db_object();

    int error;

    // move an object into another document - needed by cutting and pasting
    // operation - this moves the underlying blobs and attributes and nothing
    // else. If you need to do anything more complex (which you probably won't)
    // you will have to override this.
    // before calling this, you must make sure that all the attributes and
    // blobs that form part of the object are in the cache (calling unserialise
    // attributes should do it).
    virtual void move_to_document(int& err, int docid);

    // these routines are needed by the cache - but are NOT
    // compulsory as such. You only need to worry about these
    // if your object holds a lot of stuff in memory rather than
    // reading it from the DB when it needs it

    // how much real memory are you using?
    virtual uint32 memory_size(int& err)
    {
        err = MX_ERROR_OK;
        return 0L;
    };

    // add a new attribute
    // this routine makes a copy of the attribute object.
    void add_attribute(int& err, mx_attribute* initial);

    // delete an attribute - use carefully! (don't delete
    // id and id_array attributes unless you are doing
    // it from within a method in document, sheet or area.
    void delete_attribute(int& err, const char* name);

    // set an attribute value
    // This makes a copy of the attribute so, if you change
    // the value in your local copy, call this again.
    void set(int& err, mx_attribute* v);

    // get an attribute : this returns a copy of the
    // attribute so make sure you delete it when
    // you've finished with it
    mx_attribute* get(int& err, const char* name);

    // get a read only attribute : this returns a
    // pointer to the object stored in the db client
    // - so you can't change it. This pointer is only
    // guaranteed to be valid until the next database
    // access.
    const mx_attribute* get_readonly(int& err, const char* name);

    // this gets called by the client cache when it wants
    // to commit the data. This method should serialise
    // you complex attributes and store them in a blob. It needn't
    // free any memory - you will be asked expliclity to do that
    // should the cache require it.
    virtual void serialise_attributes(int& err);

    // called to get the attributes in from disk
    virtual void unserialise_attributes(int& err);

    // free any memory you are using. The object will have already been
    // asked to serialise itself when this is called.
    virtual void uncache(int& err);

    // BLOB handling
    void create_blob(int& err, const char* name);
    void delete_blob(int& err, const char* name);

    uint32 get_blob_size(int& err, const char* name);
    void set_blob_size(int& err, const char* name, uint32 size);

    void set_blob_data(int& err, const char* name, uint32 start_index, uint32 n_bytes, uint8* data);

    void get_blob_data(int& err, const char* name, uint32 start_index, uint32 n_bytes, uint8* data);

    // convenience routine for copying blob data
    void copy_blob(int& err, bool createNew, const char* name, mx_db_object* inObject, const char* inname);

    // returns a read only pointer to the data for a blob
    // stored in the db client. This pointer is only guaranteed
    // to be valid until the next database access. Also, if
    // you ask to read too many bytes, the call may return an
    // error because it can't get them all into memory at once.
    //
    // maximum size is define above MX_DB_MAX_READONLY_BLOB_SIZE
    //
    const uint8* get_blob_readonly(int& err, const char* name, uint32 start_index, uint32 n_bytes);

    // some convenience functions
    void set_int(int& err, const char* name, int32 i);
    void set_id(int& err, const char* name, uint32 id);
    void set_float(int& err, const char* name, float i);
    void set_string(int& err, const char* name, const char* s);
    void set_int_array(int& err, const char* name, int32* a, int length);
    void set_id_array(int& err, const char* name, uint32* a, int length);
    void set_float_array(int& err, const char* name, float* a, int length);

    int32 get_int(int& err, const char* name);
    float get_float(int& err, const char* name);
    const char* get_string(int& err, const char* name);

    // if the attribute doesn't exist, get a default value
    int32 get_default_int(int& err, const char* name, int32 def);
    uint32 get_default_id(int& err, const char* name, uint32 def);
    float get_default_float(int& err, const char* name, float def);
    const char* get_default_string(int& err, const char* name, const char* def);

    // manipulating arrays of object ids
    uint32 get_id_array_item(int& err, const char* name, int32 index);
    void set_id_array_item(int& err, const char* name, int32 index, uint32 id);
    void add_id_array_item(int& err, const char* name, uint32 id);
    void del_id_array_item(int& err, const char* name, int32 index);
    int32 get_id_array_length(int& err, const char* name);
    const uint32* get_id_array_ro(int& err, const char* name);

    // manipulating arrays of ints
    int32 get_int_array_item(int& err, const char* name, int32 index);
    void set_int_array_item(int& err, const char* name, int32 index, int32 i);
    void add_int_array_item(int& err, const char* name, int32 i);
    void del_int_array_item(int& err, const char* name, int32 index);
    int32 get_int_array_length(int& err, const char* name);
    const int32* get_int_array_ro(int& err, const char* name);

    // manipulating arrays of floats
    float get_float_array_item(int& err, const char* name, int32 index);
    void set_float_array_item(int& err, const char* name, int32 index, float f);
    void add_float_array_item(int& err, const char* name, float f);
    void del_float_array_item(int& err, const char* name, int32 index);
    int32 get_float_array_length(int& err, const char* name);
    const float* get_float_array_ro(int& err, const char* name);

    // get owning object - may be null
    virtual mx_db_object* get_owner_object(int& err);

    // set owning object
    void set_owner_object(int& err, mx_db_object* o);

    // get document id
    int get_docid() const;

    // get the objects memory state
    virtual mx_db_mem_state_t get_mem_state();

    // set the objects memory state
    virtual void set_mem_state(mx_db_mem_state_t s);

    // is the object controlled by another one?
    bool is_controlled() const;

    // these should only be called by the cache
    virtual void cc_serialise_attributes(int& err);
    virtual void cc_unserialise_attributes(int& err);
    virtual void cc_uncache(int& err);

protected:
    uint32 db_id; // object id
    uint32 doc_id; // document id

private:
    mx_db_mem_state_t mem_state;
    bool controlled;
};

#endif
