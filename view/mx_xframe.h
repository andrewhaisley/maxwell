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
#ifndef _MX_XFRAME_H
#define _MX_XFRAME_H

/*
 * MODULE : mx_xframe.h
 *
 * AUTHOR : David Miller
 *
 * This file
 *
 * DESCRIPTION:
 * Module mx_xframe.h
 *
 *
 *
 *
 */

#include "mx_frame.h"
#include "mx_ui_object.h"
#include "mx_xcb.h"
#include <Xm/Xm.h>

/*-------------------------------------------------
 * CLASS: mx_xframe
 *
 * DESCRIPTION: A frame which has an x widget and
 * x callbacks. This is not a ui object as by default it
 * sets the active widget to be the parent widget. The
 * screen device is attached to the active widget
 */

class mx_xframe : public mx_frame, public mx_xcallback_target {
public:
    mx_xframe(int& err,
        mx_doc_coord_t& initialTopLeft,
        mx_point& initialSize,
        double initialZoom,
        mx_frame_target& iframeTarget,
        Widget parentWidget);

    virtual void createDevice(int& err);
    virtual void resetSheetLayout(int& err);

    virtual void draw(int& err, mx_draw_event& event);

    virtual void setTopLeft(int& err,
        mx_point& itopLeft);

    static void defaultDeviceToFrame(int& err,
        mx_callback_target* source,
        mx_callback_target* dest,
        mx_doc_coord_t& coord,
        MX_COORD_TYPE coordType,
        bool& isInRange);

    static void defaultFrameToFrameTarget(int& err,
        mx_callback_target* source,
        mx_callback_target* dest,
        mx_doc_coord_t& coord,
        MX_COORD_TYPE coordType,
        bool& isInRange);

    virtual void frameScroll(int& err,
        mx_doc_coord_t& scrollPostion,
        MX_SCROLL_OFFSET offset,
        bool sameX,
        bool sameY,
        mx_doc_coord_t& actualScrollPosition,
        bool doScroll);
    virtual void scroll(int& err,
        mx_doc_coord_t& scrollPostion,
        MX_SCROLL_OFFSET offset,
        bool sameX,
        bool sameY,
        mx_doc_coord_t& actualScrollPosition);

    virtual void buttonMotion(int& err,
        mx_button_event& buttonEvent);

    virtual void resize(int& err,
        mx_resize_event& event);

    virtual void setZoom(int& err, double izoom);

    // if true the frame will keep button motion events
    // within the displayed area
    inline void setMouseScrolling(bool imouseScrolling) { mouseScrolling = imouseScrolling; };
    inline int getMouseScrolling() { return mouseScrolling; };

    // the low level scroll event
    virtual void scroll(int& err,
        mx_scroll_event& scrollEvent);

    // the low level scroll event - used to set up the
    // event
    virtual void preScroll(int& err,
        mx_scroll_event& scrollEvent);

    void makeVisible(int& err,
        mx_doc_coord_t& visiblePos,
        double extra_offset = 0.0);

    // make an area visible as far as possible.
    void makeVisible(int& err,
        mx_doc_coord_t& top_left,
        mx_doc_coord_t& bottom_right,
        double extra_offset = 0.0);

protected:
    // the parent widget of the frame
    Widget parentWidget;
    // the active widget to which the device is to be attached
    Widget activeWidget;

    void scrollEventToTL(mx_scroll_event& scrollEvent,
        mx_doc_coord_t& scrollPosition,
        bool& sameX,
        bool& sameY,
        MX_SCROLL_OFFSET& offset);

private:
    // whether the frame is to chase the mouse pointer
    bool mouseScrolling;

    static MX_SCROLL_OFFSET scrollOffsets[3][3];

    void makeVisibleInternal(int& err,
        mx_doc_coord_t& top_left,
        mx_doc_coord_t& bottom_right,
        double extra_offset);
};

/*-------------------------------------------------
 * CLASS: mx_xfframe
 *
 * DESCRIPTION: A frame which has an x widget and
 * x callbacks. This is a ui object as the active
 * widget is a frame widget
 *
 */

class mx_xfframe : public mx_xframe, public mx_ui_object {
public:
    mx_xfframe(int& err,
        mx_doc_coord_t& initialTopLeft,
        mx_point& initialSize,
        double initialZoom,
        mx_frame_target& iframeTarget,
        Widget parentWidget);
};

/*-------------------------------------------------
 * CLASS: mx_scrollable_frame
 *
 * DESCRIPTION: An xframe which is scrollable
 *
 *
 */

class mx_scrollable_frame : public mx_xframe, public mx_ui_object {
public:
    mx_scrollable_frame(int& err,
        mx_doc_coord_t& initialTopLeft,
        mx_point& initialSize,
        double initialZoom,
        mx_frame_target& iframeTarget,
        Widget parentWidget,
        bool createframeWidget,
        mx_point& scrollIncrement,
        bool isAutomatic,
        mx_rect* iscrollAdd = NULL);

    void scrollDrag(int& err,
        mx_scroll_event& scrollEvent);

    void resize(int& err,
        mx_resize_event& event);

    inline mx_point getScrollIncrement() { return scrollIncrement; };
    void setScrollIncrement(int& err, mx_point& iscrollIncrement);

    void setScrollAdd(int& err, mx_rect& sadd);
    inline mx_rect getScrollAdd() { return scrollAdd; };

    void resetSheetLayout(int& err);

    // get the current external selection
    virtual void requestSelection(int& err);

    // tell the frame an external selection is available
    virtual void externalSelectionAvailable();

    void scroll(int& err,
        mx_doc_coord_t& scrollPostion,
        MX_SCROLL_OFFSET offset,
        bool sameX,
        bool sameY,
        mx_doc_coord_t& actualScrollPosition);

    void setTopLeft(int& err,
        mx_point& itopLeft);

    void setSize(int& err,
        const mx_point& isize);

    void setZoom(int& err, double izoom);

    void frameScroll(int& err,
        mx_doc_coord_t& scrollPostion,
        MX_SCROLL_OFFSET offset,
        bool sameX,
        bool sameY,
        mx_doc_coord_t& actualScrollPosition,
        bool doScroll);

private:
    // the frame widget
    Widget frameWidget;

    // whether the frame is automatic
    bool isAutomatic;

    mx_point scrollIncrement;

    void setScrollBars(int& err);

    mx_rect scrollAdd;

    void externalSelectionCB(char* s);

    static Bool convert_selection_proc(
        Widget w,
        Atom* selection,
        Atom* target,
        Atom* type_return,
        XtPointer* value_return,
        unsigned long* length_return,
        int* format_return);

    const char* getExternalSelection(int& err);

    static void selection_cb(
        Widget w,
        XtPointer client_data,
        Atom* selection,
        Atom* type,
        XtPointer value,
        unsigned long* value_length,
        int* format);

    static mx_scrollable_frame* frame_owning_selection;
};

#endif
