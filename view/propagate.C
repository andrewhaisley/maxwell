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
 * MODULE : propagate.C
 *
 * AUTHOR : David Miller
 *
 * This file
 *
 * DESCRIPTION: Routines to propagate events
 * Module propagate.C
 *
 *
 *
 *
 */

#include "mx_cb.h"
#include "mx_event.h"

/*-------------------------------------------------
 * FUNCTION: mx_key_event::convert
 *
 * DESCRIPTION:
 *
 *
 */

void mx_key_event::convert(int& err,
    int destId,
    mx_callback_target* destinationTarget,
    mx_event* outEvent)
{
    mx_key_event* event = (mx_key_event*)outEvent;

    *event = *this;
    return;
}

/*-------------------------------------------------
 * FUNCTION: mx_scroll_event::convert
 *
 * DESCRIPTION:
 *
 *
 */

void mx_scroll_event::convert(int& err,
    int destId,
    mx_callback_target* destinationTarget,
    mx_event* outEvent)
{
    bool isInRange;
    MX_COORD_CONVERSION_FUNC conversionFunction;
    mx_callback_target* sourceTarget;
    mx_scroll_event* event = (mx_scroll_event*)outEvent;

    *event = *this;

    // get the coordinate conversion function

    conversionFunction = destinationTarget->getInProp(destId)->getFunction();
    sourceTarget = destinationTarget->getInProp(destId)->getTarget();

    if (conversionFunction != NULL) {
        (*conversionFunction)(err, sourceTarget, destinationTarget, event->newTopLeft, MX_COORD_C,
            isInRange);
        MX_ERROR_CHECK(err);
    }

    if (conversionFunction != NULL) {
        (*conversionFunction)(err, sourceTarget, destinationTarget, event->oldTopLeft, MX_COORD_C,
            isInRange);
        MX_ERROR_CHECK(err);
    }

    return;
abort:
    return;
}

/*-------------------------------------------------
 * FUNCTION: mx_button_event::convert
 *
 * DESCRIPTION:
 *
 *
 */

void mx_button_event::convert(int& err,
    int destId,
    mx_callback_target* destinationTarget,
    mx_event* outEvent)
{
    MX_COORD_CONVERSION_FUNC conversionFunction;
    mx_callback_target* sourceTarget;
    mx_button_event* event = (mx_button_event*)outEvent;

    *event = *this;

    // get the coordinate conversion function

    conversionFunction = destinationTarget->getInProp(destId)->getFunction();
    sourceTarget = destinationTarget->getInProp(destId)->getTarget();

    if (conversionFunction != NULL) {
        (*conversionFunction)(err, sourceTarget, destinationTarget, event->coord, MX_COORD_BR,
            event->coordInRange);
        MX_ERROR_CHECK(err);
    }

    if (conversionFunction != NULL) {
        (*conversionFunction)(err, sourceTarget, destinationTarget, event->startCoord, MX_COORD_TL,
            event->startCoordInRange);
        MX_ERROR_CHECK(err);
    }

    return;
abort:
    return;
}

/*-------------------------------------------------
 * FUNCTION: mx_button_event::propagate
 *
 * DESCRIPTION:
 *
 *
 */

