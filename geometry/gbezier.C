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
 * MODULE : gmx_bezier.C
 *
 * AUTHOR : David Miller
 *
 *
 *
 * DESCRIPTION: Mx_Bezier curve stuff
 * Module gmx_bezier.C
 *
 *
 *
 *
 */

#include <float.h>
#include <geometry.h>
#include <mx.h>

/*-------------------------------------------------
 * Function: mx_bezier::mx_bezier
 *
 * DESCRIPTION: Create a bezier from control points
 *
 *
 */

mx_bezier::mx_bezier(mx_point& ia0,
    mx_point& ia1,
    mx_point& ia2,
    mx_point& ia3)
{
    a0 = ia0;
    a1 = ia1;
    a2 = ia2;
    a3 = ia3;
}

/*-------------------------------------------------
 * FUNCTION: mx_bezier::linearFactor
 *
 * DESCRIPTION: Calculate the step size to approximate
 *              a bezier curve by straight lines
 *
 */

double mx_bezier::linearFactor(double tolerance)
{
    // The distance between the curve and
    // a set of line segments with t = 0,T,2T
    // etc can be approximated by
    //
    //   (T**2)/8 (fx''(t) + fy''(t))
    //
    //   Hence need to work out a maximum value
    //   of f'' for both x and y directions
    //

    double fact1, fact2, fact, facx, facy, error;

    fact1 = a0.x - 2 * a1.x + a2.x;
    if (fact1 < 0)
        fact1 = -fact1;
    fact2 = a1.x - 2 * a2.x + a3.x;
    if (fact2 < 0)
        fact2 = -fact2;
    facx = fact1 > fact2 ? fact1 : fact2;

    fact1 = a0.y - 2 * a1.y + a2.y;
    if (fact1 < 0)
        fact1 = -fact1;
    fact2 = a1.y - 2 * a2.y + a3.y;
    if (fact2 < 0)
        fact2 = -fact2;
    facy = fact1 > fact2 ? fact1 : fact2;

    fact = facx + facy;
    if (fact == 0) {
        return 1.0;
    } else {
        error = (8 * tolerance) / (6.0 * fact);
        return sqrt(error);
    }
}

/*-------------------------------------------------
 * FUNCTION: mx_bezier::bz
 *
 * DESCRIPTION: Get a point from a parameter along a
 * bezier curve
 *
 */

mx_point mx_bezier::bz(double t)
{
    mx_point p;
    double t2, t3, tm1, tm12, tm13;

    t2 = t * t;
    t3 = t2 * t;
    tm1 = 1 - t;
    tm12 = tm1 * tm1;
    tm13 = tm12 * tm1;
    p.x = a3.x * t3 + 3 * a2.x * t2 * tm1 + 3 * a1.x * t * tm12 + a0.x * tm13;
    p.y = a3.y * t3 + 3 * a2.y * t2 * tm1 + 3 * a1.y * t * tm12 + a0.y * tm13;
    return p;
}

/*-------------------------------------------------
 * FUNCTION: mx_bezier::box
 *
 * DESCRIPTION: Get the bounding box of a bezier
 *
 *
 */

mx_rect mx_bezier::box()
{
    mx_rect rt(a0, a0);

    // bezier is bounded by its control points

    if (a1.x < rt.xb)
        rt.xb = a1.x;
    if (a1.y < rt.yb)
        rt.yb = a1.y;
    if (a1.x > rt.xt)
        rt.xt = a1.x;
    if (a1.y > rt.yt)
        rt.yt = a1.y;

    if (a2.x < rt.xb)
        rt.xb = a2.x;
    if (a2.y < rt.yb)
        rt.yb = a2.y;
    if (a2.x > rt.xt)
        rt.xt = a2.x;
    if (a2.y > rt.yt)
        rt.yt = a2.y;

    if (a3.x < rt.xb)
        rt.xb = a3.x;
    if (a3.y < rt.yb)
        rt.yb = a3.y;
    if (a3.x > rt.xt)
        rt.xt = a3.x;
    if (a3.y > rt.yt)
        rt.yt = a3.y;

    rt.setSpanArea(FALSE, TRUE);

    return rt;
}

/*-------------------------------------------------
 * FUNCTION: mx_bezier::cps_convex
 *
 * DESCRIPTION:
 *
 *
 */

