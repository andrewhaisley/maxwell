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
 * MODULE : mx_dg_qgeom.C
 *
 * AUTHOR : David Miller
 *
 * This file
 *
 * DESCRIPTION:
 * Module mx_dg_qgeom.C
 *
 *
 *
 *
 * $id$
 * $log$
 *
 */

#include "mx_dg_area.h"

/*-------------------------------------------------
 * FUNCTION: mx_dg_quad_geom::mx_dg_quad_geom
 *
 * DESCRIPTION:
 *
 *
 */

mx_dg_quad_geom::mx_dg_quad_geom()
{
    geomBuffer = NULL;
    geomBufferSize = 0;
    quadGeomBuffer = NULL;
    quadGeomBufferSize = 0;
    groupInformation = FALSE;
    hgeom = NULL;
    mergeGeometries = TRUE;
    segmentGetType = MX_DG_SEGMENT_ALL_WITH_FIRST;
}

/*-------------------------------------------------
 * FUNCTION: mx_dg_quad_geom::~mx_dg_quad_geom
 *
 * DESCRIPTION:
 *
 *
 */

mx_dg_quad_geom::~mx_dg_quad_geom()
{
    clearGeometry();

    delete[] geomBuffer;
    delete[] quadGeomBuffer;
}

/*-------------------------------------------------
 * FUNCTION: mx_dg_quad_geom::init
 *
 * DESCRIPTION:
 *
 *
 */

void mx_dg_quad_geom::init()
{
    segmentTable.nsegments = 0;
    clearGeometry();
}

/*-------------------------------------------------
 * FUNCTION: mx_dg_quad_geom::init
 *
 * DESCRIPTION:
 *
 *
 */

void mx_dg_quad_geom::initIntern()
{
    clearGeometry(TRUE);

    geomDataSize = 0;
    quadGeomDataSize = 0;
    flags = 0;
    type = MX_GEOM_INVALID;
    stage = MX_DG_QGEOM_GET_NONE;
}

/*-------------------------------------------------
 * FUNCTION: mx_dg_quad_geom::createEmptyGeometry
 *
 * DESCRIPTION: Create a geometry whose data is in
 *              the geomBuffer area
 */

mx_geom* mx_dg_quad_geom::createEmptyGeometry()
{
    mx_geom* rgeom = NULL;

    // create an empty geometry
    switch (type) {
    case MX_GEOM_SPAN:
        rgeom = new gmx_span();
        break;
    case MX_GEOM_RECT:
        rgeom = new gmx_rect();
        break;
    case MX_GEOM_ARC:
        rgeom = new gmx_arc();
        break;
    case MX_GEOM_BEZIER:
        //	geom = new gmx_bezier() ;
        break;
    case MX_GEOM_POLYPOINT: {
        gmx_polypoint* pp = new gmx_polypoint();
        rgeom = pp;

        if (flags & MX_DG_QUAD_IS_AREA) {
            pp->pl.set_area(TRUE);
        } else {
            pp->pl.set_area(FALSE);
        }

        if (flags & MX_DG_QUAD_IS_CLOSED) {
            pp->pl.set_closed(TRUE);
        } else {
            pp->pl.set_closed(FALSE);
        }
    } break;
    default:
        return NULL;
        break;
    }

    setGeom(rgeom, TRUE);

    return rgeom;
}

/*-------------------------------------------------
 * FUNCTION: mx_dg_quad_geom::createGeometry
 *
 * DESCRIPTION: Create a geometry whose data is in
 *              the geomBuffer area
 */

void mx_dg_quad_geom::createGeometry(int& err)
{
    unsigned char* unserialiseBuffer;

    err = MX_ERROR_OK;

    if (getStage() >= MX_DG_QGEOM_GET_GEOM)
        return;

    createEmptyGeometry();

    // unserialise into the geometry
    unserialiseBuffer = geomBuffer;
    hgeom->unserialise(err, &unserialiseBuffer);
    MX_ERROR_CHECK(err);

    setStage(MX_DG_QGEOM_GET_GEOM);

    return;
abort:
    return;
}

/*-------------------------------------------------
 * FUNCTION: mx_dg_quad_geom::get_serialised_size
 *
 * DESCRIPTION: Get the serialised size for the
 *              quad geom not including the geometry
 *              data
 */

