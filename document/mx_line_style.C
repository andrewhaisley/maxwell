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
#include <mx.h>
#include <mx_db.h>

#include <mx_line_style.h>

bool mx_line_style::getArrows(bool& doArrowStart,
    bool& doArrowEnd)

{
    bool doArrows = false;

    switch (arrow_type) {
    case mx_arrow_none:
        doArrows = FALSE;
        doArrowStart = FALSE;
        doArrowEnd = FALSE;
        break;
    case mx_arrow_start:
        doArrows = TRUE;
        doArrowStart = TRUE;
        doArrowEnd = FALSE;
        break;
    case mx_arrow_end:
        doArrows = TRUE;
        doArrowStart = FALSE;
        doArrowEnd = TRUE;
        break;
    case mx_arrow_both:
        doArrows = TRUE;
        doArrowStart = TRUE;
        doArrowEnd = TRUE;
        break;
    }

    return doArrows;
}
void mx_line_style::equate(const mx_line_style& iline_style)
{
    line_type = iline_style.line_type;
    arrow_pattern = iline_style.arrow_pattern;
    arrow_type = iline_style.arrow_type;
    join_type = iline_style.join_type;
    cap_type = iline_style.cap_type;
    line_id = iline_style.line_id;
    dash_style = iline_style.dash_style;
    colour = iline_style.colour;
    colour_id = iline_style.colour_id;
    width = iline_style.width;
}

mx_line_style::mx_line_style(const mx_line_style& iline_style)
{
    equate(iline_style);
}

void mx_line_style::init()
{
    line_type = mx_solid;
    arrow_pattern = mx_arrow_triangle;
    arrow_type = mx_arrow_none;
    width = 0.0;
    join_type = mx_join_miter;
    cap_type = mx_cap_butt;
    line_id = -1;
    colour_id = -1;
}

mx_line_style::mx_line_style()
{
    init();
}

mx_line_style::mx_line_style(mx_line_type_t t, float w)
{
    init();
    line_type = t;
    width = w;
}

uint32 mx_line_style::get_serialised_size(int& err)
{
    uint32 res;

    res = SLS_ENUM;

    if (line_type != mx_no_line) {
        res += 4 * SLS_ENUM + SLS_FLOAT + SLS_INT32;

        res += dash_style.get_serialised_size(err);
        MX_ERROR_CHECK(err);

        res += colour.get_serialised_size(err);
        MX_ERROR_CHECK(err);
    }

abort:
    return res;
}

void mx_line_style::setDashStyle(mx_dash_style& idashStyle)
{
    dash_style = idashStyle;
    line_type = mx_custom_line;
}

void mx_line_style::serialise(int& err, uint8** buffer)
{
    err = MX_ERROR_OK;

    serialise_enum(line_type, buffer);

    if (line_type != mx_no_line) {
        serialise_enum(arrow_pattern, buffer);
        serialise_enum(arrow_type, buffer);
        serialise_enum(join_type, buffer);
        serialise_enum(cap_type, buffer);
        serialise_float(width, buffer);
        serialise_int32(line_id, buffer);

        dash_style.serialise(err, buffer);
        MX_ERROR_CHECK(err);

        colour.serialise(err, buffer);
        MX_ERROR_CHECK(err);
    }

abort:;
}

void mx_line_style::unserialise(int& err, uint8** buffer)
{
    uint16 e;

    err = MX_ERROR_OK;
    unserialise_enum(e, buffer);
    line_type = (mx_line_type_t)e;

    if (line_type != mx_no_line) {
        unserialise_enum(e, buffer);
        arrow_pattern = (mx_arrow_pattern_type_t)e;
        unserialise_enum(e, buffer);
        arrow_type = (mx_arrow_type_t)e;
        unserialise_enum(e, buffer);
        join_type = (mx_join_type_t)e;
        unserialise_enum(e, buffer);
        cap_type = (mx_cap_type_t)e;

        unserialise_float(width, buffer);
        unserialise_int32(line_id, buffer);

        // unserialise the dash type is it is defined
        dash_style.unserialise(err, buffer);
        MX_ERROR_CHECK(err);

        colour.unserialise(err, buffer);
        MX_ERROR_CHECK(err);
    } else {
        init();
        dash_style.init();
        colour.init();
        line_type = mx_no_line;
    }

abort:;
}

bool mx_line_style::sameLineType(const mx_line_style& s) const
{
    if ((line_type == mx_custom_line) && (s.line_type == mx_custom_line)) {
        return (dash_style == s.dash_style);
    } else {
        return (line_type == s.line_type);
    }
}

/*-------------------------------------------------
 * FUNCTION: mx_line_style::setStyleChanges
 *
 * DESCRIPTION:
 *
 *
 */

void mx_line_style::setStyleChanges(bool setNew,
    mx_line_style& newStyle,
    bool& setWidth,
    bool& setJoin,
    bool& setCap,
    bool& setLineType)
{
    if (setNew) {
        setWidth = TRUE;
        setJoin = TRUE;
        setCap = TRUE;
        setLineType = TRUE;
        *this = newStyle;
    } else {
        setWidth = !MX_FLOAT_EQ(width, newStyle.width);
        width = newStyle.width;

        setJoin = (join_type != newStyle.join_type);
        join_type = newStyle.join_type;

        setCap = (cap_type != newStyle.cap_type);
        cap_type = newStyle.cap_type;

        setLineType = !sameLineType(newStyle);

        if (setLineType) {
            line_type = newStyle.line_type;
            dash_style = newStyle.dash_style;
        }
    }
}

bool operator==(const mx_line_style& s1, const mx_line_style& s2)
{
    // compare line type and dashes

    if (s1.sameLineType(s2)) {
        if (s1.line_type == mx_no_line) {
            // they are both no line types - that's enough
            return TRUE;
        } else {
            // other parameters need to agree
            return s1.arrow_pattern == s2.arrow_pattern && s1.arrow_type == s2.arrow_type && MX_FLOAT_EQ(s1.width, s2.width) && s1.join_type == s2.join_type && s1.cap_type == s2.cap_type && s1.colour == s2.colour;
        }
    } else {
        // line types are different
        return FALSE;
    }
}

bool operator!=(const mx_line_style& s1, const mx_line_style& s2)
{
    return !(s1 == s2);
}

mx_line_style::mx_line_style(mx_line_type_t t, float w, const char* colour_name)
{
    init();

    line_type = t;
    width = w;
    colour.name = colour_name;
}

mx_line_style& mx_line_style::operator=(const mx_line_style& iline_style)
{
    equate(iline_style);
    return *this;
}
