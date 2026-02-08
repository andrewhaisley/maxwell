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
#include "mx_aa.h"
#include "mx_wp_cursor.h"
#include <mx_area.h>
#include <mx_complex_w.h>
#include <mx_cw_t.h>
#include <mx_frame.h>
#include <mx_para_pos.h>
#include <mx_paragraph.h>
#include <mx_sc_device.h>

// minimum handle width is 2 pixels, max is 10 pixels, normal proportion is
// a fifth of the smallest side

static const double min_dev_handle_width = 2;
static const double max_dev_handle_width = 10;
static const double max_handle_proportion = 0.2;

inline static double get_handle_width(mx_device* device,
    const mx_doc_coord_t& tl,
    const mx_doc_coord_t& br)
{
    double min_handle_width = device->dev2frame(min_dev_handle_width);
    double max_handle_width = device->dev2frame(max_dev_handle_width);
    double r_width = br.p.x - tl.p.x;
    double r_height = br.p.y - tl.p.y;
    double res;

    res = GMIN(r_width, r_height) * max_handle_proportion;
    res = GMIN(max_handle_width, res);
    res = GMAX(min_handle_width, res);
    return res;
}

inline static void draw_handles(int& err, mx_screen_device* device,
    const mx_doc_coord_t& tl, const mx_doc_coord_t& br,
    bool drawToWindowOnly)
{
    device->setDrawToWindowOnly(drawToWindowOnly);
    device->setMonitor(!drawToWindowOnly);
    device->xorResizeHandles(err, tl, br, get_handle_width(device, tl, br));
    MX_ERROR_CHECK(err);
abort:;
}

static inline double get_minimum_side_length(mx_device* dev,
    const mx_doc_coord_t& tl,
    const mx_doc_coord_t& br)
{
    // 20 pixels is the minimum size
    return dev->dev2frame(20.0);
}

inline static void frame_coords(mx_device* dev, mx_doc_coord_t& c)
{
    int err = MX_ERROR_OK;
    mx_point p;
    dev->getFrame()->docCoordToCoord(err, c, p);
    MX_ERROR_CHECK(err);
    c.p = p;
    c.sheet_number = 0;
    return;
abort:
    global_error_trace->print();
}

void mx_active_area::mx_aa_handle::drag_to_point(mx_device* dev,
    const mx_doc_coord_t& c)
{
    mx_doc_coord_t actual_c = c, tmp = tl;
    double min_side_length = get_minimum_side_length(dev, tl, br);
    double old_w = fabs(tl.p.x - br.p.x);
    double old_h = fabs(tl.p.y - br.p.y);
    double new_w, new_h, x_ratio, y_ratio, ratio;

    // convert the coordinate given (which may be on another sheet), to the
    // equivalent (for this view mode) on the sheet which the active area is on
    frame_coords(dev, actual_c);
    frame_coords(dev, tmp);

    actual_c.p -= tmp.p;

    actual_c.sheet_number = tl.sheet_number;
    actual_c.p += tl.p;

    switch (type) {
    case mx_aah_top_left_e:
        new_w = br.p.x - actual_c.p.x;
        new_h = br.p.y - actual_c.p.y;
        new_w = GMAX(new_w, min_side_length);
        new_h = GMAX(new_h, min_side_length);
        x_ratio = new_w / old_w;
        y_ratio = new_h / old_h;
        ratio = GMAX(x_ratio, y_ratio);

        tl.p.x = br.p.x - old_w * ratio;
        tl.p.y = br.p.y - old_h * ratio;

        break;

    case mx_aah_top_e:
        if (br.p.y - min_side_length > actual_c.p.y) {
            tl.p.y = actual_c.p.y;
        }
        break;
    case mx_aah_top_right_e:
        new_w = actual_c.p.x - tl.p.x;
        new_h = br.p.y - actual_c.p.y;
        new_w = GMAX(new_w, min_side_length);
        new_h = GMAX(new_h, min_side_length);
        x_ratio = new_w / old_w;
        y_ratio = new_h / old_h;
        ratio = GMAX(x_ratio, y_ratio);

        br.p.x = tl.p.x + old_w * ratio;
        tl.p.y = br.p.y - old_h * ratio;

        break;

    case mx_aah_left_e:
        if (br.p.x - min_side_length > actual_c.p.x) {
            tl.p.x = actual_c.p.x;
        }
        break;

    case mx_aah_right_e:
        if (tl.p.x + min_side_length < actual_c.p.x) {
            br.p.x = actual_c.p.x;
        }
        break;

    case mx_aah_bottom_left_e:
        new_w = br.p.x - actual_c.p.x;
        new_h = actual_c.p.y - tl.p.y;
        new_w = GMAX(new_w, min_side_length);
        new_h = GMAX(new_h, min_side_length);
        x_ratio = new_w / old_w;
        y_ratio = new_h / old_h;
        ratio = GMAX(x_ratio, y_ratio);

        tl.p.x = br.p.x - old_w * ratio;
        br.p.y = tl.p.y + old_h * ratio;

        break;

    case mx_aah_bottom_e:
        if (tl.p.y + min_side_length < actual_c.p.y) {
            br.p.y = actual_c.p.y;
        }
        break;

    case mx_aah_bottom_right_e:
        new_w = actual_c.p.x - tl.p.x;
        new_h = actual_c.p.y - tl.p.y;
        new_w = GMAX(new_w, min_side_length);
        new_h = GMAX(new_h, min_side_length);
        x_ratio = new_w / old_w;
        y_ratio = new_h / old_h;
        ratio = GMAX(x_ratio, y_ratio);

        br.p.x = tl.p.x + old_w * ratio;
        br.p.y = tl.p.y + old_h * ratio;

        break;

    default:
        mx_printf_warning("internal error");
        break;
    }
}

