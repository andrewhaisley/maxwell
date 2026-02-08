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
 * MODULE : mx_ftarget.C
 *
 * AUTHOR : David Miller
 *
 * This file
 *
 * DESCRIPTION:
 * Module mx_ftarget.C
 *
 *
 *
 *
 */

#include "mx_ftarget.h"
#include "mx_wp_lay.h"

/*-------------------------------------------------
 * FUNCTION: mx_frame_target::mx_frame_target
 *
 * DESCRIPTION:
 *
 *
 */

mx_frame_target::mx_frame_target(mx_sheet_layout* layout)
{
    sheet_layout_allocated = FALSE;
    sheetLayout = layout;

    // set to receive the standard events
    setProcessMask(MX_EVENT_STANDARD);
}

mx_frame_target::mx_frame_target()
{
    sheet_layout_allocated = TRUE;
    sheetLayout = get_simple_layout();

    // set to receive the standard events
    setProcessMask(MX_EVENT_STANDARD);
}

mx_frame_target::~mx_frame_target()
{
    if (sheet_layout_allocated)
        delete sheetLayout;
}

mx_sheet_layout* mx_frame_target::get_simple_layout()
{
    mx_sheet_size sheet(mx_point(10000, 10000));
    mx_wp_layout* res;

    int err = MX_ERROR_OK;

    res = new mx_wp_layout;

    res->setSnapTolerance(0.00001);

    res->addSheets(err, 0, 1, &sheet);
    MX_ERROR_CHECK(err);

    return (mx_sheet_layout*)res;
abort:
    return (mx_sheet_layout*)res;
}

void mx_frame_target::setSheetLayout(mx_sheet_layout* l)
{
    if (sheet_layout_allocated)
        delete sheetLayout;
    sheet_layout_allocated = FALSE;
    sheetLayout = l;
}
