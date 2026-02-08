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
 * MODULE/CLASS : mx_dg_editor
 *
 * AUTHOR : David Miller
 *
 *
 *
 * DESCRIPTION:
 *
 */

#include <mx_alay.h>
#include <mx_dg_editor.h>
#include <mx_dg_menubar.h>
#include <mx_dg_paint.h>
#include <mx_dg_state.h>
#include <mx_dg_toolbar.h>
#include <mx_sc_device.h>
#include <mx_stsbar.h>

class mx_dg_point_state;

mx_dg_editor::mx_dg_editor(int& err, mx_app& a, mx_diagram_area* dgarea)
    : mx_editor(a, nullptr)
    , layout(nullptr)
    , menu(nullptr)
    , toolbar(nullptr)
    , statusbar(nullptr)
    , frame(nullptr)
    , undoInfo(this, 20)
{
    mx_text_event te(MX_DG_SPAN);

    err = MX_ERROR_OK;

    area = dgarea;

    (void)set_sheet_layout(err, dgarea);
    MX_ERROR_CHECK(err);

    setSheetLayout(layout);

    menu = new mx_dg_menubar(window);
    window->add_ui_object(menu);

    toolbar = new mx_dg_toolbar(window);

    window->add_ui_object(toolbar);

    window->set_title("diagram");

    create_frame(err);
    MX_ERROR_CHECK(err);

    window->add_ui_object(frame);

    statusbar = new mx_statusbar(window);
    window->add_ui_object(statusbar);

    statusbar->update(MX_MAXWELL_VERSION);

    window->initial_focus((mx_screen_device*)(frame->getDevice()));

    window->activate(err);
    MX_ERROR_CHECK(err);

    ((mx_screen_device*)frame->getDevice())->addProcessMask(MX_EVENT_PREDRAW | MX_EVENT_POSTDRAW);
    ((mx_screen_device*)frame->getDevice())->take_focus();

    topState = new mx_dg_top_state(this);

    currentState = topState;
    currentLevel = 0;

    // pass on events to the top frame
    topState->setToReceiveEvents(err, this, nullptr);
    MX_ERROR_CHECK(err);

    // set the editor to add spans be default
    textEntry(err, te);
    MX_ERROR_CHECK(err);

    lastHashKey = 1;

    addStandardStyles(err);
    MX_ERROR_CHECK(err);

    lastGeometry = nullptr;
    snapDistance = 5.0;

    return;

abort:;
}

void mx_dg_editor::addStandardStyles(int& err)
{
    mx_gline_style* lineStyle;
    mx_garea_style* areaStyle;
    mx_gfill_style* fillStyle;
    mx_gborder_style* borderStyle;
    int32 fillStyleId;
    int32 borderStyleId;

    mx_all_styles* styles = area->getAllStyles();

    // add black as the current colour
    mx_colour* colour = new mx_colour;

    currentColourId = styles->insertColour(err, colour);
    MX_ERROR_CHECK(err);

    lineStyle = new mx_gline_style("Solid");
    lineStyle->style.width = 1;

    currentLineStyleId = styles->insert(err, lineStyle);
    MX_ERROR_CHECK(err);

    fillStyle = new mx_gfill_style("SolidFill");

    fillStyleId = styles->insert(err, fillStyle);
    MX_ERROR_CHECK(err);

    borderStyle = new mx_gborder_style("SolidFill");
    borderStyle->style.fill_style.fill_id = fillStyleId;

    borderStyleId = styles->insert(err, borderStyle);
    MX_ERROR_CHECK(err);

    areaStyle = new mx_garea_style("SolidFill");
    currentAreaStyleId = styles->insert(err, areaStyle);
    MX_ERROR_CHECK(err);

    areaStyle->style.border_style.border_id = borderStyleId;

abort:;
}

mx_dg_editor::~mx_dg_editor()
{
    int err;

    delete topState;
    delete lastGeometry;

    delSelected(err);
}

bool mx_dg_editor::testSelectedHit(int& err, mx_point& testPoint)
{
    mx_snap_info snapInfo;

    // loop over all selected items looking for a hit
    for (auto i : m_selected) {
        i.second->geom->dist2snap(testPoint, snapInfo);

        if (snapInfo.getCount() != MX_GEOM_SNAP_NONE) {
            return true;
        }
    }

    return false;
}

