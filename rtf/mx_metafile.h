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
 * MODULE : mx_metafile.h
 *
 * AUTHOR : David Miller
 *
 * This file
 *
 * DESCRIPTION:
 * Module mx_metafile.h
 *
 *
 * Revision 1.1  1996/10/06 13:15:45  andrew
 * Initial revision
 *
 *
 */

#ifndef MX_METAFILE_HEADER
#define MX_METAFILE_HEADER

#define MX_METAFILE_HEADER_WORD_SIZE 9
#define MX_METAFILE_WINDOWS_VERSION 0x0300
#define MX_METAFILE_HEADER_SIZE 36
#define MX_METAFILE_WORD_SIZE 4
#define MX_METAFILE_DWORD_SIZE 8

#include "mx_error.h"
#include "mx_geometry.h"

typedef enum {
    MX_METAFILE_Unhandled = 0xFFFF,
    MX_METAFILE_NullFunction = 0x0000,
    MX_METAFILE_SetWindowOrg = 0x020B,
    MX_METAFILE_SetWindowExt = 0x020C,
    MX_METAFILE_SetTextColor = 0x0209,
    MX_METAFILE_SetBkColor = 0x0201,
    MX_METAFILE_SetStretchBltMode = 0x0107,
    MX_METAFILE_StretchDIBits = 0x0F43,
    MX_METAFILE_SetMapMode = 0x0103
} MX_METAFILE_FUNCTION;

class mx_metafile_reader {
public:
    int readWord(int& err, char** buffer);
    int readDWord(int& err, char** buffer);
    virtual ~mx_metafile_reader() { };
};

class mx_metafile_colour : public mx_metafile_reader {
public:
    void set(int& err, char** buffer);
    void set(int& err, int* params);

    int red;
    int green;
    int blue;
};

class mx_bitmapinfo : public mx_metafile_reader {
public:
    void set(int& err, char** buffer);

    int size;
    int biWidth;
    int biHeight;
    int biPlanes;
    int biBitCount;
    int biCompression;
    int biSizeImage;
    int biXPelsPerMeter;
    int biYPelsPerMeter;
    int biClrUsed;
    int biClrImportant;
};

class mx_stretchDIBits : public mx_metafile_reader {
public:
    void set(int& err, char** buffer);

    int dwRop;
    int Usag;
    int srcYext;
    int srcXext;
    int srcY;
    int srcX;
    int dstYext;
    int dstXext;
    int dstY;
    int dstX;
    mx_bitmapinfo bitmapinfo;
    int ncolours;
    mx_metafile_colour DIBcolours[256];
    char* data;
};

class mx_metafile_header : public mx_metafile_reader {
public:
    void set(int& err, char** buffer);

    int mtType;
    int mtHeaderSize;
    int mtVersion;
    int mtSize;
    int mtNoObjects;
    int mtMaxRecord;
    int mtNoParameters;
};

class mx_standard_metafile_record : public mx_metafile_reader {
public:
    mx_standard_metafile_record();
    ~mx_standard_metafile_record();

    void setHeader(int& err, char** buffer);
    void setParams(int& err, char** buffer);

    void set(int isize, int ifunction,
        int inparams, int* iparams);

    int size;
    int function;
    MX_METAFILE_FUNCTION handledFunction;
    int nparams;
    int nallocParams;
    int* params;
};

class mx_metafile_output : public mx_metafile_reader {
public:
    mx_metafile_output();

    virtual bool handleRecord(mx_standard_metafile_record& record);

    void processStandardRecord(int& err,
        mx_standard_metafile_record& record);

    void processNonStandardRecord(int& err,
        mx_standard_metafile_record& record,
        char** buffer);

    virtual void processSetWindowOrg(int& err,
        mx_standard_metafile_record& record) { err = MX_ERROR_OK; };

    virtual void processSetMapMode(int& err,
        mx_standard_metafile_record& record) { err = MX_ERROR_OK; };

    virtual void processSetWindowExt(int& err,
        mx_standard_metafile_record& record) { err = MX_ERROR_OK; };
    virtual void processSetTextColor(int& err,
        mx_standard_metafile_record& record) { err = MX_ERROR_OK; };
    virtual void processSetBkColor(int& err,
        mx_standard_metafile_record& record) { err = MX_ERROR_OK; };
    virtual void processSetStretchBltMode(int& err,
        mx_standard_metafile_record& record) { err = MX_ERROR_OK; };
    virtual void processStretchDIBits(int& err,
        mx_standard_metafile_record& record,
        char** buffer) { err = MX_ERROR_OK; };

    inline void setSize(mx_point& isize) { size = isize; };
    inline void setScale(mx_point& iscale) { scale = iscale; };
    inline void setCrop(mx_rect& icrop) { crop = icrop; };

    mx_point size;
    mx_point scale;
    mx_rect crop;
};

class mx_metafile_input : public mx_metafile_reader {
public:
    int testRecordLength(int& err,
        int bufferWordSize,
        char* testBuffer);

    virtual void getHeaderData(int& err, char* buffer) = 0;
    virtual void getNextRecordData(int& err, int bufferWordSize, char* buffer) = 0;
    virtual ~mx_metafile_input() { };
};

class mx_metafile_buffer_input : public mx_metafile_input {
public:
    char* buffer;
    int bufferSize;
    int bufferOffset;
    mx_metafile_buffer_input(int& err, int ibufferSize, char* ibuffer);
    void getHeaderData(int& err, char* buffer);
    void getNextRecordData(int& err, int bufferWordSize, char* buffer);
    virtual ~mx_metafile_buffer_input() { };
};

class mx_metafile_file_input : public mx_metafile_input {
public:
    FILE* file;
    mx_metafile_file_input(int& err, char* fileName);
    void getHeaderData(int& err, char* buffer);
    void getNextRecordData(int& err, int bufferWordSize, char* buffer);
};

class mx_metafile {
public:
    mx_metafile(mx_metafile_input* iinput, mx_metafile_output* ioutput);
    virtual ~mx_metafile();

    mx_metafile_header header;
    mx_standard_metafile_record standardRecord;

    // default method is just to read the whole file
    // into memory
    char* buffer;
    int allocBufferSize;
    int bufferSize;

    bool isStandardRecord();

    mx_metafile_input* input;
    mx_metafile_output* output;

    void setBufferSize(int newSize);
    void process(int& err);
    bool processNextRecord(int& err);
};

#endif
