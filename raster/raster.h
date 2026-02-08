#pragma once

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

#include <map>

#include <geometry.h>
#include <memory.h>
#include <mx_colour.h>

#define RAS_VALUE_SMALL 0.000001
#define RAS_VALUE_CSMALL 0.999999

#define RAST_NUMPLANES 3

// The pack type of the raster - this determines the type of
// data the raster data is packed into into. Initially only the
// two types bit and byte are used. Later developments may see
// raster types integer and real etc.

typedef enum {
    RPACK_BIT,
    RPACK_BYTE,
    RPACK_SHORT,
    RPACK_INT
} RAST_PACK;

// Datatype describes the type if the data. e.g a raster dataset
// can be binary, but this does not imply bit packed.

typedef enum {
    RDATA_BINARY,
    RDATA_GREYSCALE,
    RDATA_COLOUR,
    RDATA_RGB
} RAST_DATA;

typedef enum {
    RCACHE_NONE,
    RCACHE_TILE,
    RCACHE_DICT,
    RCACHE_DEFAULT
} RAST_CACHE;

typedef enum {
    COMP_UNCOMPRESSED,
    COMP_REMOVE,
    COMP_PACKBITS,
    COMP_ZCOMP,
    COMP_NCOMPS
} COMP_TYPE;

typedef void (*rasdecompFunc)(int& err,
    unsigned char* uncompressed,
    int nbytes,
    unsigned char* compressed,
    int nbytesCompressed,
    void* data);

typedef int (*rascompFunc)(int& err,
    unsigned char* uncompressed,
    int nbytes,
    unsigned char* compressed,
    void* data);

COMP_TYPE defaultCompType(RAST_PACK packType,
    RAST_DATA dataType,
    int nplanes);

RAST_CACHE defaultCacheType(COMP_TYPE compType, RAST_CACHE cacheType);

void lookupConvert(int npixels,
    unsigned char* inbuffer,
    RAST_PACK outpack,
    unsigned char* outbuffer,
    LUT_VALUE* convert);

void copyBuffer(unsigned char* bufferIn,
    mx_irect& in,
    mx_irect& inc,
    int pixelBitSize,
    unsigned char* bufferOut,
    mx_irect& out,
    mx_irect& outci);

int removeValueComp(int& err,
    unsigned char* uncompressed,
    int nbytes,
    unsigned char* compressed,
    void* data);

void removeValueDecomp(int& err,
    unsigned char* uncompressed,
    int nbytes,
    unsigned char* compressed,
    int nbytesCompressed,
    void* data);

void rasterunPackbits(int& err,
    unsigned char* uncompressed,
    int nbytes,
    unsigned char* compressed,
    int nbytesCompressed,
    void* data);

int rasterPackbits(int& err,
    unsigned char* uncompressed,
    int nbytes,
    unsigned char* compressed,
    void* data);

void rasterZuncomp(int& err,
    unsigned char* uncompressed,
    int nbytes,
    unsigned char* compressed,
    int nbytesCompressed,
    void* data);

int rasterZcomp(int& err,
    unsigned char* uncompressed,
    int nbytes,
    unsigned char* compressed,
    void* data);

void reverseBits(int nbytes,
    unsigned char* inbuffer,
    unsigned char* outbuffer);

void rasPack(int npackedPixels,
    unsigned char* packed,
    unsigned char* unpacked);

void rasUnpack(int npackedPixels,
    unsigned char* packed,
    unsigned char* unpacked);

void scaleDecompose(double scale1,
    double scale2,
    int32& irscale,
    double& rrscale);

void rasterFlip(int nbytes,
    int value,
    unsigned char* bufferIn,
    unsigned char* bufferOut);

void rasterInvert(int nbytes,
    unsigned char* bufferIn,
    unsigned char* bufferOut);

// Forward definition

class raster;

/*-------------------------------------------------
 * CLASS: rasterIterator
 *
 * DESCRIPTION: The class type for iteratively retrieving
 * tiles of data over an area. Normally the raster iterator
 * on a raster will suffice. If a raster dataset needs to
 * be iterated twice at the same time then this class can
 * be used to give an iterator outside the raster. Note that
 * these cases the user must be careful not to delete the raster
 * whilst the iterator is active.
 */

class rasterIterator {
public:
    rasterIterator(bool ignoreUnknown = FALSE);

    // Set up the iterator
    void set(int& err, raster* iraster, mx_irect& getRect);

    // Get the raster on the iterator
    inline raster* getRaster() { return thisRaster; };

