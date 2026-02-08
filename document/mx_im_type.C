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
 * MODULE/CLASS : mx_im_type
 *
 * AUTHOR : Andrew Haisley
 *
 * The author should insert the name of whoever should own the
 * copyright (e.g. Tangent Data, Dave Miller, Andy Toone,
 * Ltd etc etc).
 *
 * This file
 *
 * DESCRIPTION:
 *
 */

#include <mx_image_area.h>
#include <mxraster.h>
#include <rasdib.h>
#include <rasjpeg.h>
#include <raspng.h>
#include <rastiff.h>
#include <rasxbm.h>
#include <traster.h>

/*-------------------------------------------------
 * FUNCTION: mx_image_area::defaultColours
 *
 * DESCRIPTION: Default colours for dithering
 *
 *
 */

void mx_image_area::defaultColours(int* ndefColours,
    mx_colour** defColours)
{
    static int defCol[27][3] = {
        { 0, 0, 0 },
        { 0, 0, 127 },
        { 0, 0, 255 },
        { 0, 127, 0 },
        { 0, 127, 127 },
        { 0, 127, 255 },
        { 0, 255, 0 },
        { 0, 255, 127 },
        { 0, 255, 255 },
        { 127, 0, 0 },
        { 127, 0, 127 },
        { 127, 0, 255 },
        { 127, 127, 0 },
        { 127, 127, 127 },
        { 127, 127, 255 },
        { 127, 255, 0 },
        { 127, 255, 127 },
        { 127, 255, 255 },
        { 255, 0, 0 },
        { 255, 0, 127 },
        { 255, 0, 255 },
        { 255, 127, 0 },
        { 255, 127, 127 },
        { 255, 127, 255 },
        { 255, 255, 0 },
        { 255, 255, 127 },
        { 255, 255, 255 }
    };
    static mx_colour defColC[27];

    for (int i = 0; i < 27; i++) {
        defColC[i].red = defCol[i][0];
        defColC[i].green = defCol[i][1];
        defColC[i].blue = defCol[i][2];
        defColC[i].name = NULL;
    }

    *ndefColours = sizeof(defColC) / sizeof(mx_colour);
    *defColours = defColC;
    return;
}

/*-------------------------------------------------
 * FUNCTION: mx_image_area::init
 *
 * DESCRIPTION:
 *
 *
 */

void mx_image_area::init()
{
    type = MX_IMAGE_AREA_UNDEFINED;
    inputRaster = NULL;
    subsampledRaster = NULL;
    fullImportRaster = NULL;
    displayRaster = NULL;
    printRaster = NULL;
    filename = NULL;
    subsampledExists = FALSE;
    fullImportExists = FALSE;
    lastRasterSize.x = -1;
    lastRasterSize.y = -1;
    numColours = 0;
}

/*-------------------------------------------------
 * FUNCTION: mx_image_area::createBasics
 *
 * DESCRIPTION:
 *
 *
 */

void mx_image_area::createBasics(int& err,
    char* ifilename,
    mx_image_area_t itype,
    bool icreateSubsampled,
    bool icreateFullImport,
    int inumColours)
{
    // Just create the filename as a link

    if (ifilename != NULL) {
        filename = new char[strlen(ifilename) + 1];
        strcpy(filename, ifilename);
    } else {
        filename = new char[1];
        filename[0] = 0;
    }

    type = itype;
    subsampledExists = icreateSubsampled;
    fullImportExists = icreateFullImport;
    numColours = inumColours;

    // set the type in the database and the area
    set_int(err, "imageType", type);
    MX_ERROR_CHECK(err);

    set_string(err, "imageFilename", filename);
    MX_ERROR_CHECK(err);

    set_int(err, "imageSubsampled", subsampledExists);
    MX_ERROR_CHECK(err);

    set_int(err, "fullImport", fullImportExists);
    MX_ERROR_CHECK(err);

    set_int(err, "numColours", numColours);
    MX_ERROR_CHECK(err);

    return;
abort:
    return;
}

/*-------------------------------------------------
 * FUNCTION: mx_image_area::readBasics
 *
 * DESCRIPTION:
 *
 *
 */

