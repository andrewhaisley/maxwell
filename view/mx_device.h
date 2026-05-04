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
#ifndef _MX_DEVICE_H
#define _MX_DEVICE_H

/*
 * MODULE : mx_device.h
 *
 * AUTHOR : David Miller
 *
 * This file
 *
 * DESCRIPTION: The generic device for drawing
 * Module mx_device.h
 *
 *
 *
 */

class mx_frame;

#include <mx.h>
#include <mx_doc_arc.h>
#include <mx_doc_coord.h>
#include <mx_doc_poly.h>
#include <mx_doc_rect.h>
#include <mx_geometry.h>
#include <mx_spline.h>
#include <mx_style.h>

//
// the gravity to use for redraws
// The gravity in a scroll tells you which
// corner needs to be totally redrawn and is
// not already part of the display
//

typedef enum {
    MX_DEVICE_GRAVITY_UNDEFINED,
    MX_DEVICE_GRAVITY_TL,
    MX_DEVICE_GRAVITY_TR,
    MX_DEVICE_GRAVITY_BL,
    MX_DEVICE_GRAVITY_BR
} MX_DEVICE_GRAVITY;

//
// The type of colour which the device can
// cope with
//

typedef enum {
    MX_DEVICE_RGB,
    MX_DEVICE_LUT,
    MX_DEVICE_BIT,
    MX_DEVICE_GREY
} MX_DEVICE_RASTER;

// A text item which indicates a chunk of text in a given character style.
// Used for drawing whole lines of text at once
struct mx_text_item {
    inline mx_text_item()
    {
        chars = NULL;
        num_chars = 0;
        delta = 0.0f;
        mod = NULL;
    };

    const char* chars;
    uint32 num_chars;
    float delta;
    const mx_char_style_mod* mod;
};

/*-------------------------------------------------
 * CLASS: mx_device
 *
 * DESCRIPTION: The basic generic device
 *
 *
 */

class mx_device : public mx_rtti {
    MX_RTTI(mx_device_class_e)

public:
    //***************************************************************
    //
    //  Constructor
    //
    //***************************************************************

    mx_device(mx_frame& iframe);

    virtual ~mx_device();

    //***************************************************************
    //
    //  Routines for the size of the device
    //
    //***************************************************************

    inline double getScreenResolution() { return screenResolution; };
    mx_point getMMsize() { return getDevicePixelSize() / screenResolution; };
    mx_point getFrameSize();
    virtual mx_ipoint getDevicePixelSize() = 0;

    //***************************************************************
    //
    //  Routines to drive clipping
    //
    //***************************************************************

    // pop a clip mask
    virtual void popClipMask(int& err) = 0;

    void pushClipRect(int& err,
        const mx_doc_coord_t& topLeft,
        const mx_doc_coord_t& bottomRight,
        const mx_angle& angle,
        bool doIntersect);

    // clip to rectangle in pixels
    virtual void pushClipRect(int& err,
        const mx_irect& coords,
        const mx_angle& angle,
        bool doIntersect)
        = 0;

    // clip to lines
    void pushClipPolypoint(int& err,
        mx_doc_polypoint& pp,
        bool doIntersect);

    // clip to lines in pixels
    virtual void pushClipPolypoint(int& err,
        const mx_ipolypoint& pp,
        bool doIntersect)
        = 0;

    // clip to arc
    void pushClipArc(int& err,
        mx_doc_arc& a,
        bool doIntersect);

    virtual void pclipArc(int& err,
        mx_ipoint& centre,
        mx_ipoint& size,
        bool isFull,
        double startAng,
        double endAng,
        mx_angle& angle,
        bool doIntersect) { err = MX_ERROR_OK; };

    // clip to sector
    void pushClipSector(int& err,
        mx_doc_arc& a,
        bool doIntersect);

    virtual void pclipSector(int& err,
        mx_ipoint& centre,
        mx_ipoint& size,
        bool isFull,
        double startAng,
        double endAng,
        mx_angle& angle,
        bool doIntersect) { err = MX_ERROR_OK; };

    //***************************************************************
    //
    //  Routines for basic drawing. In general the drawing routines
    //  come in two forms - one which is driven from a frame target
    //  and one is a pure pixel based driver.
    //
    //  The frame drivable target has routines which live in the
    //  device. These call the pixel driven routines which are
    //  pure virtual functions. These pixel driven functions should
    //  therefore be pretty simple.
    //
    //***************************************************************

