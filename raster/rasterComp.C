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
 * MODULE : rasterComp.C
 *
 * AUTHOR : David Miller
 *
 * DESCRIPTION:
 * Module rasterComp.C
 *
 *
 */

#include "raster.h"
extern "C" {
#include "zlib.h"
}

#define MIND(x, y) ((x) < (y) ? (x) : (y))

/*-------------------------------------------------
 * FUNCTION: findRemoveValue
 *
 * DESCRIPTION: Find the value to remove from a
 *              raster
 *              Return the compresses size
 */

static int findRemoveValue(unsigned char* uncompressed,
    int nbytes,
    unsigned char& value)
{
    unsigned char* enduncompressed;
    int counts[256];
    int indexSize = (nbytes + 7) / 8;
    int icount, maxcount;

    // Initialise counts to zero
    memset(counts, 0, sizeof(counts));

    // Set end counter
    enduncompressed = uncompressed + nbytes;

    // Loop through the data getting the count
    while (uncompressed < enduncompressed)
        (counts[*uncompressed++])++;

    // Now find the most common value

    icount = 0;
    maxcount = 0;

    for (icount = 0; icount < 256; icount++) {
        if (counts[icount] > maxcount) {
            maxcount = counts[icount];
            value = icount;
        }
    }

    if (maxcount == nbytes) {
        // All the same
        return 1;
    } else {
        // if no compression return nbytes
        return MIND(nbytes, indexSize + 1 + nbytes - maxcount);
    }
}

/*-------------------------------------------------
 * FUNCTION: removeValueComp
 *
 * DESCRIPTION:
 *
 *
 */

int removeValueComp(int& err,
    unsigned char* uncompressed,
    int nbytes,
    unsigned char* compressed,
    void* data)
{
    unsigned char value;
    int compressedSize;
    int indexSize;
    unsigned char* indexptr;
    unsigned char* enduncompressed;
    int offset;

    err = MX_ERROR_OK;

    // find the most common value
    compressedSize = findRemoveValue(uncompressed, nbytes, value);

    if (compressedSize < nbytes) {
        // Tile does compress
        // Always store the common value
        // If the tile is all one value this is all that is stored

        *compressed++ = value;

        if (compressedSize != 1) {
            indexSize = (nbytes + 7) / 8;

            // Set up index
            indexptr = compressed;
            memset(indexptr, 0, indexSize);

            // set up output data
            compressed += indexSize;

            enduncompressed = uncompressed + nbytes;

            offset = 0;
            while (uncompressed < enduncompressed) {
                // Not the value - set bit to mark this
                // and store the value
                if (*uncompressed != value) {
                    *indexptr |= (1 << offset);
                    *compressed++ = *uncompressed;
                }

                // Onto next byte
                uncompressed++;
                if (offset == 7) {
                    offset = 0;
                    indexptr++;
                } else {
                    offset++;
                }
            }
        }
    }

    return compressedSize;
}

/*-------------------------------------------------
 * FUNCTION: removeValueDecomp
 *
 * DESCRIPTION:
 *
 *
 */

void removeValueDecomp(int& err,
    unsigned char* uncompressed,
    int nbytes,
    unsigned char* compressed,
    int nbytesCompressed,
    void* data)
{
    unsigned char* enduncompressed;
    unsigned char* indexptr;
    int offset, indexSize;
    unsigned char value = *compressed++;

    err = MX_ERROR_OK;

    if (nbytesCompressed == 1) {
        // all the same value
        memset(uncompressed, value, nbytes);
    } else {
        // set up data and index pointers
        enduncompressed = uncompressed + nbytes;
        indexSize = (nbytes + 7) / 8;

        indexptr = compressed;
        compressed += indexSize;

        offset = 0;
        while (uncompressed < enduncompressed) {
            if (*indexptr & (1 << offset)) {
                // Not the set value - get from input data stream
                *uncompressed++ = *compressed++;
            } else {
                // Is the value
                *uncompressed++ = value;
            }

            if (offset == 7) {
                indexptr++;
                offset = 0;
            } else {
                offset++;
            }
        }
    }
}

/*-------------------------------------------------
 * FUNCTION: rasterunPackbits
 *
 * DESCRIPTION:
 *
 *
 */

void rasterunPackbits(int& err,
    unsigned char* uncompressed,
    int nbytes,
    unsigned char* compressed,
    int nbytesCompressed,
    void* data)
{
    unsigned char* enduncompressed = uncompressed + nbytes;
    unsigned char* endcompressed = compressed + nbytesCompressed;
    unsigned char runValue;
    unsigned char* newuncompressed;

    err = MX_ERROR_OK;

    while (compressed < endcompressed) {
        // Get the runlength
        runValue = *compressed++;

        if (runValue >= 128) {
            // literal run
            runValue = runValue - 127;

            newuncompressed = uncompressed + runValue;
            MX_ERROR_ASSERT(err, newuncompressed <= enduncompressed);

            memcpy(uncompressed, compressed, runValue);
            compressed += runValue;
        } else {
            // replicated run
            runValue++;

            newuncompressed = uncompressed + runValue;
            MX_ERROR_ASSERT(err, newuncompressed <= enduncompressed);

            memset(uncompressed, *compressed++, runValue);
        }
        uncompressed = newuncompressed;
    }
    return;
abort:
    return;
}

/*-------------------------------------------------
 * FUNCTION: rasterPackbits
 *
 * DESCRIPTION:
 *
 *
 */

