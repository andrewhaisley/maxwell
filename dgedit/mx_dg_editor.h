#pragma once

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
 *  The mx_dg_editor class provides a diagram editing interface on
 *  a diagram area
 *
 */

#include <map>

#include <mx_editor.h>
#include <mx_dg_area.h>
#include <mx_painter.h>

// forward declarations
class mx_dg_menubar;
class mx_dg_toolbar;
class mx_statusbar;
class mx_scrollable_frame;
class mx_area_layout;
class mx_dg_editor;
class mx_dg_state;
class mx_dg_selected;

typedef enum {
    MX_DGT_NONE,
    MX_DGT_TOP,
    MX_DGT_SPAN,
    MX_DGT_ARC,
    MX_DGT_FULL_CIRCLE,
    MX_DGT_FULL_ELLIPSE,
    MX_DGT_PARTIAL_CIRCLE,
    MX_DGT_PARTIAL_ELLIPSE,
    MX_DGT_POINT,
    MX_DGT_POLYPOINT,
    MX_DGT_RECT,
    MX_DGT_SNAP,
    MX_DGT_SELECT,
    MX_DGT_LAST
} MX_DGT_STATE;

#define MX_DG_UPDATE "UPDATE"
#define MX_DG_QUIT "QUIT"
#define MX_DG_RETURN "RETURN"
#define MX_DG_UNDO "UNDO"
#define MX_DG_REDO "REDO"
#define MX_DG_CUT "CUT"
#define MX_DG_PASTE "PASTE"
#define MX_DG_SELECT_ALL "SELECT_ALL"
#define MX_DG_ROTATE "ROTATE"
#define MX_DG_SCALE "SCALE"
#define MX_DG_UP_LEVEL "UP_LEVEL"
#define MX_DG_DOWN_LEVEL "DOWN_LEVEL"
#define MX_DG_CHOOSE_LEVEL "CHOOSE_LEVEL"
#define MX_DG_CURRENT_LEVEL "CURRENT_LEVEL"
#define MX_DG_LINE_STYLE "LINE_STYLE"
#define MX_DG_AREA_STYLE "AREA_STYLE"
#define MX_DG_TEXT_STYLE "TEXT_STYLE"
#define MX_DG_COLOUR_STYLE "COLOUR_STYLE"
#define MX_DG_GRID "GRID"
#define MX_DG_SNAP "SNAP"
#define MX_DG_GUIDE "GUIDE"
#define MX_DG_SPAN "SPAN"
#define MX_DG_ARC "ARC"
#define MX_DG_PLINE "PLINE"
#define MX_DG_RECT "RECT"
#define MX_DG_SELECT "SELECT"
#define MX_DG_ALIGN "ALIGN"
#define MX_DG_FLIP "FLIP"
#define MX_DG_GROUP "GROUP"
#define MX_DG_UNGROUP "UNGROUP"

#define MX_DG_NO_HIGHLIGHT_KEY 0
#define MX_DG_ALL_HIGHLIGHT_KEY ((uint32) - 1)

#define MX_DG_ALL_SELECTED_KEY ((uint32) - 1)

class mx_dg_select_iterator : public mx_dg_iterator {
public:
    mx_dg_select_iterator(mx_dg_editor* editor);

    void processFunction(int& err,
        mx_dg_quad_geom& quadGeom,
        bool& advance,
        bool& done,
        bool skippedSome);

    void processFunctionWGM(int& err,
        mx_dg_quad_geom& quadGeom);

    inline void setTolerance(double tolerance)
    {
        snapInfo.setTolerance(tolerance);
    }

    inline void resetSnapInfo() { snapInfo.setCount(MX_GEOM_SNAP_NONE); };
    inline void setPoint(const mx_point& ip) { testPoint = ip; };

    inline void setWholeGeometryMode(bool im) { wholeGeometryMode = im; };
    inline bool getWholeGeometryMode() const { return wholeGeometryMode; };
    inline void setAdditiveMode(bool im) { additiveMode = im; };
    inline bool getAdditiveMode() const { return additiveMode; };
    inline void setSnapping(bool is) { snapInfo.setMode(is); };

private:
    mx_dg_editor* editor;
    mx_snap_info snapInfo;
    mx_point testPoint;
    uint32 lastSelectedId;
    bool wholeGeometryMode;
    bool additiveMode;
};

