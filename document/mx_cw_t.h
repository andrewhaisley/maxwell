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
#ifndef MX_CW_T_H
#define MX_CW_T_H
/*
 * MODULE/CLASS :  mx_cw_t
 *
 * AUTHOR : Tom Newton
 *
 *
 *
 * DESCRIPTION:
 *
 * Items of different sorts in a complex word.
 *
 *
 *
 *
 */
#include "mx_field_w.h"
#include <mx.h>
#include <mx_char_style.h>
#include <mx_sl_obj.h>

// used by the undo stuff only
#define MX_CW_T_PARA_BREAK_CHAR 10
#define MX_CW_T_LINE_BREAK_CHAR 11
#define MX_CW_T_TAB_CHAR 12
#define MX_CW_T_SPACE_CHAR 13

// forward definitions
class mx_area;

class mx_cw_t : public mx_serialisable_object {
public:
    virtual mx_cw_t* deep_copy() = 0;

    // modify the given character style if this item is a style mod. By default
    // this does nothing
    virtual void mod_style(mx_char_style& s);

    // get the width of this item with a given character style.
    virtual float width(const mx_char_style& s) = 0;

    // get the spacing/ascender/descender of this item with a given character
    // style. By default returns value determined by the style
    virtual float spacing(const mx_char_style& s);
    virtual float ascender(const mx_char_style& s);
    virtual float descender(const mx_char_style& s);

    // returns TRUE if the size of this thing potentially changes the line
    // height of the complex word that contains these items. Returns FALSE by
    // default and TRUE for area items and style change items
    virtual bool line_difference();

    // move this word item to another document. Moves any underlying database
    // objects to the new document database
    virtual void move_to_document(int& err, mx_document* doc) { };

    // sets this items page number
    virtual void set_page_number(uint32 num) { };

    virtual void serialise(int& err, uint8** buffer) = 0;
    virtual void unserialise(int& err, uint8** buffer, uint32 docid) = 0;
    virtual uint32 get_serialised_size(int& err) = 0;
};

class mx_character_cw_t : public mx_cw_t {
    MX_RTTI(mx_character_cw_t_class_e);

public:
    mx_character_cw_t(char nc = ' ');
    virtual mx_cw_t* deep_copy();

    // get the width of this item with a given character style.
    virtual float width(const mx_char_style& s);

    virtual void serialise(int& err, uint8** buffer);
    virtual void unserialise(int& err, uint8** buffer, uint32 docid);
    virtual uint32 get_serialised_size(int& err);

    char c;
};

class mx_style_mod_cw_t : public mx_cw_t {
    MX_RTTI(mx_style_mod_cw_t_class_e);

public:
    mx_style_mod_cw_t() { };
    mx_style_mod_cw_t(const mx_char_style_mod& nm);
    virtual mx_cw_t* deep_copy();

    // this modifies the character style
    virtual void mod_style(mx_char_style& s);

    virtual bool line_difference();

    // get the width of this item with a given character style.
    // this actually modifies the character style
    virtual float width(const mx_char_style& s);

    virtual void serialise(int& err, uint8** buffer);
    virtual void unserialise(int& err, uint8** buffer, uint32 docid);
    virtual uint32 get_serialised_size(int& err);

    mx_char_style_mod m;
};

class mx_field_word_cw_t : public mx_cw_t {
    MX_RTTI(mx_field_word_cw_t_class_e);

public:
    mx_field_word_cw_t() { };
    mx_field_word_cw_t(const mx_field_word& nw);
    virtual mx_cw_t* deep_copy();

    // get the width of this item with a given character style.
    virtual float width(const mx_char_style& s);

    // sets this items page number
    virtual void set_page_number(uint32 num) { w.set_page_number(num); };

    virtual void serialise(int& err, uint8** buffer);
    virtual void unserialise(int& err, uint8** buffer, uint32 docid);
    virtual uint32 get_serialised_size(int& err);

    mx_field_word w;
};

class mx_area_cw_t : public mx_cw_t {
    MX_RTTI(mx_area_cw_t_class_e);

public:
    // simply uses the area pointer
    mx_area_cw_t(mx_area* na = NULL);

    // destructor
    ~mx_area_cw_t();

    // get the width of this item with a given character style.
    virtual float width(const mx_char_style& s);

    // get the height/ascender/descender of this item with a given character
    // style. By default returns value determined by the style
    virtual float height(const mx_char_style& s);
    virtual float ascender(const mx_char_style& s);
    virtual float descender(const mx_char_style& s);
    virtual bool line_difference();

    virtual mx_cw_t* deep_copy();
    virtual void move_to_document(int& err, mx_document* doc);

    virtual void serialise(int& err, uint8** buffer);
    virtual void unserialise(int& err, uint8** buffer, uint32 docid);
    virtual uint32 get_serialised_size(int& err);

    mx_area* a;
};

#endif // MX_CW_T_H
