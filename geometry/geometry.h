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
 * MODULE : geometry.h
 *
 * AUTHOR : David Miller
 *
 * This file
 *
 * DESCRIPTION: Basic geometry descriptions
 * Module mx_geom.h
 *
 *
 *
 */

#ifndef MX_GEOM_H
#define MX_GEOM_H

#include <mx.h>

#include <math.h>
#include <mx_sl_obj.h>
#include <stdlib.h>
#include <string.h>

/* Forward definition */
class mx_point;
class mx_polypoint;
class mx_ipolypoint;
class mx_rect;
class mx_snap_info;
class mx_irect;
class mx_affine;
class mx_rotation;
class mx_geom;
class mx_angle;

#define GABS(x) ((x) > 0 ? (x) : -(x))
#define GMAX(x, y) ((x) > (y) ? (x) : (y))
#define GMIN(x, y) ((x) < (y) ? (x) : (y))
#define GNINT(x) ((x) > 0 ? ((x) + 0.5) : ((x) - 0.5))

#define GEOM_SMALL 0.000001
#define PITWO (2 * PI)
#define PITHREE (3 * PI)
#define PIFOUR (4 * PI)
#define RADTODEG (180.0 / PI)
#define DEGTORAD (PI / 180.0)

typedef enum {
    MX_COORD_TL,
    MX_COORD_BR,
    MX_COORD_TR,
    MX_COORD_BL,
    MX_COORD_C
} MX_COORD_TYPE;

typedef enum {
    GEOM_INTERSECT_NONE,
    GEOM_INTERSECT_POINT,
    GEOM_INTERSECT_LINE,
    GEOM_INTERSECT_AREA,
    GEOM_INTERSECT_CONTAINED
} GEOM_INTERSECT_TYPE;

typedef enum {
    ARC_ARC = 1,
    ARC_CHORD,
    ARC_SECTOR,
    ARC_NONE
} ARC_TYPE;

typedef enum {
    POLY_LINE = 1,
    POLY_POINT,
    POLY_SEGMENT,
    POLY_CSEGMENT
} POLYPOINT_TYPE;

#define MX_GEOM_SNAP_NONE 0

extern mx_snap_info defaultSnapInfo;
extern mx_angle defaultAngle;
extern mx_point defaultPoint;

/*-------------------------------------------------
 * CLASS: mx_ipoint
 *
 * DESCRIPTION: Basic point with integer coordinates
 * This class exists oon its own as a lightweight
 * class (no virtual functions)
 *
 * The coordinates are considered in the Maxwell coordinate
 * system e.g. (0,0) is the top left, (1,1) to the bottom
 * right
 */

class mx_ipoint {
public:
    int32 x;
    int32 y;

    // Constructors - including one from an
    // mx_ipoint which allows point and mx_ipoint to
    // be automatically converted

    mx_ipoint();
    mx_ipoint(int32 x, int32 y);
    mx_ipoint(const mx_point& p);

    mx_point convert();

    // Basic operators

    inline mx_ipoint& operator+=(const mx_ipoint& p)
    {
        x += p.x;
        y += p.y;
        return *this;
    };

    inline mx_ipoint operator-()
    {
        mx_ipoint ip(-x, -y);
        return ip;
    };

    inline mx_ipoint operator+(const mx_ipoint& p2)
    {
        mx_ipoint pt(this->x + p2.x, this->y + p2.y);
        return pt;
    };

    inline mx_ipoint operator-(const mx_ipoint& p2) const
    {
        mx_ipoint pt(x - p2.x, y - p2.y);

        return pt;
    };

    inline mx_ipoint& operator-=(const mx_ipoint& p)
    {
        x -= p.x;
        y -= p.y;
        return *this;
    };

    inline mx_ipoint& operator*=(double f)
    {
        /*
        double temp ;
        temp = x*f ;
        x    = (int32) GNINT(temp) ;

        temp = y*f ;
        y    = (int32) GNINT(temp) ;
        */
        return *this;
    };

    inline mx_ipoint operator/=(double f)
    {
        double temp;

        temp = x / f;
        x = (int32)GNINT(temp);

        temp = y / f;
        y = (int32)GNINT(temp);

        return *this;
    };

    inline friend bool operator==(const mx_ipoint& p1, const mx_ipoint& p2)
    {
        return (p1.x == p2.x) && (p1.y == p2.y);
    };

    inline friend bool operator!=(const mx_ipoint& p1, const mx_ipoint& p2)
    {
        return (p1.x != p2.x) || (p1.y != p2.y);
    };

    mx_point operator*(double f);
    mx_point operator/(double f);

    mx_ipoint operator*(int s);

    // go from (x,y) to (-y,x) i.e.
    // perpendicular

    inline mx_ipoint flip()
    {
        mx_ipoint p(-y, x);
        return p;
    };

    // distance from this point to
    // another point
    // Two metrics are used -  the normal
    // euclidean and the quicker modulus (L1) metric
    double dist(const mx_point& p) const;
    double dist1(const mx_point& p) const;

protected:
private:
};

typedef enum {
    MX_POINT_SNAP_NONE = MX_GEOM_SNAP_NONE,
    MX_POINT_SNAP_NO_COMPONENTS,
    MX_POINT_SNAP_WHOLE
} MX_POINT_SNAP;

/*-------------------------------------------------
 * CLASS: mx_point
 *
 * DESCRIPTION: Basic point with double coordinates
 *
 * This class exists oon its own as a lightweight
 * class (no virtual functions)
 *
 * The coordinates are considered in the Maxwell coordinate
 * system e.g. (0,0) is the top left, (1,1) to the bottom
 * right
 */

class mx_point : public mx_serialisable_object {
public:
    double x;
    double y;

    // Constructors - including one from an
    // mx_ipoint which allows mx_point and mx_ipoint to
    // be automatically converted

    mx_point();
    mx_point(double x, double y);
    mx_point(const mx_ipoint& ip);

    // Basic operators

    mx_point rotate(const mx_rotation& rotation) const;
    mx_point affine(mx_affine& aff);

    inline mx_point& operator+=(const mx_point& p)
    {
        x += p.x;
        y += p.y;
        return *this;
    };
    inline mx_point& operator-=(const mx_point& p)
    {
        x -= p.x;
        y -= p.y;
        return *this;
    };

    inline int wholeSelectCount() { return MX_POINT_SNAP_WHOLE; };

    bool mergeSelectCount(int nselectCounts,
        int* selectCounts);

    // inline friend mx_point operator-(const mx_point p1, const mx_point p2)
    //{
    // mx_point res = p1;
    // res.x -= p2.x;
    // res.y -= p2.y;
    // return res;
    //}

    inline mx_point& operator*=(double f)
    {
        x *= f;
        y *= f;
        return *this;
    };
    inline mx_point& operator/=(double f)
    {
        x /= f;
        y /= f;
        return *this;
    };

    inline mx_point operator-()
    {
        mx_point pt(-x, -y);
        return pt;
    };
    inline mx_point operator+(const mx_point& p2) const
    {
        mx_point pt(x + p2.x, y + p2.y);
        return pt;
    };

    inline mx_point operator-(const mx_point& p2) const
    {
        mx_point pt(x - p2.x, y - p2.y);
        return pt;
    };

    friend mx_point operator*(const mx_point& p, double f)
    {
        mx_point pt(p.x * f, p.y * f);
        return pt;
    };

    friend mx_point operator/(const mx_point& p, double f)
    {
        mx_point pt(p.x / f, p.y / f);
        return pt;
    };

    inline double operator*(const mx_point& p2)
    {
        return (this->x * p2.x) + (this->y * p2.y);
    };

    inline double operator^(const mx_point& p2)
    {
        return (this->x * p2.y) - (this->y * p2.x);
    };

