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
 * MODULE/CLASS : mx_image_area_painter
 *
 * AUTHOR : Andrew Haisley
 *
 *
 *
 * DESCRIPTION:
 *
 * A class to draw image areas
 *
 *
 *
 *
 *
 */

#include <mx.h>
#include <mx_device.h>
#include <mx_image_area.h>
#include <mx_img_paint.h>

extern int currentMem(char* str);

mx_image_area_painter* mx_image_area_painter::the_painter = NULL;

mx_image_area_painter* mx_image_area_painter::painter()
{
    init();
    return the_painter;
}

void mx_image_area_painter::init()
{
    if (the_painter == NULL) {
        the_painter = new mx_image_area_painter();
    }
}

void mx_image_area_painter::setForeAndBack(int& err,
    mx_colour& setColour1,
    mx_colour& setColour2)
{
    LUT_VALUE fg, bg;

    err = MX_ERROR_OK;

    device->registerColour(&setColour1, &bg);
    device->registerColour(&setColour2, &fg);

    device->setForegroundColour(err, fg);
    MX_ERROR_CHECK(err);

    device->setBackgroundColour(err, bg);
    MX_ERROR_CHECK(err);

abort:;
}

void mx_image_area_painter::registerColours(int& err,
    mx_image_area* ia,
    LUT_VALUE* lut,
    int* lutSize,
    int* ndisplayBits,
    RAST_DATA* data,
    RAST_PACK* pack,
    int* ncolours,
    mx_colour** colours,
    bool newRaster)

{
    raster* sourceRaster;
    mx_colour setColour;
    mx_colour setColour2;
    int numColours;
    int icolour;
    // int32     *sigs ;
    int greyValue;
    LUT_VALUE pixel;
    mx_colour* colourArray;
    int testColours;
    int shiftValue, nreps;
    int ioutColour;

    *lutSize = 0;
    *ndisplayBits = 0;
    *data = RDATA_COLOUR;
    *pack = RPACK_BYTE;
    *colours = NULL;
    *ncolours = 0;

    setColour.name = NULL;

    sourceRaster = ia->getSourceRaster();

    *ndisplayBits = sourceRaster->getSigbits(0);

    if (sourceRaster == NULL) {
        MX_ERROR_THROW(err, MX_ERROR_NULL_RASTER);
        return;
    }

    switch (sourceRaster->getPack()) {
    case RPACK_BIT:

        *data = RDATA_BINARY;
        *pack = RPACK_BIT;

        numColours = sourceRaster->getColours(&colourArray);

        if (numColours == 0) {
            // want to set foreground and blackground to black
            // and white
            setColour.red = setColour.green = setColour.blue = 255;
            setColour2.red = setColour2.green = setColour2.blue = 0;
        } else {
            setColour = colourArray[0];
            setColour2 = colourArray[1];
        }

        setForeAndBack(err, setColour, setColour2);
        MX_ERROR_CHECK(err);

        break;
    case RPACK_BYTE:

        // already displaying this raster
        if (!newRaster)
            return;

        switch (device->depth()) {
        case 32:
            *pack = RPACK_INT;
            break;
        case 16:
            *pack = RPACK_SHORT;
            break;
        default:
            *pack = RPACK_BYTE;
            break;
        }

        switch (sourceRaster->getData()) {
        case RDATA_GREYSCALE:

            //*lutSize = numColours = (1<<sigs[0]) ;

            // only use at most 16 colours though
            if (numColours > 16) {
                numColours = 16;
                *ndisplayBits = 4;
                // shiftValue = sigs[0] - 4 ;
            } else {
                shiftValue = 0;
            }

            // get the number or repetitions
            nreps = 1 << shiftValue;

            // Try the pixel value - do it like this
            // so all grey values get the same
            // pixel values (i.e. dont use (numColours-1)

            ioutColour = 0;
            for (icolour = 0; icolour < numColours; icolour++) {
                if (icolour == numColours - 1) {
                    greyValue = 255;
                } else {
                    greyValue = (icolour * 255 / numColours);
                }

                setColour.red = greyValue;
                setColour.green = greyValue;
                setColour.blue = greyValue;

                device->registerColour(&setColour, &pixel);

                for (int irep = 0; irep < nreps; irep++) {
                    lut[ioutColour] = pixel;
                    ioutColour++;
                }
            }

            break;

        case RDATA_COLOUR:

            numColours = sourceRaster->getColours(&colourArray);

            for (icolour = 0; icolour < numColours; icolour++) {
                // Try the pixel value

                setColour.red = colourArray[icolour].red;
                setColour.green = colourArray[icolour].green;
                setColour.blue = colourArray[icolour].blue;

                device->registerColour(&setColour, &pixel);

                lut[icolour] = pixel;
            }

            *lutSize = numColours;

            break;

        case RDATA_RGB:

            ia->defaultColours(&numColours, &colourArray);

            *colours = colourArray;
            *ncolours = numColours;

            for (icolour = 0; icolour < numColours; icolour++) {
                // Try the pixel value

                setColour.red = colourArray[icolour].red;
                setColour.green = colourArray[icolour].green;
                setColour.blue = colourArray[icolour].blue;

                device->registerColour(&setColour, &pixel);

                lut[icolour] = pixel;
            }

            *lutSize = numColours;

            *ndisplayBits = 1;
            testColours = 2;

            while (testColours < numColours) {
                testColours *= 2;
                (*ndisplayBits)++;
            }

            break;
        default:
            return;
        }
        break;
    default:
        return;
    }

    return;
abort:
    return;
}

