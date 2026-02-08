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
 * MODULE : mx_sc_cb.C
 *
 * AUTHOR : David Miller
 *
 * This file
 *
 * DESCRIPTION: Callback routines on the screen device
 * Module mx_sc_cb.C
 *
 *
 *
 *
 */

#include "mx_sc_device.h"

/*-------------------------------------------------
 * FUNCTION: mx_screen_device::expose
 *
 * DESCRIPTION:
 *
 *
 */

void mx_screen_device::expose(int& err,
    mx_expose_event& event)
{
    mx_ipoint topLeft = pixelSnap(event.getTL());
    mx_ipoint bottomRight = pixelSnap(event.getBR());

    GC temp_gc;

    err = MX_ERROR_OK;

    if (storePixmap == XtUnspecifiedPixmap) {
        mx_resize_event resizeEvent;
        mx_doc_coord_t resizeSize;
        XGCValues xgcv;

        // this is the first expose event for the whole window
        // get the size of the window

        setWindowSize();

        // generate a resize event - we dont get one from Motif ! */
        resizeEvent.setTL(resizeSize);
        resizeSize.p = windowSize;
        resizeEvent.setBR(resizeSize);
        resize(err, resizeEvent);
        MX_ERROR_CHECK(err);

        // initialise X defaults
        window = XtWindow(widget);

        // get the default GC
        temp_gc = DefaultGC(display, DefaultScreen(display));

        // create a new one
        gc = XCreateGC(display, window, 0, NULL);

        // copy the default one into it
        XCopyGC(display, temp_gc, 0xFFFFFFFF, gc);

        // set the graphics exposure flag off - dont
        // want these events

        xgcv.graphics_exposures = FALSE;

        // reset GC
        XChangeGC(display, gc, GCGraphicsExposures, &xgcv);

        // Make the backing store pixmap, so that we can cope with expose events
        storePixmap = initialisePixmap();

        // now draw the whole window - but wait for expose events

        redraw(err, MX_DEVICE_GRAVITY_UNDEFINED, windowSize, windowSize, FALSE, FALSE);
        MX_ERROR_CHECK(err);
    }

    // everything should be in the pixmap

    copyPixmapToWindow(topLeft, bottomRight);

    return;
abort:
    return;
}

/*-------------------------------------------------
 * FUNCTION: mx_screen_device::draw
 *
 * DESCRIPTION:
 *
 *
 */

void mx_screen_device::draw(int& err,
    mx_draw_event& drawEvent)
{
    mx_draw_event testEvent = drawEvent;
    mx_doc_coord_t tl = drawEvent.getTL();
    mx_doc_coord_t br = drawEvent.getBR();

    err = MX_ERROR_OK;

    if (!inDrawEvent) {
        // start a drawing event if not already in one

        processPreDraw(err, drawEvent);
        MX_ERROR_CHECK(err);

        inDrawEvent = TRUE;
    }

    // do the drawing

    // we know what we are drawing
    setMonitor(FALSE);

    if (applyClipMask) {
        mx_ipoint tli(pixelSnap(tl));
        mx_ipoint bri(pixelSnap(br));
        mx_irect clipRect(tli, bri);

        pushClipRect(err, clipRect, defaultAngle, TRUE);
        MX_ERROR_CHECK(err);
    }

    testEvent = drawEvent;

    if (pixelAdd != 0) {
        mx_doc_coord_t pt;

        pt = drawEvent.getTL();
        pt.p.x -= pixelAdd;
        pt.p.y -= pixelAdd;
        testEvent.setTL(pt);

        pt = drawEvent.getBR();
        pt.p.x += pixelAdd;
        pt.p.y += pixelAdd;
        testEvent.setBR(pt);
    }

    processDraw(err, testEvent);
    MX_ERROR_CHECK(err);

    processSpecialDraw(err, testEvent);
    MX_ERROR_CHECK(err);

    if (applyClipMask) {
        popClipMask(err);
        MX_ERROR_CHECK(err);
    }

    setMonitor(TRUE);

    // finished drawing a selection
    if (testEvent.getCount() == 0) {
        // get out of the draw even
        processPostDraw(err, drawEvent);
        MX_ERROR_CHECK(err);

        inDrawEvent = FALSE;
    }
    return;
abort:
    return;
}

/*-------------------------------------------------
 * FUNCTION: mx_screen_device::resize
 *
 * DESCRIPTION:
 *
 *
 */

void mx_screen_device::resize(int& err,
    mx_resize_event& event)
{
    mx_ipoint oldWindowSize;

    err = MX_ERROR_OK;

    oldWindowSize = windowSize;

    // get the new window size from the event
    windowSize = event.getBR().p;

    // got a resize event

    if (storePixmap != XtUnspecifiedPixmap) {
        Pixmap newPixmap;

        // havent had an expose event and something will
        // be in the pixmap

        // copy the old pixmap to a new pixmap

        //  Make the backing store pixmap
        newPixmap = initialisePixmap();

        XCopyArea(display, storePixmap, newPixmap, gc,
            0, 0, oldWindowSize.x, oldWindowSize.y, 0, 0);

        XFreePixmap(display, storePixmap);

        storePixmap = newPixmap;

        // wait for expose events
        redraw(err, MX_DEVICE_GRAVITY_TL, oldWindowSize, windowSize, FALSE, FALSE);
        MX_ERROR_CHECK(err);
    }

    // propagate event
    mx_callback_target::resize(err, event);
    MX_ERROR_CHECK(err);

    return;
abort:
    return;
}

