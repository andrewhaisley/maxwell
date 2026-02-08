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
#ifndef MX_SYMBOL_D
#define MX_SYMBOL_D
/*
 * MODULE/CLASS : mx_symbol_d
 *
 * AUTHOR : Andrew Haisley
 *
 * This file
 *
 * DESCRIPTION:
 *
 *  A symbol selection dialog.
 *
 *
 *
 *
 */

#include "mx_dialog.h"
#include <Xm/Xm.h>
#include <mx_font.h>
#include <mx_ui_object.h>

class mx_symbol_d : public mx_dialog {

public:
    mx_symbol_d(Widget parent, mx_font* f);
    void activate_d(mx_font* f = NULL);
    int run(mx_font* f = NULL);

    int selected_character;

    Widget dismiss_button;
    Widget char_buttons[256];

    void draw_symbols();
    void handle_input(XEvent* e);

private:
    void xor_square(int x, int y);
    void square_pos(int x, int y, int& i, int& j);
    Widget drawing_area;

    GC gc;

    char font_name[200];
    int last_x, last_y;

    mx_font *m_font;
};

#endif