void mx_button_event::propagate(int& err,
    mx_callback_target* destinationTarget)
{
    switch (getType()) {
    case MX_EVENT_BUTTON_PRESS:
        destinationTarget->buttonPress(err, *this);
        MX_ERROR_CHECK(err);
        break;
    case MX_EVENT_BUTTON_RELEASE:
        destinationTarget->buttonRelease(err, *this);
        MX_ERROR_CHECK(err);
        break;
    case MX_EVENT_BUTTON_MOTION_START:

        // save the last motion
        destinationTarget->setLastMotion(this->getCoord());

        destinationTarget->buttonMotionStart(err, *this);
        MX_ERROR_CHECK(err);

        break;
    case MX_EVENT_BUTTON_MOTION:

        // if checking for motion is on
        // then check if the point is the same

        if (destinationTarget->getFilterMotion()) {
            if (this->getCoord() != destinationTarget->getLastMotion()) {
                // save the last motion
                destinationTarget->setLastMotion(this->getCoord());

                destinationTarget->buttonMotion(err, *this);
                MX_ERROR_CHECK(err);
            }
        } else {
            destinationTarget->buttonMotion(err, *this);
            MX_ERROR_CHECK(err);
        }

        break;
    case MX_EVENT_BUTTON_MOTION_END:
        destinationTarget->buttonMotionEnd(err, *this);
        MX_ERROR_CHECK(err);
        break;
    case MX_EVENT_DOUBLE_BUTTON_PRESS:
        destinationTarget->buttonDoublePress(err, *this);
        MX_ERROR_CHECK(err);
        break;
    case MX_EVENT_TRIPLE_BUTTON_PRESS:
        destinationTarget->buttonTriplePress(err, *this);
        MX_ERROR_CHECK(err);
        break;
    case MX_EVENT_MOUSE_MOTION:
        destinationTarget->buttonMotion(err, *this);
        MX_ERROR_CHECK(err);
        break;
    default:
        break;
    }

    return;
abort:
    return;
}

/*-------------------------------------------------
 * FUNCTION: mx_key_event::propagate
 *
 * DESCRIPTION:
 *
 *
 */

void mx_key_event::propagate(int& err,
    mx_callback_target* destinationTarget)
{
    if (release) {
        destinationTarget->keyRelease(err, *this);
        MX_ERROR_CHECK(err);
    } else {
        destinationTarget->keyPress(err, *this);
        MX_ERROR_CHECK(err);
    }

    return;
abort:
    return;
}

/*-------------------------------------------------
 * FUNCTION: mx_draw_event::propagate
 *
 * DESCRIPTION:
 *
 *
 */

void mx_draw_event::propagate(int& err,
    mx_callback_target* destinationTarget)
{
    switch (getType()) {
    case MX_EVENT_PREDRAW:
        destinationTarget->preDraw(err, *this);
        MX_ERROR_CHECK(err);
        break;
    case MX_EVENT_DRAW:
        destinationTarget->draw(err, *this);
        MX_ERROR_CHECK(err);
        break;
    case MX_EVENT_SPECIAL_DRAW:
        destinationTarget->specialDraw(err, *this);
        MX_ERROR_CHECK(err);
        break;
    case MX_EVENT_POSTDRAW:
        destinationTarget->postDraw(err, *this);
        MX_ERROR_CHECK(err);
        break;
    default:
        break;
    }

    return;
abort:
    return;
}

/*-------------------------------------------------
 * FUNCTION: mx_scroll_event::propagate
 *
 * DESCRIPTION:
 *
 *
 */

void mx_scroll_event::propagate(int& err,
    mx_callback_target* destinationTarget)
{
    switch (getType()) {
    case MX_EVENT_PRESCROLL:
        destinationTarget->preScroll(err, *this);
        MX_ERROR_CHECK(err);
        break;
    case MX_EVENT_SCROLL:
        destinationTarget->scroll(err, *this);
        MX_ERROR_CHECK(err);
        break;
    case MX_EVENT_POSTSCROLL:
        destinationTarget->postScroll(err, *this);
        MX_ERROR_CHECK(err);
        break;
    case MX_EVENT_SCROLLDRAG:
        destinationTarget->scrollDrag(err, *this);
        MX_ERROR_CHECK(err);
        break;
    default:
        break;
    }

    return;
abort:
    return;
}

/*-------------------------------------------------
 * FUNCTION: mx_expose_event::convert
 *
 * DESCRIPTION:
 *
 *
 */

void mx_expose_event::convert(int& err,
    int destId,
    mx_callback_target* destinationTarget,
    mx_event* outEvent)
{
    bool isInRange;
    MX_COORD_CONVERSION_FUNC conversionFunction;
    mx_callback_target* sourceTarget;
    mx_expose_event* event = (mx_expose_event*)outEvent;

    *event = *this;

    // get the coordinate conversion function

    conversionFunction = destinationTarget->getInProp(destId)->getFunction();
    sourceTarget = destinationTarget->getInProp(destId)->getTarget();

    if (conversionFunction != NULL) {
        (*conversionFunction)(err, sourceTarget, destinationTarget, event->tl, MX_COORD_TL,
            isInRange);
        MX_ERROR_CHECK(err);
    }

    if (conversionFunction != NULL) {
        (*conversionFunction)(err, sourceTarget, destinationTarget, event->br, MX_COORD_BR,
            isInRange);
        MX_ERROR_CHECK(err);
    }

    return;
abort:
    return;
}

