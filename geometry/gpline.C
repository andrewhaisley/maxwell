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
 * MODULE : gpline.C
 *
 * AUTHOR : David Miller
 *
 * This file
 *
 * DESCRIPTION: Polyline stuff
 * Module gpline.C
 *
 *
 *
 */

#include <geometry.h>
#include <mx.h>

/*-------------------------------------------------
 * FUNCTION: mx_ipolypoint::init
 *
 * DESCRIPTION:
 *
 *
 */

void mx_ipolypoint::init(bool resetPoints,
    POLYPOINT_TYPE itype)
{
    if (resetPoints) {
        delete[] points;
        points = NULL;
        num_alloc_points = 0;
    }

    num_points = 0;
    is_closed = FALSE;
    is_area = FALSE;
    allowCoincident = TRUE;

    box.xt = box.xb = box.yt = box.yb = 0;
    unsetMask();
    type = itype;
}

/*-------------------------------------------------
 * FUNCTION: mx_ipolypoint::mx_ipolypoint
 *
 * DESCRIPTION:
 *
 *
 */

mx_ipolypoint::mx_ipolypoint(POLYPOINT_TYPE itype)
{
    points = NULL;
    init(TRUE, itype);
}

/*-------------------------------------------------
 * FUNCTION: mx_ipolypoint::mx_ipolypoint
 *
 * DESCRIPTION:
 *
 *
 */

mx_ipolypoint::mx_ipolypoint(int inum_points,
    POLYPOINT_TYPE itype)
{
    int ipoint;
    mx_ipoint inpoint;

    points = NULL;
    init(TRUE, itype);

    setMemorySize(inum_points);

    for (ipoint = 0; ipoint < inum_points; ipoint++) {
        addPoint(inpoint);
    }
}

/*-------------------------------------------------
 * FUNCTION: mx_ipolypoint::clear
 *
 * DESCRIPTION:
 *
 *
 */

void mx_ipolypoint::clear(int npoints)
{
    bool resetPoints = (npoints > num_alloc_points) || (npoints == -1);

    init(resetPoints, type);

    // now add the new memory
    setMemorySize(npoints);
}

/*-------------------------------------------------
 * FUNCTION: mx_ipolypoint::~mx_ipolypoint
 *
 * DESCRIPTION:
 *
 *
 */

mx_ipolypoint::~mx_ipolypoint()
{
    clear(-1);
}

/*-------------------------------------------------
 * FUNCTION: mx_ipolypoint::copy
 *
 * DESCRIPTION:
 *
 *
 */

void mx_ipolypoint::copy(const mx_ipolypoint& pp)
{
    mx_ipoint inpoint;

    copyBasics(pp);

    clear(pp.get_num_points());

    copyPoints(pp, 0, pp.get_num_points());
}

/*-------------------------------------------------
 * FUNCTION: mx_ipolypoint::mx_ipolypoint
 *
 * DESCRIPTION:
 *
 *
 */

mx_ipolypoint::mx_ipolypoint(const mx_ipolypoint& pp)
{
    copy(pp);
}

/*-------------------------------------------------
 * FUNCTION: mx_ipolypoint::bounding_box
 *
 * DESCRIPTION:
 *
 *
 */

mx_irect mx_ipolypoint::bounding_box() const
{
    return box;
}

/*-------------------------------------------------
 * FUNCTION: mx_ipolypoint::operator+=(const mx_point &p)
 *
 * DESCRIPTION:
 *
 *
 */

void mx_ipolypoint::operator+=(const mx_ipoint& p)
{
    int i;

    for (i = 0; i <= num_points; i++) {
        points[i] += p;
    }

    box += p;
}

/*-------------------------------------------------
 * FUNCTION: mx_ipolypoint::operator=(const mx_ipolypoint &pp)
 *
 * DESCRIPTION:
 *
 *
 */

mx_ipolypoint& mx_ipolypoint::operator=(const mx_ipolypoint& pp)
{
    copy(pp);

    return *this;
}

