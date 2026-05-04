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
 * MODULE : mx_sc_draw.C
 *
 * AUTHOR : David Miller
 *
 * This file
 *
 * DESCRIPTION: The drawing routines for a screen device
 * Module mx_sc_draw.C
 *
 *
 *
 *
 */


#include <stdio.h>

#include <mx_device.h>
#include <mx_sc_device.h>
#include <mx_frame.h>

inline static uint32 get_xor_colour_value(Screen* s)
{
    static Screen* screen = NULL;
    static uint32 colour_value;
    uint32 white_colour_value, black_colour_value;

    if (screen != s) {
        screen = s;
        white_colour_value = WhitePixelOfScreen(s);
        black_colour_value = BlackPixelOfScreen(s);
        colour_value = white_colour_value ^ black_colour_value;
    }
    return colour_value;
}

int mx_screen_device::get_xft_width(const char *s, int len)
{
    XGlyphInfo ext;
    XftTextExtentsUtf8(display, m_xft_font, (const FcChar8*)s, len, &ext);
    return ext.width;
}


/*-------------------------------------------------
 * FUNCTION: mx_screen_device::monitorArcSize
 *
 * DESCRIPTION: Draw a line  in pixel coordinates in the
 * current style
 *
 */

void mx_screen_device::monitorArcSize(const mx_ipoint& size,
    const mx_ipoint& centre)
{
    mx_irect trect;

    trect.xt = centre.x - size.x;
    trect.xb = centre.x + size.x;
    trect.yt = centre.y - size.y;
    trect.yb = centre.y + size.y;

    trect.normalise();

    if (monitorRect.xb > trect.xb - extendWidth)
        monitorRect.xb = trect.xb - extendWidth;
    if (monitorRect.xb > trect.yb - extendWidth)
        monitorRect.yb = trect.yb - extendWidth;

    if (monitorRect.xt < trect.xt + extendWidth)
        monitorRect.xt = trect.xt + extendWidth;
    if (monitorRect.yt < trect.yt + extendWidth)
        monitorRect.yt = trect.yt + extendWidth;
}

/*-------------------------------------------------
 * FUNCTION: mx_screen_device::monitorPolypointSize
 *
 * DESCRIPTION: Draw a line  in pixel coordinates in the
 * current style
 *
 */

void mx_screen_device::monitorPolypointSize(const mx_ipolypoint& pp)
{
    mx_irect trect = pp.bounding_box();

    trect.normalise();

    if (monitorRect.xb > trect.xb - extendWidth)
        monitorRect.xb = trect.xb - extendWidth;
    if (monitorRect.xb > trect.yb - extendWidth)
        monitorRect.yb = trect.yb - extendWidth;

    if (monitorRect.xt < trect.xt + extendWidth)
        monitorRect.xt = trect.xt + extendWidth;
    if (monitorRect.yt < trect.yt + extendWidth)
        monitorRect.yt = trect.yt + extendWidth;
}

/*-------------------------------------------------
 * FUNCTION: mx_screen_device::pfillRect
 *
 * DESCRIPTION: Fill a rectangle in pixel coordinates
 *
 *
 */

void mx_screen_device::pfillRect(int& err,
    const mx_ipoint& topLeft,
    const mx_ipoint& bottomRight,
    const mx_angle& angle)
{
    long unsigned int width = bottomRight.x - topLeft.x + 1;
    long unsigned int height = bottomRight.y - topLeft.y + 1;

    // draw to the pixmap

    if (drawToWindowOnly) {
        XFillRectangle(display, window, gc,
            topLeft.x, topLeft.y, width, height);
    } else {
        XFillRectangle(display, storePixmap, gc,
            topLeft.x, topLeft.y, width, height);

        if (doMonitor) {
            if (monitorRect.xb > topLeft.x)
                monitorRect.xb = topLeft.x;
            if (monitorRect.yb > topLeft.y)
                monitorRect.yb = topLeft.y;

            if (monitorRect.xt < bottomRight.x)
                monitorRect.xt = bottomRight.x;
            if (monitorRect.yt < bottomRight.y)
                monitorRect.yt = bottomRight.y;
        }
    }

    return;
}

