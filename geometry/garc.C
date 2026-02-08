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
 * MODULE : garc.C
 *
 * AUTHOR : David Miller
 *
 *
 *
 * DESCRIPTION: Conic section stuff
 * Module garc.C
 *
 *
 *
 *
 */

#include <geometry.h>
#include <mx.h>

double mx_arc::cfactor = (360.0 * 64) / PITWO;

inline bool inRange(double bot,
    double top,
    double value)
{
    return (value >= bot) && (value <= top);
}

/*-------------------------------------------------
 * FUNCTION: mx_arc::mx_arc
 *
 * DESCRIPTION:
 *
 *
 */

mx_arc::mx_arc(const mx_point& ic,
    const mx_point& size,
    double startAng,
    double endAng,
    ARC_TYPE iarcType,
    bool iisArea,
    double iangle)
{
    setArc(ic, size, startAng, endAng,
        iarcType, iisArea, iangle);
}

/*-------------------------------------------------
 * FUNCTION: mx_arc::setArc
 *
 * DESCRIPTION:
 *
 *
 */

void mx_arc::setArc(const mx_point& ic,
    const mx_point& size,
    double startAng,
    double endAng,
    ARC_TYPE iarcType,
    bool iisArea,
    double iangle)
{
    arcType = iarcType;

    c = ic;
    s = size;
    isArea = iisArea;

    angle.set(iangle);

    // Get startAng in range [0,2*pi) -
    // endAng is greater than startAng and
    // hence could be in the range [0,4*pi)

    while (startAng >= PITWO)
        startAng -= PITWO;
    while (startAng < 0)
        startAng += PITWO;

    while (endAng > PITWO)
        endAng -= PITWO;
    while (endAng < 0)
        endAng += PITWO;

    if (endAng < startAng)
        endAng += PITWO;

    sa.set(startAng);
    startArc = ac(startAng);

    ea.set(endAng);
    endArc = ac(endAng);
}

/*-------------------------------------------------
 * FUNCTION: mx_arc::tangent
 *
 * DESCRIPTION: Return the inward arc normal
 *
 *
 */

mx_point mx_arc::tangent(double ia)
{
    mx_point pt(-s.x * sin(ia), s.y * cos(ia));

    if (angle.a() == 0.0) {
        return pt.uspan();
    } else {
        mx_rotation rot(c, angle);

        return pt = pt.uspan().rotate(rot);
    }
}

/*-------------------------------------------------
 * FUNCTION: mx_arc::normal
 *
 * DESCRIPTION: Return the inward arc normal
 *
 *
 */

mx_point mx_arc::normal(double angle)
{
    return tangent(angle).flip();
}

/*-------------------------------------------------
 * FUNCTION: mx_arc::normalisedArc
 *
 * Rotate a  point so that it is normalised
 * with respect to the rotation angle and centre
 */

mx_arc mx_arc::normalisedArc() const
{
    mx_arc retArc;

    retArc.c.x = retArc.c.y = 0;

    retArc.setArc(retArc.c,
        this->s,
        this->sa.a(),
        this->ea.a(),
        this->arcType,
        this->isArea,
        0);

    return retArc;
}

/*-------------------------------------------------
 * FUNCTION: mx_arc::normalisePoint
 *
 * Rotate a  point so that it is normalised
 * with respect to the rotation angle and centre
 */

mx_point mx_arc::normalisedPoint(const mx_point& p) const
{
    mx_point offset = p - c;

    if (angle.a() != 0) {
        // rotate the point back
        mx_rotation backRotation(-angle.a());
        offset = offset.rotate(backRotation);
    }

    return offset;
}

/*-------------------------------------------------
 * FUNCTION: mx_arc::unnormalisePoint
 *
 * Rotate a  point in normalised coordinates so
 * that it is in real coordinates
 *
 */

mx_point mx_arc::unnormalisedPoint(const mx_point& p) const
{
    mx_point offset;

    if (angle.a() != 0) {
        // rotate the point back
        mx_rotation forwardRotation(angle.a());
        offset = p.rotate(forwardRotation);
    } else {
        offset = p;
    }

    offset += c;

    return offset;
}

/*-------------------------------------------------
 * FUNCTION: mx_arc::pointToQuadrant
 *
 * DESCRIPTION: Get the quadrant a point lies in
 *
 */

int mx_arc::quadrant(const mx_point& p) const
{
    mx_angle ang(pointToAngle(p));

    return ang.quadrant();
}

/*-------------------------------------------------
 * FUNCTION: mx_arc::pointToDist
 *
 * DESCRIPTION: Get a non-dimensional estimate of
 * the distance of a point from the centre of the
 * ellipse. The ellipse determines a coordinate
 * system - based about its aspect ratio. The
 * ellipse itself is at a distance of 1.
 *
 * If pointToDist is less than one the point is
 * within the ellipse.
 */

double mx_arc::pointToDist(const mx_point& p) const
{
    mx_point offset = normalisedPoint(p);
    double d1;
    double d2;

    if (pointEllipse()) {
        if ((offset.x == 0.0) && (offset.y == 0.0)) {
            return 1;
        } else {
            return HUGE_VAL;
        }
    } else if (xLine()) {
        if (offset.y == 0.0) {
            d1 = offset.x / s.x;
            return d1 * d1;
        } else {
            return HUGE_VAL;
        }
    } else if (yLine()) {
        if (offset.x == 0.0) {
            d2 = offset.y / s.y;
            return d2 * d2;
        } else {
            return HUGE_VAL;
        }
    } else {
        d1 = offset.x / s.x;
        d2 = offset.y / s.y;
        return (d1 * d1 + d2 * d2);
    }
}

/*-------------------------------------------------
 * FUNCTION: mx_arc::pointToAngle
 *
 * DESCRIPTION: Return the ellipse angle of
 *              a point. This is the angle
 *              used in the ellipse parametrisation
 *              not the euclidean angle from
 *              the centre
 * The ellipse determines a coordinate
 * system - based about its aspect ratio. The
 * lines of constant angle are straight lines from
 * the ellipse centre, but the angle is not necessarily
 * the same as the euclidean angle (is for a circle).
 */

double mx_arc::pointToAngle(const mx_point& p) const
{
    mx_point offset = normalisedPoint(p);

    if ((offset.x == 0.0) && (offset.y == 0.0)) {
        // point is at centre - set to mid angle
        return (sa.a() + ea.a()) * 0.5;
    } else if (pointEllipse()) {
        // arc is degenerate in size - assume s.x = s.y
        return gatan2(offset.y, offset.x, sa.a());
    } else {
        return gatan2(offset.y * s.x, offset.x * s.y, sa.a());
    }
}