    inline friend bool operator==(const mx_point& p1, const mx_point& p2)
    {
        return (MX_FLOAT_EQ(p1.x, p2.x) && MX_FLOAT_EQ(p1.y, p2.y));
    };

    inline friend bool operator!=(const mx_point& p1, const mx_point& p2)
    {
        return (!MX_FLOAT_EQ(p1.x, p2.x) || !MX_FLOAT_EQ(p1.y, p2.y));
    };

    inline bool operator>(const mx_point& p2)
    {
        if (this->y > p2.y) {
            return TRUE;
        } else if (this->y == p2.y) {
            if (this->x > p2.x) {
                return TRUE;
            }
        }
        return FALSE;
    };

    inline bool operator>=(const mx_point& p2)
    {
        if (*this == p2)
            return TRUE;

        return (*this > p2);
    };

    inline bool operator<(const mx_point& p2)
    {
        if (this->y < p2.y) {
            return TRUE;
        } else if (this->y == p2.y) {
            if (this->x < p2.x) {
                return TRUE;
            }
        }
        return FALSE;
    };

    inline bool operator<=(const mx_point& p2)
    {
        if (*this == p2)
            return TRUE;

        return (*this < p2);
    };

    inline mx_point spanpoint(mx_point p2, double fac)
    {
        mx_point p = (*this);
        mx_point p3 = p2 * fac;

        p *= (1.0 - fac);
        p += p3;

        return p;
    };

    // go from (x,y) to (-y,x) i.e.
    // perpendicular

    inline mx_point flip() const
    {
        mx_point p(-y, x);
        return p;
    };
    mx_point uspan(double* length = NULL) const;
    inline double length2() const { return (x * x + y * y); };
    inline double length() const { return sqrt(length2()); };

    // distance from this point to
    // another point

    inline double dist2(const mx_point& p2) const
    {
        return (x - p2.x) * (x - p2.x) + (y - p2.y) * (y - p2.y);
    };

    inline double dist(const mx_point& p2) const
    {
        return sqrt(dist2(p2));
    };

    inline double dist1(const mx_point& p2) const
    {
        return GABS(this->x - p2.x) + GABS(this->y - p2.y);
    };

    double dist2snap(const mx_point& p,
        mx_snap_info& snapInfo = defaultSnapInfo,
        double tolerance = GEOM_SMALL) const;

    void serialise(int& err, uint8** buffer);
    void unserialise(int& err, uint8** buffer);
    uint32 get_serialised_size(int& err);

protected:
private:
};

/*-------------------------------------------------
 * CLASS: mx_angle
 *
 * DESCRIPTION: Information about angles
 *
 *
 */

class mx_angle {
public:
    mx_angle(double iangle = 0);
    mx_angle(const mx_angle& iangle);
    void set(double iangle = 0);
    void set(const mx_angle& iangle);
    int quadrant();
    static mx_point squadrantRange(int quadrant);
    mx_point quadrantRange();
    inline double a() const { return angle; };
    inline double c() const { return cosang; };
    inline double s() const { return sinang; };

private:
    // make these private as we need to
    // maintain concistency
    double angle;
    double cosang;
    double sinang;
};

/*-------------------------------------------------
 * CLASS: mx_rotation
 *
 * DESCRIPTION: Information about rotations
 *
 *
 */

class mx_rotation {
public:
    mx_rotation(double iangle = 0);
    mx_rotation(const mx_angle& a);
    mx_rotation(const mx_point& c, double iangle = 0);
    mx_rotation(const mx_point& c, const mx_angle& iangle);
    void set(const mx_point& ic, double iangle = 0);
    void set(const mx_point& ic, const mx_angle& iangle);
    mx_point c;
    mx_angle a;
};

/*-------------------------------------------------
 * CLASS: mx_bezier
 *
 * DESCRIPTION:
 *
 *
 */

class mx_bezier : public mx_serialisable_object {
public:
    mx_bezier() { a0.x = a0.y = a1.x = a1.y = a2.x = a2.y = a3.x = a3.y = 0; };
    mx_bezier(mx_point& ia0,
        mx_point& ia1,
        mx_point& ia2,
        mx_point& ia3);
    // standard bezier control points
    mx_point a0;
    mx_point a1;
    mx_point a2;
    mx_point a3;
    mx_point bz(double t);
    double linearFactor(double tolerance);
    mx_rect box();
    double dist2snap(const mx_point& p,
        mx_snap_info& snapInfo = defaultSnapInfo,
        double tolerance = GEOM_SMALL) const;
    inline double dist(mx_point& p) const { return sqrt(dist2snap(p)); };

    void serialise(int& err, uint8** buffer);
    void unserialise(int& err, uint8** buffer);
    uint32 get_serialised_size(int& err);

private:
    bool cpsConvex();
    double distConvex2(mx_point& p, double minDist,
        double tolerance, mx_point* pt);
    double internDist2(mx_point& p,
        double minDist,
        double tolerance,
        double splitLevel,
        double maxLevel,
        double currentDist,
        mx_point* pt);
};

/*-------------------------------------------------
 * ENUMS: mx_arc
 *
 * DESCRIPTION: Arc components
 *
 *
 */

typedef enum {
    MX_ARC_SNAP_NONE = MX_GEOM_SNAP_NONE,
    MX_ARC_SNAP_START,
    MX_ARC_SNAP_END,
    MX_ARC_SNAP_CENTRE,
    MX_ARC_SNAP_ARC,
    MX_ARC_SNAP_NO_COMPONENTS,
    MX_ARC_SNAP_WHOLE
} MX_ARC_SNAP;

typedef enum {
    MX_CHORD_SNAP_NONE = MX_GEOM_SNAP_NONE,
    MX_CHORD_SNAP_START,
    MX_CHORD_SNAP_END,
    MX_CHORD_SNAP_CENTRE,
    MX_CHORD_SNAP_ARC,
    MX_CHORD_SNAP_CHORD,
    MX_CHORD_SNAP_NO_COMPONENTS,
    MX_CHORD_SNAP_WHOLE
} MX_CHORD_SNAP;

typedef enum {
    MX_SECTOR_SNAP_NONE = MX_GEOM_SNAP_NONE,
    MX_SECTOR_SNAP_START,
    MX_SECTOR_SNAP_END,
    MX_SECTOR_SNAP_CENTRE,
    MX_SECTOR_SNAP_ARC,
    MX_SECTOR_SNAP_SPAN1,
    MX_SECTOR_SNAP_SPAN2,
    MX_SECTOR_SNAP_NO_COMPONENTS,
    MX_SECTOR_SNAP_WHOLE
} MX_SECTOR_SNAP;

/*-------------------------------------------------
 * CLASS: mx_arc
 *
 * DESCRIPTION:
 *
 *
 */

class mx_arc : public mx_serialisable_object {
public:
    static double cfactor;

    mx_arc()
    {
        c.x = c.y = s.x = s.y = 0;
        arcType = ARC_ARC;
        isArea = FALSE;
    };

    mx_arc(const mx_point& c,
        const mx_point& size,
        double startAng,
        double endAng,
        ARC_TYPE arcType = ARC_ARC,
        bool isArea = FALSE,
        double angle = 0);

    mx_rect box();
    mx_point ac(double angle) const;

    int numberComponents();
    int wholeSelectCount();
    bool mergeSelectCount(int nselectCounts,
        int* selectCounts);

    double dist2arc(const mx_point& p,
        mx_snap_info& snapInfo,
        double tolerance) const;

    double dist2snap(const mx_point& p,
        mx_snap_info& snapInfo = defaultSnapInfo,
        double tolerance = GEOM_SMALL) const;

