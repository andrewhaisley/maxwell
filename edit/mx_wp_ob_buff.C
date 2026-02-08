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
 * MODULE : mx_wp_object_buffer.C
 *
 * AUTHOR : Andrew Haisley
 *
 *
 *
 * DESCRIPTION:
 *   an object that holds parts of a wp document
 *
 *
 *
 */

#include <mx_ta_buff.h>
#include <mx_list_iter.h>
#include <mx_text_buff.h>
#include <mx_wp_ob_buff.h>
#include <mx_cw_t.h>
#include <mx_paragraph.h>
#include <mx_ta_pos.h>
#include <mx_txt_pos.h>
#include <mx_wp_doc.h>
#include <mx_wp_pos.h>

mx_wp_object_buffer::mx_wp_object_buffer(mx_wp_document* d)
    : mx_buffer(d)
{
    contents = new mx_list;
    buf_base_style_name = NULL;
}

mx_wp_object_buffer::mx_wp_object_buffer(const mx_wp_object_buffer& ob)
    : mx_buffer(ob)
{
    mx_list_iterator iter(*ob.contents);

    contents = new mx_list;

    start_mod = ob.start_mod;
    buf_base_style_mod = ob.buf_base_style_mod;
    buf_base_style_name = ob.buf_base_style_name;

    buffer_contains_cells = ob.buffer_contains_cells;
    buffer_contains_text = ob.buffer_contains_text;
    buffer_contains_tables = ob.buffer_contains_tables;

    if (buf_base_style_name != NULL) {
        buf_base_style_name = mx_string_copy(buf_base_style_name);
    }

    while (iter.more()) {
        mx_rtti* o = (mx_rtti*)iter.data();
        switch (o->rtti_class()) {
        case mx_text_buffer_class_e:
            contents->append(new mx_text_buffer(*(mx_text_buffer*)o));
            break;
        case mx_table_buffer_class_e:
            contents->append(new mx_table_buffer(*(mx_table_buffer*)o));
            break;
        default:
            // must be an area
            contents->append(((mx_area*)o)->duplicate());
            break;
        }
    }

    return;
}

mx_wp_object_buffer::~mx_wp_object_buffer()
{
    mx_list_iterator iter(*contents);
    while (iter.more())
        delete (mx_rtti*)iter.data();
    delete contents;
    if (buf_base_style_name)
        delete[] buf_base_style_name;
}

static void get_style_mod(int& err, mx_wp_document* doc,
    mx_wp_doc_pos& doc_pos,
    mx_char_style_mod& cs_mod)
{
    mx_paragraph* para;
    mx_para_pos* pp;
    bool b;

    b = doc_pos.is_paragraph(err);
    MX_ERROR_CHECK(err);

    if (!b) {
        cs_mod = mx_char_style_mod();
    } else {
        mx_char_style_mod* cs_mod_p;
        para = doc_pos.get_para(err, doc);
        MX_ERROR_CHECK(err);

        pp = doc_pos.get_para_pos(err);
        MX_ERROR_CHECK(err);

        mx_word::set_operations_to_right_of_mods();
        cs_mod_p = para->aka_get_style_mod(err, *pp);
        MX_ERROR_CHECK(err);

        cs_mod = *cs_mod_p;
    }
abort:;
}

static void get_style(int& err, mx_wp_document* doc,
    mx_wp_doc_pos& doc_pos,
    mx_char_style& cs)
{
    mx_paragraph* para;
    mx_para_pos* pp;
    bool b;

    b = doc_pos.is_paragraph(err);
    MX_ERROR_CHECK(err);

    if (!b) {
        cs = mx_char_style();
    } else {
        mx_char_style* cs_p;
        para = doc_pos.get_para(err, doc);
        MX_ERROR_CHECK(err);

        pp = doc_pos.get_para_pos(err);
        MX_ERROR_CHECK(err);

        mx_word::set_operations_to_right_of_mods();
        cs_p = para->aka_get_style(err, *pp);
        MX_ERROR_CHECK(err);

        cs = *cs_p;
    }
abort:;
}

static void get_join_requirements(int& err,
    mx_wp_doc_pos& p1,
    mx_wp_doc_pos& p2,
    mx_wp_document& doc,
    bool& join_paras,
    bool& join_words)
{
    bool b2, b1 = p1.is_at_start_of_para(err, &doc);
    MX_ERROR_CHECK(err);

    if (b1) {
        join_paras = join_words = FALSE;
        return;
    }

    b1 = p1.is_same_para_as(err, &p2);
    MX_ERROR_CHECK(err);

    if (b1) {
        join_paras = FALSE;
        join_words = TRUE;
        return;
    }

    b1 = p1.is_table(err);
    MX_ERROR_CHECK(err);

    b2 = p2.is_table(err);
    MX_ERROR_CHECK(err);

    if ((b1 && !b2) || (!b1 && b2)) {
        // one point is in a table, but the other isn't, so we don't want to
        // join anything

        join_paras = join_words = FALSE;
        return;
    }

    if (b1 && b2) {
        // both positions are in tables, now are they in the same cell?

        b2 = p1.is_same_cell_as(err, &p2);
        MX_ERROR_CHECK(err);

        if (!b2) {
            join_paras = join_words = FALSE;
            return;
        }
    }

    // otherwise we are in the middle of paragraphs so we want to join
    // paragraphs and words
    join_paras = join_words = TRUE;
abort:;
}

