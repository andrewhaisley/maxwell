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
 * MODULE/CLASS :  mx_wp_repaginator
 *
 * AUTHOR : Andrew Haisley
 *
 *
 *
 * DESCRIPTION:
 *
 * A class used by wp documents when repaginating. It is used to find the
 * lengths (heights) of things in a document. Thes area paragraphs and
 * table areas which may have been split up into bits by previous
 * reformats and the like.
 *
 *
 *
 *
 */

#include <mx.h>
#include <mx_rtti.h>
#include <mx_wp_rp.h>

#include <mx_ap_iter.h>
#include <mx_area.h>
#include <mx_paragraph.h>
#include <mx_style.h>
#include <mx_text_area.h>
#include <mx_list_iter.h>
#include <mx_wp_doc.h>

mx_wp_repaginator::mx_wp_repaginator(int& err, mx_wp_document* doc, int start_sheet)
{
    mx_wp_doc_pos start, end;
    mx_rtti* o;

    items = new mx_list;
    breaks = new mx_list;

    mx_wp_repaginator::start_sheet = start_sheet;
    mx_wp_repaginator::doc = doc;

    start.moveto_start_sheet(err, doc, start_sheet);
    MX_ERROR_CHECK(err);

    end.moveto_end(err, doc);
    MX_ERROR_CHECK(err);

    {
        mx_area_para_iterator iter(doc, start, end);

        do {
            MX_ERROR_CHECK(err);

            o = iter.data(err);
            MX_ERROR_CHECK(err);

            if (o->rtti_class() == mx_paragraph_class_e) {
                add_paragraph(err,
                    iter.sheet_num(),
                    iter.sheet(),
                    iter.area(),
                    (mx_paragraph*)o);
                MX_ERROR_CHECK(err);
            } else {
                add_area(err, iter.sheet_num(), iter.sheet(), iter.area());
                MX_ERROR_CHECK(err);
            }
        } while (iter.more(err));
        MX_ERROR_CHECK(err);
    }

abort:;
}

mx_wp_repaginator::~mx_wp_repaginator()
{
    mx_repagination_item* i;
    mx_break_item_t* b;

    mx_list_iterator iter(*items);
    mx_list_iterator iter2(*breaks);

    while (iter.more()) {
        i = (mx_repagination_item*)iter.data();
        delete i;
    }

    delete items;

    while (iter2.more()) {
        b = (mx_break_item_t*)iter2.data();
        delete b;
    }

    delete breaks;
}

void mx_wp_repaginator::find_sheet_breaks(int& err)
{
    int item_index, num_items;
    float temp, height, height_so_far, item_height, area_height;
    mx_sheet* s;
    mx_text_area* a;

    mx_repagination_item* item;

    s = doc->sheet(err, 0);
    MX_ERROR_CHECK(err);

    temp = height = s->get_height_minus_margins(err);
    MX_ERROR_CHECK(err);

    a = doc->header(err);
    MX_ERROR_CHECK(err);

    if (a != NULL) {
        area_height = a->get_height(err);
        MX_ERROR_CHECK(err);

        if (area_height > (temp * MX_WP_DOC_MAX_HEADER)) {
            height -= temp * MX_WP_DOC_MAX_HEADER;
            MX_ERROR_CHECK(err);
        } else {
            height -= a->get_height(err);
            MX_ERROR_CHECK(err);
        }
    }

    a = doc->footer(err);
    MX_ERROR_CHECK(err);

    if (a != NULL) {
        area_height = a->get_height(err);
        MX_ERROR_CHECK(err);

        if (area_height > (temp * MX_WP_DOC_MAX_HEADER)) {
            height -= temp * MX_WP_DOC_MAX_HEADER;
            MX_ERROR_CHECK(err);
        } else {
            height -= a->get_height(err);
            MX_ERROR_CHECK(err);
        }
    }

    // height may now be very small or -ve due to silly page setup - force
    // it to be at least 10mm.
    if (height < 10.0) {
        height = 10.0;
    }

    num_items = items->get_num_items();

    height_so_far = 0.0;
    item_index = 0;
    while (item_index < num_items) {
        item = (mx_repagination_item*)items->get(err, item_index);
        MX_ERROR_CHECK(err);

        // does this item have a break before it? But only check this if we're
        // not on the first part of the first sheet
        if (!(start_sheet == 0 && item_index == 0) && item->get_constraint() == mx_repagination_item::mx_break_before_e) {
            // yes, put a sheet break in before it
            add_sheet_break(err, item_index, 0, 0);
            MX_ERROR_CHECK(err);

            height_so_far = item->get_height();

            if (height_so_far > height) {
                // the whole of this item won't fit on this sheet.
                handle_sheet_overflow(err, height, height_so_far, item, item_index);
                MX_ERROR_CHECK(err);
            } else {
                // it fits totally
                item_index++;
            }
        } else {
            item_height = item->get_height();

            if ((item_height + height_so_far) > height) {
                // the whole of this item won't fit on this sheet.
                handle_sheet_overflow(err, height, height_so_far, item, item_index);
                MX_ERROR_CHECK(err);
            } else {
                // it fits totally
                height_so_far += item_height;
                item_index++;
            }
        }
    }

abort:;
}

