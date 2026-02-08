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
 * MODULE : rasjpeg.C
 *
 * AUTHOR : David Miller
 *
 * This file
 *
 * DESCRIPTION: A TIFF raster
 * Module rasjpeg.C
 *
 *
 *
 *
 * $id$
 * $log$
 *
 */

#include "rasjpeg.h"

/*-------------------------------------------------
 * FUNCTION: MXjpgErrorOutput
 *
 * DESCRIPTION: Horrible routine for jpeg library
 * error handling
 *
 */

METHODDEF(void)
MXjpegErrorOutput(j_common_ptr cinfo)
{
    /* cinfo->err really points to a my_error_mgr struct, so coerce pointer */
    char buffer[JMSG_LENGTH_MAX];

    (*cinfo->err->format_message)(cinfo, buffer);
}

/*-------------------------------------------------
 * FUNCTION: MXjpgErrorExit
 *
 * DESCRIPTION: Horrible routine for jpeg library
 * error handling
 *
 */

METHODDEF(void)
MXjpegErrorExit(j_common_ptr cinfo)
{

    /* cinfo->err really points to a my_error_mgr struct, so coerce pointer */
    MXerrorMgr* errMgr = (MXerrorMgr*)cinfo->err;

    /* Always display the message. */
    /* We could postpone this until after returning, if we chose. */
    (*cinfo->err->output_message)(cinfo);

    /* Return control to the setjmp point */
    longjmp(errMgr->setjmp_buffer, 1);
}

/*-------------------------------------------------
 * FUNCTION: rasjpeg::reset
 *
 * DESCRIPTION:
 *
 *
 */

void rasjpeg::reset(int& err)
{
    err = MX_ERROR_OK;

    /* seek to beginning of file */

    fseek(fp, 0L, 0);

    (void)jpeg_read_header(&cinfo, TRUE);

    if (cinfo.jpeg_color_space == JCS_GRAYSCALE) {
        cinfo.out_color_space = JCS_GRAYSCALE;
        cinfo.quantize_colors = FALSE;
    } else {
        cinfo.out_color_space = JCS_RGB;
        if (get24bits) {
            cinfo.quantize_colors = FALSE;
        } else {
            cinfo.desired_number_of_colors = ncolours;
            cinfo.quantize_colors = TRUE;
            cinfo.two_pass_quantize = TRUE;
        }
    }

    // get dimensions of strcuture
    jpeg_calc_output_dimensions(&cinfo);

    // Start decompress - get the colour data here
    jpeg_start_decompress(&cinfo);

    return;
}

/*-------------------------------------------------
 * FUNCTION: rasjpeg::~rasjpeg
 *
 * DESCRIPTION: Constructor of a jpeg raster
 *
 *
 */

rasjpeg::~rasjpeg()
{
    // Set the error jump for this routine

    if (setjmp(jerr.setjmp_buffer)) {
        return;
    }

    if (fp != NULL) {
        jpeg_destroy_decompress(&cinfo);
        fclose(fp);
        fp = NULL;
    }
}

/*-------------------------------------------------
 * FUNCTION: rasjpeg::rasjpeg
 *
 * DESCRIPTION: Constructor of a jpeg raster
 *
 *
 */

rasjpeg::rasjpeg(int& err,
    char* filename, bool iget24bits,
    int incolours,
    RAST_CACHE cacheType)
    : raster(err)

