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
 * MODULE/CLASS :  mx_break_word
 *
 * AUTHOR : Andrew Haisley
 *
 *
 *
 * DESCRIPTION:
 *
 * Words that represent breaks and similar things. Current types
 * are:
 *   paragraph break
 *   long word (new line due to very long word breaking in two)
 *   line break (hard carriage return)
 *   page break (forced new page)
 *
 *
 *
 *
 *
 */
#ifndef MX_BREAK_WORD_H
#define MX_BREAK_WORD_H

#include "mx_word.h"
#include <mx.h>
#include <mx_char_style.h>
#include <mx_rtti.h>

enum mx_break_word_type_t {
    mx_paragraph_break_e,
    mx_line_break_e,
    mx_long_word_break_e
};

class mx_break_word : public mx_word {
    MX_RTTI(mx_break_word_class_e)

public:
    mx_break_word();
    mx_break_word(mx_break_word_type_t t);

    mx_break_word(const mx_break_word& s);
    virtual ~mx_break_word();

    mx_break_word_type_t type();

    virtual void serialise(int& err, uint8** buffer);
    virtual void unserialise(int& err, uint8** buffer, uint32 docid);
    virtual uint32 get_serialised_size(int& err);

    virtual mx_word* duplicate() { return new mx_break_word(*this); };

    // a string representation of thw word
    virtual const char* cstring();

    // the metrics according to the underlying character style(s)
    // the style may be modified as a result of this call.
    virtual void metrics(
        int& err,
        float& spacing,
        float& ascender,
        float& descender);

    virtual bool is_empty() { return FALSE; };
    virtual bool is_printable() { return FALSE; };

    // returns TRUE if this word can be part of a long word broken over a
    // number of lines
    virtual bool can_be_part_of_long_word();

    // insert a style change to the left of the given character index - throw
    // an error if word cannot have a style mod, or if index is out of range
    virtual void insert(int& err, mx_char_style_mod& s, int index = 0);

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

    // removes unecessary mods from this word
    virtual void minimise_mods();

    // looks through style modifications to see if they have the
    // revert_to_default flag set, and if they do then change the mod to
    // revert to the new base style
    virtual void replace_base_style(int& err, mx_char_style& cs);

    // goes through a word and sets the revert_to_default flag on any style
    // mods which modify the style to the one given (cs)
    virtual void set_style_mod_revert_flags(int& err, mx_char_style& cs);

    // split the word at a given index
    virtual mx_word* split(int& err, int index);

    // get the style at the end of the word
    virtual void get_end_style(mx_char_style& s);

    // get the style at given character index
    virtual void get_index_style(int& err, int index, mx_char_style& s);

private:
    mx_char_style_mod mod;
    mx_break_word_type_t word_type;
};

#endif
