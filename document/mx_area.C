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
 * MODULE/CLASS : mx_area
 *
 * AUTHOR : Andrew Haisley
 *
 *
 *
 * DESCRIPTION:
 *
 * Areas have stuff in them......
 *
 *
 *
 */

#include <float.h>
#include <limits.h>

#include <mx_area.h>
#include <mx_area_paint.h>
#include <mx_doc.h>
#include <mx_sheet.h>

mx_area::mx_area(uint32 docid, mx_paragraph_style* s, bool is_controlled)
    : mx_paintable(docid, is_controlled)
{
    outline_cache.coords = NULL;
    base_style = s;
    style_mod = NULL;
}

mx_area::mx_area(uint32 docid, uint32 oid, bool is_controlled)
    : mx_paintable(docid, oid, is_controlled)
{
    outline_cache.coords = NULL;
    base_style = NULL;
    style_mod = NULL;
}

mx_area::mx_area(mx_area& a)
    : mx_paintable(a)
{
    // we now have a new db object. The only bits that can be copied at this
    // point are the outline and the origin. The sub-classes will have a lot
    // more work to do.
    mx_point origin, original_size, top_left_crop;
    const mx_quadspline_t* pp;

    outline_cache.coords = NULL;
    base_style = NULL;

    MX_ERROR_CHECK(error);
    // make sure that the other areas bits are in memory
    a.cc_unserialise_attributes(error);
    MX_ERROR_CHECK(error);

    base_style = a.base_style;
    style_mod = a.style_mod;
    if (style_mod != NULL) {
        style_mod = new mx_paragraph_style_mod(*style_mod);
    }

    pp = a.get_outline_readonly(error);
    if (error == MX_DB_CLIENT_CACHE_NO_SUCH_ATTR) {
        MX_ERROR_CLEAR(error);
    } else {
        MX_ERROR_CHECK(error);

        set_outline(error, *pp);
        MX_ERROR_CHECK(error);
    }

    origin = a.get_origin(error);
    MX_ERROR_CHECK(error);

    set_origin(error, origin);
    MX_ERROR_CHECK(error);

    if (a.get_original_size(error, original_size)) {
        this->set_original_size(error, original_size);
        MX_ERROR_CHECK(error);
    }

    if (a.get_top_left_crop(error, top_left_crop)) {
        this->set_top_left_crop(error, top_left_crop);
        MX_ERROR_CHECK(error);
    }

    return;
abort:
    global_error_trace->print();
    MX_ERROR_CLEAR(error);
}

void mx_area::move_to_document(int& err, mx_document* new_doc)
{
    mx_paragraph_style *old_ps = this->get_base_paragraph_style(), *new_ps;

    mx_db_object::move_to_document(err, new_doc->get_docid());
    MX_ERROR_CHECK(err);

    if (old_ps) {
        new_ps = new_doc->add_or_get_style(err, old_ps);
        MX_ERROR_CHECK(err);

        set_paragraph_style(new_ps);
    }
abort:;
}

mx_area::~mx_area()
{
    if (outline_cache.coords != NULL)
        delete[] outline_cache.coords;
    if (style_mod != NULL)
        delete style_mod;
}

void mx_area::delete_links(int& err)
{
    int i, n;

    mx_area_link_t* a;

    // first, need to unlink from chain
    remove_from_chain(err);
    MX_ERROR_CHECK(err);

    // now unlink from linked areas
    n = number_linked_areas(err);
    MX_ERROR_CHECK(err);

    for (i = 0; i < n; i++) {
        a = mx_area::linked_area(err, 0);
        MX_ERROR_CHECK(err);

        unlink_from_area(err, (mx_area*)a->linked_area);
        MX_ERROR_CHECK(err);

        delete a;
    }

    return;
abort:;
}

const mx_quadspline_t* mx_area::get_outline_readonly(int& err)
{
    const mx_attribute *xs, *ys, *flags;

    uint32 i;

    xs = this->get_readonly(err, MX_AREA_OUTLINE_X);
    MX_ERROR_CHECK(err);

    ys = this->get_readonly(err, MX_AREA_OUTLINE_Y);
    MX_ERROR_CHECK(err);

    flags = this->get_readonly(err, MX_AREA_OUTLINE_FLAG);
    MX_ERROR_CHECK(err);

    if (outline_cache.coords != NULL) {
        if (outline_cache.num_coords != xs->length) {
            delete[] outline_cache.coords;
            outline_cache.coords = new mx_spline_coord_t[xs->length];
        }
    } else {
        outline_cache.coords = new mx_spline_coord_t[xs->length];
    }

    outline_cache.num_coords = xs->length;

    for (i = 0; i < xs->length; i++) {
        outline_cache.coords[i].p.x = xs->value.fa[i];
        outline_cache.coords[i].p.y = ys->value.fa[i];
        outline_cache.coords[i].flag = flags->value.ia[i];
    }

    return &outline_cache;
abort:
    return NULL;
}

