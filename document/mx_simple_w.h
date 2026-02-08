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
 * MODULE/CLASS :  mx_simple_word
 *
 * AUTHOR : Andrew Haisley
 *
 *
 *
 * DESCRIPTION:
 *
 * Simple words with 8 bit characters and no style changes
 *
 *
 *
 *
 *
 */
#ifndef MX_SIMPLE_WORD_H
#define MX_SIMPLE_WORD_H

#include <mx.h>
#include <mx_rtti.h>
#include <mx_word.h>

class mx_complex_word;

#define MX_SIMPLE_WORD_MAX_STATIC_LENGTH 12

class mx_simple_word : public mx_word {
    MX_RTTI(mx_simple_word_class_e)

    friend class mx_complex_w;

public:
    mx_simple_word(const char* string);
    mx_simple_word(const char* string, int len);
    mx_simple_word(mx_simple_word& s);
    mx_simple_word();
    virtual ~mx_simple_word();

    virtual mx_word* duplicate() { return new mx_simple_word(*this); };

    // the width according to the underlying outline font.
    virtual float width();
    virtual float width(int index);

    // set the word from a C string
    void set(const char* s);
    void set(const char* s, int len);

    mx_simple_word& operator=(mx_simple_word& w)
    {
        set((char*)w.cstring());
        return *this;
    };
    mx_simple_word& operator=(char* s)
    {
        set(s);
        return *this;
    };

    // get the word as a C string
    virtual const char* cstring();

    // get the length
    int length() const;

    // get/set a particular character - can't range check, so be careful
    char& operator[](int i);

    // get character with range check
    char get(int& err, int i);

    // append another word
    mx_simple_word& operator+=(mx_simple_word& w);

    // append a complex word
    mx_complex_word& operator+=(mx_complex_word& w);

    // append a character
    mx_simple_word& operator+=(char c);

    // append a string
    mx_simple_word& operator+=(char* s);

    // insert a character to the left of the given index
    virtual void insert(int& err, char c, int index = 0);

    // insert characters to the left of the given index
    virtual void insert(int& err, char* s, int index = 0);

    // delete a character
    mx_cw_t* remove(int& err, int index = 0);

    // split the word at a given index returning a new word.
    virtual mx_word* split(int& err, int index);

    // split before a given width
    virtual mx_word* split(int& err, float width);

    // cut a bit of a word out
    virtual mx_word* cut(int& err, int start, int end);

    // remove a bit of the word
    virtual mx_cw_t* remove(int& err, int start, int end);

    virtual mx_word* move_last_in_start(int& err, int n, mx_word* w);
    virtual mx_word* move_first_in_end(int& err, int n, mx_word* w);

    virtual void serialise(int& err, uint8** buffer);
    virtual void unserialise(int& err, uint8** buffer, uint32 docid);
    virtual uint32 get_serialised_size(int& err);

    virtual bool is_empty();

    virtual int get_num_visible_items();

    // returns TRUE if this word can be part of a long word broken over a
    // number of lines
    virtual bool can_be_part_of_long_word();

private:
    char word_array[MX_SIMPLE_WORD_MAX_STATIC_LENGTH + 1];
    char* word_pointer;

    void init();

    int word_length;
    int buffer_length;
};

#endif
