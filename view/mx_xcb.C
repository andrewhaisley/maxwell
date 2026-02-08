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
 * MODULE : mx_xcb.C
 *
 * AUTHOR : David Miller
 *
 * This file
 *
 * DESCRIPTION: Callbacks on the basic callback target.
 * The virtual callbacks for this class do nothing, but
 * the real Xt callback also sits on this class and this
 * does lots of work.
 *
 * Module mx_xcb.C
 *
 *
 *
 *
 */

#include "limits.h"
#include "mx_xcb.h"

/*-------------------------------------------------
 * FUNCTION: mx_xcallback_target::xevTOmxee
 *
 * DESCRIPTION: Convert an X callback button event
 * to a mx expose event
 *
 */

void mx_xcallback_target::xevTOmxse(int& err,
    mx_scroll_event& mxse,
    Widget eventWidget,
    XmScrollBarCallbackStruct* scrollData)
{
    err = MX_ERROR_OK;

    mxse.setTLvalue(scrollData->value);

    if (eventWidget == hscrollWidget) {
        mxse.setIsVertical(FALSE);
    } else {
        mxse.setIsVertical(TRUE);
    }

    mxse.setUseScrollPosition(FALSE);

    if (scrollData->reason == XmCR_DRAG) {
        mxse.setType(MX_EVENT_SCROLLDRAG);
    } else {
        mxse.setType(MX_EVENT_NO_EVENT);
    }

    return;
}

/*-------------------------------------------------
 * FUNCTION: mx_xcallback_target::xevTOmxee
 *
 * DESCRIPTION: Convert an X callback button event
 * to a mx expose event
 *
 */

void mx_xcallback_target::xevTOmxee(int& err,
    mx_expose_event& mxee,
    XExposeEvent* xee)
{
    mx_doc_coord_t tl, br;

    err = MX_ERROR_OK;

    tl.sheet_number = 0;
    tl.p.x = xee->x;
    tl.p.y = xee->y;

    br.sheet_number = 0;
    br.p.x = tl.p.x + (xee->width - 1);
    br.p.y = tl.p.y + (xee->height - 1);

    mxee.setRect(tl, br);
    mxee.setCount(xee->count);
}

/*-------------------------------------------------
 * FUNCTION: mx_xcallback_target::xevTOmxre
 *
 * DESCRIPTION: Convert an X callback button event
 * to a mx expose event
 *
 */

void mx_xcallback_target::xevTOmxre(int& err,
    mx_resize_event& mxre,
    XConfigureEvent* xre)
{
    mx_doc_coord_t tl(0, 0, 0);
    mx_doc_coord_t br;

    err = MX_ERROR_OK;

    br.sheet_number = 0;
    br.p.x = xre->width;
    br.p.y = xre->height;

    mxre.setRect(tl, br);
}

/*-------------------------------------------------
 * FUNCTION: mx_xcallback_target::xevTOmxbe
 *
 * DESCRIPTION: Convert an X callback button event
 * to a mx button event
 *
 */

void mx_xcallback_target::xevTOmxbe(int& err,
    unsigned long int type,
    int nclicks,
    mx_button_event& mxbe,
    XButtonEvent* sxbe,
    XButtonEvent* xbe,
    XMotionEvent* xme)
{
    bool shiftDown, controlDown;
    int buttonNumber;
    mx_doc_coord_t whereIn;
    XButtonEvent* testBE;

    err = MX_ERROR_OK;

    testBE = (xbe == NULL) ? sxbe : xbe;

    if (testBE == NULL) {
        buttonNumber = 0;
        shiftDown = ((xme->state & ShiftMask) != 0);
        controlDown = ((xme->state & ControlMask) != 0);
    } else {
        // set button number
        switch (testBE->button) {
        case Button2:
            buttonNumber = 2;
            break;
        case Button3:
            buttonNumber = 3;
            break;
        default:
            buttonNumber = 1;
            break;
        }

        shiftDown = ((testBE->state & ShiftMask) != 0);
        controlDown = ((testBE->state & ControlMask) != 0);
    }

    mxbe.setButtonNumber(buttonNumber);

    // set modifiers
    mxbe.setShift(shiftDown);
    mxbe.setControl(controlDown);

    // set clicks
    mxbe.setNclicks(nclicks);

    // set type
    mxbe.setType(type);

    whereIn.sheet_number = 0;
    if (sxbe != NULL) {
        whereIn.p.x = sxbe->x;
        whereIn.p.y = sxbe->y;

        mxbe.setStartCoord(whereIn);
    }

    if (xbe != NULL) {
        whereIn.p.x = xbe->x;
        whereIn.p.y = xbe->y;
    } else {
        // xme must be set
        whereIn.p.x = xme->x;
        whereIn.p.y = xme->y;
    }

    mxbe.setCoord(whereIn);
}

