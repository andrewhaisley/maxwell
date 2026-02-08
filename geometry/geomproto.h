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
 * MODULE : geomproto.h
 *
 * AUTHOR : David Miller
 *
 * This file
 *
 * DESCRIPTION:
 * Module geomproto.h
 *
 *
 *
 *
 */

#ifndef GEOMETRYPROTO_H
#define GEOMETRYPROTO_H

/*-------------------------------------------------
 * FUNCTION: operator*(point,f) (f,point)
 *
 * DESCRIPTION: p  = p1*f scaling
 *
 *
 */

mx_point operator*(double f, mx_point& p);
mx_rect operator*(double f, mx_rect& r);

// Two points
GEOM_INTERSECT_TYPE intersect(int& err,
    mx_point& p1,
    mx_point& p2,
    mx_geom** geom = NULL,
    double tolerance = GEOM_SMALL);

// span with polyline

GEOM_INTERSECT_TYPE intersect(int& err,
    mx_polypoint& pl1,
    mx_rect& r1,
    mx_geom** geom = NULL,
    double tolerance = GEOM_SMALL);

// Point and rectangle/span

GEOM_INTERSECT_TYPE intersect(int& err,
    mx_point& p1,
    mx_rect& r1,
    mx_geom** geom = NULL,
    double tolerance = GEOM_SMALL);

GEOM_INTERSECT_TYPE intersect(int& err,
    mx_rect& r1,
    mx_point& p1,
    mx_geom** geom = NULL,
    double tolerance = GEOM_SMALL);

// Two rectangles

GEOM_INTERSECT_TYPE intersect(int& err,
    mx_rect& r1,
    mx_rect& r2,
    mx_geom** geom = NULL,
    double tolerance = GEOM_SMALL);

// arc and point
GEOM_INTERSECT_TYPE intersect(int& err,
    mx_arc& a1,
    mx_point& p2,
    mx_geom** geom = NULL,
    double tolerance = GEOM_SMALL);

// point and arc
GEOM_INTERSECT_TYPE intersect(int& err,
    mx_point& p1,
    mx_arc& a2,
    mx_geom** geom = NULL,
    double tolerance = GEOM_SMALL);

// arc and rect/span
GEOM_INTERSECT_TYPE intersect(int& err,
    mx_arc& a1,
    mx_rect& r2,
    mx_geom** geom = NULL,
    double tolerance = GEOM_SMALL);

// rect/span with arc
GEOM_INTERSECT_TYPE intersect(int& err,
    mx_rect& r1,
    mx_arc& a2,
    mx_geom** geom = NULL,
    double tolerance = GEOM_SMALL);

void doubleDecomp(double r, int& ir, float& rr);

/*-------------------------------------------------
 * FUNCTION: gatan2
 *
 * DESCRIPTION: Get angle relative to base angle
 *
 */

double gatan2(double y, double x, double baseAngle);

typedef double (*GMINIMIZER_FUNC)(double testValue,
    void* funcData);

/*-------------------------------------------------
 * FUNCTION: gminimizer
 *
 * DESCRIPTION: Functional minimiser
 *
 */

double gminimizer(double startValue,
    double endValue,
    double tolerance,
    GMINIMIZER_FUNC gminFunc,
    void* funcData,
    double& minValue);

#endif
