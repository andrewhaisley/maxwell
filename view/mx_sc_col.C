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
 * MODULE : mx_sc_col.C
 *
 * AUTHOR : David Miller
 *
 * This file
 *
 * DESCRIPTION:
 * Module mx_sc_col.C
 *
 *
 *
 */

#include <mx_sc_device.h>

std::map<LUT_VALUE, XColor *>       mx_screen_device::m_colour_dict;
std::map<std::string, LUT_VALUE>    mx_screen_device::m_colour_name_dict;
std::map<uint32, LUT_VALUE>         mx_screen_device::m_colour_val_dict;

/*-------------------------------------------------
 * FUNCTION: mx_screen_device::registerColour
 *
 * DESCRIPTION:
 *
 *
 */

void mx_screen_device::registerColour(mx_colour* colour, LUT_VALUE* pixel)
{
    XColor screenColour, exactColour;
    XColor* nextColour;
    bool gotPixel = false;
    bool found = false;
    int dist, newdist;

    *pixel = BlackPixel(display, DefaultScreen(display));

    // Try by name first
    if (colour->name != NULL) {
        if (m_colour_name_dict.find(colour->name) == m_colour_name_dict.end()) {
            if (XLookupColor(display, DefaultColormap(display,DefaultScreen(display)), colour->name,&screenColour,&exactColour)) {
                gotPixel = XAllocNamedColor(display,DefaultColormap(display,DefaultScreen(display)), colour->name,&screenColour,&exactColour);
                if (gotPixel) {
                    m_colour_name_dict[colour->name] = screenColour.pixel;
                }
            }
        } else {
            *pixel = m_colour_name_dict[colour->name];
            return;
        }
    }

    if (!gotPixel) {
        uint32 dict_lu = ((((uint32)colour->red) << 16 & 0x00ff0000) | (((uint32)colour->green) << 8 & 0x0000ff00) | (((uint32)colour->blue) & 0x000000ff));

        if (m_colour_val_dict.find(dict_lu) == m_colour_val_dict.end()) {
            screenColour.red = (uint16)colour->red << 8;
            screenColour.green = (uint16)colour->green << 8;
            screenColour.blue = (uint16)colour->blue << 8;
            screenColour.flags = DoRed | DoGreen | DoBlue;

            gotPixel = XAllocColor(display, DefaultColormap(display, DefaultScreen(display)), &screenColour);

            if (gotPixel) {
                m_colour_val_dict[dict_lu] = screenColour.pixel;
            }
        } else {
            *pixel = m_colour_val_dict[dict_lu];
            return;
        }
    }

    if (!gotPixel) {
        // Havent got a pixel value yet - map to the nearest for which
        // we have read access
        dist = 65536 * 4;
        bool foundPixel = false;

        for (auto i : m_colour_dict)
        {
            nextColour = i.second;
            newdist = abs(colour->red - nextColour->red) + abs(colour->green - nextColour->green) + abs(colour->blue - nextColour->blue);

            if (newdist < dist) {
                foundPixel = true;
                *pixel = nextColour->pixel;
                dist = newdist;
            }
        }

        // No colour - use the foreground colour
        if (!foundPixel) {
            *pixel = BlackPixel(display, DefaultScreen(display));
        }
    } else {
        *pixel = screenColour.pixel;
        found = false;

        for (auto i : m_colour_dict)
        {
            nextColour = i.second;
            if ((nextColour->red == screenColour.red) && (nextColour->green == screenColour.green) && (nextColour->blue == screenColour.blue)) {
                found = true;
                break;
            }
        }

        if (!found) {
            nextColour = new XColor;

            *nextColour = screenColour;
            m_colour_dict[*pixel ] = nextColour;
        }
    }
}

/*-------------------------------------------------
 * FUNCTION: setForegroundColour
 *
 * DESCRIPTION:
 *
 *
 */

void mx_screen_device::setForegroundColour(int& err, LUT_VALUE pixel)
{
    err = MX_ERROR_OK;

    XSetForeground(display, gc, pixel);

    foreground = pixel;

    return;
}

/*-------------------------------------------------
 * FUNCTION: setBackgroundColour
 *
 * DESCRIPTION:
 *
 *
 */

void mx_screen_device::setBackgroundColour(int& err, LUT_VALUE pixel)
{
    err = MX_ERROR_OK;

    XSetBackground(display, gc, pixel);

    background = pixel;

    return;
}

void mx_screen_device::setForegroundColour(int& err, mx_colour& c)
{
    if (!c.pixel_value_set) {
        registerColour(&c, &c.pixel_value);
        c.pixel_value_set = true;
    }

    setForegroundColour(err, c.pixel_value);
    MX_ERROR_CHECK(err);

    m_xft_foreground_colour = getXftColour(display, c);

abort:;
}

void mx_screen_device::setBackgroundColour(int& err, mx_colour& c)
{
    if (!c.pixel_value_set) {
        registerColour(&c, &c.pixel_value);

        c.pixel_value_set = true;
    }

    setBackgroundColour(err, c.pixel_value);
    MX_ERROR_CHECK(err);
abort:;
}


XftColor mx_screen_device::getXftColour(Display *display, const mx_colour &c)
{
    XftColor res;

    uint16_t r = c.red;
    uint16_t g = c.green;
    uint16_t b = c.blue;

    r *= 257;
    g *= 257;
    b *= 257;

    XRenderColor xr_col = {r, g, b, 0xffff};
    XftColorAllocValue(display, DefaultVisual(display, DefaultScreen(display)), DefaultColormap(display, DefaultScreen(display)), &xr_col, &res);

    return res;
}
