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
 *  Dialogs with validation
 *
 *
 *
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <Xm/Text.h>
#include <Xm/Xm.h>

#include <mx_valid_d.h>
#include <mx_std.h>

mx_valid_d::mx_valid_d(const char* name, Widget parent, bool do_modal, bool resizable) : mx_dialog(name, parent, do_modal, resizable)
{
}

mx_valid_d::~mx_valid_d()
{
    for (auto i : m_inform_dialogs) {
        delete i.second;
    }
}

void mx_valid_d::run_inform(const char* name, mx_inform_d_t type)
{
    if (m_inform_dialogs.find(name) == m_inform_dialogs.end()) {
        m_inform_dialogs[name] = new mx_inform_d(name, dialog, type);
    }

    mx_inform_d* d = m_inform_dialogs[name];

    d->centre();
    d->run_modal();
    d->deactivate();
}

bool mx_valid_d::validate_float(const char* name, Widget w, float min, float max)
{
    char* s = XmTextGetString(w);
    float f = atof(s);
    if (mx_is_blank(s)) {
        XtFree(s);
        return FALSE;
    }
    XtFree(s);

    if (f < min || f > max) {
        run_inform(name, error_e);
        return FALSE;
    }
    return TRUE;
}

bool mx_valid_d::validate_int(const char* name, Widget w, int min, int max)
{
    char* s = XmTextGetString(w);
    int i = atoi(s);

    if (mx_is_blank(s)) {
        XtFree(s);
        return FALSE;
    }
    XtFree(s);

    if (i < min || i > max) {
        run_inform(name, error_e);
        return FALSE;
    }
    return TRUE;
}
