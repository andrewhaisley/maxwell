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
 * MODULE : mx_pr_clip.C
 *
 * AUTHOR : David Miller
 *
 * This file
 *
 * DESCRIPTION: Clip routines on the print device
 * Module mx_pr_clip.C
 *
 */

#include "mx_pr_device.h"

/*-------------------------------------------------
 * FUNCTION: mx_print_device::pushClipMask
 *
 * DESCRIPTION: Pop a clip mask
 *
 *
 */

void mx_print_device::pushClipMask(int& err,
    bool doIntersect)
{
    // here the current path has been set up as
    // the new clip path to interact with the old
    // clip mask - can only do intersect at the
    // moment - that's the only one we currently need

    err = MX_ERROR_OK;

    // save the graphics state - clip to the current path
    // and then clear the path
    if (fprintf(tempFile, "gsave ec np\n") == EOF) {
        MX_ERROR_THROW(err, MX_PS_FILE_ERROR);
    }
abort:;
}

/*-------------------------------------------------
 * FUNCTION: mx_print_device::popClipMask
 *
 * DESCRIPTION: Pop a clip mask
 *
 *
 */

void mx_print_device::popClipMask(int& err)
{
    err = MX_ERROR_OK;

    // restore the old clipping path and
    // clear the path that was clipped
    if (fprintf(tempFile, "grestore np\n") == EOF) {
        MX_ERROR_THROW(err, MX_PS_FILE_ERROR);
    }

    // need to reset all styles
    resetStyles();
abort:;
}

/*-------------------------------------------------
 * FUNCTION: mx_print_device::pushClipLines
 *
 * DESCRIPTION: Clip output to a polygon. Return a pointer
 *              to the region so that clipping can
 *              be turned off
 */

void mx_print_device::pushClipPolypoint(int& err,
    const mx_ipolypoint& pp,
    bool doIntersect)
{
    err = MX_ERROR_OK;

    setPolypointPath(err, pp, TRUE);
    MX_ERROR_CHECK(err);

    pushClipMask(err, doIntersect);
    MX_ERROR_CHECK(err);
abort:
    return;
}

/*-------------------------------------------------
 * FUNCTION: mx_print_device::pushClipRect
 *
 * DESCRIPTION: Clip output to a rectanble. Return a pointer
 *              to the region so that clipping can
 *              be turned off
 */

void mx_print_device::pushClipRect(int& err,
    const mx_irect& coord,
    const mx_angle& angle,
    bool doIntersect)
{
    err = MX_ERROR_OK;
    poperRect(err, coord, angle, "rc");
    MX_ERROR_CHECK(err);
abort:;
}

/*-------------------------------------------------
 * FUNCTION: mx_print_device::pclipArc
 *
 * DESCRIPTION: Clip output to an arc
 *
 */

void mx_print_device::pclipArc(int& err,
    const mx_ipoint& centre,
    const mx_ipoint& size,
    bool isFull,
    double startAng,
    double endAng,
    const mx_angle& angle,
    bool doIntersect)
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

    pushClipMask(err, TRUE);
    MX_ERROR_CHECK(err);
abort:
    return;
}

/*-------------------------------------------------
 * FUNCTION: mx_print_device::pclipSector
 *
 * DESCRIPTION: Clip output to an arc
 *
 */

void mx_print_device::pclipSector(int& err,
    const mx_ipoint& centre,
    const mx_ipoint& size,
    bool isFull,
    double startAng,
    double endAng,
    const mx_angle& angle,
    bool doIntersect)
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

    pushClipMask(err, TRUE);
    MX_ERROR_CHECK(err);
abort:
    return;
}