/*-------------------------------------------------
 * FUNCTION: mx_xcallback_target::xevTOmxke
 *
 * DESCRIPTION: Convert an X callback button event
 * to a mx button event
 *
 */

int mx_xcallback_target::xevTOmxke(int& err,
    bool isRelease,
    mx_key_event& mxke,
    XKeyEvent* xke)
{
    XComposeStatus composeStatus;
    KeySym keysym;
    int nchars;
    char* storedString;
    int storedStringSize;
    bool shiftDown, controlDown;
    char* keyStr;

    err = MX_ERROR_OK;

    storedString = mxke.getString();
    storedStringSize = mxke.getTotalStringSize();

    nchars = XLookupString(xke, storedString, storedStringSize,
        &keysym, &composeStatus);

    // if the string is too large dont do anything
    if (nchars >= storedStringSize)
        nchars = 0;

    storedString[nchars] = 0;

    keyStr = XKeysymToString(keysym);

    if (keyStr == NULL) {
        mxke.setKeyName("");
        mxke.setStringSize(0);
    } else {
        mxke.setKeyName(keyStr);
        mxke.setStringSize(nchars);
    }

    shiftDown = ((xke->state & ShiftMask) != 0);
    controlDown = ((xke->state & ControlMask) != 0);
    mxke.setShift(shiftDown);
    mxke.setControl(controlDown);
    mxke.setRelease(isRelease);

    return nchars;
}

/*-------------------------------------------------
 * FUNCTION: mx_xcallback_target::setCBwidget
 *
 * DESCRIPTION:
 *
 *
 */

void mx_xcallback_target::setCBwidget(Widget iwidget)
{
    // always add the event handlers - even if the mask
    // means they will be thrown away - dealing with
    // all the multiple clicks and motion events with
    // different event masks o the widget would be
    // horrible

    if (iwidget == NULL)
        return;

    cbwidget = iwidget;

    eventMask = KeyPressMask | KeyReleaseMask | ButtonPressMask | ButtonReleaseMask | ExposureMask | StructureNotifyMask | ButtonMotionMask;

    XtAddEventHandler(cbwidget,
        eventMask,
        False,
        (XtEventHandler)&mxXtEH,
        (XtPointer)this);
}

/*-------------------------------------------------
 * FUNCTION: mx_xcallback_target::mx_xcallback_target
 *
 * DESCRIPTION:
 *
 *
 */

void mx_xcallback_target::setEvent(int& err,
    unsigned long int eventType,
    bool setOn)
{
    EventMask mask;

    err = MX_ERROR_OK;

    switch (eventType) {
    case MX_EVENT_MOUSE_MOTION:
        mask = PointerMotionMask;

        if (setOn) {
            // grab the mouse pointer
            XGrabPointer(XtDisplay(cbwidget),
                XtWindow(cbwidget),
                False,
                ButtonPressMask | ButtonReleaseMask | ButtonMotionMask | PointerMotionMask,
                GrabModeAsync,
                GrabModeAsync,
                None,
                None,
                CurrentTime);
        } else {
            // ungrab the mouse pointer
            XUngrabPointer(XtDisplay(cbwidget),
                CurrentTime);
        }
        break;
    default:
        return;
        break;
    }

    if (setOn) {
        eventMask |= mask;

        XtAddEventHandler(cbwidget,
            mask,
            False,
            (XtEventHandler)&mxXtEH,
            (XtPointer)this);
    } else {
        eventMask &= ~mask;
        XtRemoveEventHandler(cbwidget,
            mask,
            False,
            (XtEventHandler)&mxXtEH,
            (XtPointer)this);
    }
}

/*-------------------------------------------------
 * FUNCTION: mx_xcallback_target::mx_xcallback_target
 *
 * DESCRIPTION:
 *
 *
 */

