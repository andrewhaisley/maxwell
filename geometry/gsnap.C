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
 * MODULE : gsnap.C
 *
 * AUTHOR : David Miller
 *
 * This file
 *
 * DESCRIPTION:
 * Module gsnap.C
 *
 *
 * Revision 1.1  1996/10/06 13:15:45  andrew
 * Initial revision
 *
 *
 */

#include "geometry.h"

mx_snap_info defaultSnapInfo;

mx_snap_info::mx_snap_info(bool isnapMode,
    double isnapTolerance)

{
    snapMode = isnapMode;

    // no snap tolerance in non-snap mode
    if (!isnapMode)
        isnapTolerance = 0;

    snapTolerance = isnapTolerance;
    snapTolerance2 = isnapTolerance * isnapTolerance;

    init();
}

void mx_snap_info::init()
{
    count = MX_GEOM_SNAP_NONE;
    d = -1.0;
    offset = -1.0;
    geom = NULL;
    testTolerance = 0;
}

void mx_snap_info::copyBasics(const mx_snap_info& isnapInfo)
{
    snapMode = isnapInfo.snapMode;
    snapTolerance = isnapInfo.snapTolerance;
    snapTolerance2 = isnapInfo.snapTolerance2;
}

/*-------------------------------------------------
 * FUNCTION: mx_snap_info::setPoint
 *
 * DESCRIPTION: We have found a point. Set it
 *              if we are not in snap mode.
 *              Set in snap mode only if it
 *              is within tolerance
 *
 */

bool mx_snap_info::setPoint(const mx_point& snapPoint,
    double idistance,
    int icount,
    double ioffset)
{
    p = snapPoint;
    d = idistance;
    offset = ioffset;

    // have we found a point

    if ((!snapMode) || (idistance <= snapTolerance2)) {
        count = icount;
        return TRUE;
    } else {
        // failed to snap
        // dont upgrade count
        return FALSE;
    }
}

/*-------------------------------------------------
 * FUNCTION: mx_snap_info::alreadyFailed
 *
 * DESCRIPTION: Test if a component has already
 *              been tested
 */

bool mx_snap_info::alreadyFailed(int testCount)
{
    return (!snapMode) || (count >= testCount);
}

/*-------------------------------------------------
 * FUNCTION: mx_snap_info::transferSnap
 *
 * DESCRIPTION: When in snap mode translate a
 *              snap count in a subcomponent
 *              to the snap count in the larger
 *              component
 */

bool mx_snap_info::transferSnap(mx_snap_info& testSnap,
    int successCount,
    int newCount)
{
    // failed if not in snap mode or the
    // test snap has not reached the standard required

    if ((!snapMode) || (testSnap.count >= successCount)) {
        p = testSnap.p;
        d = testSnap.d;
        offset = testSnap.offset;

        count = newCount;
        return TRUE;
    }

    return FALSE;
}

/*-------------------------------------------------
 * FUNCTION: mx_snap_info::dist2point
 *
 * DESCRIPTION: Get the distance to a point. If in
 * snapping mode and we do snap we update the
 * testCount to be equal to the input component
 * number
 *
 */

bool mx_snap_info::dist2point(const mx_point& p,
    const mx_point& testPoint,
    int testCount,
    double tolerance,
    double& distVal)
{
    mx_snap_info snapInfoPoint;

    distVal = 0;

    // is this going to fail straightaway
    if (snapMode && alreadyFailed(testCount))
        return FALSE;

    // set the tolerances on the point snap info
    snapInfoPoint.copyBasics(*this);

    // test the point
    distVal = testPoint.dist2snap(p, snapInfoPoint, tolerance);

    // reset if it is a succesful snap
    return transferSnap(snapInfoPoint,
        MX_POINT_SNAP_WHOLE,
        testCount);
}

/*-------------------------------------------------
 * FUNCTION: mx_snap_info::dist2span
 *
 * DESCRIPTION: Get the distance to a span
 *
 */

bool mx_snap_info::dist2span(const mx_point& p,
    const mx_rect& testSpan,
    int testCount,
    double tolerance,
    double& distVal)
{
    mx_snap_info snapInfoSpan;

    distVal = 0;

    if (snapMode && alreadyFailed(testCount))
        return FALSE;

    snapInfoSpan.copyBasics(*this);

    // only interested in snapping to the whole span
    snapInfoSpan.setCount(MX_SPAN_SNAP_NO_COMPONENTS);

    // see if we have managed to snap to the span
    distVal = testSpan.dist2snap(p, snapInfoSpan, tolerance);

    return transferSnap(snapInfoSpan,
        MX_SPAN_SNAP_WHOLE,
        testCount);
}

/*-------------------------------------------------
 * FUNCTION: mx_snap_info::dist2arc
 *
 * DESCRIPTION: Get the distance to an arc span
 *
 */

bool mx_snap_info::dist2arc(const mx_point& p,
    const mx_arc& testArc,
    int testCount,
    double tolerance,
    double& distVal)
{
    mx_snap_info snapInfoArc;

    distVal = 0;

    if (snapMode && alreadyFailed(testCount))
        return FALSE;

    snapInfoArc.copyBasics(*this);

    // only interested in snapping to the whole span
    snapInfoArc.setCount(MX_ARC_SNAP_NO_COMPONENTS);

    // see if we have managed to snap to the span
    distVal = testArc.dist2arc(p, snapInfoArc, tolerance);

    return transferSnap(snapInfoArc,
        MX_ARC_SNAP_WHOLE,
        testCount);
}

/*-------------------------------------------------
 * FUNCTION: mx_snap_info::combine
 *
 * DESCRIPTION:
 *
 */

double mx_snap_info::combine(int numSnapInfos,
    mx_snap_info* snapInfos)
{
    int iinfo;

    d = HUGE_VAL;

    for (iinfo = 0; iinfo < numSnapInfos; iinfo++) {
        if (snapInfos[iinfo].d < d) {
            d = snapInfos[iinfo].d;
            p = snapInfos[iinfo].p;
            offset = snapInfos[iinfo].offset;
        }
    }

    return d;
}
