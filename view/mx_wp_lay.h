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
#ifndef _MX_WP_LAY_H
#define _MX_WP_LAY_H

/*
 * MODULE : mx_wp_lay.h
 *
 * AUTHOR : David Miller
 *
 * This file
 *
 * DESCRIPTION:
 * Module mx_wp_lay.h
 *
 *
 *
 *
 */

#include "mx_sh_lay.h"

/*-------------------------------------------------
 * CLASS: mx_wp_layout
 *
 * DESCRIPTION: Basically an array of sheet sizes
 * with their positions maintained as the sum of the
 * previous sizes in the y direction and the maximum
 * size in the x direction. Calculation
 * of postions is delayed until such time as
 * positions are required.
 *
 * Sheets are indexed in an array using the standard C
 * indexing - i.e. 0 is the index of the first sheet in
 * the array
 *
 */

class mx_wp_layout : public mx_sheet_layout {
public:
    mx_wp_layout();

    // change the sizes or positions of an existing sheet
    void changeSheets(int& err,
        int startSheet,
        int nSheets,
        mx_sheet_size* isheetSizes);

    // remove a range of sheets
    void removeSheets(int& err,
        int startSheet,
        int nSheets);

    // convert a cumulative coordinate to a doc coord
    // This is virtual as the conversion is much simpler
    //
    bool coordToDocCoord(int& err,
        const mx_point& coord,
        mx_doc_coord_t& docCoord,
        MX_COORD_TYPE coordType);

    // convert a doc coord to a cumulative coord
    bool docCoordToCoord(int& err,
        const mx_doc_coord_t& doc_coord,
        mx_point& coord);

    // iteration function
    bool nextOverArea(int& err,
        mx_doc_rect_t& output);

    inline bool getClipToX() { return clipToX; };
    inline void setClipToX(bool icl) { clipToX = icl; };

private:
    // the most advanced sheet in the array with a valid
    // cumulative total
    int lastPositionValidSheet;

    // update the valid cumulative total marker to validSheet
    void updateSheetPositions(int& err,
        int validSheet);

    // flag to see if iteration should return sheets not in
    // the x direction
    bool clipToX;
};

#endif
