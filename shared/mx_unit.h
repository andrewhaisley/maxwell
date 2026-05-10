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
#ifndef MX_UNIT_H
#define MX_UNIT_H

#include <string>
#include <ctype.h>
#include <stdlib.h>
#include <string.h>

#include <mx.h>
#include <mx_rtti.h>

// there are two interfaces here.
//
// 1. a standard set of C things.
// 2. a C++ class to do the same sorts of things.
//
// all new code doing things with unit conversions should use the class
// interface. Please note that ALL internal maxwell values are held in
// mm. Translation only occurs on user IO
//

#define MX_NUM_UNIT_TYPES 3

typedef enum {
    mx_mm,
    mx_inches,
    mx_points
} mx_unit_e;

#define MX_MM_TO_INCHES(x) ((x) / 25.4)
#define MX_INCHES_TO_MM(x) ((x) * 25.4)
#define MX_POINTS_TO_INCHES(x) ((x) / 72.0)
#define MX_INCHES_TO_POINTS(x) ((x) * 72.0)
#define MX_POINTS_TO_MM(x) ((x) * 0.3527778)
#define MX_MM_TO_POINTS(x) ((x) / 0.3527778)

// conversion to/from twips (twentieths of a point)
#define MX_TWIPS_TO_MM(twips) ((twips) * 0.017638889)
#define MX_MM_TO_TWIPS(mm) ((mm) / 0.017638889)

float mx_mm_to_unit(float mm, mx_unit_e u);
/*
 * PARAMS  :
 *   IN  - mm   a measurement in mm
 *
 * RETURNS : the value converted into another unit
 *
 */

float mx_unit_to_mm(float x, mx_unit_e u);
/*
 * PARAMS  :
 *   IN - x   a measurement in the given unit
 *
 * RETURNS : the value in mm
 *
 */

const char* mx_unit_name(mx_unit_e u);
/*
 * PARAMS  :
 *   IN - u   a unit type
 *
 * RETURNS : a symbolic name for the particular unit type
 *
 */

mx_unit_e mx_unit_type(const std::string &s);
/*
 * PARAMS  :
 *   IN - u   a unit type
 *
 * RETURNS : a unit type from a name
 *
 */

const char* mx_full_unit_name(mx_unit_e u);
/*
 * PARAMS  :
 *   IN - u   a unit type
 *
 * RETURNS : a full symbolic name for the particular unit type
 *
 */

// now, a class that does the same things

class mx_unit : public mx_rtti {
    MX_RTTI(mx_unit_class_e)

public:
    // various constructors

    // 0 mm
    mx_unit();

    // n mm
    mx_unit(float mm);

    // n units of type u
    mx_unit(float mm, mx_unit_e u);

    // from string - if the string contains a type, use that otherwise
    // assume it's in the unit type given
    // err - MX_UNIT_INVALID_TYPE   - the type named in the string was not
    // err - MX_UNIT_INVALID_STRING - the string was garbage
    // recognisable e.g. 23.5 xyz
    mx_unit(int& err, char* s, mx_unit_e u);

    // turn it into a string of the specified unit type
    const char* string(mx_unit_e u);

    // turn it into a mm string with no trailing 'mm'
    const char* string_mm();

    // value in mm
    float value();

    // value in any given unit type
    float value_unit(mx_unit_e u);

    // set value in mm
    void set_value(float v);

    // set value in unit
    void set_value(float v, mx_unit_e u);

    // set value from string
    void set_value(int& err, char* s, mx_unit_e u);

    // unit type names
    const char* unit_name(mx_unit_e u);

    // full unit type names
    const char* full_unit_name(mx_unit_e u);

private:
    float v;
    static char buffer[100];
};

#endif
