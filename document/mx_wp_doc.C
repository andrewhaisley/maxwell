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
 * MODULE/CLASS : mx_wp_document
 *
 * AUTHOR : Andrew Haisley
 *
 *
 *
 * DESCRIPTION:
 *
 * a document for a word processor
 *
 *
 *
 *
 */

#include <mx.h>
#include <mx_db.h>

#include <mx_area_pos.h>
#include <mx_doc_paint.h>
#include <mx_painter.h>
#include <mx_paragraph.h>
#include <mx_simple_w.h>
#include <mx_sizes.h>
#include <mx_ta_pos.h>
#include <mx_txt_pos.h>
#include <mx_wp_doc.h>
#include <mx_wp_rp.h>

mx_wp_document::mx_wp_document(const char* file, uint32 docid, const char* page_type)
    : mx_document(file, docid)
{
    int err = MX_ERROR_OK;
    int i, n;
    mx_paragraph_style temp_style;

    float page_width = 210.0;
    float page_height = 297.0;

    // any sheets for us?
    n = get_num_sheets(err);
    MX_ERROR_CHECK(err);

    if (n == 0) {
        // new document, set some bits up
        add_default_styles(err);
        MX_ERROR_CHECK(err);

        mx_sheet* sheet;
        mx_text_area* area;

        // get default page type/size
        for (i = 0; i < MX_NUM_PAPER_SIZES; i++) {
            if (strcmp(mx_paper_size_names[i], page_type) == 0) {
                page_width = mx_paper_widths[i];
                page_height = mx_paper_heights[i];
                break;
            }
        }

        // add a sheet
        sheet = add_sheet(err);
        MX_ERROR_CHECK(err);

        sheet->set_height(err, page_height);
        MX_ERROR_CHECK(err);

        sheet->set_width(err, page_width);
        MX_ERROR_CHECK(err);

        sheet->set_margins(err,
            MX_INCHES_TO_MM(1.25),
            MX_INCHES_TO_MM(1.25),
            MX_INCHES_TO_MM(1),
            MX_INCHES_TO_MM(1));
        MX_ERROR_CHECK(err);

        area = sheet->add_text_area(err);
        MX_ERROR_CHECK(err);

        area->set_origin(err, mx_point(MX_INCHES_TO_MM(1.25), MX_INCHES_TO_MM(1)));
        MX_ERROR_CHECK(err);

        area->set_outline(err,
            page_width - MX_INCHES_TO_MM(2.5),
            page_height - MX_INCHES_TO_MM(2));
        MX_ERROR_CHECK(err);

        // reformat the document to put everything in the right place
        reformat(err);
        MX_ERROR_CHECK(err);

        // commit the changes to disk so that reverting goes back to this
        // stage rather than a totally empty document
        commit(err);
        MX_ERROR_CHECK(err);
    }

    return;

abort:
    global_error_trace->print();
}

void mx_wp_document::add_default_styles(int& err)
{
    mx_paragraph_style p;
    mx_char_style c;
    mx_font f;

    mx_tabstop tab(mx_right, 140.0, mx_leader_dot_e);

    // normal style - utopia 12
    f = mx_font(err, mx_font::get_default_roman_font(), 12, mx_normal);
    MX_ERROR_CHECK(err);

    c = mx_char_style(f);
    c.alignment_offset = 3.0;

    p = mx_paragraph_style();

    p.set_char_style(c);
    p.set_name("default");

    add_style(err, &p);
    MX_ERROR_CHECK(err);

    // normal envelope style - utopia 14
    f = mx_font(err, mx_font::get_default_roman_font(), 14, mx_normal);
    MX_ERROR_CHECK(err);

    c = mx_char_style(f);

    c.alignment_offset = 3.0;

    p = mx_paragraph_style();
    MX_ERROR_CHECK(err);

    p.set_char_style(c);
    p.set_name("default envelope");

    add_style(err, &p);
    MX_ERROR_CHECK(err);

    // normal column style - utopia 12
    f = mx_font(err, mx_font::get_default_roman_font(), 12, mx_normal);
    MX_ERROR_CHECK(err);

    c = mx_char_style(f);

    c.alignment_offset = 3.0;

    p = mx_paragraph_style();

    p.set_char_style(c);
    p.set_name("default column");
    p.get_ruler()->left_indent = 10.0;

    add_style(err, &p);
    MX_ERROR_CHECK(err);

    // normal table text style - utopia 12
    f = mx_font(err, mx_font::get_default_roman_font(), 12, mx_normal);
    MX_ERROR_CHECK(err);

    c = mx_char_style(f);

    c.alignment_offset = 3.0;

    p = mx_paragraph_style();

    p.set_char_style(c);
    p.set_name("default table text");
    p.space_before = 0.0;
    p.space_after = 0.0;

    add_style(err, &p);
    MX_ERROR_CHECK(err);

    // body 1 style - utopia 12
    f = mx_font(err, mx_font::get_default_roman_font(), 12, mx_normal);
    MX_ERROR_CHECK(err);

    c = mx_char_style(f);

    c.alignment_offset = 3.0;

    p = mx_paragraph_style();

    p.set_char_style(c);
    p.set_name("body 1");

    add_style(err, &p);
    MX_ERROR_CHECK(err);

    // body 2 style - utopia 12
    f = mx_font(err, mx_font::get_default_roman_font(), 12, mx_normal);
    MX_ERROR_CHECK(err);

    c = mx_char_style(f);

    c.alignment_offset = 3.0;

    p = mx_paragraph_style();

    p.set_char_style(c);
    p.set_name("body 2");

    add_style(err, &p);
    MX_ERROR_CHECK(err);

    // body 3 style - utopia 12
    f = mx_font(err, mx_font::get_default_roman_font(), 12, mx_normal);
    MX_ERROR_CHECK(err);

    c = mx_char_style(f);

    c.alignment_offset = 3.0;

    p = mx_paragraph_style();

    p.set_char_style(c);
    p.set_name("body 3");

    p.get_ruler()->left_indent = MX_INCHES_TO_MM(1);

    add_style(err, &p);
    MX_ERROR_CHECK(err);

    // section 1 - Utopia bold 14
    f = mx_font(err, mx_font::get_default_roman_font(), 14, mx_bold);
    MX_ERROR_CHECK(err);

    c = mx_char_style(f);

    c.alignment_offset = 3.0;

    p = mx_paragraph_style();

    p.set_char_style(c);
    p.set_name("section 1");

    p.is_heading = TRUE;
    p.heading_level = 0;
    p.set_next_style_name("body 1");

    add_style(err, &p);
    MX_ERROR_CHECK(err);

    // section 2 - Utopia bold 12
    f = mx_font(err, mx_font::get_default_roman_font(), 12, mx_bold);

    c = mx_char_style(f);

    c.alignment_offset = 3.0;

    p = mx_paragraph_style();
    MX_ERROR_CHECK(err);

    p.set_char_style(c);
    p.set_name("section 2");
    p.set_next_style_name("body 2");

    p.is_heading = TRUE;
    p.heading_level = 1;

    add_style(err, &p);
    MX_ERROR_CHECK(err);

    // section 3 - Utopia bold 12
    f = mx_font(err, mx_font::get_default_roman_font(), 12, mx_bold);
    MX_ERROR_CHECK(err);

    c = mx_char_style(f);

    c.alignment_offset = 3.0;

    p = mx_paragraph_style();

    p.set_char_style(c);
    p.set_name("section 3");

    p.is_heading = TRUE;
    p.heading_level = 2;
    p.set_next_style_name("body 3");

    p.get_ruler()->left_indent = MX_INCHES_TO_MM(1);

    add_style(err, &p);
    MX_ERROR_CHECK(err);

    // TOC 1
    f = mx_font(err, mx_font::get_default_roman_font(), 18, mx_bold);
    MX_ERROR_CHECK(err);

    c = mx_char_style(f);

    c.alignment_offset = 3.0;

    p = mx_paragraph_style();

    p.is_toc = TRUE;
    p.heading_level = 0;

    p.set_char_style(c);
    p.set_name("TOC 1");

    p.get_ruler()->add_tab(err, tab);
    MX_ERROR_CHECK(err);

    add_style(err, &p);
    MX_ERROR_CHECK(err);

    // TOC 2
    f = mx_font(err, mx_font::get_default_roman_font(), 15, mx_bold);
    MX_ERROR_CHECK(err);

    c = mx_char_style(f);

    c.alignment_offset = 3.0;

    p = mx_paragraph_style();

    p.set_char_style(c);
    p.set_name("TOC 2");

    p.is_toc = TRUE;
    p.heading_level = 1;

    p.get_ruler()->left_indent = MX_INCHES_TO_MM(0.5);
    p.get_ruler()->add_tab(err, tab);
    MX_ERROR_CHECK(err);

    add_style(err, &p);
    MX_ERROR_CHECK(err);

    // TOC 3
    f = mx_font(err, mx_font::get_default_roman_font(), 12, mx_bold);
    MX_ERROR_CHECK(err);

    c = mx_char_style(f);
    c.alignment_offset = 3.0;
    p = mx_paragraph_style();
    p.set_char_style(c);

    p.set_name("TOC 3");
    p.is_toc = TRUE;
    p.heading_level = 2;

    p.get_ruler()->left_indent = MX_INCHES_TO_MM(1.0);
    p.get_ruler()->add_tab(err, tab);
    MX_ERROR_CHECK(err);

    add_style(err, &p);
    MX_ERROR_CHECK(err);

    write_styles(err);
    MX_ERROR_CHECK(err);

abort:;
}

