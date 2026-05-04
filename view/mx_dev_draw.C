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
/* MODULE : mx_device.C
 *
 * AUTHOR : David Miller
 *
 * This file
 *
 * DESCRIPTION:
 * Module mx_device.C
 *
 *
 *
 *
 */

#include "mx_device.h"
#include <mx.h>
#include <mx_frame.h>

/*-------------------------------------------------
 * FUNCTION: mx_device::drawRect
 *
 * DESCRIPTION:
 *
 *
 */

// draw a rectangle in maxwell coordinates
void mx_device::drawRect(int& err,
    const mx_doc_coord_t& topLeft,
    const mx_doc_coord_t& bottomRight,
    const mx_angle& angle,
    mx_line_style* lineStyle)
{
    err = MX_ERROR_OK;

    // set the line style if needed
    if (setNewLineStyle(lineStyle)) {
        setDefaultLineStyle(err, *lineStyle);
        MX_ERROR_CHECK(err);
    }

    if (invalidLineStyle(lineStyle))
        return;

    if ((angle.a() == 0) || doesRotate) {
        mx_doc_coord_t ptopLeft;
        mx_doc_coord_t pbottomRight;
        mx_ipoint pptopLeft;
        mx_ipoint ppbottomRight;

        ptopLeft = frameTarget2dev(err, topLeft);
        MX_ERROR_CHECK(err);

        pbottomRight = frameTarget2dev(err, bottomRight);
        MX_ERROR_CHECK(err);

        pptopLeft = pixelSnap(ptopLeft);
        ppbottomRight = pixelSnap(pbottomRight);

        pdrawRect(err, pptopLeft, ppbottomRight, angle);
        MX_ERROR_CHECK(err);
    } else {
        // need to convert to a polypoint

        mx_rect r(topLeft.p, bottomRight.p, FALSE, FALSE, angle.a());

        r.rectToPolypoint(FALSE, docppline.pp);

        drawPolypoint(err, docppline);
        MX_ERROR_CHECK(err);
    }

abort:;
}

/*-------------------------------------------------
 * FUNCTION: mx_device::fillRect
 *
 * DESCRIPTION:
 *
 *
 */

// draw a rectangle in maxwell coordinates
void mx_device::fillRect(int& err,
    const mx_doc_coord_t& topLeft,
    const mx_doc_coord_t& bottomRight,
    const mx_angle& angle,
    mx_fill_style* fillStyle)
{
    err = MX_ERROR_OK;

    if (setNewFillStyle(fillStyle)) {
        setDefaultFillStyle(err, *fillStyle);
        MX_ERROR_CHECK(err);
    }

    if (invalidFillStyle(fillStyle))
        return;

    if ((angle.a() == 0) || doesRotate) {
        mx_doc_coord_t ptopLeft;
        mx_doc_coord_t pbottomRight;
        mx_ipoint pptopLeft;
        mx_ipoint ppbottomRight;

        ptopLeft = frameTarget2dev(err, topLeft);
        MX_ERROR_CHECK(err);

        pbottomRight = frameTarget2dev(err, bottomRight);
        MX_ERROR_CHECK(err);

        pptopLeft = pixelSnap(ptopLeft);
        ppbottomRight = pixelSnap(pbottomRight);

        pfillRect(err, pptopLeft, ppbottomRight, angle);
        MX_ERROR_CHECK(err);
    } else {
        // need to convert to a polypoint

        mx_rect r(topLeft.p, bottomRight.p, FALSE, FALSE, angle.a());

        r.rectToPolypoint(FALSE, docppline.pp);

        fillPolypoint(err, docppline);
        MX_ERROR_CHECK(err);
    }
abort:;
}

/*-------------------------------------------------
 * FUNCTION: mx_device::drawText
 *
 * DESCRIPTION:
 *
 *
 */

void mx_device::drawText(int& err,
    const char* s,
    uint32 s_len,
    const mx_doc_coord_t& pos,
    mx_char_style& charStyle)
{
    mx_doc_coord_t ppos;
    mx_ipoint pppos;

    ppos = frameTarget2dev(err, pos);
    MX_ERROR_CHECK(err);

    pppos = pixelSnap(ppos);

    (void)pdrawText(err, s, s_len, pppos, charStyle);
    MX_ERROR_CHECK(err);
abort:;
}

/*-------------------------------------------------
 * FUNCTION: mx_device::drawText
 *
 * DESCRIPTION:
 *
 *
 */

