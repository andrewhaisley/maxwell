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
 * MODULE/CLASS : mx_painter
 *
 * AUTHOR : Andrew Haisley
 *
 *
 *
 * DESCRIPTION:
 *
 * A class of things that paint other things onto devices
 *
 *
 *
 *
 *
 */
#include <mx.h>
#include <mx_device.h>
#include <mx_doc_coord.h>
#include <mx_painter.h>

mx_painter_mode_t mx_painter::draw_mode = mx_paint_wp_normal_e;
mx_device* mx_painter::device = NULL;
mx_grid_paint mx_painter::grid_paint;

mx_painter::mx_painter()
{
    device = NULL;
}

mx_painter::~mx_painter()
{
}

void mx_painter::xorRect(
    int& err,
    mx_doc_coord_t& tl,
    mx_doc_coord_t& br)
{
    mx_ipoint itl = device->pixelSnap(device->frame2dev(tl));
    mx_ipoint ibr = device->pixelSnap(device->frame2dev(br));

    itl.y++;

    device->pxorRect(err, itl, ibr);
    MX_ERROR_CHECK(err);

abort:;
}
