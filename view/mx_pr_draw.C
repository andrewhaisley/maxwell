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
    bool doTransform;
    mx_ipoint startCoord;
    mx_ipoint size;

    err = MX_ERROR_OK;

    setRect(err, rect, angle, startCoord, size, doTransform, "smat");
    MX_ERROR_CHECK(err);

    if (fprintf(tempFile, "%d %d %d %d %s\n",
            startCoord.x,
            -startCoord.y,
            size.x,
            size.y, op)
        == EOF) {
        MX_ERROR_THROW(err, MX_PS_FILE_ERROR);
    }

    if (doTransform) {
        resetTransform(err, "smat");
        MX_ERROR_CHECK(err);
    }

abort:
    return;
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
    if (fprintf(tempFile, "%d %d mt %d %d lt st\n",
            start.x,
            -start.y,
            end.x,
            -end.y)
        == EOF) {
        MX_ERROR_THROW(err, MX_PS_FILE_ERROR);
    }
abort:;
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
    const mx_char_style& charStyle)
{
    uint32 i;
    float w;
    unsigned char* str = (unsigned char*)s;

    lastCharStyle = charStyle;

    setFont(err);
    MX_ERROR_CHECK(err);

    setColour(err, lastCharStyle.colour);
    MX_ERROR_CHECK(err);

    // check alignment
    if (lastCharStyle.alignment == mx_superscript) {
        pos.y -= (int)MX_POINTS_TO_MM(
            lastCharStyle.alignment_offset * getScreenResolution());
    } else {
        if (lastCharStyle.alignment == mx_subscript) {
            pos.y += (int)MX_POINTS_TO_MM(
                lastCharStyle.alignment_offset * getScreenResolution());
        }
    }

    if (lastCharStyle.set_width_adjust == 0) {
        if (fputc('(', tempFile) == EOF) {
            MX_ERROR_THROW(err, MX_PS_FILE_ERROR);
        }

        for (i = 0; i < s_len; i++) {
            if (str[i] == '(' || str[i] == ')' || str[i] == '\\') {
                if (fputc('\\', tempFile) == EOF) {
                    MX_ERROR_THROW(err, MX_PS_FILE_ERROR);
                }
                if (fputc(str[i], tempFile) == EOF) {
                    MX_ERROR_THROW(err, MX_PS_FILE_ERROR);
                }
            } else {
                if (str[i] > 127) {
                    if (fprintf(tempFile, "\\%03o", str[i]) == EOF) {
                        MX_ERROR_THROW(err, MX_PS_FILE_ERROR);
                    }
                } else {
                    if (fputc(str[i], tempFile) == EOF) {
                        MX_ERROR_THROW(err, MX_PS_FILE_ERROR);
                    }
                }
            }
        }

        if (fputc(')', tempFile) == EOF) {
            MX_ERROR_THROW(err, MX_PS_FILE_ERROR);
        }

        if (fprintf(tempFile, " %d %d ds\n", pos.x, -pos.y) == EOF) {
            MX_ERROR_THROW(err, MX_PS_FILE_ERROR);
        }
    } else {
        uint32 i, x = pos.x;

        for (i = 0; i < s_len; i++) {
            if (fputc('(', tempFile) == EOF) {
                MX_ERROR_THROW(err, MX_PS_FILE_ERROR);
            }

            if (str[i] == '(' || str[i] == ')' || str[i] == '\\') {
                if (fputc('\\', tempFile) == EOF) {
                    MX_ERROR_THROW(err, MX_PS_FILE_ERROR);
                }
                if (fputc(str[i], tempFile) == EOF) {
                    MX_ERROR_THROW(err, MX_PS_FILE_ERROR);
                }
            } else {
                if (str[i] > 127) {
                    if (fprintf(tempFile, "\\%03o", str[i]) == EOF) {
                        MX_ERROR_THROW(err, MX_PS_FILE_ERROR);
                    }
                } else {
                    if (fputc(str[i], tempFile) == EOF) {
                        MX_ERROR_THROW(err, MX_PS_FILE_ERROR);
                    }
                }
            }

            if (fputc(')', tempFile) == EOF) {
                MX_ERROR_THROW(err, MX_PS_FILE_ERROR);
            }
            if (fprintf(tempFile, " %d %d ds ", x, -pos.y) == EOF) {
                MX_ERROR_THROW(err, MX_PS_FILE_ERROR);
            }

            x += (int)(0.5 + frame2dev(lastCharStyle.width(str[i])));
        }
    }

    w = lastCharStyle.width(s);

    return (int)(w * getScreenResolution());

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
    err = MX_ERROR_OK;

    setPolypointPath(err, pp, TRUE);
    MX_ERROR_CHECK(err);

    if (fprintf(tempFile, "ef\n") == EOF) {
        MX_ERROR_THROW(err, MX_PS_FILE_ERROR);
    }
abort:
    return;
}