/*-------------------------------------------------
 * FUNCTION: mx_screen_device::pfillArc
 *
 * DESCRIPTION: Draw a line  in pixel coordinates in the
 * current style
 *
 */

void mx_screen_device::pfillArc(int& err,
    const mx_ipoint& centre,
    const mx_ipoint& size,
    bool isFull,
    double startAng,
    double endAng,
    const mx_angle& angle)
{
    pfillArc(err, ArcChord, centre, size, isFull, startAng, endAng, angle);
}

/*-------------------------------------------------
 * FUNCTION: mx_screen_device::pfillSector
 *
 * DESCRIPTION: Draw a line  in pixel coordinates in the
 * current style
 *
 */

void mx_screen_device::pfillSector(int& err,
    const mx_ipoint& centre,
    const mx_ipoint& size,
    bool isFull,
    double startAng,
    double endAng,
    const mx_angle& angle)
{
    pfillArc(err, ArcPieSlice, centre, size, isFull, startAng, endAng, angle);
}

/*-------------------------------------------------
 * FUNCTION: mx_screen_device::pdrawArc
 *
 * DESCRIPTION: Draw a line  in pixel coordinates in the
 * current style
 *
 */

void mx_screen_device::pdrawArc(int& err,
    const mx_ipoint& centre,
    const mx_ipoint& size,
    bool isFull,
    double startAng,
    double endAng,
    const mx_angle& angle)
{
    int istartAng;
    int iendAng;

    if (isFull) {
        istartAng = 0;
        iendAng = 360 * 64;
    } else {
        istartAng = (int)(-startAng * mx_arc::cfactor + 0.5);
        iendAng = (int)(-endAng * mx_arc::cfactor + 0.5);
        iendAng -= istartAng;
    }

    if (drawToWindowOnly) {
        XDrawArc(
            display,
            window,
            gc,
            centre.x - size.x,
            centre.y - size.y,
            2 * size.x,
            2 * size.y,
            istartAng,
            iendAng);
    } else {
        XDrawArc(
            display,
            storePixmap,
            gc,
            centre.x - size.x,
            centre.y - size.y,
            2 * size.x,
            2 * size.y,
            istartAng,
            iendAng);

        if (doMonitor)
            monitorArcSize(centre, size);
    }

    return;
}

/*-------------------------------------------------
 * FUNCTION: mx_screen_device::pfillArc
 *
 * DESCRIPTION: Draw a line  in pixel coordinates in the
 * current style
 *
 */

void mx_screen_device::pfillArc(int& err,
    int arcFillType,
    const mx_ipoint& centre,
    const mx_ipoint& size,
    bool isFull,
    double startAng,
    double endAng,
    const mx_angle& angle)
{
    int istartAng;
    int iendAng;

    XSetArcMode(display, gc, arcFillType);

    if (isFull) {
        istartAng = 0;
        iendAng = 360 * 64;
    } else {
        istartAng = (int)(-startAng * mx_arc::cfactor + 0.5);
        iendAng = (int)(-endAng * mx_arc::cfactor + 0.5);
        iendAng -= istartAng;
    }

    if (drawToWindowOnly) {
        XFillArc(
            display,
            window,
            gc,
            centre.x - size.x,
            centre.y - size.x,
            2 * size.x,
            2 * size.y,
            istartAng,
            iendAng);
    } else {
        XFillArc(
            display,
            storePixmap,
            gc,
            centre.x - size.x,
            centre.y - size.y,
            2 * size.x,
            2 * size.y,
            istartAng,
            iendAng);

        if (doMonitor)
            monitorArcSize(centre, size);
    }

    return;
}