void mx_wp_repaginator::add_sheet_break(int& err, int item_index, int bit, int sub_bit)
{
    mx_break_item_t* b;
    mx_table_area* ta;
    mx_paragraph* para;

    bool is_para;
    int nr;

    if ((item_index == 0) && (bit == 0) && (sub_bit == 0)) {
        return;
    }

    b = new mx_break_item_t;

    b->item = (mx_repagination_item*)items->get(err, item_index);
    MX_ERROR_CHECK(err);

    b->bit_num = bit;
    b->line_or_row_index = sub_bit;

    // if the break is at the end of a table or a paragraph (rather than
    // in the middle) need to nudge it on a bit.
    is_para = b->item->is_para(err);
    MX_ERROR_CHECK(err);

    if (is_para) {
        para = b->item->get_para(err, bit);
        MX_ERROR_CHECK(err);

        if (para->num_lines() == sub_bit) {
            sub_bit = 0;

            if (bit == b->item->get_num_bits() - 1) {
                bit = 0;
                item_index++;
            } else {
                bit++;
            }

            b->item = (mx_repagination_item*)items->get(err, item_index);
            MX_ERROR_CLEAR(err);

            b->bit_num = bit;
            b->line_or_row_index = sub_bit;
        }
    } else {
        // it's a table
        ta = (mx_table_area*)b->item->get_area(err, bit);
        MX_ERROR_CHECK(err);

        nr = ta->get_num_rows(err);
        MX_ERROR_CHECK(err);

        if (sub_bit == nr) {
            sub_bit = 0;

            if (bit == b->item->get_num_bits() - 1) {
                bit = 0;
                item_index++;
            } else {
                bit++;
            }

            b->item = (mx_repagination_item*)items->get(err, item_index);
            MX_ERROR_CLEAR(err);

            b->bit_num = bit;
            b->line_or_row_index = sub_bit;
        }
    }

    if (b->item == NULL) {
        delete b;
    } else {
        breaks->append(b);
    }

abort:;
}

void mx_wp_repaginator::handle_sheet_overflow(
    int& err,
    float height,
    float& height_so_far,
    mx_repagination_item* item,
    int& item_index)
{
    bool b;
    bool keep_with_next = FALSE;
    bool keep_with_prev = FALSE;
    int first_item, last_item;

    float f;

    mx_repagination_item* prev;

    mx_rtti_class_e area_class;

    // is the thing constrained to be kept with the next item?
    keep_with_next = item->get_constraint() == mx_repagination_item::mx_keep_with_next_e;

    // or with the previous
    if (item_index > 0) {
        prev = (mx_repagination_item*)items->get(err, item_index - 1);
        MX_ERROR_CHECK(err);

        keep_with_prev = prev->get_constraint() == mx_repagination_item::mx_keep_with_next_e;
    }

    if (keep_with_next || keep_with_prev) {
        // figure out the total size of the keep with next block
        f = keep_with_next_block_size(err, item_index, first_item, last_item);
        MX_ERROR_CHECK(err);

        // break before the start of it
        add_sheet_break(err, first_item, 0, 0);
        MX_ERROR_CHECK(err);

        height_so_far = 0.0;

        if (f > height) {
            // have to split something - ignore keep_together constraints
            split_keep_together_block(err, first_item, last_item, height, height_so_far);
            MX_ERROR_CHECK(err);

            item_index = last_item + 1;
            return;
        } else {
            // continue from the first item after the block
            item_index = last_item + 1;
            height_so_far = f;
            return;
        }
    } else {
        // is the thing constrained to be kept in one piece?
        if (item->get_constraint() == mx_repagination_item::mx_keep_together_e) {
            // break before start of it
            add_sheet_break(err, item_index, 0, 0);
            MX_ERROR_CHECK(err);

            if (item->get_height() > height) {
                // bigger than the sheet height so have to break it as well
                break_item(err, item_index, height, height_so_far, item);
                MX_ERROR_CHECK(err);

                item_index++;
                return;
            } else {
                item_index++;
                height_so_far = item->get_height();
                return;
            }
        } else {
            // can try to split it up

            // is the item an area or is it a para?
            b = item->is_para(err);
            MX_ERROR_CHECK(err);

            if (b) {
                // insert break(s) at the appropriate place(s) within the
                // paragraph
                find_paragraph_break_pos(err, item_index++, height, height_so_far, item);
                MX_ERROR_CHECK(err);

                return;
            } else {
                // if it's a non splittable area (i.e. isn't a table) push
                // the whole thing onto the next sheet.
                area_class = item->get_area_class(err);
                MX_ERROR_CHECK(err);

                if (area_class == mx_table_area_class_e) {
                    find_table_break_pos(err, item_index++, height, height_so_far, item);
                    MX_ERROR_CHECK(err);

                    return;
                } else {
                    add_sheet_break(err, item_index++, 0, 0);
                    MX_ERROR_CHECK(err);

                    height_so_far = item->get_height();

                    return;
                }
            }
        }
    }
abort:;
}

int mx_wp_repaginator::get_num_required_sheets()
{
    return breaks->get_num_items() + 1;
}

void mx_wp_repaginator::split_things(int& err)
{
    mx_list_iterator iter(*breaks);
    mx_break_item_t* break_item;

    while (iter.more()) {
        break_item = (mx_break_item_t*)iter.data();

        if (break_item->line_or_row_index != 0) {
            // needs splitting into two
            split_item(err, break_item);
            MX_ERROR_CHECK(err);
        } else {
            if (break_item->bit_num != 0) {
                // just split the item as the underlying thing is already
                // split in the right place
                split_item_only(err, break_item);
                MX_ERROR_CHECK(err);
            }
        }
    }

abort:;
}

void mx_wp_repaginator::split_item(int& err, mx_break_item_t* break_item)
{
    mx_repagination_item* item;

    bool b;

    item = break_item->item;

    b = item->is_bit_paragraph(err, 0);
    MX_ERROR_CHECK(err);

    if (b) {
        split_paragraph(err, break_item, item);
        MX_ERROR_CHECK(err);
    } else {
        split_table_area(err, break_item, item);
        MX_ERROR_CHECK(err);
    }

abort:;
}