void mx_image_area::readBasics(int& err)
{
    const char* ifilename;
    int filenameLen;

    // set the type in the database and the area
    type = (mx_image_area_t)get_int(err, "imageType");
    MX_ERROR_CHECK(err);

    ifilename = get_string(err, "imageFilename");
    MX_ERROR_CHECK(err);

    if (filename != NULL)
        delete[] filename;

    if (ifilename != NULL) {
        filenameLen = strlen(ifilename) + 1;

        filename = new char[filenameLen];

        strcpy(filename, ifilename);
    } else {
        filename = NULL;
    }

    subsampledExists = get_int(err, "imageSubsampled");
    MX_ERROR_CHECK(err);

    fullImportExists = get_int(err, "fullImport");
    MX_ERROR_CHECK(err);

    numColours = get_int(err, "numColours");
    MX_ERROR_CHECK(err);

    return;
abort:
    return;
}

/*-------------------------------------------------
 * FUNCTION: mx_image_area::createCopy
 *
 * DESCRIPTION: Create a stored copy of the raster
 *
 *
 */

void mx_image_area::createCopy(int& err)
{
    COMP_TYPE compType;

    err = MX_ERROR_OK;

    if (inputRaster == NULL) {
        MX_ERROR_THROW(err, MX_ERROR_NULL_RASTER);
    }

    compType = defaultCompType(inputRaster->getPack(),
        inputRaster->getData(),
        inputRaster->getPlanes());
    fullImportRaster = new mxraster(err, *this, MX_RASTER_FULL, *inputRaster,
        compType, RCACHE_DEFAULT);
    MX_ERROR_CHECK(err);

    fullImportExists = TRUE;

    set_int(err, "fullImport", fullImportExists);
    MX_ERROR_CHECK(err);

    return;
abort:
    return;
}

/*-------------------------------------------------
 * FUNCTION: createPackRaster
 *
 * DESCRIPTION: Create the subsampled view
 *
 *
 */

static raster* createPackRaster(int& err,
    raster* inputRaster)
{
    // must create a new viewable raster from the input raster
    raster* unpackRaster = nullptr;
    traster* tunpackRaster;

    int inputBits = inputRaster->pixelBitSize();

    err = MX_ERROR_OK;

    if (inputBits == 1) {
        // If the input is binary need to unpack to scale

        tunpackRaster = new traster(err, inputRaster, FALSE);
        MX_ERROR_CHECK(err);

        tunpackRaster->setPack(err, RPACK_BYTE);
        MX_ERROR_CHECK(err);

        unpackRaster = tunpackRaster;
    } else {
        unpackRaster = inputRaster;
    }

    return unpackRaster;
abort:
    return unpackRaster;
}

/*-------------------------------------------------
 * FUNCTION: mx_image_area::createSubsampled
 *
 * DESCRIPTION: Create the subsampled view
 *
 *
 */

