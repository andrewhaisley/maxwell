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
 * MODULE : mx_dg_rect.C
 *
 * AUTHOR : David Miller
 *
 * This file
 *
 * DESCRIPTION:
 * Module mx_dg_rect.C
 *
 *
 * Revision 1.3  1997/07/11 21:31:53  dave
 * Improvements
 *
 * Revision 1.2  1997/07/04 16:31:34  dave
 * Improvements for grids etc
 *
 * Revision 1.1  1997/06/27 16:41:31  dave
 * Initial revision
 *
 * Revision 1.1  1996/10/06 13:15:45  andrew
 * Initial revision
 *
 *
 */

#include "mx_dg_state.h"
#include "mx_sc_device.h"

/*-------------------------------------------------
 * FUNCTION: mx_dg_rect_state::mx_dg_rect_state
 *
 * DESCRIPTION:
 *
 *
 */

mx_dg_rect_state::mx_dg_rect_state(mx_dg_editor* editor)
    : mx_dg_state(editor)
{
    type = MX_DGT_RECT;
    npoints = 0;
    irect.r.isArea = FALSE;
    highlightedId = MX_DG_NO_HIGHLIGHT_KEY;
}

/*-------------------------------------------------
 * FUNCTION: mx_dg_rect_state::buttonPress
 *
 * DESCRIPTION:
 *
 *
 */

void mx_dg_rect_state::buttonPress(int& err,
    mx_button_event& event)
{
    ibuttonPress(err, event);
}

/*-------------------------------------------------
 * FUNCTION: mx_dg_rect_state::buttonMotion
 *
 * DESCRIPTION:
 *
 *
 */

void mx_dg_rect_state::buttonMotion(int& err,
    mx_button_event& event)
{
    err = MX_ERROR_OK;
    if (npoints != 1)
        return;
    ibuttonMotion(err, event);
}

/*-------------------------------------------------
 * FUNCTION: mx_dg_rect_state::buttonMotionEnd
 *
 * DESCRIPTION:
 *
 *
 */

void mx_dg_rect_state::buttonMotionEnd(int& err,
    mx_button_event& event)
{
    err = MX_ERROR_OK;
    if (npoints != 1)
        return;
    ibuttonMotionEnd(err, event);
}

/*-------------------------------------------------
 * FUNCTION: mx_dg_rect_state::returnGeom
 *
 * DESCRIPTION:
 *
 *
 */

void mx_dg_rect_state::returnGeom(int& err,
    mx_geom** geom)
{
    err = MX_ERROR_OK;

    *geom = NULL;

    // save the OK value - the object may be deleted
    if (npoints != 2)
        return;

    *geom = &irect;

    return;
}

/*-------------------------------------------------
 * FUNCTION: mx_dg_rect_state::processPoint
 *
 * DESCRIPTION:
 *
 *
 */

void mx_dg_rect_state::processPoint(int& err,
    mx_event* event,
    gmx_point* pf)
{
    mx_screen_device* dev = (mx_screen_device*)editor->getDevice();
    bool addPoint, undrawRect, drawRect;

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
            irect.r.xb = pf->p.x;
            irect.r.yb = pf->p.y;
            undrawRect = FALSE;
            drawRect = FALSE;
            npoints = 1;
        } else {
            npoints = 2;
            drawRect = (irect.r.end() != pf->p);
            undrawRect = drawRect;
        }
    } else {
        if (highlightedId == 0) {
            highlightedId = editor->addHighlighted(err,
                &irect);
            MX_ERROR_CHECK(err);

            undrawRect = FALSE;
            drawRect = TRUE;
        } else {
            drawRect = (irect.r.end() != pf->p);
            undrawRect = drawRect;
        }
    }

    if (undrawRect) {
        // undraw anything present
        editor->drawHighlighted(err, highlightedId);
        MX_ERROR_CHECK(err);
    }

    // set the end point
    irect.r.xt = pf->p.x;
    irect.r.yt = pf->p.y;

    if (drawRect) {
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
 * FUNCTION: mx_dg_rect_state::tidyState
 *
 * DESCRIPTION:
 *
 *
 */

void mx_dg_rect_state::tidyState(int& err)
{
    mx_screen_device* dev = (mx_screen_device*)editor->getDevice();

    err = MX_ERROR_OK;

    editor->drawHighlighted(err, highlightedId);
    MX_ERROR_CHECK(err);

    editor->delHighlighted(err, highlightedId);
    MX_ERROR_CHECK(err);

    highlightedId = MX_DG_NO_HIGHLIGHT_KEY;

    irect.r.normalise();

    dev->set_cursor(MX_NO_CURSOR);
abort:;
}
