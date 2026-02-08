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
 * MODULE : grectspan.C
 *
 * AUTHOR : David Miller
 *
 * This file
 *
 * DESCRIPTION:
 * Module gRectSpan.C
 *
 *
 *
 *
 */

#include <geometry.h>
#include <mx.h>

/*-------------------------------------------------
 * FUNCTION: irect::irect
 *
 * DESCRIPTION: Default construcutor
 *
 *
 */

mx_irect::mx_irect(bool span, bool area)
{
    xt = yt = xb = yb = 0;
    setSpanArea(span, area);
}

/*-------------------------------------------------
 * FUNCTION: mx_irect::mx_irect
 *
 * DESCRIPTION: Construcutor from four coords
 *
 *
 */

mx_irect::mx_irect(int32 pxb, int32 pyb,
    int32 pxt, int32 pyt,
    bool span,
    bool area)
{
    xt = pxt;
    yt = pyt;
    xb = pxb;
    yb = pyb;

    setSpanArea(span, area);
}

/*-------------------------------------------------
 * FUNCTION: mx_irect::mx_irect
 *
 * DESCRIPTION: Construcutor from two points
 *
 *
 */

mx_irect::mx_irect(const mx_point& pb,
    const mx_point& pt,
    bool span,
    bool area)
{
    xt = (int32)GNINT(pt.x);
    yt = (int32)GNINT(pt.y);
    xb = (int32)GNINT(pb.x);
    yb = (int32)GNINT(pb.y);

    setSpanArea(span, area);
}

/*-------------------------------------------------
 * FUNCTION: mx_irect::mx_irect
 *
 * DESCRIPTION: Construcutor from a rect
 *
 *
 */

mx_irect::mx_irect(const mx_rect& rct)
{
    xt = (int32)GNINT(rct.xt);
    yt = (int32)GNINT(rct.yt);
    xb = (int32)GNINT(rct.xb);
    yb = (int32)GNINT(rct.yb);

    setSpanArea(rct.isSpan, rct.isArea);
}

/*-------------------------------------------------
 *  FUNCTION: rectagle unit spans
 *
 * DESCRIPTION:
 *
 *
 */

mx_point mx_irect::uspan(double* lent) const
{
    double len = length();
    mx_point sp = span();

    if (lent != NULL)
        *lent = len;

    if (len > 0) {
        return (sp / len);
    } else {
        return sp * 0;
    }
}

/*-------------------------------------------------
 * FUNCTION: mx_irect::normalised
 *
 * DESCRIPTION: Return a normalised rectangle, that
 * is a rectangle where xb is less then xt and yb is
 * less than yt
 */

mx_irect mx_irect::normalised() const
{
    if (isNormalised())
        return *this;

    mx_irect retrect = *this;

    if (xb > xt) {
        retrect.xt = xb;
        retrect.xb = xt;
    } else {
        retrect.xt = xt;
        retrect.xb = xb;
    }

    if (yb > yt) {
        retrect.yt = yb;
        retrect.yb = yt;
    } else {
        retrect.yt = yt;
        retrect.yb = yb;
    }

    return retrect;
}

/*-------------------------------------------------
 * FUNCTION: mx_irect::normalise
 *
 * DESCRIPTION: Normalise a  rectangle
 *
 */

void mx_irect::normalise()
{
    if (isNormalised())
        return;

    mx_irect retrect;

    retrect = normalised();

    *this = retrect;

    return;
}

/*-------------------------------------------------
 * FUNCTION: mx_irect::setSpanArea
 *
 * DESCRIPTION: Set span and area flags
 *
 *
 */

void mx_irect::setSpanArea(bool span, bool area)
{
    isSpan = span;
    if (isSpan) {
        isArea = FALSE;
    } else {
        isArea = area;
    }
}

/*-------------------------------------------------
 * FUNCTION: mx_rect::mx_rect
 *
 * DESCRIPTION: Default construcutor
 *
 *
 */

mx_rect::mx_rect(bool span, bool area, double irotateAngle)
{
    xt = yt = xb = yb = 0;
    angle.set(irotateAngle);
    setSpanArea(span, area);
}

/*-------------------------------------------------
 * FUNCTION: mx_rect::mx_rect
 *
 * DESCRIPTION: Construcutor from four coords
 *
 *
 */

mx_rect::mx_rect(double pxb, double pyb,
    double pxt, double pyt,
    bool span, bool area,
    double irotateAngle)
{
    xt = pxt;
    yt = pyt;
    xb = pxb;
    yb = pyb;
    angle.set(irotateAngle);

    setSpanArea(span, area);
}

/*-------------------------------------------------
 * FUNCTION: mx_rect::mx_rect
 *
 * DESCRIPTION: Construcutor from two points
 *
 *
 */

