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
 * MODULE/CLASS : mx_font_metrics_store & mx_font_metrics
 *
 * AUTHOR : Tom Newton
 *
 *
 *
 * DESCRIPTION:
 *
 * This class is a font class and includes methods for querying a font about
 * its metrics and such like..
 *
 *
 *
 *
 */

#include <stdio.h>
#include <sys/stat.h>
#include <unistd.h>

#include <map>
#include <string>
#include <algorithm>
#include <cctype>

#include <mx.h>
#include <mx_font_family.h>
#include <mx_font_metrics.h>

// global variable
extern char* global_maxhome;

mx_font_family::mx_font_family()
{
    for (int i = 0; i < 4; i++) {
        styles[i] = nullptr;
    }
}

mx_font_family::~mx_font_family()
{
    for (int i = 0; i < 4; i++) {
        if (styles[i])
            delete styles[i];
    }
}

mx_font_metrics* mx_font_family::get_font_metrics(mx_font_style_t st) const
{
    if (styles[st] != nullptr) {
        return styles[st];
    } else {
        for (int i = 0; i < 4; i++) {
            if (styles[i] != nullptr) {
                return styles[i];
            }
        }
    }
    return nullptr;
}
void mx_font_family::set_font_metrics(mx_font_metrics* fm)
{
    if (styles[fm->get_style()] == fm)
        return;
    if (styles[fm->get_style()] != nullptr)
        delete styles[fm->get_style()];
    styles[fm->get_style()] = fm;
}

void mx_font_family::family_has_B_or_I(bool& b, bool& it)
{
    b = false;
    it = false;

    if (styles[mx_normal]) {
        if (styles[mx_bold]) {
            b = true;
            if (styles[mx_bold_italic] && styles[mx_italic]) {
                it = true;
            }
        } else {
            if (styles[mx_italic]) {
                it = true;
            }
        }
    }
}

std::string mx_font_family::get_name() const
{
    for (int i = 0; i < 4; i++) {
        if (styles[i])
            return styles[i]->get_family_name();
    }
    return nullptr;
}
