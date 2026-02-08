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
 * MODULE/CLASS : mx_diagram_area
 *
 * AUTHOR : Andrew Haisley
 *
 *
 *
 * DESCRIPTION:
 *
 *
 *
 */

#include <mx.h>

#include <mx_dg_area.h>
#include <mx_dg_paint.h>

mx_diagram_area::~mx_diagram_area()
{
    int ilevel;
    mx_dg_level_list* thisLevelList;

    for (ilevel = 0; ilevel < maxLevels; ilevel++) {
        thisLevelList = levelLists[ilevel];
        delete thisLevelList;
    }
}

void mx_diagram_area::initLevels()
{
    int ilevel;

    for (ilevel = 0; ilevel < maxLevels; ilevel++) {
        levelLists[ilevel] = (mx_dg_level_list*)NULL;
    }
}

mx_diagram_area::mx_diagram_area(uint32 docid, mx_paragraph_style* s, bool is_controlled)
    : mx_area(docid, s, is_controlled)
{
    // get a handle on a suitable painter
    painter = mx_diagram_area_painter::painter();

    quant = 65536;

    initLevels();

    drawAddOn = 5; // add on 5mm
}

mx_diagram_area::mx_diagram_area(uint32 docid, uint32 oid, bool is_controlled)
    : mx_area(docid, oid, is_controlled)
{
    initLevels();

    // dont unserialise yet
    //    unserialise_attributes(error) ;
    //    MX_ERROR_CHECK(error);

    // get a handle on a suitable painter
    painter = mx_diagram_area_painter::painter();
}

mx_diagram_area::mx_diagram_area(const mx_diagram_area& a)
    : mx_area(*((mx_area*)&a))
{
    unsigned char *tmp, *buffer;
    int bufferSize;
    int ilevel;
    mx_diagram_area& ar = (mx_diagram_area&)a;
    mx_dg_level* thisLevel;
    mx_dg_level* thisInLevel;
    mx_dg_level_list* thisLevelList;

    MX_ERROR_CHECK(error);

    initLevels();

    // serialise the input buffer first

    bufferSize = ar.get_serialised_size(error);
    MX_ERROR_CHECK(error);

    tmp = new unsigned char[bufferSize];
    buffer = tmp;

    ar.serialise(error, &buffer);
    MX_ERROR_CHECK(error);

    buffer = tmp;
    unserialise(error, &buffer);
    MX_ERROR_CHECK(error);

    for (ilevel = 0; ilevel < maxLevels; ilevel++) {
        thisLevelList = levelLists[ilevel];

        if (thisLevelList != NULL) {
            thisLevel = thisLevelList->getLevel();

            if (thisLevel != NULL) {
                thisInLevel = ar.getLevel(thisLevel->getHwm());

                thisLevel->copyData(error, *thisInLevel);
                MX_ERROR_CHECK(error);
            }
        }
    }

    delete[] tmp;

abort:
    return;
}

void mx_diagram_area::serialise_attributes(int& err)
{
    int32 blob_size;
    uint8 *buffer, *tmp;

    err = MX_ERROR_OK;

    blob_size = get_serialised_size(err);
    MX_ERROR_CHECK(err);

    delete_attribute(err, mx_diagram_area::areaIndex());
    MX_ERROR_CLEAR(err);

    create_blob(err, mx_diagram_area::areaIndex());
    if (err == MX_DB_CLIENT_CACHE_DUPLICATE_BLOB) {
        MX_ERROR_CLEAR(err);
    } else {
        MX_ERROR_CHECK(err);
    }

    tmp = buffer = new uint8[blob_size];

    set_blob_size(err, mx_diagram_area::areaIndex(), blob_size);
    MX_ERROR_CHECK(err);

    serialise(err, &buffer);
    MX_ERROR_CHECK(err);

    set_blob_data(err, mx_diagram_area::areaIndex(), 0, blob_size, (uint8*)tmp);
    MX_ERROR_CHECK(err);

    delete[] tmp;

abort:
    return;
}