uint32 mx_dg_quad_geom::get_serialised_size(int& err)
{
    int size;

    err = MX_ERROR_OK;

    size = SLS_INT32 + SLS_ENUM + SLS_UINT8;

    size += levelKey.get_serialised_size(err);
    MX_ERROR_CHECK(err);

    size += groupKey.get_serialised_size(err);
    MX_ERROR_CHECK(err);

    if (isSegment()) {
        if (isFirstSegment()) {
            // write out the table

            size += segmentTable.get_serialised_size(err);
            MX_ERROR_CHECK(err);

        } else {
            // write out the segment number - quad from level

            size += SLS_INT32;
        }
    }

    if (hgeom->storeBox()) {
        box = hgeom->box(err);
        MX_ERROR_CHECK(err);

        size += box.get_serialised_size(err);
        MX_ERROR_CHECK(err);
    }

abort:
    return size;
}

/*-------------------------------------------------
 * FUNCTION: mx_dg_quad_geom::serialise
 *
 * DESCRIPTION: Get the data associated with a
 * geometry used for quading. This data is the
 * properties flags, the box associated with the
 * geometry and the byte size of the geometry
 */

void mx_dg_quad_geom::serialise(int& err,
    uint8** buffer)
{
    err = MX_ERROR_OK;

    serialise_int32(geomDataSize, buffer);
    serialise_enum(type, buffer);
    serialise_uint8(flags, buffer);

    levelKey.serialise(err, buffer);
    MX_ERROR_CHECK(err);

    groupKey.serialise(err, buffer);
    MX_ERROR_CHECK(err);

    if (isSegment()) {
        if (isFirstSegment()) {
            // write out the table

            segmentTable.serialise(err, buffer);
            MX_ERROR_CHECK(err);
        } else {
            // write out the segment number

            serialise_int32(segmentNumber, buffer);
        }
    }

    if (hgeom->storeBox()) {
        // get box from geometry
        box = hgeom->box(err);
        MX_ERROR_CHECK(err);

        box.serialise(err, buffer);
        MX_ERROR_CHECK(err);
    }

abort:
    return;
}

/*-------------------------------------------------
 * FUNCTION: mx_dg_quad_geom::setGeomBuffer
 *
 * DESCRIPTION: Set the size of the geomBuffer
 *
 */

void mx_dg_quad_geom::setGeomBuffer(int newSize)
{
    geomDataSize = newSize;

    if (newSize <= geomBufferSize)
        return;

    delete[] geomBuffer;

    geomBuffer = new unsigned char[newSize];
    geomBufferSize = newSize;
}

/*-------------------------------------------------
 * FUNCTION: mx_dg_quad_geom::setQuadGeomBuffer
 *
 * DESCRIPTION: Set the size of the SerialiseBuffer
 *
 */

void mx_dg_quad_geom::setQuadGeomBuffer(int newSize)
{
    quadGeomDataSize = newSize;

    if (newSize <= quadGeomBufferSize)
        return;

    delete[] quadGeomBuffer;

    quadGeomBuffer = new unsigned char[newSize];
    quadGeomBufferSize = newSize;
}

/*-------------------------------------------------
 * FUNCTION: mx_dg_quad_geom::unserialise
 *
 * DESCRIPTION: Get the data associated with a
 * geometry used for quading. This data is the
 * properties flags, the box associated with the
 * geometry and the byte size of the geometry
 */

void mx_dg_quad_geom::unserialise(int& err,
    uint8** buffer)
{
    uint16 e;

    unserialise_int32(geomDataSize, buffer);
    unserialise_enum(e, buffer);

    type = (MX_GEOM_TYPE)e;

    unserialise_uint8(flags, buffer);

    levelKey.unserialise(err, buffer);
    MX_ERROR_CHECK(err);

    groupKey.unserialise(err, buffer);
    MX_ERROR_CHECK(err);

    // see if we can get the box coordinates

    if (isSegment()) {
        if (isFirstSegment()) {
            // write out the table

            segmentTable.unserialise(err, buffer);
            MX_ERROR_CHECK(err);
        } else {
            // write out the segment number
            unserialise_int32(segmentNumber, buffer);
        }
    }

    setStage(MX_DG_QGEOM_GET_QGEOM);

    if (hasBox()) {
        box.unserialise(err, buffer);
        MX_ERROR_CHECK(err);

        setStage(MX_DG_QGEOM_GET_GBOX);
    }

abort:
    return;
}