/*-------------------------------------------------
 * FUNCTION: mx_arc::isPoint
 *
 * DESCRIPTION:
 *
 *
 */

bool mx_arc::isPoint() const
{
    return (sa.a() == ea.a()) || (pointEllipse());
}

/*-------------------------------------------------
 * FUNCTION: arcDistanceMinimiser
 *
 * DESCRIPTION: minimising function
 *
 *
 */

static double arcDistanceMinimiser(double testValue,
    void* funcData)
{
    mxArcDistanceData* madd = (mxArcDistanceData*)funcData;
    double dist = madd->arc->ac(testValue).dist2(madd->point);

    return (madd->doMin) ? dist : -dist;
}

/*-------------------------------------------------
 * FUNCTION: nearQuads
 *
 * DESCRIPTION: Find the quads which might require
 *              iteration to find the nearest point
 *
 */

void mx_arc::nearQuads(const mx_point& p,
    mx_ipoint& inearQuads) const
{
    int iquad = inearQuads.x = quadrant(p);

    // the next quad is determined from the following (?)
    ((iquad % 2 == 0) == (s.x > s.y)) ? iquad-- : iquad++;

    if (iquad == 4) {
        iquad = 0;
    } else if (iquad == -1) {
        iquad = 3;
    }

    inearQuads.y = iquad;
}

/*-------------------------------------------------
 * FUNCTION: dist2quad
 *
 * DESCRIPTION: Find the nearest point in a quad
 *              and return whether it really
 *              lies in the arc
 *
 *   Returns true if a point has been found
 */

bool mx_arc::dist2quad(const mx_point& p,
    mx_point& outPoint,
    int iquad,
    bool clipToArc,
    double tolerance,
    double& distVal,
    double angle) const
{
    double iangle;
    mx_point angles = mx_angle::squadrantRange(iquad);
    mxArcDistanceData minData(this, p, TRUE);
    mx_ipoint testQuads;
    double testVal;
    mx_point testPoint;

    distVal = 0;

    // get the range to search in

    if (angles.x < sa.a()) {
        angles.x += PITWO;
        angles.y += PITWO;
    }

    if (clipToArc) {
        angles.x = GMAX(angles.x, sa.a());
        angles.y = GMIN(angles.y, ea.a());
    }

    if (clipToArc && (angles.x > angles.y))
        return FALSE;

    // test the end points
    outPoint = ac(angles.x);
    distVal = outPoint.dist2(p);
    angle = angles.x;

    testPoint = ac(angles.y);
    testVal = testPoint.dist2(p);

    if (testVal < distVal) {
        outPoint = testPoint;
        distVal = testVal;
        angle = angles.y;
    }

    // test if this is a quad to iterate over
    nearQuads(p, testQuads);

    if ((iquad != testQuads.x) && (iquad != testQuads.y))
        return TRUE;

    // need to iterate to solution
    iangle = gminimizer(angles.x,
        angles.y,
        tolerance,
        arcDistanceMinimiser,
        &minData,
        testVal);

    if (iangle < sa.a())
        iangle += PITWO;

    if (testVal < distVal) {
        outPoint = ac(iangle);
        distVal = testVal;
        angle = iangle;
    }

    return TRUE;
}

/*-------------------------------------------------
 * FUNCTION: mx_arc::arcToQuadrants
 *
 * DESCRIPTION: Try to snap to the end points
 *
 */

void mx_arc::arcToQuadrants(bool* quad) const
{
    if (sa.a() <= M_PI_2) {
        quad[0] = TRUE;
        quad[1] = (ea.a() >= M_PI_2);
        quad[2] = (ea.a() >= M_PI);
        quad[3] = (ea.a() >= M_PI + M_PI_2);
    } else if (sa.a() <= M_PI) {
        quad[1] = TRUE;
        quad[2] = (ea.a() >= M_PI);
        quad[3] = (ea.a() >= M_PI + M_PI_2);
        quad[0] = (ea.a() >= PITWO);
    } else if (sa.a() <= M_PI + M_PI_2) {
        quad[2] = TRUE;
        quad[3] = (ea.a() >= M_PI + M_PI_2);
        quad[0] = (ea.a() >= PITWO);
        quad[1] = (ea.a() >= PITWO + M_PI_2);
    } else {
        quad[3] = TRUE;
        quad[0] = (ea.a() >= PITWO);
        quad[1] = (ea.a() >= PITWO + M_PI_2);
        quad[2] = (ea.a() >= PITWO + M_PI);
    }
}

/*-------------------------------------------------
 * FUNCTION: mx_arc::dist2degenerateArcArc
 *
 * DESCRIPTION: Try to snap to the end points
 *
 */

bool mx_arc::dist2degenerateArcArc(const mx_point& p,
    mx_snap_info& snapInfo,
    double tolerance,
    double& distVal) const
{
    double pp, mp;
    bool quads[4];

    // get the quads the arc uses
    arcToQuadrants(quads);

    distVal = 0;

    // degenerate cases

    if (isPoint()) {
        if (snapInfo.isSnapping()) {
            // weve already failed on end points
            snapInfo.setCount(MX_GEOM_SNAP_NONE);
        } else {
            distVal = startArc.dist2(p);

            snapInfo.setPoint(startArc, distVal,
                MX_ARC_SNAP_WHOLE, 0);
        }
        return TRUE;
    } else if (xLine()) {
        // distance from horizontal line
        // half or full line

        pp = (quads[0] || quads[3]) ? c.x + s.x : c.x;
        mp = (quads[1] || quads[2]) ? c.x - s.x : c.x;

        mx_point sp(mp, c.y);
        mx_point se(pp, c.y);
        mx_rect r(sp, se, TRUE);

        snapInfo.dist2span(p, r, MX_ARC_SNAP_WHOLE,
            tolerance, distVal);

        return TRUE;
    } else if (yLine()) {
        // distance from vertical line
        // half or full line

        pp = (quads[0] || quads[1]) ? c.y + s.y : c.y;
        mp = (quads[2] || quads[3]) ? c.y - s.y : c.y;

        mx_point sp(c.x, mp);
        mx_point se(c.x, pp);
        mx_rect r(sp, se, TRUE);

        snapInfo.dist2span(p, r, MX_ARC_SNAP_WHOLE,
            tolerance, distVal);

        return TRUE;
    }

    return FALSE;
}

/*-------------------------------------------------
 * FUNCTION: mx_arc::dist2arcArcComponents
 *
 * DESCRIPTION: Find the closest approach to a
 * curved part of the arc. This
 * deals only with the arc part of the arc
 * not the chord or spokes etc.
 *
 */