mx_rect::mx_rect(const mx_point& pb,
    const mx_point& pt,
    bool span,
    bool area,
    double irotateAngle)
{
    xt = pt.x;
    yt = pt.y;
    xb = pb.x;
    yb = pb.y;
    angle.set(irotateAngle);

    setSpanArea(span, area);
}

/*-------------------------------------------------
 * FUNCTION: mx_rect::mx_rect
 *
 * DESCRIPTION: Constructor from an mx_irect
 *
 *
 */

mx_rect::mx_rect(const mx_irect& irct)
{
    xt = irct.xt;
    yt = irct.yt;
    xb = irct.xb;
    yb = irct.yb;
    angle.set(0);

    setSpanArea(irct.isSpan, irct.isArea);
}

/*-------------------------------------------------
 * FUNCTION: mx_rect::increaseRect
 *
 * DESCRIPTION:
 *
 *
 */

mx_rect mx_rect::increaseRect(mx_rect& inRect)
{
    mx_rect output = inRect.normalised();

    if (xt > output.xt)
        output.xt = xt;
    if (yt > output.yt)
        output.yt = yt;
    if (xb < output.xb)
        output.xb = xb;
    if (yb < output.yb)
        output.yb = yb;

    return output;
}

/*-------------------------------------------------
 * FUNCTION: mx_rect::setSpanArea
 *
 * DESCRIPTION: Set span and area flags
 *
 *
 */

void mx_rect::setSpanArea(bool span, bool area)
{
    isSpan = span;
    if (isSpan) {
        isArea = FALSE;
    } else {
        isArea = area;
    }
}

/*-------------------------------------------------
 *  FUNCTION: mx_rectagle unit spans
 *
 * DESCRIPTION:
 *
 *
 */

mx_point mx_rect::uspan(double* lent) const
{
    double len = length();
    mx_point sp = span();

    if (lent != NULL)
        *lent = len;

    if (len > 0) {
        return (sp / len);
    } else {
        return sp * 0;
    }
}

/*-------------------------------------------------
 * FUNCTION: mx_rect::mergeSpanSelectCount
 *
 * DESCRIPTION: Constructor from integer point
 *
 *
 */

bool mx_rect::mergeSpanSelectCount(int nselectCounts,
    int* selectCounts)
{
    bool inc[2] = { FALSE, FALSE };
    int ic, selectCount;

    for (ic = 0; ic < nselectCounts; ic++) {
        selectCount = selectCounts[ic];

        switch (selectCount) {
        case MX_SPAN_SNAP_START:
            inc[0] = TRUE;
            break;
        case MX_SPAN_SNAP_END:
            inc[1] = TRUE;
            break;
        case MX_SPAN_SNAP_WHOLE:
            inc[0] = TRUE;
            inc[1] = TRUE;
            break;
        default:
            break;
        }
        selectCounts[ic] = -1;
    }

    if (inc[0] && inc[1]) {
        selectCounts[0] = MX_SPAN_SNAP_WHOLE;
    } else if (inc[0]) {
        selectCounts[0] = MX_SPAN_SNAP_START;
    } else if (inc[1]) {
        selectCounts[0] = MX_SPAN_SNAP_END;
    }

    return (selectCounts[0] == MX_SPAN_SNAP_WHOLE);
}

/*-------------------------------------------------
 * FUNCTION: mx_rect::mergeRectSelectCount
 *
 * DESCRIPTION: Constructor from integer point
 *
 *
 */

