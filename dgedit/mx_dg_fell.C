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
 * MODULE : mx_dg_fell.C
 *
 * AUTHOR : David Miller
 *
 * This file
 *
 * DESCRIPTION:
 * Module mx_dg_fell.C
 *
 *
 *
 */

#include "mx_dg_state.h"
#include "mx_sc_device.h"

/*-------------------------------------------------
 * FUNCTION: mx_dg_fellipse_state::mx_dg_fellipse_state
 *
 * DESCRIPTION: Create with segmented arc at first
 * so that highlighted points will draw and undraw OK
 *
 */

mx_dg_fellipse_state::mx_dg_fellipse_state(mx_dg_editor* editor)
    : mx_dg_farc_state(editor)
{
    type = MX_DGT_FULL_ELLIPSE;
}

/*-------------------------------------------------
 * FUNCTION: mx_dg_fellipse_state::pointToSize
 *
 * DESCRIPTION:
 *
 *
 */

mx_point mx_dg_fellipse_state::pointToSize(mx_point& p)
{
    mx_point offset = iarc.a.gc();
    offset = p - offset;

    if (boxState) {
        offset.x = GABS(offset.x);
        offset.y = GABS(offset.y);
    } else {
        offset.x = GABS(offset.x) * M_SQRT2;
        offset.y = GABS(offset.y) * M_SQRT2;
    }

    return offset;
}
