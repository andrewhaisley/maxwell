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
 * MODULE : mx_dg_dgroup.C
 *
 * AUTHOR : David Miller
 *
 * This file
 *
 * DESCRIPTION:
 * Module mx_dg_dgroup.C
 *
 *
 * Revision 1.1  1996/10/06 13:15:45  andrew
 * Initial revision
 *
 *
 */

#include "mx_dg_area.h"

/*-------------------------------------------------
 * FUNCTION: mx_diagram_area::singleGroupGarbageCollect
 *
 * DESCRIPTION:
 *
 *
 */

void mx_diagram_area::singleGroupGarbageCollect(int& err,
    const mx_dg_group_key& groupKey)
{
    mx_dg_group_iterator iterator;
    int outOffset = 0;
    bool stillInStep = TRUE;
    mx_dg_group_key igroupKey;
    mx_dg_group_element element;
    mx_dg_quad* quad;
    mx_dg_quad_key quadKey;

    err = MX_ERROR_OK;

    iterator.start(err, this, groupKey, MX_DG_GROUP_GET_LEVEL);
    MX_ERROR_CHECK(err);

    igroupKey.setGroupId(groupKey.getGroupId());

    while (iterator.next(err)) {
        element = iterator.getGroupElement();

        if (element.getLevelKey().isNull()) {
            if (stillInStep)
                stillInStep = FALSE;
        } else {
            if (!stillInStep) {
                quad = quadFromLevel(err, element.getLevelKey(),
                    quadKey);
                MX_ERROR_CHECK(err);

                igroupKey.setOffset(outOffset);

                quad->setGroup(err, quadKey, igroupKey);
                MX_ERROR_CHECK(err);

                setGroupElement(err, igroupKey, element);
                MX_ERROR_CHECK(err);
            }

            outOffset += mx_dg_group_element_size;
        }
    }
    MX_ERROR_CHECK(err);
abort:;
}

/*-------------------------------------------------
 * FUNCTION: mx_diagram_area::createGroup
 *
 * DESCRIPTION:
 *
 *
 */

mx_dg_group_key mx_diagram_area::createGroup(int& err)
{
    mx_dg_group_key retGroupKey = nextGroupKey;

    err = MX_ERROR_OK;

    // get the latest group
    create_blob(err, mx_dg_group_element::idGroupStr(nextGroupKey));
    MX_ERROR_CHECK(err);

    // add to the list of
    nextGroupKey.setGroupId(nextGroupKey.getGroupId() + 1);

abort:
    return retGroupKey;
}

/*-------------------------------------------------
 * FUNCTION: mx_diagram_area::deleteGroup
 *
 * DESCRIPTION:
 *
 *
 */

void mx_diagram_area::deleteGroup(int& err,
    const mx_dg_group_key& groupKey)
{
    mx_dg_group_iterator iterator;
    mx_dg_group_element element;
    mx_dg_group_key nullGroupKey(-1, 0);

    err = MX_ERROR_OK;

    iterator.start(err, this, groupKey, MX_DG_GROUP_GET_LEVEL);
    MX_ERROR_CHECK(err);

    while (iterator.next(err)) {
        element = iterator.getGroupElement();

        deleteFromGroup(err, element.getLevelKey());
        MX_ERROR_CHECK(err);
    }
    MX_ERROR_CHECK(err);

    delete_blob(err, mx_dg_group_element::idGroupStr(groupKey));
    MX_ERROR_CHECK(err);

abort:
    return;
}

/*-------------------------------------------------
 * FUNCTION: mx_diagram_area::getGroupElement
 *
 * DESCRIPTION:
 *
 *
 */

mx_dg_group_element mx_diagram_area::getGroupElement(int& err,
    const mx_dg_group_key& groupKey)
{
    unsigned char buffer[mx_dg_group_element_size];
    unsigned char* ibuffer = buffer;

    mx_dg_group_element retGroupElement;

    err = MX_ERROR_OK;

    if (!groupKey.isNull()) {
        // get data in the first segment
        get_blob_data(err, mx_dg_group_element::idGroupStr(groupKey),
            groupKey.getOffset(), mx_dg_group_element_size, buffer);
        MX_ERROR_CHECK(err);

        ibuffer = buffer;

        retGroupElement.unserialise(err, &ibuffer);
        MX_ERROR_CHECK(err);
    }

abort:
    return retGroupElement;
}

/*-------------------------------------------------
 * FUNCTION: mx_diagram_area::setGroupElement
 *
 * DESCRIPTION:
 *
 *
 */

void mx_diagram_area::setGroupElement(int& err,
    const mx_dg_group_key& groupKey,
    const mx_dg_group_element& groupElement)
{
    unsigned char buffer[mx_dg_group_element_size];
    unsigned char* ibuffer = buffer;

    err = MX_ERROR_OK;

    if (groupKey.isNull())
        return;

    ((mx_dg_group_element&)groupElement).serialise(err, &ibuffer);
    MX_ERROR_CHECK(err);

    // set data in the first segment
    set_blob_data(err, mx_dg_group_element::idGroupStr(groupKey),
        groupKey.getOffset(), mx_dg_group_element_size, buffer);
    MX_ERROR_CHECK(err);
abort:;
}

/*-------------------------------------------------
 * FUNCTION: mx_diagram_area::addToGroup
 *
 * DESCRIPTION:
 *
 *
 */

void mx_diagram_area::addToGroup(int& err,
    const mx_dg_level_key& levelKey,
    const mx_dg_group_key& groupKey)
{
    mx_dg_group_key igroupKey;
    mx_dg_quad_key quadKey;
    mx_dg_group_element groupElement;
    mx_dg_quad* quad;

    err = MX_ERROR_OK;

    int blobSize = get_blob_size(err, mx_dg_group_element::idGroupStr(groupKey));
    MX_ERROR_CHECK(err);

    set_blob_size(err, mx_dg_group_element::idGroupStr(groupKey),
        blobSize + mx_dg_group_element_size);
    MX_ERROR_CHECK(err);

    igroupKey.setGroupId(groupKey.getGroupId());
    igroupKey.setOffset(blobSize);
    groupElement.setLevelKey(levelKey);

    setGroupElement(err, groupKey, groupElement);
    MX_ERROR_CHECK(err);

    quad = quadFromLevel(err, levelKey, quadKey);
    MX_ERROR_CHECK(err);

    quad->setGroup(err, quadKey, igroupKey);
    MX_ERROR_CHECK(err);

abort:;
}

/*-------------------------------------------------
 * FUNCTION: mx_diagram_area::deleteFromGroup
 *
 * DESCRIPTION:
 *
 *
 */

void mx_diagram_area::deleteFromGroup(int& err,
    const mx_dg_level_key& levelKey)
{
    mx_dg_group_key nullGroupKey;
    mx_dg_level_key nullLevelKey;
    mx_dg_quad* quad;
    mx_dg_quad_key quadKey;
    mx_dg_group_key groupKey;
    mx_dg_group_element element;

    quad = quadFromLevel(err, levelKey, quadKey);
    MX_ERROR_CHECK(err);

    groupKey = quad->getGroup(err, quadKey);
    MX_ERROR_CHECK(err);

    quad->setGroup(err, quadKey, nullGroupKey);
    MX_ERROR_CHECK(err);

    element.setLevelKey(nullLevelKey);

    setGroupElement(err, groupKey, element);
    MX_ERROR_CHECK(err);

abort:;
}
