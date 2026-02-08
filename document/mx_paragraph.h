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
#ifndef MX_PARAGRAPH_H
#define MX_PARAGRAPH_H
/*
 * MODULE/CLASS : mx_paragraph
 *
 * AUTHOR : Andrew Haisley
 *
 *
 *
 * DESCRIPTION:
 *
 * Paragraphs of text for use in an area. These objects are stored
 * as attributes of areas they belong to (i.e. they do not inherit
 * from mx_db_object). If you modify one of these, the appropriate
 * attributes will be updated for you and committed when you commit
 * the changes to the document.
 *
 *
 *
 *
 */

#include <vector>

#include <mx_nlist.h>
#include <mx_sl_obj.h>
#include <mx_style.h>

class mx_word;
class mx_document;
class mx_char_style_mod;
class mx_para_pos;
class mx_nlist;
class mx_area;
class mx_paragraph_style;
class mx_paragraph_style_mod;
class mx_char_style;
class mx_space_word;
class mx_break_word;
class mx_cw_t;

class mx_paragraph : public mx_serialisable_object {
    MX_RTTI(mx_paragraph_class_e)

    friend class mx_text_area_painter;
    friend class mx_text_buffer;
    friend class mx_para_pos;

    // unfortunately, paragraphs sometimes get split and need to be rejoined
    // later - the type of the paragraph indicates whether the thing is stand
    // alone or part of a bigger paragraph
    enum mx_paragraph_type_t {
        mx_paragraph_start_e = 32,
        mx_paragraph_middle_e,
        mx_paragraph_end_e,
        mx_paragraph_whole_e
    };

public:

    mx_paragraph();
    mx_paragraph(mx_paragraph& para);
    virtual ~mx_paragraph();

    // set the width - this is the space the paragraph sits it - the
    // formatting width is smaller by the size of the left and right
    // indents & any space taken up by the border
    void set_width(float f);

    float get_format_width();

    // retrieve word by line and word index
    mx_word* get_word(int& err, int line_index, int word_index);

    // retrieve word by position - letter index is ignored
    mx_word* get_word(int& err, const mx_para_pos& pp);

    // delete word by line and word index
    void delete_word(int& err, int line_index, int word_index);
    void delete_word(int& err, const mx_para_pos& p);

    // Return a pointer to a word and remove it from the list, but
    // don't delete it.
    mx_word* remove_word(int& err, int line_index, int word_index);
    mx_word* remove_word(int& err, mx_para_pos& p);

    // insert new word by position - when you have finished inserting words, you
    // must call reformat_styles(). This is a bit clunky, but reformat styles
    // is a relatively slow procedure so it makes sense to use it for a whole
    // batch of changes at the same time. If you know that your word didn't
    // contain any style changes, you don't need to call reformat_styles().
    // after insertion, the paragraph position is left pointing at the location
    // into which the next thing to be inserted will go
    void insert_word(int& err, mx_word* a_word, mx_para_pos& p);

    // insert a style mod
    // after insertion, the paragraph position is left pointing at the location
    // into which the next thing to be inserted will go
    void insert_mod(int& err, mx_char_style_mod& m, mx_para_pos& p);

    // insert a character - may be a space/tab as well as non-white
    // space
    void insert_char(int& err, char c, mx_para_pos& p);

    // insert a complex word item into the paragraph. The item is NOT copied,
    // so do not delete it.
    void insert_cw_item(int& err, mx_cw_t*, mx_para_pos& p);

    // insert the contents of the string into the paragraph. If an unknown
    // character (<=13) is found, the method returns, leaving the string
    // pointing to the unknown character.
    void insert_ascii_string(int& err, const char*& string, mx_para_pos& p);

    // append word to end of paragraph, not copied, so don't delete it
    void append_word(int& err, mx_word* a_word);

    // append a style change - copy is made of the mod, so you
    // can deleted# your own afterwards
    void append_mod(int& err, mx_char_style_mod& a_mod);

    void serialise(int& err, uint8** buffer);
    void unserialise(int& err, uint8** buffer);
    uint32 get_serialised_size(int& err);

    int num_lines();
    int num_words(int& err, int line_index);
    uint32 total_num_words();

    // format in a given range - the y positions of things following the
    // range may also be changed. Height changed flag is set if the overall
    // height of the paragraph was changed by the reformat
    // Note that the end position is only really a hint - if changes within
    // the range force following lines to change, it will be ignored. However,
    // reformatting will always proceed at least as far as the end position.
    void reformat(
        int& err,
        mx_para_pos& start,
        mx_para_pos& end,
        bool& height_changed);

    // do the whole paragraph
    void reformat(int& err, bool& height_changed);

    // rebuild the style list and pointers from words - this is needed after
    // a style change has been applied. It is called automatically when you set or
    // modify the base paragraph style
    void reformat_styles(int& err);

    // removes any style mods which do not affect the style for this
    // paragraph style
    void minimise_style_mods(int& err);