void mx_image_area::createSubsampled(int& err,
    mx_point& areaSize,
    mx_ipoint& pixelAreaSize)
{
    traster* scaledRaster = NULL;
    raster* unpackRaster;
    int inputBits, nditherColours, inputPlanes;
    int32_t nsigbits;
    mx_colour* ditherColours;
    RAST_PACK inputPack;
    RAST_DATA inputData;
    int32_t* sigbits;
    mx_ipoint scaledTileSize, scaledTotalSize;
    COMP_TYPE compType;
    int rowByteSize, outputBits;
    mx_ipoint tileAreaSize;

    err = MX_ERROR_OK;

    if (inputRaster == NULL) {
        MX_ERROR_THROW(err, MX_ERROR_NULL_RASTER);
    }

    inputBits = inputRaster->pixelBitSize();

    unpackRaster = createPackRaster(err, inputRaster);
    MX_ERROR_CHECK(err);

    // create the scaled raster
    scaledRaster = new traster(err, unpackRaster, (unpackRaster != inputRaster));
    MX_ERROR_CHECK(err);

    // Set the scale transform
    scaledRaster->setSize(err, pixelAreaSize);
    MX_ERROR_CHECK(err);

    // set the tilesize to something sensible
    // as this raster is subsampled for display
    // set the number of tiles across to be one
    // and the down size to create approx 16K tiles

    if (inputBits == 1) {
        // Round x size to multiple of 8
        tileAreaSize.x = (pixelAreaSize.x + 7) / 8;
        tileAreaSize.x *= 8;
        outputBits = 1;
    } else {
        tileAreaSize.x = pixelAreaSize.x;
        outputBits = 8;
    }

    rowByteSize = (tileAreaSize.x * outputBits + 7) / 8;
    tileAreaSize.y = 16384 / rowByteSize;
    if (tileAreaSize.y > pixelAreaSize.y)
        tileAreaSize.y = pixelAreaSize.y;
    if (tileAreaSize.y == 0)
        tileAreaSize.y = 1;

    scaledRaster->setTileSize(err, tileAreaSize);
    MX_ERROR_CHECK(err);

    // Here we have either a byte or RGB raster of the correct size

    inputBits = inputRaster->pixelBitSize();
    inputData = inputRaster->getData();
    inputPack = inputRaster->getPack();
    inputPlanes = inputRaster->getPlanes(&sigbits);

    if (inputBits == 1) {
        // Need to convert pack into bit data after
        // transform

        scaledRaster->setPack(err, RPACK_BIT);
        MX_ERROR_CHECK(err);
    } else if ((inputBits == 8) && (inputData == RDATA_GREYSCALE)) {
        // Greyscale - may be too many bits - set up shift

        if (sigbits[0] > 4) {
            nsigbits = 4;
            scaledRaster->setSigbits(err, inputPlanes, &nsigbits);
            MX_ERROR_CHECK(err);
        }
    } else if ((inputBits == 24) && (inputData == RDATA_RGB)) {
        // 24 bit - need to dither

        scaledTileSize = scaledRaster->getTileSize();
        scaledTotalSize = scaledRaster->getTotalSize();

        scaledTileSize.x = scaledTotalSize.x;

        defaultColours(&nditherColours, &ditherColours);

        scaledRaster->setTileSize(err, scaledTileSize);
        MX_ERROR_CHECK(err);

        scaledRaster->setColours(err, nditherColours, ditherColours);
        MX_ERROR_CHECK(err);

        scaledRaster->setData(err, RDATA_COLOUR);
        MX_ERROR_CHECK(err);

        nsigbits = 8;
        scaledRaster->setSigbits(err, 1, &nsigbits);
        MX_ERROR_CHECK(err);
    }

    compType = defaultCompType(inputPack, inputData, inputPlanes);

    subsampledRaster = new mxraster(err, *this, MX_RASTER_SUBSAMPLED, *scaledRaster,
        compType, RCACHE_DEFAULT);
    MX_ERROR_CHECK(err);

    delete scaledRaster;

    subsampledExists = TRUE;

    set_int(err, "imageSubsampled", subsampledExists);
    MX_ERROR_CHECK(err);

    // now we need to set the size explicitly
    if (inputRaster->getHasStoredMMsize()) {
        subsampledRaster->setSize(err, areaSize);
        MX_ERROR_CHECK(err);
    }

    return;
abort:
    delete scaledRaster;
    return;
}

/*-------------------------------------------------
 * FUNCTION: mx_image_area::createTIFFArea
 *
 * DESCRIPTION: New image area
 *
 *
 */

void mx_image_area::createTIFFarea(int& err,
    char* TIFFfile)
{
    err = MX_ERROR_OK;

    // Create the TIFF file raster
    inputRaster = new rastiff(err, TIFFfile, RCACHE_DICT);
    MX_ERROR_CHECK(err);

    // Set basic properties of image area

    createBasics(err, TIFFfile, MX_IMAGE_AREA_TIFF, FALSE, FALSE,
        inputRaster->getNumColours());
    MX_ERROR_CHECK(err);

    goto exit;
abort:
exit:
    return;
}

/*-------------------------------------------------
 * FUNCTION: mx_image_area::createMetafileArea
 *
 * DESCRIPTION: New image area
 *
 *
 */

