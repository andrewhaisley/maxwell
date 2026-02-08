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
#ifndef MX_TOC_D
#define MX_TOC_D
/*
 * MODULE/CLASS : mx_toc_d
 *
 * AUTHOR : Andrew Haisley
 *
 * This file
 *
 * DESCRIPTION:
 *
 *  A table of contents insert dialog
 *
 *
 *
 *
 */

#include "mx_dialog.h"
#include <Xm/Xm.h>
#include <mx_ui_object.h>

class mx_inform_d;

class mx_toc_d : public mx_dialog {

public:
    mx_toc_d(Widget parent);
    void activate_d(int num_pages, int max_indent);

    int start_page, end_page, max_indent;
    bool replace_existing;

    Widget ok_button, cancel_button, replace_toggle;

    void validate_data();

    Widget from_text, to_text, indent_text;

private:
    mx_inform_d* invalid_from_d;
    mx_inform_d* invalid_to_d;
    mx_inform_d* invalid_indent_d;
};

#endif