    // set the revert_to_default flag on all style mods which indicate a
    // change back to the current default style. Clear the flag on all others.
    void set_style_mod_revert_flags(int& err);

    // change the base paragraph style - a handle is kept to this so it
    // must hang around (it ought to be one of the styles from the document
    // style list)
    void set_paragraph_style(mx_paragraph_style* s);

    // modify the paragraph style. This routine takes the difference between the
    // supplied style and the base style so that changes in the base style
    // are handled correctly
    void modify_paragraph_style(const mx_paragraph_style& new_style);
    void modify_paragraph_style(const mx_paragraph_style_mod& difference);

    // get a pointer to the style actually used by the paragraph
    mx_paragraph_style* get_paragraph_style();

    // get a pointer to the base style used by the paragraph
    mx_paragraph_style* get_base_paragraph_style();

    // get the calculated height for the paragraph - only valid after a
    // reformatting
    float get_height();

    // if the paragraph only contains a pargraph break word this returns TRUE
    bool is_empty(int& err);

    // adjust the y positions of all the lines from and including this one
    void adjust_line_ypositions(int& err, int from_line);

    mx_paragraph_type_t get_type();
    void set_type(mx_paragraph_type_t t);

    void set_document(mx_document* doc);

    // get the style at a given position within the paragraph - this is
    // not a copy, so if you need one you'll have to make it yourself.
    mx_char_style* get_style(int& err, mx_para_pos& pp);

    // get the style mod at a given position within the paragraph - this is
    // not a copy, so if you need one you'll have to make it yourself.
    mx_char_style_mod* get_style_mod(int& err, mx_para_pos& pp);

    // split into two paragraphs at the given position
    mx_paragraph* split(int& err, mx_para_pos& p);

    // split into two paragraphs at the given line index
    mx_paragraph* split(int& err, int line_index);

    // join another paragraph onto the end - the contents of the second
    // paragraph are cleaned up and deleted so there is no need to do it
    // explicitly
    mx_para_pos join(int& err, mx_paragraph* para);

    // join the words at the given location if possible - used after a cut or
    // paste to join bits of words together. It would be neater to do this as
    // part of a reformat, but this is a lot quicker since you know exactly
    // where the join will be after after a cut or paste.
    // returns TRUE if any joining was done, FALSE otherwise
    bool join_words(int& err, mx_para_pos& pp);

    // find the y coordinate of a given line index
    float line_ypos(int& err, int line_index);

    // delete the 'thing' immediately behind the given position. A thing
    // will most likely be a single letter. p will come out set to the
    // appropriate position - deleting from the start gives a position range
    // error
    mx_cw_t* delete_to_left(int& err, mx_para_pos* p);

    // insert a line break
    void insert_line_break(int& err, mx_para_pos& p);

    // get the number of non space words in a given line, from (and including)
    // a given word_index
    int get_num_non_space_words(int& err, int line_index, int word_index = 0);

    // move this paragraph to another document. Moves any underlying database
    // objects to the new document database
    void move_to_document(int& err, mx_document* doc);

    // get the line number to break the paragraph at to make it no greater
    // than the given height. If that is not possible, returns the best it
    // can.
    int get_break_line_for_height(int& err, float f);

    float get_height_from_line(int& err, int line_num);

    // did the height change as a result of the last reformat
    bool get_height_changed();

    // what range of the paragraph was affected by the last reformat, either in
    // terms of the line numbers of the area that those lines cover. These
    // methods are only valid immediately after a reformat
    void reformat_range(int& err, int& upper_line, int& lower_line);
    void reformat_range(int& err, float& upper_pos, float& lower_pos);

    // get the highlight height for a given line in a paragraph
    void highlight_height_for_line(int& err, int line_index,
        float& height, float& descender);
    void metrics_for_line(
        int& err,
        int line_index,
        float& ascender,
        float& descender,
        float& spacing);

    // methods for doing things which do not change whether to insert to
    // the left or right of a mod.

    void aka_insert_space(int& err, mx_para_pos& p);
    void aka_insert_tab(int& err, mx_para_pos& p);
    void aka_insert_char(int& err, char c, mx_para_pos& p);
    void aka_insert_word(int& err, mx_word* a_word, mx_para_pos& p);
    void aka_insert_mod(int& err, mx_char_style_mod& m, mx_para_pos& p);
    void aka_insert_ascii_string(int& err, const char*& string, mx_para_pos& p);
    mx_char_style* aka_get_style(int& err, mx_para_pos& pp);
    mx_char_style_mod* aka_get_style_mod(int& err, mx_para_pos& pp);

    mx_db_mem_state_t get_mem_state();
    void set_mem_state(mx_db_mem_state_t s);

    // if you want to convert an ascii string into words, call this
    // repeatedly. The pointer to the string gets modified to point at the next
    // characters to be processed
    static mx_word* get_word_from_string(int& err, const char*& string);

    // returns TRUE if there is a long word break just before the paragraph
    // break and if the paragraph type is start or middle.
    bool has_split_long_word_at_end(int& err);

