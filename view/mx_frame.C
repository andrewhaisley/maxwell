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
 * MODULE : mx_frame.C
 *
 * AUTHOR : David Miller
 *
 * This file
 *
 * DESCRIPTION: Frame handling
 * Module mx_frame.C
 *
 *
 *
 *
 */

#include <mx_device.h>
#include <mx_frame.h>

/*-------------------------------------------------
 * FUNCTION: mx_frame::mx_frame
 *
 * DESCRIPTION: Frame constructor
 *
 *
 */

mx_frame::mx_frame(int& err,
    const mx_doc_coord_t& initialTopLeft,
    const mx_point& initialSize,
    double initialZoom,
    mx_frame_target& iframeTarget)
    : size(initialSize)
    , scrollLimitBox(0, 0, 0, 0)
    , targetLimitAdd(0, 0)
    , falseOrigin(0, 0)
    , frameTarget(iframeTarget)
{
    err = MX_ERROR_OK;

    outputDevice = NULL;
    zoom = initialZoom;
    docCoordToCoord(err, initialTopLeft, topLeft);
    MX_ERROR_CHECK(err);

    setDrawPerSheet(FALSE);
    useFalseOrigin = FALSE;

    return;
abort:
    return;
}

/*-------------------------------------------------
 * FUNCTION: mx_frame::~mx_frame
 *
 * DESCRIPTION: Frame destructor
 *
 *
 */

mx_frame::~mx_frame()
{
    delete outputDevice;
}

/*-------------------------------------------------
 * FUNCTION: mx_frame::setTopLeft
 *
 * DESCRIPTION:
 *
 *
 */

void mx_frame::setTopLeft(int& err, const mx_point& itopLeft)

{
    mx_doc_coord_t sp, asp;

    sp.sheet_number = 0;
    sp.p = itopLeft;

    frameScroll(err, sp, MX_SCROLL_OFFSET_TL, FALSE, FALSE, asp, TRUE);
    MX_ERROR_CHECK(err);

    return;
abort:
    return;
}

/*-------------------------------------------------
 * FUNCTION: mx_frame::setSize
 *
 * DESCRIPTION: Frame destructor
 *
 *
 */

void mx_frame::setSize(int& err, const mx_point& isize)
{
    err = MX_ERROR_OK;
    size = isize;
}

/*-------------------------------------------------
 * FUNCTION: mx_frame::setZoom
 *
 * DESCRIPTION: Frame destructor
 *
 *
 */

void mx_frame::setZoom(int& err, double izoom)
{
    err = MX_ERROR_OK;
    zoom = izoom;

    // get the size of the device in frame coordinates
    isetSize(err, outputDevice->getFrameSize());
    MX_ERROR_CHECK(err);

    outputDevice->refresh(err);
    MX_ERROR_CHECK(err);

    return;
abort:
    return;
}

/*-------------------------------------------------
 * FUNCTION: mx_frame::setTopLeftFrame
 *
 * DESCRIPTION: Frame destructor
 *
 *
 */

void mx_frame::setTopLeftFrame(int& err, const mx_doc_coord_t& frameCoord)
{
    mx_point itopLeft;

    docCoordToCoord(err, frameCoord, itopLeft);
    MX_ERROR_CHECK(err);

    setTopLeft(err, itopLeft);
    MX_ERROR_CHECK(err);

    return;
abort:
    return;
}

/*-------------------------------------------------
 * FUNCTION: mx_frame::frameScroll
 *
 * DESCRIPTION: Scoll a window in frame
 * coordinates.
 *
 */