mx_xcallback_target::mx_xcallback_target(Widget iwidget)
{
    cbwidget = NULL;
    vscrollWidget = NULL;
    hscrollWidget = NULL;

    numberButtonClicks = 0;
    buttonIsInMotion = FALSE;
    clickTimerId = 0;
    stillMotionTimerId = 0;

    hadButtonRelease = TRUE;
    nmotions = 0;
    cumulativeButtonMode = TRUE;

    // add callbacks for exposure,resize and input
    // masking of events is done either on the widget already
    // or by using the callbacks on the base mx_callback class
    // which do nothing

    eventMask = 0;
    setCBwidget(iwidget);

    return;
}

/*-------------------------------------------------
 * FUNCTION: mx_xcallback_target::mxXtCallback
 *
 * DESCRIPTION: The function to cope with Motif
 * type callbacks
 *
 */

void mx_xcallback_target::mxXtCallback(Widget widget,
    XtPointer clientData,
    XtPointer callData)
{
    int err = MX_ERROR_OK;
    XmAnyCallbackStruct* cbs = (XmAnyCallbackStruct*)callData;
    mx_xcallback_target* thisTarget = (mx_xcallback_target*)clientData;

    switch (cbs->reason) {
    case XmCR_INCREMENT:
    case XmCR_DECREMENT:
    case XmCR_PAGE_INCREMENT:
    case XmCR_PAGE_DECREMENT:
    case XmCR_TO_BOTTOM:
    case XmCR_TO_TOP:
    case XmCR_VALUE_CHANGED:
    case XmCR_DRAG:

        thisTarget->processScroll(err,
            widget,
            (XmScrollBarCallbackStruct*)callData);
        MX_ERROR_CHECK(err);
        break;
    default:
        break;
    }

    return;
abort:
    global_error_trace->print();
    MX_ERROR_CLEAR(err);
}

/*-------------------------------------------------
 * FUNCTION: mx_xcallback_target::mxXtEH
 *
 * DESCRIPTION:
 *
 *
 */

void mx_xcallback_target::mxXtEH(Widget widget,
    XtPointer clientData,
    XEvent* event,
    bool* cont_dispatch)
{
    int err = MX_ERROR_OK;
    mx_xcallback_target* thisTarget;

#ifndef sun
    *cont_dispatch = TRUE;
#endif

    thisTarget = (mx_xcallback_target*)clientData;

    switch (event->type) {
    case Expose:
        thisTarget->processExpose(err,
            (XExposeEvent*)&(event->xexpose));
        MX_ERROR_CHECK(err);
        break;
    case ConfigureNotify:
        thisTarget->processResize(err,
            (XConfigureEvent*)&(event->xconfigure));
        MX_ERROR_CHECK(err);
        break;
    case KeyPress:
        thisTarget->processKeyPress(err,
            (XKeyEvent*)&(event->xkey));
        MX_ERROR_CHECK(err);
        break;
    case KeyRelease:
        thisTarget->processKeyRelease(err,
            (XKeyEvent*)&(event->xkey));
        MX_ERROR_CHECK(err);
        break;
    case ButtonPress:
        thisTarget->processButtonPress(err,
            (XButtonEvent*)&(event->xbutton));
        MX_ERROR_CHECK(err);
        break;
    case ButtonRelease:
        thisTarget->processButtonRelease(err,
            (XButtonEvent*)&(event->xbutton));
        MX_ERROR_CHECK(err);
        break;
    case MotionNotify:
        thisTarget->processButtonMotion(err,
            &(event->xmotion));
        MX_ERROR_CHECK(err);
        break;
    default:
        break;
    }
    return;

abort:
    global_error_trace->print();
    MX_ERROR_CLEAR(err);
}

/*-------------------------------------------------
 * FUNCTION: mx_xcallback_target::mxCTCallback
 *
 * DESCRIPTION: The real static callback for click timeouts
 *
 *
 */

void mx_xcallback_target::mxCTCallback(XtPointer clientData,
    XtIntervalId* timer_id)
{
    int err = MX_ERROR_OK;
    mx_xcallback_target* thisTarget;

    thisTarget = (mx_xcallback_target*)clientData;

    thisTarget->processTimeout(err, *timer_id);
    MX_ERROR_CHECK(err);

    return;
abort:
    global_error_trace->print();
    MX_ERROR_CLEAR(err);
}

