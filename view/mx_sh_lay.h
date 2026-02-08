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
#ifndef _MX_SH_LAY_H
#define _MX_SH_LAY_H

/*
 * MODULE : mx_sh_lay.h
 *
 * AUTHOR : David Miller
 *
 * This file
 *
 * DESCRIPTION:
 * Module mx_sh_lay.h
 *
 *
 *
 */

#include <geometry.h>
#include <mx.h>
#include <mx_doc_coord.h>
#include <mx_doc_rect.h>

/*-------------------------------------------------
 * CLASS: mx_sheet_size
 *
 * DESCRIPTION: The size and position of a
 * sheet in a document (i.e. a set of sheets).
 *
 * In a WP application the y position is the offset of
 * the top left hand corner of the corner and is the
 * sum of the sizes of the previous sheets.
 *
 * The x position in a WP application is taken to be the
 * maximum sheet x size up to this sheet
 *
 * In more complex arrangements of sheets the meanings
 * of size and position could be different
 */

class mx_sheet_size {
public:
    mx_sheet_size() { };
    mx_sheet_size(const mx_sheet_size& other);
    mx_sheet_size(const mx_point& isz);
    mx_sheet_size(const mx_point& sheetSize, const mx_point& sheetPosition);
    inline void setPosition(const mx_point& ipos) { position = ipos; };
    inline void setMaxSize(const mx_point& imaxsz) { maxSize = imaxsz; };
    inline void setSize(const mx_point& isz) { size = isz; };

    inline mx_point getSize() { return size; };
    inline mx_point getPosition() { return position; };
    inline mx_point getMaxSize() { return maxSize; };

    void operator=(const mx_sheet_size& other);

protected:
private:
    // the size of sheet
    mx_point size;
    // the position of the sheet
    mx_point position;
    // the maximum dimension up to this sheet
    // that is the total size of the document if this is the
    // last sheet
    mx_point maxSize;
};

typedef enum {
    MX_SH_LAY_OFFSET_TL,
    MX_SH_LAY_OFFSET_T,
    MX_SH_LAY_OFFSET_TR,
    MX_SH_LAY_OFFSET_L,
    MX_SH_LAY_OFFSET_C,
    MX_SH_LAY_OFFSET_R,
    MX_SH_LAY_OFFSET_BL,
    MX_SH_LAY_OFFSET_B,
    MX_SH_LAY_OFFSET_BR
} MX_SH_LAY_OFFSET;

/*-------------------------------------------------
 * CLASS: mx_sheet_layout
 *
 * DESCRIPTION: Basically an array of sheet sizes
 * with their positions maintained. Calculation
 * of postions is delayed until such time as
 * positions are required.
 *
 * Sheets are indexed in an array using the standard C
 * indexing - i.e. 0 is the index of the first sheet in
 * the array
 *
 * For WP classes the document is composed of a series of
 * sheets and maintaining their position is a case of adding the
 * cumulative sizes of the sheets
 *
 * For more complex types of documents one could see this
 * class being more complex - e.g. the sheets would not just
 * be displayed as a simple array but may have a more complex
 * arrangement.
 *
 * This class is used to convert from coordinates in the multi
 * sheet format to coordinates within the sheet format. It is
 * therefore a very important class in the communication between
 * the editor and the frame, and via the frame to the display device.
 */

class mx_sheet_layout {
public:
    mx_sheet_layout();
    virtual ~mx_sheet_layout();

    // the next three routines manipulate sheets - the
    // functions on the base class adjust the memory
    // correctly. addSheets uses changeSheets to adjust
    // values so hopefully most derived classes will not
    // need to provide a virtual addSheets but just provide
    // a changeSheets

    // add a new sheet to the document size
    virtual void addSheets(int& err,
        int startSheet,
        int nSheets,
        mx_sheet_size* isheetSizes);

    // change the sizes or positions of an existing sheet
    virtual void changeSheets(int& err,
        int startSheet,
        int nSheets,
        mx_sheet_size* isheetSizes);

    // remove a range of sheets
    virtual void removeSheets(int& err,
        int startSheet,
        int nSheets);

    // convert a coordinate in the set of sheets to a doc coord
    virtual bool coordToDocCoord(int& err,
        const mx_point& coord,
        mx_doc_coord_t& docCoord,
        MX_COORD_TYPE coordType)
        = 0;

    // convert a doc coord to a cumulative coord
    virtual bool docCoordToCoord(int& err,
        const mx_doc_coord_t& doc_coord,
        mx_point& coord)
        = 0;

    // routine to initialise iterationover areas
    virtual void startIterateOverArea(int& err,
        const mx_point& tlcoord,
        const mx_point& brcoord);

    virtual bool nextOverArea(int& err,
        mx_doc_rect_t& output);

    // get the postion and size of the document up to a sheet
    mx_sheet_size getSheetSize(int& err,
        int testSheet);

    // get the size of the sheet
    mx_point getSize(int& err,
        int testSheet);

    // get the position of the sheet
    mx_point getPosition(int& err,
        int testSheet);

    // get the maximum size for the sheet
    mx_point getMaxSize(int& err,
        int testSheet);

    // get the total size of the document
    mx_point getTotalSize(int& err);

    // check if a coordinate is on a sheet
    MX_SH_LAY_OFFSET checkCoordOnSheet(int& err,
        const mx_point& coord,
        int testSheet,
        MX_COORD_TYPE coordType);

    // get the number of sheets in the document - dont return dud last
    // sheet
    inline int getNumberSheets() { return (nsheets - 1); };

    inline void setSnapTolerance(double isnapTolerance) { snapTolerance = isnapTolerance; };
    inline double getSnapTolerance() { return snapTolerance; };

protected:
    // the number of active sheets in the array
    // we actually store (n+1) sheets to have an empty
    // last sheet whichhold the cumulative size of the whole document
    // therefore when the document has n sheets nsheets is (n+1)
    int nsheets;

    // the actual sheet sizes
    mx_sheet_size* sheetSizes;
    void checkRange(int& err, int testSheet);

    // variables for sheet iteration
    mx_point tlIterate;
    mx_point brIterate;
    int lastSheetIterate;

protected:
    // tolearance for snapping to corners
    double snapTolerance;

private:
    static MX_SH_LAY_OFFSET documentOffsets[3][3];

    // the sizes of the sheets in the document

    // the total number of sheets allocated in the array
    int nallocSheets;

    // update the sheet positions to be valid. This routine may
    // move sheets according to some rules
    virtual void updateSheetPositions(int& err,
        int validSheet)
        = 0;

    void newSheetMemory(int addNewSheets);
};

#endif
