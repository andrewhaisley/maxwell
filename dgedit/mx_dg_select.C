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
 * MODULE : mx_dg_select.C
 *
 * AUTHOR : David Miller
 *
 * This file
 *
 * DESCRIPTION:
 * Module mx_dg_select.C
 *
 *
 *
 */

#include "mx_dg_state.h"

/*-------------------------------------------------
 * FUNCTION: mx_dg_select_state::mx_dg_select_state
 *
 * DESCRIPTION:
 *
 *
 */

mx_dg_select_state::mx_dg_select_state(mx_dg_editor* editor)
    : mx_dg_state(editor)
    , iterator(editor)
{
    type = MX_DGT_SELECT;
}

/*-------------------------------------------------
 * FUNCTION: mx_dg_select_state::buttonRelease
 *
 * DESCRIPTION:
 *
 *
 */

void mx_dg_select_state::buttonRelease(int& err,
    mx_button_event& event)
{
    ibuttonRelease(err, event);
}

/*-------------------------------------------------
 * FUNCTION: mx_dg_select_state::buttonMotion
 *
 * DESCRIPTION:
 *
 *
 */

void mx_dg_select_state::buttonMotion(int& err,
    mx_button_event& event)
{
    ibuttonMotion(err, event);
}

/*-------------------------------------------------
 * FUNCTION: mx_dg_select_state::buttonMotionStart
 *
 * DESCRIPTION:
 *
 *
 */

void mx_dg_select_state::buttonMotionStart(int& err,
    mx_button_event& event)
{
    ibuttonMotionStart(err, event);
}

/*-------------------------------------------------
 * FUNCTION: mx_dg_select_state::buttonMotionEnd
 *
 * DESCRIPTION:
 *
 *
 */

void mx_dg_select_state::buttonMotionEnd(int& err,
    mx_button_event& event)
{
    ibuttonMotionEnd(err, event);
}

/*-------------------------------------------------
 * FUNCTION: mx_dg_select_state::processRelease
 *
 * DESCRIPTION:
 *
 *
 */

void mx_dg_select_state::processRelease(int& err,
    mx_event* event,
    gmx_point* pf)
{
    double snapDistance = editor->getSnapDistance();
    mx_rect box(pf->p.x - snapDistance,
        pf->p.y - snapDistance,
        pf->p.x + snapDistance,
        pf->p.y + snapDistance);
    mx_button_event* bev = (mx_button_event*)event;

    err = MX_ERROR_OK;

    iterator.setAdditiveMode(bev->getShift());
    iterator.setTolerance(snapDistance);
    iterator.setPoint(pf->p);
    iterator.setAreaOfInterest(box);
    iterator.setSnapping(TRUE);
    iterator.resetSnapInfo();
    iterator.setGroupInformation(TRUE);

    iterator.start(err, editor->getArea());
    MX_ERROR_CHECK(err);

    iterator.process(err);
    MX_ERROR_CHECK(err);
abort:;
}

/*-------------------------------------------------
 * FUNCTION: mx_dg_select_state::processMotionStart
 *
 * DESCRIPTION:
 *
 *
 */

void mx_dg_select_state::processMotionStart(int& err,
    mx_event* event,
    gmx_point* pf)
{
    err = MX_ERROR_OK;

    // loop through the selected geometries to see if
    // we have hit something
    editor->testSelectedHit(err, pf->p);
    MX_ERROR_CHECK(err);

abort:;
}

/*-------------------------------------------------
 * FUNCTION: mx_dg_select_state::processMotion
 *
 * DESCRIPTION:
 *
 *
 */

void mx_dg_select_state::processMotion(int& err,
    mx_event* event,
    gmx_point* pf)
{
    err = MX_ERROR_OK;
}

/*-------------------------------------------------
 * FUNCTION: mx_dg_select_state::processMotionEnd
 *
 * DESCRIPTION:
 *
 *
 */

void mx_dg_select_state::processMotionEnd(int& err,
    mx_event* event,
    gmx_point* pf)
{
    err = MX_ERROR_OK;
}

/*-------------------------------------------------
 * FUNCTION: mx_dg_select_state::processPoint
 *
 * DESCRIPTION:
 *
 *
 */

void mx_dg_select_state::processPoint(int& err,
    mx_event* event,
    gmx_point* pf)
{
    err = MX_ERROR_OK;

    switch (event->getType()) {
    case MX_EVENT_BUTTON_RELEASE:
        processRelease(err, event, pf);
        MX_ERROR_CHECK(err);
        break;
    case MX_EVENT_BUTTON_MOTION_START:
        processMotionStart(err, event, pf);
        MX_ERROR_CHECK(err);
        break;
    case MX_EVENT_BUTTON_MOTION:
        processMotion(err, event, pf);
        MX_ERROR_CHECK(err);
        break;
    case MX_EVENT_BUTTON_MOTION_END:
        processMotionEnd(err, event, pf);
        MX_ERROR_CHECK(err);
        break;
    }

abort:;
}

