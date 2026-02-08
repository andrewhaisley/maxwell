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
 * MODULE : mx_sh_lay.C
 *
 * AUTHOR : David Miller
 *
 * This file
 *
 * DESCRIPTION:
 * Module mx_sh_lay.C
 *
 *
 *
 *
 */

#include "mx_sh_lay.h"

// set 3x3 array of offsets
MX_SH_LAY_OFFSET mx_sheet_layout::documentOffsets[3][3] = { { MX_SH_LAY_OFFSET_TL, MX_SH_LAY_OFFSET_T, MX_SH_LAY_OFFSET_TR },
    { MX_SH_LAY_OFFSET_L, MX_SH_LAY_OFFSET_C, MX_SH_LAY_OFFSET_R },
    { MX_SH_LAY_OFFSET_BL, MX_SH_LAY_OFFSET_B, MX_SH_LAY_OFFSET_BR } };

/*-------------------------------------------------
 * FUNCTION: mx_sheet_layout::mx_sheet_layout
 *
 * DESCRIPTION: Create a new document layout class
 *
 *
 */

mx_sheet_layout::mx_sheet_layout()
{
    // create an empty sheet size
    mx_point p(0.0, 0.0);
    mx_sheet_size sheetSize(p, p);

    nsheets = 0;
    nallocSheets = 0;
    sheetSizes = NULL;

    // add the empty end sheet
    newSheetMemory(1);

    sheetSizes[0] = sheetSize;
    nsheets = 1;
}

/*-------------------------------------------------
 * FUNCTION: mx_sheet_layout::~mx_sheet_layout
 *
 * DESCRIPTION: Delete a document layout class
 *
 *
 */

mx_sheet_layout::~mx_sheet_layout()
{
    delete[] sheetSizes;
}

/*-------------------------------------------------
 * FUNCTION: mx_sheet_layout::newSheetMemory
 *
 * DESCRIPTION: Get some new memory for sheets
 * Dont change nsheets here
 *
 */

void mx_sheet_layout::newSheetMemory(int addNewSheets)
{
    int numberNewSheets;
    mx_sheet_size* oldSheetSizes;

    // get the new number of sheets
    numberNewSheets = nsheets + addNewSheets;

    // test if we actually need more memory
    if (numberNewSheets <= nallocSheets)
        return;

    // need some more memory

    // save old values
    oldSheetSizes = sheetSizes;

    // get new memory
    nallocSheets = (numberNewSheets + 15) / 16;
    nallocSheets *= 16;
    sheetSizes = new mx_sheet_size[nallocSheets];

    for (int i = 0; i < nsheets; i++) {
        sheetSizes[i] = oldSheetSizes[i];
    }
    // memcpy(sheetSizes, oldSheetSizes, nsheets*sizeof(mx_sheet_size));

    // delete the old memory
    delete[] oldSheetSizes;
}

/*-------------------------------------------------
 * FUNCTION: mx_sheet_layout::addSheets
 *
 * DESCRIPTION: Add some new sheets into the sheet
 * size array. Here the first added sheet becomes
 * what was startSheet e.g. to add at the start
 * set startSheet = 0, to add at the end set
 * startSheet = nsheets
 *
 * This routine manipulates the array only
 */

void mx_sheet_layout::addSheets(int& err,
    int startSheet,
    int numberNewSheets,
    mx_sheet_size* isheetSizes)
{
    int nsheetsToMove;
    // mx_sheet_size *sourceOffset,*destOffset ;

    err = MX_ERROR_OK;

    // any new sheets to add ?
    if (numberNewSheets <= 0)
        return;

    // check input in range (can be nsheets-1 to add to end)

    nsheetsToMove = nsheets - startSheet;

    // allow for start case - the only one where no sheets can be added
    if ((startSheet < 0) || (nsheetsToMove <= 0))
        MX_ERROR_THROW(err, MX_SH_LAY_SHEET_RANGE_ERROR);

    // get memory for the sheets if needed
    newSheetMemory(numberNewSheets);

    // sourceOffset = sheetSizes + startSheet ;
    // destOffset   = sourceOffset + numberNewSheets;
    // memmove(destOffset,sourceOffset,nsheetsToMove*sizeof(mx_sheet_size)) ;
    for (int i = 0; i < nsheetsToMove; i++) {
        sheetSizes[startSheet + numberNewSheets + i] = sheetSizes[startSheet + i];
    }

    // reset nsheets
    nsheets += numberNewSheets;

    // now set the values on the new sheets
    changeSheets(err, startSheet, numberNewSheets, isheetSizes);
    MX_ERROR_CHECK(err);

abort:
    return;
}