/*-------------------------------------------------
 * FUNCTION: mx_xcallback_target::processTimeout
 *
 * DESCRIPTION: Process some button clicks
 *
 */

void mx_xcallback_target::processTimeout(int& err,
    XtIntervalId timer_id)
{
    err = MX_ERROR_OK;

    if (timer_id == clickTimerId) {
        clickTimerId = 0;

        processClickTimeout(err);
        MX_ERROR_CHECK(err);
    } else if (timer_id == stillMotionTimerId) {
        stillMotionTimerId = 0;

        processStillMotionTimeout(err);
        MX_ERROR_CHECK(err);
    }

    return;
abort:
    return;
}

/*-------------------------------------------------
 * FUNCTION: mx_xcallback_target::processButtonClicks
 *
 * DESCRIPTION: Process some button clicks
 *
 */

void mx_xcallback_target::processButtonClicks(int& err)
{
    mx_button_event mxbuttonEvent;
    int icount;

    err = MX_ERROR_OK;

    // no clicks return
    if (numberButtonClicks == 0)
        return;

    // if not processing button presses return
    if (!testProcessMask(MX_EVENT_BUTTON_PRESSES)) {
        numberButtonClicks = 0;
        goto processRelease;
    }

    switch (numberButtonClicks) {
    case 3:

        if (testProcessMask(MX_EVENT_TRIPLE_BUTTON_PRESS)) {
            xevTOmxbe(err, MX_EVENT_TRIPLE_BUTTON_PRESS, numberButtonClicks,
                mxbuttonEvent, NULL, &firstEvent, NULL);
            MX_ERROR_CHECK(err);

            buttonTriplePress(err, mxbuttonEvent);
            MX_ERROR_CHECK(err);
        } else if (testProcessMask(MX_EVENT_BUTTON_PRESS)) {
            // three single button presses
            for (icount = 0; icount < numberButtonClicks; icount++) {
                xevTOmxbe(err, MX_EVENT_BUTTON_PRESS, 1,
                    mxbuttonEvent, NULL, &firstEvent, NULL);
                MX_ERROR_CHECK(err);

                buttonPress(err, mxbuttonEvent);
                MX_ERROR_CHECK(err);
            }
        }
        break;
    case 2:
        if (testProcessMask(MX_EVENT_DOUBLE_BUTTON_PRESS)) {
            xevTOmxbe(err, MX_EVENT_DOUBLE_BUTTON_PRESS, numberButtonClicks,
                mxbuttonEvent, NULL, &firstEvent, NULL);
            MX_ERROR_CHECK(err);

            buttonDoublePress(err, mxbuttonEvent);
            MX_ERROR_CHECK(err);
        } else if (testProcessMask(MX_EVENT_BUTTON_PRESS)) {
            // two single button presses
            for (icount = 0; icount < numberButtonClicks; icount++) {
                xevTOmxbe(err, MX_EVENT_BUTTON_PRESS, 1,
                    mxbuttonEvent, NULL, &firstEvent, NULL);
                MX_ERROR_CHECK(err);

                buttonPress(err, mxbuttonEvent);
                MX_ERROR_CHECK(err);
            }
        }
        break;
    default:
        if (testProcessMask(MX_EVENT_BUTTON_PRESS)) {
            xevTOmxbe(err, MX_EVENT_BUTTON_PRESS, numberButtonClicks,
                mxbuttonEvent, NULL, &firstEvent, NULL);
            MX_ERROR_CHECK(err);

            buttonPress(err, mxbuttonEvent);
            MX_ERROR_CHECK(err);
        }
        break;
    }

    if (cumulativeButtonMode)
        return;

    numberButtonClicks = 0;

    // process a release event if one is required

processRelease:
    if (testProcessMask(MX_EVENT_BUTTON_RELEASE)) {
        // this only occurs where the button has been release before
        // the timeout -store the event here - output it after the
        // press event if required

        xevTOmxbe(err, MX_EVENT_BUTTON_RELEASE, 0,
            mxbuttonEvent, &lastReleaseEvent, &lastReleaseEvent, NULL);
        MX_ERROR_CHECK(err);

        buttonRelease(err, mxbuttonEvent);
        MX_ERROR_CHECK(err);
    }

    return;
abort:
    numberButtonClicks = 0;
    return;
}

/*-------------------------------------------------
 * FUNCTION: mx_xcallback_target::processClickTimeout
 *
 * DESCRIPTION: The timeout callback applied to a
 * particular callback target
 *
 */

