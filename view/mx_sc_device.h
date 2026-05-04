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
#ifndef _MX_SC_DEVICE_H
#define _MX_SC_DEVICE_H

/*
 * MODULE : mx_sc_device.h
 *
 * AUTHOR : David Miller
 *
 * This file
 *
 * DESCRIPTION:
 * Module mx_sc_device.h
 *
 *
 *
 */

#include <map>
#include <string>

#include <Xft/Xft.h>

#include <mx_device.h>
#include <mx_ui_object.h>
#include <mx_xcb.h>

#include <mx_char_style.h>

/*-------------------------------------------------
 * CLASS: mx_screen_device
 *
 * DESCRIPTION:
 *
 *
 */

class mx_screen_device : public mx_device, public mx_xcallback_target, public mx_ui_object {
    MX_RTTI(mx_screen_device_class_e)

public:
    //***************************************************************
    //
    //  Constructor
    //
    //***************************************************************

    mx_screen_device(int& err, Widget parentWidget, mx_frame& deviceFrame);

    //***************************************************************
    //
    //  Destructor
    //
    //***************************************************************

    ~mx_screen_device();

    //***************************************************************
    //
    //  Overridden events for the screen device as an x callback
    //  target
    //
    //***************************************************************

    void expose(int& err, mx_expose_event& event);

    void resize(int& err, mx_resize_event& event);

    void processPreDraw(int& err, mx_draw_event& event);

    void processSpecialDraw(int& err, mx_draw_event& event);

    void processDraw(int& err, mx_draw_event& event);

    void processPostDraw(int& err, mx_draw_event& event);

    void draw(int& err, mx_draw_event& event);

    //***************************************************************
    //
    //  Routines for the size of the device
    //
    //***************************************************************

    mx_ipoint getDevicePixelSize() { return windowSize; };

    //***************************************************************
    //
    //  Routines to drive clipping
    //
    //***************************************************************

    // clip in pixels
    void pushClipRect(int& err,
        const mx_irect& coords,
        const mx_angle& angle,
        bool doIntersect);
    // pop a clip mask
    void popClipMask(int& err);

    // clip to lines in pixels
    void pushClipPolypoint(int& err,
        const mx_ipolypoint& pp,
        bool doIntersect);

    // routines used to drive clipping when redrawing due to clipping etc
    // by default the screen device applies a clip mask to the area it is to
    // redraw and then asks the frame target to redraw the an area 2 pixels
    // bigger

    inline bool getApplyClipMask() { return applyClipMask; };
    inline void setApplyClipMask(bool iapplyClipMask) { applyClipMask = iapplyClipMask; };

    inline int getPixelAdd() { return pixelAdd; };
    inline void setPixelAdd(int ipixelAdd) { pixelAdd = ipixelAdd; };

    //***************************************************************
    //
    //  Routines for basic drawing in pixels only
    //
    //***************************************************************

    // draw a rectangle
    void pfillRect(int& err,
        const mx_ipoint& topLeft,
        const mx_ipoint& bottomRight,
        const mx_angle& angle);

    void pxorRect(int& err,
        const mx_ipoint& topLeft,
        const mx_ipoint& bottomRight);

    void pfillArc(int& err,
        const mx_ipoint& centre,
        const mx_ipoint& size,
        bool isFull,
        double startAng,
        double endAng,
        const mx_angle& angle);

    void pfillSector(int& err,
        const mx_ipoint& centre,
        const mx_ipoint& size,
        bool isFull,
        double startAng,
        double endAng,
        const mx_angle& angle);

    void pfillArc(int& err,
        int iarcFillType,
        const mx_ipoint& centre,
        const mx_ipoint& size,
        bool isFull,
        double startAng,
        double endAng,
        const mx_angle& angle);

    void pdrawArc(int& err,
        const mx_ipoint& centre,
        const mx_ipoint& size,
        bool isFull,
        double startAng,
        double endAng,
        const mx_angle& angle);

    void pdrawLine(int& err,
        const mx_ipoint& start,
        const mx_ipoint& end);

    void pdrawRect(int& err,
        const mx_ipoint& topLeft,
        const mx_ipoint& bottomRight,
        const mx_angle& angle);