    inline double dist(const mx_point& p) const { return sqrt(dist2snap(p)); };

    double linearFactor(double tolerance) const;

    inline bool isFull() const { return (ea.a() > (sa.a() + PITWO - GEOM_SMALL)); };
    inline bool isCircle() const { return (fabs((s.x - s.y) / GMAX(s.x, s.y))) < GEOM_SMALL; };

    bool isPoint() const;

    void setArc(const mx_point& c,
        const mx_point& size,
        double startAng,
        double endAng,
        ARC_TYPE arcType = ARC_ARC,
        bool isArea = FALSE,
        double angle = 0);

    double pointToDist(const mx_point& p) const;
    double pointToAngle(const mx_point& p) const;
    int quadrant(const mx_point& p) const;

    double distArc(const mx_point& p) const;
    double distSector(const mx_point& p) const;

    bool inArea(const mx_point& p, double snapTolerance, ARC_TYPE* nearnessType) const;

    mx_point unnormalisedPoint(const mx_point& p) const;
    mx_point normalisedPoint(const mx_point& p) const;

    mx_arc normalisedArc() const;

    mx_point tangent(double angle);
    mx_point normal(double angle);

    void serialise(int& err, uint8** buffer);
    void unserialise(int& err, uint8** buffer);
    uint32 get_serialised_size(int& err);

    // find the nearest point in a quad
    bool nearestPointInQuad(int iquad,
        bool quickAbortTest,
        const mx_point& inPoint,
        mx_point& outPoint,
        double& distVal) const;

    inline bool pointEllipse() const
    {
        return ((s.x == 0.0) && (s.y == 0.0));
    }

    inline bool xLine() const
    {
        return ((s.y == 0.0) && (s.x != 0.0));
    }

    inline bool yLine() const
    {
        return ((s.x == 0.0) && (s.y != 0.0));
    }

    inline bool isDegenerate() const
    {
        return (isPoint() || xLine() || yLine());
    }

    void arcToPolypoint(double tolerance,
        bool doAppend,
        mx_polypoint& pline) const;

    void arcToBoundingPolypoint(double tolerance,
        mx_polypoint& pline) const;

    void arcToQuadrants(bool* quad) const;

    int components(bool* hasChord = NULL,
        bool* hasSpans = NULL,
        int* wholeArcType = NULL) const;

    // these types dont need to be private - no
    // internal consistency involved

    ARC_TYPE arcType;
    bool isArea;

    inline const mx_point& gc() const { return c; };
    inline const mx_point& gs() const { return s; };
    inline const mx_point& gsap() const { return startArc; };
    inline const mx_point& geap() const { return endArc; };

    inline double gsa() const { return sa.a(); };
    inline double gea() const { return ea.a(); };
    inline double ga() const { return angle.a(); };
    mx_angle gan() const { return angle; };

    void sc(mx_point& ic);
    void ss(mx_point& is);

    void ssa(double isa);
    void sea(double iea);
    void saa(double iaa);

private:
    // centre
    mx_point c;
    // size
    mx_point s;
    // start angle -for x degress*64
    mx_angle sa;
    // end angle - for x degrees*64
    mx_angle ea;

    // rotated angle
    mx_angle angle;

    // internally hold start and end points
    mx_point startArc;
    mx_point endArc;

    bool mergeArcSelectCount(int nselectCounts,
        int* selectCounts);

    bool mergeSectorSelectCount(int nselectCounts,
        int* selectCounts);

    bool mergeChordSelectCount(int nselectCounts,
        int* selectCounts);

    double dist2arc(const mx_point& p,
        double tolerance,
        mx_point& outPoint,
        double& outAngle) const;

    void nearQuads(const mx_point& p,
        mx_ipoint& nearQuads) const;

    bool dist2quad(const mx_point& p,
        mx_point& outPoint,
        int iquad,
        bool clipToArc,
        double tolerance,
        double& distVal,
        double angle) const;

    double nearestEndPoint(const mx_point& p,
        mx_point& outPoint,
        bool& isStart) const;

    double dist2properArcArc(const mx_point& p,
        mx_snap_info& snapInfo,
        double tolerance) const;

    bool dist2arcArcComponents(const mx_point& p,
        mx_snap_info& snapInfo,
        double tolerance,
        double& distVal) const;

    bool dist2degenerateArcArc(const mx_point& p,
        mx_snap_info& snapInfo,
        double tolerance,
        double& distVal) const;

    double dist2properArc(const mx_point& p,
        mx_snap_info& snapInfo,
        double tolerance) const;

    bool dist2arcComponents(const mx_point& p,
        mx_snap_info& snapInfo,
        double tolerance,
        double& distVal) const;

    bool dist2degenerateArc(const mx_point& p,
        mx_snap_info& snapInfo,
        double tolerance,
        double& distVal) const;

    void testAngleRange(mx_rect& r,
        double testAngle,
        bool isXtest) const;

    void boxDegenerateArc(mx_rect& r) const;
};

typedef enum {
    MX_SPAN_SNAP_NONE = MX_GEOM_SNAP_NONE,
    MX_SPAN_SNAP_START,
    MX_SPAN_SNAP_END,
    MX_SPAN_SNAP_NO_COMPONENTS,
    MX_SPAN_SNAP_WHOLE
} MX_SPAN_SNAP;

typedef enum {
    MX_RECT_SNAP_NONE = MX_GEOM_SNAP_NONE,
    MX_RECT_SNAP_TL,
    MX_RECT_SNAP_TR,
    MX_RECT_SNAP_BR,
    MX_RECT_SNAP_BL,
    MX_RECT_SNAP_TOP,
    MX_RECT_SNAP_RIGHT,
    MX_RECT_SNAP_BOTTOM,
    MX_RECT_SNAP_LEFT,
    MX_RECT_SNAP_NO_COMPONENTS,
    MX_RECT_SNAP_WHOLE
} MX_RECT_SNAP;

/*-------------------------------------------------
 * CLASS: mx_rect
 *
 * DESCRIPTION: Basic rectangle with double coordinates
 *  This is a lightwieght class for rectangles - no
 *  virtual functions
 *
 * The coordinates are considered in the Maxwell coordinate
 * system e.g. (0,0) is the top left, (1,1) to the bottom
 * right.
 *
 * Rects have two interpretations - if isSpan is true it
 * is considered as a single line segment from (xb,yb)
 * (yt,yb) else it is considered to be a proper area
 * rectangle
 *
 * A non-span type rect is normalised if (xb,yb) is to the
 * top left above (xt,yt).
 *
 * e.g. (xb,yb,xt,yt)  = (0,0,1,1) is normalised
 *         but           (1,0,0,1) is not normalised
 * that is a rectangle is normalised if xb <= xt and
 * yb <= yt
 */

class mx_rect : public mx_serialisable_object {
public:
    // Default constructor

    static const int spanSize = 4 * SLS_DOUBLE + 2 * SLS_BOOL;
    static const int boxSize = 5 * SLS_DOUBLE + 2 * SLS_BOOL;

    mx_rect(bool span = FALSE,
        bool isArea = TRUE,
        double angle = 0);

    // From four coordinates

    mx_rect(double pxb, double pyb,
        double pxt, double pyt,
        bool span = FALSE,
        bool isArea = TRUE,
        double angle = 0);

    mx_rect& operator=(const mx_rect& other)
    {
        xt = other.xt;
        xb = other.xb;
        yt = other.yt;
        yb = other.yb;
        isSpan = other.isSpan;
        isArea = other.isArea;
        angle = other.angle;
        return *this;
    }

    // From two points
    mx_rect(const mx_point& pb, const mx_point& pt,
        bool span = FALSE,
        bool isArea = TRUE,
        double angle = 0);