mx_point mx_area::get_origin(int& err)
{
    float ox, oy;
    mx_point res;

    ox = this->get_default_float(err, MX_AREA_ORIGIN_X, 0.0);
    MX_ERROR_CHECK(err);

    oy = this->get_default_float(err, MX_AREA_ORIGIN_Y, 0.0);
    MX_ERROR_CHECK(err);

    res.x = ox;
    res.y = oy;

abort:
    return res;
}

float mx_area::get_rotation(int& err)
{
    float f;

    f = this->get_default_float(err, MX_AREA_ROTATION, 0.0);
    MX_ERROR_CHECK(err);

    return f;

abort:
    return 0.0;
}

void mx_area::set_outline(int& err, const mx_quadspline_t& pp)
{
    static float buf[MX_ATTRIBUTE_MAX_FLOAT_ARRAY_LENGTH];
    static int32 flags[MX_ATTRIBUTE_MAX_INT_ARRAY_LENGTH];

    uint32 i;
    mx_attribute attrib;

    if (pp.num_coords > MX_ATTRIBUTE_MAX_FLOAT_ARRAY_LENGTH) {
        err = MX_AREA_TOO_MANY_COORDS;
        return;
    }

    for (i = 0; i < pp.num_coords; i++) {
        buf[i] = pp.coords[i].p.x;
    }

    attrib.type = mx_float_array;
    attrib.value.fa = buf;
    attrib.length = pp.num_coords;
    strcpy(attrib.name, MX_AREA_OUTLINE_X);

    this->set(err, &attrib);
    MX_ERROR_CHECK(err);

    for (i = 0; i < pp.num_coords; i++) {
        buf[i] = pp.coords[i].p.y;
    }

    attrib.type = mx_float_array;
    attrib.value.fa = buf;
    attrib.length = pp.num_coords;
    strcpy(attrib.name, MX_AREA_OUTLINE_Y);

    this->set(err, &attrib);
    MX_ERROR_CHECK(err);

    for (i = 0; i < pp.num_coords; i++) {
        flags[i] = pp.coords[i].flag;
    }

    attrib.type = mx_int_array;
    attrib.value.ia = flags;
    attrib.length = pp.num_coords;
    strcpy(attrib.name, MX_AREA_OUTLINE_FLAG);

    this->set(err, &attrib);
    MX_ERROR_CHECK(err);

abort:
    return;
}

void mx_area::set_origin(int& err, mx_point o)
{
    set_float(err, MX_AREA_ORIGIN_X, o.x);
    MX_ERROR_CHECK(err);

    set_float(err, MX_AREA_ORIGIN_Y, o.y);
    MX_ERROR_CHECK(err);

abort:
    return;
}

void mx_area::set_xorigin(int& err, float x)
{
    set_float(err, MX_AREA_ORIGIN_X, x);
    MX_ERROR_CHECK(err);
abort:;
}

void mx_area::set_yorigin(int& err, float y)
{
    set_float(err, MX_AREA_ORIGIN_Y, y);
    MX_ERROR_CHECK(err);
abort:;
}

void mx_area::set_rotation(int& err, float rotation)
{
    set_float(err, MX_AREA_ROTATION, rotation);
    MX_ERROR_CHECK(err);

abort:
    return;
}

mx_db_object* mx_area::get_owner_object(int& err)
{
    mx_sheet* res;
    uint32 id;

    res = (mx_sheet*)mx_db_object::get_owner_object(err);
    if (err == MX_DB_CLIENT_CACHE_NO_SUCH_OID) {
        MX_ERROR_CLEAR(err);
        res = NULL;
    } else {
        MX_ERROR_CHECK(err);
    }

    if (res == NULL) {
        // not in cache, get the id and create it
        id = get_default_id(err, MX_DB_OBJECT_OWNER_ID, 0);
        MX_ERROR_CHECK(err);

        if (id == 0) {
            // serious error
            err = MX_ERROR_UNDEFINED;
            return NULL;
        }

        res = new mx_sheet(doc_id, id);

        if (res->error != MX_ERROR_OK) {
            err = res->error;
            return NULL;
        }
    }

    return (mx_db_object*)res;

abort:
    return NULL;
}

