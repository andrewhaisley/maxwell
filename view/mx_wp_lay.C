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
 * MODULE : mx_wp_lay.C
 *
 * AUTHOR : David Miller
 *
 * This file
 *
 * DESCRIPTION:
 * Module mx_wp_lay.C
 *
 *
 *
 *
 */

#include "mx_wp_lay.h"

/*-------------------------------------------------
 * FUNCTION: mx_wp_layout::mx_wp_layout
 *
 * DESCRIPTION: Create a new document size class
 *
 *
 */

mx_wp_layout::mx_wp_layout()
    : mx_sheet_layout()
{
    lastPositionValidSheet = -1;
    clipToX = FALSE;
}

/*-------------------------------------------------
 * FUNCTION: mx_wp_layout::changeSheets
 *
 * DESCRIPTION: Change the sheets in the sheet size
 * array.
 *
 */

void mx_wp_layout::changeSheets(int& err,
    int startSheet,
    int numberChangeSheets,
    mx_sheet_size* isheetSizes)
{
    mx_sheet_layout::changeSheets(err, startSheet,
        numberChangeSheets, isheetSizes);
    MX_ERROR_CHECK(err);

    // if the shart sheet is before the present valid sheet
    // then set the last valid sheet to before the start sheet

    if ((startSheet - 1) < lastPositionValidSheet)
        lastPositionValidSheet = startSheet - 1;

abort:
    return;
}

/*-------------------------------------------------
 * FUNCTION: mx_wp_layout::removeSheets
 *
 * DESCRIPTION: Remove some sheets from the sheet array
 *
 */

void mx_wp_layout::removeSheets(int& err,
    int startSheet,
    int numberRemoveSheets)
{
    mx_sheet_layout::removeSheets(err, startSheet,
        numberRemoveSheets);
    MX_ERROR_CHECK(err);

    // if the shart sheet is before the present valid sheet
    // then set the last valid sheet to the start sheet

    if ((startSheet - 1) < lastPositionValidSheet)
        lastPositionValidSheet = startSheet - 1;

abort:
    return;
}

/*-------------------------------------------------
 * FUNCTION: mx_wp_layout::updateSheetPositions
 *
 * DESCRIPTION: Set the cumulative position totals valid
 * to and including the last sheet
 */

void mx_wp_layout::updateSheetPositions(int& err,
    int validSheet)
{
    // test if already valid
    // if it is return immediately

    err = MX_ERROR_OK;

    if (validSheet <= lastPositionValidSheet)
        return;

    mx_point cumulativePosition(0.0, 0.0);
    mx_point maximumSize(0.0, 0.0);
    mx_point thisSheetSize(0.0, 0.0);

    // check sheet OK
    checkRange(err, validSheet);
    MX_ERROR_CHECK(err);

    // set initial cumulative total
    if (lastPositionValidSheet != -1) {
        cumulativePosition = sheetSizes[lastPositionValidSheet].getPosition();
        maximumSize = sheetSizes[lastPositionValidSheet].getMaxSize();
        thisSheetSize = sheetSizes[lastPositionValidSheet].getSize();
    }

    // loop until lastPositionValidSheet is validSheet
    // adding to the cumulative size

    while (1) {
        lastPositionValidSheet++;

        // add on the size of the next sheet
        cumulativePosition.y += thisSheetSize.y;
        sheetSizes[lastPositionValidSheet].setPosition(cumulativePosition);

        // the x total is the maximum of sheets up to this sheet
        if (maximumSize.x < thisSheetSize.x)
            maximumSize.x = thisSheetSize.x;

        maximumSize.y = cumulativePosition.y;

        sheetSizes[lastPositionValidSheet].setMaxSize(maximumSize);

        // reached end
        if (lastPositionValidSheet == validSheet)
            break;

        // get the size of the last valid sheet
        thisSheetSize = sheetSizes[lastPositionValidSheet].getSize();
    }

abort:
    return;
}

