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
 * MODULE : mx_dg_quad.C
 *
 * AUTHOR : David Miller
 *
 * This file
 *
 * DESCRIPTION:
 * Module mx_dg_quad.C
 *
 *
 *
 *
 * $id$
 * $log$
 *
 */

#include "mx_dg_area.h"

mx_dg_quad_geom mx_dg_quad::workQuadGeom;

/*-------------------------------------------------
 * FUNCTION: mx_dg_quad::mx_dg_quad
 *
 * DESCRIPTION:
 *
 *
 */

mx_dg_quad::mx_dg_quad()
{
    lowerQuads = NULL;
    idString = NULL;
    da = NULL;
}

/*-------------------------------------------------
 * FUNCTION: mx_dg_quad::~mx_dg_quad
 *
 * DESCRIPTION:
 *
 *
 */

mx_dg_quad::~mx_dg_quad()
{
    delete[] lowerQuads;
    delete[] idString;
}

/*-------------------------------------------------
 * FUNCTION: mx_dg_quad::idStr
 *
 * DESCRIPTION:
 *
 *
 */

void mx_dg_quad::idStr()
{
    char* stringBuffer;

    stringBuffer = squadId(levelKey, hwm);

    idString = new char[strlen(stringBuffer) + 1];

    strcpy(idString, stringBuffer);
}

/*-------------------------------------------------
 * FUNCTION: mx_dg_quad::idStr
 *
 * DESCRIPTION:
 *
 *
 */

char* mx_dg_quad::squadId(const mx_dg_level_key& ilevelKey,
    const mx_dg_quad_key& key)
{
    static uint8 lastLevelId = 0;
    static uint8 lastDepth = 0;
    static uint16 lastQuad = 0;

    static char stringBuffer[100] = "gd0_0_0";

    if ((lastQuad != key.getQuadId()) || (lastDepth != key.getDepth()) || (lastLevelId != ilevelKey.getLevelId())) {
        sprintf(stringBuffer, "gd%d_%d_%d",
            ilevelKey.getLevelId(),
            key.getDepth(),
            key.getQuadId());

        lastQuad = key.getQuadId();
        lastDepth = key.getDepth();
        lastLevelId = ilevelKey.getLevelId();
    }

    return stringBuffer;
}

/*-------------------------------------------------
 * FUNCTION: mx_dg_quad::set
 *
 * DESCRIPTION: Initialise a quad, optionally creating
 *              a new blob for geometry data
 *
 */

void mx_dg_quad::set(int& err,
    bool newQuad,
    mx_diagram_area* ida,
    mx_dg_quad* iparent,
    const mx_dg_level_key& ilevelKey,
    const mx_point& icentre,
    const mx_point& isize,
    const mx_dg_quad_key& key)
{
    mx_point size2 = isize * 0.5;

    err = MX_ERROR_OK;

    // set centre and size
    centre = icentre;
    size = isize;

    hwm = key;
    levelKey = ilevelKey;

    // set the extent
    quadExtent(centre, size, extent);

    // set the id string
    idStr();

    if (newQuad) {
        ida->create_blob(err, idString);
        MX_ERROR_CHECK(err);

        hwm.setOffset(0);
    }

    da = ida;
    parent = iparent;

    return;
abort:
    return;
}

/*-------------------------------------------------
 * FUNCTION: mx_dg_quad::quadExtent
 *
 */

void mx_dg_quad::quadExtent(mx_point& icentre,
    mx_point& isize,
    mx_rect& iextent)
{
    mx_point size2 = isize * 0.5;

    iextent.xb = icentre.x - size2.x;
    iextent.yb = icentre.y - size2.y;
    iextent.xt = icentre.x + size2.x;
    iextent.yt = icentre.y + size2.y;
}

/*-------------------------------------------------
 * FUNCTION: mx_dg_quad::nextQuadCoords
 *
 * DESCRIPTION: Given a quad get the coordinates of
 * the quads beneath the quad
 *
 *                  -------------------->  x
 *                  |
 *                  |         |
 *                  |     0   |   3
 *                  |         |
 *                  |   --------------
 *                  |         |
 *                  |     1   |   2
 *              y   |         |
 *
 */