uint32 mx_dg_editor::addSelected(int& err, const mx_dg_level_key& levelKey, mx_geom* geom)
{
    bool allGone;
    uint32 selectedId;

    err = MX_ERROR_OK;

    mx_dg_selected* selection = new mx_dg_selected(geom, levelKey);

    selectedId = selection->getSelectedId();

    if (m_selected.find(selectedId) != m_selected.end()) {
        // delete the existing entry
        delSelected(err, selectedId);
        MX_ERROR_CHECK(err);
    }

    m_selected[selectedId] = selection;

    selection->addSelectCount(geom, geom->selectCount, allGone);

    // draw the highlighted geometry
    drawSelected(err, selectedId);
    MX_ERROR_CHECK(err);

    return selectedId;

abort:;
    return 0;
}

mx_dg_selected* mx_dg_editor::getSelected(int& err, const mx_dg_level_key& levelKey)
{
    uint32 selectedId;

    err = MX_ERROR_OK;

    selectedId = levelKey.compress();

    mx_dg_selected* retValue = getSelected(err, selectedId);
    MX_ERROR_CHECK(err);

abort:;
    return retValue;
}

mx_dg_selected* mx_dg_editor::getSelected(int& err, uint32 hashKey)
{
    if (m_selected.find(hashKey) == m_selected.end()) {
        return nullptr;
    } else {
        return m_selected[hashKey];
    }
}

mx_geom* mx_dg_editor::getSelectedGeom(int& err, uint32 hashKey)
{
    if (m_selected.find(hashKey) == m_selected.end()) {
        return nullptr;
    } else {
        return m_selected[hashKey]->getGeom();
    }
}

bool mx_dg_editor::delSelected(int& err, const mx_dg_level_key& levelKey)
{
    uint32 selectedId;
    bool ans = false;

    err = MX_ERROR_OK;

    selectedId = levelKey.compress();

    ans = delSelected(err, selectedId);
    MX_ERROR_CHECK(err);
abort:;
    return ans;
}

bool mx_dg_editor::delSelected(int& err, uint32 hashKey, uint32 saveFromDelete)
{
    bool foundSelected = false;

    if (hashKey == MX_DG_ALL_SELECTED_KEY) {

        for (auto i : m_selected) {
            if (i.second->getSelectedId() != saveFromDelete) {
                delSelected(err, i.second->getSelectedId());
                MX_ERROR_CHECK(err);
            } else {
                foundSelected = true;
            }
        }

        if (!foundSelected) {
            m_selected.clear();
        }

    } else {
        if (m_selected.find(hashKey) == m_selected.end()) {
            err = MX_ERROR_OK;
            return false;
        }

        // undraw the selected object
        drawSelected(err, m_selected[hashKey]->getSelectedId());
        MX_ERROR_CHECK(err);

        // delete this data (and the geometry)
        delete m_selected[hashKey];

        m_selected.erase(hashKey);
        MX_ERROR_CHECK(err);
    }

abort:
    return true;
}

void mx_dg_editor::drawSelected(int& err, uint32 hashKey, mx_rect* regionOfInterest)
{
    mx_dg_selected* selection;
    mx_style_device* styleDevice = area->getStyleDevice();

    err = MX_ERROR_OK;

    // we want to use the default colours

    styleDevice->setUseDefaultStyles(true);
    styleDevice->setUseDefaultColours(true);
    styleDevice->getDevice()->setXor(err, true);
    MX_ERROR_CHECK(err);

    if (hashKey == MX_DG_ALL_SELECTED_KEY) {
        // draw everything
        for (auto i : m_selected) {
            selection = i.second;

            if (!selection->isActive)
                continue;

            drawSelectedGeom(err, selection, regionOfInterest);
            MX_ERROR_CHECK(err);
        }
    } else {
        if (m_selected.find(hashKey) != m_selected.end()) {
            selection = m_selected[hashKey];

            drawSelectedGeom(err, selection, regionOfInterest);
            MX_ERROR_CHECK(err);
        }
    }

    styleDevice->getDevice()->setXor(err, false);
    MX_ERROR_CHECK(err);

    ((mx_screen_device*)styleDevice->getDevice())->pixmapCommit();
    MX_ERROR_CHECK(err);

    styleDevice->setUseDefaultStyles(false);
    styleDevice->setUseDefaultColours(false);

    return;
abort:
    styleDevice->setUseDefaultStyles(false);
    styleDevice->setUseDefaultColours(false);
    {
        int errt;
        styleDevice->getDevice()->setXor(errt, false);
    }
}