    // From an irect (to allow interchangeability)

    mx_rect(const mx_irect& irct);

    inline int wholeSelectCount() { return isSpan ? ((int)MX_SPAN_SNAP_WHOLE) : ((int)MX_RECT_SNAP_WHOLE); };

    bool mergeSelectCount(int nselectCounts,
        int* selectCounts);

    // Basic operators

    inline mx_rect& operator+=(const mx_point& p)
    {
        xt += p.x;
        yt += p.y;
        xb += p.x;
        yb += p.y;
        return *this;
    };

    inline mx_rect& operator-=(const mx_point& p)
    {
        xt -= p.x;
        yt -= p.y;
        xb -= p.x;
        yb -= p.y;
        return *this;
    };

    inline mx_rect& operator*=(double f)
    {
        xt *= f;
        yt *= f;
        xb *= f;
        yb *= f;
        return *this;
    };

    inline mx_rect& operator/=(double f)
    {
        xt /= f;
        yt /= f;
        xb /= f;
        yb /= f;
        return *this;
    };

    inline mx_rect operator+(const mx_point& p2)
    {
        return mx_rect(this->xb + p2.x, this->yb + p2.y,
            this->xt + p2.x, this->yt + p2.y,
            this->isSpan,
            this->isArea,
            this->angle.a());
    };

    inline mx_rect operator-(const mx_point& p2)
    {
        return mx_rect(this->xb - p2.x, this->yb - p2.y,
            this->xt - p2.x, this->yt - p2.y,
            this->isSpan,
            this->isArea,
            this->angle.a());
    };

    inline mx_rect operator*(double f)
    {
        return mx_rect(this->xb * f, this->yb * f,
            this->xt * f, this->yt * f,
            this->isSpan,
            this->isArea,
            this->angle.a());
    };

    inline mx_rect operator/(double f)
    {
        return mx_rect(this->xb / f, this->yb / f,
            this->xt / f, this->yt / f,
            this->isSpan,
            this->isArea,
            this->angle.a());
    };

    inline bool operator==(mx_rect& r2)
    {
        return (this->xt == r2.xt) && (this->yt == r2.yt) && (this->xb == r2.xb) && (this->yb == r2.yb) && (this->isSpan == r2.isSpan) && (this->isArea == r2.isArea) && (this->angle.a() == r2.angle.a());
    };

    inline bool operator!=(mx_rect& r2)
    {
        return !(*this == r2);
    };

    inline bool operator>(mx_rect& r2)
    {
        mx_point p1 = this->start();
        mx_point p2 = r2.start();

        if (p1 > p2) {
            return TRUE;
        } else if (p1 == p2) {
            p1 = this->end();
            p2 = r2.end();

            return (p1 > p2);
        }

        return FALSE;
    };

    inline bool operator>=(mx_rect& r2)
    {
        if (*this == r2)
            return TRUE;

        return (*this > r2);
    };

    inline bool operator<(mx_rect& r2)
    {
        mx_point p1 = this->start();
        mx_point p2 = r2.start();

        if (p1 < p2) {
            return TRUE;
        } else if (p1 == p2) {
            p1 = this->end();
            p2 = r2.end();

            return (p1 < p2);
        }

        return FALSE;
    };

    inline bool operator<=(mx_rect& r2)
    {
        if (*this == r2)
            return TRUE;

        return (*this < r2);
    };

    // Tests for normalisation
    mx_rect box() const;
    mx_rect normalised() const;
    void normalise();
    inline bool isNormalised() const { return (isSpan) || ((xb <= xt) && (yb <= yt)); };
    bool getSpan() { return isSpan; };
    bool getArea() { return isArea; };
    bool getAngle() { return angle.a(); };

    // Basic geometry stuff
    inline double area() const { return (xt - xb) * (yt - yb); };
    inline double length2() const { return (xb - xt) * (xb - xt) + (yb - yt) * (yb - yt); };
    inline double length() const { return sqrt(length2()); };

    // Get the span of the rectangle as a point
    // mx_point = (xt-bx,yt-yb)
    mx_point span() const
    {
        mx_point p(xt - xb, yt - yb);
        return p;
    };

    // Get the unit span of the rectangle
    // optionally return length
    mx_point uspan(double* len = NULL) const;

    mx_point unnormalisedPoint(const mx_point& p) const;
    mx_point normalisedPoint(const mx_point& p) const;
    mx_rect normalisedRect() const;

    void rectToPolypoint(bool doAppend,
        mx_polypoint& pline) const;

    bool inArea(const mx_point& p,
        double snapTolerance,
        MX_RECT_SNAP* nearnessType) const;
    // (xb,yb)
    inline mx_point start() const
    {
        mx_point p(xb, yb);
        return p;
    };

    // (xt,yt)
    inline mx_point end() const
    {
        mx_point p(xt, yt);
        return p;
    };

    // Coordinates (for Maxwell device and doc)
    mx_point topLeft() const;
    mx_point bottomLeft() const;
    mx_point bottomRight() const;
    mx_point topRight() const;

    inline mx_point size() const { return bottomRight() - topLeft(); };

    inline mx_rect top() const
    {
        mx_rect s(topLeft(), topRight(), TRUE);
        return s;
    }
    inline mx_rect right() const
    {
        mx_rect s(topRight(), bottomRight(), TRUE);
        return s;
    };
    inline mx_rect bottom() const
    {
        mx_rect s(bottomRight(), bottomLeft(), TRUE);
        return s;
    };
    inline mx_rect left() const
    {
        mx_rect s(bottomLeft(), topLeft(), TRUE);
        return s;
    };

    // Change between area and span representation
    inline void setSpan(bool isspan) { setSpanArea(isspan, FALSE); };
    inline void setArea(bool isarea) { isArea = isarea; };
    inline void setAngle(double ira) { angle.set(ira); };

    bool pointIn(const mx_point& p) const;

    double xt;
    double xb;
    double yt;
    double yb;
    bool isSpan;
    bool isArea;
    mx_angle angle;

    double dist2snap(const mx_point& p,
        mx_snap_info& snapInfo = defaultSnapInfo,
        double tolerance = GEOM_SMALL) const;

    inline double dist(const mx_point& p) const { return sqrt(dist2snap(p)); };

    // The integer irect which contains the
    // real rectangle
    mx_irect container(double tolerance);

    mx_rect increaseRect(mx_rect& rect);
    void setSpanArea(bool isSpan, bool isArea);

    bool intersects(const mx_rect& o) const;

    void serialise(int& err, uint8** buffer);
    void unserialise(int& err, uint8** buffer);
    uint32 get_serialised_size(int& err);

protected:
private:
    //  bool orthogonal_intersect(mx_point top, mx_point bottom, mx_point left, mx_point right) const;

    bool mergeSpanSelectCount(int nselectCounts,
        int* selectCounts);

    bool mergeRectSelectCount(int nselectCounts,
        int* selectCounts);

    bool dist2spanComponents(const mx_point& p,
        mx_snap_info& snapInfo,
        double tolerance,
        double& distval) const;

    bool dist2degenerateSpan(const mx_point& p,
        mx_snap_info& snapInfo,
        double tolerance,
        double& distVal) const;

    double dist2properSpan(const mx_point& p,
        mx_snap_info& snapInfo,
        double tolerance) const;

    bool dist2rectComponents(const mx_point& p,
        mx_snap_info& snapInfo,
        double tolerance,
        double& distval) const;

    bool dist2degenerateRect(const mx_point& p,
        mx_snap_info& snapInfo,
        double tolerance,
        double& distVal) const;

    double dist2properRect(const mx_point& p,
        mx_snap_info& snapInfo,
        double tolerance) const;

