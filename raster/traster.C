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
 * MODULE : traster.C
 *
 * AUTHOR : David Miller
 *
 * This file
 *
 * DESCRIPTION: Transformed rasters
 * Module traster.C
 *
 *
 *
 *
 * $id$
 * $log$
 *
 */

#include "traster.h"

#define ABSD(x) ((x) > 0 ? (x) : -(x))

/*-------------------------------------------------
 * FUNCTION: ditherDist
 *
 * DESCRIPTION:
 *
 *
 */

inline int ditherDist(mx_colour* colour1,
    mx_colour* colour2)
{
    // Use L1 norm
    return ABSD(colour1->red - colour2->red) + ABSD(colour1->green - colour2->green) + ABSD(colour1->blue - colour2->blue);
}

/*-------------------------------------------------
 * FUNCTION: mx_rdither_data::mx_rdither_data
 *
 * DESCRIPTION: Set up memory for dithering
 *
 *
 */

mx_rdither_data::mx_rdither_data(int rowSize,
    int itoggle,
    bool iuniform27,
    int numColours,
    LUT_VALUE* ilut)
{
    int numInts = 3 * (rowSize + 2);

    thisError = new int[numInts];
    memset(thisError, 0, numInts * sizeof(int));

    nextError = new int[numInts];
    memset(nextError, 0, numInts * sizeof(int));

    toggle = itoggle;
    uniform27 = iuniform27;
    lut = ilut;
    colourData = new int[numColours];
    memset(colourData, 0, numColours * sizeof(int));
}

/*-------------------------------------------------
 * FUNCTION: mx_rdither_data::~mx_rdither_data
 *
 * DESCRIPTION: Set up memory for dithering
 *
 *
 */

mx_rdither_data::~mx_rdither_data()
{
    delete[] thisError;
    delete[] nextError;
    delete[] colourData;
}

/*-------------------------------------------------
 * FUNCTION: setupDither
 *
 * DESCRIPTION: Set up memory for dithering
 *
 *
 */

static mx_rdither_data* setupDither(int& err,
    int rowSize,
    int ncolours,
    mx_colour* colours,
    bool uniform27,
    LUT_VALUE* ilut)
{
    int icol1, icol2;
    int minDist, dist;
    mx_colour* testColour;
    mx_rdither_data* data = new mx_rdither_data(rowSize, 2, uniform27,
        ncolours, ilut);

    err = MX_ERROR_OK;

    // Now work out the halved minimum distance between colours
    // If a colour is within the halved distance then it is
    // mapped to this colour

    for (icol1 = 0; icol1 < ncolours; icol1++) {
        minDist = 10000000;
        testColour = colours + icol1;
        for (icol2 = 0; icol2 < ncolours; icol2++) {
            if (icol1 != icol2) {
                dist = ditherDist(testColour, colours + icol2);
                if (dist < minDist)
                    minDist = dist;
            }
        }

        (data->colourData)[icol1] = minDist / 2;
    }

    return data;
}

/*-------------------------------------------------
 * FUNCTION: getDitherPixelValue
 *
 * DESCRIPTION:
 *
 *
 */

inline unsigned char getDitherPixelValue(unsigned char pixelValue,
    int maxError,
    int thisError)
{
    int testValue;

    if (thisError > maxError) {
        thisError = maxError;
    } else if (thisError < -maxError) {
        thisError = -maxError;
    }

    testValue = pixelValue + thisError;

    if (testValue > 255) {
        testValue = 255;
    } else if (testValue < 0) {
        testValue = 0;
    }

    return testValue;
}

/*-------------------------------------------------
 * FUNCTION: ditherRaster
 *
 * DESCRIPTION: Dither an RGB tile to a colour
 *              coded tile
 *
 */

