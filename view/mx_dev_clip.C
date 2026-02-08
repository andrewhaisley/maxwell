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
 * MODULE : mx_dev_clip.C
 *
 * AUTHOR : David Miller
 *
 * This file
 *
 * DESCRIPTION:
 * Module mx_dev_clip.C
 *
 *
 * Revision 1.1  1996/10/06 13:15:45  andrew
 * Initial revision
 *
 *
 */

#include "mx_device.h"
#include <mx.h>
#include <mx_frame.h>

/*-------------------------------------------------
 * FUNCTION: mx_device::pushClipPolypoint
 *
 * DESCRIPTION:
 *
 *
 */

void mx_device::pushClipPolypoint(int& err,
    mx_doc_polypoint& pp,
    bool doIntersect)
{
    err = MX_ERROR_OK;

    // dont close
    translatePolypoint(err,
        pp,
        FALSE,
        ippline);
    MX_ERROR_CHECK(err);

    pushClipPolypoint(err, ippline, doIntersect);
    MX_ERROR_CHECK(err);

    return;
abort:
    return;
}

/*-------------------------------------------------
 * FUNCTION: mx_device::pushClipRect
 *
 * DESCRIPTION:
 *
 *
 */

void mx_device::pushClipRect(int& err,
    const mx_doc_coord_t& topLeft,
    const mx_doc_coord_t& bottomRight,
    const mx_angle& angle,
    bool doIntersect)
{
    err = MX_ERROR_OK;

    if ((angle.a() == 0) || doesRotate) {
        mx_doc_coord_t ptopLeft;
        mx_doc_coord_t pbottomRight;
        mx_ipoint pptopLeft;
        mx_ipoint ppbottomRight;
        mx_irect pprect;

        ptopLeft = frameTarget2dev(err, topLeft);
        MX_ERROR_CHECK(err);

        pbottomRight = frameTarget2dev(err, bottomRight);
        MX_ERROR_CHECK(err);

        pptopLeft = pixelSnap(ptopLeft);
        ppbottomRight = pixelSnap(pbottomRight);

        pprect.xb = pptopLeft.x;
        pprect.yb = pptopLeft.y;
        pprect.xt = ppbottomRight.x;
        pprect.yt = ppbottomRight.y;

        pushClipRect(err, pprect, angle, doIntersect);
        MX_ERROR_CHECK(err);
    } else {
        // need to convert to a polypoint

        mx_rect r(topLeft.p, bottomRight.p, FALSE, FALSE, angle.a());

        r.rectToPolypoint(FALSE, docppline.pp);

        pushClipPolypoint(err, docppline, doIntersect);
        MX_ERROR_CHECK(err);
    }

    return;
abort:
    return;
}

/*-------------------------------------------------
 * FUNCTION: mx_device::pushClipArc
 *
 * DESCRIPTION:
 *
 *
 */

void mx_device::pushClipArc(int& err,
    mx_doc_arc& a,
    bool doIntersect)
{
    double tolerance;

    err = MX_ERROR_OK;

    if (clipsArc && ((a.arc.gan().a() == 0) || doesRotate)) {
        mx_ipoint ppcentre, ppsize;

        arcPoints(err, a, ppcentre, ppsize);
        MX_ERROR_CHECK(err);

        auto aa = a.arc.gan();

        pclipArc(
            err,
            ppcentre,
            ppsize,
            a.arc.isFull(),
            a.arc.gsa(),
            a.arc.gea(),
            aa,
            doIntersect);

        MX_ERROR_CHECK(err);
    } else {
        // device doesnt draw a sector - we need to draw a
        // polypoint instead

        // approximate to half a pixel in the frame

        tolerance = 0.5 / frameResolution();

        // convert the arc to a polypoint -  let closure
        // be done by the draw polypoint routine

        a.arc.arcToBoundingPolypoint(tolerance, docppline.pp);

        // now convert polypoint points to ipolypoint points

        pushClipPolypoint(err, docppline, doIntersect);
        MX_ERROR_CHECK(err);
    }
abort:;
}

/*-------------------------------------------------
 * FUNCTION: mx_device::pushClipSector
 *
 * DESCRIPTION:
 *
 *
 */

void mx_device::pushClipSector(int& err,
    mx_doc_arc& a,
    bool doIntersect)
{
    double tolerance;

    err = MX_ERROR_OK;

    if (clipsSector && ((a.arc.gan().a() == 0) || doesRotate)) {
        mx_ipoint ppcentre, ppsize;

        arcPoints(err, a, ppcentre, ppsize);
        MX_ERROR_CHECK(err);

        auto aa = a.arc.gan();

        pclipSector(
            err,
            ppcentre,
            ppsize,
            a.arc.isFull(),
            a.arc.gsa(),
            a.arc.gea(),
            aa,
            doIntersect);
        MX_ERROR_CHECK(err);
    } else {
        // device doesnt draw a sector - we need to draw a
        // polypoint instead

        // approximate to half a pixel in the frame

        tolerance = 0.5 / frameResolution();

        // convert the arc to a polypoint -  let closure
        // be done by the draw polypoint routine

        a.arc.arcToBoundingPolypoint(tolerance, docppline.pp);

        // now convert polypoint points to ipolypoint points

        pushClipPolypoint(err, docppline, doIntersect);
        MX_ERROR_CHECK(err);
    }
abort:;
}