void mx_xcallback_target::processClickTimeout(int& err)
{
    int nsavedMotions;
    int imotion;

    err = MX_ERROR_OK;

    if (hadButtonRelease) {
        processButtonClicks(err);
        MX_ERROR_CHECK(err);
    } else {
        // reached timeout with button still down
        // start to drag

        mx_button_event mxbuttonEvent;

        // use original button press position

        if (testProcessMask(MX_EVENT_BUTTON_MOTION_START)) {
            xevTOmxbe(err, MX_EVENT_BUTTON_MOTION_START, numberButtonClicks,
                mxbuttonEvent, &firstEvent, &firstEvent, NULL);
            MX_ERROR_CHECK(err);

            buttonMotionStart(err, mxbuttonEvent);
            MX_ERROR_CHECK(err);
        } else if (testProcessMask(MX_EVENT_BUTTON_PRESS)) {
            // if button motion is not allowed - classify as a key press

            xevTOmxbe(err, MX_EVENT_BUTTON_PRESS, numberButtonClicks,
                mxbuttonEvent, &firstEvent, &firstEvent, NULL);
            MX_ERROR_CHECK(err);

            buttonMotion(err, mxbuttonEvent);
            MX_ERROR_CHECK(err);
        }

        buttonIsInMotion = TRUE;

        // now output any button motion events we have stored up
        // before getting the first event

        nsavedMotions = nmotions;
        nmotions = 0;

        if (testProcessMask(MX_EVENT_BUTTON_MOTION)) {
            for (imotion = 0; imotion < nsavedMotions; imotion++) {
                xevTOmxbe(err, MX_EVENT_BUTTON_MOTION, numberButtonClicks,
                    mxbuttonEvent, &firstEvent, NULL, motions + imotion);
                MX_ERROR_CHECK(err);

                buttonMotion(err, mxbuttonEvent);
                MX_ERROR_CHECK(err);
            }
        }
    }

    return;
abort:
    return;
}

/*-------------------------------------------------
 * FUNCTION: mx_xcallback_target::processStillMotionTimeout
 *
 * DESCRIPTION: The timeout callback applied to a
 * particular callback target
 *
 */

void mx_xcallback_target::processStillMotionTimeout(int& err)
{
    err = MX_ERROR_OK;

    // test button is still in motion
    if (!buttonIsInMotion)
        return;

    // reached timeout with button still down
    // generate new motion event

    if (testProcessMask(MX_EVENT_BUTTON_MOTION)) {
        processButtonMotion(err, &lastMotionEvent);
        MX_ERROR_CHECK(err);
    }

    return;
abort:
    return;
}

/*-------------------------------------------------
 * FUNCTION: mx_xcallback_target::processButtonPress
 *
 * DESCRIPTION:
 *
 *
 */

void mx_xcallback_target::processButtonPress(int& err,
    XButtonEvent* buttonEvent)
{
    int timeSinceLast;

    err = MX_ERROR_OK;

    hadButtonRelease = FALSE;

    // set a timer for this number of clicks and wait to see
    // if it is called

    if (cumulativeButtonMode) {
        if (numberButtonClicks > 0) {
            if (buttonEvent->button != firstEvent.button) {
                numberButtonClicks = 0;
            } else {
                if (firstEvent.time > buttonEvent->time) {
                    // pretty unusual - but better treat this case
                    timeSinceLast = UINT_MAX - firstEvent.time + buttonEvent->time;
                } else {
                    timeSinceLast = buttonEvent->time - firstEvent.time;
                }

                if (timeSinceLast > XtGetMultiClickTime(XtDisplay(cbwidget))) {
                    numberButtonClicks = 0;
                } else {
                    firstEvent.time = buttonEvent->time;
                }
            }
        }

        if (numberButtonClicks == 0)
            firstEvent = *buttonEvent;

        numberButtonClicks++;

        /* process the number of clicks we have */
        processButtonClicks(err);
        MX_ERROR_CHECK(err);

        if (numberButtonClicks == 3)
            numberButtonClicks = 0;
    } else {
        // if this is a first button event then store the first
        // event for comparison with later events
        if (numberButtonClicks == 0)
            firstEvent = *buttonEvent;

        // set release flag to FALSE ;
        nmotions = 0;

        // remove a time out if there is one
        if (clickTimerId != 0)
            XtRemoveTimeOut(clickTimerId);
        clickTimerId = 0;

        // if we have some button events on the previous
        // mouse button press on a different button
        // then process these events

        if ((numberButtonClicks > 0) && (buttonEvent->button != firstEvent.button)) {
            processButtonClicks(err);
            MX_ERROR_CHECK(err);

            numberButtonClicks = 0;

            // this is the new first button event
            firstEvent = *buttonEvent;
        }

        numberButtonClicks++;

        // we only do up to three clicks

        if (numberButtonClicks == 3) {
            processButtonClicks(err);
            MX_ERROR_CHECK(err);

            numberButtonClicks = 0;
        } else {
            clickTimerId = XtAppAddTimeOut(XtWidgetToApplicationContext(cbwidget),
                XtGetMultiClickTime(XtDisplay(cbwidget)),
                (XtTimerCallbackProc)mxCTCallback,
                (XtPointer)this);
        }
    }

    return;
abort:
    return;
}

