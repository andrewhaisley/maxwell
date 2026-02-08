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
 * MODULE/CLASS : mx_serialisable_object
 *
 * AUTHOR : Andrew Haisley
 *
 *
 *
 * DESCRIPTION:
 *
 *   Base class for all serialisable objects
 *
 *
 *
 */
#ifndef MX_SERIAL_OBJECT_H
#define MX_SERIAL_OBJECT_H

#include <mx.h>

/* sizes in bytes for serialised things */
#define SLS_INT32 4
#define SLS_UINT32 4
#define SLS_INT16 2
#define SLS_INT8 1
#define SLS_UINT16 2
#define SLS_UINT8 1
#define SLS_BOOL 1
#define SLS_ENUM 1
#define SLS_FLOAT 4
#define SLS_DOUBLE 8
#define SLS_STRING(s) ((s) == NULL ? 1 : (strlen(s) + 1))
#define SLS_INT32_A(n) (4 * n)
#define SLS_UINT32_A(n) (4 * n)
#define SLS_INT8_A(n) (n)
#define SLS_UINT8_A(n) (n)
#define SLS_FLOAT_A(n) (4 * n)
#define SLS_DOUBLE_A(n) (8 * n)

#define MX_SL_OBJ_TEMP_BUFFER_SIZE 8192

void serialise_int32(int32 i, uint8** buffer);
void serialise_uint32(uint32 i, uint8** buffer);

void serialise_int16(int16 i, uint8** buffer);
void serialise_uint16(uint16 i, uint8** buffer);

void serialise_int8(int8 i, uint8** buffer);
void serialise_uint8(uint8 i, uint8** buffer);

void serialise_float(float i, uint8** buffer);
void serialise_double(double d, uint8** buffer);

void serialise_bool(bool b, uint8** buffer);
void serialise_string(const char* s, uint8** buffer);
void serialise_enum(uint16 i, uint8** buffer);

void unserialise_int32(int32& i, uint8** buffer);
void unserialise_uint32(uint32& i, uint8** buffer);

void unserialise_int16(int16& i, uint8** buffer);
void unserialise_uint16(uint16& i, uint8** buffer);

void unserialise_int8(int8& i, uint8** buffer);
void unserialise_uint8(uint8& i, uint8** buffer);

void unserialise_float(float& i, uint8** buffer);
void unserialise_double(double& d, uint8** buffer);

void unserialise_bool(bool& b, uint8** buffer);
void unserialise_string(char* s, uint8** buffer);
void unserialise_enum(uint16& i, uint8** buffer);

// arrays where the size is known
void serialise_int32_a(int32* i, int n, uint8** buffer);
void unserialise_int32_a(int32* i, int n, uint8** buffer);

void serialise_uint16_a(uint16* i, int n, uint8** buffer);
void unserialise_uint16_a(uint16* i, int n, uint8** buffer);

void serialise_int8_a(int8* i, int n, uint8** buffer);
void unserialise_int8_a(int8* i, int n, uint8** buffer);

void serialise_char_a(const char* i, int n, uint8** buffer);
void unserialise_char_a(char* i, int n, uint8** buffer);

void serialise_float_a(float* f, int n, uint8** buffer);
void unserialise_float_a(float* f, int n, uint8** buffer);

class mx_serialisable_object : public mx_rtti {
    MX_RTTI(mx_serialisable_object_class_e)

public:
    // objects can use this for temporary store during serialisation
    static uint8 temp_buffer[MX_SL_OBJ_TEMP_BUFFER_SIZE];

    // there are two categories of serialisable object. Those that are
    // db_ojects and those that are not. Look in mx_db_object for the
    // extra routines needed by those classes.
    // all objects have these routines:
    virtual void serialise(int& err, uint8** buffer) { err = MX_ERROR_OK; };
    virtual void unserialise(int& err, uint8** buffer) { err = MX_ERROR_OK; };
    virtual uint32 get_serialised_size(int& err)
    {
        err = MX_ERROR_OK;
        return 0;
    };

    // now, a load of routines to serialise standard things
    void serialise_int32(int32 i, uint8** buffer);
    void serialise_uint32(uint32 i, uint8** buffer);
    void serialise_int16(int16 i, uint8** buffer);
    void serialise_uint16(uint16 i, uint8** buffer);
    void serialise_int8(int8 i, uint8** buffer);
    void serialise_char(char c, uint8** buffer);
    void serialise_uint8(uint8 i, uint8** buffer);
    void serialise_float(float i, uint8** buffer);
    void serialise_double(double d, uint8** buffer);
    void serialise_bool(bool b, uint8** buffer);
    void serialise_string(const char*, uint8** buffer);
    void serialise_enum(uint16 i, uint8** buffer);

    void unserialise_int32(int32& i, uint8** buffer);
    void unserialise_uint32(uint32& i, uint8** buffer);
    void unserialise_int16(int16& i, uint8** buffer);
    void unserialise_uint16(uint16& i, uint8** buffer);
    void unserialise_int8(int8& i, uint8** buffer);
    void unserialise_char(char& c, uint8** buffer)
    {
        c = **buffer;
        (*buffer)++;
    };
    void unserialise_uint8(uint8& i, uint8** buffer);
    void unserialise_float(float& i, uint8** buffer);
    void unserialise_double(double& d, uint8** buffer);
    void unserialise_bool(bool& b, uint8** buffer);
    void unserialise_string(char* s, uint8** buffer);
    void unserialise_enum(uint16& i, uint8** buffer);

