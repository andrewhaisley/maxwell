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
 * MODULE : mx_pr_ras.C
 *
 * AUTHOR : David Miller
 *
 * This file
 *
 * DESCRIPTION:
 * Module mx_pr_ras.C
 *
 *
 * Revision 1.1  1996/10/06 13:15:45  andrew
 * Initial revision
 *
 *
 */

#include <mx_pr_device.h>
#include <raster.h>

static char outputHexFromByte[256][2] = {
    { '0', '0' }, { '0', '1' }, { '0', '2' }, { '0', '3' },
    { '0', '4' }, { '0', '5' }, { '0', '6' }, { '0', '7' },
    { '0', '8' }, { '0', '9' }, { '0', 'a' }, { '0', 'b' },
    { '0', 'c' }, { '0', 'd' }, { '0', 'e' }, { '0', 'f' },
    { '1', '0' }, { '1', '1' }, { '1', '2' }, { '1', '3' },
    { '1', '4' }, { '1', '5' }, { '1', '6' }, { '1', '7' },
    { '1', '8' }, { '1', '9' }, { '1', 'a' }, { '1', 'b' },
    { '1', 'c' }, { '1', 'd' }, { '1', 'e' }, { '1', 'f' },
    { '2', '0' }, { '2', '1' }, { '2', '2' }, { '2', '3' },
    { '2', '4' }, { '2', '5' }, { '2', '6' }, { '2', '7' },
    { '2', '8' }, { '2', '9' }, { '2', 'a' }, { '2', 'b' },
    { '2', 'c' }, { '2', 'd' }, { '2', 'e' }, { '2', 'f' },
    { '3', '0' }, { '3', '1' }, { '3', '2' }, { '3', '3' },
    { '3', '4' }, { '3', '5' }, { '3', '6' }, { '3', '7' },
    { '3', '8' }, { '3', '9' }, { '3', 'a' }, { '3', 'b' },
    { '3', 'c' }, { '3', 'd' }, { '3', 'e' }, { '3', 'f' },
    { '4', '0' }, { '4', '1' }, { '4', '2' }, { '4', '3' },
    { '4', '4' }, { '4', '5' }, { '4', '6' }, { '4', '7' },
    { '4', '8' }, { '4', '9' }, { '4', 'a' }, { '4', 'b' },
    { '4', 'c' }, { '4', 'd' }, { '4', 'e' }, { '4', 'f' },
    { '5', '0' }, { '5', '1' }, { '5', '2' }, { '5', '3' },
    { '5', '4' }, { '5', '5' }, { '5', '6' }, { '5', '7' },
    { '5', '8' }, { '5', '9' }, { '5', 'a' }, { '5', 'b' },
    { '5', 'c' }, { '5', 'd' }, { '5', 'e' }, { '5', 'f' },
    { '6', '0' }, { '6', '1' }, { '6', '2' }, { '6', '3' },
    { '6', '4' }, { '6', '5' }, { '6', '6' }, { '6', '7' },
    { '6', '8' }, { '6', '9' }, { '6', 'a' }, { '6', 'b' },
    { '6', 'c' }, { '6', 'd' }, { '6', 'e' }, { '6', 'f' },
    { '7', '0' }, { '7', '1' }, { '7', '2' }, { '7', '3' },
    { '7', '4' }, { '7', '5' }, { '7', '6' }, { '7', '7' },
    { '7', '8' }, { '7', '9' }, { '7', 'a' }, { '7', 'b' },
    { '7', 'c' }, { '7', 'd' }, { '7', 'e' }, { '7', 'f' },
    { '8', '0' }, { '8', '1' }, { '8', '2' }, { '8', '3' },
    { '8', '4' }, { '8', '5' }, { '8', '6' }, { '8', '7' },
    { '8', '8' }, { '8', '9' }, { '8', 'a' }, { '8', 'b' },
    { '8', 'c' }, { '8', 'd' }, { '8', 'e' }, { '8', 'f' },
    { '9', '0' }, { '9', '1' }, { '9', '2' }, { '9', '3' },
    { '9', '4' }, { '9', '5' }, { '9', '6' }, { '9', '7' },
    { '9', '8' }, { '9', '9' }, { '9', 'a' }, { '9', 'b' },
    { '9', 'c' }, { '9', 'd' }, { '9', 'e' }, { '9', 'f' },
    { 'a', '0' }, { 'a', '1' }, { 'a', '2' }, { 'a', '3' },
    { 'a', '4' }, { 'a', '5' }, { 'a', '6' }, { 'a', '7' },
    { 'a', '8' }, { 'a', '9' }, { 'a', 'a' }, { 'a', 'b' },
    { 'a', 'c' }, { 'a', 'd' }, { 'a', 'e' }, { 'a', 'f' },
    { 'b', '0' }, { 'b', '1' }, { 'b', '2' }, { 'b', '3' },
    { 'b', '4' }, { 'b', '5' }, { 'b', '6' }, { 'b', '7' },
    { 'b', '8' }, { 'b', '9' }, { 'b', 'a' }, { 'b', 'b' },
    { 'b', 'c' }, { 'b', 'd' }, { 'b', 'e' }, { 'b', 'f' },
    { 'c', '0' }, { 'c', '1' }, { 'c', '2' }, { 'c', '3' },
    { 'c', '4' }, { 'c', '5' }, { 'c', '6' }, { 'c', '7' },
    { 'c', '8' }, { 'c', '9' }, { 'c', 'a' }, { 'c', 'b' },
    { 'c', 'c' }, { 'c', 'd' }, { 'c', 'e' }, { 'c', 'f' },
    { 'd', '0' }, { 'd', '1' }, { 'd', '2' }, { 'd', '3' },
    { 'd', '4' }, { 'd', '5' }, { 'd', '6' }, { 'd', '7' },
    { 'd', '8' }, { 'd', '9' }, { 'd', 'a' }, { 'd', 'b' },
    { 'd', 'c' }, { 'd', 'd' }, { 'd', 'e' }, { 'd', 'f' },
    { 'e', '0' }, { 'e', '1' }, { 'e', '2' }, { 'e', '3' },
    { 'e', '4' }, { 'e', '5' }, { 'e', '6' }, { 'e', '7' },
    { 'e', '8' }, { 'e', '9' }, { 'e', 'a' }, { 'e', 'b' },
    { 'e', 'c' }, { 'e', 'd' }, { 'e', 'e' }, { 'e', 'f' },
    { 'f', '0' }, { 'f', '1' }, { 'f', '2' }, { 'f', '3' },
    { 'f', '4' }, { 'f', '5' }, { 'f', '6' }, { 'f', '7' },
    { 'f', '8' }, { 'f', '9' }, { 'f', 'a' }, { 'f', 'b' },
    { 'f', 'c' }, { 'f', 'd' }, { 'f', 'e' }, { 'f', 'f' }
};

