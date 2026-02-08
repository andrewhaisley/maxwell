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
#ifndef MX_FINFO_D
#define MX_FINFO_D
/*
 * MODULE/CLASS : mx_finfo_d
 *
 * AUTHOR : Andrew Haisley
 *
 * This file
 *
 * DESCRIPTION:
 *
 *  A file permissions editing dialog
 *
 *
 *
 *
 */

#include "mx_dialog.h"
#include <Xm/Xm.h>
#include <mx.h>
#include <mx_file_type.h>
#include <mx_ui_object.h>
// #include "mx_yes_no_d.h"

class mx_finfo_d : public mx_dialog {

public:
    mx_finfo_d(const char* name, Widget parent);
    virtual void activate_d(char* file_name);

    char current_file_name[MAX_PATH_LEN];

    Widget ureadToggle, uwriteToggle;
    Widget greadToggle, gwriteToggle;
    Widget oreadToggle, owriteToggle;

private:
    Widget nameWidget;
    Widget typeWidget;
    Widget ownerWidget;
    Widget groupWidget;
    Widget dateWidget;

private:
};

#endif