void mx_device::drawText(int& err,
    const char* s,
    const mx_doc_coord_t& pos,
    mx_char_style& charStyle)
{
    drawText(err, s, strlen(s), pos, charStyle);
}

/*-------------------------------------------------
 * FUNCTION: mx_device::drawTextLine
 *
 * DESCRIPTION: Draw a line of text with varying character styles
 *
 *
 */

void mx_device::drawTextLine(int& err,
    mx_doc_coord_t& pos,
    mx_char_style& init_char_style,
    const mx_text_item* text_items,
    uint32 num_items)
{
    uint32 i, j;
    mx_char_style s = init_char_style;
    mx_doc_coord_t p = pos;

    // temporary implementation which just prints every character individually

    for (i = 0; i < num_items; i++) {
        for (j = 0; j < text_items[i].num_chars; j++) {
            drawText(err, (text_items[i].chars) + j, 1, p, s);
            MX_ERROR_CHECK(err);
        }

        p.p.x += text_items[i].delta;

        if (text_items[i].mod != NULL) {
            s += *text_items[i].mod;
        }
    }
abort:;
}

/*-------------------------------------------------
 * FUNCTION: mx_device::getCharWidth
 *
 * DESCRIPTION:
 *
 *
 */

float mx_device::getCharWidth(int& err,
    char c,
    mx_char_style& cs)
{
    double pwidth = pgetCharWidth(err, c, cs);
    MX_ERROR_CHECK(err);

    return dev2frame(pwidth);
abort:;
    return 0;
}

/*-------------------------------------------------
 * FUNCTION: mx_device::drawLine
 *
 * DESCRIPTION:
 *
 *
 */

void mx_device::drawLine(int& err,
    const mx_doc_coord_t& start,
    const mx_doc_coord_t& end,
    mx_line_style* lineStyle)
{
    mx_doc_coord_t pstart;
    mx_doc_coord_t pend;
    mx_ipoint ppstart;
    mx_ipoint ppend;

    err = MX_ERROR_OK;

    // set the line style if needed
    if (setNewLineStyle(lineStyle)) {
        setDefaultLineStyle(err, *lineStyle);
        MX_ERROR_CHECK(err);
    }

    if (invalidLineStyle(lineStyle))
        return;

    pstart = frameTarget2dev(err, start);
    MX_ERROR_CHECK(err);

    pend = frameTarget2dev(err, end);
    MX_ERROR_CHECK(err);

    ppstart = pixelSnap(pstart);
    ppend = pixelSnap(pend);

    pdrawLine(err, ppstart, ppend);
    MX_ERROR_CHECK(err);
abort:;
}

/*-------------------------------------------------
 * FUNCTION: mx_device::arcPoints
 *
 * DESCRIPTION:
 *
 *
 */

void mx_device::arcPoints(int& err,
    mx_doc_arc& a,
    mx_ipoint& ppcentre,
    mx_ipoint& ppsize)
{
    mx_doc_coord_t pcentre;
    mx_doc_coord_t centre;

    err = MX_ERROR_OK;

    centre.sheet_number = a.sheet_number;
    centre.p = a.arc.gc();

    pcentre = frameTarget2dev(err, centre);
    MX_ERROR_CHECK(err);

    ppcentre = pixelSnap(pcentre);

    ppsize.x = (int)frame2dev(a.arc.gs().x);
    ppsize.y = (int)frame2dev(a.arc.gs().y);

    if (ppsize.x <= 0)
        ppsize.x = 1;
    if (ppsize.y <= 0)
        ppsize.y = 1;

    return;
abort:;
}

/*-------------------------------------------------
 * FUNCTION: mx_device::drawArc
 *
 * DESCRIPTION:
 *
 *
 */

void mx_device::drawArc(int& err,
    mx_doc_arc& a,
    mx_line_style* lineStyle)
{
    mx_ipoint ppcentre;
    mx_ipoint ppsize;

    err = MX_ERROR_OK;

    // set the line style if needed
    if (setNewLineStyle(lineStyle)) {
        setDefaultLineStyle(err, *lineStyle);
        MX_ERROR_CHECK(err);
    }

    if (invalidLineStyle(lineStyle))
        return;

    if ((a.arc.gan().a() == 0) || doesRotate) {
        arcPoints(err, a, ppcentre, ppsize);
        MX_ERROR_CHECK(err);

        pdrawArc(
            err,
            ppcentre,
            ppsize,
            a.arc.isFull(),
            a.arc.gsa(),
            a.arc.gea(),
            a.arc.gan());
        MX_ERROR_CHECK(err);
    } else {
        // device doesnt draw a rotated arc
        // polypoint instead

        // approximate to half a pixel in the frame

        double tolerance = 0.5 / frameResolution();

        // convert the arc to a polypoint -  let closure
        // be done by the draw polypoint routine

        a.arc.arcToBoundingPolypoint(tolerance, docppline.pp);

        // now convert polypoint points to ipolypoint points

        drawPolypoint(err, docppline);
        MX_ERROR_CHECK(err);
    }

    return;

abort:;
}