bool mx_arc::dist2arcArcComponents(const mx_point& p,
    mx_snap_info& snapInfo,
    double tolerance,
    double& distVal) const
{
    distVal = 0;

    if ((!snapInfo.isSnapping()) || (snapInfo.getCount() == MX_ARC_SNAP_NO_COMPONENTS))
        return FALSE;

    if (snapInfo.alreadyFailed(MX_ARC_SNAP_WHOLE)) {
        snapInfo.setCount(MX_GEOM_SNAP_NONE);
        return TRUE;
    }

    // try snapping to the start point
    if (snapInfo.dist2point(p, startArc, MX_ARC_SNAP_START,
            tolerance, distVal))
        return TRUE;

    // try snapping to the end point
    if (snapInfo.dist2point(p, endArc, MX_ARC_SNAP_END,
            tolerance, distVal))
        return TRUE;

    if (snapInfo.alreadyFailed(MX_ARC_SNAP_START)) {
        // we have had an intersection with the end points
        // therefore we have snapped to the whole object

        snapInfo.setCount(MX_ARC_SNAP_WHOLE);
        distVal = snapInfo.getDistance();
        return TRUE;
    }

    return FALSE;
}

/*-------------------------------------------------
 * FUNCTION: mx_arc::nearestEndPoint
 *
 * DESCRIPTION: Find the closest approach to a
 * curved part of the arc. This
 * deals only with the arc part of the arc
 * not the chord or spokes etc.
 *
 */

double mx_arc::nearestEndPoint(const mx_point& p,
    mx_point& outPoint,
    bool& isStart) const
{
    double distVal1 = startArc.dist2(p);
    double distVal2 = endArc.dist2(p);

    isStart = (distVal1 < distVal2);

    if (isStart) {
        outPoint = startArc;
        return distVal1;
    } else {
        outPoint = endArc;
        return distVal2;
    }
}

/*-------------------------------------------------
 * FUNCTION: mx_arc::dist2arc
 *
 * DESCRIPTION: Find the closest approach to a
 * curved part of the arc. This
 * deals only with the arc part of the arc
 * not the chord or spokes etc.
 *
 */

double mx_arc::dist2arc(const mx_point& p,
    double tolerance,
    mx_point& outPoint,
    double& outAngle) const
{
    double distVal = 0;
    bool isStart;

    if (isCircle()) {
        if (distSector(p) <= 0) {
            // the point is the point with the same angle

            outAngle = pointToAngle(p);
            outPoint = ac(outAngle);
            distVal = outPoint.dist2(p);
        } else {
            distVal = nearestEndPoint(p, outPoint, isStart);
            outAngle = isStart ? sa.a() : ea.a();
        }
    } else {
        // we need to iterate to the nearest point
        mx_ipoint inearQuads;
        double testVal1, testAngle1 = 0.0, testVal2, testAngle2 = 0.0;
        mx_point testPoint1, testPoint2;

        nearQuads(p, inearQuads);

        if (!dist2quad(p, testPoint1,
                inearQuads.x,
                TRUE,
                tolerance,
                testVal1,
                testAngle1))
            testVal1 = HUGE_VAL;

        if (!dist2quad(p, testPoint2,
                inearQuads.y,
                TRUE,
                tolerance,
                testVal2,
                testAngle2))
            testVal2 = HUGE_VAL;

        if ((testVal1 == HUGE_VAL) && (testVal2 == HUGE_VAL)) {
            // its an end point
            distVal = nearestEndPoint(p, outPoint, isStart);
            outAngle = isStart ? sa.a() : ea.a();
        } else {
            if (testVal1 < testVal2) {
                distVal = testVal1;
                outPoint = testPoint1;
                outAngle = testAngle1;
            } else {
                distVal = testVal2;
                outPoint = testPoint2;
                outAngle = testAngle2;
            }
        }
    }

    return distVal;
}

/*-------------------------------------------------
 * FUNCTION: mx_arc::dist2properArcArc
 *
 * DESCRIPTION: Find the closest approach to a
 * curved part of the arc. This
 * deals only with the arc part of the arc
 * not the chord or spokes etc.
 *
 */

double mx_arc::dist2properArcArc(const mx_point& p,
    mx_snap_info& snapInfo,
    double tolerance) const
{
    mx_point outp;
    double offset;
    double distVal;

    if (snapInfo.isSnapping()) {
        if (isCircle()) {
            if (distSector(p) > 0) {
                snapInfo.setCount(MX_GEOM_SNAP_NONE);
                return 0;
            }
        } else {
            mx_ipoint inearQuads;
            int iquad = quadrant(p);

            // get the near quads
            nearQuads(p, inearQuads);

            if ((inearQuads.x != iquad) && (inearQuads.y != iquad)) {
                snapInfo.setCount(MX_GEOM_SNAP_NONE);
                return 0;
            }
        }
    }

    distVal = dist2arc(p, tolerance, outp, offset);

    snapInfo.setPoint(outp,
        distVal,
        MX_ARC_SNAP_WHOLE,
        offset);

    return distVal;
}

/*-------------------------------------------------
 * FUNCTION: mx_arc::dist2arc
 *
 * DESCRIPTION: Find the closest approach to a
 * curved part of the arc. This
 * deals only with the arc part of the arc
 * not the chord or spokes etc.
 *
 */

double mx_arc::dist2arc(const mx_point& p,
    mx_snap_info& snapInfo,
    double tolerance) const
{
    double distVal = 0;
    mx_point tp;

    mx_point np = normalisedPoint(p);
    mx_arc na = normalisedArc();

    // have we finished this arc or managed to snap onto an end point
    if ((!na.dist2arcArcComponents(np, snapInfo, tolerance, distVal)) && (!na.dist2degenerateArcArc(np, snapInfo, tolerance, distVal))) {
        distVal = na.dist2properArcArc(np, snapInfo, tolerance);
    }

    // now we need to denormalise the snap
    // point

    tp = snapInfo.getPoint();

    snapInfo.setPoint(unnormalisedPoint(tp));

    return distVal;
}

/*-------------------------------------------------
 * FUNCTION: mx_arc::distSector
 *
 * DESCRIPTION: Test if a point is in the sector
 *
 * DESCRIPTION: Positive - outside sector
 *              Zero     - on sector
 *              Negative - inside sector
 *
 */