// is there another area in a chain
bool mx_area::next_chained_area(int& err)
{
    uint32 i;

    i = get_default_id(err, MX_AREA_NEXT_AREA, 0);
    MX_ERROR_CHECK(err);

    return i != 0;
abort:
    return FALSE;
}

// is there a previous area in a chain
bool mx_area::previous_chained_area(int& err)
{
    uint32 i;

    i = get_default_id(err, MX_AREA_PREV_AREA, 0);
    MX_ERROR_CHECK(err);

    return i != 0;
abort:
    return FALSE;
}

// retrieves the next area in the chain
mx_area* mx_area::next_in_chain(int& err)
{
    uint32 id;
    mx_area* res;

    id = get_default_id(err, MX_AREA_NEXT_AREA, 0);
    MX_ERROR_CHECK(err);

    if (id == 0) {
        err = MX_AREA_NO_CHAIN;
        return NULL;
    }

    res = (mx_area*)mx_db_cc_get_obj_from_oid(err, doc_id, id);
    MX_ERROR_CHECK(err);

    return res;
abort:
    return NULL;
}

// retrieves the previous area in the chain
mx_area* mx_area::previous_in_chain(int& err)
{
    uint32 id;
    mx_area* res;

    id = get_default_id(err, MX_AREA_PREV_AREA, 0);
    MX_ERROR_CHECK(err);

    if (id == 0) {
        err = MX_AREA_NO_CHAIN;
        return NULL;
    }

    res = (mx_area*)mx_db_cc_get_obj_from_oid(err, doc_id, id);
    MX_ERROR_CHECK(err);

    return res;
abort:
    return NULL;
}

// insert into a chain of area - either next or prev may be
// NULL, but not both
void mx_area::insert_into_chain(int& err, mx_area* prev, mx_area* next)
{
    uint32 prev_id;
    uint32 next_id;

    if (prev != NULL) {
        prev->set_id(err, MX_AREA_NEXT_AREA, db_id);
        MX_ERROR_CHECK(err);

        prev_id = prev->get_db_id(err);
        MX_ERROR_CHECK(err);

        set_id(err, MX_AREA_PREV_AREA, prev_id);
        MX_ERROR_CHECK(err);
    } else {
        set_id(err, MX_AREA_PREV_AREA, 0);
        MX_ERROR_CHECK(err);
    }

    if (next != NULL) {
        next->set_id(err, MX_AREA_PREV_AREA, db_id);
        MX_ERROR_CHECK(err);

        next_id = next->get_db_id(err);
        MX_ERROR_CHECK(err);

        set_id(err, MX_AREA_NEXT_AREA, next_id);
        MX_ERROR_CHECK(err);
    } else {
        set_id(err, MX_AREA_NEXT_AREA, 0);
        MX_ERROR_CHECK(err);
    }
abort:;
}

// number of linked areas
int mx_area::number_linked_areas(int& err)
{
    int i;

    i = get_id_array_length(err, MX_AREA_LINKED_AREAS);
    MX_ERROR_CHECK(err);

    return i;

abort:
    return -1;
}

// linked area by index
mx_area_link_t* mx_area::linked_area(int& err, int index)
{
    mx_area_link_t* res;
    int na;
    uint32 id;

    res = new mx_area_link_t;

    na = number_linked_areas(err);
    MX_ERROR_CHECK(err);

    if ((index < 0) || (index >= na)) {
        err = MX_AREA_LINK_INDEX;
        return NULL;
    }

    id = get_id_array_item(err, MX_AREA_LINKED_AREAS, index);
    MX_ERROR_CHECK(err);

    res->linked_area = (mx_area*)mx_db_cc_get_obj_from_oid(err, doc_id, id);
    MX_ERROR_CHECK(err);

    // fill in the coordinates
    res->thispoint.x = get_float_array_item(err, MX_AREA_LINKED_THIS_X, index);
    MX_ERROR_CHECK(err);

    res->thispoint.y = get_float_array_item(err, MX_AREA_LINKED_THIS_Y, index);
    MX_ERROR_CHECK(err);

    res->otherpoint.x = get_float_array_item(err, MX_AREA_LINKED_OTHER_X, index);
    MX_ERROR_CHECK(err);

    res->otherpoint.y = get_float_array_item(err, MX_AREA_LINKED_OTHER_Y, index);
    MX_ERROR_CHECK(err);

    return res;
abort:
    return NULL;
}

