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
 * MODULE/CLASS : mx_sheet
 *
 * AUTHOR : Andrew Haisley
 *
 * This file
 *
 * DESCRIPTION:
 *
 * Sheets are containment objects for areas.
 *
 *
 *
 */
#include <mx.h>
#include <mx_db.h>
#include <mx_list_iter.h>

#include <mx_area.h>
#include <mx_dg_area.h>
#include <mx_document.h>
#include <mx_image_area.h>
#include <mx_paintable.h>
#include <mx_sh_paint.h>
#include <mx_table_area.h>
#include <mx_text_area.h>

// constructor
mx_sheet::mx_sheet(uint32 docid)
    : mx_paintable(docid)
{
    num_diagram_areas = num_image_areas = num_table_areas = num_text_areas = 0;

    painter = mx_sheet_painter::painter();
    areas = new mx_list();
    index_built = FALSE;
}

// constructor for when the oid is known
mx_sheet::mx_sheet(uint32 docid, uint32 oid)
    : mx_paintable(docid, oid)
{
    num_diagram_areas = num_image_areas = num_table_areas = num_text_areas = 0;

    painter = mx_sheet_painter::painter();
    index_built = FALSE;
    areas = new mx_list();
}

mx_sheet::~mx_sheet()
{
    delete areas;
}

void mx_sheet::delete_areas(int& err)
{
    int i;

    err = MX_ERROR_OK;

    mx_text_area* ta;
    mx_table_area* tba;
    mx_image_area* ia;
    mx_diagram_area* da;

    (void)get_num_image_areas(err);
    MX_ERROR_CHECK(err);

    (void)get_num_text_areas(err);
    MX_ERROR_CHECK(err);

    (void)get_num_table_areas(err);
    MX_ERROR_CHECK(err);

    (void)get_num_diagram_areas(err);
    MX_ERROR_CHECK(err);

    err = MX_ERROR_OK;

    for (i = 0; i < num_diagram_areas; i++) {
        da = diagram_area(err, 0);
        MX_ERROR_CHECK(err);

        da->delete_links(err);
        delete da;
        MX_ERROR_CHECK(err);
    }

    for (i = 0; i < num_image_areas; i++) {

        ia = image_area(err, 0);
        MX_ERROR_CHECK(err);

        ia->delete_links(err);
        delete ia;
        MX_ERROR_CHECK(err);
    }

    for (i = 0; i < num_text_areas; i++) {

        ta = text_area(err, 0);
        MX_ERROR_CHECK(err);

        ta->delete_links(err);
        delete ta;
        MX_ERROR_CHECK(err);
    }

    for (i = 0; i < num_table_areas; i++) {

        tba = table_area(err, 0);
        MX_ERROR_CHECK(err);

        tba->delete_links(err);
        delete tba;
        MX_ERROR_CHECK(err);
    }

    return;

abort:;
}

int mx_sheet::get_num_text_areas(int& err)
{
    err = MX_ERROR_OK;

    num_text_areas = get_id_array_length(err, MX_SHEET_TEXT_AREAS);
    MX_ERROR_CHECK(err);

    return num_text_areas;

abort:;
    return -1;
}

int mx_sheet::get_num_table_areas(int& err)
{
    err = MX_ERROR_OK;

    num_table_areas = get_id_array_length(err, MX_SHEET_TABLE_AREAS);
    MX_ERROR_CHECK(err);

    return num_table_areas;

abort:;
    return -1;
}

mx_text_area* mx_sheet::text_area(int& err, int index)
{
    int na;
    mx_text_area* t;
    uint32 oid;

    err = MX_ERROR_OK;

    na = get_num_text_areas(err);
    MX_ERROR_CHECK(err);

    if (index < 0 || index >= na) {
        err = MX_SHEET_AREA_INDEX;
        return NULL;
    } else {
        oid = get_id_array_item(err, MX_SHEET_TEXT_AREAS, index);
        MX_ERROR_CHECK(err);

        t = (mx_text_area*)mx_db_cc_get_obj_from_oid(err, doc_id, oid);
        if (err == MX_DB_CLIENT_CACHE_NO_SUCH_OID) {
            MX_ERROR_CLEAR(err);

            t = new mx_text_area(doc_id, oid);

            if (t->error != MX_ERROR_OK) {
                err = t->error;
                return NULL;
            }

            t->set_owner_object(err, this);
            MX_ERROR_CHECK(err);

            return t;
        } else {
            MX_ERROR_CHECK(err);
            return t;
        }
    }
abort:
    return NULL;
}

mx_table_area* mx_sheet::table_area(int& err, int index)
{
    int na;
    mx_table_area* t;
    uint32 oid;

    err = MX_ERROR_OK;

    na = get_num_table_areas(err);
    MX_ERROR_CHECK(err);

    if (index < 0 || index >= na) {
        err = MX_SHEET_AREA_INDEX;
        return NULL;
    } else {
        oid = get_id_array_item(err, MX_SHEET_TABLE_AREAS, index);
        MX_ERROR_CHECK(err);

        t = (mx_table_area*)mx_db_cc_get_obj_from_oid(err, doc_id, oid);
        if (err == MX_DB_CLIENT_CACHE_NO_SUCH_OID) {
            MX_ERROR_CLEAR(err);

            t = new mx_table_area(doc_id, oid);

            if (t->error != MX_ERROR_OK) {
                err = t->error;
                return NULL;
            }

            t->set_owner_object(err, this);
            MX_ERROR_CHECK(err);

            return t;
        } else {
            MX_ERROR_CHECK(err);
            return t;
        }
    }
abort:
    return NULL;
}

