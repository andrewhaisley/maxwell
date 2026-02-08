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
#ifndef MX_CHAR_STYLE_H
#define MX_CHAR_STYLE_H
/*
 * MODULE/CLASS : mx_char_style & mx_char_style_mod
 *
 * AUTHOR : Andrew Haisley
 *
 *
 *
 * DESCRIPTION:
 *
 * Styles for characters
 *
 *
 *
 */

#include <mx_db.h>

#include <mx_colour.h>
#include <mx_doc_coord.h>
#include <mx_doc_rect.h>
#include <mx_font.h>
#include <mx_font_mod.h>
#include <mx_list.h>
#include <mx_spline.h>

#define MX_MAX_CHAR_STYLES 100

typedef enum {
    mx_no_effects,
    mx_underline,
} mx_char_effects_t;

typedef enum {
    mx_normal_align,
    mx_superscript,
    mx_subscript,
} mx_align_t;

#define MX_CHAR_STYLE_DEFAULT_COLOUR_VAL 0

class mx_char_style;
class mx_char_style_mod;

class mx_char_style : public mx_serialisable_object {
    MX_RTTI(mx_char_style_class_e)

    friend class mx_char_style_mod;

public:
    // First of all public variables - this isn't a proper class...

    int8 set_width_adjust; // % (0->no adjust)
    mx_colour colour;
    mx_char_effects_t effects;
    mx_align_t alignment;

    // alignment offset in points
    float alignment_offset;

    // line space in points added to point size of font.
    float line_space;

    // Now public functions..

    mx_char_style();
    mx_char_style(mx_font& new_font);
    virtual ~mx_char_style();

    // apply a character style modification
    void operator+=(const mx_char_style_mod& m);

    mx_char_style(const mx_char_style& style);
    mx_char_style& operator=(const mx_char_style& style);

    friend bool operator==(const mx_char_style& s1, const mx_char_style& s2);
    friend bool operator!=(const mx_char_style& s1, const mx_char_style& s2);

    // We control access to the font
    mx_font* get_font() { return &font; };
    void set_font(mx_font& new_font);

    void serialise(int& err, uint8** buffer);
    void unserialise(int& err, uint8** buffer);
    uint32 get_serialised_size(int& err);

    void set_bold(bool flag);
    void set_italic(bool flag);
    bool get_bold();
    bool get_italic();

    // ascender for characters in this style
    float get_ascender() const;

    // descender for characters in this style
    float get_descender() const;

    // width of a character rendered in this style - in mm not points - includes
    // set width adjust
    float width(char c) const;

    // width of a word rendered in this style - in mm not points - includes
    // set width adjust
    float width(const char* s) const;
    float nwidth(const char* s, uint32 n) const;

private:
    mx_font font;
    void copy_it(const mx_char_style& style);
};

// represents the changes that can be made to a character style
class mx_char_style_mod : public mx_serialisable_object {
    friend class mx_char_style;

    MX_RTTI(mx_char_style_mod_class_e)

public:
    mx_char_style_mod();
    virtual ~mx_char_style_mod();
    mx_char_style_mod(const mx_char_style& s1, const mx_char_style& s2);
    mx_char_style_mod(const mx_char_style_mod& s);

    // set all mods to TRUE
    mx_char_style_mod(const mx_char_style& s);

    mx_char_style_mod& operator=(const mx_char_style_mod& other);

    // combine with another style mod
    void operator+=(const mx_char_style_mod& other);

    void serialise(int& err, uint8** buffer);
    void unserialise(int& err, uint8** buffer);
    uint32 get_serialised_size(int& err);

    // returns TRUE if this mod would modify the given character style
    bool modifies(const mx_char_style& cs) const;

    // returns TRUE if this mod does not change anything
    bool is_null() const;

    // sets the revert to default flag for any part of the mod which is
    // changing to the given default paragraph style
    void set_revert_to_default_flags(const mx_char_style& default_para_cs);

    // changes any mods with a revert flag set so that result of the
    // modification is the new default character style
    void set_new_default_cs(const mx_char_style& default_para_cs);

    void clear_revert_to_default_flags();

    // clears any mod flags in this mod which are set in the other_mod
    void clear_mods_in(const mx_char_style_mod& other_mod);

    // returns a pointer to the actual thing
    mx_font_mod* get_font_mod() const;

    // get/set/clear individual mods
    bool get_set_width_adjust_mod(int8& new_swa) const;
    bool get_colour_mod(mx_colour& new_colour) const;
    bool get_effects_mod(mx_char_effects_t& new_effects) const;
    bool get_alignment_mod(mx_align_t& new_alignment) const;
    bool get_alignment_offset_mod(float& new_offset) const;
    bool get_line_space_mod(float& new_line_space) const;

    void set_set_width_adjust_mod(int8 new_swa);
    void set_colour_mod(const mx_colour& new_colour);
    void set_effects_mod(mx_char_effects_t new_effects);
    void set_alignment_mod(mx_align_t new_alignment);
    void set_alignment_offset_mod(float new_offset);
    void set_line_space_mod(float new_line_space);

    void clear_set_width_adjust_mod();
    void clear_colour_mod();
    void clear_effects_mod();
    void clear_alignment_mod();
    void clear_alignment_offset_mod();
    void clear_line_space_mod();

private:
    // mod values

    bool set_width_adjust_mod : 1;
    bool colour_mod : 1;
    bool effects_mod : 1;
    bool alignment_mod : 1;
    bool alignment_offset_mod : 1;
    bool line_space_mod : 1;

    // revert to default flags

    bool set_width_adjust_mod_rev : 1;
    bool colour_mod_rev : 1;
    bool effects_mod_rev : 1;
    bool alignment_mod_rev : 1;
    bool alignment_offset_mod_rev : 1;
    bool line_space_mod_rev : 1;

    int8 set_width_adjust;
    mx_colour colour;
    mx_char_effects_t effects;
    mx_align_t alignment;
    float alignment_offset;
    float line_space;

    mx_font_mod font_mod;
};

#endif // MX_CHAR_STYLE_H
