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
 * MODULE/CLASS : mx_statusbar
 *
 * AUTHOR : Tom Newton
 *
 * This file
 *
 * DESCRIPTION:
 *
 *  Implementation for mx_stsbar.h
 *
 *
 *
 */

#include <Xm/Form.h>
#include <Xm/Label.h>
#include <Xm/Xm.h>

#include <mx.h>
#include <mx_bar.h>
#include <mx_stsbar.h>

mx_statusbar::mx_statusbar(mx_window* window)
    : mx_bar("statusBar", window, xmFormWidgetClass)
{
    message_label = XtVaCreateManagedWidget(
        "messageLabel",
        xmLabelWidgetClass,
        widget,
        XmNrecomputeSize, True,
        XmNleftAttachment, XmATTACH_FORM,
        XmNbottomAttachment, XmATTACH_FORM,
        XmNtopAttachment, XmATTACH_FORM,
        NULL);
}

void mx_statusbar::update(const char* message)
{
    XmString message_string;
    if (message[0] == 0 || message == NULL) {
        message_string = XmStringCreateLocalized(const_cast<char*>(" "));
    } else {
        message_string = XmStringCreateLocalized(const_cast<char*>(message));
    }

    XtVaSetValues(message_label, XmNlabelString, message_string, NULL);
    XmStringFree(message_string);
}
