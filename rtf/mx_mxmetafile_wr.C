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
 * MODULE : mx_mxmetafile_wr.C
 *
 * AUTHOR : David Miller
 *
 * This file
 *
 * DESCRIPTION:
 * Module mx_metafile_wr.C
 *
 *
 *
 */

#include "mx_image_area.h"
#include "mx_mxmetafile.h"
#include "stdlib.h"

static char outputHexFromByte[256][2] = {
    { '0', '0' }, { '0', '1' }, { '0', '2' }, { '0', '3' },
    { '0', '4' }, { '0', '5' }, { '0', '6' }, { '0', '7' },
    { '0', '8' }, { '0', '9' }, { '0', 'a' }, { '0', 'b' },
    { '0', 'c' }, { '0', 'd' }, { '0', 'e' }, { '0', 'f' },
    { '1', '0' }, { '1', '1' }, { '1', '2' }, { '1', '3' },
    { '1', '4' }, { '1', '5' }, { '1', '6' }, { '1', '7' },
    { '1', '8' }, { '1', '9' }, { '1', 'a' }, { '1', 'b' },
    { '1', 'c' }, { '1', 'd' }, { '1', 'e' }, { '1', 'f' },
    { '2', '0' }, { '2', '1' }, { '2', '2' }, { '2', '3' },
    { '2', '4' }, { '2', '5' }, { '2', '6' }, { '2', '7' },
    { '2', '8' }, { '2', '9' }, { '2', 'a' }, { '2', 'b' },
    { '2', 'c' }, { '2', 'd' }, { '2', 'e' }, { '2', 'f' },
    { '3', '0' }, { '3', '1' }, { '3', '2' }, { '3', '3' },
    { '3', '4' }, { '3', '5' }, { '3', '6' }, { '3', '7' },
    { '3', '8' }, { '3', '9' }, { '3', 'a' }, { '3', 'b' },
    { '3', 'c' }, { '3', 'd' }, { '3', 'e' }, { '3', 'f' },
    { '4', '0' }, { '4', '1' }, { '4', '2' }, { '4', '3' },
    { '4', '4' }, { '4', '5' }, { '4', '6' }, { '4', '7' },
    { '4', '8' }, { '4', '9' }, { '4', 'a' }, { '4', 'b' },
    { '4', 'c' }, { '4', 'd' }, { '4', 'e' }, { '4', 'f' },
    { '5', '0' }, { '5', '1' }, { '5', '2' }, { '5', '3' },
    { '5', '4' }, { '5', '5' }, { '5', '6' }, { '5', '7' },
    { '5', '8' }, { '5', '9' }, { '5', 'a' }, { '5', 'b' },
    { '5', 'c' }, { '5', 'd' }, { '5', 'e' }, { '5', 'f' },
    { '6', '0' }, { '6', '1' }, { '6', '2' }, { '6', '3' },
    { '6', '4' }, { '6', '5' }, { '6', '6' }, { '6', '7' },
    { '6', '8' }, { '6', '9' }, { '6', 'a' }, { '6', 'b' },
    { '6', 'c' }, { '6', 'd' }, { '6', 'e' }, { '6', 'f' },
    { '7', '0' }, { '7', '1' }, { '7', '2' }, { '7', '3' },
    { '7', '4' }, { '7', '5' }, { '7', '6' }, { '7', '7' },
    { '7', '8' }, { '7', '9' }, { '7', 'a' }, { '7', 'b' },
    { '7', 'c' }, { '7', 'd' }, { '7', 'e' }, { '7', 'f' },
    { '8', '0' }, { '8', '1' }, { '8', '2' }, { '8', '3' },
    { '8', '4' }, { '8', '5' }, { '8', '6' }, { '8', '7' },
    { '8', '8' }, { '8', '9' }, { '8', 'a' }, { '8', 'b' },
    { '8', 'c' }, { '8', 'd' }, { '8', 'e' }, { '8', 'f' },
    { '9', '0' }, { '9', '1' }, { '9', '2' }, { '9', '3' },
    { '9', '4' }, { '9', '5' }, { '9', '6' }, { '9', '7' },
    { '9', '8' }, { '9', '9' }, { '9', 'a' }, { '9', 'b' },
    { '9', 'c' }, { '9', 'd' }, { '9', 'e' }, { '9', 'f' },
    { 'a', '0' }, { 'a', '1' }, { 'a', '2' }, { 'a', '3' },
    { 'a', '4' }, { 'a', '5' }, { 'a', '6' }, { 'a', '7' },
    { 'a', '8' }, { 'a', '9' }, { 'a', 'a' }, { 'a', 'b' },
    { 'a', 'c' }, { 'a', 'd' }, { 'a', 'e' }, { 'a', 'f' },
    { 'b', '0' }, { 'b', '1' }, { 'b', '2' }, { 'b', '3' },
    { 'b', '4' }, { 'b', '5' }, { 'b', '6' }, { 'b', '7' },
    { 'b', '8' }, { 'b', '9' }, { 'b', 'a' }, { 'b', 'b' },
    { 'b', 'c' }, { 'b', 'd' }, { 'b', 'e' }, { 'b', 'f' },
    { 'c', '0' }, { 'c', '1' }, { 'c', '2' }, { 'c', '3' },
    { 'c', '4' }, { 'c', '5' }, { 'c', '6' }, { 'c', '7' },
    { 'c', '8' }, { 'c', '9' }, { 'c', 'a' }, { 'c', 'b' },
    { 'c', 'c' }, { 'c', 'd' }, { 'c', 'e' }, { 'c', 'f' },
    { 'd', '0' }, { 'd', '1' }, { 'd', '2' }, { 'd', '3' },
    { 'd', '4' }, { 'd', '5' }, { 'd', '6' }, { 'd', '7' },
    { 'd', '8' }, { 'd', '9' }, { 'd', 'a' }, { 'd', 'b' },
    { 'd', 'c' }, { 'd', 'd' }, { 'd', 'e' }, { 'd', 'f' },
    { 'e', '0' }, { 'e', '1' }, { 'e', '2' }, { 'e', '3' },
    { 'e', '4' }, { 'e', '5' }, { 'e', '6' }, { 'e', '7' },
    { 'e', '8' }, { 'e', '9' }, { 'e', 'a' }, { 'e', 'b' },
    { 'e', 'c' }, { 'e', 'd' }, { 'e', 'e' }, { 'e', 'f' },
    { 'f', '0' }, { 'f', '1' }, { 'f', '2' }, { 'f', '3' },
    { 'f', '4' }, { 'f', '5' }, { 'f', '6' }, { 'f', '7' },
    { 'f', '8' }, { 'f', '9' }, { 'f', 'a' }, { 'f', 'b' },
    { 'f', 'c' }, { 'f', 'd' }, { 'f', 'e' }, { 'f', 'f' }
};

