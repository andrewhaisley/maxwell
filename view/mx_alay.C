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
 * MODULE : mx_alay.C
 *
 * AUTHOR : David Miller
 *
 * This file
 *
 * DESCRIPTION:
 * Module mx_alay.C
 *
 *
 *
 */

#include "mx_alay.h"

/*-------------------------------------------------
 * FUNCTION: mx_area_layout::coordToDocCoord
 *
 * DESCRIPTION: Convert a coordinate to an area
 * coordinate
 *
 * The function returns TRUE if the conversion can be done
 *
 * The function returns FALSE if the conversion cannot be done.
 * that is the coordinate lies on no sheet
 *
 */

bool mx_area_layout::coordToDocCoord(int& err,
    const mx_point& coord,
    mx_doc_coord_t& docCoord,
    MX_COORD_TYPE coordType)
{
    mx_point testSize;

    err = MX_ERROR_OK;

    docCoord.sheet_number = 0;
    docCoord.p = coord;

    // need to do more checks
    testSize = getTotalSize(err);
    MX_ERROR_CHECK(err);

    return ((coord.y >= -snapTolerance) && (coord.y <= testSize.y + snapTolerance) && (coord.x >= -snapTolerance) && (coord.x <= testSize.x + snapTolerance));
abort:
    return FALSE;
}

/*-------------------------------------------------
 * FUNCTION: mx_area_layout::docCoordToCoord
 *
 * DESCRIPTION: Convert a document coord to a coord.
 * Returns FALSE if the document coord cannot be converted
 * to a proper coord. This occurs when the docCoord has
 * an invalide offset on the sheet
 */

bool mx_area_layout::docCoordToCoord(int& err,
    const mx_doc_coord_t& docCoord,
    mx_point& coord)
{
    mx_sheet_size position;
    mx_point testSize;

    err = MX_ERROR_OK;

    coord = docCoord.p;

    // need to do more checks
    testSize = getTotalSize(err);
    MX_ERROR_CHECK(err);

    return ((docCoord.p.y >= -snapTolerance) && (docCoord.p.y <= testSize.y + snapTolerance) && (docCoord.p.x >= -snapTolerance) && (docCoord.p.x <= testSize.x + snapTolerance));
abort:
    return FALSE;
}

/*-------------------------------------------------
 * FUNCTION: mx_area_layout::updateSheetPositions
 *
 * DESCRIPTION: Set the cumulative position totals valid
 * to and including the last sheet
 */

void mx_area_layout::updateSheetPositions(int& err,
    int validSheet)
{
    mx_point testSize;

    // need to do more checks

    testSize = sheetSizes[0].getSize();
    MX_ERROR_CHECK(err);

    // update the sizes of the sheet and the
    // empty sheet

    sheetSizes[0].setMaxSize(testSize);

    if (nsheets > 1) {
        sheetSizes[1].setPosition(testSize);
        sheetSizes[1].setMaxSize(testSize);
    }
abort:;
}