/*-------------------------------------------------
 * FUNCTION: mx_screen_device::pdrawLine
 *
 * DESCRIPTION: Draw a line  in pixel coordinates in the
 * current style
 *
 */

void mx_screen_device::pdrawLine(int& err,
    const mx_ipoint& start,
    const mx_ipoint& end)
{
    // draw the line
    if (drawToWindowOnly) {
        XDrawLine(display, window, gc, start.x, start.y, end.x, end.y);
    } else {
        XDrawLine(display, storePixmap, gc, start.x, start.y, end.x, end.y);

        if (doMonitor) {
            mx_irect trect(start, end);

            trect.normalise();

            if (monitorRect.xb > trect.xb - extendWidth)
                monitorRect.xb = trect.xb - extendWidth;
            if (monitorRect.xb > trect.yb - extendWidth)
                monitorRect.yb = trect.yb - extendWidth;

            if (monitorRect.xt < trect.xt + extendWidth)
                monitorRect.xt = trect.xt + extendWidth;
            if (monitorRect.yt < trect.yt + extendWidth)
                monitorRect.yt = trect.yt + extendWidth;
        }
    }

    return;
}

/*-------------------------------------------------
 * FUNCTION: mx_screen_device::pdrawRect
 *
 * DESCRIPTION: Draw a line  in pixel coordinates in the
 * current style
 *
 */

void mx_screen_device::pdrawRect(int& err,
    const mx_ipoint& topLeft,
    const mx_ipoint& bottomRight,
    const mx_angle& angle)
{
    long unsigned int width = bottomRight.x - topLeft.x + 1;
    long unsigned int height = bottomRight.y - topLeft.y + 1;

    // draw to the pixmap

    if (drawToWindowOnly) {
        XDrawRectangle(display, window, gc,
            topLeft.x, topLeft.y, width, height);
    } else {
        XDrawRectangle(display, storePixmap, gc,
            topLeft.x, topLeft.y, width, height);

        if (doMonitor) {
            if (monitorRect.xb > topLeft.x)
                monitorRect.xb = topLeft.x;
            if (monitorRect.yb > topLeft.y)
                monitorRect.yb = topLeft.y;

            if (monitorRect.xt < bottomRight.x)
                monitorRect.xt = bottomRight.x;
            if (monitorRect.yt < bottomRight.y)
                monitorRect.yt = bottomRight.y;
        }
    }

    return;
}

/*-------------------------------------------------
 * FUNCTION: mx_screen_device::set_char_style
 *
 * DESCRIPTION: sets up the basic components of a
 * given style in the GC. Colour & font.
 *
 */

void mx_screen_device::set_char_style(int& err, const mx_char_style& charStyle)
{
    double newScale;
    const mx_font* charStyleFont = ((mx_char_style&)charStyle).get_font();

    setForegroundColour(err, last_char_style.colour);
    MX_ERROR_CHECK(err);

    // get the current zoom factor
    newScale = getFrame()->getZoom();

    if (!MX_FLOAT_EQ(newScale, fontScale) || *charStyleFont != *(last_char_style.get_font())) {
        fontScale = newScale;

        m_xft_font = charStyleFont->get_xft_font(err, display, fontScale);
        MX_ERROR_CHECK(err);
    }

    last_char_style = charStyle;

    setForegroundColour(err, last_char_style.colour);
    MX_ERROR_CHECK(err);

abort:;
}

/*--------------------------------------------------------
 * FUNCTION: mx_screen_device::char_style_alignment_offset
 *
 * DESCRIPTION:
 *  calculates in pixels the vertical alignment offset for
 *  a char style
 *
 */

int mx_screen_device::char_style_alignment_offset(const mx_char_style& style)
{
    return (int)(MX_POINTS_TO_MM(style.alignment_offset) * getScreenResolution());
}

/*------------------------------------------------------------
 * FUNCTION: mx_screen_device::draw_underline
 *
 * DESCRIPTION: draws the underline for a char style if needed
 *
 */