int mx_sheet::get_num_image_areas(int& err)
{
    err = MX_ERROR_OK;

    num_image_areas = get_id_array_length(err, MX_SHEET_IMAGE_AREAS);
    MX_ERROR_CHECK(err);

    return num_image_areas;

abort:;
    return -1;
}

mx_image_area* mx_sheet::image_area(int& err, int index)
{
    int na;
    mx_image_area* t;
    uint32 oid;

    err = MX_ERROR_OK;

    na = get_num_image_areas(err);
    MX_ERROR_CHECK(err);

    if (index < 0 || index >= na) {
        err = MX_SHEET_AREA_INDEX;
        return NULL;
    } else {
        oid = get_id_array_item(err, MX_SHEET_IMAGE_AREAS, index);
        MX_ERROR_CHECK(err);

        t = (mx_image_area*)mx_db_cc_get_obj_from_oid(err, doc_id, oid);
        if (err == MX_DB_CLIENT_CACHE_NO_SUCH_OID) {
            MX_ERROR_CLEAR(err);

            t = new mx_image_area(doc_id, oid);

            if (t->error != MX_ERROR_OK) {
                err = t->error;
                return NULL;
            }

            return t;
        } else {
            MX_ERROR_CHECK(err);
            return t;
        }
    }
abort:
    return NULL;
}

int mx_sheet::get_num_diagram_areas(int& err)
{
    err = MX_ERROR_OK;

    num_diagram_areas = get_id_array_length(err, MX_SHEET_DIAGRAM_AREAS);
    MX_ERROR_CHECK(err);

    return num_diagram_areas;

abort:;
    return -1;
}

mx_diagram_area* mx_sheet::diagram_area(int& err, int index)
{
    int na;
    mx_diagram_area* t;
    uint32 oid;

    err = MX_ERROR_OK;

    na = get_num_diagram_areas(err);
    MX_ERROR_CHECK(err);

    if (index < 0 || index >= na) {
        err = MX_SHEET_AREA_INDEX;
        return NULL;
    } else {
        oid = get_id_array_item(err, MX_SHEET_DIAGRAM_AREAS, index);
        MX_ERROR_CHECK(err);

        t = (mx_diagram_area*)mx_db_cc_get_obj_from_oid(err, doc_id, oid);
        if (err == MX_DB_CLIENT_CACHE_NO_SUCH_OID) {
            MX_ERROR_CLEAR(err);

            t = new mx_diagram_area(doc_id, oid);

            if (t->error != MX_ERROR_OK) {
                err = t->error;
                return NULL;
            }

            return t;
        } else {
            MX_ERROR_CHECK(err);
            return t;
        }
    }
abort:
    return NULL;
}

mx_text_area* mx_sheet::add_text_area(int& err, int index_position)
{
    mx_text_area* area;
    uint32 oid;
    int na;

    mx_paragraph_style* s;
    mx_document* doc;

    float width, top, left, right, bottom;

    err = MX_ERROR_OK;

    na = get_num_text_areas(err);
    MX_ERROR_CHECK(err);

    if (na >= MX_ATTRIBUTE_MAX_ID_ARRAY_LENGTH) {
        err = MX_SHEET_TOO_MANY_TEXT_AREAS;
        return NULL;
    }

    doc = get_document(err);
    MX_ERROR_CHECK(err);

    s = doc->get_default_style(err);
    MX_ERROR_CHECK(err);

    area = new mx_text_area(doc_id, s);

    if (area->error != MX_ERROR_OK) {
        err = area->error;
        return NULL;
    }

    area->move_to_document(err, doc);
    MX_ERROR_CHECK(err);

    area->set_owner_object(err, this);
    MX_ERROR_CHECK(err);

    oid = area->get_db_id(err);
    MX_ERROR_CHECK(err);

    add_id_array_item(err, MX_SHEET_TEXT_AREAS, oid);
    MX_ERROR_CHECK(err);

    width = get_width(err);
    MX_ERROR_CHECK(err);

    get_margins(err, left, right, top, bottom);
    MX_ERROR_CHECK(err);

    area->set_outline(err, width - (left + right), 1.0);
    MX_ERROR_CHECK(err);

    if (index_position == -1) {
        build_area_index(err);
        MX_ERROR_CHECK(err);
    } else {
        areas->insert(err, index_position, area);
        MX_ERROR_CHECK(err);
    }

    return area;

abort:
    return NULL;
}

mx_table_area* mx_sheet::add_table_area(int& err, int index_position)
{
    mx_table_area* area;
    uint32 oid;
    int na;

    mx_document* doc;
    mx_paragraph_style* s;

    err = MX_ERROR_OK;

    na = get_num_table_areas(err);
    MX_ERROR_CHECK(err);

    if (na >= MX_ATTRIBUTE_MAX_ID_ARRAY_LENGTH) {
        err = MX_SHEET_TOO_MANY_TABLE_AREAS;
        return NULL;
    }

    doc = get_document(err);
    MX_ERROR_CHECK(err);

    s = doc->get_named_style(err, "default table text");
    MX_ERROR_CHECK(err);

    area = new mx_table_area(doc_id, s);

    if (area->error != MX_ERROR_OK) {
        err = area->error;
        return NULL;
    }

    area->move_to_document(err, doc);
    MX_ERROR_CHECK(err);

    area->set_owner_object(err, this);
    MX_ERROR_CHECK(err);

    oid = area->get_db_id(err);
    MX_ERROR_CHECK(err);

    add_id_array_item(err, MX_SHEET_TABLE_AREAS, oid);
    MX_ERROR_CHECK(err);

    if (index_position == -1) {
        build_area_index(err);
        MX_ERROR_CHECK(err);
    } else {
        areas->insert(err, index_position, area);
        MX_ERROR_CHECK(err);
    }

    return area;

abort:
    return NULL;
}

