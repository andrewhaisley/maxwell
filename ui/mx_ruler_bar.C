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
 * MODULE/CLASS :  mx_ruler_bar
 *
 * AUTHOR : Andrew Haisley
 *
 *
 *
 * DESCRIPTION:
 *
 * A class for displaying end editing rulers
 *
 *
 *
 *
 *
 */

#include <math.h>
#include <maxwell.h>
#include <mx.h>
#include <mx_list.h>
#include <mx_list_iter.h>
#include <mx_rtti.h>
#include <mx_ruler_bar.h>
#include <mx_sc_device.h>
#include <mx_style.h>
#include <mx_window.h>
#include <mx_wp_editor.h>
#include <mx_wp_toolbar.h>
#include <mx_xframe.h>

using namespace std;

mx_font* mx_ruler_bar::font = NULL;
mx_char_style* mx_ruler_bar::cs = NULL;

#define BAR_MM_HEIGHT 19
#define TICK_MM_POS 5
#define TAB_MM_POS 2

// these are in pixels
#define TAB_CHOICE_BOX_WIDTH 16
#define TAB_CHOICE_BOX_HEIGHT 16

// for some reason the ceil function doesn't seem to work within this file -
// this is a replacement....
float temp_ceil(float f)
{
    int i = (int)f;
    return i + 1.0;
}

mx_ruler_bar::mx_ruler_bar(Widget parent, mx_wp_toolbar* notify)
    : mx_frame_target()
{
    string s;
    int err = MX_ERROR_OK;
    mx_doc_coord_t tl(0, 0, 0);
    mx_point sz(1000, 1000);

    tab_type = mx_left;
    left = -10000;

    sensitive = TRUE;
    owner_bar = notify;
    ruler_frame = new mx_xframe(err, tl, sz, 1, *this, parent);
    MX_ERROR_CHECK(err);

    ruler_frame->createDevice(err);
    MX_ERROR_CHECK(err);

    ruler_device = (mx_screen_device*)(ruler_frame->getDevice());

    num_rulers = 0;

    resolution = 1;
    paper_width = 210 - MX_INCHES_TO_MM(2.5);

    s = global_user_config->get_default_string(err, "units", "mm");
    MX_ERROR_CHECK(err);

    units = mx_unit_type(s);

    XtVaSetValues(get_widget(),
        XmNleftAttachment, XmATTACH_FORM,
        XmNrightAttachment, XmATTACH_FORM,
        XmNheight,
        (int)(ruler_device->getScreenResolution() * BAR_MM_HEIGHT),
        NULL);

    return;
abort:
    global_error_trace->print();
    MX_ERROR_CLEAR(err);
}

mx_ruler_bar::~mx_ruler_bar()
{
    for (int i = 0; i < num_rulers; i++)
        delete rulers[i];
    delete ruler_frame;
}

Widget mx_ruler_bar::get_widget()
{
    return ruler_device->get_widget();
}

void mx_ruler_bar::display(int& err, mx_ruler* r[], int n, float res, mx_unit_e u, float w)
{
    int i;
    bool scale_change;
    bool need_to_redraw;

    scale_change = !MX_FLOAT_EQ(paper_width, w) || units != u;

    paper_width = w;
    units = u;

    need_to_redraw = (scale_change || n != num_rulers || n != 1);
    if (!need_to_redraw)
        need_to_redraw = (*r[0] != *rulers[0]);

    for (i = 0; i < num_rulers; i++) {
        delete rulers[i];
    }

    if (n > MX_MAX_NUM_DISPLAY_RULERS) {
        n = MX_MAX_NUM_DISPLAY_RULERS;
    }

    num_rulers = n;
    for (i = 0; i < n; i++) {
        rulers[i] = new mx_ruler(*r[i]);
    }

    if (resolution != res) {
        resolution = res;
        need_to_redraw = scale_change = TRUE;
    }

    if (need_to_redraw) {
        draw_whole_ruler(err);
        MX_ERROR_CHECK(err);

        ruler_device->pixmapCommit();
    }
abort:;
}

// callbacks for the ruler frame
void mx_ruler_bar::draw(int& err, mx_draw_event& event)
{
    if (num_rulers == 0) {
        // if the ruler bar hasn't been set up yet, just return
        return;
    }

    draw_whole_ruler(err);
    MX_ERROR_CHECK(err);

    ruler_device->pixmapCommit();
abort:;
}