/*-------------------------------------------------
 * FUNCTION: mx_wp_layout::coordToDocCoord
 *
 * DESCRIPTION: Convert a coordinate to a document
 * coordinate - the coordinate is the coordinate in
 * some aggregation of sheets - the document coordinate
 * is based on a sheet coordinate and an offset into the
 * sheet.
 *
 * The function returns TRUE if the conversion can be done
 *
 * The function returns FALSE if the conversion cannot be done.
 * that is the coordinate lies on no sheet
 *
 * This is the variant for a typical WP editor - where pages
 * are taken to be consecutive and points outside any sheets
 * make the function return FALSE
 *
 * If a point is after the end of the document then it is taken
 * to be a point on the extra sheet added to the end of a
 * document -  function returns FALSE (same behaviour for empty document)
 *
 * If the point is to the left of the right of the document then
 * it is placed on the relevant sheet with the x value of the
 * point - function returns FALSE
 *
 * If the point is before the start of the document it is taken to
 * be on the first sheet but with negative y value -function
 * returns FALSE
 */

bool mx_wp_layout::coordToDocCoord(int& err,
    const mx_point& coord,
    mx_doc_coord_t& docCoord,
    MX_COORD_TYPE coordType)
{
    // remember the previous sheet for a quick optimisation
    static int previousSheet = 0;

    bool foundSheet = FALSE;
    int endSheet = nsheets - 2;
    mx_point testSize;
    MX_SH_LAY_OFFSET testValue;
    int testSheet, minSheet, maxSheet;
    bool returnValue;

    err = MX_ERROR_OK;

    // If no sheets - add to last sheet
    if (endSheet < 0) {
        docCoord.sheet_number = 0;
        docCoord.p = coord;
        return FALSE;
    }

    // If before the first sheet set negative values on first sheet
    if (coord.y < -snapTolerance) {
        docCoord.sheet_number = 0;
        docCoord.p = coord;
        return FALSE;
    }

    // check previous sheet first - need to check it is still valid

    if (previousSheet <= endSheet) {
        testValue = checkCoordOnSheet(err, coord, previousSheet, coordType);
        MX_ERROR_CHECK(err);

        // check y value is in range
        foundSheet = (testValue >= MX_SH_LAY_OFFSET_L) && (testValue <= MX_SH_LAY_OFFSET_R);

        testSheet = previousSheet;
    }

    if (!foundSheet) {
        // need to do more checks
        testSize = getTotalSize(err);
        MX_ERROR_CHECK(err);

        // check coord is in total range
        if (coord.y >= testSize.y) {
            docCoord.sheet_number = nsheets - 1;
            docCoord.p.x = coord.x;
            docCoord.p.y = coord.y - testSize.y;
            return FALSE;
        }

        // coordinate is in total range

        // first guess for binary chop - all sheets are the same size

        testSheet = (testSize.y == 0.0)
            ? nsheets / 2
            : ((int)(coord.y * (nsheets - 1) / testSize.y));
        minSheet = 0;
        maxSheet = endSheet;

        do {
            testValue = checkCoordOnSheet(err, coord, testSheet, coordType);
            MX_ERROR_CHECK(err);

            if (testValue > MX_SH_LAY_OFFSET_R) {
                // coord is later in the document
                if (testSheet == maxSheet)
                    return FALSE;
                minSheet = testSheet;
                testSheet += (maxSheet - testSheet + 1) / 2;
            } else if (testValue < MX_SH_LAY_OFFSET_L) {
                // coord is earlier in the document
                if (testSheet == minSheet)
                    return FALSE;
                maxSheet = testSheet;
                testSheet -= (testSheet - minSheet + 1) / 2;
            } else {
                // located the y coordinate
                foundSheet = TRUE;
            }
        } while (!foundSheet);
    }

    // must be on the sheet for WP
    returnValue = (testValue == MX_SH_LAY_OFFSET_C);

    testSize = getPosition(err, testSheet);
    MX_ERROR_CHECK(err);

    // fill in output document coordinate strcuture
    docCoord.p.x = coord.x;
    docCoord.p.y = coord.y - testSize.y;
    docCoord.sheet_number = testSheet;

    // remember the succesful sheet
    previousSheet = testSheet;

    return returnValue;
abort:
    return FALSE;
}

