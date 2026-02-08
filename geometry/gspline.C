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
 * MODULE : gspline.C
 *
 * AUTHOR : David Miller
 *
 * This file
 *
 * DESCRIPTION: C2 spline stuff
 * Module gpline.C
 *
 *
 *
 *
 */

#include <geometry.h>
#include <mx.h>

/*-------------------------------------------------
 * FUNCTION: mx_spline::newMemory
 *
 * DESCRIPTION:
 *
 *
 */

void mx_spline::newMemory(int& err, int newPoints)
{
    mx_point* oldPoints;
    mx_bezier* oldBeziers;

    err = MX_ERROR_OK;

    // test if new points are needed
    if (newPoints > nallocPoints) {
        oldPoints = points;
        nallocPoints = ((newPoints / 128) + 1) * 128;
        points = new mx_point[nallocPoints];

        for (int i = 0; i < npoints; i++) {
            points[i] = oldPoints[i];
        }

        delete[] oldPoints;

        // now add new bezier

        oldBeziers = beziers;
        beziers = new mx_bezier[nallocPoints];

        for (int i = 0; i < npoints; i++) {
            beziers[i] = oldBeziers[i];
        }

        delete[] oldBeziers;
    }
}

/*-------------------------------------------------
 * FUNCTION: mx_spline::setBeziers
 *
 * DESCRIPTION:
 *
 *
 */

void mx_spline::setBeziers(int startPoint,
    int endPoint)
{
    int thisBezier;

    // does start point change
    if (startPoint < 4) {
        beziers[0].a0 = points[0];
        beziers[0].a1 = points[1];
        beziers[0].a2 = (points[1] + points[2]) * 0.5;
        beziers[0].a3 = (points[1] + 4 * points[2] + points[3]) / 6;
        startPoint = 1;
        thisBezier = 1;
    } else {
        thisBezier = startPoint;
    }

    bool doLast = (endPoint >= npoints - 4);
    if (doLast)
        endPoint = npoints - 5;

    // do all points up to end (but not last point
    while (thisBezier < endPoint) {
        beziers[thisBezier].a0 = beziers[thisBezier - 1].a3;
        beziers[thisBezier].a1 = (2.0 * points[thisBezier + 1] + points[thisBezier + 2]) / 3.0;
        beziers[thisBezier].a2 = (points[thisBezier + 1] + 2.0 * points[thisBezier + 2]) / 3.0;
        beziers[thisBezier].a3 = (points[thisBezier + 1] + 4 * points[thisBezier + 2] + points[thisBezier + 3]) / 6.0;
        thisBezier++;
    }

    // now do the last point if needed
    if (doLast) {
        beziers[thisBezier].a0 = beziers[thisBezier - 1].a3;
        beziers[thisBezier].a1 = (points[npoints - 3] + points[npoints - 2]) * 0.5;
        beziers[thisBezier].a2 = points[npoints - 2];
        beziers[thisBezier].a3 = points[npoints - 1];
    }
}

/*-------------------------------------------------
 * FUNCTION: mx_spline::addPoint
 *
 * DESCRIPTION:
 *
 *
 */

void mx_spline::addPoint(const mx_point& p)
{
    points[npoints] = p;
    npoints++;

    // affects the last two spans
    setBeziers(npoints - 2, npoints - 1);
}

/*-------------------------------------------------
 * FUNCTION: mx_spline::copy
 *
 * DESCRIPTION:
 *
 *
 */

void mx_spline::copy(int& err, mx_spline& inpl, bool fullCopy)
{
    err = MX_ERROR_OK;

    if (fullCopy) {
        // get the memory
        newMemory(err, inpl.npoints);
        MX_ERROR_CHECK(err);

        for (int i = 0; i < inpl.npoints; i++) {
            points[i] = inpl.points[i];
        }

        for (int i = 0; i < inpl.npoints; i++) {
            beziers[i] = inpl.beziers[i];
        }

    } else {
        // pointer copy
        delete[] points;
        delete[] beziers;

        beziers = inpl.beziers;
        points = inpl.points;

        nallocPoints = inpl.nallocPoints;
    }

    npoints = inpl.npoints;
    box = inpl.box;

    goto exit;
abort:
exit:
    return;
}

/*-------------------------------------------------
 * FUNCTION: mx_spline::mx_spline
 *
 * DESCRIPTION:
 *
 *
 */

mx_spline::mx_spline(int& err,
    bool doCopy,
    int inpoints,
    mx_point* ipoints)
{
    int ip;

    err = MX_ERROR_OK;

    // need at least four points
    if (inpoints < 4) {
        err = MX_SPLINE_TOO_FEW_POINTS;
        return;
    }

    // create memory

    newMemory(err, inpoints);
    MX_ERROR_CHECK(err);

    for (ip = 0; ip < inpoints; ip++) {
        addPoint(ipoints[ip]);
        MX_ERROR_CHECK(err);
    }

    setBeziers(0, npoints - 1);

    return;
abort:
    return;
}

/*-------------------------------------------------
 * FUNCTION: mx_spline::dist2
 *
 * DESCRIPTION:
 *
 *
 */

double mx_spline::dist2snap(const mx_point& p,
    mx_snap_info& snapInfo,
    double tolerance) const
{
    return 0;
}

/*-------------------------------------------------
 * FUNCTION: gmx_spline::gmx_spline
 *
 * DESCRIPTION:
 *
 *
 */

gmx_spline::gmx_spline(int& err, mx_spline& pl1, bool fullCopy)
{
    err = MX_ERROR_OK;

    spl.copy(err, pl1, fullCopy);
    MX_ERROR_CHECK(err);

    goto exit;
abort:
exit:
    return;
}

/*-------------------------------------------------
 * FUNCTION: gmx_spline::get_serialise_size
 *
 * DESCRIPTION:
 *
 *
 */

uint32 gmx_spline::get_serialised_size(int& err)
{
    int size;

    err = MX_ERROR_OK;

    size = mx_geom::get_serialised_size(err);
    MX_ERROR_CHECK(err);

    size += spl.get_serialised_size(err);
    MX_ERROR_CHECK(err);
abort:
    return size;
}

/*-------------------------------------------------
 * FUNCTION: mx_garc::serialise
 *
 * DESCRIPTION:
 *
 *
 */

void gmx_spline::serialise(int& err,
    unsigned char** buffer)
{
    err = MX_ERROR_OK;

    mx_geom::serialise(err, buffer);
    MX_ERROR_CHECK(err);

    spl.serialise(err, buffer);
    MX_ERROR_CHECK(err);

    return;
abort:
    return;
}

/*-------------------------------------------------
 * FUNCTION: gmx_spline::unserialise
 *
 * DESCRIPTION:
 *
 *
 */

void gmx_spline::unserialise(int& err,
    unsigned char** buffer)
{
    mx_geom::unserialise(err, buffer);
    MX_ERROR_CHECK(err);

    spl.unserialise(err, buffer);
    MX_ERROR_CHECK(err);

    return;
abort:
    return;
}
