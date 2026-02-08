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
#ifndef MX_EVENT_H
#define MX_EVENT_H
/*
 * MODULE : mx_event.h
 *
 * AUTHOR : David Miller
 *
 * This file
 *
 * DESCRIPTION:
 * Module mx_event.h
 *
 *
 *
 *
 */

#include <mx.h>
#include <mx_doc_coord.h>

class mx_device;

// base events handled in maxwell applications

typedef enum {
    MX_SCROLL_OFFSET_TL,
    MX_SCROLL_OFFSET_T,
    MX_SCROLL_OFFSET_TR,
    MX_SCROLL_OFFSET_L,
    MX_SCROLL_OFFSET_C,
    MX_SCROLL_OFFSET_R,
    MX_SCROLL_OFFSET_BL,
    MX_SCROLL_OFFSET_B,
    MX_SCROLL_OFFSET_BR
} MX_SCROLL_OFFSET;

const unsigned long int MX_EVENT_NO_EVENT = 0;
const unsigned long int MX_EVENT_BUTTON_PRESS = 1 << 1;
const unsigned long int MX_EVENT_BUTTON_RELEASE = 1 << 2;
const unsigned long int MX_EVENT_BUTTON_MOTION_START = 1 << 3;
const unsigned long int MX_EVENT_BUTTON_MOTION = 1 << 4;
const unsigned long int MX_EVENT_BUTTON_MOTION_END = 1 << 5;
const unsigned long int MX_EVENT_DOUBLE_BUTTON_PRESS = 1 << 6;
const unsigned long int MX_EVENT_TRIPLE_BUTTON_PRESS = 1 << 7;
const unsigned long int MX_EVENT_KEY_PRESS = 1 << 8;
const unsigned long int MX_EVENT_KEY_RELEASE = 1 << 9;
const unsigned long int MX_EVENT_PREDRAW = 1 << 10;
const unsigned long int MX_EVENT_DRAW = 1 << 11;
const unsigned long int MX_EVENT_POSTDRAW = 1 << 12;
const unsigned long int MX_EVENT_PRESCROLL = 1 << 13;
const unsigned long int MX_EVENT_SCROLL = 1 << 14;
const unsigned long int MX_EVENT_POSTSCROLL = 1 << 15;
const unsigned long int MX_EVENT_SCROLLDRAG = 1 << 16;
const unsigned long int MX_EVENT_RESIZE = 1 << 17;
const unsigned long int MX_EVENT_EXPOSE = 1 << 18;
const unsigned long int MX_EVENT_TEXT = 1 << 19;
const unsigned long int MX_EVENT_MOUSE_MOTION = 1 << 20;
const unsigned long int MX_EVENT_SPECIAL_DRAW = 1 << 21;

// composite events

// button motion events
const unsigned long int MX_EVENT_BUTTON_MOTIONS = MX_EVENT_BUTTON_MOTION_START | MX_EVENT_BUTTON_MOTION | MX_EVENT_BUTTON_MOTION_END;

// button press events
const unsigned long int MX_EVENT_BUTTON_PRESSES = MX_EVENT_BUTTON_PRESS | MX_EVENT_DOUBLE_BUTTON_PRESS | MX_EVENT_TRIPLE_BUTTON_PRESS;

// the standard events one would expect from an editor
const unsigned long int MX_EVENT_STANDARD = MX_EVENT_BUTTON_PRESSES | MX_EVENT_BUTTON_RELEASE | MX_EVENT_BUTTON_MOTIONS | MX_EVENT_KEY_PRESS | MX_EVENT_TEXT | MX_EVENT_DRAW | MX_EVENT_SPECIAL_DRAW;

// the standard events for a screen type device
const unsigned long int MX_EVENT_XEVENTS = MX_EVENT_STANDARD | MX_EVENT_EXPOSE | MX_EVENT_MOUSE_MOTION | MX_EVENT_RESIZE;

// all scrolling events