void mx_dg_editor::drawSelectedGeom(int& err,
    mx_dg_selected* selection,
    mx_rect* regionOfInterest)
{
    mx_style_device* styleDevice = area->getStyleDevice();

    err = MX_ERROR_OK;

    selection->draw(err, regionOfInterest, styleDevice);
    MX_ERROR_CHECK(err);
abort:;
}

uint32 mx_dg_editor::addHighlighted(int& err, mx_geom* geom)
{
    mx_style_defaults* defStyles = area->getStyleDevice()->getDefaults();

    err = MX_ERROR_OK;

    geom->colourId = defStyles->blackColourId;
    geom->styleId = defStyles->XORLineStyleId;

    m_tempHighlighted[lastHashKey] = geom;
    MX_ERROR_CHECK(err);

    lastHashKey++;

    return (lastHashKey - 1);
abort:;
    return 0;
}

void mx_dg_editor::drawHighlighted(int& err, uint32 hashKey)
{
    if (hashKey == MX_DG_NO_HIGHLIGHT_KEY)
        return;

    mx_style_device* styleDevice = area->getStyleDevice();

    err = MX_ERROR_OK;

    // we want to use the default colours
    styleDevice->setUseDefaultStyles(true);
    styleDevice->setUseDefaultColours(true);
    styleDevice->getDevice()->setXor(err, true);
    styleDevice->getDevice()->setDrawToWindowOnly(true);

    if (hashKey == MX_DG_ALL_HIGHLIGHT_KEY) {
        // draw everything
        for (auto i : m_tempHighlighted) {
            styleDevice->draw(err, i.second);
            MX_ERROR_CHECK(err);
        }
    } else {
        if (m_tempHighlighted.find(hashKey) != m_tempHighlighted.end()) {
            styleDevice->draw(err, m_tempHighlighted[hashKey]);
            MX_ERROR_CHECK(err);
        }
    }

abort:;
    styleDevice->setUseDefaultStyles(false);
    styleDevice->setUseDefaultColours(false);
    {
        int errt;
        styleDevice->getDevice()->setXor(errt, false);
        ((mx_screen_device*)styleDevice->getDevice())->setDrawToWindowOnly(false);
    }
    return;
}

mx_geom* mx_dg_editor::getHighlighted(int& err, uint32 hashKey)
{
    err = MX_ERROR_OK;

    if (hashKey == MX_DG_NO_HIGHLIGHT_KEY) {
        return nullptr;
    } else {
        if (m_tempHighlighted.find(hashKey) == m_tempHighlighted.end()) {
            return nullptr;
        } else {
            return m_tempHighlighted[hashKey];
        }
    }
}

void mx_dg_editor::delHighlighted(int& err, uint32 hashKey)
{
    if (hashKey == MX_DG_ALL_HIGHLIGHT_KEY) {
        m_tempHighlighted.clear();
        lastHashKey = 1;
    } else {
        if (hashKey == MX_DG_NO_HIGHLIGHT_KEY)
            return;

        m_tempHighlighted.erase(hashKey);
        MX_ERROR_CHECK(err);

        // dont let index get too big
        if ((lastHashKey - 1) == hashKey)
            lastHashKey = hashKey;
    }
abort:;
}

mx_area_layout& mx_dg_editor::set_sheet_layout(int& err,
    mx_diagram_area* dgarea)
{
    mx_point size;
    mx_sheet_size sheet;

    size.x = dgarea->get_width(err);
    MX_ERROR_CHECK(err);

    size.y = dgarea->get_height(err);
    MX_ERROR_CHECK(err);

    sheet.setSize(size);

    if (layout == nullptr) {
        layout = new mx_area_layout;

        layout->addSheets(err, 0, 1, &sheet);
        MX_ERROR_CHECK(err);
    } else {
        layout->changeSheets(err, 0, 1, &sheet);
        MX_ERROR_CHECK(err);
    }

    layout->setSnapTolerance(0.00001);

    return *layout;
abort:
    return *layout;
}

