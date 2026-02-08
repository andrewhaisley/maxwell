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
 * MODULE : raster.C
 *
 * AUTHOR : David Miller
 *
 * DESCRIPTION: Basic raster handling
 * Module raster.C
 *
 *
 */
#include "raster.h"
#include <memory.h>

rascache raster::globalRascache(32);

typedef struct bnode {
    struct bnode* set;
    struct bnode* unset;
    unsigned char value;
} bnode;

#define MIND(x, y) ((x < y) ? x : y)
#define MAXD(x, y) ((x > y) ? x : y)

#define SHL(a, b) ((unsigned char)((a) << (b)))
#define SHR(a, b) ((unsigned char)((a) >> (b)))
#define SHRL(a, b, c) (SHL(SHR(a, b), c))
#define SHLR(a, b, c) (SHR(SHL(a, b), c))
#define SHRLR(a, b, c, d) (SHR(SHRL(a, b, c), d))
#define SHLRL(a, b, c, d) (SHL(SHLR(a, b, c), d))

/*-------------------------------------------------
 * FUNCTION: rasterInvert
 *
 * DESCRIPTION:
 *
 *
 */

void rasterInvert(int nbytes,
    unsigned char* bufferIn,
    unsigned char* bufferOut)
{
    int i;
    unsigned int* ibufferIn = (unsigned int*)bufferIn;
    unsigned int* ibufferOut = (unsigned int*)bufferOut;

    while (nbytes >= ((int)sizeof(unsigned int))) {
        *ibufferOut++ = ~(*ibufferIn++);
        nbytes -= sizeof(unsigned int);
    }

    bufferIn = (unsigned char*)ibufferIn;
    bufferOut = (unsigned char*)ibufferOut;

    for (i = 0; i < nbytes; i++)
        *bufferOut++ = ~(*bufferIn++);
}

/*-------------------------------------------------
 * FUNCTION: raster Flip
 *
 * DESCRIPTION:
 *
 *
 */

void rasterFlip(int nbytes,
    int value,
    unsigned char* bufferIn,
    unsigned char* bufferOut)
{
    unsigned char* endPtr = bufferOut + nbytes;

    while (bufferOut < endPtr)
        *bufferOut++ = value - (*bufferIn++);
}

/*-------------------------------------------------
 * FUNCTION: defaultCacheType
 *
 * DESCRIPTION:
 *
 *
 */

RAST_CACHE defaultCacheType(COMP_TYPE compType,
    RAST_CACHE cacheType)
{
    if (cacheType == RCACHE_DEFAULT) {
        return (compType == COMP_UNCOMPRESSED) ? RCACHE_TILE : RCACHE_DICT;
    } else {
        return cacheType;
    }
}

/*-------------------------------------------------
 * FUNCTION: defaultCompType
 *
 * DESCRIPTION:
 *
 *
 */

COMP_TYPE defaultCompType(RAST_PACK packType,
    RAST_DATA dataType,
    int nplanes)
{
    COMP_TYPE compType;

    if (nplanes != 1) {
        /* rgb */
        compType = COMP_ZCOMP;
    } else {
        switch (packType) {
        case RPACK_BIT:
            compType = COMP_REMOVE;
            break;
        case RPACK_BYTE:
            if (dataType == RDATA_COLOUR) {
                compType = COMP_PACKBITS;
            } else {
                compType = COMP_ZCOMP;
            }

            break;
        default:
            compType = COMP_UNCOMPRESSED;
            break;
        }
    }

    return compType;
}

/*-------------------------------------------------
 * FUNCTION: copyBuffer
 *
 * DESCRIPTION:
 *
 *
 */

