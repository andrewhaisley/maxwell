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
#ifndef MX_OPT_D
#define MX_OPT_D
/*
 * MODULE/CLASS : mx_opt_d
 *
 * AUTHOR : Andrew Haisley
 *
 * This file
 *
 * DESCRIPTION:
 *
 *  Options dialog
 *
 *
 *
 */

#include "mx_dialog.h"
#include <Xm/Xm.h>
#include <mx_config.h>
#include <mx_sizes.h>

class mx_opt_d : public mx_dialog {

public:
    mx_opt_d(Widget parent);

    Widget ok_button, cancel_button;

    void activate_d(mx_config* conf);
    int run(mx_config* conf);
    void figure_options();

    mx_config* config;

private:
    Widget language_menu;
    Widget language_sub_menu;
    Widget units_menu;
    Widget units_sub_menu;
    Widget printer_menu;
    Widget printer_sub_menu;
    Widget page_menu;
    Widget page_sub_menu;
    Widget envelope_menu;
    Widget envelope_sub_menu;

    void create_menus();

    void set_language_menu(const char* current);
    void set_unit_menu(const char* current);
    void set_page_menu(const char* current);
    void set_envelope_menu(const char* current);
    void set_printer_menu(const char* current);

    int num_printer_buttons;

    Widget printer_buttons[MX_MAX_PRINTERS];
    Widget unit_buttons[MX_NUM_UNIT_TYPES];
    Widget envelope_buttons[MX_NUM_ENVELOPE_SIZES];
    Widget paper_buttons[MX_NUM_PAPER_SIZES];
    Widget language_buttons[MX_MAX_LANGUAGES];
};

#endif