/*-------------------------------------------------
 * FUNCTION: mx_dg_level::segmentGeometry
 *
 * DESCRIPTION: Add a geometry to a level
 *
 *
 */

void mx_dg_quad_geom::segmentGeometry(int& err,
    mx_geom& geom)
{
    mx_dg_quad_key quadKey;
    int segmentId = -1;
    mx_geom* inputGeometry;

    /*XXXXXXXXXXXXXXXXXXXX*/
    int maxSize = 50;

    err = MX_ERROR_OK;

    if (!geom.isSegmentable())
        return;

    segmentTable.nsegments = 0;

    do {
        segmentId = geom.segment(err,
            maxSize,
            segmentId,
            &inputGeometry);
        MX_ERROR_CHECK(err);

        if (inputGeometry == NULL)
            break;

        box = inputGeometry->box(err);
        MX_ERROR_CHECK(err);

        segmentTable.addSegmentInfo(TRUE,
            TRUE,
            inputGeometry,
            box, segmentId,
            quadKey);
        MX_ERROR_CHECK(err);
    } while (1);

abort:;
}

/*-------------------------------------------------
 * FUNCTION: mx_dg_quad_geom::setGeometry
 *
 * DESCRIPTION:
 *
 *
 */

void mx_dg_quad_geom::setGeometry(int& err,
    mx_geom& igeom,
    const mx_dg_level_key& ilevelKey,
    bool isGrouped,
    const mx_dg_group_key& igroupKey,
    int isegmentNumber)
{
    unsigned char* tmp;
    bool topLevel = (isegmentNumber == -1);
    bool serialiseGeometry = TRUE;

    err = MX_ERROR_OK;

    // top level ?
    if (topLevel) {
        init();

        segmentGeometry(err,
            igeom);
        MX_ERROR_CHECK(err);

        setGeom(&igeom, FALSE);

        serialiseGeometry = (segmentTable.nsegments == 0);
    } else {
        initIntern();
        setGeom(segmentTable.getGeom(isegmentNumber), FALSE);
    }

    if (serialiseGeometry) {
        geomDataSize = hgeom->get_serialised_size(err);
        MX_ERROR_CHECK(err);

        setGeomBuffer(geomDataSize);
        MX_ERROR_CHECK(err);

        tmp = geomBuffer;

        hgeom->serialise(err, &tmp);
        MX_ERROR_CHECK(err);
    } else {
        geomDataSize = 0;
    }

    type = hgeom->getType();

    // store a box if required - e.g. for polyline
    flags = MX_DG_QUAD_ACTIVE;

    if (hgeom->storeBox())
        flags |= MX_DG_QUAD_STORE_BOX;
    if (hgeom->dimension() == 2)
        flags |= MX_DG_QUAD_IS_AREA;
    if (hgeom->isClosed())
        flags |= MX_DG_QUAD_IS_CLOSED;

    if (segmentTable.nsegments > 0) {
        flags |= MX_DG_QUAD_IS_SEGMENT;
        if (topLevel)
            flags |= MX_DG_QUAD_FIRST_SEGMENT;
    }

    levelKey = ilevelKey;
    segmentNumber = isegmentNumber;
    groupKey = igroupKey;

    // now serialise the quad geom structure itself
    quadGeomDataSize = get_serialised_size(err);
    MX_ERROR_CHECK(err);

    setQuadGeomBuffer(quadGeomDataSize);

    tmp = quadGeomBuffer;

    serialise(err, &tmp);
    MX_ERROR_CHECK(err);

    box = hgeom->box(err);
    MX_ERROR_CHECK(err);

abort:
    return;
}

/*-------------------------------------------------
 * FUNCTION: mx_dg_quad_geom::setBox
 *
 * DESCRIPTION:
 *
 *
 */