void ditherRaster(int& err,
    raster* output,
    raster* base,
    mx_irect& in,
    unsigned char* inbuffer,
    unsigned char* outbuffer,
    mx_rtransform_data* data)
{
    unsigned char *outptr8, *cptr;
    uint16* outptr16;
    uint32* outptr32;
    int *cerror, *nerror;
    LUT_VALUE lutValue;

    int *currentError = nullptr, *nextError = nullptr;
    mx_colour* colours;
    int ix, iy;
    mx_colour thisColour;
    mx_colour* testColour;
    int minDist, icol;
    int* tempPtr;
    double errorRed, errorGreen, errorBlue;
    int thisDist;
    int maxError = 32;

    mx_ipoint tilesize = output->getTileSize();
    int errorRowByteSize = 3 * (tilesize.x + 2) * sizeof(int);

    mx_rdither_data* ditherData = (mx_rdither_data*)data;

    int redOffset, greenOffset, blueOffset;

    // Intialise colour data
    int ncolours = output->getColours(&colours);
    mx_colour* lastColour = colours;
    int lastColourNum = 0;
    int thisError;
    unsigned char thisPixel;

    // Get initial offset

    if (ditherData->toggle > 0) {
        currentError = ditherData->thisError;
        nextError = ditherData->nextError;
    } else if (ditherData->toggle < 0) {
        nextError = ditherData->thisError;
        currentError = ditherData->nextError;
    }

    if (GABS(ditherData->toggle) == 2) {
        memset(currentError, 0, errorRowByteSize);
        memset(nextError, 0, errorRowByteSize);
    }

    // Initialise input data pointers
    cptr = inbuffer;

    outptr8 = outbuffer;
    outptr16 = (uint16*)outbuffer;
    outptr32 = (uint32*)outbuffer;

    for (iy = 0; iy < tilesize.y; iy++) {
        // Intialise current errors
        cerror = currentError + 3;

        // Intialise next errors
        nerror = nextError;

        // Intialise row data pointers

        for (ix = 0; ix < tilesize.x; ix++) {
            // Get colour in 0-255 range and add error
            thisError = *cerror++;
            thisPixel = *cptr++;
            thisColour.red = getDitherPixelValue(thisPixel,
                thisError,
                maxError);

            thisError = *cerror++;
            thisPixel = *cptr++;
            thisColour.green = getDitherPixelValue(thisPixel,
                thisError,
                maxError);

            thisError = *cerror++;
            thisPixel = *cptr++;
            thisColour.blue = getDitherPixelValue(thisPixel,
                thisError,
                maxError);

            if (ditherData->uniform27) {
                redOffset = (thisColour.red + 64) / 128;
                greenOffset = (thisColour.green + 64) / 128;
                blueOffset = (thisColour.blue + 64) / 128;

                lastColourNum = redOffset * 9 + greenOffset * 3 + blueOffset;
                lastColour = colours + lastColourNum;
            } else {
                if (ditherDist(lastColour, &thisColour) > (ditherData->colourData)[lastColourNum]) {
                    // Not found within range - look for a new number

                    testColour = colours;
                    minDist = 1000000;

                    for (icol = 0; icol < ncolours; icol++) {
                        thisDist = ditherDist(testColour, &thisColour);
                        if (thisDist < (ditherData->colourData)[icol]) {
                            // So close to this colour that it must
                            // be this colour
                            lastColour = testColour;
                            lastColourNum = icol;
                            break;
                        } else if (thisDist < minDist) {
                            lastColour = testColour;
                            lastColourNum = icol;
                            minDist = thisDist;
                        }
                        testColour++;
                    }
                }
            }

            // Set output value

            if (ditherData->lut == NULL) {
                lutValue = lastColourNum;
            } else {
                lutValue = (ditherData->lut)[lastColourNum];
            }

            switch (output->getPack()) {
            case RPACK_BYTE:
                *outptr8++ = lutValue;
                break;
            case RPACK_SHORT:
                *outptr16++ = lutValue;
                break;
            case RPACK_INT:
                *outptr32++ = lutValue;
                break;
            default:
                break;
            }

            // caclculate errors
            errorRed = (thisColour.red - lastColour->red) / 16;
            errorGreen = (thisColour.green - lastColour->green) / 16;
            errorBlue = (thisColour.blue - lastColour->blue) / 16;

            // Update current error
            cerror[0] += ((int)errorRed * 7);
            cerror[1] += ((int)errorGreen * 7);
            cerror[2] += ((int)errorBlue * 7);

            // Update next errors
            nerror[0] += ((int)errorRed * 3);
            nerror[1] += ((int)errorGreen * 3);
            nerror[2] += ((int)errorBlue * 3);
            nerror[3] += ((int)errorRed * 5);
            nerror[4] += ((int)errorGreen * 5);
            nerror[5] += ((int)errorBlue * 5);
            nerror[6] += ((int)errorRed);
            nerror[7] += ((int)errorGreen);
            nerror[8] += ((int)errorBlue);

            nerror += 3;
        }

        // Swap current and next errors

        ditherData->toggle = -(ditherData->toggle);

        tempPtr = currentError;
        currentError = nextError;
        nextError = tempPtr;
        memset(nextError, 0, errorRowByteSize);
    }
}

/*-------------------------------------------------
 * FUNCTION: replicateBuffer
 *
 * DESCRIPTION: Replicate part of a buffer into
 *              another output buffer
 *
 */

static void replicateBuffer(unsigned char* bufferIn,
    mx_irect& in,
    mx_irect& inc,
    mx_irect& incr,
    int npixelbytes,
    unsigned char* bufferOut,
    mx_irect& out,
    mx_irect& outc,
    int repfactor1,
    int repfactor2)
{
    unsigned char* rowBuffero;
    unsigned char* rowBufferi;
    unsigned char *rowStartBuffero, *reprowStartBuffero;
    unsigned char* rowStartBufferi;
    int rowByteWidtho, rowByteWidthoc, rowByteWidthi;
    int nstart, nmiddle, nend, count;
    int nrows, nrowm1, irow, j, rep;

    // Byte widths
    rowByteWidtho = (out.xt - out.xb + 1) * npixelbytes;
    rowByteWidthoc = (outc.xt - outc.xb + 1) * npixelbytes;
    rowByteWidthi = (in.xt - in.xb + 1) * npixelbytes;

    // Start input and output pointers
    rowStartBuffero = bufferOut + rowByteWidtho * (outc.yb - out.yb) + (outc.xb - out.xb) * npixelbytes;
    rowStartBufferi = bufferIn + rowByteWidthi * (inc.yb - in.yb) + (inc.xb - in.xb) * npixelbytes;

    // calculate the number of pixels to output for the first
    // middle and end pixels in the output
    if (inc.xb == inc.xt) {
        // All in one input pixel
        nstart = (incr.xt - incr.xb + 1);
        nmiddle = 0;
        nend = 0;
    } else {
        // Across many pixels
        nstart = (incr.xb == 0) ? 0 : (repfactor1 - incr.xb);
        nend = (incr.xt == (repfactor1 - 1)) ? 0 : (incr.xt + 1);
        // Calculate the number of whole pixels
        nmiddle = ((outc.xt - outc.xb + 1) - nstart - nend) / repfactor1;
    }

    nrows = inc.yt - inc.yb + 1;
    nrowm1 = nrows - 1;

    for (irow = 0; irow < nrows; irow++) {
        /* Create one span replicated */
        rowBuffero = rowStartBuffero;
        rowBufferi = rowStartBufferi;
        reprowStartBuffero = rowStartBuffero;

        if (npixelbytes == 1) {
            if (nstart) {
                memset(rowBuffero, *rowBufferi++, nstart);
                rowBuffero += nstart;
            }

            for (count = 0; count < nmiddle; count++) {
                memset(rowBuffero, *rowBufferi++, repfactor1);
                rowBuffero += repfactor1;
            }

            if (nend) {
                memset(rowBuffero, *rowBufferi++, nend);
                rowBuffero += nend;
            }
        } else {
            if (nstart > 0) {
                for (j = 0; j < nstart; j++) {
                    memcpy(rowBuffero, rowBufferi, npixelbytes);
                    rowBuffero += npixelbytes;
                }
                rowBufferi += npixelbytes;
            }

            for (count = 0; count < nmiddle; count++) {
                for (j = 0; j < repfactor1; j++) {
                    memcpy(rowBuffero, rowBufferi, npixelbytes);
                    rowBuffero += npixelbytes;
                }
                rowBufferi += npixelbytes;
            }

            if (nend > 0) {
                for (j = 0; j < nend; j++) {
                    memcpy(rowBuffero, rowBufferi, npixelbytes);
                    rowBuffero += npixelbytes;
                }
                rowBufferi += npixelbytes;
            }
        }

        // Move output to next line

        rowStartBuffero += rowByteWidtho;

        // Now copy the lines
        rep = repfactor2;

        // First or last might not rep all
        if (irow == 0)
            rep -= incr.yb;
        if (irow == nrowm1)
            rep -= (repfactor2 - incr.yt - 1);

        for (j = 1; j < rep; j++) {
            memcpy(rowStartBuffero, reprowStartBuffero, rowByteWidthoc);
            rowStartBuffero += rowByteWidtho;
        }

        // go on to next input line
        rowStartBufferi += rowByteWidthi;
    }
}