void mx_image_area_painter::draw(int& err,
    mx_doc_rect_t& r,
    mx_paintable& o)
{
    err = MX_ERROR_OK;

    if (device->rtti_class() == mx_screen_device_class_e) {
        draw_to_screen(err, r, o);
        MX_ERROR_CHECK(err);
    } else {
        draw_to_printer(err, r, o);
        MX_ERROR_CHECK(err);
    }

    return;
abort:
    return;
}

void mx_image_area_painter::setup(int& err,
    raster* ras)
{
    int iplane;
    int32* sigs;

    err = MX_ERROR_OK;

    // set default number of colours
    numColours = 0;
    colours = NULL;

    // get the unpacked size

    unpackedBits = rasterBits(ras->getPack(), 1, 1);

    switch (ras->getData()) {
    case RDATA_BINARY:
        colourDeviceType = MX_DEVICE_BIT;
        numColours = ras->getNumColours();
        colours = ras->getColours();
        break;
    case RDATA_RGB:
        colourDeviceType = MX_DEVICE_RGB;
        break;
    case RDATA_GREYSCALE:
        colourDeviceType = MX_DEVICE_GREY;
        break;
    case RDATA_COLOUR:
        colourDeviceType = MX_DEVICE_LUT;
        numColours = ras->getNumColours();
        colours = ras->getColours();
        break;
    }

    numPlanes = ras->getPlanes(&sigs);
    maxSigbits = 1;

    // get the maximum number of significant bits
    for (iplane = 0; iplane < numPlanes; iplane++) {
        if (sigs[iplane] > maxSigbits)
            maxSigbits = sigs[iplane];
    }
}