void mx_diagram_area::unserialise_attributes(int& err)
{
    int32 size;

    const uint8* buffer;
    uint8 *temp_buffer, *tmp;

    size = get_blob_size(err, mx_diagram_area::areaIndex());
    if (err != MX_ERROR_OK) {
        MX_ERROR_CLEAR(err);

        const mx_attribute* a;
        uint8* tmp;

        a = get_readonly(err, mx_diagram_area::areaIndex());
        MX_ERROR_CHECK(err);

        tmp = (uint8*)a->value.ca;

        unserialise(err, &tmp);
        MX_ERROR_CHECK(err);
    } else {
        if (size > MX_DB_OBJECT_MAX_READONLY_BLOB_SIZE) {
            tmp = temp_buffer = new uint8[size];

            get_blob_data(err, mx_diagram_area::areaIndex(), 0, size, temp_buffer);
            MX_ERROR_CHECK(err);

            unserialise(err, &temp_buffer);
            MX_ERROR_CHECK(err);

            delete[] tmp;
        } else {
            buffer = get_blob_readonly(err, mx_diagram_area::areaIndex(), 0, size);
            MX_ERROR_CHECK(err);

            unserialise(err, (uint8**)&buffer);
            MX_ERROR_CHECK(err);
        }
    }

abort:
    return;
}

mx_dg_level_key mx_diagram_area::getNextLevelKey(const mx_dg_level_key& levelKey) const
{
    mx_dg_level_key retKey;
    mx_dg_level_list* thisLevelList = levelLists[levelKey.getLevelId()];

    if (thisLevelList != NULL) {
        retKey = thisLevelList->getNext();
    }

    return retKey;
}

void mx_diagram_area::serialise(int& err, uint8** buffer)
{
    uint8 nlevels = 0;
    mx_dg_level_key serialiseLevelKey = tail;

    err = MX_ERROR_OK;

    serialise_int32(quant, buffer);
    serialise_double(drawAddOn, buffer);
    serialise_int32(nextGroupKey.getGroupId(), buffer);

    quadSize.serialise(err, buffer);
    MX_ERROR_CHECK(err);

    quadCentre.serialise(err, buffer);
    MX_ERROR_CHECK(err);

    while (!serialiseLevelKey.isNull()) {
        nlevels++;
        serialiseLevelKey = getNextLevelKey(serialiseLevelKey);
    }

    serialise_uint8(nlevels, buffer);

    serialiseLevelKey = tail;
    while (!serialiseLevelKey.isNull()) {
        serialise_uint8(serialiseLevelKey.getLevelId(), buffer);
        serialiseLevelKey = getNextLevelKey(serialiseLevelKey);
    }

    serialiseLevelKey = tail;
    while (!serialiseLevelKey.isNull()) {
        getLevel(serialiseLevelKey)->serialise(err, buffer);
        MX_ERROR_CHECK(err);

        serialiseLevelKey = getNextLevelKey(serialiseLevelKey);
    }

    styles.serialise(err, buffer);
    MX_ERROR_CHECK(err);

abort:
    return;
}

void mx_diagram_area::unserialise(int& err, uint8** buffer)
{
    uint8 nlevels;
    uint8 thisLevelId;
    int32 nextGroupId;
    mx_dg_level* thisLevel;
    mx_dg_level_list* thisLevelList;
    mx_dg_level_list* lastLevelList = NULL;
    mx_dg_level_key thisLevelKey;
    mx_dg_level_key lastLevelKey;
    mx_dg_level_key serialiseLevelKey;

    err = MX_ERROR_OK;

    unserialise_int32(quant, buffer);
    unserialise_double(drawAddOn, buffer);
    unserialise_int32(nextGroupId, buffer);

    nextGroupKey.set(nextGroupId, 0);

    quadSize.unserialise(err, buffer);
    MX_ERROR_CHECK(err);

    quadCentre.unserialise(err, buffer);
    MX_ERROR_CHECK(err);

    unserialise_uint8(nlevels, buffer);

    // recreate the list

    for (int ilevel = 0; ilevel < nlevels; ilevel++) {
        thisLevelList = new mx_dg_level_list;

        unserialise_uint8(thisLevelId, buffer);

        thisLevelKey.set(thisLevelId, 0);

        if (ilevel == 0) {
            tail = thisLevelKey;
        } else {
            if (lastLevelList != NULL)
                lastLevelList->setNext(thisLevelKey);
        }

        thisLevelList->setPrev(lastLevelKey);

        levelLists[thisLevelId] = thisLevelList;

        lastLevelList = thisLevelList;
        lastLevelKey = thisLevelKey;
    }

    serialiseLevelKey = tail;
    while (!serialiseLevelKey.isNull()) {
        thisLevel = new mx_dg_level;
        thisLevelList = levelLists[serialiseLevelKey.getLevelId()];

        thisLevelKey.set(serialiseLevelKey.getLevelId(), 0);

        // tell the level about its id

        thisLevel->set(err, this, NULL, thisLevelKey, FALSE);
        MX_ERROR_CHECK(err);

        thisLevel->unserialise(err, buffer);
        MX_ERROR_CHECK(err);

        thisLevelList->setLevel(thisLevel);

        serialiseLevelKey = getNextLevelKey(serialiseLevelKey);
    }

    styles.unserialise(err, buffer);
    MX_ERROR_CHECK(err);

abort:
    return;
}

