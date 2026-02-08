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
 * MODULE : mx_sdrsp.C
 *
 * AUTHOR : David Miller
 *
 * This file
 *
 * DESCRIPTION:
 * Module mx_sdrsp.C
 *
 *
 * Revision 1.1  1996/10/06 13:15:45  andrew
 * Initial revision
 *
 *
 */

#include <mx_stydev.h>

inline static bool lscomp(int s1, int s2, mx_line_style** ls)
{
    return (ls[s1]->line_id == ls[s2]->line_id) && (ls[s1]->colour_id == ls[s2]->colour_id);
}

void mx_style_device::outputRectLines(int& err,
    const mx_doc_rect_t& rr,
    int startPoint,
    int npoints,
    bool isClosed,
    int lineStyleId,
    int colourId)
{
    int ipoint;
    mx_point inpoint;
    mx_doc_polypoint* docpp = pushPolypoint();

    err = MX_ERROR_OK;

    if ((lineStyleId == -1) || (colourId == -1))
        return;

    docpp->pp.clear(4);
    docpp->pp.set_closed(isClosed);

    for (ipoint = 0; ipoint < npoints; ipoint++) {
        switch (startPoint) {
        case 0:
            inpoint = rr.r.topLeft();
            docpp->pp.addPoint(inpoint);
            break;
        case 1:
            inpoint = rr.r.topRight();
            docpp->pp.addPoint(inpoint);
            break;
        case 2:
            inpoint = rr.r.bottomRight();
            docpp->pp.addPoint(inpoint);
            break;
        case 3:
            inpoint = rr.r.bottomLeft();
            docpp->pp.addPoint(inpoint);
            break;
        }

        startPoint++;
        if (startPoint == 4)
            startPoint = 0;
    }

    drawPolypoint(err,
        *docpp,
        lineStyleId,
        colourId);
    MX_ERROR_CHECK(err);

abort:;
    popPolypoint();
    return;
}

/*-------------------------------------------------
 * FUNCTION: mx_style_device::drawRect
 *
 * DESCRIPTION:
 *
 *
 */

void mx_style_device::drawRect(int& err,
    gmx_rect* grect)
{
    // the style of a rectangle is a border style
    mx_doc_rect_t rr(grect->r.normalised());

    err = MX_ERROR_OK;

    // might want highlights
    if (drawSelectedRect(err, grect))
        return;
    MX_ERROR_CHECK(err);

    // its a line type
    drawSimpleRect(err,
        grect->r.topLeft(),
        grect->r.bottomRight(),
        grect->r.angle,
        grect->styleId, grect->colourId);
    MX_ERROR_CHECK(err);

abort:;
}

/*-------------------------------------------------
 * FUNCTION: mx_style_device::drawSimpleRect
 *
 * DESCRIPTION: This uses a line style ID
 *
 *
 */

void mx_style_device::drawSimpleRect(int& err,
    const mx_doc_coord_t& topLeft,
    const mx_doc_coord_t& bottomRight,
    const mx_angle& angle,
    int styleId,
    int colourId)
{
    err = MX_ERROR_OK;

    if ((styleId != -1) && (colourId != -1)) {
        setupOutputLineStyle(err,
            styleId,
            colourId);
        MX_ERROR_CHECK(err);

        device->drawRect(err, topLeft, bottomRight,
            angle, &outputLineStyle);
        MX_ERROR_CHECK(err);
    }

    return;
abort:
    return;
}

/*-------------------------------------------------
 * FUNCTION: mx_style_device::drawRect
 *
 * DESCRIPTION: Draw a complex rectangle using the
 *              border id
 *
 */