/*-------------------------------------------------
 * FUNCTION: mx_screen_device::redraw
 *
 * DESCRIPTION:
 *
 *
 */

void mx_screen_device::redraw(int& err,
    MX_DEVICE_GRAVITY deviceGravity,
    const mx_point& oldSize,
    const mx_point& newSize,
    bool drawToWindow,
    bool doBlatPixmap,
    mx_point* topLeft,
    mx_point* bottomRight)
{
    mx_draw_event drawEvent;
    int count;
    mx_doc_coord_t tl[2];
    mx_doc_coord_t br[2];
    int icount;

    if (deviceGravity == MX_DEVICE_GRAVITY_UNDEFINED) {
        bool sapplyMask;
        mx_doc_coord_t drawCoordTL;
        mx_doc_coord_t drawCoordBR;
        bool applyMask = FALSE;

        // redraw the whole window
        // set the top left
        drawCoordTL.sheet_number = 0;

        if (topLeft == NULL) {

            drawCoordTL.p.x = 0;
            drawCoordTL.p.y = 0;
        } else {

            drawCoordTL.p.x = GMAX(topLeft->x, 0);
            drawCoordTL.p.y = GMAX(topLeft->y, 0);
            applyMask = TRUE;
        }

        drawEvent.setTL(drawCoordTL);

        drawCoordTL.sheet_number = 0;

        if (bottomRight == NULL) {
            drawCoordBR.p.x = newSize.x - 1;
            drawCoordBR.p.y = newSize.y - 1;
        } else {
            drawCoordBR.p.x = GMIN(bottomRight->x, newSize.x - 1);
            drawCoordBR.p.y = GMIN(bottomRight->y, newSize.y - 1);
            applyMask = TRUE;
        }

        drawEvent.setBR(drawCoordBR);
        drawEvent.setCount(1);

        // Redraw everything - no need for a clip mask
        sapplyMask = getApplyClipMask();

        setApplyClipMask(applyMask);

        if (doBlatPixmap) {
            blatPixmap(storePixmap, drawCoordTL.p, drawCoordBR.p);
        }

        draw(err, drawEvent);
        MX_ERROR_CHECK(err);

        setApplyClipMask(sapplyMask);
        MX_ERROR_CHECK(err);

        if (drawToWindow) {
            copyPixmapToWindow(drawCoordTL.p, drawCoordBR.p);
        }

        return;
    }

    count = 0;

    // always draw from top to bottom
    switch (deviceGravity) {
    case MX_DEVICE_GRAVITY_TL:

        if (oldSize.x < newSize.x) {
            tl[0].p.x = oldSize.x;
            tl[0].p.y = 0;
            br[0].p.x = newSize.x - 1;
            br[0].p.y = GMIN(oldSize.y, newSize.y) - 1;
            count++;
        }

        if (oldSize.y < newSize.y) {
            // set the top left
            tl[count].p.x = 0;
            tl[count].p.y = oldSize.y;

            br[count].p.x = newSize.x - 1;
            br[count].p.y = newSize.y - 1;

            count++;
        }

        break;

    case MX_DEVICE_GRAVITY_TR:

        if (oldSize.x < newSize.x) {
            tl[0].p.x = 0;
            tl[0].p.y = 0;
            br[0].p.x = newSize.x - oldSize.x;
            br[0].p.y = GMIN(oldSize.y, newSize.y) - 1;
            count++;
        }

        if (oldSize.y < newSize.y) {
            // set the top left
            tl[count].p.x = 0;
            tl[count].p.y = oldSize.y;

            br[count].p.x = newSize.x - 1;
            br[count].p.y = newSize.y - 1;

            count++;
        }

        break;

    case MX_DEVICE_GRAVITY_BL:

        if (oldSize.y < newSize.y) {
            tl[0].p.x = 0;
            tl[0].p.y = 0;
            br[0].p.x = newSize.x - 1;
            br[0].p.y = newSize.y - oldSize.y;
            count++;
        }

        if (oldSize.x < newSize.x) {
            tl[count].p.x = oldSize.x;
            tl[count].p.y = GMAX(0, newSize.y - oldSize.y);
            br[count].p.x = newSize.x - 1;
            br[count].p.y = newSize.y - 1;
            count++;
        }

        break;

    case MX_DEVICE_GRAVITY_BR:

        if (oldSize.y < newSize.y) {
            tl[0].p.x = 0;
            tl[0].p.y = 0;
            br[0].p.x = newSize.x - 1;
            br[0].p.y = newSize.y - oldSize.y;
            count++;
        }

        if (oldSize.x < newSize.x) {
            tl[count].p.x = 0;
            tl[count].p.y = GMAX(0, newSize.y - oldSize.y);
            br[count].p.x = newSize.x - oldSize.x;
            br[count].p.y = newSize.y;
            count++;
        }

        break;
    default:
        break;
    }

    // lets generate some draw events on the underlyings which
    // will be exposed by a draw

    for (icount = 0; icount < count; icount++) {
        tl[icount].sheet_number = 0;
        br[icount].sheet_number = 0;

        drawEvent.setTL(tl[icount]);
        drawEvent.setBR(br[icount]);
        drawEvent.setCount(count);

        if (doBlatPixmap) {
            blatPixmap(storePixmap, tl[icount].p, br[icount].p);
        }

        draw(err, drawEvent);
        MX_ERROR_CHECK(err);

        if (drawToWindow) {
            copyPixmapToWindow(tl[icount].p, br[icount].p);
        }
    }

    return;
abort:
    return;
}
