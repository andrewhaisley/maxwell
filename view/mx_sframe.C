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
 * MODULE : mx_sframe.C
 *
 * AUTHOR : David Miller
 *
 * This file
 *
 * DESCRIPTION:
 * Module mx_sframe.C
 *
 *
 *
 *
 */

#include <Xm/Frame.h>
#include <Xm/ScrollBar.h>
#include <Xm/ScrolledW.h>
#include <mx_sc_device.h>
#include <mx_xframe.h>

mx_scrollable_frame* mx_scrollable_frame::frame_owning_selection = NULL;

// set 3x3 array of offsets
MX_SCROLL_OFFSET mx_xframe::scrollOffsets[3][3] = { { MX_SCROLL_OFFSET_TL, MX_SCROLL_OFFSET_T, MX_SCROLL_OFFSET_TR },
    { MX_SCROLL_OFFSET_L, MX_SCROLL_OFFSET_C, MX_SCROLL_OFFSET_R },
    { MX_SCROLL_OFFSET_BL, MX_SCROLL_OFFSET_B, MX_SCROLL_OFFSET_BR } };

/*-------------------------------------------------
 * FUNCTION: mx_scrollable_frame::mx_scrollable_frame
 *
 * DESCRIPTION: Frame constructor for a simple
 * frame which can hold a device in a scrollable
 * window
 */

mx_scrollable_frame::mx_scrollable_frame(int& err,
    mx_doc_coord_t& initialTopLeft,
    mx_point& initialSize,
    double initialZoom,
    mx_frame_target& iframeTarget,
    Widget iparentWidget,
    bool createFrameWidget,
    mx_point& iscrollIncrement,
    bool iisAutomatic,
    mx_rect* iscrollAdd)
    : mx_xframe(err,
          initialTopLeft,
          initialSize,
          initialZoom,
          iframeTarget,
          iparentWidget)
    , scrollIncrement(iscrollIncrement)
{
    Widget vertSB, horzSB;

    MX_ERROR_CHECK(err);

    isAutomatic = iisAutomatic;
    setMouseScrolling(FALSE);
    setApplyScrollLimitToTarget(TRUE);

    setProcessMask(MX_EVENT_SFRAME);

    if (iscrollAdd != NULL)
        scrollAdd = *iscrollAdd;

    widget = XtVaCreateManagedWidget("scolledWindow", xmScrolledWindowWidgetClass,
        parentWidget, 0, NULL);

    if (createFrameWidget) {
        activeWidget = frameWidget = XtVaCreateManagedWidget("frame", xmFrameWidgetClass,
            widget, NULL);
    } else {
        activeWidget = widget;
        frameWidget = NULL;
    }

    // create the output device

    createDevice(err);
    MX_ERROR_CHECK(err);

    vertSB = XtVaCreateManagedWidget("verticalSB", xmScrollBarWidgetClass,
        widget,
        XmNorientation, XmVERTICAL,
        NULL);

    horzSB = XtVaCreateManagedWidget("horizontalSB", xmScrollBarWidgetClass,
        widget,
        XmNorientation, XmHORIZONTAL,
        NULL);

    // set the vertical scrolling widget on the callback target
    setVerticalScrollWidget(err, vertSB);
    MX_ERROR_CHECK(err);

    // set the horizontal scrolling widget on the callback target
    setHorizontalScrollWidget(err, horzSB);
    MX_ERROR_CHECK(err);

    setScrollBars(err);
    MX_ERROR_CHECK(err);

    // create the scrolling entity
    XmScrolledWindowSetAreas(widget, horzSB, vertSB, activeWidget);

abort:;
}

/*-------------------------------------------------
 * FUNCTION: mx_scrollable_frame::resetSheetLayout
 *
 * DESCRIPTION: Frame constructor for a simple
 * frame which can hold a device in a scrollable
 * window
 */

void mx_scrollable_frame::resetSheetLayout(int& err)
{
    err = MX_ERROR_OK;

    setScrollBars(err);
    MX_ERROR_CHECK(err);

    return;
abort:
    return;
}

