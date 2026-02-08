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
 * MODULE : mx_astyle.C
 *
 * AUTHOR : David Miller
 *
 * This file
 *
 * DESCRIPTION:
 * Module mx_astyle.C
 *
 *
 * Revision 1.1  1996/10/06 13:15:45  andrew
 * Initial revision
 *
 *
 */

#include "mx_gstyle.h"

/*-------------------------------------------------
 * FUNCTION: mx_all_styles::mx_all_styles
 *
 * DESCRIPTION:
 *
 *
 */

mx_all_styles::mx_all_styles()
    : dashStyles(mx_gstyle_dash)
    , lineStyles(mx_gstyle_line)
    , fillStyles(mx_gstyle_fill)
    , pointStyles(mx_gstyle_point)
    , borderStyles(mx_gstyle_border)
    , areaStyles(mx_gstyle_area)
    , hatchStyles(mx_gstyle_hatch)
{
}

/*-------------------------------------------------
 * FUNCTION: mx_all_styles::mx_all_styles
 *
 * DESCRIPTION:
 *
 *
 */

mx_all_styles::~mx_all_styles()
{
    // delete the colour map contents
    for (auto i : m_colours) {
        delete i.second;
    }
}

/*-------------------------------------------------
 * FUNCTION: mx_all_styles::get_serialised_size
 *
 * DESCRIPTION:
 *
 *
 */

uint32 mx_all_styles::get_serialised_size(int& err)
{
    int size;

    err = MX_ERROR_OK;

    size = dashStyles.get_serialised_size(err);
    MX_ERROR_CHECK(err);

    size += lineStyles.get_serialised_size(err);
    MX_ERROR_CHECK(err);

    size += fillStyles.get_serialised_size(err);
    MX_ERROR_CHECK(err);

    size += pointStyles.get_serialised_size(err);
    MX_ERROR_CHECK(err);

    size += borderStyles.get_serialised_size(err);
    MX_ERROR_CHECK(err);

    size += areaStyles.get_serialised_size(err);
    MX_ERROR_CHECK(err);

    size += hatchStyles.get_serialised_size(err);
    MX_ERROR_CHECK(err);

    // colours

    size += SLS_INT32;

    // seriaise size the colour map
    for (auto i : m_colours) {
        size += i.second->get_serialised_size(err);
        MX_ERROR_CHECK(err);
    }

abort:
    return size;
}

/*-------------------------------------------------
 * FUNCTION: mx_all_styles::serialise
 *
 * DESCRIPTION:
 *
 *
 */

void mx_all_styles::serialise(int& err,
    unsigned char** buffer)
{
    int32 ncolours;

    dashStyles.serialise(err, buffer);
    MX_ERROR_CHECK(err);

    lineStyles.serialise(err, buffer);
    MX_ERROR_CHECK(err);

    fillStyles.serialise(err, buffer);
    MX_ERROR_CHECK(err);

    pointStyles.serialise(err, buffer);
    MX_ERROR_CHECK(err);

    borderStyles.serialise(err, buffer);
    MX_ERROR_CHECK(err);

    areaStyles.serialise(err, buffer);
    MX_ERROR_CHECK(err);

    hatchStyles.serialise(err, buffer);
    MX_ERROR_CHECK(err);

    ncolours = m_colours.size();

    serialise_int32(ncolours, buffer);

    // serialize the colour map
    for (auto i : m_colours) {
        i.second->serialise(err, buffer);
        MX_ERROR_CHECK(err);
    }

abort:
    return;
}

/*-------------------------------------------------
 * FUNCTION: mx_all_styles::unserialise
 *
 * DESCRIPTION:
 *
 *
 */

