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
#ifndef MX_ZOOM_D
#define MX_ZOOM_D
/*
 * MODULE/CLASS : mx_zoom_d
 *
 * AUTHOR : Andrew Haisley
 *
 * This file
 *
 * DESCRIPTION:
 *
 *  A zoom factor dialog
 *
 *
 *
 */

#include "mx_dialog.h"
#include <Xm/Xm.h>
#include <mx_ui_object.h>

class mx_inform_d;

class mx_zoom_d : public mx_dialog {

public:
    mx_zoom_d(Widget parent);
    ~mx_zoom_d();

    int zoom_percent;

    Widget ok_button, cancel_button;
    Widget zoom_text;

    void add_zoom(int z);

    int run(int zoom_percent = 100);
    void activate_d(int zoom_percent = 100);
    mx_inform_d* bad_zoom_d;
    bool figure_options();

private:
    Widget zoom_list;
    void add_levels();
};

#endif