void mx_style_device::drawRect(int& err,
    const mx_doc_rect_t& rr,
    int borderId)
{
    // the style of a rectangle is a border style
    mx_line_style* ls[4];
    int startEdge, allStartEdge, testEdge;
    int runlength;

    err = MX_ERROR_OK;

    if (borderId == -1)
        return;

    setupOutputBorderStyle(err, borderId);
    MX_ERROR_CHECK(err);

    // see where start 0=top,1=right,2=bottom,3=left
    ls[0] = &(currentBorderStyle->top_style);
    ls[1] = &(currentBorderStyle->right_style);
    ls[2] = &(currentBorderStyle->bottom_style);
    ls[3] = &(currentBorderStyle->left_style);

    // look to see the first change

    allStartEdge = 1;
    while (lscomp(allStartEdge, 0, ls)) {
        allStartEdge++;

        // all the way around
        if (allStartEdge == 4)
            break;
    }

    if (allStartEdge == 4) {
        // output a simple drawn rect

        drawSimpleRect(err, rr.r.topLeft(), rr.r.bottomRight(), rr.r.angle,
            (ls[0])->line_id,
            (ls[0])->colour_id);
        MX_ERROR_CHECK(err);
    } else {
        // remember where we start from
        allStartEdge--;

        // have a run of two points
        runlength = 2;
        startEdge = allStartEdge;
        testEdge = startEdge;

        do {
            testEdge++;
            if (testEdge == 4)
                testEdge = 0;

            if (lscomp(startEdge, testEdge, ls)) {
                // same style
                runlength++;
            } else {
                // different stlyle
                outputRectLines(err, rr, startEdge, runlength, FALSE,
                    (ls[startEdge])->line_id,
                    (ls[startEdge])->colour_id);
                MX_ERROR_CHECK(err);

                startEdge = testEdge;
                if (startEdge == allStartEdge)
                    break;
                runlength = 2;
            }
        } while (1);
    }

abort:;
}

/*-------------------------------------------------
 * FUNCTION: mx_style_device::hatchRect
 *
 * DESCRIPTION: Hatch the current polypoint
 *
 *
 */

void mx_style_device::hatchRect(int& err,
    const mx_doc_coord_t& topLeft,
    const mx_doc_coord_t& bottomRight,
    const mx_angle& angle,
    int hatchStyleId)
{
    err = MX_ERROR_OK;

    // the span has a line style

    if (hatchStyleId == -1)
        return;

    mx_rect hatchbox(topLeft.p, bottomRight.p);

    setupOutputHatchStyle(err,
        hatchStyleId);
    MX_ERROR_CHECK(err);

    // hatching is required
    // set clip mask
    device->pushClipRect(err,
        topLeft,
        bottomRight,
        angle,
        TRUE);
    MX_ERROR_CHECK(err);

    hatchBox(err, hatchbox);
    MX_ERROR_CHECK(err);

    device->popClipMask(err);
    MX_ERROR_CHECK(err);

    // to be safe reset the style device here
    reset();

abort:
    return;
}

/*-------------------------------------------------
 * FUNCTION: mx_style_device::fillRect
 *
 * DESCRIPTION:
 *
 *
 */

void mx_style_device::fillSimpleRect(int& err,
    const mx_doc_coord_t& topLeft,
    const mx_doc_coord_t& bottomRight,
    const mx_angle& angle,
    int fillStyleId,
    int fillColourId)
{
    err = MX_ERROR_OK;

    // use the colour on the geometry
    if ((fillStyleId != -1) && (fillColourId != -1)) {
        setupOutputFillStyle(err,
            fillStyleId,
            fillColourId);
        MX_ERROR_CHECK(err);

        // fill the polypoint
        device->fillRect(err,
            topLeft,
            bottomRight,
            angle,
            &outputFillStyle);
        MX_ERROR_CHECK(err);
    }
abort:;
}

/*-------------------------------------------------
 * FUNCTION: mx_style_device::fillRect
 *
 * DESCRIPTION:
 *
 *
 */

void mx_style_device::fillRect(int& err,
    const mx_doc_rect_t& irect,
    int fillStyleId,
    int fillColourId,
    int hatchStyleId)
{
    mx_doc_coord_t topLeft(irect.r.topLeft());
    mx_doc_coord_t bottomRight(irect.r.bottomRight());

    err = MX_ERROR_OK;

    fillSimpleRect(err,
        topLeft,
        bottomRight,
        irect.r.angle,
        fillStyleId,
        fillColourId);
    MX_ERROR_CHECK(err);

    hatchRect(err,
        topLeft,
        bottomRight,
        irect.r.angle,
        hatchStyleId);
    MX_ERROR_CHECK(err);

abort:;
    return;
}