static char outputBinaryHexFromByte[256][2] = {
    { '0', '0' },
    { '8', '0' },
    { '4', '0' },
    { 'c', '0' },
    { '2', '0' },
    { 'a', '0' },
    { '6', '0' },
    { 'e', '0' },
    { '1', '0' },
    { '9', '0' },
    { '5', '0' },
    { 'd', '0' },
    { '3', '0' },
    { 'b', '0' },
    { '7', '0' },
    { 'f', '0' },
    { '0', '8' },
    { '8', '8' },
    { '4', '8' },
    { 'c', '8' },
    { '2', '8' },
    { 'a', '8' },
    { '6', '8' },
    { 'e', '8' },
    { '1', '8' },
    { '9', '8' },
    { '5', '8' },
    { 'd', '8' },
    { '3', '8' },
    { 'b', '8' },
    { '7', '8' },
    { 'f', '8' },
    { '0', '4' },
    { '8', '4' },
    { '4', '4' },
    { 'c', '4' },
    { '2', '4' },
    { 'a', '4' },
    { '6', '4' },
    { 'e', '4' },
    { '1', '4' },
    { '9', '4' },
    { '5', '4' },
    { 'd', '4' },
    { '3', '4' },
    { 'b', '4' },
    { '7', '4' },
    { 'f', '4' },
    { '0', 'c' },
    { '8', 'c' },
    { '4', 'c' },
    { 'c', 'c' },
    { '2', 'c' },
    { 'a', 'c' },
    { '6', 'c' },
    { 'e', 'c' },
    { '1', 'c' },
    { '9', 'c' },
    { '5', 'c' },
    { 'd', 'c' },
    { '3', 'c' },
    { 'b', 'c' },
    { '7', 'c' },
    { 'f', 'c' },
    { '0', '2' },
    { '8', '2' },
    { '4', '2' },
    { 'c', '2' },
    { '2', '2' },
    { 'a', '2' },
    { '6', '2' },
    { 'e', '2' },
    { '1', '2' },
    { '9', '2' },
    { '5', '2' },
    { 'd', '2' },
    { '3', '2' },
    { 'b', '2' },
    { '7', '2' },
    { 'f', '2' },
    { '0', 'a' },
    { '8', 'a' },
    { '4', 'a' },
    { 'c', 'a' },
    { '2', 'a' },
    { 'a', 'a' },
    { '6', 'a' },
    { 'e', 'a' },
    { '1', 'a' },
    { '9', 'a' },
    { '5', 'a' },
    { 'd', 'a' },
    { '3', 'a' },
    { 'b', 'a' },
    { '7', 'a' },
    { 'f', 'a' },
    { '0', '6' },
    { '8', '6' },
    { '4', '6' },
    { 'c', '6' },
    { '2', '6' },
    { 'a', '6' },
    { '6', '6' },
    { 'e', '6' },
    { '1', '6' },
    { '9', '6' },
    { '5', '6' },
    { 'd', '6' },
    { '3', '6' },
    { 'b', '6' },
    { '7', '6' },
    { 'f', '6' },
    { '0', 'e' },
    { '8', 'e' },
    { '4', 'e' },
    { 'c', 'e' },
    { '2', 'e' },
    { 'a', 'e' },
    { '6', 'e' },
    { 'e', 'e' },
    { '1', 'e' },
    { '9', 'e' },
    { '5', 'e' },
    { 'd', 'e' },
    { '3', 'e' },
    { 'b', 'e' },
    { '7', 'e' },
    { 'f', 'e' },
    { '0', '1' },
    { '8', '1' },
    { '4', '1' },
    { 'c', '1' },
    { '2', '1' },
    { 'a', '1' },
    { '6', '1' },
    { 'e', '1' },
    { '1', '1' },
    { '9', '1' },
    { '5', '1' },
    { 'd', '1' },
    { '3', '1' },
    { 'b', '1' },
    { '7', '1' },
    { 'f', '1' },
    { '0', '9' },
    { '8', '9' },
    { '4', '9' },
    { 'c', '9' },
    { '2', '9' },
    { 'a', '9' },
    { '6', '9' },
    { 'e', '9' },
    { '1', '9' },
    { '9', '9' },
    { '5', '9' },
    { 'd', '9' },
    { '3', '9' },
    { 'b', '9' },
    { '7', '9' },
    { 'f', '9' },
    { '0', '5' },
    { '8', '5' },
    { '4', '5' },
    { 'c', '5' },
    { '2', '5' },
    { 'a', '5' },
    { '6', '5' },
    { 'e', '5' },
    { '1', '5' },
    { '9', '5' },
    { '5', '5' },
    { 'd', '5' },
    { '3', '5' },
    { 'b', '5' },
    { '7', '5' },
    { 'f', '5' },
    { '0', 'd' },
    { '8', 'd' },
    { '4', 'd' },
    { 'c', 'd' },
    { '2', 'd' },
    { 'a', 'd' },
    { '6', 'd' },
    { 'e', 'd' },
    { '1', 'd' },
    { '9', 'd' },
    { '5', 'd' },
    { 'd', 'd' },
    { '3', 'd' },
    { 'b', 'd' },
    { '7', 'd' },
    { 'f', 'd' },
    { '0', '3' },
    { '8', '3' },
    { '4', '3' },
    { 'c', '3' },
    { '2', '3' },
    { 'a', '3' },
    { '6', '3' },
    { 'e', '3' },
    { '1', '3' },
    { '9', '3' },
    { '5', '3' },
    { 'd', '3' },
    { '3', '3' },
    { 'b', '3' },
    { '7', '3' },
    { 'f', '3' },
    { '0', 'b' },
    { '8', 'b' },
    { '4', 'b' },
    { 'c', 'b' },
    { '2', 'b' },
    { 'a', 'b' },
    { '6', 'b' },
    { 'e', 'b' },
    { '1', 'b' },
    { '9', 'b' },
    { '5', 'b' },
    { 'd', 'b' },
    { '3', 'b' },
    { 'b', 'b' },
    { '7', 'b' },
    { 'f', 'b' },
    { '0', '7' },
    { '8', '7' },
    { '4', '7' },
    { 'c', '7' },
    { '2', '7' },
    { 'a', '7' },
    { '6', '7' },
    { 'e', '7' },
    { '1', '7' },
    { '9', '7' },
    { '5', '7' },
    { 'd', '7' },
    { '3', '7' },
    { 'b', '7' },
    { '7', '7' },
    { 'f', '7' },
    { '0', 'f' },
    { '8', 'f' },
    { '4', 'f' },
    { 'c', 'f' },
    { '2', 'f' },
    { 'a', 'f' },
    { '6', 'f' },
    { 'e', 'f' },
    { '1', 'f' },
    { '9', 'f' },
    { '5', 'f' },
    { 'd', 'f' },
    { 'f', '3' },
    { 'f', 'b' },
    { 'f', '7' },
    { 'f', 'f' },
};

