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
 * MODULE : mx_dg_point.C
 *
 * AUTHOR : David Miller
 *
 * This file
 *
 * DESCRIPTION:
 * Module mx_dg_point.C
 *
 *
 * Revision 1.1  1996/10/06 13:15:45  andrew
 * Initial revision
 *
 *
 */

#include "mx_dg_state.h"

/*-------------------------------------------------
 * FUNCTION: mx_dg_point_state::mx_dg_point_state
 *
 * DESCRIPTION:
 *
 *
 */

mx_dg_point_state::mx_dg_point_state(mx_dg_editor* editor)
    : mx_dg_state(editor)
{
    type = MX_DGT_POINT;
    pointFound = FALSE;
}

/*-------------------------------------------------
 * FUNCTION: mx_dg_point_state::buttonAction
 *
 * DESCRIPTION:
 *
 *
 */

void mx_dg_point_state::buttonAction(int& err,
    mx_button_event& event)
{
    err = MX_ERROR_OK;

    pointFound = TRUE;

    p.p = event.getCoord().p;

    // got the point
    // this may delete this state

    endState(err, &event);
    MX_ERROR_CHECK(err);

abort:;
}

/*-------------------------------------------------
 * FUNCTION: mx_dg_point_state::getGeom
 *
 * DESCRIPTION: Return the found point
 *
 *
 */

void mx_dg_point_state::returnGeom(int& err,
    mx_geom** geom)
{
    err = MX_ERROR_OK;

    if (!pointFound)
        return;

    // snap to grid if required
    editor->getGrid()->snapPoint(p.p);

    *geom = &p;

    return;
}