void mx_ruler_bar::buttonPress(int& err, mx_button_event& event)
{
    mx_tabstop* tmp;
    int i;
    float pos;
    int num_found;

    mx_doc_coord_t where = event.getCoord(), origin;
    mx_ipoint iwhere, iorigin;

    if (!sensitive) {
        return;
    }

    iwhere = ruler_device->pixelSnap(ruler_device->frame2dev(where));
    where.p.x -= left;
    where.p.x /= resolution;

    origin.p.x = 5.0;
    origin.p.y = 12.0;

    iorigin = ruler_device->pixelSnap(ruler_device->frame2dev(origin));

    // first, is this in the tabstop type selection area ?
    if (iwhere.x >= iorigin.x && iwhere.x <= iorigin.x + 4 * TAB_CHOICE_BOX_WIDTH && iwhere.y >= iorigin.y && iwhere.y <= iorigin.y + TAB_CHOICE_BOX_HEIGHT) {
        i = (iwhere.x - iorigin.x) / TAB_CHOICE_BOX_WIDTH;

        switch (i) {
        default:
        case 0:
            tab_type = mx_left;
            break;
        case 1:
            tab_type = mx_right;
            break;
        case 2:
            tab_type = mx_centre;
            break;
        case 3:
            tab_type = mx_centre_on_dp;
            break;
        }

        draw_tab_choice(err);
        MX_ERROR_CHECK(err);

        ruler_device->pixmapCommit();
        return;
    }

    // is it outside the editable ruler bit?
    if (where.p.y > 5.0) {
        return;
    }

    // calculate tabstop position
    pos = where.p.x;

    // figure out if there is anything there already
    num_found = 0;

    for (i = 0; i < num_rulers; i++) {
        tmp = rulers[i]->get_nearest_tab(pos);
        if (tmp != NULL) {
            if (fabs(tmp->position - pos) <= TAB_TOLERANCE) {
                num_found++;
            }
        }
    }

    {
        mx_window* window = owner_bar->get_window();
        mx_wp_editor* ed = (mx_wp_editor*)window->get_editor();

        if (num_found == 0) {
            ed->add_tab(err, mx_tabstop(tab_type, pos), TAB_TOLERANCE);
        } else {
            ed->remove_tab(err, pos, TAB_TOLERANCE);
        }
        MX_ERROR_CHECK(err);
    }
abort:;
}

void mx_ruler_bar::draw_tabs(int& err)
{
    int i;
    double fx, start_pos;

    mx_ruler* ruler;

    mx_tabstop *tmp_tab, *last_tab = nullptr;
    mx_line_style ls(mx_solid, 0.0, "black");

    ruler_frame->setFalseOrigin(mx_point(left, 0));

    for (i = 0; i < num_rulers; i++) {
        ruler = rulers[i];

        if (ruler->get_num_user_tabs() == 0) {
            // just have default tab spacing between left and right
            start_pos = temp_ceil(ruler->left_indent / ruler->default_tabstop_space);
            start_pos *= ruler->default_tabstop_space;

            ls.colour = mx_colour(0, 0, 0, "blue");

            ruler_device->setDefaultLineStyle(err, ls);
            MX_ERROR_CHECK(err);

            for (fx = start_pos; fx < paper_width; fx += ruler->default_tabstop_space) {
                if (fx > (paper_width - ruler->right_indent)) {
                    break;
                }

                draw_tab(err, mx_automatic, fx * resolution, TAB_MM_POS);
                MX_ERROR_CHECK(err);
            }
        } else {
            // have user defined tabs
            mx_list_iterator it(ruler->user_tabs);

            if (num_rulers == 1) {
                ls.colour = mx_colour(0, 0, 0, "black");
            } else {
                ls.colour = mx_colour(0, 0, 0, "DarkOliveGreen");
            }

            ruler_device->setDefaultLineStyle(err, ls);
            MX_ERROR_CHECK(err);

            while (it.more()) {
                tmp_tab = (mx_tabstop*)it.data();

                last_tab = tmp_tab;

                draw_tab(err, tmp_tab->type, tmp_tab->position * resolution, TAB_MM_POS);
                MX_ERROR_CHECK(err);
            }

            start_pos = temp_ceil(last_tab->position / ruler->default_tabstop_space);
            start_pos *= ruler->default_tabstop_space;

            ls.colour = mx_colour(0, 0, 0, "blue");

            ruler_device->setDefaultLineStyle(err, ls);
            MX_ERROR_CHECK(err);

            // now, the remaining auto tabs
            for (fx = start_pos; fx < paper_width; fx += ruler->default_tabstop_space) {
                if (fx > (paper_width - ruler->right_indent)) {
                    break;
                }

                draw_tab(err, mx_automatic, fx * resolution, TAB_MM_POS);
                MX_ERROR_CHECK(err);
            }
        }
    }

abort:
    ruler_frame->setFalseOrigin(mx_point(0, 0));
    return;
}

