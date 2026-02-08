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
#ifndef MX_SPACE_WORD_H
#define MX_SPACE_WORD_H
/*
 * MODULE/CLASS :  mx_space_word
 *
 * AUTHOR : Andrew Haisley
 *
 *
 *
 * DESCRIPTION:
 *
 * Words containg spaces, tabs & character style modifications.
 *
 *
 *
 *
 */

#include "mx_word.h"
#include <mx_list.h>

// forward definitions
class mx_char_style;
class mx_char_style_mod;

class mx_space_word : public mx_word {
    MX_RTTI(mx_space_word_class_e)

    enum mx_space_word_item_t {
        mx_space_word_space_e = 75,
        mx_space_word_tab_e,
        mx_space_word_mod_e,
        mx_space_word_end_e
    };

public:
    mx_space_word(mx_space_word& o);
    mx_space_word();
    virtual ~mx_space_word();

    virtual mx_word* duplicate() { return new mx_space_word(*this); };

    virtual void serialise(int& err, uint8** buffer);
    virtual void unserialise(int& err, uint8** buffer, uint32 docid);
    virtual uint32 get_serialised_size(int& err);

    // a string representation of thw word
    virtual const char* cstring();

    // the height according to the underlying character style(s)
    virtual void metrics(int& err, float& spacing, float& ascender,
        float& descender);

    // virtual function for moving the x position of a word by a given
    // (positive or negative) offset.
    virtual void move_x(float x_offset);

    // append another space word
    mx_space_word& operator+=(mx_space_word& w);

    // append a style change - the change is copied so you can delete your
    // copy when finished with it
    mx_space_word& operator+=(mx_char_style_mod& s);

    // get the style at the end of the word given the style at the start
    void get_end_style(mx_char_style& s);

    // get the style at given character index
    void get_index_style(int& err, int index, mx_char_style& s);

    // append a tab
    void append_tab();

    // append a space
    void append_space();

    //////////////////////////////////////////////////////////////////////
    // the following functions all work with character indices rather than
    // real indices which include style mod indices
    //////////////////////////////////////////////////////////////////////

    // insert a something to the left of the given character index
    void insert_space(int& err, int index = 0);
    void insert_tab(int& err, int index = 0);
    void insert(int& err, mx_char_style_mod& s, int index = 0);

    // split the word at a given character index returning a new word.
    virtual mx_word* split(int& err, int index);

    // returns the calculated width (after formatting) of the items up to
    // the one at the given character index
    float calculated_width(int index);

    // remove a range
    virtual mx_cw_t* remove(int& err, int start, int end);

    // cut a bit of a word out
    virtual mx_word* cut(int& err, int start, int end);

    virtual void move_style_mod_out_end(int& err, mx_word* w);

    // has the word got any character style mods in it?
    virtual bool has_style_mod();

    // get the style mod at a given character index. If there is none, then
    // return NULL, but not an error. Return errors if the index is out of range
    virtual mx_char_style_mod* get_style_mod(int& err, int character_index);

    virtual bool is_empty();

    virtual bool is_printable() { return FALSE; };

    // removes unecessary mods from this word
    virtual void minimise_mods();

    // looks through style modifications to see if they have the
    // revert_to_default flag set, and if they do then change the mod to
    // revert to the new base style
    virtual void replace_base_style(int& err, mx_char_style& cs);

    // goes through a word and sets the revert_to_default flag on any style
    // mods which modify the style to the one given (cs)
    virtual void set_style_mod_revert_flags(int& err, mx_char_style& cs);

    // get the width of all the spaces (if any) after the last tab in the word
    float width_of_trailing_spaces();

    // how many spaces are there in the word?
    int num_spaces();

    // how many visible items in total
    virtual int get_num_visible_items();

    /////////////////////////////////////////////////////////////////////
    // the following functions all work with real item indices rather than
    // character indices which ignore style mod indices
    /////////////////////////////////////////////////////////////////////

    // these routines only work if there are no tabs in the word
    virtual float width();
    virtual float width(int index);

    // returns the index of the first/last tab in the word, or -1 if the word
    // has no tab spaces
    int get_first_tab_index();
    int get_last_tab_index();

    // how many items in total
    int get_num_items();

    // get the type of a given item
    mx_space_word_item_t item_type(int& err, int index);

    // get/set the position of the given item. Used by the
    // reformatting/painters
    float item_pos(int& err, int index);
    void item_pos(int& err, int index, float new_pos);

    // get the style change at the given index
    mx_char_style_mod* get(int& err, int index);

private:
    int get_char_index(int& err, int index);

    mx_list items;
};

#endif