void copyBuffer(unsigned char* bufferIn,
    mx_irect& in,
    mx_irect& inc,
    int pixelBitSize,
    unsigned char* bufferOut,
    mx_irect& out,
    mx_irect& outci)
{
    int widthi, widtho, widthc;
    int bit_widthi, bit_widtho, bit_widthc;
    int byte_widthi, byte_widtho, byte_widthc;
    int bit_off_widthi, bit_off_widtho, bit_off_widthc;
    int pixel_off_in, pixel_off_out;
    int bit_off_in, bit_off_out;
    int byte_off_in, byte_off_out;
    unsigned char* coffset_byteo;
    unsigned char* coffset_bytei;
    int coffset_bito, ccoffset_bito;
    int coffset_biti, ccoffset_biti;
    unsigned char* ioffset_byteo;
    unsigned char* ioffset_bytei;
    int ioffset_bito;
    int ioffset_biti;
    int shift, cshift, shiftn, cshiftn, shifter, cshifter, n_bits_left, nbits;
    int nrows, nbytes;
    unsigned char worki1, worko1;
    unsigned char worki2, worko2;

    // Shift out so that the common areas overlap
    widthi = in.xt - in.xb + 1;
    widthc = inc.xt - inc.xb + 1;
    widtho = out.xt - out.xb + 1;
    pixel_off_in = widthi * (inc.yb - in.yb) + (inc.xb - in.xb);
    pixel_off_out = widtho * (inc.yb - out.yb) + (inc.xb - out.xb);
    bit_off_in = (pixel_off_in * pixelBitSize) % 8;
    bit_off_out = (pixel_off_out * pixelBitSize) % 8;
    byte_off_in = (pixel_off_in * pixelBitSize) / 8;
    byte_off_out = (pixel_off_out * pixelBitSize) / 8;
    bit_widthi = widthi * pixelBitSize;
    bit_widtho = widtho * pixelBitSize;
    bit_widthc = widthc * pixelBitSize;
    byte_widthi = bit_widthi / 8;
    byte_widtho = bit_widtho / 8;
    byte_widthc = bit_widthc / 8;
    bit_off_widthi = bit_widthi % 8;
    bit_off_widtho = bit_widtho % 8;
    bit_off_widthc = bit_widthc % 8;
    ioffset_byteo = bufferOut + byte_off_out;
    ioffset_bytei = bufferIn + byte_off_in;
    ioffset_bito = bit_off_out;
    ioffset_biti = bit_off_in;
    nrows = inc.yt - inc.yb + 1;

    if ((ioffset_biti == 0) && (ioffset_bito == 0) && (bit_off_widthi == 0)
        && (bit_off_widtho == 0) && (bit_off_widthc == 0)) {
        if ((in.xb == inc.xb) && (in.xt == inc.xt) && (out.xb == inc.xb) && (out.xt == inc.xt)) {
            memcpy(ioffset_byteo, ioffset_bytei, byte_widthc * nrows);
        } else {
            while (nrows > 0) {
                memcpy(ioffset_byteo, ioffset_bytei, byte_widthc);
                ioffset_byteo += byte_widtho;
                ioffset_bytei += byte_widthi;
                nrows--;
            }
        }
    } else {
        while (nrows > 0) {
            n_bits_left = bit_widthc;
            coffset_byteo = ioffset_byteo;
            coffset_bytei = ioffset_bytei;
            coffset_bito = ioffset_bito;
            coffset_biti = ioffset_biti;
            ccoffset_biti = 8 - coffset_biti;
            ccoffset_bito = 8 - coffset_bito;
            shift = ccoffset_biti - ccoffset_bito;
            cshift = 8 - shift;
            if (shift == 0) {
                if (coffset_bito != 0) {
                    nbits = MIND(bit_widthc, ccoffset_bito);
                    worki1 = SHLRL(*coffset_bytei++, ccoffset_biti - nbits, 8 - nbits,
                        coffset_bito);
                    worko1 = SHLR(*coffset_byteo, ccoffset_bito, ccoffset_bito);
                    worko2 = SHRL(*coffset_byteo, nbits + coffset_bito,
                        nbits + coffset_bito);
                    *coffset_byteo++ = worko1 | worko2 | worki1;
                    n_bits_left = bit_widthc - nbits;
                }
                if (n_bits_left != 0) {
                    nbytes = n_bits_left / 8;
                    nbits = n_bits_left % 8;
                    if (nbytes != 0) {
                        memcpy(coffset_byteo, coffset_bytei, nbytes);
                        coffset_byteo += nbytes;
                        coffset_bytei += nbytes;
                    }
                    if (nbits != 0) {
                        worki1 = SHLR(*coffset_bytei, 8 - nbits, 8 - nbits);
                        worko1 = SHRL(*coffset_byteo, nbits, nbits);
                        *coffset_byteo = worko1 | worki1;
                    }
                }
            } else {
                if (shift > 0) {
                    if (coffset_bito != 0) {
                        nbits = MIND(bit_widthc, ccoffset_bito);
                        worki1 = SHLRL(*coffset_bytei, ccoffset_biti - nbits,
                            8 - nbits, coffset_bito);
                        worko1 = SHLR(*coffset_byteo, ccoffset_bito, ccoffset_bito);
                        worko2 = SHRL(*coffset_byteo, nbits + coffset_bito,
                            nbits + coffset_bito);
                        *coffset_byteo++ = worko1 | worko2 | worki1;
                        n_bits_left = bit_widthc - nbits;
                    }
                    shifter = shift;
                    cshifter = cshift;
                } else {
                    shiftn = -shift;
                    cshiftn = 8 - shiftn;
                    if (coffset_bito != 0) {
                        nbits = MIND(bit_widthc, ccoffset_bito);
                        if (nbits <= ccoffset_biti) {
                            worki1 = SHLRL(*coffset_bytei, ccoffset_biti - nbits,
                                8 - nbits, coffset_bito);
                            worko1 = SHLR(*coffset_byteo, ccoffset_bito,
                                ccoffset_bito);
                            worko2 = SHRL(*coffset_byteo, nbits + coffset_bito,
                                nbits + coffset_bito);
                            *coffset_byteo++ = worko1 | worko2 | worki1;
                            n_bits_left = bit_widthc - nbits;
                        } else {
                            worki1 = SHRL(*coffset_bytei++, coffset_biti,
                                coffset_bito);
                            n_bits_left = bit_widthc - nbits;
                            nbits -= ccoffset_biti;
                            worki2 = SHLR(*coffset_bytei, 8 - nbits, shiftn - nbits);
                            worko1 = SHLR(*coffset_byteo, ccoffset_bito,
                                ccoffset_bito);
                            worko2 = SHRL(*coffset_byteo, cshiftn + nbits,
                                cshiftn + nbits);
                            *coffset_byteo++ = worko1 | worko2 | worki1 | worki2;
                        }
                    }
                    cshifter = shiftn;
                    shifter = cshiftn;
                }
                while (n_bits_left >= 8) {
                    worki1 = SHR(*coffset_bytei++, cshifter);
                    worki2 = SHL(*coffset_bytei, shifter);
                    *coffset_byteo++ = worki1 | worki2;
                    n_bits_left -= 8;
                }
                if (n_bits_left != 0) {
                    if (n_bits_left <= shifter) {
                        worki1 = SHLR(*coffset_bytei, shifter - n_bits_left,
                            8 - n_bits_left);
                        worko1 = SHRL(*coffset_byteo, n_bits_left, n_bits_left);
                        *coffset_byteo = worki1 | worko1;
                    } else {
                        worki1 = SHR(*coffset_bytei++, cshifter);
                        worki2 = SHLR(*coffset_bytei, 8 + shifter - n_bits_left,
                            8 - n_bits_left);
                        worko1 = SHRL(*coffset_byteo, n_bits_left, n_bits_left);
                        *coffset_byteo = worki1 | worki2 | worko1;
                    }
                }
            }
            nrows--;
            ioffset_byteo += byte_widtho;
            ioffset_bytei += byte_widthi;
            ioffset_bito += bit_off_widtho;
            ioffset_biti += bit_off_widthi;
            if (ioffset_bito >= 8) {
                ioffset_bito -= 8;
                ioffset_byteo++;
            }
            if (ioffset_biti >= 8) {
                ioffset_biti -= 8;
                ioffset_bytei++;
            }
        }
    }
}

/*-------------------------------------------------
 * FUNCTION: lookupConvert
 *
 * DESCRIPTION:
 *
 *
 */

void lookupConvert(int npixels,
    unsigned char* inbuffer,
    RAST_PACK outpack,
    unsigned char* outbuffer,
    LUT_VALUE* convert)
{
    int i;

    switch (outpack) {
    case RPACK_BYTE:
        for (i = 0; i < npixels; i++) {
            *outbuffer++ = convert[*inbuffer++];
        }
        break;
    case RPACK_SHORT: {
        uint16* outshort = (uint16*)outbuffer;

        for (i = 0; i < npixels; i++) {
            *outshort++ = convert[*inbuffer++];
        }
    } break;
    case RPACK_INT: {
        uint32* outshort = (uint32*)outbuffer;

        for (i = 0; i < npixels; i++) {
            *outshort++ = convert[*inbuffer++];
        }
    } break;
    default:
        break;
    }
}

/*-------------------------------------------------
 * FUNCTION: reverseBits
 *
 * DESCRIPTION:
 *
 *
 */