bool mx_rect::mergeRectSelectCount(int nselectCounts,
    int* selectCounts)
{
    bool inc[4] = { FALSE, FALSE, FALSE, FALSE };
    int ic, icount, selectCount;

    for (ic = 0; ic < nselectCounts; ic++) {
        selectCount = selectCounts[ic];

        switch (selectCount) {
        case MX_RECT_SNAP_TL:
            inc[0] = TRUE;
            break;
        case MX_RECT_SNAP_TR:
            inc[1] = TRUE;
            break;
        case MX_RECT_SNAP_BR:
            inc[2] = TRUE;
            break;
        case MX_RECT_SNAP_BL:
            inc[3] = TRUE;
            break;
        case MX_RECT_SNAP_TOP:
            inc[0] = TRUE;
            inc[1] = TRUE;
            break;
        case MX_RECT_SNAP_RIGHT:
            inc[1] = TRUE;
            inc[2] = TRUE;
            break;
        case MX_RECT_SNAP_BOTTOM:
            inc[3] = TRUE;
            inc[2] = TRUE;
            break;
        case MX_RECT_SNAP_LEFT:
            inc[0] = TRUE;
            inc[3] = TRUE;
            break;
        case MX_RECT_SNAP_WHOLE:
            inc[0] = TRUE;
            inc[1] = TRUE;
            inc[2] = TRUE;
            inc[3] = TRUE;
            break;
        default:
            break;
        }
        selectCounts[ic] = -1;
    }

    // count the number of matches
    icount = 0;
    for (ic = 0; ic < 4; ic++) {
        if (inc[ic])
            icount++;
    }

    if (icount >= 3) {
        selectCounts[0] = MX_RECT_SNAP_WHOLE;
    } else if (icount == 2) {
        if (inc[0] && inc[1]) {
            selectCounts[0] = MX_RECT_SNAP_TOP;
        } else if (inc[1] && inc[2]) {
            selectCounts[0] = MX_RECT_SNAP_RIGHT;
        } else if (inc[2] && inc[3]) {
            selectCounts[0] = MX_RECT_SNAP_BOTTOM;
        } else if (inc[3] && inc[0]) {
            selectCounts[0] = MX_RECT_SNAP_LEFT;
        } else {
            selectCounts[0] = MX_RECT_SNAP_WHOLE;
        }
    } else if (icount == 1) {
        if (inc[0]) {
            selectCounts[0] = MX_RECT_SNAP_TL;
        } else if (inc[1]) {
            selectCounts[0] = MX_RECT_SNAP_TR;
        } else if (inc[2]) {
            selectCounts[0] = MX_RECT_SNAP_BR;
        } else if (inc[3]) {
            selectCounts[0] = MX_RECT_SNAP_BL;
        }
    }

    return (selectCounts[0] == MX_RECT_SNAP_WHOLE);
}

/*-------------------------------------------------
 * FUNCTION: mx_rect::mergeSelectCount
 *
 * DESCRIPTION: Constructor from integer point
 *
 *
 */

bool mx_rect::mergeSelectCount(int nselectCounts,
    int* selectCounts)
{
    if (isSpan) {
        return mergeSpanSelectCount(nselectCounts,
            selectCounts);
    } else {
        return mergeRectSelectCount(nselectCounts,
            selectCounts);
    }
}

/*-------------------------------------------------
 * FUNCTION: mx_rect::normalise
 *
 * DESCRIPTION: Normalise a  rectangle
 *
 */

void mx_rect::normalise()
{
    if (isNormalised())
        return;

    mx_rect retrect;

    retrect = normalised();

    *this = retrect;

    return;
}

/*-------------------------------------------------
 * FUNCTION: mx_rect::normalised
 *
 * DESCRIPTION: Return a normalised rectangle, that
 * is a rectangle where xb is less then xt and yb is
 * less than yt
 */

mx_rect mx_rect::normalised() const
{
    if (isNormalised())
        return *this;

    mx_rect retrect = *this;

    if (xb > xt) {
        retrect.xt = xb;
        retrect.xb = xt;
    } else {
        retrect.xt = xt;
        retrect.xb = xb;
    }

    if (yb > yt) {
        retrect.yt = yb;
        retrect.yb = yt;
    } else {
        retrect.yt = yt;
        retrect.yb = yb;
    }

    return retrect;
}

/*-------------------------------------------------
 * FUNCTION: mx_rect::dist2spanComponents
 *
 * DESCRIPTION:
 *
 *
 */

bool mx_rect::dist2spanComponents(const mx_point& p,
    mx_snap_info& snapInfo,
    double tolerance,
    double& distVal) const
{
    if ((!snapInfo.isSnapping()) || (snapInfo.getCount() == MX_SPAN_SNAP_NO_COMPONENTS))
        return FALSE;

    if (snapInfo.alreadyFailed(MX_SPAN_SNAP_WHOLE)) {
        snapInfo.setCount(MX_GEOM_SNAP_NONE);
        return TRUE;
    }

    // try snapping to the start point
    if (snapInfo.dist2point(p, start(), MX_SPAN_SNAP_START,
            tolerance, distVal))
        return TRUE;

    // try snapping to the end point
    if (snapInfo.dist2point(p, end(), MX_SPAN_SNAP_END,
            tolerance, distVal))
        return TRUE;

    if (snapInfo.alreadyFailed(MX_SPAN_SNAP_START)) {
        // we have had an intersection with the end points
        // therefore we have snapped to the whole object

        snapInfo.setCount(MX_SPAN_SNAP_WHOLE);
        distVal = snapInfo.getDistance();
        return TRUE;
    }

    return FALSE;
}

/*-------------------------------------------------
 * FUNCTION: mx_rect::dist2degenerateSpan
 *
 * DESCRIPTION:
 *
 *
 */

