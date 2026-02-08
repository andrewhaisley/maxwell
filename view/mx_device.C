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
 * MODULE : mx_device.C
 *
 * AUTHOR : David Miller
 *
 * This file
 *
 * DESCRIPTION:
 * Module mx_device.C
 *
 *
 *
 */

#include <mx.h>
#include <mx_device.h>
#include <mx_frame.h>
#include <mx_geometry.h>

mx_line_style defaultLineStyle(mx_solid, 0.0);
mx_fill_style defaultFillStyle;
mx_ipolypoint mx_device::ippline;
mx_polypoint mx_device::ppline;
mx_doc_polypoint mx_device::docppline;

/*-------------------------------------------------
 * FUNCTION: mx_device::mx_device
 *
 * DESCRIPTION:
 *
 *
 */

mx_device::mx_device(mx_frame& iframe)
    : frame(iframe)
{
    doesRGB = FALSE;
    doesLUT = TRUE;
    doesBit = TRUE;

    drawsChord = FALSE;
    drawsSector = FALSE;
    fillsSector = FALSE;

    drawsBeziers = FALSE;
    fillsBeziers = FALSE;

    clipsArc = FALSE;
    clipsSector = FALSE;

    closesLinesWithoutLastPoint = FALSE;

    doesRotate = FALSE;
    doesAffine = FALSE;
}

/*-------------------------------------------------
 * FUNCTION: mx_device::~mx_device
 *
 * DESCRIPTION:
 *
 *
 */

mx_device::~mx_device()
{
}

/*-------------------------------------------------
 * FUNCTION: mx_device::dev2frame
 *
 * DESCRIPTION:
 *
 *
 */

mx_doc_coord_t mx_device::dev2frame(const mx_doc_coord_t& devPoint)
{
    mx_doc_coord_t retPoint;

    retPoint.sheet_number = 0;

    // convert from pixels to mm
    retPoint.p = devPoint.p / (frame.getZoom() * screenResolution);

    // add on origin

    retPoint.p += frame.getTopLeft();

    if (frame.getFalseOriginSet()) {
        retPoint.p -= frame.getFalseOrigin();
    }

    return retPoint;
}

/*-------------------------------------------------
 * FUNCTION: mx_device::dev2frame
 *
 * DESCRIPTION:
 *
 *
 */

double mx_device::dev2frame(double devLength)
{
    return devLength / (frame.getZoom() * screenResolution);
}

/*-------------------------------------------------
 * FUNCTION: mx_device::frame2dev
 *
 * DESCRIPTION:
 *
 *
 */

double mx_device::frame2dev(double frameLength)
{
    return frameLength * frame.getZoom() * screenResolution;
}

/*-------------------------------------------------
 * FUNCTION: mx_device::frame2dev
 *
 * DESCRIPTION:
 *
 *
 */

mx_doc_coord_t mx_device::frame2dev(const mx_doc_coord_t& framePoint)
{
    mx_doc_coord_t retPoint;

    retPoint.sheet_number = 0;
    retPoint.p = framePoint.p - frame.getTopLeft();

    // if there is a false origin then use this to adjust
    // the display
    if (frame.getFalseOriginSet()) {
        retPoint.p += frame.getFalseOrigin();
    }

    // convert from pixels to mm
    retPoint.p = retPoint.p * frame.getZoom() * screenResolution;

    return retPoint;
}

/*-------------------------------------------------
 * FUNCTION: mx_device::dev2frameTarget
 *
 * DESCRIPTION:
 *
 *
 */

mx_doc_coord_t mx_device::dev2frameTarget(int& err,
    const mx_doc_coord_t& devPoint)
{
    mx_doc_coord_t frameTargetCoord;
    mx_doc_coord_t frameCoord;

    err = MX_ERROR_OK;

    frameCoord = dev2frame(devPoint);

    frame.coordToDocCoord(err, frameCoord.p, frameTargetCoord, MX_COORD_C);
    MX_ERROR_CHECK(err);

    return frameTargetCoord;
abort:
    return frameTargetCoord;
}

