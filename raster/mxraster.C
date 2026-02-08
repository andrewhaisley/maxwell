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
 * MODULE : mxraster.C
 *
 * AUTHOR : David Miller
 *
 * DESCRIPTION:
 * Module mxraster.C
 *
 *
 *
 */

#include "mxraster.h"
#include <netinet/in.h>

static rasdecompFunc rasdecompFuncs[COMP_NCOMPS] = { NULL, &removeValueDecomp, &rasterunPackbits,
    rasterZuncomp };
static rascompFunc rascompFuncs[COMP_NCOMPS] = { NULL, &removeValueComp, &rasterPackbits,
    rasterZcomp };
char mxraster::stringBuffer[100];

/*-------------------------------------------------
 * FUNCTION: mxraster::indexReadSize
 *
 * DESCRIPTION:
 *
 *
 */

inline int mxraster::indexReadSize()
{
    return (comp == COMP_UNCOMPRESSED) ? 1 : 2;
}

/*-------------------------------------------------
 * FUNCTION: mxraster::indexSize
 *
 * DESCRIPTION:
 *
 *
 */

inline int mxraster::indexSize()
{
    return totalTiles() * indexReadSize();
}

/*-------------------------------------------------
 * FUNCTION: mxraster::indexReadByteSize
 *
 * DESCRIPTION:
 *
 *
 */

inline int mxraster::indexReadByteSize()
{
    return indexReadSize() * sizeof(int32);
}

/*-------------------------------------------------
 * FUNCTION: mxraster::indexByteSize
 *
 * DESCRIPTION:
 *
 *
 */

inline int mxraster::indexByteSize()
{
    return indexSize() * sizeof(int32);
}

/*-------------------------------------------------
 * FUNCTION: mxraster::idStr
 *
 * DESCRIPTION:
 *
 *
 */

inline const char* mxraster::idStr(const char* prop)
{
    sprintf(stringBuffer, "%s%d", prop, numRaster);

    return stringBuffer;
}

/*-------------------------------------------------
 * FUNCTION: mxraster::get_serialised_size
 *
 * DESCRIPTION: Get a raster from a database object
 *
 *
 */

uint32 mxraster::get_serialised_size(int& err)
{
    int sizeR;
    int icolour;
    mx_colour* thisColour;

    sizeR = (10 + nplanes) * SLS_INT32;
    sizeR += indexByteSize();
    sizeR += SLS_FLOAT * 2 + SLS_BOOL;

    for (icolour = 0; icolour < numColours; icolour++) {
        thisColour = colours + icolour;

        sizeR += thisColour->get_serialised_size(err);
        MX_ERROR_CHECK(err);
    }

    sizeR += object->get_int(err, idStr("hwm"));
    MX_ERROR_CHECK(err);

    return sizeR;

abort:
    return 0;
}

/*-------------------------------------------------
 * FUNCTION: mxraster::serialise
 *
 * DESCRIPTION: Get a raster from a database object
 *
 *
 */

void mxraster::serialise(int& err,
    unsigned char** buffer)
{
    int iplane;
    int icolour;
    int dataSize;
    mx_colour* thisColour;
    int32* indexData;
    char* dataData;

    serialise_int32(pack, buffer);
    serialise_int32(data, buffer);
    serialise_int32(size.x, buffer);
    serialise_int32(size.y, buffer);
    serialise_int32(tileSize.x, buffer);
    serialise_int32(tileSize.y, buffer);
    serialise_int32(nplanes, buffer);
    serialise_bool(hasStoredMMsize, buffer);
    serialise_float(storedMMsize.x, buffer);
    serialise_float(storedMMsize.y, buffer);

    for (iplane = 0; iplane < nplanes; iplane++)
        serialise_int32(*(sigbits + iplane), buffer);

    serialise_int32(comp, buffer);
    serialise_int32(numColours, buffer);

    for (icolour = 0; icolour < numColours; icolour++) {
        thisColour = colours + icolour;

        thisColour->serialise(err, buffer);
        MX_ERROR_CHECK(err);
    }

    indexData = (int32*)object->get_blob_readonly(err,
        idStr("index"), 0,
        indexByteSize());
    MX_ERROR_CHECK(err);

    serialise_int32_a(indexData, indexSize(), buffer);

    dataSize = object->get_int(err, idStr("hwm"));
    MX_ERROR_CHECK(err);

    serialise_int32(dataSize, buffer);

    dataData = (char*)object->get_blob_readonly(err,
        idStr("data"), 0,
        dataSize);
    MX_ERROR_CHECK(err);

    serialise_char_a(dataData, dataSize, buffer);

abort:
    return;
}

