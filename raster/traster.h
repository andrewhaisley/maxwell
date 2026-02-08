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
 * MODULE : traster.h
 *
 * AUTHOR : David Miller
 *
 * This file
 *
 * DESCRIPTION:
 * Module traster.h
 *
 *
 *
 *
 * $id$
 * $log$
 *
 */

#ifndef TRASTER_H
#define TRASTER_H

#include "raster.h"

/*-------------------------------------------------
 * CLASS: mx_rtransform_data
 *
 * DESCRIPTION:
 *
 *
 */

class mx_rtransform_data {
};

/*-------------------------------------------------
 * CLASS: mx_rshift_data
 *
 * DESCRIPTION:
 *
 *
 */

class mx_rshift_data : public mx_rtransform_data {
public:
    mx_rshift_data(int ishift, LUT_VALUE* ilut)
    {
        shift = ishift;
        lut = ilut;
    };
    int shift;
    LUT_VALUE* lut;
};

/*-------------------------------------------------
 * CLASS: mx_rshift_data
 *
 * DESCRIPTION:
 *
 *
 */

class mx_rdither_data : public mx_rtransform_data {
public:
    mx_rdither_data(int rowSize,
        int itoggle,
        bool iuniform27,
        int numColours,
        LUT_VALUE* lut);

    ~mx_rdither_data();

    int* thisError;
    int* nextError;
    bool uniform27;
    int toggle;
    int* colourData;
    LUT_VALUE* lut;
};
/*-------------------------------------------------
 * CLASS: mx_tile_array
 *
 * DESCRIPTION:
 *
 *
 */

class mx_tile_array {
public:
    mx_ipoint tsz;
    mx_ipoint ntiles;
    mx_ipoint tl;
    mx_ipoint br;
    mx_ipoint ptl;
    mx_ipoint oft;
    int pixx; // X pixel in tile
    int toff; // Tile offset
    int npb;
    int offs;

    int ntotalTiles;
    unsigned char** tiles;
    rasItem** items;

    unsigned char* ptr;

    unsigned char* getPtr(mx_ipoint& pt);
    unsigned char* getNextPtr();
    unsigned char* addPtr(int add);

protected:
private:
};

/*-------------------------------------------------
 * FUNCTION: mx_tile_array::getPtr
 *
 * DESCRIPTION: Get a pointer to a pixel in a tile
 *              array
 *
 */

inline unsigned char* mx_tile_array::getPtr(mx_ipoint& pt)
{
    // Make offset
    oft = pt - ptl;

    // Store the x offset
    pixx = oft.x % tsz.x;

    // Store the tileoff
    toff = (oft.y / tsz.y) * ntiles.x + (oft.x / tsz.x);

    // Store the start offset
    offs = (oft.y % tsz.y) * npb * tsz.x;

    // Get the pointer in the tile
    ptr = tiles[toff] + offs + pixx * npb;

    return ptr;
}

/*-------------------------------------------------
 * FUNCTION: mx_tile_array::getNextPtr
 *
 * DESCRIPTION:
 *
 *
 */

inline unsigned char* mx_tile_array::getNextPtr()
{
    pixx++;
    if (pixx == tsz.x) {
        toff++;
        pixx = 0;
        ptr = tiles[toff] + offs;
    } else {
        ptr += npb;
    }
    return ptr;
}

/*-------------------------------------------------
 * FUNCTION: mx_tile_array::addPtr
 *
 * DESCRIPTION:
 *
 *
 */

inline unsigned char* mx_tile_array::addPtr(int add)
{
    pixx += add;

    if (pixx >= tsz.x) {
        while (pixx >= tsz.x) {
            toff++;
            pixx -= tsz.x;
        }
        ptr = tiles[toff] + offs + pixx * npb;
    } else {
        ptr += (npb * add);
    }
    return ptr;
}

typedef void (*rasterTransformFunction)(int& err,
    raster* output,
    raster* base,
    mx_tile_array* tileArray,
    mx_irect& out,
    unsigned char* buffer,
    mx_rtransform_data* data);

typedef void (*rasterDataFunction)(int& err,
    raster* output,
    raster* base,
    mx_irect& in,
    unsigned char* inbuffer,
    unsigned char* outbuffer,
    mx_rtransform_data* data);

void shiftRaster(int& err,
    raster* output,
    raster* base,
    mx_irect& in,
    unsigned char* inbuffer,
    unsigned char* outbuffer,
    mx_rtransform_data* data);

void ditherRaster(int& err,
    raster* output,
    raster* base,
    mx_irect& in,
    unsigned char* inbuffer,
    unsigned char* outbuffer,
    mx_rtransform_data* data);

void lutRaster(int& err,
    raster* output,
    raster* base,
    mx_irect& in,
    unsigned char* inbuffer,
    unsigned char* outbuffer,
    mx_rtransform_data* data);

void packRaster(int& err,
    raster* output,
    raster* base,
    mx_irect& in,
    unsigned char* inbuffer,
    mx_irect& out,
    unsigned char* outbuffer,
    mx_rtransform_data* data);

void shiftRaster(int& err,
    raster* output,
    raster* base,
    mx_irect& in,
    unsigned char* inbuffer,
    mx_irect& out,
    unsigned char* outbuffer,
    mx_rtransform_data* data);

void unpackRaster(int& err,
    raster* output,
    raster* base,
    mx_irect& in,
    unsigned char* inbuffer,
    mx_irect& out,
    unsigned char* outbuffer,
    mx_rtransform_data* data);

void rescaleRaster(int& err,
    raster* output,
    raster* base,
    mx_tile_array* tileArray,
    mx_irect& out,
    unsigned char* buffer,
    mx_rtransform_data* data);

void isubsampleRaster(int& err,
    raster* output,
    raster* base,
    mx_tile_array* tileArray,
    mx_irect& out,
    unsigned char* buffer,
    mx_rtransform_data* data);

void ireplicateRaster(int& err,
    raster* output,
    raster* base,
    mx_tile_array* tileArray,
    mx_irect& out,
    unsigned char* buffer,
    mx_rtransform_data* data);

/*-------------------------------------------------
 * CLASS: traster
 *
 * DESCRIPTION: A transformation raster
 *
 *
 */

class traster : public raster {
public:
    traster(int& err,
        raster* base, bool deleteSource, RAST_CACHE cacheType = RCACHE_DICT);
    ~traster();
    unsigned char* getTile(int& err,
        unsigned char* thisBuffer,
        bool getForEdit,
        bool& unknown,
        mx_ipoint& tile);
    void setSize(int& err, mx_ipoint& isize);
    void setTileSize(int& err, mx_ipoint& itilesize);
    void setPack(int& err, RAST_PACK pack);

    void setLut(int& err, LUT_VALUE* lut, int lutSize);
    void setSpecialData(int& err, bool forTrans, mx_rtransform_data* data);

    void setTransFunction(int& err, rasterTransformFunction transFunc);
    void setDataFunction(int& err, rasterDataFunction dataFunc);

    void getScaleChangeFunction(int& err, double fac1, double fac2);
    void getDataChangeFunction(int& err);

    void setSigbits(int& err, int32_t inplanes, int32_t* isigbits);
    void setData(int& err, RAST_DATA idata);
    void setColours(int& err, int inumColours, mx_colour* icolours);

    LUT_VALUE* lut;

protected:
private:
    rasterTransformFunction transformFunction;
    rasterDataFunction dataFunction;
    unsigned char* storeBuffer;
    bool used;
    raster* base;
    bool deleteSource;
    mx_rtransform_data* transSpecialData;
    mx_rtransform_data* dataSpecialData;
};

#endif