/*-------------------------------------------------
 * FUNCTION: mx_scrollable_frame::scroll
 *
 * DESCRIPTION: Scoll a window in frame target
 * coordinates .
 *
 */

void mx_scrollable_frame::scroll(int& err,
    mx_doc_coord_t& scrollPosition,
    MX_SCROLL_OFFSET offset,
    bool sameX,
    bool sameY,
    mx_doc_coord_t& actualScrollPosition)
{
    err = MX_ERROR_OK;

    mx_xframe::scroll(err, scrollPosition, offset, sameX, sameY,
        actualScrollPosition);
    MX_ERROR_CHECK(err);

    setScrollBars(err);
    MX_ERROR_CHECK(err);

    return;
abort:
    return;
}

/*-------------------------------------------------
 * FUNCTION: mx_scrollable_frame::scrollDrag
 *
 * DESCRIPTION: Scroll a window in frame target
 * coordinates .
 *
 */

void mx_scrollable_frame::scrollDrag(int& err,
    mx_scroll_event& scrollEvent)
{
    mx_doc_coord_t scrollPosition;
    bool sameX, sameY;
    MX_SCROLL_OFFSET offset;

    err = MX_ERROR_OK;

    scrollEventToTL(scrollEvent, scrollPosition, sameX, sameY, offset);

    scrollEvent.setOldTopLeft(scrollPosition);
    scrollEvent.setNewTopLeft(scrollPosition);

    // propagate event
    mx_callback_target::scrollDrag(err, scrollEvent);
    MX_ERROR_CHECK(err);

abort:;
}

/*-------------------------------------------------
 * FUNCTION: mx_scrollable_frame::setScrollBars
 *
 * DESCRIPTION: Scoll a window in frame
 * coordinates.
 *
 */

void mx_scrollable_frame::setScrollBars(int& err)
{
    Widget vertSB, horzSB;
    int iarg;
    Arg args[6];
    int sliderSizev, minSizev, maxSizev, topLeftv, totalSizev, siv, sizev;
    mx_point totalSize;
    int addt, addb;

    err = MX_ERROR_OK;

    // get the size of the frameable target in frame coordinates
    totalSize = getFrameTarget().getTotalSize(err);
    MX_ERROR_CHECK(err);

    vertSB = getVerticalScrollWidget();
    horzSB = getHorizontalScrollWidget();

    if (topLeft.y > 0) {
        topLeftv = (int)(topLeft.y * 100 + 0.5);
    } else {
        topLeftv = (int)(topLeft.y * 100 - 0.5);
    }

    addb = (int)(scrollAdd.yb * 100 + 0.5);
    addt = (int)(scrollAdd.yt * 100 + 0.5);
    totalSizev = (int)(totalSize.y * 100 + 0.5);
    sizev = (int)(size.y * 100 + 0.5);
    siv = (int)(scrollIncrement.y * 100 + 0.5);

    maxSizev = GMAX(topLeftv + sizev, totalSizev + addt);
    minSizev = GMIN(topLeftv, -addb);
    sliderSizev = GMIN(sizev, maxSizev - minSizev);
    if (sliderSizev == 0)
        sliderSizev = 1;
    if (sliderSizev > (maxSizev - topLeftv))
        sliderSizev = maxSizev - topLeftv;

    iarg = 0;
    XtSetArg(args[iarg], XmNpageIncrement, sliderSizev);
    iarg++;
    XtSetArg(args[iarg], XmNmaximum, maxSizev);
    iarg++;
    XtSetArg(args[iarg], XmNminimum, minSizev);
    iarg++;
    XtSetArg(args[iarg], XmNsliderSize, sliderSizev);
    iarg++;
    XtSetArg(args[iarg], XmNvalue, topLeftv);
    iarg++;
    XtSetArg(args[iarg], XmNincrement, siv);
    iarg++;
    XtSetValues(vertSB, args, iarg);

    if (topLeft.y > 0) {
        topLeftv = (int)(topLeft.x * 100 + 0.5);
    } else {
        topLeftv = (int)(topLeft.x * 100 - 0.5);
    }

    addb = (int)(scrollAdd.xb * 100 + 0.5);
    addt = (int)(scrollAdd.xt * 100 + 0.5);
    totalSizev = (int)(totalSize.x * 100 + 0.5);
    sizev = (int)(size.x * 100 + 0.5);
    siv = (int)(scrollIncrement.x * 100 + 0.5);

    maxSizev = GMAX(topLeftv + sizev, totalSizev + addt);
    minSizev = GMIN(topLeftv, -addb);
    sliderSizev = GMIN(sizev, maxSizev - minSizev);
    if (sliderSizev == 0)
        sliderSizev = 1;
    if (sliderSizev > (maxSizev - topLeftv))
        sliderSizev = maxSizev - topLeftv;

    iarg = 0;
    XtSetArg(args[iarg], XmNpageIncrement, sliderSizev);
    iarg++;
    XtSetArg(args[iarg], XmNmaximum, maxSizev);
    iarg++;
    XtSetArg(args[iarg], XmNminimum, minSizev);
    iarg++;
    XtSetArg(args[iarg], XmNsliderSize, sliderSizev);
    iarg++;
    XtSetArg(args[iarg], XmNvalue, topLeftv);
    iarg++;
    XtSetArg(args[iarg], XmNincrement, siv);
    iarg++;
    XtSetValues(horzSB, args, iarg);

abort:;
}