void mx_wp_document::reformat(int& err, mx_wp_doc_pos& start, mx_wp_doc_pos& end)
{
    int i, start_sn, end_sn;
    bool b;
    mx_wp_sheet_pos ss, se;
    mx_sheet* s;

    // reformat the individual sheets/areas in the range given and then repaginate.
    start_sn = start.get_sheet();
    end_sn = end.get_sheet();

    for (i = start_sn; i <= end_sn; i++) {
        s = sheet(err, i);
        MX_ERROR_CHECK(err);

        if (i == start_sn) {
            ss = *(start.get_sheet_pos());

            if (i == end_sn) {
                se = *(end.get_sheet_pos());
            } else {
                se.moveto_end(err, s);
                MX_ERROR_CHECK(err);
            }

            reformat_sheet(err, i, ss, se, b);
            MX_ERROR_CHECK(err);
        } else {
            if (i == end_sn) {
                ss.moveto_start(err, s);
                MX_ERROR_CHECK(err);

                se = *(end.get_sheet_pos());

                reformat_sheet(err, i, ss, se, b);
                MX_ERROR_CHECK(err);
            } else {
                reformat_sheet(err, i, b);
                MX_ERROR_CHECK(err);
            }
        }
    }

abort:;
}

void mx_wp_document::reformat(int& err)
{
    mx_wp_doc_pos start;
    mx_wp_doc_pos end;

    start.moveto_start(err, this);
    MX_ERROR_CHECK(err);

    end.moveto_end(err, this);
    MX_ERROR_CHECK(err);

    reformat(err, start, end);
    MX_ERROR_CHECK(err);

abort:;
}

mx_sheet_size mx_wp_document::get_sheet_visible_size(int& err, int i, mx_painter_mode_t mode)
{
    mx_sheet_size res;
    mx_point p;
    mx_sheet* s;

    s = sheet(err, i);
    MX_ERROR_CHECK(err);

    p.x = s->get_width(err, mode);
    MX_ERROR_CHECK(err);

    p.y = s->get_height(err, mode);
    MX_ERROR_CHECK(err);

    res.setSize(p);

abort:
    return res;
}

mx_text_area* mx_wp_document::header(int& err)
{
    bool b;
    mx_sheet* s;
    mx_text_area* a;

    b = is_header(err);
    MX_ERROR_CHECK(err);

    if (b) {
        s = get_header_footer_sheet(err);
        MX_ERROR_CHECK(err);

        a = (mx_text_area*)s->get_area(err, 0);
        MX_ERROR_CHECK(err);

        return a;
    } else {
        return NULL;
    }

abort:
    return NULL;
}

mx_text_area* mx_wp_document::footer(int& err)
{
    bool b;
    mx_sheet* s;
    mx_text_area* a;

    b = is_footer(err);
    MX_ERROR_CHECK(err);

    if (b) {
        s = get_header_footer_sheet(err);
        MX_ERROR_CHECK(err);

        a = (mx_text_area*)s->get_area(err, 1);
        MX_ERROR_CHECK(err);

        return a;
    } else {
        return NULL;
    }

abort:
    return NULL;
}

void mx_wp_document::reposition_sheet_areas(int& err, int sn)
{
    // loop through the areas. Each is positioned directly below the previous row
    // or directly to the right in rows with more than one area
    // also positioning across the page for non-text areas in arranged
    mx_sheet* s;
    float width, left, right, top, bottom;
    float y_pos;
    int i, n;

    mx_list* areas;
    mx_area* a;

    s = sheet(err, sn);
    MX_ERROR_CHECK(err);

    areas = s->get_area_index(err);
    MX_ERROR_CHECK(err);

    s->get_margins(err, left, right, top, bottom);
    MX_ERROR_CHECK(err);

    width = s->get_width(err);
    MX_ERROR_CHECK(err);

    y_pos = top;
    n = areas->get_num_items();

    for (i = 0; i < n; i++) {
        a = (mx_area*)areas->get(err, i);
        MX_ERROR_CHECK(err);

        reposition_sheet_area(
            err,
            a,
            left,
            right,
            top,
            width - (left + right),
            y_pos);
        MX_ERROR_CHECK(err);
    }
abort:;
}

