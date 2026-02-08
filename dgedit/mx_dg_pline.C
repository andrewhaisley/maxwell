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
 * MODULE : mx_dg_pline.C
 *
 * AUTHOR : David Miller
 *
 * This file
 *
 * DESCRIPTION:
 * Module mx_dg_pline.C
 *
 *
 */

#include "mx_dg_state.h"
#include "mx_sc_device.h"

/*-------------------------------------------------
 * FUNCTION: mx_dg_polypoint_state::mx_dg_polypoint_state
 *
 * DESCRIPTION: Create with segmented polypoint at first
 * so that highlighted points will draw and undraw OK
 *
 */

mx_dg_polypoint_state::mx_dg_polypoint_state(mx_dg_editor* editor)
    : mx_dg_state(editor)
{
    type = MX_DGT_POLYPOINT;
    ipp.pl.set_type(POLY_CSEGMENT);
    ipp.pl.set_allow_coincident(FALSE);
    highlightedId = MX_DG_NO_HIGHLIGHT_KEY;
    spanHighlightedId = MX_DG_NO_HIGHLIGHT_KEY;
}

/*-------------------------------------------------
 * FUNCTION: mx_dg_span_state::buttonPress
 *
 * DESCRIPTION:
 *
 *
 */

void mx_dg_polypoint_state::buttonPress(int& err,
    mx_button_event& event)
{
    ibuttonPress(err, event);
}

/*-------------------------------------------------
 * FUNCTION: mx_dg_span_state::buttonDoublePress
 *
 * DESCRIPTION:
 *
 *
 */

void mx_dg_polypoint_state::buttonDoublePress(int& err,
    mx_button_event& event)
{
    ibuttonPress(err, event);
}

/*-------------------------------------------------
 * FUNCTION: mx_dg_polypoint_state::buttonMotion
 *
 * DESCRIPTION:
 *
 *
 */

void mx_dg_polypoint_state::buttonMotion(int& err,
    mx_button_event& event)
{
    err = MX_ERROR_OK;
    if (ipp.pl.get_num_points() == 0)
        return;
    ibuttonMotion(err, event);
}

/*-------------------------------------------------
 * FUNCTION: mx_dg_polypoint_state::buttonMotionEnd
 *
 * DESCRIPTION:
 *
 *
 */

void mx_dg_polypoint_state::buttonMotionEnd(int& err,
    mx_button_event& event)
{
    err = MX_ERROR_OK;
    if (ipp.pl.get_num_points() == 0)
        return;
    ibuttonMotionEnd(err, event);
}

/*-------------------------------------------------
 * FUNCTION: mx_dg_polypoint_state::returnGeom
 *
 * DESCRIPTION:
 *
 *
 */

void mx_dg_polypoint_state::returnGeom(int& err,
    mx_geom** geom)
{
    err = MX_ERROR_OK;

    *geom = NULL;

    if (ipp.pl.get_num_points() == 0)
        return;

    *geom = &ipp;

    return;
}

/*-------------------------------------------------
 * FUNCTION: mx_dg_polypoint_state::keyPress
 *
 * DESCRIPTION:
 *
 *
 */

void mx_dg_polypoint_state::keyPress(int& err, mx_key_event& event)
{
    err = MX_ERROR_OK;

    if (strcmp(event.getKeyName(), "Escape") == 0) {
        mx_dg_state::keyPress(err, event);
        MX_ERROR_CHECK(err);
    } else {
        endState(err, &event);
        MX_ERROR_CHECK(err);
    }
abort:;
}

/*-------------------------------------------------
 * FUNCTION: mx_dg_polypoint_state::processPoint
 *
 * DESCRIPTION:
 *
 *
 */