/*-------------------------------------------------
 * FUNCTION: mx_ipolypoint::operator=(const mx_ipolypoint &pp)
 *
 * DESCRIPTION:
 *
 *
 */

mx_ipoint& mx_ipolypoint::operator[](int index) const
{
    return points[index];
}

/*-------------------------------------------------
 * FUNCTION: mx_ipolypoint::setMemorySize
 *
 * DESCRIPTION:
 *
 *
 */

void mx_ipolypoint::setMemorySize(int newSize)
{
    int ipoint;
    mx_ipoint* oldPoints;

    // do we need more memory
    if (newSize <= num_alloc_points)
        return;

    oldPoints = points;
    num_alloc_points = newSize;

    points = new mx_ipoint[num_alloc_points];

    for (ipoint = 0; ipoint < num_points; ipoint++) {
        points[ipoint] = oldPoints[ipoint];
    }

    delete[] oldPoints;
}

/*-------------------------------------------------
 * FUNCTION: mx_ipolypoint::addPoint
 *
 * DESCRIPTION:
 *
 *
 */

bool mx_ipolypoint::addPoint(const mx_ipoint& p)
{

    if ((!allowCoincident) && (p == points[num_points - 1]))
        return FALSE;

    // do we need more memory
    setMemorySize(num_points + 1);

    points[num_points] = p;
    num_points++;

    // now adjust the box
    if (num_points == 1) {
        box.xb = box.xt = p.x;
        box.yb = box.yt = p.y;
        box.setSpanArea(FALSE, TRUE);
    } else {
        box.xb = GMIN(box.xb, p.x);
        box.yb = GMIN(box.yb, p.y);
        box.xt = GMAX(box.xt, p.x);
        box.yt = GMAX(box.yt, p.y);
    }
    return TRUE;
}

/*-------------------------------------------------
 * FUNCTION: mx_ipolypoint::addNewPoint
 *
 * DESCRIPTION:
 *
 *
 */

bool mx_ipolypoint::addNewPoint(const mx_ipoint& p)
{
    bool addThePoint;

    if (num_points != 0) {
        addThePoint = (points[num_points - 1] != p);
    } else {
        addThePoint = TRUE;
    }

    if (addThePoint) {
        addPoint(p);
    }

    return addThePoint;
}

/*-------------------------------------------------
 * FUNCTION: mx_ipolypoint::coincidentEnds
 *
 * DESCRIPTION:
 *
 *
 */

bool mx_ipolypoint::coincidentEnds()
{
    if (num_points == 0) {
        return TRUE;
    } else {
        return (points[0] == points[num_points - 1]);
    }
}

/*-------------------------------------------------
 * FUNCTION: mx_polypoint::getPoint
 *
 * DESCRIPTION:
 *
 *
 */

mx_point mx_polypoint::getPoint(double rpoint) const
{
    int ipoint;

    if (rpoint <= 0) {
        return points[0];
    } else if (rpoint >= num_points) {
        return points[num_points - 1];
    } else {
        ipoint = (int)rpoint;
        rpoint -= ipoint;

        if (rpoint == 0) {
            return points[ipoint];
        } else {
            return points[ipoint].spanpoint(points[ipoint + 1], rpoint);
        }
    }
}

/*-------------------------------------------------
 * FUNCTION: mx_polypoint::init
 *
 * DESCRIPTION:
 *
 *
 */

void mx_polypoint::init(bool resetPoints,
    POLYPOINT_TYPE itype)
{
    if (resetPoints) {
        delete[] points;
        points = NULL;
        num_alloc_points = 0;
    }

    num_points = 0;
    is_closed = FALSE;
    is_area = FALSE;
    box.xt = box.xb = box.yt = box.yb = 0;
    unsetMask();
    type = itype;
    allowCoincident = TRUE;
}

/*-------------------------------------------------
 * FUNCTION: mx_polypoint::mx_polypoint
 *
 * DESCRIPTION:
 *
 *
 */

mx_polypoint::mx_polypoint(POLYPOINT_TYPE itype)
{
    points = NULL;
    init(TRUE, itype);
}

