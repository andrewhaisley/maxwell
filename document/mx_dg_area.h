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
 * MODULE/CLASS : mx_diagram_area
 *
 * AUTHOR : Andrew Haisley & Dave Miller
 *
 *
 *
 * DESCRIPTION:
 *
 * Areas specifically for holding diagrams
 *
 *
 *
 *
 */

#include <map>

#include <mx.h>
#include <mx_db.h>

#include <mx_area.h>
#include <mx_gstyle.h>
#include <mx_stydev.h>

typedef enum {
    MX_DG_GROUP_GET_NONE,
    MX_DG_GROUP_GET_LEVEL,
    MX_DG_GROUP_GET_QUAD,
    MX_DG_GROUP_GET_GEOM
} MX_DG_GROUP_GET_STAGE;

typedef enum {
    MX_DG_QUAD_ACTIVE = 1,
    MX_DG_QUAD_STORE_BOX = 2,
    MX_DG_QUAD_IS_SEGMENT = 4,
    MX_DG_QUAD_FIRST_SEGMENT = 8,
    MX_DG_QUAD_IS_AREA = 16,
    MX_DG_QUAD_IS_CLOSED = 32
} mx_dg_flag_t;

typedef enum {
    MX_DG_SEGMENT_ACTIVE = 1,
    MX_DG_SEGMENT_FILTERED = 2
} mx_dg_segment_flag_t;

// store in order of work required
typedef enum {
    MX_DG_QGEOM_GET_NONE = 0,
    MX_DG_QGEOM_GET_QGEOM = 1,
    MX_DG_QGEOM_GET_GBOX = 2,
    MX_DG_QGEOM_GET_GFILTER = 3,
    MX_DG_QGEOM_GET_GBUFFER = 4,
    MX_DG_QGEOM_GET_GEOM = 5
} mx_dg_qgeom_get_t;

typedef enum {
    MX_DG_SEGMENT_INDIVIDUAL,
    MX_DG_SEGMENT_ALL_WITH_FIRST,
    MX_DG_SEGMENT_MATCHING_WITH_FIRST
} mx_dg_segment_get_t;

typedef enum {
    MX_DG_FILTER_CHECK_BOX = 1
} mx_dg_filter_flag_t;

class mx_dg_level_list;
class mx_dg_level;
class mx_dg_quad;
class mx_diagram_area;
class mx_dg_filter;
class mx_dg_level_element;

const int mx_dg_level_key_size = SLS_UINT32;
const int mx_dg_group_element_size = mx_dg_level_key_size;
const int mx_dg_group_key_size = 2 * SLS_INT32;
const int mx_dg_quad_key_size = (SLS_UINT8 + SLS_UINT16 + SLS_UINT32);
const int mx_dg_level_element_size = (mx_dg_quad_key_size + 2 * SLS_INT32);
const int mx_dg_level_garbage_size = (mx_dg_level_key_size);

class mx_dg_quad_key : public mx_serialisable_object {
public:
    static const int maxDepth;
    mx_dg_quad_key();
    mx_dg_quad_key(uint8 id,
        uint16 iq,
        int32 io);
    void next(int iquad, mx_dg_quad_key& key);
    int depthQuad(int atDepth) const;
    uint32 compress();

    inline void setOffset(int io) { offset = io; };
    inline int32 getOffset() const { return offset; };

    inline void setDepth(uint8 id) { depth = id; };
    inline uint8 getDepth() const { return depth; };

    inline void setQuadId(uint16 iq) { quadId = iq; };
    inline uint16 getQuadId() const { return quadId; };

    void serialise(int& err, uint8** buffer);
    void unserialise(int& err, uint8** buffer);
    uint32 get_serialised_size(int& err);

    inline mx_dg_quad_key& operator+=(int32 extension)
    {
        offset += extension;
        return *this;
    };

    inline mx_dg_quad_key operator+(int32 extension) const
    {
        mx_dg_quad_key qk(depth, quadId, offset + extension);
        return qk;
    };

    inline static int size() { return mx_dg_quad_key_size; };

    inline bool isNull() const { return (offset == -1); };

private:
    uint8 depth;
    uint16 quadId;
    int32 offset;
};

/*-------------------------------------------------
 * CLASS: mx_dg_group_key
 *
 * DESCRIPTION:
 *
 *
 */

class mx_dg_group_key : public mx_serialisable_object {
public:
    mx_dg_group_key()
    {
        groupId = 0;
        offset = -1;
    };
    mx_dg_group_key(int32 groupId,
        int32 offset = -1);

    const int groupIdOffset = 0;
    const int groudIdSize = SLS_INT32;
    const int offsetOffset = SLS_INT32;
    const int offsetSize = SLS_INT32;
    const int groupSize = mx_dg_group_key_size;

    mx_dg_group_key& operator=(const mx_dg_group_key& other)
    {
        groupId = other.groupId;
        offset = other.offset;
        return *this;
    }

    void serialise(int& err, uint8** buffer);
    void unserialise(int& err, uint8** buffer);
    uint32 get_serialised_size(int& err);

    inline static int size() { return mx_dg_group_key_size; };

    inline void set(int32 igroupId, int32 ioffset)
    {
        groupId = igroupId;
        offset = ioffset;
    };

