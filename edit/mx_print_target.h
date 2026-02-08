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
 * MODULE/CLASS :  mx_print_target
 *
 * AUTHOR : Andrew Haisley
 *
 *
 *
 * DESCRIPTION:
 *
 *  a target for the print frame
 *
 *
 *
 *
 *
 */
#ifndef MX_PRINT_TARGET
#define MX_PRINT_TARGET

#include <mx.h>
#include <mx_pframe.h>
#include <mx_rtti.h>
#include <mx_wp_lay.h>

class mx_wp_document;

class mx_print_target : public mx_frame_target {
public:
    mx_print_target(int& err, mx_sheet_layout* layout, mx_wp_document* doc);

    // all it can do is draw things
    void draw(int& err, mx_draw_event& event);

private:
    mx_wp_document* document;
};

#endif
