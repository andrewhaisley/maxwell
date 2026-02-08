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
#ifndef MX_DG_STATE
#define MX_DG_STATE
/*
 * MODULE : mx_dg_state.h
 *
 * AUTHOR : David Miller
 *
 * This file
 *
 * DESCRIPTION:
 * Module mx_dg_state.h
 *
 *
 * Revision 1.1  1996/10/06 13:15:45  andrew
 * Initial revision
 *
 *
 */

#include "mx_dg_editor.h"

/*-------------------------------------------------
 * CLASS: mx_dg_state
 *
 * DESCRIPTION: The idea of an editor state is that
 * a state would
 *
 */

class mx_dg_state : public mx_callback_target {
public:
    mx_dg_state(mx_dg_editor* editor);
    ~mx_dg_state();

    void clearState();

    inline void setNextState(mx_dg_state* istate)
    {
        nextState = istate;
        if (istate != NULL)
            nextState->previousState = this;
    };
    inline void setType(MX_DGT_STATE itype) { type = itype; };
    inline MX_DGT_STATE getType() { return type; };
    inline mx_dg_editor* getEditor() { return editor; };

    void textEntry(int& err, mx_text_event& event);

    // default - no actions - these propagate events back
    void buttonPress(int& err, mx_button_event& event);
    void buttonRelease(int& err, mx_button_event& event);
    void buttonMotionStart(int& err,
        mx_button_event& event);
    void buttonMotion(int& err,
        mx_button_event& event);
    void buttonMotionEnd(int& err,
        mx_button_event& event);

    void keyPress(int& err,
        mx_key_event& event);

    // default real actions - e.g. point entry is often the
    // same for different states
    void ibuttonPress(int& err, mx_button_event& event);
    void ibuttonRelease(int& err, mx_button_event& event);
    void ibuttonMotionStart(int& err,
        mx_button_event& event);
    void ibuttonMotion(int& err,
        mx_button_event& event);
    void ibuttonMotionEnd(int& err,
        mx_button_event& event);

    virtual void tidyState(int& err) { err = MX_ERROR_OK; };
    void getStateData(int& err,
        mx_event* event,
        mx_dg_state* inState);

    virtual void setActive(int& err,
        mx_event* event,
        mx_dg_state* inState) { err = MX_ERROR_OK; };

    virtual void propagatedUp(int& err,
        mx_event* event,
        mx_dg_state* inState);

    virtual void returnGeom(int& err, mx_geom** geom) { *geom = NULL; }

    virtual void processPoint(int& err, mx_event* event, gmx_point* pf)
    {
        err = MX_ERROR_OK;
    };

    virtual void processSpan(int& err, mx_event* event, gmx_span* sp)
    {
        err = MX_ERROR_OK;
    };

    virtual void processRect(int& err, mx_event* event, gmx_rect* rt)
    {
        err = MX_ERROR_OK;
    };

    virtual void processPolypoint(int& err, mx_event* event, gmx_polypoint* pp)
    {
        err = MX_ERROR_OK;
    };

    virtual void processArc(int& err, mx_event* event, gmx_arc* a)
    {
        err = MX_ERROR_OK;
    };

    inline mx_dg_state* getNextState() { return nextState; };
    inline mx_dg_state* getPreviousState() { return previousState; };

    void endState(int& err,
        mx_event* event);

    void collapseStates(int& err);

protected:
    mx_dg_editor* editor;
    mx_dg_state* nextState;
    mx_dg_state* previousState;
    MX_DGT_STATE type;
};

/*-------------------------------------------------
 * CLASS: mx_dg_top_state
 *
 * DESCRIPTION: The idea of an editor state is that
 * a state would
 *
 */

class mx_dg_top_state : public mx_dg_state {
public:
    mx_dg_top_state(mx_dg_editor* editor);
    void textEntry(int& err, mx_text_event& event);

    void setActive(int& err,
        mx_event* event,
        mx_dg_state* inState);

    void propagatedUp(int& err,
        mx_event* event,
        mx_dg_state* inState);

    void keyPress(int& err,
        mx_key_event& event);

    void processArc(int& err, mx_event* event, gmx_arc* sp);
    void processSpan(int& err, mx_event* event, gmx_span* sp);
    void processRect(int& err, mx_event* event, gmx_rect* rt);
    void processPolypoint(int& err, mx_event* event, gmx_polypoint* pp);

private:
};