/*-------------------------------------------------
 * FUNCTION: mx_mxmetafile_writer::mx_mxmetafile_writer
 *
 * DESCRIPTION: Read a word form a metafile buffer
 *
 *
 */

mx_mxmetafile_writer::mx_mxmetafile_writer(mx_image_area* iimage)
{
    image = iimage;
    bufferSize = 0;
    allocBufferSize = 0;
    buffer = NULL;
    incrementSize = 100000;
    largestRecordSize = 0;
    totalRecordSize = 0;

    ncolours = 0;
    nbits = 0;
    sizeImage = 0;
    extraPixels = 0;
    outputRaster = NULL;
    deleteOutputRaster = FALSE;
}

/*-------------------------------------------------
 * FUNCTION: mx_mxmetafile_writer::~mx_mxmetafile_writer
 *
 * DESCRIPTION: Read a word form a metafile buffer
 *
 *
 */

mx_mxmetafile_writer::~mx_mxmetafile_writer()
{
    delete[] buffer;

    if (deleteOutputRaster)
        delete outputRaster;
}

/*-------------------------------------------------
 * FUNCTION: mx_mxmetafile_writer::addToBuffer
 *
 * DESCRIPTION: Read a word form a metafile buffer
 *
 *
 */

void mx_mxmetafile_writer::addToBuffer(int addSize,
    const char* addData)
{
    int newBufferSize = bufferSize + addSize;
    char* newBuffer;

    if (newBufferSize > allocBufferSize) {
        allocBufferSize += incrementSize;
        if (allocBufferSize < newBufferSize) {
            allocBufferSize = newBufferSize;
        }

        newBuffer = new char[allocBufferSize];

        memcpy(newBuffer, buffer, bufferSize);

        delete[] buffer;

        buffer = newBuffer;
    }

    memcpy(buffer + bufferSize, addData, addSize);

    bufferSize = newBufferSize;
}