bool mx_rect::dist2degenerateSpan(const mx_point& p,
    mx_snap_info& snapInfo,
    double tolerance,
    double& distVal) const
{
    distVal = 0;

    // not degenerate
    if (length2() > 0)
        return FALSE;

    if (snapInfo.isSnapping()) {
        // if snapping we have already failed
        snapInfo.setCount(MX_GEOM_SNAP_NONE);
    } else {
        distVal = start().dist2(p);

        snapInfo.setPoint(start(), distVal,
            MX_SPAN_SNAP_WHOLE, 0);
    }

    return TRUE;
}

/*-------------------------------------------------
 * FUNCTION: mx_rect::dist2properSpan
 *
 * DESCRIPTION:
 *
 *
 */

double mx_rect::dist2properSpan(const mx_point& p,
    mx_snap_info& snapInfo,
    double tolerance) const
{
    double length;
    bool isXaligned = (yt == yb);
    bool isYaligned = (xt == xb);
    double distance;
    mx_point s;
    mx_point testPoint;
    double offset;
    double testDistance;
    bool toLeft, toRight;

    if (isXaligned) {
        double XB = GMIN(xb, xt);
        double XT = GMAX(xb, xt);

        testPoint.x = p.x;
        testPoint.y = yt;
        offset = (p.x - XB) / (XT - XB);
        testDistance = (p.y - yt) * (p.y - yt);
    } else if (isYaligned) {
        double YB = GMIN(yb, yt);
        double YT = GMAX(yb, yt);

        testPoint.x = xt;
        testPoint.y = p.y;
        offset = (p.y - YB) / (YT - YB);
        testDistance = (p.x - xt) * (p.x - xt);
    } else {
        s = uspan(&length);
        mx_point d(p.x - xb, p.y - yb);
        distance = s * d;

        // point normal to span
        mx_point n = s.flip();
        testDistance = n * d;
        testDistance *= testDistance;
        offset = distance / length;

        testPoint = start() + distance * s;
    }

    toLeft = (offset <= -snapInfo.getSnapTolerance());
    toRight = (offset >= 1 + snapInfo.getSnapTolerance());

    if (snapInfo.isSnapping() && (toLeft || toRight)) {
        // if snapping and outside the range of the span
        // we have already failed
        snapInfo.setCount(MX_GEOM_SNAP_NONE);
        return 0;
    }

    if (toLeft) {
        testPoint = start();
        testDistance = start().dist2(p);
        offset = 0;
    } else if (toRight) {
        testPoint = end();
        testDistance = end().dist2(p);
        offset = 1;
    }

    // set info on the snap structure
    snapInfo.setPoint(testPoint, testDistance,
        MX_SPAN_SNAP_WHOLE, offset);

    return testDistance;
}

/*-------------------------------------------------
 * FUNCTION: mx_rect::dist2span
 *
 * DESCRIPTION:
 *
 *
 */

double mx_rect::dist2span(const mx_point& p,
    mx_snap_info& snapInfo,
    double tolerance) const
{
    double distVal = 0;

    // have we managed to snap to any components
    if (dist2spanComponents(p, snapInfo, tolerance, distVal))
        return distVal;

    // See if the span is degenerate
    if (dist2degenerateSpan(p, snapInfo, tolerance, distVal))
        return distVal;

    return dist2properSpan(p, snapInfo, tolerance);
}

/*-------------------------------------------------
 * FUNCTION: mx_rect::dist2degenerateRect
 *
 * DESCRIPTION: The input point and rectangle are normalises
 *
 *
 */

bool mx_rect::dist2degenerateRect(const mx_point& p,
    mx_snap_info& snapInfo,
    double tolerance,
    double& distVal) const
{
    distVal = 0;

    // not degenerate
    if (area() > 0)
        return FALSE;

    if (snapInfo.isSnapping()) {
        // if snapping we have already failed
        snapInfo.setCount(MX_GEOM_SNAP_NONE);
    } else {
        if ((xt - xb) > 0) {
            distVal = top().dist2span(p);
        } else {
            distVal = right().dist2span(p);
        }

        snapInfo.setCount(MX_RECT_SNAP_WHOLE);
    }

    return TRUE;
}

/*-------------------------------------------------
 * FUNCTION: mx_rect::dist2rectComponents
 *
 * DESCRIPTION: The input point and rectangle are normalises
 *
 *
 */

