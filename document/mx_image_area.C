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
 * MODULE/CLASS : mx_image_area
 *
 * AUTHOR : Andrew Haisley
 *
 *
 *
 * DESCRIPTION:
 *
 *
 *
 */

#include <mx.h>

#include <mx_image_area.h>
#include <mx_img_paint.h>
#include <mxraster.h>

#include <sys/resource.h>
#include <sys/time.h>
#include <unistd.h>

int currentMem(char* str)
{
    static long int lastValue = 0;
    long int newValue;
    long int diff;

    struct rlimit lim;
    getrlimit(RLIMIT_DATA, &lim);

    newValue = (long int)sbrk(0);
    diff = newValue - lastValue;
    if (diff < 0)
        diff = -diff;

    lastValue = newValue;

    return 0;
}

mx_image_area::mx_image_area(uint32 docid,
    mx_paragraph_style* s,
    bool is_controlled)
    : mx_area(docid, s, is_controlled)
{
    // get a handle on a suitable painter
    painter = mx_image_area_painter::painter();

    init();
}

mx_image_area::mx_image_area(uint32 docid, uint32 oid, bool is_controlled)
    : mx_area(docid, oid, is_controlled)
{
    mx_ipoint areaPixelSize;

    // get a handle on a suitable painter
    painter = mx_image_area_painter::painter();

    init();

    // get the type,filename and whether a subsampled view exists
    readBasics(error);
    MX_ERROR_CHECK(error);

    if (subsampledExists) {
        subsampledRaster = new mxraster(error, *this, MX_RASTER_SUBSAMPLED,
            TRUE, RCACHE_DEFAULT);
        MX_ERROR_CHECK(error);
    }

    if (fullImportExists) {
        fullImportRaster = new mxraster(error, *this, MX_RASTER_FULL,
            TRUE, RCACHE_DEFAULT);
        MX_ERROR_CHECK(error);
    }

    return;
abort:
    global_error_trace->print();
    MX_ERROR_CLEAR(error);
}

mx_image_area::mx_image_area(const mx_image_area& a)
    : mx_area(*((mx_area*)&a))
{
    mxraster* ras;

    MX_ERROR_CHECK(error);

    init();

    // get a handle on a suitable painter
    painter = mx_image_area_painter::painter();

    // copy the basics
    createBasics(error,
        a.filename,
        a.type,
        a.subsampledExists,
        a.fullImportExists,
        a.numColours);
    MX_ERROR_CHECK(error);

    // copy the subsampled raster
    if (a.subsampledExists) {
        ras = a.subsampledRaster;

        subsampledRaster = new mxraster(error, *this, MX_RASTER_SUBSAMPLED, *ras,
            ras->getCompression(),
            ras->getCacheType());
        MX_ERROR_CHECK(error);
    }

    // copy the full raster
    if (a.fullImportExists) {
        ras = a.fullImportRaster;

        fullImportRaster = new mxraster(error, *this, MX_RASTER_FULL, *ras,
            ras->getCompression(),
            ras->getCacheType());
        MX_ERROR_CHECK(error);
    }

    return;
abort:
    global_error_trace->print();
    MX_ERROR_CLEAR(error);
}

mx_image_area::~mx_image_area()
{
    if (fullImportExists)
        delete fullImportRaster;
    if (subsampledExists)
        delete subsampledRaster;

    delete inputRaster;

    deleteDisplayRaster();
    deletePrintRaster();

    delete[] filename;
}

void mx_image_area::serialise(int& err, uint8** buffer)
{
    err = MX_ERROR_OK;

    mx_area::serialise(err, buffer);
    MX_ERROR_CHECK(err);

    serialise_int32(type, buffer);
    serialise_bool(subsampledExists, buffer);
    serialise_bool(fullImportExists, buffer);
    serialise_int32(numColours, buffer);
    serialise_string(filename, buffer);

    if (subsampledExists) {
        subsampledRaster->serialise(err, buffer);
        MX_ERROR_CHECK(err);
    }

    if (fullImportExists) {
        fullImportRaster->serialise(err, buffer);
        MX_ERROR_CHECK(err);
    }

    return;
abort:
    return;
}

void mx_image_area::unserialise(int& err, uint8** buffer)
{
    int32 val32;
    char ifilename[1000];
    mx_image_area_t itype;
    bool isubsampledExists, ifullImportExists, inumColours;

    err = MX_ERROR_OK;

    mx_area::unserialise(err, buffer);
    MX_ERROR_CHECK(err);

    unserialise_int32(val32, buffer);
    itype = (mx_image_area_t)val32;

    unserialise_bool(isubsampledExists, buffer);
    unserialise_bool(ifullImportExists, buffer);
    unserialise_int32(val32, buffer);
    inumColours = val32;

    unserialise_string(ifilename, buffer);

    createBasics(err, ifilename, itype, isubsampledExists,
        ifullImportExists, inumColours);
    MX_ERROR_CHECK(err);

    if (subsampledExists) {
        subsampledRaster = new mxraster(err, *this, MX_RASTER_SUBSAMPLED,
            FALSE, RCACHE_NONE);
        MX_ERROR_CHECK(err);

        subsampledRaster->unserialise(err, buffer);
        MX_ERROR_CHECK(err);
    }

    if (fullImportExists) {
        fullImportRaster = new mxraster(err, *this, MX_RASTER_FULL,
            FALSE, RCACHE_NONE);
        MX_ERROR_CHECK(err);

        fullImportRaster->unserialise(err, buffer);
        MX_ERROR_CHECK(err);
    }

    return;
abort:
    return;
}

uint32 mx_image_area::get_serialised_size(int& err)
{
    int size;

    err = MX_ERROR_OK;

    size = mx_area::get_serialised_size(err);
    MX_ERROR_CHECK(err);

    size += (2 * SLS_INT32 + (2 * SLS_BOOL) + SLS_STRING(filename));

    if (subsampledExists) {
        size += subsampledRaster->get_serialised_size(err);
        MX_ERROR_CHECK(err);
    }

    if (fullImportExists) {
        size += fullImportRaster->get_serialised_size(err);
        MX_ERROR_CHECK(err);
    }

    return size;
abort:
    return 0;
}

void mx_image_area::move_to_document(int& err, mx_document* new_doc)
{
    mx_area::move_to_document(err, new_doc);
    MX_ERROR_CHECK(err);

    init();

    // re-read stuff from the database since we are now using a new one
    readBasics(err);
    MX_ERROR_CHECK(err);

    if (subsampledExists) {
        subsampledRaster = new mxraster(err, *this, MX_RASTER_SUBSAMPLED,
            TRUE, RCACHE_DEFAULT);
        MX_ERROR_CHECK(err);
    }

    if (fullImportExists) {
        fullImportRaster = new mxraster(err, *this, MX_RASTER_FULL,
            TRUE, RCACHE_DEFAULT);
        MX_ERROR_CHECK(err);
    }
abort:;
}

void mx_image_area::uncache(int& err)
{
    err = MX_ERROR_OK;
}

uint32 mx_image_area::memory_size(int& err)
{
    return 0;
}

mx_area* mx_image_area::duplicate()
{
    mx_image_area* res;

    res = new mx_image_area(*this);

    return (mx_area*)res;
}
