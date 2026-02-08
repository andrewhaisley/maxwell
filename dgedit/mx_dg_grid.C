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
 * MODULE/CLASS : mx_dg_editor
 *
 * AUTHOR : David Miller
 *
 *
 *
 * DESCRIPTION:
 *
 */

#include "mx_dg_editor.h"

#define MX_DG_MAX_GRID_POINTS 512

mx_dg_grid::mx_dg_grid()
    : size(10, 10)
{
    active = FALSE;
}

/*-------------------------------------------------
 * FUNCTION: mx_dg_grid::snapPoint
 *
 * DESCRIPTION:
 *
 *
 */

void mx_dg_grid::snapPoint(mx_point& inpoint)
{
    if (!active)
        return;

    inpoint.x = (inpoint.x - anchor.x) / size.x;
    inpoint.x = rint(inpoint.x) * size.x + anchor.x;

    inpoint.y = (inpoint.y - anchor.y) / size.y;
    inpoint.y = rint(inpoint.y) * size.y + anchor.y;
}

/*-------------------------------------------------
 * FUNCTION: mx_dg_grid::draw
 *
 * DESCRIPTION:
 *
 *
 */

void mx_dg_grid::draw(int& err,
    mx_rect& drawRect,
    mx_device* device)
{
    double rval;
    static mx_doc_polypoint pp;
    int startX, endX, startY, endY;
    int npoints, nxpoints, nypoints;
    int nppoints;
    double startPointx;
    int ix, iy;
    mx_colour colour;

    err = MX_ERROR_OK;
    if (!active)
        return;

    mx_point addPoint, saddPoint;

    // get integer coordinates of the starts
    // of the grid

    rval = (drawRect.xb - anchor.x) / size.x;
    startX = (int)GNINT(floor(rval));

    rval = (drawRect.xt - anchor.x) / size.x;
    endX = (int)GNINT(ceil(rval));

    rval = (drawRect.yb - anchor.y) / size.y;
    startY = (int)GNINT(floor(rval));

    rval = (drawRect.yt - anchor.y) / size.y;
    endY = (int)GNINT(ceil(rval));

    nxpoints = endX - startX + 1;
    nypoints = endY - startY + 1;
    npoints = nxpoints * nypoints;

    npoints = GMIN(npoints,
        MX_DG_MAX_GRID_POINTS);

    pp.pp.clear(npoints);

    startPointx = startX * size.x + anchor.x;
    addPoint.y = startY * size.y + anchor.y;
    nppoints = 0;

    device->setForegroundColour(err, colour);
    MX_ERROR_CHECK(err);

    for (iy = 0; iy < nypoints; iy++) {
        addPoint.x = startPointx;
        for (ix = 0; ix < nxpoints; ix++) {
            pp.pp.addPoint(addPoint);

            nppoints++;

            if (nppoints == MX_DG_MAX_GRID_POINTS) {
                device->drawPoints(err, pp);
                MX_ERROR_CHECK(err);

                nppoints = 0;
            }
            addPoint.x += size.x;
        }
        addPoint.y += size.y;
    }

    // flush the buffer
    if (nppoints > 0) {
        device->drawPoints(err, pp);
        MX_ERROR_CHECK(err);
    }
abort:;
}