/*-------------------------------------------------
 * FUNCTION: mx_pr_device::pdrawTile
 *
 * DESCRIPTION:
 *
 *
 */

void mx_print_device::pdrawTile(int& err,
    MX_DEVICE_RASTER tileType,
    int nsigbits,
    int nunpackedBits,
    mx_ipoint& tileSize,
    mx_ipoint& topLeft,
    void* tile,
    mx_irect* clipRect)
{
    static char outputHexFromBinaryByte[256][2] = {
        { 'f', 'f' }, { '7', 'f' }, { 'b', 'f' }, { '3', 'f' },
        { 'd', 'f' }, { '5', 'f' }, { '9', 'f' }, { '1', 'f' },
        { 'e', 'f' }, { '6', 'f' }, { 'a', 'f' }, { '2', 'f' },
        { 'c', 'f' }, { '4', 'f' }, { '8', 'f' }, { '0', 'f' },
        { 'f', '7' }, { '7', '7' }, { 'b', '7' }, { '3', '7' },
        { 'd', '7' }, { '5', '7' }, { '9', '7' }, { '1', '7' },
        { 'e', '7' }, { '6', '7' }, { 'a', '7' }, { '2', '7' },
        { 'c', '7' }, { '4', '7' }, { '8', '7' }, { '0', '7' },
        { 'f', 'b' }, { '7', 'b' }, { 'b', 'b' }, { '3', 'b' },
        { 'd', 'b' }, { '5', 'b' }, { '9', 'b' }, { '1', 'b' },
        { 'e', 'b' }, { '6', 'b' }, { 'a', 'b' }, { '2', 'b' },
        { 'c', 'b' }, { '4', 'b' }, { '8', 'b' }, { '0', 'b' },
        { 'f', '3' }, { '7', '3' }, { 'b', '3' }, { '3', '3' },
        { 'd', '3' }, { '5', '3' }, { '9', '3' }, { '1', '3' },
        { 'e', '3' }, { '6', '3' }, { 'a', '3' }, { '2', '3' },
        { 'c', '3' }, { '4', '3' }, { '8', '3' }, { '0', '3' },
        { 'f', 'd' }, { '7', 'd' }, { 'b', 'd' }, { '3', 'd' },
        { 'd', 'd' }, { '5', 'd' }, { '9', 'd' }, { '1', 'd' },
        { 'e', 'd' }, { '6', 'd' }, { 'a', 'd' }, { '2', 'd' },
        { 'c', 'd' }, { '4', 'd' }, { '8', 'd' }, { '0', 'd' },
        { 'f', '5' }, { '7', '5' }, { 'b', '5' }, { '3', '5' },
        { 'd', '5' }, { '5', '5' }, { '9', '5' }, { '1', '5' },
        { 'e', '5' }, { '6', '5' }, { 'a', '5' }, { '2', '5' },
        { 'c', '5' }, { '4', '5' }, { '8', '5' }, { '0', '5' },
        { 'f', '9' }, { '7', '9' }, { 'b', '9' }, { '3', '9' },
        { 'd', '9' }, { '5', '9' }, { '9', '9' }, { '1', '9' },
        { 'e', '9' }, { '6', '9' }, { 'a', '9' }, { '2', '9' },
        { 'c', '9' }, { '4', '9' }, { '8', '9' }, { '0', '9' },
        { 'f', '1' }, { '7', '1' }, { 'b', '1' }, { '3', '1' },
        { 'd', '1' }, { '5', '1' }, { '9', '1' }, { '1', '1' },
        { 'e', '1' }, { '6', '1' }, { 'a', '1' }, { '2', '1' },
        { 'c', '1' }, { '4', '1' }, { '8', '1' }, { '0', '1' },
        { 'f', 'e' }, { '7', 'e' }, { 'b', 'e' }, { '3', 'e' },
        { 'd', 'e' }, { '5', 'e' }, { '9', 'e' }, { '1', 'e' },
        { 'e', 'e' }, { '6', 'e' }, { 'a', 'e' }, { '2', 'e' },
        { 'c', 'e' }, { '4', 'e' }, { '8', 'e' }, { '0', 'e' },
        { 'f', '6' }, { '7', '6' }, { 'b', '6' }, { '3', '6' },
        { 'd', '6' }, { '5', '6' }, { '9', '6' }, { '1', '6' },
        { 'e', '6' }, { '6', '6' }, { 'a', '6' }, { '2', '6' },
        { 'c', '6' }, { '4', '6' }, { '8', '6' }, { '0', '6' },
        { 'f', 'a' }, { '7', 'a' }, { 'b', 'a' }, { '3', 'a' },
        { 'd', 'a' }, { '5', 'a' }, { '9', 'a' }, { '1', 'a' },
        { 'e', 'a' }, { '6', 'a' }, { 'a', 'a' }, { '2', 'a' },
        { 'c', 'a' }, { '4', 'a' }, { '8', 'a' }, { '0', 'a' },
        { 'f', '2' }, { '7', '2' }, { 'b', '2' }, { '3', '2' },
        { 'd', '2' }, { '5', '2' }, { '9', '2' }, { '1', '2' },
        { 'e', '2' }, { '6', '2' }, { 'a', '2' }, { '2', '2' },
        { 'c', '2' }, { '4', '2' }, { '8', '2' }, { '0', '2' },
        { 'f', 'c' }, { '7', 'c' }, { 'b', 'c' }, { '3', 'c' },
        { 'd', 'c' }, { '5', 'c' }, { '9', 'c' }, { '1', 'c' },
        { 'e', 'c' }, { '6', 'c' }, { 'a', 'c' }, { '2', 'c' },
        { 'c', 'c' }, { '4', 'c' }, { '8', 'c' }, { '0', 'c' },
        { 'f', '4' }, { '7', '4' }, { 'b', '4' }, { '3', '4' },
        { 'd', '4' }, { '5', '4' }, { '9', '4' }, { '1', '4' },
        { 'e', '4' }, { '6', '4' }, { 'a', '4' }, { '2', '4' },
        { 'c', '4' }, { '4', '4' }, { '8', '4' }, { '0', '4' },
        { 'f', '8' }, { '7', '8' }, { 'b', '8' }, { '3', '8' },
        { 'd', '8' }, { '5', '8' }, { '9', '8' }, { '1', '8' },
        { 'e', '8' }, { '6', '8' }, { 'a', '8' }, { '2', '8' },
        { 'c', '8' }, { '4', '8' }, { '8', '8' }, { '0', '8' },
        { 'f', '0' }, { '7', '0' }, { 'b', '0' }, { '3', '0' },
        { 'd', '0' }, { '5', '0' }, { '9', '0' }, { '1', '0' },
        { 'e', '0' }, { '6', '0' }, { 'a', '0' }, { '2', '0' },
        { 'c', '0' }, { '4', '0' }, { '8', '0' }, { '0', '0' }
    };

    static char outputHexFromByte16[16] = {
        '0', '1', '2', '3',
        '4', '5', '6', '7',
        '8', '9', 'a', 'b',
        'c', 'd', 'e', 'f'
    };

    static char outputHexFromByte4[4][4] = {
        { '0', '1', '2', '3' },
        { '4', '5', '6', '7' },
        { '8', '9', 'a', 'b' },
        { 'c', 'd', 'e', 'f' }
    };

    static char outputHexFromByte2[2][2][2][2] = {
        { { { '0', '1' }, { '2', '3' } },
            { { '4', '5' }, { '6', '7' } } },
        { { { '8', '9' }, { 'a', 'b' } },
            { { 'c', 'd' }, { 'e', 'f' } } }
    };

    int nrowBytes = 0;
    int packingType;
    unsigned char* buffer;
    unsigned char* startBuffer;
    unsigned char* endBuffer;
    int extraBytes = 0;
    int ibyte;
    unsigned char ebuffer[8] = { 0, 0, 0, 0, 0, 0, 0, 0 };
    int irow;
    mx_ipoint rowSize;
    int originalOffset;
    int originalByteOffset = 0;
    int noutputBits = 0, noutputBytes = 0;

    static unsigned char* reverseBuffer = NULL;
    static unsigned char* unpackBuffer = NULL;
    static int unpackBufferSize = 0;

    err = MX_ERROR_OK;

    if (clipRect == NULL) {
        rowSize.x = tileSize.x;
        rowSize.y = tileSize.y;
        originalOffset = 0;
    } else {
        rowSize.x = clipRect->xt - clipRect->xb + 1;
        rowSize.y = clipRect->yt - clipRect->yb + 1;
        originalOffset = clipRect->yb * tileSize.x + clipRect->xb;
    }

    switch (nsigbits) {
    case 1:
        if (nunpackedBits == 1) {
            packingType = 1;
        } else {
            packingType = 2;
        }
        break;
    case 2:
        packingType = 3;
        break;
    case 4:
        packingType = 4;
        break;
    case 8:
        packingType = 5;
        break;
    default:
        packingType = 1;
        break;
    }

    if (packingType == 1) {
        if ((clipRect != NULL) && ((clipRect->xb % 8 != 0) || (clipRect->xt % 8 != 7))) {
            // not on eight bit boundaries
            // unpack

            int unpackSize = tileSize.x * tileSize.y;
            int reverseSize = unpackSize / 8;

            if (unpackSize > unpackBufferSize) {
                delete[] unpackBuffer;
                reverseBuffer = new unsigned char[reverseSize];
                unpackBuffer = new unsigned char[unpackSize];
                unpackBufferSize = unpackSize;
            }

            /* 1 is black in binary but white in grey scale */
            rasterInvert(reverseSize, (unsigned char*)tile, reverseBuffer);

            rasUnpack(unpackSize, reverseBuffer, unpackBuffer);

            tile = unpackBuffer;
            tileType = MX_DEVICE_GREY;
            packingType = 2;
        }
    }

    // calculate the number of bits to be displayed for a line
    // this must be an eight bit boundary !!

    switch (tileType) {
    case MX_DEVICE_RGB:
        originalByteOffset = originalOffset * 3;
        noutputBytes = rowSize.x * 3;
        noutputBits = noutputBytes * nsigbits;
        nrowBytes = tileSize.x * 3;
        break;
    case MX_DEVICE_BIT:
        originalByteOffset = originalOffset / 8;
        noutputBytes = (rowSize.x + 7) / 8;
        noutputBits = rowSize.x;
        nrowBytes = (tileSize.x + 7) / 8;
        break;
    case MX_DEVICE_LUT:
    case MX_DEVICE_GREY:
        originalByteOffset = originalOffset;
        noutputBytes = rowSize.x;
        noutputBits = noutputBytes * nsigbits;
        nrowBytes = tileSize.x;
        break;
    }

    // calculate how many padding bytes we need

    extraBytes = noutputBits % 8;
    if (extraBytes != 0) {
        extraBytes = extraBytes / nsigbits;
        noutputBytes -= extraBytes;
    }

    switch (packingType) {
    case 1:

        startBuffer = ((unsigned char*)tile) + originalByteOffset;
        for (irow = 0; irow < rowSize.y; irow++) {
            endBuffer = startBuffer + noutputBytes;
            buffer = startBuffer;

            while (buffer < endBuffer) {
                if (fputc(outputHexFromBinaryByte[*buffer][0], tempFile) == EOF) {
                    MX_ERROR_THROW(err, MX_PS_FILE_ERROR);
                }
                if (fputc(outputHexFromBinaryByte[*buffer][1], tempFile) == EOF) {
                    MX_ERROR_THROW(err, MX_PS_FILE_ERROR);
                }
                buffer++;
            }
            startBuffer += nrowBytes;
        }
        break;
    case 2:

        startBuffer = ((unsigned char*)tile) + originalByteOffset;

        for (irow = 0; irow < rowSize.y; irow++) {
            endBuffer = startBuffer + noutputBytes;
            buffer = startBuffer;

            while (buffer < endBuffer) {
                if (fputc(outputHexFromByte2[*buffer][*(buffer + 1)][*(buffer + 2)][*(buffer + 3)],
                        tempFile)
                    == EOF) {
                    MX_ERROR_THROW(err, MX_PS_FILE_ERROR);
                }
                buffer += 4;
            }

            // see if we need any padding

            if (extraBytes) {
                for (ibyte = 0; ibyte < extraBytes; ibyte++) {
                    ebuffer[ibyte] = *buffer++;
                }

                if (fputc(outputHexFromByte2[*ebuffer][*(ebuffer + 1)][*(ebuffer + 2)][*(ebuffer + 3)],
                        tempFile)
                    == EOF) {
                    MX_ERROR_THROW(err, MX_PS_FILE_ERROR);
                }
                if (fputc(outputHexFromByte2[*(ebuffer + 4)][*(ebuffer + 1)]
                                            [*(ebuffer + 2)][*(ebuffer + 3)],
                        tempFile)
                    == EOF) {
                    MX_ERROR_THROW(err, MX_PS_FILE_ERROR);
                }
            }

            startBuffer += nrowBytes;
        }
        break;
    case 3:

        startBuffer = ((unsigned char*)tile) + originalByteOffset;

        for (irow = 0; irow < rowSize.y; irow++) {
            endBuffer = startBuffer + noutputBytes;
            buffer = startBuffer;

            while (buffer < endBuffer) {
                if (fputc(outputHexFromByte4[*buffer][*(buffer + 1)],
                        tempFile)
                    == EOF) {
                    MX_ERROR_THROW(err, MX_PS_FILE_ERROR);
                }
                buffer += 2;
            }

            if (extraBytes) {
                for (ibyte = 0; ibyte < extraBytes; ibyte++) {
                    ebuffer[ibyte] = *buffer++;
                }

                if (fputc(outputHexFromByte4[*ebuffer][*(ebuffer + 1)],
                        tempFile)
                    == EOF) {
                    MX_ERROR_THROW(err, MX_PS_FILE_ERROR);
                }

                if (fputc(outputHexFromByte4[*(ebuffer + 2)][*(ebuffer + 3)],
                        tempFile)
                    == EOF) {
                    MX_ERROR_THROW(err, MX_PS_FILE_ERROR);
                }
            }

            startBuffer += nrowBytes;
        }

        break;
    case 4:

        startBuffer = ((unsigned char*)tile) + originalByteOffset;

        for (irow = 0; irow < rowSize.y; irow++) {
            endBuffer = startBuffer + noutputBytes;
            buffer = startBuffer;

            while (buffer < endBuffer) {
                if (fputc(outputHexFromByte16[*buffer],
                        tempFile)
                    == EOF) {
                    MX_ERROR_THROW(err, MX_PS_FILE_ERROR);
                }
                buffer++;
            }

            if (extraBytes) {
                for (ibyte = 0; ibyte < extraBytes; ibyte++) {
                    ebuffer[ibyte] = *buffer++;
                }

                if (fputc(outputHexFromByte16[*ebuffer],
                        tempFile)
                    == EOF) {
                    MX_ERROR_THROW(err, MX_PS_FILE_ERROR);
                }

                if (fputc(outputHexFromByte16[*(ebuffer + 1)],
                        tempFile)
                    == EOF) {
                    MX_ERROR_THROW(err, MX_PS_FILE_ERROR);
                }
            }

            startBuffer += nrowBytes;
        }

        break;
    case 5:

        startBuffer = ((unsigned char*)tile) + originalByteOffset;

        for (irow = 0; irow < rowSize.y; irow++) {
            endBuffer = startBuffer + noutputBytes;
            buffer = startBuffer;

            while (buffer < endBuffer) {
                if (fputc(outputHexFromByte[*buffer][0], tempFile) == EOF) {
                    MX_ERROR_THROW(err, MX_PS_FILE_ERROR);
                }
                if (fputc(outputHexFromByte[*buffer][1], tempFile) == EOF) {
                    MX_ERROR_THROW(err, MX_PS_FILE_ERROR);
                }
                buffer++;
            }

            startBuffer += nrowBytes;
        }

        break;
    }
abort:;
}

