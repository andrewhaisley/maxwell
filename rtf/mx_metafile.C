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
 * MODULE : mx_metafile.C
 *
 * AUTHOR : David Miller
 *
 * This file
 *
 * DESCRIPTION:
 * Module mx_metafile.C
 *
 * Completely general handling of metafiles
 *
 *
 * Revision 1.1  1996/10/06 13:15:45  andrew
 * Initial revision
 *
 *
 */

#include "mx_metafile.h"
#include "stdlib.h"

/*-------------------------------------------------
 * FUNCTION: mx_metafile_reader::readWord
 *
 * DESCRIPTION: Read a word form a metafile buffer
 *
 *
 */

inline int mx_metafile_reader::readWord(int& err,
    char** buffer)
{
    char hexString[] = "0x????";
    char* endPtr;
    int answer;

    err = MX_ERROR_OK;

    hexString[2] = (*buffer)[2];
    hexString[3] = (*buffer)[3];
    hexString[4] = (*buffer)[0];
    hexString[5] = (*buffer)[1];

    answer = strtol(hexString, &endPtr, 0);

    *buffer += 4;

    // test that we have read an integer

    if (endPtr != hexString + 6) {
        MX_ERROR_THROW(err, MX_ERROR_METAFILE_DATA_ERROR);
    }

    return answer;
abort:;
    return 0;
}

/*-------------------------------------------------
 * FUNCTION: mx_metafile_reader::readDWord
 *
 * DESCRIPTION: Read a double word form a
 *              metafile buffer
 *
 *
 */

inline int mx_metafile_reader::readDWord(int& err,
    char** buffer)
{
    char hexString[] = "0x????????";
    char* endPtr;
    int answer;

    err = MX_ERROR_OK;

    hexString[2] = (*buffer)[6];
    hexString[3] = (*buffer)[7];
    hexString[4] = (*buffer)[4];
    hexString[5] = (*buffer)[5];
    hexString[6] = (*buffer)[2];
    hexString[7] = (*buffer)[3];
    hexString[8] = (*buffer)[0];
    hexString[9] = (*buffer)[1];

    answer = strtol(hexString, &endPtr, 0);

    *buffer += 8;

    // test that we have read an integer

    if (endPtr != hexString + 10) {
        MX_ERROR_THROW(err, MX_ERROR_METAFILE_DATA_ERROR);
    }

    return answer;
abort:;
    return 0;
}

/*-------------------------------------------------
 * FUNCTION: mx_metafile_header::set
 *
 * DESCRIPTION: Set a metafile header from values
 *              in a buffer
 *
 */

void mx_metafile_header::set(int& err,
    char** buffer)
{
    err = MX_ERROR_OK;

    mtType = readWord(err, buffer);
    MX_ERROR_CHECK(err);

    mtHeaderSize = readWord(err, buffer);
    MX_ERROR_CHECK(err);

    if (mtHeaderSize != MX_METAFILE_HEADER_WORD_SIZE) {
        MX_ERROR_THROW(err, MX_ERROR_METAFILE_DATA_ERROR);
    }

    mtVersion = readWord(err, buffer);
    MX_ERROR_CHECK(err);

    if (mtVersion != MX_METAFILE_WINDOWS_VERSION) {
        MX_ERROR_THROW(err, MX_ERROR_METAFILE_DATA_ERROR);
    }

    mtSize = readDWord(err, buffer);
    MX_ERROR_CHECK(err);

    mtNoObjects = readWord(err, buffer);
    MX_ERROR_CHECK(err);

    mtMaxRecord = readDWord(err, buffer);
    MX_ERROR_CHECK(err);

    mtNoParameters = readWord(err, buffer);
    MX_ERROR_CHECK(err);

abort:;
}

/*-------------------------------------------------
 * FUNCTION: mx_standard_metafile_record::mx_standard_metafile_record
 *
 * DESCRIPTION:
 *
 *
 */

mx_standard_metafile_record::mx_standard_metafile_record()
{
    nparams = 0;
    nallocParams = 0;
    params = NULL;
}

/*-------------------------------------------------
 * FUNCTION: mx_standard_metafile_record::~mx_standard_metafile_record
 *
 * DESCRIPTION:
 *
 *
 */

mx_standard_metafile_record::~mx_standard_metafile_record()
{
    delete[] params;
}

/*-------------------------------------------------
 * FUNCTION: mx_standard_metafile_record::set
 *
 * DESCRIPTION: Set a standard metafile record
 *
 *
 */