// rejoin any split text containers at point 1 and at point 2
static void rejoin_split_text_containers(int& err,
    mx_wp_doc_pos& point1,
    mx_wp_doc_pos& point2,
    mx_wp_document* doc)
{
    bool b = point1.is_in_split_text_container(err, doc);
    MX_ERROR_CHECK(err);

    if (b) {
        point1.save(err, doc);
        MX_ERROR_CHECK(err);

        point2.save(err, doc);
        MX_ERROR_CHECK(err);

        doc->rejoin_split_text_container(err, point1);
        MX_ERROR_CHECK(err);

        point1.restore(err, doc);
        MX_ERROR_CHECK(err);

        point2.restore(err, doc);
        MX_ERROR_CHECK(err);
    }

    b = point2.is_in_split_text_container(err, doc);
    MX_ERROR_CHECK(err);

    if (b) {
        point1.save(err, doc);
        MX_ERROR_CHECK(err);

        point2.save(err, doc);
        MX_ERROR_CHECK(err);

        doc->rejoin_split_text_container(err, point2);
        MX_ERROR_CHECK(err);

        point1.restore(err, doc);
        MX_ERROR_CHECK(err);

        point2.restore(err, doc);
        MX_ERROR_CHECK(err);
    }
abort:;
}

// cuts the range of stuff out of a wp document
void mx_wp_object_buffer::cut(
    int& err,
    mx_wp_document& doc,
    mx_wp_doc_pos& point1,
    mx_wp_doc_pos& point2,
    mx_edit_op edit_op)
{
    mx_char_style start_cs, end_cs;
    mx_wp_sheet_pos *ssp, *sep, tp;
    int i;
    mx_sheet* s;
    bool need_to_join_paras = FALSE, need_to_join_words = FALSE;

    buffer_contains_cells = FALSE;
    buffer_contains_text = FALSE;
    buffer_contains_tables = FALSE;

    if (edit_op != mx_remove_e)
        clear_old_contents(&doc);

    // rejoin any split logical paragraphs/tables before we do the cut/copy

    rejoin_split_text_containers(err, point1, point2, &doc);
    MX_ERROR_CHECK(err);

    if (edit_op != mx_copy_e) {
        // need to fix styles after cut, and also store a pair of mods for the
        // cut.

        point1.save(err, &doc, point1.get_sheet());
        MX_ERROR_CHECK(err);

        get_style(err, &doc, point1, start_cs);
        MX_ERROR_CHECK(err);

        get_style(err, &doc, point2, end_cs);
        MX_ERROR_CHECK(err);

        get_style_mod(err, &doc, point1, start_mod);
        MX_ERROR_CHECK(err);

        get_join_requirements(err, point1, point2, doc,
            need_to_join_paras, need_to_join_words);
        MX_ERROR_CHECK(err);

        if (need_to_join_paras && edit_op != mx_remove_e) {
            // save the paragraph style of the second paragraph, if we have
            // some words to be cut from the start of this paragraph

            mx_paragraph_style* ps;
            mx_paragraph* para = point2.get_para(err, &doc);
            MX_ERROR_CHECK(err);

            ps = para->get_base_paragraph_style();

            if (buf_base_style_name)
                delete[] buf_base_style_name;
            buf_base_style_name = mx_string_copy(ps->get_name());
            buf_base_style_mod = mx_paragraph_style_mod(
                *ps, *para->get_paragraph_style());

            (void)this->get_document()->add_or_get_style(err, ps);
            MX_ERROR_CHECK(err);
        }
    }

    ssp = point1.get_sheet_pos();
    sep = point2.get_sheet_pos();
    ssp->set_inside();
    sep->set_inside();
    i = point1.get_sheet();

    while (TRUE) {
        s = doc.sheet(err, i);
        MX_ERROR_CHECK(err);

        if ((i == point1.get_sheet()) && !ssp->is_start()) {
            if (i == point2.get_sheet()) {
                // range is entirely within one sheet
                cut_from_sheet(err, s, *ssp, *sep, edit_op);
                MX_ERROR_CHECK(err);
            } else {
                // range starts on this sheet and ends on a later one

                tp.moveto_end(err, s);
                MX_ERROR_CHECK(err);

                tp.set_after_end();

                cut_from_sheet(err, s, *ssp, tp, edit_op);
                MX_ERROR_CHECK(err);
            }
        } else {
            if (i == point2.get_sheet()) {
                // range starts on a previous sheet but ends on this one
                tp.moveto_start(err, s);
                MX_ERROR_CHECK(err);

                tp.set_before_start();

                cut_from_sheet(err, s, tp, *sep, edit_op);
                MX_ERROR_CHECK(err);
            } else {
                // range covers whole sheet
                cut_sheet(err, doc, i, edit_op);
                MX_ERROR_CHECK(err);

                if (edit_op != mx_copy_e) {
                    i--;
                    point2.back_sheet(err);
                    MX_ERROR_CHECK(err);
                }
            }
        }

        if (i++ == point2.get_sheet()) {
            break;
        }
    }

    if (edit_op != mx_copy_e) {
        mx_paragraph* para;
        // for a cut do any joining necessary, and restore positions

        point1.restore(err, &doc);
        MX_ERROR_CHECK(err);

        if (need_to_join_paras) {
            doc.join_para_with_next(err, point1);
            MX_ERROR_CHECK(err);
        }

        point1.restore(err, &doc);
        MX_ERROR_CHECK(err);

        if (need_to_join_words) {
            mx_char_style_mod mod(start_cs, end_cs);
            mx_para_pos* pp;

            para = point1.get_para(err, &doc);
            MX_ERROR_CHECK(err);

            pp = point1.get_para_pos(err);
            MX_ERROR_CHECK(err);

            para->join_words(err, *pp);
            MX_ERROR_CHECK(err);

            mx_word::set_operations_to_right_of_mods();
            para->aka_insert_mod(err, mod, *pp);
            MX_ERROR_CHECK(err);
        }

        para = point1.get_para(err, &doc);
        MX_ERROR_CHECK(err);

        para->reformat_styles(err);
        MX_ERROR_CHECK(err);

        point2 = point1;
    }

abort:;
}