double mx_arc::distSector(const mx_point& p) const
{
    double testAngle = pointToAngle(p);
    double answer;

    // bound to be in sector
    if (isFull()) {
        // return point in centre
        answer = -PI;
    } else {
        double test1 = sa.a() - testAngle;
        double test2 = testAngle - ea.a();

        answer = GMAX(test1, test2);
    }

    return answer;
}

/*-------------------------------------------------
 * FUNCTION: mx_arc::distArc
 *
 * DESCRIPTION: Positive - outside ellipse
 *              Zero     - on ellipse
 *              Negative - inside ellipse
 *
 */

double mx_arc::distArc(const mx_point& p) const
{
    return (pointToDist(p) - 1);
}

/*-------------------------------------------------
 * FUNCTION: mx_arc::inArea
 *
 * DESCRIPTION: Test if a point is in an area. If not
 * return the component that of the arc that is nearest to
 * the point
 */

bool mx_arc::inArea(const mx_point& p,
    double tolerance,
    ARC_TYPE* nearnessType) const
{
    mx_point centreOffset = p - c;

    bool inSector = (distSector(p) < tolerance);
    bool inWholeEllipse = (distArc(p) < tolerance);

    if (arcType == ARC_SECTOR) {
        if (inSector && inWholeEllipse)
            return TRUE;

        if (!inSector) {
            *nearnessType = ARC_SECTOR;
            return FALSE;
        } else {
            *nearnessType = ARC_ARC;
            return FALSE;
        }
    }

    // chord type - include ARC_ARC

    mx_point sp = startArc - c;
    mx_point ep = endArc - c;

    if (ea.a() - sa.a() > PI) {
        // flipped - angle is over PI

        // If in sector is in the area if inside ellipse
        if (!inSector) {
            // else needs to be nearer to the
            // origin than the two end points

            // solve d = A * sp + B * ep
            // and want A+B > 1.0

            double deter = sp ^ ep;

            // get coordinates of d in terms of two end spans
            double A = (centreOffset ^ ep) / deter;
            double B = -(centreOffset ^ sp) / deter;

            // Test if is further away from span
            if ((A + B) >= 1.0) {
                *nearnessType = ARC_CHORD;
                return FALSE;
            }
        } else if (!inWholeEllipse) {
            *nearnessType = ARC_ARC;
            return FALSE;
        } else {
            return TRUE;
        }
    }

    // small chord
    if (!inSector) {
        *nearnessType = ARC_CHORD;
        return FALSE;
    } else {
        // is in sector

        if (!inWholeEllipse) {
            *nearnessType = ARC_ARC;
            return FALSE;
        } else {
            double deter = sp ^ ep;

            // get coordinates of d in terms of two end spans
            double A = (centreOffset ^ ep) / deter;
            double B = -(centreOffset ^ sp) / deter;

            // Test if is nearer from the origin
            if ((A + B) <= 1.0) {
                *nearnessType = ARC_CHORD;
                return FALSE;
            }
        }
    }
    return TRUE;
}

/*-------------------------------------------------
 * FUNCTION: mx_arc::dist2degenerateArc
 *
 * DESCRIPTION:
 *
 *
 */

bool mx_arc::dist2degenerateArc(const mx_point& p,
    mx_snap_info& snapInfo,
    double tolerance,
    double& distVal) const
{
    if (isDegenerate()) {
        if (snapInfo.isSnapping()) {
            // weve already failed on end points
            snapInfo.setCount(MX_GEOM_SNAP_NONE);
        } else {
            dist2degenerateArcArc(p, snapInfo, tolerance, distVal);
        }
        return TRUE;
    } else {
        return FALSE;
    }
}

/*-------------------------------------------------
 * FUNCTION: mx_arc::dist2arcComponents
 *
 * DESCRIPTION:
 *
 *
 */

bool mx_arc::dist2arcComponents(const mx_point& p,
    mx_snap_info& snapInfo,
    double tolerance,
    double& distVal) const
{
    distVal = 0;
    bool hasSpans = false, hasChord = false;
    int wholeArcType = 0;

    if ((!snapInfo.isSnapping()) || (snapInfo.getCount() == (wholeArcType - 1)))
        return FALSE;

    if (snapInfo.alreadyFailed(wholeArcType)) {
        snapInfo.setCount(MX_GEOM_SNAP_NONE);
        return TRUE;
    }

    // try snapping to the start
    if (snapInfo.dist2point(p, startArc, MX_ARC_SNAP_START,
            tolerance, distVal))
        return TRUE;

    // try snapping to the start
    if (snapInfo.dist2point(p, endArc, MX_ARC_SNAP_END,
            tolerance, distVal))
        return TRUE;

    // try snapping to the centre
    if (snapInfo.dist2point(p, c, MX_ARC_SNAP_CENTRE,
            tolerance, distVal))
        return TRUE;

    // try snapping to the arc
    if (snapInfo.dist2arc(p, *this, MX_ARC_SNAP_ARC,
            tolerance, distVal))
        return TRUE;

    if (hasChord) {
        mx_rect r(startArc, endArc, TRUE);

        // try snapping to the arc
        if (snapInfo.dist2span(p, r, MX_CHORD_SNAP_CHORD,
                tolerance, distVal))
            return TRUE;
    }

    if (hasSpans) {
        mx_rect r1(c, startArc, TRUE);
        mx_rect r2(c, endArc, TRUE);

        if (snapInfo.dist2span(p, r1, MX_SECTOR_SNAP_SPAN1,
                tolerance, distVal))
            return TRUE;

        if (snapInfo.dist2span(p, r2, MX_SECTOR_SNAP_SPAN2,
                tolerance, distVal))
            return TRUE;
    }

    if (snapInfo.alreadyFailed(MX_ARC_SNAP_START)) {
        // we have had an intersection with the end points
        // therefore we have snapped to the whole object

        snapInfo.setCount(wholeArcType);
        distVal = snapInfo.getDistance();
        return TRUE;
    }

    return FALSE;
}

/*-------------------------------------------------
 * FUNCTION: mx_arc::numberComponents
 *
 * DESCRIPTION:
 *
 *
 */

int mx_arc::numberComponents()
{
    return components();
}

/*-------------------------------------------------
 * FUNCTION: mx_arc::wholeSelectCount
 *
 * DESCRIPTION:
 *
 *
 */

int mx_arc::wholeSelectCount()
{
    bool hasChord, hasSector;
    int count = 0;

    components(&hasChord, &hasSector, &count);

    return count;
}

/*-------------------------------------------------
 * FUNCTION: mx_arc::mergeArcSelectCount
 *
 * DESCRIPTION: Constructor from integer point
 *
 *
 */

