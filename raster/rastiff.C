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
 * MODULE : rastiff.C
 *
 * AUTHOR : David Miller
 *
 * This file
 *
 * DESCRIPTION: A TIFF raster
 * Module rastiff.C
 *
 *
 *
 *
 * $id$
 * $log$
 *
 */

#include "rastiff.h"

/*-------------------------------------------------
 * FUNCTION: mxTiffError
 *
 * DESCRIPTION:
 *
 *
 */

static void mxTiffError(const char* module, const char* fmt, va_list ap)
{
    if (module != NULL)
        fprintf(stderr, "%s: ", module);
    fprintf(stderr, "Warning, ");
    vfprintf(stderr, fmt, ap);
    fprintf(stderr, ".\n");
}

/*-------------------------------------------------
 * FUNCTION: mxTiffError
 *
 * DESCRIPTION:
 *
 *
 */

static void mxTiffWarning(const char* module, const char* fmt, va_list ap)
{
    if (module != NULL)
        fprintf(stderr, "%s: ", module);
    fprintf(stderr, "Warning, ");
    vfprintf(stderr, fmt, ap);
    fprintf(stderr, ".\n");
}

/*-------------------------------------------------
 * FUNCTION: rastiff::rastiff
 *
 * DESCRIPTION: Destructor of a tiff raster
 *
 *
 */

rastiff::~rastiff()
{
    if (tiff != NULL)
        TIFFClose(tiff);
}

/*-------------------------------------------------
 * FUNCTION: rastiff::rastiff
 *
 * DESCRIPTION: Constructor of a tiff raster
 *
 *
 */

rastiff::rastiff(int& err, char* filename, RAST_CACHE cacheType)
    : raster(err)
{
    uint32_t getValue;
    uint16_t getValue16;
    uint16_t *red, *green, *blue;
    int iplane;

    tiff = NULL;
    doFlip = FALSE;

    MX_ERROR_CHECK(err);

    TIFFSetErrorHandler(mxTiffError);
    TIFFSetWarningHandler(mxTiffWarning);

    // Open the file
    tiff = TIFFOpen(filename, "r");
    if (tiff == NULL) {
        MX_ERROR_THROW(err, MX_TIFF_OPEN_FAILED);
    }

    // Get the number of significant bits
    if (!TIFFGetFieldDefaulted(tiff, TIFFTAG_FILLORDER, &getValue16)) {
        MX_ERROR_THROW(err, MX_TIFF_NO_TAG);
    }

    fillorder = getValue16;

    // Image width
    if (!TIFFGetField(tiff, TIFFTAG_IMAGEWIDTH, &getValue)) {
        MX_ERROR_THROW(err, MX_TIFF_NO_TAG);
    }

    size.x = getValue;

    // Image height
    if (!TIFFGetField(tiff, TIFFTAG_IMAGELENGTH, &getValue)) {
        MX_ERROR_THROW(err, MX_TIFF_NO_TAG);
    }

    size.y = getValue;

    // Get the samples per pixel
    if (!TIFFGetFieldDefaulted(tiff, TIFFTAG_SAMPLESPERPIXEL, &getValue16)) {
        MX_ERROR_THROW(err, MX_TIFF_NO_TAG);
    }

    nplanes = getValue16;

    // Get the number of significant bits
    if (!TIFFGetFieldDefaulted(tiff, TIFFTAG_BITSPERSAMPLE, &getValue16)) {
        MX_ERROR_THROW(err, MX_TIFF_NO_TAG);
    }

    // All planes have the same number of bits (???)
    for (iplane = 0; iplane < nplanes; iplane++) {
        sigbits[iplane] = getValue16;
    }

    // See if the TIFF data is tiles
    if (TIFFIsTiled(tiff)) {
        // Yes - get tile size
        if (!TIFFGetField(tiff, TIFFTAG_TILEWIDTH, &getValue)) {
            MX_ERROR_THROW(err, MX_TIFF_NO_TAG);
        }
        tileSize.x = getValue;

        if (!TIFFGetField(tiff, TIFFTAG_TILELENGTH, &getValue)) {
            MX_ERROR_THROW(err, MX_TIFF_NO_TAG);
        }
        tileSize.y = getValue;
    } else {
        // Not tiled - use the rows per strip  as the tile height
        if (!TIFFGetField(tiff, TIFFTAG_ROWSPERSTRIP, &getValue)) {
            MX_ERROR_THROW(err, MX_TIFF_NO_TAG);
        }

        tileSize.y = getValue;

        // Set tile width to be the image size - rounded up to
        // a byte boundary for bit data
        if (sigbits[0] == 1) {
            tileSize.x = ((size.x + 7) / 8) * 8;
        } else {
            tileSize.x = size.x;
        }
    }

    // Get orientation
    if (!TIFFGetFieldDefaulted(tiff, TIFFTAG_ORIENTATION, &getValue16)) {
        MX_ERROR_THROW(err, MX_TIFF_NO_TAG);
    }

    // Values other than ORIENTATION_TOPLEFT are non-standard
    // Return error for the time being
    if (getValue16 != ORIENTATION_TOPLEFT) {
        MX_ERROR_THROW(err, MX_ERROR_TIFF_ORIENTATION);
    }

    // If the number of planes is greater than one then use
    // only files with a planar configuration
    if (nplanes != 1) {
        if (TIFFGetField(tiff, TIFFTAG_PLANARCONFIG, &getValue16)) {
            MX_ERROR_THROW(err, MX_TIFF_NO_TAG);
        }

        if (getValue16 != PLANARCONFIG_CONTIG) {
            MX_ERROR_THROW(err, MX_ERROR_TIFF_PLANARCONFIG);
        }
    }

    // Get the resolution unit

    defaultMMsize();

    if (TIFFGetField(tiff, TIFFTAG_RESOLUTIONUNIT, &getValue16)) {
        double convFactor;
        float getRes1, getRes2;

        if (getValue16 == 2) {
            // dots per inch
            convFactor = 1 / 25.4;
        } else if (getValue == 3) {
            // dots per cm
            convFactor = 0.1;
        } else {
            // unknown
            convFactor = 1.0;
        }

        if (TIFFGetField(tiff, TIFFTAG_XRESOLUTION, &getRes1) && TIFFGetField(tiff, TIFFTAG_YRESOLUTION, &getRes2)) {
            hasStoredMMsize = (convFactor != 1.0);
            storedMMsize.x = size.x / (getRes1 * convFactor);
            storedMMsize.y = size.y / (getRes2 * convFactor);
        }
    }

    // Get the photometric type
    if (!TIFFGetField(tiff, TIFFTAG_PHOTOMETRIC, &getValue16)) {
        MX_ERROR_THROW(err, MX_TIFF_NO_TAG);
    }

    switch (getValue16) {
    case PHOTOMETRIC_MINISWHITE:
        // Binary of greyscale

        data = (sigbits[0] == 1) ? RDATA_BINARY : RDATA_GREYSCALE;
        pack = (sigbits[0] == 1) ? RPACK_BIT : RPACK_BYTE;
        doFlip = (sigbits[0] == 1) ? FALSE : TRUE;
        numColours = 0;
        colours = NULL;
        break;
    case PHOTOMETRIC_MINISBLACK:
        // Binary of greyscale
        data = (sigbits[0] == 1) ? RDATA_BINARY : RDATA_GREYSCALE;
        pack = (sigbits[0] == 1) ? RPACK_BIT : RPACK_BYTE;
        doFlip = (sigbits[0] == 1) ? TRUE : FALSE;
        numColours = 0;
        colours = NULL;
        break;
    case PHOTOMETRIC_PALETTE:
        // Palette colour - must be a colour map
        data = RDATA_COLOUR;
        pack = RPACK_BYTE;

        /* Now read the colour map */
        numColours = (1 << sigbits[0]);

        // Allocate space for the colours
        colours = new mx_colour[numColours];

        // Get array pointers
        if (!TIFFGetField(tiff, TIFFTAG_COLORMAP,
                &red, &green, &blue)) {
            MX_ERROR_THROW(err, MX_TIFF_NO_TAG);
        }

        // Initialise colours
        for (int icol = 0; icol < numColours; icol++) {
            colours[icol].red = red[icol] >> 8;
            colours[icol].green = green[icol] >> 8;
            colours[icol].blue = blue[icol] >> 8;
            colours[icol].name = NULL;
        }

        break;
    case PHOTOMETRIC_RGB:
        // RGB - use natural pixel values as the colour
        data = RDATA_RGB;
        pack = RPACK_BYTE;
        numColours = 0;
        colours = NULL;
        break;
    default:
        MX_ERROR_THROW(err, MX_ERROR_INTERNAL_TIFF_ERROR);
    }

    createCache(err, cacheType);
    MX_ERROR_CHECK(err);

    return;
abort:
    return;
}

