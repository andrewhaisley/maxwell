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
 * MODULE/CLASS : mx_unit
 *
 * AUTHOR : Andrew Haisley
 *
 *
 *
 *
 *
 */

#include <algorithm>
#include <ctype.h>
#include <errno.h>
#include <mx.h>
#include <mx_unit.h>
#include <stdlib.h>
#include <string.h>

using namespace std;

#define MX_NUM_UNIT_NAMES 10

static char unit_names[MX_NUM_UNIT_NAMES][15] = {
    "mm",
    "millimetres",
    "i",
    "in",
    "inches",
    "\"",
    "points",
    "pts",
    "pt",
    "p"
};

static mx_unit_e unit_types[MX_NUM_UNIT_NAMES] = {
    mx_mm,
    mx_mm,
    mx_inches,
    mx_inches,
    mx_inches,
    mx_inches,
    mx_points,
    mx_points,
    mx_points,
    mx_points
};

float mx_mm_to_unit(float mm, mx_unit_e u)
/*
 * PARAMS  :
 *   IN  - mm   a measurement in mm
 *
 * RETURNS : the value converted into another unit
 *
 */
{
    switch (u) {
    default:
    case mx_mm:
        return mm;
    case mx_inches:
        return MX_MM_TO_INCHES(mm);
    case mx_points:
        return MX_MM_TO_POINTS(mm);
    }
}

float mx_unit_to_mm(float x, mx_unit_e u)
/*
 * PARAMS  :
 *   IN - x   a measurement in the given unit
 *
 * RETURNS : the value in mm
 *
 */
{
    switch (u) {
    default:
    case mx_mm:
        return x;
    case mx_inches:
        return MX_INCHES_TO_MM(x);
    case mx_points:
        return MX_POINTS_TO_MM(x);
    }
}

const char* mx_unit_name(mx_unit_e u)
/*
 * PARAMS  :
 *   IN - u   a unit type
 *
 * RETURNS : a symbolic name for the particular unit type
 *
 */
{
    switch (u) {
    default:
    case mx_mm:
        return "mm";
    case mx_inches:
        return "in";
    case mx_points:
        return "pts";
    }
}

mx_unit_e mx_unit_type(const string &t)
/*
 * PARAMS  :
 *   IN - u   a unit type
 *
 * RETURNS : a unit type from a name
 *
 */
{
    string s = t;
 
    transform(s.begin(), s.end(), s.begin(), ::tolower);

    if (s == "mm")
        return mx_mm;
    else if (s == "millimetres")
        return mx_mm;
    else if (s == "in")
        return mx_inches;
    else if (s == "inches")
        return mx_inches;
    else if (s == "points")
        return mx_points;
    else if (s == "pts")
        return mx_points;
    else
        return mx_mm;
}

const char* mx_full_unit_name(mx_unit_e u)
/*
 * PARAMS  :
 *   IN - u   a unit type
 *
 * RETURNS : a full symbolic name for the particular unit type
 *
 */
{
    switch (u) {
    default:
    case mx_mm:
        return "millimetres";
    case mx_inches:
        return "inches";
    case mx_points:
        return "points";
    }
}

mx_unit::mx_unit()
{
    v = 0.0;
}

mx_unit::mx_unit(float mm)
{
    v = mm;
}

mx_unit::mx_unit(float mm, mx_unit_e u)
{
    v = mx_unit_to_mm(mm, u);
}

mx_unit::mx_unit(int& err, char* s, mx_unit_e u)
{
    set_value(err, s, u);
    MX_ERROR_CHECK(err);
abort:;
}

void mx_unit::set_value(float v, mx_unit_e u)
{
    mx_unit::v = mx_unit_to_mm(v, u);
}

void mx_unit::set_value(float v)
{
    mx_unit::v = v;
}

void mx_unit::set_value(int& err, char* s, mx_unit_e u)
{
    int i = 0, j, num_stops = 0, n;
    float f;

    err = MX_ERROR_OK;

    // should be a number followed by something like
    // mm, millimeters, points, pt, pts, inches, in "

    // must be at least on digit
    if (!isdigit(s[0])) {
        err = MX_UNIT_INVALID_STRING;
        v = 0.0;
        return;
    }

    // go forward to first non-digit except for one '.'
    while (TRUE) {
        if (s[i] == 0) {
            break;
        }
        if (s[i] == '.') {
            if (num_stops == 0) {
                num_stops++;
            } else {
                break;
            }
        }
    }
    f = atof(s);

    if (s[i] == 0) {
        // no units in the string, default to the type given
        v = mx_unit_to_mm(f, u);
    }

    for (n = 0; n < MX_NUM_UNIT_NAMES; i++) {
        if (strlen(s + i) < strlen(unit_names[n])) {
            j = strlen(s + i);
        } else {
            j = strlen(unit_names[n]);
        }

        if (strncasecmp(s + i, unit_names[n], j) == 0) {
            u = unit_types[n];
            v = mx_unit_to_mm(f, u);
            return;
        }
    }
    err = MX_UNIT_INVALID_TYPE;
}

const char* mx_unit::string(mx_unit_e u)
{
    sprintf(buffer, "%.2f %s", v, mx_unit_name(u));
    return buffer;
}

const char* mx_unit::string_mm()
{
    sprintf(buffer, "%.2f %s", v, mx_unit_name(mx_mm));
    return buffer;
}

float mx_unit::value()
{
    return v;
}

float mx_unit::value_unit(mx_unit_e u)
{
    return mx_mm_to_unit(v, u);
}

const char* mx_unit::unit_name(mx_unit_e u)
{
    return mx_unit_name(u);
}

const char* mx_unit::full_unit_name(mx_unit_e u)
{
    return mx_full_unit_name(u);
}

char mx_unit::buffer[100];