void reverseBits(int nbytes,
    unsigned char* inbuffer,
    unsigned char* outbuffer)
{
    int i;

    static unsigned char rev[256] = {
        0, 128, 64, 192, 32, 160, 96, 224, 16, 144, 80, 208, 48, 176, 112, 240,
        8, 136, 72, 200, 40, 168, 104, 232, 24, 152, 88, 216, 56, 184, 120, 248,
        4, 132, 68, 196, 36, 164, 100, 228, 20, 148, 84, 212, 52, 180, 116, 244,
        12, 140, 76, 204, 44, 172, 108, 236, 28, 156, 92, 220, 60, 188, 124, 252,
        2, 130, 66, 194, 34, 162, 98, 226, 18, 146, 82, 210, 50, 178, 114, 242, 10,
        138, 74, 202, 42, 170, 106, 234, 26, 154, 90, 218, 58, 186, 122, 250, 6,
        134, 70, 198, 38, 166, 102, 230, 22, 150, 86, 214, 54, 182, 118, 246, 14,
        142, 78, 206, 46, 174, 110, 238, 30, 158, 94, 222, 62, 190, 126, 254, 1,
        129, 65, 193, 33, 161, 97, 225, 17, 145, 81, 209, 49, 177, 113, 241, 9, 137,
        73, 201, 41, 169, 105, 233, 25, 153, 89, 217, 57, 185, 121, 249, 5, 133, 69,
        197, 37, 165, 101, 229, 21, 149, 85, 213, 53, 181, 117, 245, 13, 141, 77,
        205, 45, 173, 109, 237, 29, 157, 93, 221, 61, 189, 125, 253, 3, 131, 67, 195,
        35, 163, 99, 227, 19, 147, 83, 211, 51, 179, 115, 243, 11, 139, 75, 203, 43,
        171, 107, 235, 27, 155, 91, 219, 59, 187, 123, 251, 7, 135, 71, 199, 39, 167,
        103, 231, 23, 151, 87, 215, 55, 183, 119, 247, 15, 143, 79, 207, 47, 175, 111,
        239, 31, 159, 95, 223, 63, 191, 127, 255
    };

    for (i = 0; i < nbytes; i++) {
        *outbuffer++ = rev[*inbuffer++];
    }
}

/*-------------------------------------------------
 * FUNCTION: getTotalSize
 *
 * DESCRIPTION: See if a tile coordinate is within
 * range
 *
 */

mx_ipoint raster::getTotalSize()
{
    mx_ipoint pt = getTiles();

    pt.x *= tileSize.x;
    pt.y *= tileSize.y;

    return pt;
}

/*-------------------------------------------------
 * FUNCTION: raster::putTile
 *
 * DESCRIPTION:
 *
 *
 */

void raster::putTile(int& err,
    unsigned char* buffer,
    mx_ipoint& tile)
{
    err = MX_RASTER_NOT_EDITABLE;
}

/*-------------------------------------------------
 * FUNCTION: setNode
 *
 * DESCRIPTION: Create the node structure for unpacking
 *
 *
 */

static void setNode(bnode* node,
    int level,
    int nlevel,
    int value,
    bnode* nodes,
    int& nodeCount)
{
    node->value = value;

    if (level != nlevel) {
        node->set = nodes + nodeCount++;
        node->unset = nodes + nodeCount++;

        setNode(node->set, level + 1, nlevel, value + (1 << level), nodes, nodeCount);
        setNode(node->unset, level + 1, nlevel, value, nodes, nodeCount);
    }
}

/*-------------------------------------------------
 * FUNCTION: rasPack
 *
 * DESCRIPTION: Pack an array of integers
 *
 *
 */

void rasPack(int npackedPixels,
    unsigned char* packed,
    unsigned char* unpacked)
{
    static bool first = TRUE;
    static bnode nodes[512];
    int nodeCount, i;
    unsigned char* endunPacked;
    bnode* node;

    if (first) {
        // Create the node strcuture
        nodeCount = 1;
        setNode(nodes, 0, 8, 0, nodes, nodeCount);
        first = FALSE;
    }

    // Set end
    endunPacked = unpacked + npackedPixels;

    while (unpacked < endunPacked) {
        node = nodes;
        for (i = 0; i < 8; i++) {
            if (*unpacked++) {
                node = node->set;
            } else {
                node = node->unset;
            }
        }
        *packed++ = node->value;
    }
}

/*-------------------------------------------------
 * FUNCTION: packArray
 *
 * DESCRIPTION:
 *
 *
 */