/*-------------------------------------------------
 * FUNCTION: mx_polypoint::mx_polypoint
 *
 * DESCRIPTION:
 *
 *
 */

mx_polypoint::mx_polypoint(int inum_points,
    POLYPOINT_TYPE itype)
{
    int ipoint;
    mx_point inpoint;

    points = NULL;
    init(TRUE, itype);

    setMemorySize(inum_points);

    for (ipoint = 0; ipoint < inum_points; ipoint++) {
        addPoint(inpoint);
    }
}

/*-------------------------------------------------
 * FUNCTION: mx_polypoint::clear
 *
 * DESCRIPTION:
 *
 *
 */

void mx_polypoint::clear(int npoints)
{
    bool resetPoints = ((npoints == -1) || (npoints > num_alloc_points));

    init(resetPoints, type);
}

/*-------------------------------------------------
 * FUNCTION: mx_polypoint::~mx_polypoint
 *
 * DESCRIPTION:
 *
 *
 */

mx_polypoint::~mx_polypoint()
{
    clear(-1);
}

/*-------------------------------------------------
 * FUNCTION: mx_polypoint::copy
 *
 * DESCRIPTION:
 *
 *
 */

void mx_polypoint::copy(const mx_ipolypoint& pp)
{
    mx_point inpoint;

    copyBasics(pp);

    clear(pp.get_num_points());

    copyPoints(pp, 0, pp.get_num_points());
}

/*-------------------------------------------------
 * FUNCTION: mx_polypoint::copy
 *
 * DESCRIPTION:
 *
 *
 */

void mx_polypoint::copy(const mx_polypoint& pp)
{
    mx_point inpoint;

    copyBasics(pp);

    clear(pp.get_num_points());

    copyPoints(pp, 0, pp.get_num_points());
}

/*-------------------------------------------------
 * FUNCTION: mx_polypoint::mx_polypoint
 *
 * DESCRIPTION:
 *
 *
 */

mx_polypoint::mx_polypoint(const mx_ipolypoint& pp)
{
    copy(pp);
}

/*-------------------------------------------------
 * FUNCTION: mx_polypoint::mx_polypoint
 *
 * DESCRIPTION:
 *
 *
 */

mx_polypoint::mx_polypoint(const mx_polypoint& pp)
{
    copy(pp);
}

/*-------------------------------------------------
 * FUNCTION: mx_polypoint::bounding_box
 *
 * DESCRIPTION:
 *
 *
 */

mx_rect mx_polypoint::bounding_box() const
{
    return box;
}

/*-------------------------------------------------
 * FUNCTION: mx_polypoint::operator+=(const mx_point &p)
 *
 * DESCRIPTION:
 *
 *
 */

void mx_polypoint::operator+=(const mx_point& p)
{
    int i;

    for (i = 0; i <= num_points; i++) {
        points[i] += p;
    }

    box += p;
}

/*-------------------------------------------------
 * FUNCTION: mx_polypoint::operator=(const mx_polypoint &pp)
 *
 * DESCRIPTION:
 *
 *
 */

mx_polypoint& mx_polypoint::operator=(const mx_polypoint& pp)
{
    copy(pp);

    return *this;
}

/*-------------------------------------------------
 * FUNCTION: mx_polypoint::operator=(const mx_polypoint &pp)
 *
 * DESCRIPTION:
 *
 *
 */

mx_polypoint& mx_polypoint::operator=(const mx_ipolypoint& pp)
{
    copy(pp);

    return *this;
}

/*-------------------------------------------------
 * FUNCTION: mx_polypoint::operator=(const mx_polypoint &pp)
 *
 * DESCRIPTION:
 *
 *
 */

mx_point& mx_polypoint::operator[](int index) const
{
    return points[index];
}

/*-------------------------------------------------
 * FUNCTION: mx_polypoint::get_serialise_size
 *
 * DESCRIPTION:
 *
 *
 */

uint32 mx_polypoint::get_serialised_size(int& err)
{
    int ipoint;
    int size;

    err = MX_ERROR_OK;

    size = basicSize;

    for (ipoint = 0; ipoint < num_points; ipoint++) {
        size += points[ipoint].get_serialised_size(err);
        MX_ERROR_CHECK(err);
    }

abort:
    return size;
}