/*-------------------------------------------------
 * FUNCTION: mx_device::fillArc
 *
 * DESCRIPTION:
 *
 *
 */

void mx_device::fillArc(int& err,
    mx_doc_arc& a,
    mx_fill_style* fillStyle)
{
    mx_ipoint ppcentre;
    mx_ipoint ppsize;

    err = MX_ERROR_OK;

    if (setNewFillStyle(fillStyle)) {
        setDefaultFillStyle(err, *fillStyle);
        MX_ERROR_CHECK(err);
    }

    if (invalidFillStyle(fillStyle))
        return;

    if ((a.arc.gan().a() == 0) || doesRotate) {
        arcPoints(err, a, ppcentre, ppsize);
        MX_ERROR_CHECK(err);

        pfillArc(err, ppcentre, ppsize, a.arc.isFull(), a.arc.gsa(),
            a.arc.gea(), a.arc.gan());
        MX_ERROR_CHECK(err);
    } else {
        // device doesnt draw a sector - we need to draw a
        // polypoint instead

        // approximate to half a pixel in the frame

        double tolerance = 0.5 / frameResolution();

        // convert the arc to a polypoint -  let closure
        // be done by the draw polypoint routine

        a.arc.arcToBoundingPolypoint(tolerance, docppline.pp);

        // now convert polypoint points to ipolypoint points

        fillPolypoint(err, docppline);
        MX_ERROR_CHECK(err);
    }
abort:
    return;
}

/*-------------------------------------------------
 * FUNCTION: mx_device::drawSector
 *
 * DESCRIPTION:
 *
 *
 */

void mx_device::drawSector(int& err,
    mx_doc_arc& a,
    mx_line_style* lineStyle)
{
    mx_ipoint ppcentre;
    mx_ipoint ppsize;
    double tolerance;

    err = MX_ERROR_OK;

    // set the line style if needed
    if (setNewLineStyle(lineStyle)) {
        setDefaultLineStyle(err, *lineStyle);
        MX_ERROR_CHECK(err);
    }

    if (invalidLineStyle(lineStyle))
        return;

    if (drawsSector && ((a.arc.gan().a() == 0) || doesRotate)) {
        arcPoints(err, a, ppcentre, ppsize);
        MX_ERROR_CHECK(err);

        pdrawSector(err, ppcentre, ppsize, a.arc.isFull(), a.arc.gsa(),
            a.arc.gea(), a.arc.gan());
        MX_ERROR_CHECK(err);
    } else {
        // device doesnt draw a sector - we need to draw a
        // polypoint instead

        // approximate to half a pixel in the frame

        tolerance = 0.5 / frameResolution();

        // convert the arc to a polypoint -  let closure
        // be done by the draw polypoint routine

        a.arc.arcToBoundingPolypoint(tolerance, docppline.pp);

        // now convert polypoint points to ipolypoint points

        drawPolypoint(err, docppline);
        MX_ERROR_CHECK(err);
    }
abort:;
}

/*-------------------------------------------------
 * FUNCTION: mx_device::fillSector
 *
 * DESCRIPTION:
 *
 *
 */

void mx_device::fillSector(int& err,
    mx_doc_arc& a,
    mx_fill_style* fillStyle)
{
    double tolerance;
    mx_ipoint ppcentre;
    mx_ipoint ppsize;

    err = MX_ERROR_OK;

    if (setNewFillStyle(fillStyle)) {
        setDefaultFillStyle(err, *fillStyle);
        MX_ERROR_CHECK(err);
    }

    if (invalidFillStyle(fillStyle))
        return;

    if (fillsSector && ((a.arc.gan().a() == 0) || doesRotate)) {
        arcPoints(err, a, ppcentre, ppsize);
        MX_ERROR_CHECK(err);

        pfillSector(err, ppcentre, ppsize, a.arc.isFull(), a.arc.gsa(),
            a.arc.gea(), a.arc.gan());
        MX_ERROR_CHECK(err);
    } else {
        // device doesnt draw a sector - we need to draw a
        // polypoint instead

        // approximate to half a pixel in the frame

        tolerance = 0.5 / frameResolution();

        // convert the arc to a polypoint -  let closure
        // be done by the draw polypoint routine

        a.arc.arcToBoundingPolypoint(tolerance, docppline.pp);

        // now convert polypoint points to ipolypoint points

        fillPolypoint(err, docppline);
        MX_ERROR_CHECK(err);
    }
abort:;
}