void mx_image_area::createMETAFILEarea(int& err,
    mx_mxmetafile_output& metafileData)
{
    err = MX_ERROR_OK;

    inputRaster = new rasdib(err, metafileData);
    MX_ERROR_CHECK(err);

    // Set basic properties of image area

    createBasics(err, NULL, MX_IMAGE_AREA_METAFILE, FALSE, TRUE,
        inputRaster->getNumColours());
    MX_ERROR_CHECK(err);

    goto exit;
abort:
exit:
    return;
}

/*-------------------------------------------------
 * FUNCTION: mx_image_area::createJPEGarea
 *
 * DESCRIPTION: New image area
 *
 *
 */

void mx_image_area::createJPEGarea(int& err,
    char* jpegfile,
    bool get24bits,
    int ncolours)
{
    err = MX_ERROR_OK;

    // Create the TIFF file raster
    inputRaster = new rasjpeg(err, jpegfile, get24bits, ncolours, RCACHE_DICT);
    MX_ERROR_CHECK(err);

    // Set basic properties of image area

    createBasics(err, jpegfile, MX_IMAGE_AREA_JPEG, FALSE, FALSE,
        inputRaster->getNumColours());
    MX_ERROR_CHECK(err);

    goto exit;
abort:
exit:
    return;
}

/*-------------------------------------------------
 * FUNCTION: mx_image_area::createPNGarea
 *
 * DESCRIPTION: New image area
 *
 *
 */

void mx_image_area::createPNGarea(int& err,
    char* pngfile)
{
    err = MX_ERROR_OK;

    // Create the TIFF file raster
    inputRaster = new raspng(err, pngfile, RCACHE_DICT);
    MX_ERROR_CHECK(err);

    // Set basic properties of image area

    createBasics(err, pngfile, MX_IMAGE_AREA_PNG, FALSE, FALSE,
        inputRaster->getNumColours());
    MX_ERROR_CHECK(err);

    goto exit;
abort:
exit:
    return;
}

/*-------------------------------------------------
 * FUNCTION: mx_image_area::createXBMarea
 *
 * DESCRIPTION: New image area
 *
 *
 */

void mx_image_area::createXBMarea(int& err,
    char* XBMfile)
{
    err = MX_ERROR_OK;

    // Create the TIFF file raster
    inputRaster = new rasxbm(err, XBMfile, TRUE, RCACHE_DICT);
    MX_ERROR_CHECK(err);

    // Set basic properties of image area

    createBasics(err, XBMfile, MX_IMAGE_AREA_XBM, FALSE, FALSE,
        inputRaster->getNumColours());
    MX_ERROR_CHECK(err);

    goto exit;
abort:
exit:
    return;
}

/*-------------------------------------------------
 * FUNCTION: mx_image_area::createXPMarea
 *
 * DESCRIPTION: New image area
 *
 *
 */

void mx_image_area::createXPMarea(int& err,
    char* XPMfile)
{
    err = MX_ERROR_OK;

    // Create the TIFF file raster
    inputRaster = new rasxbm(err, XPMfile, FALSE, RCACHE_DICT);
    MX_ERROR_CHECK(err);

    // Set basic properties of image area

    createBasics(err, XPMfile, MX_IMAGE_AREA_XPM, FALSE, FALSE,
        inputRaster->getNumColours());
    MX_ERROR_CHECK(err);

    goto exit;
abort:
exit:
    return;
}

/*-------------------------------------------------
 * FUNCTION: mx_image_area::inputRasterSize
 *
 * DESCRIPTION: Calculate the size of the raster
 * in an image
 *
 */

mx_point mx_image_area::areaSize(int& err,
    mx_point& pageSize,
    double pixelResolution)
{
    mx_point retArea;

    if (inputRaster == NULL) {
        MX_ERROR_THROW(err, MX_ERROR_NULL_RASTER);
    }

    retArea = inputRaster->areaSize(err, pageSize, pixelResolution);
    MX_ERROR_CHECK(err);

    return retArea;
abort:
    return retArea;
}

/*-------------------------------------------------
 * FUNCTION: mx_image_area::createStoredRaster
 *
 * DESCRIPTION: New image area
 *
 *
 */

