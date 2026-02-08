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
 * MODULE/CLASS : mx_text_area_painter
 *
 * AUTHOR : Andrew Haisley
 *
 *
 *
 * DESCRIPTION:
 *
 * A class to draw text areas onto devices
 *
 *
 *
 *
 *
 */

#include <limits.h>
#include <mx.h>
#include <mx_cw_t.h>
#include <mx_device.h>
#include <mx_frame.h>
#include <mx_list.h>
#include <mx_list_iter.h>
#include <mx_nlist.h>
#include <mx_nlist_iter.h>
#include <mx_para_pos.h>
#include <mx_paragraph.h>
#include <mx_simple_w.h>
#include <mx_text_area.h>
#include <mx_txt_paint.h>
#include <mx_txt_pos.h>

mx_text_area_painter* mx_text_area_painter::the_painter = NULL;

mx_text_area_painter* mx_text_area_painter::painter()
{
    init();
    return the_painter;
}

void mx_text_area_painter::init()
{
    if (the_painter == NULL) {
        the_painter = new mx_text_area_painter();
    }
}

void mx_text_area_painter::draw(int& err, mx_doc_rect_t& r, mx_paintable& o)
{
    mx_text_area* a = (mx_text_area*)&o;
    mx_paragraph* para;
    int n, i;
    mx_border_style* bs = a->get_paragraph_style()->get_border_style();

    float height, y_offset = 0;
    mx_rect temp;

    current_area = a;

    // make sure the stuff is there to be drawn
    a->cc_unserialise_attributes(err);
    MX_ERROR_CHECK(err);

    mx_area_painter::draw_area_background(err, a);
    MX_ERROR_CHECK(err);

    mx_area_painter::draw_area_border(err, a);
    MX_ERROR_CHECK(err);

    mx_area_painter::add_border_to_false_origin(err, a);
    MX_ERROR_CHECK(err);

    n = a->num_paragraphs(err);
    MX_ERROR_CHECK(err);

    para = a->get(err, 0);
    MX_ERROR_CHECK(err);

    {
        // adjust the clipping rectangle for the border
        mx_point adjust(bs->distance_from_contents, bs->distance_from_contents);
        if (bs->left_style.line_type != mx_no_line) {
            adjust.x += bs->left_style.width;
        }
        if (bs->top_style.line_type != mx_no_line) {
            adjust.y += bs->top_style.width;
        }
        r.r -= adjust;
    }

    for (i = 0; i < n; i++) {
        para = a->get(err, i);
        MX_ERROR_CHECK(err);

        height = para->get_height();

        // filter for y position within document rectangle - should also
        // do y position at some point.
        temp = mx_rect(0, y_offset, a->get_width(err), y_offset + height);
        MX_ERROR_CHECK(err);

        if (r.r.intersects(temp)) {
            draw_paragraph(err, para, r.r, y_offset);
            MX_ERROR_CHECK(err);
        } else {
            // if the paragraph is below the view port, we can stop here as
            // all following paragraphs will be further down.
            if (temp.topLeft() > r.r.topLeft()) {
                break;
            }
        }

        y_offset += height;
    }

abort:;
}

void mx_text_area_painter::draw_paragraph(int& err,
    mx_paragraph* para,
    const mx_rect& r,
    float y_offset)
{
    bool done_loop = FALSE;
    float y;
    int i, n = para->num_lines();

    current_para = para;

    draw_background(err, para, y_offset);
    MX_ERROR_CHECK(err);

    draw_border(err, para, r, y_offset);
    MX_ERROR_CHECK(err);

    for (i = 1; i < n && !done_loop; i++) {
        y = y_offset + para->line_ypos(err, i);
        MX_ERROR_CHECK(err);

        if (y >= r.yb) {
            draw_paragraph_line(err, para, r, y_offset, i - 1);
            MX_ERROR_CHECK(err);

            done_loop = (y > r.yt);
        }
    }

    // draw the last line
    draw_paragraph_line(err, para, r, y_offset, i - 1);
    MX_ERROR_CHECK(err);
abort:;
}

void mx_text_area_painter::draw_underline(int& err, const mx_doc_coord_t& p2,
    mx_char_style& cs_before)
{
    if (cs_before.effects == mx_underline) {
        mx_line_style ls(mx_solid, 0.5);
        mx_doc_coord_t p1 = p2;
        p1.p.x = underline_start_x;

        ls.colour = cs_before.colour;

        device->drawLine(err, p1, p2, &ls);
        MX_ERROR_CHECK(err);
    }

    underline_start_x = p2.p.x;
abort:;
}