void mx_dg_quad::nextQuadCoords(mx_point iCentre,
    mx_point iSize,
    mx_point& nCentre,
    mx_point& nSize,
    int quadNo)
{
    mx_point nSize2 = iSize * 0.25;

    // size halves
    nSize = iSize * 0.5;

    switch (quadNo) {
    case 0:
        nCentre = iCentre - nSize2;
        break;
    case 1:
        nCentre.x = iCentre.x - nSize2.x;
        nCentre.y = iCentre.y + nSize2.y;
        break;
    case 2:
        nCentre = iCentre + nSize2;
        break;
    case 3:
        nCentre.x = iCentre.x + nSize2.x;
        nCentre.y = iCentre.y - nSize2.y;
        break;
    }
}

/*-------------------------------------------------
 * FUNCTION: mx_dg_quad::unserialise
 *
 * DESCRIPTION: Unserialise a quad from part of a
 *              buffer
 *              This routine updates the offset
 *              into the buffer as it reads the
 *              the quad data
 *
 */

void mx_dg_quad::unserialise(int& err,
    uint8** buffer)
{
    int iquad;
    mx_point nCentre, nSize;
    bool lowerQuadsExist;
    mx_dg_quad_key key;
    int32 ihwm;

    err = MX_ERROR_OK;

    unserialise_int32(ihwm, buffer);
    unserialise_bool(lowerQuadsExist, buffer);

    hwm.setOffset(ihwm);

    // test if quad has lower quads
    if (lowerQuadsExist) {
        // not the end - get the lower quads
        lowerQuads = new mx_dg_quad[4];

        // serialise the lower quads
        for (iquad = 0; iquad < 4; iquad++) {
            nextQuadCoords(centre, size, nCentre, nSize, iquad);

            // set centre and size

            hwm.next(iquad, key);

            lowerQuads[iquad].set(err, FALSE, da, this,
                levelKey, nCentre, nSize, key);
            MX_ERROR_CHECK(err);

            lowerQuads[iquad].unserialise(err, buffer);
            MX_ERROR_CHECK(err);
        }
    }

abort:
    return;
}

/*-------------------------------------------------
 * FUNCTION: mx_dg_quad::garbageCollect
 *
 * DESCRIPTION:
 *
 *
 */

void mx_dg_quad::garbageCollect(int& err)
{
    err = MX_ERROR_OK;
}

/*-------------------------------------------------
 * FUNCTION: mx_dg_quad::serialise
 *
 * DESCRIPTION: Serialise a quad into a buffer.
 * The quad may need to grow the buffer so returns
 * to the caller any new address, new size or offset
 */

void mx_dg_quad::serialise(int& err,
    uint8** buffer)
{
    // set boolean if lower quads exist
    bool lowerQuadsExist = (lowerQuads != NULL);
    int iquad;

    err = MX_ERROR_OK;

    // store hwm
    serialise_int32(hwm.getOffset(), buffer);

    serialise_bool(lowerQuadsExist, buffer);

    if (lowerQuadsExist) {
        // serialise lower quads
        for (iquad = 0; iquad < 4; iquad++) {
            lowerQuads[iquad].serialise(err, buffer);
            MX_ERROR_CHECK(err);
        }
    }

abort:
    return;
}

/*-------------------------------------------------
 * FUNCTION: mx_dg_quad::get_serialised_size
 *
 * DESCRIPTION: Serialise a quad into a buffer.
 * The quad may need to grow the buffer so returns
 * to the caller any new address, new size or offset
 */

