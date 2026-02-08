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
 * MODULE : mx_sstyle.C
 *
 * AUTHOR : David Miller
 *
 * This file
 *
 * DESCRIPTION:
 * Module mx_sstyle.C
 *
 *
 * Revision 1.1  1996/10/06 13:15:45  andrew
 * Initial revision
 *
 *
 */

#include "mx_gstyle.h"

/*-------------------------------------------------
 * FUNCTION: mx_single_styles::mx_single_styles
 *
 * DESCRIPTION:
 *
 *
 */

mx_single_styles::mx_single_styles(mx_gstyle_type_t istyleType)
{
    currentId = 0;
    styleType = istyleType;
}

/*-------------------------------------------------
 * FUNCTION: mx_single_styles::~mx_single_styles
 *
 * DESCRIPTION:
 *
 *
 */

mx_single_styles::~mx_single_styles()
{
    for (auto i : m_stylesByName)
    {
        delete i.second;
    }

    m_stylesByName.clear();
}

/*-------------------------------------------------
 * FUNCTION: mx_single_styles::add
 *
 * DESCRIPTION: Insert a new style into global collections
 *
 *
 */

void mx_single_styles::add(int& err, mx_geomstyle* gstyle)
{
    err = MX_ERROR_OK;

    // Add to maps
    m_stylesById[gstyle->getId()] = gstyle;
    m_stylesByName[gstyle->getName()] = gstyle;
}

/*-------------------------------------------------
 * FUNCTION: mx_single_styles::insert
 *
 * DESCRIPTION: Insert a new style into global collections
 *
 *
 */

int mx_single_styles::insert(int& err, mx_geomstyle* gstyle)
{
    err = MX_ERROR_OK;

    // add a totally new style
    if (gstyle->getId() != -1)
        return gstyle->getId();

    // get next id
    gstyle->setId(currentId);
    currentId++;

    add(err, gstyle);
    MX_ERROR_CHECK(err);

abort:
    return (currentId - 1);
}

/*-------------------------------------------------
 * FUNCTION: mx_single_styles::getStyleById
 *
 * DESCRIPTION:
 *
 *
 */

mx_geomstyle* mx_single_styles::getStyleById(int& err, int id)
{
    // get from map
    if (m_stylesById.find(id) == m_stylesById.end()) {
        return nullptr;
    } else {
        return m_stylesById[id];
    }
}

/*-------------------------------------------------
 * FUNCTION: mx_single_styles::getStyleByName
 *
 * DESCRIPTION:
 *
 *
 */

mx_geomstyle* mx_single_styles::getStyleByName(int& err,
    const char* iname)
{
    mx_geomstyle* gs = nullptr;

    err = MX_ERROR_OK;

    if (iname == nullptr) {
        MX_ERROR_THROW(err, MX_NULL_GSTYLE_NAME);
    }

    if (m_stylesByName.find(iname) == m_stylesByName.end())
    {
        gs = nullptr;
    }
    else
    {   
        gs = m_stylesByName[iname];
    }

abort:
    return gs;
}

/*-------------------------------------------------
 * FUNCTION: mx_single_styles::get_serialised_size
 *
 * DESCRIPTION:
 *
 *
 */

uint32 mx_single_styles::get_serialised_size(int& err)
{
    // current id and number of styles
    int size = 3 * SLS_INT32;

    for (auto i : m_stylesByName)
    {
        size += i.second->get_serialised_size(err);
        MX_ERROR_CHECK(err);
    }

abort:
    return size;
}

/*-------------------------------------------------
 * FUNCTION: mx_single_styles::serialise
 *
 * DESCRIPTION:
 *
 *
 */

void mx_single_styles::serialise(int& err, unsigned char** buffer)
{
    serialise_int32(currentId, buffer);
    serialise_int32(m_stylesByName.size(), buffer);
    serialise_int32(styleType, buffer);

    for (auto i : m_stylesByName)
    {
        i.second->serialise(err, buffer);
        MX_ERROR_CHECK(err);
    }

abort:
    return;
}

/*-------------------------------------------------
 * FUNCTION: mx_single_styles::unserialise
 *
 * DESCRIPTION:
 *
 *
 */

void mx_single_styles::unserialise(int& err,
    unsigned char** buffer)
{
    int32 numberStyles;
    int32 istyle;
    mx_gline_style* lineStyle;
    mx_gdash_style* dashStyle;
    mx_gpoint_style* pointStyle;
    mx_garea_style* areaStyle;
    mx_gborder_style* borderStyle;
    mx_ghatch_style* hatchStyle;
    mx_gfill_style* fillStyle;
    mx_geomstyle* gstyle = nullptr;
    int iistyle;

    unserialise_int32(currentId, buffer);
    unserialise_int32(numberStyles, buffer);
    unserialise_int32(istyle, buffer);

    styleType = (mx_gstyle_type_t)istyle;

    for (iistyle = 0; iistyle < numberStyles; iistyle++) {
        switch (styleType) {
        case mx_gstyle_dash:
            dashStyle = new mx_gdash_style;
            gstyle = dashStyle;
            break;
        case mx_gstyle_line:
            lineStyle = new mx_gline_style;
            gstyle = lineStyle;
            break;
        case mx_gstyle_fill:
            fillStyle = new mx_gfill_style;
            gstyle = fillStyle;
            break;
        case mx_gstyle_point:
            pointStyle = new mx_gpoint_style;
            gstyle = pointStyle;
            break;
        case mx_gstyle_area:
            areaStyle = new mx_garea_style;
            gstyle = areaStyle;
            break;
        case mx_gstyle_border:
            borderStyle = new mx_gborder_style;
            gstyle = borderStyle;
            break;
        case mx_gstyle_hatch:
            hatchStyle = new mx_ghatch_style;
            gstyle = hatchStyle;
            break;
        default:
            break;
        }

        if (gstyle) {
            gstyle->unserialise(err, buffer);
            MX_ERROR_CHECK(err);

            add(err, gstyle);
            MX_ERROR_CHECK(err);
        }
    }
abort:
    return;
}
