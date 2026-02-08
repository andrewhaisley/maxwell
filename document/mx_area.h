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
 * MODULE/CLASS : mx_area
 *
 * AUTHOR : Andrew Haisley
 *
 *
 *
 * DESCRIPTION:
 *
 * Areas have stuff in them......
 *
 *
 *
 *
 */
#ifndef MX_AREA_H
#define MX_AREA_H

#include <geometry.h>
#include <mx_paintable.h>
#include <mx_spline.h>
#include <mx_style.h>

class mx_object_buffer;
class mx_document;
class mx_sheet;
class mx_area_pos;

#define MX_AREA_OUTLINE_X "xs"
#define MX_AREA_OUTLINE_Y "ys"
#define MX_AREA_OUTLINE_FLAG "flags"
#define MX_AREA_ORIGIN_X "ox"
#define MX_AREA_ORIGIN_Y "oy"
#define MX_AREA_ROTATION "rot"
#define MX_AREA_B_COLOUR_R "bcr"
#define MX_AREA_B_COLOUR_G "bcg"
#define MX_AREA_B_COLOUR_B "bcb"
#define MX_AREA_B_COLOUR_NAME "bcn"
#define MX_AREA_F_COLOUR_R "fcr"
#define MX_AREA_F_COLOUR_G "fcg"
#define MX_AREA_F_COLOUR_B "fcb"
#define MX_AREA_F_COLOUR_NAME "fcn"

#define MX_AREA_NEXT_AREA "nextarea"
#define MX_AREA_PREV_AREA "prevarea"

#define MX_AREA_LINKED_AREAS "linkedids"
#define MX_AREA_LINKED_THIS_X "linkedtx"
#define MX_AREA_LINKED_THIS_Y "linkedty"
#define MX_AREA_LINKED_OTHER_X "linkedox"
#define MX_AREA_LINKED_OTHER_Y "linkedoy"

#define MX_AREA_ORIGINAL_SIZE_X "osx"
#define MX_AREA_ORIGINAL_SIZE_Y "osy"
#define MX_AREA_TOP_LEFT_CROP_X "tlx"
#define MX_AREA_TOP_LEFT_CROP_Y "tly"
#define MX_AREA_SCALE_X "sx"
#define MX_AREA_SCALE_Y "sy"

typedef struct
{
    mx_db_object* linked_area;

    // the point in this area (relative to the first point)
    // that is matched with the point in the other
    mx_point thispoint;

    // the other
    mx_point otherpoint;

} mx_area_link_t;

class mx_area : public mx_paintable {
    MX_RTTI(mx_area_class_e)

public:
    // a controlled area is one that is tightly managed by another area.
    // the main effect is that the serialise_attributes method will do
    // nothing for a controlled area as it is assumed that the owner
    // will take responsiblility for serialising the area into a buffer it
    // will managed itself.

    mx_area(uint32 docid, mx_paragraph_style* s, bool is_controlled = FALSE);
    mx_area(uint32 docid, uint32 oid, bool is_controlled = FALSE);

    // a copy constructor is necessary as it used in cut/paste operations
    // where a deep copy is vital.
    // Note that this method creates a new database object.
    mx_area(mx_area& a);

    // this calls the copy constructor, but you don't need to know the
    // derived class to use it
    virtual mx_area* duplicate() = 0;

    // move the object to another document - subclass must make sure that
    // all it's attributes and blobs are in the cache before calling this.
    virtual void move_to_document(int& err, mx_document* doc);

    virtual ~mx_area();

    void delete_links(int& err);

    // is there another area in a chain
    bool next_chained_area(int& err);

    // is there a previous area in a chain
    bool previous_chained_area(int& err);

    // retrieves the next area in the chain
    mx_area* next_in_chain(int& err);

    // retrieves the previous area in the chain
    mx_area* previous_in_chain(int& err);

    // insert into a chain of area - either next or prev may be
    // NULL, but not both
    void insert_into_chain(int& err, mx_area* prev, mx_area* next);

    // remove from the chain
    void remove_from_chain(int& err);

    // number of linked areas
    int number_linked_areas(int& err);

    // linked area by index
    mx_area_link_t* linked_area(int& err, int index);

    // create a link
    void link_to_area(int& err, mx_area* linked_area, mx_point thispoint, mx_point otherpoint);

    // delete a link
    void unlink_from_area(int& err, mx_area* area);

    // some pre-defined attributes. These are just simple
    // wrappers around the normal attribute access
    // mechanism (see mx_db_object)

    const mx_quadspline_t* get_outline_readonly(int& err);
    mx_point get_origin(int& err);
    float get_rotation(int& err);

    void set_origin(int& err, mx_point o);
    void set_xorigin(int& err, float x);
    void set_yorigin(int& err, float y);
    void set_rotation(int& err, float rotation);