/*-------------------------------------------------
 * FUNCTION: mx_dg_select_state::tidyState
 *
 * DESCRIPTION:
 *
 *
 */

void mx_dg_select_state::tidyState(int& err)
{
    err = MX_ERROR_OK;
}

/*-------------------------------------------------
 * FUNCTION: mx_dg_select_state::tidyState
 *
 * DESCRIPTION:
 *
 *
 */

void mx_dg_select_state::keyPress(int& err,
    mx_key_event& event)
{
    err = MX_ERROR_OK;

    if ((strcmp(event.getString(), "n") == 0) || (strcmp(event.getString(), "N") == 0)) {
        // go on to the next snap point

        iterator.setAdditiveMode(event.getShift());

        iterator.process(err);
        MX_ERROR_CHECK(err);
    }

abort:;
}

/*-------------------------------------------------
 * FUNCTION: mx_dg_select_iterator::mx_dg_select_iterator
 *
 * DESCRIPTION:
 *
 *
 */

mx_dg_select_iterator::mx_dg_select_iterator(mx_dg_editor* ieditor)
{
    editor = ieditor;
    wholeGeometryMode = TRUE;
    additiveMode = FALSE;
}

/*-------------------------------------------------
 * FUNCTION: mx_dg_select_iterator::processFunctionWGM
 *
 * DESCRIPTION:
 *
 *
 */

void mx_dg_select_iterator::processFunctionWGM(int& err,
    mx_dg_quad_geom& quadGeom)
{
    mx_dg_selected* previousSelection;
    bool doAdd;

    err = MX_ERROR_OK;

    previousSelection = editor->getSelected(err,
        quadGeom.getLevelKey());
    MX_ERROR_CHECK(err);

    if (previousSelection == NULL) {
        doAdd = TRUE;
    } else {
        doAdd = (!additiveMode);
    }

    if (!additiveMode) {
        // delete everything from the selected table
        editor->delSelected(err, MX_DG_ALL_SELECTED_KEY);
        MX_ERROR_CHECK(err);
    }

    if (quadGeom.getGroupKey().isNull()) {
        if (doAdd) {
            mx_geom* newGeom;

            if (quadGeom.isFirstSegment()) {
                newGeom = quadGeom.extractGeom(0);
            } else {
                newGeom = quadGeom.extractGeom();
            }

            newGeom->selectCount = mx_dg_selected::all_selection;

            lastSelectedId = editor->addSelected(err,
                quadGeom.getLevelKey(),
                newGeom);
            MX_ERROR_CHECK(err);
        }
    } else {
        // it's in a group - add the whole group
        mx_dg_group_iterator iterator;
        mx_dg_group_element element;
        mx_geom* newGeom;

        iterator.start(err,
            editor->getArea(),
            quadGeom.getGroupKey());

        iterator.setDoExtract(doAdd);

        while (iterator.next(err)) {
            element = iterator.getGroupElement();

            if (doAdd) {
                newGeom = element.extractGeom();

                newGeom->selectCount = mx_dg_selected::all_selection;

                editor->addSelected(err,
                    element.getLevelKey(),
                    newGeom);
                MX_ERROR_CHECK(err);
            } else {
                editor->delSelected(err, element.getLevelKey());
                MX_ERROR_CHECK(err);
            }
        }
        MX_ERROR_CHECK(err);
    }
abort:;
}

/*-------------------------------------------------
 * FUNCTION: mx_dg_select_iterator::processFunction
 *
 * DESCRIPTION:
 *
 *
 */