/*-------------------------------------------------
 * FUNCTION: mxraster::unserialise
 *
 * DESCRIPTION: Get a raster from a database object
 *
 *
 */

void mxraster::unserialise(int& err,
    unsigned char** buffer)
{
    int iplane;
    int icolour;
    int32 dataSize;
    mx_colour* thisColour;
    mx_ipoint isize;
    mx_ipoint itileSize;
    RAST_PACK ipack;
    RAST_DATA idata;
    int32 inplanes;
    int32 isigbits[RAST_NUMPLANES];
    int32 inumColours;
    mx_colour* icolours;
    COMP_TYPE icomp;
    int32 val32;
    bool ihasStoredMMsize;
    float valfloat;
    mx_point istoredMMsize;
    unsigned char transferData[16384];
    int nbytesToTransfer, nbytesOffset, thisTransferSize;

    unserialise_int32(val32, buffer);
    ipack = (RAST_PACK)val32;

    unserialise_int32(val32, buffer);
    idata = (RAST_DATA)val32;

    unserialise_int32(val32, buffer);
    isize.x = val32;

    unserialise_int32(val32, buffer);
    isize.y = val32;

    unserialise_int32(val32, buffer);
    itileSize.x = val32;

    unserialise_int32(val32, buffer);
    itileSize.y = val32;

    unserialise_int32(val32, buffer);
    inplanes = val32;

    unserialise_bool(ihasStoredMMsize, buffer);
    unserialise_float(valfloat, buffer);
    istoredMMsize.x = valfloat;
    unserialise_float(valfloat, buffer);
    istoredMMsize.y = valfloat;

    for (iplane = 0; iplane < inplanes; iplane++)
        unserialise_int32(*(isigbits + iplane), buffer);

    unserialise_int32(val32, buffer);
    icomp = (COMP_TYPE)val32;

    unserialise_int32(val32, buffer);
    inumColours = val32;

    icolours = NULL;

    if (inumColours > 0) {
        icolours = new mx_colour[inumColours];

        for (icolour = 0; icolour < inumColours; icolour++) {
            thisColour = icolours + icolour;

            thisColour->unserialise(err, buffer);
            MX_ERROR_CHECK(err);
        }
    }

    setRaster(err, *object, numRaster, isize, itileSize,
        ihasStoredMMsize, istoredMMsize, ipack,
        idata, inplanes, isigbits,
        inumColours, icolours,
        icomp, savedCacheType);
    MX_ERROR_CHECK(err);

    delete[] icolours;

    nbytesToTransfer = indexByteSize();
    nbytesOffset = 0;

    while (nbytesToTransfer > 0) {
        thisTransferSize = (nbytesToTransfer > 16384) ? 16384 : nbytesToTransfer;

        unserialise_int32_a((int32*)transferData,
            nbytesToTransfer / sizeof(int32), buffer);

        object->set_blob_data(err, idStr("index"), nbytesOffset, thisTransferSize,
            (unsigned char*)transferData);
        MX_ERROR_CHECK(err);

        nbytesToTransfer -= thisTransferSize;
        nbytesOffset += thisTransferSize;
    }

    unserialise_int32(dataSize, buffer);

    object->set_int(err, idStr("hwm"), dataSize);
    MX_ERROR_CHECK(err);

    object->set_blob_size(err, idStr("data"), dataSize);
    MX_ERROR_CHECK(err);

    nbytesToTransfer = dataSize;
    nbytesOffset = 0;

    while (nbytesToTransfer > 0) {
        thisTransferSize = (nbytesToTransfer > 16384) ? 16384 : nbytesToTransfer;

        unserialise_char_a((char*)transferData, thisTransferSize, buffer);

        object->set_blob_data(err, idStr("data"), nbytesOffset, thisTransferSize,
            (unsigned char*)transferData);
        MX_ERROR_CHECK(err);

        nbytesToTransfer -= thisTransferSize;
        nbytesOffset += thisTransferSize;
    }

    if (cacheType == RCACHE_NONE) {
        cacheType = defaultCacheType(comp, RCACHE_DEFAULT);

        createCache(err, cacheType);
        MX_ERROR_CHECK(err);
    }

abort:
    return;
}

