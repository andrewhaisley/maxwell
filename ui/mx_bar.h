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
#ifndef MX_BAR_H
#define MX_BAR_H
/*
 * MODULE/CLASS : mx_bar.h
 *
 * AUTHOR : Tom Newton
 *
 * This file
 *
 * DESCRIPTION:
 *
 *
 *  A base class for bars of various types : menubars, toolbars, ruler bars.
 *
 *
 *
 *
 */

#include <X11/Intrinsic.h>
#include <Xm/Xm.h>
#include <mx.h>
#include <mx_ui_object.h>

// forward declarations
class mx_window;
class mx_menubar;

class mx_bar : public mx_ui_object {
    MX_RTTI(mx_bar_class_e)

public:
    mx_bar(mx_window* parent);
    mx_bar(const char* name, mx_window* window);
    mx_bar(const char* name, mx_window* window, WidgetClass c);
    virtual ~mx_bar();

    mx_window* get_window();

protected:
    mx_window* parent;
};

#endif // MX_BAR_H
