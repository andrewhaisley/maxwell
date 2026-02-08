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
 * MODULE : mx_dg_iter.C
 *
 * AUTHOR : David Miller
 *
 * This file
 *
 * DESCRIPTION:
 * Module mx_dg_iter.C
 *
 *
 */

#include "mx_dg_area.h"

mx_dg_iterator::mx_dg_iterator()
{
    useFilter = FALSE;
    setGroupInformation(FALSE);
    quadInformation = FALSE;
    singleLevelIteration = FALSE;
    singleQuadIteration = FALSE;
    da = NULL;
    currentLevel = NULL;
    currentQuad = NULL;
    useAreaOfInterest = FALSE;
    qgeomGetType = MX_DG_QGEOM_GET_GEOM;
    advanceItem = TRUE;
    allowInactive = FALSE;
}

void mx_dg_iterator::start(int& err,
    mx_diagram_area* ida,
    const mx_dg_level_key* const ilevelKey,
    const mx_dg_quad_key* const iquadKey)
{
    err = MX_ERROR_OK;

    da = ida;
    advanceItem = TRUE;

    singleLevelIteration = (ilevelKey != NULL);

    if (singleLevelIteration) {
        currentLevel = da->getLevel(*ilevelKey);

        singleQuadIteration = (iquadKey != NULL);

        if (singleQuadIteration) {
            currentQuad = da->getQuad(*ilevelKey,
                *iquadKey);
        } else {
            currentQuad = currentLevel->getTopQuad();
        }
    } else {
        currentLevel = da->getFirstLevel();
        currentQuad = currentLevel->getTopQuad();
    }

    currentQuadKey = currentQuad->getHwm();
    currentQuadKey.setOffset(0);
}

bool mx_dg_iterator::nextGeometry(int& err,
    bool& skippedSome)
{
    bool reachedEnd;

    skippedSome = FALSE;
    reachedEnd = FALSE;

    bool retVal = nextGeometryIntern(err, FALSE,
        reachedEnd, skippedSome);
    MX_ERROR_CHECK(err);

    // finished iteration

    if (reachedEnd) {
        end(err);
        MX_ERROR_CHECK(err);
    }

    return retVal;
abort:
    return TRUE;
}

void mx_dg_iterator::process(int& err)
{
    bool reachedEnd = FALSE;
    bool skippedSome = FALSE;

    nextGeometryIntern(err, TRUE,
        reachedEnd, skippedSome);
    MX_ERROR_CHECK(err);

    if (reachedEnd) {
        end(err);
        MX_ERROR_CHECK(err);
    }
abort:;
}

inline void mx_dg_iterator::advanceKey(int& err)
{
    err = MX_ERROR_OK;
    currentQuadKey += quadGeom.writeSize();
}

bool mx_dg_iterator::advance(int& err,
    bool doingProcess,
    bool found,
    bool& skippedSome)
{
    // advance is an in/out variable for the process function
    bool done;
    bool doAdvance, returnNow;

    if (found) {
        if (doingProcess) {
            processFunction(err,
                quadGeom,
                advanceItem,
                done,
                skippedSome);
            MX_ERROR_CHECK(err);

            skippedSome = FALSE;

            quadGeom.clearGeometry();

            doAdvance = advanceItem;
            returnNow = done;
        } else {
            advanceItem = TRUE;
            doAdvance = TRUE;
            returnNow = TRUE;
        }
    } else {
        skippedSome = TRUE;
        advanceItem = TRUE;
        doAdvance = TRUE;
        returnNow = FALSE;
    }

    if (doAdvance) {
        advanceKey(err);
        MX_ERROR_CHECK(err);
    }

    return returnNow;
abort:
    return TRUE;
}

bool mx_dg_iterator::nextGeometryIntern(int& err,
    bool doingProcess,
    bool& reachedEnd,
    bool& skippedSome)
{
    bool found;
    bool returnFound;
    mx_dg_filter* ifilter = useFilter ? &filter : ((mx_dg_filter*)NULL);
    err = MX_ERROR_OK;

    // loop over levels
    while (currentLevel != NULL) {
        // loop over quads
        while (currentQuad != NULL) {
            // loop over geometries

            while ((currentQuadKey.getOffset() < currentQuad->getHwm().getOffset())) {
                found = quadGeom.extractFromBlob(err,
                    da,
                    currentLevel->getHwm(),
                    currentQuadKey,
                    qgeomGetType,
                    allowInactive,
                    ifilter);
                MX_ERROR_CHECK(err);

                returnFound = advance(err, doingProcess,
                    found, skippedSome);
                MX_ERROR_CHECK(err);

                if (returnFound)
                    return TRUE;
            }

            if (singleQuadIteration) {
                currentQuad = NULL;
                break;
            }

            currentQuad = currentQuad->nextQuad();

            if (currentQuad == NULL)
                break;

            currentQuadKey = currentQuad->getHwm();
            currentQuadKey.setOffset(0);
        }

        if (singleLevelIteration) {
            currentLevel = NULL;
            break;
        }

        currentLevel = da->getNextLevel(currentLevel->getHwm());
        if (currentLevel == NULL)
            break;

        currentQuadKey = currentLevel->getTopQuad()->getHwm();
        currentQuadKey.setOffset(0);
    }

    reachedEnd = TRUE;

abort:
    return FALSE;
}