bool mx_arc::mergeArcSelectCount(int nselectCounts,
    int* selectCounts)
{
    bool inc[3] = { FALSE, FALSE, FALSE };
    int ic, selectCount, icount;

    for (ic = 0; ic < nselectCounts; ic++) {
        selectCount = selectCounts[ic];

        switch (selectCount) {
        case MX_ARC_SNAP_START:
            inc[0] = TRUE;
            break;
        case MX_ARC_SNAP_END:
            inc[1] = TRUE;
            break;
        case MX_ARC_SNAP_CENTRE:
            inc[2] = TRUE;
            break;
        case MX_ARC_SNAP_ARC:
            inc[0] = TRUE;
            inc[1] = TRUE;
            break;
        case MX_ARC_SNAP_WHOLE:
            inc[0] = TRUE;
            inc[1] = TRUE;
            inc[2] = TRUE;
            break;
        default:
            break;
        }

        selectCounts[ic] = -1;
    }

    // count the number of matches
    icount = 0;
    for (ic = 0; ic < 3; ic++) {
        if (inc[ic])
            icount++;
    }

    if (icount == 3) {
        selectCounts[0] = MX_ARC_SNAP_WHOLE;
    } else if (icount == 2) {
        if (inc[0] && inc[1]) {
            selectCounts[0] = MX_ARC_SNAP_ARC;
        } else if (inc[0] && inc[2]) {
            selectCounts[0] = MX_ARC_SNAP_START;
            selectCounts[1] = MX_ARC_SNAP_CENTRE;
        } else {
            selectCounts[0] = MX_ARC_SNAP_START;
            selectCounts[1] = MX_ARC_SNAP_START;
        }
    } else if (icount == 1) {
        if (inc[0]) {
            selectCounts[0] = MX_ARC_SNAP_START;
        } else if (inc[1]) {
            selectCounts[0] = MX_ARC_SNAP_END;
        } else {
            selectCounts[0] = MX_ARC_SNAP_END;
        }
    }

    return (selectCounts[0] == MX_ARC_SNAP_WHOLE);
}

/*-------------------------------------------------
 * FUNCTION: mx_arc::mergeChordSelectCount
 *
 * DESCRIPTION: Constructor from integer point
 *
 *
 */

bool mx_arc::mergeChordSelectCount(int nselectCounts,
    int* selectCounts)
{
    bool inc[3] = { FALSE, FALSE, FALSE };
    int ic, selectCount, icount;

    for (ic = 0; ic < nselectCounts; ic++) {
        selectCount = selectCounts[ic];

        switch (selectCount) {
        case MX_CHORD_SNAP_START:
            inc[0] = TRUE;
            break;
        case MX_CHORD_SNAP_END:
            inc[1] = TRUE;
            break;
        case MX_CHORD_SNAP_CENTRE:
            inc[2] = TRUE;
            break;
        case MX_CHORD_SNAP_ARC:
        case MX_CHORD_SNAP_CHORD:
            inc[0] = TRUE;
            inc[1] = TRUE;
            break;
        case MX_CHORD_SNAP_WHOLE:
            inc[0] = TRUE;
            inc[1] = TRUE;
            inc[2] = TRUE;
            break;
        default:
            break;
        }

        selectCounts[ic] = -1;
    }

    // count the number of matches
    icount = 0;
    for (ic = 0; ic < 3; ic++) {
        if (inc[ic])
            icount++;
    }

    if (icount == 3) {
        selectCounts[0] = MX_CHORD_SNAP_WHOLE;
    } else if (icount == 2) {
        if (inc[0] && inc[1]) {
            selectCounts[0] = MX_CHORD_SNAP_ARC;
        } else if (inc[0] && inc[2]) {
            selectCounts[0] = MX_CHORD_SNAP_START;
            selectCounts[1] = MX_CHORD_SNAP_CENTRE;
        } else {
            selectCounts[0] = MX_CHORD_SNAP_START;
            selectCounts[1] = MX_CHORD_SNAP_START;
        }
    } else if (icount == 1) {
        if (inc[0]) {
            selectCounts[0] = MX_CHORD_SNAP_START;
        } else if (inc[1]) {
            selectCounts[0] = MX_CHORD_SNAP_END;
        } else {
            selectCounts[0] = MX_CHORD_SNAP_END;
        }
    }

    return (selectCounts[0] == MX_CHORD_SNAP_WHOLE);
}

/*-------------------------------------------------
 * FUNCTION: mx_arc::mergeSectorSelectCount
 *
 * DESCRIPTION: Constructor from integer point
 *
 *
 */

bool mx_arc::mergeSectorSelectCount(int nselectCounts,
    int* selectCounts)
{
    bool inc[3] = { FALSE, FALSE, FALSE };
    int ic, selectCount, icount;

    for (ic = 0; ic < nselectCounts; ic++) {
        selectCount = selectCounts[ic];

        switch (selectCount) {
        case MX_SECTOR_SNAP_START:
            inc[0] = TRUE;
            break;
        case MX_SECTOR_SNAP_END:
            inc[1] = TRUE;
            break;
        case MX_SECTOR_SNAP_CENTRE:
            inc[2] = TRUE;
            break;
        case MX_SECTOR_SNAP_ARC:
            inc[0] = TRUE;
            inc[1] = TRUE;
            break;
        case MX_SECTOR_SNAP_SPAN1:
            inc[0] = TRUE;
            inc[2] = TRUE;
            break;
        case MX_SECTOR_SNAP_SPAN2:
            inc[0] = TRUE;
            inc[2] = TRUE;
            break;
        case MX_SECTOR_SNAP_WHOLE:
            inc[0] = TRUE;
            inc[1] = TRUE;
            inc[2] = TRUE;
            break;
        default:
            break;
        }

        selectCounts[ic] = -1;
    }

    // count the number of matches
    icount = 0;
    for (ic = 0; ic < 3; ic++) {
        if (inc[ic])
            icount++;
    }

    if (icount == 3) {
        selectCounts[0] = MX_SECTOR_SNAP_WHOLE;
    } else if (icount == 2) {
        if (inc[0] && inc[1]) {
            selectCounts[0] = MX_SECTOR_SNAP_ARC;
        } else if (inc[0] && inc[2]) {
            selectCounts[0] = MX_SECTOR_SNAP_SPAN1;
        } else {
            selectCounts[0] = MX_SECTOR_SNAP_SPAN2;
        }
    } else if (icount == 1) {
        if (inc[0]) {
            selectCounts[0] = MX_SECTOR_SNAP_START;
        } else if (inc[1]) {
            selectCounts[0] = MX_SECTOR_SNAP_END;
        } else {
            selectCounts[0] = MX_SECTOR_SNAP_END;
        }
    }

    return (selectCounts[0] == MX_SECTOR_SNAP_WHOLE);
}

