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
#ifndef MX_ATTRIBUTE_H
#define MX_ATTRIBUTE_H
/*
 * MODULE/CLASS : mx_attribute
 *
 * AUTHOR : Andrew Haisley
 *
 *
 *
 * DESCRIPTION:
 *
 * mx_attribute : named attributes with values
 *
 *
 *
 */

#include <mx.h>
#include <mx_sl_obj.h>

#define MX_ATTRIBUTE_NAME_LENGTH 28

// types of attribute
typedef enum { mx_int = 1,
    mx_float,
    mx_id, // object id
    mx_char_array,
    mx_int_array,
    mx_float_array,
    mx_id_array,
    mx_string,
    mx_data,
    mx_unknown
} mx_attribute_type;

typedef union {
    int32 i;
    float f;
    uint32 id;
    char* ca;
    int32* ia;
    float* fa;
    uint32* ida;
    void* data;
    const char* s;
} mx_attribute_value;

// maximum size of an array attribute (blobs may, of course, be any size.
// this maximum refers to the size of the value only
#define MX_ATTRIBUTE_MAX_SIZE 65536

#define MX_ATTRIBUTE_MAX_ID_ARRAY_LENGTH (MX_ATTRIBUTE_MAX_SIZE / 4)
#define MX_ATTRIBUTE_MAX_INT_ARRAY_LENGTH (MX_ATTRIBUTE_MAX_SIZE / 4)
#define MX_ATTRIBUTE_MAX_FLOAT_ARRAY_LENGTH (MX_ATTRIBUTE_MAX_SIZE / 4)

class mx_attribute : mx_serialisable_object {

    MX_RTTI(mx_attribute_class_e)

public:
    char name[MX_ATTRIBUTE_NAME_LENGTH];
    mx_attribute_type type;
    mx_attribute_value value;

    // length is number of items, not number of bytes.
    // only non-zero for arrays
    uint32 length;

    // size in bytes of whole attribute
    uint32 size(int& err);

    // no name or type
    mx_attribute();

    virtual ~mx_attribute();

    // no initial value
    mx_attribute(const char* name, mx_attribute_type type);

    // initial value
    mx_attribute(const char* name, mx_attribute_type type, mx_attribute_value v, int length);

    // copy function
    void operator=(const mx_attribute& old);

    void set_name(int& err, char* name);

    void set_value(int& err, mx_attribute_value v, mx_attribute_type type, int length);
    void free_value();

    virtual void serialise(int& err, uint8** buf);
    virtual void unserialise(int& err, uint8** buf);
    virtual uint32 get_serialised_size(int& err);

private:
    mx_attribute_value copy_value(int& err, mx_attribute_value v, int length);

    void init_value();
};

/*-------------------------------------------------
 * FUNCTION: mx_attribute_size
 *
 */

uint32 mx_attribute_size(mx_attribute_type type,
    int length,
    mx_attribute_value& val);

/*-------------------------------------------------
 * FUNCTION: mx_attribute_free
 */

void mx_attribute_free(mx_attribute_type type,
    mx_attribute_value& val);

/*-------------------------------------------------
 * FUNCTION: mx_attribute_copy
 *
 */

void mx_attribute_copy(int& err,
    mx_attribute_type type,
    int arraySize,
    mx_attribute_value& newval,
    mx_attribute_value& oldval);

/*-------------------------------------------------
 * FUNCTION: mx_attribute_cmp
 *
 *
 */
int mx_attribute_cmp(int& err,
    mx_attribute_type type,
    mx_attribute_value& key1,
    mx_attribute_value& key2);

#endif