void mx_wp_object_buffer::copy(int& err,
    mx_wp_document& doc,
    mx_wp_doc_pos& point1,
    mx_wp_doc_pos& point2)
{
    cut(err, doc, point1, point2, mx_copy_e);
}

void mx_wp_object_buffer::remove(int& err,
    mx_wp_document& doc,
    mx_wp_doc_pos& point1,
    mx_wp_doc_pos& point2)
{
    cut(err, doc, point1, point2, mx_remove_e);
}

void mx_wp_object_buffer::insert(int& err, mx_wp_document& doc,
    mx_wp_doc_pos& p,
    bool& need_delete_ta_to_undo)
{
    mx_area_pos* ap;

    need_delete_ta_to_undo = FALSE;

    ap = p.get_area_pos(err);
    MX_ERROR_CHECK(err);

    switch (ap->rtti_class()) {
    case mx_table_area_pos_class_e:
        insert_into_table(err, doc, p, need_delete_ta_to_undo);
        MX_ERROR_CHECK(err);
        break;
    case mx_text_area_pos_class_e:
        insert_into_text(err, doc, p);
        MX_ERROR_CHECK(err);
        break;
    default:
        MX_ERROR_THROW(err, MX_ERROR_UNDEFINED);
    }
abort:;
}

void mx_wp_object_buffer::cut_sheet(int& err, mx_wp_document& doc, int sn,
    mx_edit_op edit_op)
{
    mx_wp_sheet_pos start;
    mx_wp_sheet_pos end;
    mx_sheet* s;

    s = doc.sheet(err, sn);
    MX_ERROR_CHECK(err);

    start.moveto_start(err, s);
    MX_ERROR_CHECK(err);

    end.moveto_end(err, s);
    MX_ERROR_CHECK(err);

    start.set_before_start();
    end.set_after_end();

    // cut the contents
    cut_from_sheet(err, s, start, end, edit_op);
    MX_ERROR_CHECK(err);

    if (edit_op != mx_copy_e) {
        // cut the sheet itself
        doc.delete_sheet_index(err, sn);
        MX_ERROR_CHECK(err);
    }
abort:;
}

static mx_area_pos* new_area_pos_for_area(int& err, mx_area* a)
{
    mx_area_pos* pos = NULL;

    switch (a->rtti_class()) {
    case mx_text_area_class_e:
        pos = new mx_text_area_pos();
        break;
    case mx_table_area_class_e:
        pos = new mx_table_area_pos();
        break;
    default:
        MX_ERROR_THROW(err, MX_ERROR_UNDEFINED);
    }
abort:
    return pos;
}

void mx_wp_object_buffer::cut_from_sheet(int& err, mx_sheet* s,
    mx_wp_sheet_pos& start,
    mx_wp_sheet_pos& end,
    mx_edit_op edit_op)
{
    int n = 0, i = start.get_area_index();
    mx_area* a;
    mx_area_pos *sap, *eap;

    if (start.is_start())
        start.set_before_start();

    while (i <= end.get_area_index()) {
        a = s->get_area(err, i);
        MX_ERROR_CHECK(err);

        // setup start position

        if (i == start.get_area_index() && n++ == 0) {
            // only use the start area position for the first area cut.
            // Otherwise we are cutting subsequent areas and want to cut from
            // the start of it.
            sap = start.get_area_pos(err);
            MX_ERROR_CHECK(err);
            sap = sap->duplicate();
        } else {
            sap = new_area_pos_for_area(err, a);
            MX_ERROR_CHECK(err);

            sap->moveto_start(err, a);
            MX_ERROR_CHECK(err);
            sap->set_before_start();
        }

        // setup end position

        if (i == end.get_area_index()) {
            eap = end.get_area_pos(err);
            MX_ERROR_CHECK(err);
            eap = eap->duplicate();

            if (end.is_after_end())
                eap->set_after_end();
        } else {
            eap = new_area_pos_for_area(err, a);
            MX_ERROR_CHECK(err);

            eap->moveto_end(err, a);
            MX_ERROR_CHECK(err);

            eap->set_after_end();
        }

        cut_from_area(err, end, s, i, a, sap, eap, edit_op);
        MX_ERROR_CHECK(err);

        delete sap;
        delete eap;
        i++;
    }

    if (edit_op != mx_copy_e) {
        // might need to patch things back together if we now have two text
        // areas following each other
        i = start.get_area_index();

        join_areas(err, i, s);
        MX_ERROR_CHECK(err);
    }

abort:;
}

void mx_wp_object_buffer::join_areas(int& err, int i, mx_sheet* s)
{
    if (i >= s->get_num_areas() || i < 0) {
        return;
    }

    if (i < (s->get_num_areas() - 1)) {
        s->join_areas(err, i);
        MX_ERROR_CHECK(err);
    }

    if (i > 0) {
        s->join_areas(err, i - 1);
        MX_ERROR_CHECK(err);
    }
abort:;
}