/*-------------------------------------------------
 * FUNCTION: mx_style_device::fillComplexRect
 *
 * DESCRIPTION:
 *
 *
 */

void mx_style_device::fillComplexRect(int& err,
    gmx_rect* grect)
{
    mx_doc_rect_t rr(grect->r.normalised());
    mx_point inpoint;
    int lineStyleId;
    int lineColourId;
    int fillId;
    int hatchId;
    int borderId;

    err = MX_ERROR_OK;

    // the style on the geometry is the area style -
    // this holds a border style and an
    // hatch style in addition to the proper fill style

    setupOutputAreaStyle(err, grect->styleId);
    MX_ERROR_CHECK(err);

    fillIds(err,
        currentAreaStyle,
        borderId,
        fillId,
        hatchId,
        lineStyleId,
        lineColourId);
    MX_ERROR_CHECK(err);

    fillRect(err,
        rr,
        fillId,
        grect->colourId,
        hatchId);
    MX_ERROR_CHECK(err);

    drawRect(err, rr,
        borderId);
    MX_ERROR_CHECK(err);
abort:;
}

/*-------------------------------------------------
 * FUNCTION: mx_style_device::fillRect
 *
 * DESCRIPTION:
 *
 *
 */

void mx_style_device::fillRect(int& err,
    gmx_rect* grect)
{
    err = MX_ERROR_OK;

    // might want highlights
    if (drawSelectedRect(err, grect))
        return;
    MX_ERROR_CHECK(err);

    // its a line type
    fillComplexRect(err, grect);
    MX_ERROR_CHECK(err);

abort:;
}

/*-------------------------------------------------
 * FUNCTION: mx_style_device::drawSelectedSpan
 *
 * DESCRIPTION:
 *
 *
 */

bool mx_style_device::drawSelectedSpan(int& err,
    gmx_span* gspan)
{
    bool doStartPoint, doEndPoint;

    err = MX_ERROR_OK;

    switch (gspan->selectCount) {
    case MX_SPAN_SNAP_NONE:
        return FALSE;
        break;
    case MX_SPAN_SNAP_START:
        doStartPoint = TRUE;
        doEndPoint = FALSE;
        break;
    case MX_SPAN_SNAP_END:
        doStartPoint = FALSE;
        doEndPoint = TRUE;
        break;
    case MX_SPAN_SNAP_WHOLE:
    default:
        doStartPoint = TRUE;
        doEndPoint = TRUE;
        break;
    }

    if (doStartPoint) {
        drawSelectMark(err, gspan->s.start());
        MX_ERROR_CHECK(err);
    }

    if (doEndPoint) {
        drawSelectMark(err, gspan->s.end());
        MX_ERROR_CHECK(err);
    }

    return TRUE;
abort:
    return FALSE;
}

/*-------------------------------------------------
 * FUNCTION: mx_style_device::drawSelectedRect
 *
 * DESCRIPTION:
 *
 *
 */

bool mx_style_device::drawSelectedRect(int& err,
    gmx_rect* grect)
{
    bool drawTL, drawBR, drawBL, drawTR;

    err = MX_ERROR_OK;

    drawTL = drawBR = drawBL = drawTR = FALSE;

    switch (grect->selectCount) {
    case MX_RECT_SNAP_NONE:
        return FALSE;
        break;
    case MX_RECT_SNAP_TL:
        drawTL = TRUE;
        break;
    case MX_RECT_SNAP_TR:
        drawTR = TRUE;
        break;
    case MX_RECT_SNAP_BR:
        drawBR = TRUE;
        break;
    case MX_RECT_SNAP_BL:
        drawBL = TRUE;
        break;
    case MX_RECT_SNAP_TOP:
        drawTL = TRUE;
        drawTR = TRUE;
        break;
    case MX_RECT_SNAP_RIGHT:
        drawTR = TRUE;
        drawBR = TRUE;
        break;
    case MX_RECT_SNAP_BOTTOM:
        drawBL = TRUE;
        drawBR = TRUE;
        break;
    case MX_RECT_SNAP_LEFT:
        drawTL = TRUE;
        drawBL = TRUE;
        break;
    case MX_RECT_SNAP_WHOLE:
    default:
        drawTL = TRUE;
        drawBL = TRUE;
        drawTR = TRUE;
        drawBR = TRUE;
        break;
    }

    if (drawTL) {
        drawSelectMark(err, grect->r.topLeft());
        MX_ERROR_CHECK(err);
    }

    if (drawTR) {
        drawSelectMark(err, grect->r.topRight());
        MX_ERROR_CHECK(err);
    }

    if (drawBL) {
        drawSelectMark(err, grect->r.bottomLeft());
        MX_ERROR_CHECK(err);
    }

    if (drawBR) {
        drawSelectMark(err, grect->r.bottomRight());
        MX_ERROR_CHECK(err);
    }

    return TRUE;

abort:
    return FALSE;
}

