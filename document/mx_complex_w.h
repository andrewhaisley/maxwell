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
 * MODULE/CLASS :  mx_complex_word
 *
 * AUTHOR : Andrew Haisley
 *
 *
 *
 * DESCRIPTION:
 *
 * Words with character style modifications.
 *
 *
 *
 *
 */
#ifndef MX_COMPLEX_WORD_H
#define MX_COMPLEX_WORD_H

#include <mx_list.h>
#include <mx_word.h>

// forward definitions
class mx_cw_t;
class mx_char_style_mod;
class mx_field_word;
class mx_area;
class mx_simple_word;

class mx_complex_word : public mx_word {
    MX_RTTI(mx_complex_word_class_e)

public:
    mx_complex_word();
    mx_complex_word(const char* string);
    mx_complex_word(mx_complex_word& o);
    virtual ~mx_complex_word();

    virtual mx_word* duplicate() { return new mx_complex_word(*this); };

    // the width according to the underlying outline font(s).
    virtual float width();
    virtual float width(int index);

    // the height according to the underlying character style(s)
    virtual void metrics(
        int& err,
        float& spacing,
        float& ascender,
        float& descender);

    // get the style at the end of the word
    virtual void get_end_style(mx_char_style& s);

    // get the style at given character index
    void get_index_style(int& err, int index, mx_char_style& s);

    // get the word as a C string
    virtual const char* cstring();

    virtual void serialise(int& err, uint8** buffer);
    virtual void unserialise(int& err, uint8** buffer, uint32 docid);
    virtual uint32 get_serialised_size(int& err);

    // get the complex word item at the given index
    mx_cw_t* get_item(int& err, int32 i);

    // get the number of items in the complex word
    uint32 get_num_items();

    // append another word
    mx_complex_word& operator+=(mx_simple_word& w);
    mx_complex_word& operator+=(mx_complex_word& w);

    // append a character
    mx_complex_word& operator+=(char c);

    // append a string
    mx_complex_word& operator+=(char* s);

    // append a field word
    mx_complex_word& operator+=(mx_field_word& fw);

    // append an image or diagram area
    mx_complex_word& operator+=(mx_area& na);

    // append a style change - the change is copied so you can delete your
    // copy when finished with it
    mx_complex_word& operator+=(mx_char_style_mod& s);

    // insert a character to the left of the given character index - note
    // that character indexes are just that (i.e. they ignore style changes)
    void insert(int& err, char c, int index = 0);
    void insert(int& err, char* s, int index = 0);

    // insert a style change to the left of the given character index - note
    // that character indexes are just that (i.e. they ignore style changes)
    void insert(int& err, mx_char_style_mod& s, int index = 0);

    // insert/append a complex word item into the paragraph. The item is NOT
    // copied, so do not delete it.
    void insert(int& err, mx_cw_t* item, int index = 0);

    void append(mx_cw_t* item);

    // remove a range of characters
    virtual mx_cw_t* remove(int& err, int start, int end);

    // split the word at a given index returning a new word.
    virtual mx_word* split(int& err, int index);

    // split the word at a given width
    virtual mx_word* split(int& err, float w);

    // joins the other word to the end of this one and deletes the other one
    void join(int& err, mx_complex_word* w);

    // cut a bit of a word out
    virtual mx_word* cut(int& err, int start, int end);

    // returns TRUE if this word can be part of a long word broken over a
    // number of lines
    virtual bool can_be_part_of_long_word();

    // is the word actually simple - no style changes that is?
    bool is_simple();

    virtual mx_word* move_last_in_start(int& err, int n, mx_word* w);
    virtual mx_word* move_first_in_end(int& err, int n, mx_word* w);
    virtual void move_style_mod_out_end(int& err, mx_word* w);

    // has the word got any character style mods in it?
    virtual bool has_style_mod();

    // get the style mod at a given character index. If there is none, then
    // return NULL, but not an error. Return errors if the index is out of range
    virtual mx_char_style_mod* get_style_mod(int& err, int character_index);

    virtual bool is_empty();

    // how many visible items in total
    virtual int get_num_visible_items();

    // sets the page number
    virtual void set_page_number(uint32 n);

    // move this word to another document. Moves any underlying database
    // objects to the new document database
    virtual void move_to_document(int& err, mx_document* doc);

    // removes unecessary mods from this word
    virtual void minimise_mods();

    // looks through style modifications to see if they have the
    // revert_to_default flag set, and if they do then change the mod to
    // revert to the new base style
    virtual void replace_base_style(int& err, mx_char_style& cs);

    // goes through a word and sets the revert_to_default flag on any style
    // mods which modify the style to the one given (cs)
    virtual void set_style_mod_revert_flags(int& err, mx_char_style& cs);

    // get the visible character index of an item
    int get_visible_index(int& err, int i);

    // get the item index from a visible character index
    int get_char_index(int& err, int index);

private:
    mx_list items;

    char* word_pointer;
};

#endif