void mx_text_area_painter::draw_paragraph_line(int& err, mx_paragraph* para,
    const mx_rect& redraw_rect,
    float y_offset, int line)
{
    mx_rect r = redraw_rect;
    mx_word* w;
    mx_word* prev_w = NULL;
    mx_list* row;

    float left_gap; // position of the leftmost word (ie, the gap between the
                    // margins edge and the left word, to see how much the
                    // line has been shifted by reformatting
    float left;

    row = para->words.row(err, line);
    MX_ERROR_CHECK(err);

    w = (mx_word*)row->get(err, 0);
    MX_ERROR_CHECK(err);

    left_gap = w->x;
    left = para->get_style_to_use().get_ruler()->left_indent;

    // adjust the redraw rect for the left indent
    r -= mx_point(left, 0);

    underline_start_x = left + left_gap;

    {
        bool done_loop = FALSE;
        mx_list_iterator iter(*row);
        while (!done_loop && iter.more()) {
            w = (mx_word*)iter.data();

            if (prev_w != NULL && w->x >= r.xb) {
                switch (prev_w->rtti_class()) {
                case mx_simple_word_class_e:
                case mx_field_word_class_e:
                    draw_simple_word(err, left, y_offset, prev_w);
                    MX_ERROR_CHECK(err);
                    break;
                case mx_complex_word_class_e:
                    draw_complex_word(err, r, left, y_offset, (mx_complex_word*)prev_w);
                    MX_ERROR_CHECK(err);
                    break;
                case mx_break_word_class_e:
                    draw_break_word(err, left, y_offset, (mx_break_word*)prev_w);
                    MX_ERROR_CHECK(err);
                    break;
                case mx_space_word_class_e:
                    draw_space_word(err, left, y_offset, (mx_space_word*)prev_w, w,
                        left_gap);
                    MX_ERROR_CHECK(err);
                    break;
                default:
                    break;
                }
                done_loop = (w->x > r.xt);
            }
            prev_w = w;
        }
    }

    if (w->x < r.xt) {
        switch (w->rtti_class()) {
        case mx_simple_word_class_e:
        case mx_field_word_class_e:
            draw_simple_word(err, left, y_offset, w);
            MX_ERROR_CHECK(err);
            break;
        case mx_complex_word_class_e:
            draw_complex_word(err, r, left, y_offset, (mx_complex_word*)w);
            MX_ERROR_CHECK(err);
            break;
        case mx_break_word_class_e:
            draw_break_word(err, left, y_offset, (mx_break_word*)w);
            MX_ERROR_CHECK(err);
            break;
        case mx_space_word_class_e:
            draw_space_word(err, left, y_offset, (mx_space_word*)w, NULL,
                left_gap);
            MX_ERROR_CHECK(err);
            break;
        default:
            break;
        }
    }
    draw_underline(err, mx_doc_coord_t(w->x + left, w->y + y_offset),
        *w->get_style());
    MX_ERROR_CHECK(err);

abort:;
}

mx_text_area_painter::mx_text_area_painter()
    : mx_area_painter()
{
}

mx_text_area_painter::~mx_text_area_painter()
{
}

mx_line_style mx_text_area_painter::get_line_style_for_edge(mx_border_style* s, int i)
{
    mx_line_style ls(mx_no_line, 0.0);

    switch (current_para->get_type()) {
    case mx_paragraph::mx_paragraph_start_e:
        switch (i % 4) {
        case 0:
            return s->top_style;
        case 1:
            return s->right_style;
        case 2:
            return ls;
        case 3:
            return s->left_style;
        }
    case mx_paragraph::mx_paragraph_middle_e:
        switch (i % 4) {
        case 0:
            return ls;
        case 1:
            return s->right_style;
        case 2:
            return ls;
        case 3:
            return s->left_style;
        }
    case mx_paragraph::mx_paragraph_end_e:
        switch (i % 4) {
        case 0:
            return ls;
        case 1:
            return s->right_style;
        case 2:
            return s->bottom_style;
        case 3:
            return s->left_style;
        }
    default:
    case mx_paragraph::mx_paragraph_whole_e:
        switch (i % 4) {
        case 0:
            return s->top_style;
        case 1:
            return s->right_style;
        case 2:
            return s->bottom_style;
        case 3:
            return s->left_style;
        }
    }
    return ls;
}