/*-------------------------------------------------
 * FUNCTION: retileRaster
 *
 * DESCRIPTION: Retile a raster
 *
 *
 */

void retileRaster(int& err,
    raster* output,
    raster* base,
    mx_tile_array* tileArray,
    mx_irect& out,
    unsigned char* buffer,
    mx_rtransform_data* data)
{
    mx_ipoint tileSize = base->getTileSize();
    mx_irect outRect, inRect;
    int npixelbits;
    int tileCount;
    int ix, iy;

    err = MX_ERROR_OK;

    npixelbits = base->pixelBitSize();

    tileCount = 0;

    // Loop through the tile array copying the
    // relevant pixels from each input tile to the
    // output tile

    for (iy = tileArray->tl.y; iy <= tileArray->br.y; iy++) {
        for (ix = tileArray->tl.x; ix <= tileArray->br.x; ix++) {
            inRect.xb = ix * tileSize.x;
            inRect.yb = iy * tileSize.y;
            inRect.xt = inRect.xb + tileSize.x - 1;
            inRect.yt = inRect.yb + tileSize.y - 1;

            outRect.xb = (inRect.xb > out.xb) ? inRect.xb : out.xb;
            outRect.yb = (inRect.yb > out.yb) ? inRect.yb : out.yb;
            outRect.xt = (inRect.xt < out.xt) ? inRect.xt : out.xt;
            outRect.yt = (inRect.yt < out.yt) ? inRect.yt : out.yt;

            copyBuffer(tileArray->tiles[tileCount],
                inRect,
                outRect,
                npixelbits,
                buffer,
                out,
                outRect);

            tileCount++;
        }
    }
}

/*-------------------------------------------------
 * FUNCTION: isubsampleRaster
 *
 * DESCRIPTION: Do an integer subsample on a raster
 *
 *
 */

void isubsampleRaster(int& err,
    raster* output,
    raster* base,
    mx_tile_array* tileArray,
    mx_irect& out,
    unsigned char* buffer,
    mx_rtransform_data* data)
{
    int ijumpx, ijumpy;
    bool nextPixx;
    double scaleFactor1, scaleFactor2, inoffx, inoffy;
    int npixelBytes;
    unsigned char *endRowPtr, *endTilePtr;
    int rowSize;
    int32 instartptx;
    mx_ipoint inpt;
    mx_ipoint baseSize, outputSize, outputTilesize;

    err = MX_ERROR_OK;

    baseSize = base->getSize();
    outputSize = output->getSize();
    outputTilesize = output->getTileSize();

    // Get the scale change
    scaleFactor1 = ((double)outputSize.x) / baseSize.x;
    scaleFactor2 = ((double)outputSize.y) / baseSize.y;

    // Decompose the scale into an integer and real part
    ijumpx = (int)((1.0 / scaleFactor1) + 0.5);
    ijumpy = (int)((1.0 / scaleFactor2) + 0.5);

    // Get sizes
    npixelBytes = base->pixelByteSize();
    rowSize = outputTilesize.x * npixelBytes;

    // Set up y iteration parameters
    endTilePtr = buffer + outputTilesize.y * rowSize;

    // Scale output to input values
    scaleDecompose(out.xb / scaleFactor1, 1.0, instartptx, inoffx);
    scaleDecompose(out.yb / scaleFactor2, 1.0, inpt.y, inoffy);

    // Check if 1-1 in x dmx_irection
    nextPixx = (ijumpx == 1);

    if (npixelBytes == 1) {
        unsigned char lastValue;

        do {
            // Set up x parameters
            inpt.x = instartptx;
            endRowPtr = buffer + rowSize;

            *buffer++ = *(tileArray->getPtr(inpt));
            inpt.x += ijumpy;

            while (buffer < endRowPtr) {
                if (nextPixx) {
                    // Moved to the next pixel
                    lastValue = *(tileArray->getNextPtr());
                    *buffer++ = lastValue;
                } else {
                    // Moved several pixels along
                    lastValue = *(tileArray->addPtr(ijumpx));
                    *buffer++ = lastValue;
                }
                inpt.x += ijumpx;
            }

            inpt.y += ijumpy;

        } while (buffer < endTilePtr);
    } else {
        do {
            // Set up x parameters
            inpt.x = instartptx;
            endRowPtr = buffer + rowSize;

            memcpy(buffer, tileArray->getPtr(inpt), npixelBytes);
            buffer += npixelBytes;
            inpt.x += ijumpy;

            while (buffer < endRowPtr) {
                if (nextPixx) {
                    // Moved to next pixel
                    memcpy(buffer, tileArray->getNextPtr(), npixelBytes);
                    buffer += npixelBytes;
                } else {
                    // Moved several pixels along
                    memcpy(buffer, tileArray->addPtr(ijumpx), npixelBytes);
                    buffer += npixelBytes;
                }
                inpt.x += ijumpx;
            }

            inpt.y += ijumpy;

        } while (buffer < endTilePtr);
    }
}