uint32 mx_dg_quad::get_serialised_size(int& err)
{
    int iquad;
    int size;
    bool lowerQuadsExist = (lowerQuads != NULL);

    err = MX_ERROR_OK;

    size = SLS_INT32 + SLS_BOOL;

    // set boolean if lower quads exist

    if (lowerQuadsExist) {
        // serialise lower quads
        for (iquad = 0; iquad < 4; iquad++) {
            size += lowerQuads[iquad].get_serialised_size(err);
            MX_ERROR_CHECK(err);
        }
    }

abort:
    return size;
}

/*-------------------------------------------------
 * FUNCTION: mx_dg_quad::boxToQuad
 *
 * DESCRIPTION: determine the quad a box goes in.
 * Return -1 if the box does not go into a lower
 * quad
 */

int mx_dg_quad::boxToQuad(const mx_rect& box)
{
    if (box.xt < centre.x) {
        if (box.yt < centre.y) {
            return 0;
        } else if (box.yb > centre.y) {
            return 1;
        }
    } else if (box.xb > centre.x) {
        if (box.yt < centre.y) {
            return 3;
        } else if (box.yb > centre.y) {
            return 2;
        }
    }

    return -1;
}

/*-------------------------------------------------
 * FUNCTION: mx_dg_quad::reQuad
 *
 * DESCRIPTION:
 *
 *
 */

void mx_dg_quad::reQuad(int& err,
    bool properRequad)
{
    int iquad;
    mx_point nCentre, nSize;
    int nextQuad;
    bool stillInStep = TRUE;
    mx_dg_iterator iterator;
    mx_dg_quad_geom& iquadGeom = iterator.getQuadGeom();
    mx_dg_quad_key newHwm;
    bool skippedSome;

    err = MX_ERROR_OK;

    if (properRequad) {
        // allocate lower  quads

        lowerQuads = new mx_dg_quad[4];

        for (iquad = 0; iquad < 4; iquad++) {
            nextQuadCoords(centre, size, nCentre, nSize, iquad);

            // set centre and size
            hwm.next(iquad, newHwm);

            lowerQuads[iquad].set(err, TRUE, da, this, levelKey,
                nCentre, nSize, newHwm);
            MX_ERROR_CHECK(err);
        }

        iterator.setAllowInactive(TRUE);
    }

    // start an iterator just for this quad

    iterator.setGetType(MX_DG_QGEOM_GET_GBUFFER);
    iterator.setSegmentGetType(MX_DG_SEGMENT_INDIVIDUAL);
    iterator.setGroupInformation(FALSE);

    iterator.start(err,
        da,
        &levelKey,
        &hwm);
    MX_ERROR_CHECK(err);

    newHwm = hwm;
    newHwm.setOffset(0);

    while (iterator.nextGeometry(err, skippedSome)) {
        nextQuad = properRequad ? boxToQuad(iquadGeom.box) : -1;

        if (nextQuad != -1) {
            mx_dg_quad* thisQuad = lowerQuads + nextQuad;
            mx_dg_level* level = da->getLevel(iquadGeom.getLevelKey());

            if (iquadGeom.needsLevel()) {
                level->setQuad(err, iquadGeom.getLevelKey(), thisQuad->getHwm());
                MX_ERROR_CHECK(err);
            }

            thisQuad->addNewGeometry(err, iquadGeom);
            MX_ERROR_CHECK(err);

            stillInStep = FALSE;
        } else {
            // still in this quad - do we need to re add

            if (stillInStep)
                stillInStep = !skippedSome;

            if (!stillInStep) {
                mx_dg_level* level = da->getLevel(iquadGeom.getLevelKey());

                if (iquadGeom.needsLevel()) {
                    level->setQuad(err, iquadGeom.getLevelKey(), newHwm);
                    MX_ERROR_CHECK(err);
                }

                writeGeometry(err, newHwm, iquadGeom);
                MX_ERROR_CHECK(err);
            }

            newHwm += iquadGeom.writeSize();
        }
    }
    MX_ERROR_CHECK(err);

    // reset blob size if it has shrinked
    if (!stillInStep) {
        da->set_blob_size(err, idString, newHwm.getOffset());
        MX_ERROR_CHECK(err);

        hwm = newHwm;
    }

    return;
abort:
    return;
}