void mx_text_area_painter::get_coords_for_edge(
    int& err,
    mx_line_style& s,
    int i,
    mx_point& p1,
    mx_point& p2)
{
    float left, right, top, bottom;

    left = current_para->get_style_to_use().get_ruler()->left_indent;

    right = current_area->width_minus_border(err) - current_para->get_style_to_use().get_ruler()->right_indent;
    MX_ERROR_CHECK(err);

    top = current_para->get_style_to_use().space_before;
    bottom = current_para->get_height() - current_para->get_style_to_use().space_after;

    switch (i % 4) {
    default:
    case 0:
        p1.x = left;
        p1.y = top + s.width / 2;
        p2.x = right;
        p2.y = p1.y;
        break;
    case 1:
        p1.x = right - s.width / 2;
        p1.y = top;
        p2.x = p1.x;
        p2.y = bottom;
        break;
    case 2:
        p1.y = bottom - s.width / 2;
        p1.x = right;
        p2.y = p1.y;
        p2.x = left;
        break;
    case 3:
        p1.x = left + s.width / 2;
        p1.y = bottom;
        p2.x = p1.x;
        p2.y = top;
        break;
    }
abort:;
}

void mx_text_area_painter::draw_border(int& err, mx_paragraph* para, const mx_rect& r, float y_offset)
{
    mx_line_style ls;

    mx_point p1, p2;
    mx_border_style* bs;

    int i;

    for (i = 0; i < 4; i++) {
        bs = para->get_style_to_use().get_border_style();

        ls = get_line_style_for_edge(bs, i);

        if (ls.line_type != mx_no_line) {
            get_coords_for_edge(err, ls, i, p1, p2);
            MX_ERROR_CHECK(err);

            p1.y += y_offset;
            p2.y += y_offset;

            if ((((p1.y + ls.width) < r.yb) && ((p2.y + ls.width) < r.yb)) || (((p1.y - ls.width) > r.yt) && ((p2.y - ls.width) > r.yt))) {
                continue;
            }

            grid_paint.add(err, ls, p1, p2);
            MX_ERROR_CHECK(err);
        }
    }
abort:;
}

static void draw_text_in_chunks(int& err, mx_device* device,
    mx_doc_coord_t& pos,
    const char* str, uint32 word_len,
    mx_char_style* s)
{
    static uint32 chunk_size = 4;
    uint32 i;

    while (word_len > chunk_size) {
        device->drawText(err, (char*)str, chunk_size, pos, *s);
        MX_ERROR_CHECK(err);

        for (i = 0; i < chunk_size; i++) {
            pos.p.x += s->width(*str++);
        }
        word_len -= chunk_size;
    }

    device->drawText(err, (char*)str, word_len, pos, *s);
    MX_ERROR_CHECK(err);

abort:;
}

void mx_text_area_painter::draw_simple_word(int& err, float x_offset, float y_offset, mx_word* w)
{
    mx_doc_coord_t pos(w->x + x_offset, w->y + y_offset, 0);

    draw_text_in_chunks(err, device, pos, w->cstring(), w->get_num_visible_items(),
        w->get_style());
    MX_ERROR_CHECK(err);

abort:;
}

static void draw_area_cw_t(int& err, const mx_rect& r, mx_point& p,
    mx_device* device, mx_area_cw_t* e)
{
    float w, h;
    mx_doc_coord_t oldFalseOrigin, newFalseOrigin;
    mx_doc_rect_t redraw_rect = r;
    mx_point false_origin_offset = p;

    w = e->a->get_width(err);
    MX_ERROR_CHECK(err);

    h = e->a->get_height(err);
    MX_ERROR_CHECK(err);

    false_origin_offset.y -= h;

    oldFalseOrigin = device->getFrame()->getDocFalseOrigin(err);
    MX_ERROR_CHECK(err);

    newFalseOrigin = oldFalseOrigin;
    newFalseOrigin.p += false_origin_offset;

    // adjust the redraw rectangle for the new false origin
    redraw_rect.r -= false_origin_offset;

    // set where the next character should be drawn
    p.x += w;

    // find the actual draw area by intersecting the redraw rectangle with the
    // areas outline

    if (redraw_rect.r.xb < 0.0)
        redraw_rect.r.xb = 0.0;
    if (redraw_rect.r.yb < 0.0)
        redraw_rect.r.yb = 0.0;
    if (redraw_rect.r.xt > w)
        redraw_rect.r.xt = w;
    if (redraw_rect.r.yt > h)
        redraw_rect.r.yt = h;

    // set the new false origin and draw area

    device->getFrame()->setFalseOrigin(err, newFalseOrigin);
    MX_ERROR_CHECK(err);

    e->a->redraw(err, redraw_rect);
    MX_ERROR_CHECK(err);

    // restore the old false origin

    device->getFrame()->setFalseOrigin(err, oldFalseOrigin);
    MX_ERROR_CHECK(err);

abort:;
}

