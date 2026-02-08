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

#include <map>
#include <string>
#include <vector>
#include <memory>

#include <mx.h>
#include <mx_font.h>

class mx_font_metrics;

// This class describes a family of fonts. It is a very simple class which
// simple holds pointers to four font metrics objects, one for each style,
// normal, bold, italic, and bold-italic.
class mx_font_family {
public:
    mx_font_family();
    ~mx_font_family();

    // gets a pointer to some font metrics for the given style. Returns nullptr if
    // there are none for a given style.
    mx_font_metrics* get_font_metrics(mx_font_style_t st) const;

    // stores the font metrics class in the relevant slot;
    void set_font_metrics(mx_font_metrics* fm);

    // Tells us whether the font family has bold or italics defined for it
    void family_has_B_or_I(bool& b, bool& it);

    // returns the name of the font family
    std::string get_name() const;

private:
    friend uint32 mx_font::em_width(char character) const;

    mx_font_metrics* styles[4];
};