class mx_dg_grid {
public:
    mx_dg_grid();

    inline bool isActive() { return active; };
    inline mx_point getSize() { return size; };
    inline mx_point getAnchor() { return anchor; };

    inline void toggleActive() { active = !active; };
    inline void setActive(bool iactive) { active = iactive; };
    inline void setSize(mx_point& ip) { size = ip; };
    inline void setAnchor(mx_point& ip) { anchor = ip; };

    void snapPoint(mx_point& inpoint);
    void draw(int& err,
        mx_rect& drawRect,
        mx_device* device);

private:
    bool active;
    mx_point size;
    mx_point anchor;
};

class mx_dg_selected {
    friend class mx_dg_editor;

public:
    mx_dg_selected(mx_geom* geom,
        const mx_dg_level_key& levelKey);
    ~mx_dg_selected();
    inline mx_geom* getGeom() const { return geom; };

    inline uint32 getSelectedId() const { return selectedId; };

    inline void setSelectedId(uint32 id) { selectedId = id; };

    inline int getStyleId() const { return storedStyleId; };
    inline int getColourId() const { return storedColourId; };

    static const int null_selection = -1;
    static const int all_selection = -2;

    void addSelectCount(mx_geom* geom,
        int selectCount,
        bool& allGone);

    void draw(int& err,
        mx_rect* regionOfInterest,
        mx_style_device* device);

private:
    mx_geom* geom;
    bool isActive;
    uint32 selectedId;
    bool addSelections;
    int nselectCounts;
    int* selectCounts;
    int storedStyleId;
    int storedColourId;
};

class mx_dg_ur_item {
public:
    mx_dg_ur_item() { };
    virtual ~mx_dg_ur_item() { };

public:
    virtual void undo(int& err, mx_dg_editor* ed) = 0;
    virtual void redo(int& err, mx_dg_editor* ed) = 0;
};

class mx_dg_geom_ur_item : public mx_dg_ur_item {
public:
    mx_dg_geom_ur_item() { replaceGeom = nullptr; };
    ~mx_dg_geom_ur_item() { delete replaceGeom; };

    inline mx_dg_level_key getLevelKey() const { return levelKey; };
    inline void setLevelKey(const mx_dg_level_key& ik) { levelKey = ik; };

protected:
    mx_dg_level_key levelKey;
    mx_geom* replaceGeom;
};

class mx_dg_swap_ur_item : public mx_dg_geom_ur_item {
public:
    void undo(int& err, mx_dg_editor* ed) { };
    void redo(int& err, mx_dg_editor* ed) { };
};

class mx_dg_add_ur_item : public mx_dg_geom_ur_item {
public:
    void undo(int& err, mx_dg_editor* ed);
    void redo(int& err, mx_dg_editor* ed);

private:
    mx_dg_level_key prevKey;
    mx_dg_level_key nextKey;
};

class mx_dg_delete_ur_item : public mx_dg_geom_ur_item {
public:
    void undo(int& err, mx_dg_editor* ed) { };
    void redo(int& err, mx_dg_editor* ed) { };
};

class mx_dg_undo_action {
public:
    mx_dg_undo_action();
    ~mx_dg_undo_action();

    void undo(int& err, mx_dg_editor* editor);
    void redo(int& err, mx_dg_editor* editor);

    void addUndoItem(mx_dg_ur_item* uitem);
    void clear();

    inline bool isClear() { return nundos == 0; };

    inline void setAreaOfInterest(const mx_rect& r) { areaOfInterest = r; };

private:
    int nallocUndos;
    int nundos;
    mx_dg_ur_item** undoItems;
    mx_rect areaOfInterest;
};

class mx_dg_undo {
public:
    mx_dg_undo(mx_dg_editor* editor,
        int maxLevels);

    ~mx_dg_undo();

    void undo(int& err);
    void redo(int& err);

    void startTransaction();

    void addUndoItem(mx_dg_ur_item* uitem);
    inline void setAreaOfInterest(const mx_rect& r)
    {
        actions[currentAction].setAreaOfInterest(r);
    };

private:
    mx_dg_editor* editor;
    int currentAction;
    int maxLevels;
    mx_dg_undo_action* actions;
};

