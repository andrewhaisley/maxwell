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
#ifndef MX_XCB_H
#define MX_XCB_H
/*
 * MODULE : mx_xcb.h
 *
 * AUTHOR : David Miller
 *
 * This file
 *
 * DESCRIPTION: Treatment of X callbacks
 * Module mx_xcb.h
 *
 *
 *
 *
 */

#include <X11/Intrinsic.h>
#include <Xm/ScrollBar.h>
#include <Xm/Xm.h>
#include <mx_cb.h>

#define MX_MAX_MOTIONS 50

class mx_xcallback_target : public mx_callback_target {
public:
    mx_xcallback_target(Widget iwidget = NULL);

    void setCBwidget(Widget iwidget);
    inline Widget getCBWidget() { return cbwidget; };
    void setVerticalScrollWidget(int& err, Widget vscrollWidget);
    void setHorizontalScrollWidget(int& err, Widget hscrollWidget);

    inline Widget getVerticalScrollWidget() { return vscrollWidget; };
    inline Widget getHorizontalScrollWidget() { return hscrollWidget; };

    virtual void expose(int& err, mx_expose_event& event)
    {
        mx_callback_target::expose(err, event);
    };

    virtual void resize(int& err, mx_resize_event& event)
    {
        mx_callback_target::resize(err, event);
    };

    virtual void preDraw(int& err, mx_draw_event& event)
    {
        mx_callback_target::preDraw(err, event);
    };

    virtual void specialDraw(int& err, mx_draw_event& event)
    {
        mx_callback_target::specialDraw(err, event);
    };

    virtual void draw(int& err, mx_draw_event& event)
    {
        mx_callback_target::draw(err, event);
    };

    virtual void postDraw(int& err, mx_draw_event& event)
    {
        mx_callback_target::postDraw(err, event);
    };

    void forceScroll(int& err,
        mx_scroll_event& scrollEvent);

    void setEvent(int& err,
        unsigned long int eventType,
        bool setOn);

private:
    // the widget for the x callback target
    Widget cbwidget;

    // scrolling widgets
    Widget vscrollWidget;
    Widget hscrollWidget;

    // true if dragging is occuring
    bool buttonIsInMotion;

    // the number of button clicks
    int numberButtonClicks;

    // the timer id
    XtIntervalId clickTimerId;

    // the timer id
    XtIntervalId stillMotionTimerId;

    // test for button release
    bool hadButtonRelease;

    // Cumulative button mode
    bool cumulativeButtonMode;

    // the first of a set of events for button presses
    XButtonEvent firstEvent;

    // the current mask
    EventMask eventMask;

    // saved motion events awaiting the timeout after a key press
    int nmotions;
    XMotionEvent motions[MX_MAX_MOTIONS];

    XMotionEvent lastMotionEvent;

    XButtonEvent lastReleaseEvent;

    // the mx button motion callback
    static void mxXtEH(Widget widget,
        XtPointer clientdata,
        XEvent* event,
        bool* cont_dispatch);

    // Motif callbacks
    static void mxXtCallback(Widget widget,
        XtPointer clientData,
        XtPointer callData);

    // button click and button motion timeout callback
    static void mxCTCallback(XtPointer client_data,
        XtIntervalId* timer_id);

    void processTimeout(int& err,
        XtIntervalId timer_id);

    void processClickTimeout(int& err);

    void processStillMotionTimeout(int& err);

    void processKeyPress(int& err,
        XKeyEvent* keyEvent);

    void processKeyRelease(int& err,
        XKeyEvent* keyEvent);

    void processButtonPress(int& err,
        XButtonEvent* keyEvent);

    void processButtonMotion(int& err,
        XMotionEvent* keyEvent);

    void processButtonRelease(int& err,
        XButtonEvent* keyEvent);

    void processButtonClicks(int& err);

    void processResize(int& err,
        XConfigureEvent* configureEvent);

    void processExpose(int& err,
        XExposeEvent* exposeEvent);

    void processScroll(int& err,
        Widget eventWidget,
        XmScrollBarCallbackStruct* scrollData);

    // convert an x event to a Maxwell button event
    void xevTOmxbe(int& err,
        unsigned long int type,
        int nclicks,
        mx_button_event& mxbe,
        XButtonEvent* sxbe,
        XButtonEvent* xbe,
        XMotionEvent* xme);

    int xevTOmxke(int& err,
        bool isRelease,
        mx_key_event& mxke,
        XKeyEvent* xke);

    void xevTOmxee(int& err,
        mx_expose_event& mxee,
        XExposeEvent* xee);

    void xevTOmxre(int& err,
        mx_resize_event& mxre,
        XConfigureEvent* xre);

    void xevTOmxse(int& err,
        mx_scroll_event& mxse,
        Widget eventWidget,
        XmScrollBarCallbackStruct* scrollData);
};

#endif
