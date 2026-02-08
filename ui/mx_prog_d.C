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
 * MODULE/CLASS : mx_prog_d
 *
 * AUTHOR : Andrew Haisley
 *
 * This file
 *
 * DESCRIPTION:
 *
 *  A dialog to displat progress.
 *
 *
 *
 */

#include <dirent.h>
#include <errno.h>
#include <pwd.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>

#include "mx_dialog.h"
#include <Xm/DrawingA.h>
#include <Xm/Label.h>
#include <Xm/LabelG.h>
#include <Xm/List.h>
#include <Xm/Protocols.h>
#include <Xm/PushBG.h>
#include <Xm/RowColumn.h>
#include <Xm/Separator.h>
#include <Xm/Text.h>
#include <Xm/Xm.h>
#include <mx.h>
#include <mx_std.h>
#include <mx_ui_object.h>
// #include "mx_yes_no_d.h"
// #include "mx_open_d.h"
#include <mx_file_type.h>

#include "mx_prog_d.h"

static void drawing_area_cb(
    Widget widget,
    XtPointer client_data,
    XtPointer call_data)
{
    mx_prog_d* d = (mx_prog_d*)client_data;
    XmDrawingAreaCallbackStruct* cbs = (XmDrawingAreaCallbackStruct*)call_data;

    if (cbs->reason == XmCR_INPUT) {
        return;
    } else {
        d->set_progress();
    }
}

static void button_cb(
    Widget widget,
    XtPointer client_data,
    XtPointer call_data)
{
    mx_prog_d* d = (mx_prog_d*)client_data;

    if (widget == d->cancel_button) {
        d->modal_result = cancel_e;
    }
}

void mx_prog_d::init(Widget parent, bool has_bar)
{
    XGCValues gcv;
    Display* display;
    XColor screen_colour, exact_colour;

    XtVaSetValues(action_area, XmNfractionBase, 3, NULL);

    // first, the text
    if (has_bar) {
        text = XtVaCreateManagedWidget("text",
            xmLabelWidgetClass, control_area,
            XmNleftAttachment, XmATTACH_FORM,
            XmNrightAttachment, XmATTACH_FORM,
            XmNtopAttachment, XmATTACH_FORM,
            XmNtopOffset, 10,
            XmNbottomOffset, 10,
            XmNleftOffset, 15,
            XmNrightOffset, 15,
            NULL);
    } else {
        text = XtVaCreateManagedWidget("text",
            xmLabelWidgetClass, control_area,
            XmNleftAttachment, XmATTACH_FORM,
            XmNrightAttachment, XmATTACH_FORM,
            XmNbottomAttachment, XmATTACH_FORM,
            XmNtopAttachment, XmATTACH_FORM,
            XmNtopOffset, 10,
            XmNleftOffset, 15,
            XmNrightOffset, 15,
            NULL);
    }

    if (has_bar) {
        drawing_area = XtVaCreateManagedWidget(
            "drawingArea",
            xmDrawingAreaWidgetClass, control_area,
            XmNleftAttachment, XmATTACH_FORM,
            XmNrightAttachment, XmATTACH_FORM,
            XmNtopAttachment, XmATTACH_WIDGET,
            XmNtopWidget, text,
            XmNbottomAttachment, XmATTACH_FORM,
            XmNtopOffset, 10,
            XmNleftOffset, 15,
            XmNrightOffset, 15,
            XmNbottomOffset, 10,
            XmNheight, 70,
            XmNwidth, 350,
            NULL);

        gcv.foreground = BlackPixelOfScreen(XtScreen(drawing_area));
        gc = XCreateGC(
            XtDisplay(drawing_area),
            RootWindowOfScreen(XtScreen(drawing_area)),
            GCForeground,
            &gcv);

        XtAddCallback(
            drawing_area,
            XmNexposeCallback,
            drawing_area_cb,
            (XtPointer)this);
        pct = 0;

        display = XtDisplay(drawing_area);

        black_pixel = BlackPixel(display, 0);

        if (XLookupColor(display,
                DefaultColormap(display, DefaultScreen(display)),
                "grey70", &screen_colour, &exact_colour)) {
            if (XAllocNamedColor(display,
                    DefaultColormap(display, DefaultScreen(display)),
                    "grey70", &screen_colour, &exact_colour)) {
                grey_pixel = exact_colour.pixel;
            }
        } else {
            grey_pixel = black_pixel;
        }
    } else {
        XtVaSetValues(pane, XmNwidth, 350, NULL);
    }

    // now, the action buttons
    cancel_button = XtVaCreateManagedWidget(
        "cancel",
        xmPushButtonGadgetClass, action_area,
        XmNshowAsDefault, False,
        XmNdefaultButtonShadowThickness, 1,
        XmNleftAttachment, XmATTACH_POSITION,
        XmNleftPosition, 1,
        XmNrightAttachment, XmATTACH_POSITION,
        XmNrightPosition, 2,
        NULL);

    XtAddCallback(
        cancel_button,
        XmNactivateCallback,
        button_cb,
        (XtPointer)this);

    XtManageChild(action_area);
    XtManageChild(control_area);
    XtManageChild(pane);
}

mx_prog_d::mx_prog_d(Widget parent, bool has_bar)
    : mx_dialog("progress", parent, TRUE, FALSE)
{
    init(parent, has_bar);
}

void mx_prog_d::set_message(const char* message)
{
    XmString str;

    str = XmStringCreate(const_cast<char*>(message), XmFONTLIST_DEFAULT_TAG);
    XtVaSetValues(text, XmNlabelString, str, NULL);
    XmStringFree(str);
    XFlush(XtDisplay(dialog));
    XmUpdateDisplay(text);
}

void mx_prog_d::activate_d(const char* message)
{
    modal_result = none_e;

    set_message(message);
    centre();

    set_parent_busy();
    XtPopup(dialog, XtGrabNonexclusive);
    mx_dialog::activate();

    XFlush(XtDisplay(dialog));
}

bool mx_prog_d::peek_cancel()
{
    XEvent next_event;

    if (!XtAppPeekEvent(global_app_context, &next_event)) {
        return FALSE;
    }

    while (modal_result == none_e) {
        if (XtAppPending(global_app_context)) {
            mx_ui_object::XtAppNextEvent(global_app_context, &next_event);
            if (next_event.type == KeyPress) {
                if (next_event.xkey.state & Mod1Mask) {
                    if (handle_acc(dialog, &next_event)) {
                        continue;
                    }
                }
            }
            XtDispatchEvent(&next_event);
        } else {
            return FALSE;
        }
    }

    return modal_result == cancel_e;
}

void mx_prog_d::set_progress()
{
    set_progress(pct);
}

void mx_prog_d::set_progress(int percent)
{
    Dimension w, h;
    Display* display;
    Drawable drawable;

    int bx, by, bh, bw;

    display = XtDisplay(drawing_area);
    drawable = XtWindow(drawing_area);

    XtVaGetValues(drawing_area, XmNwidth, &w, XmNheight, &h, NULL);

    bx = 50;
    by = 20;
    bw = (w - (bx * 2));
    bh = (h - (by * 2));

    pct = percent;

    if (percent == 0) {
        // special case, clear out and reset bar
        XClearWindow(display, drawable);
    }

    XSetForeground(display, gc, black_pixel);
    XDrawRectangle(display, drawable, gc, bx, by, bw, bh);

    XSetForeground(display, gc, grey_pixel);
    XFillRectangle(display, drawable, gc,
        bx + 1, by + 1, ((bw - 1) * pct) / 100, bh - 1);

    XFlush(XtDisplay(drawing_area));
}