void mx_wp_object_buffer::cut_from_area(
    int& err,
    mx_wp_sheet_pos& p,
    mx_sheet* s,
    int& area_index,
    mx_area* a,
    mx_area_pos* sap,
    mx_area_pos* eap,
    mx_edit_op edit_op)
{
    bool b = sap->is_start() || sap->is_before_start();

    // is the whole area covered ?
    if (b && eap->is_after_end()) {
        // cut whole area
        mx_document* d = this->get_document();

        if (edit_op == mx_copy_e) {
            a = a->duplicate();
        } else {
            a = s->remove_area(err, area_index);
            MX_ERROR_CHECK(err);

            p.back_area(err);
            if (err == MX_POSITION_RANGE_ERROR) {
                MX_ERROR_CLEAR(err);
            } else {
                MX_ERROR_CHECK(err);
                area_index--;
            }
        }

        if (edit_op == mx_remove_e) {
            delete a;
        } else {
            a->move_to_document(err, d);
            MX_ERROR_CHECK(err);
            contents->append(a);

            set_contents_flags(a);
        }
        return;
    }

    // no, must be either a text or table area - no others can have a
    // partial selection in a wp document.
    switch (a->rtti_class()) {
    case mx_text_area_class_e:
        cut_from_text_area(
            err,
            (mx_text_area*)a,
            (mx_text_area_pos*)sap,
            (mx_text_area_pos*)eap,
            edit_op);
        MX_ERROR_CHECK(err);
        break;

    case mx_table_area_class_e:
        cut_from_table_area(err, p, s, area_index,
            (mx_table_area*)a,
            (mx_table_area_pos*)sap,
            (mx_table_area_pos*)eap,
            edit_op);
        MX_ERROR_CHECK(err);
        break;

    default:
        // for all other types, can only cut the whole thing
        MX_ERROR_THROW(err, MX_WP_BUFFER_BAD_CLASS);
        break;
    }
abort:;
}

void mx_wp_object_buffer::cut_from_text_area(
    int& err,
    mx_text_area* a,
    mx_text_area_pos* sap,
    mx_text_area_pos* eap,
    mx_edit_op edit_op)
{
    mx_text_buffer* text_buff;

    if (sap->rtti_class() != mx_text_area_pos_class_e || eap->rtti_class() != mx_text_area_pos_class_e) {
        MX_ERROR_THROW(err, MX_WP_BUFFER_BAD_CLASS);
    }

    text_buff = new mx_text_buffer(this->get_document());
    text_buff->cut(
        err,
        *((mx_text_area*)a),
        *((mx_text_area_pos*)sap),
        *((mx_text_area_pos*)eap),
        edit_op);
    MX_ERROR_CHECK(err);

    if (edit_op == mx_remove_e) {
        delete text_buff;
    } else {
        contents->append(text_buff);
        buffer_contains_text = TRUE;
    }
abort:;
}

void mx_wp_object_buffer::cut_from_table_area(
    int& err,
    mx_wp_sheet_pos& p,
    mx_sheet* s,
    int& area_index,
    mx_table_area* a,
    mx_table_area_pos* sap,
    mx_table_area_pos* eap,
    mx_edit_op edit_op)
{
    mx_text_area* ta;

    MX_ERROR_ASSERT(err, (sap->is_a(mx_table_area_pos_class_e) && eap->is_a(mx_table_area_pos_class_e)));

    // is the selection totally within one cell?
    if (sap->is_same_cell_as(*eap) && sap->is_inside() && eap->is_inside()) {
        // yes, cut it out as a text area range instead.
        ta = a->get_cell(err, sap->row, sap->column);
        MX_ERROR_CHECK(err);

        sap->text_pos.set_inside();
        eap->text_pos.set_inside();

        cut_from_text_area(err, ta, &(sap->text_pos), &(eap->text_pos),
            edit_op);
        MX_ERROR_CHECK(err);
    } else if (sap->is_before_start() || eap->is_after_end()) {
        // in this case we want to split the table and store the split off
        // table area

        if (sap->is_before_start()) {
            // split underneath the end position

            int num_rows = a->get_num_rows(err);
            MX_ERROR_CHECK(err);

            eap->row++;

            if (eap->row == num_rows) {
                eap->set_after_end();
                cut_from_area(err, p, s, area_index, a, sap, eap, edit_op);
                MX_ERROR_CHECK(err);
            } else {
                cut_table_rows(err, p, s, area_index, a, eap, top_rows_e, edit_op);
                MX_ERROR_CHECK(err);
            }

        } else {
            // split above the start position
            MX_ERROR_ASSERT(err, eap->is_after_end());

            cut_table_rows(err, p, s, area_index, a, sap, bottom_rows_e, edit_op);
            MX_ERROR_CHECK(err);
        }
    } else {
        // otherwise cut table cells from the table area
        mx_table_buffer* table_buff;

        table_buff = new mx_table_buffer(this->get_document());

        table_buff->cut(err, *a, *sap, *eap, edit_op);
        MX_ERROR_CHECK(err);

        if (edit_op == mx_remove_e) {
            delete table_buff;
        } else {
            contents->append(table_buff);
            buffer_contains_cells = TRUE;
        }
    }
abort:;
}

