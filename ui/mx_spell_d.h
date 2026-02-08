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
#ifndef MX_SPELL_D
#define MX_SPELL_D
/*
 * MODULE/CLASS : mx_spell_d
 *
 * AUTHOR : Andrew Haisley
 *
 * This file
 *
 * DESCRIPTION:
 *
 *  A spell checker dialog
 *
 *
 *
 *
 */

#include "mx_dialog.h"
#include <Xm/Xm.h>
#include <mx_ui_object.h>

class mx_spell_d : public mx_dialog {

public:
    mx_spell_d(Widget parent, const char* language);

    Widget quit_button, list, word_text, replacement_text;
    Widget ignore_button, ignore_all_button, replace_button;
    Widget back_button, add_dict_button;

    void activate();
    bool wrong_spelling(const char* word);
    int run_modal(const char* word);
    void handle_button(Widget w);

    char original_word[100];
    char replacement[100];

    void set_language(const char* s);
    void spawn_ispell(const char* language);
    void finish_ispell();

    bool spawn_ispell_failed;

private:
    void set_language_menu(Widget w, const char* def);
    void fill_list();

    int to_ispell_fd[2];
    int from_ispell_fd[2];

    pid_t child_pid;

    char current_language[100];
    void ispell_fail();
};

#endif