    inline void setGroupId(int32 igroupId) { groupId = igroupId; };
    inline int32 getGroupId() const { return groupId; };
    inline void setOffset(int32 ioffset) { offset = ioffset; };
    inline int32 getOffset() const { return offset; };

    inline bool isNull() const { return offset == -1; };

private:
    int32 groupId;
    int32 offset;
};

/*-------------------------------------------------
 * CLASS: mx_dg_level_key
 *
 * DESCRIPTION: A diagram level class. The diagram
 *              is split into a number of levels to
 *              allow drawing order to be controlled.
 *
 */

class mx_dg_level_key : public mx_serialisable_object {
public:
    mx_dg_level_key()
    {
        levelId = 0;
        offset = -1;
    };
    mx_dg_level_key(uint8 levelId,
        int32 offset = -1);

    void serialise(int& err, uint8** buffer);
    void unserialise(int& err, uint8** buffer);
    uint32 get_serialised_size(int& err);

    inline uint32 compress() const
    {
        return levelId + ((offset / mx_dg_level_element_size) << 8);
    };

    inline void uncompress(uint32 geometryId)
    {
        offset = (geometryId >> 8);
        levelId = geometryId - offset;
        offset *= mx_dg_level_element_size;
    };

    inline mx_dg_level_key& operator+=(int32 extension)
    {
        offset += extension;
        return *this;
    };

    inline mx_dg_level_key operator+(int32 extension) const
    {
        mx_dg_level_key lk(levelId, offset + extension);
        return lk;
    };

    inline bool isNull() const { return (offset == -1); };

    inline static int size() { return mx_dg_level_key_size; };

    inline void set(uint8 ilevelId, int32 ioffset)
    {
        levelId = ilevelId;
        offset = ioffset;
    };

    inline void setLevelId(uint8 ilevelId) { levelId = ilevelId; };
    inline uint8 getLevelId() const { return levelId; };
    inline void setOffset(int32 io) { offset = io; };
    inline int32 getOffset() const { return offset; };

private:
    uint8 levelId;
    int32 offset;
};

/*-------------------------------------------------
 * CLASS: mx_dg_level_element
 *
 * DESCRIPTION: A diagram level class. The diagram
 *              is split into a number of levels to
 *              allow drawing order to be controlled.
 *
 */

class mx_dg_level_element : public mx_serialisable_object {
public:
    static const int nextOffset = 0;
    static const int nextSize = SLS_INT32;
    static const int prevOffset = SLS_INT32;
    static const int prevSize = SLS_INT32;
    static const int quadOffset = 2 * SLS_INT32;

    void serialise(int& err, uint8** buffer);
    void unserialise(int& err, uint8** buffer);
    uint32 get_serialised_size(int& err);

    inline static int size() { return mx_dg_level_element_size; };

    inline void setNext(mx_dg_level_key& inext) { next = inext; };
    inline mx_dg_level_key getNext() const { return next; };
    inline void setPrev(mx_dg_level_key& iprev) { prev = iprev; };
    inline mx_dg_level_key getPrev() const { return prev; };

    inline void setQuadKey(const mx_dg_quad_key& iq) { quadKey = iq; };
    inline mx_dg_quad_key getQuadKey() const { return quadKey; };

private:
    mx_dg_level_key next;
    mx_dg_level_key prev;
    mx_dg_quad_key quadKey;
};

/*-------------------------------------------------
 * CLASS: mx_dg_segment
 *
 * DESCRIPTION: Geometry data required for quading
 *
 *
 */

class mx_dg_segment : public mx_serialisable_object {
    friend class mx_dg_quad;
    friend class mx_dg_quad_geom;
    friend class mx_dg_segment_table;
    friend class mx_dg_level_iterator;
    friend class mx_dg_level_garbage_iterator;
    friend class mx_dg_level_draw_iterator;
    friend class mx_dg_select_iterator;

    static const int size = SLS_UINT8 + mx_rect::boxSize + mx_dg_quad_key_size;
    static const int quadKeyOffset = 0;

private:
    mx_dg_segment() { selectGeom = NULL; };
    ~mx_dg_segment() { clear(); };

    void clear()
    {
        if (deleteableGeom)
            delete selectGeom;
        selectGeom = NULL;
        flags &= (~(uint8)MX_DG_SEGMENT_ACTIVE);
    };

    inline bool isActive() const { return (flags & MX_DG_SEGMENT_ACTIVE); };
    inline bool isFiltered() const { return (flags & MX_DG_SEGMENT_FILTERED); };

    void serialise(int& err, uint8** buffer);
    void unserialise(int& err, uint8** buffer);
    uint32 get_serialised_size(int& err);

    void set(uint8 flags,
        bool deleteableGeom,
        mx_geom* geom,
        const mx_rect& box,
        int32 id,
        const mx_dg_quad_key& quadKey);

    bool deleteableGeom;
    uint8 flags;
    mx_geom* selectGeom;
    mx_rect box;
    int32 id;
    mx_dg_quad_key quadKey;
};

/*-------------------------------------------------
 * CLASS: mx_dg_segment_table
 *
 * DESCRIPTION: Geometry data required for quading
 *
 *
 */

