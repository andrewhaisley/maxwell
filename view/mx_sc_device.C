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
 * MODULE : mx_sc_device.C
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

#include <Xm/DrawingA.h>
#include <maxwell.h>
#include <mx_sc_device.h>

// static array of xpoints needed for drawing

int mx_screen_device::nxpoints = 0;
XPoint* mx_screen_device::xpoints = NULL;
int mx_screen_device::nxsegments = 0;
XSegment* mx_screen_device::xsegments = NULL;

/*-------------------------------------------------
 * FUNCTION: mx_screen_device::mx_screen_device
 *
 * DESCRIPTION:
 *
 *
 */

mx_screen_device::mx_screen_device(int& err, Widget parentWidget, mx_frame& frame)
    : mx_device(frame)
    , monitorRect(1000000, 1000000, -1, -1)
{
    m_xft_font = nullptr;
    int screenNumber;
    double iscreenResolution;

    static char transString[] = "<Key>Tab: DrawingAreaInput()";
    XtTranslations translations;

    // set the maxwell events to be the default for a screen device

    setProcessMask(MX_EVENT_XEVENTS);

    // Create a drawing area widget - we will use its X window to draw into
    widget = XtVaCreateManagedWidget("screenDevice",
        xmDrawingAreaWidgetClass, parentWidget,
        NULL);

    translations = XtParseTranslationTable(transString);
    XtOverrideTranslations(widget, translations);

    // set the callback widget
    setCBwidget(widget);

    // get the screen resolution
    screen = XtScreen(widget);
    iscreenResolution = ((double)screen->width) / ((double)screen->mwidth);
    setScreenResolution(iscreenResolution);

    // initialise store pixmap to a dummy
    storePixmap = XtUnspecifiedPixmap;

    // initialise other x parameters to dummies.
    // these get processed on the first xevent
    window = XtUnspecifiedWindow;
    display = NULL;
    gc = NULL;

    // get the initial window size
    setWindowSize();

    // get the basic colours for the widget
    setBasicColours();

    inDrawEvent = FALSE;

    // clip stacks
    clipStackSize = 4;
    clipStack = new Region[clipStackSize];
    clipStackOffset = 0;

    // maximum number of requests
    maxRequests = XMaxRequestSize(XtDisplay(widget));
    maxExtRequests = XExtendedMaxRequestSize(XtDisplay(widget));

    doMonitor = FALSE;
    drawToWindowOnly = FALSE;
    applyClipMask = TRUE;
    pixelAdd = 2;
    extendWidth = 2;

    nallocDashLengths = 0;
    dashLengths = NULL;
    cdashLengths = NULL;

    // get the display
    display = XtDisplay(widget);
    screenNumber = DefaultScreen(display);

    // set some default values for the foreground and background
    foreground = BlackPixel(display, screenNumber);
    background = WhitePixel(display, screenNumber);

    // and default colours for XFT text
    XRenderColor xr_black = {0x0000, 0x0000, 0x0000, 0xffff};
    XftColorAllocValue(display, DefaultVisual(display, DefaultScreen(display)), DefaultColormap(display, DefaultScreen(display)), &xr_black, &m_xft_foreground_colour);

    XRenderColor xr_white = {0xffff, 0xffff, 0xffff, 0xffff};
    XftColorAllocValue(display, DefaultVisual(display, DefaultScreen(display)), DefaultColormap(display, DefaultScreen(display)), &xr_white, &m_xft_background_colour);

    fontScale = 0.0;
    arcFillType = ArcChord - 1000;

    caretVisible = caretOn = FALSE;

    caretTimer = XtAppAddTimeOut(
        global_app_context,
        500L,
        mx_screen_device::caretXtCallback,
        this);

    fillsSector = TRUE;

    return;
}

/*-------------------------------------------------
 * FUNCTION: mx_screen_device::~mx_screen_device
 *
 * DESCRIPTION:
 *
 *
 */

