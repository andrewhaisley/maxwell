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
#ifndef MX_WP_STSBAR_H
#define MX_WP_STSBAR_H
/*
 * MODULE/CLASS : mx_wp_statusbar
 *
 * AUTHOR : Andrew Haisley
 *
 *
 *
 * DESCRIPTION:
 *
 *
 *
 *
 */

#include <Xm/Xm.h>
#include <mx.h>
#include <mx_bar.h>
#include <mx_stsbar.h>
#include <mx_window.h>

class mx_wp_doc_pos;
class mx_wp_document;

class mx_wp_statusbar : public mx_statusbar {
    MX_RTTI(mx_wp_statusbar_class_e)

public:
    mx_wp_statusbar(mx_window* window);

    virtual void update_b(const char* message, bool hide_other);

    // Updates the status bar with various items or information
    void update_b(int& err, mx_wp_doc_pos& position, mx_wp_document* doc, mx_unit_e u, bool hide_other, float zoom = 100.0);

private:
    Widget labels[4];
    Widget frames[4];

    // current page number
    int current_page;

    // current xy position and zoom
    double current_x, current_y;
    float current_zoom;
};

#endif // MX_WP_STSBAR_H