// create a link
void mx_area::link_to_area(int& err, mx_area* linked_area, mx_point thispoint, mx_point otherpoint)
{
    uint32 link_id;

    link_id = linked_area->get_db_id(err);
    MX_ERROR_CHECK(err);

    add_float_array_item(err, MX_AREA_LINKED_THIS_X, thispoint.x);
    MX_ERROR_CHECK(err);

    add_float_array_item(err, MX_AREA_LINKED_THIS_Y, thispoint.y);
    MX_ERROR_CHECK(err);

    add_float_array_item(err, MX_AREA_LINKED_OTHER_X, otherpoint.x);
    MX_ERROR_CHECK(err);

    add_float_array_item(err, MX_AREA_LINKED_OTHER_Y, otherpoint.y);
    MX_ERROR_CHECK(err);

    add_id_array_item(err, MX_AREA_LINKED_AREAS, link_id);
    MX_ERROR_CHECK(err);

    link_id = get_db_id(err);
    MX_ERROR_CHECK(err);

    linked_area->add_float_array_item(err, MX_AREA_LINKED_OTHER_X, thispoint.x);
    MX_ERROR_CHECK(err);

    linked_area->add_float_array_item(err, MX_AREA_LINKED_OTHER_Y, thispoint.y);
    MX_ERROR_CHECK(err);

    linked_area->add_float_array_item(err, MX_AREA_LINKED_THIS_X, otherpoint.x);
    MX_ERROR_CHECK(err);

    linked_area->add_float_array_item(err, MX_AREA_LINKED_THIS_Y, otherpoint.y);
    MX_ERROR_CHECK(err);

    linked_area->add_id_array_item(err, MX_AREA_LINKED_AREAS, link_id);
    MX_ERROR_CHECK(err);

abort:;
}

// remove from the chain of areas
void mx_area::remove_from_chain(int& err)
{
    mx_area* next;
    mx_area* prev;
    uint32 prev_id;
    uint32 next_id;

    prev = mx_area::previous_in_chain(err);
    if (err == MX_AREA_NO_CHAIN) {
        MX_ERROR_CLEAR(err);
    } else {
        MX_ERROR_CHECK(err);
    }

    next = mx_area::next_in_chain(err);
    if (err == MX_AREA_NO_CHAIN) {
        MX_ERROR_CLEAR(err);
    } else {
        MX_ERROR_CHECK(err);
    }

    if (prev == NULL) {
        prev_id = 0;
    } else {
        prev_id = prev->get_db_id(err);
        MX_ERROR_CHECK(err);
    }

    if (next == NULL) {
        next_id = 0;
    } else {
        next_id = next->get_db_id(err);
        MX_ERROR_CHECK(err);
    }

    if ((prev_id == 0) && (next_id == 0)) {
        return;
    }

    // unlink this one from the chain
    set_id(err, MX_AREA_PREV_AREA, 0);
    MX_ERROR_CHECK(err);

    set_id(err, MX_AREA_NEXT_AREA, 0);
    MX_ERROR_CHECK(err);

    if (prev != NULL) {
        prev->set_id(err, MX_AREA_NEXT_AREA, next_id);
        MX_ERROR_CHECK(err);
    }

    if (next != NULL) {
        next->set_id(err, MX_AREA_PREV_AREA, prev_id);
        MX_ERROR_CHECK(err);
    }

abort:;
}

void mx_area::unlink_from_area(int& err, mx_area* area)
{
    uint32 link_id;
    uint32 i;
    const mx_attribute* a;

    link_id = area->get_db_id(err);
    MX_ERROR_CHECK(err);

    a = get_readonly(err, MX_AREA_LINKED_AREAS);
    MX_ERROR_CHECK(err);

    for (i = 0; i < a->length; i++) {
        if (a->value.ida[i] == link_id) {
            del_id_array_item(err, MX_AREA_LINKED_AREAS, i);
            MX_ERROR_CHECK(err);

            break;
        }
    }

    if (i >= a->length) {
        err = MX_AREA_NOT_LINKED;
        return;
    }

    link_id = get_db_id(err);
    MX_ERROR_CHECK(err);

    a = area->get_readonly(err, MX_AREA_LINKED_AREAS);
    MX_ERROR_CHECK(err);

    for (i = 0; i < a->length; i++) {
        if (a->value.ida[i] == link_id) {
            area->del_id_array_item(err, MX_AREA_LINKED_AREAS, i);
            MX_ERROR_CHECK(err);

            break;
        }
    }

    if (i >= a->length) {
        err = MX_AREA_NOT_LINKED;
        return;
    }
abort:;
}

