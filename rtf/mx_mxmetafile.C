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
 * MODULE : mx_mxmetafile.C
 *
 * AUTHOR : David Miller
 *
 * This file
 *
 * DESCRIPTION:
 * Module mx_mxmetafile.C
 *
 *
 * Revision 1.1  1996/10/06 13:15:45  andrew
 * Initial revision
 *
 *
 */

#include "mx_mxmetafile.h"

void mx_mxmetafile_output::convertColour(mx_metafile_colour& mfColour,
    mx_colour& mxColour)

{
    mxColour.red = mfColour.red;
    mxColour.green = mfColour.green;
    mxColour.blue = mfColour.blue;
    mxColour.name = NULL;
}

void mx_mxmetafile_output::processSetWindowOrg(int& err,
    mx_standard_metafile_record& record)
{
    err = MX_ERROR_OK;

    if (record.nparams != 2) {
        MX_ERROR_THROW(err, MX_ERROR_METAFILE_PARAM_ERROR);
    }

    windowOrg.x = record.params[1];
    windowOrg.y = record.params[0];

abort:;
}

void mx_mxmetafile_output::processSetMapMode(int& err,
    mx_standard_metafile_record& record)
{
    err = MX_ERROR_OK;

    if (record.nparams != 1) {
        MX_ERROR_THROW(err, MX_ERROR_METAFILE_PARAM_ERROR);
    }

    mapMode = record.params[0];

abort:;
}

void mx_mxmetafile_output::processSetWindowExt(int& err,
    mx_standard_metafile_record& record)
{
    err = MX_ERROR_OK;

    if (record.nparams != 2) {
        MX_ERROR_THROW(err, MX_ERROR_METAFILE_PARAM_ERROR);
    }

    windowExt.x = record.params[1];
    windowExt.y = record.params[0];

abort:;
}

void mx_mxmetafile_output::processSetTextColor(int& err,
    mx_standard_metafile_record& record)
{
    mx_metafile_colour mfcolour;

    err = MX_ERROR_OK;

    if (record.nparams != 2) {
        MX_ERROR_THROW(err, MX_ERROR_METAFILE_PARAM_ERROR);
    }

    mfcolour.set(err, record.params);
    MX_ERROR_CHECK(err);

    convertColour(mfcolour, textColour);
    MX_ERROR_CHECK(err);

abort:;
}

void mx_mxmetafile_output::processSetBkColor(int& err,
    mx_standard_metafile_record& record)
{
    mx_metafile_colour mfcolour;

    err = MX_ERROR_OK;

    if (record.nparams != 2) {
        MX_ERROR_THROW(err, MX_ERROR_METAFILE_PARAM_ERROR);
    }

    mfcolour.set(err, record.params);
    MX_ERROR_CHECK(err);

    convertColour(mfcolour, bkColour);
    MX_ERROR_CHECK(err);

abort:;
}

void mx_mxmetafile_output::processSetStretchBltMode(int& err,
    mx_standard_metafile_record& record)
{
    err = MX_ERROR_OK;

    if (record.nparams != 1) {
        MX_ERROR_THROW(err, MX_ERROR_METAFILE_PARAM_ERROR);
    }

    stretchBltMode = record.params[0];

abort:;
}

void mx_mxmetafile_output::processStretchDIBits(int& err,
    mx_standard_metafile_record& record,
    char** buffer)
{
    err = MX_ERROR_OK;

    stretchDIB.set(err, buffer);
    MX_ERROR_CHECK(err);

    // convert the colours

    for (int icolour = 0; icolour < stretchDIB.ncolours; icolour++) {
        convertColour(stretchDIB.DIBcolours[icolour], mxDIBcolours[icolour]);
        MX_ERROR_CHECK(err);
    }

    isGreyScale = TRUE;

    for (int icolour = 0; icolour < stretchDIB.ncolours; icolour++) {
        isGreyScale = (mxDIBcolours[icolour].red == icolour) && (mxDIBcolours[icolour].green == icolour) && (mxDIBcolours[icolour].blue == icolour);

        if (!isGreyScale)
            break;
    }

abort:;
}