mx_active_area::mx_active_area(mx_wp_cursor* my_cursor)
{
    active_opaque_area = NULL;
    state = mx_aas_normal_e;
    cursor = my_cursor;
    handles_are_drawn = scrolling = FALSE;
}

bool mx_active_area::is_active() const
{
    return state != mx_aas_normal_e;
}

bool mx_active_area::is_dragging() const
{
    return state == mx_aas_drag_e;
}

void mx_active_area::draw_active_handles(int& err)
{
    toggle_active_handles(err, TRUE);
}

void mx_active_area::undraw_active_handles(int& err)
{
    toggle_active_handles(err, FALSE);
}

void mx_active_area::toggle_active_handles(int& err, bool on)
{
    mx_screen_device* device = (mx_screen_device*)cursor->get_device();

    if ((handles_are_drawn && on) || (!handles_are_drawn && !on))
        return;

    MX_ERROR_ASSERT(err, device->is_a(mx_screen_device_class_e));

    draw_handles(err, device, drag_handle.tl, drag_handle.br, FALSE);
    MX_ERROR_CHECK(err);

    // if we are drawing the handles then commit the pixmap
    if (on)
        device->pixmapCommit();

    handles_are_drawn = on;

abort:;
}

void mx_active_area::single_click(int& err, mx_button_event& be)
{
    mx_wp_document* doc = cursor->get_document();
    mx_wp_doc_pos pos = *cursor->get_position();
    mx_rect area_rect;
    bool try_pos_to_left = FALSE;

    if (be.getShift())
        return;

    scrolling = FALSE;

    this->deactivate();

    // cursor could be on or to the right of an area character which was
    // clicked on, so make sure we test both these cases

    get_area(err, &pos, doc);
    try_pos_to_left = (err != MX_ERROR_OK);
    MX_ERROR_CLEAR(err);

    if (!try_pos_to_left) {
        // point could be in the area under the cursor
        area_rect = mx_rect(drag_handle.tl.p, drag_handle.br.p);
        try_pos_to_left = (!area_rect.pointIn(be.getCoord().p));

        if (!try_pos_to_left) {
            // point is in this area
            state = mx_aas_active_e;
            return;
        }
    }

    if (try_pos_to_left) {
        pos.left(err, doc);
        MX_ERROR_CLEAR(err);

        get_area(err, &pos, doc);
        if (err == MX_ERROR_OK) {
            area_rect = mx_rect(drag_handle.tl.p, drag_handle.br.p);
            if (area_rect.pointIn(be.getCoord().p)) {
                // point is in area to left of cursor
                cursor->set_position(err, pos);
                MX_ERROR_CHECK(err);

                state = mx_aas_active_e;
                return;
            }
        }
        MX_ERROR_CLEAR(err);
    }

    // point not in this area or in the area to the left of the cursor
    state = mx_aas_normal_e;
abort:;
}

