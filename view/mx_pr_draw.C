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
 * MODULE : mx_pr_draw.C
 *
 * AUTHOR : David Miller
 *
 * This file
 *
 * DESCRIPTION: The drawing routines for a print device
 * Module mx_sc_draw.C
 *
 *
 */

#include <mx_list_iter.h>
#include <mx_pr_device.h>

/*-------------------------------------------------
 * FUNCTION: mx_print_device::pxorRect
 *
 * DESCRIPTION: xor a rectangle in pixel coordinates
 *
 *
 */

void mx_print_device::pxorRect(int& err,
    const mx_ipoint& topLeft,
    const mx_ipoint& bottomRight)
{
}

/*-------------------------------------------------
 * FUNCTION: mx_print_device::popRect
 *
 * DESCRIPTION: Fill a rectangle in pixel coordinates
 *
 *
 */

void mx_print_device::poperRect(int& err,
    const mx_irect& rect,
    const mx_angle& angle,
    const char* op)
{
}

/*-------------------------------------------------
 * FUNCTION: mx_print_device::pfillRect
 *
 * DESCRIPTION: Fill a rectangle in pixel coordinates
 *
 *
 */

void mx_print_device::pfillRect(int& err,
    const mx_ipoint& topLeft,
    const mx_ipoint& bottomRight,
    const mx_angle& angle)
{
    mx_irect rect(topLeft, bottomRight);

    err = MX_ERROR_OK;
    poperRect(err, rect, angle, "rf");
    MX_ERROR_CHECK(err);
abort:;
}

/*-------------------------------------------------
 * FUNCTION: mx_print_device::pdrawRect
 *
 * DESCRIPTION: Fill a rectangle in pixel coordinates
 *
 *
 */

void mx_print_device::pdrawRect(int& err,
    const mx_ipoint& topLeft,
    const mx_ipoint& bottomRight,
    const mx_angle& angle)
{
    mx_irect rect(topLeft, bottomRight);

    err = MX_ERROR_OK;
    poperRect(err, rect, angle, "rs");
    MX_ERROR_CHECK(err);
abort:;
}

/*-------------------------------------------------
 * FUNCTION: mx_print_device::pdrawLine
 *
 * DESCRIPTION: Draw a line  in pixel coordinates in the
 * current style
 *
 */

void mx_print_device::pdrawLine(int& err,
    const mx_ipoint& start,
    const mx_ipoint& end)
{
}

/*-------------------------------------------------
 * FUNCTION: mx_print_device::pdrawText
 *
 * DESCRIPTION: Draw some text in pixel coordinates in the
 * given style
 *
 */

int mx_print_device::pdrawText(int& err,
    const char* s,
    uint32 s_len,
    mx_ipoint pos,
    mx_char_style& charStyle)
{
    lastCharStyle = charStyle;

    setColour(err, lastCharStyle.colour);
    MX_ERROR_CHECK(err);

    // adjust alignment as needed
    if (lastCharStyle.alignment == mx_superscript) {
        pos.y -= MX_POINTS_TO_MM(lastCharStyle.alignment_offset);
    } else {
        if (lastCharStyle.alignment == mx_subscript) {
            pos.y += MX_POINTS_TO_MM(lastCharStyle.alignment_offset);
        }
    }

    cairo_set_font_face(m_cairo, lastCharStyle.get_font()->get_cairo_font());
    cairo_set_font_size(m_cairo, lastCharStyle.get_font()->get_size());

    if (lastCharStyle.set_width_adjust == 0) {
        cairo_move_to(m_cairo, MX_MM_TO_POINTS(pos.x), MX_MM_TO_POINTS(pos.y));
        cairo_show_text(m_cairo, std::string(s, s_len).c_str());

    } else {
        uint32 x = pos.x;

        for (uint32 i = 0; i < s_len; i++) {
            cairo_move_to(m_cairo, MX_MM_TO_POINTS(x), MX_MM_TO_POINTS(pos.y));
            cairo_show_text(m_cairo, std::string(s + i, 1).c_str());

            x += (int)(0.5 + frame2dev(lastCharStyle.width(s[i])));
        }
    }

    return (int)(lastCharStyle.width(s) * getScreenResolution());

abort:
    return 0;
}

/*-------------------------------------------------
 * FUNCTION: mx_print_device::pdrawTextLine
 *
 * DESCRIPTION: Draw some text in pixel coordinates in the
 * given style
 *
 */

void mx_print_device::pdrawTextLine(int& err,
    const mx_ipoint& pos,
    const mx_char_style& init_char_style,
    const mx_text_item* text_items,
    uint32 num_items)
{
    MX_ERROR_THROW(err, MX_ERROR_NOT_IMPLEMENTED);
abort:;
}

uint32 mx_print_device::pgetCharWidth(int& err,
    char c,
    const mx_char_style& cs)
{
    return (uint32)(cs.width(c) * getScreenResolution());
}

void mx_print_device::pfillPolypoint(int& err, const mx_ipolypoint& pp)
{
}

void mx_print_device::pdrawPolypoint(int& err, const mx_ipolypoint& pp)
{
}

void mx_print_device::setRect(int& err,
    const mx_irect& rect,
    const mx_angle& angle,
    mx_ipoint& startPoint,
    mx_ipoint& size,
    bool& doTransform,
    const char* saveName)
{
    err = MX_ERROR_OK;

    doTransform = (angle.a() != 0);

    startPoint.x = rect.xb;
    startPoint.y = rect.yb;
    size.x = rect.xt - rect.xb;
    size.y = rect.yt - rect.yb;

    if (doTransform) {
        setTransform(err,
            saveName,
            TRUE,
            startPoint,
            FALSE,
            defaultPoint,
            TRUE,
            angle);
        MX_ERROR_CHECK(err);

        startPoint.x = 0;
        startPoint.y = size.y;
    }
abort:;
}

void mx_print_device::pArcPath(int& err,
    const mx_ipoint& centre,
    const mx_ipoint& size,
    bool isFull,
    double startAng,
    double endAng,
    bool isClosed,
    bool isSector,
    const mx_angle& angle)
{
}

void mx_print_device::pdrawArc(int& err,
    const mx_ipoint& centre,
    const mx_ipoint& size,
    bool isFull,
    double startAng,
    double endAng,
    const mx_angle& angle)
{
}

void mx_print_device::pfillArc(int& err,
    const mx_ipoint& centre,
    const mx_ipoint& size,
    bool isFull,
    double startAng,
    double endAng,
    const mx_angle& angle)
{
}

void mx_print_device::pdrawSector(int& err,
    const mx_ipoint& centre,
    const mx_ipoint& size,
    bool isFull,
    double startAng,
    double endAng,
    const mx_angle& angle)
{
}

void mx_print_device::pfillSector(int& err,
    const mx_ipoint& centre,
    const mx_ipoint& size,
    bool isFull,
    double startAng,
    double endAng,
    const mx_angle& angle)
{
}

void mx_print_device::pdrawChord(int& err,
    const mx_ipoint& centre,
    const mx_ipoint& size,
    bool isFull,
    double startAng,
    double endAng,
    const mx_angle& angle)
{
}

void mx_print_device::pdrawCircle(int& err, const mx_ipoint& centre, int radius)
{
}

void mx_print_device::pfillCircle(int& err, const mx_ipoint& centre, int radius)
{
}