void mx_image_area_painter::draw_to_printer(int& err,
    mx_doc_rect_t& r,
    mx_paintable& o)
{
    mx_image_area* ia = (mx_image_area*)&o;
    raster* printRaster;
    mx_ipoint pixelSize;
    mx_doc_rect_t ir;
    mx_point topLeft, totalSize;
    mx_irect pixelRect;
    mx_point scale;
    mx_point resolution;
    mx_ipoint printSize;
    mx_point realSize;

    err = MX_ERROR_OK;

    ia->crop_details(err, topLeft, totalSize);
    MX_ERROR_CHECK(err);

    // first try to open the full raster
    printRaster = ia->createPrintRaster(err,
        device->frameResolution());
    MX_ERROR_CHECK(err);

    pixelSize = printRaster->getSize();

    /* get the actual resolution of the pixels */
    resolution.x = pixelSize.x / totalSize.x;
    resolution.y = pixelSize.y / totalSize.y;

    // get the pixels to draw
    drawPixels(err, printRaster, r, resolution, topLeft, pixelRect);
    MX_ERROR_CHECK(err);

    // if not in range do nothing

    if ((pixelRect.xt < 0) || (pixelRect.yt < 0) || (pixelRect.xb >= pixelSize.x) || (pixelRect.yb >= pixelSize.y)) {
        ia->deletePrintRaster();
        return;
    }

    setup(err, printRaster);
    MX_ERROR_CHECK(err);

    /* determine how many pixels to draw */
    printSize.x = pixelRect.xt - pixelRect.xb + 1;
    printSize.y = pixelRect.yt - pixelRect.yb + 1;

    /* and how big a size they cover */
    realSize.x = printSize.x / resolution.x;
    realSize.y = printSize.y / resolution.y;

    device->outputImageHeader(err,
        topLeft,
        realSize,
        colourDeviceType,
        maxSigbits,
        numPlanes,
        numColours,
        colours,
        printSize,
        FALSE);
    MX_ERROR_CHECK(err);

    drawTiles(err, printRaster, ir, 1, topLeft, pixelRect);
    MX_ERROR_CHECK(err);

    device->outputImageTrailer(err,
        FALSE);
    MX_ERROR_CHECK(err);

    ia->deletePrintRaster();
abort:
    return;
}

void mx_image_area_painter::draw_to_screen(int& err,
    mx_doc_rect_t& r,
    mx_paintable& o)
{
    mx_image_area* ia = (mx_image_area*)&o;
    mx_doc_coord_t p1, p2;
    mx_line_style ls(mx_solid, 0.0, "black");
    raster* displayRaster;
    LUT_VALUE lut[256];
    int lutSize;
    int ndisplayBits;
    RAST_PACK pack;
    RAST_DATA data;
    int ncolours;
    mx_colour* colours;
    double frameResolution;
    mx_point totalSize;
    bool newRaster;
    mx_ipoint newRasterSize;
    mx_point topLeft;
    mx_irect pixelRect;
    mx_point resolution;

    // frame resolution is the number of pixels per mm
    // in the frame coordinates
    frameResolution = device->frameResolution();

    ia->crop_details(err, topLeft, totalSize);
    MX_ERROR_CHECK(err);

    newRaster = ia->newDisplayRaster(err, frameResolution,
        totalSize, newRasterSize);
    MX_ERROR_CHECK(err);

    // register any colours needed with the device
    registerColours(err, ia, lut, &lutSize, &ndisplayBits, &data, &pack,
        &ncolours, &colours, newRaster);
    MX_ERROR_CHECK(err);

    if (newRaster) {
        displayRaster = ia->createDisplayRaster(err,
            frameResolution,
            lut,
            lutSize,
            ndisplayBits, data, pack,
            ncolours, colours,
            newRasterSize);
        MX_ERROR_CHECK(err);
    } else {
        displayRaster = ia->getDisplayRaster();
    }

    setup(err, displayRaster);
    MX_ERROR_CHECK(err);

    resolution.x = resolution.y = frameResolution;

    drawPixels(err, displayRaster, r, resolution, topLeft, pixelRect);
    MX_ERROR_CHECK(err);

    drawTiles(err, displayRaster, r, frameResolution, topLeft, pixelRect);
    MX_ERROR_CHECK(err);

    // if we have set foreground and background we may now need to
    // set the values back again
    {
        mx_colour white(255, 255, 255, NULL);
        mx_colour black(0, 0, 0, NULL);

        setForeAndBack(err, white, black);
        MX_ERROR_CHECK(err);
    }

abort:
    return;
}