/*-------------------------------------------------
 * FUNCTION: mx_polypoint::serialise
 *
 * DESCRIPTION:
 *
 *
 */

void mx_polypoint::serialise(int& err,
    unsigned char** buffer)
{
    int ipoint;

    err = MX_ERROR_OK;

    allowCoincident = FALSE;

    serialise_int32(num_points, buffer);
    serialise_bool(is_closed, buffer);
    serialise_bool(is_area, buffer);
    serialise_enum(type, buffer);

    for (ipoint = 0; ipoint < num_points; ipoint++) {
        points[ipoint].serialise(err, buffer);
        MX_ERROR_CHECK(err);
    }

    return;
abort:
    return;
}

/*-------------------------------------------------
 * FUNCTION: mx_polypoint::unserialise
 *
 * DESCRIPTION:
 *
 *
 */

void mx_polypoint::unserialise(int& err,
    unsigned char** buffer)
{
    int32 inpoints;
    mx_point ipoint;
    int ispoint;
    uint16 e;

    err = MX_ERROR_OK;

    allowCoincident = TRUE;

    unserialise_int32(inpoints, buffer);
    unserialise_bool(is_closed, buffer);
    unserialise_bool(is_area, buffer);
    unserialise_enum(e, buffer);

    type = (POLYPOINT_TYPE)e;

    for (ispoint = 0; ispoint < inpoints; ispoint++) {
        ipoint.unserialise(err, buffer);
        MX_ERROR_CHECK(err);

        addPoint(ipoint);
        MX_ERROR_CHECK(err);
    }

    return;
abort:
    return;
}

/*-------------------------------------------------
 * FUNCTION: mx_polypoint::setMemorySize
 *
 * DESCRIPTION:
 *
 *
 */

void mx_polypoint::setMemorySize(int newSize)
{
    int ipoint;
    mx_point* oldPoints;

    // do we need more memory
    if (newSize <= num_alloc_points)
        return;

    // round to the nearest 128 points
    newSize = (newSize + 127) / 128;
    newSize *= 128;

    oldPoints = points;
    num_alloc_points = newSize;

    points = new mx_point[num_alloc_points];

    for (ipoint = 0; ipoint < num_points; ipoint++) {
        points[ipoint] = oldPoints[ipoint];
    }

    delete[] oldPoints;
}

/*-------------------------------------------------
 * FUNCTION: mx_polypoint::addNewPoint
 *
 * DESCRIPTION:
 *
 *
 */

bool mx_polypoint::addNewPoint(const mx_point& p)
{
    bool addThePoint;

    if (num_points != 0) {
        addThePoint = (points[num_points - 1] != p);
    } else {
        addThePoint = TRUE;
    }

    if (addThePoint) {
        addPoint(p);
    }

    return addThePoint;
}

/*-------------------------------------------------
 * FUNCTION: mx_polypoint::addPoint
 *
 * DESCRIPTION:
 *
 *
 */

bool mx_polypoint::addPoint(const mx_point& p)
{
    if ((!allowCoincident) && (num_points > 0) && (p == points[num_points - 1]))
        return FALSE;

    // do we need more memory
    setMemorySize(num_points + 1);

    points[num_points] = p;
    num_points++;

    // now adjust the box
    if (num_points == 1) {
        box.xb = box.xt = p.x;
        box.yb = box.yt = p.y;
        box.setSpanArea(FALSE, TRUE);
    } else {
        box.xb = GMIN(box.xb, p.x);
        box.yb = GMIN(box.yb, p.y);
        box.xt = GMAX(box.xt, p.x);
        box.yt = GMAX(box.yt, p.y);
    }

    return TRUE;
}

/*-------------------------------------------------
 * FUNCTION: mx_polypoint::in_area
 *
 * DESCRIPTION:
 *
 *
 */

