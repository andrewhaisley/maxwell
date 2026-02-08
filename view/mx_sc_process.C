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
 * MODULE : mx_sc_process.C
 *
 * AUTHOR : David Miller
 *
 * This file
 *
 * DESCRIPTION: Handling of events created by the screen device
 * Module mx_sc_process.C
 *
 *
 *
 */

#include "mx_sc_device.h"

/*-------------------------------------------------
 * FUNCTION: mx_screen_device::processPreDraw
 *
 * DESCRIPTION:
 *
 *
 */

void mx_screen_device::processPreDraw(int& err,
    mx_draw_event& drawEvent)
{
    err = MX_ERROR_OK;

    if (!testProcessMask(MX_EVENT_PREDRAW))
        return;

    drawEvent.setType(MX_EVENT_PREDRAW);

    preDraw(err, drawEvent);
    MX_ERROR_CHECK(err);

    return;
abort:
    return;
}

/*-------------------------------------------------
 * FUNCTION: mx_screen_device::processSpecialDraw
 *
 * DESCRIPTION:
 *
 *
 */

void mx_screen_device::processSpecialDraw(int& err,
    mx_draw_event& drawEvent)
{
    err = MX_ERROR_OK;

    if (!testProcessMask(MX_EVENT_SPECIAL_DRAW))
        return;

    drawEvent.setType(MX_EVENT_SPECIAL_DRAW);

    specialDraw(err, drawEvent);
    MX_ERROR_CHECK(err);

    return;
abort:
    return;
}

/*-------------------------------------------------
 * FUNCTION: mx_screen_device::processDraw
 *
 * DESCRIPTION:
 *
 *
 */

void mx_screen_device::processDraw(int& err,
    mx_draw_event& drawEvent)
{
    err = MX_ERROR_OK;

    if (!testProcessMask(MX_EVENT_DRAW))
        return;

    drawEvent.setType(MX_EVENT_DRAW);

    // now propagate
    mx_callback_target::draw(err, drawEvent);
    MX_ERROR_CHECK(err);

    return;
abort:
    return;
}

/*-------------------------------------------------
 * FUNCTION: mx_screen_device::processPostDraw
 *
 * DESCRIPTION:
 *
 *
 */

void mx_screen_device::processPostDraw(int& err,
    mx_draw_event& drawEvent)
{
    err = MX_ERROR_OK;

    if (!testProcessMask(MX_EVENT_POSTDRAW))
        return;

    drawEvent.setType(MX_EVENT_POSTDRAW);

    postDraw(err, drawEvent);
    MX_ERROR_CHECK(err);

    return;
abort:
    return;
}