void mx_ruler_bar::draw_tab(int& err, mx_tabstop_type_t t, float x, float y)
{
    mx_doc_coord_t ref;

    mx_ipoint p_ref, dc1, dc2;

    ref.p.x = x;
    ref.p.y = y;

    p_ref = ruler_device->pixelSnap(ruler_device->frame2dev(ref));

    switch (t) {
    case mx_automatic:
    case mx_left:
        dc1.x = p_ref.x;
        dc1.y = p_ref.y;
        dc2.x = p_ref.x - 5;
        dc2.y = p_ref.y;

        ruler_device->pdrawLine(err, dc1, dc2);
        MX_ERROR_CHECK(err);

        dc1.x = p_ref.x;
        dc1.y = p_ref.y;
        dc2.x = p_ref.x - 2;
        dc2.y = p_ref.y - 2;

        ruler_device->pdrawLine(err, dc1, dc2);
        MX_ERROR_CHECK(err);

        dc1.x = p_ref.x;
        dc1.y = p_ref.y;
        dc2.x = p_ref.x - 2;
        dc2.y = p_ref.y + 2;

        ruler_device->pdrawLine(err, dc1, dc2);
        MX_ERROR_CHECK(err);

        break;
    case mx_right:
        dc1.x = p_ref.x;
        dc1.y = p_ref.y;
        dc2.x = p_ref.x + 5;
        dc2.y = p_ref.y;

        ruler_device->pdrawLine(err, dc1, dc2);
        MX_ERROR_CHECK(err);

        dc1.x = p_ref.x;
        dc1.y = p_ref.y;
        dc2.x = p_ref.x + 2;
        dc2.y = p_ref.y - 2;

        ruler_device->pdrawLine(err, dc1, dc2);
        MX_ERROR_CHECK(err);

        dc1.x = p_ref.x;
        dc1.y = p_ref.y;
        dc2.x = p_ref.x + 2;
        dc2.y = p_ref.y + 2;

        ruler_device->pdrawLine(err, dc1, dc2);
        MX_ERROR_CHECK(err);

        break;
    case mx_centre:
        dc1.x = p_ref.x;
        dc1.y = p_ref.y - 2;
        dc2.x = p_ref.x;
        dc2.y = p_ref.y + 4;

        ruler_device->pdrawLine(err, dc1, dc2);
        MX_ERROR_CHECK(err);

        dc1.x = p_ref.x;
        dc1.y = p_ref.y - 2;
        dc2.x = p_ref.x - 2;
        dc2.y = p_ref.y;

        ruler_device->pdrawLine(err, dc1, dc2);
        MX_ERROR_CHECK(err);

        dc1.x = p_ref.x;
        dc1.y = p_ref.y - 2;
        dc2.x = p_ref.x + 2;
        dc2.y = p_ref.y;

        ruler_device->pdrawLine(err, dc1, dc2);
        MX_ERROR_CHECK(err);

        break;
    case mx_centre_on_dp:
        dc1.x = p_ref.x;
        dc1.y = p_ref.y - 4;
        dc2.x = p_ref.x;
        dc2.y = p_ref.y - 4;

        ruler_device->pdrawLine(err, dc1, dc2);
        MX_ERROR_CHECK(err);

        dc1.x = p_ref.x;
        dc1.y = p_ref.y - 2;
        dc2.x = p_ref.x;
        dc2.y = p_ref.y + 4;

        ruler_device->pdrawLine(err, dc1, dc2);
        MX_ERROR_CHECK(err);

        dc1.x = p_ref.x;
        dc1.y = p_ref.y - 2;
        dc2.x = p_ref.x - 2;
        dc2.y = p_ref.y;

        ruler_device->pdrawLine(err, dc1, dc2);
        MX_ERROR_CHECK(err);

        dc1.x = p_ref.x;
        dc1.y = p_ref.y - 2;
        dc2.x = p_ref.x + 2;
        dc2.y = p_ref.y;

        ruler_device->pdrawLine(err, dc1, dc2);
        MX_ERROR_CHECK(err);
    }
    return;
abort:;
}

