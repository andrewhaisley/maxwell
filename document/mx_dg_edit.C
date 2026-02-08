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
 * MODULE : mx_dg_edit.C
 *
 * AUTHOR : David Miller
 *
 * This file
 *
 * DESCRIPTION:
 * Module mx_dg_edit.C
 *
 *
 * Revision 1.1  1996/10/06 13:15:45  andrew
 * Initial revision
 *
 *
 */

#include "mx_dg_area.h"

void mx_diagram_area::addNewGeometry(int& err,
    const mx_dg_quad_geom& quadGeom)
{
    mx_dg_level_key levelKey = quadGeom.getLevelKey();
    mx_dg_quad_key quadKey = quadGeom.getQuadKey();
    mx_dg_group_key groupKey = quadGeom.getGroupKey();

    // we are adding a totally new level
    mx_dg_level* level = getLevel(levelKey);
    mx_dg_quad* quad = getQuad(levelKey,
        quadKey);

    // add the new element to the level provided it is
    // a real new geometry

    if (quadGeom.needsLevel()) {
        level->addNewGeometry(err, levelKey, quadKey);
        MX_ERROR_CHECK(err);
    }

    // now we need to add to the quad

    quad->addNewGeometry(err, quadGeom);
    MX_ERROR_CHECK(err);

abort:;
}
