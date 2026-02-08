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
 * MODULE : mx_sc_sty.C
 *
 * AUTHOR : David Miller
 *
 * This file
 *
 * DESCRIPTION:
 * Module mx_sc_sty.C
 *
 *
 *
 *
 */

#include "mx_sc_device.h"

/*-------------------------------------------------
 * FUNCTION: mx_screen_device::setLineStyle
 *
 * DESCRIPTION:
 *
 *
 */

void mx_screen_device::setDefaultLineStyle(int& err,
    mx_line_style& s)
{
    float dashList[2];
    int ndashes;

    // set up x line styles
    static int joinStyles[mx_njoin_types] = { JoinBevel, JoinRound, JoinMiter };
    static int capStyles[mx_ncap_types] = { CapButt, CapRound, CapProjecting, CapNotLast };

    unsigned int pixWidth;

    setForegroundColour(err, s.colour);
    MX_ERROR_CHECK(err);

    // get line width - allow to be zero
    pixWidth = (unsigned int)(frame2dev(s.width) + 0.5);
    extendWidth = pixWidth + 1;

    switch (s.line_type) {
    case mx_solid:
        XSetLineAttributes(display, gc, pixWidth, LineSolid, capStyles[s.cap_type], joinStyles[s.join_type]);
        break;
    case mx_dotted:
        XSetLineAttributes(display, gc, pixWidth, LineSolid, capStyles[s.cap_type], joinStyles[s.join_type]);
        ndashes = 2;
        dashList[0] = 0.5;
        dashList[1] = 0.5;
        setDashStyle(err, ndashes, dashList, 0.0);
        MX_ERROR_CHECK(err);
        break;
    case mx_dashed:
        XSetLineAttributes(display, gc, pixWidth, LineSolid, capStyles[s.cap_type], joinStyles[s.join_type]);
        ndashes = 2;
        dashList[0] = 3.0;
        dashList[1] = 1.0;
        setDashStyle(err, ndashes, dashList, 0.0);
        MX_ERROR_CHECK(err);
        break;
    case mx_custom_line:
        XSetLineAttributes(display, gc, pixWidth, LineSolid, capStyles[s.cap_type], joinStyles[s.join_type]);
        setDashStyle(err,
            s.dash_style.get_ndashes(),
            s.dash_style.get_dashes(),
            s.dash_style.get_dash_offset());
        MX_ERROR_CHECK(err);
        break;
    default:
    case mx_no_line:
        return;
    }
abort:;
}

/*-------------------------------------------------
 * FUNCTION: mx_screen_device::setDashStyle
 *
 * DESCRIPTION:
 *
 *
 */

void mx_screen_device::setDashStyle(int& err,
    int ndashes,
    float* dashes,
    float dashOffset)
{
    float *startDash, *endDash, startLength;
    int startPixel, endPixel, nPixels;
    int idash;
    bool nonZero;
    int *inputDash, *endInputDash, *outputDash;
    bool doingInputDash, doingOutputDash;
    int currentLength;
    int* dashPix;
    int thisLength;
    XGCValues xgcv;

    err = MX_ERROR_OK;

    if (ndashes == 0) {
        xgcv.line_style = LineSolid;

        XChangeGC(display, gc, GCLineStyle, &xgcv);
        return;
    }

    // Dont always do dashing the X way

    nusedDashes = 0;
    useXDash = TRUE;

    // get memory to hold pixel converted dashes

    if (ndashes > nallocDashLengths) {
        if (dashLengths != NULL)
            delete[] dashLengths;
        dashLengths = new int[ndashes];

        // unsigned char lengths for X
        if (dashLengths != NULL)
            delete[] cdashLengths;
        cdashLengths = new unsigned char[ndashes];

        nallocDashLengths = ndashes;
    }

    // set dash Offset
    pixDashOffset = (int)(frame2dev(dashOffset) + 0.5);

    // first pass - get the lengths of the dashes in pixels

    startDash = dashes;
    endDash = dashes + ndashes;
    startLength = 0.0;
    dashPix = dashLengths;
    nonZero = TRUE;

    while (startDash < endDash) {
        startPixel = (int)(frame2dev(startLength) + 0.5);
        startLength += *startDash++;
        endPixel = (int)(frame2dev(startLength) + 0.5);
        nPixels = endPixel - startPixel;
        *dashPix++ = nPixels;
        nonZero = nonZero && (nPixels != 0);
    }

    // second pass - remove 0 length dashes
    if (nonZero) {
        nusedDashes = ndashes;
    } else {
        // set up pointers into pixel length array - we will
        // overwrite some values if zero lengths are found

        inputDash = dashLengths;
        endInputDash = dashLengths + ndashes;
        outputDash = dashLengths;

        // set dash-gap in phase initially
        doingInputDash = TRUE;
        doingOutputDash = TRUE;

        // save current pixel run length
        currentLength = 0;

        while (inputDash < endInputDash) {
            thisLength = *inputDash++;
            if (thisLength != 0) {
                // non-zero length

                if (doingInputDash == doingOutputDash) {
                    // doing the same dash-gap type - add
                    currentLength += thisLength;
                } else {
                    // different dash-gap type
                    // output last type if non-zero length
                    if (currentLength != 0) {
                        *outputDash++ = currentLength;
                        nusedDashes++;

                        // change output phase
                        doingOutputDash = !doingOutputDash;
                        currentLength = 0;
                    }
                }
            }

            // always change input phase
            doingInputDash = !doingInputDash;
        }

        // output last length
        if (currentLength != 0) {
            *outputDash++ = currentLength;
            nusedDashes++;
        }
    }

    // must have at least two dash lengths - else put out solid line
    // which is the default setup

    if (nusedDashes < 2) {
        nusedDashes = 0;
        return;
    }

    // all dashlengths must be less than 255 if X is
    // to be used to draw dashed lines  - pretty crap
    // of X !!

    for (idash = 0; idash < nusedDashes; idash++) {
        useXDash = useXDash && (dashLengths[idash] < 255);
    }

    if (!useXDash) {
        MX_ERROR_THROW(err, MX_XDASHES_FAILED);
    }

    xgcv.line_style = LineOnOffDash;

    // accelerated setting
    if ((nusedDashes == 2) && (dashLengths[0] == dashLengths[1])) {

        xgcv.dashes = dashLengths[0];
        xgcv.dash_offset = pixDashOffset;
        XChangeGC(display, gc, GCDashOffset | GCDashList | GCLineStyle, &xgcv);
    } else {
        // use the allocated unsigned char memory - it's not sure from  X
        // documentation if we need to allocate here !!
        // better be safe

        // change to dotted GC
        XChangeGC(display, gc, GCLineStyle, &xgcv);

        for (idash = 0; idash < nusedDashes; idash++)
            cdashLengths[idash] = dashLengths[idash];

        XSetDashes(display, gc, pixDashOffset, (char*)cdashLengths, nusedDashes);
    }

    return;
abort:
    return;
}

/*-------------------------------------------------
 * FUNCTION: mx_screen_device::setFillStyle
 *
 * DESCRIPTION:
 *
 *
 */

void mx_screen_device::setDefaultFillStyle(int& err,
    mx_fill_style& s)
{
    if (s.type == mx_fill_colour) {
        setForegroundColour(err, s.colour);
        MX_ERROR_CHECK(err);
    }

abort:;
}
