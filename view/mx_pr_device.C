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
 * MODULE : mx_pr_device.C
 *
 * AUTHOR : David Miller
 *
 * DESCRIPTION:
 * Module mx_pr_device.C
 *
 *
 */
#include <pwd.h>

#include <mx_list_iter.h>
#include <mx_pr_device.h>

/*-------------------------------------------------
 * FUNCTION: mx_print_device::mx_print_device
 *
 * DESCRIPTION:
 *
 *
 */

mx_print_device::mx_print_device(int& err, mx_frame& deviceFrame, mx_point& deviceSize) : mx_device(deviceFrame)
{
    err = MX_ERROR_OK;

    m_cairo_surface = nullptr;
    m_cairo = nullptr;

    // cairo copes with RGB
    doesRGB = true;

    // cairo can join and fill any path
    drawsChord = true;
    drawsSector = true;
    fillsSector = true;

    // and beziers
    drawsBeziers = true;
    fillsBeziers = true;

    // can clip too
    clipsArc = true;
    clipsSector = true;

    // and transformations
    doesRotate = true;
    doesAffine = true;

    // we don't need a resolution, everything is done with floats
    setScreenResolution(1.0);

    document_fonts = new mx_list;

}

/*-------------------------------------------------
 * FUNCTION: mx_print_device::mx_print_device
 *
 * DESCRIPTION:
 *
 *
 */
mx_print_device::~mx_print_device()
{
    mx_list_iterator iter(*document_fonts);
    mx_font* f;

    while (iter.more()) {
        f = (mx_font*)iter.data();
        delete f;
    }

    delete document_fonts;

    return;
}

/*-------------------------------------------------
 * FUNCTION: mx_print_device:setShiftOrigin
 *
 * DESCRIPTION: Shift the contents of a window so
 * that the background pixmap is used where possible
 * and the new areas are redrawn.
 *
 * This is typically used by scrolling to save any
 * area which can be redrawn any then to redraw the
 * new areas.
 */

void mx_print_device::setShiftOrigin(mx_doc_coord_t& wantedShiftOrigin, mx_doc_coord_t& gotShiftOrigin, bool doScroll)
{
    gotShiftOrigin = pixelSnapFrame(wantedShiftOrigin);
    return;
}

/*-------------------------------------------------
 * FUNCTION: mx_print_device::closeGap
 *
 * DESCRIPTION: Close a gap in the device in a direction,
 * from a point. The gapSize must have either a zero
 * x or a zero y. The non-zero value may be negative
 * depending on the direction in which the gap is to
 * be closed. The startPoint pixel is the 'first' pixel
 * to disappear
 */

void mx_print_device::closeGap(int& err, bool doX, mx_point& frameStartPoint, double frameGapSize)
{
}

/*-------------------------------------------------
 * FUNCTION: mx_print_device::makeGap
 *
 * DESCRIPTION: Make a gap in the device in a direction,
 * from a point. The gap size may be negative depending
 * on the direction in which the gap is to
 * be made. The startPoint pixel is the 'first' pixel
 * to be moved
 */

void mx_print_device::makeGap(int& err, bool doX, mx_point& frameStartPoint, double frameGapSize)
{
}

/*-------------------------------------------------
 * FUNCTION: mx_print_device:shift
 *
 * DESCRIPTION: Shift the contents of a window so
 * that the background pixmap is used where possible
 * and the new areas are redrawn.
 *
 * This is typically used by scrolling to save any
 * area which can be redrawn any then to redraw the
 * new areas.
 */

void mx_print_device::shift(int& err)
{
}

/*-------------------------------------------------
 * FUNCTION: mx_print_device:refresh
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

void mx_print_device::refresh(int& err,
    mx_point* topLeft,
    mx_point* bottomRight)
{
    mx_doc_coord_t devTopLeft;
    mx_doc_coord_t devBottomRight;
    mx_doc_coord_t frameTopLeft;
    mx_doc_coord_t frameBottomRight;
    mx_draw_event drawEvent;

    err = MX_ERROR_OK;

    // convert top left coordinate to device coordinates
    if (topLeft != NULL) {
        frameTopLeft.sheet_number = 0;
        frameTopLeft.p = *topLeft;

        devTopLeft = frame2dev(frameTopLeft);

        devTopLeft.p.x = GMAX(devTopLeft.p.x, 0);
        devTopLeft.p.y = GMAX(devTopLeft.p.y, 0);
    }

    // convert bottom right coordinate to device coordinates
    if (bottomRight != NULL) {
        frameBottomRight.sheet_number = 0;
        frameBottomRight.p = *bottomRight;

        devBottomRight = frame2dev(frameBottomRight);

        devBottomRight.p.x = GMIN(bottomRight->x, printerSize.x - 1);
        devBottomRight.p.y = GMIN(bottomRight->y, printerSize.y - 1);
    } else {
        devBottomRight.p.x = printerSize.x - 1;
        devBottomRight.p.y = printerSize.y - 1;
    }

    drawEvent.setTL(devTopLeft);
    drawEvent.setBR(devBottomRight);
    drawEvent.setCount(1);
    drawEvent.setType(MX_EVENT_DRAW);

    draw(err, drawEvent);
    MX_ERROR_CHECK(err);

    return;
abort:
    return;
}

void mx_print_device::startPage(int& err, int n, float page_width, float page_height, bool is_landscape)
{
    auto w = is_landscape ? page_height : page_width;
    auto h = is_landscape ? page_width : page_height ;

    w = MX_MM_TO_POINTS(w);
    h = MX_MM_TO_POINTS(h);

    if (!m_cairo_surface) {
        m_cairo_surface = cairo_pdf_surface_create(m_output_file_name.c_str(), w, h);
        m_cairo = cairo_create(m_cairo_surface);
    } else {
        cairo_pdf_surface_set_size(m_cairo_surface, w, h);
    }
}

void mx_print_device::endPage(int& err)
{
    cairo_show_page(m_cairo);
}

void mx_print_device::setOutputFile(const char *file_name)
{
    m_output_file_name = file_name;
}

void mx_print_device::setPolypointPath(int& err, const mx_ipolypoint& pp, bool doNotClosePath)
{
}

void mx_print_device::setTransform(int& err,
    const char* saveName,
    bool doShift,
    const mx_point& shift,
    bool doScale,
    const mx_point& scaleFactor,
    bool doRotation,
    const mx_angle& angle)
{
}

void mx_print_device::resetTransform(int& err, const char* saveName)
{
}

void mx_print_device::startPrint()
{
}

void mx_print_device::endPrint()
{
    if (m_cairo != nullptr) {
        cairo_destroy(m_cairo);
        cairo_surface_destroy(m_cairo_surface);

        m_cairo = nullptr;
        m_cairo_surface = nullptr;
    }
}
