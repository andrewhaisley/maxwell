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
 * MODULE : gpoint.C
 *
 * AUTHOR : David Miller
 *
 * This file
 *
 * DESCRIPTION:
 * Module gpoint.C
 *
 *
 *
 *
 */

#include <geometry.h>
#include <mx.h>

mx_point defaultPoint;

/*-------------------------------------------------
 * FUNCTION: mx_point::mx_point
 *
 * DESCRIPTION: Default constructor
 *
 *
 */

mx_point::mx_point()
{
    x = y = 0;
}

/*-------------------------------------------------
 * FUNCTION: mx_point::mx_point
 *
 * DESCRIPTION: Constructor from two coords
 *
 *
 */

mx_point::mx_point(double px, double py)
{
    x = px;
    y = py;
}

/*-------------------------------------------------
 * FUNCTION: mx_point::mx_point
 *
 * DESCRIPTION: Constructor from integer point
 *
 *
 */

mx_point::mx_point(const mx_ipoint& ip)
{
    x = ip.x;
    y = ip.y;
}

/*-------------------------------------------------
 *  FUNCTION: rectagle unit spans
 *
 * DESCRIPTION:
 *
 *
 */

mx_point mx_point::uspan(double* lent) const
{
    double len = length();

    if (lent != NULL)
        *lent = len;

    if (len > 0) {
        return (*this / len);
    } else {
        return *this * 0;
    }
}

/*-------------------------------------------------
 * FUNCTION: mx_point::rotate
 *
 * DESCRIPTION: Constructor from integer point
 *
 *
 */

mx_point mx_point::rotate(const mx_rotation& rotation) const
{
    if (rotation.a.a() == 0) {
        return *this;
    } else {
        mx_point po(rotation.a.c() * this->x
                - rotation.a.s() * this->y,
            rotation.a.s() * this->x
                + rotation.a.c() * this->y);
        return po;
    }
}

/*-------------------------------------------------
 * FUNCTION: mx_point::affine
 *
 * DESCRIPTION: Constructor from integer point
 *
 *
 */

mx_point mx_point::affine(mx_affine& aff)
{
    mx_point po = aff * (*this);
    return po;
}

/*-------------------------------------------------
 * FUNCTION: mx_point::mergeSelectCount
 *
 * DESCRIPTION: Constructor from integer point
 *
 *
 */

bool mx_point::mergeSelectCount(int nselectCounts,
    int* selectCounts)
{
    int ic;
    bool allTrue = FALSE;

    for (ic = 0; ic < nselectCounts; ic++) {
        if (selectCounts[ic] == MX_POINT_SNAP_WHOLE)
            allTrue = TRUE;
        selectCounts[ic] = -1;
    }

    if (allTrue)
        selectCounts[0] = MX_POINT_SNAP_WHOLE;

    return allTrue;
}

/*-------------------------------------------------
 * FUNCTION: mx_point::dist2
 *
 * DESCRIPTION: Constructor from integer point
 *
 *
 */

double mx_point::dist2snap(const mx_point& p,
    mx_snap_info& snapInfo,
    double tolerance) const
{
    if ((snapInfo.isSnapping()) && (snapInfo.alreadyFailed(MX_POINT_SNAP_WHOLE))) {
        snapInfo.setCount(MX_GEOM_SNAP_NONE);
        return 0;
    }

    // get value for distance
    double distVal = dist2(p);

    snapInfo.setPoint(*this, distVal, MX_POINT_SNAP_WHOLE, 0);

    return distVal;
}

/*-------------------------------------------------
 * FUNCTION: mx_ipoint::mx_ipoint
 *
 * DESCRIPTION: Default constructor
 *
 *
 */

mx_ipoint::mx_ipoint()
{
    x = y = 0;
}

/*-------------------------------------------------
 * FUNCTION: mx_ipoint::mx_ipoint
 *
 * DESCRIPTION: Constructor from two coords
 *
 *
 */

mx_ipoint::mx_ipoint(int32 px, int32 py)
{
    x = px;
    y = py;
}

/*-------------------------------------------------
 * FUNCTION: mx_ipoint::mx_ipoint
 *
 * DESCRIPTION: Constructor from real point
 *
 *
 */

mx_ipoint::mx_ipoint(const mx_point& rp)
{
    x = (int32)GNINT(rp.x);
    y = (int32)GNINT(rp.y);
}