/*-------------------------------------------------
 * FUNCTION: mx_arc::mergeSelectCount
 *
 * DESCRIPTION:
 *
 *
 */

bool mx_arc::mergeSelectCount(int nselectCounts,
    int* selectCounts)
{
    switch (arcType) {
    case ARC_ARC:
        return mergeArcSelectCount(nselectCounts,
            selectCounts);
        break;
    case ARC_CHORD:
        return mergeChordSelectCount(nselectCounts,
            selectCounts);
        break;
    case ARC_SECTOR:
        return mergeSectorSelectCount(nselectCounts,
            selectCounts);
        break;
    default:
        return FALSE;
    }
}

/*-------------------------------------------------
 * FUNCTION: mx_arc::components
 *
 * DESCRIPTION:
 *
 *
 */

int mx_arc::components(bool* hasChord,
    bool* hasSector,
    int* wholeArcType) const
{
    switch (arcType) {
    case ARC_ARC:
        if (hasChord)
            *hasChord = FALSE;
        if (hasSector)
            *hasSector = FALSE;
        if (wholeArcType)
            *wholeArcType = MX_ARC_SNAP_WHOLE;

        /* start,end,centre,arc */

        return 4;

        break;
    case ARC_CHORD:
        if (hasChord)
            *hasChord = TRUE;
        if (hasSector)
            *hasSector = FALSE;
        if (wholeArcType)
            *wholeArcType = MX_CHORD_SNAP_WHOLE;

        /* start,end,centre,arc,chord,whole */

        return 6;

        break;
    case ARC_SECTOR:
        if (hasChord)
            *hasChord = FALSE;
        if (hasSector)
            *hasSector = TRUE;
        if (wholeArcType)
            *wholeArcType = MX_SECTOR_SNAP_WHOLE;

        /* start,end,centre,arc,span to start, span to end, whole */

        return 7;

        break;
    default:
        return 0;
        break;
    }
}

/*-------------------------------------------------
 * FUNCTION: mx_arc::dist2properArc
 *
 * DESCRIPTION:
 *
 *
 */

double mx_arc::dist2properArc(const mx_point& p,
    mx_snap_info& snapInfo,
    double tolerance) const
{
    double distVal = 0;
    ARC_TYPE nearnessType;
    bool isInArea = inArea(p, snapInfo.getSnapTolerance(),
        &nearnessType);
    bool hasSpans = false, hasChord = false;
    int wholeArcType = 0;
    bool checkArc = TRUE;
    bool checkChord = hasChord;
    bool checkSpans = hasSpans;
    bool checkStart = FALSE;
    bool checkEnd = FALSE;

    // if it is an area we have been successful
    if (isInArea && isArea) {
        // we are in the area so have hit the point
        snapInfo.setPoint(p, 0, wholeArcType, 0);
        return 0;
    } else if (snapInfo.isSnapping()) {
        // we've failed to snap
        snapInfo.setCount(MX_GEOM_SNAP_NONE);
        return 0;
    }

    if (!isInArea) {
        switch (nearnessType) {
        case ARC_ARC:
            checkChord = FALSE;
            checkSpans = FALSE;
            break;
        case ARC_CHORD:
            checkArc = FALSE;
            checkSpans = FALSE;

            if (!hasChord) {
                // need to check the end points of the chord
                checkStart = TRUE;
                checkEnd = TRUE;
            }

            break;
        case ARC_SECTOR:
            checkArc = FALSE;
            checkChord = FALSE;
            break;
        default:
            break;
        }
    }

    int nsnapInfos = 0;
    mx_snap_info* thisSnapInfo;
    mx_snap_info testSnapInfos[6];

    if (checkArc) {
        thisSnapInfo = testSnapInfos + nsnapInfos;

        thisSnapInfo->copyBasics(snapInfo);

        thisSnapInfo->dist2arc(p, *this, MX_ARC_SNAP_ARC,
            tolerance, distVal);

        nsnapInfos++;
    }

    if (checkSpans) {
        mx_rect r1(c, startArc, TRUE);
        mx_rect r2(c, endArc, TRUE);

        thisSnapInfo = testSnapInfos + nsnapInfos;

        thisSnapInfo->dist2span(p, r1, MX_SECTOR_SNAP_SPAN1,
            tolerance, distVal);

        thisSnapInfo++;

        thisSnapInfo->dist2span(p, r2, MX_SECTOR_SNAP_SPAN2,
            tolerance, distVal);

        nsnapInfos += 2;
    }

    if (checkChord) {
        thisSnapInfo = testSnapInfos + nsnapInfos;

        mx_rect r1(startArc, endArc, TRUE);

        thisSnapInfo->dist2span(p, r1, MX_CHORD_SNAP_CHORD,
            tolerance, distVal);

        nsnapInfos++;
    }

    if (checkStart) {
        thisSnapInfo = testSnapInfos + nsnapInfos;

        thisSnapInfo->dist2point(p, startArc, MX_ARC_SNAP_START,
            tolerance, distVal);

        nsnapInfos++;
    }

    if (checkEnd) {
        thisSnapInfo = testSnapInfos + nsnapInfos;

        thisSnapInfo->dist2point(p, endArc, MX_ARC_SNAP_END,
            tolerance, distVal);

        nsnapInfos++;
    }

    distVal = snapInfo.combine(nsnapInfos, testSnapInfos);

    return distVal;
}

/*-------------------------------------------------
 * FUNCTION: mx_arc::dist2snap
 *
 * DESCRIPTION:
 *
 *
 */

double mx_arc::dist2snap(const mx_point& p,
    mx_snap_info& snapInfo,
    double tolerance) const

{
    double distVal = 0;

    mx_point np = normalisedPoint(p);
    mx_arc na = normalisedArc();
    mx_point tp;

    // have we finished this arc or managed to snap onto an end point
    if ((!na.dist2arcComponents(np, snapInfo, tolerance, distVal)) && (!na.dist2degenerateArc(np, snapInfo, tolerance, distVal))) {
        distVal = na.dist2properArc(np, snapInfo, tolerance);
    }

    // now we need to denormalise the snap
    // point

    tp = snapInfo.getPoint();

    snapInfo.setPoint(unnormalisedPoint(tp));

    return distVal;
}

/*-------------------------------------------------
 * FUNCTION: arc::testAngleRange
 *
 * DESCRIPTION:
 *
 *
 */

