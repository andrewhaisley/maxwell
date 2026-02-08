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
#ifndef MX_HSEARCH_D
#define MX_HSEARCH_D
/*
 * MODULE/CLASS : mx_hsearch_d
 *
 * AUTHOR : Andrew Haisley
 *
 * This file
 *
 * DESCRIPTION:
 *
 *  A search dialog for the help system.
 *
 *
 *
 */

#include "mx_dialog.h"
#include <Xm/Xm.h>
#include <mx_ui_object.h>

#define MAX_HSEARCH_PATTERN 100
#define MAX_HSEARCH_FILES 200

class mx_hsearch_d : public mx_dialog {

public:
    mx_hsearch_d(Widget parent);

    char selected_topic[MAX_HSEARCH_PATTERN];

    Widget goto_button, cancel_button, search_button;

    void add_pattern();
    void search();

    Widget pattern_text, pattern_list;
    Widget topic_list;

    void set_topic_file(char* topic);

private:
    char* help_files[MAX_HSEARCH_FILES];
    char* help_topics[MAX_HSEARCH_FILES];
    void load_topic_map();

    int num_help_files, num_patterns;
};

#endif