class mx_dg_segment_table : public mx_serialisable_object {
    friend class mx_dg_quad_geom;
    friend class mx_dg_quad;
    friend class mx_dg_level;
    friend class mx_dg_iterator;
    friend class mx_dg_level_iterator;
    friend class mx_dg_level_garbage_iterator;
    friend class mx_dg_level_draw_iterator;
    friend class mx_dg_select_iterator;

private:
    mx_dg_segment_table();
    ~mx_dg_segment_table();

    void clear();

    void serialise(int& err, uint8** buffer);
    void unserialise(int& err, uint8** buffer);
    uint32 get_serialised_size(int& err);

    inline mx_geom* getGeom(int is)
    {
        return segments[is].selectGeom;
    }

    void fuseSegments(int& err);

    void addSegmentInfo(uint8 flags,
        bool deleteableGeom,
        mx_geom* igeom,
        const mx_rect& ibox,
        int iid,
        const mx_dg_quad_key& quadKey);

    inline void set(int isegment,
        uint8 flags,
        bool deleteableGeom,
        mx_geom* geom,
        const mx_rect& box,
        int id,
        const mx_dg_quad_key& quadKey)
    {
        segments[isegment].set(flags, deleteableGeom, geom, box, id, quadKey);
    };

    int32 nsegments;
    int32 nallocsegments;
    mx_dg_segment* segments;
};

/*-------------------------------------------------
 * CLASS: mx_dg_quad_geom
 *
 * DESCRIPTION: Geometry data required for quading
 *
 *
 */

class mx_dg_quad_geom : public mx_serialisable_object {
    friend class mx_dg_quad;
    friend class mx_dg_level;
    friend class mx_dg_iterator;
    friend class mx_dg_level_iterator;
    friend class mx_dg_level_garbage_iterator;
    friend class mx_dg_level_draw_iterator;
    friend class mx_dg_select_iterator;

public:
    inline mx_dg_group_key getGroupKey() const { return groupKey; };
    inline mx_geom* getGeom(int isegment = -1)
    {
        mx_geom* retGeom;

        if (isegment == -1) {
            retGeom = hgeom;
        } else {
            retGeom = segmentTable.segments[isegment].selectGeom;
        }

        return retGeom;
    }
    inline mx_dg_level_key getLevelKey() const { return levelKey; };
    inline mx_dg_quad_key getQuadKey() const { return quadKey; };
    inline int32 getSegmentNumber() const { return segmentNumber; };
    inline mx_dg_qgeom_get_t getStage() const { return stage; };
    inline bool isSegment() const { return (flags & MX_DG_QUAD_IS_SEGMENT); };
    inline bool isFirstSegment() const { return (flags & MX_DG_QUAD_FIRST_SEGMENT); };
    inline bool isSubsequentSegment() const { return isSegment() && (!isFirstSegment()); };
    inline mx_rect getBox() const { return box; };
    inline void setMergeGeometries(bool img) { mergeGeometries = img; };

    inline bool needsLevel() const
    {
        return (!isSegment()) || (isFirstSegment());
    };

    mx_geom* extractGeom(int isegment = -1);

    ~mx_dg_quad_geom();

    static const int geomLevelOffsetOffset = SLS_INT32;
    static const int geomLevelOffsetSize = SLS_INT32;
    static const int geomFlagOffset = (2 * SLS_INT32) + (SLS_ENUM);
    static const int geomFlagSize = SLS_UINT8;
    static const int levelOffset = geomFlagOffset + geomFlagSize;
    static const int levelSize = mx_dg_level_key_size;
    static const int groupOffset = levelOffset + levelSize;
    static const int groupSize = mx_dg_group_key_size;
    static const int segmentTableSegmentOffset = groupOffset + groupSize;

private:
    mx_dg_quad_geom();

    void init();
    void initIntern();

    inline int getGeomDataSize() { return geomDataSize; };

    void serialise(int& err, uint8** buffer);
    void unserialise(int& err, uint8** buffer);
    uint32 get_serialised_size(int& err);

    void setQuadGeomBuffer(int newSize);
    void setGeomBuffer(int newSize);

    unsigned char getFlags() { return flags; };

    inline bool isActive() const { return (flags & MX_DG_QUAD_ACTIVE); };
    inline bool hasBox() const { return (flags & MX_DG_QUAD_STORE_BOX); };

    inline void setGeom(mx_geom* igeom, bool ideleteableGeom)
    {
        clearGeometry(TRUE);
        hgeom = igeom;
        deleteableGeom = ideleteableGeom;
    };

    inline void setBox(int& err);
    inline void setStage(mx_dg_qgeom_get_t istage)
    {
        if (istage > stage)
            stage = istage;
    };

    inline int32 writeSize() const { return SLS_INT32 + geomDataSize + quadGeomDataSize; };

    inline void setLevelKey(const mx_dg_level_key& lk) { levelKey = lk; };
    inline void setQuadKey(const mx_dg_quad_key& qk) { quadKey = qk; };
    inline void setGroupKey(const mx_dg_group_key& gk) { groupKey = gk; };

    inline void setGroupInformation(bool igi) { groupInformation = igi; };
    void createGeometry(int& err);