void mx_wp_repaginator::split_paragraph(
    int& err,
    mx_break_item_t* break_item,
    mx_repagination_item* item)
{
    mx_text_area* ta;
    mx_paragraph* p;
    mx_paragraph* new_para;
    mx_sheet* s;

    int bn, n;

    mx_repagination_item* new_item;
    mx_break_item_t* next_break;

    int sheet_index, para_index;

    ta = (mx_text_area*)item->get_area(err, break_item->bit_num);
    MX_ERROR_CHECK(err);

    p = item->get_para(err, break_item->bit_num);
    MX_ERROR_CHECK(err);

    s = item->get_sheet(err, break_item->bit_num);
    MX_ERROR_CHECK(err);

    new_para = p->split(err, break_item->line_or_row_index);
    MX_ERROR_CHECK(err);

    para_index = ta->get_index(err, p);
    MX_ERROR_CHECK(err);

    ta->insert(err, para_index + 1, new_para);
    MX_ERROR_CHECK(err);

    sheet_index = item->get_sheet_index(err, break_item->bit_num);
    MX_ERROR_CHECK(err);

    item->insert_para(
        err,
        break_item->bit_num + 1,
        sheet_index,
        s,
        ta,
        new_para);

    MX_ERROR_CHECK(err);

    if (break_item->bit_num == 0) {
        new_item = item->split(err, break_item->bit_num + 1);
        MX_ERROR_CHECK(err);

        n = get_item_num(break_item) + 1;

        items->insert(err, n, new_item);
        MX_ERROR_CHECK(err);
    } else {
        new_item = item->split(err, break_item->bit_num);
        MX_ERROR_CHECK(err);

        n = get_item_num(break_item) + 1;

        items->insert(err, n, new_item);
        MX_ERROR_CHECK(err);

        new_item = new_item->split(err, 1);
        MX_ERROR_CHECK(err);

        items->insert(err, n + 1, new_item);
        MX_ERROR_CHECK(err);
    }

    // patch the following break items that point to the thing
    // we've just broken up
    bn = breaks->find_index(err, break_item) + 1;
    MX_ERROR_CHECK(err);

    while (bn < breaks->get_num_items()) {
        next_break = (mx_break_item_t*)breaks->get(err, bn);
        MX_ERROR_CHECK(err);

        if (next_break->item == item) {
            next_break->item = new_item;

            // are they in the same bit?
            if (next_break->bit_num == break_item->bit_num) {
                next_break->line_or_row_index -= break_item->line_or_row_index;
            }
            next_break->bit_num -= break_item->bit_num;
            bn++;
        } else {
            break;
        }
    }

    break_item->item = new_item;
    break_item->bit_num = 0;
    break_item->line_or_row_index = 0;

abort:;
}

void mx_wp_repaginator::split_table_area(
    int& err,
    mx_break_item_t* break_item,
    mx_repagination_item* item)

{
    mx_table_area* ta;
    mx_table_area* new_area;
    mx_sheet* s;

    int bn, n;

    mx_repagination_item* new_item;
    mx_break_item_t* next_break;

    int sheet_index;

    ta = (mx_table_area*)item->get_area(err, break_item->bit_num);
    MX_ERROR_CHECK(err);

    s = item->get_sheet(err, break_item->bit_num);
    MX_ERROR_CHECK(err);

    new_area = ta->repaginate_split(err, break_item->line_or_row_index);
    MX_ERROR_CHECK(err);

    sheet_index = item->get_sheet_index(err, break_item->bit_num);
    MX_ERROR_CHECK(err);

    (void)s->get_index_position(err, ta);
    MX_ERROR_CHECK(err);

    item->insert_area(
        err,
        break_item->bit_num + 1,
        sheet_index,
        s,
        new_area);
    MX_ERROR_CHECK(err);

    if (break_item->bit_num == 0) {
        new_item = item->split(err, break_item->bit_num + 1);
        MX_ERROR_CHECK(err);

        n = get_item_num(break_item) + 1;

        items->insert(err, n, new_item);
        MX_ERROR_CHECK(err);
    } else {
        new_item = item->split(err, break_item->bit_num);
        MX_ERROR_CHECK(err);

        n = get_item_num(break_item) + 1;

        items->insert(err, n, new_item);
        MX_ERROR_CHECK(err);

        new_item = new_item->split(err, 1);
        MX_ERROR_CHECK(err);

        items->insert(err, n + 1, new_item);
        MX_ERROR_CHECK(err);
    }

    // patch the following break items that point to the thing
    // we've just broken up
    bn = breaks->find_index(err, break_item) + 1;
    MX_ERROR_CHECK(err);

    while (bn < breaks->get_num_items()) {
        next_break = (mx_break_item_t*)breaks->get(err, bn);
        MX_ERROR_CHECK(err);

        if (next_break->item == item) {
            next_break->item = new_item;

            // are they in the same bit?
            if (next_break->bit_num == break_item->bit_num) {
                next_break->line_or_row_index -= break_item->line_or_row_index;
            }
            next_break->bit_num -= break_item->bit_num;
            bn++;
        } else {
            break;
        }
    }

    break_item->item = new_item;
    break_item->bit_num = 0;
    break_item->line_or_row_index = 0;

abort:;
}