void mx_screen_device::draw_underline(
    int& err,
    const mx_ipoint& start,
    const mx_ipoint& end,
    mx_char_style& style)
{
    if (style.effects == mx_underline) {
        mx_line_style ls(mx_solid, 1.0);

        setDefaultLineStyle(err, ls);
        MX_ERROR_CHECK(err);

        pdrawLine(err, start, end);
        MX_ERROR_CHECK(err);
    }
abort:;
}

/*-------------------------------------------------
 * FUNCTION: mx_screen_device::pdrawText
 *
 * DESCRIPTION: Draw some text in pixel coordinates in the
 * given style
 *
 */

int mx_screen_device::pdrawText(int& err,
    const char* s,
    uint32 s_len,
    mx_ipoint pos,
    mx_char_style& charStyle)
{
    mx_ipoint end_pos(pos);

    // this may well need to be optimised later - see how slow it
    // is first
    set_char_style(err, charStyle);
    MX_ERROR_CHECK(err);

    // check alignment
    if (last_char_style.alignment == mx_superscript) {
        pos.y -= char_style_alignment_offset(charStyle);
    } else {
        if (last_char_style.alignment == mx_subscript) {
            pos.y += char_style_alignment_offset(charStyle);
        }
    }

    // with no set width adjust, we can use XDrawString - otherwise,
    // have to draw each character individually
    if (last_char_style.set_width_adjust == 0) {
        if (drawToWindowOnly) {
            auto draw = XftDrawCreate(display, window, DefaultVisual(display, DefaultScreen(display)), DefaultColormap(display, DefaultScreen(display)));

            XftDrawStringUtf8(draw, &m_xft_foreground_colour, m_xft_font,
                              pos.x, pos.y,
                              (const FcChar8 *)s,
                              (int)s_len);
        } else {
            auto draw = XftDrawCreate(display, storePixmap, DefaultVisual(display, DefaultScreen(display)), DefaultColormap(display, DefaultScreen(display)));

            XftDrawStringUtf8(draw, &m_xft_foreground_colour, m_xft_font,
                              pos.x, pos.y,
                              (const FcChar8 *)s,
                              (int)s_len);
        }
        end_pos.x += get_xft_width(s, s_len);
    } else {
        uint32 i, x = pos.x;
        float f;

        for (i = 0; i < s_len; i++) {
            if (drawToWindowOnly) {
                auto draw = XftDrawCreate(display, window, DefaultVisual(display, DefaultScreen(display)), DefaultColormap(display, DefaultScreen(display)));

                XftDrawStringUtf8(draw, &m_xft_foreground_colour, m_xft_font,
                                  pos.x, pos.y,
                                  (const FcChar8 *)s + i,
                                  1);
            } else {
                auto draw = XftDrawCreate(display, storePixmap, DefaultVisual(display, DefaultScreen(display)), DefaultColormap(display, DefaultScreen(display)));

                XftDrawStringUtf8(draw, &m_xft_foreground_colour, m_xft_font,
                                  pos.x, pos.y,
                                  (const FcChar8 *)s + i,
                                  1);
            }
            f = get_xft_width(s + i, 1);
            end_pos.x += (int)f;
            f *= 100 + last_char_style.set_width_adjust;
            f /= 100;
            x += (int)f;
        }
    }

    //    draw_underline(err, pos, end_pos, charStyle);
    //    MX_ERROR_CHECK(err);

    if (doMonitor && (!drawToWindowOnly)) {
        end_pos.y -= m_xft_font->ascent;
        pos.y += m_xft_font->ascent;

        mx_irect trect(pos, end_pos);

        trect.normalise();

        if (monitorRect.xb > trect.xb)
            monitorRect.xb = trect.xb;
        if (monitorRect.xb > trect.yb)
            monitorRect.yb = trect.yb;

        if (monitorRect.xt < trect.xt)
            monitorRect.xt = trect.xt;
        if (monitorRect.yt < trect.yt)
            monitorRect.yt = trect.yt;
    }

abort:
    return end_pos.x - pos.x;
}