void rasUnpack(int npackedPixels,
    unsigned char* packed,
    unsigned char* unpacked)
{
    static unsigned char unpackArray[256][8] = {
        { 0, 0, 0, 0, 0, 0, 0, 0 }, { 1, 0, 0, 0, 0, 0, 0, 0 }, { 0, 1, 0, 0, 0, 0, 0, 0 }, { 1, 1, 0, 0, 0, 0, 0, 0 },
        { 0, 0, 1, 0, 0, 0, 0, 0 }, { 1, 0, 1, 0, 0, 0, 0, 0 }, { 0, 1, 1, 0, 0, 0, 0, 0 }, { 1, 1, 1, 0, 0, 0, 0, 0 },
        { 0, 0, 0, 1, 0, 0, 0, 0 }, { 1, 0, 0, 1, 0, 0, 0, 0 }, { 0, 1, 0, 1, 0, 0, 0, 0 }, { 1, 1, 0, 1, 0, 0, 0, 0 },
        { 0, 0, 1, 1, 0, 0, 0, 0 }, { 1, 0, 1, 1, 0, 0, 0, 0 }, { 0, 1, 1, 1, 0, 0, 0, 0 }, { 1, 1, 1, 1, 0, 0, 0, 0 },
        { 0, 0, 0, 0, 1, 0, 0, 0 }, { 1, 0, 0, 0, 1, 0, 0, 0 }, { 0, 1, 0, 0, 1, 0, 0, 0 }, { 1, 1, 0, 0, 1, 0, 0, 0 },
        { 0, 0, 1, 0, 1, 0, 0, 0 }, { 1, 0, 1, 0, 1, 0, 0, 0 }, { 0, 1, 1, 0, 1, 0, 0, 0 }, { 1, 1, 1, 0, 1, 0, 0, 0 },
        { 0, 0, 0, 1, 1, 0, 0, 0 }, { 1, 0, 0, 1, 1, 0, 0, 0 }, { 0, 1, 0, 1, 1, 0, 0, 0 }, { 1, 1, 0, 1, 1, 0, 0, 0 },
        { 0, 0, 1, 1, 1, 0, 0, 0 }, { 1, 0, 1, 1, 1, 0, 0, 0 }, { 0, 1, 1, 1, 1, 0, 0, 0 }, { 1, 1, 1, 1, 1, 0, 0, 0 },
        { 0, 0, 0, 0, 0, 1, 0, 0 }, { 1, 0, 0, 0, 0, 1, 0, 0 }, { 0, 1, 0, 0, 0, 1, 0, 0 }, { 1, 1, 0, 0, 0, 1, 0, 0 },
        { 0, 0, 1, 0, 0, 1, 0, 0 }, { 1, 0, 1, 0, 0, 1, 0, 0 }, { 0, 1, 1, 0, 0, 1, 0, 0 }, { 1, 1, 1, 0, 0, 1, 0, 0 },
        { 0, 0, 0, 1, 0, 1, 0, 0 }, { 1, 0, 0, 1, 0, 1, 0, 0 }, { 0, 1, 0, 1, 0, 1, 0, 0 }, { 1, 1, 0, 1, 0, 1, 0, 0 },
        { 0, 0, 1, 1, 0, 1, 0, 0 }, { 1, 0, 1, 1, 0, 1, 0, 0 }, { 0, 1, 1, 1, 0, 1, 0, 0 }, { 1, 1, 1, 1, 0, 1, 0, 0 },
        { 0, 0, 0, 0, 1, 1, 0, 0 }, { 1, 0, 0, 0, 1, 1, 0, 0 }, { 0, 1, 0, 0, 1, 1, 0, 0 }, { 1, 1, 0, 0, 1, 1, 0, 0 },
        { 0, 0, 1, 0, 1, 1, 0, 0 }, { 1, 0, 1, 0, 1, 1, 0, 0 }, { 0, 1, 1, 0, 1, 1, 0, 0 }, { 1, 1, 1, 0, 1, 1, 0, 0 },
        { 0, 0, 0, 1, 1, 1, 0, 0 }, { 1, 0, 0, 1, 1, 1, 0, 0 }, { 0, 1, 0, 1, 1, 1, 0, 0 }, { 1, 1, 0, 1, 1, 1, 0, 0 },
        { 0, 0, 1, 1, 1, 1, 0, 0 }, { 1, 0, 1, 1, 1, 1, 0, 0 }, { 0, 1, 1, 1, 1, 1, 0, 0 }, { 1, 1, 1, 1, 1, 1, 0, 0 },
        { 0, 0, 0, 0, 0, 0, 1, 0 }, { 1, 0, 0, 0, 0, 0, 1, 0 }, { 0, 1, 0, 0, 0, 0, 1, 0 }, { 1, 1, 0, 0, 0, 0, 1, 0 },
        { 0, 0, 1, 0, 0, 0, 1, 0 }, { 1, 0, 1, 0, 0, 0, 1, 0 }, { 0, 1, 1, 0, 0, 0, 1, 0 }, { 1, 1, 1, 0, 0, 0, 1, 0 },
        { 0, 0, 0, 1, 0, 0, 1, 0 }, { 1, 0, 0, 1, 0, 0, 1, 0 }, { 0, 1, 0, 1, 0, 0, 1, 0 }, { 1, 1, 0, 1, 0, 0, 1, 0 },
        { 0, 0, 1, 1, 0, 0, 1, 0 }, { 1, 0, 1, 1, 0, 0, 1, 0 }, { 0, 1, 1, 1, 0, 0, 1, 0 }, { 1, 1, 1, 1, 0, 0, 1, 0 },
        { 0, 0, 0, 0, 1, 0, 1, 0 }, { 1, 0, 0, 0, 1, 0, 1, 0 }, { 0, 1, 0, 0, 1, 0, 1, 0 }, { 1, 1, 0, 0, 1, 0, 1, 0 },
        { 0, 0, 1, 0, 1, 0, 1, 0 }, { 1, 0, 1, 0, 1, 0, 1, 0 }, { 0, 1, 1, 0, 1, 0, 1, 0 }, { 1, 1, 1, 0, 1, 0, 1, 0 },
        { 0, 0, 0, 1, 1, 0, 1, 0 }, { 1, 0, 0, 1, 1, 0, 1, 0 }, { 0, 1, 0, 1, 1, 0, 1, 0 }, { 1, 1, 0, 1, 1, 0, 1, 0 },
        { 0, 0, 1, 1, 1, 0, 1, 0 }, { 1, 0, 1, 1, 1, 0, 1, 0 }, { 0, 1, 1, 1, 1, 0, 1, 0 }, { 1, 1, 1, 1, 1, 0, 1, 0 },
        { 0, 0, 0, 0, 0, 1, 1, 0 }, { 1, 0, 0, 0, 0, 1, 1, 0 }, { 0, 1, 0, 0, 0, 1, 1, 0 }, { 1, 1, 0, 0, 0, 1, 1, 0 },
        { 0, 0, 1, 0, 0, 1, 1, 0 }, { 1, 0, 1, 0, 0, 1, 1, 0 }, { 0, 1, 1, 0, 0, 1, 1, 0 }, { 1, 1, 1, 0, 0, 1, 1, 0 },
        { 0, 0, 0, 1, 0, 1, 1, 0 }, { 1, 0, 0, 1, 0, 1, 1, 0 }, { 0, 1, 0, 1, 0, 1, 1, 0 }, { 1, 1, 0, 1, 0, 1, 1, 0 },
        { 0, 0, 1, 1, 0, 1, 1, 0 }, { 1, 0, 1, 1, 0, 1, 1, 0 }, { 0, 1, 1, 1, 0, 1, 1, 0 }, { 1, 1, 1, 1, 0, 1, 1, 0 },
        { 0, 0, 0, 0, 1, 1, 1, 0 }, { 1, 0, 0, 0, 1, 1, 1, 0 }, { 0, 1, 0, 0, 1, 1, 1, 0 }, { 1, 1, 0, 0, 1, 1, 1, 0 },
        { 0, 0, 1, 0, 1, 1, 1, 0 }, { 1, 0, 1, 0, 1, 1, 1, 0 }, { 0, 1, 1, 0, 1, 1, 1, 0 }, { 1, 1, 1, 0, 1, 1, 1, 0 },
        { 0, 0, 0, 1, 1, 1, 1, 0 }, { 1, 0, 0, 1, 1, 1, 1, 0 }, { 0, 1, 0, 1, 1, 1, 1, 0 }, { 1, 1, 0, 1, 1, 1, 1, 0 },
        { 0, 0, 1, 1, 1, 1, 1, 0 }, { 1, 0, 1, 1, 1, 1, 1, 0 }, { 0, 1, 1, 1, 1, 1, 1, 0 }, { 1, 1, 1, 1, 1, 1, 1, 0 },
        { 0, 0, 0, 0, 0, 0, 0, 1 }, { 1, 0, 0, 0, 0, 0, 0, 1 }, { 0, 1, 0, 0, 0, 0, 0, 1 }, { 1, 1, 0, 0, 0, 0, 0, 1 },
        { 0, 0, 1, 0, 0, 0, 0, 1 }, { 1, 0, 1, 0, 0, 0, 0, 1 }, { 0, 1, 1, 0, 0, 0, 0, 1 }, { 1, 1, 1, 0, 0, 0, 0, 1 },
        { 0, 0, 0, 1, 0, 0, 0, 1 }, { 1, 0, 0, 1, 0, 0, 0, 1 }, { 0, 1, 0, 1, 0, 0, 0, 1 }, { 1, 1, 0, 1, 0, 0, 0, 1 },
        { 0, 0, 1, 1, 0, 0, 0, 1 }, { 1, 0, 1, 1, 0, 0, 0, 1 }, { 0, 1, 1, 1, 0, 0, 0, 1 }, { 1, 1, 1, 1, 0, 0, 0, 1 },
        { 0, 0, 0, 0, 1, 0, 0, 1 }, { 1, 0, 0, 0, 1, 0, 0, 1 }, { 0, 1, 0, 0, 1, 0, 0, 1 }, { 1, 1, 0, 0, 1, 0, 0, 1 },
        { 0, 0, 1, 0, 1, 0, 0, 1 }, { 1, 0, 1, 0, 1, 0, 0, 1 }, { 0, 1, 1, 0, 1, 0, 0, 1 }, { 1, 1, 1, 0, 1, 0, 0, 1 },
        { 0, 0, 0, 1, 1, 0, 0, 1 }, { 1, 0, 0, 1, 1, 0, 0, 1 }, { 0, 1, 0, 1, 1, 0, 0, 1 }, { 1, 1, 0, 1, 1, 0, 0, 1 },
        { 0, 0, 1, 1, 1, 0, 0, 1 }, { 1, 0, 1, 1, 1, 0, 0, 1 }, { 0, 1, 1, 1, 1, 0, 0, 1 }, { 1, 1, 1, 1, 1, 0, 0, 1 },
        { 0, 0, 0, 0, 0, 1, 0, 1 }, { 1, 0, 0, 0, 0, 1, 0, 1 }, { 0, 1, 0, 0, 0, 1, 0, 1 }, { 1, 1, 0, 0, 0, 1, 0, 1 },
        { 0, 0, 1, 0, 0, 1, 0, 1 }, { 1, 0, 1, 0, 0, 1, 0, 1 }, { 0, 1, 1, 0, 0, 1, 0, 1 }, { 1, 1, 1, 0, 0, 1, 0, 1 },
        { 0, 0, 0, 1, 0, 1, 0, 1 }, { 1, 0, 0, 1, 0, 1, 0, 1 }, { 0, 1, 0, 1, 0, 1, 0, 1 }, { 1, 1, 0, 1, 0, 1, 0, 1 },
        { 0, 0, 1, 1, 0, 1, 0, 1 }, { 1, 0, 1, 1, 0, 1, 0, 1 }, { 0, 1, 1, 1, 0, 1, 0, 1 }, { 1, 1, 1, 1, 0, 1, 0, 1 },
        { 0, 0, 0, 0, 1, 1, 0, 1 }, { 1, 0, 0, 0, 1, 1, 0, 1 }, { 0, 1, 0, 0, 1, 1, 0, 1 }, { 1, 1, 0, 0, 1, 1, 0, 1 },
        { 0, 0, 1, 0, 1, 1, 0, 1 }, { 1, 0, 1, 0, 1, 1, 0, 1 }, { 0, 1, 1, 0, 1, 1, 0, 1 }, { 1, 1, 1, 0, 1, 1, 0, 1 },
        { 0, 0, 0, 1, 1, 1, 0, 1 }, { 1, 0, 0, 1, 1, 1, 0, 1 }, { 0, 1, 0, 1, 1, 1, 0, 1 }, { 1, 1, 0, 1, 1, 1, 0, 1 },
        { 0, 0, 1, 1, 1, 1, 0, 1 }, { 1, 0, 1, 1, 1, 1, 0, 1 }, { 0, 1, 1, 1, 1, 1, 0, 1 }, { 1, 1, 1, 1, 1, 1, 0, 1 },
        { 0, 0, 0, 0, 0, 0, 1, 1 }, { 1, 0, 0, 0, 0, 0, 1, 1 }, { 0, 1, 0, 0, 0, 0, 1, 1 }, { 1, 1, 0, 0, 0, 0, 1, 1 },
        { 0, 0, 1, 0, 0, 0, 1, 1 }, { 1, 0, 1, 0, 0, 0, 1, 1 }, { 0, 1, 1, 0, 0, 0, 1, 1 }, { 1, 1, 1, 0, 0, 0, 1, 1 },
        { 0, 0, 0, 1, 0, 0, 1, 1 }, { 1, 0, 0, 1, 0, 0, 1, 1 }, { 0, 1, 0, 1, 0, 0, 1, 1 }, { 1, 1, 0, 1, 0, 0, 1, 1 },
        { 0, 0, 1, 1, 0, 0, 1, 1 }, { 1, 0, 1, 1, 0, 0, 1, 1 }, { 0, 1, 1, 1, 0, 0, 1, 1 }, { 1, 1, 1, 1, 0, 0, 1, 1 },
        { 0, 0, 0, 0, 1, 0, 1, 1 }, { 1, 0, 0, 0, 1, 0, 1, 1 }, { 0, 1, 0, 0, 1, 0, 1, 1 }, { 1, 1, 0, 0, 1, 0, 1, 1 },
        { 0, 0, 1, 0, 1, 0, 1, 1 }, { 1, 0, 1, 0, 1, 0, 1, 1 }, { 0, 1, 1, 0, 1, 0, 1, 1 }, { 1, 1, 1, 0, 1, 0, 1, 1 },
        { 0, 0, 0, 1, 1, 0, 1, 1 }, { 1, 0, 0, 1, 1, 0, 1, 1 }, { 0, 1, 0, 1, 1, 0, 1, 1 }, { 1, 1, 0, 1, 1, 0, 1, 1 },
        { 0, 0, 1, 1, 1, 0, 1, 1 }, { 1, 0, 1, 1, 1, 0, 1, 1 }, { 0, 1, 1, 1, 1, 0, 1, 1 }, { 1, 1, 1, 1, 1, 0, 1, 1 },
        { 0, 0, 0, 0, 0, 1, 1, 1 }, { 1, 0, 0, 0, 0, 1, 1, 1 }, { 0, 1, 0, 0, 0, 1, 1, 1 }, { 1, 1, 0, 0, 0, 1, 1, 1 },
        { 0, 0, 1, 0, 0, 1, 1, 1 }, { 1, 0, 1, 0, 0, 1, 1, 1 }, { 0, 1, 1, 0, 0, 1, 1, 1 }, { 1, 1, 1, 0, 0, 1, 1, 1 },
        { 0, 0, 0, 1, 0, 1, 1, 1 }, { 1, 0, 0, 1, 0, 1, 1, 1 }, { 0, 1, 0, 1, 0, 1, 1, 1 }, { 1, 1, 0, 1, 0, 1, 1, 1 },
        { 0, 0, 1, 1, 0, 1, 1, 1 }, { 1, 0, 1, 1, 0, 1, 1, 1 }, { 0, 1, 1, 1, 0, 1, 1, 1 }, { 1, 1, 1, 1, 0, 1, 1, 1 },
        { 0, 0, 0, 0, 1, 1, 1, 1 }, { 1, 0, 0, 0, 1, 1, 1, 1 }, { 0, 1, 0, 0, 1, 1, 1, 1 }, { 1, 1, 0, 0, 1, 1, 1, 1 },
        { 0, 0, 1, 0, 1, 1, 1, 1 }, { 1, 0, 1, 0, 1, 1, 1, 1 }, { 0, 1, 1, 0, 1, 1, 1, 1 }, { 1, 1, 1, 0, 1, 1, 1, 1 },
        { 0, 0, 0, 1, 1, 1, 1, 1 }, { 1, 0, 0, 1, 1, 1, 1, 1 }, { 0, 1, 0, 1, 1, 1, 1, 1 }, { 1, 1, 0, 1, 1, 1, 1, 1 },
        { 0, 0, 1, 1, 1, 1, 1, 1 }, { 1, 0, 1, 1, 1, 1, 1, 1 }, { 0, 1, 1, 1, 1, 1, 1, 1 }, { 1, 1, 1, 1, 1, 1, 1, 1 }
    };

    unsigned char* endUnpacked = unpacked + npackedPixels;

    do {
        memcpy(unpacked, unpackArray[*packed], 8);
        packed++;
        unpacked += 8;
    } while (unpacked < endUnpacked);
}