bool mx_rect::dist2rectComponents(const mx_point& p,
    mx_snap_info& snapInfo,
    double tolerance,
    double& distVal) const
{
    if ((!snapInfo.isSnapping()) || (snapInfo.getCount() == MX_RECT_SNAP_NO_COMPONENTS))
        return FALSE;

    if (snapInfo.alreadyFailed(MX_RECT_SNAP_WHOLE)) {
        snapInfo.setCount(MX_GEOM_SNAP_NONE);
        return TRUE;
    }

    // try snapping to the top left
    if (snapInfo.dist2point(p, topLeft(), MX_RECT_SNAP_TL,
            tolerance, distVal))
        return TRUE;

    // try snapping to the top right
    if (snapInfo.dist2point(p, topRight(), MX_RECT_SNAP_TR,
            tolerance, distVal))
        return TRUE;

    // try snapping to the bottom right
    if (snapInfo.dist2point(p, bottomRight(), MX_RECT_SNAP_BR,
            tolerance, distVal))
        return TRUE;

    // try snapping to the bottom left
    if (snapInfo.dist2point(p, bottomLeft(), MX_RECT_SNAP_BL,
            tolerance, distVal))
        return TRUE;

    // these snaps could be done more efficiently since we
    // are x or y aligned with the spans

    // try snapping to the top
    if (snapInfo.dist2span(p, top(), MX_RECT_SNAP_TOP,
            tolerance, distVal))
        return TRUE;

    // try snapping to the right
    if (snapInfo.dist2span(p, right(), MX_RECT_SNAP_RIGHT,
            tolerance, distVal))
        return TRUE;

    // try snapping to the right
    if (snapInfo.dist2span(p, bottom(), MX_RECT_SNAP_BOTTOM,
            tolerance, distVal))
        return TRUE;

    // try snapping to the right
    if (snapInfo.dist2span(p, left(), MX_RECT_SNAP_LEFT,
            tolerance, distVal))
        return TRUE;

    if (snapInfo.alreadyFailed(MX_RECT_SNAP_TL)) {
        // we have had an intersection with the end points
        // therefore we have snapped to the whole object

        snapInfo.setCount(MX_RECT_SNAP_WHOLE);
        distVal = snapInfo.getDistance();
        return TRUE;
    }

    return FALSE;
}

/*-------------------------------------------------
 * FUNCTION: mx_rect::inArea
 *
 * DESCRIPTION: The input point and rectangle
 * are normalised
 *
 */

bool mx_rect::inArea(const mx_point& p,
    double tolerance,
    MX_RECT_SNAP* nearnessType) const
{
    bool isLeft = (p.x <= (xb - tolerance));
    bool isRight = (p.x >= (xt + tolerance));
    bool isAbove = (p.y <= (yb - tolerance));
    bool isBelow = (p.y >= (yt + tolerance));

    if (isLeft) {
        if (isAbove) {
            *nearnessType = MX_RECT_SNAP_TL;
        } else if (isBelow) {
            *nearnessType = MX_RECT_SNAP_BL;
        } else {
            *nearnessType = MX_RECT_SNAP_LEFT;
        }
    } else if (isRight) {
        if (isAbove) {
            *nearnessType = MX_RECT_SNAP_TR;
        } else if (isBelow) {
            *nearnessType = MX_RECT_SNAP_BR;
        } else {
            *nearnessType = MX_RECT_SNAP_RIGHT;
        }
    } else {
        if (isAbove) {
            *nearnessType = MX_RECT_SNAP_TOP;
        } else if (isBelow) {
            *nearnessType = MX_RECT_SNAP_BOTTOM;
        } else {
            return TRUE;
        }
    }

    return FALSE;
}

/*-------------------------------------------------
 * FUNCTION: mx_rect::dist2properRect
 *
 * DESCRIPTION: The input point and rectangle are
 * normalised here
 *
 */