    void pdrawPoints(int& err, const mx_ipolypoint& pp);
    void pdrawPolypoint(int& err, const mx_ipolypoint& pp);
    void pdrawSegments(int& err,
        const mx_ipolypoint& pp,
        bool linesCoincident);

    void pfillPolypoint(int& err,
        const mx_ipolypoint& pp);
    void pdrawCircle(int& err,
        const mx_ipoint& centre,
        int radius);

    void pfillCircle(int& err,
        const mx_ipoint& centre,
        int radius);

    // draw some text in a character style
    int pdrawText(int& err,
        const char* s,
        uint32 s_len,
        mx_ipoint pos,
        mx_char_style& charStyle);

    // draw a line of text in one go
    virtual void pdrawTextLine(int& err,
        const mx_ipoint& pos,
        const mx_char_style& init_char_style,
        const mx_text_item* text_items,
        uint32 num_items);

    // returns the pixel width of a character according to a given device
    uint32 pgetCharWidth(int& err,
        char c,
        const mx_char_style& cs);

    void pdrawTile(int& err,
        MX_DEVICE_RASTER tileType,
        int nsigbits,
        int nunpackedBits,
        mx_ipoint& tileSize,
        mx_ipoint& topLeft,
        void* tile,
        mx_irect* clipRect = NULL);

    // draw a set of "resize handles", ie eight little boxes at the corner
    // of an unfilled reactangle. Do all in xormode so it can be undone
    // quickly by doing another call with the same parameters
    void pxorResizeHandles(int& err,
        const mx_ipoint& topLeft,
        const mx_ipoint& bottomRight,
        uint32 handleWidth);

    void outputImageHeader(int& err,
        mx_point& topLeftOffset,
        mx_point& realSize,
        MX_DEVICE_RASTER deviceType,
        int nbits,
        int nplanes,
        int ncolours,
        mx_colour* colours,
        mx_ipoint& imageSize,
        bool clipSet) { err = MX_ERROR_OK; };

    void outputImageTrailer(int& err,
        bool clipSet) { err = MX_ERROR_OK; };

    int depth();

    //***************************************************************
    //
    // Routines to drive redrawing and scrolling of a device
    //
    //***************************************************************

    void setShiftOrigin(mx_doc_coord_t& wantedShiftOrigin,
        mx_doc_coord_t& gotShiftOrigin,
        bool doShift);

    void makeGap(int& err,
        bool doX,
        mx_point& frameStartPoint,
        double frameGapSize);

    void closeGap(int& err,
        bool doX,
        mx_point& frameStartPoint,
        double frameGapSize);

    // shift the contents of a window in frame coordinates (used in scrolling and repositioning)
    void shift(int& err);

    // redraw the whole window
    void refresh(int& err,
        mx_point* topLeft = NULL,
        mx_point* bottomRight = NULL);
    //***************************************************************
    //
    // Routines to set line and fill styles on the device
    //
    //***************************************************************

    void setDefaultLineStyle(int& err,
        mx_line_style& s);

    void setDefaultFillStyle(int& err,
        mx_fill_style& s);

    //***************************************************************
    //
    // Routines to manipulate colours on the device
    //
    //***************************************************************

    void registerColour(mx_colour* colour, LUT_VALUE* pixel);

    void setForegroundColour(int& err, LUT_VALUE pixel);
    void setBackgroundColour(int& err, LUT_VALUE pixel);

    void setForegroundColour(int& err, mx_colour& c);
    void setBackgroundColour(int& err, mx_colour& c);

    static XftColor getXftColour(Display *display, const mx_colour &c);

    //***************************************************************
    //
    // Routines to manipulate pixmaps. The screen device is driven
    // so that entities are only drawn into the pixmaps. The screen
    // device needs to keep a list of the maximum extent it needs to
    // redraw. This functionality can be turned off - for instance
    // if we are redrawing for an expose/resize event we know we
    // have to redraw everything.
    //
    // When the device is being driven from outside the use may
    // call the pixmapCommit function to blat from the pixmap to
    // the window the extent which has been drawn into
    //
    //***************************************************************

    void setDrawToWindowOnly(bool idrawToWindowOnly)
    {
        drawToWindowOnly = idrawToWindowOnly;
    };

    inline void setMonitor(bool idoMonitor)
    {
        doMonitor = idoMonitor;
    };