/*-------------------------------------------------
 * FUNCTION: scaleDecompose
 *
 * DESCRIPTION:
 *
 *
 */

void scaleDecompose(double scale1,
    double scale2,
    int32& irscale,
    double& rrscale)
{
    double diff;

    diff = scale1 / scale2;

    if (diff >= 0.0) {
        irscale = (int)diff;
        rrscale = diff - irscale;
    } else {
        diff = -diff;
        irscale = (int)diff;
        irscale = -irscale;
        rrscale = diff + irscale;
        if (rrscale != 0.0) {
            irscale--;
            rrscale = 1.0 - rrscale;
        }
    }
}

/*-------------------------------------------------
 * FUNCTION: raster::copyBasics
 *
 * DESCRIPTION: Copy the basic properties of a raster
 *
 *
 */

void raster::copyBasics(int& err, raster& base)
{
    int i;

    size = base.size;
    tileSize = base.tileSize;
    pack = base.pack;
    data = base.data;
    nplanes = base.nplanes;
    hasStoredMMsize = base.hasStoredMMsize;
    storedMMsize = base.storedMMsize;

    for (i = 0; i < RAST_NUMPLANES; i++)
        sigbits[i] = base.sigbits[i];

    numColours = base.numColours;
    if (numColours > 0) {
        colours = new mx_colour[numColours];
        for (int i = 0; i < numColours; i++) {
            colours[i] = base.colours[i];
        }
    }
}

