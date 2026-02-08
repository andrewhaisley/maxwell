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
#ifndef _MX_ALAY_H
#define _MX_ALAY_H

/*
 * MODULE : mx_alay.h
 *
 * AUTHOR : David Miller
 *
 * This file
 *
 * DESCRIPTION:
 * Module mx_alay.h
 *
 *
 */

#include "mx_sh_lay.h"

/*-------------------------------------------------
 * CLASS: mx_area_layout
 *
 * DESCRIPTION: Basically a single sheet layout
 *
 */

class mx_area_layout : public mx_sheet_layout {
public:
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

private:
    void updateSheetPositions(int& err,
        int validSheet);
};

#endif