void mx_dg_editor::create_frame(int& err)
{
    mx_doc_coord_t top_left(0, 0);
    mx_point size(210, 192);
    mx_point increment(5, 5);

    frame = new mx_scrollable_frame(
        err,
        top_left,
        size,
        1,
        *this,
        window->get_form(),
        true,
        increment,
        false);
    MX_ERROR_CHECK(err);

    frame->setApplyScrollLimitToTarget(true);
    frame->setMouseScrolling(true);
    frame->addProcessMask(MX_EVENT_KEY_PRESS | MX_EVENT_PREDRAW | MX_EVENT_POSTDRAW | MX_EVENT_MOUSE_MOTION);

    setup_frame_target_params();
abort:;
}

void mx_dg_editor::setup_frame_target_params()
{
    addProcessMask(MX_EVENT_ALL_SCROLL | MX_EVENT_KEY_PRESS | MX_EVENT_PREDRAW | MX_EVENT_POSTDRAW | MX_EVENT_BUTTON_PRESSES | MX_EVENT_MOUSE_MOTION);
}

void mx_dg_editor::preScroll(int& err, mx_scroll_event& event)
{
    err = MX_ERROR_OK;

    drawHighlighted(err);
    MX_ERROR_CHECK(err);
abort:;
}

void mx_dg_editor::postScroll(int& err, mx_scroll_event& event)
{
    err = MX_ERROR_OK;

    drawHighlighted(err);
    MX_ERROR_CHECK(err);
abort:;
}

void mx_dg_editor::draw(int& err, mx_draw_event& event)
{
    mx_device* dev;
    mx_doc_rect_t r(event.getTL(), event.getBR());

    dev = event.getDevice();

    mx_painter::set_device(*dev);

    if (dev->is_a(mx_screen_device_class_e)) {
        ((mx_screen_device*)dev)->setDrawToWindowOnly(false);
    }

    mx_diagram_area_painter::painter()->draw(err, r, *area);
    MX_ERROR_CHECK(err);

    // draw any selected items

    drawSelected(err);
    MX_ERROR_CHECK(err);

    // draw any grid if needed
    grid.draw(err, r.r, getDevice());
    MX_ERROR_CHECK(err);

abort:;
}

/*-------------------------------------------------
 * FUNCTION: mx_dg_editor::pushState
 *
 * DESCRIPTION:
 *
 *
 */

mx_dg_state* mx_dg_editor::pushState(int& err,
    MX_DGT_STATE newStateType)
{
    mx_dg_state* newState;

    switch (newStateType) {
    case MX_DGT_TOP:
        newState = new mx_dg_top_state(this);
        break;
    case MX_DGT_SPAN:
        newState = new mx_dg_span_state(this);
        break;
    case MX_DGT_ARC:
        newState = new mx_dg_arc_state(this);
        break;
    case MX_DGT_FULL_CIRCLE:
        newState = new mx_dg_fcircle_state(this);
        break;
    case MX_DGT_FULL_ELLIPSE:
        newState = new mx_dg_fellipse_state(this);
        break;
    case MX_DGT_PARTIAL_CIRCLE:
        newState = new mx_dg_pcircle_state(this);
        break;
    case MX_DGT_PARTIAL_ELLIPSE:
        newState = new mx_dg_pellipse_state(this);
        break;
    case MX_DGT_POINT:
        newState = new mx_dg_point_state(this);
        break;
    case MX_DGT_POLYPOINT:
        newState = new mx_dg_polypoint_state(this);
        break;
    case MX_DGT_RECT:
        newState = new mx_dg_rect_state(this);
        break;
    case MX_DGT_SNAP:
        newState = new mx_dg_snap_state(this);
        break;
    case MX_DGT_SELECT:
        newState = new mx_dg_select_state(this);
        break;
    default:
        return nullptr;
    }

    pushState(err, newState);
    MX_ERROR_CHECK(err);

    return newState;
abort:
    return nullptr;
}

void mx_dg_editor::pushState(int& err, mx_dg_state* newState)
{
    err = MX_ERROR_OK;

    if (newState == nullptr)
        return;

    currentState->setNextState(newState);

    setToNotPropagateEvents(currentState);

    // pass on events to the top frame
    newState->setToReceiveEvents(err, this, nullptr);
    MX_ERROR_CHECK(err);

    currentState = newState;

abort:;
}

mx_dg_state* mx_dg_editor::popState(int& err)
{
    mx_dg_state* retState = currentState;

    err = MX_ERROR_OK;

    setToNotPropagateEvents(currentState);

    currentState = currentState->getPreviousState();

    // pass on events to the top frame
    if (currentState != nullptr) {
        currentState->setNextState(nullptr);

        currentState->setToReceiveEvents(err, this, nullptr);
        MX_ERROR_CHECK(err);
    }

    return retState;

abort:;
    return nullptr;
}