/*-------------------------------------------------
 * FUNCTION: raster::createCache
 *
 * DESCRIPTION: Constructor
 *
 *
 */

void raster::createCache(int& err, RAST_CACHE icacheType)
{
    cacheType = icacheType;

    switch (cacheType) {
    case RCACHE_TILE:
        tileBuffer = new unsigned char[tileByteSize()];
        break;
    case RCACHE_DICT:
        tileDict.clear();
        break;
    default:
        break;
    }
}

/*-------------------------------------------------
 * FUNCTION: raster::raster
 *
 * DESCRIPTION: Constructor
 *
 *
 */

raster::raster(int& err)
{
    err = globalRascache.error;
    MX_ERROR_CHECK(err);

    buffer = nullptr;
    bufferSize = 0;
    tileBuffer = nullptr;
    colours = nullptr;
    hasStoredMMsize = FALSE;
    storedMMsize.x = 1;
    storedMMsize.y = 1;

abort:
    return;
}

/*-------------------------------------------------
 * FUNCTION: raster::~raster
 *
 * DESCRIPTION: Destructor - delete the subsampled
 * raster.
 *
 */

raster::~raster()
{
    int err;

    if (buffer != nullptr)
        delete[] buffer;
    if (buffer != nullptr)
        delete[] tileBuffer;
    if (buffer != nullptr)
        delete[] colours;

    // Delete this raster from the global cache
    globalRascache.del(err, this);
    tileDict.clear();
}

/*-------------------------------------------------
 * FUNCTION: nullTile
 *
 * DESCRIPTION: Get a tile of zero data
 *
 *
 */

static unsigned char* nullTile(int& err,
    int32 requiredSize)
{
    static int32 size = 0;
    static unsigned char* zbuffer = nullptr;

    err = MX_ERROR_OK;

    // New buffer required - delete the old one
    if (requiredSize > size) {
        delete[] zbuffer;

        // Create the new buffer
        zbuffer = new unsigned char[requiredSize];

        size = requiredSize;
    }

    // Fill with zeroes
    memset(zbuffer, 0, requiredSize);

    return zbuffer;
}

/*-------------------------------------------------
 * FUNCTION: tile
 *
 * DESCRIPTION: Get a tile of raster data
 *
 *
 */