/*-------------------------------------------------
 * FUNCTION: mx_xcallback_target::processKeyPress
 *
 * DESCRIPTION:
 *
 *
 */

void mx_xcallback_target::processKeyPress(int& err,
    XKeyEvent* keyEvent)
{
    mx_key_event mxKeyEvent;

    if (testProcessMask(MX_EVENT_KEY_PRESS)) {
        // test the next event
        (void)xevTOmxke(err, FALSE, mxKeyEvent, keyEvent);
        MX_ERROR_CHECK(err);

        keyPress(err, mxKeyEvent);
        MX_ERROR_CHECK(err);

#ifdef MX_EVENT_CHECK

        do {
            retVal = XCheckWindowEvent(keyEvent->display,
                keyEvent->window,
                XtAllEvents,
                &retEvent);

            // no more events
            if (retVal == FALSE)
                break;

            switch (retEvent.type) {
            case KeyPress:
                break;
            default:
                XPutBackEvent(keyEvent->display,
                    &retEvent);
                break;
            }

        } while (1);

#endif
    }

    return;
abort:
    return;
}

/*-------------------------------------------------
 * FUNCTION: mx_xcallback_target::processKeyRelease
 *
 * DESCRIPTION:
 *
 *
 */

void mx_xcallback_target::processKeyRelease(int& err,
    XKeyEvent* keyEvent)
{
    mx_key_event mxKeyEvent;

    if (testProcessMask(MX_EVENT_KEY_RELEASE)) {
        xevTOmxke(err, TRUE, mxKeyEvent, keyEvent);
        MX_ERROR_CHECK(err);

        keyRelease(err, mxKeyEvent);
        MX_ERROR_CHECK(err);
    }

    return;
abort:
    return;
}

/*-------------------------------------------------
 * FUNCTION: mx_xcallback_target::processButtonRelease
 *
 * DESCRIPTION:
 *
 *
 */

void mx_xcallback_target::processButtonRelease(int& err,
    XButtonEvent* buttonEvent)
{
    mx_button_event mxbuttonEvent;

    hadButtonRelease = TRUE;

    if (buttonIsInMotion && (buttonEvent->button == firstEvent.button)) {
        if (testProcessMask(MX_EVENT_BUTTON_MOTION_END)) {
            // use original button press position
            xevTOmxbe(err, MX_EVENT_BUTTON_MOTION_END, 0,
                mxbuttonEvent, &firstEvent, buttonEvent, NULL);
            MX_ERROR_CHECK(err);

            buttonMotionEnd(err, mxbuttonEvent);
            MX_ERROR_CHECK(err);
        } else if (testProcessMask(MX_EVENT_BUTTON_RELEASE)) {
            xevTOmxbe(err, MX_EVENT_BUTTON_RELEASE, 0,
                mxbuttonEvent, &firstEvent, buttonEvent, NULL);
            MX_ERROR_CHECK(err);

            buttonRelease(err, mxbuttonEvent);
            MX_ERROR_CHECK(err);
        }

        buttonIsInMotion = FALSE;
        numberButtonClicks = 0;
    } else {
        if (testProcessMask(MX_EVENT_BUTTON_RELEASE)) {
            if (cumulativeButtonMode) {
                xevTOmxbe(err, MX_EVENT_BUTTON_RELEASE, 0,
                    mxbuttonEvent, &firstEvent, buttonEvent, NULL);
                MX_ERROR_CHECK(err);

                buttonRelease(err, mxbuttonEvent);
                MX_ERROR_CHECK(err);
            } else {
                // this only occurs where the button has been release before
                // the timeout -store the event here - output it after the
                // press event if required

                lastReleaseEvent = *buttonEvent;
            }
        }
    }

    return;
abort:
    return;
}

