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
 * MODULE : mx_dg_snap.C
 *
 * AUTHOR : David Miller
 *
 * This file
 *
 * DESCRIPTION:
 * Module mx_dg_snap.C
 *
 *
 */

#include "mx_dg_state.h"
#include "mx_sc_device.h"

/*-------------------------------------------------
 * FUNCTION: mx_dg_snap_state::mx_dg_snap_state
 *
 * DESCRIPTION:
 *
 *
 */

mx_dg_snap_state::mx_dg_snap_state(mx_dg_editor* editor)
    : mx_dg_state(editor)
{
    type = MX_DGT_SNAP;
    npoints = 0;
    highlightedId = MX_DG_NO_HIGHLIGHT_KEY;
}

/*-------------------------------------------------
 * FUNCTION: mx_dg_snap_state::buttonPress
 *
 * DESCRIPTION:
 *
 *
 */

void mx_dg_snap_state::buttonPress(int& err,
    mx_button_event& event)
{
    ibuttonPress(err, event);
}

/*-------------------------------------------------
 * FUNCTION: mx_dg_snap_state::buttonMotion
 *
 * DESCRIPTION:
 *
 *
 */

void mx_dg_snap_state::buttonMotion(int& err,
    mx_button_event& event)
{
    err = MX_ERROR_OK;
    if (npoints != 1)
        return;
    ibuttonMotion(err, event);
}

/*-------------------------------------------------
 * FUNCTION: mx_dg_snap_state::buttonMotionEnd
 *
 * DESCRIPTION:
 *
 *
 */

void mx_dg_snap_state::buttonMotionEnd(int& err,
    mx_button_event& event)
{
    err = MX_ERROR_OK;
    if (npoints != 1)
        return;
    ibuttonMotionEnd(err, event);
}

/*-------------------------------------------------
 * FUNCTION: mx_dg_snap_state::processPoint
 *
 * DESCRIPTION:
 *
 *
 */

void mx_dg_snap_state::processPoint(int& err,
    mx_event* event,
    gmx_point* pf)
{
    mx_screen_device* dev = (mx_screen_device*)editor->getDevice();
    bool addPoint, undrawSnap, drawSnap;
    mx_point snapPoint;
    mx_geom* lastGeometry = editor->getLastGeometry();

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
            snapPoint = pf->p;

            undrawSnap = FALSE;
            drawSnap = FALSE;
            npoints = 1;
        } else {
            // the point to add is the snap too point on
            // the last geometry

            if (lastGeometry != NULL) {
                mx_snap_info si;

                lastGeometry->dist2snap(pf->p, si);

                snapPoint = si.getPoint();
            }

            npoints = 2;

            drawSnap = TRUE;
            undrawSnap = drawSnap;
        }
    } else {
        if (lastGeometry != NULL) {
            mx_snap_info si;

            lastGeometry->dist2snap(pf->p, si);

            snapPoint = si.getPoint();
        }

        if (highlightedId == 0) {
            highlightedId = editor->addHighlighted(err,
                &ispan);
            MX_ERROR_CHECK(err);

            undrawSnap = FALSE;
            drawSnap = TRUE;
        } else {
            drawSnap = (ispan.s.end() != pf->p);
            undrawSnap = drawSnap;
        }
    }

    if (undrawSnap) {
        // undraw anything present
        editor->drawHighlighted(err, highlightedId);
        MX_ERROR_CHECK(err);
    }

    // set the end point
    ispan.s.xb = pf->p.x;
    ispan.s.yb = pf->p.y;
    ispan.s.xt = snapPoint.x;
    ispan.s.yt = snapPoint.y;

    if (drawSnap) {
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

/*-------------------------------------------------
 * FUNCTION: mx_dg_snap_state::tidyState
 *
 * DESCRIPTION:
 *
 *
 */

void mx_dg_snap_state::tidyState(int& err)
{
    mx_screen_device* dev = (mx_screen_device*)editor->getDevice();

    err = MX_ERROR_OK;

    editor->drawHighlighted(err, highlightedId);
    MX_ERROR_CHECK(err);

    editor->delHighlighted(err, highlightedId);
    MX_ERROR_CHECK(err);

    highlightedId = MX_DG_NO_HIGHLIGHT_KEY;

    dev->set_cursor(MX_NO_CURSOR);
abort:;
}
