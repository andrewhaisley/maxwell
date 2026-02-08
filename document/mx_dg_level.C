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
 * MODULE : mx_dg_level.C
 *
 * AUTHOR : David Miller
 *
 * This file
 *
 * DESCRIPTION:
 * Module mx_dg_level.C
 *
 *
 * Revision 1.1  1996/10/06 13:15:45  andrew
 * Initial revision
 *
 *
 */

#include "mx_dg_area.h"

int mx_dg_level_iterator::numExtentIdAnswers;
int mx_dg_level_iterator::numAllocExtentIdAnswers;
uint8* mx_dg_level_iterator::extentIdAnswers;

std::map<uint32, uint8 *> mx_dg_level_iterator::m_extentLookUp;

/*-------------------------------------------------
 * FUNCTION: mx_dg_level::idStr
 *
 * DESCRIPTION:
 *
 *
 */

void mx_dg_level::idStr()
{
    char stringBuffer[100];

    sprintf(stringBuffer, "gl%d", hwm.getLevelId());

    idString = new char[strlen(stringBuffer) + 1];

    strcpy(idString, stringBuffer);
}

/*-------------------------------------------------
 * FUNCTION: mx_dg_level::mx_dg_level
 *
 * DESCRIPTION:
 *
 *
 */

mx_dg_level::mx_dg_level()
    : hwm(0, 0)
    , head(0)
    , tail(0)
{
    idString = nullptr;
    name = nullptr;
    noGarbageCollectNeeded = true;
}

/*-------------------------------------------------
 * FUNCTION: mx_dg_level::~mx_dg_level
 *
 * DESCRIPTION:
 *
 *
 */

mx_dg_level::~mx_dg_level()
{
    delete[] idString;
    delete[] name;
}

/*-------------------------------------------------
 * FUNCTION: mx_dg_level::set
 *
 * DESCRIPTION: Set values on brand new level in memory
 *
 *
 */

void mx_dg_level::set(int& err,
    mx_diagram_area* ida,
    const char* iname,
    const mx_dg_level_key& hwmKey,
    bool isNew)
{
    mx_dg_quad_key quadKey;

    err = MX_ERROR_OK;

    hwm = hwmKey;
    da = ida;
    idStr();
    name = nullptr;

    if (iname != nullptr) {
        name = new char[strlen(iname) + 1];
        strcpy(name, iname);
    }

    if (isNew) {
        da->create_blob(err, idString);
        MX_ERROR_CHECK(err);

        topQuad.set(err, true,
            da, nullptr,
            hwm,
            da->getQuadCentre(),
            da->getQuadSize(),
            quadKey);
        MX_ERROR_CHECK(err);
    }
abort:;
}

/*-------------------------------------------------
 * FUNCTION: mx_dg_level::get_serialised_size
 *
 * DESCRIPTION:
 *
 *
 */

uint32 mx_dg_level::get_serialised_size(int& err)
{
    uint32 size = 3 * SLS_INT32 + SLS_STRING(name);

    err = MX_ERROR_OK;

    size += topQuad.get_serialised_size(err);
    MX_ERROR_CHECK(err);

abort:
    return size;
}

/*-------------------------------------------------
 * FUNCTION: mx_dg_level::serialise
 *
 * DESCRIPTION:
 *
 *
 */

void mx_dg_level::serialise(int& err,
    uint8** buffer)
{
    serialise_int32(hwm.getOffset(), buffer);
    serialise_int32(head.getOffset(), buffer);
    serialise_int32(tail.getOffset(), buffer);
    serialise_string(name, buffer);

    topQuad.serialise(err, buffer);
    MX_ERROR_CHECK(err);

abort:
    return;
}

/*-------------------------------------------------
 * FUNCTION: mx_dg_level::unserialise
 *
 * DESCRIPTION: Unserialise a level
 *
 *
 */

void mx_dg_level::unserialise(int& err,
    uint8** buffer)
{
    // by here we have already set the level id
    // on the hwm
    uint8 levelId = hwm.getLevelId();
    int32 offset;
    char iname[1000];
    mx_dg_quad_key quadKey;

    unserialise_int32(offset, buffer);
    hwm.set(levelId, offset);

    unserialise_int32(offset, buffer);
    head.set(levelId, offset);

    unserialise_int32(offset, buffer);
    tail.set(levelId, offset);

    unserialise_string(iname, buffer);
    name = new char[strlen(iname) + 1];
    strcpy(name, iname);

    topQuad.set(err, false,
        da, nullptr,
        hwm,
        da->getQuadCentre(),
        da->getQuadSize(),
        quadKey);
    MX_ERROR_CHECK(err);

    topQuad.unserialise(err, buffer);
    MX_ERROR_CHECK(err);

abort:
    return;
}