/*-------------------------------------------------
 * FUNCTION: mx_device::drawChord
 *
 * DESCRIPTION:
 *
 *
 */

void mx_device::drawChord(int& err,
    mx_doc_arc& a,
    mx_line_style* lineStyle)
{
    mx_ipoint ppcentre;
    mx_ipoint ppsize;
    double tolerance;

    err = MX_ERROR_OK;

    // set the line style if needed
    if (setNewLineStyle(lineStyle)) {
        setDefaultLineStyle(err, *lineStyle);
        MX_ERROR_CHECK(err);
    }

    if (invalidLineStyle(lineStyle))
        return;

    if (drawsChord && ((a.arc.gan().a() == 0) || doesRotate)) {
        arcPoints(err, a, ppcentre, ppsize);
        MX_ERROR_CHECK(err);

        pdrawChord(err, ppcentre, ppsize, a.arc.isFull(), a.arc.gsa(),
            a.arc.gea(), a.arc.gan());
        MX_ERROR_CHECK(err);
    } else {
        // device doesnt draw a sector - we need to draw a
        // polypoint instead

        // approximate to half a pixel in the frame

        tolerance = 0.5 / frameResolution();

        // convert the arc to a polypoint -  let closure
        // be done by the draw polypoint routine

        a.arc.arcToBoundingPolypoint(tolerance, docppline.pp);

        drawPolypoint(err, docppline);
        MX_ERROR_CHECK(err);
    }

    return;

abort:;
}

void mx_device::drawCircle(int& err,
    mx_doc_coord_t& centre,
    float radius,
    mx_line_style* lineStyle)
{
    mx_doc_coord_t pcentre;
    mx_ipoint ppcentre;

    err = MX_ERROR_OK;

    int iradius = (int)frame2dev(radius);

    if (iradius <= 0) {
        iradius = 1;
    }

    // set the line style if needed
    if (setNewLineStyle(lineStyle)) {
        setDefaultLineStyle(err, *lineStyle);
        MX_ERROR_CHECK(err);
    }

    if (invalidLineStyle(lineStyle))
        return;

    pcentre = frameTarget2dev(err, centre);
    MX_ERROR_CHECK(err);

    ppcentre = pixelSnap(pcentre);

    pdrawCircle(err, ppcentre, iradius);
    MX_ERROR_CHECK(err);
abort:;
}