void mx_print_device::pdrawPolypoint(int& err, const mx_ipolypoint& pp)
{
    err = MX_ERROR_OK;

    setPolypointPath(err, pp, FALSE);
    MX_ERROR_CHECK(err);

    if (fprintf(tempFile, "st\n") == EOF) {
        MX_ERROR_THROW(err, MX_PS_FILE_ERROR);
    }

abort:;
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
    mx_ipoint ocentre;
    int osize;
    double oendAng, ostartAng;
    bool doScale = (size.x != size.y);
    bool doRotate = (angle.a() != 0);
    bool doTransform = (doScale || doRotate);

    err = MX_ERROR_OK;

    if (doTransform) {
        // we need to scale
        // save the old state

        setTransform(err,
            "smat",
            TRUE,
            centre,
            doScale,
            size,
            doRotate,
            angle);
        MX_ERROR_CHECK(err);

        ocentre.x = ocentre.y = 0;
        osize = 1;
    } else {
        ocentre = centre;
        osize = size.x;
    }

    if (isFull) {
        ostartAng = 0;
        oendAng = 360;
    } else {
        oendAng = 360.0 - (startAng * 360.0 / PITWO);
        ostartAng = 360.0 - (endAng * 360.0 / PITWO);
    }

    if (isSector) {
        // add initial point
        if (fprintf(tempFile, "%d %d mv ",
                ocentre.x,
                -ocentre.y)
            == EOF) {
            MX_ERROR_THROW(err, MX_PS_FILE_ERROR);
        }
    } else {
        // make sure we clear the path
        if (fprintf(tempFile, "np ") == EOF) {
            MX_ERROR_THROW(err, MX_PS_FILE_ERROR);
        }
    }

    // add the arc
    if (fprintf(tempFile, "%d %d %d %.2f %.2f arc ",
            ocentre.x,
            -ocentre.y,
            osize,
            ostartAng,
            oendAng)
        == EOF) {
        MX_ERROR_THROW(err, MX_PS_FILE_ERROR);
    }

    if (isClosed) {
        if (fprintf(tempFile, "cp\n") == EOF) {
            MX_ERROR_THROW(err, MX_PS_FILE_ERROR);
        }
    }

    // set the old transform back for drawing -
    // otherwise line width is scaled

    if (doTransform) {
        resetTransform(err,
            "smat");
        MX_ERROR_CHECK(err);
    }
abort:;
}

void mx_print_device::pdrawArc(int& err,
    const mx_ipoint& centre,
    const mx_ipoint& size,
    bool isFull,
    double startAng,
    double endAng,
    const mx_angle& angle)
{
    err = MX_ERROR_OK;

    pArcPath(err,
        centre,
        size,
        isFull,
        startAng,
        endAng,
        FALSE,
        FALSE,
        angle);
    MX_ERROR_CHECK(err);

    if (fprintf(tempFile, "st\n") == EOF) {
        MX_ERROR_THROW(err, MX_PS_FILE_ERROR);
    }

    return;
abort:
    return;
}

void mx_print_device::pfillArc(int& err,
    const mx_ipoint& centre,
    const mx_ipoint& size,
    bool isFull,
    double startAng,
    double endAng,
    const mx_angle& angle)
{
    err = MX_ERROR_OK;

    pArcPath(err,
        centre,
        size,
        isFull,
        startAng,
        endAng,
        FALSE,
        FALSE,
        angle);
    MX_ERROR_CHECK(err);

    if (fprintf(tempFile, "ef\n") == EOF) {
        MX_ERROR_THROW(err, MX_PS_FILE_ERROR);
    }

    return;
abort:
    return;
}

void mx_print_device::pdrawSector(int& err,
    const mx_ipoint& centre,
    const mx_ipoint& size,
    bool isFull,
    double startAng,
    double endAng,
    const mx_angle& angle)
{
    err = MX_ERROR_OK;

    pArcPath(err,
        centre,
        size,
        isFull,
        startAng,
        endAng,
        TRUE,
        TRUE,
        angle);
    MX_ERROR_CHECK(err);

    if (fprintf(tempFile, "st\n") == EOF) {
        MX_ERROR_THROW(err, MX_PS_FILE_ERROR);
    }

    return;
abort:
    return;
}

void mx_print_device::pfillSector(int& err,
    const mx_ipoint& centre,
    const mx_ipoint& size,
    bool isFull,
    double startAng,
    double endAng,
    const mx_angle& angle)
{
    err = MX_ERROR_OK;

    pArcPath(err,
        centre,
        size,
        isFull,
        startAng,
        endAng,
        FALSE,
        TRUE,
        angle);
    MX_ERROR_CHECK(err);

    if (fprintf(tempFile, "ef\n") == EOF) {
        MX_ERROR_THROW(err, MX_PS_FILE_ERROR);
    }

    return;
abort:
    return;
}

void mx_print_device::pdrawChord(int& err,
    const mx_ipoint& centre,
    const mx_ipoint& size,
    bool isFull,
    double startAng,
    double endAng,
    const mx_angle& angle)
{
    err = MX_ERROR_OK;

    pArcPath(err,
        centre,
        size,
        isFull,
        startAng,
        endAng,
        TRUE,
        FALSE,
        angle);
    MX_ERROR_CHECK(err);

    if (fprintf(tempFile, "st\n") == EOF) {
        MX_ERROR_THROW(err, MX_PS_FILE_ERROR);
    }

    return;
abort:
    return;
}

void mx_print_device::pdrawCircle(int& err, const mx_ipoint& centre, int radius)
{
    mx_ipoint size(radius, radius);

    pdrawArc(err, centre, size, TRUE, 0, 0, defaultAngle);
    MX_ERROR_CHECK(err);

abort:;
}

void mx_print_device::pfillCircle(int& err, const mx_ipoint& centre, int radius)
{
    mx_ipoint size(radius, radius);

    pfillArc(err, centre, size, TRUE, 0, 0, defaultAngle);
    MX_ERROR_CHECK(err);

abort:;
}
