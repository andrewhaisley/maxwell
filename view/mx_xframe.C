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
 * MODULE : mx_xframe.C
 *
 * AUTHOR : David Miller
 *
 * This file
 *
 * DESCRIPTION:
 * Module mx_xframe.C
 *
 *
 *
 *
 */

#include <Xm/Frame.h>
#include <Xm/ScrollBar.h>
#include <Xm/ScrolledW.h>
#include <mx_sc_device.h>
#include <mx_xframe.h>

/*-------------------------------------------------
 * FUNCTION: mx_xframe::defaultDeviceToFrame
 *
 * DESCRIPTION: Routine to convert from screen device coodinates
 * to x frame coordinates
 *
 */

void mx_xframe::defaultDeviceToFrame(int& err,
    mx_callback_target* source,
    mx_callback_target* dest,
    mx_doc_coord_t& coord,
    MX_COORD_TYPE coordType,
    bool& isInRange)
{
    mx_screen_device* screenDevice = (mx_screen_device*)source;

    coord = screenDevice->dev2frame(coord);
}

/*-------------------------------------------------
 * FUNCTION: mx_xframe::defaultFrameToFrameTarget
 *
 * DESCRIPTION: Routine to convert from screen device coodinates
 * to x frame coordinates
 *
 */

void mx_xframe::defaultFrameToFrameTarget(int& err,
    mx_callback_target* source,
    mx_callback_target* dest,
    mx_doc_coord_t& coord,
    MX_COORD_TYPE coordType,
    bool& isInRange)
{
    mx_xframe* frame = (mx_xframe*)source;
    mx_point pt = coord.p;

    isInRange = frame->coordToDocCoord(err, pt, coord, coordType);
    MX_ERROR_CHECK(err);

    return;
abort:
    return;
}

/*-------------------------------------------------
 * FUNCTION: mx_xframe::mx_xframe
 *
 * DESCRIPTION: Frame constructor for a simple
 * frame which can hold a device
 *
 */

mx_xframe::mx_xframe(int& err,
    mx_doc_coord_t& initialTopLeft,
    mx_point& initialSize,
    double initialZoom,
    mx_frame_target& iframeTarget,
    Widget iparentWidget)
    : mx_frame(err,
          initialTopLeft,
          initialSize,
          initialZoom,
          iframeTarget)
{
    // check error code
    MX_ERROR_CHECK(err);

    // set the parent and the active widget to be the
    // parent widget
    parentWidget = iparentWidget;
    activeWidget = iparentWidget;

    setProcessMask(MX_EVENT_STANDARD);

    // set the frame to transmit events to the frame target

    iframeTarget.setToReceiveEvents(err, this, defaultFrameToFrameTarget);
    MX_ERROR_CHECK(err);

    return;
abort:
    return;
}

/*-------------------------------------------------
 * FUNCTION: mx_xframe::draw
 *
 * DESCRIPTION: Frame constructor for a simple
 * frame which can hold a device
 *
 */

void mx_xframe::draw(int& err,
    mx_draw_event& event)
{
    mx_draw_event newEvent = event;

    // set the device on the output device
    newEvent.setDevice(getDevice());

    if (getDrawPerSheet()) {
        mx_doc_rect_t docRect;
        mx_doc_coord_t coord;
        mx_doc_coord_t tl, br;
        mx_point pt;

        // set up iteration
        getFrameTarget().getSheetLayout().startIterateOverArea(err,
            event.getTL().p,
            event.getBR().p);
        MX_ERROR_CHECK(err);

        // iterate over sheets
        while (getFrameTarget().getSheetLayout().nextOverArea(err, docRect)) {
            // convert top left of doc rect to a doc coord in frame coordinates
            coord.sheet_number = docRect.sheet_number;
            coord.p = docRect.r.topLeft();

            docCoordToCoord(err, coord, pt);
            MX_ERROR_CHECK(err);

            tl.sheet_number = 0;
            tl.p = pt;

            // convert bottom right  of doc rect to a doc coord in frame coordinates
            coord.p = docRect.r.bottomRight();

            docCoordToCoord(err, coord, pt);
            MX_ERROR_CHECK(err);

            br.sheet_number = 0;
            br.p = pt;

            // set the new event just for this sheet
            newEvent.setRect(tl, br);

            // propagate each event separately
            mx_callback_target::draw(err, newEvent);
            MX_ERROR_CHECK(err);
        }
        MX_ERROR_CHECK(err);
    } else {
        // propagate each event separately
        mx_callback_target::draw(err, newEvent);
        MX_ERROR_CHECK(err);
    }

    return;
abort:
    return;
}