void mx_wp_repaginator::figure_destinations(int& err)
{
    int dest_sheet = start_sheet, break_num = 0;
    int item_num = 0;

    mx_repagination_item* item;

    mx_break_item_t* next_break;

    // go through the things from figuring out what sheet they should
    // be on
    while (TRUE) {
        if (break_num >= breaks->get_num_items()) {
            // move all the remaining stuff to the current sheet
            while (item_num < items->get_num_items()) {
                item = (mx_repagination_item*)items->get(err, item_num);
                MX_ERROR_CHECK(err);

                item->set_dest(dest_sheet);

                item_num++;
            }

            // finished
            break;
        } else {
            next_break = (mx_break_item_t*)breaks->get(err, break_num++);
            MX_ERROR_CHECK(err);

            while (item_num < get_item_num(next_break)) {
                item = (mx_repagination_item*)items->get(err, item_num);
                MX_ERROR_CHECK(err);

                item->set_dest(dest_sheet);

                item_num++;
            }

            dest_sheet++;
        }
    }
abort:;
}

void mx_wp_repaginator::move_things(int& err)
{
    int bn, i, n, source_sheet;
    bool needs_moving;

    mx_repagination_item* item;

    // first, split any paragraphs that need it - this also splits items
    // so that each item fits entirely within its destination sheet
    split_things(err);
    MX_ERROR_CHECK(err);

    // figure out what sheet each of the items should go into
    figure_destinations(err);
    MX_ERROR_CHECK(err);

#if 0
    fprintf(stderr, "after figure dest\n");
    fprintf(stderr, "=================\n");
    print_debug();
#endif

    // first pass - from the start to the end moving things that need
    // to be moved backwards
    n = items->get_num_items();

    for (i = 0; i < n; i++) {
        item = (mx_repagination_item*)items->get(err, i);
        MX_ERROR_CHECK(err);

        needs_moving = FALSE;

        // go through each bit to see if any need moving
        for (bn = 0; bn < item->get_num_bits(); bn++) {
            source_sheet = item->get_sheet_index(err, bn);
            MX_ERROR_CHECK(err);

            if (source_sheet > item->get_dest()) {
                needs_moving = TRUE;
            }
        }

        if (needs_moving) {
            move_item(err, item->get_dest(), item);
            MX_ERROR_CHECK(err);
        }
    }

    // second pass - from the end the start moving things that need
    // to be moved forwards
    for (i = n - 1; i >= 0; i--) {
        item = (mx_repagination_item*)items->get(err, i);
        MX_ERROR_CHECK(err);

        needs_moving = FALSE;

        // go through each bit to see if any need moving
        for (bn = 0; bn < item->get_num_bits(); bn++) {
            source_sheet = item->get_sheet_index(err, bn);
            MX_ERROR_CHECK(err);

            if (source_sheet < item->get_dest()) {
                needs_moving = TRUE;
            }
        }

        if (needs_moving) {
            move_item(err, item->get_dest(), item);
            MX_ERROR_CHECK(err);
        }
    }

    // reposition things on those sheets that have been modified
    reposition_sheet_areas(err);
    MX_ERROR_CHECK(err);

abort:;
}

void mx_wp_repaginator::reposition_sheet_areas(int& err)
{
    for (auto i : m_sheets_to_reposition) {
        doc->reposition_sheet_areas(err, i.first);
        MX_ERROR_CHECK(err);
    }

abort:;
}

void mx_wp_repaginator::move_item(int& err, int dest_sheet, mx_repagination_item* item)
{
    int i, sn;
    bool b;

    mx_paragraph* p;
    mx_sheet* s;
    mx_area* a;

    m_sheets_to_reposition[dest_sheet] = (mx_sheet *)1;
    MX_ERROR_CLEAR(err);

    for (i = 0; i < item->get_num_bits(); i++) {
        b = item->is_bit_paragraph(err, i);
        MX_ERROR_CHECK(err);

        if (b) {
            p = item->get_para(err, i);
            MX_ERROR_CHECK(err);

            sn = item->get_sheet_index(err, i);
            MX_ERROR_CHECK(err);

            s = item->get_sheet(err, i);
            MX_ERROR_CHECK(err);

            m_sheets_to_reposition[sn] = s;

            a = item->get_area(err, i);
            MX_ERROR_CHECK(err);

            move_paragraph(err, dest_sheet, sn, a, p);
            MX_ERROR_CHECK(err);
        } else {
            sn = item->get_sheet_index(err, i);
            MX_ERROR_CHECK(err);

            s = item->get_sheet(err, i);
            MX_ERROR_CHECK(err);

            m_sheets_to_reposition[sn] = s;

            a = item->get_area(err, i);
            MX_ERROR_CHECK(err);

            move_area(err, dest_sheet, sn, a);
            MX_ERROR_CHECK(err);
        }
    }

abort:;
}

void mx_wp_repaginator::move_paragraph(
    int& err,
    int dest_sheet,
    int source_sheet,
    mx_area* a,
    mx_paragraph* p)
{
    // if dest == source don't need to do anything

    if (dest_sheet < source_sheet) {
        move_paragraph_into_sheet_bottom(err, dest_sheet, source_sheet, a, p);
        MX_ERROR_CHECK(err);
    } else {
        if (dest_sheet > source_sheet) {
            move_paragraph_into_sheet_top(err, dest_sheet, source_sheet, a, p);
            MX_ERROR_CHECK(err);
        }
    }
abort:;
}