/*-------------------------------------------------
 * FUNCTION: mx_print_device::pdrawText
 *
 * DESCRIPTION: Draw some text in pixel coordinates in the
 * given style
 *
 */

void mx_screen_device::pdrawTextLine(int& err,
    const mx_ipoint& pos,
    const mx_char_style& init_char_style,
    const mx_text_item* text_items,
    uint32 num_items)
{
    MX_ERROR_THROW(err, MX_ERROR_NOT_IMPLEMENTED);
abort:;
}

uint32 mx_screen_device::pgetCharWidth(int& err,
    char c,
    const mx_char_style& cs)
{
    // this may well need to be optimised later - see how slow it
    // is first
    set_char_style(err, cs);
    MX_ERROR_CHECK(err);

    return get_xft_width(&c, 1);
abort:
    return 0;
}

void mx_screen_device::pfillPolypoint(int& err, const mx_ipolypoint& pp)
{
    int maxPoints;
    int npoints;

    err = MX_ERROR_OK;

    // get X maximum of points in one go
    maxPoints = getMaxRequests() - 4;
    npoints = pp.get_num_points();

    if (npoints > maxPoints) {
        MX_ERROR_THROW(err, MX_XNPOINTS_FAILED);
    }

    polypointToXPoints(err, pp);
    MX_ERROR_CHECK(err);

    // draw the lines
    if (drawToWindowOnly) {
        XFillPolygon(display, window, gc, xpoints, npoints, Complex, CoordModeOrigin);
    } else {
        XFillPolygon(display, storePixmap, gc, xpoints, npoints, Complex, CoordModeOrigin);

        if (doMonitor)
            monitorPolypointSize(pp);
    }

    return;
abort:;
}

void mx_screen_device::pdrawSegments(int& err,
    const mx_ipolypoint& pp,
    bool linesCoincident)
{
    int maxSegments;
    int nsegmentsLeft, ioutSegment;
    int noutsegments;
    int nsegments;

    err = MX_ERROR_OK;

    // get X maximum of segments in one go
    maxSegments = (getMaxRequests() / 2) - 3;

    // convert the polypoint to XSegments
    nsegments = polypointToXSegments(err, pp, linesCoincident);
    MX_ERROR_CHECK(err);

    nsegmentsLeft = nsegments;
    ioutSegment = 0;

    while (nsegmentsLeft > 0) {
        // how many segments can we put out in one go
        noutsegments = GMIN(nsegmentsLeft, maxSegments);

        // draw the lines
        if (drawToWindowOnly) {
            XDrawSegments(display, window, gc, xsegments + ioutSegment,
                noutsegments);
        } else {
            XDrawSegments(display, storePixmap, gc, xsegments + ioutSegment,
                noutsegments);
        }

        // update counters
        ioutSegment += noutsegments;
        nsegmentsLeft -= noutsegments;
    }

    if (doMonitor && (!drawToWindowOnly))
        monitorPolypointSize(pp);

abort:
    return;
}

void mx_screen_device::pdrawPolypoint(int& err,
    const mx_ipolypoint& pp)
{
    pdrawPolypointIntern(err, pp, TRUE);
}

void mx_screen_device::pdrawPoints(int& err,
    const mx_ipolypoint& pp)
{
    pdrawPolypointIntern(err, pp, FALSE);
}