bool mx_polypoint::in_area(const mx_point& p) const
{
    mx_point *thisPoint, *nextPoint, *endPoint;
    int ncross;
    double xcross, ratio;

    if (!is_area)
        return FALSE;

    ncross = 0;

    // else see how often boundary crosses the line
    // from (p.x,p.y) to (p.x,infinity)

    thisPoint = points;
    nextPoint = points + 1;
    endPoint = points + num_points;

    while (nextPoint < endPoint) {
        if (((thisPoint->y >= p.y) && (nextPoint->y < p.y)) || ((thisPoint->y <= p.y) && (nextPoint->y > p.y))) {
            // Crosses line

            ratio = (nextPoint->x - thisPoint->x) / (nextPoint->y - thisPoint->y);
            xcross = thisPoint->x + (p.y - thisPoint->y) * ratio;

            if (xcross > p.x)
                ncross++;
        }
        nextPoint = thisPoint;
        thisPoint++;
    }

    return (ncross % 2 == 1);
}

/*-------------------------------------------------
 * FUNCTION: mx_polypoint::mergeSelectCount
 *
 * DESCRIPTION:
 *
 *
 */

bool mx_polypoint::mergeSelectCount(int nselectCounts,
    int* selectCounts)
{
    int noc, totalCount, ic;
    bool inc[num_points];
    bool allTrue = FALSE;
    int lastSpanCount = 2 * num_points - 1;
    int selectCount;

    if (is_closed)
        lastSpanCount++;

    components(noc, totalCount);

    for (ic = 0; ic < num_points; ic++)
        inc[ic] = FALSE;

    for (ic = 0; ic < nselectCounts; ic++) {
        selectCount = selectCounts[ic];

        if ((selectCount >= 1) && (selectCount <= num_points)) {
            inc[selectCount] = TRUE;
        } else if ((selectCount > num_points) && (selectCount <= lastSpanCount)) {
            int endPoint = selectCount - num_points;
            int startPoint = endPoint - 1;

            if (endPoint == num_points)
                endPoint = 0;

            inc[startPoint] = TRUE;
            inc[endPoint] = TRUE;
        } else if (selectCount == totalCount) {
            allTrue = TRUE;
            break;
        } else if (selectCount == -1) {
            break;
        }
        selectCounts[ic] = -1;
    }

    // test if everything is true
    if (!allTrue) {
        allTrue = TRUE;

        for (ic = 0; ic < num_points; ic++) {
            if (!inc[ic]) {
                allTrue = FALSE;
                break;
            }
        }
    }

    if (allTrue || (num_points == 1)) {
        selectCounts[0] = totalCount;
    } else {
        int outSelectCount = 0;
        int startPoint = 0;
        int endPoint = num_points;
        bool doEndSpan = FALSE;

        if (is_closed && inc[0] && inc[num_points - 1]
            && (!inc[1])) {
            doEndSpan = TRUE;
            startPoint++;
            endPoint--;
        }

        ic = startPoint;

        while (ic < endPoint) {
            if (inc[ic]) {
                if (ic == endPoint - 1) {
                    selectCounts[outSelectCount] = ic + 1;
                    outSelectCount++;
                } else if (inc[ic + 1]) {
                    selectCounts[outSelectCount] = num_points + ic;
                    outSelectCount++;
                    ic++;
                } else {
                    selectCounts[outSelectCount] = ic + 1;
                    outSelectCount++;
                }
            }

            ic++;
        }

        if (doEndSpan) {
            selectCounts[outSelectCount] = 2 * num_points;
        }
    }

    return (selectCounts[0] == totalCount);
}

/*-------------------------------------------------
 * FUNCTION: mx_polypoint::dist2
 *
 * DESCRIPTION:
 *
 *
 */

int mx_polypoint::wholeSelectCount()
{
    int noc, count;

    components(noc, count);

    return count;
}

/*-------------------------------------------------
 * FUNCTION: mx_polypoint::dist2
 *
 * DESCRIPTION:
 *
 *
 */

void mx_polypoint::components(int& noComponentsCount,
    int& wholeCount) const
{
    noComponentsCount = 2 * num_points;
    if (is_closed)
        noComponentsCount++;

    wholeCount = noComponentsCount + 1;
}