void mx_wp_repaginator::move_paragraph_into_sheet_bottom(
    int& err,
    int dest_sheet,
    int source_sheet,
    mx_area* a,
    mx_paragraph* p)
{
    mx_text_area *ta, *dest_ta;
    mx_sheet* dest;
    mx_sheet* source;
    mx_paragraph* temp;

    int n;
    bool b;

    dest = doc->sheet(err, dest_sheet);
    MX_ERROR_CHECK(err);

    source = doc->sheet(err, source_sheet);
    MX_ERROR_CHECK(err);

    ta = (mx_text_area*)a;

    ta->remove(err, p);
    MX_ERROR_CHECK(err);

    if (dest->is_bottom_area_text()) {
        dest_ta = (mx_text_area*)dest->get_bottom_area(err);
        MX_ERROR_CHECK(err);
    } else {
        dest_ta = dest->add_text_area_to_bottom(err);
        MX_ERROR_CHECK(err);

        temp = dest_ta->remove(err, 0);
        MX_ERROR_CHECK(err);

        delete temp;
    }

    dest_ta->append(err, p);
    MX_ERROR_CHECK(err);

    dest_ta->recalculate_outline(err, b);
    MX_ERROR_CHECK(err);

    n = ta->num_paragraphs(err);
    MX_ERROR_CHECK(err);

    if (n == 0) {
        source->delete_text_area_ref(err, ta);
        MX_ERROR_CHECK(err);
    } else {
        ta->recalculate_outline(err, b);
        MX_ERROR_CHECK(err);
    }

abort:;
}

void mx_wp_repaginator::move_paragraph_into_sheet_top(
    int& err,
    int dest_sheet,
    int source_sheet,
    mx_area* a,
    mx_paragraph* p)
{
    mx_text_area *ta, *dest_ta;
    mx_sheet* dest;
    mx_sheet* source;
    mx_paragraph* temp;

    int n;
    bool b;

    dest = doc->sheet(err, dest_sheet);
    MX_ERROR_CHECK(err);

    source = doc->sheet(err, source_sheet);
    MX_ERROR_CHECK(err);

    ta = (mx_text_area*)a;

    ta->remove(err, p);
    MX_ERROR_CHECK(err);

    if (dest->is_top_area_text()) {
        dest_ta = (mx_text_area*)dest->get_top_area(err);
        MX_ERROR_CHECK(err);
    } else {
        dest_ta = dest->add_text_area_to_top(err);
        MX_ERROR_CHECK(err);

        temp = dest_ta->remove(err, 0);
        MX_ERROR_CHECK(err);

        delete temp;
    }

    dest_ta->insert(err, 0, p);
    MX_ERROR_CHECK(err);

    dest_ta->recalculate_outline(err, b);
    MX_ERROR_CHECK(err);

    n = ta->num_paragraphs(err);
    MX_ERROR_CHECK(err);

    if (n == 0) {
        source->delete_text_area_ref(err, ta);
        MX_ERROR_CHECK(err);
    } else {
        ta->recalculate_outline(err, b);
        MX_ERROR_CHECK(err);
    }

abort:;
}

void mx_wp_repaginator::move_area(
    int& err,
    int dest_sheet,
    int source_sheet,
    mx_area* a)
{
    // if dest == source don't need to do anything

    if (dest_sheet < source_sheet) {
        move_area_into_sheet_bottom(
            err,
            dest_sheet,
            source_sheet,
            a);
        MX_ERROR_CHECK(err);
    } else {
        if (dest_sheet > source_sheet) {
            move_area_into_sheet_top(
                err,
                dest_sheet,
                source_sheet,
                a);
            MX_ERROR_CHECK(err);
        }
    }
abort:;
}

void mx_wp_repaginator::move_area_into_sheet_top(
    int& err,
    int dest_sheet,
    int source_sheet,
    mx_area* a)
{
    mx_sheet* dest;
    mx_sheet* source;
    mx_area* area_below;
    int na;

    dest = doc->sheet(err, dest_sheet);
    MX_ERROR_CHECK(err);

    source = doc->sheet(err, source_sheet);
    MX_ERROR_CHECK(err);

    source->move_area_out(err, a, dest);
    MX_ERROR_CHECK(err);

    if (a->rtti_class() == mx_table_area_class_e) {
        na = dest->get_num_areas();

        if (na > 1) {
            area_below = dest->get_area(err, 1);
            MX_ERROR_CHECK(err);

            if (area_below->rtti_class() == mx_table_area_class_e) {
                rejoin_table_areas(err, dest, 0, 1);
                MX_ERROR_CHECK(err);
            }
        }
    }

abort:;
}

void mx_wp_repaginator::move_area_into_sheet_bottom(
    int& err,
    int dest_sheet,
    int source_sheet,
    mx_area* a)
{
    mx_sheet* dest;
    mx_sheet* source;
    int na;
    mx_area* area_above;

    dest = doc->sheet(err, dest_sheet);
    MX_ERROR_CHECK(err);

    source = doc->sheet(err, source_sheet);
    MX_ERROR_CHECK(err);

    dest->move_area_in(err, a, source);
    MX_ERROR_CHECK(err);

    if (a->rtti_class() == mx_table_area_class_e) {
        na = dest->get_num_areas();

        if (na > 1) {
            area_above = dest->get_area(err, na - 2);
            MX_ERROR_CHECK(err);

            if (area_above->rtti_class() == mx_table_area_class_e) {
                rejoin_table_areas(err, dest, na - 2, na - 1);
                MX_ERROR_CHECK(err);
            }
        }
    }

abort:;
}

void mx_wp_repaginator::rejoin_things(int& err)
{
    mx_sheet* s;
    mx_area* a;

    for (auto si : m_sheets_to_reposition) {
        s = doc->sheet(err, si.first);
        MX_ERROR_CHECK(err);

        for (int i = 0; i < s->get_num_areas(); i++) {
            a = s->get_area(err, i);
            MX_ERROR_CHECK(err);

            if (a->rtti_class() == mx_text_area_class_e) {
                ((mx_text_area*)a)->rejoin_logical_paras(err);
                MX_ERROR_CHECK(err);
            }
        }
    }

abort:;
}

