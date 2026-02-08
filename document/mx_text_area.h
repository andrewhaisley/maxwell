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
#ifndef MX_TEXT_AREA_H
#define MX_TEXT_AREA_H
/*
 * MODULE/CLASS : mx_text_area
 *
 * AUTHOR : Andrew Haisley
 *
 *
 *
 * DESCRIPTION:
 *
 * Areas specifically for holding text
 *
 *
 *
 */

#include <mx_area.h>
#include <mx_complex_w.h>

#define MX_TEXT_AREA_PARAS "paras"
#define MX_TEXT_AREA_NPARAS "nparas"
#define MX_TEXT_AREA_PARA_INDICES "paraindices"

class mx_paragraph;
class mx_list;
class mx_text_area_pos;

class mx_text_area : public mx_area {
    MX_RTTI(mx_text_area_class_e)

public:
    mx_text_area(uint32 docid, mx_paragraph_style* style, bool is_controlled = FALSE);
    mx_text_area(uint32 docid, uint32 oid, bool is_controlled = FALSE);
    mx_text_area(mx_text_area& a);
    virtual ~mx_text_area();

    virtual mx_area* duplicate();

    // clears all the contents leaving an empty area with one paragraph
    void clear_contents(int& err);

    // move into another document
    virtual void move_to_document(int& err, mx_document* doc);

    // document about to be committed  - put attributes into blobs etc
    virtual void serialise_attributes(int& err);

    // not part of the required stuff for a db_object but a sensible extra
    void unserialise_attributes(int& err);

    // free some memory
    virtual void uncache(int& err);
    virtual uint32 memory_size(int& err);

    void serialise(int& err, uint8** buffer);
    void unserialise(int& err, uint8** buffer);
    uint32 get_serialised_size(int& err);

    // text areas are flowable
    virtual bool is_flowable() { return TRUE; };

    virtual mx_object_buffer* force_height(
        int& err,
        float requested,
        float& actual);

    // iterates through paragraphs and replaces any references to the given
    // base style with the new reference. It also calls the superclasses method.
    virtual void replace_style(int& err, mx_paragraph_style* style_to_replace,
        mx_paragraph_style* replacement_style);

    // reformat a given range. This is the smallest range within the area
    // that will be reformatted - changes may filter down as a result of
    // size changes etc.
    virtual void reformat(
        int& err,
        mx_text_area_pos& start,
        mx_text_area_pos& end,
        bool& height_changed);

    // reformat the whole area
    virtual void reformat(
        int& err,
        bool& height_changed);

    int num_paragraphs(int& err);

    float paragraph_y_offset(int& err, int index);

    mx_paragraph* get(int& err, int index);
    mx_paragraph* remove(int& err, int index);
    mx_paragraph* swap(int& err, int index, mx_paragraph* p);
    mx_paragraph* remove(int& err, mx_paragraph* p);
    mx_paragraph* insert_new_paragraph(int& err, mx_text_area_pos& p);
    void insert(int& err, int index, mx_paragraph* p);
    void insert(int& err, mx_paragraph* p, mx_text_area_pos& ap);
    void insert_adjust_position(int& err, mx_paragraph* para, mx_text_area_pos& p);
    void append(int& err, mx_paragraph* p);

    // split at a given position - the next text area created is added
    // to the sheet (and index) in the appropriate postion and a pointer
    // to it is returned
    mx_text_area* split(int& err, mx_text_area_pos& p);

    // can this area be joined with the other?
    virtual bool can_join(int& err, mx_area* a);

    // join this area with another. The second area is deleted as part of this
    // process. Throws an error if the areas are incompatible.
    virtual void join(int& err, mx_area* a);

    // get the full paragraph style at the given position - this is not
    // a copy so you don't have to free it afterwards. The position must
    // be a text area position of course.
    virtual mx_paragraph_style* get_style(int& err, mx_area_pos* p);

    // delete the thing to the left of the position
    mx_cw_t* delete_to_left(
        int& err,
        mx_text_area_pos* p,
        bool& paragraph_deleted);

    // insert a character - may be space or tab as well as printing character
    void insert_char(int& err, char c, mx_text_area_pos* p);

    // insert a complex word item. The item is NOT copied, so do not delete it.
    void insert_cw_item(int& err, mx_cw_t*, mx_text_area_pos* p);

    // insert a line break
    void insert_line_break(int& err, mx_text_area_pos* p);

    // insert a paragraph break
    void insert_para_break(int& err, mx_text_area_pos* p);

    // work out the new outline following a reformat
    void recalculate_outline(int& err, bool& height_changed);

    // find the index of a paragraph
    int get_index(int& err, mx_paragraph* p);

    // rejoin adjacent paragraphs where they are extensions of the
    // same logical one
    void rejoin_logical_paras(int& err);

    // print debug information
    virtual void print_debug();

    virtual mx_db_mem_state_t get_mem_state();
    virtual void set_mem_state(mx_db_mem_state_t s);

    // has this area got a bage break before it
    virtual bool has_page_break_before(int& err);

private:
    mx_list paragraphs;

    void rejoin_from_position(int& err, int& para_index);
};

#endif // MX_TEXT_AREA_H