void mx_standard_metafile_record::set(int isize, int ifunction,
    int inparams, int* iparams)
{
    size = isize;
    function = ifunction;

    if (inparams > nallocParams) {
        delete[] params;

        params = new int[inparams];

        nallocParams = inparams;
    }

    nparams = inparams;

    for (int iparam = 0; iparam < nparams; iparam++) {
        params[iparam] = iparams[iparam];
    }
}

/*-------------------------------------------------
 * FUNCTION: mx_standard_metafile_record::setHeader
 *
 * DESCRIPTION: Set a standard metafile record
 *
 *
 */

void mx_standard_metafile_record::setHeader(int& err,
    char** buffer)
{
    err = MX_ERROR_OK;

    size = readDWord(err, buffer);
    MX_ERROR_CHECK(err);

    function = readWord(err, buffer);
    MX_ERROR_CHECK(err);
abort:;
}

/*-------------------------------------------------
 * FUNCTION: mx_standard_metafile_record::setParams
 *
 * DESCRIPTION: Set a standard metafile record
 *
 *
 */

void mx_standard_metafile_record::setParams(int& err,
    char** buffer)
{
    err = MX_ERROR_OK;

    nparams = size - 3;

    if (nparams > nallocParams) {
        delete[] params;

        params = new int[nparams];

        nallocParams = nparams;
    }

    for (int iparam = 0; iparam < nparams; iparam++) {
        params[iparam] = readWord(err, buffer);
        MX_ERROR_CHECK(err);
    }
abort:;
}

/*-------------------------------------------------
 * FUNCTION: mx_metafile_output::mx_metafile_output
 *
 * DESCRIPTION: Determine
 *
 *
 */

mx_metafile_output::mx_metafile_output()
    : size(-1, 1)
    , scale(-1, -1)
    , crop(-1, -1, -1, -1)
{
}
/*-------------------------------------------------
 * FUNCTION: mx_metafile_output::processNonStandardRecord
 *
 * DESCRIPTION: Determine
 *
 *
 */

void mx_metafile_output::processNonStandardRecord(int& err,
    mx_standard_metafile_record& record,
    char** buffer)
{
    err = MX_ERROR_OK;

    switch (record.handledFunction) {
    case MX_METAFILE_StretchDIBits:
        processStretchDIBits(err, record, buffer);
        MX_ERROR_CHECK(err);
        break;
    default:
        break;
    }
abort:;
}

/*-------------------------------------------------
 * FUNCTION: mx_metafile_output::processStandardRecord
 *
 * DESCRIPTION: Determine
 *
 *
 */

void mx_metafile_output::processStandardRecord(int& err,
    mx_standard_metafile_record& record)
{
    err = MX_ERROR_OK;

    switch (record.handledFunction) {
    case MX_METAFILE_SetMapMode:
        processSetMapMode(err, record);
        MX_ERROR_CHECK(err);
        break;
    case MX_METAFILE_SetWindowOrg:
        processSetWindowOrg(err, record);
        MX_ERROR_CHECK(err);
        break;
    case MX_METAFILE_SetWindowExt:
        processSetWindowExt(err, record);
        MX_ERROR_CHECK(err);
        break;
    case MX_METAFILE_SetTextColor:
        processSetTextColor(err, record);
        MX_ERROR_CHECK(err);
        break;
    case MX_METAFILE_SetBkColor:
        processSetBkColor(err, record);
        MX_ERROR_CHECK(err);
        break;
    case MX_METAFILE_SetStretchBltMode:
        processSetStretchBltMode(err, record);
        MX_ERROR_CHECK(err);
        break;
    default:
        break;
    }
abort:;
}

/*-------------------------------------------------
 * FUNCTION: mx_metafile::standardRecord
 *
 * DESCRIPTION: Set a standard metafile record
 *
 *
 */

bool mx_metafile::isStandardRecord()
{
    switch (standardRecord.handledFunction) {
    case MX_METAFILE_StretchDIBits:
        return FALSE;
    default:
        return TRUE;
    }
}

mx_metafile::mx_metafile(mx_metafile_input* iinput,
    mx_metafile_output* ioutput)
{
    buffer = NULL;
    allocBufferSize = 0;
    bufferSize = 0;
    input = iinput;
    output = ioutput;
}

mx_metafile::~mx_metafile()
{
    delete[] buffer;
}

void mx_metafile::setBufferSize(int newSize)
{
    if (newSize > allocBufferSize) {
        delete[] buffer;

        allocBufferSize = newSize;

        buffer = new char[newSize];
    }

    bufferSize = newSize;
}