/*-------------------------------------------------
 * FUNCTION: mx_xcallback_target::processButtonMotion
 *
 * DESCRIPTION:
 *
 *
 */

void mx_xcallback_target::processButtonMotion(int& err,
    XMotionEvent* motionEvent)
{
    mx_button_event mxbuttonEvent;

    /* clear the timer */

    if (stillMotionTimerId != 0) {
        // remove a time out if there is one
        if (stillMotionTimerId != 0)
            XtRemoveTimeOut(stillMotionTimerId);
        stillMotionTimerId = 0;
    }

    if (hadButtonRelease) {
        if (testProcessMask(MX_EVENT_MOUSE_MOTION)) {
            // button is not down - an ordinary motion event
            xevTOmxbe(err, MX_EVENT_MOUSE_MOTION, 0,
                mxbuttonEvent, NULL, NULL, motionEvent);
            MX_ERROR_CHECK(err);

            buttonMotion(err, mxbuttonEvent);
            MX_ERROR_CHECK(err);
        }
    } else if (buttonIsInMotion || (cumulativeButtonMode)) {
        if (!buttonIsInMotion) {
            // the start of a button motion
            // create a button motion start event

            if (testProcessMask(MX_EVENT_BUTTON_MOTION_START)) {
                xevTOmxbe(err, MX_EVENT_BUTTON_MOTION_START, numberButtonClicks,
                    mxbuttonEvent, &firstEvent, &firstEvent, NULL);
                MX_ERROR_CHECK(err);

                buttonMotionStart(err, mxbuttonEvent);
                MX_ERROR_CHECK(err);
            }
        }

        buttonIsInMotion = TRUE;

        if (testProcessMask(MX_EVENT_BUTTON_MOTION)) {
            xevTOmxbe(err, MX_EVENT_BUTTON_MOTION, numberButtonClicks,
                mxbuttonEvent, &firstEvent, NULL, motionEvent);
            MX_ERROR_CHECK(err);

            buttonMotion(err, mxbuttonEvent);
            MX_ERROR_CHECK(err);
        }
    } else {
        // save the event as we havent yet has the motion start
        // event - 50 should be enough

        if (nmotions < MX_MAX_MOTIONS) {
            motions[nmotions] = *motionEvent;
            nmotions++;
        }
    }

    lastMotionEvent = *motionEvent;

    // add the timer

    stillMotionTimerId = XtAppAddTimeOut(XtWidgetToApplicationContext(cbwidget),
        XtGetMultiClickTime(XtDisplay(cbwidget)),
        (XtTimerCallbackProc)mxCTCallback,
        (XtPointer)this);

    return;
abort:
    return;
}

/*-------------------------------------------------
 * FUNCTION: mx_xcallback_target::processResize
 *
 * DESCRIPTION:
 *
 *
 */

void mx_xcallback_target::processResize(int& err,
    XConfigureEvent* configureEvent)
{
    mx_resize_event mxresizeEvent;

    err = MX_ERROR_OK;

    if (testProcessMask(MX_EVENT_RESIZE)) {
        xevTOmxre(err, mxresizeEvent, configureEvent);
        MX_ERROR_CHECK(err);

        resize(err, mxresizeEvent);
        MX_ERROR_CHECK(err);
    }

    return;
abort:
    return;
}

/*-------------------------------------------------
 * FUNCTION: mx_xcallback_target::processExpose
 *
 * DESCRIPTION:
 *
 *
 */

void mx_xcallback_target::processExpose(int& err,
    XExposeEvent* exposeEvent)
{
    mx_expose_event mxexposeEvent;

    err = MX_ERROR_OK;

    if (testProcessMask(MX_EVENT_EXPOSE)) {
        xevTOmxee(err, mxexposeEvent, exposeEvent);
        MX_ERROR_CHECK(err);

        expose(err, mxexposeEvent);
        MX_ERROR_CHECK(err);
    }

    return;
abort:
    return;
}

