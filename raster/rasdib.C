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
 * MODULE : rasdib.C
 *
 * AUTHOR : David Miller
 *
 * DESCRIPTION:
 * Module rasdib.C
 *
 *
 */

#include "maxwell.h"
#include "rasdib.h"

/*-------------------------------------------------
 * FUNCTION: rasdib::rasdib
 *
 * DESCRIPTION: Constructor of a tiff raster
 *
 *
 */

rasdib::~rasdib()
{
    delete[] dibbuffer;
}

inline unsigned char rasdib::hexStr(char str)
{
    switch (str) {
    case '0':
        return 0x0;
        break;
    case '1':
        return 0x1;
        break;
    case '2':
        return 0x2;
        break;
    case '3':
        return 0x3;
        break;
    case '4':
        return 0x4;
        break;
    case '5':
        return 0x5;
        break;
    case '6':
        return 0x6;
        break;
    case '7':
        return 0x7;
        break;
    case '8':
        return 0x8;
        break;
    case '9':
        return 0x9;
        break;
    case 'a':
        return 0xa;
        break;
    case 'b':
        return 0xb;
        break;
    case 'c':
        return 0xc;
        break;
    case 'd':
        return 0xd;
        break;
    case 'e':
        return 0xe;
        break;
    case 'f':
        return 0xf;
        break;
    }
    return 0x0;
}

/*-------------------------------------------------
 * FUNCTION: rasdib::rasdib
 *
 * DESCRIPTION: Constructor of a tiff raster
 *
 *
 */