/*-------------------------------------------------
 * FUNCTION: mx_mxmetafile_writer::writeWord
 *
 * DESCRIPTION: Read a word form a metafile buffer
 *
 *
 */

inline void mx_mxmetafile_writer::writeWord(int value, char* bufferPos)
{
    char valStr[20];
    char outString[4];

    sprintf(valStr, "%04x", value);

    outString[0] = valStr[2];
    outString[1] = valStr[3];
    outString[2] = valStr[0];
    outString[3] = valStr[1];

    if (bufferPos != NULL) {
        memcpy(bufferPos, outString, 4);
    } else {
        addToBuffer(4, outString);
    }
}

/*-------------------------------------------------
 * FUNCTION: mx_metafile_writer::writeDWord
 *
 * DESCRIPTION: Read a double word form a
 *              metafile buffer
 *
 *
 */

inline void mx_mxmetafile_writer::writeDWord(int value, char* bufferPos)
{
    char valStr[20];
    char outString[8];

    sprintf(valStr, "%08x", value);

    outString[0] = valStr[6];
    outString[1] = valStr[7];
    outString[2] = valStr[4];
    outString[3] = valStr[5];
    outString[4] = valStr[2];
    outString[5] = valStr[3];
    outString[6] = valStr[0];
    outString[7] = valStr[1];

    if (bufferPos != NULL) {
        memcpy(bufferPos, outString, 8);
    } else {
        addToBuffer(8, outString);
    }
}