/*-------------------------------------------------
 * FUNCTION: mx_sheet_layout::changeSheets
 *
 * DESCRIPTION: Change the sheets in the sheet size
 * array.
 *
 */

void mx_sheet_layout::changeSheets(int& err,
    int startSheet,
    int numberChangeSheets,
    mx_sheet_size* isheetSizes)
{
    // mx_sheet_size *sourceOffset ;

    err = MX_ERROR_OK;

    if (numberChangeSheets <= 0)
        return;

    // check input in range
    if ((startSheet < 0) || (startSheet + numberChangeSheets >= nsheets))
        MX_ERROR_THROW(err, MX_SH_LAY_SHEET_RANGE_ERROR);

    // sourceOffset = sheetSizes + startSheet ;

    // memcpy(sourceOffset,isheetSizes, numberChangeSheets*sizeof(mx_sheet_size)) ;
    for (int i = 0; i < numberChangeSheets; i++) {
        sheetSizes[startSheet + i] = isheetSizes[i];
    }

abort:
    return;
}

/*-------------------------------------------------
 * FUNCTION: mx_sheet_layout::removeSheets
 *
 * DESCRIPTION: Remove some sheets from the sheet array
 *
 */

void mx_sheet_layout::removeSheets(int& err,
    int startSheet,
    int numberRemoveSheets)
{
    // mx_sheet_size *sourceOffset,*destOffset ;
    int nsheetsToMove;

    err = MX_ERROR_OK;

    nsheetsToMove = nsheets - startSheet - numberRemoveSheets;

    // check input in range
    if ((startSheet < 0) || (nsheetsToMove <= 0))
        MX_ERROR_THROW(err, MX_SH_LAY_SHEET_RANGE_ERROR);

    // destOffset     = sheetSizes + startSheet ;
    // sourceOffset   = destOffset + numberRemoveSheets;
    // memmove(destOffset,sourceOffset,nsheetsToMove*sizeof(mx_sheet_size)) ;
    for (int i = 0; i < nsheetsToMove; i++) {
        sheetSizes[startSheet + i] = sheetSizes[startSheet + i + numberRemoveSheets];
    }

    nsheets -= numberRemoveSheets;

abort:
    return;
}

/*-------------------------------------------------
 * FUNCTION: mx_sheet_layout::checkRange
 *
 * DESCRIPTION: Check a sheet offset is valid
 *
 */

void mx_sheet_layout::checkRange(int& err, int testSheet)
{
    err = MX_ERROR_OK;

    // check input in range - can include nsheets-1 to get total
    // size

    if ((testSheet < 0) || (testSheet > (nsheets - 1)))
        MX_ERROR_THROW(err, MX_SH_LAY_SHEET_RANGE_ERROR);

abort:
    return;
}

/*-------------------------------------------------
 * FUNCTION: mx_sheet_layout::getSheetSize
 *
 * DESCRIPTION: Get the size of a sheet in a
 * document
 */

mx_sheet_size mx_sheet_layout::getSheetSize(int& err,
    int testSheet)
{
    err = MX_ERROR_OK;

    checkRange(err, testSheet);
    MX_ERROR_CHECK(err);

    updateSheetPositions(err, testSheet);
    MX_ERROR_CHECK(err);

    return sheetSizes[testSheet];
abort:
    return sheetSizes[0];
}

/*-------------------------------------------------
 * FUNCTION: mx_sheet_layout::getSize
 *
 * DESCRIPTION: Get the size of a sheet in a
 * document
 */

mx_point mx_sheet_layout::getSize(int& err,
    int testSheet)
{
    mx_sheet_size sheetSize = getSheetSize(err, testSheet);
    MX_ERROR_CHECK(err);

abort:
    return sheetSize.getSize();
}

/*-------------------------------------------------
 * FUNCTION: mx_sheet_layout::getMaxSize
 *
 * DESCRIPTION: Get the size of a sheet in a
 * document
 */

mx_point mx_sheet_layout::getMaxSize(int& err,
    int testSheet)
{
    mx_sheet_size sheetSize = getSheetSize(err, testSheet);
    MX_ERROR_CHECK(err);

abort:
    return sheetSize.getMaxSize();
}

/*-------------------------------------------------
 * FUNCTION: mx_sheet_layout::getTotalSize
 *
 * DESCRIPTION: Get the size of a sheet in a
 * document
 */

