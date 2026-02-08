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
#ifndef MX_PARA_STYLE_H
#define MX_PARA_STYLE_H
/*
 * MODULE/CLASS : mx_paragraph_style & mx_paragraph_style_mod
 *
 * AUTHOR : Andrew Haisley/Tom Newton
 *
 *
 *
 * DESCRIPTION:
 *
 * Styles for paragraphs. These are the same as named styles
 *
 *
 *
 */
#include <mx_db.h>

#include "mx_bd_style.h"
#include "mx_char_style.h"
#include "mx_fill_style.h"
#include "mx_ruler.h"
#include <mx_doc_coord.h>
#include <mx_doc_rect.h>
#include <mx_font.h>
#include <mx_list.h>
#include <mx_spline.h>

class mx_paragraph_style;
class mx_paragraph_style_mod;

#define MX_MAX_PARA_STYLES MX_MAX_CHAR_STYLES

enum mx_justification_t {
    mx_left_aligned,
    mx_right_aligned,
    mx_justified,
    mx_centred
};

#define MX_PARA_STYLE_MAX_NAME 100

class mx_paragraph_style : public mx_serialisable_object {
    MX_RTTI(mx_paragraph_style_class_e)

    friend class mx_paragraph_style_mod;

public:
    mx_justification_t justification;

    bool keep_lines_together;
    bool keep_with_next;
    float space_before; /* mm */
    float space_after; /* mm */
    bool is_heading; /* heading style? */
    bool is_toc; /* table of contents style? */
    uint8 heading_level;
    bool page_break_before;
    float line_spacing;

    mx_paragraph_style();
    virtual ~mx_paragraph_style();

    mx_paragraph_style(const mx_paragraph_style& para);

    mx_paragraph_style& operator=(const mx_paragraph_style& para);
    friend bool operator==(const mx_paragraph_style& s1, const mx_paragraph_style& s2);
    friend bool operator!=(const mx_paragraph_style& s1, const mx_paragraph_style& s2);

    mx_char_style* get_char_style() const { return (mx_char_style*)&default_char_style; };
    mx_border_style* get_border_style() const { return (mx_border_style*)&default_border_style; };
    mx_ruler* get_ruler() const { return (mx_ruler*)&ruler; };
    char* get_name() const { return (char*)name; };
    char* get_next_style_name() const { return next_style_name; };

    void set_next_style_name(const char* name);
    void set_char_style(const mx_char_style& st);
    void set_ruler(const mx_ruler& rl);
    void set_name(const char* nm);
    void set_border_style(const mx_border_style& st);

    uint32 get_serialised_size(int& err);
    void serialise(int& err, uint8** buffer);
    void unserialise(int& err, uint8** buffer);

    // apply a paragraph style modification
    void operator+=(const mx_paragraph_style_mod& m);

private:
    mx_char_style default_char_style;
    mx_border_style default_border_style;
    mx_ruler ruler;
    char name[MX_PARA_STYLE_MAX_NAME];
    char* next_style_name;
};

// represents the changes that can be made to a paragraph style
class mx_paragraph_style_mod : public mx_serialisable_object {
    friend class mx_paragraph_style;

    MX_RTTI(mx_paragraph_style_mod_class_e)

public:
    mx_paragraph_style_mod();
    virtual ~mx_paragraph_style_mod();
    mx_paragraph_style_mod(const mx_paragraph_style& s1, const mx_paragraph_style& s2);
    mx_paragraph_style_mod(const mx_paragraph_style_mod& s);

    // set all mods to TRUE from the given paragraph style
    mx_paragraph_style_mod(const mx_paragraph_style& ps);

    mx_paragraph_style_mod& operator=(const mx_paragraph_style_mod& other);
    mx_paragraph_style_mod& operator+=(const mx_paragraph_style_mod& other);

    void serialise(int& err, uint8** buffer);
    void unserialise(int& err, uint8** buffer);
    uint32 get_serialised_size(int& err);

    // does this mod modify the given paragraph style?
    bool modifies(const mx_paragraph_style& ps) const;

    // is this style mod a null mod
    bool is_null() const;

    // clears any mod flags in this mod which are set in the other_mod
    void clear_mods_in(const mx_paragraph_style_mod& other_mod);

    // get/set the sub mods

    mx_char_style_mod* get_char_style_mod() const;
    mx_border_style_mod* get_border_style_mod() const;

    void set_char_style_mod(const mx_char_style_mod& csm);
    void set_border_style_mod(const mx_border_style_mod& bsm);

    // get/set/clear indivdual mods

    bool get_justification_mod(mx_justification_t& new_just) const;
    bool get_keep_lines_together_mod(bool& new_klt) const;
    bool get_keep_with_next_mod(bool& new_kwn) const;
    bool get_space_before_mod(float& new_sb) const;
    bool get_space_after_mod(float& new_sa) const;
    bool get_is_heading_mod(bool& new_ih) const;
    bool get_is_toc_mod(bool& new_it) const;
    bool get_heading_level_mod(uint8& new_hl) const;
    bool get_page_break_before_mod(bool& new_pbb) const;
    bool get_line_spacing_mod(float& new_ls) const;
    bool get_ruler_mod(mx_ruler& new_ruler) const;

    void set_justification_mod(mx_justification_t new_just);
    void set_keep_lines_together_mod(bool new_klt);
    void set_keep_with_next_mod(bool new_kwn);
    void set_space_before_mod(float new_sb);
    void set_space_after_mod(float new_sa);
    void set_is_heading_mod(bool new_ih);
    void set_is_toc_mod(bool new_it);
    void set_heading_level_mod(uint8 new_hl);
    void set_page_break_before_mod(bool new_pbb);
    void set_line_spacing_mod(float new_ls);
    void set_ruler_mod(const mx_ruler& new_ruler);

    void clear_justification_mod();
    void clear_keep_lines_together_mod();
    void clear_keep_with_next_mod();
    void clear_space_before_mod();
    void clear_space_after_mod();
    void clear_is_heading_mod();
    void clear_is_toc_mod();
    void clear_heading_level_mod();
    void clear_page_break_before_mod();
    void clear_line_spacing_mod();
    void clear_ruler_mod();

private:
    bool justification_mod : 1;
    bool keep_lines_together_mod : 1;
    bool keep_with_next_mod : 1;
    bool space_before_mod : 1;
    bool space_after_mod : 1;
    bool is_heading_mod : 1;
    bool is_toc_mod : 1;
    bool heading_level_mod : 1;
    bool page_break_before_mod : 1;
    bool line_spacing_mod : 1;
    bool ruler_mod : 1;

    bool keep_lines_together : 1;
    bool keep_with_next : 1;
    bool is_heading : 1;
    bool is_toc : 1;
    bool page_break_before : 1;
    mx_justification_t justification;
    float space_before;
    float space_after;
    uint8 heading_level;
    float line_spacing;
    mx_ruler ruler;

    mx_char_style_mod char_style_mod;
    mx_border_style_mod border_style_mod;
};

#endif // MX_PARA_STYLE_H