void mx_image_area_painter::drawPixels(int& err,
    raster* iraster,
    mx_doc_rect_t& ir,
    mx_point& displayResolution,
    mx_point& topLeft,
    mx_irect& pixelRect)
{
    mx_ipoint rasterSize = iraster->getSize();

    err = MX_ERROR_OK;

    // the area to be drawn must be shifted by the
    // crop offset - this is the coordinate of the top
    // left hand corner

    pixelRect.xt = (int)((ir.r.xt - topLeft.x) * displayResolution.x + 0.5);
    pixelRect.xb = (int)((ir.r.xb - topLeft.x) * displayResolution.x + 0.5);
    pixelRect.yt = (int)((ir.r.yt - topLeft.y) * displayResolution.y + 0.5);
    pixelRect.yb = (int)((ir.r.yb - topLeft.y) * displayResolution.y + 0.5);

    if (pixelRect.yb < 0)
        pixelRect.yb = 0;
    if (pixelRect.yt > rasterSize.y)
        pixelRect.yt = rasterSize.y;
    if (pixelRect.xb < 0)
        pixelRect.xb = 0;
    if (pixelRect.xt > rasterSize.x)
        pixelRect.xt = rasterSize.x;

    (pixelRect.xt)--;
    (pixelRect.yt)--;

    return;
}

void mx_image_area_painter::drawTiles(int& err,
    raster* iraster,
    mx_doc_rect_t& ir,
    double displayResolution,
    mx_point& topLeft,
    mx_irect& pixelRect)
{
    rasterIterator rit(TRUE);
    mx_ipoint tileCoord, outputTileSize;
    mx_ipoint tileSize = iraster->getTileSize();
    mx_doc_coord_t tilePos;
    mx_irect currentPixels;
    void* buffer;
    mx_doc_coord_t topLeftCoord;
    mx_irect* clipRect;
    mx_irect iclipRect;
    mx_irect offset;

    rit.set(err, iraster, pixelRect);
    MX_ERROR_CHECK(err);

    while ((buffer = rit.next(err, currentPixels, tileCoord))) {
        topLeftCoord.p.x = currentPixels.xb / displayResolution;
        topLeftCoord.p.y = currentPixels.yb / displayResolution;

        // the top left coordinates must be moved back
        // the coordinate of the top of the first tile say
        // is -topLeft relative to the area

        topLeftCoord.p += topLeft;

        offset.xt = currentPixels.xt - pixelRect.xt;
        offset.yt = currentPixels.yt - pixelRect.yt;
        offset.xb = pixelRect.xb - currentPixels.xb;
        offset.yb = pixelRect.yb - currentPixels.yb;

        if ((offset.xt > 0) || (offset.yt > 0) || (offset.xb > 0) || (offset.yb > 0)) {
            iclipRect.xt = tileSize.x - GMAX(offset.xt, 0) - 1;
            iclipRect.yt = tileSize.y - GMAX(offset.yt, 0) - 1;
            iclipRect.xb = GMAX(0, offset.xb);
            iclipRect.yb = GMAX(0, offset.yb);

            topLeftCoord.p.x += iclipRect.xb / displayResolution;
            topLeftCoord.p.y += iclipRect.yb / displayResolution;

            clipRect = &iclipRect;
        } else {
            clipRect = NULL;
        }

        device->drawTile(err,
            colourDeviceType,
            maxSigbits,
            unpackedBits,
            tileSize,
            topLeftCoord,
            buffer,
            clipRect);
    }
    MX_ERROR_CHECK(err);

    return;
abort:
    return;
}

mx_image_area_painter::mx_image_area_painter()
    : mx_area_painter()
{
}

mx_image_area_painter::~mx_image_area_painter()
{
}