/*-------------------------------------------------
 * CLASS: mx_dg_point_state
 *
 * DESCRIPTION: The idea of an editor state is that
 * a state would
 *
 */

class mx_dg_point_state : public mx_dg_state {
public:
    mx_dg_point_state(mx_dg_editor* editor);

    void buttonPress(int& err, mx_button_event& event)
    {
        buttonAction(err, event);
    };
    void buttonRelease(int& err, mx_button_event& event)
    {
        buttonAction(err, event);
    };
    void buttonMotionStart(int& err, mx_button_event& event)
    {
        buttonAction(err, event);
    };
    void buttonMotion(int& err,
        mx_button_event& event)
    {
        buttonAction(err, event);
    };
    void buttonMotionEnd(int& err,
        mx_button_event& event)
    {
        buttonAction(err, event);
    };

    void returnGeom(int& err, mx_geom** geom);

private:
    void buttonAction(int& err,
        mx_button_event& event);
    bool pointFound;
    gmx_point p;
};

/*-------------------------------------------------
 * CLASS: mx_dg_span_state
 *
 * DESCRIPTION: The idea of an editor state is that
 * a state would
 *
 */

class mx_dg_span_state : public mx_dg_state {
public:
    mx_dg_span_state(mx_dg_editor* editor);

    void buttonPress(int& err, mx_button_event& event);
    void buttonMotion(int& err,
        mx_button_event& event);
    void buttonMotionEnd(int& err,
        mx_button_event& event);

    void tidyState(int& err);

    void returnGeom(int& err, mx_geom** geom);

    void processPoint(int& err,
        mx_event* event,
        gmx_point* pf);

private:
    int npoints;
    gmx_span ispan;
    uint32 highlightedId;
};

/*-------------------------------------------------
 * CLASS: mx_dg_snap_state
 *
 * DESCRIPTION: The idea of an editor state is that
 * a state would
 *
 */

class mx_dg_snap_state : public mx_dg_state {
public:
    mx_dg_snap_state(mx_dg_editor* editor);

    void buttonPress(int& err, mx_button_event& event);
    void buttonMotion(int& err,
        mx_button_event& event);
    void buttonMotionEnd(int& err,
        mx_button_event& event);

    void tidyState(int& err);

    void processPoint(int& err,
        mx_event* event,
        gmx_point* pf);

private:
    int npoints;
    gmx_span ispan;
    uint32 highlightedId;
};

/*-------------------------------------------------
 * CLASS: mx_dg_rect_state
 *
 * DESCRIPTION: The idea of an editor state is that
 * a state would
 *
 */

class mx_dg_rect_state : public mx_dg_state {
public:
    mx_dg_rect_state(mx_dg_editor* editor);

    void buttonPress(int& err, mx_button_event& event);
    void buttonMotion(int& err,
        mx_button_event& event);
    void buttonMotionEnd(int& err,
        mx_button_event& event);

    void tidyState(int& err);

    void returnGeom(int& err, mx_geom** geom);

    void processPoint(int& err,
        mx_event* event,
        gmx_point* pf);

private:
    int npoints;
    gmx_rect irect;
    uint32 highlightedId;
};

/*-------------------------------------------------
 * CLASS: mx_dg_arc_state
 *
 * DESCRIPTION: The idea of an editor state is that
 * a state would
 *
 */

class mx_dg_arc_state : public mx_dg_state {
public:
    mx_dg_arc_state(mx_dg_editor* editor);

    void buttonPress(int& err, mx_button_event& event);
    void buttonMotion(int& err,
        mx_button_event& event);
    void buttonMotionEnd(int& err,
        mx_button_event& event);

    void tidyState(int& err);

    void returnGeom(int& err, mx_geom** geom);

    void processArc(int& err,
        mx_event* event,
        gmx_arc* a);

protected:
    int npoints;
    gmx_arc iarc;
    uint32 highlightedId;
};

/*-------------------------------------------------
 * CLASS: mx_dg_farc_state
 *
 * DESCRIPTION: The idea of an editor state is that
 * a state would
 *
 */

class mx_dg_farc_state : public mx_dg_arc_state {
public:
    mx_dg_farc_state(mx_dg_editor* editor);

    void buttonPress(int& err, mx_button_event& event);

    void processPoint(int& err,
        mx_event* event,
        gmx_point* pf);

    inline void setBoxState(bool ibx) { boxState = ibx; };

protected:
    virtual mx_point pointToSize(mx_point& p) = 0;
    bool boxState;
};