/*-------------------------------------------------
 * FUNCTION: mxraster::mxraster
 *
 * DESCRIPTION: Get a raster from a database object
 *
 *
 */

mxraster::mxraster(int& err,
    mx_db_object& robj,
    int nraster,
    bool alreadyExists,
    RAST_CACHE cacheType)
    : raster(err)
{
    int iplane;
    int icolour;
    mx_colour* thisColour;

    MX_ERROR_CHECK(err);

    numRaster = nraster;
    object = &robj;

    id = robj.get_db_id(err);
    MX_ERROR_CHECK(err);

    if (alreadyExists) {
        pack = (RAST_PACK)robj.get_int(err, idStr("pack"));
        MX_ERROR_CHECK(err);

        data = (RAST_DATA)robj.get_int(err, idStr("data"));
        MX_ERROR_CHECK(err);

        size.x = robj.get_int(err, idStr("sizex"));
        MX_ERROR_CHECK(err);

        size.y = robj.get_int(err, idStr("sizey"));
        MX_ERROR_CHECK(err);

        tileSize.x = robj.get_int(err, idStr("tilesizex"));
        MX_ERROR_CHECK(err);

        tileSize.y = robj.get_int(err, idStr("tilesizey"));
        MX_ERROR_CHECK(err);

        hasStoredMMsize = robj.get_int(err, idStr("hasStoredMMsize"));
        MX_ERROR_CHECK(err);

        storedMMsize.x = robj.get_float(err, idStr("storedMMsizex"));
        MX_ERROR_CHECK(err);

        storedMMsize.y = robj.get_float(err, idStr("storedMMsizey"));
        MX_ERROR_CHECK(err);

        nplanes = robj.get_int(err, idStr("nplanes"));
        MX_ERROR_CHECK(err);

        for (iplane = 0; iplane < nplanes; iplane++) {
            sigbits[iplane] = robj.get_int_array_item(err,
                idStr("sigbits"),
                iplane);
            MX_ERROR_CHECK(err);
        }

        comp = (COMP_TYPE)robj.get_int(err, idStr("comp"));
        MX_ERROR_CHECK(err);

        numColours = robj.get_int(err, idStr("ncolours"));
        MX_ERROR_CHECK(err);

        if (numColours > 0) {
            colours = new mx_colour[numColours];

            for (icolour = 0; icolour < numColours; icolour++) {
                thisColour = colours + icolour;

                thisColour->red = robj.get_int_array_item(err,
                    idStr("colours"),
                    icolour * 3);
                MX_ERROR_CHECK(err);

                thisColour->green = robj.get_int_array_item(err,
                    idStr("colours"),
                    icolour * 3 + 1);
                MX_ERROR_CHECK(err);

                thisColour->blue = robj.get_int_array_item(err,
                    idStr("colours"),
                    icolour * 3 + 2);
                MX_ERROR_CHECK(err);

                thisColour->name = NULL;
            }
        }
    }

    cacheType = defaultCacheType(comp, cacheType);

    createCache(err, cacheType);
    MX_ERROR_CHECK(err);

    savedCacheType = cacheType;
abort:
    return;
}

/*-------------------------------------------------
 * FUNCTION: mxraster::setRaster
 *
 * DESCRIPTION: Create a new mxraster
 *
 *
 */