    // draw a rectangle in maxwell coordinates
    virtual void fillRect(int& err,
        const mx_doc_coord_t& topLeft,
        const mx_doc_coord_t& bottomLeft,
        const mx_angle& angle,
        mx_fill_style* fillStyle = NULL);

    // draw a rectangle in pixel device coordinates
    virtual void pfillRect(int& err,
        const mx_ipoint& topLeft,
        const mx_ipoint& bottomLeft,
        const mx_angle& angle)
        = 0;

    // xor a rectangle in maxwell coordinates
    virtual void xorRect(int& err,
        mx_doc_coord_t& topLeft,
        mx_doc_coord_t& bottomLeft);

    // xor a rectangle in pixel device coordinates
    virtual void pxorRect(int& err,
        const mx_ipoint& topLeft,
        const mx_ipoint& bottomLeft)
        = 0;

    // if you supply a style, it becomes the default for subsequent
    // operations
    virtual void drawLine(int& err,
        const mx_doc_coord_t& start,
        const mx_doc_coord_t& end,
        mx_line_style* lineStyle = NULL);

    // draw a line in pixel device coordinates
    virtual void pdrawLine(int& err,
        const mx_ipoint& start,
        const mx_ipoint& end)
        = 0;

    virtual void drawRect(int& err,
        const mx_doc_coord_t& topLeft,
        const mx_doc_coord_t& bottomLeft,
        const mx_angle& angle,
        mx_line_style* lineStyle = NULL);

    // draw a line in pixel device coordinates
    virtual void pdrawRect(int& err,
        const mx_ipoint& start,
        const mx_ipoint& end,
        const mx_angle& angle)
        = 0;

    // draw an arc in pixel coordinates
    virtual void pdrawArc(int& err,
        mx_ipoint& centre,
        mx_ipoint& size,
        bool isFull,
        double startAng,
        double endAng,
        const mx_angle& angle) { err = MX_ERROR_OK; };

    virtual void pfillArc(int& err,
        mx_ipoint& centre,
        mx_ipoint& size,
        bool isFull,
        double startAng,
        double endAng,
        const mx_angle& angle) { err = MX_ERROR_OK; };

    virtual void pdrawChord(int& err,
        mx_ipoint& centre,
        mx_ipoint& size,
        bool isFull,
        double startAng,
        double endAng,
        const mx_angle& angle) { err = MX_ERROR_OK; };

    virtual void pdrawSector(int& err,
        mx_ipoint& centre,
        mx_ipoint& size,
        bool isFull,
        double startAng,
        double endAng,
        const mx_angle& angle) { err = MX_ERROR_OK; };

    virtual void pfillSector(int& err,
        mx_ipoint& centre,
        mx_ipoint& size,
        bool isFull,
        double startAng,
        double endAng,
        const mx_angle& angle) { err = MX_ERROR_OK; };

    virtual void drawArc(int& err,
        mx_doc_arc& a,
        mx_line_style* lineStyle = NULL);

    virtual void fillArc(int& err,
        mx_doc_arc& a,
        mx_fill_style* fillStyle = NULL);

    virtual void drawChord(int& err,
        mx_doc_arc& a,
        mx_line_style* lineStyle = NULL);

    virtual void drawSector(int& err,
        mx_doc_arc& a,
        mx_line_style* lineStyle = NULL);

    virtual void fillSector(int& err,
        mx_doc_arc& a,
        mx_fill_style* fillStyle = NULL);

    // if you supply a style, it becomes the default for subsequent
    // operations
    virtual void drawCircle(int& err,
        mx_doc_coord_t& centre,
        float radius,
        mx_line_style* lineStyle = NULL);

    // draw a circle in pixel device coordinates
    virtual void pdrawCircle(int& err,
        const mx_ipoint& centre,
        int radius)
        = 0;

    virtual void fillCircle(int& err,
        const mx_doc_coord_t& centre,
        float radius,
        mx_fill_style* fillStyle = NULL);

    virtual void pfillCircle(int& err,
        const mx_ipoint& centre,
        int radius)
        = 0;

    virtual void drawPolypoint(int& err,
        mx_doc_polypoint& pp,
        mx_line_style* lineStyle = NULL);

    virtual void pdrawPolypoint(int& err,
        const mx_ipolypoint& pp)
        = 0;