bool mx_bezier::cpsConvex()
{
    double length;
    mx_rect r(a0, a3, TRUE);
    mx_point sp = r.uspan(&length);
    mx_point up = sp.flip();

    // Get perpendicular of control
    // points along span

    double perp1 = (a1 - a0) * up;
    double perp2 = (a2 - a0) * up;

    if (((perp1 > 0) && (perp2 > 0)) || ((perp1 < 0) && (perp2 < 0))) {
        // Get the length along the span
        double along1 = (a1 - a0) * sp;
        double along2 = (a2 - a0) * sp;

        if (along1 < along2) {
            perp1 = fabs(perp1);
            perp2 = fabs(perp2);

            // First three points convex ?
            if (perp1 * along2 < perp2 * along1)
                return FALSE;

            // Last three point convex
            along1 = length - along1;
            along2 = length - along2;

            if (perp1 * along2 > perp2 * along1)
                return FALSE;

            return TRUE;
        } else {
            // points switch - not convex
            return FALSE;
        }
    } else {
        // control points on different sides
        // Not convex
        return FALSE;
    }
}

/*-------------------------------------------------
 * FUNCTION: mx_bezier::distConvex
 *
 * DESCRIPTION: Get the bounding box of a bezier
 *
 *
 */

double mx_bezier::distConvex2(mx_point& p, double minDist, double tolerance, mx_point* pt)
{
    // Now iterate to minimim point
    double t1 = 0;
    double t4 = 1.0;
    double t2 = t1 + (t4 - t1) * mx_geom::fac1;
    double t3 = t1 + (t4 - t1) * mx_geom::fac2;

    double tolerance2 = tolerance * tolerance;

    double dist1i = bz(t1).dist2(p);
    double dist2i = bz(t2).dist2(p);
    double dist3i = bz(t3).dist2(p);
    double dist4i = bz(t4).dist2(p);

    if (dist2i >= dist1i) {
        // Must be an end point

        /* ?? */
        /* need to test point is on right side of convexity */
        return GMIN(dist1i, dist4i);
    }

    do {
        if ((dist1i > dist2i) && (dist2i > dist3i)) {
            t1 = t2;
            dist1i = dist2i;

            t2 = t3;
            dist2i = dist3i;

            t3 = t1 + (t4 - t1) * mx_geom::fac2;

            dist3i = bz(t3).dist2(p);
        } else {
            t4 = t3;
            dist4i = dist3i;

            t3 = t2;
            dist3i = dist2i;

            t2 = t1 + (t4 - t1) * mx_geom::fac1;

            dist2i = bz(t2).dist2(p);
        }
    } while (fabs(dist1i - dist4i) > tolerance2);

    // go for midpoint

    t1 = (t1 + t4) * 0.5;

    return bz(t1).dist2(p);
}

/*-------------------------------------------------
 * FUNCTION: mx_bezier::dist
 *
 * DESCRIPTION: Get the bounding box of a bezier
 *
 *
 */

double mx_bezier::dist2snap(const mx_point& p,
    mx_snap_info& snapInfo,
    double tolerance) const

{
    return 0;

    // break the bezier into convex bits as much
    // as possible

    /*
      return internDist2(p,minDist,tolerance,0,7,DBL_MAX,pt) ;
      */
}

/*-------------------------------------------------
 * FUNCTION: mx_bezier::internDist2
 *
 * DESCRIPTION: Get the bounding box of a bezier
 *
 *
 */

