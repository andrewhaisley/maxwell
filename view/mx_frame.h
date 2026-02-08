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
#ifndef _MX_FRAME_H
#define _MX_FRAME_H

/*
 * MODULE : mx_frame.h
 *
 * AUTHOR : David Miller
 *
 * This file
 *
 * DESCRIPTION:
 * Module mx_frame.h
 *
 *
 *
 *
 */

#include "mx_ftarget.h"
class mx_device;

/*-------------------------------------------------
 * CLASS: mx_frame
 *
 * DESCRIPTION: A class for holding a screen device.
 * The frame can be extended to be a scrollable frame
 * The important aspect of the frame is to allow
 * document coordinates to be turned into screen pixels
 *
 * The frames coordinate system is as a single sheet
 *
 * The basic frame can be used as a dummy frame to
 * place around a frameable object whilst it is
 * being printed. It is a light weight object
 * - different from the mx_xframe and the
 * mx_scrollable_xframe which do a lot more work
 *
 */

class mx_device;

class mx_frame {
public:
    // Constructors/destructors
    mx_frame(int& err,
        const mx_doc_coord_t& initialTopLeft,
        const mx_point& initialSize,
        double initialZoom,
        mx_frame_target& iframeTarget);

    virtual ~mx_frame();

    // force the frame to resize its scrollbars
    // this routine will get the sheet layout from
    // the frameable object and adjust the scrollbars
    // appropriately - this is called by an editor to
    // only chnage the size of the scrollbars and the
    // scrollbar position.

    // this routine will look at the sheet layout on
    // the frameable object

    virtual void resetSheetLayout(int& err) = 0;

    // a frame creates and uses a device - the device knows
    // such things as what is being displayed

    // get the (screen ? ) device
    inline mx_device* getDevice() { return outputDevice; };

    // refresh the view
    void refresh(int& err,
        mx_doc_coord_t* topLeft = NULL,
        mx_doc_coord_t* bottomRight = NULL);

    // create the device for this frame - the device could
    // be different for different devices - the device will
    // need to know about the frame as it needs to convert
    // events in its coordinate system to events in the
    // doc coord system
    virtual void createDevice(int& err) = 0;

    virtual void resize(int& err, mx_resize_event& event);

    void setTopLeftFrame(int& err, const mx_doc_coord_t& frameCoord);

    inline void getTopLeftFrame(int& err, mx_doc_coord_t& frameCoord)
    {
        coordToDocCoord(err, topLeft, frameCoord, MX_COORD_TL);
    };

    virtual void setTopLeft(int& err, const mx_point& itopLeft);
    virtual void setSize(int& err, const mx_point& isize);
    virtual void setZoom(int& err, double izoom);

    inline mx_point getTopLeft() { return topLeft; };
    inline mx_point getSize() { return size; };
    inline double getZoom() { return zoom; };

    // get the current external selection
    virtual void requestSelection(int& err);

    // tell the frame an external selection is available
    virtual void externalSelectionAvailable();

    // convert a coordinate in the set of sheets to a doc coord
    inline bool coordToDocCoord(int& err,
        const mx_point& coord,
        mx_doc_coord_t& docCoord,
        MX_COORD_TYPE coordType)
    {
        return frameTarget.coordToDocCoord(err, coord, docCoord, coordType);
    };

    // convert a doc coord to a cumulative coord
    inline bool docCoordToCoord(int& err,
        const mx_doc_coord_t& docCoord,
        mx_point& coord)
    {
        return frameTarget.docCoordToCoord(err, docCoord, coord);
    };

    inline mx_frame_target& getFrameTarget() { return frameTarget; };

    void clippedTopLeftCoord(int& err,
        mx_point& topLeftCoord);

    inline bool getApplyScrollLimitToTarget()
    {
        return applyScrollLimitToTarget;
    }
    inline bool getApplyScrollLimit() { return applyScrollLimit; };

    inline mx_rect getScrollLimitBox() { return scrollLimitBox; };
    inline mx_rect getTargetLimitAdd() { return targetLimitAdd; };

    inline void setApplyScrollLimitToTarget(bool ilimit)
    {
        applyScrollLimitToTarget = ilimit;
        applyScrollLimit = ilimit;
    };
    inline void setApplyScrollLimit(bool ilimit)
    {
        applyScrollLimit = ilimit;
    };

    inline void setScrollLimitBox(const mx_rect& irect)
    {
        scrollLimitBox = irect;
    };

    inline void setTargetLimitAdd(const mx_rect& inrect)
    {
        targetLimitAdd = inrect;
    };

    // force a scroll - here scroll position is
    // a coordinate that is to be scrolled to
    // offset is one of the values in MX_SCROLL_OFFSET
    // (e.g. if offset if MX_SCROLL_OFFSET_C then
    // the scrolled position. For various reasons the
    // window may not be able to scroll to the position
    // required - it therefore returns the position it
    // actually has scrolled to

    virtual void scroll(int& err,
        mx_doc_coord_t& scrollPostion,
        MX_SCROLL_OFFSET offset,
        bool sameX,
        bool sameY,
        mx_doc_coord_t& actualScrollPosition);

    inline void setDrawPerSheet(bool idrawPerSheet) { drawPerSheet = idrawPerSheet; };
    inline bool getDrawPerSheet() { return drawPerSheet; };

    // routines for setting and clearing false origins
    void setFalseOrigin(int& err, const mx_doc_coord_t& ifalseOrigin);

    // routines for setting and clearing false origins
    inline void setFalseOrigin(const mx_point& ifalseOrigin)
    {
        useFalseOrigin = TRUE;
        falseOrigin = ifalseOrigin;
    }

    inline bool getFalseOriginSet() { return useFalseOrigin; };
    inline mx_point getFalseOrigin() { return falseOrigin; };
    mx_doc_coord_t getDocFalseOrigin(int& err);
    inline void clearFalseOrigin() { useFalseOrigin = FALSE; };

    inline void isetZoom(double izoom) { zoom = izoom; };
    void isetSize(int& err, const mx_point& newSize);
    inline void isetTopLeft(mx_point const& itopLeft) { topLeft = itopLeft; };

    // shift some data - note gapSize can be negative - in which
    // case the gap can eat old space
    // this routine works in document documents

    void makeGap(int& err,
        bool doX,
        mx_doc_coord_t& docCoord,
        double docGapSize);

    void closeGap(int& err,
        bool doX,
        mx_doc_coord_t& docCoord,
        double docGapSize);

protected:
    // the output device
    mx_device* outputDevice;

    // the top left of the device in the
    // coordinates of the frame (e.g sheet_number is always 0 )
    mx_point topLeft;
    mx_point size;

    virtual void frameScroll(int& err,
        mx_doc_coord_t& scrollPostion,
        MX_SCROLL_OFFSET offset,
        bool sameX,
        bool sameY,
        mx_doc_coord_t& actualScrollPosition,
        bool doScroll);

private:
    // the scale factor between the object and how it should be
    // displayed
    double zoom;

    // scroll limit box
    bool applyScrollLimit;
    bool applyScrollLimitToTarget;
    mx_rect scrollLimitBox;
    mx_rect targetLimitAdd;

    bool drawPerSheet;

    // false origins - used to convert from frame to device coordinates
    // with an offset - not used for device to frame
    bool useFalseOrigin;
    mx_point falseOrigin;

    // the thing in the frame
    mx_frame_target& frameTarget;
};

#endif