mx_image_area* mx_sheet::add_image_area(int& err, int index_position)
{
    mx_image_area* area;
    uint32 oid;
    int na;

    mx_document* doc;
    mx_paragraph_style* s;

    doc = get_document(err);
    MX_ERROR_CHECK(err);

    s = doc->get_default_style(err);
    MX_ERROR_CHECK(err);

    na = get_num_image_areas(err);
    MX_ERROR_CHECK(err);

    if (na >= MX_ATTRIBUTE_MAX_ID_ARRAY_LENGTH) {
        err = MX_SHEET_TOO_MANY_IMAGE_AREAS;
        return NULL;
    }

    area = new mx_image_area(doc_id, s);

    if (area->error != MX_ERROR_OK) {
        err = area->error;
        return NULL;
    }

    area->move_to_document(err, doc);
    MX_ERROR_CHECK(err);

    area->set_owner_object(err, this);
    MX_ERROR_CHECK(err);

    oid = area->get_db_id(err);
    MX_ERROR_CHECK(err);

    add_id_array_item(err, MX_SHEET_IMAGE_AREAS, oid);
    MX_ERROR_CHECK(err);

    if (index_position == -1) {
        build_area_index(err);
        MX_ERROR_CHECK(err);
    } else {
        areas->insert(err, index_position, area);
        MX_ERROR_CHECK(err);
    }
    return area;

abort:
    return NULL;
}

mx_diagram_area* mx_sheet::add_diagram_area(int& err, int index_position)
{
    mx_diagram_area* area;
    uint32 oid;
    int na;

    mx_paragraph_style* s;
    mx_document* doc;

    err = MX_ERROR_OK;

    na = get_num_diagram_areas(err);
    MX_ERROR_CHECK(err);

    doc = get_document(err);
    MX_ERROR_CHECK(err);

    s = doc->get_default_style(err);
    MX_ERROR_CHECK(err);

    if (na >= MX_ATTRIBUTE_MAX_ID_ARRAY_LENGTH) {
        err = MX_SHEET_TOO_MANY_DIAGRAM_AREAS;
        return NULL;
    }

    area = new mx_diagram_area(doc_id, s);

    if (area->error != MX_ERROR_OK) {
        err = area->error;
        return NULL;
    }

    area->move_to_document(err, doc);
    MX_ERROR_CHECK(err);

    area->set_owner_object(err, this);
    MX_ERROR_CHECK(err);

    oid = area->get_db_id(err);
    MX_ERROR_CHECK(err);

    add_id_array_item(err, MX_SHEET_DIAGRAM_AREAS, oid);
    MX_ERROR_CHECK(err);

    if (index_position == -1) {
        build_area_index(err);
        MX_ERROR_CHECK(err);
    } else {
        areas->insert(err, index_position, area);
        MX_ERROR_CHECK(err);
    }

    return area;

abort:
    return NULL;
}

float mx_sheet::get_width(int& err)
{
    float f;

    err = MX_ERROR_OK;

    f = get_default_float(err, MX_SHEET_WIDTH, 0.0);
    MX_ERROR_CHECK(err);

    return f;
abort:;
    return 0.0;
}

void mx_sheet::set_width(int& err, float width)
{
    mx_attribute da;

    err = MX_ERROR_OK;

    da.type = mx_float;
    da.value.f = width;
    da.length = 0;
    strcpy(da.name, MX_SHEET_WIDTH);

    set(err, &da);
    MX_ERROR_CHECK(err);
abort:;
}

float mx_sheet::get_height(int& err)
{
    float f;

    err = MX_ERROR_OK;

    if (!index_built) {
        build_area_index(err);
        MX_ERROR_CHECK(err);
    }

    f = get_default_float(err, MX_SHEET_HEIGHT, 0.0);
    MX_ERROR_CHECK(err);

    return f;
abort:;
    return 0.0;
}

void mx_sheet::set_height(int& err, float height)
{
    mx_attribute da;

    err = MX_ERROR_OK;

    da.type = mx_float;
    da.value.f = height;
    da.length = 0;
    strcpy(da.name, MX_SHEET_HEIGHT);

    set(err, &da);
    MX_ERROR_CHECK(err);
abort:;
}

void mx_sheet::delete_text_area(int& err, int index)
{
    int i, na;
    uint32 oid;
    mx_text_area* t;

    err = MX_ERROR_OK;

    na = get_num_text_areas(err);
    MX_ERROR_CHECK(err);

    if ((index < 0) || (index >= na)) {
        err = MX_SHEET_AREA_INDEX;
        return;
    } else {
        oid = get_id_array_item(err, MX_SHEET_TEXT_AREAS, index);
        MX_ERROR_CHECK(err);

        t = (mx_text_area*)mx_db_cc_get_obj_from_oid(err, doc_id, oid);
        if (err == MX_DB_CLIENT_CACHE_NO_SUCH_OID) {
            // never got cached, so can ignore the C++ object part
            MX_ERROR_CLEAR(err);

            mx_db_cc_delete_oid(err, doc_id, oid);
            MX_ERROR_CHECK(err);
        } else {
            i = areas->find_index(err, t);
            MX_ERROR_CHECK(err);

            areas->remove(err, i);
            MX_ERROR_CHECK(err);

            MX_ERROR_CHECK(err);
            delete t;
        }

        del_id_array_item(err, MX_SHEET_TEXT_AREAS, index);
        MX_ERROR_CHECK(err);
    }

abort:;
}