void mx_wp_document::reposition_sheet_area(
    int& err,
    mx_area* a,
    float left,
    float right,
    float top,
    float width,
    float& y_pos)
{
    mx_point origin;
    float area_width;

    origin.y = y_pos;

    // if this is a text area, the positioning is not related to the
    // paragraph style
    if (a->rtti_class() == mx_text_area_class_e) {
        //  put on left margin
        origin.x = left;
    } else {
        // do best we can in the constraints of the page width.
        // if the area is wider than the available width, place on
        // left margin + left indent
        area_width = a->get_width(err);
        MX_ERROR_CHECK(err);

        origin.x = get_area_x_pos(a, area_width, width, left, right);

        // add in space before to the y position
        origin.y += a->get_paragraph_style()->space_before;

        y_pos += a->get_paragraph_style()->space_before + a->get_paragraph_style()->space_after;
    }

    a->set_origin(err, origin);
    MX_ERROR_CHECK(err);

    y_pos += a->get_height(err);
    MX_ERROR_CHECK(err);

abort:;
}

float mx_wp_document::get_area_x_pos(
    mx_area* a,
    float area_width,
    float width,
    float left,
    float right)
{
    mx_paragraph_style* s = a->get_paragraph_style();

    switch (s->justification) {
    case mx_justified:
    case mx_left_aligned:
        return left + s->get_ruler()->left_indent;
    case mx_right_aligned:
        return width - area_width + left - s->get_ruler()->right_indent;
    case mx_centred:
        return left + ((width - area_width) / 2);
    }
    return left;
}

void mx_wp_document::repaginate(int& err, int start_sheet)
{
    int i, ns, num_new, num_surplus;
    mx_sheet* s;
    float w, h, left, right, top, bottom;

    if (start_sheet > 0) {
        start_sheet--;
    }

    mx_wp_repaginator lengths(err, this, start_sheet);
    MX_ERROR_CHECK(err);

    lengths.find_sheet_breaks(err);
    MX_ERROR_CHECK(err);

    // make sure the document has enough sheets
    ns = get_num_sheets(err);
    MX_ERROR_CHECK(err);

    if (lengths.get_num_required_sheets() > (ns - start_sheet)) {
        num_new = lengths.get_num_required_sheets() - (ns - start_sheet);

        for (i = 0; i < num_new; i++) {
            s = sheet(err, 0);
            MX_ERROR_CHECK(err);

            s->get_margins(err, left, right, top, bottom);
            MX_ERROR_CHECK(err);

            w = s->get_width(err);
            MX_ERROR_CHECK(err);

            h = s->get_height(err);
            MX_ERROR_CHECK(err);

            s = add_sheet(err);
            MX_ERROR_CHECK(err);

            s->set_margins(err, left, right, top, bottom);
            MX_ERROR_CHECK(err);

            s->set_width(err, w);
            MX_ERROR_CHECK(err);

            s->set_height(err, h);
            MX_ERROR_CHECK(err);
        }
    }

    // move the things to their correct sheet
    lengths.move_things(err);
    MX_ERROR_CHECK(err);

    // and patch them back together
    lengths.rejoin_things(err);
    MX_ERROR_CHECK(err);

    num_surplus = ns - (lengths.get_num_required_sheets() + start_sheet);

    // delete any surplus sheets
    for (i = 0; i < num_surplus; i++) {
        delete_sheet_index(err, ns - i - 1);
        MX_ERROR_CHECK(err);
    }
abort:;
}

void mx_wp_document::reformat_sheet(
    int& err,
    int sn,
    mx_wp_sheet_pos& start_pos,
    mx_wp_sheet_pos& end_pos,
    bool& height_changed)
{
    // iterate through the areas in the sheet from the start to end position
    // reformatting them as we go
    int x;
    bool b;
    mx_sheet* s;
    mx_area* a;
    mx_list* areas;
    float width;

    mx_area_pos *sap, *eap, *tap;

    height_changed = FALSE;

    s = sheet(err, sn);
    MX_ERROR_CHECK(err);

    width = s->get_width_minus_margins(err);
    MX_ERROR_CHECK(err);

    areas = s->get_area_index(err);
    MX_ERROR_CHECK(err);

    x = start_pos.get_area_index();

    sap = start_pos.get_area_pos(err);
    MX_ERROR_CHECK(err);

    eap = end_pos.get_area_pos(err);
    MX_ERROR_CHECK(err);

    mx_paragraph::set_page_number(sn + 1);

    while (TRUE) {
        a = (mx_area*)areas->get(err, x);
        MX_ERROR_CHECK(err);

        if (x == start_pos.get_area_index()) {
            if (x == end_pos.get_area_index()) {
                reformat_area(err, sap, eap, a, width, b);
                MX_ERROR_CHECK(err);
            } else {
                tap = start_pos.get_area_pos(err)->duplicate();
                MX_ERROR_CHECK(err);

                tap->moveto_end(err, a);
                MX_ERROR_CHECK(err);

                reformat_area(err, sap, tap, a, width, b);
                MX_ERROR_CHECK(err);

                delete tap;
            }
        } else {
            if (x == end_pos.get_area_index()) {
                tap = end_pos.get_area_pos(err)->duplicate();
                MX_ERROR_CHECK(err);

                tap->moveto_start(err, a);
                MX_ERROR_CHECK(err);

                reformat_area(err, tap, eap, a, width, b);
                MX_ERROR_CHECK(err);

                delete tap;
            } else {
                // reformat the whole area
                if (a->rtti_class() == mx_text_area_class_e) {
                    a->set_width(err, width);
                    MX_ERROR_CHECK(err);
                }

                a->reformat(err, b);
                MX_ERROR_CHECK(err);
            }
        }
        if (b) {
            height_changed = TRUE;
        }

        if (x == end_pos.get_area_index()) {
            break;
        } else {
            x++;
        }
    }

    // always reposition areas for now - the current test does not allow for
    // the case where a whole area may have been removed, and so a more
    // sopisticated test of height changed must be used, but there's no
    // point if it is quick enough to always do it
    // if (height_changed)
    {
        reposition_sheet_areas(err, sn);
        MX_ERROR_CHECK(err);
    }

abort:;
}

void mx_wp_document::reformat_area(
    int& err,
    mx_area_pos* start_pos,
    mx_area_pos* end_pos,
    mx_area* a,
    float width,
    bool& height_changed)
{
    height_changed = FALSE;

    switch (a->rtti_class()) {
    case mx_text_area_class_e:
        mx_text_area_pos* sp;
        mx_text_area_pos* ep;

        sp = (mx_text_area_pos*)start_pos;
        ep = (mx_text_area_pos*)end_pos;

        a->set_width(err, width);
        MX_ERROR_CHECK(err);

        ((mx_text_area*)a)->reformat(err, *sp, *ep, height_changed);
        MX_ERROR_CHECK(err);

        break;
    case mx_table_area_class_e:
        mx_table_area_pos* tsp;
        mx_table_area_pos* tep;

        tsp = (mx_table_area_pos*)start_pos;
        tep = (mx_table_area_pos*)end_pos;

        ((mx_table_area*)a)->reformat(err, *tsp, *tep, height_changed);
        MX_ERROR_CHECK(err);

        break;
    default:
        // don't know about the contents, so reformat the whole thing.
        // have to do this as a global style might have changed affecting the
        // border (for example)
        a->reformat(err, height_changed);
        MX_ERROR_CHECK(err);
    }
abort:;
}