/*-------------------------------------------------
 * FUNCTION: mx_polypoint::dist2
 *
 * DESCRIPTION:
 *
 *
 */

bool mx_polypoint::dist2plineComponents(const mx_point& p,
    mx_snap_info& snapInfo,
    double tolerance,
    double& distVal) const
{
    int noComponentsCount;
    int wholeCount;
    int ipoint, ispan;

    distVal = 0;

    components(noComponentsCount, wholeCount);

    if ((!snapInfo.isSnapping()) || (snapInfo.getCount() == noComponentsCount))
        return FALSE;

    for (ipoint = 0; ipoint < num_points; ipoint++) {
        // try snapping to the points
        if (snapInfo.dist2point(p, points[ipoint], ipoint + 1,
                tolerance, distVal))
            return TRUE;
    }

    for (ispan = 0; ispan < num_points - 1; ispan++) {
        mx_rect span(points[ispan], points[ispan + 1], TRUE);

        // try snapping to the start point
        if (snapInfo.dist2span(p, span, num_points + ispan + 1,
                tolerance, distVal))
            return TRUE;
    }

    if (is_closed) {
        mx_rect span(points[num_points - 1], points[0], TRUE);

        // try snapping to the start point
        if (snapInfo.dist2span(p, span, 2 * num_points,
                tolerance, distVal))
            return TRUE;
    }

    if (snapInfo.alreadyFailed(1)) {
        // we have had an intersection with the end points
        // therefore we have snapped to the whole object

        snapInfo.setCount(wholeCount);
        distVal = snapInfo.getDistance();
        return TRUE;
    }

    return FALSE;
}

/*-------------------------------------------------
 * FUNCTION: mx_polypoint::dist2
 *
 * DESCRIPTION:
 *
 *
 */

inline bool mx_polypoint::quickSpanCheck(const mx_point& p,
    const mx_point& startPoint,
    const mx_point& endPoint,
    double testVal) const
{
    double xmin, xmax, ymin, ymax;
    double distx, disty;

    // optimise by finding distance from box

    if (startPoint.x > endPoint.x) {
        xmax = startPoint.x;
        xmin = endPoint.x;
    } else {
        xmin = startPoint.x;
        xmax = endPoint.x;
    }

    if (startPoint.y > endPoint.y) {
        ymax = startPoint.y;
        ymin = endPoint.y;
    } else {
        ymin = startPoint.y;
        ymax = endPoint.y;
    }

    if (p.x < xmin) {
        distx = xmin - p.x;
    } else if (p.x > xmax) {
        distx = p.x - xmax;
    } else {
        distx = 0;
    }

    if (p.y < ymin) {
        disty = ymin - p.y;
    } else if (p.y > ymax) {
        disty = p.y - ymax;
    } else {
        disty = 0;
    }

    return (distx * distx + disty * disty > testVal);
}

/*-------------------------------------------------
 * FUNCTION: mx_polypoint::dist2
 *
 * DESCRIPTION:
 *
 *
 */

double mx_polypoint::dist2properPline(const mx_point& p,
    mx_snap_info& snapInfo,
    double tolerance) const
{
    int noComponents;
    int wholeCount;
    double distVal = 0;
    int ispan;
    mx_snap_info testSnapInfo;
    double testVal;

    components(noComponents, wholeCount);

    if (in_area(p)) {
        // we are in the area so have hit the point
        snapInfo.setPoint(p, 0, wholeCount, 0);
        return 0;
    } else if (snapInfo.isSnapping()) {
        // we've failed to snap
        snapInfo.setCount(MX_GEOM_SNAP_NONE);
        return 0;
    }

    distVal = HUGE_VAL;

    for (ispan = 0; ispan < num_points - 1; ispan++) {
        if (quickSpanCheck(p, points[ispan], points[ispan + 1], distVal))
            continue;

        mx_rect span(points[ispan], points[ispan + 1], TRUE);

        // try snapping to the start point
        testSnapInfo.dist2span(p, span, num_points + ispan,
            tolerance, testVal);

        if (testVal < distVal) {
            snapInfo = testSnapInfo;
            distVal = testVal;
        }
    }

    if (is_closed) {
        if (!quickSpanCheck(p, points[num_points - 1], points[0], distVal)) {
            mx_rect span(points[num_points - 1], points[0], TRUE);

            // try snapping to the start point
            testSnapInfo.dist2span(p, span, 2 * num_points,
                tolerance, testVal);

            if (testVal < distVal) {
                snapInfo = testSnapInfo;
                distVal = testVal;
            }
        }
    }

    return distVal;
}