void mx_text_area_painter::draw_complex_word(int& err, const mx_rect& r,
    float x_offset, float y_offset,
    mx_complex_word* w)
{
    mx_cw_t* e = NULL;
    mx_char_style s = *w->get_style();
    uint32 i, num_items = w->get_num_items();

    mx_doc_coord_t dc(w->x + x_offset, w->y + y_offset, 0);

    for (i = 0; i < num_items; i++) {
        e = (mx_cw_t*)w->get_item(err, i);
        MX_ERROR_CHECK(err);

        switch (e->rtti_class()) {
        case mx_character_cw_t_class_e: {
            char ch = ((mx_character_cw_t*)e)->c;
            device->drawText(err, &ch, 1, dc, s);
            MX_ERROR_CHECK(err);
            dc.p.x += s.width(ch);
            break;
        }
        case mx_style_mod_cw_t_class_e:
            draw_underline(err, dc, s);
            MX_ERROR_CHECK(err);
            e->mod_style(s);
            break;
        case mx_field_word_cw_t_class_e: {
            mx_field_word* fw = &((mx_field_word_cw_t*)e)->w;
            mx_doc_coord_t pos = dc;
            draw_text_in_chunks(err, device, pos, fw->cstring(),
                strlen(fw->cstring()), &s);
            MX_ERROR_CHECK(err);
            dc.p.x += fw->width(s);
            break;
        }
        case mx_area_cw_t_class_e:
            draw_area_cw_t(err, r, dc.p, device, (mx_area_cw_t*)e);
            MX_ERROR_CHECK(err);
            break;
        default:
            // should not be here
            MX_ERROR_THROW(err, MX_ERROR_UNDEFINED);
            break;
        }
    }
abort:;
}

void mx_text_area_painter::draw_line_break(int& err, float x, float y)
{
    mx_doc_coord_t where(x, y, 0);
    mx_ipoint iwhere = device->pixelSnap(device->frame2dev(where));
    mx_line_style ls(mx_solid, 0.0, "grey30");
    mx_ipoint p1, p2;

    device->setDefaultLineStyle(err, ls);
    MX_ERROR_CHECK(err);

    p1 = iwhere;
    p1.y -= 3;

    p2 = p1;
    p2.x += 5;

    device->pdrawLine(err, p1, p2);
    MX_ERROR_CHECK(err);

    p2 = p1;
    p2.x += 2;
    p2.y += 2;

    device->pdrawLine(err, p1, p2);
    MX_ERROR_CHECK(err);

    p2 = p1;
    p2.x += 2;
    p2.y -= 2;

    device->pdrawLine(err, p1, p2);
    MX_ERROR_CHECK(err);

    p1.x += 5;
    p2 = p1;
    p2.y -= 6;

    device->pdrawLine(err, p1, p2);
    MX_ERROR_CHECK(err);

abort:;
}

void mx_text_area_painter::draw_paragraph_break(
    int& err,
    float x_offset,
    float y_offset,
    mx_break_word* w)
{
    mx_char_style s;
    char str[2];

    str[0] = 182;
    str[1] = 0;

    s.get_font()->set_size(w->get_style()->get_font()->get_size());
    s.colour.name = "grey30";

    device->drawText(
        err,
        str,
        mx_doc_coord_t(w->x + x_offset, w->y + y_offset, 0),
        s);
    MX_ERROR_CHECK(err);

abort:;
}

void mx_text_area_painter::draw_break_word(int& err, float x_offset, float y_offset, mx_break_word* w)
{
    if (draw_mode == mx_paint_wp_codes_e) {
        if (w->type() == mx_paragraph_break_e) {
            if (current_para->get_type() == mx_paragraph::mx_paragraph_end_e || current_para->get_type() == mx_paragraph::mx_paragraph_whole_e) {
                draw_paragraph_break(err, x_offset, y_offset, w);
                MX_ERROR_CHECK(err);
            }
        } else if (w->type() == mx_line_break_e) {
            draw_line_break(err, w->x + x_offset, w->y + y_offset);
            MX_ERROR_CHECK(err);
        }
    }
abort:;
}