void mx_arc::testAngleRange(mx_rect& r,
    double testAngle,
    bool isXtest) const
{
    double size;
    bool inFirstRange = (inRange(sa.a(), ea.a(), testAngle)) || (inRange(sa.a(), ea.a(), testAngle + PITWO));
    bool inSecondRange = (inRange(sa.a(), ea.a(), testAngle + PI)) || (inRange(sa.a(), ea.a(), testAngle + PITHREE));

    // arc doesnt intersect the angle of interest
    if ((!inFirstRange) && (!inSecondRange))
        return;

    mx_point testPoint = ac(testAngle);

    if (isXtest) {
        size = (testPoint.x - c.x);
    } else {
        size = (testPoint.y - c.y);
    }

    bool firstIsMax = (size > 0);
    if (!firstIsMax)
        size = -size;

    if (inFirstRange) {
        if (isXtest) {
            if (firstIsMax) {
                r.xt = testPoint.x;
            } else {
                r.xb = testPoint.x;
            }
        } else {
            if (firstIsMax) {
                r.yt = testPoint.y;
            } else {
                r.yb = testPoint.y;
            }
        }
    }

    if (inSecondRange) {
        if (isXtest) {
            if (firstIsMax) {
                r.xb = c.x - size;
            } else {
                r.xt = c.x + size;
            }
        } else {
            if (firstIsMax) {
                r.yb = c.y - size;
            } else {
                r.yb = c.y + size;
            }
        }
    }
}

/*-------------------------------------------------
 * FUNCTION: arc::boxDegenerateArc
 *
 * DESCRIPTION:
 *
 *
 */

void mx_arc::boxDegenerateArc(mx_rect& r) const
{
    if (isPoint())
        return;

    // get the quads the arc uses
    bool quads[4];
    arcToQuadrants(quads);
    mx_point testPoint1, testPoint2;

    if (xLine()) {
        // distance from horizontal line
        // half or full line

        testPoint1 = (quads[0] || quads[3]) ? ac(0) : c;
        testPoint2 = (quads[1] || quads[2]) ? ac(PI) : c;

        if (testPoint1.x > r.xt) {
            r.xt = testPoint1.x;
        } else if (testPoint1.x < r.xb) {
            r.xb = testPoint1.x;
        }

        if (testPoint2.x > r.xt) {
            r.xt = testPoint2.x;
        } else if (testPoint2.x < r.xb) {
            r.xb = testPoint2.x;
        }
    } else {
        // yline

        testPoint1 = (quads[0] || quads[1]) ? ac(PI2) : c;
        testPoint2 = (quads[2] || quads[3]) ? ac(PI2 + PI) : c;

        if (testPoint1.y > r.yt) {
            r.yt = testPoint1.y;
        } else if (testPoint1.y < r.yb) {
            r.yb = testPoint1.y;
        }

        if (testPoint2.y > r.yt) {
            r.yt = testPoint2.y;
        } else if (testPoint2.y < r.yb) {
            r.yb = testPoint2.y;
        }
    }
}

/*-------------------------------------------------
 * FUNCTION: arc::box
 *
 * DESCRIPTION:
 *
 *
 */

mx_rect mx_arc::box()
{
    mx_rect r;

    r.xt = r.xb = startArc.x;
    r.yt = r.yb = startArc.y;

    if (endArc.x > r.xt)
        r.xt = endArc.x;
    if (endArc.y > r.yt)
        r.yt = endArc.y;
    if (endArc.x < r.xb)
        r.xb = endArc.x;
    if (endArc.y < r.yb)
        r.yb = endArc.y;

    if (arcType == ARC_SECTOR) {
        // Include the centre
        if (c.x < r.xb)
            r.xb = c.x;
        if (c.x > r.xt)
            r.xt = c.x;
        if (c.y < r.yb)
            r.yb = c.y;
        if (c.y > r.yt)
            r.yt = c.y;
    }

    r.setSpanArea(FALSE, TRUE);

    // finished if degenerate

    if (isDegenerate()) {
        boxDegenerateArc(r);
        return r;
    }

    if (angle.a() == 0) {
        if (inRange(sa.a(), ea.a(), 0.0) || inRange(sa.a(), ea.a(), PITWO)) {
            r.xt = s.x + c.x;
        }

        if (inRange(sa.a(), ea.a(), PI2) || inRange(sa.a(), ea.a(), PITWO + PI2)) {
            r.yt = s.y + c.y;
        }

        if (inRange(sa.a(), ea.a(), PI) || inRange(sa.a(), ea.a(), PITHREE)) {
            r.xb = -s.x + c.x;
        }

        if (inRange(sa.a(), ea.a(), PI + PI2) || inRange(sa.a(), ea.a(), PITHREE + PI2)) {
            r.yb = -s.y + c.y;
        }
    } else {
        // find the angles of the max x and y relative to
        // rotated angle

        // angle 1 is max/min y, angle2 in max/min x
        // want test angles in the range (0,PI)
        double angle1 = gatan2(s.y * angle.c(), s.x * angle.s(), 0);
        double angle2 = gatan2(-s.y * angle.s(), s.y * angle.c(), 0);

        if (angle1 >= PI)
            angle1 -= PI;
        if (angle2 >= PI)
            angle2 -= PI;

        testAngleRange(r, angle1, FALSE);
        testAngleRange(r, angle2, TRUE);
    }

    return r;
}

/*-------------------------------------------------
 * FUNCTION: mx_arc::arcToPolyline
 *
 * DESCRIPTION:
 *
 * Convert an arc to a polyline - optionally
 * closing the polyline with duplicate start
 * and end points
 */

void mx_arc::arcToPolypoint(double tolerance,
    bool doAppend,
    mx_polypoint& pline) const
{
    mx_point op;
    double outputAngle;
    // get appoximation factor
    double lf = linearFactor(tolerance);

    // work out a rough number of points
    int npoints = ((int)((ea.a() - sa.a()) / lf)) + 2;

    if (doAppend) {
        pline.setMemorySize(pline.get_num_points() + npoints);
    } else {
        // clear out old points and make enough space for
        // new points
        pline.clear(npoints);
    }

    outputAngle = sa.a();

    do {
        op = ac(outputAngle);
        pline.addPoint(op);
        outputAngle += lf;
    } while (outputAngle < ea.a());

    op = ac(ea.a());
    pline.addPoint(op);
}

/*-------------------------------------------------
 * FUNCTION: mx_arc::arcToBoundingPolyline
 *
 * DESCRIPTION:
 *
 * Convert an arc to a polyline - optionally
 * closing the polyline with duplicate start
 * and end points
 */