void mx_dg_quad_geom::setBox(int& err)
{
    err = MX_ERROR_OK;

    if (getStage() < MX_DG_QGEOM_GET_GBOX)
        return;

    if (hgeom != NULL) {
        box = hgeom->box(err);
        MX_ERROR_CHECK(err);
    } else {
        MX_ERROR_THROW(err, MX_NO_GEOM_BOX);
        MX_ERROR_CHECK(err);
    }

    setStage(MX_DG_QGEOM_GET_GBOX);

abort:
    return;
}

/*-------------------------------------------------
 * FUNCTION: mx_dg_segment::set
 *
 * DESCRIPTION:
 *
 *
 */

void mx_dg_segment::set(uint8 iflags,
    bool deleteableGeom,
    mx_geom* igeom,
    const mx_rect& ibox,
    int32 iid,
    const mx_dg_quad_key& iquadKey)
{
    flags = iflags;
    deleteableGeom = deleteableGeom;
    selectGeom = igeom;
    box = ibox;
    id = iid;
    quadKey = iquadKey;
}

/*-------------------------------------------------
 * FUNCTION: mx_dg_segment_table::mx_dg_segment_table
 *
 * DESCRIPTION:
 *
 *
 */

mx_dg_segment_table::mx_dg_segment_table()
{
    nsegments = 0;
    nallocsegments = 0;
    segments = NULL;
}

/*-------------------------------------------------
 * FUNCTION: mx_dg_segment_table::mx_dg_segment_table
 *
 * DESCRIPTION:
 *
 *
 */

mx_dg_segment_table::~mx_dg_segment_table()
{
    delete[] segments;
}

/*-------------------------------------------------
 * FUNCTION: mx_dg_segment_table::mx_dg_segment_table
 *
 * DESCRIPTION:
 *
 *
 */

void mx_dg_segment_table::clear()
{
    for (int is = 0; is < nsegments; is++)
        segments[is].clear();
}

/*-------------------------------------------------
 * FUNCTION: mx_dg_quad_geom::mergeSegments
 *
 * DESCRIPTION:
 *
 *
 */

void mx_dg_segment_table::fuseSegments(int& err)
{
    bool mergeAll = TRUE;
    bool doFirstMerge = FALSE;
    bool doLastMerge;
    int firstGeomOffset = -1;
    int lastGeomOffset = -1;

    mx_geom* outGeom = NULL;

    for (int isegment = 0; isegment < nsegments; isegment++) {
        mx_dg_segment* thisSegment = segments + isegment;

        if (!thisSegment->isActive())
            continue;

        if (!thisSegment->isFiltered()) {
            if (firstGeomOffset == -1)
                doFirstMerge = FALSE;
            mergeAll = FALSE;
            outGeom = NULL;
            doLastMerge = FALSE;
        } else {
            if (firstGeomOffset == -1)
                doFirstMerge = TRUE;

            lastGeomOffset = isegment;
            doLastMerge = TRUE;

            if (outGeom == NULL) {
                outGeom = thisSegment->selectGeom;
                thisSegment->id = 0;
            } else {
                thisSegment->id = outGeom->fuse(err,
                    thisSegment->selectGeom,
                    FALSE);
                MX_ERROR_CHECK(err);

                thisSegment->clear();
            }
        }
        if (firstGeomOffset == -1)
            firstGeomOffset = isegment;
    }

    if ((!mergeAll) && doFirstMerge && doLastMerge
        && outGeom->joinEndSegments()) {
        segments[lastGeomOffset].id = outGeom->fuse(err,
            segments[firstGeomOffset].selectGeom,
            TRUE);
        MX_ERROR_CHECK(err);
    }

abort:;
}

/*-------------------------------------------------
 * FUNCTION: mx_dg_segment_table::get_serialised_size
 *
 * DESCRIPTION:
 *
 *
 */

uint32 mx_dg_segment_table::get_serialised_size(int& err)
{
    uint32 size;

    err = MX_ERROR_OK;

    size = SLS_INT32;

    for (int isegment = 0; isegment < nsegments; isegment++) {
        size += segments[isegment].get_serialised_size(err);
        MX_ERROR_CHECK(err);
    }
abort:;
    return size;
}

/*-------------------------------------------------
 * FUNCTION: mx_dg_segment_table::serialise
 *
 * DESCRIPTION:
 *
 *
 */