/*-------------------------------------------------
 * FUNCTION: mx_ipoint::dist
 *
 * DESCRIPTION:
 *
 *
 */

double mx_ipoint::dist(const mx_point& p) const
{
    return p.dist(*this);
}

/*-------------------------------------------------
 * FUNCTION: mx_ipoint::dist1
 *
 * DESCRIPTION:
 *
 *
 */

double mx_ipoint::dist1(const mx_point& p) const
{
    return p.dist1(*this);
}

/*-------------------------------------------------
 * Function: mx_ipoint::operator*
 *
 * DESCRIPTION:
 *
 *
 */

mx_point mx_ipoint::operator*(double f)
{
    mx_point pt(x * f, y * f);
    return pt;
}

/*-------------------------------------------------
 * Function: mx_ipoint::operator*
 *
 * DESCRIPTION:
 *
 *
 */

mx_ipoint mx_ipoint::operator*(int s)
{
    mx_ipoint pt(x * s, y * s);
    return pt;
}

/*-------------------------------------------------
 * Function: mx_ipoint::operator/
 *
 * DESCRIPTION:
 *
 *
 */

mx_point mx_ipoint::operator/(double f)
{
    mx_point pt(x / f, y / f);
    return pt;
}

/*-------------------------------------------------
 * FUNCTION: gpoint::gmx_point
 *
 * DESCRIPTION:
 *
 *
 */

gmx_point::gmx_point(const mx_point& pp)
{
    type = MX_GEOM_POINT;
    p = pp;
}

/*-------------------------------------------------
 * Function: gmx_point::intersectBox
 *
 * DESCRIPTION:
 *
 *
 */

GEOM_INTERSECT_TYPE gmx_point::intersectBox(int& err, mx_rect& r, double tolerance)
{
    err = MX_ERROR_OK;

    return GEOM_INTERSECT_NONE;

    //    return intersect(err,p,r,NULL,tolerance) ;
}

/*-------------------------------------------------
 * FUNCTION: mx_point::get_serialise_size
 *
 * DESCRIPTION:
 *
 *
 */

uint32 mx_point::get_serialised_size(int& err)
{
    err = MX_ERROR_OK;

    return 2 * SLS_DOUBLE;
}

/*-------------------------------------------------
 * FUNCTION: mx_garc::serialise
 *
 * DESCRIPTION:
 *
 *
 */

void mx_point::serialise(int& err,
    unsigned char** buffer)
{
    err = MX_ERROR_OK;

    serialise_double(x, buffer);
    serialise_double(y, buffer);

    return;
}

/*-------------------------------------------------
 * FUNCTION: mx_point::unserialise
 *
 * DESCRIPTION:
 *
 *
 */

void mx_point::unserialise(int& err,
    unsigned char** buffer)
{
    err = MX_ERROR_OK;

    unserialise_double(x, buffer);
    unserialise_double(y, buffer);

    return;
}

/*-------------------------------------------------
 * FUNCTION: gmx_point::get_serialise_size
 *
 * DESCRIPTION:
 *
 *
 */

uint32 gmx_point::get_serialised_size(int& err)
{
    int size;

    err = MX_ERROR_OK;

    size = mx_geom::get_serialised_size(err);
    MX_ERROR_CHECK(err);

    size += p.get_serialised_size(err);
    MX_ERROR_CHECK(err);
abort:
    return size;
}

/*-------------------------------------------------
 * FUNCTION: mx_gpoint::serialise
 *
 * DESCRIPTION:
 *
 *
 */

void gmx_point::serialise(int& err,
    unsigned char** buffer)
{
    err = MX_ERROR_OK;

    mx_geom::serialise(err, buffer);
    MX_ERROR_CHECK(err);

    p.serialise(err, buffer);
    MX_ERROR_CHECK(err);

    return;
abort:
    return;
}

/*-------------------------------------------------
 * FUNCTION: gmx_point::unserialise
 *
 * DESCRIPTION:
 *
 *
 */

void gmx_point::unserialise(int& err,
    unsigned char** buffer)
{
    mx_geom::unserialise(err, buffer);
    MX_ERROR_CHECK(err);

    p.unserialise(err, buffer);
    MX_ERROR_CHECK(err);

    return;
abort:
    return;
}
