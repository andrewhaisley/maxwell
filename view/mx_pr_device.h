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
#ifndef _MX_PR_DEVICE_H
#define _MX_PR_DEVICE_H

/*
 * MODULE : mx_pr_device.h
 *
 * AUTHOR : David Miller
 *
 * This file
 *
 * DESCRIPTION:
 * Module mx_pr_device.h
 *
 *
 */

#include <mx_cb.h>
#include <mx_device.h>

/*-------------------------------------------------
 * CLASS: mx_print_device
 *
 * DESCRIPTION:
 *
 *
 */

class mx_print_device : public mx_device, public mx_callback_target {
    MX_RTTI(mx_printer_device_class_e)

public:
    //***************************************************************
    //
    //  Constructor
    //
    //***************************************************************

    mx_print_device(int& err,
        mx_frame& deviceFrame,
        mx_point& deviceSize,
        double deviceResolution);

    //***************************************************************
    //
    //  Destructor
    //
    //***************************************************************

    ~mx_print_device();

    //***************************************************************
    //
    //  Routines for the size of the device
    //
    //***************************************************************

    mx_ipoint getDevicePixelSize() { return printerSize; };

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

    virtual void pclipArc(int& err,
        const mx_ipoint& centre,
        const mx_ipoint& size,
        bool isFull,
        double startAng,
        double endAng,
        const mx_angle& angle,
        bool doIntersect);

    virtual void pclipSector(int& err,
        const mx_ipoint& centre,
        const mx_ipoint& size,
        bool isFull,
        double startAng,
        double endAng,
        const mx_angle& angle,
        bool doIntersect);

    //***************************************************************
    //
    //  Routines for basic drawing in pixels only
    //
    //***************************************************************

    void setRect(int& err,
        const mx_irect& rect,
        const mx_angle& angle,
        mx_ipoint& startPoint,
        mx_ipoint& size,
        bool& doTransform,
        const char* saveName);

    // fill a rectangles
    void pfillRect(int& err,
        const mx_ipoint& topLeft,
        const mx_ipoint& bottomRight,
        const mx_angle& angle);

    void pxorRect(int& err,
        const mx_ipoint& topLeft,
        const mx_ipoint& bottomRight);

    void pdrawArc(int& err,
        const mx_ipoint& centre,
        const mx_ipoint& size,
        bool isFull,
        double startAng,
        double endAng,
        const mx_angle& angle);

    void pfillArc(int& err,
        const mx_ipoint& centre,
        const mx_ipoint& size,
        bool isFull,
        double startAng,
        double endAng,
        const mx_angle& angle);

    void pdrawChord(int& err,
        const mx_ipoint& centre,
        const mx_ipoint& size,
        bool isFull,
        double startAng,
        double endAng,
        const mx_angle& angle);

    void pdrawSector(int& err,
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

    void pdrawLine(int& err,
        const mx_ipoint& start,
        const mx_ipoint& end);

    void poperRect(int& err,
        const mx_irect& rect,
        const mx_angle& angle,
        const char* op);

    void pdrawRect(int& err,
        const mx_ipoint& topLeft,
        const mx_ipoint& bottomRight,
        const mx_angle& angle);

    void pdrawPoints(int& err, const mx_ipolypoint& pp) { err = MX_ERROR_OK; };
    void pdrawSegments(int& err,
        const mx_ipolypoint& pp,
        bool linesCoincident) { err = MX_ERROR_OK; };

    void pdrawPolypoint(int& err, const mx_ipolypoint& pp);

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
        const mx_char_style& charStyle);

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

    void outputImageHeader(int& err,
        mx_point& topLeftOffset,
        mx_point& realSize,
        MX_DEVICE_RASTER deviceType,
        int nbits,
        int nplanes,
        int ncolours,
        mx_colour* colours,
        mx_ipoint& imageSize,
        bool clipSet);

    void outputImageTrailer(int& err,
        bool clipSet);
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

    //***************************************************************
    //
    // printer specific things
    //
    //***************************************************************

    void setOutputFile(int& err, FILE* f);

    void sendPreamble(int& err,
        int numPages,
        int numCopies,
        const char* paperType,
        float pageWidth,
        float pageHeight,
        const char* title,
        bool includeFonts);

    void sendPostamble(int& err);

    void startPage(int& err, int n, float page_width, float page_height, bool is_landscape);
    void endPage(int& err);

    void sendProcset(int& err);

    void setFont(int& err);

    void setColour(int& err, const mx_colour& c);

    void sendFonts(int& err);
    void sendFontEncodings(int& err);
    void sendFontResourceList(int& err);

    void setXor(int& err, bool setOn) { err = MX_ERROR_OK; };

private:
    mx_ipoint printerSize;
    FILE* outputFile;
    FILE* tempFile;

    void pushClipMask(int& err,
        bool doIntersect);

    void setPolypointPath(int& err,
        const mx_ipolypoint& pp,
        bool doNotClosePath);

    void pArcPath(int& err,
        const mx_ipoint& centre,
        const mx_ipoint& size,
        bool isFull,
        double startAng,
        double endAng,
        bool isClosed,
        bool isSector,
        const mx_angle& angle);

    void setDashStyle(int& err,
        int ndashes,
        float* dashes,
        float dashOffset);

    void setTransform(int& err,
        const char* saveName,
        bool doShift,
        const mx_point& shift,
        bool doScale,
        const mx_point& scaleFactor,
        bool doRotation,
        const mx_angle& angle);

    void resetTransform(int& err,
        const char* saveName);

    bool charStyleSet;
    mx_char_style lastCharStyle;
    mx_list* document_fonts;

    bool isLastFillStyleSet;
    mx_fill_style lastFillStyle;

    bool isLastLineStyleSet;
    mx_line_style lastLineStyle;

    bool isLastColourSet;
    mx_colour lastColourSet;

    bool isLastFontSet;
    char lastFontNameSet[200];
    float lastFontSizeSet;

    bool includeFonts;

    void resetStyles();
};

#endif