/*-------------------------------------------------
 * FUNCTION: mx_dg_level::undeleteGeometry
 *
 * DESCRIPTION: Add a geometry to a level
 *
 *
 */

void mx_dg_level::undeleteGeometry(int& err,
    const mx_dg_level_key& levelKey,
    const mx_dg_level_key& nextLevelKey,
    const mx_dg_level_key& prevLevelKey)
{
    mx_dg_quad* quad;
    mx_dg_quad_key quadKey;

    // put back in the chain

    setNext(err, prevLevelKey, levelKey);
    MX_ERROR_CHECK(err);

    setPrev(err, nextLevelKey, levelKey);
    MX_ERROR_CHECK(err);

    if (nextLevelKey.isNull())
        head = levelKey;
    if (prevLevelKey.isNull())
        tail = levelKey;

    quadKey = getQuad(err, levelKey);
    MX_ERROR_CHECK(err);

    quad = da->getQuad(levelKey, quadKey);

    quad->undeleteGeometry(err, levelKey, quadKey);
    MX_ERROR_CHECK(err);

abort:;
    return;
}

/*-------------------------------------------------
 * FUNCTION: mx_dg_level::deleteGeometry
 *
 * DESCRIPTION: Add a geometry to a level
 *
 *
 */

void mx_dg_level::deleteGeometry(int& err,
    const mx_dg_level_key& levelKey,
    mx_dg_level_key& nextLevelKey,
    mx_dg_level_key& prevLevelKey)
{
    mx_dg_quad* quad;
    mx_dg_quad_key quadKey;

    // take out of the chain

    nextLevelKey = getNext(err, levelKey);
    MX_ERROR_CHECK(err);

    prevLevelKey = getPrev(err, levelKey);
    MX_ERROR_CHECK(err);

    setNext(err, prevLevelKey, nextLevelKey);
    MX_ERROR_CHECK(err);

    setPrev(err, nextLevelKey, prevLevelKey);
    MX_ERROR_CHECK(err);

    if (nextLevelKey.isNull())
        head = prevLevelKey;
    if (prevLevelKey.isNull())
        tail = nextLevelKey;

    quadKey = getQuad(err, levelKey);
    MX_ERROR_CHECK(err);

    quad = da->getQuad(levelKey, quadKey);

    quad->deleteGeometry(err, levelKey, quadKey);
    MX_ERROR_CHECK(err);

abort:;
    return;
}

/*-------------------------------------------------
 * FUNCTION: mx_dg_level::addGeometry
 *
 * DESCRIPTION: Add a geometry to a level
 *
 *
 */

mx_dg_level_key mx_dg_level::addGeometry(int& err,
    mx_geom& geom)
{
    mx_dg_level_key retLevelKey = hwm;
    mx_dg_group_key nullKey;
    mx_dg_quad_geom& quadGeom = mx_dg_quad::getWorkQuadGeom();

    // set at top level
    quadGeom.setGeometry(err, geom, hwm, false, nullKey, -1);
    MX_ERROR_CHECK(err);

    // add to the geometry
    topQuad.addGeometry(err, quadGeom);
    MX_ERROR_CHECK(err);

abort:;
    return retLevelKey;
}

/*-------------------------------------------------
 * FUNCTION: mx_dg_level::addNewGeometry
 *
 * DESCRIPTION: Add a geometry to a level
 *
 *
 */

void mx_dg_level::addNewGeometry(int& err,
    const mx_dg_level_key& levelKey,
    const mx_dg_quad_key& quadKey)
{
    // we are going to add depth,quadId,offset and
    // next offset into the table

    mx_dg_level_element levelElement;
    mx_dg_level_key nullKey(levelKey.getLevelId());

    err = MX_ERROR_OK;

    da->set_blob_size(err, idString,
        hwm.getOffset() + mx_dg_level_element::size());
    MX_ERROR_CHECK(err);

    levelElement.setNext(nullKey);
    levelElement.setPrev(head);
    levelElement.setQuadKey(quadKey);

    setElement(err, hwm, levelElement);
    MX_ERROR_CHECK(err);

    setNext(err, head, hwm);
    MX_ERROR_CHECK(err);

    // set variables last
    head = hwm;
    if (tail.isNull())
        tail = hwm;

    hwm += mx_dg_level_element::size();

    return;
abort:;
}