void mx_image_area::createStoredRaster(int& err,
    mx_point& areaSize,
    double pixelResolution)
{
    mx_ipoint areaPixelSize;
    mx_ipoint rasterPixelSize;

    err = MX_ERROR_OK;

    if (inputRaster == NULL) {
        MX_ERROR_THROW(err, MX_ERROR_NULL_RASTER);
    }

    // see if we want to create a subsampled raster or
    // a copy raster

    // get the size of the raster in pixels

    rasterPixelSize = inputRaster->getSize();

    // if the pixel resolution is negative we are going to store the
    // whole raster anyway

    areaPixelSize.x = (int)(areaSize.x * pixelResolution + 0.5);
    areaPixelSize.y = (int)(areaSize.y * pixelResolution + 0.5);

    if ((pixelResolution > 0) && ((areaPixelSize.x < rasterPixelSize.x) || (areaPixelSize.y < rasterPixelSize.y))) {
        // need to subsample

        createSubsampled(err, areaSize, areaPixelSize);
        MX_ERROR_CHECK(err);
    } else {
        createCopy(err);
        MX_ERROR_CHECK(err);
    }

    return;
abort:
    return;
}

/*-------------------------------------------------
 * FUNCTION: mx_image_area::deleteInputRaster
 *
 * DESCRIPTION: New image area
 *
 *
 */

void mx_image_area::deleteInputRaster()
{
    delete inputRaster;
    inputRaster = NULL;
}

/*-------------------------------------------------
 * FUNCTION: mx_image_area::deleteDisplayRaster
 *
 * DESCRIPTION: New image area
 *
 *
 */

void mx_image_area::deleteDisplayRaster()
{
    if ((displayRaster != subsampledRaster) && (displayRaster != fullImportRaster))
        delete displayRaster;
    displayRaster = NULL;
}

/*-------------------------------------------------
 * FUNCTION: mx_image_area::deletePrintRaster
 *
 * DESCRIPTION: New image area
 *
 *
 */

void mx_image_area::deletePrintRaster()
{
    if ((printRaster != subsampledRaster) && (printRaster != fullImportRaster))
        delete printRaster;
    printRaster = NULL;
}

/*-------------------------------------------------
 * FUNCTION: mx_image_area::getSourceRaster
 *
 * DESCRIPTION: New image area
 *
 *
 */

raster* mx_image_area::getSourceRaster()
{
    raster* sourceRaster;

    sourceRaster = getSubsampledRaster();

    if (sourceRaster == NULL)
        sourceRaster = getFullImportRaster();

    return sourceRaster;
}

/*-------------------------------------------------
 * FUNCTION: mx_image_area::newDisplayRaster
 *
 * DESCRIPTION: New image area
 *
 *
 */

bool mx_image_area::newDisplayRaster(int& err,
    double displayResolution,
    mx_point& totalSize,
    mx_ipoint& newRasterSize)
{
    mx_point imageScale;

    err = MX_ERROR_OK;

    newRasterSize.x = (int)(totalSize.x * displayResolution + 0.5);
    newRasterSize.y = (int)(totalSize.y * displayResolution + 0.5);

    if (displayRaster == NULL)
        return TRUE;

    return !((lastRasterSize.x == newRasterSize.x) && (lastRasterSize.y == newRasterSize.y));
    return FALSE;
}

/*-------------------------------------------------
 * FUNCTION: mx_image_area::createDisplayRaster
 *
 * DESCRIPTION: New image area
 *
 *
 */