void mx_dg_editor::toggleGrid(int& err)
{
    err = MX_ERROR_OK;

    grid.toggleActive();

    frame->refresh(err);
    MX_ERROR_CHECK(err);
abort:;
}

mx_device* mx_dg_editor::getDevice()
{
    return (frame == nullptr) ? ((mx_device*)nullptr) : frame->getDevice();
}

void mx_dg_editor::addGeometry(int& err,
    mx_geom& geom)
{
    mx_dg_level_key levelKey;
    mx_dg_add_ur_item* undoAddItem;
    mx_rect box = geom.box(err);
    mx_dg_level_key currentLevelKey(currentLevel, 0);
    mx_geom* dupGeom;

    // delete any selections

    delSelected(err);
    MX_ERROR_CHECK(err);

    geom.colourId = currentColourId;

    geom.styleId = (geom.dimension() == 1) ? currentLineStyleId : currentAreaStyleId;

    levelKey = area->addGeometry(err, currentLevelKey, geom);
    MX_ERROR_CHECK(err);

    // add the geometry to the selected list

    dupGeom = geom.duplicate();
    dupGeom->selectCount = mx_dg_selected::all_selection;

    undoAddItem = new mx_dg_add_ur_item;
    undoAddItem->setLevelKey(levelKey);

    undoInfo.startTransaction();
    undoInfo.addUndoItem(undoAddItem);
    undoInfo.setAreaOfInterest(box);

    delete lastGeometry;

    lastGeometry = geom.duplicate();

    refresh(err, box);
    MX_ERROR_CHECK(err);

    addSelected(err, levelKey, dupGeom);
    MX_ERROR_CHECK(err);

abort:;
}

void mx_dg_editor::refresh(int& err,
    const mx_rect& ibox)
{
    mx_doc_coord_t tl;
    mx_doc_coord_t br;
    double drawAddOn;

    mx_rect box = ibox;

    err = MX_ERROR_OK;

    drawAddOn = area->getDrawAddOn();

    box.xb -= drawAddOn;
    box.yb -= drawAddOn;
    box.xt += drawAddOn;
    box.yt += drawAddOn;

    tl.p = box.topLeft();
    br.p = box.bottomRight();

    // draw the area
    frame->refresh(err, &tl, &br);
    MX_ERROR_CHECK(err);
abort:;
}

void mx_dg_editor::setMouseMotion(int& err,
    bool setOn)
{
    ((mx_screen_device*)frame->getDevice())->setEvent(err, MX_EVENT_MOUSE_MOTION, setOn);
}

void mx_dg_editor::bell()
{
    window->bell();
}

void mx_dg_editor::deleteGeometry(int& err,
    const mx_dg_level_key& levelKey,
    mx_dg_level_key& nextLevelKey,
    mx_dg_level_key& prevLevelKey)
{
    uint32 selectedId;
    mx_dg_selected* selection;

    err = MX_ERROR_OK;

    selectedId = levelKey.compress();

    selection = getSelected(err, selectedId);
    MX_ERROR_CHECK(err);

    if (selection != nullptr)
        selection->isActive = false;

    area->deleteGeometry(err, levelKey, nextLevelKey,
        prevLevelKey);
    MX_ERROR_CHECK(err);

abort:;
}

void mx_dg_editor::undeleteGeometry(int& err,
    const mx_dg_level_key& levelKey,
    const mx_dg_level_key& nextLevelKey,
    const mx_dg_level_key& prevLevelKey)
{
    uint32 selectedId;
    mx_dg_selected* selection;

    err = MX_ERROR_OK;

    selectedId = levelKey.compress();

    selection = getSelected(err, selectedId);
    MX_ERROR_CHECK(err);

    if (selection != nullptr)
        selection->isActive = true;

    area->undeleteGeometry(err, levelKey, nextLevelKey,
        prevLevelKey);
    MX_ERROR_CHECK(err);

abort:;
}

int mx_dg_editor::numberSelected(int& err)
{
    return m_selected.size();
}

void mx_dg_editor::createGroup(int& err,
    mx_dg_group_key& groupKey)
{
    err = MX_ERROR_OK;

    (void)numberSelected(err);
    MX_ERROR_CHECK(err);

abort:;
}
