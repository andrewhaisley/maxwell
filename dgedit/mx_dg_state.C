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
 * MODULE : mx_dg_state.C
 *
 * AUTHOR : David Miller
 *
 * This file
 *
 * DESCRIPTION:
 * Module mx_dg_state.C
 *
 *
 *
 *
 * $id$
 * $log$
 *
 */

#include "mx_dg_editor.h"
#include "mx_dg_state.h"

/*-------------------------------------------------
 * FUNCTION: mx_dg_state::mx_dg_state
 *
 * DESCRIPTION:
 *
 *
 */

mx_dg_state::mx_dg_state(mx_dg_editor* ieditor)
{
    previousState = NULL;
    nextState = NULL;
    clearState();
    editor = ieditor;
    setProcessMask(MX_EVENT_STANDARD | MX_EVENT_MOUSE_MOTION);
}

/*-------------------------------------------------
 * FUNCTION: mx_dg_state::clearState
 *
 * DESCRIPTION:
 *
 *
 */

void mx_dg_state::clearState()
{
    // recursively delete all states in the chain

    delete nextState;
    nextState = NULL;
}

/*-------------------------------------------------
 * FUNCTION: mx_dg_state::~mx_dg_state
 *
 * DESCRIPTION:
 *
 *
 */

mx_dg_state::~mx_dg_state()
{
    clearState();
}

/*-------------------------------------------------
 * FUNCTION: mx_dg_state::textEntry
 *
 * DESCRIPTION:
 *
 *
 */

void mx_dg_state::textEntry(int& err, mx_text_event& event)
{
    err = MX_ERROR_OK;

    // the real state doesnt cope with the text event
    // so we propagate it back to the previous state to see
    // what it wants to do

    // NB this may this lead to this state being deleted

    if (previousState != NULL) {
        previousState->propagatedUp(err, &event, this);
        MX_ERROR_CHECK(err);
    }
abort:;
}

/*-------------------------------------------------
 * FUNCTION: mx_dg_state::buttonPress
 *
 * DESCRIPTION:
 *
 *
 */

void mx_dg_state::buttonPress(int& err, mx_button_event& event)
{
    err = MX_ERROR_OK;

    // the real state doesnt cope with the text event
    // so we propagate it back to the previous state to see
    // what it wants to do

    // NB this may this lead to this state being deleted

    if (previousState != NULL) {
        previousState->buttonPress(err, event);
        MX_ERROR_CHECK(err);
    }
abort:;
}

/*-------------------------------------------------
 * FUNCTION: mx_dg_state::buttonRelease
 *
 * DESCRIPTION:
 *
 *
 */

void mx_dg_state::buttonRelease(int& err, mx_button_event& event)
{
    err = MX_ERROR_OK;

    // the real state doesnt cope with the text event
    // so we propagate it back to the previous state to see
    // what it wants to do

    // NB this may this lead to this state being deleted

    if (previousState != NULL) {
        previousState->buttonRelease(err, event);
        MX_ERROR_CHECK(err);
    }
abort:;
}

/*-------------------------------------------------
 * FUNCTION: mx_dg_state::buttonMotionStart
 *
 * DESCRIPTION:
 *
 *
 */

void mx_dg_state::buttonMotionStart(int& err, mx_button_event& event)
{
    err = MX_ERROR_OK;

    // the real state doesnt cope with the text event
    // so we propagate it back to the previous state to see
    // what it wants to do

    // NB this may this lead to this state being deleted

    if (previousState != NULL) {
        previousState->buttonMotionStart(err, event);
        MX_ERROR_CHECK(err);
    }
abort:;
}

/*-------------------------------------------------
 * FUNCTION: mx_dg_state::buttonMotion
 *
 * DESCRIPTION:
 *
 *
 */

void mx_dg_state::buttonMotion(int& err, mx_button_event& event)
{
    err = MX_ERROR_OK;

    // the real state doesnt cope with the text event
    // so we propagate it back to the previous state to see
    // what it wants to do

    // NB this may this lead to this state being deleted

    if (previousState != NULL) {
        previousState->buttonMotion(err, event);
        MX_ERROR_CHECK(err);
    }
abort:;
}

