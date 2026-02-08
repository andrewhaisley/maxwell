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
 * MODULE/CLASS : mx_bar
 *
 * AUTHOR : Tom Newton
 *
 * This file
 *
 * DESCRIPTION:
 *
 *  Implementation of a bar base class, for use by subclasses - menu bars,
 *  tool bars etc.
 *
 *
 *
 */

#include <Xm/RowColumn.h>
#include <Xm/Xm.h>
#include <mx.h>
#include <mx_bar.h>
#include <mx_ui_object.h>
#include <mx_window.h>

mx_bar::mx_bar(const char* name, mx_window* parent)
    : mx_ui_object(name, xmRowColumnWidgetClass, parent->get_form())
{
    XtManageChild(widget); // Bars are always managed by default
    XtVaSetValues(widget, XmNorientation, XmHORIZONTAL, NULL);

    mx_bar::parent = parent;
}

mx_bar::mx_bar(const char* name, mx_window* parent, WidgetClass c)
    : mx_ui_object(name, c, parent->get_form())
{
    XtManageChild(widget); // Bars are always managed by default

    mx_bar::parent = parent;
}

mx_bar::mx_bar(mx_window* parent)
    : mx_ui_object()
{
    mx_bar::parent = parent;
}

mx_bar::~mx_bar()
{
}

mx_window* mx_bar::get_window()
{
    return parent;
}
