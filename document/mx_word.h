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
#ifndef MX_WORD_H
#define MX_WORD_H
/*
 * MODULE/CLASS : mx_word
 *
 * AUTHOR : Andrew Haisley
 *
 *
 *
 * DESCRIPTION:
 *
 * Words are things in paragraphs. They have some very simple required
 * methods but everything else is likely to change per derived class.
 *
 * mx_word
 *   mx_simple_word
 *   mx_complex_word
 *   mx_space_word    - hard spaces and tabs
 *   mx_break_word    - line break & page break
 *
 *
 *
 */

// #include <mx.h>
// #include <mx_db.h>
// #include <mx_style.h>

#include <mx_sl_obj.h>

// forward definitions
class mx_char_style;
class mx_char_style_mod;
class mx_ruler;
class mx_document;
class mx_cw_t;

class mx_word : public mx_serialisable_object {
    MX_RTTI(mx_word_class_e)

public:
    virtual void print_debug();

    mx_word();
    mx_word(mx_char_style* s);
    virtual ~mx_word();

    // no copy of the style is kept, so it must not be deleted
    void set_style(mx_char_style* s);

    // get a pointer to the word's style
    mx_char_style* get_style();

    // the width according to the underlying outline font.
    virtual float width() { return 0.0; };
    virtual float width(mx_char_style& s) { return 0.0; };
    virtual float width(int index) { return 0.0; };

    // the width up to a character index
    // virtual float width(int index);

    // calculate the style at the end of the word by adding in any
    // embedded style changes to the style given
    virtual void get_end_style(mx_char_style& s);

    // get the style at given character index
    virtual void get_index_style(int& err, int index, mx_char_style& s);

    // a string representation of thw word
    virtual const char* cstring();

    // the metrics according to the underlying character style(s)
    // the style may be modified as a result of this call.
    virtual void metrics(
        int& err,
        float& spacing,
        float& ascender,
        float& descender);

    virtual void serialise(int& err, uint8** buffer) = 0;
    virtual void unserialise(int& err, uint8** buffer, uint32 docid) = 0;
    virtual uint32 get_serialised_size(int& err) = 0;

    // calculated and used by owning object reformat routine
    float x;
    float y;

    // virtual function for moving the x position of a word by a given
    // (positive or negative) offset. Overridden by subclasses if necessary
    virtual void move_x(float x_offset);

    // things for reformatting etc
    virtual mx_word* duplicate() = 0;

    // move the last N visible items out of another word into the start
    // of this one. You have to use the pointer returned to replace the
    // word you were using as this operation can change the class of
    // the word
    virtual mx_word* move_last_in_start(int& err, int n, mx_word* w);

    // move the first N visible items from another word into the end
    // of this word. You have to use the pointer returned to replace the
    // word you were using as this operation can change the class of
    // the word.
    virtual mx_word* move_first_in_end(int& err, int n, mx_word* w);

    // insert a style change to the left of the given character index - throw
    // an error if word cannot have a style mod, or if index is out of range
    virtual void insert(int& err, mx_char_style_mod& s, int index = 0);

    virtual void insert(int& err, char c, int index = 0) { };
    virtual void insert(int& err, char* s, int index = 0) { };

    // if the last item in this word is a style mod, move it to the
    // beginning of the second word. If the second word cannot have a
    // style mod, throw an error
    virtual void move_style_mod_out_end(int& err, mx_word* w);

    // has the word got any character style mods in it?
    virtual bool has_style_mod();

    // get the style mod at a given character index. If there is none, then
    // return NULL, but not an error. Return errors if the index is out of
    // range
    virtual mx_char_style_mod* get_style_mod(int& err, int character_index);

    // is there anything in the word?
    virtual bool is_empty();

    // split the word at a given index
    virtual mx_word* split(int& err, int index);

    // cut a bit of a word out
    virtual mx_word* cut(int& err, int start, int end);

    // remove a bit of a word - return is NULL except when
    // end == start + 1
    virtual mx_cw_t* remove(int& err, int start, int end);

    // split the word at a given width - returns NULL if the word cannot
    // be made to split that small
    virtual mx_word* split(int& err, float width);

    // how may space consuming items does the word have
    virtual int get_num_visible_items();

    // returns TRUE if this word results in something being printed
    virtual bool is_printable() { return TRUE; };

    // returns TRUE if this word can be part of a long word broken over a
    // number of lines
    virtual bool can_be_part_of_long_word();

    // sets the page number on words which need to know about it
    // virtual void set_page_number(uint32 n){};

    // move this word to another document. Moves any underlying database
    // objects to the new document database
    virtual void move_to_document(int& err, mx_document* doc) { };

    // removes unecessary mods from this word
    virtual void minimise_mods() { };

    // looks through style modifications to see if they have the
    // revert_to_default flag set, and if they do then change the mod to
    // revert to the new base style
    virtual void replace_base_style(int& err, mx_char_style& cs) { };

    // goes through a word and sets the revert_to_default flag on any style
    // mods which modify the style to the one given (cs)
    virtual void set_style_mod_revert_flags(int& err, mx_char_style& cs) { };

    // sets a variable which determines whether operations on words apply to
    // the left of style mods, or to the right (obviously only applies to
    // words which can have style mods
    static void set_operations_to_left_of_mods();
    static void set_operations_to_right_of_mods();

    // are we performing operations to left/right of mods?
    static bool get_operations_to_left_of_mods();
    static bool get_operations_to_right_of_mods();

protected:
    mx_char_style* style;
    static bool operations_to_left_of_mods;
};

#endif