/*-------------------------------------------------
 * FUNCTION: mx_mxmetafile_writer::writeHeader
 *
 * DESCRIPTION: Set a metafile header from values
 *              in a buffer
 *
 */

void mx_mxmetafile_writer::writeHeader()
{
    // type
    writeWord(1);

    // header size
    writeWord(9);

    // version
    writeWord(0x0300);

    // size - this gets reset later
    writeDWord(totalRecordSize);

    // No of objects
    writeWord(0);

    // max record size - this gets reset later
    writeDWord(largestRecordSize);

    // no of parameters
    writeWord(0);
}

/*-------------------------------------------------
 * FUNCTION: mx_mxmetafile_writer::writeSR
 *
 * DESCRIPTION: Set a metafile header from values
 *              in a buffer
 *
 */

void mx_mxmetafile_writer::writeSR()
{
    int iparam;

    writeDWord(sr.size);
    writeWord(sr.function);

    for (iparam = 0; iparam < sr.nparams; iparam++) {
        writeWord(sr.params[iparam]);
    }

    if (largestRecordSize < sr.size) {
        largestRecordSize = sr.size;
    }
}

/*-------------------------------------------------
 * FUNCTION: mx_mxmetafile_writer::output
 *
 * DESCRIPTION: Set a metafile header from values
 *              in a buffer
 *
 */

void mx_mxmetafile_writer::output(int& err)
{
    int params[2];

    err = MX_ERROR_OK;

    outputRaster = image->getBestRaster(err,
        deleteOutputRaster);
    MX_ERROR_CHECK(err);

    setRasterDetails(err);
    MX_ERROR_CHECK(err);

    writeHeader();

    // set map mode
    params[0] = 8;
    sr.set(4, MX_METAFILE_SetMapMode, 1, params);
    writeSR();

    // set window org
    params[0] = 0;
    params[1] = 0;
    sr.set(5, MX_METAFILE_SetWindowOrg, 2, params);
    writeSR();

    // set window ext - this is the number of
    // pixels in the raster

    params[0] = rasterSize.y;
    params[1] = rasterSize.x;
    sr.set(5, MX_METAFILE_SetWindowExt, 2, params);
    writeSR();

    params[0] = 4;
    sr.set(4, MX_METAFILE_SetStretchBltMode, 1, params);
    writeSR();

    outputDIB(err);
    MX_ERROR_CHECK(err);

    params[0] = 1;
    sr.set(4, MX_METAFILE_SetStretchBltMode, 1, params);
    writeSR();

    sr.set(3, MX_METAFILE_NullFunction, 0, params);
    writeSR();

    addToBuffer(1, "");

abort:;
}

/*-------------------------------------------------
 * FUNCTION: mx_mxmetafile_writer::outputDIB
 *
 * DESCRIPTION: Set a metafile header from values
 *              in a buffer
 *
 */

void mx_mxmetafile_writer::outputDIB(int& err)
{
    err = MX_ERROR_OK;

    writeDWord(largestRecordSize);
    writeWord(MX_METAFILE_StretchDIBits);

    // dwrop
    writeDWord(13369376);

    // usag
    writeWord(0);

    // y size
    writeWord(rasterSize.y);
    MX_ERROR_CHECK(err);

    // x size
    writeWord(rasterSize.x);
    MX_ERROR_CHECK(err);

    // y origin
    writeWord(0);

    // x origin
    writeWord(0);

    // y size
    writeWord(rasterSize.y);
    MX_ERROR_CHECK(err);

    // x size
    writeWord(rasterSize.x);
    MX_ERROR_CHECK(err);

    // y origin
    writeWord(0);

    // x origin
    writeWord(0);

    outputBitMapInfo(err);
    MX_ERROR_CHECK(err);

    outputColourInfo(err);
    MX_ERROR_CHECK(err);

    outputBitMap(err);
    MX_ERROR_CHECK(err);

abort:;
}