    void drawPoints(int& err,
        mx_doc_polypoint& pp);

    virtual void pdrawPoints(int& err,
        const mx_ipolypoint& pp)
        = 0;

    void drawSegments(int& err,
        mx_doc_polypoint& pp,
        bool linesCoincident,
        mx_line_style* lineStyle = NULL);

    virtual void pdrawSegments(int& err,
        const mx_ipolypoint& pp,
        bool linesCoincident)
        = 0;

    virtual void fillPolypoint(int& err,
        mx_doc_polypoint& pp,
        mx_fill_style* fillStyle = NULL);

    virtual void pfillPolypoint(int& err,
        const mx_ipolypoint& pp)
        = 0;

    // draw some text in a given character style
    virtual void drawText(int& err,
        const char* s,
        const mx_doc_coord_t& pos,
        mx_char_style& charStyle);

    // draw some text in a given character style, using a string of a given
    // length
    virtual void drawText(int& err,
        const char* s,
        uint32 s_len,
        const mx_doc_coord_t& pos,
        mx_char_style& charStyle);

    // returns the pixel width of the text drawn
    virtual int pdrawText(int& err,
        const char* s,
        uint32 s_len,
        mx_ipoint pos,
        mx_char_style& charStyle)
        = 0;

    // draw a line of text in one go
    virtual void drawTextLine(int& err,
        mx_doc_coord_t& pos,
        mx_char_style& init_char_style,
        const mx_text_item* text_items,
        uint32 num_items);

    // draw a line of text in one go
    virtual void pdrawTextLine(int& err,
        const mx_ipoint& pos,
        const mx_char_style& init_char_style,
        const mx_text_item* text_items,
        uint32 num_items)
        = 0;

    virtual void drawTile(int& err,
        MX_DEVICE_RASTER tileType,
        int nsigbits,
        int nunpackedBits,
        mx_ipoint& tileSize,
        mx_doc_coord_t& topLeft,
        void* tile,
        mx_irect* clipRect = NULL);

    virtual void pdrawTile(int& err,
        MX_DEVICE_RASTER tileType,
        int nsigbits,
        int nunpackedBits,
        mx_ipoint& tileSize,
        mx_ipoint& topLeft,
        void* tile,
        mx_irect* clipRect = NULL)
        = 0;

    // draw a set of "resize handles", ie eight little boxes at the corner
    // of an unfilled reactangle. Done all in xormode so it can be undone
    // quickly by doing another call with the same parameters
    virtual void xorResizeHandles(int& err,
        const mx_doc_coord_t& topLeft,
        const mx_doc_coord_t& bottomRight,
        double handleWidth);

    // note default is to do nothing for this routine
    virtual void pxorResizeHandles(int& err,
        const mx_ipoint& topLeft,
        const mx_ipoint& bottomRight,
        uint32 handleWidth);

    // returns the maxwell width of a character according to a given device
    virtual float getCharWidth(int& err,
        char c,
        mx_char_style& cs);

    // returns the pixel width of a character according to a given device
    virtual uint32 pgetCharWidth(int& err,
        char c,
        const mx_char_style& cs)
        = 0;

    virtual void outputImageHeader(int& err,
        mx_point& topLeftOffset,
        mx_point& realSize,
        MX_DEVICE_RASTER deviceType,
        int nbits,
        int nplanes,
        int ncolours,
        mx_colour* colours,
        mx_ipoint& imageSize,
        bool clipSet)
        = 0;

    virtual void outputImageTrailer(int& err,
        bool clipSet)
        = 0;

    virtual int depth() { return 8; };
    //***************************************************************
    //
    // Routines to convert between device (pixel) coordinates
    // to frame coordinates (all values on one sheet) and frame
    // target coordinates (these can be on multiple sheets)
    //
    //***************************************************************

    // convert lengths between the device and the frame
    double dev2frame(double devLength);
    double frame2dev(double frameLength);

    // convert between device and frame coordinates
    mx_doc_coord_t dev2frame(const mx_doc_coord_t& devPoint);
    mx_doc_coord_t frame2dev(const mx_doc_coord_t& framePoint);

    // convert between device and frame target coordinates
    mx_doc_coord_t frameTarget2dev(int& err,
        const mx_doc_coord_t& frameTargetCoord);
    mx_doc_coord_t dev2frameTarget(int& err,
        const mx_doc_coord_t& devCoord);