void mx_sheet::delete_table_area(int& err, int index)
{
    int i, na;
    uint32 oid;
    mx_table_area* t;

    err = MX_ERROR_OK;

    na = get_num_table_areas(err);
    MX_ERROR_CHECK(err);

    if ((index < 0) || (index >= na)) {
        err = MX_SHEET_AREA_INDEX;
        return;
    } else {
        oid = get_id_array_item(err, MX_SHEET_TABLE_AREAS, index);
        MX_ERROR_CHECK(err);

        t = (mx_table_area*)mx_db_cc_get_obj_from_oid(err, doc_id, oid);
        if (err == MX_DB_CLIENT_CACHE_NO_SUCH_OID) {
            // never got cached, so can ignore the C++ object part
            MX_ERROR_CLEAR(err);

            mx_db_cc_delete_oid(err, doc_id, oid);
            MX_ERROR_CHECK(err);
        } else {
            MX_ERROR_CHECK(err);

            i = areas->find_index(err, t);
            MX_ERROR_CHECK(err);

            areas->remove(err, i);
            MX_ERROR_CHECK(err);

            t->delete_cells(err);
            MX_ERROR_CHECK(err);
            delete t;
        }

        del_id_array_item(err, MX_SHEET_TABLE_AREAS, index);
        MX_ERROR_CHECK(err);
    }

abort:;
}

void mx_sheet::delete_image_area(int& err, int index)
{
    int i, na;
    uint32 oid;
    mx_image_area* t;

    err = MX_ERROR_OK;

    na = get_num_image_areas(err);
    MX_ERROR_CHECK(err);

    if ((index < 0) || (index >= na)) {
        err = MX_SHEET_AREA_INDEX;
        return;
    } else {
        oid = get_id_array_item(err, MX_SHEET_IMAGE_AREAS, index);
        MX_ERROR_CHECK(err);

        t = (mx_image_area*)mx_db_cc_get_obj_from_oid(err, doc_id, oid);
        if (err == MX_DB_CLIENT_CACHE_NO_SUCH_OID) {
            // never got cached, so can ignore the C++ object part
            MX_ERROR_CLEAR(err);

            mx_db_cc_delete_oid(err, doc_id, oid);
            MX_ERROR_CHECK(err);
        } else {
            MX_ERROR_CHECK(err);

            i = areas->find_index(err, t);
            MX_ERROR_CHECK(err);

            areas->remove(err, i);
            MX_ERROR_CHECK(err);

            delete t;
        }

        del_id_array_item(err, MX_SHEET_IMAGE_AREAS, index);
        MX_ERROR_CHECK(err);
    }

abort:;
}

void mx_sheet::delete_diagram_area(int& err, int index)
{
    int i, na;
    uint32 oid;
    mx_diagram_area* t;

    err = MX_ERROR_OK;

    na = get_num_diagram_areas(err);
    MX_ERROR_CHECK(err);

    if ((index < 0) || (index >= na)) {
        err = MX_SHEET_AREA_INDEX;
        return;
    } else {
        oid = get_id_array_item(err, MX_SHEET_DIAGRAM_AREAS, index);
        MX_ERROR_CHECK(err);

        t = (mx_diagram_area*)mx_db_cc_get_obj_from_oid(err, doc_id, oid);
        if (err == MX_DB_CLIENT_CACHE_NO_SUCH_OID) {
            // never got cached, so can ignore the C++ object part
            MX_ERROR_CLEAR(err);

            mx_db_cc_delete_oid(err, doc_id, oid);
            MX_ERROR_CHECK(err);
        } else {
            MX_ERROR_CHECK(err);

            i = areas->find_index(err, t);
            MX_ERROR_CHECK(err);

            areas->remove(err, i);
            MX_ERROR_CHECK(err);

            delete t;
        }

        del_id_array_item(err, MX_SHEET_DIAGRAM_AREAS, index);
        MX_ERROR_CHECK(err);
    }

abort:;
}

void mx_sheet::delete_text_area_ref(int& err, mx_text_area* a)
{
    int n, i, oid, x;

    err = MX_ERROR_OK;

    n = get_num_text_areas(err);
    MX_ERROR_CHECK(err);

    x = a->get_db_id(err);
    MX_ERROR_CHECK(err);

    for (i = 0; i < n; i++) {
        oid = get_id_array_item(err, MX_SHEET_TEXT_AREAS, i);
        MX_ERROR_CHECK(err);

        if (oid == x) {
            delete_text_area(err, i);
            MX_ERROR_CHECK(err);

            return;
        }
    }
abort:;
}

void mx_sheet::delete_table_area_ref(int& err, mx_table_area* a)
{
    int n, i, oid, x;

    err = MX_ERROR_OK;

    n = get_num_table_areas(err);
    MX_ERROR_CHECK(err);

    x = a->get_db_id(err);
    MX_ERROR_CHECK(err);

    for (i = 0; i < n; i++) {
        oid = get_id_array_item(err, MX_SHEET_TABLE_AREAS, i);
        MX_ERROR_CHECK(err);

        if (oid == x) {
            delete_table_area(err, i);
            MX_ERROR_CHECK(err);

            return;
        }
    }
abort:;
}

void mx_sheet::delete_image_area_ref(int& err, mx_image_area* a)
{
    int n, i, oid, x;

    err = MX_ERROR_OK;

    n = get_num_image_areas(err);
    MX_ERROR_CHECK(err);

    x = a->get_db_id(err);
    MX_ERROR_CHECK(err);

    for (i = 0; i < n; i++) {
        oid = get_id_array_item(err, MX_SHEET_IMAGE_AREAS, i);
        MX_ERROR_CHECK(err);

        if (oid == x) {
            delete_image_area(err, i);
            MX_ERROR_CHECK(err);

            return;
        }
    }
abort:;
}