raster* mx_image_area::createDisplayRaster(int& err,
    double displayResolution,
    LUT_VALUE* lut,
    int lutSize,
    int32_t ndisplayBits,
    RAST_DATA data,
    RAST_PACK pack,
    int ncolours,
    mx_colour* colours,
    mx_ipoint& newRasterSize)
{
    raster* sourceRaster;
    mx_ipoint rasterSize;
    raster* outputRaster;
    traster* transRaster;
    bool doScaleChange;
    bool doPackChange;
    bool doDataChange;
    bool doLUTchange;
    bool doSigbitsChange;
    bool doColourChange;
    bool doPixelChange;

    err = MX_ERROR_OK;

    // use either the subsampled Raster
    // or the full import raster
    // whatever gives the best match

    if (subsampledRaster == NULL) {
        sourceRaster = fullImportRaster;
    } else {
        sourceRaster = subsampledRaster;
    }

    rasterSize = sourceRaster->getSize();

    if (displayRaster != NULL)
        deleteDisplayRaster();
    setLastRasterSize(newRasterSize);

    doScaleChange = (rasterSize != newRasterSize);
    doPackChange = (sourceRaster->getPack() != pack);
    doDataChange = (sourceRaster->getData() != data);
    doLUTchange = (lutSize != 0);
    doSigbitsChange = (sourceRaster->getSigbits(0) != ndisplayBits);
    doColourChange = (colours != NULL);

    doPixelChange = (doPackChange || doDataChange || doLUTchange || doSigbitsChange || doColourChange);

    if ((!doScaleChange) && (!doPixelChange)) {
        // no change
        outputRaster = sourceRaster;
    } else {
        /* scale the raster for display on the screen */

        raster* unpackRaster;
        bool doPackingChange;
        raster* dataChangeRaster;
        traster* tdataChangeRaster;

        // we may need to unpack from binary to byte
        // for resampling

        if (doScaleChange) {
            unpackRaster = createPackRaster(err, sourceRaster);
            MX_ERROR_CHECK(err);
        } else {
            unpackRaster = sourceRaster;
        }

        // have we created an unpack raster
        doPackingChange = (sourceRaster != unpackRaster);

        // do data change first - this is a performance
        // optimisation when zoomed in

        if (doPixelChange) {
            tdataChangeRaster = new traster(err, unpackRaster,
                doPackingChange);
            MX_ERROR_CHECK(err);

            if (doDataChange) {
                tdataChangeRaster->setData(err, data);
                MX_ERROR_CHECK(err);
            }

            if (doPackChange) {
                tdataChangeRaster->setPack(err, pack);
                MX_ERROR_CHECK(err);
            }

            if (doSigbitsChange) {
                tdataChangeRaster->setSigbits(err, 1, &ndisplayBits);
                MX_ERROR_CHECK(err);
            }

            if (doLUTchange) {
                tdataChangeRaster->setLut(err, lut, lutSize);
                MX_ERROR_CHECK(err);
            }

            if (doColourChange) {
                tdataChangeRaster->setColours(err, ncolours, colours);
                MX_ERROR_CHECK(err);
            }

            dataChangeRaster = tdataChangeRaster;
        } else {
            dataChangeRaster = unpackRaster;
        }

        if (doScaleChange) {
            transRaster = new traster(err, dataChangeRaster,
                (sourceRaster != dataChangeRaster));
            MX_ERROR_CHECK(err);

            transRaster->setSize(err, newRasterSize);
            MX_ERROR_CHECK(err);

            // set pack to bit after rescaling
            if (doPackingChange) {
                transRaster->setPack(err, RPACK_BIT);
                MX_ERROR_CHECK(err);
            }

            outputRaster = transRaster;
        } else {
            outputRaster = dataChangeRaster;
        }
    }

    displayRaster = outputRaster;

    return outputRaster;

abort:
    return NULL;
}

/*-------------------------------------------------
 * FUNCTION: mx_image_area::getBestRaster
 *
 * DESCRIPTION: New image area
 *
 *
 */

raster* mx_image_area::getBestRaster(int& err,
    bool& needsDeleting)
{
    raster* bestRaster = NULL;

    needsDeleting = TRUE;

    if (fullImportRaster != NULL) {
        bestRaster = fullImportRaster;
        needsDeleting = FALSE;
    } else {
        // try to open the full original file if its still there

        switch (type) {
        case MX_IMAGE_AREA_TIFF:
            // Create the TIFF file raster
            bestRaster = new rastiff(err, filename, RCACHE_DICT);
            break;
        case MX_IMAGE_AREA_JPEG:
            if (numColours == 0) {
                bestRaster = new rasjpeg(err, filename, TRUE, 0, RCACHE_DICT);
            } else {
                bestRaster = new rasjpeg(err, filename, FALSE, numColours, RCACHE_DICT);
            }
            break;
        case MX_IMAGE_AREA_PNG:
            bestRaster = new raspng(err, filename, RCACHE_DICT);
            break;
        case MX_IMAGE_AREA_XBM:
            bestRaster = new rasxbm(err, filename, TRUE, RCACHE_DICT);
            break;
        case MX_IMAGE_AREA_XPM:
            bestRaster = new rasxbm(err, filename, FALSE, RCACHE_DICT);
            break;
        default:
            return NULL;
        }

        // if an error has occurred reading the file - use the
        // mxraster
        if (err != MX_ERROR_OK) {
            MX_ERROR_CLEAR(err);
            delete bestRaster;
            bestRaster = NULL;
        }

        // we will delete the original file raster when the
        // print raster is deleted - we do keep the print
        // raster around

        needsDeleting = (bestRaster != NULL);

        // original file has gone - use the stored raster
        if (!needsDeleting) {
            bestRaster = subsampledRaster;
        }
    }

    return bestRaster;
}

