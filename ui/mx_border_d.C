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
 * MODULE/CLASS : mx_border_d
 *
 * AUTHOR : Andrew Haisley
 *
 * This file
 *
 * DESCRIPTION:
 *
 *  A border style dialog
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

#include <Xm/ComboBox.h>
#include <Xm/DrawingA.h>
#include <Xm/Form.h>
#include <Xm/Frame.h>
#include <Xm/Label.h>
#include <Xm/LabelG.h>
#include <Xm/List.h>
#include <Xm/Protocols.h>
#include <Xm/PushBG.h>
#include <Xm/RowColumn.h>
#include <Xm/Separator.h>
#include <Xm/Text.h>
#include <Xm/ToggleB.h>
#include <Xm/ToggleBG.h>
#include <Xm/Xm.h>
#include <mx.h>
#include <mx_file_type.h>
#include <mx_std.h>
#include <mx_ui_object.h>

#include "mx_border_d.h"
#include <mx_bd_style.h>
#include <mx_nlist.h>

static void drawing_area_cb(
    Widget widget,
    XtPointer client_data,
    XtPointer call_data)
{
    mx_border_d* d = (mx_border_d*)client_data;
    XmDrawingAreaCallbackStruct* cbs = (XmDrawingAreaCallbackStruct*)call_data;

    XEvent* event = cbs->event;

    if (cbs->reason == XmCR_INPUT) {
        d->handle_input(event);
    } else {
        d->draw_sample();
    }
}

static void line_colour_cb(Widget widget,
    XtPointer client_data,
    XtPointer call_data)
{
    mx_border_d* d = (mx_border_d*)client_data;
    d->set_line_colour(widget);
}

static void bg_colour_cb(Widget widget,
    XtPointer client_data,
    XtPointer call_data)
{
    mx_border_d* d = (mx_border_d*)client_data;
    d->set_bg_colour(widget);
}

static void button_cb(
    Widget widget,
    XtPointer client_data,
    XtPointer call_data)
{
    mx_border_d* d = (mx_border_d*)client_data;

    if (widget == d->ok_button) {
        d->get_options();
        d->modal_result = yes_e;
    } else {
        if (widget == d->apply_button) {
            d->get_options();
            d->modal_result = apply_e;
        } else {
            d->modal_result = cancel_e;
        }
    }
}

static void style_cb(
    Widget widget,
    XtPointer client_data,
    XtPointer call_data)
{
    mx_border_d* d = (mx_border_d*)client_data;

    if (widget == d->dashed_button) {
        d->line_type = mx_dashed;
    } else {
        if (widget == d->dotted_button) {
            d->line_type = mx_dotted;
        } else {
            d->line_type = mx_solid;
        }
    }
}