    inline bool isSegmentHeader() const
    {
        return (isSegment() && isFirstSegment());
    };

    mx_geom* createEmptyGeometry();

    void clearGeometry(bool mainGeomOnly = FALSE);

    void setGeometry(int& err,
        mx_geom& igeom,
        const mx_dg_level_key& ilevelKey,
        bool isGrouped,
        const mx_dg_group_key& igroupKey,
        int isegmentNumber);

    bool extractFromBlob(int& err,
        mx_diagram_area* da,
        const mx_dg_level_key& levelKey,
        const mx_dg_quad_key& quadKey,
        mx_dg_qgeom_get_t qgeomGetType,
        bool allowInactive,
        const mx_dg_filter* const filter = NULL);

    bool extractFromBlobIntern(int& err,
        mx_diagram_area* da,
        const mx_dg_level_key& ilevelKey,
        const mx_dg_quad_key& iquadKey,
        mx_dg_qgeom_get_t qgeomGetType,
        bool allowInactive,
        const mx_dg_filter* const filter);

    void extractGeomFromBlob(int& err,
        mx_diagram_area* da,
        char* idString,
        int32 currentOffset,
        mx_dg_qgeom_get_t qgeomGetType);

    void segmentGeometry(int& err,
        mx_geom& geom);

    void fuseSegments(int& err);

    unsigned char* geomBuffer;
    int geomBufferSize;
    unsigned char* quadGeomBuffer;
    int quadGeomBufferSize;

    bool deleteableGeom;
    mx_geom* hgeom;
    int32 geomDataSize;
    int32 quadGeomDataSize;

    mx_dg_quad_key quadKey;
    mx_dg_level_key levelKey;

    unsigned char flags;
    MX_GEOM_TYPE type;

    mx_rect box;

    mx_dg_group_key groupKey;

    mx_dg_qgeom_get_t stage;

    bool groupInformation;

    // segment information
    mx_dg_segment_table segmentTable;
    int32 segmentNumber;
    bool mergeGeometries;

    inline mx_dg_segment_get_t getSegmentGetType() const
    {
        return segmentGetType;
    }

    inline void setSegmentGetType(mx_dg_segment_get_t isgt) { segmentGetType = isgt; };

    mx_dg_segment_get_t segmentGetType;
};

/*-------------------------------------------------
 * CLASS: mx_dg_filter
 *
 * DESCRIPTION: A class to filter out geometries
 *              based on the quad geometry
 *
 */

class mx_dg_filter {
public:
    mx_dg_filter();

    bool filterGeometry(int& err,
        mx_dg_quad_geom& quadGeom) const;

    bool testFilterBox(const mx_rect& r) const;

    void setFilterBox(const mx_rect& irect);

    inline const mx_rect& getBox() { return filterBox; };
    inline bool testFilter(mx_dg_filter_flag_t test)
    {
        return (test & flags);
    };

private:
    uint32 flags;
    mx_rect filterBox;
};

/*-------------------------------------------------
 * CLASS: mx_dg_iterator
 *
 * DESCRIPTION: An iterator class for
 *
 *
 */

class mx_dg_iterator {
public:
    mx_dg_iterator();

    inline void setAreaOfInterest(const mx_rect& irect)
    {
        useFilter = TRUE;
        filter.setFilterBox(irect);
    };

    inline bool getGroupInformation() const { return groupInformation; };
    inline void setGroupInformation(bool igi)
    {
        groupInformation = igi;
        quadGeom.setGroupInformation(igi);
    };

    inline void setSegmentGetType(mx_dg_segment_get_t isgt)
    {
        quadGeom.setSegmentGetType(isgt);
    }

    inline void setMergeGeometries(bool img)
    {
        quadGeom.setMergeGeometries(img);
    }

    inline bool getQuadInformation() const { return quadInformation; };
    inline void setQuadInformation(bool igi) { quadInformation = igi; };

    inline bool getAllowInactive() const { return allowInactive; };
    inline void setAllowInactive(bool igi) { allowInactive = igi; };

    inline void setGetType(mx_dg_qgeom_get_t igt) { qgeomGetType = igt; };
    inline mx_dg_qgeom_get_t getGetType() { return qgeomGetType; };

    virtual void start(int& err,
        mx_diagram_area* ida,
        const mx_dg_level_key* const ilevelKey = NULL,
        const mx_dg_quad_key* const iquadKey = NULL);

    void process(int& err);

    mx_dg_quad_geom& getQuadGeom() { return quadGeom; };

    bool nextGeometry(int& err,
        bool& skippedSome);

protected:
    inline const mx_dg_filter* const getFilter()
    {
        return (useFilter ? &filter : (mx_dg_filter*)NULL);
    };

    // the function to call when a geometry has been found

    virtual void processFunction(int& err,
        mx_dg_quad_geom& quadGeom,
        bool& advance,
        bool& done,
        bool skippedSome)
    {
        err = MX_ERROR_OK;
    };

    virtual void end(int& err) { err = MX_ERROR_OK; };

    virtual bool nextGeometryIntern(int& err,
        bool doingProcess,
        bool& reachedEnd,
        bool& skippedSome);

    virtual void advanceKey(int& err);

    bool advance(int& err,
        bool doingProcess,
        bool found,
        bool& skippedSome);