void mxraster::setRaster(int& err,
    mx_db_object& robj,
    int nraster,
    mx_ipoint& isize,
    mx_ipoint& itileSize,
    bool ihasStoredMMsize,
    mx_point& istoredMMsize,
    RAST_PACK ipack,
    RAST_DATA idata,
    int32 inplanes,
    int32* isigbits,
    int32 inumColours,
    mx_colour* icolours,
    COMP_TYPE icomp,
    RAST_CACHE cacheType)
{
    int iplane;
    int icolour;
    mx_colour* thisColour;
    int32* thisColourArray;
    int32 colourArray[256 * 3];

    numRaster = nraster;
    object = &robj;

    id = robj.get_db_id(err);
    MX_ERROR_CHECK(err);

    pack = ipack;
    robj.set_int(err, idStr("pack"), pack);
    MX_ERROR_CHECK(err);

    data = idata;
    robj.set_int(err, idStr("data"), data);
    MX_ERROR_CHECK(err);

    size = isize;
    robj.set_int(err, idStr("sizex"), size.x);
    MX_ERROR_CHECK(err);
    robj.set_int(err, idStr("sizey"), size.y);
    MX_ERROR_CHECK(err);

    tileSize = itileSize;
    robj.set_int(err, idStr("tilesizex"), tileSize.x);
    MX_ERROR_CHECK(err);
    robj.set_int(err, idStr("tilesizey"), tileSize.y);
    MX_ERROR_CHECK(err);

    hasStoredMMsize = ihasStoredMMsize;
    robj.set_int(err, idStr("hasStoredMMsize"), hasStoredMMsize);
    MX_ERROR_CHECK(err);

    setSize(err, istoredMMsize);
    MX_ERROR_CHECK(err);

    nplanes = inplanes;
    robj.set_int(err, idStr("nplanes"), nplanes);
    MX_ERROR_CHECK(err);

    for (iplane = 0; iplane < nplanes; iplane++)
        sigbits[iplane] = isigbits[iplane];

    robj.set_int_array(err, idStr("sigbits"), isigbits, nplanes);
    MX_ERROR_CHECK(err);

    numColours = inumColours;
    robj.set_int(err, idStr("ncolours"), numColours);
    MX_ERROR_CHECK(err);

    if (numColours > 0) {
        colours = new mx_colour[numColours];

        for (int i = 0; i < numColours; i++) {
            colours[i] = icolours[i];
        }

        thisColourArray = colourArray;

        for (icolour = 0; icolour < numColours; icolour++) {
            thisColour = colours + icolour;

            *thisColourArray++ = thisColour->red;
            *thisColourArray++ = thisColour->green;
            *thisColourArray++ = thisColour->blue;
        }

        robj.set_int_array(err, idStr("colours"), colourArray, numColours * 3);
        MX_ERROR_CHECK(err);
    }

    comp = icomp;
    robj.set_int(err, idStr("comp"), comp);
    MX_ERROR_CHECK(err);

    // Create the index data
    robj.create_blob(err, idStr("index"));
    MX_ERROR_CHECK(err);

    robj.set_blob_size(err, idStr("index"), indexByteSize());
    MX_ERROR_CHECK(err);

    // Create the data blob

    robj.create_blob(err, idStr("data"));
    MX_ERROR_CHECK(err);

    // set the zero high water mark
    object->set_int(err, idStr("hwm"), 0);
    MX_ERROR_CHECK(err);

    cacheType = defaultCacheType(comp, cacheType);

    createCache(err, cacheType);
    MX_ERROR_CHECK(err);

    return;
abort:
    return;
}

/*-------------------------------------------------
 * FUNCTION: mxraster::mxraster
 *
 * DESCRIPTION: Create a new mxraster
 *
 *
 */

mxraster::mxraster(int& err,
    mx_db_object& robj,
    int nraster,
    mx_ipoint& isize,
    mx_ipoint& itileSize,
    bool ihasStoredMMsize,
    mx_point& istoredMMsize,
    RAST_PACK ipack,
    RAST_DATA idata,
    int32 inplanes,
    int32* isigbits,
    int32 numColours,
    mx_colour* colours,
    COMP_TYPE icomp,
    RAST_CACHE cacheType)
    : raster(err)
{
    MX_ERROR_CHECK(err);

    setRaster(err, robj, nraster, isize, itileSize,
        ihasStoredMMsize, istoredMMsize,
        ipack, idata, inplanes, isigbits,
        numColours, colours, icomp, cacheType);
    MX_ERROR_CHECK(err);

    return;
abort:
    return;
}

