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
#ifndef MX_COLOUR_H
#define MX_COLOUR_H
/*
 * MODULE/CLASS : mx_colour
 *
 * AUTHOR : Andrew Haisley/Tom Newton
 *
 *
 *
 * DESCRIPTION:
 *
 * Styles for everything. Ranging from colours/fonts for characters
 * through to paragraph justification types......
 *
 *
 *
 */

#include <mx_db.h>

typedef struct
{
    unsigned char r;
    unsigned char g;
    unsigned char b;
} mx_colour_val_struct_t;

// colour names
#define MX_NUM_COLOURS 16

extern const char* mx_colour_label_names[MX_NUM_COLOURS];
extern const char* mx_colour_names[MX_NUM_COLOURS];
extern mx_colour_val_struct_t mx_colour_values[MX_NUM_COLOURS];

class mx_colour : public mx_serialisable_object {
    friend class mx_screen_device;

    MX_RTTI(mx_colour_class_e)

public:
    uint8 red;
    uint8 green;
    uint8 blue;
    const char* name;

    mx_colour();
    mx_colour(int r, int g, int b, const char* n);
    mx_colour(const char* name);

    friend bool operator==(const mx_colour& c1, const mx_colour& c2);
    friend bool operator!=(const mx_colour& c1, const mx_colour& c2);

    void init();

    uint32 get_serialised_size(int& err);
    void serialise(int& err, uint8** buffer);
    void unserialise(int& err, uint8** buffer);

    void setColourChanges(bool setNew, const mx_colour& newColour, bool& setColour);

private:
    // used by screen device for optimisation.
    bool pixel_value_set;
    LUT_VALUE pixel_value;
};

#endif // MX_COLOUR_H