/*-------------------------------------------------
 * FUNCTION: mx_dg_quad::addGeometry
 *
 * DESCRIPTION: Add a geometry to a quad. Only
 * ever called on the top quad
 *
 */

mx_dg_quad_key mx_dg_quad::addGeometry(int& err,
    mx_dg_quad_geom& quadGeom)
{
    int nextQuad;
    int newHwm;
    int quant;
    mx_dg_quad_key retQuadKey;
    mx_dg_group_key nullKey;

    err = MX_ERROR_OK;

    // see if the geometry fits in a lower box
    if (lowerQuads == NULL) {
        nextQuad = -1;
    } else {
        nextQuad = boxToQuad(quadGeom.box);
    }

    if (nextQuad != -1) {
        // add to a lower quad
        retQuadKey = lowerQuads[nextQuad].addGeometry(err, quadGeom);
        MX_ERROR_CHECK(err);

        return retQuadKey;
    }

    // if the first time over the quantisation size - then
    // requad

    newHwm = hwm.getOffset() + quadGeom.writeSize();
    quant = da->getQuant();

    if ((lowerQuads == NULL) && (newHwm > quant) && (hwm.getDepth() < hwm.maxDepth)) {
        // temporarily save the geometry buffer - requading may overwrite the
        // global buffer

        // requad this quad
        reQuad(err, TRUE);
        MX_ERROR_CHECK(err);

        // and add data again
        retQuadKey = addGeometry(err, quadGeom);
        MX_ERROR_CHECK(err);

        return retQuadKey;
    }

    // add the size of the quad geom buffer

    retQuadKey = hwm;

    quadGeom.setQuadKey(hwm);

    da->addNewGeometry(err, quadGeom);
    MX_ERROR_CHECK(err);

    // now add any segments which might need to be added

    if (quadGeom.isSegmentHeader()) {
        for (int isegment = 0; isegment < quadGeom.segmentTable.nsegments; isegment++) {
            mx_geom* tgeom = quadGeom.getGeom();

            quadGeom.setGeometry(err,
                *tgeom,
                quadGeom.getLevelKey(),
                FALSE,
                nullKey,
                isegment);
            MX_ERROR_CHECK(err);

            addGeometry(err, quadGeom);
            MX_ERROR_CHECK(err);
        }
    }

abort:
    return retQuadKey;
}

/*-------------------------------------------------
 * FUNCTION: mx_dg_quad::addNewGeometry
 *
 * DESCRIPTION: Add a geometry to a quad
 *
 *
 */

void mx_dg_quad::addNewGeometry(int& err,
    const mx_dg_quad_geom& quadGeom)
{
    err = MX_ERROR_OK;

    da->set_blob_size(err, idString,
        hwm.getOffset() + quadGeom.writeSize());
    MX_ERROR_CHECK(err);

    writeGeometry(err, hwm, quadGeom);
    MX_ERROR_CHECK(err);

    hwm += quadGeom.writeSize();

abort:;
}

/*-------------------------------------------------
 * FUNCTION: mx_dg_quad::writeGeometry
 *
 * DESCRIPTION: Add a geometry to a quad
 *
 *
 */

void mx_dg_quad::writeGeometry(int& err,
    const mx_dg_quad_key& key,
    const mx_dg_quad_geom& quadData)
{
    uint8 buff[SLS_INT32];
    uint8* buffPtr = buff;
    int offset = key.getOffset();

    err = MX_ERROR_OK;

    serialise_int32(quadData.quadGeomDataSize, &buffPtr);

    // set data in the first segment
    da->set_blob_data(err, idString, offset, SLS_INT32, buff);
    MX_ERROR_CHECK(err);

    offset += SLS_INT32;

    da->set_blob_data(err, idString, offset,
        quadData.quadGeomDataSize,
        quadData.quadGeomBuffer);
    MX_ERROR_CHECK(err);

    if (quadData.geomDataSize > 0) {
        offset += quadData.quadGeomDataSize;

        da->set_blob_data(err, idString, offset,
            quadData.geomDataSize,
            quadData.geomBuffer);
        MX_ERROR_CHECK(err);
    }

    if (quadData.isSubsequentSegment()) {
        mx_dg_quad_key parentQuadKey;
        mx_dg_quad* parentQuad = da->quadFromLevel(err, quadData.getLevelKey(),
            parentQuadKey);
        MX_ERROR_CHECK(err);

        parentQuad->setTableEntry(err, parentQuadKey,
            quadData.getSegmentNumber(),
            key);
        MX_ERROR_CHECK(err);
    }

abort:;
}