/*-------------------------------------------------
 * FUNCTION: mx_xframe::createDevice
 *
 * DESCRIPTION: Frame constructor for a simple
 * frame which can hold a device
 *
 */

void mx_xframe::createDevice(int& err)
{
    outputDevice = new mx_screen_device(err, activeWidget, *this);
    MX_ERROR_CHECK(err);

    // set this frame to receive events from the device
    setToReceiveEvents(err, (mx_screen_device*)outputDevice, defaultDeviceToFrame);
    MX_ERROR_CHECK(err);

    // get the size of the device in frame coordinates
    isetSize(err, outputDevice->getFrameSize());
    MX_ERROR_CHECK(err);

    return;
abort:
    return;
}

/*-------------------------------------------------
 * FUNCTION: mx_xfframe::mx_xfframe
 *
 * DESCRIPTION: Frame constructor for a simple
 * frame which can hold a device in a Motif frame
 *
 */

mx_xfframe::mx_xfframe(int& err,
    mx_doc_coord_t& initialTopLeft,
    mx_point& initialSize,
    double initialZoom,
    mx_frame_target& iframeTarget,
    Widget iparentWidget)
    : mx_xframe(err,
          initialTopLeft,
          initialSize,
          initialZoom,
          iframeTarget,
          iparentWidget)
{
    MX_ERROR_CHECK(err);

    widget = activeWidget = XtVaCreateManagedWidget("frame", xmFrameWidgetClass,
        parentWidget, NULL);

    createDevice(err);
    MX_ERROR_CHECK(err);

    setProcessMask(MX_EVENT_STANDARD);

    return;
abort:
    return;
}

/*-------------------------------------------------
 * FUNCTION: mx_xframe::resetSheetLayout
 *
 * DESCRIPTION: Frame constructor for a simple
 * frame which can hold a device in a scrollable
 * window
 */

void mx_xframe::resetSheetLayout(int& err)
{
}

/*-------------------------------------------------
 * FUNCTION: mx_xframe::scroll
 *
 * DESCRIPTION: Scoll a window in frame target
 * coordinates .
 *
 */

void mx_xframe::scroll(int& err,
    mx_doc_coord_t& scrollPosition,
    MX_SCROLL_OFFSET offset,
    bool sameX,
    bool sameY,
    mx_doc_coord_t& actualScrollPosition)
{
    mx_frame::scroll(err, scrollPosition, offset, sameX, sameY,
        actualScrollPosition);
}

/*-------------------------------------------------
 * FUNCTION: mx_xframe::SetTopLeft
 *
 * DESCRIPTION:
 *
 *
 */

void mx_xframe::setTopLeft(int& err,
    mx_point& itopLeft)

{
    mx_scroll_event scrollEvent;
    mx_doc_coord_t mp;

    bool sameX = (itopLeft.x == getTopLeft().x);
    bool sameY = (itopLeft.y == getTopLeft().y);

    if (sameX && sameY)
        return;

    mp.sheet_number = 0;
    mp.p = itopLeft;

    scrollEvent.setUseScrollPosition(TRUE);
    scrollEvent.setScrollPosition(mp);
    scrollEvent.setSameX(sameX);
    scrollEvent.setSameY(sameY);
    scrollEvent.setOffset(MX_SCROLL_OFFSET_TL);

    forceScroll(err, scrollEvent);
    MX_ERROR_CHECK(err);

    return;
abort:
    return;
}

/*-------------------------------------------------
 * FUNCTION: mx_xframe::frameScroll
 *
 * DESCRIPTION:
 *
 *
 */