void mx_sheet::delete_diagram_area_ref(int& err, mx_diagram_area* a)
{
    int n, i, oid, x;

    err = MX_ERROR_OK;

    n = get_num_diagram_areas(err);
    MX_ERROR_CHECK(err);

    x = a->get_db_id(err);
    MX_ERROR_CHECK(err);

    for (i = 0; i < n; i++) {
        oid = get_id_array_item(err, MX_SHEET_DIAGRAM_AREAS, i);
        MX_ERROR_CHECK(err);

        if (oid == x) {
            delete_diagram_area(err, i);
            MX_ERROR_CHECK(err);

            return;
        }
    }
abort:;
}

void mx_sheet::move_area(int& err, mx_area* area, mx_sheet* dest, const char* attr_name)
{
    int id, i, n, temp;

    err = MX_ERROR_OK;

    id = area->get_db_id(err);
    MX_ERROR_CHECK(err);

    n = get_id_array_length(err, attr_name);
    MX_ERROR_CHECK(err);

    for (i = 0; i < n; i++) {
        temp = get_id_array_item(err, attr_name, i);
        MX_ERROR_CHECK(err);

        if (temp == id) {
            del_id_array_item(err, attr_name, i);
            MX_ERROR_CHECK(err);

            dest->add_id_array_item(err, attr_name, id);
            MX_ERROR_CHECK(err);

            area->set_owner_object(err, dest);
            MX_ERROR_CHECK(err);

            return;
        }
    }

    err = MX_SHEET_NO_SUCH_AREA;

abort:;
}

void mx_sheet::move_text_area(int& err, mx_text_area* area, mx_sheet* dest)
{
    move_area(err, area, dest, MX_SHEET_TEXT_AREAS);
    MX_ERROR_CHECK(err);
abort:;
}

void mx_sheet::move_table_area(int& err, mx_table_area* area, mx_sheet* dest)
{
    move_area(err, area, dest, MX_SHEET_TABLE_AREAS);
    MX_ERROR_CHECK(err);
abort:;
}

void mx_sheet::move_image_area(int& err, mx_image_area* area, mx_sheet* dest)
{
    move_area(err, area, dest, MX_SHEET_IMAGE_AREAS);
    MX_ERROR_CHECK(err);
abort:;
}

void mx_sheet::move_diagram_area(int& err, mx_diagram_area* area, mx_sheet* dest)
{
    move_area(err, area, dest, MX_SHEET_DIAGRAM_AREAS);
    MX_ERROR_CHECK(err);
abort:;
}

void mx_sheet::build_area_index(int& err)
{
    int i, n;
    mx_area* a;

    if (areas->get_num_items() != 0) {
        index_built = TRUE;
        return;
    }

    // get rid of the old index
    delete areas;
    areas = new mx_list();

    n = get_num_text_areas(err);
    MX_ERROR_CHECK(err);

    for (i = 0; i < n; i++) {
        a = text_area(err, i);
        MX_ERROR_CHECK(err);

        add_area_to_index(err, a);
        MX_ERROR_CHECK(err);
    }

    n = get_num_image_areas(err);
    MX_ERROR_CHECK(err);

    for (i = 0; i < n; i++) {
        a = image_area(err, i);
        MX_ERROR_CHECK(err);

        add_area_to_index(err, a);
        MX_ERROR_CHECK(err);
    }

    n = get_num_table_areas(err);
    MX_ERROR_CHECK(err);

    for (i = 0; i < n; i++) {
        a = table_area(err, i);
        MX_ERROR_CHECK(err);

        add_area_to_index(err, a);
        MX_ERROR_CHECK(err);
    }

    n = get_num_diagram_areas(err);
    MX_ERROR_CHECK(err);

    for (i = 0; i < n; i++) {
        a = diagram_area(err, i);
        MX_ERROR_CHECK(err);

        add_area_to_index(err, a);
        MX_ERROR_CHECK(err);
    }

    index_built = TRUE;
abort:;
}

void mx_sheet::add_area_to_index(int& err, mx_area* a)
{
    mx_point o1, o2;
    mx_area* ta;

    int i, n;

    o1 = a->get_origin(err);
    MX_ERROR_CHECK(err);

    n = areas->get_num_items();

    for (i = 0; i < n; i++) {
        ta = (mx_area*)areas->get(err, i);
        MX_ERROR_CHECK(err);

        o2 = ta->get_origin(err);
        MX_ERROR_CHECK(err);

        if (o1.y < o2.y) {
            areas->insert(err, i, a);
            MX_ERROR_CHECK(err);

            return;
        }
    }

    areas->append(a);

abort:;
}

mx_list* mx_sheet::get_area_index(int& err)
{
    if (!index_built) {
        build_area_index(err);
        MX_ERROR_CHECK(err);
    }
abort:
    return areas;
}

void mx_sheet::set_margins(int& err, float left, float right, float top, float bottom)
{
    set_float(err, MX_SHEET_LEFT_MARGIN, left);
    MX_ERROR_CHECK(err);

    set_float(err, MX_SHEET_RIGHT_MARGIN, right);
    MX_ERROR_CHECK(err);

    set_float(err, MX_SHEET_TOP_MARGIN, top);
    MX_ERROR_CHECK(err);

    set_float(err, MX_SHEET_BOTTOM_MARGIN, bottom);
    MX_ERROR_CHECK(err);
abort:;
}