void mx_wp_document::reformat_sheet(int& err, int sn, bool& height_changed)
{
    mx_wp_sheet_pos start;
    mx_wp_sheet_pos end;

    mx_sheet* s;

    s = sheet(err, sn);
    MX_ERROR_CHECK(err);

    start.moveto_start(err, s);
    MX_ERROR_CHECK(err);

    end.moveto_end(err, s);
    MX_ERROR_CHECK(err);

    reformat_sheet(err, sn, start, end, height_changed);
    MX_ERROR_CHECK(err);
abort:;
}

mx_cw_t* mx_wp_document::delete_to_left(int& err, mx_wp_doc_pos* p, bool& deleted_paragraph)
{
    mx_area* a;
    mx_area_pos *ap, *prev_ap;

    mx_wp_sheet_pos* sp;
    mx_wp_doc_pos temp_pos = *p;

    mx_cw_t* res = NULL;

    a = p->get_area(err, this);
    MX_ERROR_CHECK(err);

    ap = p->get_area_pos(err);
    MX_ERROR_CHECK(err);

    if (ap->is_start()) {
        // is this a table area?
        if (a->rtti_class() == mx_table_area_class_e) {
            MX_ERROR_THROW(err, MX_POSITION_RANGE_ERROR);
        }

        // are we at the start of a sheet?
        (void)sheet(err, p->get_sheet());
        MX_ERROR_CHECK(err);

        sp = p->get_sheet_pos();
        if (sp->is_start()) {
            if (p->get_sheet() == 0) {
                MX_ERROR_THROW(err, MX_POSITION_RANGE_ERROR);
            } else {
                // is the previous area a table?
                temp_pos.prev_area(err, this);
                MX_ERROR_CHECK(err);

                prev_ap = temp_pos.get_area_pos(err);
                MX_ERROR_CHECK(err);

                if (prev_ap->rtti_class() == mx_table_area_class_e) {
                    MX_ERROR_THROW(err, MX_POSITION_RANGE_ERROR);
                } else {
                    // join the two sheets together and do a normal delete
                    join_sheets(err, temp_pos.get_sheet(), temp_pos.get_sheet() + 1);
                    MX_ERROR_CHECK(err);

                    *p = temp_pos;

                    p->right(err, this);
                    MX_ERROR_CHECK(err);

                    a = p->get_area(err, this);
                    MX_ERROR_CHECK(err);

                    ap = p->get_area_pos(err);
                    MX_ERROR_CHECK(err);
                }
            }
        } else {
            MX_ERROR_THROW(err, MX_POSITION_RANGE_ERROR);
        }
    }

    switch (a->rtti_class()) {
    case mx_text_area_class_e:
        res = ((mx_text_area*)a)->delete_to_left(err, (mx_text_area_pos*)ap, deleted_paragraph);
        MX_ERROR_CHECK(err);
        break;
    case mx_table_area_class_e:
        res = ((mx_table_area*)a)->delete_to_left(err, (mx_table_area_pos*)ap, deleted_paragraph);
        MX_ERROR_CHECK(err);
        break;
    default:
        MX_ERROR_THROW(err, MX_POSITION_RANGE_ERROR);
        break;
    }

abort:
    return res;
}

mx_paragraph_style* mx_wp_document::get_para_style(int& err, mx_wp_doc_pos* p)
{
    mx_area* a;
    mx_area_pos* ap;
    mx_paragraph_style* res;

    a = p->get_area(err, this);
    MX_ERROR_CHECK(err);

    ap = p->get_area_pos(err);
    MX_ERROR_CHECK(err);

    res = a->get_style(err, ap);
    MX_ERROR_CHECK(err);

    return res;
abort:
    return NULL;
}

void mx_wp_document::insert_char(int& err, char c, mx_wp_doc_pos& p)
{
    mx_area* a;
    mx_area_pos* ap;

    a = p.get_area(err, this);
    MX_ERROR_CHECK(err);

    ap = p.get_area_pos(err);
    MX_ERROR_CHECK(err);

    switch (a->rtti_class()) {
    case mx_text_area_class_e:
        ((mx_text_area*)a)->insert_char(err, c, (mx_text_area_pos*)ap);
        MX_ERROR_CHECK(err);
        break;
    case mx_table_area_class_e:
        ((mx_table_area*)a)->insert_char(err, c, (mx_table_area_pos*)ap);
        MX_ERROR_CHECK(err);
        break;
    default:
        MX_ERROR_THROW(err, MX_POSITION_RANGE_ERROR);
        break;
    }

abort:;
}

void mx_wp_document::insert_cw_item(int& err, mx_cw_t* item, mx_wp_doc_pos& p)
{
    mx_area* a;
    mx_area_pos* ap;

    a = p.get_area(err, this);
    MX_ERROR_CHECK(err);

    ap = p.get_area_pos(err);
    MX_ERROR_CHECK(err);

    switch (a->rtti_class()) {
    case mx_text_area_class_e:
        ((mx_text_area*)a)->insert_cw_item(err, item, (mx_text_area_pos*)ap);
        MX_ERROR_CHECK(err);
        break;
    case mx_table_area_class_e:
        ((mx_table_area*)a)->insert_cw_item(err, item, (mx_table_area_pos*)ap);
        MX_ERROR_CHECK(err);
        break;
    default:
        MX_ERROR_THROW(err, MX_POSITION_RANGE_ERROR);
        break;
    }

abort:;
}

void mx_wp_document::insert_line_break(int& err, mx_wp_doc_pos& p)
{
    mx_area* a;
    mx_area_pos* ap;

    a = p.get_area(err, this);
    MX_ERROR_CHECK(err);

    ap = p.get_area_pos(err);
    MX_ERROR_CHECK(err);

    switch (a->rtti_class()) {
    case mx_text_area_class_e:
        ((mx_text_area*)a)->insert_line_break(err, (mx_text_area_pos*)ap);
        MX_ERROR_CHECK(err);
        break;
    case mx_table_area_class_e:
        ((mx_table_area*)a)->insert_line_break(err, (mx_table_area_pos*)ap);
        MX_ERROR_CHECK(err);
        break;
    default:
        break;
    }

abort:;
}

void mx_wp_document::insert_para_break(int& err, mx_wp_doc_pos& p)
{
    mx_area* a;
    mx_area_pos* ap;

    a = p.get_area(err, this);
    MX_ERROR_CHECK(err);

    ap = p.get_area_pos(err);
    MX_ERROR_CHECK(err);

    switch (a->rtti_class()) {
    case mx_text_area_class_e:
        ((mx_text_area*)a)->insert_para_break(err, (mx_text_area_pos*)ap);
        MX_ERROR_CHECK(err);
        break;
    case mx_table_area_class_e:
        ((mx_table_area*)a)->insert_para_break(err, (mx_table_area_pos*)ap);
        MX_ERROR_CHECK(err);
        break;
    default:
        MX_ERROR_THROW(err, MX_POSITION_RANGE_ERROR);
        break;
    }

abort:;
}