void mx_wp_repaginator::add_area(int& err, int sheet_num, mx_sheet* s, mx_area* a)
{
    mx_repagination_item::mx_constraint_e c;
    mx_repagination_item* item;

    mx_table_area::mx_table_type_t type;

    if (a->rtti_class() == mx_table_area_class_e) {
        type = ((mx_table_area*)a)->get_type(err);
        MX_ERROR_CHECK(err);

        switch (type) {
        default:
        case mx_table_area::mx_table_start_e:
        case mx_table_area::mx_table_whole_e:
            c = get_constraint(err, a);
            MX_ERROR_CHECK(err);

            item = new mx_repagination_item(c);

            item->add_area(err, sheet_num, s, a);
            MX_ERROR_CHECK(err);

            items->append(item);
            break;

        case mx_table_area::mx_table_middle_e:
        case mx_table_area::mx_table_end_e:
            item = (mx_repagination_item*)items->last();

            item->add_area(err, sheet_num, s, a);
            MX_ERROR_CHECK(err);

            break;
        }
    } else {
        c = get_constraint(err, a);
        MX_ERROR_CHECK(err);

        item = new mx_repagination_item(c);

        item->add_area(err, sheet_num, s, a);
        MX_ERROR_CHECK(err);

        items->append(item);
    }
abort:;
}

mx_repagination_item::mx_constraint_e mx_wp_repaginator::get_constraint(int& err, mx_area* a)
{
    return get_constraint(a->get_area_style());
}

mx_repagination_item::mx_constraint_e mx_wp_repaginator::get_constraint(mx_paragraph_style* s)
{
    // page break before takes precedence over all others
    if (s->page_break_before) {
        return mx_repagination_item::mx_break_before_e;
    } else {
        // keep together is next most important
        if (s->keep_lines_together) {
            return mx_repagination_item::mx_keep_together_e;
        } else {
            if (s->keep_with_next) {
                return mx_repagination_item::mx_keep_with_next_e;
            } else {
                return mx_repagination_item::mx_none_e;
            }
        }
    }
}

mx_repagination_item::mx_constraint_e mx_wp_repaginator::get_constraint(int& err, mx_paragraph* p)
{
    return get_constraint(p->get_paragraph_style());
}

void mx_wp_repaginator::add_paragraph(
    int& err,
    int sheet_num,
    mx_sheet* s,
    mx_area* a,
    mx_paragraph* p)
{
    mx_repagination_item::mx_constraint_e c;
    mx_repagination_item* item;

    // if this is part of a longer paragraph (and is not the
    // first bit), get the last thing added and add it in
    // to that.
    switch (p->get_type()) {
    default:
    case mx_paragraph::mx_paragraph_start_e:
    case mx_paragraph::mx_paragraph_whole_e:
        c = get_constraint(err, p);
        MX_ERROR_CHECK(err);

        item = new mx_repagination_item(c);

        item->add_para(err, sheet_num, s, (mx_text_area*)a, p);
        MX_ERROR_CHECK(err);

        items->append(item);
        break;

    case mx_paragraph::mx_paragraph_middle_e:
    case mx_paragraph::mx_paragraph_end_e:
        item = (mx_repagination_item*)items->last();

        item->add_para(err, sheet_num, s, (mx_text_area*)a, p);
        MX_ERROR_CHECK(err);

        break;
    }
abort:;
}

void mx_wp_repaginator::print_debug()
{
    mx_list_iterator iter(*items);
    mx_list_iterator iter2(*breaks);
    mx_repagination_item* item;
    mx_break_item_t* br;
    int i = 0;

    fprintf(stderr, "========== items ============================================\n");
    while (iter.more()) {
        item = (mx_repagination_item*)iter.data();
        fprintf(stderr, "--------item %d\n", i++);
        item->print_debug();
    }

    fprintf(stderr, "========== breaks ============================================\n");
    i = 0;
    while (iter2.more()) {
        br = (mx_break_item_t*)iter2.data();
        fprintf(stderr, "--------break %d  item = %d  bit = %d  line/row = %d\n",
            i++,
            get_item_num(br),
            br->bit_num,
            br->line_or_row_index);
    }
}

void mx_wp_repaginator::find_paragraph_break_pos(
    int& err,
    int item_index,
    float height,
    float& height_so_far,
    mx_repagination_item* item)
{
    int bit_num = 0, line_num = 0;
    bool found_break;
    float rh;

    mx_break_item_t* last_break;

    do {
        // find break position starting from the given bit and line
        found_break = find_break_pos_from(
            err,
            item,
            bit_num,
            line_num,
            height,
            height_so_far);
        MX_ERROR_CHECK(err);

        if (found_break) {
            // check if last break was at same position
            last_break = (mx_break_item_t*)breaks->last();

            if (last_break == NULL) {
                if (item_index == 0 && bit_num == 0 && line_num == 0) {
                    // every sheet must have something on it
                    add_sheet_break(err, item_index, bit_num, ++line_num);
                    MX_ERROR_CHECK(err);
                } else {
                    add_sheet_break(err, item_index, bit_num, line_num);
                    MX_ERROR_CHECK(err);
                }
            } else {
                if (last_break->bit_num == bit_num && last_break->line_or_row_index == line_num && last_break->item == item) {
                    // every sheet must have something on it
                    add_sheet_break(err, item_index, bit_num, ++line_num);
                    MX_ERROR_CHECK(err);
                } else {
                    add_sheet_break(err, item_index, bit_num, line_num);
                    MX_ERROR_CHECK(err);
                }
            }

            height_so_far = 0.0;
        }

        rh = remaining_height(err, item, bit_num, line_num);
        MX_ERROR_CHECK(err);
    } while (rh > height);

    height_so_far = rh;

abort:;
}