/*-------------------------------------------------
 * FUNCTION: mx_scrollable_frame::resize
 *
 * DESCRIPTION: Scoll a
 * coordinates.
 *
 */

void mx_scrollable_frame::resize(int& err,
    mx_resize_event& event)
{
    err = MX_ERROR_OK;

    mx_xframe::resize(err, event);
    MX_ERROR_CHECK(err);

    setScrollBars(err);
    MX_ERROR_CHECK(err);

    return;
abort:
    return;
}

/*-------------------------------------------------
 * FUNCTION: mx_scrollable_frame::SetTopLeft
 *
 * DESCRIPTION:
 *
 *
 */

void mx_scrollable_frame::setTopLeft(int& err,
    mx_point& itopLeft)
{
    err = MX_ERROR_OK;

    mx_xframe::setTopLeft(err, itopLeft);
    MX_ERROR_CHECK(err);

    setScrollBars(err);
    MX_ERROR_CHECK(err);

    return;
abort:
    return;
}

/*-------------------------------------------------
 * FUNCTION: mx_scrollable_frame::setSize
 *
 * DESCRIPTION:
 *
 *
 */

void mx_scrollable_frame::setSize(int& err,
    const mx_point& isize)
{
    // cant force a size change

    MX_ERROR_THROW(err, MX_SCROLL_SIZE_CHANGE);
    return;
abort:
    return;
}

/*-------------------------------------------------
 * FUNCTION: mx_scrollable_frame::setZoom
 *
 * DESCRIPTION:
 *
 *
 */

void mx_scrollable_frame::setZoom(int& err,
    double izoom)
{
    err = MX_ERROR_OK;

    mx_xframe::setZoom(err, izoom);
    MX_ERROR_CHECK(err);

    setScrollBars(err);
    MX_ERROR_CHECK(err);

    return;
abort:
    return;
}

/*-------------------------------------------------
 * FUNCTION: mx_scrollable_frame::frameScroll
 *
 * DESCRIPTION:
 *
 *
 */

void mx_scrollable_frame::frameScroll(int& err,
    mx_doc_coord_t& scrollPosition,
    MX_SCROLL_OFFSET offset,
    bool sameX,
    bool sameY,
    mx_doc_coord_t& actualScrollPosition,
    bool doScroll)
{
    err = MX_ERROR_OK;

    mx_xframe::frameScroll(err, scrollPosition,
        offset, sameX, sameY, actualScrollPosition,
        doScroll);
    MX_ERROR_CHECK(err);

    if (doScroll) {
        setScrollBars(err);
        MX_ERROR_CHECK(err);
    }

    return;
abort:
    return;
}