/*-------------------------------------------------
 * FUNCTION: mx_dg_quad::copyData
 *
 * DESCRIPTION: Add a geometry in a particular level
 *
 *
 */

void mx_dg_quad::copyData(int& err,
    mx_dg_quad& inQuad)
{
    mx_diagram_area* thisArea = da;
    mx_diagram_area* thatArea = inQuad.getDiagramArea();
    int iquad;

    thisArea->copy_blob(err, TRUE, idString,
        thatArea, idString);
    MX_ERROR_CHECK(err);

    // now draw the lower quads
    if (lowerQuads != NULL) {
        for (iquad = 0; iquad < 4; iquad++) {
            lowerQuads[iquad].copyData(err, inQuad.lowerQuads[iquad]);
            MX_ERROR_CHECK(err);
        }
    }

    return;
abort:
    return;
}

/*-------------------------------------------------
 * FUNCTION: mx_dg_quad::nextQuad
 *
 * DESCRIPTION: Get the next quad for an iteration
 *
 *
 */

mx_dg_quad* mx_dg_quad::nextQuad()
{
    mx_dg_quad* parentQuad = this;
    int testDepth = hwm.getDepth();
    int currentSubQuad;

    if (lowerQuadsExist()) {
        // return the first lower quad
        return lowerQuads;
    } else if (testDepth == 0) {
        return NULL;
    } else {
        do {
            testDepth--;
            currentSubQuad = hwm.depthQuad(testDepth);
            parentQuad = parentQuad->parent;
        } while ((parentQuad != NULL) && (currentSubQuad == 3));

        if (parentQuad == NULL) {
            return NULL;
        } else {
            return (parentQuad->lowerQuads + currentSubQuad + 1);
        }
    }
}

/*-------------------------------------------------
 * FUNCTION: mx_dg_quad::setLevel
 *
 * DESCRIPTION: Get the next quad for an iteration
 *
 *
 */

void mx_dg_quad::setLevel(int& err,
    const mx_dg_quad_key& quadKey,
    const mx_dg_level_key& levelKey)
{
    uint8 buf[mx_dg_level_key_size];
    uint8* tmpBuffer;

    tmpBuffer = (uint8*)buf;

    ((mx_dg_level&)levelKey).serialise(err, &tmpBuffer);
    MX_ERROR_CHECK(err);

    da->set_blob_data(err, idString,
        quadKey.getOffset() + mx_dg_quad_geom::levelOffset,
        mx_dg_quad_geom::levelSize,
        buf);
    MX_ERROR_CHECK(err);

abort:;
}

/*-------------------------------------------------
 * FUNCTION: mx_dg_quad::getLevel
 *
 * DESCRIPTION: Get the next quad for an iteration
 *
 *
 */

mx_dg_level_key mx_dg_quad::getLevel(int& err,
    const mx_dg_quad_key& quadKey)
{
    uint8 buf[mx_dg_level_key_size];
    uint8* tmpBuffer;
    mx_dg_level_key retLevelKey;

    da->get_blob_data(err, idString,
        quadKey.getOffset() + mx_dg_quad_geom::levelOffset,
        mx_dg_quad_geom::levelSize,
        buf);
    MX_ERROR_CHECK(err);

    tmpBuffer = (uint8*)buf;
    retLevelKey.unserialise(err, &tmpBuffer);
    MX_ERROR_CHECK(err);

abort:;
    return retLevelKey;
}