/*-------------------------------------------------
 * FUNCTION: mx_image_area::createPrintRaster
 *
 * DESCRIPTION: New image area
 *
 *
 */

raster* mx_image_area::createPrintRaster(int& err,
    double printResolution)
{
    mx_point realSize;
    mx_ipoint printPixelSize;
    mx_ipoint rasterSize;
    mx_ipoint tileSize;
    raster* outputRaster;
    traster* transRaster;
    bool doScaleChange;
    bool doTileSizeChange;
    raster* sourceRaster = NULL;
    bool deleteOriginal;
    mx_ipoint scaleSize;
    int byteWidth, nplanes;
    mx_ipoint sourceTileSize;

    err = MX_ERROR_OK;

    deletePrintRaster();

    sourceRaster = getBestRaster(err, deleteOriginal);
    MX_ERROR_CHECK(err);

    realSize.x = get_width(err);
    MX_ERROR_CHECK(err);

    realSize.y = get_height(err);
    MX_ERROR_CHECK(err);

    printPixelSize.x = (int)((realSize.x * printResolution) + 0.5);
    printPixelSize.y = (int)((realSize.y * printResolution) + 0.5);

    rasterSize = sourceRaster->getSize();

    // if the number of rasters in the raster is more than
    // twice the number of pixels in the display device we will
    // not send the whole raster to the display device - only
    // a subsampled image  allowing two input pixels for each
    // print pixel - giving the device so leeway to rend the
    // image in the best way

    doScaleChange = ((rasterSize.x > 2 * printPixelSize.x) || (rasterSize.y > 2 * printPixelSize.y));
    if (doScaleChange) {
        scaleSize.x = 2 * printPixelSize.x;
        scaleSize.y = 2 * printPixelSize.y;
    } else {
        scaleSize = rasterSize;
    }

    nplanes = sourceRaster->getPlanes();

    // get the byte size of a full scan line
    byteWidth = rasterBytes(sourceRaster->getPack(),
        scaleSize.x,
        nplanes);

    // calculate the number of pixels across the tile

    if (sourceRaster->pixelBitSize() == 1) {
        tileSize.x = byteWidth * 8;
    } else {
        tileSize.x = scaleSize.x;
    }

    sourceTileSize = sourceRaster->getTileSize();

    // output around 16384 bytes per tile
    tileSize.y = 16384 / byteWidth;
    if (tileSize.y == 0) {
        tileSize.y = 1;
    } else if (tileSize.y > sourceTileSize.y) {
        tileSize.y = sourceTileSize.y;
    } else if (tileSize.y > rasterSize.y) {
        tileSize.y = rasterSize.y;
    }

    doTileSizeChange = (tileSize != sourceTileSize);

    if ((!doScaleChange) && (!doTileSizeChange)) {
        // no change
        outputRaster = sourceRaster;
    } else {
        transRaster = new traster(err, sourceRaster, deleteOriginal);
        MX_ERROR_CHECK(err);

        if (doScaleChange) {
            transRaster->setSize(err, scaleSize);
            MX_ERROR_CHECK(err);
        }

        if (doTileSizeChange) {
            transRaster->setTileSize(err, tileSize);
            MX_ERROR_CHECK(err);
        }

        outputRaster = transRaster;
    }

    printRaster = outputRaster;

    return printRaster;

abort:
    return NULL;
}