void mx_text_area_painter::draw_space_word(int& err, float x_offset,
    float y_offset, mx_space_word* w,
    mx_word* next, float left_gap)
{
    int n, i;
    mx_space_word::mx_space_word_item_t t;
    float pos;
    mx_char_style s = *w->get_style();

    n = w->get_num_items();

    for (i = 0; i < n; i++) {
        t = w->item_type(err, i);
        MX_ERROR_CHECK(err);

        pos = w->item_pos(err, i);
        MX_ERROR_CHECK(err);

        switch (t) {
        case mx_space_word::mx_space_word_space_e:
            draw_space_mark(err, pos + x_offset, w->y + y_offset, s);
            MX_ERROR_CHECK(err);
            break;
        case mx_space_word::mx_space_word_tab_e: {
            float right_x = pos + 6;
            mx_ruler* r = current_para->get_style_to_use().get_ruler();
            mx_tabstop tab;

            r->get_right_tab(err, pos - left_gap + r->left_indent, tab);
            MX_ERROR_CHECK(err);

            if (i < n - 1) {
                right_x = w->item_pos(err, i + 1);
                MX_ERROR_CHECK(err);
            } else if (next != NULL) {
                right_x = next->x;
            }

            draw_tab_leader(err, pos + x_offset, right_x + x_offset,
                w->y + y_offset, tab.leader_type, s);
            MX_ERROR_CHECK(err);
            break;
        }
        case mx_space_word::mx_space_word_mod_e: {
            mx_char_style_mod* m = w->get(err, i);
            MX_ERROR_CHECK(err);
            draw_underline(err, mx_doc_coord_t(pos + x_offset, w->y + y_offset), s);
            MX_ERROR_CHECK(err);
            s += *m;
            break;
        }
        default:
            break;
        }
    }
abort:;
}

void mx_text_area_painter::draw_tab_mark(int& err, float left_x, float right_x,
    float y, mx_char_style& s)
{
    mx_doc_coord_t where(right_x, y - 1.5, 0);
    mx_ipoint iwhere = device->pixelSnap(device->frame2dev(where));
    mx_line_style ls(mx_solid, 0.0, "grey30");
    mx_ipoint p1, p2;

    device->setDefaultLineStyle(err, ls);
    MX_ERROR_CHECK(err);

    p1 = iwhere;

    p2 = p1;
    p2.x -= 5;

    device->pdrawLine(err, p1, p2);
    MX_ERROR_CHECK(err);

    p2 = p1;
    p2.x -= 2;
    p2.y -= 2;

    device->pdrawLine(err, p1, p2);
    MX_ERROR_CHECK(err);

    p2 = p1;
    p2.x -= 2;
    p2.y += 2;

    device->pdrawLine(err, p1, p2);
    MX_ERROR_CHECK(err);

abort:;
}
static char d_array[] = "................................................................................................................................................................................................................................................................................................................................................................................................................................................................................................................................................................................................................................................................................................................................................................................................................................................................................................................................................................................................................................................................";
static char h_array[] = "----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------";
static char ul_array[] = "________________________________________________________________________________________________________________________________________________________________________________________________________________________________________________________________________________________________________________________________________________________________________________________________________________________________________________________________________________________________________________________________________________________________________________________________________________________________________________________________________________________________________________________________________________________________________________________________________________________________________________________________________________________________________________________________________________________________________________________________________________________________________________________________";
static char eq_array[] = "================================================================================================================================================================================================================================================================================================================================================================================================================================================================================================================================================================================================================================================================================================================================================================================================================================================================================================================================================================================================================================================================";

