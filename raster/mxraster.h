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
 * MODULE : mxraster.h
 *
 * AUTHOR : David Miller
 *
 * This file
 *
 * DESCRIPTION:
 * Module mxraster.h
 *
 *
 *
 *
 * $id$
 * $log$
 *
 */

#ifndef MXRASTER_H
#define MXRASTER_H

#include "mx_db_object.h"
#include "raster.h"

/*-------------------------------------------------
 * CLASS: mxraster
 *
 * DESCRIPTION: A raster based on a database object
 * e.g. an image area
 *
 */

class mxraster : public raster {
public:
    mxraster(int& err,
        mx_db_object& robj,
        int nraster,
        bool alreadyExists,
        RAST_CACHE cacheType = RCACHE_NONE);

    // Create a new raster
    mxraster(int& err,
        mx_db_object& robj,
        int nraster,
        mx_ipoint& isize,
        mx_ipoint& itileSize,
        bool ihasStoredResolution,
        mx_point& istoredResolution,
        RAST_PACK ipack,
        RAST_DATA idata,
        int32 inplanes,
        int32* isigbits,
        int32 inumColours,
        mx_colour* colours,
        COMP_TYPE icomp,
        RAST_CACHE cacheType = RCACHE_NONE);

    // Create from an existing raster
    mxraster(int& err,
        mx_db_object& robj,
        int nraster,
        raster& rast,
        COMP_TYPE comp,
        RAST_CACHE cacheType = RCACHE_NONE);

    inline COMP_TYPE getCompression() { return comp; };

    uint32 get_serialised_size(int& err);

    void serialise(int& err,
        unsigned char** buffer);

    void unserialise(int& err,
        unsigned char** buffer);

    unsigned char* getTile(int& err,
        unsigned char* thisBuffer,
        bool getForEdit,
        bool& unknown,
        mx_ipoint& tile);

    void putTile(int& err,
        unsigned char* buffer,
        mx_ipoint& tile);

    void setSize(int& err,
        mx_point& areaSize);

protected:
private:
    static char stringBuffer[100];

    const char* idStr(const char* prop);

    int indexSize();
    int indexReadSize();
    int indexByteSize();
    int indexReadByteSize();

    void setRaster(int& err,
        mx_db_object& robj,
        int nraster,
        mx_ipoint& isize,
        mx_ipoint& itileSize,
        bool ihasStoredResolution,
        mx_point& istoredResolution,
        RAST_PACK ipack,
        RAST_DATA idata,
        int32 inplanes,
        int32* isigbits,
        int32 numColours,
        mx_colour* colours,
        COMP_TYPE icomp,
        RAST_CACHE cacheType);

    void readIndex(int& err,
        mx_ipoint& tile,
        int32* indexData,
        bool& unknown);

    void writeIndex(int& err,
        mx_ipoint& tile,
        int32* indexData);

    // Object identifier
    mx_db_object* object;

    int numRaster;
    // The object identifier
    uint32 id;
    // The compression used in this raster
    COMP_TYPE comp;

    RAST_CACHE savedCacheType;
};

#endif
