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
 * MODULE/CLASS : mx_symbol_d
 *
 * AUTHOR : Andrew Haisley
 *
 * This file
 *
 * DESCRIPTION:
 *
 *  A symbol selection dialog.
 *
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
#include <mx_font.h>

#include "mx_symbol_d.h"

static void drawing_area_cb(
    Widget widget,
    XtPointer client_data,
    XtPointer call_data)
{
    mx_symbol_d* d = (mx_symbol_d*)client_data;
    XmDrawingAreaCallbackStruct* cbs = (XmDrawingAreaCallbackStruct*)call_data;

    XEvent* event = cbs->event;

    switch (cbs->reason) {
    case XmCR_INPUT:
        d->handle_input(event);
        break;
    case XmCR_EXPOSE:
    case XmCR_RESIZE:
        d->draw_symbols();
        break;
    default:
        break;
    }
}

static void button_cb(
    Widget widget,
    XtPointer client_data,
    XtPointer call_data)
{
    mx_symbol_d* d = (mx_symbol_d*)client_data;

    if (widget == d->dismiss_button) {
        d->modal_result = cancel_e;
    }
}

mx_symbol_d::mx_symbol_d(Widget parent, mx_font *f) : mx_dialog("symbol", parent, TRUE, TRUE)
{
    XGCValues gcv;
    Display* display;

    m_font = f;

    XtVaSetValues(action_area, XmNfractionBase, 3, NULL);

    drawing_area = XtVaCreateManagedWidget(
        "rowColumn",
        xmDrawingAreaWidgetClass, control_area,
        XmNbottomAttachment, XmATTACH_FORM,
        XmNtopAttachment, XmATTACH_FORM,
        XmNleftAttachment, XmATTACH_FORM,
        XmNrightAttachment, XmATTACH_FORM,
        XmNwidth, 400,
        XmNheight, 400,
        NULL);

    XtAddCallback(
        drawing_area,
        XmNinputCallback,
        drawing_area_cb,
        (XtPointer)this);

    XtAddCallback(
        drawing_area,
        XmNexposeCallback,
        drawing_area_cb,
        (XtPointer)this);

    gcv.foreground = BlackPixelOfScreen(XtScreen(drawing_area));
    gcv.background = WhitePixelOfScreen(XtScreen(drawing_area));
    display = XtDisplay(drawing_area);

    gc = XCreateGC(
        display,
        RootWindowOfScreen(XtScreen(drawing_area)),
        GCForeground | GCBackground,
        &gcv);

    // now, the action button
    dismiss_button = XtVaCreateManagedWidget(
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
        dismiss_button,
        XmNactivateCallback,
        button_cb,
        (XtPointer)this);

    XtManageChild(drawing_area);
    XtManageChild(action_area);
    XtManageChild(control_area);
    XtManageChild(pane);
}

void mx_symbol_d::activate_d(mx_font* f)
{
    m_font = f;

    draw_symbols();
    mx_dialog::activate();
}

int mx_symbol_d::run(mx_font* f)
{
    int res;

    centre();
    activate_d(f);
    res = run_modal();
    deactivate();

    return res;
}

void mx_symbol_d::draw_symbols()
{
    int i, j, ho, sw, err;
    Dimension width, height;
    Display* display;
    Drawable drawable;

    unsigned char s[2];

    display = XtDisplay(drawing_area);
    drawable = XtWindow(drawing_area);

    XtVaGetValues(drawing_area, XmNwidth, &width, XmNheight, &height, NULL);
    XClearWindow(display, drawable);

    m_font->set_size(14.0);

    XftColor xft_color;
    XRenderColor xr_black = {0x0000, 0x0000, 0x0000, 0xffff};
    XftColorAllocValue(display, DefaultVisual(display, DefaultScreen(display)), DefaultColormap(display, DefaultScreen(display)), &xr_black, &xft_color);
    auto draw = XftDrawCreate(display, drawable, DefaultVisual(display, DefaultScreen(display)), DefaultColormap(display, DefaultScreen(display)));

    auto xft_fs = m_font->get_xft_font(err, display, 1.0);
    MX_ERROR_CHECK(err);

    /* draw the grid first */
    for (i = 0; i <= width; i += (width / 16)) {
        XDrawLine(display, drawable, gc, i, 0, i, height);
    }

    XDrawLine(display, drawable, gc, width - 1, 0, width - 1, height);

    for (j = 0; j <= height; j += (height / 16)) {
        XDrawLine(display, drawable, gc, 0, j, width, j);
    }

    XDrawLine(display, drawable, gc, 0, height - 1, width, height - 1);

    ho = ((height / 16) - xft_fs->ascent) / 2;

    for (j = 0; j < 16; j++) {
        for (i = 0; i < 16; i++) {
            s[0] = j * 16 + i;
            s[1] = 0;
            if (s[0] == 0) {
                s[0] = ' ';
            }

            sw = mx_font::get_xft_width(xft_fs, display, (char *)s, 1);
            XftDrawStringUtf8(
                    draw, 
                    &xft_color, 
                    xft_fs, 
                    (width / 16) * i + (((width / 16) - sw) / 2), 
                    (height / 16) * (j + 1) - ho, 
                    (const FcChar8 *)s, 
                    1);

#if 0
            sw = XTextWidth(fs, (char*)s, 1);
            XDrawString(display, drawable, gc,
                (width / 16) * i + (((width / 16) - sw) / 2),
                (height / 16) * (j + 1) - ho,
                (char*)s,
                1);
#endif
        }
    }
abort:;
}

void mx_symbol_d::handle_input(XEvent* e)
{
    int i1, i2, j1, j2;

    if (e->xany.type == ButtonPress) {
        xor_square(e->xbutton.x, e->xbutton.y);
        last_x = e->xbutton.x;
        last_y = e->xbutton.y;
    } else {
        if (e->xany.type == ButtonRelease) {
            square_pos(last_x, last_y, i1, j1);
            square_pos(e->xbutton.x, e->xbutton.y, i2, j2);
            xor_square(last_x, last_y);

            if (i1 == i2 && j1 == j2) {
                selected_character = i1 + j1 * 16;
                modal_result = yes_e;
            }
        }
    }
}

void mx_symbol_d::square_pos(int x, int y, int& i, int& j)
{
    Dimension width, height;

    XtVaGetValues(
        drawing_area,
        XmNwidth, &width,
        XmNheight, &height,
        NULL);

    i = (x / (width / 16));
    j = (y / (height / 16));
}

void mx_symbol_d::xor_square(int x, int y)
{
    Dimension width, height;
    int i, j;
    Display* display;
    Drawable drawable;

    display = XtDisplay(drawing_area);
    drawable = XtWindow(drawing_area);

    XtVaGetValues(
        drawing_area,
        XmNwidth, &width,
        XmNheight, &height,
        NULL);

    square_pos(x, y, i, j);

    XSetFunction(display, gc, GXxor);
    XSetFillStyle(display, gc, FillSolid);
    XSetForeground(display, gc, 1);

    XFillRectangle(display, drawable, gc,
        i * (width / 16),
        j * (height / 16),
        width / 16,
        height / 16);

    XSetFunction(display, gc, GXcopy);
    XSetForeground(
        display,
        gc,
        BlackPixelOfScreen(XtScreen(drawing_area)));

    XFlush(display);
}