/*-------------------------------------------------
 * CLASS: mx_dg_fcircle_state
 *
 * DESCRIPTION: The idea of an editor state is that
 * a state would
 *
 */

class mx_dg_fcircle_state : public mx_dg_farc_state {
public:
    mx_dg_fcircle_state(mx_dg_editor* editor);

private:
    mx_point pointToSize(mx_point& p);
};

/*-------------------------------------------------
 * CLASS: mx_dg_fellipse_state
 *
 * DESCRIPTION: The idea of an editor state is that
 * a state would
 *
 */

class mx_dg_fellipse_state : public mx_dg_farc_state {
public:
    mx_dg_fellipse_state(mx_dg_editor* editor);

private:
    mx_point pointToSize(mx_point& p);
};

/*-------------------------------------------------
 * CLASS: mx_dg_parc_state
 *
 * DESCRIPTION: The idea of an editor state is that
 * a state would
 *
 */

class mx_dg_parc_state : public mx_dg_arc_state {
public:
    mx_dg_parc_state(mx_dg_editor* editor);

    void buttonPress(int& err,
        mx_button_event& event);

    void processPoint(int& err,
        mx_event* event,
        gmx_point* pf);

    void processArc(int& err,
        mx_event* event,
        gmx_arc* arc);

protected:
    virtual double pointToAngle(mx_point& p) = 0;

private:
    /*
      npoints = 0 nothing doing
      npoints = 1 got arc
      npoints = 2 got start point
      npoints = 3 finished
      */
    double startAng;
};

/*-------------------------------------------------
 * CLASS: mx_dg_pcircle_state
 *
 * DESCRIPTION: The idea of an editor state is that
 * a state would
 *
 */

class mx_dg_pcircle_state : public mx_dg_parc_state {
public:
    mx_dg_pcircle_state(mx_dg_editor* editor);

private:
    double pointToAngle(mx_point& p);
};

/*-------------------------------------------------
 * CLASS: mx_dg_pellipse_state
 *
 * DESCRIPTION: The idea of an editor state is that
 * a state would
 *
 */

class mx_dg_pellipse_state : public mx_dg_parc_state {
public:
    mx_dg_pellipse_state(mx_dg_editor* editor);

private:
    double pointToAngle(mx_point& p);
};

/*-------------------------------------------------
 * CLASS: mx_dg_polypoint_state
 *
 * DESCRIPTION: The idea of an editor state is that
 * a state would
 *
 */

class mx_dg_polypoint_state : public mx_dg_state {
public:
    mx_dg_polypoint_state(mx_dg_editor* editor);

    void buttonPress(int& err, mx_button_event& event);
    void buttonDoublePress(int& err, mx_button_event& event);
    void buttonMotion(int& err,
        mx_button_event& event);
    void buttonMotionEnd(int& err,
        mx_button_event& event);

    void keyPress(int& err,
        mx_key_event& event);

    void returnGeom(int& err, mx_geom** geom);

    void processPoint(int& err,
        mx_event* event,
        gmx_point* pf);

    void tidyState(int& err);

private:
    gmx_polypoint ipp;
    gmx_span tempSpan;
    uint32 highlightedId;
    uint32 spanHighlightedId;
};

/*-------------------------------------------------
 * CLASS: mx_dg_select_state
 *
 * DESCRIPTION: The idea of an editor state is that
 * a state would
 *
 */

class mx_dg_select_state : public mx_dg_state {
public:
    mx_dg_select_state(mx_dg_editor* editor);

    void buttonRelease(int& err, mx_button_event& event);
    void buttonMotionStart(int& err,
        mx_button_event& event);
    void buttonMotion(int& err,
        mx_button_event& event);
    void buttonMotionEnd(int& err,
        mx_button_event& event);

    void keyPress(int& err,
        mx_key_event& event);

    void processPoint(int& err,
        mx_event* event,
        gmx_point* pf);

    void processRelease(int& err,
        mx_event* event,
        gmx_point* pf);

    void processMotionStart(int& err,
        mx_event* event,
        gmx_point* pf);

    void processMotion(int& err,
        mx_event* event,
        gmx_point* pf);

    void processMotionEnd(int& err,
        mx_event* event,
        gmx_point* pf);

    void tidyState(int& err);

private:
    mx_dg_select_iterator iterator;
};

#endif