bool mx_wp_repaginator::find_break_pos_from(
    int& err,
    mx_repagination_item* item,
    int& bit_num,
    int& line_num,
    float height,
    float height_so_far)
{
    float height_within_bit, h;
    mx_paragraph* p;

    while (bit_num < item->get_num_bits()) {
        p = item->get_para(err, bit_num);
        MX_ERROR_CHECK(err);

        h = p->get_height();

        // how far down within the bit are we already?
        height_within_bit = p->get_height_from_line(err, line_num);
        MX_ERROR_CHECK(err);

        if (((h - height_within_bit) + height_so_far) > height) {
            // it's in this bit
            line_num = p->get_break_line_for_height(
                err,
                (height - height_so_far) + height_within_bit);
            MX_ERROR_CHECK(err);

            return TRUE;
        } else {
            // break must be in next bits somewhere
            height_so_far += h - height_within_bit;
            line_num = 0;
            bit_num++;
        }
    }

abort:
    return FALSE;
}

float mx_wp_repaginator::remaining_height(
    int& err,
    mx_repagination_item* item,
    int bit_num,
    int line_num)
{
    int i, nb;
    float res = 0.0;

    mx_paragraph* p;

    p = item->get_para(err, bit_num);
    MX_ERROR_CHECK(err);

    res = p->get_height() - p->get_height_from_line(err, line_num);
    MX_ERROR_CHECK(err);

    nb = item->get_num_bits();

    for (i = bit_num + 1; i < nb; i++) {
        p = item->get_para(err, i);
        MX_ERROR_CHECK(err);

        res += p->get_height();
        MX_ERROR_CHECK(err);
    }

abort:
    return res;
}

float mx_wp_repaginator::keep_with_next_block_size(
    int& err,
    int item_index,
    int& first_item,
    int& last_item)
{
    int i;
    float res = 0.0;

    mx_repagination_item* item;

    if (item_index > 0) {
        i = item_index - 1;

        item = (mx_repagination_item*)items->get(err, i);
        MX_ERROR_CHECK(err);

        if (item->get_constraint() == mx_repagination_item::mx_keep_with_next_e) {
            // previous item is part of block, so there may be more
            while (i >= 0) {
                item = (mx_repagination_item*)items->get(err, i);
                MX_ERROR_CHECK(err);

                if (item->get_constraint() == mx_repagination_item::mx_keep_with_next_e) {
                    i--;
                } else {
                    break;
                }
            }
            first_item = i + 1;
        } else {
            first_item = item_index;
        }
    } else {
        first_item = 0;
    }

    // go forwards until first non keep with next
    i = item_index + 1;

    while (i < items->get_num_items()) {
        item = (mx_repagination_item*)items->get(err, i);
        MX_ERROR_CHECK(err);

        if (item->get_constraint() != mx_repagination_item::mx_keep_with_next_e) {
            break;
        }
        i++;
    }

    last_item = i - 1;

    for (i = first_item; i <= last_item; i++) {
        item = (mx_repagination_item*)items->get(err, i);
        MX_ERROR_CHECK(err);

        res += item->get_height();
    }

    return res;

abort:
    return 0.0;
}

void mx_wp_repaginator::find_table_break_pos(
    int& err,
    int item_index,
    float height,
    float& height_so_far,
    mx_repagination_item* item)
{
    int bit_num = 0, line_num = 0;
    bool found_break;
    float rh;

    mx_break_item_t* last_break;

    do {
        // find break position starting from the given bit and line
        found_break = find_table_break_pos_from(
            err,
            item,
            bit_num,
            line_num,
            height,
            height_so_far);
        MX_ERROR_CHECK(err);

        if (found_break) {
            // check if last break was at same position
            last_break = (mx_break_item_t*)breaks->last();

            if (last_break == NULL) {
                if (bit_num == 0 && line_num == 0 && item_index == 0) {
                    // every sheet must have something on it
                    add_sheet_break(err, item_index, bit_num, ++line_num);
                    MX_ERROR_CHECK(err);
                } else {
                    add_sheet_break(err, item_index, bit_num, line_num);
                    MX_ERROR_CHECK(err);
                }
            } else {
                if (last_break->bit_num == bit_num && last_break->line_or_row_index == line_num && last_break->item == item) {
                    // every sheet must have something on it
                    add_sheet_break(err, item_index, bit_num, ++line_num);
                    MX_ERROR_CHECK(err);
                } else {
                    add_sheet_break(err, item_index, bit_num, line_num);
                    MX_ERROR_CHECK(err);
                }
            }

            height_so_far = 0.0;
        }

        rh = remaining_table_height(err, item, bit_num, line_num);
        MX_ERROR_CHECK(err);
    } while (rh > height);

    height_so_far = rh;

abort:;
}

void mx_wp_repaginator::break_item(
    int& err,
    int item_index,
    float height,
    float& height_so_far,
    mx_repagination_item* item)
{
    bool b;

    mx_area* a;

    b = item->is_para(err);
    MX_ERROR_CHECK(err);

    if (b) {
        find_paragraph_break_pos(err, item_index, height, height_so_far, item);
        MX_ERROR_CHECK(err);
    } else {
        a = item->get_area(err, 0);
        MX_ERROR_CHECK(err);

        if (a->rtti_class() == mx_table_area_class_e) {
            find_table_break_pos(err, item_index, height, height_so_far, item);
            MX_ERROR_CHECK(err);
        } else {
            // can't split it, so leave as it is
            height_so_far = 0.0;
        }
    }
abort:;
}