int rasterPackbits(int& err,
    unsigned char* uncompressed,
    int nbytes,
    unsigned char* compressed,
    void* data)
{
    unsigned char* enduncompressed = uncompressed + nbytes;
    unsigned char* enduncompressedm1 = enduncompressed - 1;
    int outbytes = 0;
    unsigned char lastValue;
    int nbytesLeft;

    err = MX_ERROR_OK;

    // Set up default of being on a zero length literal run
    bool doingRun = FALSE;
    int runLength = 0;
    unsigned char* startLiteral = uncompressed;

    while (uncompressed < enduncompressed) {
        // Here we are either starting a replicated run
        // or wondering if one should be started because
        // two consecutive values have been found or we
        // are starting a brand new run

        if (doingRun) {
            // Definite new run

            // Output the old literal run if required
            if (runLength > 0) {
                outbytes += (1 + runLength);
                if (outbytes >= nbytes)
                    return nbytes;

                *compressed++ = runLength + 127;
                memcpy(compressed, startLiteral, runLength);
                compressed += runLength;
                runLength = 0;
            }

            // At start of a replicated run of at least three
            lastValue = *uncompressed;
            uncompressed += 3;
            runLength = 3;

            // Do the whole run
            while ((uncompressed < enduncompressed) && (runLength < 128)
                && (*uncompressed == lastValue)) {
                runLength++;
                uncompressed++;
            }

            // output a replicated run
            outbytes += 2;
            if (outbytes >= nbytes)
                return nbytes;
            *compressed++ = (runLength - 1);
            *compressed++ = lastValue;

            // Set a zero length literal runlength and go back to
            // look for run
            doingRun = FALSE;
            startLiteral = uncompressed;
            runLength = 0;
        } else {
            // Possibly not on a replicated run - here we are looking
            // to either start an existing replicated run and if not continue an
            // existing (possibly zero length) literal run

            // Are there enough pixels for a replicated run
            nbytesLeft = (enduncompressed - uncompressed);
            if (nbytesLeft < 3) {
                // Output a literal run
                runLength += nbytesLeft;

                // Might need two literal runs
                if (runLength > 128) {
                    // output first part of the literal
                    outbytes += 129;
                    if (outbytes >= nbytes)
                        return nbytes;

                    *compressed++ = 255;
                    memcpy(compressed, startLiteral, 128);
                    startLiteral += 128;
                    compressed += 128;
                    runLength -= 128;
                }

                // output main literal run
                outbytes += (1 + runLength);
                if (outbytes >= nbytes)
                    return nbytes;

                *compressed++ = runLength + 127;
                memcpy(compressed, startLiteral, runLength);
                compressed += runLength;

                return outbytes;
            } else {
                // Are we on a literal

                doingRun = ((*uncompressed == *(uncompressed + 1)) && (*uncompressed == *(uncompressed + 2)));
                if (!doingRun) {
                    // on a literal run - possibly extended from an earlier
                    // literal run which had two consecutive bytes the same

                    // Loop while not one from the end - runlenght is not
                    // too long and there are not two consecutive values the
                    // same
                    do {
                        runLength++;
                        uncompressed++;
                        lastValue = *uncompressed;
                    } while ((uncompressed < enduncompressedm1) && (runLength < 128)
                        && (lastValue != *(uncompressed + 1)));

                    // If runlength is too long - output the run
                    if (runLength == 128) {
                        outbytes += (1 + runLength);
                        if (outbytes >= nbytes)
                            return nbytes;

                        *compressed++ = runLength + 127;
                        memcpy(compressed, startLiteral, runLength);
                        compressed += runLength;

                        runLength = 0;
                        startLiteral = uncompressed;
                    }
                }
            }
        }
    }

    // Literal run left at the end
    if (runLength > 0) {
        outbytes += (1 + runLength);
        if (outbytes >= nbytes)
            return nbytes;

        *compressed++ = runLength + 127;
        memcpy(compressed, startLiteral, runLength);
    }

    return outbytes;
}

/*-------------------------------------------------
 * FUNCTION: rasterZuncomp
 *
 * DESCRIPTION:
 *
 *
 */

void rasterZuncomp(int& err,
    unsigned char* uncompressed,
    int nbytes,
    unsigned char* compressed,
    int nbytesCompressed,
    void* data)
{
    uLong testSize = nbytes;
    int ans;

    err = MX_ERROR_OK;

    ans = uncompress(uncompressed, &testSize,
        compressed, nbytesCompressed);

    if ((ans != Z_OK) || (testSize != (uLong)nbytes)) {
        MX_ERROR_THROW(err, MX_COMPRESS_ERROR_RASTER);
    }

    return;
abort:
    return;
}

/*-------------------------------------------------
 * FUNCTION: rasterZcomp
 *
 * DESCRIPTION:
 *
 *
 */

int rasterZcomp(int& err,
    unsigned char* uncompressed,
    int nbytes,
    unsigned char* compressed,
    void* data)
{
    uLong testSize;
    int ans;

    err = MX_ERROR_OK;

    testSize = nbytes;

    ans = compress(compressed, &testSize,
        uncompressed, nbytes);

    if (ans == Z_BUF_ERROR) {
        testSize = nbytes;
    } else if (ans != Z_OK) {
        MX_ERROR_THROW(err, MX_COMPRESS_ERROR_RASTER);
    }

    return testSize;
abort:
    return 0;
}