mx_screen_device::~mx_screen_device()
{
    caretOn = FALSE;
    XtRemoveTimeOut(caretTimer);

    if (storePixmap != XtUnspecifiedPixmap)
        XFreePixmap(display, storePixmap);

    delete[] clipStack;
    if (dashLengths != NULL)
        delete[] dashLengths;
    if (cdashLengths != NULL)
        delete[] cdashLengths;

    if (gc != NULL)
        XFreeGC(display, gc);
}

/*-------------------------------------------------
 * FUNCTION: mx_screen_device::setWindowSize
 *
 * DESCRIPTION:
 *
 *
 */

void mx_screen_device::setWindowSize()
{
    Dimension width, height;

    Arg args[2];

    XtSetArg(args[0], XmNwidth, &width);
    XtSetArg(args[1], XmNheight, &height);
    XtGetValues(widget, args, 2);

    windowSize.x = width;
    windowSize.y = height;

    return;
}

/*-------------------------------------------------
 * FUNCTION: mx_screen_device::setBasicColours
 *
 * DESCRIPTION:
 *
 *
 */

void mx_screen_device::setBasicColours()
{
    Arg args[2];

    XtSetArg(args[0], XmNforeground, &foreground);
    XtSetArg(args[1], XmNbackground, &background);
    XtGetValues(widget, args, 2);
}

/*-------------------------------------------------
 * FUNCTION: mx_screen_device::blatPixmap
 *
 * DESCRIPTION:
 *
 *
 */

void mx_screen_device::blatPixmap(Pixmap ipixmap,
    const mx_ipoint& tl,
    const mx_ipoint& br)
{
    XSetForeground(display, gc, background);

    XFillRectangle(display, ipixmap, gc, tl.x, tl.y, br.x - tl.x + 1, br.y - tl.y + 1);

    XSetForeground(display, gc, foreground);

    return;
}

/*-------------------------------------------------
 * FUNCTION: mx_screen_device::initialisePixmap
 *
 * DESCRIPTION:
 *
 *
 */

Pixmap mx_screen_device::initialisePixmap()
{
    Pixmap newPixmap;
    mx_ipoint zp(0, 0);

    //  Make the backing store pixmap
    newPixmap = XCreatePixmap(display, window, windowSize.x, windowSize.y,
        DefaultDepthOfScreen(screen));

    // clear the pixmap to background

    blatPixmap(newPixmap, zp, windowSize);

    return newPixmap;
}

/*-------------------------------------------------
 * FUNCTION: mx_screen_device::setXPoints
 *
 * DESCRIPTION: Create an array of xpoints
 *
 *
 */

void mx_screen_device::setXPoints(int& err,
    int npoints)
{
    if (npoints <= nxpoints)
        return;

    nxpoints = (npoints + 511) / 512;
    nxpoints *= 512;

    if (xpoints != NULL)
        delete[] xpoints;

    xpoints = new XPoint[nxpoints];
    return;
}

/*-------------------------------------------------
 * FUNCTION: mx_screen_device::setXSegments
 *
 * DESCRIPTION: Create an array of xpoints
 *
 *
 */

void mx_screen_device::setXSegments(int& err,
    int nsegments)
{
    if (nsegments <= nxsegments)
        return;

    nxsegments = (nsegments + 255) / 256;
    nxsegments *= 256;

    if (xsegments != NULL)
        delete[] xsegments;

    xsegments = new XSegment[nxsegments];
    return;
}

/*-------------------------------------------------
 * FUNCTION: mx_screen_device:pixmapCommit
 *
 * DESCRIPTION: Create an array of xpoints
 *
 *
 */

void mx_screen_device::pixmapCommit()
{
    if (!doMonitor)
        return;

    int width = monitorRect.xt - monitorRect.xb + 1;
    int height = monitorRect.yt - monitorRect.yb + 1;

    if ((width < 0) || (height < 0))
        return;

    XCopyArea(display, storePixmap, window, gc,
        monitorRect.xb,
        monitorRect.yb,
        width,
        height,
        monitorRect.xb,
        monitorRect.yb);

    monitorRect.xb = 1000000;
    monitorRect.yb = 1000000;
    monitorRect.xt = -1;
    monitorRect.yt = -1;
}