/*-------------------------------------------------
 * FUNCTION: ireplicateRaster
 *
 * DESCRIPTION: Do an integer replication of a
 *              raster
 *
 */

void ireplicateRaster(int& err,
    raster* output,
    raster* base,
    mx_tile_array* tileArray,
    mx_irect& out,
    unsigned char* buffer,
    mx_rtransform_data* data)
{
    mx_ipoint tileSize = base->getTileSize();
    mx_irect outRect, inRect;
    int repfactor1, repfactor2;
    int npixelbytes;
    mx_irect inc, incr;
    mx_ipoint baseSize = base->getSize();
    mx_ipoint outputSize = output->getSize();
    int tileCount;
    int ix, iy;

    err = MX_ERROR_OK;

    // Get the scale change
    repfactor1 = (int)((((double)outputSize.x) / baseSize.x) + 0.5);
    repfactor2 = (int)((((double)outputSize.y) / baseSize.y) + 0.5);

    npixelbytes = base->pixelByteSize();

    // Loop through the input tiles getting the data for
    // replicating the tile data

    tileCount = 0;

    for (iy = tileArray->tl.y; iy <= tileArray->br.y; iy++) {
        for (ix = tileArray->tl.x; ix <= tileArray->br.x; ix++) {
            inRect.xb = ix * tileSize.x;
            inRect.yb = iy * tileSize.y;
            inRect.xt = inRect.xb + tileSize.x - 1;
            inRect.yt = inRect.yb + tileSize.y - 1;

            outRect.xb = inRect.xb * repfactor1;
            outRect.yb = inRect.yb * repfactor2;
            outRect.xt = (inRect.xt + 1) * repfactor1 - 1;
            outRect.yt = (inRect.yt + 1) * repfactor2 - 1;

            outRect.xb = (outRect.xb > out.xb) ? outRect.xb : out.xb;
            outRect.yb = (outRect.yb > out.yb) ? outRect.yb : out.yb;
            outRect.xt = (outRect.xt < out.xt) ? outRect.xt : out.xt;
            outRect.yt = (outRect.yt < out.yt) ? outRect.yt : out.yt;

            inc.xb = outRect.xb / repfactor1;
            incr.xb = outRect.xb % repfactor1;
            inc.xt = outRect.xt / repfactor1;
            incr.xt = outRect.xt % repfactor1;
            inc.yb = outRect.yb / repfactor2;
            incr.yb = outRect.yb % repfactor2;
            inc.yt = outRect.yt / repfactor2;
            incr.yt = outRect.yt % repfactor2;

            replicateBuffer(tileArray->tiles[tileCount],
                inRect,
                inc,
                incr,
                npixelbytes,
                buffer,
                out,
                outRect,
                repfactor1,
                repfactor2);

            tileCount++;
        }
    }
}

/*-------------------------------------------------
 * FUNCTION: rescaleRaster
 *
 * DESCRIPTION: Do a non-integer type rescaling
 *
 *
 */