void mx_wp_object_buffer::cut_table_rows(int& err,
    mx_wp_sheet_pos& p,
    mx_sheet* s,
    int& area_index,
    mx_table_area* a,
    mx_table_area_pos* tabp,
    mx_which_rows top_or_bottom,
    mx_edit_op edit_op)
{
    mx_table_area* split_tab;

    if (edit_op == mx_copy_e) {
        // split a copy of the table rows
        a = (mx_table_area*)a->duplicate();

        // area must be part of a sheet to be split
        s->add_area(err, a, area_index);
        MX_ERROR_CHECK(err);

        a->normal_split(err, *tabp);
        MX_ERROR_CHECK(err);

        a = (mx_table_area*)s->remove_area(err, area_index);
        MX_ERROR_CHECK(err);

        split_tab = (mx_table_area*)s->remove_area(err, area_index);
        MX_ERROR_CHECK(err);

        MX_ERROR_ASSERT(err, a->is_a(mx_table_area_class_e));
        MX_ERROR_ASSERT(err, split_tab->is_a(mx_table_area_class_e));

        if (top_or_bottom == top_rows_e) {
            delete split_tab;
            split_tab = a;
        } else {
            delete a;
        }
    } else {
        // split the actual table rows
        int area_index_to_remove = area_index;

        split_tab = a->normal_split(err, *tabp);
        MX_ERROR_CHECK(err);

        // remove the top or bottom bit from the sheet
        if (top_or_bottom == bottom_rows_e)
            area_index_to_remove++;

        split_tab = (mx_table_area*)s->remove_area(err, area_index_to_remove);
        MX_ERROR_CHECK(err);

        MX_ERROR_ASSERT(err, split_tab->is_a(mx_table_area_class_e));

        if (edit_op == mx_remove_e) {
            delete split_tab;
        }
    }

    if (edit_op != mx_remove_e) {
        split_tab->move_to_document(err, this->get_document());
        contents->append(split_tab);
        buffer_contains_tables = TRUE;
    }
abort:;
}

bool mx_wp_object_buffer::is_empty()
{
    return contents->get_num_items() == 0;
}

void mx_wp_object_buffer::clear_old_contents(mx_wp_document* cutting_from_doc)
{
    int err = MX_ERROR_OK;
    mx_list_iterator iter(*contents);

    while (iter.more())
        delete (mx_rtti*)iter.data();
    delete contents;
    contents = new mx_list;

    if (buf_base_style_name != NULL) {
        delete[] buf_base_style_name;
        buf_base_style_name = NULL;
    }

    mx_document* d = this->get_document();

    if (d != cutting_from_doc) {
        // if we are cutting from the same document then we don't need to clear
        // the old styles, since they will all be the same anyway

        d->delete_all_styles(err);
        MX_ERROR_CHECK(err);
    }

    return;
abort:
    global_error_trace->print();
    MX_ERROR_CLEAR(err);
}

void mx_wp_object_buffer::set_contents_flags(mx_area* a)
{
    switch (a->rtti_class()) {
    case mx_text_area_class_e:
        buffer_contains_text = TRUE;
        break;
    case mx_table_area_class_e:
        buffer_contains_tables = TRUE;
        break;
    default:
        break;
    }
}

bool mx_wp_object_buffer::contains_cells()
{
    return buffer_contains_cells;
}

bool mx_wp_object_buffer::contains_cells_only()
{
    return buffer_contains_cells && !buffer_contains_text && !buffer_contains_tables;
}

bool mx_wp_object_buffer::contains_text()
{
    return buffer_contains_text;
}

bool mx_wp_object_buffer::contains_text_only()
{
    return buffer_contains_text && !buffer_contains_cells && !buffer_contains_tables;
}

bool mx_wp_object_buffer::contains_tables()
{
    return buffer_contains_tables;
}

bool mx_wp_object_buffer::contains_tables_only()
{
    return !buffer_contains_cells && !buffer_contains_text && buffer_contains_tables;
}

bool mx_wp_object_buffer::contains_table_at_start()
{
    int err = MX_ERROR_OK;
    mx_table_area* area;

    if (contents->get_num_items() < 1) {
        return FALSE;
    }

    area = (mx_table_area*)contents->get(err, 0);
    MX_ERROR_CHECK(err);

    return area->is_a(mx_table_area_class_e);

abort:
    MX_ERROR_CLEAR(err);
    global_error_trace->print();
    return FALSE;
}

bool mx_wp_object_buffer::contains_table_at_end()
{
    mx_table_area* area;

    if (contents->get_num_items() < 1) {
        return FALSE;
    }

    area = (mx_table_area*)contents->last();

    return area->is_a(mx_table_area_class_e);
}

bool mx_wp_object_buffer::contains_words_at_start()
{
    int err = MX_ERROR_OK;
    mx_text_buffer* buf;

    if (contents->get_num_items() < 1) {
        return FALSE;
    }

    buf = (mx_text_buffer*)contents->get(err, 0);
    MX_ERROR_CHECK(err);

    if (buf->is_a(mx_text_buffer_class_e))
        return buf->contains_words();

    return FALSE;
abort:
    MX_ERROR_CLEAR(err);
    global_error_trace->print();
    return FALSE;
}

bool mx_wp_object_buffer::contains_words_at_end()
{
    mx_text_buffer* buf;

    if (contents->get_num_items() < 1) {
        return FALSE;
    }

    buf = (mx_text_buffer*)contents->last();

    if (buf->is_a(mx_text_buffer_class_e))
        return buf->contains_words();

    return FALSE;
}

