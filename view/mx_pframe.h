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
#ifndef _MX_PFRAME_H
#define _MX_PFRAME_H

/*
 * MODULE : mx_pframe.h
 *
 * AUTHOR : David Miller
 *
 * This file
 *
 * DESCRIPTION:
 * Module mx_pframe.h
 *
 *
 */

#include "mx_cb.h"
#include "mx_frame.h"

/*-------------------------------------------------
 * CLASS: mx_print_frame
 *
 * DESCRIPTION: A frame which has an x widget and
 * x callbacks. This is not a ui object as by default it
 * sets the active widget to be the parent widget. The
 * screen device is attached to the active widget
 */

class mx_print_frame : public mx_frame, public mx_callback_target {
public:
    mx_print_frame(int& err,
        mx_doc_coord_t& initialTopLeft,
        mx_point& initialSize,
        double initialZoom,
        double printerResolution,
        mx_frame_target& iframeTarget);

    void setSheet(int& err,
        int sheet_number);

    void printSheet(int& err,
        int sheet_number);

    void createDevice(int& err);

    void draw(int& err, mx_draw_event& event);

    static void defaultDeviceToFrame(int& err,
        mx_callback_target* source,
        mx_callback_target* dest,
        mx_doc_coord_t& coord,
        MX_COORD_TYPE coordType,
        bool& isInRange);

    static void defaultFrameToFrameTarget(int& err,
        mx_callback_target* source,
        mx_callback_target* dest,
        mx_doc_coord_t& coord,
        MX_COORD_TYPE coordType,
        bool& isInRange);

    void resetSheetLayout(int& err);

protected:
private:
    int currentSheet;
    double printerResolution;
    mx_point printerSize;
};

#endif
