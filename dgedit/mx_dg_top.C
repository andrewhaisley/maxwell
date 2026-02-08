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
 * MODULE : mx_dg_top.C
 *
 * AUTHOR : David Miller
 *
 * This file
 *
 * DESCRIPTION:
 * Module mx_dg_top.C
 *
 *
 * Revision 1.1  1996/10/06 13:15:45  andrew
 * Initial revision
 *
 *
 */

#include "mx_dg_state.h"

/*-------------------------------------------------
 * FUNCTION: mx_dg_top_state::mx_dg_top_state
 *
 * DESCRIPTION:
 *
 *
 */

mx_dg_top_state::mx_dg_top_state(mx_dg_editor* editor)
    : mx_dg_state(editor)
{
    type = MX_DGT_TOP;
}

#ifdef ffdsafsfdsaffdfa

/*-------------------------------------------------
 * FUNCTION: mx_dg_top_state::buttonPress
 *
 * DESCRIPTION:
 *
 *
 */

void mx_dg_top_state::buttonPress(int& err,
    mx_button_event& event)
{
    err = MX_ERROR_OK;

    // if a second button press go into select mode
    if (event->getButtonEvent() == 2) {
        mx_dg_state* newState = editor->pushState(err, MX_DGT_SELECT);
        MX_ERROR_CHECK(err);

        // and now pass the button press on
        newState->buttonPress(err, event);
        MX_ERROR_CHECK(err);
    }
abort:;
}

#endif

/*-------------------------------------------------
 * FUNCTION: mx_dg_top_state::mx_dg_top_state
 *
 * DESCRIPTION:
 *
 *
 */

void mx_dg_top_state::textEntry(int& err, mx_text_event& event)
{
    MX_DGT_STATE nextType = MX_DGT_NONE;

    if (strcmp(event.getText(), MX_DG_SPAN) == 0) {
        nextType = MX_DGT_SPAN;
    } else if (strcmp(event.getText(), MX_DG_RECT) == 0) {
        nextType = MX_DGT_RECT;
    } else if (strcmp(event.getText(), MX_DG_PLINE) == 0) {
        nextType = MX_DGT_POLYPOINT;
    } else if (strcmp(event.getText(), MX_DG_ARC) == 0) {
        nextType = MX_DGT_FULL_ELLIPSE;
    } else if (strcmp(event.getText(), MX_DG_SNAP) == 0) {
        nextType = MX_DGT_SNAP;
    } else if (strcmp(event.getText(), MX_DG_SELECT) == 0) {
        nextType = MX_DGT_SELECT;
    } else if (strcmp(event.getText(), MX_DG_GRID) == 0) {
        editor->toggleGrid(err);
        MX_ERROR_CHECK(err);
    } else if (strcmp(event.getText(), MX_DG_UNDO) == 0) {
        editor->undo(err);
        MX_ERROR_CHECK(err);
    } else if (strcmp(event.getText(), MX_DG_REDO) == 0) {
        editor->redo(err);
        MX_ERROR_CHECK(err);
    } else if (strcmp(event.getText(), MX_DG_GROUP) == 0) {
        mx_dg_group_key groupKey;

        editor->createGroup(err, groupKey);
        MX_ERROR_CHECK(err);
    }

    // already in some other state
    // abort the existing next state and impose
    // the new state

    if (nextType != MX_DGT_NONE) {
        editor->pushState(err, nextType);
        MX_ERROR_CHECK(err);
    }

abort:;
}

/*-------------------------------------------------
 * FUNCTION: mx_dg_top_state::setActive
 *
 * DESCRIPTION:
 *
 *
 */

void mx_dg_top_state::setActive(int& err,
    mx_event* event,
    mx_dg_state* inState)
{
    err = MX_ERROR_OK;

    // when set active reset the state again
    editor->pushState(err, inState->getType());
    MX_ERROR_CHECK(err);
abort:;
}

/*-------------------------------------------------
 * FUNCTION: mx_dg_top_state::processSpan
 *
 * DESCRIPTION:
 *
 *
 */

void mx_dg_top_state::processSpan(int& err,
    mx_event* event,
    gmx_span* sp)
{
    editor->addGeometry(err,
        *sp);
}

/*-------------------------------------------------
 * FUNCTION: mx_dg_top_state::processRect
 *
 * DESCRIPTION:
 *
 *
 */

void mx_dg_top_state::processRect(int& err,
    mx_event* event,
    gmx_rect* rect)
{
    editor->addGeometry(err,
        *rect);
}

/*-------------------------------------------------
 * FUNCTION: mx_dg_top_state::processSpan
 *
 * DESCRIPTION:
 *
 *
 */

void mx_dg_top_state::processArc(int& err,
    mx_event* event,
    gmx_arc* a)
{
    editor->addGeometry(err,
        *a);
}

/*-------------------------------------------------
 * FUNCTION: mx_dg_top_state::processPolypoint
 *
 * DESCRIPTION:
 *
 *
 */

void mx_dg_top_state::processPolypoint(int& err,
    mx_event* event,
    gmx_polypoint* pp)
{
    editor->addGeometry(err,
        *pp);
}

/*-------------------------------------------------
 * FUNCTION: mx_dg_state::propagatedUp
 *
 * DESCRIPTION:
 *
 *
 */

void mx_dg_top_state::propagatedUp(int& err,
    mx_event* event,
    mx_dg_state* inState)
{
    mx_text_event* textEvent = (mx_text_event*)event;

    err = MX_ERROR_OK;

    switch (event->getType()) {
    case MX_EVENT_TEXT:
        if ((strcmp(textEvent->getText(), MX_DG_GRID) != 0) && (nextState != NULL)) {
            nextState->collapseStates(err);
            MX_ERROR_CHECK(err);
        }

        textEntry(err, *textEvent);
        MX_ERROR_CHECK(err);
        break;
    default:
        break;
    }
abort:;
}

/*-------------------------------------------------
 * FUNCTION: mx_top_state::keyPress
 *
 * DESCRIPTION:
 *
 *
 */

void mx_dg_top_state::keyPress(int& err, mx_key_event& event)
{
    err = MX_ERROR_OK;

    if (strcmp(event.getKeyName(), "Escape") == 0) {
        MX_DGT_STATE nextType;

        if (nextState == NULL)
            return;

        nextType = nextState->getType();

        nextState->collapseStates(err);
        MX_ERROR_CHECK(err);

        editor->pushState(err, nextType);
        MX_ERROR_CHECK(err);

        editor->bell();
    }

abort:;
}