void mx_wp_document::join_para_with_next(int& err, mx_wp_doc_pos& p)
{
    mx_text_area *txta, *next_txta;
    mx_text_area_pos *txtp, *next_txtp;
    mx_paragraph *para, *next_para;
    mx_wp_doc_pos pos = p, next_pos = p;
    mx_sheet* next_s;
    int num_paras;

    txta = (mx_text_area*)pos.get_area(err, this);
    MX_ERROR_CHECK(err);

    txtp = (mx_text_area_pos*)pos.get_area_pos(err);
    MX_ERROR_CHECK(err);

    if (txta->is_a(mx_table_area_class_e)) {
        // if this is a table area then delegate to the table area

        mx_table_area* taba = (mx_table_area*)txta;
        mx_table_area_pos* tabp = (mx_table_area_pos*)txtp;
        MX_ERROR_ASSERT(err, tabp->is_a(mx_table_area_pos_class_e));

        taba->join_para_with_next(err, *tabp);
        MX_ERROR_CHECK(err);
        return;
    }

    next_pos.next_para(err, this);
    MX_ERROR_CHECK(err);

    next_txta = (mx_text_area*)next_pos.get_area(err, this);
    MX_ERROR_CHECK(err);

    next_txtp = (mx_text_area_pos*)next_pos.get_area_pos(err);
    MX_ERROR_CHECK(err);

    if (txta->rtti_class() != next_txta->rtti_class()) {
        MX_ERROR_THROW(err, MX_ERROR_UNDEFINED);
    }

    // both positions should be in text areas

    MX_ERROR_ASSERT(err, txta->is_a(mx_text_area_class_e) && next_txta->is_a(mx_text_area_class_e) && txtp->is_a(mx_text_area_pos_class_e) && next_txtp->is_a(mx_text_area_pos_class_e));

    next_para = next_txta->remove(err, next_txtp->paragraph_index);
    MX_ERROR_CHECK(err);

    para = txta->remove(err, txtp->paragraph_index);
    MX_ERROR_CHECK(err);

    para->join(err, next_para);
    MX_ERROR_CHECK(err);

    txta->insert(err, txtp->paragraph_index, para);
    MX_ERROR_CHECK(err);

    // now remove the next area and sheet if they are empty
    next_s = this->sheet(err, next_pos.get_sheet());
    MX_ERROR_CHECK(err);

    num_paras = next_txta->num_paragraphs(err);
    MX_ERROR_CHECK(err);

    if (num_paras < 1) {
        next_s->remove_area(err, next_pos.get_sheet_pos()->get_area_index());
        delete next_txta;
    }

    if (next_s->get_num_areas() < 1) {
        this->delete_sheet_index(err, next_pos.get_sheet());
        MX_ERROR_CHECK(err);
    }

abort:;
}

void mx_wp_document::join_table_with_next(int& err, mx_wp_doc_pos& p)
{
    mx_sheet* next_sheet;
    mx_table_area *taba, *next_taba;
    mx_wp_doc_pos pos = p, next_pos = p;

    taba = (mx_table_area*)pos.get_area(err, this);
    MX_ERROR_CHECK(err);
    MX_ERROR_ASSERT(err, taba->is_a(mx_table_area_class_e));

    next_pos.next_area(err, this);
    MX_ERROR_CHECK(err);

    next_taba = (mx_table_area*)next_pos.get_area(err, this);
    MX_ERROR_CHECK(err);
    MX_ERROR_ASSERT(err, next_taba->is_a(mx_table_area_class_e));

    next_sheet = this->sheet(err, next_pos.get_sheet());
    MX_ERROR_CHECK(err);

    next_taba = (mx_table_area*)next_sheet->remove_area(
        err, next_pos.get_sheet_pos()->get_area_index());
    MX_ERROR_CHECK(err);
    MX_ERROR_ASSERT(err, next_taba->is_a(mx_table_area_class_e));

    taba->join(err, next_taba);
    MX_ERROR_CHECK(err);

    if (next_sheet->get_num_areas() < 1) {
        this->delete_sheet_ref(err, next_sheet);
        MX_ERROR_CHECK(err);
    }

abort:;
}

void mx_wp_document::rejoin_split_text_container(int& err, mx_wp_doc_pos& p)
{
    // rejoin a split table or paragraph by pulling previous/following
    // text_containers into this one

    bool b = p.is_table(err);
    MX_ERROR_CHECK(err);

    if (b) {
        this->rejoin_split_table(err, p);
        MX_ERROR_CHECK(err);
    } else {
        this->rejoin_split_paragraph(err, p);
        MX_ERROR_CHECK(err);
    }
abort:;
}

void mx_wp_document::rejoin_split_paragraph(int& err, mx_wp_doc_pos& p)
{
    mx_wp_doc_pos pos = p;
    mx_paragraph* para = pos.get_para(err, this);
    MX_ERROR_CHECK(err);

    while (para->get_type() != mx_paragraph::mx_paragraph_whole_e && para->get_type() != mx_paragraph::mx_paragraph_start_e) {
        pos.prev_para(err, this);
        MX_ERROR_CHECK(err);

        para = pos.get_para(err, this);
        MX_ERROR_CHECK(err);
    }

    while (para->get_type() == mx_paragraph::mx_paragraph_start_e) {
        this->join_para_with_next(err, pos);
        MX_ERROR_CHECK(err);

        para = pos.get_para(err, this);
        MX_ERROR_CHECK(err);
    }

    MX_ERROR_ASSERT(err, para->get_type() == mx_paragraph::mx_paragraph_whole_e);
abort:;
}

void mx_wp_document::rejoin_split_table(int& err, mx_wp_doc_pos& p)
{
    mx_wp_doc_pos pos = p;
    mx_table_area::mx_table_type_t type;
    mx_table_area* taba = (mx_table_area*)pos.get_area(err, this);
    MX_ERROR_CHECK(err);
    MX_ERROR_ASSERT(err, taba->is_a(mx_table_area_class_e));

    type = taba->get_type(err);
    MX_ERROR_CHECK(err);

    // rewind to the start of the logical table

    while (type != mx_table_area::mx_table_whole_e && type != mx_table_area::mx_table_start_e) {
        pos.prev_area(err, this);
        MX_ERROR_CHECK(err);

        taba = (mx_table_area*)pos.get_area(err, this);
        MX_ERROR_CHECK(err);
        MX_ERROR_ASSERT(err, taba->is_a(mx_table_area_class_e));

        type = taba->get_type(err);
        MX_ERROR_CHECK(err);
    }

    // now join the table areas until the type is whole

    while (type == mx_table_area::mx_table_start_e) {
        this->join_table_with_next(err, pos);
        MX_ERROR_CHECK(err);

        taba = (mx_table_area*)pos.get_area(err, this);
        MX_ERROR_CHECK(err);
        MX_ERROR_ASSERT(err, taba->is_a(mx_table_area_class_e));

        type = taba->get_type(err);
        MX_ERROR_CHECK(err);
    }
    MX_ERROR_ASSERT(err, type == mx_table_area::mx_table_whole_e);
abort:;
}