/*-------------------------------------------------
 * FUNCTION: mx_dg_level_draw_iterator::processFunction
 *
 * DESCRIPTION: Draw a level
 *
 *
 */

void mx_dg_level_draw_iterator::processFunction(int& err,
    mx_dg_quad_geom& quadGeom,
    bool& advance,
    bool& done,
    bool skippedSome)
{
    err = MX_ERROR_OK;

    done = false;
    advance = true;

    if (quadGeom.isSegmentHeader()) {
        for (int isegment = 0; isegment < quadGeom.segmentTable.nsegments; isegment++) {
            mx_dg_segment* thisSegment = quadGeom.segmentTable.segments + isegment;

            if ((thisSegment->isFiltered()) && (thisSegment->selectGeom != nullptr)) {
                device->draw(err, thisSegment->selectGeom);
                MX_ERROR_CHECK(err);
            }
        }
    } else {
        device->draw(err, quadGeom.getGeom());
        MX_ERROR_CHECK(err);
    }

abort:;
}

/*-------------------------------------------------
 * FUNCTION: mx_dg_level::draw
 *
 * DESCRIPTION: Draw a level
 *
 *
 */

void mx_dg_level::draw(int& err,
    mx_style_device* device,
    const mx_rect& r)
{
    // get the static quad geom here
    mx_dg_level_draw_iterator levelIterator;

    err = MX_ERROR_OK;

    levelIterator.setAreaOfInterest(r);
    levelIterator.setSegmentGetType(MX_DG_SEGMENT_MATCHING_WITH_FIRST);
    levelIterator.setStyleDevice(device);

    levelIterator.start(err, da, &hwm);
    MX_ERROR_CHECK(err);

    levelIterator.process(err);
    MX_ERROR_CHECK(err);

    return;
abort:
    return;
}

/*-------------------------------------------------
 * FUNCTION: mx_dg_level::copyData
 *
 * DESCRIPTION: Draw a level
 *
 *
 */

void mx_dg_level::copyData(int& err,
    mx_dg_level& inlevel)
{
    mx_diagram_area* thatArea = inlevel.getDiagramArea();

    err = MX_ERROR_OK;

    // copy the level data

    da->copy_blob(err, true, idString,
        thatArea, idString);
    MX_ERROR_CHECK(err);

    topQuad.copyData(err, inlevel.topQuad);
    MX_ERROR_CHECK(err);

abort:
    return;
}

/*-------------------------------------------------
 * FUNCTION: mx_dg_level_iterator::start
 *
 * DESCRIPTION: Reset a geometry in the level list
 *              Occurs during requading
 *
 */

void mx_dg_level_iterator::start(int& err,
    mx_diagram_area* ida,
    const mx_dg_level_key* const ilevelKey,
    const mx_dg_quad_key* const iquadKey)
{
    err = MX_ERROR_OK;

    da = ida;

    currentLevel = da->getLevel(*ilevelKey);
    levelKey = currentLevel->getTail();

    numExtentIdAnswers = 0;
    m_extentLookUp.clear();
}

/*-------------------------------------------------
 * FUNCTION: mx_dg_level::quadIdToExtent
 *
 * DESCRIPTION:
 *
 *
 */

void mx_dg_level::quadIdToExtent(const mx_dg_quad_key& key,
    mx_rect& orect)
{
    mx_point centre = topQuad.getCentre();
    mx_point size = topQuad.getSize();
    uint8 currentDepth = 0;
    int currentQuadId;

    while (currentDepth < key.getDepth()) {
        currentQuadId = key.depthQuad(currentDepth);

        mx_dg_quad::nextQuadCoords(centre, size,
            centre, size,
            currentQuadId);
        currentDepth++;
    }

    mx_dg_quad::quadExtent(centre, size, orect);
}

/*-------------------------------------------------
 * FUNCTION: mx_dg_level_iterator::testAreaOfInterest
 *
 * DESCRIPTION:
 *
 *
 */