    double dist2span(const mx_point& p,
        mx_snap_info& snapInfo = defaultSnapInfo,
        double tolerance = GEOM_SMALL) const;

    double dist2rect(const mx_point& p,
        mx_snap_info& snapInfo = defaultSnapInfo,
        double tolerance = GEOM_SMALL) const;
};

/*-------------------------------------------------
 * CLASS: mx_irect
 *
 * DESCRIPTION: Basic rectangle with integer coordinates
 *  This is a lightwieght class for rectangles - no
 *  virtual functions
 *
 * The coordinates are considered in the Maxwell coordinate
 * system e.g. (0,0) is the top left, (1,1) to the bottom
 * right.
 *
 * Rects have two interpretations - if isSpan is true it
 * is considered as a single line segment from (xb,yb)
 * (yt,yb) else it is considered to be a proper area
 * rectangle
 *
 * A non-span type rect is normalised if (xb,yb) is to the
 * top left above (xt,yt).
 *
 * e.g. (xb,yb,xt,yt)  = (0,0,1,1) is normalised
 *         but           (1,0,0,1) is not normalised
 * that is a rectangle is normalised if xb <= xt and
 * yb <= yt

 * If the isSpan field is TRUE the structure represents a
 * directed span from (xt,yt) to (xb,yb) , that is a single
 * line segment
 */

class mx_irect {
public:
    // Basic constructor
    mx_irect(bool span = FALSE, bool area = TRUE);

    // From four coords
    mx_irect(int32 pxb, int32 pyb,
        int32 pxt, int32 pyt,
        bool span = FALSE,
        bool area = TRUE);

    // Two points (use mx_ipoint to mx_ipoint conversion to
    //                  use with mx_ipoint argument)

    mx_irect(const mx_point& pb, const mx_point& pt,
        bool span = FALSE,
        bool area = TRUE);

    // From a rect (for interchangeability

    mx_irect(const mx_rect& rct);

    // Basic operators

    inline mx_irect& operator+=(const mx_ipoint& p)
    {
        xt += p.x;
        xb += p.x;
        yt += p.y;
        yb += p.y;

        return *this;
    };

    inline mx_irect& operator-=(const mx_ipoint& p)
    {
        xt -= p.x;
        yt -= p.y;
        xb -= p.x;
        yb -= p.y;
        return *this;
    };

    inline mx_irect& operator*=(double f)
    {
        double temp;

        temp = xt * f;
        xt = (int32)GNINT(temp);

        temp = yt * f;
        yt = (int32)GNINT(temp);

        temp = xb * f;
        xb = (int32)GNINT(temp);

        temp = yb * f;
        yb = (int32)GNINT(temp);

        return *this;
    };
    inline mx_irect& operator/=(double f)
    {
        double temp;

        temp = xt / f;
        xt = (int32)GNINT(temp);

        temp = yt / f;
        yt = (int32)GNINT(temp);

        temp = xb / f;
        xb = (int32)GNINT(temp);

        temp = yb / f;
        yb = (int32)GNINT(temp);

        return *this;
    };

    inline mx_irect operator+(const mx_ipoint& p2)
    {
        return mx_irect(this->xb + p2.x, this->yb + p2.y,
            this->xt + p2.x, this->yt + p2.y);
    };

    inline mx_irect operator-(const mx_ipoint& p2)
    {
        return mx_irect(this->xb - p2.x, this->yb - p2.y,
            this->xt - p2.x, this->yt - p2.y);
    };

    // Normalisation routines

    mx_irect normalised() const;
    void normalise();
    inline bool isNormalised() const { return (isSpan) || ((xb <= xt) && (yb <= yt)); };
    inline bool getSpan() { return isSpan; };
    inline bool getArea() { return isArea; };

    // Basic geometry stuff
    inline double area() const { return (xt - xb) * (yt - yb); };
    inline double length2() const { return (xb - xt) * (xb - xt) + (yb - yt) * (yb - yt); };
    inline double length() const { return sqrt(length2()); };

    // Get the span of the rectangle as a point
    // point = (xt-bx,yt-yb)

    mx_point span() const
    {
        mx_point p(xt - xb, yt - yb);
        return p;
    };

    // Get the unit span of the rectangle
    // optionally return length

    mx_point uspan(double* len = NULL) const;

    // (xb,yb)
    inline mx_ipoint start() const
    {
        mx_point p(xb, yb);
        return p;
    };

    // (xt,yt)
    inline mx_ipoint end() const
    {
        mx_point p(xt, yt);
        return p;
    };

    // Coordinates (for Maxwell device and doc)
    inline mx_ipoint topLeft() const { return mx_ipoint(GMIN(xt, xb), GMIN(yt, yb)); };
    inline mx_ipoint bottomLeft() const { return mx_ipoint(GMIN(xt, xb), GMAX(yt, yb)); };
    inline mx_ipoint bottomRight() const { return mx_ipoint(GMAX(xt, xb), GMAX(yt, yb)); };
    inline mx_ipoint topRight() const { return mx_ipoint(GMAX(xt, xb), GMIN(yt, yb)); };
    inline mx_ipoint size()
    {
        mx_ipoint p1(topLeft());
        mx_ipoint p2(bottomRight());
        mx_ipoint p3(p1);

        p3 -= p2;

        return p3;
    };

    // change span interpretation
    inline void setSpan(bool isspan) { setSpanArea(isspan, FALSE); };
    inline void setArea(bool isarea) { isArea = isarea; };

    int32 xt;
    int32 xb;
    int32 yt;
    int32 yb;
    bool isSpan;
    bool isArea;
    void setSpanArea(bool isSpan, bool isArea);

protected:
private:
};

/*------------------------------------------------------
 * CLASS: mx_polypoint
 *
 * DESCRIPTION: a basic polypoint made up of mx_points.
 */

class mx_polypoint : public mx_serialisable_object {
public:
    mx_polypoint(POLYPOINT_TYPE type = POLY_LINE);
    mx_polypoint(int num_points, POLYPOINT_TYPE type = POLY_LINE);
    mx_polypoint(const mx_ipolypoint& pp);
    mx_polypoint(const mx_polypoint& pp);
    ~mx_polypoint();

    mx_rect bounding_box() const;

    mx_point& operator[](int index) const;

    mx_point getPoint(double rindex) const;

    void operator+=(const mx_point& p);
    mx_polypoint& operator=(const mx_polypoint& pp);
    mx_polypoint& operator=(const mx_ipolypoint& pp);

    void serialise(int& err, uint8** buffer);
    void unserialise(int& err, uint8** buffer);
    uint32 get_serialised_size(int& err);

    bool addPoint(const mx_point& p);

    // add a new point if it is not coincident
    bool addNewPoint(const mx_point& p);

    void setMemorySize(int newSize);

    double dist2snap(const mx_point& p,
        mx_snap_info& snapInfo = defaultSnapInfo,
        double tolerance = GEOM_SMALL) const;
    inline double dist(mx_point& p) const { return sqrt(dist2snap(p)); };

    inline void set_area(bool iis_area) { is_area = iis_area; };
    inline bool get_area() const { return is_area; };

    inline void set_closed(bool iis_closed) { is_closed = iis_closed; };
    inline bool get_closed() const { return is_closed; };

    inline int get_num_points() const { return num_points; };

    bool in_area(const mx_point& p) const;

    void copy(const mx_ipolypoint& pp);
    void copy(const mx_polypoint& pp);

    inline void setPoint(int i, mx_point& pt) { points[i] = pt; };
    void clear(int npoints);

    // clipping start and end of the polypoint

    inline double getStartMask() const { return startMask; };
    inline void setStartMask(double imask) { startMask = imask; };