void mx_text_area_painter::draw_tab_leader(int& err, float left_x,
    float right_x, float y,
    mx_tabstop_leader_type_t l,
    mx_char_style& s)
{
    char fill_char;
    float fill_char_width;
    char* fill_array = NULL;
    uint32 num_chars;

    if (draw_mode == mx_paint_wp_codes_e) {
        draw_tab_mark(err, left_x, right_x, y, s);
        MX_ERROR_CHECK(err);
    }

    switch (l) {
    case mx_leader_space_e:
        return;
    case mx_leader_dot_e:
        fill_array = d_array;
        fill_char = '.';
        break;
    case mx_leader_hyphen_e:
        fill_array = h_array;
        fill_char = '-';
        break;
    case mx_leader_underline_e:
    case mx_leader_thickline_e:
        fill_array = ul_array;
        fill_char = '_';
        break;
    case mx_leader_equals_e:
        fill_array = eq_array;
        fill_char = '=';
        break;
    default:
        // shouldn't be here
        MX_ERROR_THROW(err, MX_ERROR_UNDEFINED);
    }

    fill_char_width = device->getCharWidth(err, fill_char, s);
    MX_ERROR_CHECK(err);

    num_chars = (uint32)((right_x - left_x) / fill_char_width);

    if (num_chars > 1024)
        num_chars = 1024;

    if (num_chars > 0) {
        device->drawText(err, fill_array, num_chars,
            mx_doc_coord_t(left_x, y, 0), s);
        MX_ERROR_CHECK(err);
    }

abort:;
}

void mx_text_area_painter::draw_space_mark(int& err, float x, float y,
    mx_char_style& s)
{
    if (draw_mode == mx_paint_wp_codes_e) {
        float width;
        mx_fill_style fs("grey30");

        width = s.width(' ');

        device->fillCircle(err, mx_doc_coord_t(x - 0.5 + width / 2, y - 2.0, 0), 1.0, &fs);

        MX_ERROR_CHECK(err);
    }
abort:;
}

void mx_text_area_painter::draw_background(int& err, mx_paragraph* para,
    float y_offset)
{
    mx_doc_coord_t tl;
    mx_doc_coord_t br;
    mx_fill_style* fs = &para->get_style_to_use().get_border_style()->fill_style;

    if (fs->type != mx_fill_transparent) {
        tl.p.x = para->get_style_to_use().get_ruler()->left_indent;
        tl.p.y = y_offset;

        br.p.x = current_area->width_minus_border(err) - para->get_style_to_use().get_ruler()->right_indent;
        MX_ERROR_CHECK(err);

        br.p.y = y_offset + para->get_height();

        switch (current_para->get_type()) {
        case mx_paragraph::mx_paragraph_start_e:
            tl.p.y += para->get_style_to_use().space_before;
            break;
        case mx_paragraph::mx_paragraph_end_e:
            br.p.y -= para->get_style_to_use().space_after;
            break;
        case mx_paragraph::mx_paragraph_whole_e:
            br.p.y -= para->get_style_to_use().space_after;
            tl.p.y += para->get_style_to_use().space_before;
            break;
        default:
        case mx_paragraph::mx_paragraph_middle_e:
            break;
        }

        device->fillRect(err, tl, br, defaultAngle, fs);
        MX_ERROR_CHECK(err);
    }
abort:;
}

void mx_text_area_painter::highlight_in_area(int& err, mx_area* a, mx_area_pos* start, mx_area_pos* end)
{
    mx_text_area_pos* tstart = (mx_text_area_pos*)start;
    mx_text_area_pos* tend = (mx_text_area_pos*)end;

    mx_text_area* ta = (mx_text_area*)a;

    int i;

    mx_area_painter::add_border_to_false_origin(err, a);
    MX_ERROR_CHECK(err);

    for (i = tstart->paragraph_index; i <= tend->paragraph_index; i++) {
        if (i == tstart->paragraph_index) {
            if (i < tend->paragraph_index) {
                // starts in this paragraph and extends into later ones
                highlight_to_end_of_para(err, ta, i, tstart->para_pos);
                MX_ERROR_CHECK(err);
            } else {
                // total within this paragraph
                highlight_in_para(err, ta, i, tstart->para_pos, tend->para_pos);
                MX_ERROR_CHECK(err);
            }
        } else {
            if (i < tend->paragraph_index) {
                // covers whole paragraph
                highlight_whole_para(err, ta, i);
                MX_ERROR_CHECK(err);
            } else {
                // starts in previous paragraph and extends into this one
                highlight_from_start_of_para(err, ta, i, tend->para_pos);
                MX_ERROR_CHECK(err);
            }
        }
    }

abort:;
}