void mx_wp_document::reformat_from_start_of_sheet(int& err, mx_wp_doc_pos& start, mx_wp_doc_pos& end)
{
    mx_wp_doc_pos sheet_start = start;

    sheet_start.moveto_start_sheet(err, this, sheet_start.get_sheet());
    MX_ERROR_CHECK(err);

    reformat(err, sheet_start, end);
    MX_ERROR_CHECK(err);
abort:;
}

void mx_wp_document::remove_area(int& err, mx_wp_doc_pos* p)
{
    mx_sheet* s;
    mx_area* a;
    mx_area *prev, *next;
    int i, na;
    bool b;

    s = sheet(err, p->get_sheet());
    MX_ERROR_CHECK(err);

    na = s->get_num_areas();

    if (na == 1) {
        delete_sheet_index(err, p->get_sheet());
        MX_ERROR_CHECK(err);

        return;
    }

    i = p->get_sheet_pos()->get_area_index();

    a = s->remove_area(err, i);
    MX_ERROR_CHECK(err);

    delete a;

    if (i != 0 && i != (na - 1)) {
        prev = s->get_area(err, i - 1);
        MX_ERROR_CHECK(err);

        next = s->get_area(err, i);
        MX_ERROR_CHECK(err);

        b = prev->can_join(err, next);
        MX_ERROR_CHECK(err);

        if (b) {
            s->remove_area(err, i);
            MX_ERROR_CHECK(err);

            prev->join(err, next);
            MX_ERROR_CHECK(err);
        }
    }
abort:;
}

int mx_wp_document::split(int& err, mx_wp_doc_pos& p)
{
    mx_sheet* s;
    mx_area* a;
    mx_text_area* ta;
    bool b;

    mx_area_pos* ap;
    mx_text_area_pos* tap;
    mx_wp_sheet_pos* sp;

    s = sheet(err, p.get_sheet());
    MX_ERROR_CHECK(err);

    a = p.get_sheet_pos()->area(err, s);
    MX_ERROR_CHECK(err);

    sp = p.get_sheet_pos();

    ap = sp->get_area_pos(err);
    MX_ERROR_CHECK(err);

    if (ap->rtti_class() == mx_table_area_pos_class_e) {
        mx_table_area* taba = (mx_table_area*)a;
        mx_table_area_pos* tabp = (mx_table_area_pos*)ap;

        MX_ERROR_ASSERT(err, taba->is_a(mx_table_area_class_e));

        if (tabp->row == 0)
            return sp->get_area_index();

        taba->normal_split(err, *tabp);
        MX_ERROR_CHECK(err);

        return sp->get_area_index() + 1;
    } else {
        if (ap->rtti_class() != mx_text_area_pos_class_e) {
            b = ap->is_end(err, a);
            MX_ERROR_CHECK(err);

            return b ? sp->get_area_index() + 1 : sp->get_area_index();
        } else {
            ta = (mx_text_area*)a;
            tap = (mx_text_area_pos*)ap;

            // try to split the text area at the current position - we will get
            // an error if the position is invalid

            ta->split(err, *tap);
            if (err != MX_TEXT_AREA_INVALID_SPLIT_POS) {
                MX_ERROR_CHECK(err);
                return sp->get_area_index() + 1;
            } else {
                MX_ERROR_CLEAR(err);

                b = !tap->is_start() && tap->is_end(err, ta);
                MX_ERROR_CHECK(err);

                return b ? sp->get_area_index() + 1 : sp->get_area_index();
            }
        }
    }
abort:
    return -1;
}

void mx_wp_document::set_highlight_range(mx_wp_doc_pos& start, mx_wp_doc_pos& end)
{
    if (start < end) {
        ((mx_document_painter*)painter)->set_highlight_range(start, end);
    } else {
        ((mx_document_painter*)painter)->set_highlight_range(end, start);
    }
}

void mx_wp_document::clear_highlight()
{
    ((mx_document_painter*)painter)->clear_highlight();
}

void mx_wp_document::set_page_size(int& err, float width, float height,
    bool is_landscape)
{
    int i, n;
    mx_sheet* s;

    n = get_num_sheets(err);
    MX_ERROR_CHECK(err);

    for (i = 0; i < n; i++) {
        s = sheet(err, i);
        MX_ERROR_CHECK(err);

        s->set_landscape(err, is_landscape);
        MX_ERROR_CHECK(err);

        s->set_width(err, width);
        MX_ERROR_CHECK(err);

        s->set_height(err, height);
        MX_ERROR_CHECK(err);
    }

abort:;
}

void mx_wp_document::set_page_margins(int& err, float left, float right, float top,
    float bottom)
{
    int i, n;
    mx_sheet* s;

    n = get_num_sheets(err);
    MX_ERROR_CHECK(err);

    for (i = 0; i < n; i++) {
        s = sheet(err, i);
        MX_ERROR_CHECK(err);

        s->set_margins(err, left, right, top, bottom);
        MX_ERROR_CHECK(err);
    }

abort:;
}

void mx_wp_document::clear_header(int& err)
{
    mx_text_area* a;
    mx_sheet* s;

    a = header(err);
    MX_ERROR_CHECK(err);

    if (a != NULL) {
        s = get_header_footer_sheet(err);
        MX_ERROR_CHECK(err);

        a = (mx_text_area*)s->remove_area(err, 0);
        MX_ERROR_CHECK(err);

        delete a;

        s->add_text_area(err, 0);
        MX_ERROR_CHECK(err);
    }

    set_is_footer(err, FALSE);
    MX_ERROR_CHECK(err);

abort:;
}

void mx_wp_document::clear_footer(int& err)
{
    mx_text_area* a;
    mx_sheet* s;

    a = footer(err);
    MX_ERROR_CHECK(err);

    if (a != NULL) {
        s = get_header_footer_sheet(err);
        MX_ERROR_CHECK(err);

        a = (mx_text_area*)s->remove_area(err, 1);
        MX_ERROR_CHECK(err);

        delete a;

        s->add_text_area(err, 1);
        MX_ERROR_CHECK(err);
    }

    set_is_footer(err, FALSE);
    MX_ERROR_CHECK(err);

abort:;
}

void mx_wp_document::set_header(int& err, mx_text_area* a)
{
    mx_text_area* ta;
    mx_sheet* s;

    s = get_header_footer_sheet(err);
    MX_ERROR_CHECK(err);

    ta = (mx_text_area*)s->remove_area(err, 0);
    MX_ERROR_CHECK(err);

    delete ta;

    a->move_to_document(err, this);
    MX_ERROR_CHECK(err);

    s->add_area(err, a, 0);
    MX_ERROR_CHECK(err);

    set_is_header(err, TRUE);
    MX_ERROR_CHECK(err);

abort:;
}

void mx_wp_document::set_footer(int& err, mx_text_area* a)
{
    mx_text_area* ta;
    mx_sheet* s;

    s = get_header_footer_sheet(err);
    MX_ERROR_CHECK(err);

    ta = (mx_text_area*)s->remove_area(err, 1);
    MX_ERROR_CHECK(err);

    delete ta;

    a->move_to_document(err, this);
    MX_ERROR_CHECK(err);

    s->add_area(err, a, 1);
    MX_ERROR_CHECK(err);

    set_is_footer(err, TRUE);
    MX_ERROR_CHECK(err);

abort:;
}