/*-------------------------------------------------
 * FUNCTION: mx_expose_event::propagate
 *
 * DESCRIPTION:
 *
 *
 */

void mx_expose_event::propagate(int& err,
    mx_callback_target* destinationTarget)
{
    err = MX_ERROR_OK;

    destinationTarget->expose(err, *this);
    MX_ERROR_CHECK(err);

    return;
abort:
    return;
}

/*-------------------------------------------------
 * FUNCTION: mx_resize_event::propagate
 *
 * DESCRIPTION:
 *
 *
 */

void mx_resize_event::propagate(int& err,
    mx_callback_target* destinationTarget)
{
    err = MX_ERROR_OK;

    destinationTarget->resize(err, *this);
    MX_ERROR_CHECK(err);

    return;
abort:
    return;
}

/*-------------------------------------------------
 * FUNCTION: mx_draw_event::convert
 *
 * DESCRIPTION:
 *
 *
 */

void mx_draw_event::convert(int& err,
    int destId,
    mx_callback_target* destinationTarget,
    mx_event* outEvent)
{
    bool isInRange;
    MX_COORD_CONVERSION_FUNC conversionFunction;
    mx_callback_target* sourceTarget;
    mx_draw_event* event = (mx_draw_event*)outEvent;

    *event = *this;

    // get the coordinate conversion function

    conversionFunction = destinationTarget->getInProp(destId)->getFunction();
    sourceTarget = destinationTarget->getInProp(destId)->getTarget();

    if (conversionFunction != NULL) {
        (*conversionFunction)(err, sourceTarget, destinationTarget, event->tl, MX_COORD_TL,
            isInRange);
        MX_ERROR_CHECK(err);
    }

    if (conversionFunction != NULL) {
        (*conversionFunction)(err, sourceTarget, destinationTarget, event->br, MX_COORD_BR,
            isInRange);
        MX_ERROR_CHECK(err);
    }

    return;
abort:
    return;
}

/*-------------------------------------------------
 * FUNCTION: mx_resize_event::convert
 *
 * DESCRIPTION:
 *
 *
 */

void mx_resize_event::convert(int& err,
    int destId,
    mx_callback_target* destinationTarget,
    mx_event* outEvent)
{
    bool isInRange;
    MX_COORD_CONVERSION_FUNC conversionFunction;
    mx_callback_target* sourceTarget;
    mx_resize_event* event = (mx_resize_event*)outEvent;

    *event = *this;

    // get the coordinate conversion function

    conversionFunction = destinationTarget->getInProp(destId)->getFunction();
    sourceTarget = destinationTarget->getInProp(destId)->getTarget();

    if (conversionFunction != NULL) {
        (*conversionFunction)(err, sourceTarget, destinationTarget, event->tl, MX_COORD_TL,
            isInRange);
        MX_ERROR_CHECK(err);
    }

    if (conversionFunction != NULL) {
        (*conversionFunction)(err, sourceTarget, destinationTarget, event->br, MX_COORD_BR,
            isInRange);
        MX_ERROR_CHECK(err);
    }

    return;
abort:
    return;
}

/*-------------------------------------------------
 * FUNCTION: mx_text_event::propagate
 *
 * DESCRIPTION:
 *
 *
 */

void mx_text_event::propagate(int& err,
    mx_callback_target* destinationTarget)
{
    err = MX_ERROR_OK;

    destinationTarget->textEntry(err, *this);
    MX_ERROR_CHECK(err);

    return;
abort:
    return;
}

/*-------------------------------------------------
 * FUNCTION: mx_text_event::convert
 *
 * DESCRIPTION:
 *
 *
 */

void mx_text_event::convert(int& err,
    int destId,
    mx_callback_target* destinationTarget,
    mx_event* outEvent)
{
    mx_text_event* event = (mx_text_event*)outEvent;

    *event = *this;
    return;
}