void mx_sheet::get_margins(int& err, float& left, float& right, float& top, float& bottom)
{
    left = right = top = bottom = 0;

    left = get_default_float(err, MX_SHEET_LEFT_MARGIN, 0.0);
    MX_ERROR_CHECK(err);

    right = get_default_float(err, MX_SHEET_RIGHT_MARGIN, 0.0);
    MX_ERROR_CHECK(err);

    top = get_default_float(err, MX_SHEET_TOP_MARGIN, 0.0);
    MX_ERROR_CHECK(err);

    bottom = get_default_float(err, MX_SHEET_BOTTOM_MARGIN, 0.0);
    MX_ERROR_CHECK(err);
abort:;
}

mx_document* mx_sheet::get_document(int& err)
{
    return (mx_document*)get_owner_object(err);
}

float mx_sheet::get_height(int& err, mx_painter_mode_t mode)
{
    float height, left, right, top, bottom, lowest, area_height;
    mx_list_iterator* iter = NULL;
    mx_point o;
    mx_area* a;

    height = get_height(err);
    MX_ERROR_CHECK(err);

    if (mode == mx_paint_wp_full_e) {
        return height;
    }

    get_margins(err, left, right, top, bottom);
    MX_ERROR_CHECK(err);

    iter = new mx_list_iterator(*areas);

    lowest = 0.0;
    while (iter->more()) {
        a = (mx_area*)iter->data();

        o = a->get_origin(err);
        MX_ERROR_CHECK(err);

        area_height = a->get_height(err);
        MX_ERROR_CHECK(err);

        if ((o.y + area_height) > lowest) {
            lowest = o.y + area_height;
        }
    }

    delete iter;

    return lowest - top;

abort:;
    if (iter != NULL) {
        delete iter;
    }
    return 0.0;
}

float mx_sheet::get_width(int& err, mx_painter_mode_t mode)
{
    float width, left, right, top, bottom;

    width = get_width(err);
    MX_ERROR_CHECK(err);

    switch (mode) {
    default:
    case mx_paint_wp_normal_e:
    case mx_paint_wp_codes_e:
        get_margins(err, left, right, top, bottom);
        MX_ERROR_CHECK(err);

        return width - (left + right);
    case mx_paint_wp_full_e:
        return width;
    }
abort:
    return 0.0;
}

void mx_sheet::move_area_out(int& err, int area_index, mx_sheet* dest)
{
    mx_area* area = (mx_area*)areas->get(err, area_index);
    MX_ERROR_CHECK(err);

    move_area_out(err, area, dest);
    MX_ERROR_CHECK(err);

abort:;
}

void mx_sheet::move_area_in(int& err, int area_index, mx_sheet* src)
{
    mx_area* area = (mx_area*)(src->areas->get(err, area_index));
    MX_ERROR_CHECK(err);

    move_area_in(err, area, src);
    MX_ERROR_CHECK(err);

abort:;
}

mx_area* mx_sheet::remove_area(int& err, int index)
{
    mx_area* res;

    res = (mx_area*)areas->remove(err, index);
    MX_ERROR_CHECK(err);

    switch (res->rtti_class()) {
    case mx_text_area_class_e:
        remove_text_area_ref(err, (mx_text_area*)res);
        MX_ERROR_CHECK(err);
        break;

    case mx_table_area_class_e:
        remove_table_area_ref(err, (mx_table_area*)res);
        MX_ERROR_CHECK(err);
        break;

    case mx_image_area_class_e:
        remove_image_area_ref(err, (mx_image_area*)res);
        MX_ERROR_CHECK(err);
        break;

    case mx_diagram_area_class_e:
        remove_diagram_area_ref(err, (mx_diagram_area*)res);
        MX_ERROR_CHECK(err);
        break;

    default:
        break;
    }

    return res;

abort:;
    return NULL;
}

void mx_sheet::remove_text_area(int& err, int index)
{
    int na;

    err = MX_ERROR_OK;

    na = get_num_text_areas(err);
    MX_ERROR_CHECK(err);

    if ((index < 0) || (index >= na)) {
        err = MX_SHEET_AREA_INDEX;
        return;
    } else {
        del_id_array_item(err, MX_SHEET_TEXT_AREAS, index);
        MX_ERROR_CHECK(err);
    }

abort:;
}

void mx_sheet::remove_table_area(int& err, int index)
{
    int na;
    err = MX_ERROR_OK;

    na = get_num_table_areas(err);
    MX_ERROR_CHECK(err);

    if ((index < 0) || (index >= na)) {
        err = MX_SHEET_AREA_INDEX;
        return;
    } else {
        del_id_array_item(err, MX_SHEET_TABLE_AREAS, index);
        MX_ERROR_CHECK(err);
    }

abort:;
}

void mx_sheet::remove_image_area(int& err, int index)
{
    int na;

    err = MX_ERROR_OK;

    na = get_num_image_areas(err);
    MX_ERROR_CHECK(err);

    if ((index < 0) || (index >= na)) {
        err = MX_SHEET_AREA_INDEX;
        return;
    } else {
        del_id_array_item(err, MX_SHEET_IMAGE_AREAS, index);
        MX_ERROR_CHECK(err);
    }

abort:;
}

void mx_sheet::remove_diagram_area(int& err, int index)
{
    int na;

    err = MX_ERROR_OK;

    na = get_num_diagram_areas(err);
    MX_ERROR_CHECK(err);

    if ((index < 0) || (index >= na)) {
        err = MX_SHEET_AREA_INDEX;
        return;
    } else {
        del_id_array_item(err, MX_SHEET_DIAGRAM_AREAS, index);
        MX_ERROR_CHECK(err);
    }

abort:;
}

