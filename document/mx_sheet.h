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
 * MODULE/CLASS : mx_sheet
 *
 * AUTHOR : Andrew Haisley
 *
 *
 *
 * DESCRIPTION:
 *
 * Sheets are containment objects for areas.
 *
 *
 *
 */
#ifndef MX_SHEET_H
#define MX_SHEET_H

#include <mx.h>
#include <mx_db.h>

#include <mx_dg_area.h>
#include <mx_doc_coord.h>
#include <mx_doc_rect.h>
#include <mx_image_area.h>
#include <mx_paintable.h>
#include <mx_painter.h>
#include <mx_spline.h>
#include <mx_table_area.h>
#include <mx_text_area.h>
#include <mx_wp_sh_pos.h>

class mx_document;

#define MX_SHEET_WIDTH "width"
#define MX_SHEET_HEIGHT "height"

#define MX_SHEET_IMAGE_AREAS "ia"
#define MX_SHEET_TEXT_AREAS "ta"
#define MX_SHEET_DIAGRAM_AREAS "da"
#define MX_SHEET_TABLE_AREAS "tba"

#define MX_SHEET_LEFT_MARGIN "lmargin"
#define MX_SHEET_RIGHT_MARGIN "rmargin"
#define MX_SHEET_TOP_MARGIN "tmargin"
#define MX_SHEET_BOTTOM_MARGIN "bmargin"

#define MX_SHEET_ORIENTATION "orient"

class mx_sheet : public mx_paintable {
    MX_RTTI(mx_sheet_class_e)

    friend class mx_sheet_painter;

public:
    // constructor
    mx_sheet(uint32 docid);

    // constructor for when the oid is known
    mx_sheet(uint32 docid, uint32 oid);

    ~mx_sheet();

    int get_num_text_areas(int& err);
    int get_num_table_areas(int& err);
    int get_num_image_areas(int& err);
    int get_num_diagram_areas(int& err);

    // get a single area by index
    mx_text_area* text_area(int& err, int index);
    mx_table_area* table_area(int& err, int index);
    mx_image_area* image_area(int& err, int index);
    mx_diagram_area* diagram_area(int& err, int index);

    // delete a single area by index
    void delete_text_area(int& err, int index);
    void delete_table_area(int& err, int index);
    void delete_image_area(int& err, int index);
    void delete_diagram_area(int& err, int index);

    // remove a single area by index
    void remove_text_area(int& err, int index);
    void remove_table_area(int& err, int index);
    void remove_image_area(int& err, int index);
    void remove_diagram_area(int& err, int index);

    // delete a single area by object
    void delete_text_area_ref(int& err, mx_text_area* o);
    void delete_table_area_ref(int& err, mx_table_area* o);
    void delete_image_area_ref(int& err, mx_image_area* o);
    void delete_diagram_area_ref(int& err, mx_diagram_area* o);

    // remove - just unlinks, doesn't delete
    void remove_text_area_ref(int& err, mx_text_area* o);
    void remove_table_area_ref(int& err, mx_table_area* o);
    void remove_image_area_ref(int& err, mx_image_area* o);
    void remove_diagram_area_ref(int& err, mx_diagram_area* o);

    // methods for adding all the different area types
    mx_text_area* add_text_area(int& err, int index_position = -1);
    mx_table_area* add_table_area(int& err, int index_position = -1);
    mx_image_area* add_image_area(int& err, int index_position = -1);
    mx_diagram_area* add_diagram_area(int& err, int index_position = -1);

    // methods for moving areas between sheets
    void move_text_area(int& err, mx_text_area* area, mx_sheet* dest);
    void move_table_area(int& err, mx_table_area* area, mx_sheet* dest);
    void move_image_area(int& err, mx_image_area* area, mx_sheet* dest);
    void move_diagram_area(int& err, mx_diagram_area* area, mx_sheet* dest);

    // moves the given area (of whatever type) into the start of another sheet
    void move_area_out(int& err, int area_index, mx_sheet* dest);
    void move_area_out(int& err, mx_area* a, mx_sheet* dest);

    // moves the given area (of whatever type) from another sheet into the
    // the end of this one
    void move_area_in(int& err, int area_index, mx_sheet* src);
    void move_area_in(int& err, mx_area* a, mx_sheet* src);

    // some pre-defined attributes. These are just simple
    // wrappers around the normal attribute access
    // mechanism (see mx_db_object)

    float get_width(int& err);
    void set_width(int& err, float w);
    float get_height(int& err);
    void set_height(int& err, float h);

    // get width minus margins
    float get_width_minus_margins(int& err);

    // get height minus margins
    float get_height_minus_margins(int& err);

    // get the width and height of the visible component in different paint
    // modes.
    float get_height(int& err, mx_painter_mode_t mode);
    float get_width(int& err, mx_painter_mode_t mode);

    void delete_areas(int& err);

    mx_list* get_area_index(int& err);
    mx_area* get_area(int& err, int i);
    int get_num_areas();
    int get_index_position(int& err, mx_area* a);

    mx_area* get_bottom_area(int& err);
    mx_area* get_top_area(int& err);

    mx_text_area* add_text_area_to_top(int& err);
    mx_text_area* add_text_area_to_bottom(int& err);

    bool is_bottom_area_text();
    bool is_top_area_text();

    // set/get  the paper margins
    void set_margins(int& err, float left, float right, float top, float bottom);
    void get_margins(int& err, float& left, float& right, float& top, float& bottom);

    // get the owning document
    mx_document* get_document(int& err);

    // unlink the area from the various indexes and stuff, but don't delete it
    mx_area* remove_area(int& err, int index);

    // set/unset landcape mode (default is portrait)
    void set_landscape(int& err, bool b);
    bool get_landscape(int& err);

    // insert a pre-existing area. This adds it to the area index at the
    // specified index and also to the appropriate area type list
    void add_area(int& err, mx_area* a, int i);

    // if possible, joins the areas at index i and index i + 1. If it cannot
    // join areas because they are incompatible, then the routine returns
    // without error. If an error occurs it is returned in the usual way.
    void join_areas(int& err, int i);

    void highlight_from_start_of_sheet(int& err, mx_wp_sheet_pos& end);
    void highlight_whole_sheet(int& err);
    void highlight_in_sheet(int& err, mx_wp_sheet_pos& start, mx_wp_sheet_pos& end);
    void highlight_to_end_of_sheet(int& err, mx_wp_sheet_pos& start);

private:
    // the areas stored as described below
    mx_list* areas;
    bool index_built;

    void move_area(int& err, mx_area* area, mx_sheet* dest, const char* attr_name);

    // builds an index of areas on position. The format is a list of lists
    // if areas. Each item in the first list is further down the page and
    // each item in the nested list is further across
    void build_area_index(int& err);

    void add_area_to_index(int& err, mx_area* a);

    int num_image_areas;
    int num_text_areas;
    int num_table_areas;
    int num_diagram_areas;
};

#endif