    void pixmapCommit();

    //***************************************************************
    //
    // routines to drive the caret (cursor to all those non-computer
    // scientists who don't understand these things)
    //
    //***************************************************************

    void switchCaretOff();
    void switchCaretOn();
    void caretPosition(int& err, mx_doc_coord_t& pos, float height);

    // called when the application shuts down
    static void unloadFontsAndColours();

    void setXor(int& err, bool setOn);

private:
    //***************************************************************
    //
    // Basic parameters
    //
    //***************************************************************

    Display* display;
    GC gc;
    Window window;
    mx_ipoint windowSize;
    Screen* screen;

    // the stored pixmap
    Pixmap storePixmap;

    // foreground and background colour
    unsigned long foreground;
    unsigned long background;

    // the screen device creates draw events - so need process functions
    bool inDrawEvent;
    mx_ipoint shiftOrigin;

    Pixmap initialisePixmap();
    void setWindowSize();
    void setBasicColours();

    void blatPixmap(Pixmap ipixmap,
        const mx_ipoint& tl,
        const mx_ipoint& br);

    void copyPixmapToWindow(const mx_ipoint& topLeft,
        const mx_ipoint& bottomRight);

    void redraw(int& err,
        MX_DEVICE_GRAVITY deviceOffset,
        const mx_point& oldSize,
        const mx_point& newSize,
        bool copyToWindow,
        bool doBlatPixmap,
        mx_point* topLeft = NULL,
        mx_point* bottomRight = NULL);
    // clip masks
    Region* clipStack;
    int clipStackOffset;
    int clipStackSize;

    // push a new region on to the stack
    void pushClipMask(int& err,
        Region newRegion,
        bool doIntersect);

    // draw a polypoint as lines or points
    void pdrawPolypointIntern(int& err,
        const mx_ipolypoint& pp,
        bool drawLines);

    // maximum number of requests and a static array of
    // xpoint to be used

    int maxRequests;
    int maxExtRequests;

    inline int getMaxRequests() { return GMAX(maxRequests, maxExtRequests); };

    // arc fill type

    int arcFillType;

    static int nxpoints;
    static XPoint* xpoints;
    static int nxsegments;
    static XSegment* xsegments;
    void setXPoints(int& err,
        int npoints);
    void setXSegments(int& err,
        int nsegments);

    // variable to monitor the size drawn to

    bool doMonitor;
    mx_irect monitorRect;

    // whether to clip on draw

    bool applyClipMask;
    int pixelAdd;

    int extendWidth;

    int nallocDashLengths;
    int nusedDashes;
    int* dashLengths;
    unsigned char* cdashLengths;
    int pixDashOffset;
    bool useXDash;

    static std::map<LUT_VALUE, XColor *> m_colour_dict;
    static std::map<std::string, LUT_VALUE> m_colour_name_dict;
    static std::map<uint32, LUT_VALUE> m_colour_val_dict;

    mx_char_style last_char_style;

    bool font_set;

    int char_style_alignment_offset(const mx_char_style& style);
    void set_char_style(int& err, const mx_char_style& charStyle);
    void draw_underline(int& err,
        const mx_ipoint& start,
        const mx_ipoint& end,
        mx_char_style& style);

    void setDashStyle(int& err, int ndashes, float* dashes, float dashOffset);

    void polypointToXPoints(int& err,
        const mx_ipolypoint& pp);

    int polypointToXSegments(int& err,
        const mx_ipolypoint& pp,
        bool linesCoincident);

    void monitorArcSize(const mx_ipoint& size,
        const mx_ipoint& centre);

    void monitorPolypointSize(const mx_ipolypoint& pp);

    float fontScale;
    int last_char_style_pixel;

    bool caretVisible;
    bool caretOn;

    mx_doc_coord_t caretTop;
    mx_doc_coord_t caretBottom;

    void invertCaret();

    static void caretXtCallback(XtPointer cd, XtIntervalId* id);
    void caretCallback();

    int get_xft_width(const char *s, int len);

    XtIntervalId caretTimer;

    bool drawToWindowOnly;

    XftFont *m_xft_font;
    XftColor m_xft_foreground_colour;
    XftColor m_xft_background_colour;
};

#endif
