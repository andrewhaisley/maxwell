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

#include <cairo-ft.h>

#include <Xft/Xft.h>
#include <fontconfig/fontconfig.h>

#include <memory>
#include <vector>
#include <map>
#include <string>


#include <mx.h>
#include <mx_db.h>
#include <mx_sl_obj.h>

class mx_font;
class mx_font_mod;
class mx_font_family;
class mx_font_metrics_store;

typedef enum {
    mx_normal = 0,
    mx_bold,
    mx_italic,
    mx_bold_italic
} mx_font_style_t;

struct mx_font_key_t
{
    std::string     m_family;
    float           m_size;
    mx_font_style_t m_style;
    float           m_scale;
    float           m_resolution;

    bool operator<(const mx_font_key_t &o) const;
};

#define MX_FONT_MAX_FAMILY_NAME_LEN 100

class mx_font : mx_serialisable_object {
    MX_RTTI(mx_font_class_e)

    friend class mx_font_mod;

public:
    mx_font(
        int&              err,
        const std::string &family_name,
        float             size,
        mx_font_style_t   new_style);

    // default constructor - gives the system default roman font
    mx_font();
    virtual ~mx_font();

    // get a list of all available font families
    std::vector<std::string> get_font_family_names();

    // get_family 
    std::shared_ptr<mx_font_family> get_font_family() const;
    std::string get_family() const;

    float get_size() const;
    mx_font_style_t get_style() const;
    bool get_bold() const;
    bool get_italic() const;

    // used by the printer device driver
    std::string get_filename() const;

    // height in MM above the baseline of the tallest character in this font
    // a -ve value -> below the baseline, +ve above
    float get_ascender() const;

    // height in MM below the baseline of the lowest character in this font
    // a -ve value -> below the baseline, +ve above
    float get_descender() const;

    // methods for setting things

    // These set various font attributes - they return whether the change
    // worked or not.
    bool set_family(int& err, const std::string &new_family_name);
    bool set_size(float new_size);
    bool set_style(int& err, mx_font_style_t new_style);
    void set_bold(int& err, bool flag);
    void set_italic(int& err, bool flag);
    bool set_nearest_family(int& err, const std::string &name);

    // the following are used to get information about font metrics, such
    // as the width of a character or string etc..

    float width(const std::string &s) const;
    float width(char character) const;
    uint32 em_width(char character) const;
    float em_to_maxwell(uint32 em_width) const;
    uint32 em_to_maxwell_int(uint32 em_width) const;

    virtual void serialise(int& err, uint8** buffer);
    virtual void unserialise(int& err, uint8** buffer);
    virtual uint32 get_serialised_size(int& err);

    friend bool operator==(const mx_font& f1, const mx_font& f2);
    friend bool operator!=(const mx_font& f1, const mx_font& f2);

    // apply a font modification
    mx_font& operator+=(const mx_font_mod& m);

    // has this font got a standard latin encoding?
    bool has_latin_encoding(int& err);

    // get the Xft font for the family, size and style
    XftFont *get_xft_font(int &err, Display *dpy, float scale) const;

    // get the Cairo font
    cairo_font_face_t *get_cairo_font() const;
    
    static std::string get_default_roman_font();
    static std::string get_default_symbol_font();

    static int get_xft_width(XftFont *f, Display *display, const char *s, int len);

private:

    // For use inside a constructor to init all fields to initial values and
    // allocate space.
    void init_font();

    // screen resolution
    float get_resolution() const;

    static std::map<mx_font_key_t, XftFont *> m_font_cache;
    static std::map<mx_font_key_t, cairo_font_face_t *> m_cairo_font_cache;

    // This should be empty when it has an invalid value.
    std::string m_typeface_family;

    mx_font_style_t typeface_style;
    float typeface_size; /* in points */

    std::shared_ptr<mx_font_family> m_font_family;

    // This contains an object which manages all the font metrics objects in
    // the system
    static mx_font_metrics_store* m_font_metrics_store;

    // XFT font config
    static FcConfig *m_config;
};

