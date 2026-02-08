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
 * MODULE : mx_sc_ras.C
 *
 * AUTHOR : David Miller
 *
 * This file
 *
 * DESCRIPTION:
 * Module mx_sc_ras.C
 *
 *
 * Revision 1.1  1996/10/06 13:15:45  andrew
 * Initial revision
 *
 *
 */

#include <mx_sc_device.h>

/*-------------------------------------------------
 * FUNCTION: mx_sc_device::pdrawTile
 *
 * DESCRIPTION:
 *
 *
 */

void mx_screen_device::pdrawTile(int& err,
    MX_DEVICE_RASTER tileType,
    int nsigbits,
    int nunpackedBits,
    mx_ipoint& tileSize,
    mx_ipoint& topLeft,
    void* tile,
    mx_irect* clipRect)
{
    XImage* xImage;
    mx_ipoint size, src;

    err = MX_ERROR_OK;

    switch (tileType) {
    case MX_DEVICE_BIT:
        xImage = XCreateImage(display,
            DefaultVisual(display,
                DefaultScreen(display)),
            1, XYBitmap,
            0, (char*)tile,
            tileSize.x,
            tileSize.y,
            8, (tileSize.x + 7) / 8);
        break;

    case MX_DEVICE_LUT:

        xImage = XCreateImage(display,
            DefaultVisual(display,
                DefaultScreen(display)),
            depth(), ZPixmap,
            0, (char*)tile,
            tileSize.x,
            tileSize.y,
            depth(), (tileSize.x * depth()) / 8);

        break;
    default:
        return;
        break;
    }

    xImage->byte_order = LSBFirst;
    xImage->bitmap_bit_order = LSBFirst;

    if (clipRect != NULL) {
        src.x = clipRect->xb;
        src.y = clipRect->yb;

        size.x = clipRect->xt - clipRect->xb + 1;
        size.y = clipRect->yt - clipRect->yb + 1;
    } else {
        src.x = 0;
        src.y = 0;
        size.x = xImage->width;
        size.y = xImage->height;
    }

    if (drawToWindowOnly) {
        XPutImage(display, window,
            gc, xImage, src.x, src.y,
            topLeft.x, topLeft.y, size.x, size.y);
    } else {
        XPutImage(display, storePixmap,
            gc, xImage, src.x, src.y,
            topLeft.x, topLeft.y, size.x, size.y);
    }

    xImage->data = NULL;
    XDestroyImage(xImage);

    if (doMonitor && (!drawToWindowOnly)) {
        mx_irect trect;

        trect.xt = topLeft.x;
        trect.xb = topLeft.x + size.x;
        trect.yt = topLeft.y;
        trect.yb = topLeft.y + size.y;

        trect.normalise();

        if (monitorRect.xb > trect.xb)
            monitorRect.xb = trect.xb;
        if (monitorRect.xb > trect.yb)
            monitorRect.yb = trect.yb;

        if (monitorRect.xt < trect.xt)
            monitorRect.xt = trect.xt;
        if (monitorRect.yt < trect.yt)
            monitorRect.yt = trect.yt;
    }

    return;
}