bool mx_dg_level_iterator::testAreaOfInterest(int& err)
{
    bool inters;
    mx_rect extent;
    mx_dg_quad_key key;
    uint32 depthQuadId;

    err = MX_ERROR_OK;

    if (!filter.testFilter(MX_DG_FILTER_CHECK_BOX))
        return true;

    if (!currentLevel->getTopQuad()->lowerQuadsExist())
        return true;

    key = quadGeom.getQuadKey();

    // test if in the hash table
    depthQuadId = key.compress();

    if (m_extentLookUp.find(depthQuadId) == m_extentLookUp.end()) {
        if (numExtentIdAnswers == numAllocExtentIdAnswers) {
            uint8* oldAnswers = extentIdAnswers;

            numAllocExtentIdAnswers += 128;

            extentIdAnswers = new uint8[numAllocExtentIdAnswers];

            memcpy(extentIdAnswers, oldAnswers, numExtentIdAnswers * sizeof(uint8));

            delete[] oldAnswers;
        }

        currentLevel->quadIdToExtent(key, extent);

        // if doesnt intersect with quad go to next geometry
        inters = intersects(extent);

        extentIdAnswers[numExtentIdAnswers] = inters;
        m_extentLookUp[depthQuadId] = extentIdAnswers + numExtentIdAnswers;
        numExtentIdAnswers++;

        return inters;
    } else {
        MX_ERROR_CHECK(err);

        return m_extentLookUp[depthQuadId];
    }
abort:
    return true;
}

/*-------------------------------------------------
 * FUNCTION: mx_dg_level_iterator::advanceKey
 *
 * DESCRIPTION:
 *
 *
 */

void mx_dg_level_iterator::advanceKey(int& err)
{
    err = MX_ERROR_OK;

    levelKey = currentLevel->getNext(err, levelKey);
    MX_ERROR_CHECK(err);
abort:;
}

/*-------------------------------------------------
 * FUNCTION: mx_dg_level_iterator::nextGeometry
 *
 * DESCRIPTION:
 *
 *
 */

bool mx_dg_level_iterator::nextGeometryIntern(int& err,
    bool doingProcess,
    bool& reachedEnd,
    bool& skippedSome)
{
    bool found;
    mx_rect extent;
    mx_dg_quad_key key;
    bool returnFound;

    while (!levelKey.isNull()) {
        if (groupInformation) {
            mx_dg_group_key groupKey;

            groupKey = currentLevel->getGroup(err, levelKey);
            MX_ERROR_CHECK(err);

            quadGeom.setGroupKey(groupKey);
            MX_ERROR_CHECK(err);
        }

        if (quadInformation || (qgeomGetType != MX_DG_QGEOM_GET_NONE)) {
            key = currentLevel->getQuad(err, levelKey);
            MX_ERROR_CHECK(err);

            quadGeom.setQuadKey(key);
            MX_ERROR_CHECK(err);
        }

        if (qgeomGetType != MX_DG_QGEOM_GET_NONE) {
            found = testAreaOfInterest(err);
            MX_ERROR_CHECK(err);

            if (found) {
                found = quadGeom.extractFromBlob(err,
                    da,
                    levelKey,
                    key,
                    qgeomGetType,
                    allowInactive,
                    &filter);
                MX_ERROR_CHECK(err);
            }
        } else {
            found = true;
        }

        returnFound = advance(err, doingProcess, found, skippedSome);
        MX_ERROR_CHECK(err);

        if (returnFound)
            return true;
    }

    reachedEnd = true;

abort:
    return false;
}

/*-------------------------------------------------
 * FUNCTION: mx_dg_level::getGeometry
 *
 * DESCRIPTION: Extract a single geometry from a level
 *
 */

bool mx_dg_level::getGeometry(int& err,
    bool doExtract,
    const mx_dg_quad_key& quadKey,
    mx_geom** geom)
{
    mx_dg_quad_geom& iquadGeom = mx_dg_quad::getWorkQuadGeom();
    bool found;

    err = MX_ERROR_OK;

    found = iquadGeom.extractFromBlob(err, da, hwm, quadKey,
        MX_DG_QGEOM_GET_GEOM, false,
        nullptr);
    MX_ERROR_CHECK(err);

    if (doExtract) {
        *geom = iquadGeom.extractGeom();
    } else {
        *geom = iquadGeom.getGeom();
    }

    return found;
abort:
    return false;
}