/*-------------------------------------------------
 * FUNCTION: mx_wp_layout::docCoordToCoord
 *
 * DESCRIPTION: Convert a document coord to a coord.
 * Returns FALSE if the document coord cannot be converted
 * to a proper coord. This occurs when the docCoord has
 * an invalide offset on the sheet
 */

bool mx_wp_layout::docCoordToCoord(int& err,
    const mx_doc_coord_t& docCoord,
    mx_point& coord)
{
    mx_sheet_size position;
    bool retValue;

    err = MX_ERROR_OK;

    // get the details of the sheet (and check number is valid
    position = getSheetSize(err, docCoord.sheet_number);
    MX_ERROR_CHECK(err);

    // check docCoord is sensible for this sheet
    retValue = !((docCoord.p.x < 0) || (docCoord.p.y < 0) || (docCoord.p.x >= position.getSize().x) || (docCoord.p.y >= position.getSize().y));

    // the coord is the position of the sheet plus the offset in
    // this sheet

    coord.x = docCoord.p.x;
    coord.y = position.getPosition().y + docCoord.p.y;

    return retValue;
abort:
    return FALSE;
}

/*-------------------------------------------------
 * FUNCTION: mx_wp_layout::nextOverArea
 *
 * DESCRIPTION: Get the next area to iterate over.
 * Clip to sheets
 *
 *
 *
 */

bool mx_wp_layout::nextOverArea(int& err,
    mx_doc_rect_t& output)
{
    bool foundSheet;
    mx_point bottomRight, topLeft;
    mx_doc_coord_t docCoord;

    err = MX_ERROR_OK;

    if (lastSheetIterate == -1) {
        // need to do more checks
        mx_point testSize = getTotalSize(err);
        MX_ERROR_CHECK(err);

        // if outside range nothing to return

        if ((brIterate.y <= 0) || (clipToX && brIterate.x <= 0) || (tlIterate.y >= testSize.y) || (clipToX && tlIterate.x >= testSize.x)) {
            return FALSE;
        }

        // in y range - get the sheet
        coordToDocCoord(err, tlIterate, docCoord, MX_COORD_TL);
        MX_ERROR_CHECK(err);

        // set for this sheet
        lastSheetIterate = docCoord.sheet_number - 1;
    }

    foundSheet = FALSE;

    do {
        // move on to the next sheet
        lastSheetIterate++;

        // end of document ?
        if (lastSheetIterate >= nsheets - 1)
            return FALSE;

        // get the topleft and bottom right of this sheet
        topLeft = getPosition(err, lastSheetIterate);
        MX_ERROR_CHECK(err);

        // have we reached the end of the area to be iterated over
        if (topLeft.y >= brIterate.y)
            return FALSE;

        bottomRight = topLeft + getSize(err, lastSheetIterate);
        MX_ERROR_CHECK(err);

        // if this sheet does not intersect in the x direction go on to next sheet
        if (clipToX && ((topLeft.x >= brIterate.x) || bottomRight.x <= tlIterate.x))
            continue;

        // we have found a sheet

        foundSheet = TRUE;
        output.sheet_number = lastSheetIterate;
        output.r.xb = GMAX(topLeft.x, tlIterate.x) - topLeft.x;
        output.r.yb = GMAX(topLeft.y, tlIterate.y) - topLeft.y;
        output.r.xt = GMIN(bottomRight.x, brIterate.x) - topLeft.x;
        output.r.yt = GMIN(bottomRight.y, brIterate.y) - topLeft.y;
    } while (!foundSheet);

    return TRUE;
abort:
    return FALSE;
}
