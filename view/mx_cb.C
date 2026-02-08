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
 * MODULE : mx_cb.C
 *
 * AUTHOR : David Miller
 *
 * This file
 *
 * DESCRIPTION: Maxwell type callbacks
 * Module mx_cb.C
 *
 *
 *
 *
 */

#include "mx_cb.h"

/*-------------------------------------------------
 * FUNCTION: mx_callback_target::mx_callback_target
 *
 * DESCRIPTION:
 *
 *
 */

mx_callback_target::mx_callback_target()
{
    // propagate no event by default
    processMask = 0;
    noutPropagate = 0;
    ninPropagate = 0;

    for (int icount = 0; icount < MX_MAX_PROPAGATE; icount++) {
        inPropagate[icount].set(NULL, NULL);
        outPropagate[icount].set(NULL, -1);
    }

    filterSameMotion = FALSE;
}

/*-------------------------------------------------
 * FUNCTION: mx_callback_target::~mx_callback_target
 *
 * DESCRIPTION:
 *
 *
 */

mx_callback_target::~mx_callback_target()
{
    mx_callback_target *source, *target;
    int icount;

    // all things which propagated to this target must be
    // informed to no longer propagate events

    for (icount = 0; icount < MX_MAX_PROPAGATE; icount++) {
        source = inPropagate[icount].getTarget();
        if (source == NULL)
            continue;

        source->setToNotPropagateEvents(this);
    }

    // all things which this callback structure transmitted to
    // must be informed that they will no longer be receiving events

    for (icount = 0; icount < MX_MAX_PROPAGATE; icount++) {
        target = outPropagate[icount].getTarget();
        if (target == NULL)
            continue;

        target->setToNotReceiveEvents(outPropagate[icount].getId());
    }
}

// all callbacks are required to exist on the base class
// but they dont do anything except propagate

/*-------------------------------------------------
 * FUNCTION: mx_callback_target::buttonMotionStart
 *
 * DESCRIPTION:
 *
 *
 */

void mx_callback_target::buttonMotionStart(int& err,
    mx_button_event& event)
{
    mx_button_event outEvent;

    err = MX_ERROR_OK;

    propagateEvent(err, &event, &outEvent);
    MX_ERROR_CHECK(err);

    return;

abort:
    return;
}

/*-------------------------------------------------
 * FUNCTION: mx_callback_target::buttonMotion
 *
 * DESCRIPTION:
 *
 *
 */

void mx_callback_target::buttonMotion(int& err,
    mx_button_event& event)
{
    mx_button_event outEvent;

    err = MX_ERROR_OK;

    propagateEvent(err, &event, &outEvent);
    MX_ERROR_CHECK(err);

    return;

abort:
    return;
}

/*-------------------------------------------------
 * FUNCTION: mx_callback_target::buttonMotionEnd
 *
 * DESCRIPTION:
 *
 *
 */

void mx_callback_target::buttonMotionEnd(int& err,
    mx_button_event& event)
{
    mx_button_event outEvent;

    err = MX_ERROR_OK;

    propagateEvent(err, &event, &outEvent);
    MX_ERROR_CHECK(err);

    return;

abort:
    return;
}

/*-------------------------------------------------
 * FUNCTION: mx_callback_target::buttonPress
 *
 * DESCRIPTION:
 *
 *
 */

void mx_callback_target::buttonPress(int& err,
    mx_button_event& event)
{
    mx_button_event outEvent;

    err = MX_ERROR_OK;

    propagateEvent(err, &event, &outEvent);
    MX_ERROR_CHECK(err);

    return;

abort:
    return;
}

/*-------------------------------------------------
 * FUNCTION: mx_callback_target::externalSelection
 *
 * DESCRIPTION:
 *
 *
 */

void mx_callback_target::externalSelection(int& err, char* s)
{
}

/*-------------------------------------------------
 * FUNCTION: mx_callback_target::buttonRelease
 *
 * DESCRIPTION:
 *
 *
 */

void mx_callback_target::buttonRelease(int& err,
    mx_button_event& event)
{
    mx_button_event outEvent;

    err = MX_ERROR_OK;

    propagateEvent(err, &event, &outEvent);
    MX_ERROR_CHECK(err);

    return;

abort:
    return;
}

/*-------------------------------------------------
 * FUNCTION: mx_callback_target::buttonDoublePress
 *
 * DESCRIPTION:
 *
 *
 */

void mx_callback_target::buttonDoublePress(int& err,
    mx_button_event& event)
{
    mx_button_event outEvent;

    err = MX_ERROR_OK;

    propagateEvent(err, &event, &outEvent);
    MX_ERROR_CHECK(err);

    return;

abort:
    return;
}

/*-------------------------------------------------
 * FUNCTION: mx_callback_target::buttonTriplePress
 *
 * DESCRIPTION:
 *
 *
 */