double mx_rect::dist2properRect(const mx_point& p,
    mx_snap_info& snapInfo,
    double tolerance) const
{
    double distVal = 0;

    // first check if is in the area
    MX_RECT_SNAP nearnessType;
    bool isInArea = inArea(p, snapInfo.getSnapTolerance(),
        &nearnessType);

    // if it is an area we have been successful
    if (isInArea && isArea) {
        // we are in the area so have hit the point
        snapInfo.setPoint(p, 0,
            MX_RECT_SNAP_WHOLE, 0);
        return 0;
    } else if (snapInfo.isSnapping()) {
        // we've failed to snap
        snapInfo.setCount(MX_GEOM_SNAP_NONE);
        return 0;
    }

    // here we are not in snap mode and have to find
    // the nearest point among the components

    if (isInArea) {
        double coord1[3] = { p.x, yt, p.y };
        double coord2[3] = { xb, p.y, yb };
        MX_RECT_SNAP sp[3] = { MX_RECT_SNAP_LEFT,
            MX_RECT_SNAP_BOTTOM,
            MX_RECT_SNAP_TOP };

        double testVal = xt - p.x;
        nearnessType = MX_RECT_SNAP_RIGHT;
        double newVal;

        // the point is one of the intersections of the
        // point with the cross

        for (int iedge = 0; iedge < 3; iedge++) {
            newVal = coord1[iedge] - coord2[iedge];
            if (newVal < testVal) {
                testVal = newVal;
                nearnessType = sp[iedge];
            }
        }
    }

    switch (nearnessType) {
    case MX_RECT_SNAP_TL:
        snapInfo.dist2point(p, topLeft(), MX_RECT_SNAP_TL,
            tolerance, distVal);
        break;
    case MX_RECT_SNAP_TOP:
        snapInfo.dist2span(p, top(), MX_RECT_SNAP_TOP,
            tolerance, distVal);
        break;
    case MX_RECT_SNAP_TR:
        snapInfo.dist2point(p, topRight(), MX_RECT_SNAP_TR,
            tolerance, distVal);
        break;
    case MX_RECT_SNAP_RIGHT:
        snapInfo.dist2span(p, right(), MX_RECT_SNAP_RIGHT,
            tolerance, distVal);
        break;
    case MX_RECT_SNAP_BR:
        snapInfo.dist2point(p, bottomRight(), MX_RECT_SNAP_BR,
            tolerance, distVal);
        break;
    case MX_RECT_SNAP_BOTTOM:
        snapInfo.dist2span(p, bottom(), MX_RECT_SNAP_BOTTOM,
            tolerance, distVal);
        break;
    case MX_RECT_SNAP_BL:
        snapInfo.dist2point(p, bottomLeft(), MX_RECT_SNAP_BL,
            tolerance, distVal);
        break;
    case MX_RECT_SNAP_LEFT:
        snapInfo.dist2span(p, left(), MX_RECT_SNAP_LEFT,
            tolerance, distVal);
        break;
    default:
        break;
    }

    return distVal;
}

/*-------------------------------------------------
 * FUNCTION: mx_rect::dist2rect
 *
 * DESCRIPTION: the input point and rectangle are
 * normalised here
 *
 */

double mx_rect::dist2rect(const mx_point& p,
    mx_snap_info& snapInfo,
    double tolerance) const
{
    double distVal = 0;

    // have we managed to snap to any components
    if (dist2rectComponents(p, snapInfo, tolerance, distVal))
        return distVal;

    // is the rectangle degenerate
    if (dist2degenerateRect(p, snapInfo, tolerance, distVal))
        return distVal;

    return dist2properRect(p, snapInfo, tolerance);
}

/*-------------------------------------------------
 * FUNCTION: mx_rect::dist2snap
 *
 * DESCRIPTION:
 *
 *
 */

double mx_rect::dist2snap(const mx_point& p,
    mx_snap_info& snapInfo,
    double tolerance) const
{
    if (isSpan) {
        return dist2span(p, snapInfo, tolerance);
    } else {
        mx_rect r = normalisedRect();
        mx_point tp = normalisedPoint(p);

        double distVal = r.dist2rect(tp, snapInfo, tolerance);

        // now we need to denormalise the snap
        // point

        tp = snapInfo.getPoint();

        snapInfo.setPoint(unnormalisedPoint(tp));

        return distVal;
    }
}

/*-------------------------------------------------
 * FUNCTION: mx_rect::container
 *
 * DESCRIPTION:
 *
 *
 */

mx_irect mx_rect::container(double snapTolerance)
{
    int ixb, iyb, ixt, iyt;
    float rxb, ryb, rxt, ryt;

    doubleDecomp(xb, ixb, rxb);
    doubleDecomp(xt, ixt, rxt);
    doubleDecomp(yb, iyb, ryb);
    doubleDecomp(yt, iyt, ryt);

    if (rxb > 1.0 - snapTolerance)
        ixb++;
    if (ryb > 1.0 - snapTolerance)
        iyb++;

    if (rxt > snapTolerance)
        ixt++;
    if (ryt > snapTolerance)
        iyt++;

    mx_irect retRect(ixb, iyb, ixt, iyt);

    return retRect;
}

/*-------------------------------------------------
 * FUNCTION: mx_rect::get_serialise_size
 *
 * DESCRIPTION:
 *
 *
 */

uint32 mx_rect::get_serialised_size(int& err)
{
    err = MX_ERROR_OK;

    return isSpan ? spanSize : boxSize;
}

/*-------------------------------------------------
 * FUNCTION: mx_rect::serialise
 *
 * DESCRIPTION:
 *
 *
 */

void mx_rect::serialise(int& err,
    unsigned char** buffer)
{
    err = MX_ERROR_OK;

    serialise_double(xb, buffer);
    serialise_double(yb, buffer);
    serialise_double(xt, buffer);
    serialise_double(yt, buffer);

    serialise_bool(isSpan, buffer);
    serialise_bool(isArea, buffer);

    if (!isSpan) {
        serialise_double(angle.a(), buffer);
    }
}

