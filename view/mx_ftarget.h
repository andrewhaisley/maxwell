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
#ifndef _MX_FTARGET_H
#define _MX_FTARGET_H

/*
 * MODULE : mx_ftarget.h
 *
 * AUTHOR : David Miller
 *
 * This file
 *
 * DESCRIPTION:
 * Module mx_ftarget.h
 *
 *
 *
 *
 */

#include "mx_cb.h"
#include "mx_sh_lay.h"
#include "mx_wp_lay.h"

class mx_frame_target : public mx_callback_target {
public:
    mx_frame_target(mx_sheet_layout* layout);

    // a simple target with one very big sheet
    mx_frame_target();
    ~mx_frame_target();

    // get the sheet layout from a frameable object
    // the sheet layout tells
    mx_sheet_layout& getSheetLayout() { return *sheetLayout; };

    // set a new sheet layout
    void setSheetLayout(mx_sheet_layout* layout);

    // convert a coordinate in the set of sheets to a doc coord
    inline bool coordToDocCoord(int& err,
        const mx_point& coord,
        mx_doc_coord_t& docCoord,
        MX_COORD_TYPE coordType)
    {
        return sheetLayout->coordToDocCoord(err, coord, docCoord, coordType);
    };

    // convert a doc coord to a cumulative coord
    inline bool docCoordToCoord(int& err,
        const mx_doc_coord_t& docCoord,
        mx_point& coord)
    {
        return sheetLayout->docCoordToCoord(err, docCoord, coord);
    };

    // convert a doc coord to a cumulative coord
    inline mx_point getTotalSize(int& err)
    {
        return sheetLayout->getTotalSize(err);
    };

    inline void setSnapTolerance(double snapTolerance)
    {
        sheetLayout->setSnapTolerance(snapTolerance);
    }

    inline double getSnapTolerance()
    {
        return sheetLayout->getSnapTolerance();
    }

private:
    // every framable object must have a sheet layout
    mx_sheet_layout* sheetLayout;

    mx_sheet_layout* get_simple_layout();

    // bit of a fudge, because in some cases this object holds on to a
    // reference to a sheet layout owned by other objects and sometimes owns
    // a sheet layout
    bool sheet_layout_allocated;
};

#endif