void mx_ruler_bar::erase_tab(int& err, mx_tabstop_type_t t, float x, float y)
{
    mx_doc_coord_t ref;
    mx_ipoint dc1, dc2, p_ref;

    mx_fill_style fs("lightgrey");

    ruler_device->setDefaultFillStyle(err, fs);
    MX_ERROR_CHECK(err);

    ref.p.x = x;
    ref.p.y = y;

    p_ref = ruler_device->pixelSnap(ruler_device->frame2dev(ref));

    switch (t) {
    case mx_automatic:
    case mx_left:
        dc1.x = p_ref.x - 5;
        dc1.y = p_ref.y - 3;
        dc2.x = p_ref.x + 1;
        dc2.y = p_ref.y + 3;
        break;

    case mx_right:
        dc1.x = p_ref.x - 1;
        dc1.y = p_ref.y - 3;
        dc2.x = p_ref.x + 6;
        dc2.y = p_ref.y + 3;
        break;

    case mx_centre:
        dc1.x = p_ref.x - 3;
        dc1.y = p_ref.y - 2;
        dc2.x = p_ref.x + 3;
        dc2.y = p_ref.y + 5;
        break;

    case mx_centre_on_dp:
        dc1.x = p_ref.x - 3;
        dc1.y = p_ref.y - 4;
        dc2.x = p_ref.x + 3;
        dc2.y = p_ref.y + 5;
        break;
    }

    ruler_device->pfillRect(err, dc1, dc2, defaultAngle);
    MX_ERROR_CHECK(err);
abort:;
}

void mx_ruler_bar::draw_left_indent(int& err, mx_ruler* ruler)
{
    mx_doc_coord_t ref;

    mx_ipoint p_ref, dc1, dc2;

    int i;

    ref.p.x = ruler->left_indent * resolution;
    ref.p.y = TAB_MM_POS;

    p_ref = ruler_device->pixelSnap(ruler_device->frame2dev(ref));

    for (i = 0; i < 5; i++) {
        dc1.x = p_ref.x - 4 + i;
        dc1.y = p_ref.y - i;
        dc2.x = p_ref.x;
        dc2.y = p_ref.y - i;

        ruler_device->pdrawLine(err, dc1, dc2);
        MX_ERROR_CHECK(err);
    }
abort:;
}

void mx_ruler_bar::draw_right_indent(int& err, mx_ruler* ruler)
{
    mx_doc_coord_t ref;
    mx_ipoint p_ref, dc1, dc2;

    int i;

    ref.p.x = (paper_width - ruler->right_indent) * resolution;
    ref.p.y = TAB_MM_POS;

    p_ref = ruler_device->pixelSnap(ruler_device->frame2dev(ref));

    for (i = 0; i < 5; i++) {
        dc1.x = p_ref.x;
        dc1.y = p_ref.y - i;
        dc2.x = p_ref.x + 4 - i;
        dc2.y = p_ref.y - i;

        ruler_device->pdrawLine(err, dc1, dc2);
        MX_ERROR_CHECK(err);

        dc1.x = p_ref.x;
        dc1.y = p_ref.y + i;
        dc2.x = p_ref.x + 4 - i;
        dc2.y = p_ref.y + i;

        ruler_device->pdrawLine(err, dc1, dc2);
        MX_ERROR_CHECK(err);
    }
abort:;
}