/*-------------------------------------------------
 * FUNCTION: mxraster::mxraster
 *
 * DESCRIPTION: Create a new mxraster from a raster
 *
 */

mxraster::mxraster(int& err,
    mx_db_object& robj,
    int nraster,
    raster& iraster,
    COMP_TYPE icomp,
    RAST_CACHE cacheType)
    : raster(err)
{
    mx_ipoint insize;
    mx_irect extent;
    mx_ipoint tsize = iraster.getSize();
    mx_ipoint ttileSize = iraster.getTileSize();
    mx_point tstoredMMsize = iraster.getStoredMMsize();

    MX_ERROR_CHECK(err);

    // Set parameters

    setRaster(err, robj, nraster, tsize, ttileSize,
        iraster.getHasStoredMMsize(), tstoredMMsize,
        iraster.getPack(), iraster.getData(),
        iraster.getPlanes(), iraster.getSigbits(),
        iraster.getNumColours(), iraster.getColours(), icomp, cacheType);

    // Copy data
    insize = getSize();
    extent.xb = 0;
    extent.yb = 0;
    extent.xt = insize.x - 1;
    extent.yt = insize.y - 1;

    iraster.copyData(err, *this, extent);
    MX_ERROR_CHECK(err);

    return;
abort:
    return;
}

/*-------------------------------------------------
 * FUNCTION: mxraster::readIndex
 *
 * DESCRIPTION:
 *
 *
 */

void mxraster::readIndex(int& err,
    mx_ipoint& tile,
    int32* indexData,
    bool& unknown)
{
    int tileId, indexOffset;

    err = MX_ERROR_OK;

    tileId = tileIndex(tile);

    // Get offset into index blob
    indexOffset = indexReadByteSize() * tileId;

    // Get the index data
    object->get_blob_data(err, idStr("index"), indexOffset,
        indexReadByteSize(),
        (unsigned char*)indexData);
    MX_ERROR_CHECK(err);

    indexData[0] = ntohl(indexData[0]);
    indexData[1] = ntohl(indexData[1]);

    // Offset of zero is unknown
    unknown = (indexData[0] == 0);

    // Get real offset
    (indexData[0])--;

    // set tile size if the data is not compressed
    if (comp == COMP_UNCOMPRESSED)
        indexData[1] = tileByteSize();

    goto exit;
abort:
exit:
    return;
}

/*-------------------------------------------------
 * FUNCTION: mxraster::writeIndex
 *
 * DESCRIPTION:
 *
 *
 */

void mxraster::writeIndex(int& err,
    mx_ipoint& tile,
    int32* indexData)
{
    int tileId, indexOffset;

    err = MX_ERROR_OK;

    tileId = tileIndex(tile);

    indexOffset = indexReadByteSize() * tileId;

    // Add one (so zero size means non-existence)

    (indexData[0])++;

    // convert to network format
    indexData[0] = htonl(indexData[0]);
    indexData[1] = htonl(indexData[1]);

    // write the index data
    object->set_blob_data(err, idStr("index"), indexOffset,
        indexReadByteSize(),
        (unsigned char*)indexData);
    MX_ERROR_CHECK(err);

    indexData[0] = ntohl(indexData[0]);
    indexData[1] = ntohl(indexData[1]);

    (indexData[0])--;

    goto exit;
abort:
exit:
    return;
}

/*-------------------------------------------------
 * FUNCTION: mxraster::getTile
 *
 * DESCRIPTION: Get a tile from the raster
 *
 *
 */