void mx_sheet::remove_text_area_ref(int& err, mx_text_area* a)
{
    int n, i, oid, x;

    err = MX_ERROR_OK;

    n = get_num_text_areas(err);
    MX_ERROR_CHECK(err);

    x = a->get_db_id(err);
    MX_ERROR_CHECK(err);

    for (i = 0; i < n; i++) {
        oid = get_id_array_item(err, MX_SHEET_TEXT_AREAS, i);
        MX_ERROR_CHECK(err);

        if (oid == x) {
            remove_text_area(err, i);
            MX_ERROR_CHECK(err);

            return;
        }
    }
abort:;
}

void mx_sheet::remove_table_area_ref(int& err, mx_table_area* a)
{
    int n, i, oid, x;

    err = MX_ERROR_OK;

    n = get_num_table_areas(err);
    MX_ERROR_CHECK(err);

    x = a->get_db_id(err);
    MX_ERROR_CHECK(err);

    for (i = 0; i < n; i++) {
        oid = get_id_array_item(err, MX_SHEET_TABLE_AREAS, i);
        MX_ERROR_CHECK(err);

        if (oid == x) {
            remove_table_area(err, i);
            MX_ERROR_CHECK(err);

            return;
        }
    }
abort:;
}

void mx_sheet::remove_image_area_ref(int& err, mx_image_area* a)
{
    int n, i, oid, x;

    err = MX_ERROR_OK;

    n = get_num_image_areas(err);
    MX_ERROR_CHECK(err);

    x = a->get_db_id(err);
    MX_ERROR_CHECK(err);

    for (i = 0; i < n; i++) {
        oid = get_id_array_item(err, MX_SHEET_IMAGE_AREAS, i);
        MX_ERROR_CHECK(err);

        if (oid == x) {
            remove_image_area(err, i);
            MX_ERROR_CHECK(err);

            return;
        }
    }
abort:;
}

void mx_sheet::remove_diagram_area_ref(int& err, mx_diagram_area* a)
{
    int n, i, oid, x;

    err = MX_ERROR_OK;

    n = get_num_diagram_areas(err);
    MX_ERROR_CHECK(err);

    x = a->get_db_id(err);
    MX_ERROR_CHECK(err);

    for (i = 0; i < n; i++) {
        oid = get_id_array_item(err, MX_SHEET_DIAGRAM_AREAS, i);
        MX_ERROR_CHECK(err);

        if (oid == x) {
            remove_diagram_area(err, i);
            MX_ERROR_CHECK(err);

            return;
        }
    }
abort:;
}

mx_area* mx_sheet::get_area(int& err, int i)
{
    if (!index_built) {
        build_area_index(err);
        MX_ERROR_CHECK(err);
    }
    return (mx_area*)areas->get(err, i);
abort:
    return NULL;
}

int mx_sheet::get_num_areas()
{
    return areas->get_num_items();
}

int mx_sheet::get_index_position(int& err, mx_area* a)
{
    int i;
    mx_area* area;

    for (i = 0; i < areas->get_num_items(); i++) {
        area = (mx_area*)areas->get(err, i);
        MX_ERROR_CHECK(err);

        if (area == a) {
            return i;
        }
    }
abort:;
    return -1;
}

void mx_sheet::set_landscape(int& err, bool b)
{
    set_int(err, MX_SHEET_ORIENTATION, b ? 1 : 0);
    MX_ERROR_CHECK(err);
abort:;
}

bool mx_sheet::get_landscape(int& err)
{
    int i;

    i = get_default_int(err, MX_SHEET_ORIENTATION, 0);
    MX_ERROR_CHECK(err);

    return i == 1;
abort:
    return FALSE;
}

float mx_sheet::get_width_minus_margins(int& err)
{
    float left, right, width;

    left = get_default_float(err, MX_SHEET_LEFT_MARGIN, 0.0);
    MX_ERROR_CHECK(err);

    right = get_default_float(err, MX_SHEET_RIGHT_MARGIN, 0.0);
    MX_ERROR_CHECK(err);

    width = get_default_float(err, MX_SHEET_WIDTH, 0.0);
    MX_ERROR_CHECK(err);

    return width - (left + right);

abort:
    return 0.0;
}

float mx_sheet::get_height_minus_margins(int& err)
{
    float top, bottom, height;

    top = get_default_float(err, MX_SHEET_TOP_MARGIN, 0.0);
    MX_ERROR_CHECK(err);

    bottom = get_default_float(err, MX_SHEET_BOTTOM_MARGIN, 0.0);
    MX_ERROR_CHECK(err);

    height = get_default_float(err, MX_SHEET_HEIGHT, 0.0);
    MX_ERROR_CHECK(err);

    return height - (top + bottom);

abort:
    return 0.0;
}

void mx_sheet::add_area(int& err, mx_area* a, int i)
{
    const char* attribute_name;
    int oid;

    switch (a->rtti_class()) {
    case mx_text_area_class_e:
        attribute_name = MX_SHEET_TEXT_AREAS;
        break;
    case mx_table_area_class_e:
        attribute_name = MX_SHEET_TABLE_AREAS;
        break;
    case mx_image_area_class_e:
        attribute_name = MX_SHEET_IMAGE_AREAS;
        break;
    case mx_diagram_area_class_e:
        attribute_name = MX_SHEET_DIAGRAM_AREAS;
        break;
    default:
        MX_ERROR_THROW(err, MX_SHEET_BAD_AREA_CLASS);
    }

    a->set_owner_object(err, this);
    MX_ERROR_CHECK(err);

    oid = a->get_db_id(err);
    MX_ERROR_CHECK(err);

    add_id_array_item(err, attribute_name, oid);
    MX_ERROR_CHECK(err);

    areas->insert(err, i, a);
    MX_ERROR_CHECK(err);

abort:;
}