void rescaleRaster(int& err,
    raster* output,
    raster* base,
    mx_tile_array* tileArray,
    mx_irect& out,
    unsigned char* buffer,
    mx_rtransform_data* data)
{
    double rjumpx, rjumpy;
    int32 ijumpx, ijumpy;
    bool samePixy, samePixx, nextPixx;
    double inoffx, instartoffx;
    double inoffy;
    double scaleFactor1, scaleFactor2;
    int npixelBytes;
    unsigned char *previousRowPtr, *endRowPtr, *endTilePtr;
    int rowSize;
    int32 instartptx;
    mx_ipoint inpt;
    int pixDiff, lastiny;
    mx_ipoint baseSize, outputSize, outputTilesize;

    err = MX_ERROR_OK;

    baseSize = base->getSize();
    outputSize = output->getSize();
    outputTilesize = output->getTileSize();

    // Get the scale change
    scaleFactor1 = ((double)baseSize.x) / outputSize.x;
    scaleFactor2 = ((double)baseSize.y) / outputSize.y;

    // Decompose the scale into an integer and real part
    scaleDecompose(scaleFactor1, 1.0, ijumpx, rjumpx);
    scaleDecompose(scaleFactor2, 1.0, ijumpy, rjumpy);

    // Get sizes
    npixelBytes = base->pixelByteSize();
    rowSize = outputTilesize.x * npixelBytes;

    // Set up y iteration parameters
    samePixy = FALSE;
    previousRowPtr = buffer - rowSize;

    endRowPtr = buffer + rowSize;
    endTilePtr = buffer + rowSize * outputTilesize.y;

    // Scale output to input values
    scaleDecompose(out.xb * scaleFactor1, 1.0, instartptx, instartoffx);
    scaleDecompose(out.yb * scaleFactor2, 1.0, inpt.y, inoffy);

    if (npixelBytes == 1) {
        unsigned char lastValue;

        do {
            // Set up x parameters
            inpt.x = instartptx;
            inoffx = instartoffx;
            samePixx = FALSE;
            nextPixx = FALSE;

            // replicate the previous line
            if (samePixy) {
                memcpy(buffer, previousRowPtr, rowSize);
                buffer += rowSize;
            } else {
                // Start of line
                lastValue = *(tileArray->getPtr(inpt));
                *buffer++ = lastValue;

                // do jump
                pixDiff = inpt.x;
                inpt.x += ijumpx;
                inoffx += rjumpx;
                if (inoffx >= 1.0) {
                    inpt.x++;
                    inoffx -= 1.0;
                }

                pixDiff = inpt.x - pixDiff;

                switch (pixDiff) {
                case 0:
                    samePixx = TRUE;
                    break;
                case 1:
                    samePixx = FALSE;
                    nextPixx = TRUE;
                    break;
                default:
                    samePixx = FALSE;
                    nextPixx = FALSE;
                    break;
                }

                // loop through other pixels
                while (buffer < endRowPtr) {
                    if (samePixx) {
                        // Still in the same pixel
                        *buffer++ = lastValue;
                    } else if (nextPixx) {
                        // Moved to the next pixel
                        lastValue = *(tileArray->getNextPtr());
                        *buffer++ = lastValue;
                    } else {
                        // Moved several pixels along
                        lastValue = *(tileArray->addPtr(pixDiff));
                        *buffer++ = lastValue;
                    }

                    // do jump
                    pixDiff = inpt.x;
                    inpt.x += ijumpx;
                    inoffx += rjumpx;
                    if (inoffx >= 1.0) {
                        inpt.x++;
                        inoffx -= 1.0;
                    }

                    pixDiff = inpt.x - pixDiff;

                    switch (pixDiff) {
                    case 0:
                        samePixx = TRUE;
                        break;
                    case 1:
                        samePixx = FALSE;
                        nextPixx = TRUE;
                        break;
                    default:
                        samePixx = FALSE;
                        nextPixx = FALSE;
                        break;
                    }
                }
            }
            lastiny = inpt.y;
            inpt.y += ijumpy;
            inoffy += rjumpy;
            if (inoffy >= 1.0) {
                inpt.y++;
                inoffy -= 1.0;
            }
            previousRowPtr += rowSize;
            endRowPtr += rowSize;

            samePixy = (inpt.y == lastiny);
        } while (buffer < endTilePtr);
    } else {

        unsigned char* lastValuePtr;

        do {
            // Set up x parameters
            inpt.x = instartptx;
            inoffx = instartoffx;
            samePixx = FALSE;
            nextPixx = FALSE;

            if (samePixy) {
                memcpy(buffer, previousRowPtr, rowSize);
                buffer += rowSize;
            } else {
                // Start of line
                lastValuePtr = tileArray->getPtr(inpt);
                memcpy(buffer, lastValuePtr, npixelBytes);
                buffer += npixelBytes;

                pixDiff = inpt.x;
                inpt.x += ijumpx;
                inoffx += rjumpx;
                if (inoffx >= 1.0) {
                    inpt.x++;
                    inoffx -= 1.0;
                }

                pixDiff = inpt.x - pixDiff;

                switch (pixDiff) {
                case 0:
                    samePixx = TRUE;
                    break;
                case 1:
                    samePixx = FALSE;
                    nextPixx = TRUE;
                    break;
                default:
                    samePixx = FALSE;
                    nextPixx = FALSE;
                    break;
                }

                while (buffer < endRowPtr) {
                    if (samePixx) {
                        memcpy(buffer, lastValuePtr, npixelBytes);
                        buffer += npixelBytes;
                    } else if (nextPixx) {
                        // Moved to the next pixel
                        lastValuePtr = tileArray->getNextPtr();
                        memcpy(buffer, lastValuePtr, npixelBytes);
                        buffer += npixelBytes;
                    } else {
                        // Moved several pixels along
                        lastValuePtr = tileArray->addPtr(pixDiff);
                        memcpy(buffer, lastValuePtr, npixelBytes);
                        buffer += npixelBytes;
                    }

                    pixDiff = inpt.x;
                    inpt.x += ijumpx;
                    inoffx += rjumpx;
                    if (inoffx >= 1.0) {
                        inpt.x++;
                        inoffx -= 1.0;
                    }

                    pixDiff = inpt.x - pixDiff;

                    switch (pixDiff) {
                    case 0:
                        samePixx = TRUE;
                        break;
                    case 1:
                        samePixx = FALSE;
                        nextPixx = TRUE;
                        break;
                    default:
                        samePixx = FALSE;
                        nextPixx = FALSE;
                        break;
                    }
                }
            }

            lastiny = inpt.y;
            inpt.y += ijumpy;
            inoffy += rjumpy;
            if (inoffy >= 1.0) {
                inpt.y++;
                inoffy -= 1.0;
            }
            previousRowPtr += rowSize;
            endRowPtr += rowSize;

            samePixy = (inpt.y == lastiny);
        } while (buffer < endTilePtr);
    }
}

/*-------------------------------------------------
 * Function: traster::~traster
 *
 * DESCRIPTION:
 *
 *
 */

traster::~traster()
{
    delete[] storeBuffer;
    delete[] lut;
    delete transSpecialData;
    delete dataSpecialData;

    if (deleteSource)
        delete base;
}

/*-------------------------------------------------
 * FUNCTION: traster::setTransFunction
 *
 * DESCRIPTION: Set the transformation function
 *
 *
 */

void traster::setTransFunction(int& err,
    rasterTransformFunction transFunc)
{
    err = MX_ERROR_OK;

    if (used) {
        err = MX_RASTER_USED;
        return;
    }

    transformFunction = transFunc;
}

/*-------------------------------------------------
 * FUNCTION: traster::setDataFunction
 *
 * DESCRIPTION: Set the data function
 *
 *
 */

void traster::setDataFunction(int& err,
    rasterDataFunction dataFunc)
{
    err = MX_ERROR_OK;

    if (used) {
        err = MX_RASTER_USED;
        return;
    }
    dataFunction = dataFunc;
}