void mx_dg_polypoint_state::processPoint(int& err,
    mx_event* event,
    gmx_point* pf)
{
    bool addPoint = false, undrawSpan = false, drawSpan = false;
    mx_screen_device* dev = (mx_screen_device*)editor->getDevice();
    mx_point startPoint;
    int npoints = ipp.pl.get_num_points();

    switch (event->getType()) {
    case MX_EVENT_DOUBLE_BUTTON_PRESS:

        // the end of the polypoint

        endState(err, event);
        MX_ERROR_CHECK(err);
        return;

        break;

    case MX_EVENT_BUTTON_PRESS:

        addPoint = TRUE;

        if (npoints == 0) {
            editor->setMouseMotion(err, TRUE);
            MX_ERROR_CHECK(err);

            highlightedId = editor->addHighlighted(err,
                &ipp);
            MX_ERROR_CHECK(err);

            undrawSpan = FALSE;
            drawSpan = FALSE;
        } else {
            undrawSpan = TRUE;
            drawSpan = TRUE;
        }

        break;
    case MX_EVENT_BUTTON_MOTION:
        addPoint = TRUE;
        undrawSpan = FALSE;
        drawSpan = TRUE;
        dev->set_cursor(MX_PENCIL_CURSOR);
        break;
    case MX_EVENT_MOUSE_MOTION:
        addPoint = FALSE;
        undrawSpan = TRUE;
        drawSpan = TRUE;
        dev->set_cursor(MX_CROSSHAIR_CURSOR);
        break;
    case MX_EVENT_BUTTON_MOTION_END:
        addPoint = TRUE;
        undrawSpan = FALSE;
        drawSpan = TRUE;
        break;
    default:
        break;
    }

    if (npoints > 0)
        startPoint = ipp.pl[npoints - 1];

    // add the point - might abort due to duplicates
    if (addPoint && (!ipp.pl.addPoint(pf->p)))
        return;

    if (undrawSpan) {
        // undraw the present span
        editor->drawHighlighted(err, spanHighlightedId);
        MX_ERROR_CHECK(err);
    }

    // get the highlighted id
    if (drawSpan) {
        if (spanHighlightedId == 0) {
            spanHighlightedId = editor->addHighlighted(err,
                &tempSpan);
            MX_ERROR_CHECK(err);
        }

        tempSpan.s.xb = startPoint.x;
        tempSpan.s.yb = startPoint.y;
        tempSpan.s.xt = pf->p.x;
        tempSpan.s.yt = pf->p.y;

        // draw the present span
        editor->drawHighlighted(err, spanHighlightedId);
        MX_ERROR_CHECK(err);

        if (addPoint) {
            editor->delHighlighted(err, spanHighlightedId);
            MX_ERROR_CHECK(err);

            spanHighlightedId = MX_DG_NO_HIGHLIGHT_KEY;
        }
    }
abort:;
}

/*-------------------------------------------------
 * FUNCTION: mx_dg_polypoint_state::tidyState
 *
 * DESCRIPTION:
 *
 *
 */

void mx_dg_polypoint_state::tidyState(int& err)
{
    mx_screen_device* dev = (mx_screen_device*)editor->getDevice();

    err = MX_ERROR_OK;

    // undraw the whole polypoint
    editor->drawHighlighted(err, highlightedId);
    MX_ERROR_CHECK(err);

    // delete the polypoint from the display list
    editor->delHighlighted(err, highlightedId);
    MX_ERROR_CHECK(err);

    highlightedId = MX_DG_NO_HIGHLIGHT_KEY;

    // undraw the span
    editor->drawHighlighted(err, spanHighlightedId);
    MX_ERROR_CHECK(err);

    // delete the tmeporary span from the display list
    editor->delHighlighted(err, spanHighlightedId);
    MX_ERROR_CHECK(err);

    spanHighlightedId = MX_DG_NO_HIGHLIGHT_KEY;

    editor->setMouseMotion(err, FALSE);
    MX_ERROR_CHECK(err);

    dev->set_cursor(MX_NO_CURSOR);

    // reset the type of the polypoint
    ipp.pl.set_type(POLY_LINE);
abort:;
}