void mx_sheet::join_areas(int& err, int i)
{
    mx_area *a1, *a2;
    bool b;

    a1 = get_area(err, i);
    MX_ERROR_CHECK(err);

    a2 = get_area(err, i + 1);
    MX_ERROR_CHECK(err);

    b = a1->can_join(err, a2);
    MX_ERROR_CHECK(err);

    if (b) {
        a2 = remove_area(err, i + 1);
        MX_ERROR_CHECK(err);

        a1->join(err, a2);
        MX_ERROR_CHECK(err);
    }
abort:;
}

mx_text_area* mx_sheet::add_text_area_to_top(int& err)
{
    mx_text_area* res;
    float width, top, left, right, bottom;

    res = add_text_area(err, 0);
    MX_ERROR_CHECK(err);

    get_margins(err, left, right, top, bottom);
    MX_ERROR_CHECK(err);

    width = get_width(err);
    MX_ERROR_CHECK(err);

    res->set_outline(err, width - (left + right), 1.0);
    MX_ERROR_CHECK(err);

    return res;

abort:
    return NULL;
}

mx_text_area* mx_sheet::add_text_area_to_bottom(int& err)
{
    mx_text_area* res;

    res = add_text_area(err, areas->get_num_items());
    MX_ERROR_CHECK(err);

    return res;

abort:
    return NULL;
}

bool mx_sheet::is_bottom_area_text()
{
    mx_area* a;
    int err = MX_ERROR_OK;

    if (areas->get_num_items() == 0) {
        return FALSE;
    } else {
        a = (mx_area*)areas->get(err, areas->get_num_items() - 1);
        MX_ERROR_CHECK(err);

        return a->rtti_class() == mx_text_area_class_e;
    }

abort:
    return FALSE;
}

bool mx_sheet::is_top_area_text()
{
    mx_area* a;
    int err = MX_ERROR_OK;

    if (areas->get_num_items() == 0) {
        return FALSE;
    } else {
        a = (mx_area*)areas->get(err, 0);
        MX_ERROR_CHECK(err);

        return a->rtti_class() == mx_text_area_class_e;
    }

abort:
    return FALSE;
}

mx_area* mx_sheet::get_bottom_area(int& err)
{
    return get_area(err, get_num_areas() - 1);
}

mx_area* mx_sheet::get_top_area(int& err)
{
    return get_area(err, 0);
}

void mx_sheet::move_area_out(int& err, mx_area* area, mx_sheet* dest)
{
    int i;

    // first move the area pointer
    switch (area->rtti_class()) {
    case mx_text_area_class_e:
        move_text_area(err, (mx_text_area*)area, dest);
        MX_ERROR_CHECK(err);
        break;

    case mx_table_area_class_e:
        move_table_area(err, (mx_table_area*)area, dest);
        MX_ERROR_CHECK(err);
        break;

    case mx_image_area_class_e:
        move_image_area(err, (mx_image_area*)area, dest);
        MX_ERROR_CHECK(err);
        break;

    case mx_diagram_area_class_e:
        move_diagram_area(err, (mx_diagram_area*)area, dest);
        MX_ERROR_CHECK(err);
        break;
    default:
        break;
    }

    i = areas->find_index(err, area);
    MX_ERROR_CHECK(err);

    areas->remove(err, i);
    MX_ERROR_CHECK(err);

    dest->areas->insert(err, 0, area);
    MX_ERROR_CHECK(err);

abort:;
}

void mx_sheet::move_area_in(int& err, mx_area* area, mx_sheet* src)
{
    int i;

    // first move the area pointer
    switch (area->rtti_class()) {
    case mx_text_area_class_e:
        src->move_text_area(err, (mx_text_area*)area, this);
        MX_ERROR_CHECK(err);
        break;

    case mx_table_area_class_e:
        src->move_table_area(err, (mx_table_area*)area, this);
        MX_ERROR_CHECK(err);
        break;

    case mx_image_area_class_e:
        src->move_image_area(err, (mx_image_area*)area, this);
        MX_ERROR_CHECK(err);
        break;

    case mx_diagram_area_class_e:
        src->move_diagram_area(err, (mx_diagram_area*)area, this);
        MX_ERROR_CHECK(err);
        break;
    default:
        break;
    }

    i = src->areas->find_index(err, area);
    MX_ERROR_CHECK(err);

    src->areas->remove(err, i);
    MX_ERROR_CHECK(err);

    areas->append(area);
abort:;
}

void mx_sheet::highlight_from_start_of_sheet(int& err, mx_wp_sheet_pos& end)
{
    ((mx_sheet_painter*)painter)->highlight_from_start_of_sheet(err, this, end);
}

void mx_sheet::highlight_whole_sheet(int& err)
{
    ((mx_sheet_painter*)painter)->highlight_whole_sheet(err, this);
}

void mx_sheet::highlight_in_sheet(int& err, mx_wp_sheet_pos& start, mx_wp_sheet_pos& end)
{
    ((mx_sheet_painter*)painter)->highlight_in_sheet(err, this, start, end);
}

void mx_sheet::highlight_to_end_of_sheet(int& err, mx_wp_sheet_pos& start)
{
    ((mx_sheet_painter*)painter)->highlight_to_end_of_sheet(err, this, start);
}