void mx_screen_device::pdrawPolypointIntern(int& err,
    const mx_ipolypoint& pp,
    bool drawLines)
{
    int maxPoints;
    int npointsLeft, ioutPoint;
    int noutpoints;
    int npoints;

    err = MX_ERROR_OK;

    // get X maximum of points in one go
    if (drawLines) {
        maxPoints = (getMaxRequests() / 2) - 3;
    } else {
        maxPoints = getMaxRequests() - 3;
    }

    npoints = pp.get_num_points();

    // convert the polypoint to XPoints
    polypointToXPoints(err, pp);
    MX_ERROR_CHECK(err);

    npointsLeft = npoints;
    ioutPoint = 0;

    while (npointsLeft > 0) {
        // how many points can we put out in one go
        noutpoints = GMIN(npointsLeft, maxPoints);

        // draw the lines
        if (drawToWindowOnly) {
            if (drawLines) {
                XDrawLines(display, window, gc, xpoints + ioutPoint,
                    noutpoints, CoordModeOrigin);
            } else {
                XDrawPoints(display, window, gc, xpoints + ioutPoint,
                    noutpoints, CoordModeOrigin);
            }
        } else {
            if (drawLines) {
                XDrawLines(display, storePixmap, gc, xpoints + ioutPoint,
                    noutpoints, CoordModeOrigin);
            } else {
                XDrawPoints(display, storePixmap, gc, xpoints + ioutPoint,
                    noutpoints, CoordModeOrigin);
            }
        }

        // update counters
        ioutPoint += noutpoints;
        npointsLeft -= noutpoints;
    }

    if (doMonitor && (!drawToWindowOnly))
        monitorPolypointSize(pp);

    return;
abort:;
}

void mx_screen_device::pdrawCircle(int& err, const mx_ipoint& centre, int radius)
{
    mx_ipoint size(radius, radius);

    err = MX_ERROR_OK;

    pdrawArc(err, centre, size, TRUE, 0, 0, defaultAngle);
    MX_ERROR_CHECK(err);

    return;
abort:
    return;
}

void mx_screen_device::pfillCircle(int& err, const mx_ipoint& centre, int radius)
{
    mx_ipoint size(radius, radius);

    pfillArc(err, centre, size, TRUE, 0, 0, defaultAngle);
    MX_ERROR_CHECK(err);
abort:
    return;
}

void mx_screen_device::invertCaret()
{
    int err = MX_ERROR_OK;
    XGCValues xgcv;

    mx_doc_coord_t pstart;
    mx_doc_coord_t pend;

    mx_ipoint ppstart;
    mx_ipoint ppend;

    if (!caretOn || !XtIsRealized(widget) || (gc == NULL)) {
        return;
    }

    pstart = frameTarget2dev(err, caretBottom);
    MX_ERROR_CHECK(err);

    pend = frameTarget2dev(err, caretTop);
    MX_ERROR_CHECK(err);

    ppstart = pixelSnap(pstart);
    ppend = pixelSnap(pend);

    XSetForeground(display, gc, get_xor_colour_value(screen));
    XSetLineAttributes(display, gc, 1, LineSolid, CapButt, JoinMiter);
    XSetFunction(display, gc, GXxor);
    xgcv.clip_mask = None;
    XChangeGC(display, gc, GCClipMask, &xgcv);

    XDrawLine(display, window, gc, ppstart.x, ppstart.y, ppend.x, ppend.y);

    XSetFunction(display, gc, GXcopy);

    caretVisible = !caretVisible;

    return;
abort:
    global_error_trace->print();
    MX_ERROR_CLEAR(err);
}

/*-------------------------------------------------
 * FUNCTION: mx_screen_device::pxorRect
 *
 * DESCRIPTION: xor a rectangle in pixel coordinates
 *
 *
 */