    // arrays where the size is known
    void serialise_int32_a(int32* i, int n, uint8** buffer);
    void unserialise_int32_a(int32* i, int n, uint8** buffer);
    void serialise_uint16_a(uint16* i, int n, uint8** buffer);
    void unserialise_uint16_a(uint16* i, int n, uint8** buffer);
    void serialise_int8_a(int8* i, int n, uint8** buffer);
    void unserialise_int8_a(int8* i, int n, uint8** buffer);
    void serialise_float_a(float* f, int n, uint8** buffer);
    void unserialise_float_a(float* f, int n, uint8** buffer);
};

inline void mx_serialisable_object::serialise_int32(int32 i, uint8** buffer)
{
    ::serialise_int32(i, buffer);
}

inline void mx_serialisable_object::serialise_uint32(uint32 i, uint8** buffer)
{
    ::serialise_uint32(i, buffer);
}

inline void mx_serialisable_object::serialise_int16(int16 i, uint8** buffer)
{
    ::serialise_int16(i, buffer);
}

inline void mx_serialisable_object::serialise_uint16(uint16 i, uint8** buffer)
{
    ::serialise_uint16(i, buffer);
}

inline void mx_serialisable_object::serialise_enum(uint16 i, uint8** buffer)
{
    ::serialise_enum(i, buffer);
}

inline void mx_serialisable_object::serialise_int8(int8 i, uint8** buffer)
{
    ::serialise_int8(i, buffer);
}

inline void mx_serialisable_object::serialise_uint8(uint8 i, uint8** buffer)
{
    ::serialise_uint8(i, buffer);
}

inline void mx_serialisable_object::serialise_float(float i, uint8** buffer)
{
    ::serialise_float(i, buffer);
}

inline void mx_serialisable_object::serialise_double(double d, uint8** buffer)
{
    ::serialise_double(d, buffer);
}

inline void mx_serialisable_object::serialise_bool(bool b, uint8** buffer)
{
    ::serialise_bool(b, buffer);
}

// inline void mx_serialisable_object::serialise_string(const char *s, uint8 **buffer)
//{
//::serialise_char_a(s, strlen(s), buffer);
//}

inline void mx_serialisable_object::serialise_string(const char* s, uint8** buffer)
{
    ::serialise_string(s, buffer);
}

inline void mx_serialisable_object::unserialise_int32(int32& i, uint8** buffer)
{
    ::unserialise_int32(i, buffer);
}

inline void mx_serialisable_object::unserialise_uint32(uint32& i, uint8** buffer)
{
    ::unserialise_uint32(i, buffer);
}

inline void mx_serialisable_object::unserialise_int16(int16& i, uint8** buffer)
{
    ::unserialise_int16(i, buffer);
}

inline void mx_serialisable_object::unserialise_uint16(uint16& i, uint8** buffer)
{
    ::unserialise_uint16(i, buffer);
}

inline void mx_serialisable_object::unserialise_enum(uint16& i, uint8** buffer)
{
    ::unserialise_enum(i, buffer);
}

inline void mx_serialisable_object::unserialise_int8(int8& i, uint8** buffer)
{
    ::unserialise_int8(i, buffer);
}

inline void mx_serialisable_object::unserialise_uint8(uint8& i, uint8** buffer)
{
    ::unserialise_uint8(i, buffer);
}

inline void mx_serialisable_object::unserialise_float(float& i, uint8** buffer)
{
    ::unserialise_float(i, buffer);
}

inline void mx_serialisable_object::unserialise_double(double& d, uint8** buffer)
{
    ::unserialise_double(d, buffer);
}

inline void mx_serialisable_object::unserialise_bool(bool& b, uint8** buffer)
{
    ::unserialise_bool(b, buffer);
}

inline void mx_serialisable_object::unserialise_string(char* s, uint8** buffer)
{
    ::unserialise_string(s, buffer);
}

inline void mx_serialisable_object::serialise_int32_a(int32* i, int n, uint8** buffer)
{
    ::serialise_int32_a(i, n, buffer);
}

inline void mx_serialisable_object::unserialise_int32_a(int32* i, int n, uint8** buffer)
{
    ::unserialise_int32_a(i, n, buffer);
}

inline void mx_serialisable_object::serialise_uint16_a(uint16* i, int n, uint8** buffer)
{
    ::serialise_uint16_a(i, n, buffer);
}

inline void mx_serialisable_object::unserialise_uint16_a(uint16* i, int n, uint8** buffer)
{
    ::unserialise_uint16_a(i, n, buffer);
}

inline void mx_serialisable_object::serialise_float_a(float* i, int n, uint8** buffer)
{
    ::serialise_float_a(i, n, buffer);
}

inline void mx_serialisable_object::unserialise_float_a(float* i, int n, uint8** buffer)
{
    ::unserialise_float_a(i, n, buffer);
}

inline void mx_serialisable_object::serialise_int8_a(int8* i, int n, uint8** buffer)
{
    ::serialise_int8_a(i, n, buffer);
}

inline void mx_serialisable_object::unserialise_int8_a(int8* i, int n, uint8** buffer)
{
    ::unserialise_int8_a(i, n, buffer);
}

#endif