/*-------------------------------------------------
 * FUNCTION: mx_mxmetafile_writer::outputColourInfo
 *
 * DESCRIPTION: Set a metafile header from values
 *              in a buffer
 *
 */

void mx_mxmetafile_writer::outputColourInfo(int& err)
{
    unsigned char colourValues[4] = { 0, 0, 0, 0 };
    mx_colour* colours = outputRaster->getColours();

    switch (outputRaster->getData()) {
    case RDATA_COLOUR:
    case RDATA_BINARY:

        if (outputRaster->getNumColours() != 0) {
            for (int icolour = 0; icolour < ncolours; icolour++) {
                colourValues[0] = colours[icolour].blue;
                colourValues[1] = colours[icolour].green;
                colourValues[2] = colours[icolour].red;

                addToBuffer(2, outputHexFromByte[colourValues[0]]);
                addToBuffer(2, outputHexFromByte[colourValues[1]]);
                addToBuffer(2, outputHexFromByte[colourValues[2]]);
                addToBuffer(2, outputHexFromByte[colourValues[3]]);
            }
        } else {
            // default binary
            addToBuffer(2, outputHexFromByte[colourValues[0]]);
            addToBuffer(2, outputHexFromByte[colourValues[1]]);
            addToBuffer(2, outputHexFromByte[colourValues[2]]);
            addToBuffer(2, outputHexFromByte[colourValues[3]]);

            colourValues[0] = colourValues[1] = colourValues[2] = 255;

            addToBuffer(2, outputHexFromByte[colourValues[0]]);
            addToBuffer(2, outputHexFromByte[colourValues[1]]);
            addToBuffer(2, outputHexFromByte[colourValues[2]]);
            addToBuffer(2, outputHexFromByte[colourValues[3]]);
        }
        break;
    case RDATA_GREYSCALE:

        for (int icolour = 0; icolour < ncolours; icolour++) {
            colourValues[0] = icolour;
            colourValues[1] = icolour;
            colourValues[2] = icolour;

            addToBuffer(2, outputHexFromByte[colourValues[0]]);
            addToBuffer(2, outputHexFromByte[colourValues[1]]);
            addToBuffer(2, outputHexFromByte[colourValues[2]]);
            addToBuffer(2, outputHexFromByte[colourValues[3]]);
        }
        break;
    default:
        break;
    }
}

/*-------------------------------------------------
 * FUNCTION: mx_mxmetafile_writer::outputBitMap
 *
 * DESCRIPTION: Set a metafile header from values
 *              in a buffer
 *
 */

void mx_mxmetafile_writer::outputBitMap(int& err)
{
    mx_irect pixelCoords;
    unsigned char* outBuffer;
    unsigned char value, maxValue;
    int ipixels;
    int shiftValue;
    int nbytes;
    err = MX_ERROR_OK;

    pixelCoords.xb = 0;
    pixelCoords.xt = rasterSize.x - 1;

    if (nbits == 1)
        pixelCoords.xt += extraPixels;

    for (int irow = rasterSize.y - 1; irow >= 0; irow--) {
        pixelCoords.yb = irow;
        pixelCoords.yt = irow;

        outBuffer = outputRaster->getBuffer(err, pixelCoords);
        MX_ERROR_CHECK(err);

        switch (nbits) {
        case 24:

            for (ipixels = 0; ipixels < rasterSize.x; ipixels++) {
                addToBuffer(2, outputHexFromByte[*(outBuffer + 2)]);
                addToBuffer(2, outputHexFromByte[*(outBuffer + 1)]);
                addToBuffer(2, outputHexFromByte[*(outBuffer)]);

                outBuffer += 3;
            }

            for (ipixels = 0; ipixels < extraPixels; ipixels++) {
                addToBuffer(2, outputHexFromByte[0]);
                addToBuffer(2, outputHexFromByte[0]);
                addToBuffer(2, outputHexFromByte[0]);
            }

            break;

        case 8:

            switch (outputRaster->getData()) {
            case RDATA_COLOUR:

                for (ipixels = 0; ipixels < rasterSize.x; ipixels++) {
                    addToBuffer(2, outputHexFromByte[*(outBuffer)]);
                    outBuffer++;
                }

                for (ipixels = 0; ipixels < extraPixels; ipixels++) {
                    addToBuffer(2, outputHexFromByte[0]);
                }
                break;

            case RDATA_GREYSCALE:

                shiftValue = 8 - outputRaster->getSigbits(0);
                if (shiftValue == 8)
                    shiftValue = 0;
                maxValue = (1 << outputRaster->getSigbits(0)) - 1;

                for (ipixels = 0; ipixels < rasterSize.x; ipixels++) {
                    if (*outBuffer == maxValue) {
                        value = 255;
                    } else {
                        value = (*(outBuffer)) << shiftValue;
                    }

                    addToBuffer(2, outputHexFromByte[value]);

                    outBuffer++;
                }

                for (ipixels = 0; ipixels < extraPixels; ipixels++) {
                    addToBuffer(2, outputHexFromByte[0]);
                }
                break;
            default:
                break;
            }
            break;
        case 1:

            // use a look up table
            nbytes = (rasterSize.x + extraPixels) / 8;

            if (outputRaster->getNumColours() == 0) {
                for (ipixels = 0; ipixels < nbytes; ipixels++) {
                    value = ~(*outBuffer);
                    addToBuffer(2, outputBinaryHexFromByte[value]);
                    outBuffer++;
                }
            } else {
                for (ipixels = 0; ipixels < nbytes; ipixels++) {
                    addToBuffer(2, outputBinaryHexFromByte[*outBuffer]);
                    outBuffer++;
                }
            }
        default:
            break;
        }

        // add a new line - look's like \r is important for
        // Word import !

        addToBuffer(2, "\r\n");
    }
abort:;
}