/*-------------------------------------------------
 * FUNCTION: mx_screen_device:copyPixmapToWindow
 *
 * DESCRIPTION: Create an array of xpoints
 *
 *
 */

void mx_screen_device::copyPixmapToWindow(const mx_ipoint& topLeft,
    const mx_ipoint& bottomRight)
{
    int width = bottomRight.x - topLeft.x + 1;
    int height = bottomRight.y - topLeft.y + 1;

    if (caretVisible) {
        invertCaret();
    }

    XCopyArea(display, storePixmap, window, gc,
        topLeft.x, topLeft.y, width, height, topLeft.x, topLeft.y);
}

/*-------------------------------------------------
 * FUNCTION: mx_screen_device:setShiftOrigin
 *
 * DESCRIPTION: Shift the contents of a window so
 * that the background pixmap is used where possible
 * and the new areas are redrawn.
 *
 * This is typically used by scrolling to save any
 * area which can be redrawn any then to redraw the
 * new areas.
 */

void mx_screen_device::setShiftOrigin(mx_doc_coord_t& wantedShiftOrigin,
    mx_doc_coord_t& gotShiftOrigin,
    bool doScroll)
{
    gotShiftOrigin = pixelSnapFrame(wantedShiftOrigin);

    if (doScroll) {
        shiftOrigin = pixelSnap(frame2dev(gotShiftOrigin));
    }
}

/*-------------------------------------------------
 * FUNCTION: mx_screen_device::closeGap
 *
 * DESCRIPTION: Close a gap in the device in a direction,
 * from a point. The gapSize must have either a zero
 * x or a zero y. The non-zero value may be negative
 * depending on the direction in which the gap is to
 * be closed. The startPoint pixel is the 'first' pixel
 * to disappear
 */