    inline bool intersects(const mx_rect& extent)
    {
        return (filter.testFilter(MX_DG_FILTER_CHECK_BOX)
                ? filter.getBox().intersects(extent)
                : TRUE);
    };

    bool useFilter;
    mx_dg_filter filter;

    bool useAreaOfInterest;
    bool advanceItem;

    mx_diagram_area* da;
    mx_dg_level* currentLevel;
    mx_dg_quad* currentQuad;
    mx_dg_quad_key currentQuadKey;

    mx_dg_quad_geom quadGeom;

    mx_dg_qgeom_get_t qgeomGetType;

    bool allowInactive;
    bool groupInformation;
    bool quadInformation;
    bool singleLevelIteration;
    bool singleQuadIteration;
};
/*-------------------------------------------------
 * CLASS: mx_dg_quad
 *
 * DESCRIPTION: A quad class for the diagram area
 *
 *
 */

class mx_dg_quad : public mx_serialisable_object {
    friend class mx_dg_quad_geom;
    friend class mx_dg_level;
    friend class mx_diagram_area;
    friend class mx_dg_iterator;
    friend class mx_dg_group_iterator;
    friend class mx_dg_level_iterator;
    friend class mx_dg_level_draw_iterator;
    friend class mx_dg_level_garbage_iterator;

private:
    mx_dg_quad();
    ~mx_dg_quad();

    void serialise(int& err, uint8** buffer);
    void unserialise(int& err, uint8** buffer);
    uint32 get_serialised_size(int& err);

    static void quadExtent(mx_point& iCentre,
        mx_point& iSize,
        mx_rect& irect);

    static void nextQuadCoords(mx_point iCentre,
        mx_point iSize,
        mx_point& nCentre,
        mx_point& nSize,
        int quadNo);

    void garbageCollect(int& err);

    int boxToQuad(const mx_rect& box);

    void deleteGeometry(int& err,
        const mx_dg_level_key& levelKey,
        const mx_dg_quad_key& quadKey);

    void undeleteGeometry(int& err,
        const mx_dg_level_key& levelKey,
        const mx_dg_quad_key& quadKey);

    mx_dg_quad_key addGeometry(int& err,
        mx_dg_quad_geom& quadGeom);

    void set(int& err,
        bool newQuad,
        mx_diagram_area* da,
        mx_dg_quad* parent,
        const mx_dg_level_key& levelKey,
        const mx_point& icentre,
        const mx_point& isize,
        const mx_dg_quad_key& key);

    mx_geom* getGeometry(int& err,
        mx_dg_quad_key& key);

    void copyData(int& err,
        mx_dg_quad& inQuad);

    inline bool lowerQuadsExist() { return (lowerQuads != NULL); };

    inline static mx_dg_quad_geom& getWorkQuadGeom() { return workQuadGeom; };

    static char* squadId(const mx_dg_level_key& ilevelKey,
        const mx_dg_quad_key& key);

    inline mx_point getCentre() const { return centre; };
    inline mx_point getSize() const { return size; };

    inline mx_dg_quad_key getHwm() const { return hwm; };

    inline mx_diagram_area* getDiagramArea() const { return da; };

    // next quad in an iteration
    mx_dg_quad* nextQuad();

    void setTableEntry(int& err,
        const mx_dg_quad_key& quadKey,
        int isegment,
        const mx_dg_quad_key& segmentQuadKey);

    uint8 getFlags(int* err,
        const mx_dg_quad_key& quadKey);

    void setFlags(int* err,
        const mx_dg_quad_key& quadKey,
        uint8 flags);

    mx_dg_group_key getGroup(int& err,
        const mx_dg_quad_key& quadKey);
    void setGroup(int& err,
        const mx_dg_quad_key& quadKey,
        const mx_dg_group_key& groupKey);

    mx_dg_level_key getLevel(int& err,
        const mx_dg_quad_key& quadKey);

    void setLevel(int& err,
        const mx_dg_quad_key& quadKey,
        const mx_dg_level_key& levelKey);

    void setFlags(int& err,
        const mx_dg_quad_key& quadKey,
        uint8 flags);

    uint8 getFlags(int& err,
        const mx_dg_quad_key& quadKey);

    inline mx_dg_quad* getSubQuad(int subQuad) const
    {
        return (lowerQuads == NULL) ? ((mx_dg_quad*)NULL) : lowerQuads + subQuad;
    }

    void idStr();

    void reQuad(int& err,
        bool properRequad);

    // the highest byte used in the blob for
    // the quad

    mx_dg_quad_key hwm;
    mx_dg_level_key levelKey;

    mx_diagram_area* da;

    // The centre and size of the quad
    mx_point centre;
    mx_point size;
    mx_rect extent;

    // The lower quads
    mx_dg_quad* lowerQuads;
    char* idString;

    // parent
    mx_dg_quad* parent;

    // a quad geometry for moving data around
    static mx_dg_quad_geom workQuadGeom;

    void addNewGeometry(int& err,
        const mx_dg_quad_geom& quadData);

    void writeGeometry(int& err,
        const mx_dg_quad_key& key,
        const mx_dg_quad_geom& quadData);
};