/*-------------------------------------------------
 * FUNCTION: mx_dg_state::buttonMotion
 *
 * DESCRIPTION:
 *
 *
 */

void mx_dg_state::buttonMotionEnd(int& err, mx_button_event& event)
{
    err = MX_ERROR_OK;

    // the real state doesnt cope with the text event
    // so we propagate it back to the previous state to see
    // what it wants to do

    // NB this may this lead to this state being deleted

    if (previousState != NULL) {
        previousState->buttonMotionEnd(err, event);
        MX_ERROR_CHECK(err);
    }
abort:;
}

/*-------------------------------------------------
 * FUNCTION: mx_dg_state::keyPress
 *
 * DESCRIPTION:
 *
 *
 */

void mx_dg_state::keyPress(int& err, mx_key_event& event)
{
    err = MX_ERROR_OK;

    // the real state doesnt cope with the text event
    // so we propagate it back to the previous state to see
    // what it wants to do

    // NB this may this lead to this state being deleted

    if (previousState != NULL) {
        previousState->keyPress(err, event);
        MX_ERROR_CHECK(err);
    }
abort:;
}

/*-------------------------------------------------
 * FUNCTION: mx_dg_state::ibuttonPress
 *
 * DESCRIPTION:
 *
 *
 */

void mx_dg_state::ibuttonPress(int& err, mx_button_event& event)
{
    mx_dg_state* newState;

    err = MX_ERROR_OK;

    // create a new point state

    newState = editor->pushState(err, MX_DGT_POINT);
    MX_ERROR_CHECK(err);

    // send the event onto the point event
    newState->buttonPress(err, event);
    MX_ERROR_CHECK(err);
abort:;
}

/*-------------------------------------------------
 * FUNCTION: mx_dg_state::ibuttonRelease
 *
 * DESCRIPTION:
 *
 *
 */

void mx_dg_state::ibuttonRelease(int& err, mx_button_event& event)
{
    mx_dg_state* newState;

    err = MX_ERROR_OK;

    // create a new point state

    newState = editor->pushState(err, MX_DGT_POINT);
    MX_ERROR_CHECK(err);

    // send the event onto the point event
    newState->buttonRelease(err, event);
    MX_ERROR_CHECK(err);
abort:;
}

/*-------------------------------------------------
 * FUNCTION: mx_dg_state::buttonMotionStart
 *
 * DESCRIPTION:
 *
 *
 */

void mx_dg_state::ibuttonMotionStart(int& err, mx_button_event& event)
{
    mx_dg_state* newState;

    err = MX_ERROR_OK;

    // create a new point state

    newState = editor->pushState(err, MX_DGT_POINT);
    MX_ERROR_CHECK(err);

    // send the event onto the point event
    newState->buttonMotionStart(err, event);
    MX_ERROR_CHECK(err);
abort:;
}

/*-------------------------------------------------
 * FUNCTION: mx_dg_state::buttonMotion
 *
 * DESCRIPTION:
 *
 *
 */

void mx_dg_state::ibuttonMotion(int& err, mx_button_event& event)
{
    mx_dg_state* newState;

    err = MX_ERROR_OK;

    // create a new point state

    newState = editor->pushState(err, MX_DGT_POINT);
    MX_ERROR_CHECK(err);

    // send the event onto the point event
    newState->buttonMotion(err, event);
    MX_ERROR_CHECK(err);
abort:;
}

/*-------------------------------------------------
 * FUNCTION: mx_dg_state::buttonMotion
 *
 * DESCRIPTION:
 *
 *
 */

void mx_dg_state::ibuttonMotionEnd(int& err, mx_button_event& event)
{
    mx_dg_state* newState;

    err = MX_ERROR_OK;

    // create a new point state

    newState = editor->pushState(err, MX_DGT_POINT);
    MX_ERROR_CHECK(err);

    // send the event onto the point event
    newState->buttonMotionEnd(err, event);
    MX_ERROR_CHECK(err);
abort:;
}