// how high is the area in mm? This is a bounding box height
float mx_area::get_height(int& err)
{
    mx_rect r;

    r = bounding_box(err);
    if (err == MX_DB_CLIENT_CACHE_NO_SUCH_ATTR) {
        MX_ERROR_CLEAR(err);
        return 0.0;
    }
    MX_ERROR_CHECK(err);

    return r.yt - r.yb;
abort:
    return 0.0;
}

// how wide is the area in mm? This is a bounding box width
float mx_area::get_width(int& err)
{
    mx_rect r;

    r = bounding_box(err);
    if (err == MX_DB_CLIENT_CACHE_NO_SUCH_ATTR) {
        MX_ERROR_CLEAR(err);
        return 0.0;
    } else {
        MX_ERROR_CHECK(err);
    }

    return r.xt - r.xb;
abort:
    return 0.0;
}

// both together
mx_rect mx_area::bounding_box(int& err)
{
    mx_rect r;
    mx_point p;

    const mx_quadspline_t* pt;

    pt = get_outline_readonly(err);
    MX_ERROR_CHECK(err);

    if (pt->num_coords > 0) {
        r.xt = r.yt = -DBL_MAX;
        r.xb = r.yb = DBL_MAX;

        for (uint32 ic = 0; ic < pt->num_coords; ic++) {
            p = (pt->coords)[ic].p;
            if (p.x > r.xt)
                r.xt = p.x;
            if (p.y > r.yt)
                r.yt = p.y;
            if (p.x < r.xb)
                r.xb = p.x;
            if (p.y < r.yb)
                r.yb = p.y;
        }
    }

abort:
    return r;
}

void mx_area::set_outline(int& err, const mx_doc_rect_t& r)
{
    mx_spline_coord_t coords[4];
    mx_quadspline_t pp;

    pp.num_coords = 4;
    pp.coords = coords;

    coords[0].p.x = r.r.xt;
    coords[0].p.y = r.r.yt;
    coords[0].flag = TRUE;
    coords[1].p.x = r.r.xb;
    coords[1].p.y = r.r.yt;
    coords[1].flag = TRUE;
    coords[2].p.x = r.r.xb;
    coords[2].p.y = r.r.yb;
    coords[2].flag = TRUE;
    coords[3].p.x = r.r.xt;
    coords[3].p.y = r.r.yb;
    coords[3].flag = TRUE;

    set_outline(err, pp);
    MX_ERROR_CHECK(err);

abort:;
}

void mx_area::set_outline(int& err, float width, float height)
{
    mx_spline_coord_t coords[4];
    mx_quadspline_t pp;

    pp.num_coords = 4;
    pp.coords = coords;

    coords[0].p.x = 0.0;
    coords[0].p.y = 0.0;
    coords[0].flag = TRUE;
    coords[1].p.x = width;
    coords[1].p.y = 0.0;
    coords[1].flag = TRUE;
    coords[2].p.x = width;
    coords[2].p.y = height;
    coords[2].flag = TRUE;
    coords[3].p.x = 0.0;
    coords[3].p.y = height;
    coords[3].flag = TRUE;

    set_outline(err, pp);
    MX_ERROR_CHECK(err);

abort:;
}

void mx_area::reformat(int& err, bool& height_changed)
{
    height_changed = FALSE;
}

mx_document* mx_area::get_document(int& err)
{
    mx_sheet* s;
    mx_document* res;

    s = get_sheet(err);
    MX_ERROR_CHECK(err);

    if (s == NULL) {
        return NULL;
    } else {
        res = s->get_document(err);
        MX_ERROR_CHECK(err);

        return res;
    }

abort:;
    return NULL;
}

mx_sheet* mx_area::get_sheet(int& err)
{
    mx_sheet* res;

    res = (mx_sheet*)get_owner_object(err);
    if (err != MX_ERROR_OK) {
        MX_ERROR_CLEAR(err);
    } else {
        MX_ERROR_ASSERT(err, res->is_a(mx_sheet_class_e));
        return res;
    }
abort:
    return NULL;
}