/*-------------------------------------------------
 * FUNCTION: mx_polypoint::dist2
 *
 * DESCRIPTION:
 *
 *
 */

double mx_polypoint::dist2snap(const mx_point& p,
    mx_snap_info& snapInfo,
    double tolerance) const
{
    double distVal;

    if (num_points == 0) {
        mx_point tempPoint(HUGE_VAL, HUGE_VAL);

        snapInfo.setPoint(tempPoint, HUGE_VAL,
            MX_GEOM_SNAP_NONE, 0);
        return HUGE_VAL;
    }

    // have we managed to snap to any components
    if (dist2plineComponents(p, snapInfo, tolerance, distVal))
        return distVal;

    return dist2properPline(p, snapInfo, tolerance);
}

/*-------------------------------------------------
 * FUNCTION: mx_polypoint::coincidentEnds
 *
 * DESCRIPTION:
 *
 *
 */

bool mx_polypoint::coincidentEnds()
{
    if (num_points == 0) {
        return TRUE;
    } else {
        return (points[0] == points[num_points - 1]);
    }
}

/*-------------------------------------------------
 * FUNCTION: gmx_polypoint::gmx_polypoint
 *
 * DESCRIPTION:
 *
 *
 */

gmx_polypoint::gmx_polypoint(const mx_polypoint& pl1)
{
    pl.copy(pl1);
}

/*-------------------------------------------------
 * Function: gmx_polypoint::intersectBox
 *
 * DESCRIPTION:
 *
 *
 */

GEOM_INTERSECT_TYPE gmx_polypoint::intersectBox(int& err, mx_rect& r, double tolerance)
{
    err = MX_ERROR_OK;
    return GEOM_INTERSECT_NONE;
}

/*-------------------------------------------------
 * FUNCTION: gmx_polypoint::get_serialise_size
 *
 * DESCRIPTION:
 *
 *
 */

uint32 gmx_polypoint::get_serialised_size(int& err)
{
    int size;

    err = MX_ERROR_OK;

    size = mx_geom::get_serialised_size(err);
    MX_ERROR_CHECK(err);

    size += pl.get_serialised_size(err);
    MX_ERROR_CHECK(err);
abort:
    return size;
}

/*-------------------------------------------------
 * FUNCTION: mx_gpolypoint::serialise
 *
 * DESCRIPTION:
 *
 *
 */

void gmx_polypoint::serialise(int& err,
    unsigned char** buffer)
{
    err = MX_ERROR_OK;

    mx_geom::serialise(err, buffer);
    MX_ERROR_CHECK(err);

    pl.serialise(err, buffer);
    MX_ERROR_CHECK(err);

    return;
abort:
    return;
}

/*-------------------------------------------------
 * FUNCTION: gmx_polypoint::unserialise
 *
 * DESCRIPTION:
 *
 *
 */

void gmx_polypoint::unserialise(int& err,
    unsigned char** buffer)
{
    mx_geom::unserialise(err, buffer);
    MX_ERROR_CHECK(err);

    pl.unserialise(err, buffer);
    MX_ERROR_CHECK(err);

    return;
abort:
    return;
}

void mx_polypoint::copyPoints(const mx_polypoint& pp,
    int startPoint,
    int endPoint)
{
    for (int ipoint = startPoint; ipoint < endPoint; ipoint++) {
        addPoint(pp[ipoint]);
    }
}

