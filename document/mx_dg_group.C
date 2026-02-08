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
 * MODULE : mx_dg_group.C
 *
 * AUTHOR : David Miller
 *
 * This file
 *
 * DESCRIPTION:
 * Module mx_dg_group.C
 *
 */

#include "mx_dg_area.h"

char* mx_dg_group_element::idGroupStr(const mx_dg_group_key& groupKey)
{
    static char str[20];
    static int lastGroupId = -1;
    int groupId = groupKey.getGroupId();

    if (lastGroupId != groupId)
        sprintf(str, "gp_%d", groupId);

    return str;
}

/*-------------------------------------------------
 * FUNCTION: mx_dg_group_element::mx_dg_group_element
 *
 * DESCRIPTION:
 *
 *
 */

mx_dg_group_element::mx_dg_group_element(const mx_dg_level_key& key)
    : levelKey(key)
{
    geom = NULL;
    stage = MX_DG_GROUP_GET_NONE;
}

/*-------------------------------------------------
 * FUNCTION: mx_dg_group_element::get_serialised_size
 *
 * DESCRIPTION:
 *
 *
 */

uint32 mx_dg_group_element::get_serialised_size(int& err)
{
    err = MX_ERROR_OK;

    return mx_dg_group_element::size();
}

/*-------------------------------------------------
 * FUNCTION: mx_dg_group_element::serialise
 *
 * DESCRIPTION:
 *
 *
 */

void mx_dg_group_element::serialise(int& err,
    uint8** buffer)
{
    err = MX_ERROR_OK;

    levelKey.serialise(err, buffer);
}

/*-------------------------------------------------
 * FUNCTION: mx_dg_group_element::unserialise
 *
 * DESCRIPTION:
 *
 *
 */

void mx_dg_group_element::unserialise(int& err,
    uint8** buffer)
{
    err = MX_ERROR_OK;

    levelKey.unserialise(err, buffer);
}

/*-------------------------------------------------
 * FUNCTION: mx_dg_group_iterator::mx_dg_group_iterator
 *
 * DESCRIPTION:
 *
 *
 */

mx_dg_group_iterator::mx_dg_group_iterator()
{
    currentOffset = 0;
    stage = MX_DG_GROUP_GET_GEOM;
    area = NULL;
    doExtract = FALSE;
}

/*-------------------------------------------------
 * FUNCTION: mx_dg_group_iterator::mx_dg_group_iterator
 *
 * DESCRIPTION:
 *
 *
 */

mx_dg_group_iterator::~mx_dg_group_iterator()
{
}

/*-------------------------------------------------
 * FUNCTION: mx_dg_group_iterator::start
 *
 * DESCRIPTION:
 *
 *
 */

void mx_dg_group_iterator::start(int& err,
    mx_diagram_area* iarea,
    const mx_dg_group_key& igroupKey,
    MX_DG_GROUP_GET_STAGE istage)
{
    char* idString = mx_dg_group_element::idGroupStr(groupKey.getGroupId());

    err = MX_ERROR_OK;

    iarea = area;
    groupKey = igroupKey;
    currentOffset = 0;
    stage = istage;

    topOffset = area->get_blob_size(err, idString);
    MX_ERROR_CHECK(err);

abort:;
}

/*-------------------------------------------------
 * FUNCTION: mx_dg_group_iterator::next
 *
 * DESCRIPTION:
 *
 *
 */

bool mx_dg_group_iterator::next(int& err)
{
    const uint8* buffer;
    uint8* tmpBuffer;
    mx_dg_quad_key quadKey;
    char* idString = mx_dg_group_element::idGroupStr(groupKey.getGroupId());

    err = MX_ERROR_OK;

    if (currentOffset >= topOffset)
        return FALSE;

    buffer = area->get_blob_readonly(err,
        idString,
        currentOffset,
        mx_dg_group_element::size());
    MX_ERROR_CHECK(err);

    tmpBuffer = (uint8*)buffer;

    element.unserialise(err, &tmpBuffer);
    MX_ERROR_CHECK(err);

    currentOffset += mx_dg_group_element::size();

    if (stage > MX_DG_GROUP_GET_LEVEL) {
        // we need to get the quad information

        mx_dg_level* thisLevel = area->getLevel(element.getLevelId());

        quadKey = thisLevel->getQuad(err,
            element.getLevelKey());
        MX_ERROR_CHECK(err);

        element.setQuadKey(quadKey);

        if (stage > MX_DG_GROUP_GET_QUAD) {
            mx_geom* geom;

            // we need to get the geometry information from the quad

            thisLevel->getGeometry(err,
                doExtract,
                element.getQuadKey(),
                &geom);
            MX_ERROR_CHECK(err);

            element.setGeom(geom, doExtract);
        }
    }

    return TRUE;
abort:
    return FALSE;
}

/*-------------------------------------------------
 * Function: mx_dg_group_key::mx_dg_group_key
 *
 * DESCRIPTION:
 *
 *
 */

mx_dg_group_key::mx_dg_group_key(int32 igroupId,
    int32 ioffset)
{
    groupId = igroupId;
    offset = ioffset;
}

/*-------------------------------------------------
 * FUNCTION: mx_dg_group_key::get_serialised_size
 *
 * DESCRIPTION:
 *
 *
 */

uint32 mx_dg_group_key::get_serialised_size(int& err)
{
    err = MX_ERROR_OK;

    return mx_dg_group_key::size();
}

/*-------------------------------------------------
 * FUNCTION: mx_dg_group_key::serialise
 *
 * DESCRIPTION:
 *
 *
 */

void mx_dg_group_key::serialise(int& err,
    uint8** buffer)
{
    err = MX_ERROR_OK;

    serialise_int32(groupId, buffer);
    serialise_int32(offset, buffer);
}

/*-------------------------------------------------
 * FUNCTION: mx_dg_group_key::unserialise
 *
 * DESCRIPTION:
 *
 *
 */

void mx_dg_group_key::unserialise(int& err,
    uint8** buffer)
{
    err = MX_ERROR_OK;

    unserialise_int32(groupId, buffer);
    unserialise_int32(offset, buffer);
}