static const uint32 num_attributes = 12;
static const char* attribute_names[num_attributes] = {
    MX_AREA_OUTLINE_X,
    MX_AREA_OUTLINE_Y,
    MX_AREA_OUTLINE_FLAG,
    MX_AREA_ORIGIN_X,
    MX_AREA_ORIGIN_Y,
    MX_AREA_ROTATION,
    MX_AREA_ORIGINAL_SIZE_X,
    MX_AREA_ORIGINAL_SIZE_Y,
    MX_AREA_TOP_LEFT_CROP_X,
    MX_AREA_TOP_LEFT_CROP_Y,
    MX_AREA_SCALE_X,
    MX_AREA_SCALE_Y
};

void mx_area::serialise(int& err, uint8** buffer)
{
    uint32 i;
    mx_attribute* attr = NULL;

    for (i = 0; i < num_attributes; i++) {
        attr = (mx_attribute*)get_readonly(err, attribute_names[i]);
        if (err == MX_DB_CLIENT_CACHE_NO_SUCH_ATTR) {
            MX_ERROR_CLEAR(err);
            serialise_bool(FALSE, buffer);
        } else {
            MX_ERROR_CHECK(err);
            serialise_bool(TRUE, buffer);

            attr->serialise(err, buffer);
            MX_ERROR_CHECK(err);
        }
    }

    serialise_string(base_style->get_name(), buffer);

    if (style_mod == NULL) {
        mx_paragraph_style_mod psm;
        psm.serialise(err, buffer);
    } else {
        style_mod->serialise(err, buffer);
    }
    MX_ERROR_CHECK(err);
abort:;
}

void mx_area::unserialise(int& err, uint8** buffer)
{
    mx_paragraph_style_mod psm;
    mx_attribute attr;
    bool b;
    uint32 i;
    mx_document* doc;
    char temp[MX_PARA_STYLE_MAX_NAME];

    for (i = 0; i < num_attributes; i++) {
        unserialise_bool(b, buffer);

        if (b) {
            attr.unserialise(err, buffer);
            MX_ERROR_CHECK(err);

            set(err, &attr);
            MX_ERROR_CHECK(err);
        }
    }

    // get the style name
    unserialise_string(temp, buffer);

    doc = get_document(err);
    MX_ERROR_CHECK(err);

    base_style = doc->get_named_style(err, temp);
    MX_ERROR_CHECK(err);

    psm.unserialise(err, buffer);
    MX_ERROR_CHECK(err);

    if (psm.is_null()) {
        if (style_mod != NULL) {
            delete style_mod;
            style_mod = NULL;
        }
    } else if (style_mod == NULL) {
        style_mod = new mx_paragraph_style_mod(psm);
    } else {
        *style_mod = psm;
    }
abort:;
}

uint32 mx_area::get_serialised_size(int& err)
{
    uint32 i, res = 0;
    mx_attribute* attr = NULL;

    res += num_attributes * SLS_BOOL;

    for (i = 0; i < num_attributes; i++) {
        attr = (mx_attribute*)get_readonly(err, attribute_names[i]);
        if (err == MX_DB_CLIENT_CACHE_NO_SUCH_ATTR) {
            MX_ERROR_CLEAR(err);
        } else {
            MX_ERROR_CHECK(err);

            res += attr->get_serialised_size(err);
            MX_ERROR_CHECK(err);
        }
    }

    res += SLS_STRING(base_style->get_name());
    if (style_mod == NULL) {
        mx_paragraph_style_mod psm;
        res += psm.get_serialised_size(err);
    } else {
        res += style_mod->get_serialised_size(err);
    }
    MX_ERROR_CHECK(err);

    return res;
abort:;
    return 0;
}

void mx_area::replace_style(int& err, mx_paragraph_style* style_to_replace,
    mx_paragraph_style* replacement_style)
{
    if (base_style == style_to_replace) {
        set_paragraph_style(replacement_style);
    }
}

void mx_area::set_paragraph_style(mx_paragraph_style* s)
{
    base_style = s;
}

void mx_area::modify_paragraph_style(mx_paragraph_style& new_style)
{
    mx_paragraph_style_mod psm;

    if (base_style == NULL) {
        psm = mx_paragraph_style_mod(mx_paragraph_style(), new_style);
    } else {
        psm = mx_paragraph_style_mod(*base_style, new_style);
    }

    if (psm.is_null()) {
        if (style_mod != NULL) {
            delete style_mod;
            style_mod = NULL;
        }
    } else if (style_mod == NULL) {
        style_mod = new mx_paragraph_style_mod(psm);
    } else {
        *style_mod = psm;
    }
}