/*-------------------------------------------------
 * CLASS: mx_dg_level_iterator
 *
 * DESCRIPTION: A diagram level class. The diagram
 *              is split into a number of levels to
 *              allow drawing order to be controlled.
 *
 */

class mx_dg_level_iterator : public mx_dg_iterator {
public:
    // start an iteration

    void start(int& err,
        mx_diagram_area* ida,
        const mx_dg_level_key* const ilevelKey = NULL,
        const mx_dg_quad_key* const iquadKey = NULL);

private:
    virtual bool nextGeometryIntern(int& err,
        bool doingProcess,
        bool& reachedEnd,
        bool& skippedSome);

    void advanceKey(int& err);

    bool testAreaOfInterest(int& err);

    // a sensible number of quads in a diagram
    static int numExtentIdAnswers;
    static int numAllocExtentIdAnswers;
    static uint8* extentIdAnswers;
    static std::map<uint32, uint8 *> m_extentLookUp;

    mx_dg_level_key levelKey;
};

/*-------------------------------------------------
 * CLASS: mx_dg_level_draw_iterator
 *
 * DESCRIPTION: A diagram level class. The diagram
 *              is split into a number of levels to
 *              allow drawing order to be controlled.
 *
 */

class mx_dg_level_draw_iterator : public mx_dg_level_iterator {
public:
    void processFunction(int& err,
        mx_dg_quad_geom& quadGeom,
        bool& advance,
        bool& done,
        bool skippedSome);

    inline void setStyleDevice(mx_style_device* id) { device = id; };

private:
    mx_style_device* device;
};

/*-------------------------------------------------
 * CLASS: mx_dg_level_garbage_iterator
 *
 * DESCRIPTION: A diagram level class. The diagram
 *              is split into a number of levels to
 *              allow drawing order to be controlled.
 *
 */

class mx_dg_level_garbage_iterator : public mx_dg_level_iterator {
public:
    void start(int& err,
        mx_diagram_area* ida,
        const mx_dg_level_key* const ilevelKey = NULL,
        const mx_dg_quad_key* const iquadKey = NULL);

    void processFunction(int& err,
        mx_dg_quad_geom& quadGeom,
        bool& advance,
        bool& done,
        bool skippedSome);

    void end(int& err);

private:
    int nlevelGeoms;
    char idString[50];
    mx_dg_level_key levelKey;
};

/*-------------------------------------------------
 * CLASS: mx_dg_group_element
 *
 * DESCRIPTION:
 *
 *
 */

class mx_dg_group_element : public mx_serialisable_object {
public:
    mx_dg_group_element()
    {
        geom = NULL;
        stage = MX_DG_GROUP_GET_NONE;
    };
    mx_dg_group_element(const mx_dg_level_key& levelKey);
    ~mx_dg_group_element()
    {
        if (deleteableGeom)
            delete geom;
    };
    void serialise(int& err, uint8** buffer);
    void unserialise(int& err, uint8** buffer);
    uint32 get_serialised_size(int& err);

    inline void setLevelId(uint8 ilevelId) { levelKey.setLevelId(ilevelId); };
    inline uint8 getLevelId() const { return levelKey.getLevelId(); };
    inline void setOffset(int32 iOffset) { levelKey.setOffset(iOffset); };
    inline int32 getOffset() const { return levelKey.getOffset(); };

    inline void setGeom(mx_geom* igeom,
        bool idel)
    {
        if (deleteableGeom)
            delete geom;
        geom = igeom;
        deleteableGeom = idel;
        stage = MX_DG_GROUP_GET_GEOM;
    };
    inline mx_geom* getGeom() const { return geom; };
    inline mx_geom* extractGeom()
    {
        geom = NULL;
        return geom;
    };

    inline void setQuadKey(const mx_dg_quad_key& iq)
    {
        quadKey = iq;
        stage = MX_DG_GROUP_GET_QUAD;
    };
    inline mx_dg_quad_key getQuadKey() const { return quadKey; };
    inline mx_dg_level_key getLevelKey() const { return levelKey; };
    inline void setLevelKey(const mx_dg_level_key& ilk)
    {
        levelKey = ilk;
        stage = MX_DG_GROUP_GET_LEVEL;
    };

    inline MX_DG_GROUP_GET_STAGE getStage() const { return stage; };

    static char* idGroupStr(const mx_dg_group_key& groupKey);
    static int size() { return mx_dg_group_element_size; };

private:
    mx_dg_level_key levelKey;
    mx_dg_quad_key quadKey;
    bool deleteableGeom;
    mx_geom* geom;
    MX_DG_GROUP_GET_STAGE stage;
};

/*-------------------------------------------------
 * CLASS: mx_dg_level
 *
 * DESCRIPTION: A diagram level class. The diagram
 *              is split into a number of levels to
 *              allow drawing order to be controlled.
 *
 */

class mx_dg_level : public mx_serialisable_object {
    friend class mx_dg_level_list;
    friend class mx_dg_quad;
    friend class mx_dg_quad_geom;
    friend class mx_diagram_area;
    friend class mx_dg_iterator;
    friend class mx_dg_group_iterator;
    friend class mx_dg_level_iterator;
    friend class mx_dg_level_draw_iterator;
    friend class mx_dg_level_garbage_iterator;

private:
    mx_dg_level();
    ~mx_dg_level();