/*-------------------------------------------------
 * FUNCTION: mx_device::frameTarget2dev
 *
 * DESCRIPTION:
 *
 *
 */

mx_doc_coord_t mx_device::frameTarget2dev(int& err,
    const mx_doc_coord_t& frameTargetCoord)
{
    mx_doc_coord_t frameCoord;

    err = MX_ERROR_OK;

    frameCoord.sheet_number = 0;
    frame.docCoordToCoord(err, frameTargetCoord, frameCoord.p);
    MX_ERROR_CHECK(err);

    return frame2dev(frameCoord);
abort:
    return frameCoord;
}

/*-------------------------------------------------
 * FUNCTION: mx_device:pixelSnap
 *
 * DESCRIPTION: Convert a frame coordinate to a
 * frame coordinate based at the nearest pixel
 *
 */

mx_ipoint mx_device::pixelSnap(const mx_doc_coord_t& deviceCoord)
{
    mx_ipoint snappedCoord;

    // round to pixels
    if (deviceCoord.p.x > 0) {
        snappedCoord.x = (int)(deviceCoord.p.x + 0.5);
    } else {
        snappedCoord.x = (int)(deviceCoord.p.x - 0.5);
    }

    if (deviceCoord.p.y > 0) {
        snappedCoord.y = (int)(deviceCoord.p.y + 0.5);
    } else {
        snappedCoord.y = (int)(deviceCoord.p.y - 0.5);
    }

    return snappedCoord;
}

/*-------------------------------------------------
 * FUNCTION: mx_device:pixelSnapFrame
 *
 * DESCRIPTION: Convert a frame coordinate to a
 * frame coordinate based at the nearest pixel
 *
 */

mx_doc_coord_t mx_device::pixelSnapFrame(const mx_doc_coord_t& position)

{
    mx_doc_coord_t pixelSnappedPosition;

    // convert to a device position
    mx_doc_coord_t deviceCoord = frame2dev(position);

    // snap the pixels
    deviceCoord.p = pixelSnap(deviceCoord);

    // convert the rounded value back to a frame position
    pixelSnappedPosition = dev2frame(deviceCoord);

    return pixelSnappedPosition;
}

/*-------------------------------------------------
 * FUNCTION: mx_device:pixelSnapFrameTarget
 *
 * DESCRIPTION: Convert a frame coordinate to a
 * frame coordinate based at the nearest pixel
 *
 */

mx_doc_coord_t mx_device::pixelSnapFrameTarget(int& err,
    const mx_doc_coord_t& position)
{
    mx_doc_coord_t frameCoord;
    mx_doc_coord_t snappedFrameCoord;

    err = MX_ERROR_OK;

    frameCoord.sheet_number = 0;
    frame.docCoordToCoord(err, position, frameCoord.p);
    MX_ERROR_CHECK(err);

    snappedFrameCoord = pixelSnapFrame(frameCoord);

    frame.coordToDocCoord(err, snappedFrameCoord.p, frameCoord, MX_COORD_C);
    MX_ERROR_CHECK(err);

abort:;
    return frameCoord;
}

/*-------------------------------------------------
 * FUNCTION: mx_device:getFrameSize
 *
 * DESCRIPTION: Convert a frame coordinate to a
 * frame coordinate based at the nearest pixel
 *
 */

mx_point mx_device::getFrameSize()
{
    mx_doc_coord_t devCoord;
    mx_doc_coord_t frameCoord;

    devCoord.sheet_number = 0;
    devCoord.p = getDevicePixelSize();

    frameCoord = dev2frame(devCoord);

    return (frameCoord.p - frame.getTopLeft());
}

/*-------------------------------------------------
 * FUNCTION: mx_device:frameResolution
 *
 * DESCRIPTION: Get pixel resolution, including frame
 * zoom
 *
 */

double mx_device::frameResolution()
{
    return screenResolution * frame.getZoom();
}