mx_paragraph_style* mx_area::get_paragraph_style()
{
    return this->get_area_style();
}

float mx_area::width_minus_border(int& err)
{
    float res;
    mx_border_style* bs;

    res = get_width(err);
    MX_ERROR_CHECK(err);

    bs = this->get_area_style()->get_border_style();

    if (bs->left_style.line_type != mx_no_line) {
        res -= bs->left_style.width;
    }

    if (bs->right_style.line_type != mx_no_line) {
        res -= bs->right_style.width;
    }
    res -= 2 * bs->distance_from_contents;

    return res;
abort:;
    return 0.0;
}

mx_paragraph_style* mx_area::get_style(int& err, mx_area_pos* p)
{
    return this->get_area_style();
}

mx_paragraph_style* mx_area::get_area_style()
{
    static mx_paragraph_style* st = NULL;

    if (st == NULL) {
        st = new mx_paragraph_style;
    }

    if (base_style == NULL) {
        *st = mx_paragraph_style();
    } else {
        *st = *base_style;
    }

    if (style_mod != NULL) {
        *st += *style_mod;
    }
    return st;
}

void mx_area::set_width(int& err, float w)
{
    float h;

    h = get_height(err);
    MX_ERROR_CHECK(err);

    set_outline(err, w, h);
    MX_ERROR_CHECK(err);

abort:;
}

void mx_area::highlight_from_start_of_area(int& err, mx_area_pos* end)
{
    ((mx_area_painter*)painter)->highlight_from_start_of_area(err, this, end);
}

void mx_area::highlight_whole_area(int& err)
{
    ((mx_area_painter*)painter)->highlight_whole_area(err, this);
}

void mx_area::highlight_in_area(int& err, mx_area_pos* start, mx_area_pos* end)
{
    ((mx_area_painter*)painter)->highlight_in_area(err, this, start, end);
}

void mx_area::highlight_to_end_of_area(int& err, mx_area_pos* start)
{
    ((mx_area_painter*)painter)->highlight_to_end_of_area(err, this, start);
}
void mx_area::set_scale(int& err,
    mx_point& scale)
{
    err = MX_ERROR_OK;

    set_float(err, MX_AREA_SCALE_X, scale.x);
    MX_ERROR_CHECK(err);

    set_float(err, MX_AREA_SCALE_Y, scale.y);
    MX_ERROR_CHECK(err);

    return;
abort:
    return;
}

bool mx_area::get_scale(int& err, mx_point& scale)
{
    err = MX_ERROR_OK;

    scale.x = get_float(err, MX_AREA_SCALE_X);
    if (err == MX_DB_CLIENT_CACHE_NO_SUCH_ATTR) {
        MX_ERROR_CLEAR(err);

        scale.x = scale.y = 1;

        return FALSE;
    }
    MX_ERROR_CHECK(err);

    scale.y = get_float(err, MX_AREA_SCALE_Y);
    MX_ERROR_CHECK(err);

    return TRUE;

abort:
    return FALSE;
}

void mx_area::set_original_size(int& err, mx_point& original_size)
{
    mx_point osize;
    bool found;

    err = MX_ERROR_OK;

    // this can only be set once

    found = get_original_size(err, osize);
    MX_ERROR_CHECK(err);

    if (found)
        return;

    set_float(err, MX_AREA_ORIGINAL_SIZE_X, original_size.x);
    MX_ERROR_CHECK(err);

    set_float(err, MX_AREA_ORIGINAL_SIZE_Y, original_size.y);
    MX_ERROR_CHECK(err);

    return;
abort:
    return;
}

bool mx_area::get_original_size(int& err, mx_point& original_size)
{
    err = MX_ERROR_OK;

    original_size.x = get_float(err, MX_AREA_ORIGINAL_SIZE_X);
    if (err == MX_DB_CLIENT_CACHE_NO_SUCH_ATTR) {
        MX_ERROR_CLEAR(err);

        original_size.x = get_width(err);
        MX_ERROR_CHECK(err);

        original_size.y = get_height(err);
        MX_ERROR_CHECK(err);

        return FALSE;
    }
    MX_ERROR_CHECK(err);

    original_size.y = get_float(err, MX_AREA_ORIGINAL_SIZE_Y);
    MX_ERROR_CHECK(err);

    return TRUE;
abort:
    return FALSE;
}

