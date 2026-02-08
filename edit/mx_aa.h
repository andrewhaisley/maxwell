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
#ifndef MX_AA_H
#define MX_AA_H
/*
 * MODULE/CLASS :  mx_active_area
 *
 * AUTHOR : Tom Newton
 *
 *
 *
 * DESCRIPTION:
 *
 * object for handle active areas (ones within resize handles active around
 * their edges
 *
 *
 *
 *
 */
#include <mx.h>
#include <mx_doc_coord.h>

class mx_wp_cursor;
class mx_button_event;
class mx_area;
class mx_device;
class mx_screen_device;
class mx_wp_doc_pos;
class mx_wp_document;

class mx_active_area {
public:
    mx_active_area(mx_wp_cursor* my_cursor);

    bool is_active() const;
    bool is_dragging() const;
    void draw_active_handles(int& err);
    void undraw_active_handles(int& err);

    void single_click(int& err, mx_button_event& be);
    void button_motion(int& err, mx_button_event& be);
    void button_motion_end(int& err, mx_button_event& be);

    void deactivate();

    // gets the area at the given position. Position must be on an
    // area complex word item or this method fails.
    void get_area(int& err, mx_wp_doc_pos* pos, mx_wp_document* doc);

    void pre_scroll();
    void post_scroll();

    void pre_redraw();
    void post_redraw();

private:
    // are the active handles on the screen
    bool handles_are_drawn;

    // is the screen currently being scrolled
    bool scrolling;

    void toggle_active_handles(int& err, bool on);

    enum mx_active_area_state {
        mx_aas_normal_e = 25,
        mx_aas_active_e,
        mx_aas_drag_e
    };

    // the state of this active area. If the area is not active, the state is
    // normal. After a single click inside the area, the state is active. A
    // single click outside the area returns the state to normal. A click
    // inside the area may move the state to drag if the click is in one of the
    // resize/crop "handles". Button motion events then maintain the drag state
    // until a button motion end event.
    mx_active_area_state state;

    // the area which is active
    mx_area* active_opaque_area;

    // the origin in document coordinates of the active area
    mx_doc_coord_t active_area_origin;

    // the cursor for this active area
    mx_wp_cursor* cursor;

    enum mx_aah_type {
        mx_aah_top_left_e,
        mx_aah_top_e,
        mx_aah_top_right_e,
        mx_aah_left_e,
        mx_aah_right_e,
        mx_aah_bottom_left_e,
        mx_aah_bottom_e,
        mx_aah_bottom_right_e
    };

    // object for dragging with
    struct mx_aa_handle {
        void drag_to_point(mx_device* dev, const mx_doc_coord_t& c);

        mx_aah_type type;
        mx_doc_coord_t tl, br;
    };

    void get_drag_handle_object(int& err, mx_button_event& be,
        mx_screen_device* dev);

    // the handle which is being held
    mx_aa_handle drag_handle;
};

#endif // MX_AA_H