    // Get the next tile from the raster
    // Returns NULL at end of the iteration
    unsigned char* next(int& err, mx_irect& currentPixels,
        mx_ipoint& tileCoords);

    inline bool getIgnoreUnknown() { return ignoreUnknown; };
    inline void setIgnoreUnknown(bool iignoreUnknown) { ignoreUnknown = iignoreUnknown; };

protected:
private:
    // The attached raster
    raster* thisRaster;

    // Count prameters
    mx_ipoint currentTile;
    mx_ipoint startTile;
    mx_ipoint endTile;
    bool ignoreUnknown;
};

/*-------------------------------------------------
 * CLASS: rascache
 *
 * DESCRIPTION: A global rascache structure holds
 * a variable number of raster tiles at a given
 * time. The tiles are also held in a collection of
 * raster cache items held on the raster class.
 * The raster cache items also live in a doubly
 * linked list on the raster cache class.
 *
 * The procedure of getting a tile is therefore
 *
 * 1) look in the collection of the raster. If it is
 *    there all well and good. Tell the raster cache to
 *    move the item corresponging to the
 *    tile to the head of its list
 * 2) If it isnt create a new piece of tile memory, fill
 *    it with the correct data, and then tell the raster
 *    cache to add the tile on an item it to its list.
 * 3) If the specified number of tiles is in the list,
 *    the item at the end of the cache must be dropped.
 *    The item has the raster it has attached to, from
 *    which the callback which removes the item from the
 *    raster cache can be removed.
 *
 *    NOTE :: The raster cache is only used for those
 *    rasters based on externally held data .e.g. in a TIFF
 *    file. Rasters in the MAXWELL internal format have
 *    data cahced within the database.
 */

// The list item structure

#define RASITEM_LOCKED 1 // Should really be a count
#define RASITEM_EDITED 2

#define RLOCK(item) ((item)->flag |= RASITEM_LOCKED)
#define RUNLOCK(item) ((item)->flag &= ~RASITEM_LOCKED)
#define RISLOCKED(item) (((item)->flag & RASITEM_LOCKED) != 0)

#define REDIT(item) ((item)->flag |= RASITEM_EDITED)
#define RUNEDIT(item) ((item)->flag &= ~RASITEM_EDITED)
#define RISEDITED(item) (((item)->flag & RASITEM_EDITED) != 0)

typedef struct rasterItem {
    raster* iraster; // The raster the item is attached to
    int itileIndex; // The tile within the raster
    unsigned char* ibuffer; // The memory for the tile

    rasterItem* next; // Linled list next
    rasterItem* prev; // Linked list prev
    unsigned char flag; // Flags about the item
} rasItem;

// The raster cache item. Only one of these items is likely to
// exist at a given time.

class rascache {
public:
    rascache(int32 size);

    // Add the tile to the list
    void add(int& err, raster* rast, int32 tileIndex,
        unsigned char* buffer, rasItem** index);
    // Move the tile to the head of the list
    void move(rasItem* index);
    // Delete all tiles for a raster
    void del(int& err, raster* rast);
    int error;

protected:
private:
    int32 initialSize;
    rasItem* head;
    rasItem* tail;
    rasItem* freeList;
    void splice(rasItem* thisRasItem);
    void getFromFreeList(rasItem** thisRasItem);
    void returnToFreeList(rasItem* thisRasItem);
    void newFreeList(int& err);
};

/*-------------------------------------------------
 * CLASS: raster
 *
 * DESCRIPTION: The basic raster class. This class
 * holds the parameters for a maxwell raster. The
 * important virtual function is the call to get a
 * tile of data.
 */

class raster {
public:
    // The cache shared over all rasters
    static rascache globalRascache;

    // Error from constructor

    raster(int& err);
    virtual ~raster();

    // The routine which gets a tile of data from the
    // underlying data - this may be a maxwell type raster
    // or a TIFF or JPEG file etc

    unsigned char* tile(int& err,
        bool getForEdit,
        bool& unknown,
        mx_ipoint& tilePos,
        rasItem** item = NULL);

    // This routine puts a tile of data
    void put(int& err,
        unsigned char* data,
        bool flush,
        mx_ipoint& tilePos);

    // The virtual function returns the data with the
    // tile. The input paramater buffer can be passed as
    // non-null in which case this is the piece of memory
    // to be filled with the data - or it can be NULL in which
    // case it is assumed that the getTile function is performing its
    // ownm memory management (as for a MAXWELL raster) .
    virtual unsigned char* getTile(int& err,
        unsigned char* buffer,
        bool getForEdit,
        bool& unknown,
        mx_ipoint& tile)
        = 0;