{
    int bytesPerRow;
    double convFactor;

    MX_ERROR_CHECK(err);

    fp = NULL;
    get24bits = iget24bits;
    ncolours = incolours;

    /* open the file */
    fp = fopen(filename, "rb");
    if (fp == NULL) {
        MX_ERROR_THROW(err, MX_ERROR_NO_JPEG_FILE);
    }

    // Set up the error handling
    cinfo.err = jpeg_std_error(&jerr.pub);
    jerr.pub.error_exit = MXjpegErrorExit;
    jerr.pub.output_message = MXjpegErrorOutput;

    // Set the error jump for this routine
    if (setjmp(jerr.setjmp_buffer)) {
        MX_ERROR_THROW(err, MX_ERROR_INTERNAL_JPEG_ERROR);
    }

    // Initialise decompression stucture
    jpeg_create_decompress(&cinfo);

    // apply source
    jpeg_stdio_src(&cinfo, fp);

    // Set up the jpeg internal structures
    reset(err);
    MX_ERROR_CHECK(err);

    if (cinfo.jpeg_color_space == JCS_GRAYSCALE) {
        nplanes = 1;
        data = RDATA_GREYSCALE;
        numColours = 0;
        colours = NULL;
    } else {
        cinfo.out_color_space = JCS_RGB;
        if (get24bits) {
            nplanes = 3;
            data = RDATA_RGB;
            numColours = 0;
            colours = NULL;
        } else {
            data = RDATA_COLOUR;
            nplanes = 1;
        }
    }

    if (data == RDATA_COLOUR) {
        numColours = cinfo.actual_number_of_colors;

        sigbits[0] = sigbitsFromColours();

        // Allocate space for the colours
        colours = new mx_colour[numColours];

        // Initialise colours
        for (int icol = 0; icol < numColours; icol++) {
            colours[icol].red = cinfo.colormap[0][icol];
            colours[icol].green = cinfo.colormap[1][icol];
            colours[icol].blue = cinfo.colormap[2][icol];
            colours[icol].name = NULL;
        }
    } else {
        // All planes have the same number of bits
        for (int iplane = 0; iplane < nplanes; iplane++) {
            sigbits[iplane] = 8;
        }
    }

    pack = RPACK_BYTE;

    size.x = cinfo.output_width;
    size.y = cinfo.output_height;
    tileSize.x = size.x;

    bytesPerRow = size.x * nplanes;

    tileSize.y = 16384 / bytesPerRow;
    if (tileSize.y == 0)
        tileSize.y = 1;
    if (tileSize.y > size.y)
        tileSize.y = size.y;

    if (cinfo.density_unit == 1) {
        // dots per inch
        hasStoredMMsize = TRUE;
        convFactor = 1.0 / 25.4;
    } else if (cinfo.density_unit == 2) {
        // dots per cm
        hasStoredMMsize = TRUE;
        convFactor = 0.1;
    } else {
        hasStoredMMsize = FALSE;
        convFactor = 1.0;
    }

    hasStoredMMsize = (convFactor != 1.0);
    storedMMsize.x = size.x / (cinfo.X_density * convFactor);
    storedMMsize.y = size.y / (cinfo.Y_density * convFactor);

    createCache(err, cacheType);
    MX_ERROR_CHECK(err);

    return;
abort:
    return;
}

/*-------------------------------------------------
 * FUNCTION: rasjpeg::getTile
 *
 * DESCRIPTION: The virtual function for getting a
 * tile of raster from a TIFF file. The tile is
 * a TIFF strip
 */

unsigned char* rasjpeg::getTile(int& err,
    unsigned char* buffer,
    bool getForEdit,
    bool& unknown,
    mx_ipoint& itile)
{
    int rowSize;
    unsigned int lineWanted;
    int irow;
    JSAMPROW rowptr[1];

    err = MX_ERROR_OK;
    unknown = FALSE;

    // Set up horrible error handling
    if (setjmp(jerr.setjmp_buffer)) {
        MX_ERROR_THROW(err, MX_ERROR_INTERNAL_JPEG_ERROR);
    }

    // Get the first scanline required
    lineWanted = itile.y * tileSize.y;

    // If we have already gone beyond the point
    // wanted then we need to start again
    if (cinfo.output_scanline > lineWanted) {
        jpeg_abort_decompress(&cinfo);
        reset(err);
        MX_ERROR_CHECK(err);
    }

    // Now get up to the required line
    while (cinfo.output_scanline < lineWanted) {
        rowptr[0] = (JSAMPROW)buffer;
        jpeg_read_scanlines(&cinfo, rowptr, 1);
    }

    // Blat the buffer
    rowSize = size.x * nplanes;
    memset(buffer, 0, tileByteSize());

    for (irow = 0; irow < tileSize.y; irow++) {
        // On the right line - do the read
        rowptr[0] = (JSAMPROW)(buffer + irow * rowSize);
        jpeg_read_scanlines(&cinfo, rowptr, 1);
        if (cinfo.output_scanline == (unsigned int)size.y)
            break;
    }

    return buffer;

abort:
    return buffer;
}