/*-------------------------------------------------
 * FUNCTION: traster::setSpecialData
 *
 * DESCRIPTION: Set special data for a transform
 *
 *
 */

void traster::setSpecialData(int& err, bool forTrans,
    mx_rtransform_data* data)
{
    if (used) {
        MX_ERROR_THROW(err, MX_RASTER_USED);
    }

    if (forTrans) {
        transSpecialData = data;
    } else {
        dataSpecialData = data;
    }

    goto exit;
abort:
exit:
    return;
}

/*-------------------------------------------------
 * Function: traster::traster
 *
 * DESCRIPTION:
 *
 *
 */

traster::traster(int& err,
    raster* ibase,
    bool ideleteSource,
    RAST_CACHE cacheType)
    : raster(err)
{
    MX_ERROR_CHECK(err);

    // Remember base
    base = ibase;
    storeBuffer = NULL;
    lut = NULL;
    transformFunction = NULL;
    dataFunction = NULL;
    transSpecialData = NULL;
    dataSpecialData = NULL;

    // Copy the basic parameters
    copyBasics(err, *ibase);
    MX_ERROR_CHECK(err);

    used = FALSE;
    deleteSource = ideleteSource;

    createCache(err, cacheType);
    MX_ERROR_CHECK(err);

    goto exit;
abort:
exit:
    return;
}

/*-------------------------------------------------
 * FUNCTION: traster::setSize
 *
 * DESCRIPTION: Set the image size
 *
 *
 */

void traster::setSize(int& err, mx_ipoint& isize)
{
    err = MX_ERROR_OK;

    if (used) {
        MX_ERROR_THROW(err, MX_RASTER_USED);
    }

    size = isize;
    goto exit;
abort:
exit:
    return;
}

/*-------------------------------------------------
 * FUNCTION: traster::setLut
 *
 * DESCRIPTION: Set a lookup table
 *
 *
 */

void traster::setLut(int& err, LUT_VALUE* ilut,
    int lutSize)
{
    err = MX_ERROR_OK;

    if (used) {
        MX_ERROR_THROW(err, MX_RASTER_USED);
    }

    if (lutSize < 256)
        lutSize = 256;

    lut = new LUT_VALUE[256];

    memset(lut, 0, 256 * sizeof(LUT_VALUE));
    memcpy(lut, ilut, lutSize * sizeof(LUT_VALUE));

    goto exit;
abort:
exit:
    return;
}

/*-------------------------------------------------
 * FUNCTION: traster::setTileSize
 *
 * DESCRIPTION: Set the tile size
 *
 *
 */

void traster::setTileSize(int& err, mx_ipoint& itilesize)
{
    err = MX_ERROR_OK;

    if (used) {
        MX_ERROR_THROW(err, MX_RASTER_USED);
    }

    tileSize = itilesize;
    goto exit;
abort:
exit:
    return;
}

/*-------------------------------------------------
 * FUNCTION: traster::setPack
 *
 * DESCRIPTION: Set the pack type
 *
 *
 */

void traster::setPack(int& err, RAST_PACK ipack)
{
    err = MX_ERROR_OK;

    if (used) {
        MX_ERROR_THROW(err, MX_RASTER_USED);
    }

    pack = ipack;

    goto exit;

abort:
exit:
    return;
}

/*-------------------------------------------------
 * FUNCTION: scaleSnap
 *
 * DESCRIPTION: Snap a value
 *
 *
 */

void scaleSnap(double scaleFactor,
    int32& ifactor,
    double& rfactor)
{
    scaleDecompose(scaleFactor, 1.0, ifactor, rfactor);

    if (rfactor < RAS_VALUE_SMALL) {
        rfactor = 0;
    } else if (rfactor > RAS_VALUE_CSMALL) {
        ifactor++;
        rfactor = 0;
    }
}

/*-------------------------------------------------
 * FUNCTION: getScaleChangeFunction
 *
 * DESCRIPTION: Get the transformation function
 *              for rescaling or retiling
 *
 *
 */

void traster::getScaleChangeFunction(int& err,
    double scaleFactor1,
    double scaleFactor2)
{

    int32 ix, iy;
    double rx, ry;

    err = MX_ERROR_OK;

    if (transformFunction != NULL)
        return;

    // Decompose the scale into an integer and real part
    scaleSnap(scaleFactor1, ix, rx);
    scaleSnap(scaleFactor2, iy, ry);

    if ((ix == 0) && (iy == 0)) {
        // Might be a replication
        scaleFactor1 = 1.0 / scaleFactor1;
        scaleFactor2 = 1.0 / scaleFactor2;

        scaleSnap(scaleFactor1, ix, rx);
        scaleSnap(scaleFactor2, iy, ry);

        if ((rx == 0.0) && (ry == 0.0)) {
            transformFunction = ireplicateRaster;
        } else {
            transformFunction = rescaleRaster;
        }
    } else if ((rx == 0.0) && (ry == 0.0)) {
        // An integer subsampling - might be
        // an identity - set NULL in this
        // case

        if ((ix == 1) && (iy == 1)) {
            if (base->getTileSize() != tileSize) {
                transformFunction = retileRaster;
            } else {
                transformFunction = NULL;
            }
        } else {
            transformFunction = isubsampleRaster;
        }
    } else {
        transformFunction = rescaleRaster;
    }
}

/*-------------------------------------------------
 * FUNCTION: shiftRaster
 *
 * DESCRIPTION: Shift a (greyscale) raster down
 *
 *
 */

