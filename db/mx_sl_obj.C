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
#include <mx.h>
#include <mx_sl_obj.h>
#include <netinet/in.h>

uint8 mx_serialisable_object::temp_buffer[MX_SL_OBJ_TEMP_BUFFER_SIZE];

void serialise_int32(int32 i, uint8** buffer)
{
    unsigned long l;

    l = htonl(i);
    memcpy(*buffer, &l, 4);
    (*buffer) += 4;
}

void serialise_enum(uint16 i, uint8** buffer)
{
    uint8 e;

    e = (uint8)i;
    memcpy(*buffer, &e, 1);
    (*buffer)++;
}

void unserialise_enum(uint16& i, uint8** buffer)
{
    uint8 e;

    memcpy(&e, *buffer, 1);
    i = (uint16)e;
    (*buffer)++;
}

void serialise_uint32(uint32 i, uint8** buffer)
{
    unsigned long l;

    l = htonl(i);
    memcpy(*buffer, &l, 4);
    (*buffer) += 4;
}

void serialise_int16(int16 i, uint8** buffer)
{
    unsigned short l;

    l = htons(i);
    memcpy(*buffer, &l, 2);
    (*buffer) += 2;
}

void serialise_uint16(uint16 i, uint8** buffer)
{
    unsigned short l;

    l = htons(i);
    memcpy(*buffer, &l, 2);
    (*buffer) += 2;
}

void serialise_float(float i, uint8** buffer)
{
    unsigned long l1, l2;

    memcpy(&l1, &i, 4);
    l2 = htonl(l1);
    memcpy(*buffer, &l2, 4);
    (*buffer) += 4;
}

void serialise_double(double d, uint8** buffer)
{
    float f;

    if (sizeof(double) == sizeof(float)) {
        serialise_float(d, buffer);
    } else {
        unsigned char* pd = (unsigned char*)&d;

        memcpy(&f, pd, sizeof(float));
        serialise_float(f, buffer);
        memcpy(&f, pd + sizeof(float), sizeof(float));
        serialise_float(f, buffer);
    }
}

void serialise_bool(bool b, uint8** buffer)
{
    if (b) {
        **buffer = 1;
    } else {
        **buffer = 0;
    }
    (*buffer)++;
}

void serialise_string(const char* s, uint8** buffer)
{
    int i = 0;

    if (s == NULL) {
        **buffer = 0;
        (*buffer)++;
        return;
    }

    while (TRUE) {
        **buffer = s[i];
        (*buffer)++;

        if (s[i] == 0) {
            break;
        } else {
            i++;
        }
    }
}

void unserialise_int32(int32& i, uint8** buffer)
{
    unsigned long l;

    memcpy(&l, *buffer, 4);
    (*buffer) += 4;
    i = ntohl(l);
}

void unserialise_uint32(uint32& i, uint8** buffer)
{
    unsigned long l;

    memcpy(&l, *buffer, 4);
    (*buffer) += 4;
    i = ntohl(l);
}

void unserialise_int16(int16& i, uint8** buffer)
{
    unsigned short l;

    memcpy(&l, *buffer, 2);
    (*buffer) += 2;
    i = ntohs(l);
}

void unserialise_uint16(uint16& i, uint8** buffer)
{
    unsigned short l;

    memcpy(&l, *buffer, 2);
    (*buffer) += 2;
    i = ntohs(l);
}

void unserialise_float(float& i, uint8** buffer)
{
    unsigned long l1, l2;

    memcpy(&l1, *buffer, 4);
    (*buffer) += 4;
    l2 = ntohl(l1);
    memcpy(&i, &l2, 4);
}

void unserialise_double(double& d, uint8** buffer)
{
    unsigned long l1, l2;
    float f;

    if (sizeof(double) == sizeof(float)) {
        unserialise_float(f, buffer);
        d = f;
    } else {
        unsigned char* pd = (unsigned char*)&d;

        memcpy(&l1, *buffer, sizeof(float));
        (*buffer) += sizeof(float);
        l2 = ntohl(l1);
        memcpy(pd, &l2, sizeof(float));

        memcpy(&l1, *buffer, sizeof(float));
        (*buffer) += sizeof(float);
        l2 = ntohl(l1);
        memcpy(pd + sizeof(float), &l2, sizeof(float));
    }
}

void unserialise_bool(bool& b, uint8** buffer)
{
    b = **buffer;
    (*buffer)++;
}

void unserialise_string(char* s, uint8** buffer)
{
    int i = 0;

    while (TRUE) {
        s[i] = **buffer;
        (*buffer)++;
        if (s[i] == 0) {
            return;
        } else {
            i++;
        }
    }
}

void serialise_int32_a(int32* i, int n, uint8** buffer)
{
    int a;

    for (a = 0; a < n; a++) {
        serialise_int32(i[a], buffer);
    }
}

void unserialise_int32_a(int32* i, int n, uint8** buffer)
{
    int a;

    for (a = 0; a < n; a++) {
        unserialise_int32(i[a], buffer);
    }
}

void serialise_int8(int8 i, uint8** buffer)
{
    memcpy(*buffer, &i, 1);
    (*buffer)++;
}

void serialise_char(char i, uint8** buffer)
{
    memcpy(*buffer, &i, 1);
    (*buffer)++;
}

void unserialise_int8(int8& i, uint8** buffer)
{
    i = **buffer;
    (*buffer)++;
}

void unserialise_char(char& i, uint8** buffer)
{
    i = **buffer;
    (*buffer)++;
}

void serialise_uint8(uint8 i, uint8** buffer)
{
    memcpy(*buffer, &i, 1);
    (*buffer)++;
}

void unserialise_uint8(uint8& i, uint8** buffer)
{
    i = *((uint8*)*buffer);
    (*buffer)++;
}

void serialise_int8_a(int8* i, int n, uint8** buffer)
{
    memcpy(*buffer, i, n);
    (*buffer) += n;
}

void serialise_char_a(const char* i, int n, uint8** buffer)
{
    memcpy(*buffer, i, n);
    (*buffer) += n;
}

void unserialise_int8_a(int8* i, int n, uint8** buffer)
{
    memcpy(i, *buffer, n);
    (*buffer) += n;
}

void unserialise_char_a(char* i, int n, uint8** buffer)
{
    memcpy(i, *buffer, n);
    (*buffer) += n;
}

void serialise_float_a(float* f, int n, uint8** buffer)
{
    int a;

    for (a = 0; a < n; a++) {
        serialise_float(f[a], buffer);
    }
}

void unserialise_float_a(float* f, int n, uint8** buffer)
{
    int a;

    for (a = 0; a < n; a++) {
        unserialise_float(f[a], buffer);
    }
}

void serialise_uint16_a(uint16* i, int n, uint8** buffer)
{
    int a;

    for (a = 0; a < n; a++) {
        serialise_uint16(i[a], buffer);
    }
}

void unserialise_uint16_a(uint16* i, int n, uint8** buffer)
{
    int a;

    for (a = 0; a < n; a++) {
        unserialise_uint16(i[a], buffer);
    }
}
