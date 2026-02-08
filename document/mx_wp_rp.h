#pragma once

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

#include <map>

#include <mx.h>
#include <mx_rtti.h>
#include <mx_wp_rp_item.h>

class mx_area;
class mx_text_area;
class mx_paragraph;
class mx_paragraph_style;
class mx_list;
class mx_wp_document;

typedef struct
{
    mx_repagination_item* item;

    int bit_num;
    int line_or_row_index;
} mx_break_item_t;

class mx_wp_repaginator // : public mx_rtti
{
    // MX_RTTI(mx_wp_repaginator_class_e)

public:
    mx_wp_repaginator(int& err, mx_wp_document* doc, int start_sheet);
    virtual ~mx_wp_repaginator();

    void find_sheet_breaks(int& err);
    int get_num_required_sheets();
    void move_things(int& err);
    void rejoin_things(int& err);

    void print_debug();

private:
    mx_list* items;
    mx_list* breaks;
    std::map<int, mx_sheet *> m_sheets_to_reposition;
    mx_wp_document* doc;

    int start_sheet;

    void add_area(int& err, int sheet_num, mx_sheet* s, mx_area* a);
    void add_paragraph(
        int& err,
        int sheet_num,
        mx_sheet* s,
        mx_area* a,
        mx_paragraph* p);
    mx_repagination_item::mx_constraint_e get_constraint(int& err, mx_area* a);
    mx_repagination_item::mx_constraint_e get_constraint(int& err, mx_paragraph* p);
    mx_repagination_item::mx_constraint_e get_constraint(mx_paragraph_style* s);

    void add_sheet_break(int& err, int item_index, int bit, int sub_bit);

    void handle_sheet_overflow(
        int& err,
        float height,
        float& height_so_far,
        mx_repagination_item* item,
        int& item_index);

    void find_paragraph_break_pos(
        int& err,
        int item_index,
        float height,
        float& height_so_far,
        mx_repagination_item* item);

    void find_table_break_pos(
        int& err,
        int item_index,
        float height,
        float& height_so_far,
        mx_repagination_item* item);

    float keep_with_next_block_size(
        int& err,
        int item_index,
        int& first_item,
        int& last_item);

    void break_item(
        int& err,
        int item_index,
        float height,
        float& height_so_far,
        mx_repagination_item* item);

    void split_keep_together_block(
        int& err,
        int first_item,
        int last_item,
        float height,
        float& height_so_far);

    void move_item(
        int& err,
        int dest_sheet,
        mx_repagination_item* item);

    void move_paragraph(
        int& err,
        int dest_sheet,
        int source_sheet,
        mx_area* a,
        mx_paragraph* p);

    void move_area(int& err,
        int dest_sheet,
        int source_sheet,
        mx_area* a);

    void move_area_into_sheet_top(
        int& err,
        int dest_sheet,
        int source_sheet,
        mx_area* a);

    void move_area_into_sheet_bottom(
        int& err,
        int dest_sheet,
        int source_sheet,
        mx_area* a);

    void move_paragraph_into_sheet_bottom(
        int& err,
        int dest_sheet,
        int source_sheet,
        mx_area* a,
        mx_paragraph* p);

    void move_paragraph_into_sheet_top(
        int& err,
        int dest_sheet,
        int source_sheet,
        mx_area* a,
        mx_paragraph* p);

    void split_things(int& err);
    void figure_destinations(int& err);
    void split_item(int& err, mx_break_item_t* break_item);
    void split_paragraph(int& err,
        mx_break_item_t* break_item,
        mx_repagination_item* item);
    void split_table_area(int& err,
        mx_break_item_t* break_item,
        mx_repagination_item* item);

    void increment_areas(int& err,
        int item_num,
        int sheet_index,
        int area_index);

    void increment_paragraphs(
        int& err,
        int item_num,
        int sheet_index,
        int area_index);

    void reposition_sheet_areas(int& err);

    void increment_break_item_nums(
        int& err,
        mx_break_item_t* break_item);

    void split_item_only(int& err,
        mx_break_item_t* break_item);

    int get_item_num(mx_break_item_t* item);

    float remaining_height(
        int& err,
        mx_repagination_item* item,
        int bit_num,
        int line_num);

    bool find_break_pos_from(
        int& err,
        mx_repagination_item* item,
        int& bit_num,
        int& line_num,
        float height,
        float height_so_far);

    float remaining_table_height(
        int& err,
        mx_repagination_item* item,
        int bit_num,
        int line_num);

    bool find_table_break_pos_from(
        int& err,
        mx_repagination_item* item,
        int& bit_num,
        int& line_num,
        float height,
        float height_so_far);

    void rejoin_table_areas(int& err, mx_sheet* s, int i, int j);
};