void mx_xframe::frameScroll(int& err,
    mx_doc_coord_t& scrollPosition,
    MX_SCROLL_OFFSET offset,
    bool sameX,
    bool sameY,
    mx_doc_coord_t& actualScrollPosition,
    bool doScroll)
{
    mx_frame::frameScroll(err, scrollPosition,
        offset, sameX, sameY, actualScrollPosition,
        doScroll);
}

/*-------------------------------------------------
 * FUNCTION: mx_xframe::resize
 *
 * DESCRIPTION:
 *
 *
 */

void mx_xframe::resize(int& err,
    mx_resize_event& event)
{
    mx_frame::resize(err, event);
}

/*-------------------------------------------------
 * FUNCTION: mx_xframe::setZoom
 *
 * DESCRIPTION:
 *
 *
 */

void mx_xframe::setZoom(int& err, double izoom)
{
    mx_frame::setZoom(err, izoom);
}

/*-------------------------------------------------
 * FUNCTION: mx_xframe::makeVisible
 *
 * DESCRIPTION: Make a point visible in the frame
 * if required using frame target coordinates
 *
 */

void mx_xframe::makeVisible(int& err,
    mx_doc_coord_t& visiblePos,
    double extra_offset)
{
    makeVisible(err, visiblePos, visiblePos, extra_offset);
    MX_ERROR_CHECK(err);
abort:;
}

/*-------------------------------------------------
 * FUNCTION: mx_xframe::makeVisible
 *
 * DESCRIPTION: Make an area visible in the frame if required using frame
 * target coordinates
 *
 */
void mx_xframe::makeVisible(int& err,
    mx_doc_coord_t& top_left,
    mx_doc_coord_t& bottom_right,
    double extra_offset)
{
    mx_doc_coord_t tl, br;

    docCoordToCoord(err, top_left, tl.p);
    MX_ERROR_CHECK(err);

    docCoordToCoord(err, bottom_right, br.p);
    MX_ERROR_CHECK(err);

    makeVisibleInternal(err, tl, br, extra_offset);
    MX_ERROR_CHECK(err);

abort:;
}

/*-------------------------------------------------
 * FUNCTION: mx_xframe::makeVisibleInternal
 *
 * DESCRIPTION: Make an area visible in the frame if required
 *
 */
void mx_xframe::makeVisibleInternal(int& err,
    mx_doc_coord_t& top_left,
    mx_doc_coord_t& bottom_right,
    double extra)
{
    int offset1 = 1;
    int offset2 = 1;
    bool sameX = TRUE;
    bool sameY = TRUE;
    mx_point tl(getTopLeft());
    mx_point sz(getSize());
    mx_point areaSize = bottom_right.p - top_left.p;
    mx_point extra_offset(0, 0);
    mx_doc_coord_t visiblePos;
    MX_SCROLL_OFFSET scrollOffset;

    // first work out whether the whole area plus the extra requested will fit in
    // the frame at its current size. If it does, then we just scroll as
    // usual. If not we get rid of the extra offset and see if it
    // fits. Otherwise, we just make sure the middle of the area is visible.

    if (areaSize.x + extra < sz.x) {
        extra_offset.x = extra;
    } else if (areaSize.x < sz.x) {
        extra_offset.x = 0.0;
    } else {
        extra_offset.x = -areaSize.x * 0.5;
    }

    if (areaSize.y + extra < sz.y) {
        extra_offset.y = extra;
    } else if (areaSize.y < sz.y) {
        extra_offset.y = 0.0;
    } else {
        extra_offset.y = -areaSize.y * 0.5;
    }

    if (top_left.p.y < tl.y) {
        offset2 = 0;
        sameY = FALSE;
        visiblePos.p.y = top_left.p.y - extra_offset.y;
    } else if (bottom_right.p.y > tl.y + sz.y) {
        offset2 = 2;
        sameY = FALSE;
        visiblePos.p.y = bottom_right.p.y + extra_offset.y;
    }

    if (top_left.p.x < tl.x) {
        offset1 = 0;
        sameX = FALSE;
        visiblePos.p.x = top_left.p.x - extra_offset.x;
    } else if (bottom_right.p.x > tl.x + sz.x) {
        offset1 = 2;
        sameX = FALSE;
        visiblePos.p.x = bottom_right.p.x + extra_offset.x;
    }

    scrollOffset = scrollOffsets[offset2][offset1];

    if (scrollOffset != MX_SCROLL_OFFSET_C) {
        mx_scroll_event scrollEvent;

        scrollEvent.setUseScrollPosition(TRUE);
        scrollEvent.setScrollPosition(visiblePos);
        scrollEvent.setSameX(sameX);
        scrollEvent.setSameY(sameY);
        scrollEvent.setOffset(scrollOffset);

        // scroll to the position

        forceScroll(err, scrollEvent);
        MX_ERROR_CHECK(err);
    }

    return;
abort:
    return;
}

