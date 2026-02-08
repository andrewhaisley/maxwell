#pragma once 

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
 * MODULE/CLASS : mx_valid_d
 *
 * AUTHOR : Andrew Haisley
 *
 * This file
 *
 * DESCRIPTION:
 *
 *  Dialogs with some validation routines for fields
 *
 *
 *
 *
 */

#include <map>
#include <string>

#include <stdio.h>
#include <stdlib.h>

#include <mx_dialog.h>
#include <mx_inform_d.h>

class mx_valid_d : public mx_dialog {
public:
    mx_valid_d(
        const char* name,
        Widget parent,
        bool do_modal,
        bool resizable);

    ~mx_valid_d();

    // runs an inform dialog by its resource name - all dialogs
    // run like this are cleaned up automatically when the object is
    // is deleted
    void run_inform(const char* name, mx_inform_d_t type);

    bool validate_float(const char* name, Widget w, float min, float max);
    bool validate_int(const char* name, Widget w, int min, int max);

protected:
    std::map<std::string, mx_inform_d *> m_inform_dialogs;
};

