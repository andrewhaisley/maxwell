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
 * MODULE : mx_dg_parc.C
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

mx_dg_parc_state::mx_dg_parc_state(mx_dg_editor* editor)
    : mx_dg_arc_state(editor)
{
}

/*-------------------------------------------------
 * FUNCTION: mx_dg_parc_state::buttonPress
 *
 * DESCRIPTION:
 *
 *
 */

void mx_dg_parc_state::buttonPress(int& err,
    mx_button_event& event)
{
    if (npoints == 0) {
        // get the full arc first

        MX_DGT_STATE nextType = (type == MX_DGT_PARTIAL_CIRCLE) ? MX_DGT_FULL_CIRCLE : MX_DGT_FULL_ELLIPSE;

        nextState = editor->pushState(err, nextType);
        MX_ERROR_CHECK(err);

        // send on the button press
        nextState->buttonPress(err, event);
        MX_ERROR_CHECK(err);
    } else {
        // treat the point here
        ibuttonPress(err, event);
        MX_ERROR_CHECK(err);
    }
abort:;
}

/*-------------------------------------------------
 * FUNCTION: mx_dg_parc_state::processArc
 *
 * DESCRIPTION:
 *
 *
 */

void mx_dg_parc_state::processArc(int& err,
    mx_event* event,
    gmx_arc* iiarc)
{
    // got the full arc
    npoints = 1;
    iarc = *iiarc;

    // redraw the arc to start off

    highlightedId = editor->addHighlighted(err,
        &iarc);
    MX_ERROR_CHECK(err);

    // draw anything present
    editor->drawHighlighted(err, highlightedId);
    MX_ERROR_CHECK(err);

abort:;
}

/*-------------------------------------------------
 * FUNCTION: mx_dg_parc_state::processPoint
 *
 * DESCRIPTION:
 *
 *
 */

void mx_dg_parc_state::processPoint(int& err,
    mx_event* event,
    gmx_point* pf)
{
    mx_screen_device* dev = (mx_screen_device*)editor->getDevice();
    bool addPoint;
    double endAng = 0.0;

    switch (event->getType()) {
    case MX_EVENT_BUTTON_PRESS:
        // get the start angle
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
        if (npoints == 1) {
            startAng = pointToAngle(pf->p);
            endAng = iarc.a.gea();
            npoints = 2;
        } else if (npoints == 2) {
            npoints = 3;

            endAng = pointToAngle(pf->p);
            if (endAng < startAng)
                endAng += PITWO;
        }
    } else {
        endAng = pointToAngle(pf->p);
        if (endAng < startAng)
            endAng += PITWO;
    }

    if ((startAng != iarc.a.gsa()) || (endAng != iarc.a.gea())) {
        // undraw anything present
        editor->drawHighlighted(err, highlightedId);
        MX_ERROR_CHECK(err);

        iarc.a.ssa(startAng);
        iarc.a.sea(endAng);

        // draw anything present
        editor->drawHighlighted(err, highlightedId);
        MX_ERROR_CHECK(err);
    }

    if (npoints == 3) {
        endState(err, event);
        MX_ERROR_CHECK(err);
    }

abort:;
}