void mx_ruler_bar::draw_first_line_indent(int& err, mx_ruler* ruler)
{
    mx_doc_coord_t ref;
    mx_ipoint dc1, dc2, p_ref;
    int i;

    ref.p.x = (ruler->left_indent + ruler->first_line_indent) * resolution;
    ref.p.y = TAB_MM_POS;

    p_ref = ruler_device->pixelSnap(ruler_device->frame2dev(ref));

    for (i = 0; i < 5; i++) {
        dc1.x = p_ref.x - 4 + i;
        dc1.y = p_ref.y + i + 2;
        dc2.x = p_ref.x;
        dc2.y = p_ref.y + i + 2;

        ruler_device->pdrawLine(err, dc1, dc2);
        MX_ERROR_CHECK(err);
    }
abort:;
}

void mx_ruler_bar::draw_indents(int& err)
{
    int i;

    mx_point mmSize = ruler_device->getMMsize();
    mx_doc_coord_t c1(mmSize.x, TICK_MM_POS);

    mx_ipoint dc1;

    ruler_frame->setFalseOrigin(mx_point(left, 0));

    mx_fill_style fs("lightgrey");
    mx_line_style ls(mx_solid, 0.0, "black");

    dc1 = ruler_device->pixelSnap(ruler_device->frame2dev(c1));

    ruler_device->setDefaultFillStyle(err, fs);
    MX_ERROR_CHECK(err);

    ruler_device->pfillRect(err,
        mx_ipoint(0, 0),
        mx_ipoint(dc1.x, dc1.y - 1), defaultAngle);

    if (num_rulers == 1) {
        ls.colour = mx_colour(0, 0, 0, "black");
    } else {
        ls.colour = mx_colour(0, 0, 0, "DarkOliveGreen");
    }

    ruler_device->setDefaultLineStyle(err, ls);
    MX_ERROR_CHECK(err);

    for (i = 0; i < num_rulers; i++) {
        draw_first_line_indent(err, rulers[i]);
        MX_ERROR_CHECK(err);

        draw_right_indent(err, rulers[i]);
        MX_ERROR_CHECK(err);

        draw_left_indent(err, rulers[i]);
        MX_ERROR_CHECK(err);
    }

abort:
    ruler_frame->setFalseOrigin(mx_point(0, 0));
}

void mx_ruler_bar::draw_whole_ruler(int& err)
{
    draw_basic_ruler(err);
    MX_ERROR_CHECK(err);

    if (sensitive) {
        draw_indents(err);
        MX_ERROR_CHECK(err);

        draw_tabs(err);
        MX_ERROR_CHECK(err);
    }

    draw_tab_choice(err);
    MX_ERROR_CHECK(err);

    return;

abort:;
}