void mx_screen_device::closeGap(int& err,
    bool doX,
    mx_point& frameStartPoint,
    double frameGapSize)
{
    mx_ipoint copyFrom;
    mx_ipoint copyTo;
    mx_ipoint copySize;
    mx_point redrawTL;
    mx_point redrawBR;
    int gapSize;
    mx_doc_coord_t devPoint;
    mx_point frameEndPoint;
    mx_ipoint startPoint;
    mx_ipoint endPoint;
    bool doCopy = FALSE;
    int endGap;

    devPoint = frame2dev(frameStartPoint);
    startPoint = pixelSnap(devPoint);

    frameEndPoint = frameStartPoint;
    if (doX) {
        frameEndPoint.x += frameGapSize;
    } else {
        frameEndPoint.y += frameGapSize;
    }

    devPoint = frame2dev(frameEndPoint);
    endPoint = pixelSnap(devPoint);

    if (doX) {
        gapSize = endPoint.x - startPoint.x;
    } else {
        gapSize = endPoint.y - startPoint.y;
    }

    if (gapSize == 0)
        return;

    if (doX) {
        copySize.y = windowSize.y;
        copyFrom.y = 0;
        copyTo.y = 0;
        redrawTL.y = 0;
        redrawBR.y = windowSize.y - 1;

        endGap = startPoint.x + gapSize;

        if (gapSize > 0) {
            /* test if any change in the area */
            if (startPoint.x >= windowSize.x)
                return;

            /* there is a shift of some size - set the
               shift point to 0  */
            if (startPoint.x < 0) {
                endGap -= startPoint.x;
                startPoint.x = 0;
            }

            copyFrom.x = endGap;
            copyTo.x = startPoint.x;
            copySize.x = windowSize.x - endGap;
            redrawTL.x = GMAX(0, windowSize.x - gapSize);
            redrawBR.x = windowSize.x - 1;
        } else {
            // test if any change in the area */
            if (startPoint.x < 0)
                return;

            /* there is a shift of some size - set the
               shift point to the window edge  */
            if (startPoint.x >= windowSize.x) {
                endGap += (windowSize.x - 1 - startPoint.x);
                startPoint.x = windowSize.x - 1;
            }

            copyFrom.x = 0;
            copyTo.x = -gapSize;
            copySize.x = endGap + 1;
            redrawTL.x = 0;
            redrawBR.x = GMIN(windowSize.x, -gapSize) - 1;
        }
        doCopy = (copySize.x > 0);
    } else {
        copySize.x = windowSize.x;
        copyFrom.x = 0;
        copyTo.x = 0;
        redrawTL.x = 0;
        redrawBR.x = windowSize.x - 1;

        endGap = startPoint.y + gapSize;

        if (gapSize > 0) {
            /* test if any change in the area */
            if (startPoint.y >= windowSize.y)
                return;

            /* there is a shift of some size - set the
               shift point to 0  */
            if (startPoint.y < 0) {
                endGap -= startPoint.y;
                startPoint.y = 0;
            }

            copyFrom.y = endGap;
            copyTo.y = startPoint.y;
            copySize.y = windowSize.y - endGap;
            redrawTL.y = GMAX(0, windowSize.y - gapSize);
            redrawBR.y = windowSize.y - 1;
        } else {
            // test if any change in the area */
            if (startPoint.y < 0)
                return;

            /* there is a shift of some size - set the
               shift point to the window edge  */
            if (startPoint.y >= windowSize.y) {
                endGap += (windowSize.y - 1 - startPoint.y);
                startPoint.y = windowSize.y - 1;
            }

            copyFrom.y = 0;
            copyTo.y = -gapSize;
            copySize.y = endGap + 1;
            redrawTL.y = 0;
            redrawBR.y = GMIN(windowSize.y, -gapSize) - 1;
        }
        doCopy = (copySize.y > 0);
    }

    if (doCopy) {
        // blat the shifted area
        XCopyArea(display, storePixmap, window, gc,
            copyFrom.x, copyFrom.y,
            copySize.x, copySize.y,
            copyTo.x, copyTo.y);

        XCopyArea(display, storePixmap, storePixmap, gc,
            copyFrom.x, copyFrom.y,
            copySize.x, copySize.y,
            copyTo.x, copyTo.y);
    }

    // now redraw the exposed area
    redraw(err, MX_DEVICE_GRAVITY_UNDEFINED, copySize, windowSize, TRUE, TRUE,
        &redrawTL, &redrawBR);
    MX_ERROR_CHECK(err);

    return;
abort:
    return;
}

/*-------------------------------------------------
 * FUNCTION: mx_screen_device::makeGap
 *
 * DESCRIPTION: Make a gap in the device in a direction,
 * from a point. The gap size may be negative depending
 * on the direction in which the gap is to
 * be made. The startPoint pixel is the 'first' pixel
 * to be moved
 */