void mx_wp_object_buffer::insert_text_buffer(int& err, mx_wp_document& doc, mx_text_buffer* b, mx_wp_doc_pos& p)
{
    mx_area_pos* ap;
    mx_table_area_pos* tap;

    mx_text_area* text_area;
    mx_table_area* table_area;

    ap = p.get_area_pos(err);
    MX_ERROR_CHECK(err);

    switch (ap->rtti_class()) {
    case mx_text_area_pos_class_e:

        text_area = (mx_text_area*)p.get_area(err, &doc);
        MX_ERROR_CHECK(err);

        b->insert(err, *text_area, *((mx_text_area_pos*)ap));
        MX_ERROR_CHECK(err);

        break;

    case mx_table_area_pos_class_e:

        table_area = (mx_table_area*)p.get_area(err, &doc);
        MX_ERROR_CHECK(err);

        tap = (mx_table_area_pos*)ap;

        text_area = table_area->get_cell(err, tap->row, tap->column);
        MX_ERROR_CHECK(err);

        b->insert(err, *text_area, tap->text_pos);
        MX_ERROR_CHECK(err);

        break;

    default:
        mx_sheet* s;

        float width;
        bool temp;
        int i;

        mx_text_area_pos tp;

        i = doc.split(err, p);
        MX_ERROR_CHECK(err);

        s = doc.sheet(err, p.get_sheet());
        MX_ERROR_CHECK(err);

        text_area = s->add_text_area(err, i);
        MX_ERROR_CHECK(err);

        b->insert(err, *text_area, tp);
        MX_ERROR_CHECK(err);

        width = s->get_width_minus_margins(err);
        MX_ERROR_CHECK(err);

        text_area->set_outline(err, width, 1.0);
        MX_ERROR_CHECK(err);

        text_area->reformat(err, temp);
        MX_ERROR_CHECK(err);

        join_areas(err, i, s);
        MX_ERROR_CHECK(err);

        break;
    }

abort:;
}

void mx_wp_object_buffer::insert_table_buffer(int& err, mx_wp_document& doc, mx_table_buffer* b, mx_wp_doc_pos& p)
{
    mx_area_pos* ap;
    mx_table_area* table_area;

    ap = p.get_area_pos(err);
    MX_ERROR_CHECK(err);

    if (ap->rtti_class() == mx_table_area_pos_class_e) {
        table_area = (mx_table_area*)p.get_area(err, &doc);
        MX_ERROR_CHECK(err);

        b->insert(err, *table_area, *((mx_table_area_pos*)ap));
        MX_ERROR_CHECK(err);

        table_area->reposition_cells(err);
        MX_ERROR_CHECK(err);

        table_area->recalculate_outline(err);
        MX_ERROR_CHECK(err);
    } else {
        mx_table_area_pos tap;
        mx_sheet* s;
        int i;

        s = doc.sheet(err, p.get_sheet());
        MX_ERROR_CHECK(err);

        i = doc.split(err, p);
        MX_ERROR_CHECK(err);

        table_area = s->add_table_area(err, i);
        MX_ERROR_CHECK(err);

        table_area->set_num_rows(err, b->num_rows);
        MX_ERROR_CHECK(err);

        table_area->set_num_columns(err, b->num_columns);
        MX_ERROR_CHECK(err);

        b->insert(err, *table_area, tap);
        MX_ERROR_CHECK(err);

        table_area->reposition_cells(err);
        MX_ERROR_CHECK(err);

        table_area->recalculate_outline(err);
        MX_ERROR_CHECK(err);

        p.get_sheet_pos()->moveto_start(err, s, i + 1);
        MX_ERROR_CLEAR(err);

        p.make_valid(err, &doc);
        MX_ERROR_CHECK(err);
    }

abort:;
}

void mx_wp_object_buffer::insert_area(int& err, mx_wp_document& doc, mx_area* a, mx_wp_doc_pos& p)
{
    mx_area* new_area;
    mx_sheet* s;
    mx_area_pos* ap;
    int i;

    new_area = a->duplicate();

    new_area->move_to_document(err, &doc);
    MX_ERROR_CHECK(err);

    ap = p.get_area_pos(err);
    MX_ERROR_CHECK(err);

    (void)p.get_area(err, &doc);
    MX_ERROR_CHECK(err);

    if (ap->is_start()) {
        // if we're at the start of an area, just insert an area at the current
        // position
        i = p.get_sheet_pos()->get_area_index();
    } else {
        i = doc.split(err, p);
        MX_ERROR_CHECK(err);
    }

    s = doc.sheet(err, p.get_sheet());
    MX_ERROR_CHECK(err);

    s->add_area(err, new_area, i);
    MX_ERROR_CHECK(err);

    p.get_sheet_pos()->moveto_start(err, s, i + 1);
    MX_ERROR_CLEAR(err);

    p.make_valid(err, &doc);
    MX_ERROR_CHECK(err);

abort:;
}

void mx_wp_object_buffer::move_to_document(int& err, mx_wp_document* d)
{
    mx_list_iterator iter(*contents);

    d->delete_all_styles(err);
    MX_ERROR_CHECK(err);

    while (iter.more()) {
        mx_rtti* o = (mx_rtti*)iter.data();

        switch (o->rtti_class()) {
        case mx_text_buffer_class_e:
        case mx_table_buffer_class_e:
            ((mx_buffer*)o)->set_document(err, d);
            MX_ERROR_CHECK(err);
            break;
        default:
            // must be an area
            ((mx_area*)o)->move_to_document(err, d);
            MX_ERROR_CHECK(err);
            break;
        }
    }
abort:;
}

