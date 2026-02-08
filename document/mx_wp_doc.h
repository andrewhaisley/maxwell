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

#ifndef MX_WP_DOCUMENT_H
#define MX_WP_DOCUMENT_H

#include "mx_doc.h"
#include <mx.h>
#include <mx_sh_lay.h>

#define MX_WP_DOCUMENT_IS_HEADER "bhead"
#define MX_WP_DOCUMENT_IS_FOOTER "bfoot"
#define MX_WP_DOCUMENT_HF_SHEET "hfsid"

// maximum header/footer size as a proportion of a sheet size
#define MX_WP_DOC_MAX_HEADER (1.0 / 3.0)
#define MX_WP_DOC_MAX_FOOTER (1.0 / 3.0)

class mx_wp_document : public mx_document {
    friend class mx_wp_repaginator;

    MX_RTTI(mx_wp_document_class_e)

public:
    // as normal but, in the case of a new document, sets up some
    // default contents
    mx_wp_document(const char* file, uint32 docid, const char* page_size = "A4");

    // does things like flowing over sheet boundaries etc
    void reformat(int& err, mx_wp_doc_pos& start, mx_wp_doc_pos& end);

    // same but form the start of the sheet containing the start position
    void reformat_from_start_of_sheet(int& err, mx_wp_doc_pos& start, mx_wp_doc_pos& end);

    // reformats the whole document - after a global style change for example
    void reformat(int& err);

    virtual void add_default_styles(int& err);

    // get the size of the visisble portion of a sheet. This is
    // peculiar to wp docs where there are bits (headers & footers)
    // that don't always get displayed but are still there.
    mx_sheet_size get_sheet_visible_size(int& err, int i, mx_painter_mode_t mode);

    // get the text area used for the header & footer - returns NULL
    // if there are non
    mx_text_area* header(int& err);
    mx_text_area* footer(int& err);

    // set the header and footer
    void set_header(int& err, mx_text_area* a);
    void set_footer(int& err, mx_text_area* a);

    // clear the header and footer
    void clear_header(int& err);
    void clear_footer(int& err);

    // delete the thing to the left of the postion. The position
    // is left pointing to the left of whatever got deleted
    mx_cw_t* delete_to_left(int& err, mx_wp_doc_pos* p, bool& deleted_paragraph);

    // remove the area at the given position - rejoins surrounding areas
    // is necessary
    void remove_area(int& err, mx_wp_doc_pos* p);

    // get the style at a position. This is not a copy so don't delete or
    // modify it.
    mx_paragraph_style* get_para_style(int& err, mx_wp_doc_pos* p);

    // insert a character. may be space or tab as well as printing
    // character
    void insert_char(int& err, char c, mx_wp_doc_pos& p);

    // insert a complex word item. The item is NOT copied, so do not delete it.
    void insert_cw_item(int& err, mx_cw_t*, mx_wp_doc_pos& p);

    // insert a line break
    void insert_line_break(int& err, mx_wp_doc_pos& p);

    // insert a paragraph break - split the paragraph at the given
    // position
    void insert_para_break(int& err, mx_wp_doc_pos& p);

    // insert a pre-existing paragraph after the para at the given position.
    void insert_next_para(int& err, mx_wp_doc_pos& pos, mx_paragraph* p);

    // returns the character style at the given position - statically allocated
    mx_char_style* get_char_style(int& err, const mx_wp_doc_pos& pos);

    // join the paragraph at the position with the next one. throws an error
    // if the next paragraph cannot be joined
    void join_para_with_next(int& err, mx_wp_doc_pos& p);

    // join the table at the position with the next one. throws an error
    // if the next table cannot be joined or if the position is not in a
    // table area
    void join_table_with_next(int& err, mx_wp_doc_pos& p);

    // join the thing at the current position if it has been split by
    // repagination. rejoin_split_text_container() merely calls one of the two
    // following methods as appropriate
    void rejoin_split_text_container(int& err, mx_wp_doc_pos& p);
    void rejoin_split_paragraph(int& err, mx_wp_doc_pos& pos);
    void rejoin_split_table(int& err, mx_wp_doc_pos& pos);

    // split the document at the given position. If the position is in
    // the middle of a text area, this creates a new text are otherwise
    // does nothing. returns the index of the area created or just of the
    // old area.
    int split(int& err, mx_wp_doc_pos& p);

    // the really nasty bit...... sort out paragraph constraints, page and area
    // sizes etc and make it all fit
    void repaginate(int& err, int start_sheet);

    // set the highlight region - used by redraw
    void set_highlight_range(mx_wp_doc_pos& start, mx_wp_doc_pos& end);

    // clear the highlight
    void clear_highlight();

    // set/get the page size for all sheets in the document
    void set_page_size(int& err, float width, float height, bool is_landscape);
    void get_page_size(int& err, float& width, float& height, bool& is_landscape);

    // set/get the margins of all pages in the document
    void set_page_margins(int& err, float left, float right, float top, float bottom);
    void get_page_margins(int& err, float& left, float& right, float& top, float& bottom);

    // reformat whole sheet
    void reformat_sheet(int& err, int sn, bool& height_changed);

    // insert an string of text
    void insert_text(int& err, mx_wp_doc_pos& pos, const char* string);

    // Get word at given position
    mx_word* get_word(int& err, const mx_wp_doc_pos& pos);

    // get the "logical" word at given position. This means if the word at the
    // given position has been split across lines/pages, it will be joined up
    // and returned whole.. The word returned is only guaranteed to be valid
    // until the next document access, since it may be allocated
    // statically. the other two positions are set to be the start and end of
    // the logical word respectively.
    mx_word* get_logical_word(int& err,
        const mx_wp_doc_pos& pos,
        mx_wp_doc_pos& start_word,
        mx_wp_doc_pos& end_word);

    // similar to get_logical_word(), but it limits the output word to be within
    // the positions passed in the start_limit, end_limit parameters
    mx_word* get_logical_word_with_limits(int& err,
        const mx_wp_doc_pos& pos,
        mx_wp_doc_pos& start_limit,
        mx_wp_doc_pos& end_limit);

private:
    // reposition the areas on the sheets. This is here (rather than in the
    // sheet class) as the way this happens is tightly bound to how a WP
    // works. A DTP package for example would want to do this very differently
    void reposition_sheet_areas(int& err, int sn);

    // reformat a bit of a single sheet - comments as for last routine
    void reformat_sheet(
        int& err,
        int sn,
        mx_wp_sheet_pos& start_pos,
        mx_wp_sheet_pos& end_pos,
        bool& height_changed);

    // this just calls the reformat method on the area - in a separate routine as it has
    // to do a bit of casting
    void reformat_area(int& err,
        mx_area_pos* start_pos,
        mx_area_pos* end_pos,
        mx_area* a,
        float width,
        bool& height_changed);

    void reposition_sheet_area(
        int& err,
        mx_area* a,
        float left,
        float right,
        float top,
        float width,
        float& y_pos);

    float get_area_x_pos(mx_area* a,
        float area_width,
        float width,
        float left,
        float right);

    mx_sheet* get_header_footer_sheet(int& err);
    bool is_header(int& err);
    bool is_footer(int& err);

    void set_is_header(int& err, bool b);
    void set_is_footer(int& err, bool b);

    void join_sheets(int& err, int sn1, int sn2);

    virtual void replace_style(int& err, mx_paragraph_style* style_to_replace,
        mx_paragraph_style* replacement_style);
};
#endif