/*-------------------------------------------------
 * FUNCTION: mx_rect::unserialise
 *
 * DESCRIPTION:
 *
 *
 */

void mx_rect::unserialise(int& err,
    unsigned char** buffer)
{
    double iangle;

    err = MX_ERROR_OK;

    unserialise_double(xb, buffer);
    unserialise_double(yb, buffer);
    unserialise_double(xt, buffer);
    unserialise_double(yt, buffer);

    unserialise_bool(isSpan, buffer);
    unserialise_bool(isArea, buffer);

    if (!isSpan) {
        unserialise_double(iangle, buffer);
    } else {
        iangle = 0;
    }

    angle.set(iangle);
}

/*-------------------------------------------------
 * FUNCTION: gmx_rect::gmx_rect
 *
 * DESCRIPTION:
 *
 *
 */

gmx_rect::gmx_rect(const mx_rect& rr)
{
    type = MX_GEOM_RECT;
    r = rr;
    r.isSpan = FALSE;
}

/*-------------------------------------------------
 * Function: gmx_rect::intersectBox
 *
 * DESCRIPTION:
 *
 *
 */
GEOM_INTERSECT_TYPE gmx_rect::intersectBox(int& err, mx_rect& rtest, double tolerance)
{
    err = MX_ERROR_OK;
    //    return intersect(err,r,rtest,NULL,tolerance) ;
    return GEOM_INTERSECT_NONE;
}

/*-------------------------------------------------
 * FUNCTION: gspan::gspan
 *
 * DESCRIPTION:
 *
 *
 */

gmx_span::gmx_span(const mx_rect& rr)
{
    type = MX_GEOM_SPAN;
    s = rr;
    s.isSpan = TRUE;
    s.isArea = FALSE;
}

/*-------------------------------------------------
 * Function: gmx_span::intersectBox
 *
 * DESCRIPTION:
 *
 *
 */
GEOM_INTERSECT_TYPE gmx_span::intersectBox(int& err, mx_rect& r, double tolerance)
{
    err = MX_ERROR_OK;

    return GEOM_INTERSECT_NONE;

    //    return intersect(err,s,r,NULL,tolerance) ;
}

/*-------------------------------------------------
 * FUNCTION: gmx_rect::get_serialise_size
 *
 * DESCRIPTION:
 *
 *
 */

uint32 gmx_rect::get_serialised_size(int& err)
{
    int size;

    err = MX_ERROR_OK;

    size = mx_geom::get_serialised_size(err);
    MX_ERROR_CHECK(err);

    size += r.get_serialised_size(err);
    MX_ERROR_CHECK(err);
abort:
    return size;
}

/*-------------------------------------------------
 * FUNCTION: mx_grect::serialise
 *
 * DESCRIPTION:
 *
 *
 */

void gmx_rect::serialise(int& err,
    unsigned char** buffer)
{
    err = MX_ERROR_OK;

    mx_geom::serialise(err, buffer);
    MX_ERROR_CHECK(err);

    r.serialise(err, buffer);
    MX_ERROR_CHECK(err);

    return;
abort:
    return;
}

/*-------------------------------------------------
 * FUNCTION: gmx_rect::unserialise
 *
 * DESCRIPTION:
 *
 *
 */

void gmx_rect::unserialise(int& err,
    unsigned char** buffer)
{
    mx_geom::unserialise(err, buffer);
    MX_ERROR_CHECK(err);

    r.unserialise(err, buffer);
    MX_ERROR_CHECK(err);

    return;
abort:
    return;
}

/*-------------------------------------------------
 * FUNCTION: gmx_span::get_serialise_size
 *
 * DESCRIPTION:
 *
 *
 */

uint32 gmx_span::get_serialised_size(int& err)
{
    int size;

    err = MX_ERROR_OK;

    size = mx_geom::get_serialised_size(err);
    MX_ERROR_CHECK(err);

    size += s.get_serialised_size(err);
    MX_ERROR_CHECK(err);
abort:
    return size;
}

/*-------------------------------------------------
 * FUNCTION: mx_gspan::serialise
 *
 * DESCRIPTION:
 *
 *
 */

void gmx_span::serialise(int& err,
    unsigned char** buffer)
{
    err = MX_ERROR_OK;

    mx_geom::serialise(err, buffer);
    MX_ERROR_CHECK(err);

    s.serialise(err, buffer);
    MX_ERROR_CHECK(err);

    return;
abort:
    return;
}

/*-------------------------------------------------
 * FUNCTION: gmx_span::unserialise
 *
 * DESCRIPTION:
 *
 *
 */