/*-------------------------------------------------
 * FUNCTION: mx_xframe::buttonMotion
 *
 * DESCRIPTION:
 *
 *
 */

void mx_xframe::buttonMotion(int& err,
    mx_button_event& event)
{
    if (getMouseScrolling()) {
        mx_doc_coord_t mp = event.getCoord();

        makeVisibleInternal(err, mp, mp, 0.0);
        MX_ERROR_CHECK(err);
    }

    // propagate event
    mx_callback_target::buttonMotion(err, event);
    MX_ERROR_CHECK(err);

    return;
abort:
    return;
}

/*-------------------------------------------------
 * FUNCTION: mx_xframe::scrollEventtoTL
 *
 * DESCRIPTION: Scoll a window in frame target
 * coordinates .
 *
 */

void mx_xframe::scrollEventToTL(mx_scroll_event& scrollEvent,
    mx_doc_coord_t& scrollPosition,
    bool& sameX,
    bool& sameY,
    MX_SCROLL_OFFSET& offset)
{

    if (scrollEvent.getUseScrollPosition()) {
        sameX = scrollEvent.getSameX();
        sameY = scrollEvent.getSameY();
        scrollPosition = scrollEvent.getScrollPosition();
        offset = scrollEvent.getOffset();
    } else {
        scrollPosition.sheet_number = 0;
        scrollPosition.p = getTopLeft();
        offset = MX_SCROLL_OFFSET_TL;

        sameX = FALSE;
        sameY = FALSE;

        if (scrollEvent.getIsVertical()) {
            sameX = TRUE;
            scrollPosition.p.y = scrollEvent.getTLvalue() / 100;
        } else {
            sameY = TRUE;
            scrollPosition.p.x = scrollEvent.getTLvalue() / 100;
        }
    }

    return;
}

/*-------------------------------------------------
 * FUNCTION: mx_xframe::preScroll
 *
 * DESCRIPTION: Scroll a window in frame target
 * coordinates .
 *
 */

void mx_xframe::preScroll(int& err,
    mx_scroll_event& scrollEvent)
{
    mx_doc_coord_t scrollPosition;
    mx_doc_coord_t actualScrollPosition;
    bool sameX, sameY;
    MX_SCROLL_OFFSET offset;

    scrollEventToTL(scrollEvent, scrollPosition, sameX, sameY, offset);

    frameScroll(err, scrollPosition, offset, sameX, sameY,
        actualScrollPosition, FALSE);
    MX_ERROR_CHECK(err);

    scrollPosition.p = getTopLeft();

    scrollEvent.setOldTopLeft(scrollPosition);
    scrollEvent.setNewTopLeft(actualScrollPosition);

    // propagate event
    mx_callback_target::preScroll(err, scrollEvent);
    MX_ERROR_CHECK(err);

abort:;
}

/*-------------------------------------------------
 * FUNCTION: mx_xframe::scroll
 *
 * DESCRIPTION: Scoll a window in frame
 * coordinates.
 *
 */

void mx_xframe::scroll(int& err,
    mx_scroll_event& scrollEvent)
{
    mx_doc_coord_t scrollPosition;
    mx_doc_coord_t actualScrollPosition;
    bool sameX, sameY;
    MX_SCROLL_OFFSET offset;

    scrollEventToTL(scrollEvent, scrollPosition, sameX, sameY, offset);

    frameScroll(err, scrollPosition, offset, sameX, sameY,
        actualScrollPosition, TRUE);
    MX_ERROR_CHECK(err);

    // propagate event
    mx_callback_target::scroll(err, scrollEvent);
    MX_ERROR_CHECK(err);

    return;
abort:
    return;
}