void mx_area::set_top_left_crop(int& err, mx_point& top_left_crop)
{
    err = MX_ERROR_OK;

    set_float(err, MX_AREA_TOP_LEFT_CROP_X, top_left_crop.x);
    MX_ERROR_CHECK(err);

    set_float(err, MX_AREA_TOP_LEFT_CROP_Y, top_left_crop.y);
    MX_ERROR_CHECK(err);

    return;
abort:
    return;
}

bool mx_area::get_top_left_crop(int& err, mx_point& top_left_crop)
{
    err = MX_ERROR_OK;

    top_left_crop.x = get_float(err, MX_AREA_TOP_LEFT_CROP_X);
    if (err == MX_DB_CLIENT_CACHE_NO_SUCH_ATTR) {
        MX_ERROR_CLEAR(err);

        top_left_crop.x = 0;
        top_left_crop.y = 0;

        return FALSE;
    }
    MX_ERROR_CHECK(err);

    top_left_crop.y = get_float(err, MX_AREA_TOP_LEFT_CROP_Y);
    MX_ERROR_CHECK(err);

    return TRUE;
abort:
    return FALSE;
}

void mx_area::scale(int& err,
    mx_point& new_size)
{
    mx_point new_scale;
    mx_point old_size;
    mx_point old_scale;
    bool found1, found2;
    mx_point scale_factor;
    mx_point top_left;

    err = MX_ERROR_OK;

    // get the old scale
    found1 = get_scale(err, old_scale);
    MX_ERROR_CHECK(err);

    // if scaling need to reset the top left too
    found2 = get_top_left_crop(err, top_left);
    MX_ERROR_CHECK(err);

    old_size.x = get_width(err);
    MX_ERROR_CHECK(err);

    old_size.y = get_height(err);
    MX_ERROR_CHECK(err);

    scale_factor.x = new_size.x / old_size.x;
    scale_factor.y = new_size.y / old_size.y;
    new_scale.x = old_scale.x * scale_factor.x;
    new_scale.y = old_scale.y * scale_factor.y;

    if (found2) {
        top_left.x *= scale_factor.x;
        top_left.y *= scale_factor.y;

        set_top_left_crop(err, top_left);
        MX_ERROR_CHECK(err);
    }

    set_scale(err, new_scale);
    MX_ERROR_CHECK(err);

    if ((!found1) && (!found2)) {
        set_original_size(err, old_size);
        MX_ERROR_CHECK(err);
    }

abort:
    return;
}

void mx_area::crop(int& err,
    const mx_rect& crop)
{
    mx_point origin;
    mx_point top_left_crop;
    bool found;

    err = MX_ERROR_OK;

    origin = get_origin(err);
    MX_ERROR_CHECK(err);

    found = get_top_left_crop(err, top_left_crop);
    MX_ERROR_CHECK(err);

    top_left_crop.x -= (crop.xb - origin.x);
    top_left_crop.y -= (crop.yb - origin.y);

    set_top_left_crop(err, top_left_crop);
    MX_ERROR_CHECK(err);

    if (!found) {
        mx_point original_size;

        original_size.x = get_width(err);
        MX_ERROR_CHECK(err);

        original_size.y = get_height(err);
        MX_ERROR_CHECK(err);

        set_original_size(err, original_size);
        MX_ERROR_CHECK(err);
    }

    return;

abort:
    return;
}

/*-------------------------------------------------
 * FUNCTION: mx_area::crop_details
 *
 * DESCRIPTION: Get the crop details in frame mms
 *
 *
 */

void mx_area::crop_details(int& err,
    mx_point& top_left,
    mx_point& total_size)
{
    mx_point image_scale;

    err = MX_ERROR_OK;

    get_scale(err, image_scale);
    MX_ERROR_CHECK(err);

    get_top_left_crop(err, top_left);
    MX_ERROR_CHECK(err);

    get_original_size(err, total_size);
    MX_ERROR_CHECK(err);

    total_size.x *= image_scale.x;
    total_size.y *= image_scale.y;

abort:
    return;
}

bool mx_area::has_page_break_before(int& err)
{
    return this->get_area_style()->page_break_before;
}

bool mx_area::can_join(int& err, mx_area* a)
{
    return FALSE;
}

void mx_area::join(int& err, mx_area* a)
{
    MX_ERROR_THROW(err, MX_ERROR_NOT_IMPLEMENTED);
abort:;
}
