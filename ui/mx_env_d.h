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
#ifndef MX_ENV_D
#define MX_ENV_D
/*
 * MODULE/CLASS : mx_env_d
 *
 * AUTHOR : Andrew Haisley
 *
 * This file
 *
 * DESCRIPTION:
 *
 *  An envelope setup dialog
 *
 *
 *
 */

#include "mx_dialog.h"
#include <Xm/Xm.h>
#include <mx_ui_object.h>

#define MX_ENV_MAX_TEXT 2000

class mx_env_d : public mx_dialog {

public:
    mx_env_d(Widget parent);

    // values selected by user
    float width;
    float height;

    char address[MX_ENV_MAX_TEXT];

    bool is_landscape; // otherwise portrait

    Widget ok_button, cancel_button;

    void activate_d(
        float width,
        float height,
        bool is_landscape,
        char* address,
        mx_unit_e u);

    void figure_options();
    void set_options();
    void set_size(char* s);

private:
    void fill_size_list();

    Widget list, address_text;
    Widget landscape_toggle, portrait_toggle;
    Widget width_text, height_text;

    mx_unit_e units;
};

#endif