    inline double getEndMask() const { return endMask; };
    inline void setEndMask(double imask) { endMask = imask; };
    inline void unsetMask() { startMask = endMask = 0; };
    bool coincidentEnds();

    inline bool get_allow_coincident() const { return allowCoincident; };
    inline void set_allow_coincident(bool ia) { allowCoincident = ia; };
    inline void set_type(POLYPOINT_TYPE itype) { type = itype; };
    POLYPOINT_TYPE get_type() const { return type; };

    int wholeSelectCount();

    bool mergeSelectCount(int nselectCounts,
        int* selectCounts);

    void components(int& noComponentsCount,
        int& wholeCount) const;

    void append(const mx_polypoint& rest);

    bool split(int& err,
        int32 maxSize,
        int32 inSegmentId,
        int32& outSegmentId,
        mx_polypoint& startPolypoint);

    void copyBasics(const mx_ipolypoint& pp);
    void copyBasics(const mx_polypoint& pp);
    void copyPoints(const mx_ipolypoint& pp, int start, int end);
    void copyPoints(const mx_polypoint& pp, int start, int end);

private:
    void init(bool resetPoints, POLYPOINT_TYPE itype);

    const int basicSize = (SLS_INT32 + 2 * SLS_BOOL + SLS_ENUM);

    bool dist2plineComponents(const mx_point& p,
        mx_snap_info& snapInfo,
        double tolerance,
        double& distVal) const;

    inline bool quickSpanCheck(const mx_point& p,
        const mx_point& startPoint,
        const mx_point& endPoint,
        double testVal) const;

    double dist2properPline(const mx_point& p,
        mx_snap_info& snapInfo,
        double tolerance) const;
    mx_point* points;
    int num_points;
    int num_alloc_points;
    bool is_closed;
    bool is_area;
    mx_rect box;
    double startMask;
    double endMask;
    POLYPOINT_TYPE type;
    bool allowCoincident;
};

/*------------------------------------------------------
 * CLASS: mx_ipolypoint
 *
 * DESCRIPTION: a basic polypoint made up of mx_ipoints.
 */

class mx_ipolypoint {
public:
    mx_ipolypoint(POLYPOINT_TYPE type = POLY_LINE);
    mx_ipolypoint(const mx_ipolypoint& pp);
    mx_ipolypoint(const mx_polypoint& pp);
    mx_ipolypoint(int num_points, POLYPOINT_TYPE type = POLY_LINE);
    ~mx_ipolypoint();

    mx_irect bounding_box() const;

    mx_ipoint& operator[](int index) const;
    void operator+=(const mx_ipoint& p);
    mx_ipolypoint& operator=(const mx_polypoint& pp);
    mx_ipolypoint& operator=(const mx_ipolypoint& pp);

    bool addPoint(const mx_ipoint& p);

    // add a new point if it is not coincident
    bool addNewPoint(const mx_ipoint& p);

    void setMemorySize(int newSize);

    inline void set_area(bool iis_area) { is_area = iis_area; };
    inline bool get_area() const { return is_area; };

    inline void set_closed(bool iis_closed) { is_closed = iis_closed; };
    inline bool get_closed() const { return is_closed; };

    inline int get_num_points() const { return num_points; };

    void copy(const mx_ipolypoint& pp);

    inline void setPoint(int i, mx_ipoint& pt) { points[i] = pt; };
    void clear(int npoints);

    // clipping start and end of the polypoint

    inline double getStartMask() { return startMask; };
    inline void setStartMask(double imask) { startMask = imask; };

    inline double getEndMask() { return endMask; };
    inline void setEndMask(double imask) { endMask = imask; };
    inline void unsetMask() { startMask = endMask = 0; };

    bool coincidentEnds();

    inline bool get_allow_coincident() const { return allowCoincident; };
    inline void set_allow_coincident(bool ia) { allowCoincident = ia; };

    inline void set_type(POLYPOINT_TYPE itype) { type = itype; };
    POLYPOINT_TYPE get_type() const { return type; };

    void copyBasics(const mx_ipolypoint& pp);
    void copyPoints(const mx_ipolypoint& pp, int start, int end);

private:
    void init(bool resetPoints, POLYPOINT_TYPE itype);
    mx_ipoint* points;
    int num_points;
    int num_alloc_points;
    bool is_closed;
    bool is_area;
    mx_irect box;

    double startMask;
    double endMask;
    POLYPOINT_TYPE type;
    bool allowCoincident;
};

/*-------------------------------------------------
 * CLASS: mx_spline
 *
 * DESCRIPTION: C2 bezier spline
 *
 *
 */

class mx_spline : public mx_serialisable_object {
public:
    mx_spline()
    {
        points = NULL;
        beziers = NULL;
        npoints = 0;
        nallocPoints = 0;
    };
    mx_spline(int& err, bool doCopy, int inpoints, mx_point* points);
    mx_bezier* beziers;

    double dist2snap(const mx_point& p,
        mx_snap_info& snapInfo = defaultSnapInfo,
        double tolerance = GEOM_SMALL) const;

    inline double dist(mx_point& p) const { return sqrt(dist2snap(p)); };
    void copy(int& err, mx_spline& inpl, bool fullCopy);
    inline mx_rect bounding_box() { return box; };

    void serialise(int& err, uint8** buffer)
    {
        err = MX_ERROR_OK;
        return;
    };
    void unserialise(int& err, uint8** buffer)
    {
        err = MX_ERROR_OK;
        return;
    };
    uint32 get_serialised_size(int& err)
    {
        err = MX_ERROR_OK;
        return 0;
    };

protected:
private:
    void setBeziers(int startPoint,
        int endPoint);
    void newMemory(int& err,
        int newPoints);
    void addPoint(const mx_point& p);

    mx_point* points;
    int npoints;
    int nallocPoints;
    mx_rect box;
};

// Some simple geometry types to be getting on with

typedef enum {
    MX_GEOM_INVALID,
    MX_GEOM_POINT,
    MX_GEOM_SPAN,
    MX_GEOM_RECT,
    MX_GEOM_LINE,
    MX_GEOM_POLYPOINT,
    MX_GEOM_ARC,
    MX_GEOM_BEZIER,
    MX_GEOM_SPLINE,
    MX_GEOM_NONE
} MX_GEOM_TYPE;

/*-------------------------------------------------
 * CLASS: mx_geom
 *
 * DESCRIPTION: A general geometry which may be a
 * complex combination of other geometries. This is
 * the heavy weight geometry class from which
 * virtual functions for size etc should be
 * put. This geometry class only deals with the
 * geometric properties of geometries and not how to
 * serialise them and draw them
 *
 * many operations will return geometries so we will
 * need to know their type.
 */

class mx_geom : public mx_serialisable_object {
public:
    // values used for iteration
    static double fac1;
    static double fac2;

    mx_geom();

    MX_GEOM_TYPE type;
    int32 styleId;
    int32 colourId;
    int selectCount;

    inline void getDetails(int32& s, int32& c)
    {
        s = styleId;
        c = colourId;
    };
    inline void setDetails(int32 s, int32 c)
    {
        styleId = s;
        colourId = c;
    };

    virtual int dimension() = 0;
    virtual bool storeBox() { return TRUE; };
    virtual mx_rect box(int& err) = 0;

    virtual GEOM_INTERSECT_TYPE intersectBox(int& err,
        mx_rect& r,
        double tolerance = GEOM_SMALL)
        = 0;

    virtual double dist2snap(const mx_point& p,
        mx_snap_info& snapInfo = defaultSnapInfo,
        double tolerance = GEOM_SMALL) const;

    inline double dist(mx_point& p) const { return sqrt(dist2snap(p)); };
    inline MX_GEOM_TYPE getType() { return type; };