/*-------------------------------------------------
 * FUNCTION: mx_dg_level::setElement
 *
 * DESCRIPTION: Reset a geometry in the level list
 *              Occurs during requading
 *
 */

void mx_dg_level::setElement(int& err,
    const mx_dg_level_key& key,
    const mx_dg_level_element& element)
{
    // we are going to add depth,quadId,offset and
    // next offset into the table
    unsigned char buffer[mx_dg_level_element::size()];
    unsigned char* ibuffer = buffer;

    err = MX_ERROR_OK;

    if (key.isNull())
        return;

    ((mx_dg_level_element&)element).serialise(err, &ibuffer);
    MX_ERROR_CHECK(err);

    // set data in the first segment
    da->set_blob_data(err, idString, key.getOffset(),
        mx_dg_level_element::size(), buffer);
    MX_ERROR_CHECK(err);

abort:;
}

/*-------------------------------------------------
 * FUNCTION: mx_dg_level::setQuad
 *
 * DESCRIPTION: Reset a geometry in the level list
 *              Occurs during requading
 *
 */

void mx_dg_level::setQuad(int& err,
    const mx_dg_level_key& key,
    const mx_dg_quad_key& quadKey)
{
    // we are going to add depth,quadId,offset and
    // next offset into the table

    unsigned char buffer[mx_dg_quad_key::size()];
    unsigned char* ibuffer = buffer;

    if (key.isNull())
        return;

    err = MX_ERROR_OK;

    ((mx_dg_quad_key&)quadKey).serialise(err, &ibuffer);
    MX_ERROR_CHECK(err);

    // set data in the first segment
    da->set_blob_data(err, idString,
        key.getOffset() + mx_dg_level_element::quadOffset,
        mx_dg_quad_key::size(), buffer);
    MX_ERROR_CHECK(err);

abort:;
}

/*-------------------------------------------------
 * FUNCTION: mx_dg_level::setNext
 *
 * DESCRIPTION: Reset a geometry in the level list
 *              Occurs during reordering
 *
 */

void mx_dg_level::setNext(int& err,
    const mx_dg_level_key& key,
    const mx_dg_level_key& inext)
{
    // reset the next field

    unsigned char buffer[mx_dg_level_element::nextSize];
    unsigned char* ibuffer = buffer;

    err = MX_ERROR_OK;

    if (key.isNull())
        return;

    serialise_int32(inext.getOffset(), &ibuffer);
    MX_ERROR_CHECK(err);

    // set data in the first segment
    da->set_blob_data(err, idString,
        key.getOffset() + mx_dg_level_element::nextOffset,
        mx_dg_level_element::nextSize, buffer);
    MX_ERROR_CHECK(err);

abort:;
}

/*-------------------------------------------------
 * FUNCTION: mx_dg_level::setPrev
 *
 * DESCRIPTION: Reset a geometry in the level list
 *              Occurs during reordering
 *
 */

void mx_dg_level::setPrev(int& err,
    const mx_dg_level_key& key,
    const mx_dg_level_key& iprev)
{
    // reset the prev field

    unsigned char buffer[mx_dg_level_element::prevSize];
    unsigned char* ibuffer = buffer;

    err = MX_ERROR_OK;

    if (key.isNull())
        return;

    serialise_int32(iprev.getOffset(), &ibuffer);
    MX_ERROR_CHECK(err);

    // set data in the first segment
    da->set_blob_data(err, idString,
        key.getOffset() + mx_dg_level_element::prevOffset,
        mx_dg_level_element::prevSize, buffer);
    MX_ERROR_CHECK(err);

abort:;
}

/*-------------------------------------------------
 * FUNCTION: mx_dg_level::getQuad
 *
 * DESCRIPTION: Reset a geometry in the level list
 *              Occurs during requading
 *
 */

mx_dg_quad_key mx_dg_level::getQuad(int& err,
    const mx_dg_level_key& key)
{
    const uint8* buffer;
    mx_dg_quad_key iquadKey;

    err = MX_ERROR_OK;

    if (key.isNull())
        return iquadKey;

    // set data in the first segment
    buffer = da->get_blob_readonly(err, idString,
        key.getOffset() + mx_dg_level_element::quadOffset,
        mx_dg_quad_key::size());
    MX_ERROR_CHECK(err);

    iquadKey.unserialise(err, (uint8**)&buffer);
    MX_ERROR_CHECK(err);

abort:
    return iquadKey;
}