    // set the page number for this paragraph. Takes effect on the next reformat
    static void set_page_number(uint32 page_num);

private:
    // owning document
    mx_document* doc;

    // recalculate the height value
    void recalculate_height(int& err);

    // this points to one of the named styles stored on the document
    mx_paragraph_style* style;

    // apply this to the named style to get the actual style to use, if NULL it
    // means that there is no mod
    mx_paragraph_style_mod* style_mod;

    // get the actual paragraph style to use from the base style and the style mod
    mx_paragraph_style& get_style_to_use();

    // lines of words held in an n-list
    mx_nlist words;

    // sets the overflow flag to TRUE iff the effect of reformatting the
    // line will change things in following lines.
    void reformat_line_left_justify(
        int& err,
        int line_index,
        int word_index,
        bool& overflow);

    // used by the centred and right justified routines since they are
    // practically the same, with the exception of the amount of the gap that
    // each word is moved by.
    void reformat_line_justify(int& err, int line_index, int word_index, bool& overflow, float gap_adjust);
    void reformat_line_right_justify(
        int& err,
        int line_index,
        int word_index,
        bool& overflow);
    void reformat_line_centre_justify(
        int& err,
        int line_index,
        int word_index,
        bool& overflow);
    void reformat_line_full_justify(
        int& err,
        int line_index,
        int word_index,
        bool& overflow);

    float left_justify_space_width(
        int& err,
        mx_space_word* word,
        int line_index,
        int word_index);

    void reformat_line_left_justify_overflow(int& err, mx_word* word, int line_index, int word_index);
    bool left_justify_underflow(int& err, int line_index, float x);
    float descender_for_line(int& err, int line_index);
    float ascender_for_line(int& err, int line_index);
    float height_for_line(int& err, int line_index);
    float left_indent_to_text();
    void space_to_centre_dp_tabify(
        int& err,
        float& x,
        float& tab_position,
        int item_index,
        int line_index,
        int word_index);
    void space_to_centre_tabify(
        int& err,
        float& x,
        float& tab_position,
        int item_index,
        int line_index,
        int word_index);
    void space_to_right_tabify(
        int& err,
        float& x,
        float& tab_position,
        int item_index,
        int line_index,
        int word_index);

    float left_justify_tab_width(
        int& err,
        float x,
        mx_space_word* word,
        int item_index,
        int line_index,
        int word_index);

    void rejoin_and_split_long_word(int& err, int line_index, int word_index, bool& overflow);
    void split_long_word(int& err, mx_word* word, int line_index, int word_index);
    void reformat_line_left_justify_break(int& err, mx_break_word* word, int line_index, int word_index, bool& overflow);
    void recalculate_format_width();
    float get_line_start_pos(int& err, int line_index, int word_index);
    float get_left_border_width();

    float figure_ypos_for_line(int& err, int line_index, bool& moved);

    void set_line_ypos(int& err, int line, float pos, bool& moved);

    // saves the positions of words on the given line in the given list
    void save_word_positions(int& err, int line, std::vector<float>& list);

    // compares words positions of the words on a line with those in the list
    // (which is assumed to be generated by save_word_positions())
    bool word_positions_changed(int& err, int line, std::vector<float>& list);

    // sets the reformat range so that it includes the given line
    void set_reformat_range(int& err, int line);

    // split word at position if its in the middle of a word
    void split_word(int& err, mx_para_pos& p);

    // fix any style mods for this paragraph if the base style has changed
    void fix_styles_for_base_style_change(int& err, mx_char_style& new_base_cs);

    // the width of the containing thing (area)
    float width;

    // the width available for reformatting into
    float format_width;

    // calculated by the paragraph itself. Includes any space before
    // or after and the space taken up by the border (if any)
    float height;

    // a list of pointers to character styles - these are pointed to by the style
    // pointers for individual words
    // this list is only ever added to. it is cleared up when either when a
    // style reformat happens or when the paragraph is deleted. This is not
    // entirely memory efficient but it isn't all that bad either.
    mx_list styles;

    mx_paragraph_type_t type;

    void delete_words();
    void delete_styles();

    // returns TRUE if any joining was done, FALSE otherwise
    bool join_words(int& err, const mx_para_pos& p1, const mx_para_pos& p2);

    // makes sure that any style mods at the end of the word at the first
    // position are shifted to the start of the word at the second position. If
    // the first word only contains a mod and is made empty and deleted, the
    // method returns TRUE, else FALSE.
    bool fix_trailing_style_mods(int& err, const mx_para_pos& p1,
        const mx_para_pos& p2);

    // remove all blank lines from this line on
    void delete_blank_lines(int& err, int line_index);

    // delete the end of paragraph break word
    void delete_paragraph_break(int& err);

    // reposition the lines in the y direction
    void reposition_lines(int& err);

    // check for special case of trailing paragraph break after
    // a hard line break
    void check_trailing_para_break(int& err);

    bool height_changed;
    int reformat_upper, reformat_lower;

    mx_db_mem_state_t mem_state;
};

#endif