void mx_metafile::process(int& err)
{
    bool moreRecords;
    char* startBuffer;

    err = MX_ERROR_OK;

    setBufferSize(MX_METAFILE_HEADER_SIZE);

    input->getHeaderData(err, buffer);
    MX_ERROR_CHECK(err);

    startBuffer = buffer;

    header.set(err, &startBuffer);
    MX_ERROR_CHECK(err);

    setBufferSize(header.mtMaxRecord * MX_METAFILE_WORD_SIZE);
    MX_ERROR_CHECK(err);

    do {
        input->getNextRecordData(err, header.mtMaxRecord, buffer);
        MX_ERROR_CHECK(err);

        moreRecords = processNextRecord(err);
        MX_ERROR_CHECK(err);
    } while (moreRecords);

abort:;
}

/*-------------------------------------------------
 * FUNCTION: mx_metafile::handleRecord
 *
 * DESCRIPTION: Determine
 *
 *
 */

bool mx_metafile_output::handleRecord(mx_standard_metafile_record& record)
{
    switch (record.function) {
    case MX_METAFILE_SetMapMode:
    case MX_METAFILE_SetWindowOrg:
    case MX_METAFILE_SetWindowExt:
    case MX_METAFILE_SetTextColor:
    case MX_METAFILE_SetBkColor:
    case MX_METAFILE_StretchDIBits:
    case MX_METAFILE_SetStretchBltMode:
        record.handledFunction = (MX_METAFILE_FUNCTION)record.function;
        return TRUE;
    default:

        record.handledFunction = MX_METAFILE_Unhandled;
        return FALSE;
    }
}

bool mx_metafile::processNextRecord(int& err)
{
    char* startBuffer = buffer;

    // first test if it is a standard record

    standardRecord.setHeader(err, &startBuffer);
    MX_ERROR_CHECK(err);

    if (output->handleRecord(standardRecord)) {
        if (isStandardRecord()) {
            standardRecord.setParams(err, &startBuffer);
            MX_ERROR_CHECK(err);

            output->processStandardRecord(err, standardRecord);
            MX_ERROR_CHECK(err);
        } else {
            output->processNonStandardRecord(err,
                standardRecord,
                &startBuffer);
            MX_ERROR_CHECK(err);
        }
    }

abort:;
    return (standardRecord.function != 0x00);
}

int mx_metafile_input::testRecordLength(int& err,
    int bufferWordSize,
    char* testBuffer)
{
    int testSize = 0;

    err = MX_ERROR_OK;

    testSize = readDWord(err, &testBuffer);
    MX_ERROR_CHECK(err);

    if (testSize > bufferWordSize) {
        MX_ERROR_THROW(err, MX_ERROR_METAFILE_DATA_ERROR);
    }

abort:
    return testSize * MX_METAFILE_WORD_SIZE;
}

mx_metafile_buffer_input::mx_metafile_buffer_input(int& err,
    int ibufferSize,
    char* ibuffer)
{
    err = MX_ERROR_OK;

    buffer = ibuffer;
    bufferSize = ibufferSize;
    bufferOffset = 0;
}

void mx_metafile_buffer_input::getHeaderData(int& err,
    char* obuffer)
{
    err = MX_ERROR_OK;

    if (MX_METAFILE_HEADER_SIZE + bufferOffset > bufferSize) {
        MX_ERROR_THROW(err, MX_ERROR_METAFILE_BUFFER_ERROR);
    }

    memcpy(obuffer, buffer + bufferOffset, MX_METAFILE_HEADER_SIZE);

    bufferOffset += MX_METAFILE_HEADER_SIZE;

abort:;
}

void mx_metafile_buffer_input::getNextRecordData(int& err,
    int bufferWordSize,
    char* obuffer)
{
    int testSize;

    err = MX_ERROR_OK;

    if (MX_METAFILE_DWORD_SIZE + bufferOffset > bufferSize) {
        MX_ERROR_THROW(err, MX_ERROR_METAFILE_BUFFER_ERROR);
    }

    testSize = testRecordLength(err, bufferWordSize, buffer + bufferOffset);
    MX_ERROR_CHECK(err);

    if (testSize + bufferOffset > bufferSize) {
        MX_ERROR_THROW(err, MX_ERROR_METAFILE_BUFFER_ERROR);
    }

    memcpy(obuffer, buffer + bufferOffset, testSize);
    bufferOffset += testSize;

abort:;
}

mx_metafile_file_input::mx_metafile_file_input(int& err,
    char* fileName)
{
    err = MX_ERROR_OK;

    file = fopen(fileName, "r");

    if (file == NULL) {
        MX_ERROR_THROW(err, MX_ERROR_METAFILE_FILE_ERROR);
    }
abort:;
}