double mx_bezier::internDist2(mx_point& p,
    double minDist,
    double tolerance,
    double splitLevel,
    double maxLevel,
    double currentDist,
    mx_point* pt)
{
    // First check against the box
    double distanceTest = box().dist2snap(p);

    // Too far away
    if (distanceTest > currentDist)
        return DBL_MAX;

    if (cpsConvex()) {
        // Its convex we can finish this one
        distanceTest = distConvex2(p, minDist, tolerance, pt);
        return GMIN(distanceTest, currentDist);
    } else {
        if (splitLevel == maxLevel) {
            mx_rect r;
            mx_point p1;

            // probably converging on an inflexion
            // do this the long way

            // get step along the bezier
            double tstep = linearFactor(tolerance);

            // initialise variables
            double tpos = 0;
            mx_point p0 = a0;

            r.isSpan = TRUE;

            while (tpos < 1.0) {
                r.xb = p0.x;
                r.yb = p0.y;

                tpos += tstep;
                if (tpos > 1.0)
                    tpos = 1.0;

                p1 = bz(tpos);

                r.xt = p1.x;
                r.yt = p1.y;

                distanceTest = r.dist2snap(p);
                if (distanceTest < currentDist)
                    currentDist = distanceTest;

                p0 = p1;
            }
            return currentDist;
        } else {
            // Split in two

            mx_point a00 = a0;
            mx_point a10 = (a0 + a1) * 0.5;
            mx_point a30 = bz(0.5);
            mx_point a20 = a30 - (a1 - a0) * 0.125 - (a2 - a1) * 0.25 - (a3 - a2) * 0.125;

            mx_bezier bz1(a00, a10, a20, a30);

            distanceTest = bz1.internDist2(p,
                minDist,
                tolerance,
                splitLevel + 1,
                maxLevel,
                currentDist, pt);
            if (distanceTest < currentDist)
                currentDist = distanceTest;

            a30 = a3;
            a20 = (a3 + a2) * 0.5;
            a00 = a30;
            a10 = a00 + (a3 - a2) * 0.125 + (a2 - a1) * 0.25 + (a3 - a2) * 0.125;

            mx_bezier bz2(a00, a10, a20, a30);

            distanceTest = bz2.internDist2(p,
                minDist,
                tolerance,
                splitLevel + 1,
                maxLevel,
                currentDist, pt);
            if (distanceTest < currentDist)
                currentDist = distanceTest;

            return currentDist;
        }
    }
}

/*-------------------------------------------------
 * FUNCTION: mx_bezier::get_serialise_size
 *
 * DESCRIPTION:
 *
 *
 */

uint32 mx_bezier::get_serialised_size(int& err)
{
    err = MX_ERROR_OK;

    return 8 * SLS_DOUBLE;
}

/*-------------------------------------------------
 * FUNCTION: mx_bezier::serialise
 *
 * DESCRIPTION:
 *
 *
 */

void mx_bezier::serialise(int& err, uint8** buffer)
{
    serialise_double(a0.x, buffer);
    serialise_double(a0.y, buffer);
    serialise_double(a1.x, buffer);
    serialise_double(a1.y, buffer);
    serialise_double(a2.x, buffer);
    serialise_double(a2.y, buffer);
    serialise_double(a3.x, buffer);
    serialise_double(a3.y, buffer);

    return;
}

/*-------------------------------------------------
 * Function: mx_bezier::unserialise
 *
 * DESCRIPTION:
 *
 *
 */

void mx_bezier::unserialise(int& err,
    unsigned char** buffer)
{
    unserialise_double(a0.x, buffer);
    unserialise_double(a0.y, buffer);
    unserialise_double(a1.x, buffer);
    unserialise_double(a1.y, buffer);
    unserialise_double(a2.x, buffer);
    unserialise_double(a2.y, buffer);
    unserialise_double(a3.x, buffer);
    unserialise_double(a3.y, buffer);

    return;
}

/*-------------------------------------------------
 * FUNCTION: gmx_bezier::get_serialise_size
 *
 * DESCRIPTION:
 *
 *
 */

uint32 gmx_bezier::get_serialised_size(int& err)
{
    int size;

    err = MX_ERROR_OK;

    size = mx_geom::get_serialised_size(err);
    MX_ERROR_CHECK(err);

    size += b.get_serialised_size(err);
    MX_ERROR_CHECK(err);
abort:
    return size;
}

/*-------------------------------------------------
 * FUNCTION: mx_gbezier::serialise
 *
 * DESCRIPTION:
 *
 *
 */

void gmx_bezier::serialise(int& err,
    unsigned char** buffer)
{
    err = MX_ERROR_OK;

    mx_geom::serialise(err, buffer);
    MX_ERROR_CHECK(err);

    b.serialise(err, buffer);
    MX_ERROR_CHECK(err);

    return;
abort:
    return;
}

/*-------------------------------------------------
 * FUNCTION: gmx_bezier::unserialise
 *
 * DESCRIPTION:
 *
 *
 */

void gmx_bezier::unserialise(int& err,
    unsigned char** buffer)
{
    mx_geom::unserialise(err, buffer);
    MX_ERROR_CHECK(err);

    b.unserialise(err, buffer);
    MX_ERROR_CHECK(err);

    return;
abort:
    return;
}