void mx_ruler_bar::draw_tab_choice(int& err)
{
    mx_doc_coord_t c1, c2;
    mx_ipoint dc1, dc2, dc3, dc4;
    mx_tabstop_type_t t;
    int i, x;
    mx_ipoint ip;

    mx_fill_style fs;
    mx_line_style ls(mx_solid, 0.0, "white");
    mx_colour top_colour(0xef, 0xef, 0xef, NULL);
    mx_colour bottom_colour(0x75, 0x75, 0x75, NULL);
    mx_colour fill_colour(0xd3, 0xd3, 0xd3, NULL);
    mx_colour black(0, 0, 0, "black");

    ruler_device->setDefaultFillStyle(err, fs);
    MX_ERROR_CHECK(err);

    ruler_device->setDefaultLineStyle(err, ls);
    MX_ERROR_CHECK(err);

    ruler_frame->setFalseOrigin(mx_point(0, 0));

    c1.p.x = 5.0;
    c1.p.y = 12.0;

    dc1 = dc2 = ruler_device->pixelSnap(ruler_device->frame2dev(c1));

    dc2.x += 4 * TAB_CHOICE_BOX_WIDTH;
    dc2.y += TAB_CHOICE_BOX_HEIGHT;

    ruler_device->setForegroundColour(err, fill_colour);
    MX_ERROR_CHECK(err);

    ruler_device->pfillRect(err, dc1, dc2, defaultAngle);
    MX_ERROR_CHECK(err);

    for (i = 0; i < 4; i++) {
        dc2 = dc1;
        dc2.x += i * TAB_CHOICE_BOX_WIDTH;
        dc3 = dc4 = dc2;

        ruler_device->setForegroundColour(err, bottom_colour);
        MX_ERROR_CHECK(err);

        dc3.x += TAB_CHOICE_BOX_WIDTH - 1;
        dc4.x += TAB_CHOICE_BOX_WIDTH - 1;
        dc4.y += TAB_CHOICE_BOX_HEIGHT;

        ruler_device->pdrawLine(err, dc3, dc4);
        MX_ERROR_CHECK(err);

        dc3.x -= TAB_CHOICE_BOX_WIDTH - 1;
        dc3.y += TAB_CHOICE_BOX_HEIGHT;

        ruler_device->pdrawLine(err, dc4, dc3);
        MX_ERROR_CHECK(err);

        ruler_device->setForegroundColour(err, top_colour);
        MX_ERROR_CHECK(err);

        dc3 = dc4 = dc2;

        dc4.x += TAB_CHOICE_BOX_WIDTH - 1;

        ruler_device->pdrawLine(err, dc3, dc4);
        MX_ERROR_CHECK(err);

        dc4.x -= TAB_CHOICE_BOX_WIDTH - 1;
        dc4.y += TAB_CHOICE_BOX_HEIGHT;

        ruler_device->pdrawLine(err, dc3, dc4);
        MX_ERROR_CHECK(err);
    }

    switch (tab_type) {
    default:
    case mx_left:
        i = 0;
        break;
    case mx_right:
        i = 1;
        break;
    case mx_centre:
        i = 2;
        break;
    case mx_centre_on_dp:
        i = 3;
        break;
    }

    dc2 = dc1;
    dc2.x += i * TAB_CHOICE_BOX_WIDTH;
    dc3 = dc4 = dc2;

    ruler_device->setForegroundColour(err, top_colour);
    MX_ERROR_CHECK(err);

    dc3.x += TAB_CHOICE_BOX_WIDTH - 1;
    dc4.x += TAB_CHOICE_BOX_WIDTH - 1;
    dc4.y += TAB_CHOICE_BOX_HEIGHT;

    ruler_device->pdrawLine(err, dc3, dc4);
    MX_ERROR_CHECK(err);

    dc3.x -= TAB_CHOICE_BOX_WIDTH - 1;
    dc3.y += TAB_CHOICE_BOX_HEIGHT;

    ruler_device->pdrawLine(err, dc4, dc3);
    MX_ERROR_CHECK(err);

    ruler_device->setForegroundColour(err, bottom_colour);
    MX_ERROR_CHECK(err);

    dc3 = dc4 = dc2;

    dc4.x += TAB_CHOICE_BOX_WIDTH - 1;

    ruler_device->pdrawLine(err, dc3, dc4);
    MX_ERROR_CHECK(err);

    dc4.x -= TAB_CHOICE_BOX_WIDTH - 1;
    dc4.y += TAB_CHOICE_BOX_HEIGHT;

    ruler_device->pdrawLine(err, dc3, dc4);
    MX_ERROR_CHECK(err);

    ruler_device->setForegroundColour(err, black);
    MX_ERROR_CHECK(err);

    for (i = 0; i < 4; i++) {
        switch (i) {
        default:
        case 0:
            x = 2;
            t = mx_left;
            break;
        case 1:
            x = -2;
            t = mx_right;
            break;
        case 2:
            x = 0;
            t = mx_centre;
            break;
        case 3:
            x = 0;
            t = mx_centre_on_dp;
            break;
        }

        ip.y = dc1.y + (TAB_CHOICE_BOX_HEIGHT / 2);
        ip.x = x + dc1.x + (i * TAB_CHOICE_BOX_WIDTH) + (TAB_CHOICE_BOX_WIDTH / 2);
        c2 = ruler_device->dev2frame(ip);

        draw_tab(err, t, c2.p.x, c2.p.y);
        MX_ERROR_CHECK(err);
    }

abort:;
}

void mx_ruler_bar::draw_ticks(int& err)
{
    ruler_frame->setFalseOrigin(mx_point(left, 0));

    if (units == mx_mm) {
        draw_mm_ticks(err);
        MX_ERROR_CHECK(err);
    } else {
        if (units == mx_points) {
            draw_point_ticks(err);
            MX_ERROR_CHECK(err);
        } else {
            draw_inch_ticks(err);
            MX_ERROR_CHECK(err);
        }
    }
abort:
    ruler_frame->setFalseOrigin(mx_point(0, 0));
}

