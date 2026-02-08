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
 * MODULE : mx_dg_filter.C
 *
 * AUTHOR : David Miller
 *
 * This file
 *
 * DESCRIPTION:
 * Module mx_dg_filter.C
 *
 *
 * Revision 1.1  1996/10/06 13:15:45  andrew
 * Initial revision
 *
 *
 */

#include "mx_dg_area.h"

mx_dg_filter::mx_dg_filter()
{
    flags = 0;
}

void mx_dg_filter::setFilterBox(const mx_rect& irect)
{
    filterBox = irect;
    flags |= MX_DG_FILTER_CHECK_BOX;
}

bool mx_dg_filter::filterGeometry(int& err,
    mx_dg_quad_geom& quadGeom) const
{
    mx_rect box;
    bool hasIntersect;

    err = MX_ERROR_OK;

    if (flags & MX_DG_FILTER_CHECK_BOX) {
        if (quadGeom.getStage() < MX_DG_QGEOM_GET_GBOX)
            return FALSE;

        hasIntersect = filterBox.intersects(quadGeom.getBox());
    } else {
        hasIntersect = TRUE;
    }

    return hasIntersect;
}

bool mx_dg_filter::testFilterBox(const mx_rect& r) const
{
    bool hasIntersect;

    if (flags & MX_DG_FILTER_CHECK_BOX) {
        hasIntersect = filterBox.intersects(r);
    } else {
        hasIntersect = TRUE;
    }

    return hasIntersect;
}
