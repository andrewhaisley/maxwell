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
 * MODULE : gutil.C
 *
 * AUTHOR : David Miller
 *
 * This file
 *
 * DESCRIPTION:
 * Module gutil.C
 *
 *
 *
 *
 */

#include <geometry.h>
#include <mx.h>

/*-------------------------------------------------
 * FUNCTION: operator*(mx_point,f) (f,point)
 *
 * DESCRIPTION: p  = p1*f scaling
 *
 *
 */

mx_point operator*(double f, mx_point& p)
{
    return p * f;
}

mx_rect operator*(double f, mx_rect& r)
{
    return r * f;
}

/*-------------------------------------------------
 * FUNCTION: doubleDecomp
 *
 * DESCRIPTION: SPlit a number into an integer and
 *              a positive real part
 *
 */

void doubleDecomp(double r, int& ir, float& rr)
{
    ir = (int)r;
    rr = r - ir;
    if (r < 0) {
        ir = ir - 1;
        rr = rr + 1;
    }
}

/*-------------------------------------------------
 * FUNCTION: gatan2
 *
 * DESCRIPTION: Get angle relative to base angle
 *
 */

double gatan2(double y, double x, double baseAngle)
{
    double angle = atan2(y, x);
    while (angle <= baseAngle)
        angle += PITWO;
    return angle;
}

/*-------------------------------------------------
 * FUNCTION: gminimizer
 *
 * DESCRIPTION: Use golden rule search to find
 *              a minimum point
 *
 */

double gminimizer(double startValue,
    double endValue,
    double tolerance,
    GMINIMIZER_FUNC gminFunc,
    void* funcData,
    double& minValue)
{
    // Now iterate to minimim point
    double t1 = startValue;
    double t4 = endValue;
    double t2 = t1 + (t4 - t1) * mx_geom::fac1;
    double t3 = t1 + (t4 - t1) * mx_geom::fac2;
    double val1;
    double val2;
    double val3;
    double val4;
    double minPar;

    val1 = gminFunc(t1, funcData);
    val2 = gminFunc(t2, funcData);
    val3 = gminFunc(t3, funcData);
    val4 = gminFunc(t4, funcData);

    do {
        if ((val1 > val2) && (val2 > val3)) {
            t1 = t2;
            val1 = val2;

            t2 = t3;
            val2 = val3;

            t3 = t1 + (t4 - t1) * mx_geom::fac2;

            val3 = gminFunc(t3, funcData);
        } else {
            t4 = t3;
            val4 = val3;

            t3 = t2;
            val3 = val2;

            t2 = t1 + (t4 - t1) * mx_geom::fac1;

            val2 = gminFunc(t2, funcData);
        }
    } while (fabs(val1 - val4) > tolerance);

    minPar = (t1 + t4) * 0.5;
    minValue = gminFunc(minPar, funcData);

    return minPar;
}