void shiftRaster(int& err,
    raster* output,
    raster* base,
    mx_irect& in,
    unsigned char* inbuffer,
    unsigned char* outbuffer,
    mx_rtransform_data* data)
{
    int npixels = output->tilePixels();
    unsigned char* endptr = outbuffer + npixels;
    mx_rshift_data* shiftData = (mx_rshift_data*)data;
    int shift = shiftData->shift;
    LUT_VALUE* lut = shiftData->lut;

    err = MX_ERROR_OK;

    // shift down

    if (lut == NULL) {
        while (outbuffer < endptr) {
            *outbuffer++ = (*inbuffer++) >> shift;
        }
    } else {
        while (outbuffer < endptr) {
            *outbuffer++ = lut[((*inbuffer++) >> shift)];
        }
    }
}

/*-------------------------------------------------
 * FUNCTION: packRaster
 *
 * DESCRIPTION: Pack a byte raster to binary
 *
 *
 */

void packRaster(int& err,
    raster* output,
    raster* base,
    mx_irect& in,
    unsigned char* inbuffer,
    unsigned char* outbuffer,
    mx_rtransform_data* data)
{
    err = MX_ERROR_OK;

    rasPack(output->tilePixels(), outbuffer, inbuffer);
}

/*-------------------------------------------------
 * FUNCTION: lutRaster
 *
 * DESCRIPTION: Put a raster through a lookup table
 *
 *
 */

void lutRaster(int& err,
    raster* output,
    raster* base,
    mx_irect& in,
    unsigned char* inbuffer,
    unsigned char* outbuffer,
    mx_rtransform_data* data)
{
    traster* tr = (traster*)output;

    err = MX_ERROR_OK;

    lookupConvert(tr->tilePixels(),
        inbuffer,
        output->getPack(),
        outbuffer,
        tr->lut);
}

/*-------------------------------------------------
 * FUNCTION: unpackRaster
 *
 * DESCRIPTION: Unpack a bit raster to byte
 *
 *
 */

void unpackRaster(int& err,
    raster* output,
    raster* base,
    mx_irect& in,
    unsigned char* inbuffer,
    unsigned char* outbuffer,
    mx_rtransform_data* data)
{
    err = MX_ERROR_OK;

    rasUnpack(output->tilePixels(), inbuffer, outbuffer);
}

/*-------------------------------------------------
 * FUNCTION: traster::setSigbits
 *
 * DESCRIPTION: Set the number of planes and
 *             number of significant bits
 *
 */

void traster::setSigbits(int& err, int32 inplanes, int32* isigbits)
{
    int i;

    err = MX_ERROR_OK;

    if (used) {
        err = MX_RASTER_USED;
        return;
    }

    nplanes = inplanes;

    for (i = 0; i < nplanes; i++)
        sigbits[i] = isigbits[i];
}

/*-------------------------------------------------
 * FUNCTION: traster::setData
 *
 * DESCRIPTION: Set the datatype of a raster
 *
 *
 */

void traster::setData(int& err, RAST_DATA idata)
{
    err = MX_ERROR_OK;

    if (used) {
        err = MX_RASTER_USED;
        return;
    }

    data = idata;
}

/*-------------------------------------------------
 * FUNCTION: traster::setColours
 *
 * DESCRIPTION: Set the colours on a raster
 *
 *
 */

void traster::setColours(int& err, int inumColours, mx_colour* icolours)
{
    int i;

    err = MX_ERROR_OK;

    if (used) {
        err = MX_RASTER_USED;
        return;
    }

    // Delete any existing colours

    delete[] colours;

    numColours = inumColours;

    // create new colours
    colours = new mx_colour[inumColours];

    // set values - with name to NULL
    for (i = 0; i < inumColours; i++) {
        colours[i] = icolours[i];
        colours[i].name = NULL;
    }
}

/*-------------------------------------------------
 * FUNCTION: getDataChangeFunc
 *
 * DESCRIPTION: Get the data change function
 *
 *
 */

void traster::getDataChangeFunction(int& err)
{
    err = MX_ERROR_OK;

    // If data function exists use it
    if (dataFunction != NULL)
        return;

    if ((base->getData() == RDATA_RGB) && (data == RDATA_COLOUR)) {
        mx_rdither_data* ditherData;

        // dither from rgb to colour using
        // colours on the transform raster

        dataFunction = ditherRaster;

        ditherData = setupDither(err, getTileSize().x,
            numColours, colours, TRUE, lut);
        MX_ERROR_CHECK(err);

        setSpecialData(err, FALSE, ditherData);
        MX_ERROR_CHECK(err);

    } else if (base->getPack() == pack) {
        if (lut != NULL) {
            // use lut if present
            dataFunction = lutRaster;
        } else if ((base->getData() == RDATA_GREYSCALE) && (base->getSigbits(0) != sigbits[0])) {
            mx_rshift_data* shiftData;
            int shift;

            // greyscale - but number of sigbits different
            // set shift function

            dataFunction = shiftRaster;

            shift = base->getSigbits(0) - sigbits[0];
            shiftData = new mx_rshift_data(shift, lut);

            setSpecialData(err, FALSE, shiftData);
            MX_ERROR_CHECK(err);
        } else {
            dataFunction = NULL;
        }
    } else if (lut != NULL) {
        // use lut if present
        dataFunction = lutRaster;
    } else {
        // Pack type is different - binary to byte or
        // byte to binary

        if (base->getPack() == RPACK_BYTE) {
            dataFunction = packRaster;
        } else {
            dataFunction = unpackRaster;
        }
    }
    return;
abort:
    return;
}

/*-------------------------------------------------
 * FUNCTION: getTile
 *
 * DESCRIPTION: Basic get tile routine
 *
 *
 */

unsigned char* traster::getTile(int& err,
    unsigned char* thisBuffer,
    bool getForEdit,
    bool& unknown,
    mx_ipoint& tile)