/*-------------------------------------------------
 * FUNCTION: mx_pr_device::outputImageHeader
 *
 * DESCRIPTION:
 *
 *
 */

void mx_print_device::outputImageHeader(int& err,
    mx_point& topLeftOffset,
    mx_point& realSize,
    MX_DEVICE_RASTER deviceType,
    int nbits,
    int nplanes,
    int ncolours,
    mx_colour* colours,
    mx_ipoint& imageSize,
    bool clipSet)
{
    mx_doc_coord_t frameCoord;
    mx_point scale;
    int iplane;
    int icol;
    mx_colour* thisColour;
    int nsigcolours = 0;

    err = MX_ERROR_OK;

    if (!clipSet) {
        // save the graphics context
        if (fprintf(tempFile, "gsave\n") == EOF) {
            MX_ERROR_THROW(err, MX_PS_FILE_ERROR);
        }
    }

    if (ncolours > 0) {
        nsigcolours = (1 << nbits);

        if (fprintf(tempFile, "[/Indexed /DeviceRGB %d\n", nsigcolours - 1) == EOF) {
            MX_ERROR_THROW(err, MX_PS_FILE_ERROR);
        }
        if (fprintf(tempFile, "<") == EOF) {
            MX_ERROR_THROW(err, MX_PS_FILE_ERROR);
        }

        for (icol = 0; icol < ncolours; icol++) {
            if (fprintf(tempFile, " ") == EOF) {
                MX_ERROR_THROW(err, MX_PS_FILE_ERROR);
            }

            thisColour = colours + icol;

            if (fputc(outputHexFromByte[thisColour->red][0], tempFile) == EOF) {
                MX_ERROR_THROW(err, MX_PS_FILE_ERROR);
            }
            if (fputc(outputHexFromByte[thisColour->red][1], tempFile) == EOF) {
                MX_ERROR_THROW(err, MX_PS_FILE_ERROR);
            }
            if (fputc(outputHexFromByte[thisColour->green][0], tempFile) == EOF) {
                MX_ERROR_THROW(err, MX_PS_FILE_ERROR);
            }
            if (fputc(outputHexFromByte[thisColour->green][1], tempFile) == EOF) {
                MX_ERROR_THROW(err, MX_PS_FILE_ERROR);
            }
            if (fputc(outputHexFromByte[thisColour->blue][0], tempFile) == EOF) {
                MX_ERROR_THROW(err, MX_PS_FILE_ERROR);
            }
            if (fputc(outputHexFromByte[thisColour->blue][1], tempFile) == EOF) {
                MX_ERROR_THROW(err, MX_PS_FILE_ERROR);
            }
        }

        for (icol = ncolours; icol < nsigcolours; icol++) {
            if (fprintf(tempFile, " 000000") == EOF) {
                MX_ERROR_THROW(err, MX_PS_FILE_ERROR);
            }
        }

        if (fprintf(tempFile, ">\n") == EOF) {
            MX_ERROR_THROW(err, MX_PS_FILE_ERROR);
        }
        if (fprintf(tempFile, "\n") == EOF) {
            MX_ERROR_THROW(err, MX_PS_FILE_ERROR);
        }
        if (fprintf(tempFile, "] setcolorspace\n") == EOF) {
            MX_ERROR_THROW(err, MX_PS_FILE_ERROR);
        }
    } else if ((deviceType == MX_DEVICE_GREY) || (deviceType == MX_DEVICE_BIT)) {
        if (fprintf(tempFile, "/DeviceGray setcolorspace\n") == EOF) {
            MX_ERROR_THROW(err, MX_PS_FILE_ERROR);
        }
    } else if (deviceType == MX_DEVICE_RGB) {
        if (fprintf(tempFile, "/DeviceRGB setcolorspace\n") == EOF) {
            MX_ERROR_THROW(err, MX_PS_FILE_ERROR);
        }
    }

    // alter transformation to get the lower
    // left hand coordinate

    // first scale the unit square -
    // see Postscript Language reference manual
    // p 217

    frameCoord.p.x = GMAX(0, topLeftOffset.x);
    frameCoord.p.y = realSize.y + GMAX(0, topLeftOffset.y);

    frameCoord = frameTarget2dev(err, frameCoord);
    MX_ERROR_CHECK(err);

    // map unit square to real size
    scale.x = frame2dev(realSize.x);
    scale.y = frame2dev(realSize.y);

    if (fprintf(tempFile, "%.6f %.6f translate\n", frameCoord.p.x, -frameCoord.p.y) == EOF) {
        MX_ERROR_THROW(err, MX_PS_FILE_ERROR);
    }
    if (fprintf(tempFile, "%.6f %.6f scale\n", scale.x, scale.y) == EOF) {
        MX_ERROR_THROW(err, MX_PS_FILE_ERROR);
    }

    // now put out image dictionary

    if (fprintf(tempFile, "<<\n") == EOF) {
        MX_ERROR_THROW(err, MX_PS_FILE_ERROR);
    }
    if (fprintf(tempFile, "/ImageType 1\n") == EOF) {
        MX_ERROR_THROW(err, MX_PS_FILE_ERROR);
    }
    if (fprintf(tempFile, "/Width %d /Height %d\n", imageSize.x, imageSize.y) == EOF) {
        MX_ERROR_THROW(err, MX_PS_FILE_ERROR);
    }
    if (fprintf(tempFile, "/BitsPerComponent %d\n", nbits) == EOF) {
        MX_ERROR_THROW(err, MX_PS_FILE_ERROR);
    }
    if (fprintf(tempFile, "/Decode [") == EOF) {
        MX_ERROR_THROW(err, MX_PS_FILE_ERROR);
    }

    if (ncolours == 0) {
        for (iplane = 0; iplane < nplanes; iplane++) {
            if (fprintf(tempFile, " 0 1") == EOF) {
                MX_ERROR_THROW(err, MX_PS_FILE_ERROR);
            }
        }
    } else {
        if (fprintf(tempFile, " 0 %d", nsigcolours - 1) == EOF) {
            MX_ERROR_THROW(err, MX_PS_FILE_ERROR);
        }
    }

    if (fprintf(tempFile, " ]\n") == EOF) {
        MX_ERROR_THROW(err, MX_PS_FILE_ERROR);
    }
    if (fprintf(tempFile, "/ImageMatrix [ %d 0 0 %d 0 %d ]\n",
            imageSize.x,
            -imageSize.y,
            imageSize.y)
        == EOF) {
        MX_ERROR_THROW(err, MX_PS_FILE_ERROR);
    }

    if (fprintf(tempFile, "/DataSource currentfile /ASCIIHexDecode filter\n") == EOF) {
        MX_ERROR_THROW(err, MX_PS_FILE_ERROR);
    }
    if (fprintf(tempFile, ">>\n") == EOF) {
        MX_ERROR_THROW(err, MX_PS_FILE_ERROR);
    }
    if (fprintf(tempFile, "image\n") == EOF) {
        MX_ERROR_THROW(err, MX_PS_FILE_ERROR);
    }
    if (fprintf(tempFile, "\n") == EOF) {
        MX_ERROR_THROW(err, MX_PS_FILE_ERROR);
    }

    return;
abort:
    return;
}

/*-------------------------------------------------
 * FUNCTION: mx_pr_device::outputImageTrailer
 *
 * DESCRIPTION:
 *
 *
 */

void mx_print_device::outputImageTrailer(int& err,
    bool clipSet)
{
    err = MX_ERROR_OK;

    // new line
    if (fprintf(tempFile, "\n") == EOF) {
        MX_ERROR_THROW(err, MX_PS_FILE_ERROR);
    }

    // restore the old graphics context

    if (!clipSet) {
        // save the graphics context
        if (fprintf(tempFile, "grestore\n") == EOF) {
            MX_ERROR_THROW(err, MX_PS_FILE_ERROR);
        }
    }
abort:;
}