    // serialisation
    virtual void serialise(int& err, uint8** buffer);
    virtual void unserialise(int& err, uint8** buffer);
    virtual uint32 get_serialised_size(int& err);

    // the number of components of a geometry
    virtual int wholeSelectCount() = 0;

    virtual bool mergeSelectCount(int nselectCounts,
        int* selectCounts)
        = 0;

    virtual mx_geom* duplicate() const = 0;

    virtual bool isSegmentable() { return FALSE; };

    virtual int segment(int& err,
        int32 maxSize,
        int32 segmentId,
        mx_geom** splitGeometry)
    {
        err = MX_ERROR_OK;
        *splitGeometry = NULL;
        return -1;
    }

    virtual int fuse(int& err,
        mx_geom* restGeometry,
        bool isFirst)
    {
        err = MX_ERROR_OK;
        return 0;
    };

    virtual bool requireAllSegments() { return TRUE; };
    virtual bool joinEndSegments() { return TRUE; };

    virtual bool isClosed() { return FALSE; };

protected:
private:
};

/*-------------------------------------------------
 * CLASS: gmx_bezier
 *
 * DESCRIPTION:
 *
 *
 */

class gmx_bezier : public mx_geom {
public:
    mx_bezier b;
    gmx_bezier() { type = MX_GEOM_BEZIER; };
    gmx_bezier(mx_bezier& ib)
    {
        type = MX_GEOM_BEZIER;
        b = ib;
    };

    int dimension() { return 1; };
    mx_rect box(int& err)
    {
        err = MX_ERROR_OK;
        return b.box();
    };

    double dist2snap(const mx_point& p,
        mx_snap_info& snapInfo = defaultSnapInfo,
        double tolerance = GEOM_SMALL) const
    {
        return b.dist2snap(p, snapInfo, tolerance);
    };

    virtual GEOM_INTERSECT_TYPE intersectBox(int& err, mx_rect& r, double tolerance = GEOM_SMALL) { return GEOM_INTERSECT_NONE; };

    void serialise(int& err, uint8** buffer);
    void unserialise(int& err, uint8** buffer);
    uint32 get_serialised_size(int& err);
    mx_geom* duplictate() { return NULL; };

    int wholeSelectCount() { return 0; };

    bool mergeSelectCount(int nselectCounts,
        int* selectCounts) { return FALSE; };

protected:
private:
};

/*-------------------------------------------------
 * CLASS: gmx_arc
 *
 * DESCRIPTION:
 *
 *
 */

class gmx_arc : public mx_geom {
public:
    mx_arc a;
    gmx_arc() { type = MX_GEOM_ARC; };
    gmx_arc(const mx_arc& ia)
    {
        type = MX_GEOM_ARC;
        a = ia;
    };
    gmx_arc(const gmx_arc& ia) { *this = ia; };
    int dimension() { return a.isArea ? 2 : 1; };
    mx_rect box(int& err)
    {
        err = MX_ERROR_OK;
        return a.box();
    };

    double dist2snap(const mx_point& p,
        mx_snap_info& snapInfo = defaultSnapInfo,
        double tolerance = GEOM_SMALL) const
    {
        return a.dist2snap(p, snapInfo, tolerance);
    };
    virtual GEOM_INTERSECT_TYPE intersectBox(int& err,
        mx_rect& r,
        double tolerance = GEOM_SMALL);

    void serialise(int& err, uint8** buffer);
    void unserialise(int& err, uint8** buffer);
    uint32 get_serialised_size(int& err);

    inline int wholeSelectCount() { return a.wholeSelectCount(); };

    inline bool mergeSelectCount(int nselectCounts,
        int* selectCounts) { return a.mergeSelectCount(nselectCounts,
        selectCounts); };
    mx_geom* duplicate() const { return new gmx_arc(*this); };

protected:
private:
};

/*-------------------------------------------------
 * CLASS: gmx_point
 *
 * DESCRIPTION: A mx_geom point
 *
 *
 */

class gmx_point : public mx_geom {
public:
    mx_point p;

    // simple constructor
    gmx_point() { type = MX_GEOM_POINT; };
    gmx_point(const mx_point& p);
    gmx_point(const gmx_point& p) { *this = p; };

    int dimension() { return 0; };
    bool storeBox() { return FALSE; };
    inline mx_rect box(int& err)
    {
        err = MX_ERROR_OK;
        return mx_rect(p, p, FALSE, TRUE);
    };
    virtual GEOM_INTERSECT_TYPE intersectBox(int& err, mx_rect& r, double tolerance = GEOM_SMALL);

    inline double dist2snap(const mx_point& pi,
        mx_snap_info& snapInfo = defaultSnapInfo,
        double tolerance = GEOM_SMALL) const { return p.dist2snap(pi, snapInfo, tolerance); };

    void serialise(int& err, uint8** buffer);
    void unserialise(int& err, uint8** buffer);
    uint32 get_serialised_size(int& err);
    mx_geom* duplicate() const { return new gmx_point(*this); };
    inline int wholeSelectCount() { return p.wholeSelectCount(); };

    inline bool mergeSelectCount(int nselectCounts,
        int* selectCounts) { return p.mergeSelectCount(nselectCounts,
        selectCounts); };
};

/*-------------------------------------------------
 * CLASS: grect
 *
 * DESCRIPTION: A geometry area type rectangle
 *
 *
 */

class gmx_rect : public mx_geom {
public:
    mx_rect r;
    // simple constructor
    gmx_rect() { type = MX_GEOM_RECT; };
    gmx_rect(const mx_rect& p);
    gmx_rect(const gmx_rect& p)
    {
        type = p.type;
        styleId = p.styleId;
        colourId = p.colourId;
        selectCount = p.selectCount;
    }

    bool storeBox() { return FALSE; };

    int dimension() { return r.isArea ? 2 : 1; };
    mx_rect box(int& err)
    {
        err = MX_ERROR_OK;
        return r.box();
    };

    double dist2snap(const mx_point& p,
        mx_snap_info& snapInfo = defaultSnapInfo,
        double tolerance = GEOM_SMALL) const { return r.dist2snap(p, snapInfo, tolerance); };

    virtual GEOM_INTERSECT_TYPE intersectBox(int& err, mx_rect& r, double tolerance = GEOM_SMALL);

    void serialise(int& err, uint8** buffer);
    void unserialise(int& err, uint8** buffer);
    uint32 get_serialised_size(int& err);

    mx_geom* duplicate() const { return new gmx_rect(*this); };

    inline int wholeSelectCount() { return r.wholeSelectCount(); };

    inline bool mergeSelectCount(int nselectCounts,
        int* selectCounts) { return r.mergeSelectCount(nselectCounts,
        selectCounts); };

private:
};

/*-------------------------------------------------
 * CLASS: gmx_span
 *
 * DESCRIPTION: A geometry span
 *
 *
 */

class gmx_span : public mx_geom {
public:
    mx_rect s;
    // simple constructor
    gmx_span()
        : s(TRUE)
    {
        type = MX_GEOM_SPAN;
    };
    gmx_span(const mx_rect& p);
    gmx_span(const gmx_span& p)
    {
        type = p.type;
        styleId = p.styleId;
        colourId = p.colourId;
        selectCount = p.selectCount;
    }

    bool storeBox() { return FALSE; };
    int dimension() { return 1; };
    mx_rect box(int& err)
    {
        err = MX_ERROR_OK;
        return s.box();
    };

    double dist2snap(const mx_point& p,
        mx_snap_info& snapInfo = defaultSnapInfo,
        double tolerance = GEOM_SMALL) const { return s.dist2snap(p, snapInfo, tolerance); };

    virtual GEOM_INTERSECT_TYPE intersectBox(int& err, mx_rect& r, double tolerance = GEOM_SMALL);