void mx_dg_segment_table::serialise(int& err,
    uint8** buffer)
{
    err = MX_ERROR_OK;

    serialise_int32(nsegments, buffer);

    for (int isegment = 0; isegment < nsegments; isegment++) {
        segments[isegment].serialise(err, buffer);
        MX_ERROR_CHECK(err);
    }
abort:;
}

/*-------------------------------------------------
 * FUNCTION: mx_dg_segment_table::unserialise
 *
 * DESCRIPTION:
 *
 *
 */

void mx_dg_segment_table::unserialise(int& err,
    uint8** buffer)
{
    mx_dg_segment segment;
    int32 insegments;

    err = MX_ERROR_OK;

    clear();

    unserialise_int32(insegments, buffer);

    for (int isegment = 0; isegment < insegments; isegment++) {
        segment.unserialise(err, buffer);
        MX_ERROR_CHECK(err);

        addSegmentInfo(segment.flags,
            FALSE,
            NULL,
            segment.box,
            segment.id,
            segment.quadKey);
    }
abort:;
}

/*-------------------------------------------------
 * FUNCTION: mx_dg_segment::get_serialised_size
 *
 * DESCRIPTION:
 *
 *
 */

uint32 mx_dg_segment::get_serialised_size(int& err)
{
    err = MX_ERROR_OK;
    return size;
}

/*-------------------------------------------------
 * FUNCTION: mx_dg_segment::serialise
 *
 * DESCRIPTION:
 *
 *
 */

void mx_dg_segment::serialise(int& err,
    uint8** buffer)
{
    err = MX_ERROR_OK;

    quadKey.serialise(err, buffer);
    MX_ERROR_CHECK(err);

    serialise_uint8(flags, buffer);

    box.serialise(err, buffer);
    MX_ERROR_CHECK(err);

abort:;
}

/*-------------------------------------------------
 * FUNCTION: mx_dg_segment::unserialise
 *
 * DESCRIPTION:
 *
 *
 */

void mx_dg_segment::unserialise(int& err,
    uint8** buffer)
{
    err = MX_ERROR_OK;

    quadKey.unserialise(err, buffer);
    MX_ERROR_CHECK(err);

    unserialise_uint8(flags, buffer);

    box.unserialise(err, buffer);
    MX_ERROR_CHECK(err);

abort:;
}

/*-------------------------------------------------
 * FUNCTION: mx_dg_segment_table::addSegmentInfo
 *
 * DESCRIPTION:
 *
 *
 */

void mx_dg_segment_table::addSegmentInfo(uint8 iflags,
    bool deleteableGeom,
    mx_geom* igeom,
    const mx_rect& ibox,
    int iid,
    const mx_dg_quad_key& quadKey)
{
    if (nsegments == nallocsegments) {
        mx_dg_segment* oldSegments = segments;

        nallocsegments += 4;

        segments = new mx_dg_segment[nallocsegments];

        for (int i = 0; i < nsegments; i++) {
            segments[i] = oldSegments[i];
        }

        for (int is = 0; is < nsegments; is++)
            oldSegments[is].selectGeom = NULL;

        delete[] oldSegments;
    }

    set(nsegments, iflags, deleteableGeom,
        igeom, ibox, iid, quadKey);

    nsegments++;
}

/*-------------------------------------------------
 * FUNCTION: mx_dg_quad_geom::mergeSegments
 *
 * DESCRIPTION:
 *
 *
 */

void mx_dg_quad_geom::fuseSegments(int& err)
{
    err = MX_ERROR_OK;

    segmentTable.fuseSegments(err);
    MX_ERROR_CHECK(err);

abort:;
}

/*-------------------------------------------------
 * FUNCTION: mx_dg_quad_geom::extractFromBlob
 *
 * DESCRIPTION:
 *
 *
 */