/*-------------------------------------------------
 * FUNCTION: mx_mxmetafile_writer::outputBitMapInfo
 *
 * DESCRIPTION: Set a metafile header from values
 *              in a buffer
 *
 */

void mx_mxmetafile_writer::outputBitMapInfo(int& err)
{
    err = MX_ERROR_OK;

    // size
    writeDWord(40);

    // biWidth
    writeDWord(rasterSize.x);

    // biHeight
    writeDWord(rasterSize.y);

    // biPlanes
    writeWord(1);

    // biBitCount
    writeWord(nbits);

    // biCompression
    writeDWord(0);

    // size image
    writeDWord(sizeImage);

    // biXPelsPerMeter
    writeDWord(0);

    // biXPelsPerMeter
    writeDWord(0);

    // biClrUsed
    writeDWord(ncolours);

    // biClrImportant
    writeDWord(ncolours);
}

void mx_mxmetafile_writer::setRasterDetails(int& err)
{
    int ndataRecords = 0;

    err = MX_ERROR_OK;

    rasterSize = outputRaster->getSize();
    nbits = rasterBits(outputRaster->getPack(), 1,
        outputRaster->getPlanes());

    switch (nbits) {
    case 24:
        // set the number of extra pixels to be divisible by 4
        // so lines are an integral number of words
        extraPixels = rasterSize.x % 4;
        if (extraPixels != 0)
            extraPixels = 4 - extraPixels;

        ncolours = 0;

        ndataRecords = (((rasterSize.x + extraPixels) / 2) * rasterSize.y) * 3;

        break;
    case 8:

        // set the number of extra pixels to be divisible by 2
        // so lines are an integral number of words
        extraPixels = rasterSize.x % 4;
        if (extraPixels != 0)
            extraPixels = 4 - extraPixels;

        switch (outputRaster->getData()) {
        case RDATA_COLOUR:
            ncolours = outputRaster->getNumColours();
            break;
        case RDATA_GREYSCALE:
            ncolours = 256;
            break;
        default:
            break;
        }

        ndataRecords = (((rasterSize.x + extraPixels) / 2) * rasterSize.y);
        break;

    case 1:

        // set the number of extra pixels to be divisible by 2
        // so lines are an integral number of words
        extraPixels = rasterSize.x % 16;
        if (extraPixels != 0)
            extraPixels = 16 - extraPixels;

        ncolours = 2;
        ndataRecords = (((rasterSize.x + extraPixels) / 16) * rasterSize.y);

        break;
    }

    sizeImage = (nbits * (rasterSize.x + extraPixels) * rasterSize.y) / 8;

    largestRecordSize = 34 + (2 * ncolours) + ndataRecords;

    totalRecordSize = largestRecordSize + 34;
}