void mx_active_area::get_drag_handle_object(int& err, mx_button_event& be,
    mx_screen_device* dev)
{
    mx_doc_coord_t start = be.getStartCoord();
    mx_doc_coord_t origin = drag_handle.tl;
    double w = drag_handle.br.p.x - drag_handle.tl.p.x;
    double h = drag_handle.br.p.y - drag_handle.tl.p.y;
    double handle_width = get_handle_width(cursor->get_device(),
        drag_handle.tl.p,
        drag_handle.br.p);

    if (start.p.x < origin.p.x + handle_width && start.p.x > origin.p.x) {
        if (start.p.y < origin.p.y + handle_width && start.p.y > origin.p.y) {
            // clicked in top left
            drag_handle.type = mx_aah_top_left_e;
        } else if (start.p.y < origin.p.y + (h + handle_width) / 2 && start.p.y > origin.p.y + (h - handle_width) / 2) {
            // clicked in left
            drag_handle.type = mx_aah_left_e;
        } else if (start.p.y < origin.p.y + h && start.p.y > origin.p.y + h - handle_width) {
            // clicked in bottom left
            drag_handle.type = mx_aah_bottom_left_e;
        } else {
            return;
        }
    } else if (start.p.x < origin.p.x + (w + handle_width) / 2 && start.p.x > origin.p.x + (w - handle_width) / 2) {
        if (start.p.y < origin.p.y + handle_width && start.p.y > origin.p.y) {
            // clicked in top
            drag_handle.type = mx_aah_top_e;
        } else if (start.p.y < origin.p.y + h && start.p.y > origin.p.y + h - handle_width) {
            // clicked in bottom
            drag_handle.type = mx_aah_bottom_e;
        } else {
            return;
        }
    } else if (start.p.x < origin.p.x + w && start.p.x > origin.p.x + w - handle_width) {
        if (start.p.y < origin.p.y + handle_width && start.p.y > origin.p.y) {
            // clicked in top right
            drag_handle.type = mx_aah_top_right_e;
        } else if (start.p.y < origin.p.y + (h + handle_width) / 2 && start.p.y > origin.p.y + (h - handle_width) / 2) {
            // clicked in right
            drag_handle.type = mx_aah_right_e;
        } else if (start.p.y < origin.p.y + h && start.p.y > origin.p.y + h - handle_width) {
            // clicked in bottom right
            drag_handle.type = mx_aah_bottom_right_e;
        } else {
            return;
        }
    } else {
        return;
    }

    // undraw the active state handles
    undraw_active_handles(err);
    MX_ERROR_CHECK(err);

    // draw the first drag handle
    drag_handle.drag_to_point(dev, start);
    MX_ERROR_CHECK(err);

    draw_handles(err, dev, drag_handle.tl, drag_handle.br, TRUE);
    MX_ERROR_CHECK(err);

    state = mx_aas_drag_e;
abort:;
}

void mx_active_area::button_motion(int& err, mx_button_event& be)
{
    if (be.getButtonNumber() == 2 || !is_active())
        return;

    mx_screen_device* dev = (mx_screen_device*)cursor->get_device();
    MX_ERROR_ASSERT(err, dev->is_a(mx_screen_device_class_e));

    if (state == mx_aas_active_e) {
        get_drag_handle_object(err, be, dev);
        MX_ERROR_CHECK(err);
    }

    if (state != mx_aas_drag_e) {
        // must be in the drag state now
        return;
    }

    // undraw the last rectangle
    draw_handles(err, dev, drag_handle.tl, drag_handle.br, TRUE);
    MX_ERROR_CHECK(err);

    // now move to the new location
    drag_handle.drag_to_point(dev, be.getCoord());
    MX_ERROR_CHECK(err);

    // draw the new rectangle
    draw_handles(err, dev, drag_handle.tl, drag_handle.br, TRUE);
    MX_ERROR_CHECK(err);

abort:;
}

void mx_active_area::button_motion_end(int& err, mx_button_event& be)
{

    mx_screen_device* dev = (mx_screen_device*)cursor->get_device();
    mx_point new_size;
    mx_point old_size;

    if (be.getButtonNumber() == 2 || state != mx_aas_drag_e)
        return;

    MX_ERROR_ASSERT(err, dev->is_a(mx_screen_device_class_e));

    // reset the state
    state = mx_aas_active_e;

    // undraw the last rectangle
    draw_handles(err, dev, drag_handle.tl, drag_handle.br, TRUE);
    MX_ERROR_CHECK(err);

    // now move to the new location
    drag_handle.drag_to_point(dev, be.getCoord());
    MX_ERROR_CHECK(err);

    new_size = drag_handle.br.p - drag_handle.tl.p;

    if (be.getButtonNumber() == 1) {
        cursor->scale_active_area(err, active_opaque_area, new_size);
        MX_ERROR_CHECK(err);
    } else if (be.getButtonNumber() == 3) {
        mx_rect new_crop_rect(drag_handle.tl.p, drag_handle.br.p);

        // make the crop rectangel relative to 0,0
        new_crop_rect -= active_area_origin.p;

        cursor->crop_active_area(err, active_opaque_area, new_crop_rect);
        MX_ERROR_CHECK(err);
    }
    // now drop out and let cursor reformat document and redraw screen
abort:;
}