const unsigned long int MX_EVENT_ALL_SCROLL = MX_EVENT_PRESCROLL | MX_EVENT_SCROLL | MX_EVENT_POSTSCROLL | MX_EVENT_SCROLLDRAG | MX_EVENT_BUTTON_MOTION | MX_EVENT_RESIZE;

// the standard events for a frame with scrolling

const unsigned long int MX_EVENT_SFRAME = MX_EVENT_STANDARD | MX_EVENT_ALL_SCROLL;

class mx_callback_target;

/*-------------------------------------------------
 * CLASS: mx_event
 *
 * DESCRIPTION: The base class of events which Maxwell
 * handles.
 *
 */

class mx_event {
public:
    mx_event();

    // allow for a degree of multiplexing of events which may
    // be very useful in the future

    // Event propagation is done by the destination target
    // setting itself as the target on for events generated on the
    // source. When it does this it tells the source its id for the
    // source (sourceId) and the source returns its id for the
    // destiantion (destId). (i.e. sourceId is the destinations
    // identifier for the source and destId is the sources identifier
    // for the destination

    // some events dont need convertion

    virtual void convert(int& err,
        int destId,
        mx_callback_target* destinationTarget,
        mx_event* outEvent)
        = 0;
    virtual void propagate(int& err,
        mx_callback_target* destinationTarget)
    {
        err = MX_ERROR_OK;
        return;
    };
    virtual unsigned long int getType() { return MX_EVENT_NO_EVENT; };

    inline int getSourceId() { return sourceId; };
    inline void setSourceId(int isourceId) { sourceId = isourceId; };

protected:
private:
    // the id of the source - passed to the target when it set up the
    // link
    int sourceId;
};

/*-------------------------------------------------
 * CLASS: mx_button_event
 *
 * DESCRIPTION: A button event in Maxwell
 *
 *
 */

class mx_button_event : public mx_event {
public:
    mx_button_event();

    inline int getButtonNumber() { return buttonNumber; };
    inline unsigned long int getType() { return eventType; };
    inline int getNclicks() { return nclicks; };
    inline bool getShift() { return shiftDown; };
    inline bool getControl() { return controlDown; };
    inline mx_doc_coord_t getCoord() { return coord; };
    inline mx_doc_coord_t getStartCoord() { return startCoord; };
    inline bool getInRange() { return coordInRange; };
    inline bool getStartInRange() { return startCoordInRange; };

    inline void setButtonNumber(int ibuttonNumber) { buttonNumber = ibuttonNumber; };
    inline void setType(unsigned long int ieventType) { eventType = ieventType; };
    inline void setNclicks(int inclicks) { nclicks = inclicks; };
    inline void setShift(int ishiftDown) { shiftDown = ishiftDown; };
    inline void setControl(int icontrolDown) { controlDown = icontrolDown; };
    inline void setCoord(mx_doc_coord_t& icoord) { coord = icoord; };
    inline void setStartCoord(mx_doc_coord_t& istartCoord) { startCoord = istartCoord; };
    inline void setInRange(bool iinRange) { coordInRange = iinRange; };
    inline void setStartInRange(bool iinRange) { startCoordInRange = iinRange; };

    void convert(int& err,
        int destId,
        mx_callback_target* destinationTarget,
        mx_event* outEvent);

    void convert(int& err,
        int destId,
        mx_callback_target* destinationTarget);

    void propagate(int& err,
        mx_callback_target* destinationTarget);

private:
    // event type
    unsigned long int eventType;

    // button Number
    int buttonNumber;

    // the number of clicks
    int nclicks;

    // true if the shift key is down
    bool shiftDown;

    // true if the control key is down
    bool controlDown;

    // where the event occured
    mx_doc_coord_t coord;

    // for a motion event where motion started
    mx_doc_coord_t startCoord;

    // test whether the event is within range
    bool coordInRange;
    bool startCoordInRange;
};

/*-------------------------------------------------
 * CLASS: mx_key_event
 *
 * DESCRIPTION: A key event in Maxwell
 *
 *
 */

class mx_key_event : public mx_event {
public:
    mx_key_event();