    void serialise(int& err, uint8** buffer);
    void unserialise(int& err, uint8** buffer);
    uint32 get_serialised_size(int& err);
    mx_geom* duplicate() const { return new gmx_span(*this); };
    inline int wholeSelectCount() { return s.wholeSelectCount(); };

    inline bool mergeSelectCount(int nselectCounts,
        int* selectCounts) { return s.mergeSelectCount(nselectCounts,
        selectCounts); };
};

/*-------------------------------------------------
 * CLASS: gmx_polypoint
 *
 * DESCRIPTION: A polypoint geometry
 *
 *
 */

class gmx_polypoint : public mx_geom {
public:
    mx_polypoint pl;
    // simple constructor
    gmx_polypoint() { type = MX_GEOM_POLYPOINT; };
    gmx_polypoint(const mx_polypoint& pl);
    gmx_polypoint(const gmx_polypoint& pl) { *this = pl; };
    inline int dimension() { return pl.get_area() ? 2 : 1; };
    inline mx_rect box(int& err)
    {
        err = MX_ERROR_OK;
        return pl.bounding_box();
    };

    inline double dist2snap(const mx_point& p,
        mx_snap_info& snapInfo = defaultSnapInfo,
        double tolerance = GEOM_SMALL) const { return pl.dist2snap(p, snapInfo, tolerance); };

    virtual GEOM_INTERSECT_TYPE intersectBox(int& err, mx_rect& r, double tolerance = GEOM_SMALL);

    void serialise(int& err, uint8** buffer);
    void unserialise(int& err, uint8** buffer);
    uint32 get_serialised_size(int& err);
    mx_geom* duplicate() const { return new gmx_polypoint(*this); };
    bool isSegmentable() { return TRUE; };

    int segment(int& err,
        int32 maxSize,
        int32 segmentId,
        mx_geom** splitGeometry);

    int fuse(int& err,
        mx_geom* restGeometry,
        bool isFirst);

    bool requireAllSegments() { return pl.get_area(); };
    bool joinEndSegments() { return pl.get_closed(); };
    inline int wholeSelectCount() { return pl.wholeSelectCount(); };

    inline bool isClosed() { return pl.get_closed(); };
    inline bool mergeSelectCount(int nselectCounts,
        int* selectCounts) { return pl.mergeSelectCount(nselectCounts,
        selectCounts); };
};

/*-------------------------------------------------
 * CLASS: gmx_spline
 *
 * DESCRIPTION: A C2 bezier spline
 *
 *
 */

class gmx_spline : public mx_geom {
public:
    mx_spline spl;
    // simple constructor
    gmx_spline() { type = MX_GEOM_SPLINE; };
    gmx_spline(int& err, mx_spline& spl, bool copy = TRUE);
    inline int dimension() { return 1; };
    inline mx_rect box(int& err)
    {
        err = MX_ERROR_OK;
        return spl.bounding_box();
    };
    virtual GEOM_INTERSECT_TYPE intersectBox(int& err, mx_rect& r, double tolerance = GEOM_SMALL) { return GEOM_INTERSECT_NONE; };

    inline double dist2span(const mx_point& p,
        mx_snap_info& snapInfo = defaultSnapInfo,
        double tolerance = GEOM_SMALL) const
    {
        return spl.dist2snap(p, snapInfo, tolerance);
    };

    void serialise(int& err, uint8** buffer);
    void unserialise(int& err, uint8** buffer);
    uint32 get_serialised_size(int& err);
    mx_geom* duplicate() const { return NULL; };
    inline int wholeSelectCount() { return 0; };

    inline bool mergeSelectCount(int nselectCounts,
        int* selectCounts) { return FALSE; };
};

/*-------------------------------------------------
 * CLASS: mx_affine
 *
 * DESCRIPTION: Class for an affine transform
 *
 *  The transform works on a point so that
 *
 *  (pox)  =  | a11 a12 | (px)  +  (b1)
 *  (poy)     | a21 a22 | (py)     (b2)
 *
 */

class mx_affine {
public:
    mx_affine();
    inline void set(double ia11,
        double ia12,
        double ia21,
        double ia22,
        double ib1,
        double ib2)
    {
        a11 = ia11;
        a12 = ia12;
        a21 = ia21;
        a22 = ia22;
        b1 = ib1;
        b2 = ib2;
    };

    inline mx_point operator*(const mx_point& p)
    {
        mx_point po;

        po.x = this->a11 * p.x + this->a12 * p.y + this->b1;
        po.y = this->a21 * p.x + this->a22 * p.y + this->b2;

        return po;
    };

    inline mx_rect operator*(mx_rect& r)
    {
        mx_rect ro(r);

        ro.xt = this->a11 * r.xt + this->a12 * r.yt + this->b1;
        ro.yt = this->a21 * r.xt + this->a22 * r.yt + this->b2;
        ro.xb = this->a11 * r.xb + this->a12 * r.yb + this->b1;
        ro.yb = this->a21 * r.xb + this->a22 * r.yb + this->b2;

        return ro;
    };

    double a11;
    double a12;
    double a21;
    double a22;
    double b1;
    double b2;

protected:
private:
};

class mxArcDistanceData {
public:
    mxArcDistanceData(const mx_arc* a,
        const mx_point& p,
        bool idoMin)
    {
        arc = a;
        point = p;
        doMin = idoMin;
    };
    const mx_arc* arc;
    mx_point point;
    bool doMin;
};

/*-------------------------------------------------
 * CLASS: mx_snap_info
 *
 * DESCRIPTION: Information about snapping
 *
 *
 */

class mx_snap_info {
public:
    mx_snap_info(bool isnapMode = FALSE,
        double isnapTolerance = 0);

    inline void setMode(bool im) { snapMode = im; };
    inline void setTolerance(double t)
    {
        snapTolerance = t;
        snapTolerance2 = t * t;
    };

    void copyBasics(const mx_snap_info& isnapInfo);

    inline void setCount(int icount) { count = icount; };
    inline int getCount() { return count; };

    inline bool isSnapping() { return snapMode; };

    bool setPoint(const mx_point& snapPoint,
        double distance,
        int count,
        double offset);

    bool alreadyFailed(int testCount);

    inline double getDistance() { return d; };
    inline double getSnapTolerance() { return snapTolerance; };
    inline double getSnapTolerance2() { return snapTolerance2; };
    inline mx_point getPoint() { return p; };
    inline void setPoint(const mx_point& ip) { p = ip; };

    // snap to a point
    bool dist2point(const mx_point& p,
        const mx_point& testPoint,
        int testCount,
        double tolerance,
        double& distVal);

    // snap to a span
    bool dist2span(const mx_point& p,
        const mx_rect& testSpan,
        int testCount,
        double tolerance,
        double& distVal);

    // snap to an arc span
    bool dist2arc(const mx_point& p,
        const mx_arc& testArc,
        int testCount,
        double tolerance,
        double& distVal);

    bool transferSnap(mx_snap_info& testSnap,
        int successCount,
        int newCount);

    double combine(int numSnapInfos,
        mx_snap_info* snapInfos);

private:
    bool snapMode; // TRUE - use tolerance
                   // and allow multiple selections
                   // - FALSE return nearest point

    double snapTolerance2; // tolerance for snapping
    double snapTolerance;

    mx_point p; // the snapped to point
    int count; // the component the point lies in
    double offset; // the offset within the component
    double d; // the distance to the point

    // set a geometry on a mx_snap_info
    mx_geom* geom; // the geometry to test
    mx_point testPoint; // the point to test against
    double testTolerance; // the tolerance to use in
                          // geometry (non-snap) stuff

    void init();
};

#include "geomproto.h"

#endif