/*-------------------------------------------------
 * FUNCTION: mx_dg_level::getNext
 *
 * DESCRIPTION: Reset a geometry in the level list
 *              Occurs during reordering
 *
 */

mx_dg_level_key mx_dg_level::getNext(int& err,
    const mx_dg_level_key& key)
{
    // reset the next field
    const uint8* buffer;
    int32 offset;
    mx_dg_level_key inext;

    err = MX_ERROR_OK;

    if (key.isNull())
        return inext;

    // set data in the first segment
    buffer = da->get_blob_readonly(err, idString,
        key.getOffset() + mx_dg_level_element::nextOffset,
        mx_dg_level_element::nextSize);
    MX_ERROR_CHECK(err);

    unserialise_int32(offset, (uint8**)&buffer);
    MX_ERROR_CHECK(err);

    inext.set(hwm.getLevelId(), offset);

abort:
    return inext;
}

/*-------------------------------------------------
 * FUNCTION: mx_dg_level::getPrev
 *
 * DESCRIPTION: Reset a geometry in the level list
 *              Occurs during reordering
 *
 */

mx_dg_level_key mx_dg_level::getPrev(int& err,
    const mx_dg_level_key& key)
{
    // reset the prev field
    const uint8* buffer;
    int32 offset;
    mx_dg_level_key iprev;

    err = MX_ERROR_OK;

    if (key.isNull())
        return iprev;

    // set data in the first segment
    buffer = da->get_blob_readonly(err, idString, key.getOffset() + mx_dg_level_element::prevOffset,
        mx_dg_level_element::prevSize);
    MX_ERROR_CHECK(err);

    unserialise_int32(offset, (uint8**)&buffer);
    MX_ERROR_CHECK(err);

    iprev.set(hwm.getLevelId(), offset);

abort:
    return iprev;
}

/*-------------------------------------------------
 * FUNCTION: mx_dg_level::getGroup
 *
 * DESCRIPTION: Reset a geometry in the level list
 *              Occurs during reordering
 *
 */

mx_dg_group_key mx_dg_level::getGroup(int& err,
    const mx_dg_level_key& key)
{
    mx_dg_group_key groupKey;
    mx_dg_quad_geom quadGeom;
    mx_dg_quad_key quadKey;

    bool found;

    err = MX_ERROR_OK;

    if (key.isNull())
        return groupKey;

    quadKey = getQuad(err, key);
    MX_ERROR_CHECK(err);

    quadGeom.setSegmentGetType(MX_DG_SEGMENT_INDIVIDUAL);

    found = quadGeom.extractFromBlob(err, da, key, quadKey,
        MX_DG_QGEOM_GET_QGEOM, true,
        nullptr);
    MX_ERROR_CHECK(err);

    if (found)
        groupKey = quadGeom.getGroupKey();

abort:
    return groupKey;
}

/*-------------------------------------------------
 * FUNCTION: mx_dg_level_element::get_serialised_size
 *
 * DESCRIPTION:
 *
 *
 */

uint32 mx_dg_level_element::get_serialised_size(int& err)
{
    err = MX_ERROR_OK;

    return mx_dg_level_element::size();
}

/*-------------------------------------------------
 * FUNCTION: mx_dg_level_element::serialise
 *
 * DESCRIPTION:
 *
 *
 */

void mx_dg_level_element::serialise(int& err,
    uint8** buffer)
{
    err = MX_ERROR_OK;

    serialise_int32(next.getOffset(), buffer);
    serialise_int32(prev.getOffset(), buffer);

    quadKey.serialise(err, buffer);
    MX_ERROR_CHECK(err);

abort:;
}

/*-------------------------------------------------
 * FUNCTION: mx_dg_level_element::unserialise
 *
 * DESCRIPTION:
 *
 *
 */

void mx_dg_level_element::unserialise(int& err,
    uint8** buffer)
{
    int32 offset;

    err = MX_ERROR_OK;

    unserialise_int32(offset, buffer);
    next.set(0, offset);

    unserialise_int32(offset, buffer);
    prev.set(0, offset);

    quadKey.unserialise(err, buffer);
    MX_ERROR_CHECK(err);

abort:;
}

/*-------------------------------------------------
 * FUNCTION: mx_dg_level_garbage_iterator
 *
 * DESCRIPTION:
 *
 *
 */