    virtual void putTile(int& err,
        unsigned char* buffer,
        mx_ipoint& tile);

    virtual uint32 get_serialised_size(int& err)
    {
        err = MX_ERROR_OK;
        return 0;
    };

    virtual void serialise(int& err,
        unsigned char** buffer) { err = MX_ERROR_OK; };

    virtual void unserialise(int& err,
        unsigned char** buffer) { err = MX_ERROR_OK; };

    virtual void setSize(int& err,
        mx_point& areaSize);
    virtual void setSize(int& err, mx_ipoint& isize) { };

    void copyData(int& err,
        raster& dest,
        mx_irect& extent);

    // Get some data in a buffer attached to the raster
    unsigned char* getBuffer(int& err, mx_irect& pixelCoords);

    // Create a cache of data
    void createCache(int& err,
        RAST_CACHE cacheType);

    // Basic access routines
    inline mx_ipoint getSize() { return size; };
    inline mx_ipoint getTileSize() { return tileSize; };
    mx_ipoint getTotalSize();
    inline int32 tilePixels() { return tileSize.x * tileSize.y; };
    inline RAST_PACK getPack() { return pack; };
    inline RAST_DATA getData() { return data; };
    int32 getPlanes(int32** isigbits = NULL);
    int32 tileByteSize();
    inline int32 pixelByteSize();
    inline int32 pixelBitSize();
    inline int32* getSigbits() { return sigbits; };
    inline int32 getSigbits(int offset) { return sigbits[offset]; };
    bool inTileRange(mx_ipoint& itile);
    inline int32 getColours(mx_colour** colours);
    inline mx_colour* getColours() { return colours; };
    inline int32 getNumColours() { return numColours; };
    inline mx_point getStoredMMsize() { return storedMMsize; };
    inline bool getHasStoredMMsize() { return hasStoredMMsize; };
    inline RAST_CACHE getCacheType() { return cacheType; };
    // Basic translation of tiles to pixels etc.
    void pointToTile(mx_ipoint& pixelCoord, mx_ipoint& tileCoord);
    void tileToPixels(mx_ipoint& tileCoord, mx_irect& ipixelCoords);
    mx_ipoint tileCoord(int32 index);
    mx_ipoint getTiles();
    int totalTiles();

    // Remove and tile from the dictionary
    void removeTile(int& err, unsigned char* buffer, int32 tileIndex, bool isEdited);

    // Setting and using the iterator
    void setIterator(int& err, mx_irect& input);
    unsigned char* next(int& err, mx_irect& currentPixels, mx_ipoint& tileCoord);
    // Get a tile index from its coordinates
    int32 tileIndex(mx_ipoint& tileCoord);
    void copyBasics(int& err, raster& base);

    void defaultMMsize();

    mx_point areaSize(int& err,
        mx_point& pageSize,
        double pixelResolution);

    int sigbitsFromColours();

protected:
    // Basic properties of the raster
    mx_ipoint size;
    mx_ipoint tileSize;
    RAST_PACK pack;
    RAST_DATA data;
    int32 nplanes;
    int32 sigbits[RAST_NUMPLANES];
    int32 numColours;
    mx_colour* colours;

    // stored MMsize
    bool hasStoredMMsize;
    mx_point storedMMsize;

    // The collection of tiles indexed on tile id
    RAST_CACHE cacheType;
    std::map<int, rasItem *> tileDict;

private:
    // An internal file buffer to hand data back one tile of
    // data - if only one tile of data is used for a raster,
    // this will be the one and only tile returned
    unsigned char* tileBuffer;

    // An iterator to get all tiles over an area
    rasterIterator riterator;

    // A buffer to return data in
    unsigned char* buffer;
    int32 bufferSize;
};

/*-------------------------------------------------
 * FUNCTION: raster::getPlanes
 *
 * DESCRIPTION: Return the number of planes in a
 * raster - if required return the number of significant
 * bits per plane
 */

inline int32 raster::getPlanes(int32** isigbits)
{
    if (isigbits != NULL)
        *isigbits = sigbits;
    return nplanes;
}

/*-------------------------------------------------
 * FUNCTION: raster::getColours
 *
 * DESCRIPTION: Return the number of colours for the
 * raster - and the colours - only applicable for rasters
 * with a CODE_TYPE.
 */

inline int32 raster::getColours(mx_colour** icolour)
{
    if (icolour != NULL)
        *icolour = colours;
    return numColours;
}