void mx_arc::arcToBoundingPolypoint(double tolerance,
    mx_polypoint& pline) const
{
    // convert the arc to a polypoint -  let closure
    // be done by the draw polypoint routine

    arcToPolypoint(tolerance, FALSE, pline);

    switch (arcType) {
    case ARC_SECTOR:
        // now add the centre point
        pline.addPoint(c);
        break;
    default:
        break;
    }

    pline.set_closed(TRUE);

    return;
}

/*-------------------------------------------------
 * FUNCTION: arc::linearFactor
 *
 * DESCRIPTION:
 *
 * The greatest curvature is it the furthest point of
 * the arc from its centre
 *
 *     X = max(a,b) - tolerance
 *     Y = Xtan(ang/2)
 *
 *     (X/a)**2 + (Y/b)**2  == 1
 *
 *    Solve for ang given tolerance
 *
 *
 *
 */

double mx_arc::linearFactor(double tolerance) const
{
    double a, b, p1, p2;

    b = GMIN(s.x, s.y);
    a = GMAX(s.x, s.y);

    if (tolerance >= a)
        return PI2;

    p1 = 1 / ((a - tolerance) * (a - tolerance));
    p2 = 1 / (a * a);

    return 2.0 * atan(b * sqrt(p1 - p2));
}

/*-------------------------------------------------
 * FUNCTION: mx_arc::ac
 *
 * DESCRIPTION:
 *
 *
 */

mx_point mx_arc::ac(double ia) const
{
    mx_point p;

    if (angle.a() == 0) {
        p.x = c.x + s.x * cos(ia);
        p.y = c.y + s.y * sin(ia);
    } else {
        mx_rotation rotFor(angle);

        p.x = s.x * cos(ia);
        p.y = s.y * sin(ia);

        p = p.rotate(rotFor);

        p += c;
    }
    return p;
}

/*-------------------------------------------------
 * Function: gmx_arc::intersectBoxSpan
 *
 * DESCRIPTION:
 *
 *
 */

/*-------------------------------------------------
 * Function: gmx_arc::intersectBox
 *
 * DESCRIPTION:
 *
 *
 */
GEOM_INTERSECT_TYPE gmx_arc::intersectBox(int& err, mx_rect& rtest, double tolerance)
{
    mx_rect arcBox = a.box();
    GEOM_INTERSECT_TYPE retValue = GEOM_INTERSECT_NONE;

    err = MX_ERROR_OK;

    return retValue;

#ifdef dasdfdf
    // quick test

    retValue = intersect(err, arcBox, rtest, NULL, tolerance);
    MX_ERROR_CHECK(err);

    if ((retValue == GEOM_INTERSECT_NONE) || (retValue == GEOM_INTERSECT_CONTAINED))
        return retValue;

    // now look for

    return retValue;
abort:
    return GEOM_INTERSECT_NONE;
#endif
}

/*-------------------------------------------------
 * FUNCTION: mx_arc::get_serialise_size
 *
 * DESCRIPTION:
 *
 *
 */

uint32 mx_arc::get_serialised_size(int& err)
{
    err = MX_ERROR_OK;

    return 7 * SLS_DOUBLE + SLS_INT8;
}

/*-------------------------------------------------
 * FUNCTION: mx_arc::serialise
 *
 * DESCRIPTION:
 *
 *
 */

void mx_arc::serialise(int& err,
    unsigned char** buffer)
{
    char ti;

    err = MX_ERROR_OK;

    serialise_double(c.x, buffer);
    serialise_double(c.y, buffer);
    serialise_double(s.x, buffer);
    serialise_double(s.y, buffer);
    serialise_double(sa.a(), buffer);
    serialise_double(ea.a(), buffer);
    serialise_double(angle.a(), buffer);

    if (isArea) {
        ti = -arcType;
    } else {
        ti = arcType;
    }

    serialise_int8(ti, buffer);

    return;
}

/*-------------------------------------------------
 * Function: mx_arc::unserialise
 *
 * DESCRIPTION:
 *
 *
 */

void mx_arc::unserialise(int& err,
    unsigned char** buffer)
{
    int8 ti;
    double startAngle, endAngle, angle;

    err = MX_ERROR_OK;

    unserialise_double(c.x, buffer);
    unserialise_double(c.y, buffer);
    unserialise_double(s.x, buffer);
    unserialise_double(s.y, buffer);
    unserialise_double(startAngle, buffer);
    unserialise_double(endAngle, buffer);
    unserialise_double(angle, buffer);
    unserialise_int8(ti, buffer);

    isArea = (ti < 0);
    arcType = (ARC_TYPE)(isArea ? -ti : ti);

    setArc(c, s, startAngle, endAngle, arcType, isArea, angle);

    return;
}

/*-------------------------------------------------
 * FUNCTION: gmx_arc::get_serialise_size
 *
 * DESCRIPTION:
 *
 *
 */

uint32 gmx_arc::get_serialised_size(int& err)
{
    int size;

    err = MX_ERROR_OK;

    size = mx_geom::get_serialised_size(err);
    MX_ERROR_CHECK(err);

    size += a.get_serialised_size(err);
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

void gmx_arc::serialise(int& err,
    unsigned char** buffer)
{
    err = MX_ERROR_OK;

    mx_geom::serialise(err, buffer);
    MX_ERROR_CHECK(err);

    a.serialise(err, buffer);
    MX_ERROR_CHECK(err);

    return;
abort:
    return;
}

/*-------------------------------------------------
 * FUNCTION: gmx_arc::unserialise
 *
 * DESCRIPTION:
 *
 *
 */

void gmx_arc::unserialise(int& err,
    unsigned char** buffer)
{
    mx_geom::unserialise(err, buffer);
    MX_ERROR_CHECK(err);

    a.unserialise(err, buffer);
    MX_ERROR_CHECK(err);

    return;
abort:
    return;
}

void mx_arc::sc(mx_point& ic)
{
    c = ic;
    setArc(c, s, sa.a(), ea.a(), arcType, isArea, angle.a());
}
void mx_arc::ss(mx_point& is)
{
    s = is;
    setArc(c, s, sa.a(), ea.a(), arcType, isArea, angle.a());
}
void mx_arc::ssa(double isa)
{
    sa.set(isa);
    setArc(c, s, sa.a(), ea.a(), arcType, isArea, angle.a());
}
void mx_arc::sea(double iea)
{
    ea.set(iea);
    setArc(c, s, sa.a(), ea.a(), arcType, isArea, angle.a());
}
void mx_arc::saa(double iaa)
{
    angle.set(iaa);
    setArc(c, s, sa.a(), ea.a(), arcType, isArea, angle.a());
}