void mx_callback_target::buttonTriplePress(int& err,
    mx_button_event& event)
{
    mx_button_event outEvent;

    err = MX_ERROR_OK;

    propagateEvent(err, &event, &outEvent);
    MX_ERROR_CHECK(err);

    return;

abort:
    return;
}

/*-------------------------------------------------
 * FUNCTION: mx_callback_target::keyPress
 *
 * DESCRIPTION:
 *
 *
 */

void mx_callback_target::keyPress(int& err,
    mx_key_event& event)
{
    mx_key_event outEvent;

    err = MX_ERROR_OK;

    propagateEvent(err, &event, &outEvent);
    MX_ERROR_CHECK(err);

    return;

abort:
    return;
}

/*-------------------------------------------------
 * FUNCTION: mx_callback_target::keyRelease
 *
 * DESCRIPTION:
 *
 *
 */

void mx_callback_target::keyRelease(int& err,
    mx_key_event& event)
{
    mx_key_event outEvent;

    err = MX_ERROR_OK;

    propagateEvent(err, &event, &outEvent);
    MX_ERROR_CHECK(err);

    return;

abort:
    return;
}

/*-------------------------------------------------
 * FUNCTION: mx_callback_target::textEntry
 *
 * DESCRIPTION:
 *
 *
 */

void mx_callback_target::textEntry(int& err,
    mx_text_event& event)
{
    mx_text_event outEvent;

    err = MX_ERROR_OK;

    propagateEvent(err, &event, &outEvent);
    MX_ERROR_CHECK(err);

    return;

abort:
    return;
}

/*-------------------------------------------------
 * FUNCTION: mx_callback_target::preDraw
 *
 * DESCRIPTION:
 *
 *
 */

void mx_callback_target::preDraw(int& err,
    mx_draw_event& event)
{
    mx_draw_event outEvent;

    err = MX_ERROR_OK;

    propagateEvent(err, &event, &outEvent);
    MX_ERROR_CHECK(err);

    return;

abort:
    return;
}

/*-------------------------------------------------
 * FUNCTION: mx_callback_target::draw
 *
 * DESCRIPTION:
 *
 *
 */

void mx_callback_target::draw(int& err,
    mx_draw_event& event)
{
    mx_draw_event outEvent;

    err = MX_ERROR_OK;

    propagateEvent(err, &event, &outEvent);
    MX_ERROR_CHECK(err);

    event.setCount(event.getCount() - 1);

    return;

abort:
    return;
}

/*-------------------------------------------------
 * FUNCTION: mx_callback_target::specialDraw
 *
 * DESCRIPTION:
 *
 *
 */

void mx_callback_target::specialDraw(int& err,
    mx_draw_event& event)
{
    mx_draw_event outEvent;

    err = MX_ERROR_OK;

    propagateEvent(err, &event, &outEvent);
    MX_ERROR_CHECK(err);

    return;

abort:
    return;
}

/*-------------------------------------------------
 * FUNCTION: mx_callback_target::postDraw
 *
 * DESCRIPTION:
 *
 *
 */

void mx_callback_target::postDraw(int& err,
    mx_draw_event& event)
{
    mx_draw_event outEvent;

    err = MX_ERROR_OK;

    propagateEvent(err, &event, &outEvent);
    MX_ERROR_CHECK(err);

    return;

abort:
    return;
}

/*-------------------------------------------------
 * FUNCTION: mx_callback_target::preScroll
 *
 * DESCRIPTION:
 *
 *
 */

void mx_callback_target::preScroll(int& err,
    mx_scroll_event& event)
{
    mx_scroll_event outEvent;

    err = MX_ERROR_OK;

    propagateEvent(err, &event, &outEvent);
    MX_ERROR_CHECK(err);

    return;

abort:
    return;
}

/*-------------------------------------------------
 * FUNCTION: mx_callback_target::scroll
 *
 * DESCRIPTION:
 *
 *
 */

void mx_callback_target::scroll(int& err,
    mx_scroll_event& event)
{
    mx_scroll_event outEvent;

    err = MX_ERROR_OK;

    propagateEvent(err, &event, &outEvent);
    MX_ERROR_CHECK(err);

    return;

abort:
    return;
}

/*-------------------------------------------------
 * FUNCTION: mx_callback_target::postScroll
 *
 * DESCRIPTION:
 *
 *
 */

void mx_callback_target::postScroll(int& err,
    mx_scroll_event& event)
{
    mx_scroll_event outEvent;

    err = MX_ERROR_OK;

    propagateEvent(err, &event, &outEvent);
    MX_ERROR_CHECK(err);

    return;

abort:
    return;
}

/*-------------------------------------------------
 * FUNCTION: mx_callback_target::scrollDrag
 *
 * DESCRIPTION:
 *
 *
 */

void mx_callback_target::scrollDrag(int& err,
    mx_scroll_event& event)
{
    mx_scroll_event outEvent;

    err = MX_ERROR_OK;

    propagateEvent(err, &event, &outEvent);
    MX_ERROR_CHECK(err);

    return;

abort:
    return;
}

