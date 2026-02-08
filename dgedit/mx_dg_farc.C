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
 * MODULE : mx_dg_farc.C
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
#include "mx_sc_device.h"

/*-------------------------------------------------
 * FUNCTION: mx_dg_farc_state::mx_dg_farc_state
 *
 * DESCRIPTION: Create with segmented arc at first
 * so that highlighted points will draw and undraw OK
 *
 */

mx_dg_farc_state::mx_dg_farc_state(mx_dg_editor* editor)
    : mx_dg_arc_state(editor)
{
    iarc.a.ssa(0);
    iarc.a.sea(PITWO);
    boxState = TRUE;
}

/*-------------------------------------------------
 * FUNCTION: mx_dg_farc_state::buttonPress
 *
 * DESCRIPTION:
 *
 *
 */

void mx_dg_farc_state::buttonPress(int& err,
    mx_button_event& event)
{
    ibuttonPress(err, event);
}

/*-------------------------------------------------
 * FUNCTION: mx_dg_farc_state::processPoint
 *
 * DESCRIPTION:
 *
 *
 */

void mx_dg_farc_state::processPoint(int& err,
    mx_event* event,
    gmx_point* pf)
{
    mx_screen_device* dev = (mx_screen_device*)editor->getDevice();
    bool addPoint, drawArc, undrawArc;
    mx_point newSize;

    switch (event->getType()) {
    case MX_EVENT_BUTTON_PRESS:
        npoints = 0;
        addPoint = TRUE;
        break;
    case MX_EVENT_BUTTON_MOTION:
        dev->set_cursor(MX_CROSSHAIR_CURSOR);
        addPoint = FALSE;
        break;
    case MX_EVENT_BUTTON_MOTION_END:
        addPoint = TRUE;
        break;
    default:
        return;
        break;
    }

    // add highlight on first button motion

    if (addPoint) {
        if (npoints == 0) {
            iarc.a.sc(pf->p);
            undrawArc = FALSE;
            drawArc = FALSE;
            npoints = 1;
        } else {
            npoints = 2;

            newSize = pointToSize(pf->p);
            undrawArc = (iarc.a.gs() != newSize);
            drawArc = undrawArc;
        }
    } else {
        if (highlightedId == 0) {
            highlightedId = editor->addHighlighted(err,
                &iarc);
            MX_ERROR_CHECK(err);

            undrawArc = FALSE;
            drawArc = TRUE;
        } else {
            newSize = pointToSize(pf->p);
            undrawArc = (iarc.a.gs() != newSize);
            drawArc = undrawArc;
        }
    }

    if (undrawArc) {
        // undraw anything present
        editor->drawHighlighted(err, highlightedId);
        MX_ERROR_CHECK(err);
    }

    // set the end point
    iarc.a.ss(newSize);

    if (drawArc) {
        // undraw anything present
        editor->drawHighlighted(err, highlightedId);
        MX_ERROR_CHECK(err);
    }

    if (npoints == 2) {
        endState(err, event);
        MX_ERROR_CHECK(err);
    }
abort:;
}
