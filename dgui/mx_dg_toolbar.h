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
#ifndef MX_DG_TOOLBAR_H
#define MX_DG_TOOLBAR_H
/*
 * MODULE/CLASS : mx_dg_toolbar
 *
 * AUTHOR : David Miller
 *
 *
 *
 * DESCRIPTION:
 *   The diagram editor toolbar
 *
 */

#include "mx_bar.h"
#include <Xm/Xm.h>
#include <mx.h>
#include <mx_style.h>
#include <mx_window.h>
#include <mx_xframe.h>

class mx_dg_toolbar : public mx_bar, public mx_frame_target {
    //    MX_RTTI(mx_dg_toolbar_class_e)

public:
    mx_dg_toolbar(mx_window* window);
    ~mx_dg_toolbar();

private:
};

#endif // MX_DG_TOOLBAR_H