void mx_wp_object_buffer::insert_into_text(int& err, mx_wp_document& doc,
    mx_wp_doc_pos& p)
{
    mx_list_iterator iter(*contents);
    mx_char_style style_at_insert_pos;
    mx_wp_doc_pos insert_pos = p;
    mx_rtti* o;
    mx_area_pos* ap;

    ap = p.get_area_pos(err);
    MX_ERROR_CHECK(err);

    if (!this->contains_text_only()) {
        // make sure we're not trying to insert anything other than text into a
        // table
        MX_ERROR_ASSERT(err, ap->is_a(mx_text_area_pos_class_e));
    }

    get_style(err, &doc, insert_pos, style_at_insert_pos);
    MX_ERROR_CHECK(err);

    insert_pos.save(err, &doc, insert_pos.get_sheet());
    MX_ERROR_CHECK(err);

    while (iter.more()) {
        o = (mx_rtti*)iter.data();

        switch (o->rtti_class()) {
        case mx_text_buffer_class_e:
            insert_text_buffer(err, doc, (mx_text_buffer*)o, p);
            MX_ERROR_CHECK(err);
            break;
        case mx_table_buffer_class_e:
            insert_table_buffer(err, doc, (mx_table_buffer*)o, p);
            MX_ERROR_CHECK(err);
            break;
        default:
            // must be a complete area
            insert_area(err, doc, (mx_area*)o, p);
            MX_ERROR_CHECK(err);
            break;
        }
    }

    insert_pos.restore(err, &doc);
    MX_ERROR_CHECK(err);

    // what to do now depends on what we inserted. If it started with text,
    // then we need to fix the style mods at the start of the block. Equally if
    // it ended with text, then we need to fix the style mods at the end. If it
    // started with text and ended with a table area, then we may have inserted
    // one too many paragraphs, and we need to delete one.

    this->fix_post_insert_document_structure(err, doc, insert_pos, p,
        style_at_insert_pos);
    MX_ERROR_CHECK(err);

abort:;
}

void mx_wp_object_buffer::insert_into_table(int& err, mx_wp_document& doc,
    mx_wp_doc_pos& p,
    bool& need_delete_ta_to_undo)
{
    need_delete_ta_to_undo = FALSE;

    if (this->contains_text_only()) {
        insert_into_text(err, doc, p);
        MX_ERROR_CHECK(err);
    } else if (this->contains_cells_only()) {
        insert_cells_into_table(err, doc, p);
        MX_ERROR_CHECK(err);
    } else {
        insert_text_and_tables_into_table(err, doc, p, need_delete_ta_to_undo);
        MX_ERROR_CHECK(err);
    }
abort:;
}

void mx_wp_object_buffer::insert_cells_into_table(int& err,
    mx_wp_document& doc,
    mx_wp_doc_pos& p)
{
    MX_ERROR_ASSERT(err, contents->get_num_items() == 1);

    mx_table_buffer* tab_buf;
    mx_sheet* s;

    tab_buf = (mx_table_buffer*)contents->last();

    this->insert_table_buffer(err, doc, tab_buf, p);
    MX_ERROR_CHECK(err);

    p.save(err, &doc, p.get_sheet());
    MX_ERROR_CHECK(err);

    s = doc.sheet(err, p.get_sheet());
    MX_ERROR_CHECK(err);

    this->join_areas(err, p.get_sheet_pos()->get_area_index(), s);
    MX_ERROR_CHECK(err);

    p.restore(err, &doc);
    MX_ERROR_CHECK(err);
abort:;
}

void mx_wp_object_buffer ::insert_text_and_tables_into_table(int& err, mx_wp_document& doc,
    mx_wp_doc_pos& p,
    bool& need_delete_ta_to_undo)
{
    mx_sheet* s;
    int i;

    need_delete_ta_to_undo = FALSE;

    if (this->contains_table_at_start() && this->contains_table_at_end()) {
        // there is a table area at the start and the end of the buffer, so we
        // can just split the table we're in and insert areas
        mx_list_iterator iter(*contents);

        while (iter.more()) {
            this->insert_area(err, doc, (mx_area*)iter.data(), p);
            MX_ERROR_CHECK(err);
        }
    } else {
        // we've got some text to insert so we should put a text area in in
        mx_area* a;
        mx_area_pos* ap;
        bool b;

        i = doc.split(err, p);
        MX_ERROR_CHECK(err);

        s = doc.sheet(err, p.get_sheet());
        MX_ERROR_CHECK(err);

        s->add_text_area(err, i);
        MX_ERROR_CHECK(err);

        p.get_sheet_pos()->moveto_start(err, s, i);
        MX_ERROR_CLEAR(err);

        insert_into_text(err, doc, p);
        MX_ERROR_CHECK(err);

        s = doc.sheet(err, p.get_sheet());
        MX_ERROR_CHECK(err);

        a = s->get_area(err, p.get_sheet_pos()->get_area_index());
        MX_ERROR_CHECK(err);

        if (a->is_a(mx_text_area_class_e)) {
            ap = p.get_area_pos(err);
            MX_ERROR_CHECK(err);

            b = ap->is_start() && ap->is_end(err, a);
            MX_ERROR_CHECK(err);

            if (b) {
                // the text area we added is empty, so delete it. which means
                // the area does not need to be deleted on undo

                mx_wp_sheet_pos* sp = p.get_sheet_pos();

                need_delete_ta_to_undo = FALSE;

                a = s->remove_area(err, sp->get_area_index());
                MX_ERROR_CHECK(err);

                delete a;

                sp->set_area_index(err, sp->get_area_index() - 1, s);
                MX_ERROR_CHECK(err);

                p.moveto_end_row(err, &doc);
                MX_ERROR_CHECK(err);
            } else {
                // the text area is not empty, so don't delete it. This means the
                // undo must delete the area to properly undo the paste

                need_delete_ta_to_undo = TRUE;
            }
        }
    }

    p.save(err, &doc, p.get_sheet());
    MX_ERROR_CHECK(err);

    s = doc.sheet(err, p.get_sheet());
    MX_ERROR_CHECK(err);

    for (i = s->get_num_areas() - 2; i >= 0; i--) {
        s->join_areas(err, i);
        MX_ERROR_CHECK(err);
    }

    p.restore(err, &doc);
    MX_ERROR_CHECK(err);
abort:;
}

