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
 * MODULE : mx_pframe.C
 *
 * AUTHOR : David Miller
 *
 * This file
 *
 * DESCRIPTION:
 * Module mx_pframe.C
 *
 */

#include <mx_pframe.h>
#include <mx_pr_device.h>

/*-------------------------------------------------
 * FUNCTION: mx_print_frame::defaultDeviceToFrame
 *
 * DESCRIPTION: Routine to convert from screen device coodinates
 * to x frame coordinates
 *
 */

void mx_print_frame::defaultDeviceToFrame(int& err,
    mx_callback_target* source,
    mx_callback_target* dest,
    mx_doc_coord_t& coord,
    MX_COORD_TYPE coordType,
    bool& isInRange)
{
    err = MX_ERROR_OK;

    mx_print_device* printDevice = (mx_print_device*)source;

    coord = printDevice->dev2frame(coord);
}

/*-------------------------------------------------
 * FUNCTION: mx_print_frame::defaultFrameToFrameTarget
 *
 * DESCRIPTION: Routine to convert from print device coodinates
 * to print frame coordinates
 *
 */

void mx_print_frame::defaultFrameToFrameTarget(int& err,
    mx_callback_target* source,
    mx_callback_target* dest,
    mx_doc_coord_t& coord,
    MX_COORD_TYPE coordType,
    bool& isInRange)
{
    mx_print_frame* frame = (mx_print_frame*)source;
    mx_point pt = coord.p;

    isInRange = frame->coordToDocCoord(err, pt, coord, coordType);
    MX_ERROR_CHECK(err);

    return;
abort:
    return;
}

/*-------------------------------------------------
 * FUNCTION: mx_print_frame::mx_print_frame
 *
 * DESCRIPTION: Frame constructor for a simple
 * frame which can hold a device
 *
 */

mx_print_frame::mx_print_frame(int& err,
    mx_doc_coord_t& initialTopLeft,
    mx_point& initialSize,
    double initialZoom,
    double iprinterResolution,
    mx_frame_target& iframeTarget)
    : mx_frame(err,
          initialTopLeft,
          initialSize,
          initialZoom,
          iframeTarget)
{
    // check error code
    MX_ERROR_CHECK(err);

    currentSheet = 0;

    setProcessMask(MX_EVENT_STANDARD);

    // set the frame to transmit events to the frame target

    iframeTarget.setToReceiveEvents(err, this, defaultFrameToFrameTarget);
    MX_ERROR_CHECK(err);

    // save the initial size and printer resolution for creation
    // of the device
    printerSize = initialSize;
    printerResolution = iprinterResolution;

    // now create the printer device
    createDevice(err);
    MX_ERROR_CHECK(err);

    return;
abort:
    return;
}

/*-------------------------------------------------
 * FUNCTION: mx_print_frame::draw
 *
 * DESCRIPTION: Frame constructor for a simple
 * frame which can hold a device
 *
 */

void mx_print_frame::draw(int& err,
    mx_draw_event& event)
{
    mx_draw_event newEvent = event;
    mx_doc_rect_t docRect;
    mx_doc_coord_t coord;
    mx_doc_coord_t tl = event.getTL();
    mx_doc_coord_t br;

    err = MX_ERROR_OK;

    // set the routine to draw one sheet - based on
    // the top left and the current sheet number
    tl.p = getFrameTarget().getSheetLayout().getPosition(err,
        currentSheet);
    MX_ERROR_CHECK(err);

    br.p = tl.p + getFrameTarget().getSheetLayout().getSize(err, currentSheet);
    MX_ERROR_CHECK(err);

    // set the device on the output device
    newEvent.setDevice(getDevice());

    // set the new event just for this sheet
    newEvent.setRect(tl, br);

    // propagate the event to print the sheet
    mx_callback_target::draw(err, newEvent);
    MX_ERROR_CHECK(err);

    return;
abort:
    return;
}

/*-------------------------------------------------
 * FUNCTION: mx_print_frame::createDevice
 *
 * DESCRIPTION: Frame constructor for a simple
 * frame which can hold a device
 *
 */

void mx_print_frame::createDevice(int& err)
{
    err = MX_ERROR_OK;

    // create the print device with the given size and resolution
    outputDevice = new mx_print_device(err, *this, printerSize, printerResolution);
    MX_ERROR_CHECK(err);

    // set this frame to receive events from the device
    setToReceiveEvents(err, (mx_print_device*)outputDevice, defaultDeviceToFrame);
    MX_ERROR_CHECK(err);

    // get the size of the device in frame coordinates
    isetSize(err, printerSize);
    MX_ERROR_CHECK(err);

    return;
abort:
    return;
}

/*-------------------------------------------------
 * FUNCTION: mx_print_frame::setSheet
 *
 * DESCRIPTION: Set the current sheet
 *
 *
 */

void mx_print_frame::setSheet(int& err,
    int sheetNumber)
{
    mx_doc_coord_t docCoord;
    mx_point coord;

    err = MX_ERROR_OK;

    currentSheet = sheetNumber;

    docCoord.sheet_number = sheetNumber;

    docCoordToCoord(err, docCoord, coord);
    MX_ERROR_CHECK(err);

    setTopLeft(err, coord);
    MX_ERROR_CHECK(err);

    return;
abort:
    return;
}

/*-------------------------------------------------
 * FUNCTION: mx_print_frame::resetSheetLayout
 *
 * DESCRIPTION: Frame constructor for a simple
 * frame which can hold a device in a scrollable
 * window
 */

void mx_print_frame::resetSheetLayout(int& err)
{
    err = MX_ERROR_OK;
}

/*-------------------------------------------------
 * FUNCTION: mx_print_frame::printSheet
 *
 * DESCRIPTION: Print the current sheet
 *
 *
 */

void mx_print_frame::printSheet(int& err,
    int sheetNumber)
{
    // set the frame onto the current sheet
    setSheet(err, sheetNumber);
    MX_ERROR_CHECK(err);

    // and draw
    outputDevice->refresh(err);
    MX_ERROR_CHECK(err);

    return;
abort:
    return;
}