void mx_device::translatePolypoint(int& err,
    mx_doc_polypoint& dpp,
    bool addClosurePoint,
    mx_ipolypoint& ipp)
{
    int i;
    mx_doc_coord_t dc;
    mx_ipoint ipt;
    mx_point newStartPoint;
    mx_point newEndPoint;

    int num_points = dpp.pp.get_num_points();

    double startMask = dpp.pp.getStartMask();
    int istartMask = (int)startMask;
    double rstartMask = startMask - istartMask;
    double endMask = dpp.pp.getEndMask();
    int iendMask = (int)endMask;
    double rendMask = endMask - iendMask;

    bool doNewStartPoint;
    bool doNewEndPoint;

    int istartPoint;
    int iendPoint;
    int naddPoints = 0;

    err = MX_ERROR_OK;

    // nothing to do - empty polypoint
    if (startMask + endMask >= num_points - 1) {
        ipp.clear(0);
        return;
    }

    doNewStartPoint = (rstartMask > 0);
    if (doNewStartPoint) {
        istartPoint = istartMask + 1;
        newStartPoint = dpp.pp.getPoint(startMask);
        naddPoints++;
    } else {
        istartPoint = istartMask;
        // store new start point for closure
        newStartPoint = dpp.pp[istartMask];
    }

    // the end of the polypoint is clipped

    doNewEndPoint = (rendMask > 0);
    if (doNewEndPoint) {
        iendPoint = num_points - iendMask - 1;
        newEndPoint = dpp.pp.getPoint(num_points - endMask - 1);
        naddPoints++;
    } else {
        iendPoint = num_points - iendMask;
    }

    // use the global ipolypoint - clear all
    // except for memory - add 2 points for possible
    // interpolated end points
    ipp.clear(iendPoint - istartPoint + naddPoints);

    // set the closure
    ipp.set_closed(dpp.pp.get_closed());

    if (doNewStartPoint) {
        dc.p = newStartPoint;
        dc.sheet_number = dpp.sheet_number;
        ipt = pixelSnap(frameTarget2dev(err, dc));
        MX_ERROR_CHECK(err);
        ipp.addPoint(ipt);
    }

    for (i = istartPoint; i < iendPoint; i++) {
        dc.p = dpp.pp[i];
        dc.sheet_number = dpp.sheet_number;
        ipt = pixelSnap(frameTarget2dev(err, dc));
        MX_ERROR_CHECK(err);
        ipp.addPoint(ipt);
    }

    if (doNewEndPoint) {
        dc.p = newEndPoint;
        dc.sheet_number = dpp.sheet_number;
        ipt = pixelSnap(frameTarget2dev(err, dc));
        MX_ERROR_CHECK(err);
        ipp.addPoint(ipt);
    }

    // if it is closed but the device requires a coincident
    // last point - then input start point again

    if (addClosurePoint) {
        // add the new point to the polypoint
        dc.p = newStartPoint;
        dc.sheet_number = dpp.sheet_number;
        ipt = pixelSnap(frameTarget2dev(err, dc));
        MX_ERROR_CHECK(err);
        ipp.addPoint(ipt);
    }

    return;
abort:
    return;
}

void mx_device::drawPoints(int& err,
    mx_doc_polypoint& pp)
{
    translatePolypoint(err,
        pp,
        FALSE,
        ippline);
    MX_ERROR_CHECK(err);

    // draw the line
    pdrawPoints(err, ippline);
    MX_ERROR_CHECK(err);
abort:;
}

void mx_device::drawPolypoint(int& err,
    mx_doc_polypoint& dpp,
    mx_line_style* lineStyle)
{
    bool addClosurePoint = (dpp.pp.get_closed()) && (!closesLinesWithoutLastPoint);

    err = MX_ERROR_OK;

    // set the line style if needed
    if (setNewLineStyle(lineStyle)) {
        setDefaultLineStyle(err, *lineStyle);
        MX_ERROR_CHECK(err);
    }

    if (invalidLineStyle(lineStyle))
        return;

    translatePolypoint(err,
        dpp,
        addClosurePoint,
        ippline);
    MX_ERROR_CHECK(err);

    // draw the line
    pdrawPolypoint(err, ippline);
    MX_ERROR_CHECK(err);

    return;
abort:;
}

void mx_device::drawSegments(int& err,
    mx_doc_polypoint& dpp,
    bool linesCoincident,
    mx_line_style* lineStyle)
{
    err = MX_ERROR_OK;

    // set the line style if needed
    if (setNewLineStyle(lineStyle)) {
        setDefaultLineStyle(err, *lineStyle);
        MX_ERROR_CHECK(err);
    }

    if (invalidLineStyle(lineStyle))
        return;

    translatePolypoint(err,
        dpp,
        FALSE,
        ippline);
    MX_ERROR_CHECK(err);

    // draw the line
    pdrawSegments(err, ippline, linesCoincident);
    MX_ERROR_CHECK(err);

    return;
abort:;
}

void mx_device::fillPolypoint(int& err,
    mx_doc_polypoint& dpp,
    mx_fill_style* fillStyle)
{
    err = MX_ERROR_OK;

    // set the line style if needed
    if (setNewFillStyle(fillStyle)) {
        setDefaultFillStyle(err, *fillStyle);
        MX_ERROR_CHECK(err);
    }

    if (invalidFillStyle(fillStyle))
        return;

    translatePolypoint(err,
        dpp,
        FALSE,
        ippline);
    MX_ERROR_CHECK(err);

    pfillPolypoint(err, ippline);
    MX_ERROR_CHECK(err);

    return;
abort:;
}