void mx_frame::frameScroll(int& err,
    mx_doc_coord_t& scrollPosition,
    MX_SCROLL_OFFSET offset,
    bool sameX,
    bool sameY,
    mx_doc_coord_t& actualScrollPosition,
    bool doScroll)
{
    mx_point pointOffset(0, 0);
    mx_doc_coord_t topLeftCoord;
    mx_doc_coord_t snappedTopLeftCoord;

    // calculate the top left coordinate based on frame size the scroll position
    // and the offset

    mx_point frameSize = getSize();
    mx_point frameSize2 = 0.5 * frameSize;

    switch (offset) {
    case MX_SCROLL_OFFSET_TL:
        break;
    case MX_SCROLL_OFFSET_T:
        pointOffset.x = frameSize2.x;
        break;
    case MX_SCROLL_OFFSET_TR:
        pointOffset.x = frameSize.x;
        break;
    case MX_SCROLL_OFFSET_L:
        pointOffset.y = frameSize2.y;
        break;
    case MX_SCROLL_OFFSET_C:
        pointOffset.x = frameSize2.x;
        pointOffset.y = frameSize2.y;
        break;
    case MX_SCROLL_OFFSET_R:
        pointOffset.x = frameSize.x;
        pointOffset.y = frameSize2.y;
        break;
    case MX_SCROLL_OFFSET_BL:
        pointOffset.y = frameSize.y;
        break;
    case MX_SCROLL_OFFSET_B:
        pointOffset.x = frameSize2.x;
        pointOffset.y = frameSize.y;
        break;
    case MX_SCROLL_OFFSET_BR:
        pointOffset.x = frameSize.x;
        pointOffset.y = frameSize.y;
        break;
    }

    topLeftCoord.sheet_number = 0;

    if (sameX) {
        topLeftCoord.p.x = topLeft.x;
    } else {
        topLeftCoord.p.x = scrollPosition.p.x - pointOffset.x;
    }

    if (sameY) {
        topLeftCoord.p.y = topLeft.y;
    } else {
        topLeftCoord.p.y = scrollPosition.p.y - pointOffset.y;
    }

    clippedTopLeftCoord(err, topLeftCoord.p);
    MX_ERROR_CHECK(err);

    // set the shift origin prior to the scroll
    outputDevice->setShiftOrigin(topLeftCoord, snappedTopLeftCoord, doScroll);

    if (doScroll) {
        // reset the new top left coordinated
        isetTopLeft(snappedTopLeftCoord.p);

        // and shift
        outputDevice->shift(err);
        MX_ERROR_CHECK(err);
    }

    actualScrollPosition.sheet_number = 0;

    if (sameX) {
        actualScrollPosition.p.x = topLeft.x;
    } else {
        actualScrollPosition.p.x = snappedTopLeftCoord.p.x + pointOffset.x;
    }

    if (sameY) {
        actualScrollPosition.p.y = topLeft.y;
    } else {
        actualScrollPosition.p.y = snappedTopLeftCoord.p.y + pointOffset.y;
    }

    return;
abort:
    return;
}

/*-------------------------------------------------
 * FUNCTION: mx_frame::isetSize
 *
 * DESCRIPTION:
 *
 *
 */

void mx_frame::isetSize(int& err, const mx_point& isize)

{
    err = MX_ERROR_OK;
    size = isize;
}

/*-------------------------------------------------
 * FUNCTION: mx_frame::clipTopLeftCoord
 *
 * DESCRIPTION:
 *
 *
 */

void mx_frame::clippedTopLeftCoord(int& err,
    mx_point& topLeftCoord)
{
    err = MX_ERROR_OK;

    if (!applyScrollLimit)
        return;

    mx_rect tscrollLimitBox;

    if (getApplyScrollLimitToTarget()) {
        mx_point frameSize = getFrameTarget().getTotalSize(err);
        MX_ERROR_CHECK(err);

        tscrollLimitBox.xb = -targetLimitAdd.xb;
        tscrollLimitBox.yb = -targetLimitAdd.yb;
        tscrollLimitBox.xt = frameSize.x + targetLimitAdd.xt;
        tscrollLimitBox.yt = frameSize.y + targetLimitAdd.yt;
    } else {
        tscrollLimitBox = scrollLimitBox;
    }

    if (topLeftCoord.x + size.x > tscrollLimitBox.xt) {
        topLeftCoord.x = tscrollLimitBox.xt - size.x;
    }

    if (topLeftCoord.x < tscrollLimitBox.xb) {
        topLeftCoord.x = tscrollLimitBox.xb;
    }

    if (topLeftCoord.y + size.y > tscrollLimitBox.yt) {
        topLeftCoord.y = tscrollLimitBox.yt - size.y;
    }

    if (topLeftCoord.y < tscrollLimitBox.yb) {
        topLeftCoord.y = tscrollLimitBox.yb;
    }

    return;
abort:
    return;
}

/*-------------------------------------------------
 * FUNCTION: mx_frame::scroll
 *
 * DESCRIPTION: Scoll a window in frame target
 * coordinates .
 *
 */

void mx_frame::scroll(int& err,
    mx_doc_coord_t& scrollPosition,
    MX_SCROLL_OFFSET offset,
    bool sameX,
    bool sameY,
    mx_doc_coord_t& actualScrollPosition)
{
    mx_doc_coord_t frameCoord;
    mx_doc_coord_t actualFrameCoord;

    err = MX_ERROR_OK;

    // convert initial frame target coordinate to frame coodinate

    frameCoord.sheet_number = 0;
    docCoordToCoord(err, scrollPosition, frameCoord.p);
    MX_ERROR_CHECK(err);

    frameScroll(err, frameCoord, offset, sameX, sameY, actualFrameCoord, TRUE);
    MX_ERROR_CHECK(err);

    coordToDocCoord(err, actualFrameCoord.p, actualScrollPosition, MX_COORD_C);
    MX_ERROR_CHECK(err);

    return;
abort:
    return;
}