unsigned char* raster::tile(int& err, bool getForEdit, bool& unknown, mx_ipoint& tilePos, rasItem** retItem)
{
    rasItem* item;
    mx_attribute_value data;
    int tileInd;
    unsigned char* thisBuffer;
    int tilebytesize;

    data.data = nullptr;

    err = MX_ERROR_OK;
    if (retItem != nullptr)
        *retItem = nullptr;

    // Store the tile size
    tilebytesize = tileByteSize();

    // If not in range return zero tile
    if (!inTileRange(tilePos)) {
        unknown = TRUE;
        return nullTile(err, tilebytesize);
    } else {
        unknown = FALSE;
    }

    if (tileDict.size() > 0) {

        // Create index value
        tileInd = tileIndex(tilePos);

        // Is it in the collection of tiles
        // held on this raster
        if (tileDict.find(tileInd) == tileDict.end()) {

            // tile not in cache - get a new one
            // malloc the data
            thisBuffer = new unsigned char[tilebytesize];

            // Call the virtual function to get the tile data
            // For the type of raster this is
            getTile(err, thisBuffer, getForEdit, unknown, tilePos);
            MX_ERROR_CHECK(err);

            if (unknown) {
                // couldnt find data
                delete[] thisBuffer;
                return nullTile(err, tilebytesize);
            }

            // Add tile to the global list
            globalRascache.add(err, this, tileInd, thisBuffer, &item);
            MX_ERROR_CHECK(err);

            // Add to the dictionary
            tileDict[tileInd] = item;
            data.data = item;
            if (retItem != nullptr) {
                *retItem = item;
            }
        }
        else {
            // got the data
            item = (rasItem*)data.data;

            if (retItem != nullptr)
                *retItem = item;

            // move it to the head of the list
            globalRascache.move(item);

            return item->ibuffer;
        }

    } else if (tileBuffer != nullptr) {
        // A one and only tile for this buffer
        thisBuffer = tileBuffer;
        thisBuffer = getTile(err, thisBuffer, getForEdit, unknown, tilePos);
        MX_ERROR_CHECK(err);
    } else {
        // No caching - get direct - assume that the
        // getTile routine does its own buffering
        thisBuffer = getTile(err, nullptr, getForEdit, unknown, tilePos);
        MX_ERROR_CHECK(err);
    }

    return thisBuffer;

abort:
    return nullptr;
}

/*-------------------------------------------------
 * FUNCTION: put
 *
 * DESCRIPTION: Put a tile of raster data
 *
 *
 */

void raster::put(int& err,
    unsigned char* data,
    bool flush,
    mx_ipoint& tilePos)
{
    rasItem* item;
    bool unknown;
    unsigned char* ptr;

    err = MX_ERROR_OK;

    ptr = tile(err, TRUE, unknown, tilePos, &item);
    MX_ERROR_CHECK(err);

    if (unknown)
        return;

    if (ptr == nullptr) {
        ptr = data;
    } else {
        if (ptr != data)
            memcpy(ptr, data, tileByteSize());
    }

    if (tileDict.size() != 0) {
        if (flush) {
            // Edited the buffer - put it
            putTile(err, ptr, tilePos);
            MX_ERROR_CHECK(err);
        } else {
            // Just mark as edited
            REDIT(item);
        }
    } else {
        // Edited the buffer - put it
        putTile(err, ptr, tilePos);
        MX_ERROR_CHECK(err);
    }

    return;

abort:
    return;
}

/*-------------------------------------------------
 * FUNCTION: removeTile
 *
 * DESCRIPTION: Remove a tile from the dictionary
 *
 *
 */

void raster::removeTile(int& err, unsigned char* buffer, int32 tileIndex,
    bool isEdited)
{
    err = MX_ERROR_OK;

    if (isEdited) {
        mx_ipoint pt = tileCoord(tileIndex);

        putTile(err, buffer, pt);
        MX_ERROR_CHECK(err);
    }

    // delete the memory and remove from the buffer
    delete[] buffer;

    tileDict.erase(tileIndex);
    return;

abort:
    return;
}

/*-------------------------------------------------
 * FUNCTION: raster::getBuffer
 *
 * DESCRIPTION: Get a buffer of raster data.
 * The buffer is determined by the pixelCoords
 * rect
 */

unsigned char* raster::getBuffer(int& err, mx_irect& pixelCoords)
{
    mx_ipoint tileCoord;
    unsigned char* ptr;
    mx_irect tilePixelCoords, commonArea;
    int nbits;
    int rowSize;
    int thisBufferSize;

    static unsigned char* retBuffer = nullptr;
    static unsigned char retBufferSize = 0;

    err = MX_ERROR_OK;

    // work out a buffer size to hold all the data

    rowSize = rasterBytes(pack,
        pixelCoords.xt - pixelCoords.xb + 1,
        nplanes);
    thisBufferSize = rowSize * (pixelCoords.yt - pixelCoords.yb + 1);

    if (thisBufferSize > retBufferSize) {
        delete[] retBuffer;
        retBuffer = nullptr;

        retBuffer = new unsigned char[thisBufferSize];

        retBufferSize = thisBufferSize;
    }

    nbits = rasterBits(pack, 1, nplanes);

    // Set the iterator on the input raster
    setIterator(err, pixelCoords);
    MX_ERROR_CHECK(err);

    do {
        // Get the next input data
        ptr = next(err, tilePixelCoords, tileCoord);
        MX_ERROR_CHECK(err);

        if (ptr != nullptr) {
            /* get the common area between the tile and the desired
               area */

            commonArea = tilePixelCoords;

            if (pixelCoords.xb > tilePixelCoords.xb) {
                commonArea.xb = pixelCoords.xb;
            }

            if (pixelCoords.xt < tilePixelCoords.xt) {
                commonArea.xt = pixelCoords.xt;
            }

            if (pixelCoords.yb > tilePixelCoords.yb) {
                commonArea.yb = pixelCoords.yb;
            }

            if (pixelCoords.yt < tilePixelCoords.yt) {
                commonArea.yt = pixelCoords.yt;
            }

            copyBuffer(ptr,
                tilePixelCoords,
                commonArea,
                nbits,
                retBuffer,
                pixelCoords,
                commonArea);
        }
    } while (ptr != nullptr);

    return retBuffer;
abort:
    return nullptr;
}

/*-------------------------------------------------
 * FUNCTION: rasterIterator::rasterIterator
 *
 * DESCRIPTION: Raster tile iterator
 *
 *
 */

rasterIterator::rasterIterator(bool iignoreUnknown)
{
    setIgnoreUnknown(iignoreUnknown);
}

/*-------------------------------------------------
 * FUNCTION: rasterIterator::set
 *
 * DESCRIPTION: Set the raster iterator for output
 * This requires a raster to iterate over and the
 * area over which to get the data
 */