void mx_device::fillCircle(int& err,
    const mx_doc_coord_t& centre,
    float radius,
    mx_fill_style* fillStyle)
{
    mx_doc_coord_t pcentre;
    mx_ipoint ppcentre;

    err = MX_ERROR_OK;

    int iradius = (int)frame2dev(radius);

    if (iradius <= 0) {
        iradius = 1;
    }

    if (setNewFillStyle(fillStyle)) {
        setDefaultFillStyle(err, *fillStyle);
        MX_ERROR_CHECK(err);
    }

    if (invalidFillStyle(fillStyle))
        return;

    pcentre = frameTarget2dev(err, centre);
    MX_ERROR_CHECK(err);

    ppcentre = pixelSnap(pcentre);

    pfillCircle(err, ppcentre, iradius);
    MX_ERROR_CHECK(err);
abort:;
}

/*-------------------------------------------------
 * FUNCTION: mx_device::xorRect
 *
 * DESCRIPTION:
 *
 *
 */

// xor a rectangle in maxwell coordinates
void mx_device::xorRect(int& err,
    mx_doc_coord_t& topLeft,
    mx_doc_coord_t& bottomRight)
{
    mx_doc_coord_t ptopLeft;
    mx_doc_coord_t pbottomRight;
    mx_ipoint pptopLeft;
    mx_ipoint ppbottomRight;

    ptopLeft = frameTarget2dev(err, topLeft);
    MX_ERROR_CHECK(err);

    pbottomRight = frameTarget2dev(err, bottomRight);
    MX_ERROR_CHECK(err);

    pptopLeft = pixelSnap(ptopLeft);
    ppbottomRight = pixelSnap(pbottomRight);

    pxorRect(err, pptopLeft, ppbottomRight);
    MX_ERROR_CHECK(err);

abort:;
}

/*-------------------------------------------------
 * FUNCTION: mx_device::drawTile
 *
 * DESCRIPTION:
 *
 *
 */

void mx_device::drawTile(int& err,
    MX_DEVICE_RASTER tileType,
    int nsigbits,
    int nunpackedBits,
    mx_ipoint& tileSize,
    mx_doc_coord_t& topLeft,
    void* tile,
    mx_irect* clipRect)
{
    mx_doc_coord_t ptopLeft;
    mx_ipoint pptopLeft;

    err = MX_ERROR_OK;

    ptopLeft = frameTarget2dev(err, topLeft);
    MX_ERROR_CHECK(err);

    pptopLeft = pixelSnap(ptopLeft);

    pdrawTile(err, tileType, nsigbits, nunpackedBits, tileSize, pptopLeft,
        tile, clipRect);
    MX_ERROR_CHECK(err);

    return;
abort:
    return;
}

/*-------------------------------------------------
 * FUNCTION: mx_device::xorResizeHandles
 *
 * DESCRIPTION:
 *
 * draw a set of "resize handles", ie eight little boxes at the corner of an
 * unfilled reactangle. Done all in xormode so it can be undone quickly by
 * doing another call with the same parameters
 *
 */

void mx_device::xorResizeHandles(int& err,
    const mx_doc_coord_t& topLeft,
    const mx_doc_coord_t& bottomRight,
    double handleWidth)
{
    mx_line_style ls(mx_dotted, 0.0);
    mx_doc_coord_t ptopLeft;
    mx_doc_coord_t pbottomRight;
    mx_ipoint pptopLeft;
    mx_ipoint ppbottomRight;
    double pHW;
    uint32 ppHW;

    err = MX_ERROR_OK;

    setDefaultLineStyle(err, ls);
    MX_ERROR_CHECK(err);

    ptopLeft = frameTarget2dev(err, topLeft);
    MX_ERROR_CHECK(err);

    pbottomRight = frameTarget2dev(err, bottomRight);
    MX_ERROR_CHECK(err);

    pHW = frame2dev(handleWidth);
    MX_ERROR_CHECK(err);

    pptopLeft = pixelSnap(ptopLeft);
    ppbottomRight = pixelSnap(pbottomRight);
    ppHW = (int)(pHW + 0.5);

    // minimum size 1 pixel
    if (ppHW < 1)
        ppHW = 1;

    pxorResizeHandles(err, pptopLeft, ppbottomRight, ppHW);
    MX_ERROR_CHECK(err);

abort:;
}

/*-------------------------------------------------
 * FUNCTION: mx_device::pxorResizeHandles
 *
 * DESCRIPTION:
 *
 * default is to do nothing for this routine
 *
 */
void mx_device::pxorResizeHandles(int& err,
    const mx_ipoint& topLeft,
    const mx_ipoint& bottomRight,
    uint32 handleWidth)
{
}