void mx_screen_device::makeGap(int& err,
    bool doX,
    mx_point& frameStartPoint,
    double frameGapSize)
{
    mx_ipoint copyFrom;
    mx_ipoint copyTo;
    mx_ipoint copySize;
    mx_point redrawTL;
    mx_point redrawBR;
    int gapSize;
    mx_doc_coord_t devPoint;
    mx_point frameEndPoint;
    mx_ipoint startPoint;
    mx_ipoint endPoint;
    bool doCopy = FALSE;
    int endGap;

    devPoint = frame2dev(frameStartPoint);
    startPoint = pixelSnap(devPoint);

    frameEndPoint = frameStartPoint;
    if (doX) {
        frameEndPoint.x += frameGapSize;
    } else {
        frameEndPoint.y += frameGapSize;
    }

    devPoint = frame2dev(frameEndPoint);
    endPoint = pixelSnap(devPoint);

    if (doX) {
        gapSize = endPoint.x - startPoint.x;
    } else {
        gapSize = endPoint.y - startPoint.y;
    }

    if (gapSize == 0)
        return;

    // shifting in x direction
    if (doX) {
        copySize.y = windowSize.y;
        copyFrom.y = 0;
        copyTo.y = 0;
        redrawTL.y = 0;
        redrawBR.y = windowSize.y - 1;

        endGap = startPoint.x + gapSize;

        if (gapSize > 0) {
            // test if any change in the area */
            if (startPoint.x >= windowSize.x)
                return;

            /* there is a shift of some size - set the
               shift point to 0  */
            if (startPoint.x < 0) {
                endGap -= startPoint.x;
                startPoint.x = 0;
            }

            copyFrom.x = startPoint.x;
            copyTo.x = endGap;
            copySize.x = windowSize.x - endGap;
            redrawTL.x = startPoint.x;
            redrawBR.x = GMIN(endGap, windowSize.x) - 1;
        } else {
            // test if any change in the area */
            if (startPoint.x < 0)
                return;

            /* there is a shift of some size - set the
               shift point to the window edge  */
            if (startPoint.x >= windowSize.x) {
                endGap += (windowSize.x - 1 - startPoint.x);
                startPoint.x = windowSize.x - 1;
            }

            copyFrom.x = -gapSize;
            copyTo.x = 0;
            copySize.x = endGap + 1;
            redrawTL.x = GMAX(0, endGap + 1);
            redrawBR.x = startPoint.x;
        }
        doCopy = (copySize.x > 0);
    } else {
        copySize.x = windowSize.x;
        copyFrom.x = 0;
        copyTo.x = 0;
        redrawTL.x = 0;
        redrawBR.x = windowSize.x - 1;

        endGap = startPoint.y + gapSize;

        if (gapSize > 0) {
            // test if any change in the area */
            if (startPoint.y >= windowSize.y)
                return;

            /* there is a shift of some size - set the
               shift point to 0  */
            if (startPoint.y < 0) {
                endGap -= startPoint.y;
                startPoint.y = 0;
            }

            copyFrom.y = startPoint.y;
            copyTo.y = endGap;
            copySize.y = windowSize.y - endGap;
            redrawTL.y = startPoint.y;
            redrawBR.y = GMIN(endGap, windowSize.y) - 1;
        } else {
            // test if any change in the area */
            if (startPoint.y < 0)
                return;

            /* there is a shift of some size - set the
               shift point to the window edge  */
            if (startPoint.y >= windowSize.y) {
                endGap += (windowSize.y - 1 - startPoint.y);
                startPoint.y = windowSize.y - 1;
            }

            copyFrom.y = -gapSize;
            copyTo.y = 0;
            copySize.y = endGap + 1;
            redrawTL.y = GMAX(0, endGap + 1);
            redrawBR.y = startPoint.y;
        }
        doCopy = (copySize.y > 0);
    }

    if (doCopy) {
        // blat the shifted area
        XCopyArea(display, storePixmap, window, gc,
            copyFrom.x, copyFrom.y,
            copySize.x, copySize.y,
            copyTo.x, copyTo.y);

        XCopyArea(display, storePixmap, storePixmap, gc,
            copyFrom.x, copyFrom.y,
            copySize.x, copySize.y,
            copyTo.x, copyTo.y);
    }

    // now redraw the exposed area
    redraw(err, MX_DEVICE_GRAVITY_UNDEFINED, copySize, windowSize, TRUE, TRUE,
        &redrawTL, &redrawBR);
    MX_ERROR_CHECK(err);

    return;
abort:
    return;
}

/*-------------------------------------------------
 * FUNCTION: mx_screen_device:shift
 *
 * DESCRIPTION: Shift the contents of a window so
 * that the background pixmap is used where possible
 * and the new areas are redrawn.
 *
 * This is typically used by scrolling to save any
 * area which can be redrawn any then to redraw the
 * new areas.
 */