void mx_dg_level_garbage_iterator::start(int& err,
    mx_diagram_area* ida,
    const mx_dg_level_key* const ilevelKey,
    const mx_dg_quad_key* const iquadKey)
{
    mx_dg_level_iterator::start(err, ida, ilevelKey, iquadKey);
    MX_ERROR_CHECK(err);

    nlevelGeoms = 0;

    levelKey = *ilevelKey;

    sprintf(idString, "glg_%d", ilevelKey->getLevelId());

    da->create_blob(err, idString);
    MX_ERROR_CHECK(err);

abort:;
}

/*-------------------------------------------------
 * FUNCTION: mx_dg_level_garbage_iterator
 *
 * DESCRIPTION:
 *
 *
 */

void mx_dg_level_garbage_iterator::processFunction(int& err,
    mx_dg_quad_geom& quadGeom,
    bool& advance,
    bool& done,
    bool skippedSome)
{
    unsigned char buffer[mx_dg_level_garbage_size];
    unsigned char* ibuffer = buffer;
    int offset;

    advance = true;
    done = false;

    // add the quad and group information to a list

    offset = nlevelGeoms * mx_dg_level_garbage_size;

    da->set_blob_size(err, idString, offset + mx_dg_level_garbage_size);
    MX_ERROR_CHECK(err);

    quadGeom.getLevelKey().serialise(err, &ibuffer);
    MX_ERROR_CHECK(err);

    da->set_blob_data(err, idString, offset,
        mx_dg_level_garbage_size, buffer);
    MX_ERROR_CHECK(err);

    nlevelGeoms++;

abort:;
}

/*-------------------------------------------------
 * FUNCTION: mx_dg_level_garbage_iterator
 *
 * DESCRIPTION:
 *
 *
 */

void mx_dg_level_garbage_iterator::end(int& err)
{
    int ielement;
    mx_dg_level* level = da->getLevel(levelKey);
    const uint8* buffer;
    int offset = 0;
    mx_dg_quad_key quadKey;
    mx_dg_group_key groupKey;
    mx_dg_level_key outLevelKey(levelKey.getLevelId(), 0);
    mx_dg_level_key inLevelKey(levelKey.getLevelId(), 0);
    mx_dg_group_element groupElement;
    mx_dg_quad* quad;

    err = MX_ERROR_OK;

    // set the level HWM to zero

    for (ielement = 0; ielement < nlevelGeoms; ielement++) {
        buffer = da->get_blob_readonly(err, idString, offset, mx_dg_level_garbage_size);
        MX_ERROR_CHECK(err);

        inLevelKey.unserialise(err, (uint8**)&buffer);
        MX_ERROR_CHECK(err);

        quad = da->quadFromLevel(err, inLevelKey, quadKey);
        MX_ERROR_CHECK(err);

        groupKey = quad->getGroup(err, quadKey);
        MX_ERROR_CHECK(err);

        level->setQuad(err, outLevelKey, quadKey);
        MX_ERROR_CHECK(err);

        quad->setLevel(err, quadKey, outLevelKey);
        MX_ERROR_CHECK(err);

        groupElement.setLevelKey(outLevelKey);

        da->setGroupElement(err,
            groupKey,
            groupElement);
        MX_ERROR_CHECK(err);

        offset += mx_dg_level_garbage_size;
        outLevelKey += mx_dg_level_element_size;
    }

    da->set_blob_size(err, level->idString, outLevelKey.getOffset());
    MX_ERROR_CHECK(err);

    da->delete_blob(err, idString);
    MX_ERROR_CHECK(err);

abort:;
}
/*-------------------------------------------------
 * FUNCTION: mx_dg_level::garbageCollect
 *
 * DESCRIPTION:
 *
 *
 */

void mx_dg_level::garbageCollect(int& err)
{
    mx_dg_level_garbage_iterator levelIterator;

    err = MX_ERROR_OK;

    if (noGarbageCollectNeeded)
        return;

    levelIterator.setGetType(MX_DG_QGEOM_GET_NONE);
    levelIterator.setGroupInformation(true);
    levelIterator.setQuadInformation(true);

    levelIterator.start(err, da, &hwm);
    MX_ERROR_CHECK(err);

    levelIterator.process(err);
    MX_ERROR_CHECK(err);

abort:;
}