/*-------------------------------------------------
 * FUNCTION: roundToSize
 *
 * DESCRIPTION: Basic quantisation to map points
 * to tiles.
 *
 */

inline int32 roundToSize(int32 in, int32 size)
{
    if (in >= 0) {
        return (in / size);
    } else {
        return -((-in - 1) / size);
    }
}

/*-------------------------------------------------
 * FUNCTION: raster::pointToTile
 *
 * DESCRIPTION: Get the tile a pixel resides in
 *
 */

inline void raster::pointToTile(mx_ipoint& pixelCoord, mx_ipoint& tileCoord)
{
    mx_point rpoint(roundToSize(pixelCoord.x, tileSize.x),
        roundToSize(pixelCoord.y, tileSize.y));
    tileCoord = rpoint;
}

/*-------------------------------------------------
 * FUNCTION: raster::tileToPixels
 *
 * DESCRIPTION: Get the pixel coordinates of the
 * corners of a tile
 */

inline void raster::tileToPixels(mx_ipoint& tileCoord,
    mx_irect& ipixelCoords)
{
    mx_irect rec(tileCoord.x * tileSize.x,
        tileCoord.y * tileSize.y,
        (tileCoord.x + 1) * tileSize.x - 1,
        (tileCoord.y + 1) * tileSize.y - 1);

    ipixelCoords = rec;
}

/*-------------------------------------------------
 * FUNCTION: raster::setIterator
 *
 * DESCRIPTION: Setup a raster iteration of the
 * tiles within an area
 */

inline void raster::setIterator(int& err, mx_irect& pixelCoords)
{
    riterator.set(err, this, pixelCoords);
}

/*-------------------------------------------------
 * FUNCTION: raster::next
 *
 * DESCRIPTION: Get the next tile in an iteration
 * Return the pixel coordinates of the corners of this
 * tile.
 *
 */

inline unsigned char* raster::next(int& err, mx_irect& currentPixels,
    mx_ipoint& tileCoords)
{
    return riterator.next(err, currentPixels, tileCoords);
}

/*-------------------------------------------------
 * FUNCTION: raster::tileIndex
 *
 * DESCRIPTION: Get the next tile in an iteration
 * Return the pixel coordinates of the corners of this
 * tile.
 *
 */

inline int32 raster::tileIndex(mx_ipoint& tileCoord)
{
    mx_ipoint tiles = getTiles();
    return tileCoord.y * tiles.x + tileCoord.x;
}

/*-------------------------------------------------
 * FUNCTION: raster::tileCoord
 *
 * DESCRIPTION: Get the next tile in an iteration
 * Return the pixel coordinates of the corners of this
 * tile.
 *
 */

inline mx_ipoint raster::tileCoord(int32 index)
{
    mx_ipoint tileCoord;
    mx_ipoint tiles = getTiles();

    tileCoord.x = index % tiles.x;
    tileCoord.y = index / tiles.x;

    return tileCoord;
}

/*-------------------------------------------------
 * FUNCTION: rasterBits
 *
 * DESCRIPTION: Get the number of bits in some pixels
 *
 *
 */

int rasterBits(RAST_PACK packtype, int32 npixels, int32 nplanes);

/*-------------------------------------------------
 * FUNCTION: rasterBytes
 *
 * DESCRIPTION: Get the number of bytes in some pixels
 *
 *
 */

int rasterBytes(RAST_PACK packtype, int32 npixels, int32 nplanes);

/*-------------------------------------------------
 * FUNCTION: inTileRange
 *
 * DESCRIPTION: See if a tile coordinate is within
 * range
 *
 */

inline bool raster::inTileRange(mx_ipoint& itile)
{
    mx_ipoint pt = getTiles();

    return !((itile.x < 0) || (itile.y < 0) || (itile.x >= pt.x) || (itile.y >= pt.y));
}

/*-------------------------------------------------
 * FUNCTION: raster::tileByteSize()
 *
 * DESCRIPTION: Get the tile byte size
 *
 */

inline int32 raster::tileByteSize()
{
    return rasterBytes(pack, tilePixels(), nplanes);
}

/*-------------------------------------------------
 * FUNCTION: raster::pixelByteSize()
 *
 * DESCRIPTION: Get the pixel byte size
 *
 */

inline int32 raster::pixelByteSize()
{
    return rasterBytes(pack, 1, nplanes);
}

/*-------------------------------------------------
 * FUNCTION: raster::pixelBitSize()
 *
 * DESCRIPTION: Get the pixel byte size
 *
 */

inline int32 raster::pixelBitSize()
{
    return rasterBits(pack, 1, nplanes);
}