void mx_screen_device::shift(int& err)
{
    mx_ipoint newBottomRight = shiftOrigin + windowSize;
    bool interSect;

    (newBottomRight.x)--;
    (newBottomRight.y)--;

    interSect = !((shiftOrigin.x >= windowSize.x) || (shiftOrigin.y >= windowSize.y) || (newBottomRight.x < 0) || (newBottomRight.y < 0));

    if (!interSect) {
        // redraw the whole window
        redraw(err, MX_DEVICE_GRAVITY_UNDEFINED, windowSize, windowSize,
            TRUE, TRUE);
        MX_ERROR_CHECK(err);
    } else {
        mx_ipoint copyFrom;
        mx_ipoint copyTo;
        mx_ipoint copySize;
        MX_DEVICE_GRAVITY gravity;

        // calculate the gravity

        if (shiftOrigin.x < 0) {
            if (shiftOrigin.y < 0) {
                gravity = MX_DEVICE_GRAVITY_BR;
                copyFrom.x = 0;
                copyFrom.y = 0;
                copyTo.x = -shiftOrigin.x;
                copyTo.y = -shiftOrigin.y;
                copySize.x = newBottomRight.x + 1;
                copySize.y = newBottomRight.y + 1;
            } else {
                gravity = MX_DEVICE_GRAVITY_TR;
                copyFrom.x = 0;
                copyFrom.y = shiftOrigin.y;
                copyTo.x = -shiftOrigin.x;
                copyTo.y = 0;
                copySize.x = newBottomRight.x + 1;
                copySize.y = windowSize.y - shiftOrigin.y;
            }
        } else {
            if (shiftOrigin.y < 0) {
                gravity = MX_DEVICE_GRAVITY_BL;
                copyFrom.x = shiftOrigin.x;
                copyFrom.y = 0;
                copyTo.x = 0;
                copyTo.y = -shiftOrigin.y;
                copySize.x = windowSize.x - shiftOrigin.x;
                copySize.y = newBottomRight.y + 1;
            } else {
                gravity = MX_DEVICE_GRAVITY_TL;
                copyFrom.x = shiftOrigin.x;
                copyFrom.y = shiftOrigin.y;
                copyTo.x = 0;
                copyTo.y = 0;
                copySize.x = windowSize.x - shiftOrigin.x;
                copySize.y = windowSize.y - shiftOrigin.y;
            }
        }

        // now copy the saved area and redraw

        if (copyFrom != copyTo) {
            XCopyArea(display, storePixmap, window, gc,
                copyFrom.x, copyFrom.y,
                copySize.x, copySize.y,
                copyTo.x, copyTo.y);

            XCopyArea(display, storePixmap, storePixmap, gc,
                copyFrom.x, copyFrom.y,
                copySize.x, copySize.y,
                copyTo.x, copyTo.y);
        }

        // now redraw the contents
        redraw(err, gravity, copySize, windowSize, TRUE, TRUE);
        MX_ERROR_CHECK(err);
    }

    return;
abort:
    return;
}

/*-------------------------------------------------
 * FUNCTION: mx_screen_device:refresh
 *
 * DESCRIPTION: Shift the contents of a window so
 * that the background pixmap is used where possible
 * and the new areas are redrawn.
 *
 * This is typically used by scrolling to save any
 * area which can be redrawn any then to redraw the
 * new areas.
 *
 * A top left and bottom right can be defined in
 * frame coordinates
 */

void mx_screen_device::refresh(int& err,
    mx_point* topLeft,
    mx_point* bottomRight)
{
    mx_doc_coord_t devTopLeft;
    mx_doc_coord_t devBottomRight;
    mx_point* devTopLeftPtr = &(devTopLeft.p);
    mx_point* devBottomRightPtr = &(devBottomRight.p);
    mx_doc_coord_t frameTopLeft;
    mx_doc_coord_t frameBottomRight;

    // convert top left coordinate to device coordinates
    if (topLeft == NULL) {
        devTopLeftPtr = NULL;
    } else {
        frameTopLeft.sheet_number = 0;
        frameTopLeft.p = *topLeft;

        devTopLeft = frame2dev(frameTopLeft);
    }

    // convert bottom right coordinate to device coordinates
    if (bottomRight == NULL) {
        devBottomRightPtr = NULL;
    } else {
        frameBottomRight.sheet_number = 0;
        frameBottomRight.p = *bottomRight;

        devBottomRight = frame2dev(frameBottomRight);
    }

    redraw(err, MX_DEVICE_GRAVITY_UNDEFINED,
        windowSize, windowSize, TRUE, TRUE,
        devTopLeftPtr, devBottomRightPtr);
    MX_ERROR_CHECK(err);

    return;
abort:
    return;
}