void mx_ruler_bar::draw_mm_ticks(int& err)
{
    mx_point mmSize = ruler_device->getMMsize();
    mx_doc_coord_t dc1, dc2, dc3;
    mx_ipoint p1, p2;
    char s[100];

    mx_colour c1;
    mx_colour c2(0xef, 0xef, 0xef, NULL);

    c1.name = "black";

    int x, start, end;

    start = -(int)(left / resolution) - 1;
    end = (int)((mmSize.x - left) / resolution) + 1;

    for (x = start; x < end; x++) {
        dc1.p.x = dc2.p.x = x * resolution;
        dc1.p.y = dc2.p.y = TICK_MM_POS;

        if ((x % 10) == 0) {
            dc2.p.y += 4;

            sprintf(s, "%d", x / 10);

            dc3.p.x = dc1.p.x + 1;
            dc3.p.y = TICK_MM_POS + 5;

            ruler_device->drawText(err, s, dc3, *cs);
            MX_ERROR_CHECK(err);
        } else if ((x % 2) == 0) {
            dc2.p.y += 2;
        } else {
            continue;
        }

        p1 = ruler_device->pixelSnap(ruler_device->frame2dev(dc1));
        p2 = ruler_device->pixelSnap(ruler_device->frame2dev(dc2));

        ruler_device->setForegroundColour(err, c1);
        MX_ERROR_CHECK(err);

        ruler_device->pdrawLine(err, p1, p2);
        MX_ERROR_CHECK(err);

        ruler_device->setForegroundColour(err, c2);
        MX_ERROR_CHECK(err);

        p1.x++;
        p2.x++;
        ruler_device->pdrawLine(err, p1, p2);
        MX_ERROR_CHECK(err);
    }

    return;
abort:;
}

void mx_ruler_bar::draw_inch_ticks(int& err)
{
    mx_point mmSize = ruler_device->getMMsize();
    mx_doc_coord_t dc1, dc2, dc3;
    char s[100];
    mx_ipoint p1, p2;

    int x, start, end;

    mx_colour c1;
    mx_colour c2(0xef, 0xef, 0xef, NULL);

    c1.name = "black";

    start = -(int)MX_MM_TO_INCHES((left * 8.0 / resolution)) - 1;
    end = (int)MX_MM_TO_INCHES((mmSize.x - left) * 8.0 / resolution) + 1;

    for (x = start; x < end; x++) {
        dc1.p.x = dc2.p.x = MX_INCHES_TO_MM(x) * resolution / 8.0;
        dc1.p.y = dc2.p.y = TICK_MM_POS;

        if ((x % 8) == 0) {
            dc2.p.y += 4;

            sprintf(s, "%d", x / 8);

            dc3.p.x = dc1.p.x + 1;
            dc3.p.y = TICK_MM_POS + 5;

            ruler_device->drawText(err, s, dc3, *cs);
            MX_ERROR_CHECK(err);
        } else if ((x % 4) == 0) {
            dc2.p.y += 2;
        } else {
            dc2.p.y++;
        }

        p1 = ruler_device->pixelSnap(ruler_device->frame2dev(dc1));
        p2 = ruler_device->pixelSnap(ruler_device->frame2dev(dc2));

        ruler_device->setForegroundColour(err, c1);
        MX_ERROR_CHECK(err);

        ruler_device->pdrawLine(err, p1, p2);
        MX_ERROR_CHECK(err);

        ruler_device->setForegroundColour(err, c2);
        MX_ERROR_CHECK(err);

        p1.x++;
        p2.x++;
        ruler_device->pdrawLine(err, p1, p2);
        MX_ERROR_CHECK(err);
    }
    return;
abort:;
}