mx_point mx_sheet_layout::getTotalSize(int& err)
{
    mx_point pt = getMaxSize(err, nsheets - 1);
    MX_ERROR_CHECK(err);

abort:
    return pt;
}

/*-------------------------------------------------
 * FUNCTION: mx_sheet_layout::getPosition
 *
 * DESCRIPTION: Get the position of a sheet in a
 * document
 */

mx_point mx_sheet_layout::getPosition(int& err,
    int testSheet)
{
    mx_sheet_size sheetSize = getSheetSize(err, testSheet);
    MX_ERROR_CHECK(err);

abort:
    return sheetSize.getPosition();
}

/*-------------------------------------------------
 * FUNCTION: mx_sheet_layout::checkCoordOnSheet
 *
 * DESCRIPTION: Returns the appropriate value of
 * MX_SH_LAY_OFFSET
 */

MX_SH_LAY_OFFSET mx_sheet_layout::checkCoordOnSheet(int& err,
    const mx_point& coord,
    int testSheet,
    MX_COORD_TYPE coordType)
{
    mx_point topLeft, bottomRight;
    int offset1, offset2;

    // check with last coordinate first
    topLeft = getPosition(err, testSheet);
    MX_ERROR_CHECK(err);

    bottomRight = topLeft + getSize(err, testSheet);
    MX_ERROR_CHECK(err);

    // make it easier to snap some coordinates
    // switch statement doesnt seem to work here - compiler bug ?

    if (coordType == MX_COORD_TL) {
        topLeft.x -= snapTolerance;
        topLeft.y -= snapTolerance;
        bottomRight.x -= snapTolerance;
        bottomRight.y -= snapTolerance;
    } else if (coordType == MX_COORD_TR) {
        bottomRight.x += snapTolerance;
        topLeft.y -= snapTolerance;
        topLeft.x += snapTolerance;
        bottomRight.y -= snapTolerance;
    } else if (coordType == MX_COORD_BR) {
        bottomRight.x += snapTolerance;
        bottomRight.y += snapTolerance;
        topLeft.x += snapTolerance;
        topLeft.y += snapTolerance;
    } else if (coordType == MX_COORD_BL) {
        topLeft.x -= snapTolerance;
        bottomRight.y += snapTolerance;
        bottomRight.x -= snapTolerance;
        topLeft.y += snapTolerance;
    }

    if (coord.y < topLeft.y) {
        offset2 = 0;
    } else if (coord.y >= bottomRight.y) {
        offset2 = 2;
    } else {
        offset2 = 1;
    }

    if (coord.x < topLeft.x) {
        offset1 = 0;
    } else if (coord.x >= bottomRight.x) {
        offset1 = 2;
    } else {
        offset1 = 1;
    }

    return documentOffsets[offset2][offset1];
abort:
    return MX_SH_LAY_OFFSET_C;
}

/*-------------------------------------------------
 * FUNCTION: mx_sheet_layout::startIterateOverArea
 *
 * DESCRIPTION: Returns the appropriate value of
 * MX_SH_LAY_OFFSET
 */

void mx_sheet_layout::startIterateOverArea(int& err,
    const mx_point& tlcoord,
    const mx_point& brcoord)
{
    mx_point totalSize;

    mx_rect r(tlcoord, brcoord);

    err = MX_ERROR_OK;

    r.normalise();

    tlIterate = r.topLeft();
    brIterate = r.bottomRight();

    // clip to top
    if (tlIterate.x < 0)
        tlIterate.x = 0;
    if (tlIterate.y < 0)
        tlIterate.y = 0;

    // clip to bottom
    totalSize = getTotalSize(err);
    MX_ERROR_CHECK(err);

    if (brIterate.x > totalSize.x)
        brIterate.x = totalSize.x;
    if (brIterate.y > totalSize.y)
        brIterate.y = totalSize.y;

    lastSheetIterate = -1;

    return;
abort:
    return;
}

/*-------------------------------------------------
 * FUNCTION: mx_sheet_layout::nextOverArea
 *
 * DESCRIPTION: Returns the appropriate value of
 * MX_SH_LAY_OFFSET
 */

bool mx_sheet_layout::nextOverArea(int& err, mx_doc_rect_t& r)
{
    err = MX_ERROR_OK;

    if (lastSheetIterate == -1) {
        r.sheet_number = 0;
        r.r.xb = tlIterate.x;
        r.r.yb = tlIterate.y;
        r.r.xt = brIterate.x;
        r.r.yt = brIterate.y;

        lastSheetIterate = 0;
        return TRUE;
    }

    return FALSE;
}