/*-------------------------------------------------
 * FUNCTION: mx_xcallback_target::setVerticalScrollWidget
 *
 * DESCRIPTION:
 *
 *
 */

void mx_xcallback_target::setVerticalScrollWidget(int& err, Widget ivscrollWidget)
{
    err = MX_ERROR_OK;

    vscrollWidget = ivscrollWidget;

    XtAddCallback(vscrollWidget, XmNdragCallback, mxXtCallback, this);
    XtAddCallback(vscrollWidget, XmNincrementCallback, mxXtCallback, this);
    XtAddCallback(vscrollWidget, XmNdecrementCallback, mxXtCallback, this);
    XtAddCallback(vscrollWidget, XmNpageIncrementCallback, mxXtCallback, this);
    XtAddCallback(vscrollWidget, XmNpageDecrementCallback, mxXtCallback, this);
    XtAddCallback(vscrollWidget, XmNtoBottomCallback, mxXtCallback, this);
    XtAddCallback(vscrollWidget, XmNtoTopCallback, mxXtCallback, this);
    XtAddCallback(vscrollWidget, XmNvalueChangedCallback, mxXtCallback, this);
}

/*-------------------------------------------------
 * FUNCTION: mx_xcallback_target::setHorizontalScrollWidget
 *
 * DESCRIPTION:
 *
 *
 */

void mx_xcallback_target::setHorizontalScrollWidget(int& err, Widget ihscrollWidget)
{
    err = MX_ERROR_OK;

    hscrollWidget = ihscrollWidget;

    XtAddCallback(hscrollWidget, XmNdragCallback, mxXtCallback, this);
    XtAddCallback(hscrollWidget, XmNincrementCallback, mxXtCallback, this);
    XtAddCallback(hscrollWidget, XmNdecrementCallback, mxXtCallback, this);
    XtAddCallback(hscrollWidget, XmNpageIncrementCallback, mxXtCallback, this);
    XtAddCallback(hscrollWidget, XmNpageDecrementCallback, mxXtCallback, this);
    XtAddCallback(hscrollWidget, XmNtoBottomCallback, mxXtCallback, this);
    XtAddCallback(hscrollWidget, XmNtoTopCallback, mxXtCallback, this);
    XtAddCallback(hscrollWidget, XmNvalueChangedCallback, mxXtCallback, this);
}

/*-------------------------------------------------
 * FUNCTION: mx_xcallback_target::processScroll
 *
 * DESCRIPTION:
 *
 *
 */

void mx_xcallback_target::processScroll(int& err,
    Widget eventWidget,
    XmScrollBarCallbackStruct* scrollData)
{
    mx_scroll_event scrollEvent;

    err = MX_ERROR_OK;

    if (!testProcessMask(MX_EVENT_ALL_SCROLL))
        return;

    xevTOmxse(err, scrollEvent, eventWidget, scrollData);
    MX_ERROR_CHECK(err);

    if (scrollEvent.getType() == MX_EVENT_SCROLLDRAG) {
        if (testProcessMask(MX_EVENT_SCROLLDRAG)) {
            scrollDrag(err, scrollEvent);
            MX_ERROR_CHECK(err);
        }
    } else {
        forceScroll(err, scrollEvent);
        MX_ERROR_CHECK(err);
    }
    return;
abort:
    return;
}

/*-------------------------------------------------
 * FUNCTION: mx_xcallback_target::forceScroll
 *
 * DESCRIPTION:
 *
 *
 */

void mx_xcallback_target::forceScroll(int& err,
    mx_scroll_event& scrollEvent)
{
    err = MX_ERROR_OK;

    if (testProcessMask(MX_EVENT_PRESCROLL)) {
        scrollEvent.setType(MX_EVENT_PRESCROLL);
        preScroll(err, scrollEvent);
        MX_ERROR_CHECK(err);
    }

    if (testProcessMask(MX_EVENT_SCROLL)) {
        scrollEvent.setType(MX_EVENT_SCROLL);
        scroll(err, scrollEvent);
        MX_ERROR_CHECK(err);
    }

    if (testProcessMask(MX_EVENT_POSTSCROLL)) {
        scrollEvent.setType(MX_EVENT_POSTSCROLL);
        postScroll(err, scrollEvent);
        MX_ERROR_CHECK(err);
    }

    return;
abort:
    return;
}