    // set a complex outline
    void set_outline(int& err, const mx_quadspline_t& pp);

    // set a rectangle outline
    void set_outline(int& err, const mx_doc_rect_t& r);

    // set an outline just by giving width and height - it is assumed to
    // be right up against the origin
    void set_outline(int& err, float width, float height);

    // will always be an mx_sheet object
    virtual mx_db_object* get_owner_object(int& err);

    // these functions are to do with reformatting

    // some areas can flow their contents into another area of the
    // same class. e.g. tables and text areas. some can't.
    // this has nothing to do with whether the area is actually chained to
    // another area.
    virtual bool is_flowable() const { return FALSE; };

    // some areas are cropable - this means that the user can adjust
    // how much of the contents may be seen
    virtual bool is_croppable() const { return FALSE; };

    // some areas are scalable - this means that the user can adjust
    // the aspect ration and size.
    virtual bool is_scalable() const { return FALSE; };

    // scale the area to the new width and height
    virtual void scale(int& err, mx_point& new_size);

    // crop the area using the rectangle as a template for how far to move each side
    // note that the values cannot be negative -> cropping is not reversible other
    // than by an undo operation.
    virtual void crop(int& err, const mx_rect& r);

    // how high is the area in mm? This is a bounding box height
    float get_height(int& err);

    // how wide is the area in mm? This is a bounding box width
    float get_width(int& err);

    // set the width of the area
    void set_width(int& err, float w);

    // how wide is the area in mm minus the size of the left & right border
    // (if any)
    float width_minus_border(int& err);

    // both together
    mx_rect bounding_box(int& err);

    // in order to do the flowing, the owning document has to be able to
    // chop bits off the end of the area etc.

    // reformat the area - reposition things as needed, recalculate the
    // height etc. doesn't do any flowing or stuff like that, that's the
    // job of the document. Some are types won't respond to this directly as
    // most of the reformatting will be the job of the editor (e.g. diagram
    // areas). It is also possible that an area will provide a different
    // reformatting interface - it is the job of the caller to know about
    // that.
    virtual void reformat(int& err, bool& height_changed);

    // this sets the base style
    void set_paragraph_style(mx_paragraph_style* s);

    // stores the diff between the base style and the supplied style
    void modify_paragraph_style(mx_paragraph_style& new_style);

    // gets the actual style to use for the area
    mx_paragraph_style* get_paragraph_style();

    // gets the base style for the area
    mx_paragraph_style* get_base_paragraph_style() { return base_style; };

    // get the owning sheet
    mx_sheet* get_sheet(int& err);

    // get the owning document
    mx_document* get_document(int& err);

    void serialise(int& err, uint8** buffer);
    void unserialise(int& err, uint8** buffer);
    uint32 get_serialised_size(int& err);

    // if the base style for this area is the same as the style to replace, it
    // gets replaced with the replacement base style. Subclasses should iterate
    // through any subareas or references to document styles to replace them
    // too.
    virtual void replace_style(int& err, mx_paragraph_style* style_to_replace,
        mx_paragraph_style* replacement_style);

    // gets the style at the given position. style is not a copy so should
    // not be freed
    virtual mx_paragraph_style* get_style(int& err, mx_area_pos* p);

    // gets the style for the area as a whole rather than a position within it
    mx_paragraph_style* get_area_style();

    // things to do highlighting
    virtual void highlight_from_start_of_area(int& err, mx_area_pos* end);
    virtual void highlight_whole_area(int& err);
    virtual void highlight_in_area(int& err, mx_area_pos* start, mx_area_pos* end);
    virtual void highlight_to_end_of_area(int& err, mx_area_pos* start);

    // scaling and rectangle cropping
    // here original size and top left are all in frame mm

    void set_original_size(int& err, mx_point& original_size);
    bool get_original_size(int& err, mx_point& original_size);
    void set_top_left_crop(int& err, mx_point& top_left_crop);
    bool get_top_left_crop(int& err, mx_point& top_left_crop);
    void set_scale(int& err, mx_point& p);
    bool get_scale(int& err, mx_point& p);

    void crop_details(int& err,
        mx_point& top_left,
        mx_point& total_size);

    // has this area got a bage break before it
    virtual bool has_page_break_before(int& err);

    // can this area be joined with the other? Returns FALSE by default
    virtual bool can_join(int& err, mx_area* a);

    // join this area with another. Unless this is overridden this method
    // returns a not implemented error. The second area is deleted as part of
    // this process. Throws an error if the areas are incompatible.
    virtual void join(int& err, mx_area* a);

protected:
    mx_paragraph_style* base_style;
    mx_paragraph_style_mod* style_mod;

private:
    mx_quadspline_t outline_cache;
};

#endif