void mx_metafile_file_input::getHeaderData(int& err,
    char* buffer)
{
    int nitems;

    err = MX_ERROR_OK;

    nitems = fread(buffer, MX_METAFILE_HEADER_SIZE, 1, file);

    if (nitems != 1) {
        MX_ERROR_THROW(err, MX_ERROR_METAFILE_FILE_ERROR);
    }

abort:;
}

void mx_metafile_file_input::getNextRecordData(int& err,
    int bufferWordSize,
    char* buffer)
{
    int nitems;
    int testSize;

    err = MX_ERROR_OK;

    nitems = fread(buffer, MX_METAFILE_DWORD_SIZE, 1, file);

    if (nitems != 1) {
        MX_ERROR_THROW(err, MX_ERROR_METAFILE_FILE_ERROR);
    }

    testSize = testRecordLength(err, bufferWordSize, buffer);
    MX_ERROR_CHECK(err);

    nitems = fread(buffer + MX_METAFILE_DWORD_SIZE,
        testSize - MX_METAFILE_DWORD_SIZE, 1, file);
    if (nitems != 1) {
        MX_ERROR_THROW(err, MX_ERROR_METAFILE_FILE_ERROR);
    }
abort:;
}

void mx_stretchDIBits::set(int& err, char** buffer)
{
    err = MX_ERROR_OK;

    dwRop = readDWord(err, buffer);
    MX_ERROR_CHECK(err);

    Usag = readWord(err, buffer);
    MX_ERROR_CHECK(err);

    srcYext = readWord(err, buffer);
    MX_ERROR_CHECK(err);

    srcXext = readWord(err, buffer);
    MX_ERROR_CHECK(err);

    srcY = readWord(err, buffer);
    MX_ERROR_CHECK(err);

    srcX = readWord(err, buffer);
    MX_ERROR_CHECK(err);

    dstYext = readWord(err, buffer);
    MX_ERROR_CHECK(err);

    dstXext = readWord(err, buffer);
    MX_ERROR_CHECK(err);

    dstY = readWord(err, buffer);
    MX_ERROR_CHECK(err);

    dstX = readWord(err, buffer);
    MX_ERROR_CHECK(err);

    bitmapinfo.set(err, buffer);
    MX_ERROR_CHECK(err);

    if (bitmapinfo.biClrUsed == 0) {
        switch (bitmapinfo.biBitCount) {
        case 1:
            ncolours = 2;
            break;
        case 2:
            ncolours = 4;
            break;
        case 4:
            ncolours = 16;
            break;
        case 8:
            ncolours = 256;
            break;
        default:
            ncolours = 0;
        }
    } else {
        ncolours = bitmapinfo.biClrUsed;
    }

    for (int icolour = 0; icolour < ncolours; icolour++) {
        DIBcolours[icolour].set(err, buffer);
        MX_ERROR_CHECK(err);
    }

    data = *buffer;

abort:;
}

void mx_metafile_colour::set(int& err, int* params)
{
    err = MX_ERROR_OK;

    blue = params[0] & 0x00ff;
    green = (params[0] & 0xff00) >> 8;
    red = params[1] & 0x00ff;
}

void mx_metafile_colour::set(int& err, char** buffer)
{
    int params[2];

    err = MX_ERROR_OK;

    params[0] = readWord(err, buffer);
    MX_ERROR_CHECK(err);

    params[1] = readWord(err, buffer);
    MX_ERROR_CHECK(err);

    set(err, params);
    MX_ERROR_CHECK(err);

abort:;
}

void mx_bitmapinfo::set(int& err, char** buffer)
{
    err = MX_ERROR_OK;

    size = readDWord(err, buffer);
    MX_ERROR_CHECK(err);

    biWidth = readDWord(err, buffer);
    MX_ERROR_CHECK(err);

    biHeight = readDWord(err, buffer);
    MX_ERROR_CHECK(err);

    biPlanes = readWord(err, buffer);
    MX_ERROR_CHECK(err);

    biBitCount = readWord(err, buffer);
    MX_ERROR_CHECK(err);

    biCompression = readDWord(err, buffer);
    MX_ERROR_CHECK(err);

    biSizeImage = readDWord(err, buffer);
    MX_ERROR_CHECK(err);

    biXPelsPerMeter = readDWord(err, buffer);
    MX_ERROR_CHECK(err);

    biYPelsPerMeter = readDWord(err, buffer);
    MX_ERROR_CHECK(err);

    biClrUsed = readDWord(err, buffer);
    MX_ERROR_CHECK(err);

    biClrImportant = readDWord(err, buffer);
    MX_ERROR_CHECK(err);

abort:;
}