bool mx_dg_quad_geom::extractFromBlob(int& err,
    mx_diagram_area* da,
    const mx_dg_level_key& ilevelKey,
    const mx_dg_quad_key& iquadKey,
    mx_dg_qgeom_get_t qgeomGetType,
    bool allowInactive,
    const mx_dg_filter* const filter)
{
    bool answer;
    mx_dg_segment_get_t isegmentGetType = segmentGetType;
    bool useSegmentFilter = TRUE;

    err = MX_ERROR_OK;

    init();

    answer = extractFromBlobIntern(err, da, ilevelKey, iquadKey,
        qgeomGetType, allowInactive, filter);
    MX_ERROR_CHECK(err);

    if (answer && isSegmentHeader() && (segmentGetType != MX_DG_SEGMENT_INDIVIDUAL)) {
        mx_dg_quad_geom segmentQuadGeom;

        segmentQuadGeom.setSegmentGetType(MX_DG_SEGMENT_INDIVIDUAL);

        if ((hgeom != NULL) && (hgeom->requireAllSegments())) {
            isegmentGetType = MX_DG_SEGMENT_ALL_WITH_FIRST;
        }

        if (isegmentGetType == MX_DG_SEGMENT_ALL_WITH_FIRST)
            useSegmentFilter = FALSE;

        // loop over the segments

        for (int isegment = 0; isegment < segmentTable.nsegments; isegment++) {
            mx_dg_segment* thisSegment = segmentTable.segments + isegment;

            uint8 segmentFlags = thisSegment->isActive() ? MX_DG_SEGMENT_ACTIVE : 0;
            bool filtered = TRUE;
            mx_geom* segmentGeom = NULL;

            if (segmentFlags) {
                if (isegmentGetType == MX_DG_SEGMENT_MATCHING_WITH_FIRST) {
                    // we can test if the box in the segment intersects with the
                    // box in the filter - this can save going to another quad

                    // get the geometry
                    if (filter != NULL)
                        filtered = filter->testFilterBox(segmentTable.segments[isegment].box);
                }

                if (filtered) {
                    filtered = segmentQuadGeom.extractFromBlobIntern(err, da, ilevelKey,
                        segmentTable.segments[isegment].quadKey,
                        qgeomGetType, allowInactive,
                        useSegmentFilter ? filter : (const mx_dg_filter* const)NULL);
                    MX_ERROR_CHECK(err);

                    segmentGeom = segmentQuadGeom.extractGeom();
                }

                if (filtered)
                    segmentFlags |= MX_DG_SEGMENT_FILTERED;
            }

            thisSegment->flags = segmentFlags;
            thisSegment->selectGeom = segmentGeom;
            thisSegment->deleteableGeom = TRUE;
        }

        // now merge the segments if they are all active

        if (mergeGeometries) {
            fuseSegments(err);
            MX_ERROR_CHECK(err);
        }
    }

abort:;
    return answer;
}

/*-------------------------------------------------
 * FUNCTION: mx_dg_quad_geom::extractFromBlobIntern
 *
 * DESCRIPTION:
 *
 *
 */