void rasterIterator::set(int& err, raster* iraster, mx_irect& inrect)
{
    err = MX_ERROR_OK;

    // Raster cant be nullptr
    if (iraster == nullptr) {
        err = MX_ERROR_NULL_RASTER;
        return;
    }

    // Normalise the rectangle
    mx_irect nr = inrect.normalised();

    // Store the raster
    thisRaster = iraster;

    // get coordinates for start and end points

    mx_ipoint tl = nr.topLeft();
    mx_ipoint br = nr.bottomRight();

    iraster->pointToTile(tl, startTile);
    iraster->pointToTile(br, endTile);

    // Set current coordinate
    currentTile = startTile;

    return;
}

/*-------------------------------------------------
 * FUNCTION: rIterator::next()
 *
 * DESCRIPTION: Get the next tile in the iteration
 * The currentPixels mx_irect returns the corrdinates of
 * the pixels returned in the tile
 */

unsigned char* rasterIterator::next(int& err,
    mx_irect& currentPixels,
    mx_ipoint& tileCoords)
{
    unsigned char* rptr;
    bool unknown;

    // set the end
    err = MX_ERROR_OK;

    do {
        tileCoords = currentTile;

        // Have we reached the end
        if (currentTile.y > endTile.y) {
            thisRaster = nullptr;
            return nullptr;
        }

        // Get the tile for the current raster
        rptr = thisRaster->tile(err, FALSE, unknown, currentTile);
        MX_ERROR_CHECK(err);

        // Get the coordinates of this tile
        thisRaster->tileToPixels(currentTile, currentPixels);

        // if we have reached the end of the x row
        // move to the start fof the next line
        if (currentTile.x == endTile.x) {
            currentTile.x = startTile.x;
            (currentTile.y)++;
        } else {
            (currentTile.x)++;
        }

    } while (ignoreUnknown && unknown);

    goto exit;

abort:
    rptr = nullptr;
exit:

    return rptr;
}

/*-------------------------------------------------
 * FUNCTION: getTiles
 *
 * DESCRIPTION: Get the number of tiles in the raster
 *
 *
 */

mx_ipoint raster::getTiles()
{
    mx_ipoint pt;

    pt.x = (size.x + tileSize.x - 1) / tileSize.x;
    pt.y = (size.y + tileSize.y - 1) / tileSize.y;

    return pt;
}

/*-------------------------------------------------
 * FUNCTION: totalTiles
 *
 * DESCRIPTION: Get the total number of tiles in the raster
 *
 *
 */

int raster::totalTiles()
{
    mx_ipoint pt = getTiles();

    return pt.x * pt.y;
}

/*-------------------------------------------------
 * FUNCTION: copyData
 *
 * DESCRIPTION: Copy data from this raster to another
 *
 *
 */

void raster::copyData(int& err,
    raster& dest,
    mx_irect& extent)
{
    mx_ipoint tileCoord;
    unsigned char* ptr;

    // Set the iterator on the input raster
    setIterator(err, extent);
    MX_ERROR_CHECK(err);

    (void)tileByteSize();

    do {
        // Get the next input data
        ptr = next(err, extent, tileCoord);
        MX_ERROR_CHECK(err);

        if (ptr != nullptr) {
            // and put it (with flush)
            dest.put(err, ptr, TRUE, tileCoord);
            MX_ERROR_CHECK(err);
        }
    } while (ptr != nullptr);

    return;
abort:
    return;
}

/*-------------------------------------------------
 * FUNCTION: raster::defaultMMsize
 *
 * DESCRIPTION: Copy data from this raster to another
 *
 *
 */

void raster::defaultMMsize()
{
    hasStoredMMsize = FALSE;
    storedMMsize = size;
}

/*-------------------------------------------------
 * FUNCTION: raster::inputRasterSize
 *
 * DESCRIPTION: Calculate the size of the raster
 * in an image
 *
 */

mx_point raster::areaSize(int& err,
    mx_point& pageSize,
    double pixelResolution)
{
    mx_point areaSize, pixSize, scaleFactor;
    double maxFactor;

    err = MX_ERROR_OK;

    if (hasStoredMMsize) {
        // we know the size exactly
        areaSize = storedMMsize;
    } else {
        // we only know the size is a measure of the aspect
        // ratio of the size of the image

        // get the sizes assuming one pixel per screen pixel

        pixSize = size / pixelResolution;

        pixSize.x = size.x / pixelResolution;
        pixSize.y = size.y / pixelResolution;

        // take the maximum dimension and then scale the
        // other dimenson to give the correct aspect ratio to
        // the image

        if (pixSize.x > pixSize.y) {
            areaSize.x = pixSize.x;

            // scale the y size
            areaSize.y = (pixSize.x * storedMMsize.y) / storedMMsize.x;
        } else {
            areaSize.y = pixSize.y;

            // scale the x size
            areaSize.x = (pixSize.y * storedMMsize.x) / storedMMsize.y;
        }
    }

    // the size just calculated may be too big for the page
    // if it is scale the image to just fit on a page

    scaleFactor.x = areaSize.x / pageSize.x;
    scaleFactor.y = areaSize.y / pageSize.y;

    if ((scaleFactor.x > 1.0) || (scaleFactor.y > 1.0)) {
        maxFactor = GMAX(scaleFactor.x, scaleFactor.y);

        areaSize = areaSize / maxFactor;
    }

    // clip explicitly - to get around rounding
    if (areaSize.x > pageSize.x)
        areaSize.x = pageSize.x;
    if (areaSize.y > pageSize.y)
        areaSize.y = pageSize.y;

    // snap on to pageSize

    if (GABS((areaSize.x / pageSize.x) - 1.0) < GEOM_SMALL) {
        areaSize.x = pageSize.x;
    }

    if (GABS((areaSize.y / pageSize.y) - 1.0) < GEOM_SMALL) {
        areaSize.y = pageSize.y;
    }

    return areaSize;
}

/*-------------------------------------------------
 * FUNCTION: raster::setSize
 *
 * DESCRIPTION: Calculate the resolution for a
 * display raster
 *
 */

void raster::setSize(int& err,
    mx_point& areaSize)
{
    err = MX_ERROR_OK;

    storedMMsize = areaSize;
}

/*-------------------------------------------------
 * FUNCTION: raster::sigbitsFromColours
 *
 * DESCRIPTION: Calculate the resolution for a
 * display raster
 *
 */

int raster::sigbitsFromColours()
{
    int n2;
    int sigbits;

    sigbits = 0;
    n2 = 1;

    while (numColours > n2) {
        n2 = n2 << 1;
        sigbits++;
    }

    return sigbits;
}
