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

#include <map>

#include <mx_sl_obj.h>
#include <mx_style.h>

/*-------------------------------------------------
 * CLASS: mx_geomstyle
 *
 * DESCRIPTION: A general class for geometry style
 * types, holding the name and type of the style
 *
 */

class mx_geomstyle : public mx_serialisable_object {
public:
    mx_geomstyle()
    {
        id = -1;
        name = NULL;
        styleType = mx_gstyle_unknown;
        isPrivate = FALSE;
    };
    mx_geomstyle(const char* name, mx_gstyle_type_t stype, int iid = -1);

    virtual ~mx_geomstyle();

    virtual void serialise(int& err, uint8** buffer);
    virtual void unserialise(int& err, uint8** buffer);
    virtual uint32 get_serialised_size(int& err);

    inline const char* getName() { return name; };
    inline int getId() { return id; };
    inline mx_gstyle_type_t getType() { return styleType; };

    void setName(const char* iname);
    inline void setId(int iid) { id = iid; };
    inline void setType(mx_gstyle_type_t type) { styleType = type; };
    inline void setPrivate(bool iisPrivate) { isPrivate = iisPrivate; };

protected:
    int32 id;
    char* name;
    mx_gstyle_type_t styleType;
    // some styles may not be viewable to the public but
    // used internally by a style device
    bool isPrivate;
};

/*-------------------------------------------------
 * CLASS: mx_single_styles
 *
 * DESCRIPTION: A collection of styles to store
 * with a diagram
 *
 */

class mx_single_styles : public mx_serialisable_object {
public:
    mx_single_styles(mx_gstyle_type_t istyleType);
    ~mx_single_styles();

    int insert(int& err, mx_geomstyle* gstyle);
    void add(int& err, mx_geomstyle* gstyle);
    int insertColour(int& err, mx_colour* gstyle);

    void serialise(int& err, uint8** buffer);
    void unserialise(int& err, uint8** buffer);
    uint32 get_serialised_size(int& err);

    mx_geomstyle* getStyleById(int& err, int id);
    mx_geomstyle* getStyleByName(int& err, const char* name);

private:
    int32 currentId;
    mx_gstyle_type_t styleType;

    std::map<int, mx_geomstyle *> m_stylesById;
    std::map<std::string, mx_geomstyle *> m_stylesByName;
};

/*-------------------------------------------------
 * CLASS: mx_all_styles
 *
 * DESCRIPTION: A collection of styles to store
 * with a diagram
 *
 */

class mx_all_styles : public mx_serialisable_object {
public:
    mx_all_styles();
    ~mx_all_styles();

    void serialise(int& err, uint8** buffer);
    void unserialise(int& err, uint8** buffer);
    uint32 get_serialised_size(int& err);
    int insert(int& err,
        mx_geomstyle* gstyle);

    int insertColour(int& err,
        mx_colour* colour);

    mx_geomstyle* get(int& err,
        mx_gstyle_type_t styleType,
        int styleId);

    mx_colour* getColour(int& err,
        int colourId);

protected:
private:
    mx_single_styles dashStyles;
    mx_single_styles lineStyles;
    mx_single_styles fillStyles;
    mx_single_styles pointStyles;
    mx_single_styles borderStyles;
    mx_single_styles areaStyles;
    mx_single_styles hatchStyles;
    std::map<int, mx_colour *> m_colours;
};

/*-------------------------------------------------
 * CLASS: mx_gpoint_style
 *
 * DESCRIPTION:
 *
 *
 */

class mx_gpoint_style : public mx_geomstyle {
public:
    mx_gpoint_style() { };
    mx_gpoint_style(const char* name);
    mx_point_style style;
    void serialise(int& err, uint8** buffer);
    void unserialise(int& err, uint8** buffer);
    uint32 get_serialised_size(int& err);
};

/*-------------------------------------------------
 * CLASS: mx_gdash_style
 *
 * DESCRIPTION:
 *
 *
 */

class mx_gdash_style : public mx_geomstyle {
public:
    mx_gdash_style() { };
    mx_gdash_style(const char* name);
    mx_dash_style style;
    void serialise(int& err, uint8** buffer);
    void unserialise(int& err, uint8** buffer);
    uint32 get_serialised_size(int& err);
};

/*-------------------------------------------------
 * CLASS: mx_gline_style
 *
 * DESCRIPTION:
 *
 *
 */

class mx_gline_style : public mx_geomstyle {
public:
    mx_gline_style() { };
    mx_gline_style(const char* name);
    mx_line_style style;
    void serialise(int& err, uint8** buffer);
    void unserialise(int& err, uint8** buffer);
    uint32 get_serialised_size(int& err);
};

/*-------------------------------------------------
 * CLASS: mx_ghatch_style
 *
 * DESCRIPTION:
 *
 *
 */

class mx_ghatch_style : public mx_geomstyle {
public:
    mx_ghatch_style() { };
    mx_ghatch_style(const char* name);
    mx_hatch_style style;
    void serialise(int& err, uint8** buffer);
    void unserialise(int& err, uint8** buffer);
    uint32 get_serialised_size(int& err);
};

/*-------------------------------------------------
 * CLASS: mx_fill_style
 *
 * DESCRIPTION:
 *
 *
 */

class mx_gfill_style : public mx_geomstyle {
public:
    mx_gfill_style() { };
    mx_gfill_style(const char* name);
    mx_fill_style style;
    void serialise(int& err, uint8** buffer);
    void unserialise(int& err, uint8** buffer);
    uint32 get_serialised_size(int& err);
};

/*-------------------------------------------------
 * CLASS: mx_gbd_style
 *
 * DESCRIPTION:
 *
 *
 */

class mx_gborder_style : public mx_geomstyle {
public:
    mx_gborder_style() { };
    mx_gborder_style(const char* name);
    mx_border_style style;
    void serialise(int& err, uint8** buffer);
    void unserialise(int& err, uint8** buffer);
    uint32 get_serialised_size(int& err);
};

/*-------------------------------------------------
 * CLASS: mx_area_style
 *
 * DESCRIPTION:
 *
 *
 */

class mx_garea_style : public mx_geomstyle {
public:
    mx_garea_style() { };
    mx_garea_style(const char* name);
    mx_area_style style;
    void serialise(int& err, uint8** buffer);
    void unserialise(int& err, uint8** buffer);
    uint32 get_serialised_size(int& err);
};