void mx_screen_device::pxorRect(int& err,
    const mx_ipoint& topLeft,
    const mx_ipoint& bottomRight)
{
    long unsigned int width = bottomRight.x - topLeft.x + 1;
    long unsigned int height = bottomRight.y - topLeft.y + 1;

    // draw to the pixmap
    XSetFunction(display, gc, GXxor);
    XSetForeground(display, gc, get_xor_colour_value(screen));

    if (drawToWindowOnly) {
        XFillRectangle(display, window, gc,
            topLeft.x, topLeft.y, width, height);
    } else {
        XFillRectangle(display, storePixmap, gc,
            topLeft.x, topLeft.y, width, height);
    }

    XSetFunction(display, gc, GXcopy);

    if (doMonitor && (!drawToWindowOnly)) {
        if (monitorRect.xb > topLeft.x)
            monitorRect.xb = topLeft.x;
        if (monitorRect.yb > topLeft.y)
            monitorRect.yb = topLeft.y;

        if (monitorRect.xt < bottomRight.x)
            monitorRect.xt = bottomRight.x;
        if (monitorRect.yt < bottomRight.y)
            monitorRect.yt = bottomRight.y;
    }

    return;
}

/*-------------------------------------------------
 * FUNCTION: mx_screen_device::pxorResizeHandles
 *
 * DESCRIPTION:
 *
 * draw a set of "resize handles", ie eight little boxes at the corner of an
 * unfilled reactangle. Do all in xormode so it can be undone quickly by
 * doing another call with the same parameters
 *
 */
void mx_screen_device::pxorResizeHandles(int& err,
    const mx_ipoint& topLeft,
    const mx_ipoint& bottomRight,
    uint32 handleWidth)
{
    // first draw the outline

    long unsigned int width = bottomRight.x - topLeft.x - 1;
    long unsigned int height = bottomRight.y - topLeft.y - 1;
    int i;
    XRectangle rects[8];

    // draw to the pixmap
    XSetFunction(display, gc, GXxor);
    XSetForeground(display, gc, get_xor_colour_value(screen));
    if (drawToWindowOnly) {
        XDrawRectangle(display, window, gc,
            topLeft.x, topLeft.y, width, height);
    } else {
        XDrawRectangle(display, storePixmap, gc,
            topLeft.x, topLeft.y, width, height);
    }

    // now draw the the handles

    for (i = 0; i < 8; i++) {
        rects[i].width = handleWidth;
        rects[i].height = handleWidth;
    }

    rects[0].x = rects[3].x = rects[5].x = topLeft.x + 1;
    rects[1].x = rects[6].x = (topLeft.x + bottomRight.x - handleWidth) >> 1;
    rects[2].x = rects[4].x = rects[7].x = (bottomRight.x - handleWidth) - 1;

    rects[0].y = rects[1].y = rects[2].y = topLeft.y + 1;
    rects[3].y = rects[4].y = (topLeft.y + bottomRight.y - handleWidth) >> 1;
    rects[5].y = rects[6].y = rects[7].y = (bottomRight.y - handleWidth) - 1;

    if (drawToWindowOnly) {
        XFillRectangles(display, window, gc, rects, 8);
        XSetFunction(display, gc, GXcopy);
    } else {
        XFillRectangles(display, storePixmap, gc, rects, 8);
        XSetFunction(display, gc, GXcopy);
    }

    if (doMonitor && (!drawToWindowOnly)) {
        if (monitorRect.xb > topLeft.x)
            monitorRect.xb = topLeft.x;
        if (monitorRect.yb > topLeft.y)
            monitorRect.yb = topLeft.y;

        if (monitorRect.xt < bottomRight.x)
            monitorRect.xt = bottomRight.x;
        if (monitorRect.yt < bottomRight.y)
            monitorRect.yt = bottomRight.y;
    }

    return;
}

/*-------------------------------------------------
 * FUNCTION: mx_screen_device::setXor
 *
 * DESCRIPTION:
 *
 * draw a set of "resize handles", ie eight little boxes at the corner of an
 * unfilled reactangle. Do all in xormode so it can be undone quickly by
 * doing another call with the same parameters
 *
 */
void mx_screen_device::setXor(int& err, bool setOn)
{
    err = MX_ERROR_OK;

    if (setOn) {
        XSetFunction(display, gc, GXxor);
    } else {
        XSetFunction(display, gc, GXcopy);
    }
}