void mx_ipolypoint::copyPoints(const mx_ipolypoint& pp,
    int startPoint,
    int endPoint)
{
    for (int ipoint = startPoint; ipoint < endPoint; ipoint++) {
        addPoint(pp[ipoint]);
    }
}

void mx_polypoint::copyPoints(const mx_ipolypoint& pp,
    int startPoint,
    int endPoint)
{
    for (int ipoint = startPoint; ipoint < endPoint; ipoint++) {
        addPoint(pp[ipoint]);
    }
}

void mx_polypoint::copyBasics(const mx_polypoint& pp)
{
    is_closed = pp.get_closed();
    is_area = pp.get_area();
    type = pp.get_type();
    allowCoincident = pp.get_allow_coincident();
}

void mx_polypoint::copyBasics(const mx_ipolypoint& pp)
{
    is_closed = pp.get_closed();
    is_area = pp.get_area();
    type = pp.get_type();
    allowCoincident = pp.get_allow_coincident();
}

void mx_ipolypoint::copyBasics(const mx_ipolypoint& pp)
{
    is_closed = pp.get_closed();
    is_area = pp.get_area();
    type = pp.get_type();
    allowCoincident = pp.get_allow_coincident();
}

void mx_polypoint::append(const mx_polypoint& rest)
{
    // only use integer values here
    int istartPoint = (int)(rest.getStartMask() + GEOM_SMALL);
    int iendPoint = rest.get_num_points()
        - (int)(rest.getEndMask() + GEOM_SMALL);

    copyPoints(rest, istartPoint, iendPoint);
}

bool mx_polypoint::split(int& err,
    int32 maxSize,
    int32 inSegmentId,
    int32& outSegmentId,
    mx_polypoint& startPolypoint)
{
    int size;
    int ipoint;
    int startPoint;

    err = MX_ERROR_OK;

    // the maximum number of points we
    // can save

    size = basicSize;

    if (inSegmentId == 0) {
        startPoint = 0;
    } else if (inSegmentId == get_num_points()) {
        return FALSE;
    } else {
        startPoint = inSegmentId - 1;
    }

    for (ipoint = startPoint; ipoint < get_num_points(); ipoint++) {
        if (size > maxSize)
            break;

        size += points[ipoint].get_serialised_size(err);
        MX_ERROR_CHECK(err);
    }

    // the polyline doesnt need to be split
    if ((startPoint == 0) && (ipoint == get_num_points()))
        return FALSE;

    startPolypoint.copyBasics(*this);
    startPolypoint.copyPoints(*this, startPoint, ipoint);

    outSegmentId = ipoint;

abort:;
    return TRUE;
}

int gmx_polypoint::segment(int& err,
    int32 maxSize,
    int32 segmentId,
    mx_geom** splitGeometry)
{
    bool hasSplit;
    int32 retSegmentId;
    bool doingStart = (segmentId == -1);
    gmx_polypoint* splitPolypoint = new gmx_polypoint;

    err = MX_ERROR_OK;

    if (doingStart)
        segmentId = 0;

    hasSplit = pl.split(err, maxSize, segmentId, retSegmentId,
        splitPolypoint->pl);
    MX_ERROR_CHECK(err);

    if (!hasSplit) {
        delete splitPolypoint;
        splitPolypoint = NULL;
        retSegmentId = -1;
    } else {
        splitPolypoint->styleId = styleId;
        splitPolypoint->colourId = colourId;
    }

    *splitGeometry = splitPolypoint;

abort:
    return retSegmentId;
}

int gmx_polypoint::fuse(int& err,
    mx_geom* restGeometry,
    bool isFirst)
{
    gmx_polypoint* pp;

    err = MX_ERROR_OK;

    switch (restGeometry->getType()) {
    case MX_GEOM_POLYPOINT:
        pp = (gmx_polypoint*)restGeometry;

        if (isFirst) {
            pl.addPoint(pp->pl[0]);
        } else {
            // dont want first point
            pp->pl.setStartMask(1.0);
            pl.append(pp->pl);
            pl.unsetMask();
        }
        break;
    default:
        break;
    }

    return pl.get_num_points();
}