void mx_text_area_painter::highlight_to_end_of_area(int& err, mx_area* a, mx_area_pos* start)
{
    mx_text_area* ta = (mx_text_area*)a;

    mx_text_area_pos* tstart = (mx_text_area_pos*)start;

    int i, np;

    mx_area_painter::add_border_to_false_origin(err, a);
    MX_ERROR_CHECK(err);

    np = ta->num_paragraphs(err);
    MX_ERROR_CHECK(err);

    highlight_to_end_of_para(err, ta, tstart->paragraph_index, tstart->para_pos);
    MX_ERROR_CHECK(err);

    for (i = tstart->paragraph_index + 1; i < np; i++) {
        highlight_whole_para(err, ta, i);
        MX_ERROR_CHECK(err);
    }

abort:;
}

void mx_text_area_painter::highlight_from_start_of_area(int& err, mx_area* a, mx_area_pos* end)
{
    mx_text_area* ta = (mx_text_area*)a;
    mx_text_area_pos* tend = (mx_text_area_pos*)end;

    int i;

    mx_area_painter::add_border_to_false_origin(err, a);
    MX_ERROR_CHECK(err);

    for (i = 0; i < tend->paragraph_index; i++) {
        highlight_whole_para(err, ta, i);
        MX_ERROR_CHECK(err);
    }

    highlight_from_start_of_para(err, ta, tend->paragraph_index, tend->para_pos);
    MX_ERROR_CHECK(err);

abort:;
}

void mx_text_area_painter::highlight_whole_area(int& err, mx_area* a)
{
    mx_text_area* ta = (mx_text_area*)a;

    int i, np;

    mx_area_painter::add_border_to_false_origin(err, a);
    MX_ERROR_CHECK(err);

    np = ta->num_paragraphs(err);
    MX_ERROR_CHECK(err);

    for (i = 0; i < np; i++) {
        highlight_whole_para(err, ta, i);
        MX_ERROR_CHECK(err);
    }

abort:;
}

void mx_text_area_painter::highlight_from_start_of_para(
    int& err,
    mx_text_area* a,
    int i,
    mx_para_pos& pos)
{
    mx_paragraph* p;

    float y;
    int line;

    y = y_offset_for_para(err, a, i);
    MX_ERROR_CHECK(err);

    p = a->get(err, i);
    MX_ERROR_CHECK(err);

    for (line = 0; line < pos.line_index; line++) {
        highlight_line(err, y, p, line);
        MX_ERROR_CHECK(err);
    }

    highlight_from_start_of_line(err, y, p, pos);
    MX_ERROR_CHECK(err);

abort:;
}

void mx_text_area_painter::highlight_in_para(
    int& err,
    mx_text_area* a,
    int i,
    mx_para_pos& start,
    mx_para_pos& end)
{
    mx_doc_coord_t tl, br;
    mx_paragraph* p;

    float y, height, descender;
    int line;

    y = y_offset_for_para(err, a, i);
    MX_ERROR_CHECK(err);

    p = a->get(err, i);
    MX_ERROR_CHECK(err);

    if (start.line_index == end.line_index) {
        tl = start.position(err, p);
        MX_ERROR_CHECK(err);

        tl.p.y += y;

        br = end.position(err, p);
        MX_ERROR_CHECK(err);

        br.p.y += y;

        highlight_height_for_line(err, p, end.line_index, height, descender);
        MX_ERROR_CHECK(err);

        tl.p.y -= height;
        br.p.y -= descender;

        xorRect(err, tl, br);
        MX_ERROR_CHECK(err);
    } else {
        for (line = start.line_index + 1; line < end.line_index; line++) {
            highlight_line(err, y, p, line);
            MX_ERROR_CHECK(err);
        }

        highlight_from_start_of_line(err, y, p, end);
        MX_ERROR_CHECK(err);

        highlight_to_end_of_line(err, y, p, start);
        MX_ERROR_CHECK(err);
    }

abort:;
}

void mx_text_area_painter::highlight_whole_para(
    int& err,
    mx_text_area* a,
    int i)
{
    mx_paragraph* p;

    int line;
    float y;

    p = a->get(err, i);
    MX_ERROR_CHECK(err);

    y = y_offset_for_para(err, a, i);
    MX_ERROR_CHECK(err);

    for (line = 0; line < p->words.get_num_rows(); line++) {
        highlight_line(err, y, p, line);
        MX_ERROR_CHECK(err);
    }
abort:;
}

