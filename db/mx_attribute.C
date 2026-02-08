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

#include <mx_attribute.h>
#include <mx_file.h>

/*-------------------------------------------------
 * FUNCTION: mx_attribute_free
 *
 * DESCRIPTION: Delete the data on an attribute
 *
 *
 */

void mx_attribute_free(mx_attribute_type type,
    mx_attribute_value& value)
{
    switch (type) {
    case mx_char_array:
        if (value.ca != NULL) {
            delete[] value.ca;
        }
        break;
    case mx_int_array:
        if (value.ia != NULL) {
            delete[] value.ia;
        }
        break;
    case mx_id_array:
        if (value.ida != NULL) {
            delete[] value.ida;
        }
        break;
    case mx_float_array:
        if (value.fa != NULL) {
            delete[] value.fa;
        }
        break;
    case mx_string:
        if (value.s != NULL) {
            delete[] value.s;
        }
    case mx_unknown:
    default:
        break;
    }
}

/*-------------------------------------------------
 * FUNCTION: mx_attribute_size
 *
 * DESCRIPTION: Delete the data on an attribute
 *
 *
 */

uint32 mx_attribute_size(mx_attribute_type type,
    int length,
    mx_attribute_value& val)
{
    uint32 res;

    switch (type) {
    case mx_int:
        res = sizeof(int32);
        break;
    case mx_float:
        res = sizeof(float);
        break;
    case mx_id:
        res = sizeof(int32);
        break;
    case mx_char_array:
        res = length;
        break;
    case mx_int_array:
        res = length * sizeof(int32);
        break;
    case mx_float_array:
        res = length * sizeof(float);
        break;
    case mx_id_array:
        res = length * sizeof(int32);
        break;
    case mx_string:
        if (val.s == NULL) {
            res = 0;
        } else {
            res = strlen(val.s) + 1;
        }
        break;
    case mx_unknown:
    default:
        res = 0;
        break;
    }

    return res;
}

/*-------------------------------------------------
 * FUNCTION: mx_attribute_copy
 *
 * DESCRIPTION: Copy the data on an attribute
 *
 *
 */

void mx_attribute_copy(int& err,
    mx_attribute_type type,
    int length,
    mx_attribute_value& newval,
    mx_attribute_value& oldval)
{
    // now, the rest
    switch (type) {
    case mx_char_array:
        newval.ca = new char[length];
        memcpy(newval.ca, oldval.ca, length);
        break;
    case mx_int_array:
        newval.ia = new int32[length];
        memcpy(newval.ia, oldval.ia, length * sizeof(int32));
        break;
    case mx_id_array:
        newval.ida = new uint32[length];
        memcpy(newval.ida, oldval.ida, length * sizeof(int32));
        break;
    case mx_float_array:
        newval.fa = new float[length];
        memcpy(newval.fa, oldval.fa, length * sizeof(float));
        break;
    case mx_string:
        if (oldval.s == NULL) {
            newval.s = NULL;
        } else {
            newval.s = new char[strlen(oldval.s) + 1];
            strcpy(const_cast<char*>(newval.s), oldval.s);
        }
        break;
    case mx_unknown:
    default:
        newval = oldval;
        break;
    }
}

/*-------------------------------------------------
 * FUNCTION: mx_attribute_cmp
 *
 * DESCRIPTION: Compare two mx_attributes
 *
 *
 */

int mx_attribute_cmp(int& err,
    mx_attribute_type type,
    mx_attribute_value& key1,
    mx_attribute_value& key2)
{
    err = 0;

    switch (type) {
    case mx_int:

        if (key2.i == key1.i) {
            return 0;
        } else if (key2.i > key1.i) {
            return 1;
        } else {
            return -1;
        }
        break;
    case mx_id:
        if (key2.id == key1.id) {
            return 0;
        } else if (key2.id > key1.id) {
            return 1;
        } else {
            return -1;
        }
        break;
    case mx_float:
        if (key2.f == key1.f) {
            return 0;
        } else if (key2.f > key1.f) {
            return 1;
        } else {
            return -1;
        }
        break;
    case mx_string:
        return strcmp(key2.s, key1.s);
        break;
    case mx_unknown:
    default:
        return 0;
        break;
    }
}

void mx_attribute::init_value()
{
    value.i = 0;
    value.f = 0.0;
    value.id = 0;
    value.ia = NULL;
    value.ca = NULL;
    value.fa = NULL;
    value.ida = NULL;
    value.s = NULL;
}

mx_attribute::~mx_attribute()
{
}

mx_attribute::mx_attribute()
{
    this->type = mx_int;
    strcpy(this->name, "no name");
    this->length = 0;
}

mx_attribute::mx_attribute(const char* name, mx_attribute_type type)
{
    this->type = type;
    init_value();
    strcpy(this->name, name);
    this->length = 0;
}

mx_attribute::mx_attribute(const char* name, mx_attribute_type type, mx_attribute_value v, int length)
{
    int err;

    this->type = type;
    strcpy(this->name, name);
    this->length = length;
    this->value = this->copy_value(err, v, length);
}

void mx_attribute::operator=(const mx_attribute& old)
{
    int err = MX_ERROR_OK;

    strcpy(name, old.name);
    type = old.type;
    length = old.length;

    value = copy_value(err, old.value, old.length);
    MX_ERROR_CLEAR(err);
}