    inline const char* getKeyName() { return keyName; };
    inline char* getString() { return string; };
    inline int getTotalStringSize() { return 128; };
    inline int getStringSize() { return nstring; };
    inline bool getShift() { return shiftDown; };
    inline bool getControl() { return controlDown; };

    inline unsigned long int getType() { return release ? MX_EVENT_KEY_RELEASE : MX_EVENT_KEY_PRESS; };

    inline void setKeyName(const char* ikeyName) { keyName = ikeyName; };
    inline void setStringSize(int instring) { nstring = instring; };
    inline void setShift(int ishiftDown) { shiftDown = ishiftDown; };
    inline void setControl(int icontrolDown) { controlDown = icontrolDown; };
    inline void setRelease(int irelease) { release = irelease; };

    void convert(int& err,
        int destId,
        mx_callback_target* destinationTarget,
        mx_event* outEvent);

    void propagate(int& err,
        mx_callback_target* destinationTarget);

    // is the event an arrow key press
    bool is_arrow();

    // is the event a return key press
    bool is_return();

private:
    // press or release
    bool release;

    // modifier keys - for key events
    bool shiftDown;
    bool controlDown;

    // key press event information
    const char* keyName;

    // might be more than one for a particular keysym
    int nstring;
    char string[128];
};

/*-------------------------------------------------
 * CLASS: mx_text_event
 *
 * DESCRIPTION: A text event in Maxwell
 *
 *
 */

class mx_text_event : public mx_event {
public:
    mx_text_event() { text = NULL; };
    mx_text_event(const char* str)
    {
        text = NULL;
        setText(str);
    };

    virtual ~mx_text_event() { delete[] text; };

    mx_text_event& operator=(const mx_text_event& te);

    inline unsigned long int getType() { return MX_EVENT_TEXT; };

    inline const char* getText() const { return text; };
    void setText(const char* str);

    void convert(int& err,
        int destId,
        mx_callback_target* destinationTarget,
        mx_event* outEvent);

    void propagate(int& err,
        mx_callback_target* destinationTarget);

private:
    char* text;
};

/*-------------------------------------------------
 * CLASS: mx_expose_event
 *
 * DESCRIPTION: A expose event in Maxwell
 *
 *
 */

class mx_expose_event : public mx_event {
public:
    mx_expose_event() { };

    mx_expose_event(mx_device* idevice) { device = idevice; };

    inline void setRect(mx_doc_coord_t& itl,
        mx_doc_coord_t& ibr)
    {
        tl = itl;
        br = ibr;
    };
    inline mx_doc_coord_t getTL() { return tl; };
    inline mx_doc_coord_t getBR() { return br; };
    inline int getCount() { return count; };
    inline void setCount(int icount) { count = icount; };

    inline unsigned long int getType() { return MX_EVENT_EXPOSE; };

    void convert(int& err,
        int destId,
        mx_callback_target* destinationTarget,
        mx_event* outEvent);

    void propagate(int& err,
        mx_callback_target* destinationTarget);

    inline mx_device* getDevice() { return device; };
    inline void setDevice(mx_device* idevice) { device = idevice; };

private:
    // the device exposed
    mx_device* device;

    mx_doc_coord_t tl;
    mx_doc_coord_t br;
    int count;
};

/*-------------------------------------------------
 * CLASS: mx_draw_event
 *
 * DESCRIPTION: A draw event in Maxwell
 *
 *
 */

class mx_draw_event : public mx_event {
public:
    mx_draw_event() { };
    mx_draw_event(mx_device* device) { setDevice(device); };
    mx_draw_event(mx_expose_event& event);

    inline void setRect(mx_doc_coord_t& itl,
        mx_doc_coord_t& ibr)
    {
        tl = itl;
        br = ibr;
    };
    inline mx_doc_coord_t getTL() { return tl; };
    inline mx_doc_coord_t getBR() { return br; };

    inline void setTL(mx_doc_coord_t& itl) { tl = itl; };
    inline void setBR(mx_doc_coord_t& ibr) { br = ibr; };

