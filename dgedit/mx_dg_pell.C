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
 * MODULE : mx_dg_pell.C
 *
 * AUTHOR : David Miller
 *
 * This file
 *
 * DESCRIPTION:
 *
 *
 */

#include "mx_dg_state.h"

/*-------------------------------------------------
 * FUNCTION: mx_dg_pellipse_state::mx_dg_pellipse_state
 *
 * DESCRIPTION: Create with segmented arc at first
 * so that highlighted points will draw and undraw OK
 *
 */

mx_dg_pellipse_state::mx_dg_pellipse_state(mx_dg_editor* editor)
    : mx_dg_parc_state(editor)
{
    type = MX_DGT_PARTIAL_ELLIPSE;
}

/*-------------------------------------------------
 * FUNCTION: mx_dg_pellipse_state::pointToAngle
 *
 * DESCRIPTION:
 *
 *
 */

double mx_dg_pellipse_state::pointToAngle(mx_point& p)
{
    mx_point offset = iarc.a.gc();
    offset = p - offset;
    return gatan2(offset.y * iarc.a.gs().x, offset.x * iarc.a.gs().y, 0);
}
