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
 * MODULE : mx_sc_clip.C
 *
 * AUTHOR : David Miller
 *
 * This file
 *
 * DESCRIPTION: Clip routines on the screen device
 * Module mx_sc_clip.C
 *
 *
 *
 */

#include "mx_sc_device.h"

/*-------------------------------------------------
 * FUNCTION: mx_screen_device::popClipMask
 *
 * DESCRIPTION: Pop a clip mask
 *
 *
 */

void mx_screen_device::popClipMask(int& err)
{
    XGCValues xgcv;

    err = MX_ERROR_OK;

    if (clipStackOffset == 0)
        return;

    if (clipStackOffset == 1) {
        // no more clips

        xgcv.clip_mask = None;

        // reset GC
        XChangeGC(display, gc, GCClipMask, &xgcv);
    } else {
        XSetRegion(display, gc, clipStack[clipStackOffset - 2]);
    }

    // delete top region
    clipStackOffset--;
    XDestroyRegion(clipStack[clipStackOffset]);

    return;
}

/*-------------------------------------------------
 * FUNCTION: mx_screen_device::pushClipMask
 *
 * DESCRIPTION: Push a region onto a clip mask
 *
 *
 */

void mx_screen_device::pushClipMask(int& err,
    Region newRegion,
    bool doIntersect)
{
    Region iregion, currentRegion;

    err = MX_ERROR_OK;

    if (clipStackOffset == 0) {
        /* first on stack - just use */
        iregion = newRegion;
    } else {
        /* add to stack */
        iregion = XCreateRegion();
        currentRegion = clipStack[clipStackOffset - 1];

        if (doIntersect) {
            XIntersectRegion(currentRegion, newRegion, iregion);
        } else {
            XUnionRegion(currentRegion, newRegion, iregion);
        }

        XDestroyRegion(newRegion);
    }

    /* need to increase stack size */
    if (clipStackOffset == clipStackSize) {
        Region* oldClipStack = clipStack;

        clipStackSize += 4;

        clipStack = new Region[clipStackSize];

        memcpy(clipStack, oldClipStack, sizeof(Region) * clipStackSize);

        delete[] oldClipStack;
    }

    clipStack[clipStackOffset] = iregion;
    clipStackOffset++;

    /* always add */
    XSetRegion(display, gc, iregion);

    return;
}

/*-------------------------------------------------
 * FUNCTION: mx_screen_device::pushClipPolypoint
 *
 * DESCRIPTION: Clip output to a polygon. Return a pointer
 *              to the region so that clipping can
 *              be turned off
 */

void mx_screen_device::pushClipPolypoint(int& err,
    const mx_ipolypoint& pp,
    bool doIntersect)
{
    Region region;
    int maxPoints;
    int npoints;

    err = MX_ERROR_OK;

    // get X maximum of points in one go
    maxPoints = (getMaxRequests() / 2) - 3;

    npoints = pp.get_num_points();

    if (npoints > maxPoints) {
        MX_ERROR_THROW(err, MX_XNPOINTS_FAILED);
    }

    polypointToXPoints(err, pp);
    MX_ERROR_CHECK(err);

    region = XPolygonRegion(xpoints, npoints, EvenOddRule);

    pushClipMask(err, region, doIntersect);
    MX_ERROR_CHECK(err);

    return;

abort:
    return;
}

/*-------------------------------------------------
 * FUNCTION: mx_screen_device::pushClipRect
 *
 * DESCRIPTION: Clip output to a rectanble. Return a pointer
 *              to the region so that clipping can
 *              be turned off
 */

void mx_screen_device::pushClipRect(int& err,
    const mx_irect& coord,
    const mx_angle& angle,
    bool doIntersect)
{
    Region region = XCreateRegion();
    XRectangle rectangle;

    err = MX_ERROR_OK;

    rectangle.x = coord.xb;
    rectangle.y = coord.yb;
    rectangle.width = coord.xt - coord.xb + 1;
    rectangle.height = coord.yt - coord.yb + 1;

    XUnionRectWithRegion(&rectangle, region, region);

    pushClipMask(err, region, doIntersect);
    MX_ERROR_CHECK(err);

    return;

abort:
    return;
}
