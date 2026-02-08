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
 * MODULE : rasxbm.C
 *
 * AUTHOR : David Miller
 *
 * This file
 *
 * DESCRIPTION:
 * Module rasxbm.C
 *
 *
 *
 *
 * $id$
 * $log$
 *
 */

#include "maxwell.h"
#include "rasxbm.h"
#include <xpm.h>

typedef enum {
    RAS_XBM,
    RAS_XPM
} rasxbm_t;

/*-------------------------------------------------
 * FUNCTION: rasxbm::rasxbm
 *
 * DESCRIPTION: Constructor of a tiff raster
 *
 *
 */

rasxbm::~rasxbm()
{
    delete[] xbmbuffer;
}

/*-------------------------------------------------
 * FUNCTION: rasxbm::xbm
 *
 * DESCRIPTION: Constructor of a tiff raster
 *
 *
 */

void rasxbm::xbm(int& err, char* filename)
{
    int nbytes;
    unsigned int width, height;
    int t1, t2;
    unsigned char* buffer = NULL;
    int retValue;

    err = MX_ERROR_OK;

    retValue = XReadBitmapFileData(filename,
        &width,
        &height,
        &buffer,
        &t1, &t2);
    if (retValue == BitmapOpenFailed) {
        MX_ERROR_THROW(err, MX_ERROR_NO_XBM_FILE);
    } else if (retValue != BitmapSuccess) {
        MX_ERROR_THROW(err, MX_ERROR_INTERNAL_XBM_ERROR);
    }

    // set basic properties
    tileSize.x = width;
    tileSize.y = height;

    size = tileSize;

    defaultMMsize();

    pack = RPACK_BIT;
    data = RDATA_BINARY;
    nplanes = 1;
    sigbits[0] = 1;
    numColours = 0;

    nbytes = tileByteSize();

    // create the buffer
    xbmbuffer = new unsigned char[nbytes];

    // buffer starts with '
    memcpy(xbmbuffer, buffer, nbytes);
    XFree(buffer);

abort:
    return;
}

/*-------------------------------------------------
 * FUNCTION: rasxpm::xpm
 *
 * DESCRIPTION: Constructor of a tiff raster
 *
 *
 */

void rasxbm::xpm(int& err, char* filename)
{
    int nbytes;
    XpmImage image;
    XColor xcolor;
    int retValue;
    int icolour;
    char* colStr = nullptr;
    XpmColor* xpmColor;
    int ibyte;

    err = MX_ERROR_OK;

    retValue = XpmReadFileToXpmImage(filename, &image, NULL);
    if (retValue == XpmOpenFailed) {
        MX_ERROR_THROW(err, MX_ERROR_NO_XBM_FILE);
    } else if (retValue != XpmSuccess) {
        MX_ERROR_THROW(err, MX_ERROR_INTERNAL_XBM_ERROR);
    }

    // set basic properties
    tileSize.x = image.width;
    tileSize.y = image.height;
    size = tileSize;

    numColours = image.ncolors;

    if (image.cpp != 1) {
        MX_ERROR_THROW(err, MX_ERROR_INTERNAL_XBM_ERROR);
    }

    defaultMMsize();
    pack = RPACK_BYTE;
    data = RDATA_COLOUR;
    nplanes = 1;

    sigbits[0] = sigbitsFromColours();

    // Allocate space for the colours
    colours = new mx_colour[numColours];

    for (icolour = 0; icolour < numColours; icolour++) {
        xpmColor = image.colorTable + icolour;

        if (xpmColor->c_color != NULL) {
            colStr = xpmColor->c_color;
        } else if (xpmColor->g_color != NULL) {
            colStr = xpmColor->g_color;
        } else if (xpmColor->g4_color != NULL) {
            colStr = xpmColor->g4_color;
        } else if (xpmColor->m_color != NULL) {
            colStr = xpmColor->m_color;
        }

        if (!XParseColor(XtDisplay(global_top_level),
                DefaultColormap(XtDisplay(global_top_level), 0),
                colStr,
                &xcolor)) {
            MX_ERROR_THROW(err, MX_ERROR_INTERNAL_XBM_ERROR);
        }

        colours[icolour].red = xcolor.red >> 8;
        colours[icolour].green = xcolor.green >> 8;
        colours[icolour].blue = xcolor.blue >> 8;
        colours[icolour].name = NULL;
    }

    nbytes = tileByteSize();

    // create the buffer
    xbmbuffer = new unsigned char[nbytes];

    for (ibyte = 0; ibyte < nbytes; ibyte++) {
        xbmbuffer[ibyte] = (unsigned char)image.data[ibyte];
    }

    XpmFreeXpmImage(&image);

abort:
    return;
}

/*-------------------------------------------------
 * FUNCTION: rasxbm::rasxbm
 *
 * DESCRIPTION: Constructor of a tiff raster
 *
 *
 */

rasxbm::rasxbm(int& err,
    char* filename,
    bool isBitmap,
    RAST_CACHE cacheType)
    : raster(err)
{
    xbmbuffer = NULL;

    if (isBitmap) {
        xbm(err, filename);
        MX_ERROR_CHECK(err);
    } else {
        xpm(err, filename);
        MX_ERROR_CHECK(err);
    }

    return;
abort:
    return;
}

/*-------------------------------------------------
 * FUNCTION: rasxbm::getTile
 *
 * DESCRIPTION: The virtual function for getting a
 * tile of raster from a TIFF file. The tile is
 * a TIFF strip
 */

unsigned char* rasxbm::getTile(int& err,
    unsigned char* buffer,
    bool getForEdit,
    bool& unknown,
    mx_ipoint& itile)
{
    err = MX_ERROR_OK;
    unknown = FALSE;

    if (buffer != NULL) {
        memcpy(buffer, xbmbuffer, tileByteSize());
        return buffer;
    } else {
        return xbmbuffer;
    }
}