uint32 mx_diagram_area::get_serialised_size(int& err)
{
    mx_dg_level_key serialiseLevelKey = tail;

    int32 size = SLS_UINT8 + (2 * SLS_INT32) + SLS_DOUBLE;

    err = MX_ERROR_OK;

    size += quadSize.get_serialised_size(err);
    MX_ERROR_CHECK(err);

    size += quadCentre.get_serialised_size(err);
    MX_ERROR_CHECK(err);

    while (!serialiseLevelKey.isNull()) {
        // serialise the id
        size += SLS_UINT8;

        getLevel(serialiseLevelKey)->get_serialised_size(err);
        MX_ERROR_CHECK(err);

        serialiseLevelKey = getNextLevelKey(serialiseLevelKey);
    }

    size += styles.get_serialised_size(err);
    MX_ERROR_CHECK(err);

abort:
    return size;
}

void mx_diagram_area::crop(int& err, const mx_rect& r)
{
    err = MX_ERROR_OK;
}

void mx_diagram_area::scale(int& err, mx_point& new_size)
{
    err = MX_ERROR_OK;
}

void mx_diagram_area::uncache(int& err)
{
    err = MX_ERROR_OK;
}

uint32 mx_diagram_area::memory_size(int& err)
{
    return 0;
}

mx_area* mx_diagram_area::duplicate()
{
    mx_diagram_area* res;

    res = new mx_diagram_area(*this);

    return (mx_area*)res;
}

/*-------------------------------------------------
 * FUNCTION: mx_diagram_area::initialiseQuadSize
 *
 * DESCRIPTION: Add a new level to a diagram. Return
 * the new level identifier
 *
 */

void mx_diagram_area::initialiseQuadSize(const mx_point& iquadSize)
{
    // check that we have some levels

    if (!tail.isNull())
        return;

    quadSize = iquadSize;
    quadCentre = quadSize * 0.5;
}

/*-------------------------------------------------
 * FUNCTION: mx_diagram_area::addLevel
 *
 * DESCRIPTION: Add a new level to a diagram. Return
 * the new level identifier
 *
 */

mx_dg_level_key mx_diagram_area::addLevel(int& err,
    const char* name)
{
    mx_dg_level_key headLevelKey;
    mx_dg_level_key nextLevelKey;
    mx_dg_level_key newLevelKey;
    mx_dg_level_list* thisLevelList;
    mx_dg_level_list* lastLevelList;
    mx_dg_level* thisLevel;

    err = MX_ERROR_OK;

    for (int ilevel = 0; ilevel < maxLevels; ilevel++) {
        thisLevelList = levelLists[ilevel];

        if (thisLevelList == NULL) {
            newLevelKey.set(ilevel, 0);
            break;
        }
    }

    if (newLevelKey.isNull()) {
        // too many levels
        MX_ERROR_THROW(err, MX_DG_TOO_MANY_LEVELS);
    }

    // get the head

    thisLevelList = new mx_dg_level_list;

    levelLists[newLevelKey.getLevelId()] = thisLevelList;

    headLevelKey = tail;
    do {
        nextLevelKey = getNextLevelKey(headLevelKey);
        if (nextLevelKey.isNull())
            break;

        headLevelKey = nextLevelKey;
    } while (1);

    if (!headLevelKey.isNull()) {
        lastLevelList = levelLists[headLevelKey.getLevelId()];
        lastLevelList->setNext(newLevelKey);
    }

    thisLevelList->setPrev(headLevelKey);
    if (tail.isNull())
        tail = newLevelKey;

    thisLevel = new mx_dg_level;
    thisLevelList->setLevel(thisLevel);

    thisLevel->set(err, this, name, newLevelKey, TRUE);
    MX_ERROR_CHECK(err);

abort:;
    return newLevelKey;
}

/*-------------------------------------------------
 * FUNCTION: mx_diagram_area::deleteGeometry
 *
 * DESCRIPTION: Add a geometry in a particular level
 *
 *
 */

void mx_diagram_area::deleteGeometry(int& err,
    const mx_dg_level_key& levelKey,
    mx_dg_level_key& nextLevelKey,
    mx_dg_level_key& prevLevelKey)
{
    mx_dg_level* thisLevel;

    err = MX_ERROR_OK;

    cc_unserialise_attributes(err);
    MX_ERROR_CHECK(err);

    // Add geometry to level

    thisLevel = getLevel(levelKey);

    if (thisLevel == NULL) {
        MX_ERROR_THROW(err, MX_INVALID_LEVEL);
    }

    thisLevel->deleteGeometry(err, levelKey, nextLevelKey, prevLevelKey);
    MX_ERROR_CHECK(err);
abort:;
}