void mx_wp_object_buffer::fix_post_insert_document_structure(
    int& err, mx_wp_document& doc,
    mx_wp_doc_pos& insert_start,
    mx_wp_doc_pos& insert_end,
    mx_char_style& cs_at_insert_pos)
{
    if (this->contains_words_at_start()) {
        // we've got some words at the start of the stuff we've inserted so
        // insert a character style mod.

        mx_char_style start_style;
        mx_char_style_mod mod;
        mx_paragraph* para;
        mx_para_pos* pp;

        // calculate and insert mod for the start of the pasted region
        para = insert_start.get_para(err, &doc);
        MX_ERROR_CHECK(err);

        pp = insert_start.get_para_pos(err);
        MX_ERROR_CHECK(err);

        start_style = *para->get_paragraph_style()->get_char_style();
        start_style += start_mod;

        mx_word::set_operations_to_right_of_mods();
        mod = mx_char_style_mod(cs_at_insert_pos, start_style);
        para->aka_insert_mod(err, mod, *pp);
        MX_ERROR_CHECK(err);
    }

    if (this->contains_words_at_end()) {
        // insert a style mod switching from the style of the stuff we've just
        // inserted back to the style at the start of the block of stuff we've
        // inserted.

        mx_char_style* end_style;
        mx_char_style_mod mod;
        mx_paragraph* para;
        mx_para_pos* pp;

        para = insert_end.get_para(err, &doc);
        MX_ERROR_CHECK(err);

        pp = insert_end.get_para_pos(err);
        MX_ERROR_CHECK(err);

        // make sure styles are up to date
        para->reformat_styles(err);
        MX_ERROR_CHECK(err);

        mx_word::set_operations_to_right_of_mods();
        end_style = para->aka_get_style(err, *pp);
        MX_ERROR_CHECK(err);

        mod = mx_char_style_mod(*end_style, cs_at_insert_pos);
        para->aka_insert_mod(err, mod, *pp);
        MX_ERROR_CHECK(err);

        // now fix the paragraph style for the last paragraph we inserted
        if (buf_base_style_name != NULL) {
            // insert the end paragraph style
            mx_paragraph_style* para_style;
            para_style = doc.get_named_style(err, buf_base_style_name);
            MX_ERROR_CHECK(err);

            para = insert_end.get_para(err, &doc);
            MX_ERROR_CHECK(err);

            para->set_paragraph_style(para_style);
            para->modify_paragraph_style(buf_base_style_mod);
        }
    }

    if (this->contains_table_at_end() && this->contains_words_at_start()) {
        // in this case we have a table at the end, but there was no table at
        // the start, so we should try to delete the paragraph at the current
        // position (it is an artifact from the fisrt text buffer), but only if
        // we're not at the end of the document.

        bool b = insert_end.is_end(err, &doc);
        MX_ERROR_CHECK(err);

        if (!b) {
            int num_paras;
            mx_paragraph* para;
            mx_para_pos* para_pos;

            mx_text_area* area = (mx_text_area*)insert_end.get_area(err, &doc);
            MX_ERROR_CHECK(err);

            MX_ERROR_ASSERT(err, area->is_a(mx_text_area_class_e));

            para = insert_end.get_para(err, &doc);
            MX_ERROR_CHECK(err);

            para_pos = insert_end.get_para_pos(err);
            MX_ERROR_CHECK(err);

            b = para_pos->is_start() && para_pos->is_end(err, para);
            MX_ERROR_CHECK(err);

            if (b) {
                para = area->remove(err, 0);
                MX_ERROR_CHECK(err);

                delete para;

                num_paras = area->num_paragraphs(err);
                MX_ERROR_CHECK(err);

                if (num_paras == 0) {
                    mx_wp_sheet_pos* sp = insert_end.get_sheet_pos();
                    mx_sheet* s = doc.sheet(err, insert_end.get_sheet());
                    MX_ERROR_CHECK(err);

                    area = (mx_text_area*)s->remove_area(err, sp->get_area_index());
                    MX_ERROR_CHECK(err);

                    MX_ERROR_ASSERT(err, area->is_a(mx_text_area_class_e));
                    delete area;

                    sp->back_area(err);
                    MX_ERROR_CHECK(err);

                    sp->reset(err, s);
                    MX_ERROR_CHECK(err);

                    insert_end.moveto_end_area(err, &doc);
                    MX_ERROR_CHECK(err);
                } else {
                    bool b;
                    area->recalculate_outline(err, b);
                    MX_ERROR_CHECK(err);

                    insert_end.left(err, &doc);
                    MX_ERROR_CHECK(err);
                }
            }
        }
    }

    // join the areas at the start and end of the inserted block

    insert_start.save(err, &doc, insert_start.get_sheet());
    MX_ERROR_CHECK(err);

    insert_end.save(err, &doc, insert_end.get_sheet());
    MX_ERROR_CHECK(err);

    mx_sheet* s;
    int i;

    i = insert_end.get_sheet_pos()->get_area_index();
    s = doc.sheet(err, insert_end.get_sheet());
    MX_ERROR_CHECK(err);

    this->join_areas(err, i, s);
    MX_ERROR_CHECK(err);

    i = insert_start.get_sheet_pos()->get_area_index();
    s = doc.sheet(err, insert_start.get_sheet());
    MX_ERROR_CHECK(err);

    this->join_areas(err, i, s);
    MX_ERROR_CHECK(err);

    insert_start.restore(err, &doc);
    MX_ERROR_CHECK(err);

    insert_end.restore(err, &doc);
    MX_ERROR_CHECK(err);
abort:;
}