class mx_dg_editor : public mx_editor {
    // MX_RTTI(mx_dg_editor_class_e)
public:
    virtual ~mx_dg_editor();
    mx_dg_editor(int& err, mx_app& a,
        mx_diagram_area* dgarea);

    // get a pointer to the current document
    mx_document* get_document() { return nullptr; };

    mx_document* open_derived_doc_class(int& err, char* name, bool recover) { return nullptr; };

    void file_export(int& err, char* file_name, mx_file_type_t type) { };

    void setMouseMotion(int& err,
        bool setOn);

    void draw(int& err, mx_draw_event& event);
    void preScroll(int& err, mx_scroll_event& event);
    void postScroll(int& err, mx_scroll_event& event);

    mx_dg_state* pushState(int& err,
        MX_DGT_STATE newStateType);

    void pushState(int& err, mx_dg_state* newState);

    mx_dg_state* popState(int& err);

    inline mx_dg_state* getCurrentState() { return currentState; };

    // add an item to the temporarily highlighted list
    uint32 addHighlighted(int& err, mx_geom* geom);

    // remove an item to the temporarily highlighted list
    void delHighlighted(int& err, uint32 hashKey = MX_DG_ALL_HIGHLIGHT_KEY);

    void drawHighlighted(int& err, uint32 hashKey = MX_DG_ALL_HIGHLIGHT_KEY);

    mx_geom* getHighlighted(int& err, uint32 hashKey);

    // selected list

    uint32 addSelected(int& err, const mx_dg_level_key& levelKey, mx_geom* geom);

    bool testSelectedHit(int& err, mx_point& testPoint);

    bool delSelected(int& err, const mx_dg_level_key& levelKey);

    bool delSelected(int& err, uint32 hashKey = MX_DG_ALL_SELECTED_KEY, uint32 saveFromDelete = MX_DG_ALL_SELECTED_KEY);

    void drawSelected(int& err, uint32 hashKey = MX_DG_ALL_HIGHLIGHT_KEY, mx_rect* regionOfInterest = nullptr);

    void drawSelectedGeom(int& err, mx_dg_selected* selection, mx_rect* regionOfInterest = nullptr);

    mx_dg_selected* getSelected(int& err, uint32 hashKey);

    mx_dg_selected* getSelected(int& err, const mx_dg_level_key& levelKey);

    mx_geom* getSelectedGeom(int& err, uint32 hashKey);

    mx_device* getDevice();

    void addGeometry(int& err, mx_geom& geom);

    inline mx_dg_grid* getGrid() { return &grid; };

    void toggleGrid(int& err);

    void bell();

    inline mx_geom* getLastGeometry() { return lastGeometry; };

    inline double getSnapDistance() const { return snapDistance; };
    inline void setSnapDistance(double s) { snapDistance = s; };

    inline mx_diagram_area* getArea() const { return area; };

    inline void undo(int& err) { undoInfo.undo(err); };
    inline void redo(int& err) { undoInfo.redo(err); };

    void refresh(int& err, const mx_rect& ibox);

    void deleteGeometry(int& err, const mx_dg_level_key& levelKey, mx_dg_level_key& nextLevelKey, mx_dg_level_key& prevLevelKey);

    void undeleteGeometry(int& err, const mx_dg_level_key& levelKey, const mx_dg_level_key& nextLevelKey, const mx_dg_level_key& prevLevelKey);

    int numberSelected(int& err);

    void createGroup(int& err, mx_dg_group_key& groupKey);

private:
    mx_area_layout* layout;
    mx_dg_menubar* menu;
    mx_dg_toolbar* toolbar;
    mx_statusbar* statusbar;
    mx_scrollable_frame* frame;
    mx_diagram_area* area;

    // create the frame
    void create_frame(int& err);

    // set frame target parameters
    void setup_frame_target_params();

    mx_area_layout& set_sheet_layout(int& err, mx_diagram_area* dgarea);

    void addStandardStyles(int& err);

    mx_dg_grid grid;

    mx_dg_state* topState;
    mx_dg_state* currentState;

    uint32 lastHashKey;
    std::map<int, mx_geom *> m_tempHighlighted;
    std::map<int, mx_dg_selected *> m_selected;

    int32 currentColourId;
    int32 currentLineStyleId;
    int32 currentAreaStyleId;
    int currentLevel;

    mx_geom* lastGeometry;

    double snapDistance;

    mx_dg_undo undoInfo;
};