/*-------------------------------------------------
 * FUNCTION: mx_dg_state::endState
 *
 * DESCRIPTION:
 *
 *
 */

void mx_dg_state::endState(int& err,
    mx_event* event)
{
    err = MX_ERROR_OK;

    // pop the state
    editor->popState(err);
    MX_ERROR_CHECK(err);

    // clean up this state - this may do some
    // munging of the data and so is called
    // before the previous state gets the data

    tidyState(err);
    MX_ERROR_CHECK(err);

    // Do anything to make the previous state active
    previousState->setActive(err, event, this);
    MX_ERROR_CHECK(err);

    // let the previous state have this state's data
    // this may delete the previous state
    previousState->getStateData(err, event, this);
    MX_ERROR_CHECK(err);

    // now delete this state
    delete this;
abort:;
}

/*-------------------------------------------------
 * FUNCTION: mx_dg_state::propagatedUp
 *
 * DESCRIPTION:
 *
 *
 */

void mx_dg_state::propagatedUp(int& err,
    mx_event* event,
    mx_dg_state* inState)
{
    err = MX_ERROR_OK;

    // the real state doesnt cope with the text event
    // so we propagate it back to the previous state to see
    // what it wants to do

    // NB this may this lead to this state being deleted

    if (previousState != NULL) {
        previousState->propagatedUp(err, event, this);
        MX_ERROR_CHECK(err);
    }
abort:;
}

/*-------------------------------------------------
 * FUNCTION: mx_dg_state::collapseStates
 *
 * DESCRIPTION:
 *
 *
 */

void mx_dg_state::collapseStates(int& err)
{
    err = MX_ERROR_OK;

    if (nextState != NULL) {
        nextState->collapseStates(err);
        MX_ERROR_CHECK(err);
    }

    // pop the state
    editor->popState(err);
    MX_ERROR_CHECK(err);

    // clean up this state
    tidyState(err);
    MX_ERROR_CHECK(err);

    // now delete this state
    delete this;
abort:;
}

/*-------------------------------------------------
 * FUNCTION: mx_dg_state::getStateData
 *
 * DESCRIPTION:
 *
 *
 */

void mx_dg_state::getStateData(int& err,
    mx_event* event,
    mx_dg_state* inState)
{
    mx_geom* geom;
    bool getGeometry = FALSE;

    err = MX_ERROR_OK;

    switch (inState->getType()) {
    case MX_DGT_POINT:
    case MX_DGT_SPAN:
    case MX_DGT_POLYPOINT:
    case MX_DGT_ARC:
    case MX_DGT_FULL_CIRCLE:
    case MX_DGT_PARTIAL_CIRCLE:
    case MX_DGT_FULL_ELLIPSE:
    case MX_DGT_PARTIAL_ELLIPSE:
    case MX_DGT_RECT:
        getGeometry = TRUE;
        break;
    default:
        break;
    }

    if (getGeometry) {
        inState->returnGeom(err, &geom);
        MX_ERROR_CHECK(err);
        if (geom == NULL)
            return;
    }

    switch (inState->getType()) {
    case MX_DGT_POINT:
        processPoint(err, event, (gmx_point*)geom);
        MX_ERROR_CHECK(err);
        break;
    case MX_DGT_SPAN:
        processSpan(err, event, (gmx_span*)geom);
        MX_ERROR_CHECK(err);
        break;
    case MX_DGT_RECT:
        processRect(err, event, (gmx_rect*)geom);
        MX_ERROR_CHECK(err);
        break;
    case MX_DGT_POLYPOINT:
        processPolypoint(err, event, (gmx_polypoint*)geom);
        MX_ERROR_CHECK(err);
        break;
    case MX_DGT_ARC:
    case MX_DGT_FULL_CIRCLE:
    case MX_DGT_PARTIAL_CIRCLE:
    case MX_DGT_FULL_ELLIPSE:
    case MX_DGT_PARTIAL_ELLIPSE:
        processArc(err, event, (gmx_arc*)geom);
        MX_ERROR_CHECK(err);
        break;
    default:
        return;
    }

abort:;
}
