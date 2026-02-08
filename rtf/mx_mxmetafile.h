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
 * MODULE : mx_mxmetafile.h
 *
 * AUTHOR : David Miller
 *
 * This file
 *
 * DESCRIPTION:
 * Module mx_mxmetafile.h
 *
 *
 * Revision 1.1  1996/10/06 13:15:45  andrew
 * Initial revision
 *
 *
 */

#ifndef MX_MXMETAFILE_HEADER
#define MX_MXMETAFILE_HEADER

#include "mx_geometry.h"
#include "mx_metafile.h"
#include "mx_style.h"

class mx_image_area;
class raster;

class mx_mxmetafile_output : public mx_metafile_output {
public:
    void convertColour(mx_metafile_colour& mfColour,
        mx_colour& mxColour);

    void processSetMapMode(int& err,
        mx_standard_metafile_record& record);
    void processSetWindowOrg(int& err,
        mx_standard_metafile_record& record);
    void processSetWindowExt(int& err,
        mx_standard_metafile_record& record);
    void processSetTextColor(int& err,
        mx_standard_metafile_record& record);
    void processSetBkColor(int& err,
        mx_standard_metafile_record& record);
    virtual void processSetStretchBltMode(int& err,
        mx_standard_metafile_record& record);
    void processStretchDIBits(int& err,
        mx_standard_metafile_record& record,
        char** buffer);

    mx_ipoint windowOrg;
    mx_ipoint windowExt;
    mx_colour textColour;
    mx_colour bkColour;
    int stretchBltMode;
    mx_stretchDIBits stretchDIB;

    int mapMode;

    mx_colour mxDIBcolours[256];

    bool isGreyScale;
};

class mx_mxmetafile_writer {
public:
    mx_mxmetafile_writer(mx_image_area* image);
    ~mx_mxmetafile_writer();

    void output(int& err);

    void outputDIB(int& err);

    void outputBitMapInfo(int& err);

    void outputColourInfo(int& err);

    void outputBitMap(int& err);

    inline char* getBuffer() { return buffer; };

private:
    void setRasterDetails(int& err);

    mx_standard_metafile_record sr;

    mx_image_area* image;

    void addToBuffer(int addSize, const char* addData);

    void writeWord(int value, char* ibuffer = NULL);

    void writeDWord(int value, char* ibuffer = NULL);

    void writeHeader();
    void writeSR();

    char* buffer;

    int bufferSize;
    int allocBufferSize;
    int incrementSize;

    int largestRecordSize;
    int totalRecordSize;

    // raster details
    int ncolours;
    int nbits;
    mx_ipoint rasterSize;
    int sizeImage;
    int extraPixels;
    raster* outputRaster;
    bool deleteOutputRaster;
};

#endif