/*-------------------------------------------------
 * FUNCTION: mx_dg_quad::setFlags
 *
 * DESCRIPTION: Get the next quad for an iteration
 *
 *
 */

void mx_dg_quad::setFlags(int& err,
    const mx_dg_quad_key& quadKey,
    uint8 flags)
{
    uint8 buf[mx_dg_quad_geom::geomFlagSize];
    uint8* tmpBuffer;

    tmpBuffer = (uint8*)buf;
    serialise_uint8(flags, &tmpBuffer);

    da->set_blob_data(err, idString,
        quadKey.getOffset() + mx_dg_quad_geom::geomFlagOffset,
        mx_dg_quad_geom::geomFlagSize,
        buf);
    MX_ERROR_CHECK(err);

abort:;
}

/*-------------------------------------------------
 * FUNCTION: mx_dg_quad::getGroup
 *
 * DESCRIPTION:
 *
 *
 */

uint8 mx_dg_quad::getFlags(int& err,
    const mx_dg_quad_key& quadKey)
{
    uint8 flags;
    uint8 buf[mx_dg_quad_geom::geomFlagSize];
    uint8* tmpBuffer;

    da->get_blob_data(err, idString,
        quadKey.getOffset() + mx_dg_quad_geom::geomFlagOffset,
        mx_dg_quad_geom::geomFlagSize,
        buf);
    MX_ERROR_CHECK(err);

    tmpBuffer = (uint8*)buf;

    unserialise_uint8(flags, &tmpBuffer);
    MX_ERROR_CHECK(err);

abort:;
    return flags;
}

/*-------------------------------------------------
 * FUNCTION: mx_dg_quad::setGroup
 *
 * DESCRIPTION: Get the next quad for an iteration
 *
 *
 */

void mx_dg_quad::setGroup(int& err,
    const mx_dg_quad_key& quadKey,
    const mx_dg_group_key& groupKey)
{
    uint8 buf[mx_dg_group_key_size];
    uint8* tmpBuffer;

    tmpBuffer = (uint8*)buf;
    ((mx_dg_group_key&)groupKey).serialise(err, &tmpBuffer);
    MX_ERROR_CHECK(err);

    da->set_blob_data(err, idString,
        quadKey.getOffset() + mx_dg_quad_geom::groupOffset,
        mx_dg_quad_geom::groupSize,
        buf);
    MX_ERROR_CHECK(err);

abort:;
}

/*-------------------------------------------------
 * FUNCTION: mx_dg_quad::getGroup
 *
 * DESCRIPTION:
 *
 *
 */

mx_dg_group_key mx_dg_quad::getGroup(int& err,
    const mx_dg_quad_key& quadKey)
{
    uint8 buf[mx_dg_group_key_size];
    uint8* tmpBuffer;
    mx_dg_group_key retGroupKey;

    da->get_blob_data(err, idString,
        quadKey.getOffset() + mx_dg_quad_geom::groupOffset,
        mx_dg_quad_geom::groupSize,
        buf);
    MX_ERROR_CHECK(err);

    tmpBuffer = (uint8*)buf;

    retGroupKey.unserialise(err, &tmpBuffer);
    MX_ERROR_CHECK(err);

abort:;
    return retGroupKey;
}

/*-------------------------------------------------
 * FUNCTION: mx_dg_quad::deleteGeometry
 *
 * DESCRIPTION: Get the next quad for an iteration
 *
 *
 */