/*-------------------------------------------------
 * FUNCTION: mx_callback_target::resize
 *
 * DESCRIPTION:
 *
 *
 */

void mx_callback_target::resize(int& err,
    mx_resize_event& event)
{
    mx_resize_event outEvent;

    err = MX_ERROR_OK;

    propagateEvent(err, &event, &outEvent);
    MX_ERROR_CHECK(err);

    return;

abort:
    return;
}

/*-------------------------------------------------
 * FUNCTION: mx_callback_target::expose
 *
 * DESCRIPTION:
 *
 *
 */

void mx_callback_target::expose(int& err,
    mx_expose_event& event)
{
    mx_expose_event outEvent;

    err = MX_ERROR_OK;

    propagateEvent(err, &event, &outEvent);
    MX_ERROR_CHECK(err);

    return;

abort:
    return;
}

/*-------------------------------------------------
 * FUNCTION: mx_callback_target::propagateEvent
 *
 * DESCRIPTION:
 *
 *
 */

void mx_callback_target::propagateEvent(int& err,
    mx_event* originalEvent,
    mx_event* outEvent)
{
    mx_cb_out_propagate* thisOutPropagate;

    int ipropagate;

    for (ipropagate = 0; ipropagate < MX_MAX_PROPAGATE; ipropagate++) {
        thisOutPropagate = outPropagate + ipropagate;

        if (thisOutPropagate->getTarget() == NULL)
            continue;

        if (thisOutPropagate->getTarget()->testProcessMask(originalEvent->getType())) {
            // convert the event to the destination format
            originalEvent->convert(err,
                thisOutPropagate->getId(),
                thisOutPropagate->getTarget(),
                outEvent);
            MX_ERROR_CHECK(err);

            // set the targets id for this source
            outEvent->setSourceId(thisOutPropagate->getId());

            outEvent->propagate(err,
                thisOutPropagate->getTarget());
            MX_ERROR_CHECK(err);
        }
    }

    return;
abort:
    return;
}

/*-------------------------------------------------
 * FUNCTION: mx_callback_target::setToPropagateEvents
 *
 * DESCRIPTION:
 *
 *
 */

void mx_callback_target::setToPropagateEvents(int& err,
    mx_callback_target* receiver,
    int receiverId)
{
    err = MX_ERROR_OK;

    if (noutPropagate == MX_MAX_PROPAGATE) {
        MX_ERROR_THROW(err, MX_TOO_MANY_PROPAGATE);
    }

    for (int icount = 0; icount < MX_MAX_PROPAGATE; icount++) {

        if (outPropagate[icount].getTarget() == NULL) {
            outPropagate[icount].set(receiver, receiverId);
            break;
        }
    }

    noutPropagate++;

    return;
abort:
    return;
}

/*-------------------------------------------------
 * FUNCTION: mx_callback_target::setToReceiveEvents
 *
 * DESCRIPTION:
 *
 *
 */

void mx_callback_target::setToReceiveEvents(int& err,
    mx_callback_target* propagator,
    MX_COORD_CONVERSION_FUNC func)
{
    int icount;

    err = MX_ERROR_OK;

    if (ninPropagate == MX_MAX_PROPAGATE) {
        MX_ERROR_THROW(err, MX_TOO_MANY_PROPAGATE);
    }

    for (icount = 0; icount < MX_MAX_PROPAGATE; icount++) {
        if (inPropagate[icount].getTarget() == NULL) {
            inPropagate[icount].set(propagator, func);
            break;
        }
    }

    propagator->setToPropagateEvents(err,
        this,
        icount);
    MX_ERROR_CHECK(err);

    ninPropagate++;

    return;
abort:
    return;
}

/*-------------------------------------------------
 * FUNCTION: mx_callback_target::setToNotPropagateEvents
 *
 * DESCRIPTION:
 *
 *
 */

void mx_callback_target::setToNotPropagateEvents(mx_callback_target* target)
{
    int icount;

    // now find the entries which the source transmits to
    // this target with - could be more than one

    for (icount = 0; icount < MX_MAX_PROPAGATE; icount++) {
        if (outPropagate[icount].getTarget() == target) {
            target->setToNotReceiveEvents(outPropagate[icount].getId());
            outPropagate[icount].set(NULL, -1);
            noutPropagate--;
        }
    }
}

/*-------------------------------------------------
 * FUNCTION: mx_callback_target::setToNotRecieveEvents
 *
 * DESCRIPTION:
 *
 *
 */

void mx_callback_target::setToNotReceiveEvents(int id)
{
    inPropagate[id].set(NULL, NULL);
    ninPropagate--;
}

/*-------------------------------------------------
 * FUNCTION: mx_callback_target::getExternalSelection
 *
 * DESCRIPTION:
 *  called by the frame to get the value of the current
 *  external selection in its target.
 *
 */
const char* mx_callback_target::getExternalSelection(int& err)
{
    return NULL;
}