void mx_wp_repaginator::split_keep_together_block(
    int& err,
    int first_item,
    int last_item,
    float height,
    float& height_so_far)
{
    mx_repagination_item* item;
    int i;

    for (i = first_item; i <= last_item; i++) {
        item = (mx_repagination_item*)items->get(err, i);
        MX_ERROR_CHECK(err);

        if ((height_so_far + item->get_height()) > height) {
            break_item(err, i, height, height_so_far, item);
            MX_ERROR_CHECK(err);
        } else {
            height_so_far += item->get_height();
        }
    }
abort:;
}

void mx_wp_repaginator::split_item_only(
    int& err,
    mx_break_item_t* break_item)
{
    mx_repagination_item* item;
    mx_repagination_item* new_item;

    int bn;

    mx_break_item_t* next_break;

    item = break_item->item;

    new_item = item->split(err, break_item->bit_num);
    MX_ERROR_CHECK(err);

    items->insert(err, get_item_num(break_item) + 1, new_item);
    MX_ERROR_CHECK(err);

    // patch the following break items that point to the thing
    // we've just broken up
    bn = breaks->find_index(err, break_item) + 1;
    MX_ERROR_CHECK(err);

    while (bn < breaks->get_num_items()) {
        next_break = (mx_break_item_t*)breaks->get(err, bn);
        MX_ERROR_CHECK(err);

        if (next_break->item == item) {
            next_break->item = new_item;
            next_break->bit_num -= break_item->bit_num;

            bn++;
        } else {
            break;
        }
    }

    break_item->item = new_item;
    break_item->bit_num = 0;
    break_item->line_or_row_index = 0;

abort:;
}

int mx_wp_repaginator::get_item_num(mx_break_item_t* b)
{
    int err = MX_ERROR_OK;
    int i;

    i = items->find_index(err, b->item);
    MX_ERROR_CHECK(err);

    return i;

abort:
    MX_ERROR_CLEAR(err);
    return -1;
}

bool mx_wp_repaginator::find_table_break_pos_from(
    int& err,
    mx_repagination_item* item,
    int& bit_num,
    int& line_num,
    float height,
    float height_so_far)
{
    float height_within_bit, h;
    mx_table_area* a;

    while (bit_num < item->get_num_bits()) {
        a = (mx_table_area*)item->get_area(err, bit_num);
        MX_ERROR_CHECK(err);

        h = a->get_height(err);
        MX_ERROR_CHECK(err);

        // how far down within the bit are we already?
        height_within_bit = a->get_height_from_row(err, line_num);
        MX_ERROR_CHECK(err);

        if (((h - height_within_bit) + height_so_far) > height) {
            // it's in this bit
            line_num = a->get_break_row_for_height(
                err,
                (height - height_so_far) + height_within_bit);
            MX_ERROR_CHECK(err);

            return TRUE;
        } else {
            // break must be in next bits somewhere
            height_so_far += h - height_within_bit;
            line_num = 0;
            bit_num++;
        }
    }

abort:
    return FALSE;
}

float mx_wp_repaginator::remaining_table_height(
    int& err,
    mx_repagination_item* item,
    int bit_num,
    int line_num)
{
    int i, nb;
    float res = 0.0;

    mx_table_area* a;

    a = (mx_table_area*)item->get_area(err, bit_num);
    MX_ERROR_CHECK(err);

    res = a->get_height(err);
    MX_ERROR_CHECK(err);

    res -= a->get_height_from_row(err, line_num);
    MX_ERROR_CHECK(err);

    nb = item->get_num_bits();

    for (i = bit_num + 1; i < nb; i++) {
        a = (mx_table_area*)item->get_area(err, i);
        MX_ERROR_CHECK(err);

        res += a->get_height(err);
        MX_ERROR_CHECK(err);
    }

abort:
    return res;
}

void mx_wp_repaginator::rejoin_table_areas(int& err, mx_sheet* s, int i, int j)
{
    mx_table_area *top, *bottom;
    mx_table_area::mx_table_type_t top_type, bottom_type;
    int nc1, nc2;
    float w1, w2;

    top = (mx_table_area*)s->get_area(err, i);
    MX_ERROR_CHECK(err);

    bottom = (mx_table_area*)s->get_area(err, j);
    MX_ERROR_CHECK(err);

    top_type = top->get_type(err);
    MX_ERROR_CHECK(err);

    bottom_type = bottom->get_type(err);
    MX_ERROR_CHECK(err);

    if (top_type == mx_table_area::mx_table_start_e || top_type == mx_table_area::mx_table_middle_e) {
        MX_ERROR_ASSERT(err, bottom_type == mx_table_area::mx_table_end_e || bottom_type == mx_table_area::mx_table_middle_e);
    } else {
        return;
    }

    nc1 = top->get_num_columns(err);
    MX_ERROR_CHECK(err);

    nc2 = bottom->get_num_columns(err);
    MX_ERROR_CHECK(err);

    if (nc1 == nc2) {
        for (i = 0; i < nc1; i++) {
            w1 = top->get_column_width(err, i);
            MX_ERROR_CHECK(err);

            w2 = bottom->get_column_width(err, i);
            MX_ERROR_CHECK(err);

            if (!MX_FLOAT_EQ(w1, w2)) {
                break;
            }
        }

        if (i == nc1) {
            // same number of columns and all the same width

            s->remove_area(err, j);
            MX_ERROR_CHECK(err);

            top->join(err, bottom);
            MX_ERROR_CHECK(err);
        }
    }

abort:;
}