void mx_dg_quad::deleteGeometry(int& err,
    const mx_dg_level_key& levelKey,
    const mx_dg_quad_key& quadKey)
{
    uint8 flags;
    mx_dg_group_element nullElement;
    mx_dg_group_key groupKey;

    err = MX_ERROR_OK;

    workQuadGeom.extractFromBlob(err, da, levelKey, quadKey,
        MX_DG_QGEOM_GET_QGEOM, TRUE, NULL);
    MX_ERROR_CHECK(err);

    flags = workQuadGeom.flags;
    flags &= (~((uint8)(MX_DG_QUAD_ACTIVE)));

    setFlags(err, quadKey, flags);
    MX_ERROR_CHECK(err);

    groupKey = workQuadGeom.getGroupKey();

    if ((!groupKey.isNull()) && (!workQuadGeom.isSubsequentSegment())) {
        da->setGroupElement(err, groupKey, nullElement);
        MX_ERROR_CHECK(err);
    }

    // if it is segmented we need to delete all
    // the other segments too

    if (workQuadGeom.isFirstSegment()) {
        for (int is = 0; is < workQuadGeom.segmentTable.nsegments; is++) {
            mx_dg_segment* thisSegment = workQuadGeom.segmentTable.segments + is;
            mx_dg_quad* deleteQuad = da->getQuad(levelKey,
                thisSegment->quadKey);
            deleteQuad->deleteGeometry(err,
                levelKey,
                thisSegment->quadKey);
            MX_ERROR_CHECK(err);
        }
    }

abort:;
}

/*-------------------------------------------------
 * FUNCTION: mx_dg_quad::undeleteGeometry
 *
 * DESCRIPTION: Get the next quad for an iteration
 *
 *
 */

void mx_dg_quad::undeleteGeometry(int& err,
    const mx_dg_level_key& levelKey,
    const mx_dg_quad_key& quadKey)
{
    uint8 flags;
    mx_dg_group_element groupElement(levelKey);
    mx_dg_group_key groupKey;

    err = MX_ERROR_OK;

    workQuadGeom.extractFromBlob(err, da, levelKey, quadKey,
        MX_DG_QGEOM_GET_QGEOM, TRUE, NULL);
    MX_ERROR_CHECK(err);

    flags = workQuadGeom.flags;
    flags |= MX_DG_QUAD_ACTIVE;

    setFlags(err, quadKey, flags);
    MX_ERROR_CHECK(err);

    groupKey = workQuadGeom.getGroupKey();

    if ((!groupKey.isNull()) && (!workQuadGeom.isSubsequentSegment())) {
        da->setGroupElement(err, groupKey, groupElement);
        MX_ERROR_CHECK(err);
    }

    // if it is segmented we need to delete all
    // the other segments too

    if (workQuadGeom.isFirstSegment()) {
        for (int is = 0; is < workQuadGeom.segmentTable.nsegments; is++) {
            mx_dg_segment* thisSegment = workQuadGeom.segmentTable.segments + is;
            mx_dg_quad* deleteQuad = da->getQuad(levelKey,
                thisSegment->quadKey);
            deleteQuad->undeleteGeometry(err,
                levelKey,
                thisSegment->quadKey);
            MX_ERROR_CHECK(err);
        }
    }

abort:;
}

/*-------------------------------------------------
 * FUNCTION: mx_dg_quad::setTableEntry
 *
 * DESCRIPTION: Get the next quad for an iteration
 *
 *
 */

void mx_dg_quad::setTableEntry(int& err,
    const mx_dg_quad_key& quadKey,
    int isegment,
    const mx_dg_quad_key& segmentQuadKey)
{
    uint8 buf[mx_dg_quad_key_size];
    uint8* tmpBuffer;
    int offset;

    err = MX_ERROR_OK;

    tmpBuffer = (uint8*)buf;

    ((mx_dg_quad_key&)segmentQuadKey).serialise(err, &tmpBuffer);
    MX_ERROR_CHECK(err);

    offset = quadKey.getOffset() + SLS_INT32 + mx_dg_quad_geom::segmentTableSegmentOffset
        + isegment * mx_dg_segment::size + mx_dg_segment::quadKeyOffset;

    da->set_blob_data(err, idString,
        offset,
        mx_dg_quad_key_size,
        buf);
    MX_ERROR_CHECK(err);
abort:;
}