mx_attribute_value mx_attribute::copy_value(int& err, mx_attribute_value v, int length)
{
    mx_attribute_value res;

    err = MX_ERROR_OK;

    mx_attribute_copy(err, type, length, res, v);
    MX_ERROR_CHECK(err);

abort:
    return res;
}

void mx_attribute::free_value()
{
    mx_attribute_free(type, value);
}

void mx_attribute::set_name(int& err, char* name)
{
    if (strlen(name) > MX_ATTRIBUTE_NAME_LENGTH) {
        err = MX_DB_ATTRIBUTE_NAME_TOO_LONG;
    } else {
        err = MX_ERROR_OK;
        strcpy(this->name, name);
    }
}

void mx_attribute::set_value(int& err, mx_attribute_value value, mx_attribute_type type, int length)
{
    free_value();

    this->type = type;
    if ((type == mx_int_array) || (type == mx_id_array) || (type == mx_float_array)) {
        if ((length * 4) > MX_ATTRIBUTE_MAX_SIZE) {
            err = MX_DB_ATTRIBUTE_TOO_BIG;
            return;
        }
    } else {
        if (type == mx_char_array) {
            if (length > MX_ATTRIBUTE_MAX_SIZE) {
                err = MX_DB_ATTRIBUTE_TOO_BIG;
                return;
            }
        } else {
            if (type == mx_string) {
                if (strlen(value.s) > MX_ATTRIBUTE_MAX_SIZE) {
                    err = MX_DB_ATTRIBUTE_TOO_BIG;
                    return;
                }
            }
        }
    }

    this->length = length;
    this->value = this->copy_value(err, value, length);
    MX_ERROR_CHECK(err);
    return;
abort:
    return;
}

void mx_attribute::serialise(int& err, uint8** buf)
{
    uint8 c;

    serialise_string(name, buf);
    serialise_int32(length, buf);
    c = (uint8)type;
    serialise_uint8(c, buf);

    switch (type) {
    case mx_int:
        serialise_int32(value.i, buf);
        break;
    case mx_id:
        serialise_uint32(value.id, buf);
        break;
    case mx_float:
        serialise_float(value.f, buf);
        break;
    case mx_char_array:
        serialise_char_a(value.ca, length, buf);
        break;
    case mx_int_array:
        serialise_int32_a(value.ia, length, buf);
        break;
    case mx_id_array:
        serialise_int32_a((int32*)value.ida, length, buf);
        break;
    case mx_float_array:
        serialise_float_a(value.fa, length, buf);
        break;
    case mx_string:
        serialise_string(value.s, buf);
        break;
    case mx_unknown:
        break;
    default:
        err = MX_ATTRIBUTE_BAD_TYPE;
        return;
    }
    err = MX_ERROR_OK;
}

void mx_attribute::unserialise(int& err, uint8** buf)
{
    uint8 c;

    unserialise_string(name, buf);
    unserialise_uint32(length, buf);
    unserialise_uint8(c, buf);
    type = (mx_attribute_type)c;

    switch (type) {
    case mx_int:
        unserialise_int32(value.i, buf);
        break;
    case mx_id:
        unserialise_uint32(value.id, buf);
        break;
    case mx_float:
        unserialise_float(value.f, buf);
        break;
    case mx_char_array:
        value.ca = new char[length + 1];
        unserialise_char_a(value.ca, length, buf);
        break;
    case mx_int_array:
        value.ia = new int32[length];
        unserialise_int32_a(value.ia, length, buf);
        break;
    case mx_id_array:
        value.ida = new uint32[length];
        unserialise_int32_a((int32*)value.ida, length, buf);
        break;
    case mx_float_array:
        value.fa = new float[length];
        unserialise_float_a(value.fa, length, buf);
        break;
    case mx_string:
        value.s = new char[length + 1];
        unserialise_string(const_cast<char*>(value.s), buf);
        break;
    case mx_unknown:
        break;
    default:
        err = MX_ATTRIBUTE_BAD_TYPE;
        return;
    }
}

uint32 mx_attribute::get_serialised_size(int& err)
{
    int res;

    res = SLS_STRING(name) + SLS_INT32 + SLS_INT8;

    switch (type) {
    case mx_int:
        res += SLS_INT32;
        break;
    case mx_id:
        res += SLS_UINT32;
        break;
    case mx_float:
        res += SLS_FLOAT;
        break;
    case mx_char_array:
        res += SLS_INT8_A(length);
        break;
    case mx_int_array:
        res += SLS_INT32_A(length);
        break;
    case mx_id_array:
        res += SLS_UINT32_A(length);
        break;
    case mx_float_array:
        res += SLS_FLOAT_A(length);
        break;
    case mx_string:
        res += SLS_STRING(value.s);
        break;
    case mx_unknown:
        break;
    default:
        err = MX_ATTRIBUTE_BAD_TYPE;
        return 0;
    }
    err = MX_ERROR_OK;
    return res;
}

uint32 mx_attribute::size(int& err)
{
    err = MX_ERROR_OK;

    switch (type) {
    case mx_int:
        return SLS_INT32;
    case mx_id:
        return SLS_UINT32;
    case mx_float:
        return SLS_FLOAT;
    case mx_char_array:
        return length;
    case mx_int_array:
        return length * SLS_INT32;
    case mx_id_array:
        return length * SLS_UINT32;
    case mx_float_array:
        return length * SLS_FLOAT;
    case mx_string:
        return SLS_STRING(value.s);
    case mx_unknown:
        return 0;
        break;
    default:
        err = MX_ATTRIBUTE_BAD_TYPE;
        return 0;
    }
}