    void set(int& err,
        mx_diagram_area* ida,
        const char* name,
        const mx_dg_level_key& hwmKey,
        bool isNew);

    mx_dg_level_key addGeometry(int& err, mx_geom& geom);

    void deleteGeometry(int& err,
        const mx_dg_level_key& levelKey,
        mx_dg_level_key& nextLevelKey,
        mx_dg_level_key& prevLevelKey);

    void undeleteGeometry(int& err,
        const mx_dg_level_key& levelKey,
        const mx_dg_level_key& nextLevelKey,
        const mx_dg_level_key& prevLevelKey);

    inline mx_diagram_area* getDiagramArea() const { return da; };
    inline mx_dg_level_key getHwm() const { return hwm; };
    inline mx_dg_level_key getTail() const { return tail; };
    inline mx_dg_level_key getHead() const { return head; };

    void draw(int& err,
        mx_style_device* devive,
        const mx_rect& r);

    void serialise(int& err, uint8** buffer);
    void unserialise(int& err, uint8** buffer);
    uint32 get_serialised_size(int& err);

    void copyData(int& err,
        mx_dg_level& inlevel);
    inline mx_dg_quad* getTopQuad() { return &topQuad; };

    bool getGeometry(int& err,
        bool doExtract,
        const mx_dg_quad_key& key,
        mx_geom** geom);

    inline bool getActive() const { return isActive; };
    inline void setActive(bool ia) { isActive = ia; };

    void garbageCollect(int& err);

    mx_dg_level_key hwm;
    mx_dg_level_key head;
    mx_dg_level_key tail;

    bool isActive;

    void idStr();

    mx_diagram_area* da;

    mx_dg_quad topQuad;
    char* idString;

    char* name;
    bool noGarbageCollectNeeded;

    void setElement(int& err,
        const mx_dg_level_key& key,
        const mx_dg_level_element& element);

    void setQuad(int& err,
        const mx_dg_level_key& key,
        const mx_dg_quad_key& quadKey);

    void setNext(int& err,
        const mx_dg_level_key& key,
        const mx_dg_level_key& inext);

    void setPrev(int& err,
        const mx_dg_level_key& key,
        const mx_dg_level_key& iprev);

    mx_dg_quad_key getQuad(int& err,
        const mx_dg_level_key& key);

    mx_dg_level_key getNext(int& err,
        const mx_dg_level_key& key);

    mx_dg_level_key getPrev(int& err,
        const mx_dg_level_key& key);

    mx_dg_group_key getGroup(int& err,
        const mx_dg_level_key& key);

    void quadIdToExtent(const mx_dg_quad_key& key,
        mx_rect& orect);

    void addNewGeometry(int& err,
        const mx_dg_level_key& levelKey,
        const mx_dg_quad_key& quadKey);

    void requadGeometryLevel(int& err,
        const mx_dg_level_key& levelKey,
        const mx_dg_quad_key& newKey)
    {
        setQuad(err, levelKey, newKey);
    };

    inline void requadGeometryQuad(int& err,
        const mx_dg_level_key& levelKey,
        const mx_dg_quad_key& newKey)
    {
        setQuad(err, levelKey, newKey);
    };

    inline void relevelGeometry(int& err,
        const mx_dg_level_key& levelKey,
        const mx_dg_quad_key& quadKey)
    {
        addNewGeometry(err, levelKey, quadKey);
    };
};

/*-------------------------------------------------
 * CLASS: mx_dg_group_iterator
 *
 * DESCRIPTION:
 *
 *
 */

class mx_dg_group_iterator {
public:
    mx_dg_group_iterator();
    ~mx_dg_group_iterator();
    void start(int& err,
        mx_diagram_area* area,
        const mx_dg_group_key& groupKey,
        MX_DG_GROUP_GET_STAGE stage = MX_DG_GROUP_GET_GEOM);
    bool next(int& err);

    inline mx_dg_group_element getGroupElement() const { return element; };

    inline void setDoExtract(bool ide) { doExtract = ide; };

private:
    mx_diagram_area* area;
    mx_dg_group_key groupKey;
    uint32 currentOffset;
    mx_dg_group_element element;
    MX_DG_GROUP_GET_STAGE stage;
    uint32 topOffset;
    bool doExtract;
};

/*-------------------------------------------------
 * CLASS: mx_diagram_area
 *
 * DESCRIPTION:
 *
 *
 */

class mx_dg_level_list {
public:
    mx_dg_level_list() { level = NULL; };
    ~mx_dg_level_list() { delete level; };

    inline mx_dg_level_key getPrev() const { return prev; };
    inline void setPrev(const mx_dg_level_key& ip) { prev = ip; };
    inline mx_dg_level_key getNext() const { return next; };
    inline void setNext(const mx_dg_level_key& ip) { next = ip; };
    inline mx_dg_level* getLevel() const { return level; };
    inline void setLevel(mx_dg_level* ip) { level = ip; };

private:
    mx_dg_level* level;
    mx_dg_level_key prev;
    mx_dg_level_key next;
};

/*-------------------------------------------------
 * CLASS: mx_diagram_area
 *
 * DESCRIPTION:
 *
 *
 */