mx_border_d::mx_border_d(Widget parent, bool has_apply)
    : mx_dialog("border", parent, TRUE, FALSE)
{
    Arg args[20];
    Widget frame1, type_menu, type_sub_menu, line_colour_menu,
        line_colour_sub_menu, bg_colour_menu, bg_colour_sub_menu;
    Widget label_rc, control_rc;
    Widget distanceCombo, widthCombo;

    XGCValues gcv;

    current_colour_name = "black";

    XtVaSetValues(action_area, XmNfractionBase, has_apply ? 7 : 5, NULL);

    frame1 = XtVaCreateManagedWidget(
        "frame1",
        xmFrameWidgetClass,
        control_area,
        XmNtopAttachment, XmATTACH_FORM,
        XmNleftAttachment, XmATTACH_FORM,
        XmNbottomAttachment, XmATTACH_FORM,
        NULL);

    drawing_area = XtVaCreateManagedWidget(
        "drawingArea",
        xmDrawingAreaWidgetClass, frame1,
        XmNchildType, XmFRAME_WORKAREA_CHILD,
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

    XtAddCallback(
        drawing_area,
        XmNinputCallback,
        drawing_area_cb,
        (XtPointer)this);

    label_rc = XtVaCreateManagedWidget(
        "labelRC",
        xmRowColumnWidgetClass,
        control_area,
        XmNtopAttachment, XmATTACH_FORM,
        XmNorientation, XmVERTICAL,
        XmNleftAttachment, XmATTACH_WIDGET,
        XmNleftWidget, frame1,
        NULL);

    control_rc = XtVaCreateManagedWidget(
        "controlRC",
        xmRowColumnWidgetClass,
        control_area,
        XmNtopAttachment, XmATTACH_FORM,
        XmNorientation, XmVERTICAL,
        XmNleftAttachment, XmATTACH_WIDGET,
        XmNleftWidget, label_rc,
        NULL);

    XtVaCreateManagedWidget("widthLabel", xmLabelGadgetClass, label_rc, NULL);
    XtVaCreateManagedWidget("distanceLabel", xmLabelGadgetClass, label_rc, NULL);
    XtVaCreateManagedWidget("styleLabel", xmLabelGadgetClass, label_rc, NULL);
    XtVaCreateManagedWidget("lineColourLabel", xmLabelGadgetClass, label_rc, NULL);
    XtVaCreateManagedWidget("bgColourLabel", xmLabelGadgetClass, label_rc, NULL);

    widthCombo = XmCreateDropDownComboBox(control_rc, const_cast<char*>("widthCombo"), NULL, 0);

    size_text = XtNameToWidget(widthCombo, "*Text");
    size_list = XtNameToWidget(widthCombo, "*List");

    distanceCombo = XmCreateDropDownComboBox(control_rc, const_cast<char*>("distanceCombo"), NULL, 0);
    distance_text = XtNameToWidget(distanceCombo, "*Text");
    distance_list = XtNameToWidget(distanceCombo, "*List");

    // line type menu
    type_sub_menu = XmCreatePulldownMenu(control_rc, const_cast<char*>("typePulldown"), NULL, 0);
    set_type_menu(type_sub_menu);
    XtSetArg(args[0], XmNsubMenuId, type_sub_menu);
    type_menu = XmCreateOptionMenu(control_rc, const_cast<char*>("typeMenu"), args, 1);

    // line colour menu
    line_colour_sub_menu = XmCreatePulldownMenu(control_rc, const_cast<char*>("lineColourPulldown"), NULL, 0);
    set_line_colour_menu(line_colour_sub_menu);
    XtSetArg(args[0], XmNsubMenuId, line_colour_sub_menu);
    line_colour_menu = XmCreateOptionMenu(control_rc, const_cast<char*>("lineColourMenu"), args, 1);

    bg_colour_sub_menu = XmCreatePulldownMenu(control_rc, const_cast<char*>("bgColourPulldown"), NULL, 0);
    set_bg_colour_menu(bg_colour_sub_menu);
    XtSetArg(args[0], XmNsubMenuId, bg_colour_sub_menu);
    bg_colour_menu = XmCreateOptionMenu(control_rc, const_cast<char*>("bgColourMenu"), args, 1);

    // now, the action buttons
    ok_button = XtVaCreateManagedWidget(
        "ok",
        xmPushButtonGadgetClass, action_area,
        XmNshowAsDefault, True,
        XmNdefaultButtonShadowThickness, 1,
        XmNleftAttachment, XmATTACH_POSITION,
        XmNleftPosition, 1,
        XmNrightAttachment, XmATTACH_POSITION,
        XmNrightPosition, 2,
        NULL);

    XtAddCallback(
        ok_button,
        XmNactivateCallback,
        button_cb,
        (XtPointer)this);

    apply_button = NULL;
    if (has_apply) {
        apply_button = XtVaCreateManagedWidget(
            "apply",
            xmPushButtonGadgetClass, action_area,
            XmNshowAsDefault, False,
            XmNdefaultButtonShadowThickness, 1,
            XmNleftAttachment, XmATTACH_POSITION,
            XmNleftPosition, 3,
            XmNrightAttachment, XmATTACH_POSITION,
            XmNrightPosition, 4,
            NULL);

        XtAddCallback(
            apply_button,
            XmNactivateCallback,
            button_cb,
            (XtPointer)this);
    }

    cancel_button = XtVaCreateManagedWidget(
        "cancel",
        xmPushButtonGadgetClass, action_area,
        XmNshowAsDefault, False,
        XmNdefaultButtonShadowThickness, 1,
        XmNleftAttachment, XmATTACH_POSITION,
        XmNleftPosition, has_apply ? 5 : 3,
        XmNrightAttachment, XmATTACH_POSITION,
        XmNrightPosition, has_apply ? 6 : 4,
        NULL);

    XtAddCallback(
        cancel_button,
        XmNactivateCallback,
        button_cb,
        (XtPointer)this);

    XtManageChild(frame1);
    XtManageChild(control_rc);
    XtManageChild(label_rc);
    XtManageChild(type_menu);
    XtManageChild(line_colour_menu);
    XtManageChild(bg_colour_menu);
    XtManageChild(widthCombo);
    XtManageChild(distanceCombo);
    XtManageChild(action_area);
    XtManageChild(control_area);
    XtManageChild(pane);

    return;
}

mx_border_d::~mx_border_d()
{
}

void mx_border_d::activate_d(mx_nlist& initial_styles,
    mx_unit_e u, mx_border_style_mod& out_mod)
{
    result = &out_mod;
    *result = mx_border_style_mod();
    units = u;

    get_initial_mod(initial_styles);

    mx_dialog::activate();
    set_options();
    return;
}

int mx_border_d::run(mx_nlist& initial_styles,
    mx_unit_e u, mx_border_style_mod& out_mod)
{
    int res = 0;
    centre();
    activate_d(initial_styles, u, out_mod);
    res = run_modal();
    deactivate();
    return res;
}

void mx_border_d::get_initial_mod(mx_nlist& border_styles)
{
    int err = MX_ERROR_OK;
    bool not_done = true;
    uint32 num_style_rows = border_styles.get_num_rows();
    uint32 num_style_columns = border_styles.get_num_items(err, 0);
    uint32 r, c;
    mx_border_style*** bst;
    MX_ERROR_CHECK(err);

    if (num_style_rows == 0 || num_style_columns == 0) {
        mx_printf_warning("wrong parameters passed to mx_border_d::get_initial_mod()");
        return;
    }

    has_horizontal = (num_style_rows > 1);
    has_vertical = (num_style_columns > 1);

    // make an array from the nlist for easy access
    bst = new mx_border_style**[num_style_rows];
    for (r = 0; r < num_style_rows; r++) {
        bst[r] = new mx_border_style*[num_style_columns];

        for (c = 0; c < num_style_columns; c++) {
            bst[r][c] = (mx_border_style*)border_styles.item(err, r, c);
            MX_ERROR_CHECK(err);
        }
    }

    // find the top style mod
    result->set_top_style_mod(bst[0][0]->top_style);
    for (c = 1; c < num_style_columns; c++) {
        if (bst[0][0]->top_style != bst[0][c]->top_style) {
            result->clear_top_style_mod();
            break;
        }
    }

    // find the bottom style mod
    result->set_bottom_style_mod(bst[num_style_rows - 1][0]->bottom_style);
    for (c = 1; c < num_style_columns; c++) {
        if (bst[num_style_rows - 1][0]->bottom_style != bst[num_style_rows - 1][c]->bottom_style) {
            result->clear_bottom_style_mod();
            break;
        }
    }

    // find the left style mod
    result->set_left_style_mod(bst[0][0]->left_style);
    for (r = 1; r < num_style_rows; r++) {
        if (bst[0][0]->left_style != bst[r][0]->left_style) {
            result->clear_left_style_mod();
            break;
        }
    }

    // find the right style mod
    result->set_right_style_mod(bst[0][num_style_columns - 1]->right_style);
    for (r = 1; r < num_style_rows; r++) {
        if (bst[0][num_style_columns - 1]->right_style != bst[r][num_style_columns - 1]->right_style) {
            result->clear_right_style_mod();
            break;
        }
    }

    // find the horizontal/vertical style mod. This uses the convention that
    // borders inside an array of bordered object are always set above the
    // bordered element, rather than below the previous bordered element
    // (except for the last one, obviously. And the same principle applies from
    // left to right

    // first rejig the horizontal borders to make sure they're in the required layout

    if (has_horizontal) {
        for (c = 0; c < num_style_columns; c++) {
            for (r = 1; r < num_style_rows; r++) {
                if (bst[r - 1][c]->bottom_style.line_type != mx_no_line) {
                    bst[r][c]->top_style = bst[r - 1][c]->bottom_style;
                    bst[r - 1][c]->bottom_style.line_type = mx_no_line;
                }
            }
        }

        // now check the borders to see if any are different
        not_done = TRUE;
        result->set_horizontal_style_mod(bst[1][0]->top_style);
        for (c = 0; c < num_style_columns && not_done; c++) {
            for (r = 1; r < num_style_rows && not_done; r++) {
                if (bst[r][c]->top_style != bst[1][0]->top_style) {
                    result->clear_horizontal_style_mod();
                    not_done = FALSE;
                }
            }
        }
    }

    // do the same for the verticals
    if (has_vertical) {
        for (r = 0; r < num_style_rows; r++) {
            for (c = 1; c < num_style_columns; c++) {
                if (bst[r][c - 1]->right_style.line_type != mx_no_line) {
                    bst[r][c]->left_style = bst[r][c - 1]->right_style;
                    bst[r][c - 1]->right_style.line_type = mx_no_line;
                }
            }
        }

        // now check the borders to see if any are different
        not_done = TRUE;
        result->set_vertical_style_mod(bst[0][1]->left_style);
        for (r = 0; r < num_style_rows; r++) {
            for (c = 1; c < num_style_columns; c++) {
                if (bst[r][c]->left_style != bst[0][1]->left_style) {
                    result->clear_vertical_style_mod();
                    not_done = FALSE;
                }
            }
        }
    }

    // now get the distance from contents mod and fill_style mod

    result->set_distance_from_contents_mod(bst[0][0]->distance_from_contents);
    result->set_fill_style_mod(bst[0][0]->fill_style);
    for (r = 0; r < num_style_rows; r++) {
        for (c = 0; c < num_style_columns && not_done; c++) {
            if (bst[r][c]->distance_from_contents != bst[0][0]->distance_from_contents) {
                result->clear_distance_from_contents_mod();
            }
            if (bst[r][c]->fill_style != bst[0][0]->fill_style) {
                result->clear_fill_style_mod();
            }
        }
    }

    // delete the array
    for (r = 0; r < num_style_rows; r++)
        delete[] bst[r];
    delete[] bst;

    return;
abort:
    global_error_trace->print();
    MX_ERROR_CLEAR(err);
}

void mx_border_d::draw_sample()
{
    Dimension width, height;
    Display* display;
    Drawable drawable;

    XColor screen_colour, exact_colour;
    mx_line_style ls;
    mx_fill_style fs;
    bool ind;

    display = XtDisplay(drawing_area);
    drawable = XtWindow(drawing_area);

    XClearWindow(display, drawable);

    XtVaGetValues(
        drawing_area,
        XmNwidth, &width,
        XmNheight, &height,
        NULL);

    x1 = width / 4;
    x2 = width - x1;
    y1 = height / 4;
    y2 = height - y1;
    xmid = (x1 + x2) / 2;
    ymid = (y1 + y2) / 2;

    // draw the background colour

    if (result->get_fill_style_mod(fs) && fs.type == mx_fill_colour) {
        if (fs.colour.name != NULL) {
            if (XLookupColor(display,
                    DefaultColormap(display, DefaultScreen(display)),
                    fs.colour.name, &screen_colour, &exact_colour)
                && XAllocNamedColor(display,
                    DefaultColormap(display, DefaultScreen(display)),
                    fs.colour.name, &screen_colour, &exact_colour)) {
                XSetForeground(display, gc, exact_colour.pixel);
            }
        } else {
            exact_colour.red = fs.colour.red << 8;
            exact_colour.green = fs.colour.green << 8;
            exact_colour.blue = fs.colour.blue << 8;
            exact_colour.flags = DoRed | DoGreen | DoBlue;

            if (XAllocColor(display, DefaultColormap(display, DefaultScreen(display)),
                    &exact_colour)) {
                XSetForeground(display, gc, exact_colour.pixel);
            }
        }

        XSetFillStyle(display, gc, FillSolid);
        XFillRectangle(display, drawable, gc, x1 + 10, y1 + 10,
            x2 - x1 - 20, y2 - y1 - 20);
    }

    // draw grid in lightgrey
    if (XLookupColor(display,
            DefaultColormap(display, DefaultScreen(display)),
            "gray70", &screen_colour, &exact_colour)) {
        if (XAllocNamedColor(display,
                DefaultColormap(display, DefaultScreen(display)),
                "gray70", &screen_colour, &exact_colour)) {
            XSetForeground(display, gc, exact_colour.pixel);
        }
    }

    XSetLineAttributes(display, gc, 1, LineSolid, CapButt, JoinMiter);

    XDrawLine(display, drawable, gc, x1, 0, x1, height);
    XDrawLine(display, drawable, gc, x2, 0, x2, height);
    XDrawLine(display, drawable, gc, 0, y1, width, y1);
    XDrawLine(display, drawable, gc, 0, y2, width, y2);

    if (has_horizontal)
        XDrawLine(display, drawable, gc, x1, ymid, x2, ymid);
    if (has_vertical)
        XDrawLine(display, drawable, gc, xmid, y1, xmid, y2);

    // draw corner pointers in black
    if (XLookupColor(display,
            DefaultColormap(display, DefaultScreen(display)),
            "black", &screen_colour, &exact_colour)) {
        if (XAllocNamedColor(display,
                DefaultColormap(display, DefaultScreen(display)),
                "black", &screen_colour, &exact_colour)) {
            XSetForeground(display, gc, exact_colour.pixel);
        }
    }

    XDrawLine(display, drawable, gc, x1 - 3, y1 - 12, x1 + 3, y1 - 12);
    XDrawLine(display, drawable, gc, x1 - 3, y1 - 12, x1, y1 - 6);
    XDrawLine(display, drawable, gc, x1 + 3, y1 - 12, x1, y1 - 6);

    XDrawLine(display, drawable, gc, x2 - 3, y1 - 12, x2 + 3, y1 - 12);
    XDrawLine(display, drawable, gc, x2 - 3, y1 - 12, x2, y1 - 6);
    XDrawLine(display, drawable, gc, x2 + 3, y1 - 12, x2, y1 - 6);

    if (has_vertical) {
        XDrawLine(display, drawable, gc, xmid - 3, y1 - 12, xmid + 3, y1 - 12);
        XDrawLine(display, drawable, gc, xmid - 3, y1 - 12, xmid, y1 - 6);
        XDrawLine(display, drawable, gc, xmid + 3, y1 - 12, xmid, y1 - 6);

        XDrawLine(display, drawable, gc, xmid - 3, y2 + 12, xmid + 3, y2 + 12);
        XDrawLine(display, drawable, gc, xmid - 3, y2 + 12, xmid, y2 + 6);
        XDrawLine(display, drawable, gc, xmid + 3, y2 + 12, xmid, y2 + 6);
    }

    XDrawLine(display, drawable, gc, x1 - 3, y2 + 12, x1 + 3, y2 + 12);
    XDrawLine(display, drawable, gc, x1 - 3, y2 + 12, x1, y2 + 6);
    XDrawLine(display, drawable, gc, x1 + 3, y2 + 12, x1, y2 + 6);

    XDrawLine(display, drawable, gc, x2 - 3, y2 + 12, x2 + 3, y2 + 12);
    XDrawLine(display, drawable, gc, x2 - 3, y2 + 12, x2, y2 + 6);
    XDrawLine(display, drawable, gc, x2 + 3, y2 + 12, x2, y2 + 6);

    XDrawLine(display, drawable, gc, x1 - 12, y1 - 3, x1 - 12, y1 + 3);
    XDrawLine(display, drawable, gc, x1 - 12, y1 - 3, x1 - 6, y1);
    XDrawLine(display, drawable, gc, x1 - 12, y1 + 3, x1 - 6, y1);

    XDrawLine(display, drawable, gc, x1 - 12, y2 - 3, x1 - 12, y2 + 3);
    XDrawLine(display, drawable, gc, x1 - 12, y2 - 3, x1 - 6, y2);
    XDrawLine(display, drawable, gc, x1 - 12, y2 + 3, x1 - 6, y2);

    if (has_horizontal) {
        XDrawLine(display, drawable, gc, x1 - 12, ymid - 3, x1 - 12, ymid + 3);
        XDrawLine(display, drawable, gc, x1 - 12, ymid - 3, x1 - 6, ymid);
        XDrawLine(display, drawable, gc, x1 - 12, ymid + 3, x1 - 6, ymid);

        XDrawLine(display, drawable, gc, x2 + 12, ymid - 3, x2 + 12, ymid + 3);
        XDrawLine(display, drawable, gc, x2 + 12, ymid - 3, x2 + 6, ymid);
        XDrawLine(display, drawable, gc, x2 + 12, ymid + 3, x2 + 6, ymid);
    }

    XDrawLine(display, drawable, gc, x2 + 12, y1 - 3, x2 + 12, y1 + 3);
    XDrawLine(display, drawable, gc, x2 + 12, y1 - 3, x2 + 6, y1);
    XDrawLine(display, drawable, gc, x2 + 12, y1 + 3, x2 + 6, y1);

    XDrawLine(display, drawable, gc, x2 + 12, y2 - 3, x2 + 12, y2 + 3);
    XDrawLine(display, drawable, gc, x2 + 12, y2 - 3, x2 + 6, y2);
    XDrawLine(display, drawable, gc, x2 + 12, y2 + 3, x2 + 6, y2);

    ind = !result->get_top_style_mod(ls);
    draw_line(display, drawable, ls, ind, x1, y1, x2, y1);

    ind = !result->get_bottom_style_mod(ls);
    draw_line(display, drawable, ls, ind, x1, y2, x2, y2);

    ind = !result->get_left_style_mod(ls);
    draw_line(display, drawable, ls, ind, x1, y1, x1, y2);

    ind = !result->get_right_style_mod(ls);
    draw_line(display, drawable, ls, ind, x2, y1, x2, y2);

    if (has_horizontal) {
        ind = !result->get_horizontal_style_mod(ls);
        draw_line(display, drawable, ls, ind, x1, ymid, x2, ymid);
    }

    if (has_vertical) {
        ind = !result->get_vertical_style_mod(ls);
        draw_line(display, drawable, ls, ind, xmid, y1, xmid, y2);
    }

    return;
}

void mx_border_d::set_line_colour(Widget w)
{
    for (int i = 0; i < MX_NUM_COLOURS; i++) {
        if (w == line_colour_buttons[i]) {
            current_colour_name = mx_colour_names[i];
            break;
        }
    }
}

void mx_border_d::set_bg_colour(Widget w)
{
    if (w == bg_colour_buttons[0]) {
        result->set_fill_style_mod(mx_fill_style(mx_fill_transparent));
        draw_sample();
        return;
    }

    for (int i = 0; i < MX_NUM_COLOURS; i++) {
        if (w == bg_colour_buttons[i + 1]) {
            result->set_fill_style_mod(mx_fill_style(mx_colour_names[i]));
            draw_sample();
            return;
        }
    }
}

void mx_border_d::set_options()
{
    char s[30];
    float f;

    sprintf(s, "%.2f %s", mx_mm_to_unit(1.0f, units), mx_unit_name(units));
    XmTextSetString(size_text, s);

    if (!result->get_distance_from_contents_mod(f)) {
        strcpy(s, "");
    } else {
        sprintf(s, "%.2f %s",
            mx_mm_to_unit(f, units),
            mx_unit_name(units));
    }
    XmTextSetString(distance_text, s);

    fill_list(size_list);
    fill_list(distance_list);
}

void mx_border_d::set_line_colour_menu(Widget w)
{
    for (int i = 0; i < MX_NUM_COLOURS; i++) {
        line_colour_buttons[i] = XtVaCreateManagedWidget(mx_colour_label_names[i], xmPushButtonGadgetClass, w, NULL);
        XtAddCallback(line_colour_buttons[i], XmNactivateCallback, line_colour_cb, (XtPointer)this);
    }
}

void mx_border_d::set_bg_colour_menu(Widget w)
{
    bg_colour_buttons[0] = XtVaCreateManagedWidget("transparent", xmPushButtonGadgetClass, w, NULL);
    XtAddCallback(bg_colour_buttons[0], XmNactivateCallback, bg_colour_cb, (XtPointer)this);

    for (int i = 0; i < MX_NUM_COLOURS; i++) {
        bg_colour_buttons[i + 1] = XtVaCreateManagedWidget(mx_colour_label_names[i], xmPushButtonGadgetClass, w, NULL);
        XtAddCallback(bg_colour_buttons[i + 1], XmNactivateCallback, bg_colour_cb, (XtPointer)this);
    }
}

void mx_border_d::set_type_menu(Widget w)
{
    solid_button = XtVaCreateManagedWidget("solid", xmPushButtonGadgetClass, w, NULL);
    dotted_button = XtVaCreateManagedWidget("dotted", xmPushButtonGadgetClass, w, NULL);
    dashed_button = XtVaCreateManagedWidget("dashed", xmPushButtonGadgetClass, w, NULL);

    XtAddCallback(solid_button, XmNactivateCallback, style_cb, (XtPointer)this);
    XtAddCallback(dotted_button, XmNactivateCallback, style_cb, (XtPointer)this);
    XtAddCallback(dashed_button, XmNactivateCallback, style_cb, (XtPointer)this);

    line_type = mx_solid;
}

void mx_border_d::fill_list(Widget w)
{
    const int num_items = 11;
    XmString str[num_items];
    int i;
    char s[30];

    XtVaSetValues(w, XmNvisibleItemCount, num_items, NULL);

    for (i = 0; i < num_items; i++) {
        sprintf(s, "%.2f %s", mx_mm_to_unit(i, units), mx_unit_name(units));
        str[i] = XmStringCreate(s, XmFONTLIST_DEFAULT_TAG);
    }

    XtVaSetValues(w,
        XmNitemCount, num_items,
        XmNitems, str,
        NULL);

    for (i = 0; i < num_items; i++) {
        XmStringFree(str[i]);
    }
}

void mx_border_d::draw_line(
    Display* display,
    Drawable drawable,
    mx_line_style ls,
    bool ind,
    int x1,
    int y1,
    int x2,
    int y2)
{
    int style, width;
    char dash_list[2];

    XColor screen_colour, exact_colour;

    if (ind) {
        // draw indeterminate in light grey
        if (XLookupColor(display,
                DefaultColormap(display, DefaultScreen(display)),
                "gray50", &screen_colour, &exact_colour)) {
            if (XAllocNamedColor(display,
                    DefaultColormap(display, DefaultScreen(display)),
                    "gray50", &screen_colour, &exact_colour)) {
                XSetForeground(display, gc, exact_colour.pixel);
            }
        }
        XSetLineAttributes(display, gc, 5, LineSolid, CapButt, JoinMiter);
        XDrawLine(display, drawable, gc, x1, y1, x2, y2);
        return;
    }

    if (ls.line_type == mx_no_line) {
        return;
    }

    width = (int)ls.width;

    switch (ls.line_type) {
    case mx_dotted:
        dash_list[0] = 3;
        dash_list[1] = 3;
        XSetDashes(display, gc, 0, dash_list, 2);
        style = LineOnOffDash;
        break;
    case mx_dashed:
        dash_list[0] = 9;
        dash_list[1] = 3;
        XSetDashes(display, gc, 0, dash_list, 2);
        style = LineOnOffDash;
        break;
    default:
    case mx_solid:
        style = LineSolid;
        break;
    }

    if (ls.colour.name != NULL) {
        if (XLookupColor(display,
                DefaultColormap(display, DefaultScreen(display)),
                ls.colour.name, &screen_colour, &exact_colour)) {
            if (XAllocNamedColor(display,
                    DefaultColormap(display, DefaultScreen(display)),
                    ls.colour.name, &screen_colour, &exact_colour)) {
                XSetForeground(display, gc, exact_colour.pixel);
            }
        }
    }

    XSetLineAttributes(display, gc, width, style, CapButt, JoinMiter);
    XDrawLine(display, drawable, gc, x1, y1, x2, y2);
}

void mx_border_d::handle_input(XEvent* e)
{
    int last_line, this_line;

    if (e->xany.type == ButtonPress) {
        last_x = e->xbutton.x;
        last_y = e->xbutton.y;
    } else {
        if (e->xany.type == ButtonRelease) {
            last_line = line_pos(last_x, last_y);
            this_line = line_pos(e->xbutton.x, e->xbutton.y);

            if (last_line == this_line) {
                bool b = FALSE;
                mx_line_style ls, new_style = line_style();
                switch (last_line) {
                case 0:
                    b = !result->get_top_style_mod(ls);
                    toggle_style(ls, new_style, b);
                    result->set_top_style_mod(ls);
                    break;
                case 1:
                    b = !result->get_right_style_mod(ls);
                    toggle_style(ls, new_style, b);
                    result->set_right_style_mod(ls);
                    break;
                case 2:
                    b = !result->get_bottom_style_mod(ls);
                    toggle_style(ls, new_style, b);
                    result->set_bottom_style_mod(ls);
                    break;
                case 3:
                    b = !result->get_left_style_mod(ls);
                    toggle_style(ls, new_style, b);
                    result->set_left_style_mod(ls);
                    break;
                case 4:
                    b = !result->get_horizontal_style_mod(ls);
                    toggle_style(ls, new_style, b);
                    result->set_horizontal_style_mod(ls);
                    break;
                case 5:
                    b = !result->get_vertical_style_mod(ls);
                    toggle_style(ls, new_style, b);
                    result->set_vertical_style_mod(ls);
                    break;
                default:
                    return;
                }
                draw_sample();
            }
        }
    }
}

mx_line_style mx_border_d::line_style()
{
    mx_line_style res;
    char* s;

    res.line_type = line_type;
    res.colour.name = current_colour_name;
    s = XmTextGetString(size_text);
    res.width = mx_unit_to_mm(atof(s), units);
    XtFree(s);
    if (res.width <= 0.5) {
        res.width = 0.5;
    }

    return res;
}

int mx_border_d::line_pos(int x, int y)
{
    if ((y < y1 + 5) && (y > y1 - 5)) {
        return 0;
    }

    if ((y < y2 + 5) && (y > y2 - 5)) {
        return 2;
    }

    if ((x < x1 + 5) && (x > x1 - 5)) {
        return 3;
    }

    if ((x < x2 + 5) && (x > x2 - 5)) {
        return 1;
    }

    if ((y < ymid + 5) && (y > ymid - 5)) {
        return 4;
    }

    if ((x < xmid + 5) && (x > xmid - 5)) {
        return 5;
    }
    return -1;
}

void mx_border_d::toggle_style(mx_line_style& style, mx_line_style new_style, bool ind)
{
    if (ind || style.line_type == mx_no_line) {
        style = new_style;
    } else {
        style.line_type = mx_no_line;
    }
}

void mx_border_d::get_options()
{
    char* s = XmTextGetString(distance_text);

    if (strcmp(s, "") != 0) {
        float f = mx_unit_to_mm(atof(s), units);
        if (f < 0.0f)
            f = 0.0f;

        result->set_distance_from_contents_mod(f);
    }
    XtFree(s);
}