unsigned char* mxraster::getTile(int& err,
    unsigned char* thisBuffer,
    bool getForEdit,
    bool& unknown,
    mx_ipoint& tile)
{
    int32 indexData[2];
    int tilebytesize = tileByteSize();

    static unsigned char* compData = NULL;
    static int compDataSize = 0;

    err = MX_ERROR_OK;

    // Get index data for this tile
    readIndex(err, tile, indexData, unknown);
    MX_ERROR_CHECK(err);

    if (unknown) {
        // Set zero buffer - if getting for edit the tile
        // is set to exist
        if (thisBuffer != NULL)
            memset(thisBuffer, 0, tilebytesize);
        if (getForEdit)
            unknown = FALSE;
        return thisBuffer;
    }

    if (comp == COMP_UNCOMPRESSED || (indexData[1] >= tilebytesize)) {
        // If uncompressed or failed to compress do a straight read
        object->get_blob_data(err, idStr("data"),
            indexData[0], tilebytesize, thisBuffer);
        MX_ERROR_CHECK(err);
    } else {
        // read into a temporary buffer for decompression into main buffer

        if (compDataSize < tilebytesize) {
            delete[] compData;
            compData = new unsigned char[tilebytesize];

            compDataSize = tilebytesize;
        }

        // Get the compressed data
        object->get_blob_data(err, idStr("data"),
            indexData[0], indexData[1], compData);
        MX_ERROR_CHECK(err);

        // decompress the dara
        (*rasdecompFuncs[comp])(err, thisBuffer, tilebytesize,
            compData, indexData[1], NULL);
        MX_ERROR_CHECK(err);
    }

abort:
    return thisBuffer;
}

/*-------------------------------------------------
 * FUNCTION: mxraster::putTile
 *
 * DESCRIPTION:
 *
 *
 */

void mxraster::putTile(int& err,
    unsigned char* thisBuffer,
    mx_ipoint& tile)
{
    int32 indexData[2];
    int tilebytesize = tileByteSize();
    bool unknown;
    int newSize, outputSize;

    unsigned char* outputBuffer;

    static unsigned char* compData = NULL;
    static int compDataSize = 0;

    err = MX_ERROR_OK;

    if (comp != COMP_UNCOMPRESSED) {
        if (compDataSize < tilebytesize) {
            delete[] compData;
            compData = new unsigned char[tilebytesize];

            compDataSize = tilebytesize;
        }

        // Compress the data
        outputSize = (*rascompFuncs[comp])(err, thisBuffer, tilebytesize,
            compData, NULL);
        MX_ERROR_CHECK(err);

        // Compressed succesfully ?
        if (outputSize >= tilebytesize) {
            outputBuffer = thisBuffer;
            outputSize = tilebytesize;
        } else {
            outputBuffer = compData;
        }
    } else {
        // Dont compress
        outputBuffer = thisBuffer;
        outputSize = tilebytesize;
    }

    // get index data
    readIndex(err, tile, indexData, unknown);
    MX_ERROR_CHECK(err);

    // If unknown or slot is too small put at the end

    if (unknown || indexData[1] < outputSize) {
        // get the high water mark
        indexData[0] = object->get_int(err, idStr("hwm"));
        MX_ERROR_CHECK(err);

        indexData[1] = outputSize;

        newSize = indexData[0] + indexData[1];
        object->set_blob_size(err, idStr("data"), newSize);
        MX_ERROR_CHECK(err);

        // set the high water mark
        object->set_int(err, idStr("hwm"), newSize);
        MX_ERROR_CHECK(err);
    } else {
        // write to same place with new size
        indexData[1] = outputSize;
    }

    // write the new index information
    writeIndex(err, tile, indexData);
    MX_ERROR_CHECK(err);

    object->set_blob_data(err, idStr("data"),
        indexData[0], indexData[1], outputBuffer);
    MX_ERROR_CHECK(err);

    return;
abort:
    return;
}

/*-------------------------------------------------
 * FUNCTION: mxraster::setSize
 *
 * DESCRIPTION: Calculate the resolution for a
 * display raster
 *
 */

void mxraster::setSize(int& err,
    mx_point& areaSize)
{
    err = MX_ERROR_OK;

    raster::setSize(err,
        areaSize);
    MX_ERROR_CHECK(err);

    object->set_float(err, idStr("storedMMsizex"), areaSize.x);
    MX_ERROR_CHECK(err);
    object->set_float(err, idStr("storedMMsizey"), areaSize.y);
    MX_ERROR_CHECK(err);

    return;
abort:
    return;
}
