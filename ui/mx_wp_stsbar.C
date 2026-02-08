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
 * MODULE/CLASS : mx_wp_statusbar
 *
 * AUTHOR : Tom Newton/Andrew Haisley
 *
 *
 *
 * DESCRIPTION:
 *
 *  Implementation for mx_wp_stsbar.h
 *
 *
 *
 *
 */

#include <Xm/Form.h>
#include <Xm/Frame.h>
#include <Xm/Label.h>
#include <Xm/RowColumn.h>
#include <Xm/Xm.h>

#include <mx.h>
#include <mx_bar.h>
#include <mx_wp_doc.h>
#include <mx_wp_stsbar.h>

mx_wp_statusbar::mx_wp_statusbar(mx_window* window)
    : mx_statusbar(window)
{
    Widget row_column;
    int i;
    char s[20];

    row_column = XtVaCreateManagedWidget(
        "rowColumn",
        xmRowColumnWidgetClass,
        widget,
        XmNorientation, XmHORIZONTAL,
        XmNrightAttachment, XmATTACH_FORM,
        XmNbottomAttachment, XmATTACH_FORM,
        XmNtopAttachment, XmATTACH_FORM,
        NULL);

    (void)XtVaCreateManagedWidget(
        "dummyForm",
        xmFormWidgetClass,
        widget,
        XmNrightAttachment, XmATTACH_WIDGET,
        XmNrightWidget, row_column,
        XmNleftAttachment, XmATTACH_WIDGET,
        XmNleftWidget, message_label,
        XmNbottomAttachment, XmATTACH_FORM,
        XmNtopAttachment, XmATTACH_FORM,
        NULL);

    for (i = 0; i < 4; i++) {
        sprintf(s, "frame%d", i);
        frames[i] = XtVaCreateManagedWidget(
            s,
            xmFrameWidgetClass,
            row_column,
            NULL);

        sprintf(s, "label%d", i);
        labels[i] = XtVaCreateManagedWidget(
            s,
            xmLabelWidgetClass,
            frames[i],
            NULL);
    }

    current_page = -100000;
    current_x = current_y = current_zoom = -10000000.0f;
}

void mx_wp_statusbar::update_b(
    int& err,
    mx_wp_doc_pos& position,
    mx_wp_document* doc,
    mx_unit_e u,
    bool hide_other,
    float zoom)
{
    XmString s;

    char str[50];
    int page;

    mx_doc_coord_t pos;

    if (current_zoom != zoom) {
        sprintf(str, "zoom %.0f%%", zoom);
        s = XmStringCreateLocalized(str);
        XtVaSetValues(labels[0], XmNlabelString, s, NULL);
        XmStringFree(s);
        current_zoom = zoom;
    }

    page = position.get_sheet() + 1;

    if (page != current_page) {
        sprintf(str, "page %d", page);
        s = XmStringCreateLocalized(str);
        XtVaSetValues(labels[1], XmNlabelString, s, NULL);
        XmStringFree(s);
        current_page = page;
    }

    pos = position.position(err, doc);
    MX_ERROR_CHECK(err);

    if (!MX_FLOAT_EQ(current_x, pos.p.x)) {
        sprintf(str, "%.2f %s", mx_mm_to_unit(pos.p.x, u), mx_unit_name(u));
        s = XmStringCreateLocalized(str);
        XtVaSetValues(labels[2], XmNlabelString, s, NULL);
        XmStringFree(s);
        current_x = pos.p.x;
    }

    if (!MX_FLOAT_EQ(current_y, pos.p.y)) {
        sprintf(str, "%.2f %s", mx_mm_to_unit(pos.p.y, u), mx_unit_name(u));
        s = XmStringCreateLocalized(str);
        XtVaSetValues(labels[3], XmNlabelString, s, NULL);
        XmStringFree(s);
        current_y = pos.p.y;
    }
abort:;
}

void mx_wp_statusbar::update_b(const char* message, bool hide_other)
{
    int i;

    if (hide_other) {
        for (i = 0; i < 4; i++) {
            XtUnmanageChild(labels[i]);
            XtUnmanageChild(frames[i]);
        }
    } else {
        for (i = 0; i < 4; i++) {
            XtManageChild(labels[i]);
            XtManageChild(frames[i]);
        }
    }
    mx_statusbar::update(message);
}