    inline unsigned long int getType() { return eventType; };
    inline void setType(unsigned long int itype) { eventType = itype; };

    inline mx_device* getDevice() { return device; };
    inline void setDevice(mx_device* idevice) { device = idevice; };

    inline int getCount() { return count; };
    inline void setCount(int icount) { count = icount; };

    void convert(int& err,
        int destId,
        mx_callback_target* destinationTarget,
        mx_event* outEvent);

    void propagate(int& err,
        mx_callback_target* destinationTarget);

private:
    // whether this is pre draw, a draw or a post draw event
    unsigned long int eventType;

    // the device on which to draw
    mx_device* device;

    mx_doc_coord_t tl;
    mx_doc_coord_t br;

    int count;
};

/*-------------------------------------------------
 * CLASS: mx_scroll_event
 *
 * DESCRIPTION: A scroll event in Maxwell
 *
 *
 */

class mx_scroll_event : public mx_event {
public:
    mx_scroll_event();
    inline unsigned long int getType() { return eventType; };
    inline void setType(unsigned long int ieventType) { eventType = ieventType; };

    virtual void convert(int& err,
        int destId,
        mx_callback_target* destinationTarget,
        mx_event* outEvent);

    void propagate(int& err,
        mx_callback_target* destinationTarget);

    inline void setIsVertical(bool iisVertical) { isVertical = iisVertical; };
    inline void setTLvalue(double tlValue) { topLeftValue = tlValue; };
    inline void setNewTopLeft(mx_doc_coord_t& ntl) { newTopLeft = ntl; };
    inline void setOldTopLeft(mx_doc_coord_t& otl) { oldTopLeft = otl; };
    inline void setUseScrollPosition(bool iusp) { useScrollPosition = iusp; };
    inline void setScrollPosition(mx_doc_coord_t& sp) { scrollPosition = sp; };
    inline void setSameX(bool isameX) { sameX = isameX; };
    inline void setSameY(bool isameY) { sameY = isameY; };
    inline void setOffset(MX_SCROLL_OFFSET ioffset) { offset = ioffset; };

    inline mx_doc_coord_t getNewTopLeft() { return newTopLeft; };
    inline mx_doc_coord_t getOldTopLeft() { return oldTopLeft; };
    inline bool getUseScrollPosition() { return useScrollPosition; };
    inline mx_doc_coord_t getScrollPosition() { return scrollPosition; };
    inline bool getSameX() { return sameX; };
    inline bool getSameY() { return sameY; };
    inline bool getIsVertical() { return isVertical; };
    inline double getTLvalue() { return topLeftValue; };
    inline MX_SCROLL_OFFSET getOffset() { return offset; };

private:
    unsigned long int eventType;
    bool isVertical;
    double topLeftValue;
    mx_doc_coord_t newTopLeft;
    mx_doc_coord_t oldTopLeft;

    bool useScrollPosition;
    mx_doc_coord_t scrollPosition;
    MX_SCROLL_OFFSET offset;
    bool sameX;
    bool sameY;
};

/*-------------------------------------------------
 * CLASS: mx_resize_event
 *
 * DESCRIPTION: A resize event in Maxwell
 *
 *
 */

class mx_resize_event : public mx_event {
public:
    mx_resize_event() { };

    inline void setRect(mx_doc_coord_t& itl,
        mx_doc_coord_t& ibr)
    {
        tl = itl;
        br = ibr;
    };
    inline mx_doc_coord_t getTL() { return tl; };
    inline mx_doc_coord_t getBR() { return br; };

    inline void setTL(mx_doc_coord_t& itl) { tl = itl; };
    inline void setBR(mx_doc_coord_t& ibr) { br = ibr; };

    void convert(int& err,
        int destId,
        mx_callback_target* destinationTarget,
        mx_event* outEvent);

    inline unsigned long int getType() { return MX_EVENT_RESIZE; };

    void propagate(int& err,
        mx_callback_target* destinationTarget);

private:
    mx_doc_coord_t tl;
    mx_doc_coord_t br;
};

#endif