mx_sheet* mx_wp_document::get_header_footer_sheet(int& err)
{
    mx_sheet *t, *s;
    uint32 oid;
    float width, height;

    err = MX_ERROR_OK;

    s = sheet(err, 0);
    MX_ERROR_CHECK(err);

    width = s->get_width(err);
    MX_ERROR_CHECK(err);

    height = s->get_height(err);
    MX_ERROR_CHECK(err);

    oid = get_default_int(err, MX_WP_DOCUMENT_HF_SHEET, -1);
    MX_ERROR_CHECK(err);

    if ((int)oid == -1) {
        t = new mx_sheet(doc_id);

        if (t->error != MX_ERROR_OK) {
            MX_ERROR_THROW(err, t->error);
        }

        t->set_owner_object(err, this);
        MX_ERROR_CHECK(err);

        t->set_width(err, width);
        MX_ERROR_CHECK(err);

        t->set_height(err, height);
        MX_ERROR_CHECK(err);

        oid = t->get_db_id(err);
        MX_ERROR_CHECK(err);

        set_int(err, MX_WP_DOCUMENT_HF_SHEET, (int)oid);
        MX_ERROR_CHECK(err);

        // add a couple of empty text areas - first for header, second for
        // footer
        (void)t->add_text_area(err, 0);
        MX_ERROR_CHECK(err);

        (void)t->add_text_area(err, 0);
        MX_ERROR_CHECK(err);

        return t;
    } else {
        t = (mx_sheet*)mx_db_cc_get_obj_from_oid(err, doc_id, (uint32)oid);
        if (err == MX_DB_CLIENT_CACHE_NO_SUCH_OID) {
            // add it to the cache
            MX_ERROR_CLEAR(err);

            t = new mx_sheet(doc_id, oid);

            if (t->error != MX_ERROR_OK) {
                err = t->error;
                return NULL;
            }

            t->set_owner_object(err, this);
            MX_ERROR_CHECK(err);

            return t;
        } else {
            MX_ERROR_CHECK(err);
            return t;
        }
    }
abort:
    return NULL;
}

bool mx_wp_document::is_header(int& err)
{
    int res;

    res = get_default_int(err, MX_WP_DOCUMENT_IS_HEADER, 0);
    MX_ERROR_CHECK(err);

    return res == 1;
abort:
    return FALSE;
}

bool mx_wp_document::is_footer(int& err)
{
    int res;

    res = get_default_int(err, MX_WP_DOCUMENT_IS_FOOTER, 0);
    MX_ERROR_CHECK(err);

    return res == 1;
abort:
    return FALSE;
}

void mx_wp_document::set_is_header(int& err, bool b)
{
    set_int(err, MX_WP_DOCUMENT_IS_HEADER, b ? 1 : 0);
    MX_ERROR_CHECK(err);
abort:;
}

void mx_wp_document::set_is_footer(int& err, bool b)
{
    set_int(err, MX_WP_DOCUMENT_IS_FOOTER, b ? 1 : 0);
    MX_ERROR_CHECK(err);
abort:;
}

void mx_wp_document::join_sheets(int& err, int sn1, int sn2)
{
    mx_sheet *s1, *s2;
    mx_area *a1, *a2;
    bool b;

    s1 = sheet(err, sn1);
    MX_ERROR_CHECK(err);

    s2 = sheet(err, sn2);
    MX_ERROR_CHECK(err);

    // check if the last area from the first sheet and the
    // first area from the second sheet are both text areas and
    // can be joined
    a1 = s1->get_bottom_area(err);
    MX_ERROR_CHECK(err);

    a2 = s2->get_top_area(err);
    MX_ERROR_CHECK(err);

    b = a1->can_join(err, a2);
    MX_ERROR_CHECK(err);

    if (b) {
        s2->remove_area(err, 0);
        MX_ERROR_CHECK(err);

        a1->join(err, a2);
        MX_ERROR_CHECK(err);
    }

    while (s2->get_num_areas() > 0) {
        s1->move_area_in(err, 0, s2);
        MX_ERROR_CHECK(err);
    }

    reposition_sheet_areas(err, sn1);
    MX_ERROR_CHECK(err);

    delete_sheet_index(err, sn2);
    MX_ERROR_CHECK(err);

abort:;
}

void mx_wp_document::insert_next_para(
    int& err,
    mx_wp_doc_pos& pos,
    mx_paragraph* p)
{
    mx_area_pos* ap;
    mx_text_area* ta;

    mx_table_area* table;
    mx_table_area_pos* table_pos;

    int para_index;

    ap = pos.get_area_pos(err);
    MX_ERROR_CHECK(err);

    if (ap->rtti_class() == mx_text_area_pos_class_e) {
        ta = (mx_text_area*)(pos.get_area(err, this));
        MX_ERROR_CHECK(err);

        para_index = ((mx_text_area_pos*)ap)->paragraph_index;
    } else {
        if (ap->rtti_class() == mx_table_area_pos_class_e) {
            table_pos = (mx_table_area_pos*)ap;

            table = (mx_table_area*)(pos.get_area(err, this));
            MX_ERROR_CHECK(err);

            ta = table->get_cell(err, table_pos->row, table_pos->column);
            MX_ERROR_CHECK(err);

            para_index = table_pos->text_pos.paragraph_index;
        } else {
            MX_ERROR_THROW(err, MX_POSITION_RANGE_ERROR);
        }
    }

    ta->insert(err, para_index, p);
    MX_ERROR_CHECK(err);

abort:;
}

mx_char_style* mx_wp_document::get_char_style(int& err,
    const mx_wp_doc_pos& p)
{
    mx_wp_doc_pos position = p;
    mx_area_pos* ap;
    mx_area* area;
    mx_char_style* cs = NULL;

    area = position.get_area(err, this);
    MX_ERROR_CHECK(err);

    ap = position.get_area_pos(err);
    MX_ERROR_CHECK(err);

    cs = ap->get_char_style(err, area);
    MX_ERROR_CHECK(err);

abort:
    return cs;
}

void mx_wp_document::get_page_margins(int& err, float& left, float& right, float& top, float& bottom)
{
    mx_sheet* s;

    s = sheet(err, 0);
    MX_ERROR_CHECK(err);

    s->get_margins(err, left, right, top, bottom);
    MX_ERROR_CHECK(err);
abort:;
}

void mx_wp_document::get_page_size(int& err, float& width, float& height, bool& is_landscape)
{
    mx_sheet* s;

    s = sheet(err, 0);
    MX_ERROR_CHECK(err);

    width = s->get_width(err);
    MX_ERROR_CHECK(err);

    height = s->get_height(err);
    MX_ERROR_CHECK(err);

    is_landscape = s->get_landscape(err);
    MX_ERROR_CHECK(err);
abort:;
}