/*-------------------------------------------------
 * FUNCTION: mx_scrollable_frame::setScrollIncrement
 *
 * DESCRIPTION:
 *
 *
 */

void mx_scrollable_frame::setScrollIncrement(int& err,
    mx_point& increment)
{
    err = MX_ERROR_OK;

    scrollIncrement = increment;

    setScrollBars(err);
    MX_ERROR_CHECK(err);

    return;
abort:
    return;
}

/*-------------------------------------------------
 * FUNCTION: mx_scrollable_frame::setScrollAdd
 *
 * DESCRIPTION:
 *
 *
 */

void mx_scrollable_frame::setScrollAdd(int& err,
    mx_rect& Add)
{
    err = MX_ERROR_OK;

    scrollAdd = Add;

    setScrollBars(err);
    MX_ERROR_CHECK(err);

    return;
abort:
    return;
}

void mx_scrollable_frame::externalSelectionCB(char* s)
{
    int err = MX_ERROR_OK;

    getFrameTarget().externalSelection(err, s);
    MX_ERROR_CHECK(err);

    return;

abort:
    global_error_trace->print();
    MX_ERROR_CLEAR(err);
}

void mx_scrollable_frame::requestSelection(int& err)
{
    Time timestamp;
    Display* display;

    display = XtDisplay(frameWidget);

    timestamp = XtLastTimestampProcessed(display);

    XtGetSelectionValue(frameWidget,
        XA_PRIMARY,
        XA_STRING,
        (XtSelectionCallbackProc)selection_cb,
        (XtPointer*)this,
        timestamp);
}

void mx_scrollable_frame::selection_cb(
    Widget w,
    XtPointer client_data,
    Atom* selection,
    Atom* type,
    XtPointer value,
    unsigned long* value_length,
    int* format)
{
    mx_scrollable_frame* frame = (mx_scrollable_frame*)client_data;
    char s[10001];

    if (value == NULL)
        return;

    strncpy(s, (char*)value, 10000);
    s[10000] = 0;

    if (*value_length <= 10000) {
        s[*value_length] = 0;
    }

    frame->externalSelectionCB(s);
}

void mx_scrollable_frame::externalSelectionAvailable()
{
    Time timestamp;
    Display* display;

    display = XtDisplay(frameWidget);

    timestamp = XtLastTimestampProcessed(display);

    frame_owning_selection = this;

    (void)XtOwnSelection(
        frameWidget,
        XA_PRIMARY,
        timestamp,
        (XtConvertSelectionProc)convert_selection_proc,
        NULL,
        NULL);
}

Bool mx_scrollable_frame::convert_selection_proc(
    Widget w,
    Atom* selection,
    Atom* target,
    Atom* type_return,
    XtPointer* value_return,
    unsigned long* length_return,
    int* format_return)
{
    int err = MX_ERROR_OK;

    if ((*selection != XA_PRIMARY) || (*target != XA_STRING)) {
        return False;
    }

    if (frame_owning_selection != NULL) {
        const char* value;

        value = frame_owning_selection->getExternalSelection(err);
        MX_ERROR_CHECK(err);

        if (value == NULL) {
            return False;
        } else {
            *length_return = strlen(value);
            *value_return = XtMalloc(*length_return + 1);
            strcpy((char*)*value_return, value);
            *format_return = 8;
            *type_return = XA_STRING;
            return True;
        }
    }
abort:
    global_error_trace->print();
    MX_ERROR_CLEAR(err);
    return False;
}

const char* mx_scrollable_frame::getExternalSelection(int& err)
{
    const char* res;

    res = getFrameTarget().getExternalSelection(err);
    MX_ERROR_CHECK(err);

abort:
    return res;
}
