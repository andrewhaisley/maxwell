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
#ifndef MX_LINE_STYLE_H
#define MX_LINE_STYLE_H
/*
 * MODULE/CLASS : mx_line_style
 *
 * AUTHOR : Andrew Haisley
 *
 *
 *
 * DESCRIPTION:
 *
 * Styles for lines.
 *
 *
 *
 */

#include <mx_db.h>

#include <mx_colour.h>
#include <mx_dash_style.h>
#include <mx_doc_coord.h>
#include <mx_doc_rect.h>
#include <mx_spline.h>

typedef enum {
    mx_join_bevel,
    mx_join_round,
    mx_join_miter,
    mx_njoin_types
} mx_join_type_t;

typedef enum {
    mx_cap_butt,
    mx_cap_round,
    mx_cap_projecting,
    mx_cap_butt_not_last,
    mx_ncap_types
} mx_cap_type_t;

typedef enum {
    mx_arrow_none,
    mx_arrow_start,
    mx_arrow_end,
    mx_arrow_both
} mx_arrow_type_t;

typedef enum {
    mx_arrow_triangle,
    mx_arrow_lines
} mx_arrow_pattern_type_t;

typedef enum {
    mx_dotted,
    mx_dashed,
    mx_solid,
    mx_no_line,
    mx_custom_line,
    mx_use_lstyle // use the style currently set on the device
} mx_line_type_t;

class mx_line_style : public mx_serialisable_object {
    MX_RTTI(mx_line_style_class_e)

public:
    mx_line_type_t line_type;
    mx_arrow_pattern_type_t arrow_pattern;
    mx_arrow_type_t arrow_type;
    mx_join_type_t join_type;
    mx_cap_type_t cap_type;

    // the dash pattern or dash identifier for
    // a custom line
    mx_dash_style dash_style;
    int32 line_id;

    mx_line_style();
    mx_line_style(const mx_line_style& iline_style);
    mx_line_style(mx_line_type_t t, float width);
    mx_line_style(mx_line_type_t t, float width, const char* colour_name);

    void setDashStyle(mx_dash_style& dashStyle);

    void equate(const mx_line_style& iline_style);
    float width; // In MM

    uint32 get_serialised_size(int& err);
    void serialise(int& err, uint8** buffer);
    void unserialise(int& err, uint8** buffer);
    void init();

    void setStyleChanges(bool setNew,
        mx_line_style& newStyle,
        bool& setWidth,
        bool& setJoin,
        bool& setCap,
        bool& setLineType);

    bool sameLineType(const mx_line_style& s1) const;

    friend bool operator==(const mx_line_style& s1, const mx_line_style& s2);
    friend bool operator!=(const mx_line_style& s1, const mx_line_style& s2);

    mx_line_style& operator=(const mx_line_style& line_style);

    mx_colour colour;
    int32 colour_id;

    bool getArrows(bool& doArrowStart,
        bool& doArrowEnd);
};

#endif // MX_LINE_STYLE_H
