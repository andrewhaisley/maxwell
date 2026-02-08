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
 * MODULE : mx_dg_span.C
 *
 * AUTHOR : David Miller
 *
 * This file
 *
 * DESCRIPTION:
 * Module mx_dg_span.C
 *
 *
 * Revision 1.1  1996/10/06 13:15:45  andrew
 * Initial revision
 *
 *
 */

#include "mx_dg_state.h"
#include "mx_sc_device.h"

/*-------------------------------------------------
 * FUNCTION: mx_dg_span_state::mx_dg_span_state
 *
 * DESCRIPTION:
 *
 *
 */

mx_dg_span_state::mx_dg_span_state(mx_dg_editor* editor)
    : mx_dg_state(editor)
{
    type = MX_DGT_SPAN;
    npoints = 0;
    highlightedId = MX_DG_NO_HIGHLIGHT_KEY;
}

/*-------------------------------------------------
 * FUNCTION: mx_dg_span_state::buttonPress
 *
 * DESCRIPTION:
 *
 *
 */

void mx_dg_span_state::buttonPress(int& err,
    mx_button_event& event)
{
    ibuttonPress(err, event);
}

/*-------------------------------------------------
 * FUNCTION: mx_dg_span_state::buttonMotion
 *
 * DESCRIPTION:
 *
 *
 */

void mx_dg_span_state::buttonMotion(int& err,
    mx_button_event& event)
{
    err = MX_ERROR_OK;
    if (npoints != 1)
        return;
    ibuttonMotion(err, event);
}

/*-------------------------------------------------
 * FUNCTION: mx_dg_span_state::buttonMotionEnd
 *
 * DESCRIPTION:
 *
 *
 */

void mx_dg_span_state::buttonMotionEnd(int& err,
    mx_button_event& event)
{
    err = MX_ERROR_OK;
    if (npoints != 1)
        return;
    ibuttonMotionEnd(err, event);
}

/*-------------------------------------------------
 * FUNCTION: mx_dg_span_state::returnGeom
 *
 * DESCRIPTION:
 *
 *
 */

void mx_dg_span_state::returnGeom(int& err,
    mx_geom** geom)
{
    err = MX_ERROR_OK;

    *geom = NULL;

    // save the OK value - the object may be deleted
    if (npoints != 2)
        return;

    *geom = &ispan;

    return;
}

/*-------------------------------------------------
 * FUNCTION: mx_dg_span_state::processPoint
 *
 * DESCRIPTION:
 *
 *
 */

void mx_dg_span_state::processPoint(int& err,
    mx_event* event,
    gmx_point* pf)
{
    mx_screen_device* dev = (mx_screen_device*)editor->getDevice();
    bool addPoint, undrawSpan, drawSpan;

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
            ispan.s.xb = pf->p.x;
            ispan.s.yb = pf->p.y;
            undrawSpan = FALSE;
            drawSpan = FALSE;
            npoints = 1;
        } else {
            npoints = 2;
            drawSpan = (ispan.s.end() != pf->p);
            undrawSpan = drawSpan;
        }
    } else {
        if (highlightedId == 0) {
            highlightedId = editor->addHighlighted(err,
                &ispan);
            MX_ERROR_CHECK(err);

            undrawSpan = FALSE;
            drawSpan = TRUE;
        } else {
            drawSpan = (ispan.s.end() != pf->p);
            undrawSpan = drawSpan;
        }
    }

    if (undrawSpan) {
        // undraw anything present
        editor->drawHighlighted(err, highlightedId);
        MX_ERROR_CHECK(err);
    }

    // set the end point
    ispan.s.xt = pf->p.x;
    ispan.s.yt = pf->p.y;

    if (drawSpan) {
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
 * FUNCTION: mx_dg_span_state::tidyState
 *
 * DESCRIPTION:
 *
 *
 */

void mx_dg_span_state::tidyState(int& err)
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