void mx_screen_device::caretXtCallback(XtPointer cd, XtIntervalId* id)
{
    mx_screen_device* dev = (mx_screen_device*)cd;
    dev->caretCallback();
    dev->caretTimer = XtAppAddTimeOut(global_app_context, 500L, mx_screen_device::caretXtCallback, dev);
}

void mx_screen_device::caretCallback()
{
    if (caretOn) {
        invertCaret();
    }
}

void mx_screen_device::switchCaretOff()
{
    if (caretOn) {
        if (caretVisible) {
            invertCaret();
        }
        caretOn = FALSE;
    }
}

void mx_screen_device::switchCaretOn()
{
    if (!caretOn) {
        caretVisible = FALSE;
        caretOn = TRUE;
    }
}

void mx_screen_device::caretPosition(int& err, mx_doc_coord_t& pos, float height)
{
    if (caretOn && caretVisible) {
        invertCaret();
    }

    caretBottom = pos;
    caretTop.sheet_number = caretBottom.sheet_number;
    caretTop.p.x = caretBottom.p.x;
    caretTop.p.y = caretBottom.p.y - height;

    if (caretOn) {
        invertCaret();
    }
}

extern Widget global_top_level;

void mx_screen_device::unloadFontsAndColours()
{
    for (auto i : m_colour_dict) {
        delete i.second;
    }
}

void mx_screen_device::polypointToXPoints(int& err,
    const mx_ipolypoint& pp)
{
    int ipoint, npoints;

    err = MX_ERROR_OK;

    npoints = pp.get_num_points();

    // get memory for points
    setXPoints(err, npoints);
    MX_ERROR_CHECK(err);

    for (ipoint = 0; ipoint < npoints; ipoint++) {
        xpoints[ipoint].x = pp[ipoint].x;
        xpoints[ipoint].y = pp[ipoint].y;
    }

    return;
abort:;
}

int mx_screen_device::polypointToXSegments(int& err,
    const mx_ipolypoint& pp,
    bool linesCoincident)
{
    int isegment, nsegments;
    int cjump;
    int outPoint;
    int extraSegment = 0;

    err = MX_ERROR_OK;

    if (linesCoincident) {
        nsegments = pp.get_num_points() - 1;
        if (nsegments == 0)
            extraSegment = 1;
    } else {
        nsegments = pp.get_num_points() / 2;
        if ((pp.get_num_points() % 2) == 1)
            extraSegment = 1;
    }

    // get memory for points
    setXSegments(err, nsegments + extraSegment);
    MX_ERROR_CHECK(err);

    cjump = (linesCoincident) ? 0 : 1;
    outPoint = 0;

    for (isegment = 0; isegment < nsegments; isegment++) {
        xsegments[isegment].x1 = pp[outPoint].x;
        xsegments[isegment].y1 = pp[outPoint].y;
        outPoint++;
        xsegments[isegment].x2 = pp[outPoint].x;
        xsegments[isegment].y2 = pp[outPoint].y;
        outPoint += cjump;
    }

    if (extraSegment) {
        mx_ipoint lastPoint = pp[pp.get_num_points() - 1];
        xsegments[nsegments].x1 = lastPoint.x;
        xsegments[nsegments].y1 = lastPoint.y;
        xsegments[nsegments].x2 = lastPoint.x;
        xsegments[nsegments].y2 = lastPoint.y;
    }
abort:;
    return nsegments + extraSegment;
}

int mx_screen_device::depth()
{
    return DefaultDepth(display, DefaultScreen(display));
}
