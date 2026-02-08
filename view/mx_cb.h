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
#ifndef _MX_CB_H
#define _MX_CB_H
/*
 * MODULE : mx_cb.h
 *
 * AUTHOR : David Miller
 *
 * This file
 *
 * DESCRIPTION: Generalised maxwell callback
 * structures
 * Module mx_event.h
 *
 *
 *
 *
 */

#include <mx_event.h>

typedef void (*MX_COORD_CONVERSION_FUNC)(int& err,
    mx_callback_target* source,
    mx_callback_target* dest,
    mx_doc_coord_t& coord,
    MX_COORD_TYPE coordType,
    bool& isInRange);

#define MX_MAX_PROPAGATE 2

/*-------------------------------------------------
 * CLASS: mx_cb_out_propagate
 *
 * DESCRIPTION: Details of the call back targets which
 * get propagated to
 *
 */

class mx_cb_out_propagate {
public:
    mx_cb_out_propagate()
    {
        set(NULL, -1);
    };

    inline void set(mx_callback_target* idestinationTarget,
        int idestinationTargetId)
    {
        destinationTarget = idestinationTarget;
        destinationTargetId = idestinationTargetId;
    };

    inline mx_callback_target* getTarget()
    {
        return destinationTarget;
    }

    inline int getId()
    {
        return destinationTargetId;
    }

private:
    mx_callback_target* destinationTarget;
    int destinationTargetId;
};

/*-------------------------------------------------
 * CLASS: mx_cb_in_propagate
 *
 * DESCRIPTION: Details of how incoming callback events
 * get treated
 *
 */

class mx_cb_in_propagate {
public:
    mx_cb_in_propagate()
    {
        set(NULL, NULL);
    };

    inline void set(mx_callback_target* isourceTarget,
        MX_COORD_CONVERSION_FUNC iconversionFunction)
    {
        sourceTarget = isourceTarget;
        conversionFunction = iconversionFunction;
    };

    inline mx_callback_target* getTarget()
    {
        return sourceTarget;
    }

    inline MX_COORD_CONVERSION_FUNC getFunction()
    {
        return conversionFunction;
    }

private:
    mx_callback_target* sourceTarget;
    MX_COORD_CONVERSION_FUNC conversionFunction;
};

class mx_callback_target {
public:
    mx_callback_target();
    virtual ~mx_callback_target();

    int setNextTarget(mx_callback_target* inextTarget,
        int targetId);

    // these callbacks are all defined - but if they
    // are on this class they just do nothing

    // button motion callbacks
    virtual void buttonMotionStart(int& err,
        mx_button_event& event);

    virtual void buttonMotion(int& err,
        mx_button_event& event);

    virtual void buttonMotionEnd(int& err,
        mx_button_event& event);

    // external text selection callback - called in response
    // to a request rather than automatically
    virtual void externalSelection(int& err, char* sel);

    // called by frame when it wants to get the current external
    // selection
    virtual const char* getExternalSelection(int& err);

    // button press callbacks
    virtual void buttonPress(int& err,
        mx_button_event& event);
    virtual void buttonRelease(int& err,
        mx_button_event& event);

    // button double press callbacks
    virtual void buttonDoublePress(int& err,
        mx_button_event& event);

    // button triple press callbacks
    virtual void buttonTriplePress(int& err,
        mx_button_event& event);

    // key callbacks
    virtual void keyPress(int& err,
        mx_key_event& event);

    virtual void keyRelease(int& err,
        mx_key_event& event);

    // string entry callbacks - these typically get pushed
    // into the target by an application

    virtual void textEntry(int& err,
        mx_text_event& event);

    // When an area is exposed several draw events
    // may be initialised - one for each rectangular
    // area to be exposed, and one for each sheet to
    // be exposed. It is possible that the application
    // might want to do something before a draw, and then
    // clean up after a draw (e.g. a stop drawing popup).

    virtual void preDraw(int& err,
        mx_draw_event& event);

    virtual void draw(int& err,
        mx_draw_event& event);

    virtual void specialDraw(int& err,
        mx_draw_event& event);

    virtual void postDraw(int& err,
        mx_draw_event& event);

    // tell the frame target that things are about to scroll
    // this function could be used to make the frame target
    // undo a hightlight prior to a scroll

    virtual void preScroll(int& err,
        mx_scroll_event& event);

    virtual void scroll(int& err,
        mx_scroll_event& event);

    virtual void postScroll(int& err,
        mx_scroll_event& event);

    virtual void scrollDrag(int& err,
        mx_scroll_event& event);

    virtual void resize(int& err,
        mx_resize_event& event);

    virtual void expose(int& err,
        mx_expose_event& event);

    // set an event at a low device level - e.g.
    // use XSelectInput for x device
    virtual void setEvent(int& err,
        unsigned long int eventType,
        bool setOn) { err = MX_ERROR_OK; };

    // event propagation - when a event is propagated to
    // another target it may be necessary to convert the
    // coordinates to

    inline void setProcessMask(unsigned long int newMask)
    {
        processMask = newMask;
    };

    inline void addProcessMask(unsigned long int newMask)
    {
        processMask |= newMask;
    };

    inline void clearProcessMask(unsigned long int newMask)
    {
        processMask &= ~newMask;
    };

    inline bool testProcessMask(unsigned long int newMask)
    {
        return ((processMask & newMask) != 0);
    };

    inline mx_cb_in_propagate* getInProp(int offset) { return inPropagate + offset; };
    inline mx_cb_out_propagate* getOutProp(int offset) { return outPropagate + offset; };

    void setToNotReceiveEvents(int id);

    void setToPropagateEvents(int& err,
        mx_callback_target* receiver,
        int receiverId);

    void setToNotPropagateEvents(mx_callback_target* target);

    void setToReceiveEvents(int& err,
        mx_callback_target* propagator,
        MX_COORD_CONVERSION_FUNC func);

    inline bool getFilterMotion() { return filterSameMotion; };
    inline void setFilterMotion(bool ifilterSameMotion) { filterSameMotion = ifilterSameMotion; };
    inline mx_doc_coord_t getLastMotion() { return lastMotionPosition; };
    inline void setLastMotion(const mx_doc_coord_t& ilastMotionPosition)
    {
        lastMotionPosition = ilastMotionPosition;
    };

protected:
    // call to propagate an event to other targets

    void propagateEvent(int& err,
        mx_event* originalEvent,
        mx_event* outEvent);

private:
    // the events this target is interested in
    unsigned long int processMask;

    // details of targets to which this event propagates
    int noutPropagate;
    mx_cb_out_propagate outPropagate[MX_MAX_PROPAGATE];

    // details of the targets which propagate to this target
    int ninPropagate;
    mx_cb_in_propagate inPropagate[MX_MAX_PROPAGATE];

    bool filterSameMotion;
    mx_doc_coord_t lastMotionPosition;
};

#endif