rasdib::rasdib(int& err,
    mx_mxmetafile_output& metafileData,
    RAST_CACHE cacheType)
    : raster(err)
{
    unsigned char* outbuffer;
    char* inbuffer;
    unsigned char byte1, byte2;
    int nbytes;
    bool invertedColours = FALSE;
    bool normalColours = FALSE;

    MX_ERROR_CHECK(err);

    dibbuffer = NULL;

    if (metafileData.stretchDIB.bitmapinfo.biBitCount == 1) {
        tileSize.x = ((metafileData.stretchDIB.bitmapinfo.biWidth + 7) / 8) * 8;
    } else {
        tileSize.x = metafileData.stretchDIB.bitmapinfo.biWidth;
    }

    tileSize.y = metafileData.stretchDIB.bitmapinfo.biHeight;

    size.x = metafileData.stretchDIB.srcXext;
    size.y = metafileData.stretchDIB.srcYext;

    defaultMMsize();

    if ((metafileData.size.x != -1) && (metafileData.size.y != -1)) {
        hasStoredMMsize = TRUE;
        storedMMsize.x = metafileData.size.x;
        storedMMsize.y = metafileData.size.y;
    }

    if (metafileData.stretchDIB.bitmapinfo.biBitCount == 1) {
        pack = RPACK_BIT;
        data = RDATA_BINARY;
        nplanes = 1;
        sigbits[0] = 1;

        invertedColours = (metafileData.mxDIBcolours[0].red == 0) && (metafileData.mxDIBcolours[0].green == 0) && (metafileData.mxDIBcolours[0].blue == 0) && (metafileData.mxDIBcolours[1].red == 255) && (metafileData.mxDIBcolours[1].green == 255) && (metafileData.mxDIBcolours[1].blue == 255);

        normalColours = (metafileData.mxDIBcolours[1].red == 0) && (metafileData.mxDIBcolours[1].green == 0) && (metafileData.mxDIBcolours[1].blue == 0) && (metafileData.mxDIBcolours[0].red == 255) && (metafileData.mxDIBcolours[0].green == 255) && (metafileData.mxDIBcolours[0].blue == 255);

        if (normalColours || invertedColours) {
            numColours = 0;
            colours = NULL;
        } else {
            numColours = 2;

            // Allocate space for the colours
            colours = new mx_colour[2];

            colours[0] = metafileData.mxDIBcolours[0];
            colours[1] = metafileData.mxDIBcolours[1];
        }
    } else if ((metafileData.stretchDIB.bitmapinfo.biBitCount == 2) || (metafileData.stretchDIB.bitmapinfo.biBitCount == 4) || (metafileData.stretchDIB.bitmapinfo.biBitCount == 8)) {
        pack = RPACK_BYTE;
        nplanes = 1;

        sigbits[0] = metafileData.stretchDIB.bitmapinfo.biBitCount;

        if (metafileData.isGreyScale) {
            data = RDATA_GREYSCALE;
            numColours = 0;
            colours = NULL;
        } else {
            data = RDATA_COLOUR;

            numColours = metafileData.stretchDIB.bitmapinfo.biClrUsed;

            // Allocate space for the colours
            colours = new mx_colour[numColours];

            for (int icolour = 0; icolour < numColours; icolour++) {
                colours[icolour] = metafileData.mxDIBcolours[icolour];
            }
        }
    } else if (metafileData.stretchDIB.bitmapinfo.biBitCount == 24) {
        pack = RPACK_BYTE;
        nplanes = 3;
        data = RDATA_RGB;
        numColours = 0;
        colours = NULL;

        sigbits[0] = sigbits[1] = sigbits[2] = 8;
    }

    nbytes = tileByteSize();

    // create the buffer
    outbuffer = dibbuffer = new unsigned char[nbytes];
    inbuffer = metafileData.stretchDIB.data;

    if (metafileData.stretchDIB.bitmapinfo.biBitCount == 1) {
        // the data is a hex string which need to be converted
        // to bytes

        char* inRowBuffer = inbuffer;
        int rowByteSize = tileSize.x / 8;
        unsigned char* outRowBuffer = outbuffer + nbytes - rowByteSize;
        int padding = rowByteSize * 8 - metafileData.stretchDIB.bitmapinfo.biWidth;

        for (int iheight = 0; iheight < metafileData.stretchDIB.bitmapinfo.biHeight;
            iheight++) {
            static unsigned char transform[16] = { 0, 8, 4, 12, 2, 10, 6, 14,
                1, 9, 5, 13, 3, 11, 7, 15 };

            inbuffer = inRowBuffer;
            outbuffer = outRowBuffer;

            for (int iwidth = 0; iwidth < rowByteSize; iwidth++) {
                byte1 = hexStr(*inbuffer);
                inbuffer++;
                byte2 = hexStr(*inbuffer);
                inbuffer++;

                *outbuffer++ = transform[byte1] | (transform[byte2] << 4);
            }

            if ((padding > 0) && (invertedColours || (numColours != 0))) {
                static int paddingValues[8] = { 0, 128, 192, 224, 240, 248, 252, 254 };
                // we need to blat the padding with ones
                *(outbuffer - 1) |= paddingValues[padding];
            }

            inRowBuffer = inbuffer;
            outRowBuffer -= rowByteSize;
        }

        if (invertedColours) {
            rasterInvert(nbytes, dibbuffer, dibbuffer);
        }
    } else if (metafileData.stretchDIB.bitmapinfo.biBitCount == 4) {
        int nbytesPerInputRow = (metafileData.stretchDIB.bitmapinfo.biSizeImage / metafileData.stretchDIB.bitmapinfo.biHeight) * 2;

        char* inRowBuffer = inbuffer;
        unsigned char* outRowBuffer = outbuffer + nbytes - metafileData.stretchDIB.bitmapinfo.biWidth;

        for (int iheight = 0; iheight < metafileData.stretchDIB.bitmapinfo.biHeight;
            iheight++) {
            inbuffer = inRowBuffer;
            outbuffer = outRowBuffer;

            for (int iwidth = 0; iwidth < metafileData.stretchDIB.bitmapinfo.biWidth;
                iwidth++) {
                *outbuffer++ = hexStr(*inbuffer);
                inbuffer++;
            }

            inRowBuffer += nbytesPerInputRow;
            outRowBuffer -= metafileData.stretchDIB.bitmapinfo.biWidth;
        }
    } else if (metafileData.stretchDIB.bitmapinfo.biBitCount == 8) {
        int nbytesPerInputRow = (metafileData.stretchDIB.bitmapinfo.biSizeImage / metafileData.stretchDIB.bitmapinfo.biHeight) * 2;

        char* inRowBuffer = inbuffer;
        unsigned char* outRowBuffer = outbuffer + nbytes - metafileData.stretchDIB.bitmapinfo.biWidth;

        for (int iheight = 0; iheight < metafileData.stretchDIB.bitmapinfo.biHeight;
            iheight++) {
            inbuffer = inRowBuffer;
            outbuffer = outRowBuffer;

            for (int iwidth = 0; iwidth < metafileData.stretchDIB.bitmapinfo.biWidth;
                iwidth++) {
                byte1 = hexStr(*inbuffer);
                inbuffer++;
                byte2 = hexStr(*inbuffer);
                inbuffer++;

                *outbuffer++ = byte2 | (byte1 << 4);
            }

            inRowBuffer += nbytesPerInputRow;
            outRowBuffer -= metafileData.stretchDIB.bitmapinfo.biWidth;
        }
    } else if (metafileData.stretchDIB.bitmapinfo.biBitCount == 24) {
        int nbytesPerInputRow = (metafileData.stretchDIB.bitmapinfo.biSizeImage / metafileData.stretchDIB.bitmapinfo.biHeight) * 2;

        char* inRowBuffer = inbuffer;
        unsigned char* outRowBuffer = outbuffer + nbytes - (metafileData.stretchDIB.bitmapinfo.biWidth * 3);

        for (int iheight = 0; iheight < metafileData.stretchDIB.bitmapinfo.biHeight;
            iheight++) {
            inbuffer = inRowBuffer;
            outbuffer = outRowBuffer;

            for (int iwidth = 0; iwidth < metafileData.stretchDIB.bitmapinfo.biWidth;
                iwidth++) {
                byte1 = hexStr(*inbuffer);
                inbuffer++;
                byte2 = hexStr(*inbuffer);
                inbuffer++;

                *(outbuffer + 2) = byte2 | (byte1 << 4);

                byte1 = hexStr(*inbuffer);
                inbuffer++;
                byte2 = hexStr(*inbuffer);
                inbuffer++;

                *(outbuffer + 1) = byte2 | (byte1 << 4);

                byte1 = hexStr(*inbuffer);
                inbuffer++;
                byte2 = hexStr(*inbuffer);
                inbuffer++;

                *outbuffer = byte2 | (byte1 << 4);

                outbuffer += 3;
            }

            inRowBuffer += nbytesPerInputRow;
            outRowBuffer -= (metafileData.stretchDIB.bitmapinfo.biWidth * 3);
        }
    }

    return;
abort:
    return;
}

/*-------------------------------------------------
 * FUNCTION: rasdib::getTile
 *
 * DESCRIPTION: The virtual function for getting a
 * tile of raster from a TIFF file. The tile is
 * a TIFF strip
 */

unsigned char* rasdib::getTile(int& err,
    unsigned char* buffer,
    bool getForEdit,
    bool& unknown,
    mx_ipoint& itile)
{
    err = MX_ERROR_OK;
    unknown = FALSE;

    if (buffer != NULL) {
        memcpy(buffer, dibbuffer, tileByteSize());
        return buffer;
    } else {
        return dibbuffer;
    }
}