void mx_active_area::pre_scroll()
{
    int err = MX_ERROR_OK;

    scrolling = TRUE;
    switch (state) {
    case mx_aas_normal_e:
        return;
    case mx_aas_active_e:
        // undraw the active handles

        undraw_active_handles(err);
        MX_ERROR_CHECK(err);

        break;
    case mx_aas_drag_e: {
        mx_screen_device* dev = (mx_screen_device*)cursor->get_device();
        MX_ERROR_ASSERT(err, dev->is_a(mx_screen_device_class_e));

        // undraw the last rectangle
        draw_handles(err, dev, drag_handle.tl, drag_handle.br, TRUE);
        MX_ERROR_CHECK(err);

        break;
    }
    default:
        // should not be here
        MX_ERROR_THROW(err, MX_ERROR_UNDEFINED);
    }
    return;
abort:
    global_error_trace->print();
    MX_ERROR_CLEAR(err);
}

void mx_active_area::post_scroll()
{
    int err = MX_ERROR_OK;
    scrolling = FALSE;
    switch (state) {
    case mx_aas_normal_e:
        return;
    case mx_aas_active_e:
        // redraw the active handles

        draw_active_handles(err);
        MX_ERROR_CHECK(err);

        break;
    case mx_aas_drag_e: {
        mx_screen_device* dev = (mx_screen_device*)cursor->get_device();
        MX_ERROR_ASSERT(err, dev->is_a(mx_screen_device_class_e));

        // redraw the last rectangle
        draw_handles(err, dev, drag_handle.tl, drag_handle.br, TRUE);
        MX_ERROR_CHECK(err);

        break;
    }
    default:
        // should not be here
        MX_ERROR_THROW(err, MX_ERROR_UNDEFINED);
    }

    return;
abort:
    global_error_trace->print();
    MX_ERROR_CLEAR(err);
}

void mx_active_area::deactivate()
{
    int err = MX_ERROR_OK;
    if (is_active()) {
        mx_screen_device* dev = (mx_screen_device*)cursor->get_device();
        state = mx_aas_normal_e;

        undraw_active_handles(err);
        MX_ERROR_CHECK(err);
        MX_ERROR_ASSERT(err, dev->is_a(mx_screen_device_class_e));
        dev->pixmapCommit();
    }
    return;
abort:
    global_error_trace->print();
    MX_ERROR_CLEAR(err);
}

void mx_active_area::get_area(int& err, mx_wp_doc_pos* pos, mx_wp_document* doc)
{
    mx_para_pos* pp;
    mx_paragraph* p;
    mx_complex_word* cw;
    mx_area_cw_t* cw_item;
    mx_doc_coord_t origin;
    int real_index;
    bool b;

    undraw_active_handles(err);
    MX_ERROR_CHECK(err);

    b = pos->is_paragraph(err);
    MX_ERROR_CHECK(err);

    MX_ERROR_ASSERT(err, b);

    pp = pos->get_para_pos(err);
    MX_ERROR_CHECK(err);

    p = pos->get_para(err, doc);
    MX_ERROR_CHECK(err);

    cw = (mx_complex_word*)p->get_word(err, *pp);
    MX_ERROR_CHECK(err);

    MX_ERROR_ASSERT(err, cw->is_a(mx_complex_word_class_e));

    real_index = cw->get_char_index(err, pp->letter_index);
    MX_ERROR_CHECK(err);

    cw_item = (mx_area_cw_t*)cw->get_item(err, real_index);
    MX_ERROR_CHECK(err);

    MX_ERROR_ASSERT(err, cw_item->is_a(mx_area_cw_t_class_e));

    active_opaque_area = cw_item->a;

    // save the areas outline

    // position() returns the bottom left hand corner
    drag_handle.tl = pos->position(err, doc);
    MX_ERROR_CHECK(err);

    // but we need the uncorrected view position
    drag_handle.tl.p -= cursor->get_view_correction();
    drag_handle.br = drag_handle.tl;

    drag_handle.tl.p.y -= active_opaque_area->get_height(err);
    MX_ERROR_CHECK(err);

    drag_handle.br.p.x += active_opaque_area->get_width(err);
    MX_ERROR_CHECK(err);

    active_area_origin = drag_handle.tl;

abort:;
}

void mx_active_area::pre_redraw()
{
    int err = MX_ERROR_OK;

    // if we are just scrolling or the area is not active then we don't need
    // to do anything
    if (scrolling || state != mx_aas_active_e)
        return;

    undraw_active_handles(err);
    MX_ERROR_CLEAR(err);
}

void mx_active_area::post_redraw()
{
    int err = MX_ERROR_OK;

    // if we are just scrolling or the area is not active then we don't need
    // to do anything
    if (scrolling || state != mx_aas_active_e)
        return;

    get_area(err, cursor->get_position(), cursor->get_document());
    MX_ERROR_CHECK(err);

    draw_active_handles(err);
    MX_ERROR_CHECK(err);

    return;
abort:
    global_error_trace->print();
    MX_ERROR_CLEAR(err);
}