/*-------------------------------------------------
 * FUNCTION: mx_frame::resize
 *
 * DESCRIPTION: Scoll a
 * coordinates.
 *
 */

void mx_frame::resize(int& err,
    mx_resize_event& event)
{
    err = MX_ERROR_OK;

    // get the size of the device in frame coordinates
    isetSize(err, outputDevice->getFrameSize());
    MX_ERROR_CHECK(err);

    return;
abort:
    return;
}

/*-------------------------------------------------
 * FUNCTION: mx_frame::setFalseOrigin
 *
 * DESCRIPTION:
 *
 *
 */

void mx_frame::setFalseOrigin(int& err,
    const mx_doc_coord_t& ifalseOrigin)
{
    mx_point ipfalseOrigin;

    err = MX_ERROR_OK;

    docCoordToCoord(err, ifalseOrigin, ipfalseOrigin);
    MX_ERROR_CHECK(err);

    setFalseOrigin(ipfalseOrigin);

    return;

abort:
    return;
}

/*-------------------------------------------------
 * FUNCTION: mx_frame::refresh
 *
 * DESCRIPTION: Refresh the whole frame. Optionally
 * only refresh part of the frame (in document coordinates)
 *
 */
void mx_frame::refresh(int& err,
    mx_doc_coord_t* topLeft,
    mx_doc_coord_t* bottomRight)
{
    mx_point topLeftPoint;
    mx_point bottomRightPoint;
    mx_point* topLeftPointPtr = &topLeftPoint;
    mx_point* bottomRightPointPtr = &bottomRightPoint;

    if (topLeft == NULL) {
        topLeftPointPtr = NULL;
    } else {
        docCoordToCoord(err, *topLeft, topLeftPoint);
        MX_ERROR_CHECK(err);
    }

    if (bottomRight == NULL) {
        bottomRightPointPtr = NULL;
    } else {
        docCoordToCoord(err, *bottomRight, bottomRightPoint);
        MX_ERROR_CHECK(err);
    }

    outputDevice->refresh(err, topLeftPointPtr,
        bottomRightPointPtr);
    MX_ERROR_CHECK(err);

    return;
abort:
    return;
}

/*-------------------------------------------------
 * FUNCTION: mx_frame::getDocFalseOrigin
 *
 * DESCRIPTION: Refresh the whole frame
 *
 *
 */

mx_doc_coord_t mx_frame::getDocFalseOrigin(int& err)
{
    mx_doc_coord_t retCoord;

    err = MX_ERROR_OK;

    coordToDocCoord(err, falseOrigin, retCoord, MX_COORD_C);
    MX_ERROR_CHECK(err);

    return retCoord;
abort:
    return retCoord;
}
/*-------------------------------------------------
 * FUNCTION: mx_frame::makeGap
 *
 * DESCRIPTION: Make a gap for redraw into
 *
 */

void mx_frame::makeGap(int& err,
    bool doX,
    mx_doc_coord_t& docCoord,
    double docGapSize)
{
    mx_point gapStartPoint;

    err = MX_ERROR_OK;

    docCoordToCoord(err, docCoord, gapStartPoint);
    MX_ERROR_CHECK(err);

    outputDevice->makeGap(err, doX, gapStartPoint, docGapSize);
    MX_ERROR_CHECK(err);

    return;
abort:
    return;
}

/*-------------------------------------------------
 * FUNCTION: mx_frame::closeGap
 *
 * DESCRIPTION: Close a gap for redraw into
 *
 */

void mx_frame::closeGap(int& err,
    bool doX,
    mx_doc_coord_t& docCoord,
    double docGapSize)
{
    mx_point gapStartPoint;

    err = MX_ERROR_OK;

    docCoordToCoord(err, docCoord, gapStartPoint);
    MX_ERROR_CHECK(err);

    outputDevice->closeGap(err, doX, gapStartPoint, docGapSize);
    MX_ERROR_CHECK(err);

    return;
abort:
    return;
}

void mx_frame::requestSelection(int& err)
{
    err = MX_ERROR_OK;
}

void mx_frame::externalSelectionAvailable()
{
}