    // convert between device, frame and frame target coordinates
    // snapping onto pixels

    mx_doc_coord_t pixelSnapFrame(const mx_doc_coord_t& position);

    mx_doc_coord_t pixelSnapFrameTarget(int& err,
        const mx_doc_coord_t& position);

    mx_ipoint pixelSnap(const mx_doc_coord_t& position);

    //***************************************************************
    //
    // Routines to drive redrawing and scrolling of a device
    //
    //***************************************************************

    virtual void setShiftOrigin(mx_doc_coord_t& wantedShiftOrigin,
        mx_doc_coord_t& gotShiftOrigin,
        bool doScroll)
        = 0;

    virtual void makeGap(int& err,
        bool doX,
        mx_point& frameStartPoint,
        double frameGapSize)
        = 0;

    virtual void closeGap(int& err,
        bool doX,
        mx_point& frameStartPoint,
        double frameGapSize)
        = 0;

    // shift the contents of a window in frame coordinates (used in scrolling and repositioning)
    virtual void shift(int& err) = 0;

    virtual void refresh(int& err,
        mx_point* topLeft = NULL,
        mx_point* bottomRight = NULL)
        = 0;

    virtual void setDrawToWindowOnly(bool idrawToWindowOnly) { };

    //***************************************************************
    //
    // Routines to set line and fill styles on the device. These
    // become the defaults for subsequent operations.
    //
    //***************************************************************

    virtual void setDefaultLineStyle(int& err,
        mx_line_style& s)
        = 0;

    virtual void setDefaultFillStyle(int& err,
        mx_fill_style& s)
        = 0;

    //***************************************************************
    //
    // Routines to manipulate colours on the device
    //
    //***************************************************************

    virtual void registerColour(mx_colour* colour, LUT_VALUE* pixel) = 0;

    virtual void setForegroundColour(int& err, LUT_VALUE pixel) = 0;
    virtual void setBackgroundColour(int& err, LUT_VALUE pixel) = 0;

    virtual void setForegroundColour(int& err, mx_colour& c) = 0;
    virtual void setBackgroundColour(int& err, mx_colour& c) = 0;

    mx_frame* getFrame() { return &frame; };

    inline bool getRGB() { return doesRGB; };
    inline bool getLUT() { return doesLUT; };
    inline bool getBit() { return doesBit; };

    double frameResolution();

    virtual void setXor(int& err, bool setOn) = 0;

protected:
    // set the device resolution
    inline void setScreenResolution(double ires) { screenResolution = ires; };

    // see if does RGB
    bool doesRGB;
    bool doesLUT;
    bool doesBit;

    bool drawsChord;
    bool drawsSector;
    bool fillsSector;

    bool drawsBeziers;
    bool fillsBeziers;

    bool closesLinesWithoutLastPoint;

    bool clipsArc;
    bool clipsSector;

    bool doesRotate;
    bool doesAffine;

private:
    // the number of pixels per MM
    double screenResolution;

    //***************************************************************
    //
    // The frame of the device. The frame contains the information
    // which allows the device to be positioned within a document.
    // Every device must be attached to a frame
    //
    //***************************************************************

    mx_frame& frame;

    // a global polypoint to be used for drawing things which we
    // need to approximate as the device itself does not support
    // e.g. beziers,sectors

    static mx_ipolypoint ippline;
    static mx_polypoint ppline;
    static mx_doc_polypoint docppline;

    void arcPoints(int& err,
        mx_doc_arc& a,
        mx_ipoint& centre,
        mx_ipoint& size);

    void translatePolypoint(int& err,
        mx_doc_polypoint& dpp,
        bool addClosurePoint,
        mx_ipolypoint& ipp);

    inline bool setNewLineStyle(mx_line_style* ls)
    {
        return ((ls != NULL) && (ls->line_type != mx_use_lstyle));
    };

    inline bool invalidLineStyle(mx_line_style* ls)
    {
        return ((ls != NULL) && (ls->line_type == mx_no_line));
    };

    inline bool setNewFillStyle(mx_fill_style* fs)
    {
        return ((fs != NULL) && (fs->type != mx_use_fstyle));
    };

    inline bool invalidFillStyle(mx_fill_style* fs)
    {
        return ((fs != NULL) && (fs->type == mx_fill_transparent));
    };
};

#endif