bool mx_dg_quad_geom::extractFromBlobIntern(int& err,
    mx_diagram_area* da,
    const mx_dg_level_key& ilevelKey,
    const mx_dg_quad_key& iquadKey,
    mx_dg_qgeom_get_t qgeomGetType,
    bool allowInactive,
    const mx_dg_filter* const filter)
{
    int32 quadGeomSize;
    const uint8* buffer;
    uint8* unserialiseBuffer;
    char* idString = mx_dg_quad::squadId(ilevelKey, iquadKey);
    bool answer = TRUE;
    int32 currentOffset = iquadKey.getOffset();

    // initialise the quad geometry to NULL
    initIntern();

    // get the size of the geometry quad data
    buffer = da->get_blob_readonly(err, idString, currentOffset,
        SLS_INT32);
    MX_ERROR_CHECK(err);

    currentOffset += SLS_INT32;

    unserialise_int32(quadGeomSize, (uint8**)&buffer);
    MX_ERROR_CHECK(err);

    // set the buffer to hold the size
    setQuadGeomBuffer(quadGeomSize);

    // get the data for the quad geometry
    da->get_blob_data(err, idString, currentOffset,
        quadGeomDataSize,
        quadGeomBuffer);
    MX_ERROR_CHECK(err);

    currentOffset += quadGeomDataSize;

    // unserialise the quad geom

    unserialiseBuffer = quadGeomBuffer;
    unserialise(err, &unserialiseBuffer);
    MX_ERROR_CHECK(err);

    levelKey.setLevelId(ilevelKey.getLevelId());
    quadKey = iquadKey;

    if (!allowInactive)
        answer = isActive();

    // is this is a subsequent segment and
    // and sussequent segments are not required

    if (answer && isSegment()) {
        switch (segmentGetType) {
        case MX_DG_SEGMENT_INDIVIDUAL:
            break;
        case MX_DG_SEGMENT_ALL_WITH_FIRST:
        case MX_DG_SEGMENT_MATCHING_WITH_FIRST:
            answer = isFirstSegment();
            break;
        }
    }

    // failed if the quad geom is not active
    if ((!answer) || (qgeomGetType <= getStage()))
        goto gotAnswer;

    // now we need to get the box if we havent got it
    // already

    if (getStage() < MX_DG_QGEOM_GET_GBOX) {
        extractGeomFromBlob(err, da, idString,
            currentOffset,
            MX_DG_QGEOM_GET_GEOM);
        MX_ERROR_CHECK(err);

        setBox(err);
        MX_ERROR_CHECK(err);
    }

    if (qgeomGetType <= MX_DG_QGEOM_GET_GBOX)
        goto gotAnswer;

    // do we need to filter

    if (filter != NULL) {
        answer = filter->filterGeometry(err, *this);
        MX_ERROR_CHECK(err);
    }

    if ((!answer) || (qgeomGetType <= MX_DG_QGEOM_GET_GFILTER))
        goto gotAnswer;

    if (getStage() < MX_DG_QGEOM_GET_GFILTER)
        setStage(MX_DG_QGEOM_GET_GFILTER);

    // if all we want is the quad geom plus box we are done
    if ((!answer) || (qgeomGetType <= getStage()))
        goto gotAnswer;

    // now get the geometry if needed

    extractGeomFromBlob(err, da, idString, currentOffset,
        qgeomGetType);
    MX_ERROR_CHECK(err);

gotAnswer:

    // if we got a geometry we dont want delete it
    if ((getStage() >= MX_DG_QGEOM_GET_GEOM) && ((qgeomGetType < MX_DG_QGEOM_GET_GEOM) || (!answer))) {
        clearGeometry(TRUE);
    }

    return answer;
abort:
    return FALSE;
}

/*-------------------------------------------------
 * FUNCTION: mx_dg_quad::extractGeomFromBlob
 *
 * DESCRIPTION: Add data to a quad segments. This is
 * the high level function which allocates space and
 * leads to quading if required.
 *
 * Return true if the data is added to the quad.
 * Returns false if data is not added due to a new
 * quading required.
 */

void mx_dg_quad_geom::extractGeomFromBlob(int& err,
    mx_diagram_area* da,
    char* idString,
    int32 currentOffset,
    mx_dg_qgeom_get_t qgeomGetType)
{
    err = MX_ERROR_OK;

    if (geomDataSize == 0) {
        createEmptyGeometry();
        setStage(MX_DG_QGEOM_GET_GEOM);
    } else {
        setGeomBuffer(geomDataSize);
        MX_ERROR_CHECK(err);

        da->get_blob_data(err, idString,
            currentOffset,
            geomDataSize,
            geomBuffer);
        MX_ERROR_CHECK(err);

        setStage(MX_DG_QGEOM_GET_GBUFFER);

        if (qgeomGetType == MX_DG_QGEOM_GET_GEOM) {
            createGeometry(err);
            MX_ERROR_CHECK(err);
        }
    }

abort:;
}

/*-------------------------------------------------
 * FUNCTION: mx_dg_quad_geom::clearGeometry
 *
 * DESCRIPTION:
 *
 */

void mx_dg_quad_geom::clearGeometry(bool mainGeomOnly)
{
    if (deleteableGeom)
        delete hgeom;
    hgeom = NULL;

    if (mainGeomOnly)
        return;

    segmentTable.clear();
}

/*-------------------------------------------------
 * FUNCTION: mx_dg_quad_geom::extractGeometry
 *
 * DESCRIPTION:
 *
 */

mx_geom* mx_dg_quad_geom::extractGeom(int isegment)
{
    mx_geom* retGeom;

    if (isegment == -1) {
        retGeom = hgeom;
        hgeom = NULL;
    } else {
        retGeom = segmentTable.segments[isegment].selectGeom;
        segmentTable.segments[isegment].selectGeom = NULL;
    }

    return retGeom;
}