void mx_dg_select_iterator::processFunction(int& err,
    mx_dg_quad_geom& quadGeom,
    bool& advance,
    bool& done,
    bool skippedSome)
{
    mx_geom* geom = quadGeom.getGeom();
    mx_dg_selected* previousSelection;
    bool allGone = FALSE;
    bool hadHit = FALSE;

    err = MX_ERROR_OK;

    // try snapping to the geometry - we know that
    // its been box tested by now

    if (quadGeom.isFirstSegment()) {
        // its the first segment
        geom = quadGeom.getGeom(0);
    } else {
        geom = quadGeom.getGeom();
    }

    geom->dist2snap(testPoint, snapInfo);

    if (snapInfo.getCount() != MX_GEOM_SNAP_NONE) {
        hadHit = TRUE;

        if (wholeGeometryMode) {
            processFunctionWGM(err, quadGeom);
            MX_ERROR_CHECK(err);

            done = TRUE;
        } else {
            // we've found something
            if (advance) {
                // its brand new

                // is it already selected

                previousSelection = editor->getSelected(err,
                    quadGeom.getLevelKey());
                MX_ERROR_CHECK(err);

                if (previousSelection == NULL) {
                    mx_geom* newGeom;

                    if (!additiveMode) {
                        // delete everything from the selected table
                        editor->delSelected(err, MX_DG_ALL_SELECTED_KEY);
                        MX_ERROR_CHECK(err);
                    }

                    if (quadGeom.isFirstSegment()) {
                        newGeom = quadGeom.extractGeom(0);
                    } else {
                        newGeom = quadGeom.extractGeom();
                    }

                    // its totally new but we are not in whole geometry mode

                    newGeom->selectCount = snapInfo.getCount();

                    lastSelectedId = editor->addSelected(err,
                        quadGeom.getLevelKey(),
                        newGeom);
                    MX_ERROR_CHECK(err);
                } else {
                    // its already in the selection -
                    // either remove or add multiple selection points

                    if (additiveMode) {
                        int selectCount = snapInfo.getCount();

                        // undraw the present selection
                        editor->drawSelected(err, previousSelection->getSelectedId());
                        MX_ERROR_CHECK(err);

                        previousSelection->addSelectCount(geom,
                            selectCount,
                            allGone);

                        if (allGone) {
                            // it's no longer selected

                            editor->delSelected(err, previousSelection->getSelectedId());
                            MX_ERROR_CHECK(err);
                        } else {
                            // redraw the present selection
                            editor->drawSelected(err, previousSelection->getSelectedId());
                            MX_ERROR_CHECK(err);
                        }
                    } else {
                        // we want to delete all other but fot this one
                        editor->delSelected(err,
                            MX_DG_ALL_SELECTED_KEY,
                            previousSelection->getSelectedId());
                        MX_ERROR_CHECK(err);
                    }
                }
            } else {
                // we've already got it from the previous select
                // just edit the existing geometry

                geom = editor->getSelectedGeom(err, lastSelectedId);
                MX_ERROR_CHECK(err);

                // undraw the present selection
                editor->drawSelected(err, lastSelectedId);
                MX_ERROR_CHECK(err);

                geom->selectCount = snapInfo.getCount();

                // draw the new selection
                editor->drawSelected(err, lastSelectedId);
                MX_ERROR_CHECK(err);
            }
        }
    }

    // go on to next geometry
    if (wholeGeometryMode) {
        advance = TRUE;
        done = hadHit;
    } else {
        advance = (snapInfo.getCount() == MX_GEOM_SNAP_NONE);
        done = !advance;
    }

    if (advance)
        resetSnapInfo();

abort:;
}

mx_dg_selected::mx_dg_selected(mx_geom* igeom,
    const mx_dg_level_key& levelKey)
{
    geom = igeom;
    selectedId = levelKey.compress();

    addSelections = TRUE;
    nselectCounts = 0;
    selectCounts = NULL;
    isActive = TRUE;

    storedStyleId = igeom->styleId;
    storedColourId = igeom->colourId;
}

mx_dg_selected::~mx_dg_selected()
{
    delete geom;
    delete[] selectCounts;
}

void mx_dg_selected::addSelectCount(mx_geom* geom,
    int selectCount,
    bool& allGone)
{
    int ic;
    int newSize;
    int useSlot = -1;
    int* oldSelectCounts;
    bool totalSelect;

    if (selectCount == all_selection) {
        for (ic = 0; ic < nselectCounts; ic++) {
            selectCounts[ic] = null_selection;
        }

        addSelections = !addSelections;

        allGone = addSelections;
    } else {
        // find or cancel a slot for the selection

        for (ic = 0; ic < nselectCounts; ic++) {
            if (selectCounts[ic] == null_selection) {
                useSlot = ic;
                break;
            } else if (selectCounts[ic] == selectCount) {
                selectCounts[ic] = null_selection;
                useSlot = -2;
                break;
            }
        }

        if (useSlot == -1) {
            // we need more memory
            oldSelectCounts = selectCounts;
            newSize = nselectCounts + 8;

            selectCounts = new int[newSize];

            memcpy(selectCounts, oldSelectCounts, sizeof(int) * nselectCounts);

            delete[] oldSelectCounts;

            selectCounts[nselectCounts] = selectCount;

            for (ic = nselectCounts + 1; ic < newSize; ic++) {
                selectCounts[ic] = null_selection;
            }

            nselectCounts = newSize;
        } else if (useSlot != -2) {
            selectCounts[useSlot] = selectCount;
        }

        totalSelect = geom->mergeSelectCount(nselectCounts,
            selectCounts);

        if (!addSelections) {
            allGone = totalSelect;
        } else {
            allGone = (selectCounts[0] == null_selection);
        }
    }
}

void mx_dg_selected::draw(int& err,
    mx_rect* regionOfInterest,
    mx_style_device* device)
{
    if ((regionOfInterest != NULL) && (!geom->box(err).intersects(*regionOfInterest)))
        return;
    MX_ERROR_CHECK(err);

    if (((nselectCounts == 0) || (selectCounts[0] == null_selection)) && (!addSelections)) {
        // all selected

        geom->selectCount = all_selection;

        device->draw(err, geom);
        MX_ERROR_CHECK(err);
    } else {
        for (int is = 0; is < nselectCounts; is++) {
            int thisCount = selectCounts[is];
            if (thisCount == null_selection)
                break;

            geom->selectCount = selectCounts[is];

            device->draw(err, geom);
            MX_ERROR_CHECK(err);
        }
    }

    geom->selectCount = MX_GEOM_SNAP_NONE;

abort:;

    return;
}
