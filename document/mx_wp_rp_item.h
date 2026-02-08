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
 * MODULE/CLASS :  mx_wp_repagination item
 *
 * AUTHOR : Andrew Haisley
 *
 *
 *
 * DESCRIPTION:
 *
 *  Repagination items. These are groups of paragraphs or areas that share
 *  contstraints.
 *
 *
 *
 *
 */
#ifndef MX_WP_RP_ITEM_H
#define MX_WP_RP_ITEM_H

#include <mx.h>
#include <mx_rtti.h>

class mx_sheet;
class mx_area;
class mx_text_area;
class mx_paragraph;
class mx_paragraph_style;
class mx_list;
class mx_wp_document;

class mx_repagination_item {
public:
    typedef enum {
        mx_keep_together_e,
        mx_keep_with_next_e,
        mx_none_e,
        mx_break_before_e
    } mx_constraint_e;

    mx_repagination_item(mx_constraint_e c);
    mx_repagination_item(mx_constraint_e c, float h);
    ~mx_repagination_item();

    mx_constraint_e get_constraint() { return constraint; };
    void set_constraint(mx_constraint_e c) { constraint = c; };
    float get_height() { return height; };
    void set_height(float f) { height = f; };

    // these reset or add to the height automatically

    void add_area(
        int& err,
        int sheet_num,
        mx_sheet* s,
        mx_area* a);
    void add_para(
        int& err,
        int sheet_num,
        mx_sheet* s,
        mx_text_area* a,
        mx_paragraph* p);

    // insert a paragraph - does not reset the height as the new paragraph
    // is assumed to be the result of a split
    void insert_para(
        int& err,
        int bit_num,
        int sheet_num,
        mx_sheet* s,
        mx_text_area* a,
        mx_paragraph* p);

    // same for area
    void insert_area(
        int& err,
        int bit_num,
        int sheet_num,
        mx_sheet* s,
        mx_area* a);

    // set/get the destination sheet number
    void set_dest(int n) { dest_sheet = n; };
    int get_dest() { return dest_sheet; };

    // the number of bits in the item
    int get_num_bits();

    // a bit is either a paragraph or an area
    bool is_bit_paragraph(int& err, int i);

    // get area of bit
    mx_area* get_area(int& err, int i);

    // get paragraph of bit
    mx_paragraph* get_para(int& err, int i);

    // get sheet of bit
    mx_sheet* get_sheet(int& err, int i);

    // get sheet index of bit
    int get_sheet_index(int& err, int i);

    void print_debug();

    // is the item overall a paragraph?
    bool is_para(int& err);

    mx_rtti_class_e get_area_class(int& err);

    // split into two
    mx_repagination_item* split(int& err, int i);

private:
    void recalculate_height(int& err);

    mx_constraint_e constraint;
    float height;
    mx_list* bits;
    int dest_sheet;
};

#endif
