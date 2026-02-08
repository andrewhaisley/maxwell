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
#ifndef MX_STSBAR_H
#define MX_STSBAR_H
/*
 * MODULE/CLASS : mx_statusbar
 *
 * AUTHOR : Tom Newton
 *
 * This file
 *
 * DESCRIPTION:
 *
 *  This class is an example derived class for a maxwell status bar.
 *  It encapsulates all the widgets on the bar and lets you update it
 *  with various messages.
 *
 */

#include <Xm/Xm.h>
#include <mx.h>
#include <mx_bar.h>
#include <mx_window.h>

class mx_statusbar : public mx_bar {
    MX_RTTI(mx_statusbar_class_e)
public:
    mx_statusbar(mx_window* window);

    // Updates the status bar with various items or information
    virtual void update(const char* message);

protected:
    Widget message_label;
};

#endif // MX_STSBAR_H