/*-------------------------------------------------
 * FUNCTION: mx_style_device::drawSpan
 *
 * DESCRIPTION:
 *
 *
 */

void mx_style_device::drawSpan(int& err,
    gmx_span* gspan)
{
    mx_doc_coord_t start(gspan->s.start());
    mx_doc_coord_t end(gspan->s.end());
    bool startArrow;
    bool endArrow;
    bool doDraw = TRUE;
    bool isSame;
    mx_line_style* thisLineStyle;

    if (drawSelectedSpan(err, gspan))
        return;
    MX_ERROR_CHECK(err);

    thisLineStyle = getLineStyle(err, gspan->styleId, isSame);
    MX_ERROR_CHECK(err);

    if (thisLineStyle->getArrows(startArrow, endArrow)) {
        mx_point sp;
        mx_point usp;
        mx_rect rp(start.p, end.p, TRUE);
        double lineBack1;
        double lineBack2;
        double length;

        sp = rp.uspan(&length);
        usp = sp.flip();

        if (startArrow) {
            drawArrow(err, start, sp, usp,
                thisLineStyle->cap_type,
                thisLineStyle->arrow_pattern,
                thisLineStyle->width,
                gspan->colourId,
                lineBack1);
            MX_ERROR_CHECK(err);

            start.p += lineBack1 * sp;
        }

        if (endArrow) {
            mx_point msp;

            msp = -sp;

            drawArrow(err, end, msp, usp,
                thisLineStyle->cap_type,
                thisLineStyle->arrow_pattern,
                thisLineStyle->width,
                gspan->colourId,
                lineBack2);
            MX_ERROR_CHECK(err);

            end.p += lineBack2 * msp;
        }

        doDraw = (length > lineBack1 + lineBack2);
    }

    if (doDraw) {
        setupOutputLineStyle(err,
            gspan->styleId,
            gspan->colourId);
        MX_ERROR_CHECK(err);

        device->drawLine(err, start, end, &outputLineStyle);
        MX_ERROR_CHECK(err);
    }

    return;
abort:
    return;
}

/*-------------------------------------------------
 * FUNCTION: mx_style_device::hatchBox
 *
 * DESCRIPTION: Hatch a box using the current
 *              hatch style
 *
 */

