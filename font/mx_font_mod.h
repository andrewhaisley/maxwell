#pragma once

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

#include <mx.h>
#include <mx_db.h>

#include <mx_list.h>
#include <mx_sl_obj.h>

class mx_font_mod : public mx_serialisable_object {
    friend class mx_font;

    MX_RTTI(mx_font_mod_class_e)

public:
    mx_font_mod();
    virtual ~mx_font_mod();
    mx_font_mod(const mx_font& s1, const mx_font& s2);
    mx_font_mod(const mx_font_mod& s);

    // set all mods to TRUE
    mx_font_mod(const mx_font& s);

    void operator=(const mx_font_mod& other);
    mx_font_mod& operator+=(const mx_font_mod& other);

    void serialise(int& err, uint8** buffer);
    void unserialise(int& err, uint8** buffer);
    uint32 get_serialised_size(int& err);

    bool is_null() const;

    // sets the revert to default flag for any part of the mod which is
    // changing to the given default paragraph style
    void set_revert_to_default_flags(const mx_font& default_para_font);

    // changes any mods with a revert flag set so that result of the
    // modification is the new default character style
    void set_new_default_cs(const mx_font& default_para_font);

    void clear_revert_to_default_flags();

    // clears any mod flags in this mod which are set in the other_mod
    void clear_mods_in(const mx_font_mod& other_mod);

    // get/set/clear individual mods
    bool get_family_mod(std::string &new_family) const;
    bool get_size_mod(float &new_size) const;
    bool get_bold_mod(bool &bold_on) const;
    bool get_italic_mod(bool &italic_on) const;

    void set_family_mod(const std::string &new_family);
    void set_size_mod(float new_size);
    void set_bold_mod(bool bold_on);
    void set_italic_mod(bool italic_on);

    void clear_family_mod();
    void clear_size_mod();
    void clear_bold_mod();
    void clear_italic_mod();

private:
    void set_family(const std::string &s);

    // values to change
    bool typeface_family_mod : 1;
    bool typeface_size_mod : 1;
    bool typeface_bold_mod : 1;
    bool typeface_italic_mod : 1;

    // revert to default flags
    bool typeface_family_mod_rev : 1;
    bool typeface_size_mod_rev : 1;
    bool typeface_bold_mod_rev : 1;
    bool typeface_italic_mod_rev : 1;

    std::string m_typeface_family;
    float typeface_size;
    bool typeface_bold : 1;
    bool typeface_italic : 1;
};