class mx_diagram_area : public mx_area {
    friend class mx_dg_level;
    friend class mx_dg_quad;
    friend class mx_dg_iterator;
    friend class mx_dg_group_iterator;
    friend class mx_dg_level_iterator;
    friend class mx_dg_level_draw_iterator;
    friend class mx_dg_level_garbage_iterator;
    friend class mx_dg_quad_geom;

    MX_RTTI(mx_diagram_area_class_e)

public:
    mx_diagram_area(uint32 docid, mx_paragraph_style* s, bool is_controlled = FALSE);
    mx_diagram_area(uint32 docid, uint32 oid, bool is_controlled = FALSE);
    mx_diagram_area(const mx_diagram_area& a);
    virtual ~mx_diagram_area();

    virtual mx_area* duplicate();

    virtual void serialise(int& err, uint8** buffer);
    virtual void unserialise(int& err, uint8** buffer);
    virtual uint32 get_serialised_size(int& err);

    virtual bool is_scalable() const { return TRUE; };
    virtual bool is_croppable() const { return TRUE; };
    virtual bool is_flowable() { return FALSE; };

    virtual void crop(int& err, const mx_rect& r);

    virtual void scale(int& err, mx_point& new_size);

    // document about to be committed  - put attributes into blobs etc
    virtual void serialise_attributes(int& err);

    // not part of the required stuff for a db_object but a sensible extra
    void unserialise_attributes(int& err);

    virtual void uncache(int& err);
    virtual uint32 memory_size(int& err);

    mx_dg_level_key addLevel(int& err, const char* name);

    mx_dg_level_key addGeometry(int& err,
        const mx_dg_level_key& levelKey,
        mx_geom& geom);

    void deleteGeometry(int& err,
        const mx_dg_level_key& levelKey,
        mx_dg_level_key& nextLevelKey,
        mx_dg_level_key& prevLevelKey);

    void undeleteGeometry(int& err,
        const mx_dg_level_key& levelKey,
        const mx_dg_level_key& nextLevelKey,
        const mx_dg_level_key& prevLevelKey);

    inline int getQuant() const { return quant; };
    inline mx_all_styles* getAllStyles() { return &styles; };
    inline mx_style_device* getStyleDevice() { return &styleDevice; };
    inline mx_point getQuadSize() const { return quadSize; };
    inline mx_point getQuadCentre() const { return quadCentre; };

    void draw(int& err,
        mx_device* device,
        mx_doc_rect_t& r,
        int firstLevel = 0);

    inline double getDrawAddOn() const { return drawAddOn; };

    // group type things

    mx_dg_group_key createGroup(int& err);

    void deleteGroup(int& err,
        const mx_dg_group_key& groupKey);

    void addToGroup(int& err,
        const mx_dg_level_key& levelKey,
        const mx_dg_group_key& groupKey);

    void deleteFromGroup(int& err,
        const mx_dg_level_key& levelKey);

    void initialiseQuadSize(const mx_point& iquadSize);

private:
    // The diagram consists of a number of levels

    void initLevels();

    mx_all_styles styles;

    // levels
    static const int maxLevels = 256;

    mx_dg_level_list* levelLists[maxLevels];
    mx_dg_level_key tail;

    // point of centre and qaudSize
    mx_point quadSize;
    mx_point quadCentre;
    // quantiasation for the area
    int32 quant;

    // the style device for drawing in the area
    mx_style_device styleDevice;

    double drawAddOn;

    mx_dg_group_key nextGroupKey;

    void singleGroupGarbageCollect(int& err,
        const mx_dg_group_key& groupKey);

    void setQuadTableEntry(int& err,
        const mx_dg_quad_geom& quadGeom);

    void addNewGeometry(int& err,
        const mx_dg_quad_geom& quadGeom);

    mx_dg_quad* getQuad(const mx_dg_level_key& levelKey,
        const mx_dg_quad_key& quadKey) const;

    inline mx_dg_level* getLevel(const mx_dg_level_key& levelKey) const
    {
        if (levelKey.isNull())
            return NULL;
        mx_dg_level_list* tl = levelLists[levelKey.getLevelId()];
        return (tl == NULL) ? ((mx_dg_level*)NULL) : tl->getLevel();
    };

    inline mx_dg_level* getFirstLevel() const
    {
        return getLevel(tail);
    }

    inline mx_dg_level* getNextLevel(const mx_dg_level_key& levelKey) const
    {
        return getLevel(getNextLevelKey(levelKey));
    }

    mx_dg_level_key getNextLevelKey(const mx_dg_level_key& levelKey) const;

    void setGroupElement(int& err,
        const mx_dg_group_key& groupKey,
        const mx_dg_group_element& groupElement);

    mx_dg_group_element getGroupElement(int& err,
        const mx_dg_group_key& groupKey);

    inline static const char* areaIndex() { return "dgindex"; };

    inline void setQuadSize(mx_point& p) { quadSize = p; };
    inline void setQuadCentre(mx_point& p) { quadCentre = p; };

    mx_dg_quad* quadFromLevel(int& err,
        const mx_dg_level_key& levelKey,
        mx_dg_quad_key& quadKey);
};