{
    static int level = 0;
    static int ntotalLevels = 0;
    static mx_tile_array* tileArrays;

    mx_tile_array* tileArray;
    mx_tile_array* newTileArrays;
    double scaleFactor1, scaleFactor2;
    mx_irect tilePixels, iinTilePixels;
    mx_rect rtilePixels;
    mx_affine a;
    mx_rect inTilePixels;
    mx_irect outPixels;
    int tileCount, itx, ity, ntiles;
    mx_ipoint baseTile;
    rasItem* item;
    unsigned char* transformedBuffer;
    bool levelIncremented = FALSE;
    bool thisUnknown;

    err = MX_ERROR_OK;
    unknown = TRUE;

    // get scale transformation
    scaleFactor1 = ((double)base->getSize().x) / size.x;
    scaleFactor2 = ((double)base->getSize().y) / size.y;

    if (!used) {
        // get the transformations required
        getScaleChangeFunction(err, scaleFactor1, scaleFactor2);
        MX_ERROR_CHECK(err);

        getDataChangeFunction(err);
        MX_ERROR_CHECK(err);

        used = TRUE;
    }

    // Calculate the output pixel coordinates
    tileToPixels(tile, outPixels);

    // Need to create an array of tiles for a transformation
    if (transformFunction != NULL) {
        // These can exist on many levels (possibly for the
        // same raster). get the next level

        if (level == ntotalLevels) {
            // all allocated levels used
            newTileArrays = new mx_tile_array[ntotalLevels + 1];

            memcpy(newTileArrays, tileArrays, sizeof(mx_tile_array) * ntotalLevels);
            delete[] tileArrays;

            tileArrays = newTileArrays;
            tileArrays[ntotalLevels].ntotalTiles = 0;
            tileArrays[ntotalLevels].tiles = NULL;
            tileArrays[ntotalLevels].items = NULL;

            ntotalLevels++;
        }

        // get this level
        tileArray = tileArrays + level;
        levelIncremented = TRUE;
        level++;

        // Set up tile array parameters
        tileArray->tsz = base->getTileSize();
        tileArray->npb = base->pixelByteSize();

        // If there is a data function as well as a
        // transform function we need to create an
        // intermediate buffer to hold the
        // transform base data

        if (dataFunction != NULL) {
            if (storeBuffer == NULL) {
                storeBuffer = new unsigned char[tileSize.x * tileSize.y * tileArray->npb];
            }
            transformedBuffer = storeBuffer;
        } else {
            // use the output buffer
            transformedBuffer = thisBuffer;
        }

        // Get the pixels in this tile
        tilePixels = outPixels;

        // Increase by one pixel to get the area for scaling

        (tilePixels.xt)++;
        (tilePixels.yt)++;

        a.set(scaleFactor1, 0, 0, scaleFactor2, 0, 0);

        rtilePixels = tilePixels;
        inTilePixels = a * rtilePixels;

        iinTilePixels = inTilePixels.container(RAS_VALUE_SMALL);

        // Round the out tile pixels

        (iinTilePixels.xt)--;
        (iinTilePixels.yt)--;

        mx_ipoint tl = iinTilePixels.topLeft();
        mx_ipoint br = iinTilePixels.bottomRight();

        base->pointToTile(tl, tileArray->tl);
        base->pointToTile(br, tileArray->br);

        base->tileToPixels(tileArray->tl, iinTilePixels);

        tileArray->ptl = iinTilePixels.topLeft();

        tileArray->ntiles.x = tileArray->br.x - tileArray->tl.x + 1;
        tileArray->ntiles.y = tileArray->br.y - tileArray->tl.y + 1;

        ntiles = tileArray->ntiles.x * tileArray->ntiles.y;

        // get memory for the new tiles if required
        if (ntiles > tileArray->ntotalTiles) {
            delete[] tileArray->tiles;
            delete[] tileArray->items;

            tileArray->tiles = new unsigned char*[ntiles];

            tileArray->items = new rasItem*[ntiles];

            tileArray->ntotalTiles = ntiles;
        }

        // get the data for the transformation
        tileCount = 0;
        for (ity = tileArray->tl.y; ity <= tileArray->br.y; ity++) {
            baseTile.y = ity;
            for (itx = tileArray->tl.x; itx <= tileArray->br.x; itx++) {
                baseTile.x = itx;

                (tileArray->tiles)[tileCount] = base->tile(err, FALSE, thisUnknown, baseTile, &item);
                MX_ERROR_CHECK(err);

                if (unknown)
                    unknown = thisUnknown;

                (tileArray->items)[tileCount] = item;
                tileCount++;

                // lock the tile in the cache if required
                if (item != NULL)
                    RLOCK(item);
            }
        }

        if (unknown) {
            memset(thisBuffer, 0, tileByteSize());
            return thisBuffer;
        }

        (*transformFunction)(err, this, base, tileArray, outPixels, transformedBuffer, transSpecialData);
        MX_ERROR_CHECK(err);

        // err undo the locks on the items if required
        for (ity = 0; ity < tileCount; ity++) {
            item = tileArray->items[ity];
            if (item != NULL)
                RUNLOCK(item);
        }
    } else {
        // No transformation - get the base data for a data transformation
        transformedBuffer = base->tile(err, FALSE, unknown, tile, &item);
        MX_ERROR_CHECK(err);

        if (unknown) {
            memset(thisBuffer, 0, tileByteSize());
            return thisBuffer;
        }
    }

    // Do data transformtaion now
    if (dataFunction != NULL) {
        (*dataFunction)(err, this, base, outPixels, transformedBuffer, thisBuffer, dataSpecialData);
        MX_ERROR_CHECK(err);
    } else if (transformFunction == NULL) {
        memcpy(thisBuffer, transformedBuffer, tileByteSize());
    }

    goto exit;
abort:
exit:
    if (levelIncremented)
        level--;
    return thisBuffer;
}