void gmx_span::unserialise(int& err,
    unsigned char** buffer)
{
    mx_geom::unserialise(err, buffer);
    MX_ERROR_CHECK(err);

    s.unserialise(err, buffer);
    MX_ERROR_CHECK(err);

    return;
abort:
    return;
}

/*-------------------------------------------------
 * FUNCTION: gmx_span::unserialise
 *
 * DESCRIPTION:
 *
 *
 */

bool mx_rect::intersects(const mx_rect& o) const
{
    mx_rect r1 = normalised();
    mx_rect r2 = o.normalised();
    bool interx = !((r2.xb > r1.xt) || (r2.xt < r1.xb));
    bool intery = !((r2.yb > r1.yt) || (r2.yt < r1.yb));

    return interx && intery;
}

/*-------------------------------------------------
 * FUNCTION: mx_rect::pointIn
 *
 * DESCRIPTION:
 *
 *
 */

bool mx_rect::pointIn(const mx_point& tp) const
{
    mx_rect r = normalisedRect();
    mx_point p = normalisedPoint(tp);

    return (p.x <= r.xt) && (p.x >= r.xb) && (p.y <= r.yt) && (p.y >= r.yb);
}

/*-------------------------------------------------
 * FUNCTION: mx_rect::rectToPolypoint
 *
 * DESCRIPTION:
 *
 *
 */

void mx_rect::rectToPolypoint(bool doAppend,
    mx_polypoint& pline) const
{
    int npoints;

    if (isSpan) {
        npoints = 2;
    } else {
        npoints = doAppend ? 5 : 4;
    }

    if (doAppend) {
        pline.setMemorySize(pline.get_num_points() + npoints);
    } else {
        // clear out old points and make enough space for
        // new points
        pline.clear(npoints);
    }

    if (isSpan) {
        pline.addPoint(start());
        pline.addPoint(end());
    } else {
        pline.addPoint(topLeft());
        pline.addPoint(topRight());
        pline.addPoint(bottomRight());
        pline.addPoint(bottomLeft());

        if (doAppend) {
            pline.addPoint(topLeft());
        } else {
            pline.set_closed(TRUE);
        }
    }
}

mx_rect mx_rect::normalisedRect() const
{
    mx_rect r = normalised();
    r.angle.set(0);

    return r - topLeft();
}

mx_point mx_rect::normalisedPoint(const mx_point& inp) const
{
    mx_point outp = inp - topLeft();

    if (angle.a() != 0) {
        // rotate the point back
        mx_rotation backRotation(-angle.a());
        outp = outp.rotate(backRotation);
    }

    return outp;
}

mx_point mx_rect::unnormalisedPoint(const mx_point& inp) const
{
    mx_point outp;

    if (angle.a() != 0) {
        // rotate the point back
        mx_rotation forwardRotation(angle.a());
        outp = inp.rotate(forwardRotation);
    } else {
        outp = inp;
    }

    outp += topLeft();

    return outp;
}

mx_point mx_rect::topLeft() const
{
    mx_rect r = normalised();
    mx_point p(r.xb, r.yb);
    return p;
}

mx_point mx_rect::bottomLeft() const
{
    mx_rect r = normalised();

    if (angle.a() == 0) {
        return mx_point(r.xb, r.yt);
    } else {
        mx_point p(0, r.yt - r.yb);

        return unnormalisedPoint(p);
    }
}

mx_point mx_rect::bottomRight() const
{
    mx_rect r = normalised();

    if (angle.a() == 0) {
        return mx_point(r.xt, r.yt);
    } else {
        mx_point p(r.xt - r.xb, r.yt - r.yb);

        return unnormalisedPoint(p);
    }
}

mx_point mx_rect::topRight() const
{
    mx_rect r = normalised();

    if (angle.a() == 0) {
        return mx_point(r.xt, r.yb);
    } else {
        mx_point p(r.xt - r.xb, 0);

        return unnormalisedPoint(p);
    }
}

mx_rect mx_rect::box() const
{
    mx_rect r;

    if (angle.a() != 0 && (!isSpan)) {
        mx_point p;

        r.xt = r.xb;
        r.yt = r.yb;

        for (int icorner = 0; icorner < 3; icorner++) {
            switch (icorner) {
            case 0:
                p = topRight();
                break;
            case 1:
                p = bottomRight();
                break;
            case 2:
                p = bottomLeft();
                break;
            }

            if (p.x < r.xb) {
                r.xb = p.x;
            } else if (p.x > r.xt) {
                r.xt = p.x;
            }
            if (p.y < r.yb) {
                r.yb = p.y;
            } else if (p.y > r.yt) {
                r.yt = p.y;
            }
        }
    } else {
        r = *this;
        r.isSpan = FALSE;
        r.normalise();
    }

    return r;
}
