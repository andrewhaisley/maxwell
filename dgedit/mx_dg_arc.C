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
 * MODULE : mx_dg_arc.C
 *
 * AUTHOR : David Miller
 *
 * This file
 *
 * DESCRIPTION:
 * Module mx_dg_arc.C
 *
 *
 */

#include "mx_dg_state.h"
#include "mx_sc_device.h"

/*-------------------------------------------------
 * FUNCTION: mx_dg_arc_state::mx_dg_arc_state
 *
 * DESCRIPTION: Create with segmented arc at first
 * so that highlighted points will draw and undraw OK
 *
 */

mx_dg_arc_state::mx_dg_arc_state(mx_dg_editor* editor)
    : mx_dg_state(editor)
{
    type = MX_DGT_ARC;
    highlightedId = MX_DG_NO_HIGHLIGHT_KEY;
    npoints = 0;
}

/*-------------------------------------------------
 * FUNCTION: mx_dg_span_state::buttonPress
 *
 * DESCRIPTION:
 *
 *
 */

void mx_dg_arc_state::buttonPress(int& err,
    mx_button_event& event)
{
    mx_dg_state* newState = editor->pushState(err, MX_DGT_PARTIAL_ELLIPSE);
    MX_ERROR_CHECK(err);

    newState->buttonPress(err, event);
    MX_ERROR_CHECK(err);
abort:;
}

/*-------------------------------------------------
 * FUNCTION: mx_dg_arc_state::buttonMotion
 *
 * DESCRIPTION:
 *
 *
 */

void mx_dg_arc_state::buttonMotion(int& err,
    mx_button_event& event)
{
    ibuttonMotion(err, event);
}

/*-------------------------------------------------
 * FUNCTION: mx_dg_arc_state::buttonMotionEnd
 *
 * DESCRIPTION:
 *
 *
 */

void mx_dg_arc_state::buttonMotionEnd(int& err,
    mx_button_event& event)
{
    ibuttonMotionEnd(err, event);
}

/*-------------------------------------------------
 * FUNCTION: mx_dg_arc_state::returnGeom
 *
 * DESCRIPTION:
 *
 *
 */

void mx_dg_arc_state::returnGeom(int& err,
    mx_geom** geom)
{
    err = MX_ERROR_OK;

    *geom = NULL;

    //  if(!completed) return ;

    *geom = &iarc;

    return;
}

/*-------------------------------------------------
 * FUNCTION: mx_dg_arc_state::tidyState
 *
 * DESCRIPTION:
 *
 *
 */

void mx_dg_arc_state::tidyState(int& err)
{
    mx_screen_device* dev = (mx_screen_device*)editor->getDevice();

    err = MX_ERROR_OK;

    // undraw the whole arc
    editor->drawHighlighted(err, highlightedId);
    MX_ERROR_CHECK(err);

    editor->delHighlighted(err, highlightedId);
    MX_ERROR_CHECK(err);

    highlightedId = MX_DG_NO_HIGHLIGHT_KEY;

    dev->set_cursor(MX_NO_CURSOR);

abort:;
}

/*-------------------------------------------------
 * FUNCTION: mx_dg_arc_state::processArc
 *
 * DESCRIPTION:
 *
 *
 */

void mx_dg_arc_state::processArc(int& err,
    mx_event* event,
    gmx_arc* iiarc)
{
    err = MX_ERROR_OK;

    iarc = *iiarc;

    endState(err, event);
    MX_ERROR_CHECK(err);

abort:;
}