void mx_style_device::hatchBox(int& err,
    mx_rect& ihatchbox)
{
    int ihatch;
    float iangle;
    float ispacing;
    int ilineStyleId;
    int icolourId;

    double cosang, sinang, rratio;

    double kend, kstart;
    int ikend, ikstart;

    mx_doc_coord_t p1, p2;
    mx_point incPos;
    mx_point an;
    double spacing;
    mx_point startPos, endPos;
    mx_point offsetp;

    mx_point uspan;
    mx_rect hatchbox;
    mx_line_style* lineStyle;
    double addOn;
    bool isSame;
    int iline;

    err = MX_ERROR_OK;

    if (currentHatchStyle == NULL)
        return;

    an = currentHatchStyle->getAnchor();

    for (ihatch = 0; ihatch < 2; ihatch++) {
        // get hatch characteristics

        hatchbox = ihatchbox;

        currentHatchStyle->getHatchValues(ihatch,
            iangle,
            ispacing,
            ilineStyleId,
            icolourId);

        if ((ilineStyleId == -1) || (icolourId == -1))
            continue;

        lineStyle = getLineStyle(err, ilineStyleId, isSame);
        MX_ERROR_CHECK(err);

        addOn = 2.0 * lineStyle->width;

        hatchbox.xb -= addOn;
        hatchbox.yb -= addOn;
        hatchbox.xt += addOn;
        hatchbox.yt += addOn;

        cosang = cos(iangle);
        sinang = sin(iangle);
        if (cosang < 0) {
            cosang = -cosang;
            sinang = -sinang;
        }

        uspan.x = -sinang;
        uspan.y = cosang;

        if (cosang >= M_SQRT1_2) {
            if (sinang > 0) {
                // start hatching based on top left
                startPos.x = hatchbox.xt;
                startPos.y = hatchbox.yb;
                endPos.x = hatchbox.xb;
                endPos.y = hatchbox.yt;
            } else {
                // start hatching based on bottom left
                startPos.x = hatchbox.xb;
                startPos.y = hatchbox.yb;
                endPos.x = hatchbox.xt;
                endPos.y = hatchbox.yt;
            }

            spacing = ispacing / cosang;
            rratio = sinang / cosang;

            // look for solution of an + k*(0,spacing) = startPos + h*(cosang,sinang)
            //                  and an + k*(0,spacing) = endPos + h*(cosang,sinang)
            // to see where the line intersects the an x line

            incPos.x = 0.0;
            incPos.y = spacing;

            offsetp = startPos - an;
            kstart = (offsetp * uspan) / (incPos * uspan);
            ikstart = (int)GNINT(ceil(kstart));

            offsetp = endPos - an;
            kend = (offsetp * uspan) / (incPos * uspan);
            ikend = (int)GNINT(floor(kend));

            p1.p.x = hatchbox.xb;
            p1.p.y = an.y + ikstart * spacing + (hatchbox.xb - an.x) * rratio;

            p2.p.x = hatchbox.xt;
            p2.p.y = p1.p.y + (hatchbox.xt - hatchbox.xb) * rratio;
        } else {
            if (sinang > 0) {
                // start hatching based on top left
                startPos.x = hatchbox.xb;
                startPos.y = hatchbox.yt;
                endPos.x = hatchbox.xt;
                endPos.y = hatchbox.yb;
                spacing = ispacing / sinang;
            } else {
                // start hatching based on bottom left
                startPos.x = hatchbox.xb;
                startPos.y = hatchbox.yb;
                endPos.x = hatchbox.xt;
                endPos.y = hatchbox.yt;
                spacing = -ispacing / sinang;
            }

            rratio = cosang / sinang;

            // look for solution of an + k*(spacing,0) = startPos + h*(cosang,sinang)
            //                  and an + k*(spacing,0) = endPos + h*(cosang,sinang)
            // to see where the line intersects the an x line

            incPos.x = spacing;
            incPos.y = 0.0;

            offsetp = startPos - an;
            kstart = (offsetp * uspan) / (incPos * uspan);
            ikstart = (int)GNINT(ceil(kstart));

            offsetp = endPos - an;
            kend = (offsetp * uspan) / (incPos * uspan);
            ikend = (int)GNINT(floor(kend));

            p1.p.x = an.x + ikstart * spacing + (hatchbox.yb - an.y) * rratio;
            p1.p.y = hatchbox.yb;

            p2.p.x = p1.p.x + (hatchbox.yt - hatchbox.yb) * rratio;
            p2.p.y = hatchbox.yt;
        }

        setupOutputLineStyle(err, ilineStyleId, icolourId);
        MX_ERROR_CHECK(err);

        for (iline = ikstart; iline <= ikend; iline++) {
            if (iline == ikstart) {
                device->drawLine(err, p1, p2, &outputLineStyle);
                MX_ERROR_CHECK(err);
            } else {
                device->drawLine(err, p1, p2);
                MX_ERROR_CHECK(err);
            }

            p1.p += incPos;
            p2.p += incPos;
        }
    }

    return;
abort:
    return;
}