void mx_wp_document::insert_text(int& err, mx_wp_doc_pos& pos, const char* s)
{
    mx_paragraph* p;
    mx_para_pos* pp;

    while (*s != 0) {
        switch (*s) {
        case '\n':
            this->insert_para_break(err, pos);
            MX_ERROR_CHECK(err);

            pos.next_para(err, this);
            MX_ERROR_CHECK(err);

            if (*(++s) == '\r')
                s++;
            break;
        case '\r':
            this->insert_para_break(err, pos);
            MX_ERROR_CHECK(err);

            pos.next_para(err, this);
            MX_ERROR_CHECK(err);

            if (*(++s) == '\n')
                s++;
            break;
        default:
            p = pos.get_para(err, this);
            MX_ERROR_CHECK(err);

            pp = pos.get_para_pos(err);
            MX_ERROR_CHECK(err);

            p->insert_ascii_string(err, s, *pp);
            MX_ERROR_CHECK(err);

            // get rid of unreadable character
            if (*s != 0 && *s != '\n' && *s != '\r')
                s++;
            break;
        }
    }
abort:;
}

mx_word* mx_wp_document::get_word(int& err, const mx_wp_doc_pos& word_pos)
{
    mx_wp_doc_pos pos = word_pos;
    mx_paragraph* para;
    mx_para_pos* para_pos;
    mx_word* res = NULL;

    para = pos.get_para(err, this);
    MX_ERROR_CHECK(err);

    para_pos = pos.get_para_pos(err);
    MX_ERROR_CHECK(err);

    res = para->get_word(err, *para_pos);
    MX_ERROR_CHECK(err);

abort:
    return res;
}

static inline void find_logical_word_boundary(int& err, mx_wp_document* doc,
    mx_wp_doc_pos& start_word_dp,
    mx_wp_doc_pos& end_word_dp)
{
    start_word_dp.moveto_start_logical_word(err, doc);
    MX_ERROR_CHECK(err);

    end_word_dp.moveto_end_logical_word(err, doc);
    MX_ERROR_CHECK(err);
abort:;
}

static void move_to_start_and_end_of_word(int& err, mx_wp_document* doc,
    mx_wp_doc_pos& s, mx_wp_doc_pos& e)
{
    s.moveto_start_word(err, doc);
    MX_ERROR_CHECK(err);

    e.moveto_end_word(err, doc);
    MX_ERROR_CHECK(err);
abort:;
}

mx_word* mx_wp_document::get_logical_word(int& err,
    const mx_wp_doc_pos& pos,
    mx_wp_doc_pos& start_word_dp,
    mx_wp_doc_pos& end_word_dp)
{
    mx_word *res = NULL, *word;
    static mx_complex_word* cw_result = NULL;

    start_word_dp = end_word_dp = pos;

    if (cw_result != NULL) {
        delete cw_result;
        cw_result = NULL;
    }

    word = get_word(err, pos);
    MX_ERROR_CHECK(err);

    if (!word->can_be_part_of_long_word()) {
        move_to_start_and_end_of_word(err, this, start_word_dp, end_word_dp);
        MX_ERROR_CHECK(err);

        res = word;
    } else {
        mx_wp_doc_pos start_of_end_word_dp;

        find_logical_word_boundary(err, this, start_word_dp, end_word_dp);
        MX_ERROR_CHECK(err);

        start_of_end_word_dp = end_word_dp;
        start_of_end_word_dp.moveto_start_word(err, this);
        MX_ERROR_CHECK(err);

        if (start_word_dp == end_word_dp) {
            res = word;
        } else {
            // build up a complex word out of the words in the range
            mx_wp_doc_pos p = start_word_dp;
            bool finished = FALSE;

            cw_result = new mx_complex_word;

            do {
                word = this->get_word(err, p);
                MX_ERROR_CHECK(err);

                switch (word->rtti_class()) {
                case mx_simple_word_class_e:
                    *cw_result += *((mx_simple_word*)word);
                    break;
                case mx_complex_word_class_e:
                    *cw_result += *((mx_complex_word*)word);
                    break;
                default:
                    break;
                }
                finished = (p >= start_of_end_word_dp);

                if (!finished) {
                    p.next_word(err, this);
                    MX_ERROR_CHECK(err);
                }
            } while (!finished);

            res = cw_result;
        }
    }
abort:
    return res;
}

mx_word* mx_wp_document ::get_logical_word_with_limits(int& err,
    const mx_wp_doc_pos& pos,
    mx_wp_doc_pos& start_limit,
    mx_wp_doc_pos& end_limit)
{
    mx_word *word = NULL, *cut_word = NULL;
    mx_wp_doc_pos start_word_dp = start_limit;
    mx_wp_doc_pos end_word_dp = end_limit;
    mx_wp_doc_pos temp_end_limit_dp = end_limit;
    int32 items_to_cut_from_start = 0, items_to_cut_from_end = 0;

    word = this->get_logical_word(err, pos, start_word_dp, end_word_dp);
    MX_ERROR_CHECK(err);

    while (start_word_dp < start_limit) {
        start_word_dp.move_right_ignoring_false_breaks(err, this);
        MX_ERROR_CHECK(err);

        items_to_cut_from_start++;
    }

    while (temp_end_limit_dp < end_word_dp) {
        temp_end_limit_dp.move_right_ignoring_false_breaks(err, this);
        MX_ERROR_CHECK(err);

        items_to_cut_from_end++;
    }

    if (items_to_cut_from_end > 0 || items_to_cut_from_start > 0) {
        static mx_word* static_word = NULL;

        if (static_word != NULL)
            delete static_word;
        static_word = word->duplicate();
        word = static_word;

        mx_word::set_operations_to_right_of_mods();

        if (items_to_cut_from_end > 0) {
            int32 word_length = word->get_num_visible_items();
            int32 start_cut_index = word_length - items_to_cut_from_end;

            MX_ERROR_ASSERT(err, start_cut_index >= 0);

            cut_word = word->cut(err, start_cut_index, word_length);
            MX_ERROR_CHECK(err);

            delete cut_word;
        }

        if (items_to_cut_from_start > 0) {
            int32 word_length = word->get_num_visible_items();

            MX_ERROR_ASSERT(err, items_to_cut_from_start <= word_length);

            cut_word = word->cut(err, 0, items_to_cut_from_start);
            MX_ERROR_CHECK(err);

            delete cut_word;
        }
    }

    // set the limit parameters to indicate the word start/end positions
    if (start_word_dp > start_limit)
        start_limit = start_word_dp;
    if (end_word_dp < end_limit)
        end_limit = end_word_dp;

    return word;
abort:
    return NULL;
}

void mx_wp_document::replace_style(int& err, mx_paragraph_style* style_to_replace,
    mx_paragraph_style* replacement_style)
{
    int i = 0, num_areas = 0;
    mx_sheet* s = NULL;
    mx_area* a = NULL;

    s = this->get_header_footer_sheet(err);
    MX_ERROR_CHECK(err);

    num_areas = s->get_num_areas();

    for (i = 0; i < num_areas; i++) {
        a = s->get_area(err, i);
        MX_ERROR_CHECK(err);

        a->replace_style(err, style_to_replace, replacement_style);
        MX_ERROR_CHECK(err);
    }

    // replace the styles in the rest of the document
    mx_document::replace_style(err, style_to_replace, replacement_style);
    MX_ERROR_CHECK(err);
abort:;
}