/*-------------------------------------------------
 * FUNCTION: rastiff::getTile
 *
 * DESCRIPTION: The virtual function for getting a
 * tile of raster from a TIFF file. The tile is
 * a TIFF strip
 */

unsigned char* rastiff::getTile(int& err,
    unsigned char* buffer,
    bool getForEdit,
    bool& unknown,
    mx_ipoint& itile)
{
    int tilesize, nbytesRead, nextrabytes;

    err = MX_ERROR_OK;
    unknown = FALSE;

    // Get the strip bytes size
    tilesize = tileByteSize();

    if (TIFFIsTiled(tiff)) {
        if (TIFFReadEncodedTile(tiff, tileIndex(itile), buffer, tilesize) == -1) {
            err = MX_ERROR_TIFF_READ;
            return NULL;
        }
    } else {
        // Read the strip - may be the last strip
        nbytesRead = TIFFReadEncodedStrip(tiff, itile.y, buffer, tilesize);

        if (nbytesRead == -1) {
            err = MX_ERROR_TIFF_READ;
            return NULL;
        }

        // Test if the last
        nextrabytes = tilesize - nbytesRead;

        // Blat any extra to zero
        if (nextrabytes > 0)
            memset(buffer + nbytesRead, 0, nextrabytes);
    }

    if ((fillorder == FILLORDER_MSB2LSB) && (nplanes == 1) && (sigbits[0] == 1)) {
        reverseBits(tileByteSize(), buffer, buffer);
    }

    if (doFlip) {
        if (pack == RPACK_BIT) {
            rasterInvert(tilesize, buffer, buffer);
        } else if (pack == RPACK_BYTE) {
            int shiftValue = (1 << sigbits[0]) - 1;
            rasterFlip(tilesize, shiftValue, buffer, buffer);
        }
    }

    return buffer;
}
