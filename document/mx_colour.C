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
 * MODULE/CLASS : mx_colour
 *
 * AUTHOR : Andrew Haisley
 *
 *
 *
 * DESCRIPTION:
 *
 * Colours
 *
 *
 *
 *
 */
#include <mx.h>
#include <mx_db.h>

#include <mx_colour.h>

mx_colour::mx_colour()
{
    init();
}

void mx_colour::init()
{
    name = mx_colour_names[0];
    red = green = blue = 0;
    pixel_value_set = FALSE;
}

mx_colour::mx_colour(int r, int g, int b, const char* n)
{
    name = n;
    red = r;
    green = g;
    blue = b;
    pixel_value_set = FALSE;
}

mx_colour::mx_colour(const char* name)
{
    for (int i = 0; i < MX_NUM_COLOURS; i++) {
        if (strcmp(name, mx_colour_names[i]) == 0) {
            red = mx_colour_values[i].r;
            green = mx_colour_values[i].g;
            blue = mx_colour_values[i].b;
        }
    }
    pixel_value_set = FALSE;
    this->name = name;
}

uint32 mx_colour::get_serialised_size(int& err)
{
    err = MX_ERROR_OK;
    return SLS_INT8 * 3 + SLS_STRING(name);
}

void mx_colour::serialise(int& err, unsigned char** buffer)
{
    serialise_uint8(red, buffer);
    serialise_uint8(green, buffer);
    serialise_uint8(blue, buffer);
    serialise_string(name, buffer);

    err = MX_ERROR_OK;
}

void mx_colour::unserialise(int& err, unsigned char** buffer)
{
    char temp[50];

    err = MX_ERROR_OK;

    unserialise_uint8(red, buffer);
    unserialise_uint8(green, buffer);
    unserialise_uint8(blue, buffer);
    unserialise_string(temp, buffer);

    if (strlen(temp) > 0) {
        for (int i = 0; i < MX_NUM_COLOURS; i++) {
            if (strcmp(mx_colour_names[i], temp) == 0) {
                name = mx_colour_names[i];
                return;
            }
        }
        name = mx_colour_names[0];
        return;
    } else {
        name = NULL;
    }
}

bool operator==(const mx_colour& c1, const mx_colour& c2)
{
    if (c1.name == NULL) {
        if (c2.name == NULL) {
            return c1.red == c2.red && c1.green == c2.green && c1.blue == c2.blue;
        } else {
            return FALSE;
        }
    } else {
        if (c2.name == NULL) {
            return FALSE;
        } else {
            return strcmp(c1.name, c2.name) == 0;
        }
    }
}

bool operator!=(const mx_colour& c1, const mx_colour& c2)
{
    return !(c1 == c2);
}

/*-------------------------------------------------
 * FUNCTION: mx_colour::setColourChanges
 *
 * DESCRIPTION:
 *
 *
 */

void mx_colour::setColourChanges(bool setNew, const mx_colour& newColour, bool& setColour)
{
    if (setNew) {
        *this = newColour;
        setColour = TRUE;
    } else {
        setColour = (*this != newColour);
        if (setColour)
            *this = newColour;
    }
}

const char* mx_colour_label_names[16] = {
    "black", "red", "orange", "yellow", "green", "blue", "indigo", "violet",
    "white", "ltred", "ltorange", "ltyellow", "ltgreen", "ltblue", "ltindigo",
    "ltviolet"
};
mx_colour_val_struct_t mx_colour_values[16] = { { 0, 0, 0 }, { 139, 0, 0 }, { 255, 165, 0 }, { 255, 255, 0 }, { 0, 255, 0 }, { 0, 0, 255 }, { 85, 26, 139 }, { 148, 0, 211 }, { 255, 255, 255 }, { 255, 0, 0 }, { 205, 133, 0 }, { 255, 255, 224 }, { 144, 238, 144 }, { 173, 216, 230 }, { 55, 48, 255 }, { 238, 130, 238 } };

const char* mx_colour_names[16] = {
    "black", "dark red", "orange", "yellow", "green", "blue", "purple4", "dark violet",
    "white", "red", "orange3", "light yellow", "light green", "light blue", "purple1",
    "violet"
};