void mx_text_area_painter::highlight_line(
    int& err,
    float y,
    mx_paragraph* p,
    int line)
{
    mx_para_pos start, end;
    mx_doc_coord_t tl, br;

    float height, descender;

    mx_char_style cs;

    mx_word* w;
    mx_break_word* bw;

    end.moveto_end(err, p, line);
    MX_ERROR_CHECK(err);

    start.line_index = line;

    highlight_height_for_line(err, p, line, height, descender);
    MX_ERROR_CHECK(err);

    tl = start.position(err, p);
    MX_ERROR_CHECK(err);

    tl.p.y += y;

    br = end.position(err, p);
    MX_ERROR_CHECK(err);

    br.p.y += y;

    tl.p.y -= height;
    br.p.y -= descender;

    w = (mx_word*)p->words.item(err, end.line_index, end.word_index);
    MX_ERROR_CHECK(err);

    if (w->rtti_class() == mx_break_word_class_e) {
        bw = (mx_break_word*)w;

        if (bw->type() == mx_paragraph_break_e) {
            br.p.x += cs.width(182);
        } else if (bw->type() == mx_line_break_e) {
            br.p.x += device->dev2frame(6);
        }
    }

    xorRect(err, tl, br);
    MX_ERROR_CHECK(err);

abort:;
}

void mx_text_area_painter::highlight_to_end_of_line(
    int& err,
    float y,
    mx_paragraph* p,
    mx_para_pos& start)
{
    mx_para_pos end;
    mx_doc_coord_t tl, br;

    mx_char_style cs;

    mx_word* w;
    mx_break_word* bw;

    float height, descender;

    end.moveto_end(err, p, start.line_index);
    MX_ERROR_CHECK(err);

    highlight_height_for_line(err, p, start.line_index, height, descender);
    MX_ERROR_CHECK(err);

    tl = start.position(err, p);
    MX_ERROR_CHECK(err);

    tl.p.y += y;

    br = end.position(err, p);
    MX_ERROR_CHECK(err);

    br.p.y += y;

    w = (mx_word*)p->words.item(err, end.line_index, end.word_index);
    MX_ERROR_CHECK(err);

    w->get_end_style(cs);

    tl.p.y -= height;
    br.p.y -= descender;

    if (w->rtti_class() == mx_break_word_class_e) {
        bw = (mx_break_word*)w;

        if (bw->type() == mx_paragraph_break_e) {
            br.p.x += cs.width(182);
        } else if (bw->type() == mx_line_break_e) {
            br.p.x += device->dev2frame(6);
        }
    }

    xorRect(err, tl, br);
    MX_ERROR_CHECK(err);

abort:;
}

void mx_text_area_painter::highlight_from_start_of_line(
    int& err,
    float y,
    mx_paragraph* p,
    mx_para_pos& end)
{
    mx_para_pos start;
    mx_doc_coord_t tl, br;

    float height, descender;

    start.line_index = end.line_index;

    highlight_height_for_line(err, p, start.line_index, height, descender);
    MX_ERROR_CHECK(err);

    tl = start.position(err, p);
    MX_ERROR_CHECK(err);

    tl.p.y += y;

    br = end.position(err, p);
    MX_ERROR_CHECK(err);

    br.p.y += y;

    tl.p.y -= height;
    br.p.y -= descender;

    xorRect(err, tl, br);
    MX_ERROR_CHECK(err);

abort:;
}

void mx_text_area_painter::highlight_to_end_of_para(
    int& err,
    mx_text_area* a,
    int i,
    mx_para_pos& pos)
{
    mx_paragraph* p;

    float y;
    int line, np;

    y = y_offset_for_para(err, a, i);
    MX_ERROR_CHECK(err);

    p = a->get(err, i);
    MX_ERROR_CHECK(err);

    np = p->words.get_num_rows();

    for (line = pos.line_index + 1; line < np; line++) {
        highlight_line(err, y, p, line);
        MX_ERROR_CHECK(err);
    }

    highlight_to_end_of_line(err, y, p, pos);
    MX_ERROR_CHECK(err);

abort:;
}

float mx_text_area_painter::y_offset_for_para(int& err, mx_text_area* a, int i)
{
    int x;
    float res = 0.0;

    mx_paragraph* p;

    for (x = 0; x < i; x++) {
        p = a->get(err, x);
        MX_ERROR_CHECK(err);

        res += p->get_height();
    }

abort:
    return res;
}

void mx_text_area_painter::highlight_height_for_line(
    int& err,
    mx_paragraph* p,
    int line_index,
    float& height,
    float& descender)
{
    p->highlight_height_for_line(err, line_index, height, descender);
    MX_ERROR_CHECK(err);
abort:;
}