void mx_ruler_bar::draw_point_ticks(int& err)
{
    mx_point mmSize = ruler_device->getMMsize();
    mx_doc_coord_t dc1, dc2, dc3;
    char s[10];
    mx_ipoint p1, p2;

    int x, start, end;

    mx_colour c1;
    mx_colour c2(0xef, 0xef, 0xef, NULL);

    c1.name = "black";

    start = -(int)MX_MM_TO_POINTS((left / resolution)) - 1;
    end = (int)MX_MM_TO_POINTS((mmSize.x - left) / resolution) + 1;

    for (x = start; x < end; x++) {
        dc1.p.x = dc2.p.x = MX_POINTS_TO_MM(x) * resolution;
        dc1.p.y = dc2.p.y = TICK_MM_POS;
        dc2.p.x = dc1.p.x;

        if ((x % 100) == 0) {
            dc2.p.y += 4;

            sprintf(s, "%d", x);

            dc3.p.x = dc1.p.x + 1;
            dc3.p.y = TICK_MM_POS + 5;

            ruler_device->drawText(err, s, dc3, *cs);
            MX_ERROR_CHECK(err);
        } else if ((x % 50) == 0) {
            dc2.p.y += 2;
        } else if ((x % 10) == 0) {
            dc2.p.y++;
        } else {
            continue;
        }

        p1 = ruler_device->pixelSnap(ruler_device->frame2dev(dc1));
        p2 = ruler_device->pixelSnap(ruler_device->frame2dev(dc2));

        ruler_device->setForegroundColour(err, c1);
        MX_ERROR_CHECK(err);

        ruler_device->pdrawLine(err, p1, p2);
        MX_ERROR_CHECK(err);

        ruler_device->setForegroundColour(err, c2);
        MX_ERROR_CHECK(err);

        p1.x++;
        p2.x++;
        ruler_device->pdrawLine(err, p1, p2);
        MX_ERROR_CHECK(err);
    }
    return;

abort:;
}

void mx_ruler_bar::draw_basic_ruler(int& err)
{
    mx_ipoint dc1, dc2;

    ruler_frame->setFalseOrigin(mx_point(0, 0));

    mx_point mmSize = ruler_device->getMMsize();
    mx_ipoint pixelSize = ruler_device->getDevicePixelSize();

    mx_fill_style fs1("lightgrey");

    mx_line_style ls(mx_solid, 0.0, "black");

    mx_colour c1(0x75, 0x75, 0x75, NULL);
    mx_colour c2;
    mx_colour c3(0xef, 0xef, 0xef, NULL);

    c2.name = "black";

    if (font == NULL) {
        font = new mx_font(err, mx_font::get_default_roman_font(), 10, mx_normal);
        MX_ERROR_CHECK(err);

        cs = new mx_char_style(*font);
        MX_ERROR_CHECK(err);
    }

    ruler_device->setDefaultFillStyle(err, fs1);
    MX_ERROR_CHECK(err);

    ruler_device->pfillRect(err, mx_ipoint(0, 0), pixelSize, defaultAngle);
    MX_ERROR_CHECK(err);

    dc1 = ruler_device->pixelSnap(ruler_device->frame2dev(
        mx_doc_coord_t(0, TICK_MM_POS)));

    dc1.x = 0;
    dc2.x = pixelSize.x;
    dc2.y = dc1.y;

    ruler_device->setDefaultLineStyle(err, ls);
    MX_ERROR_CHECK(err);

    dc1.y += 2;
    dc2.y += 2;

    ruler_device->setForegroundColour(err, c3);
    MX_ERROR_CHECK(err);

    ruler_device->pdrawLine(err, dc1, dc2);
    MX_ERROR_CHECK(err);

    draw_ticks(err);
    MX_ERROR_CHECK(err);

    dc1.y -= 2;
    dc2.y -= 2;

    ruler_device->setForegroundColour(err, c1);
    MX_ERROR_CHECK(err);

    ruler_device->pdrawLine(err, dc1, dc2);
    MX_ERROR_CHECK(err);

    dc1.y++;
    dc2.y++;

    ruler_device->setForegroundColour(err, c2);
    MX_ERROR_CHECK(err);

    ruler_device->pdrawLine(err, dc1, dc2);
    MX_ERROR_CHECK(err);

    return;
abort:;
}

// this is only called for changed of resolution or units
void mx_ruler_bar::display(int& err, float res, mx_unit_e u)
{
    left *= res / resolution;
    units = u;
    resolution = res;

    draw_whole_ruler(err);
    MX_ERROR_CHECK(err);

    ruler_device->pixmapCommit();
abort:;
}

void mx_ruler_bar::set_left_origin(int& err, float l)
{
    float new_left = l * resolution;

    if (new_left != left) {
        left = new_left;

        draw_whole_ruler(err);
        MX_ERROR_CHECK(err);

        ruler_device->pixmapCommit();
    }
abort:;
}

void mx_ruler_bar::set_sensitive(int& err, bool b)
{
    sensitive = b;
}