/*-------------------------------------------------
 * FUNCTION: mx_diagram_area::undeleteGeometry
 *
 * DESCRIPTION: Add a geometry in a particular level
 *
 *
 */

void mx_diagram_area::undeleteGeometry(int& err,
    const mx_dg_level_key& levelKey,
    const mx_dg_level_key& nextLevelKey,
    const mx_dg_level_key& prevLevelKey)
{
    mx_dg_level* thisLevel;

    err = MX_ERROR_OK;

    cc_unserialise_attributes(err);
    MX_ERROR_CHECK(err);

    // Add geometry to level

    thisLevel = getLevel(levelKey);

    if (thisLevel == NULL) {
        MX_ERROR_THROW(err, MX_INVALID_LEVEL);
    }

    thisLevel->undeleteGeometry(err, levelKey, nextLevelKey, prevLevelKey);
    MX_ERROR_CHECK(err);
abort:;
}

/*-------------------------------------------------
 * FUNCTION: mx_diagram_area::addGeometry
 *
 * DESCRIPTION: Add a geometry in a particular level
 *
 *
 */

mx_dg_level_key mx_diagram_area::addGeometry(int& err,
    const mx_dg_level_key& levelKey,
    mx_geom& geom)
{
    mx_dg_level_key retLevel;

    mx_dg_level* thisLevel;

    err = MX_ERROR_OK;

    cc_unserialise_attributes(err);
    MX_ERROR_CHECK(err);

    // Add geometry to level

    thisLevel = getLevel(levelKey);

    if (thisLevel == NULL) {
        MX_ERROR_THROW(err, MX_INVALID_LEVEL);
    }

    retLevel = thisLevel->addGeometry(err, geom);
    MX_ERROR_CHECK(err);

abort:
    return retLevel;
}

/*-------------------------------------------------
 * FUNCTION: mx_diagram_area::draw
 *
 * DESCRIPTION: Add a geometry in a particular level
 *
 *
 */

void mx_diagram_area::draw(int& err,
    mx_device* device,
    mx_doc_rect_t& r,
    int firstLevel)
{
    mx_rect rx;
    mx_dg_level* thisLevel;
    mx_dg_level_key drawLevelKey;

    err = MX_ERROR_OK;

    cc_unserialise_attributes(err);
    MX_ERROR_CHECK(err);

    // expand the rectangle to include any wide lines etc
    rx = r.r;
    rx.xb -= drawAddOn;
    rx.yb -= drawAddOn;
    rx.xt += drawAddOn;
    rx.yb += drawAddOn;

    styleDevice.setDevice(device);
    styleDevice.setStyles(&styles);

    drawLevelKey = tail;
    while (!drawLevelKey.isNull()) {
        thisLevel = getLevel(drawLevelKey);

        thisLevel->draw(err, &styleDevice, rx);
        MX_ERROR_CHECK(err);

        drawLevelKey = getNextLevelKey(drawLevelKey);
    }

    return;
abort:
    return;
}

/*-------------------------------------------------
 * FUNCTION: mx_diagram_area::getQuad
 *
 * DESCRIPTION: Add a geometry in a particular level
 *
 *
 */

mx_dg_quad* mx_diagram_area::getQuad(const mx_dg_level_key& levelKey,
    const mx_dg_quad_key& quadKey) const
{
    int subQuad;
    mx_dg_quad* retQuad;
    int quadDepth = quadKey.getDepth();
    mx_dg_level* level = getLevel(levelKey);

    if (level == NULL)
        return NULL;

    retQuad = level->getTopQuad();

    for (int idepth = 0; idepth < quadDepth; idepth++) {
        subQuad = quadKey.depthQuad(idepth);

        retQuad = retQuad->getSubQuad(subQuad);
        if (retQuad == NULL)
            break;
    }

    return retQuad;
}

/*-------------------------------------------------
 * FUNCTION: mx_diagram_area::quadFromLevel
 *
 * DESCRIPTION: Add a geometry in a particular level
 *
 *
 */

mx_dg_quad* mx_diagram_area::quadFromLevel(int& err,
    const mx_dg_level_key& levelKey,
    mx_dg_quad_key& quadKey)
{
    mx_dg_level* level = getLevel(levelKey);

    err = MX_ERROR_OK;

    quadKey = level->getQuad(err, levelKey);
    MX_ERROR_CHECK(err);

    return getQuad(levelKey, quadKey);
abort:
    return NULL;
}