void mx_all_styles::unserialise(int& err,
    unsigned char** buffer)
{
    int32 ncolours, icolour;
    mx_colour* thisColour;

    dashStyles.unserialise(err, buffer);
    MX_ERROR_CHECK(err);

    lineStyles.unserialise(err, buffer);
    MX_ERROR_CHECK(err);

    fillStyles.unserialise(err, buffer);
    MX_ERROR_CHECK(err);

    pointStyles.unserialise(err, buffer);
    MX_ERROR_CHECK(err);

    borderStyles.unserialise(err, buffer);
    MX_ERROR_CHECK(err);

    areaStyles.unserialise(err, buffer);
    MX_ERROR_CHECK(err);

    hatchStyles.unserialise(err, buffer);
    MX_ERROR_CHECK(err);

    unserialise_int32(ncolours, buffer);

    for (icolour = 0; icolour < ncolours; icolour++) {
        thisColour = new mx_colour;
        thisColour->unserialise(err, buffer);
        MX_ERROR_CHECK(err);

        m_colours[icolour] = thisColour;
    }

abort:
    return;
}

/*-------------------------------------------------
 * FUNCTION: mx_all_styles::getColour
 *
 * DESCRIPTION:
 *
 *
 */

mx_colour* mx_all_styles::getColour(int& err, int colourId)
{
    if (m_colours.find(colourId) == m_colours.end()) {
        return nullptr;
    } else {
        return m_colours[colourId];
    }
}

/*-------------------------------------------------
 * FUNCTION: mx_all_styles::get
 *
 * DESCRIPTION:
 *
 *
 */

mx_geomstyle* mx_all_styles::get(int& err,
    mx_gstyle_type_t styleType,
    int styleId)
{
    mx_geomstyle* retVal = NULL;

    switch (styleType) {
    case mx_gstyle_dash:
        retVal = dashStyles.getStyleById(err, styleId);
        MX_ERROR_CHECK(err);
        break;
    case mx_gstyle_line:
        retVal = lineStyles.getStyleById(err, styleId);
        MX_ERROR_CHECK(err);
        break;
    case mx_gstyle_fill:
        retVal = fillStyles.getStyleById(err, styleId);
        MX_ERROR_CHECK(err);
        break;
    case mx_gstyle_point:
        retVal = pointStyles.getStyleById(err, styleId);
        MX_ERROR_CHECK(err);
        break;
    case mx_gstyle_border:
        retVal = borderStyles.getStyleById(err, styleId);
        MX_ERROR_CHECK(err);
        break;
    case mx_gstyle_area:
        retVal = areaStyles.getStyleById(err, styleId);
        MX_ERROR_CHECK(err);
        break;
    case mx_gstyle_hatch:
        retVal = hatchStyles.getStyleById(err, styleId);
        MX_ERROR_CHECK(err);
        break;
    default:
        retVal = NULL;
        break;
    }

abort:
    return retVal;
}

/*-------------------------------------------------
 * FUNCTION: mx_all_styles::insert
 *
 * DESCRIPTION:
 *
 *
 */

int mx_all_styles::insert(int& err,
    mx_geomstyle* geomstyle)
{
    int retVal = -1;

    switch (geomstyle->getType()) {
    case mx_gstyle_dash:
        retVal = dashStyles.insert(err, geomstyle);
        MX_ERROR_CHECK(err);
        break;
    case mx_gstyle_line:
        retVal = lineStyles.insert(err, geomstyle);
        MX_ERROR_CHECK(err);
        break;
    case mx_gstyle_fill:
        retVal = fillStyles.insert(err, geomstyle);
        MX_ERROR_CHECK(err);
        break;
    case mx_gstyle_point:
        retVal = pointStyles.insert(err, geomstyle);
        MX_ERROR_CHECK(err);
        break;
    case mx_gstyle_border:
        retVal = borderStyles.insert(err, geomstyle);
        MX_ERROR_CHECK(err);
        break;
    case mx_gstyle_area:
        retVal = areaStyles.insert(err, geomstyle);
        MX_ERROR_CHECK(err);
        break;
    case mx_gstyle_hatch:
        retVal = hatchStyles.insert(err, geomstyle);
        MX_ERROR_CHECK(err);
        break;
    default:
        retVal = 0;
        break;
    }

abort:
    return retVal;
}

/*-------------------------------------------------
 * FUNCTION: mx_all_styles::insertColour
 *
 * DESCRIPTION:
 *
 *
 */

int mx_all_styles::insertColour(int& err, mx_colour* colour)
{
    int retVal = m_colours.size();
    m_colours[retVal] = colour;
    return retVal;
}
