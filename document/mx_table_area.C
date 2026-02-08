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
 * MODULE/CLASS : mx_table_area
 *
 * AUTHOR : Andrew Haisley
 *
 *
 *
 * DESCRIPTION:
 *
 *
 *
 */

#include <mx.h>
#include <mx_db.h>

#include "mx_table_area.h"
#include "mx_text_area.h"
#include <mx_doc_coord.h>
#include <mx_doc_rect.h>
#include <mx_paragraph.h>
#include <mx_sheet.h>
#include <mx_spline.h>
#include <mx_ta_pos.h>
#include <mx_tab_paint.h>

#define MX_TABLE_NUM_COLUMNS "columns"
#define MX_TABLE_NUM_ROWS "rows"
#define MX_TABLE_TYPE "type"

// two blobs are used
// this one holds the object ids of the text areas making up the table
#define MX_TABLE_AREA_CELL_OIDS "celloids"
// and this one holds the areas themselves
#define MX_TABLE_AREA_CELLS "cells"

// create a new table area
mx_table_area::mx_table_area(uint32 docid, mx_paragraph_style* s)
    : mx_area(docid, s)
{
    int err = MX_ERROR_OK;

    cell_oids = NULL;
    cell_objs = NULL;

    set_num_rows(err, 0);
    MX_ERROR_CHECK(err);

    set_num_columns(err, 0);
    MX_ERROR_CHECK(err);

    // get a handle on a suitable painter
    painter = mx_table_area_painter::painter();

    return;
abort:
    global_error_trace->print();
    MX_ERROR_CLEAR(err);
}

// create a new table area - get data from database
mx_table_area::mx_table_area(uint32 docid, uint32 oid)
    : mx_area(docid, oid)
{
    cell_oids = NULL;
    cell_objs = NULL;

    // get a handle on a suitable painter
    painter = mx_table_area_painter::painter();

    return;
}

// copy constructor
mx_table_area::mx_table_area(const mx_table_area& a)
    : mx_area((mx_area&)a)
{
    int i, j, nc, nr, err = MX_ERROR_OK;
    mx_text_area* ta;

    mx_table_area* tab = (mx_table_area*)&a;

    // get a handle on a suitable painter
    painter = mx_table_area_painter::painter();

    nc = tab->get_num_columns(err);
    MX_ERROR_CHECK(err);

    nr = tab->get_num_rows(err);
    MX_ERROR_CHECK(err);

    set_int(err, MX_TABLE_NUM_COLUMNS, nc);
    MX_ERROR_CHECK(err);

    set_int(err, MX_TABLE_NUM_ROWS, nr);
    MX_ERROR_CHECK(err);

    cell_oids = new uint32[nc * nr];
    cell_objs = new mx_text_area*[nc * nr];

    for (i = 0; i < nr; i++) {
        for (j = 0; j < nc; j++) {
            ta = tab->get_cell(err, i, j);
            MX_ERROR_CHECK(err);

            cell_objs[(i * nc) + j] = (mx_text_area*)ta->duplicate();
            cell_oids[(i * nc) + j] = cell_objs[(i * nc) + j]->get_db_id(err);
            MX_ERROR_CHECK(err);
        }
    }

    return;

abort:
    global_error_trace->print();
    MX_ERROR_CLEAR(err);
}

mx_table_area::~mx_table_area()
{
    int err = MX_ERROR_OK;

    if (mx_db_object::get_mem_state() == mx_on_disk_e) {
        // nothing to do
    } else {
        int nc, nr, i;

        nc = get_num_columns(err);
        MX_ERROR_CHECK(err);

        nr = get_num_rows(err);
        MX_ERROR_CHECK(err);

        for (i = 0; i < (nc * nr); i++) {
            delete cell_objs[i];
        }

        // delete cell object arrays etc
        delete[] cell_objs;
        delete[] cell_oids;
    }
    return;

abort:
    global_error_trace->print();
    MX_ERROR_CLEAR(err);
}

void mx_table_area::set_num_rows(int& err, int rows)
{
    int nc, nr, x;
    mx_sheet* sheet;
    mx_document* doc;
    mx_paragraph* p;

    doc = get_document(err);
    MX_ERROR_CHECK(err);

    sheet = get_sheet(err);
    MX_ERROR_CHECK(err);

    nr = get_num_rows(err);
    MX_ERROR_CHECK(err);

    if (rows == nr) {
        return;
    }

    nc = get_num_columns(err);
    MX_ERROR_CHECK(err);

    cc_unserialise_attributes(err);
    MX_ERROR_CHECK(err);

    if (nc == 0) {
        // must have at least one column
        nc = 1;
        nr = rows;

        cell_oids = new uint32[nc * nr];
        cell_objs = new mx_text_area*[nc * nr];

        for (x = 0; x < nr; x++) {
            cell_objs[x] = new mx_text_area(doc_id, base_style, TRUE);
            MX_ERROR_CHECK(cell_objs[x]->error);

            cell_objs[x]->set_owner_object(err, (mx_db_object*)sheet);
            MX_ERROR_CHECK(err);

            cell_oids[x] = cell_objs[x]->get_db_id(err);
            MX_ERROR_CHECK(err);

            p = cell_objs[x]->get(err, 0);
            MX_ERROR_CHECK(err);

            p->move_to_document(err, doc);
            MX_ERROR_CHECK(err);
        }

        set_int(err, MX_TABLE_NUM_ROWS, nr);
        MX_ERROR_CHECK(err);

        set_int(err, MX_TABLE_NUM_COLUMNS, nc);
        MX_ERROR_CHECK(err);
    } else {
        if (rows > nr) {
            insert_rows_no_reposition(err, nr, (rows - nr));
            MX_ERROR_CHECK(err);
        } else {
            remove_rows_no_reposition(err, rows, (nr - rows));
            MX_ERROR_CHECK(err);
        }
    }

    reposition_cells(err);
    MX_ERROR_CHECK(err);

    recalculate_outline(err);
    MX_ERROR_CHECK(err);

abort:;
}

void mx_table_area::set_num_columns(int& err, int columns)
{
    int nc, nr, x;
    mx_document* doc;
    mx_sheet* sheet;
    mx_paragraph* p;

    doc = get_document(err);
    MX_ERROR_CHECK(err);

    sheet = get_sheet(err);
    MX_ERROR_CHECK(err);

    nc = get_num_columns(err);
    MX_ERROR_CHECK(err);

    if (columns == nc) {
        return;
    }

    nr = get_num_rows(err);
    MX_ERROR_CHECK(err);

    cc_unserialise_attributes(err);
    MX_ERROR_CHECK(err);

    if (nr == 0) {
        // must have at least one row
        nr = 1;
        nc = columns;

        cell_oids = new uint32[nc * nr];
        cell_objs = new mx_text_area*[nc * nr];

        for (x = 0; x < nc; x++) {
            cell_objs[x] = new mx_text_area(doc_id, base_style, TRUE);
            MX_ERROR_CHECK(cell_objs[x]->error);

            cell_oids[x] = cell_objs[x]->get_db_id(err);
            MX_ERROR_CHECK(err);

            cell_objs[x]->set_owner_object(err, (mx_db_object*)sheet);
            MX_ERROR_CHECK(err);

            p = cell_objs[x]->get(err, 0);
            MX_ERROR_CHECK(err);

            p->move_to_document(err, doc);
            MX_ERROR_CHECK(err);
        }

        set_int(err, MX_TABLE_NUM_ROWS, nr);
        MX_ERROR_CHECK(err);

        set_int(err, MX_TABLE_NUM_COLUMNS, nc);
        MX_ERROR_CHECK(err);
    } else {
        if (columns > nc) {
            insert_columns_no_reposition(err, nc, (columns - nc));
            MX_ERROR_CHECK(err);
        } else {
            remove_columns_no_reposition(err, columns, (nc - columns));
            MX_ERROR_CHECK(err);
        }
    }

    reposition_cells(err);
    MX_ERROR_CHECK(err);

    recalculate_outline(err);
    MX_ERROR_CHECK(err);

abort:;
}

int mx_table_area::get_num_columns(int& err)
{
    int res;

    res = get_default_int(err, MX_TABLE_NUM_COLUMNS, 0);
    MX_ERROR_CHECK(err);

    return res;

abort:
    return -1;
}

int mx_table_area::get_num_rows(int& err)
{
    int res;

    res = get_default_int(err, MX_TABLE_NUM_ROWS, 0);
    MX_ERROR_CHECK(err);

    return res;

abort:
    return -1;
}

mx_text_area* mx_table_area::get_cell(int& err, int row, int column)
{
    int nc, nr;

    cc_unserialise_attributes(err);
    MX_ERROR_CHECK(err);

    nc = get_num_columns(err);
    MX_ERROR_CHECK(err);

    nr = get_num_rows(err);
    MX_ERROR_CHECK(err);

    MX_ERROR_ASSERT(err, (row < nr && row >= 0));
    MX_ERROR_ASSERT(err, (column < nc && column >= 0));

    return cell_objs[(row * nc) + column];
abort:;
    return NULL;
}

void mx_table_area::remove_rows(int& err, int row, int n)
{
    remove_rows_no_reposition(err, row, n);
    MX_ERROR_CHECK(err);

    reposition_cells(err);
    MX_ERROR_CHECK(err);

    recalculate_outline(err);
    MX_ERROR_CHECK(err);
abort:;
}

void mx_table_area::remove_rows_no_reposition(int& err, int row, int n)
{
    mx_text_area** new_cell_objs;
    uint32* new_cell_oids;
    int nr, nc, x, y;

    cc_unserialise_attributes(err);
    MX_ERROR_CHECK(err);

    nc = get_num_columns(err);
    MX_ERROR_CHECK(err);

    nr = get_num_rows(err);
    MX_ERROR_CHECK(err);

    new_cell_oids = new uint32[nc * (nr - n)];
    new_cell_objs = new mx_text_area*[nc * (nr - n)];

    // delete the ones we don't want anymore
    for (y = row; y < (row + n); y++) {
        for (x = 0; x < nc; x++) {
            delete cell_objs[x + y * nc];
        }
    }

    // copy the remaining cells across
    for (y = 0; y < row; y++) {
        for (x = 0; x < nc; x++) {
            new_cell_oids[x + y * nc] = cell_oids[x + y * nc];
            new_cell_objs[x + y * nc] = cell_objs[x + y * nc];
        }
    }

    for (y = row + n; y < nr; y++) {
        for (x = 0; x < nc; x++) {
            new_cell_oids[x + ((y - n) * nc)] = cell_oids[x + y * nc];
            new_cell_objs[x + ((y - n) * nc)] = cell_objs[x + y * nc];
        }
    }

    set_int(err, MX_TABLE_NUM_ROWS, nr - n);
    MX_ERROR_CHECK(err);

    delete[] cell_objs;
    cell_objs = new_cell_objs;

    delete[] cell_oids;
    cell_oids = new_cell_oids;
abort:;
}

void mx_table_area::remove_columns(int& err, int column, int n)
{
    remove_columns_no_reposition(err, column, n);
    MX_ERROR_CHECK(err);

    reposition_cells(err);
    MX_ERROR_CHECK(err);

    recalculate_outline(err);
    MX_ERROR_CHECK(err);
abort:;
}

void mx_table_area::remove_columns_no_reposition(int& err, int column, int n)
{
    mx_text_area** new_cell_objs;
    uint32* new_cell_oids;
    int nr, nc, x, y;

    cc_unserialise_attributes(err);
    MX_ERROR_CHECK(err);

    nc = get_num_columns(err);
    MX_ERROR_CHECK(err);

    nr = get_num_rows(err);
    MX_ERROR_CHECK(err);

    new_cell_oids = new uint32[(nc - n) * nr];
    new_cell_objs = new mx_text_area*[(nc - n) * nr];

    // delete the ones we don't want anymore
    for (x = column; x < (column + n); x++) {
        for (y = 0; y < nr; y++) {
            delete cell_objs[x + y * nc];
        }
    }

    // copy the remaining cells across
    for (x = 0; x < column; x++) {
        for (y = 0; y < nr; y++) {
            new_cell_oids[x + y * (nc - n)] = cell_oids[x + y * nc];
            new_cell_objs[x + y * (nc - n)] = cell_objs[x + y * nc];
        }
    }

    for (x = column + n; x < nc; x++) {
        for (y = 0; y < nr; y++) {
            new_cell_oids[x - n + y * (nc - n)] = cell_oids[x + y * nc];
            new_cell_objs[x - n + y * (nc - n)] = cell_objs[x + y * nc];
        }
    }

    set_int(err, MX_TABLE_NUM_COLUMNS, nc - n);
    MX_ERROR_CHECK(err);

    delete[] cell_objs;
    cell_objs = new_cell_objs;

    delete[] cell_oids;
    cell_oids = new_cell_oids;
abort:;
}

void mx_table_area::insert_rows(int& err, int row, int n)
{
    insert_rows_no_reposition(err, row, n);
    MX_ERROR_CHECK(err);

    reposition_cells(err);
    MX_ERROR_CHECK(err);

    recalculate_outline(err);
    MX_ERROR_CHECK(err);
abort:;
}

void mx_table_area::insert_rows_no_reposition(int& err, int row, int n)
{
    mx_text_area** new_cell_objs;
    uint32* new_cell_oids;
    int nr, nc, x, y, i, j;
    float pre_height, pre_width;
    mx_document* doc;
    mx_paragraph* p;

    mx_sheet* sheet = get_sheet(err);
    MX_ERROR_CHECK(err);

    doc = sheet->get_document(err);
    MX_ERROR_CHECK(err);

    cc_unserialise_attributes(err);
    MX_ERROR_CHECK(err);

    nc = get_num_columns(err);
    MX_ERROR_CHECK(err);

    nr = get_num_rows(err);
    MX_ERROR_CHECK(err);

    new_cell_oids = new uint32[nc * (nr + n)];
    new_cell_objs = new mx_text_area*[nc * (nr + n)];

    // copy the existing cells across
    for (y = 0; y < row; y++) {
        for (x = 0; x < nc; x++) {
            new_cell_oids[x + y * nc] = cell_oids[x + y * nc];
            new_cell_objs[x + y * nc] = cell_objs[x + y * nc];
        }
    }

    for (y = row; y < nr; y++) {
        for (x = 0; x < nc; x++) {
            new_cell_oids[x + ((y + n) * nc)] = cell_oids[x + y * nc];
            new_cell_objs[x + ((y + n) * nc)] = cell_objs[x + y * nc];
        }
    }

    for (y = row; y < (row + n); y++) {
        for (x = 0; x < nc; x++) {
            i = x + y * nc;
            new_cell_objs[i] = new mx_text_area(doc_id, base_style, TRUE);
            MX_ERROR_CHECK(new_cell_objs[i]->error);

            new_cell_oids[i] = new_cell_objs[i]->get_db_id(err);
            MX_ERROR_CHECK(err);

            new_cell_objs[i]->set_owner_object(err, (mx_db_object*)sheet);
            MX_ERROR_CHECK(err);

            p = new_cell_objs[i]->get(err, 0);
            MX_ERROR_CHECK(err);

            p->move_to_document(err, doc);
            MX_ERROR_CHECK(err);

            // if there is a previous row, set the widths of the new rows text
            // areas to be the same as the previous row
            if (y > 0) {
                j = x + (y - 1) * nc;
            } else {
                // failing that, use the row after the new ones
                j = x + (row + n) * nc;
            }

            pre_height = new_cell_objs[j]->get_height(err);
            MX_ERROR_CHECK(err);

            pre_width = new_cell_objs[j]->get_width(err);
            MX_ERROR_CHECK(err);

            new_cell_objs[i]->set_outline(err, pre_width, pre_height);
            MX_ERROR_CHECK(err);
        }
    }

    set_int(err, MX_TABLE_NUM_ROWS, nr + n);
    MX_ERROR_CHECK(err);

    delete[] cell_objs;
    cell_objs = new_cell_objs;

    delete[] cell_oids;
    cell_oids = new_cell_oids;
abort:;
}

void mx_table_area::insert_columns(int& err, int column, int n)
{
    insert_columns_no_reposition(err, column, n);
    MX_ERROR_CHECK(err);

    reposition_cells(err);
    MX_ERROR_CHECK(err);

    recalculate_outline(err);
    MX_ERROR_CHECK(err);
abort:;
}

void mx_table_area::insert_columns_no_reposition(int& err, int column, int n)
{
    mx_document* doc;
    mx_text_area** new_cell_objs;
    uint32* new_cell_oids;
    int nr, nc, x, y, i;
    mx_paragraph* p;

    float width;

    mx_sheet* sheet = get_sheet(err);
    MX_ERROR_CHECK(err);

    doc = sheet->get_document(err);
    MX_ERROR_CHECK(err);

    cc_unserialise_attributes(err);
    MX_ERROR_CHECK(err);

    nc = get_num_columns(err);
    MX_ERROR_CHECK(err);

    if (column >= nc) {
        width = get_column_width(err, nc - 1);
        MX_ERROR_CHECK(err);
    } else {
        width = get_column_width(err, column);
        MX_ERROR_CHECK(err);
    }

    nr = get_num_rows(err);
    MX_ERROR_CHECK(err);

    new_cell_oids = new uint32[(nc + n) * nr];
    new_cell_objs = new mx_text_area*[(nc + n) * nr];

    // copy the existing cells across
    for (x = 0; x < column; x++) {
        for (y = 0; y < nr; y++) {
            new_cell_oids[x + y * (n + nc)] = cell_oids[x + y * nc];
            new_cell_objs[x + y * (n + nc)] = cell_objs[x + y * nc];
        }
    }

    for (x = column; x < nc; x++) {
        for (y = 0; y < nr; y++) {
            new_cell_oids[x + n + y * (n + nc)] = cell_oids[x + y * nc];
            new_cell_objs[x + n + y * (n + nc)] = cell_objs[x + y * nc];
        }
    }

    for (x = column; x < (column + n); x++) {
        for (y = 0; y < nr; y++) {
            i = x + y * (n + nc);
            new_cell_objs[i] = new mx_text_area(doc_id, base_style, TRUE);
            MX_ERROR_CHECK(new_cell_objs[i]->error);

            new_cell_oids[i] = new_cell_objs[i]->get_db_id(err);
            MX_ERROR_CHECK(err);

            new_cell_objs[i]->set_owner_object(err, (mx_db_object*)sheet);
            MX_ERROR_CHECK(err);

            p = new_cell_objs[i]->get(err, 0);
            MX_ERROR_CHECK(err);

            p->move_to_document(err, doc);
            MX_ERROR_CHECK(err);

            new_cell_objs[i]->set_outline(err, width, 1.0);
            MX_ERROR_CHECK(err);
        }
    }

    set_int(err, MX_TABLE_NUM_COLUMNS, nc + n);
    MX_ERROR_CHECK(err);

    delete[] cell_objs;
    cell_objs = new_cell_objs;

    delete[] cell_oids;
    cell_oids = new_cell_oids;
abort:;
}

void mx_table_area::join_para_with_next(int& err, mx_table_area_pos& p)
{
    int para_num, n;
    mx_paragraph *para, *next_para;
    mx_text_area* table_cell = this->get_cell(err, p.row, p.column);
    MX_ERROR_CHECK(err);

    para_num = p.text_pos.paragraph_index;

    n = table_cell->num_paragraphs(err);
    MX_ERROR_CHECK(err);

    if (para_num >= n) {
        MX_ERROR_THROW(err, MX_POSITION_RANGE_ERROR);
    }

    para = table_cell->remove(err, para_num);
    MX_ERROR_CHECK(err);

    next_para = table_cell->remove(err, para_num + 1);
    MX_ERROR_CHECK(err);

    para->join(err, next_para);
    MX_ERROR_CHECK(err);

    table_cell->insert(err, para_num, para);
    MX_ERROR_CHECK(err);
abort:;
}

void mx_table_area::serialise_attributes(int& err)
{
    int blob_size = 0;
    int nr, nc, i, n;

    uint8 *tmp, *buffer;

    nc = get_num_columns(err);
    MX_ERROR_CHECK(err);

    nr = get_num_rows(err);
    MX_ERROR_CHECK(err);

    blob_size = mx_area::get_serialised_size(err);
    MX_ERROR_CHECK(err);

    for (i = 0; i < (nc * nr); i++) {
        blob_size += (n = cell_objs[i]->get_serialised_size(err));
        MX_ERROR_CHECK(err);
    }

    if (blob_size != 0) {
        tmp = buffer = new uint8[blob_size];

        mx_area::serialise(err, &buffer);
        MX_ERROR_CHECK(err);

        for (i = 0; i < (nc * nr); i++) {
            cell_objs[i]->serialise(err, &buffer);
            MX_ERROR_CHECK(err);
        }

        if (blob_size < MX_ATTRIBUTE_MAX_SIZE) {
            mx_attribute a;

            delete_blob(err, MX_TABLE_AREA_CELLS);
            MX_ERROR_CLEAR(err);

            strcpy(a.name, MX_TABLE_AREA_CELLS);
            a.type = mx_char_array;

            a.length = blob_size;
            a.value.ca = (char*)tmp;

            set(err, &a);
            MX_ERROR_CHECK(err);
        } else {
            delete_attribute(err, MX_TABLE_AREA_CELLS);
            MX_ERROR_CLEAR(err);

            create_blob(err, MX_TABLE_AREA_CELLS);
            if (err == MX_DB_CLIENT_CACHE_DUPLICATE_BLOB) {
                MX_ERROR_CLEAR(err);
            } else {
                MX_ERROR_CHECK(err);
            }

            set_blob_size(err, MX_TABLE_AREA_CELLS, blob_size);
            MX_ERROR_CHECK(err);

            set_blob_data(err, MX_TABLE_AREA_CELLS, 0, blob_size, tmp);
            MX_ERROR_CHECK(err);
        }

        delete[] tmp;
    }

    // now, serialise the object ids
    create_blob(err, MX_TABLE_AREA_CELL_OIDS);
    if (err == MX_DB_CLIENT_CACHE_DUPLICATE_BLOB) {
        MX_ERROR_CLEAR(err);
    } else {
        MX_ERROR_CHECK(err);
    }

    if ((nc * nr * 4) < MX_ATTRIBUTE_MAX_SIZE) {
        mx_attribute a;

        delete_blob(err, MX_TABLE_AREA_CELL_OIDS);
        MX_ERROR_CLEAR(err);

        strcpy(a.name, MX_TABLE_AREA_CELL_OIDS);
        a.type = mx_id_array;

        a.length = nc * nr;
        a.value.ida = cell_oids;

        set(err, &a);
        MX_ERROR_CHECK(err);
    } else {
        delete_attribute(err, MX_TABLE_AREA_CELL_OIDS);
        MX_ERROR_CLEAR(err);

        set_blob_size(err, MX_TABLE_AREA_CELL_OIDS, nc * nr * 4);
        MX_ERROR_CHECK(err);

        tmp = buffer = new uint8[nc * nr * 4];

        serialise_int32_a((int32*)cell_oids, nc * nr, &buffer);

        set_blob_data(err, MX_TABLE_AREA_CELL_OIDS, 0, nc * nr * 4, tmp);
        MX_ERROR_CHECK(err);

        delete[] tmp;
    }

abort:;
}

void mx_table_area::unserialise_attributes(int& err)
{
    int nc, nr, blob_size;
    uint8 *buffer, *tmp;

    // unserialise all the children here
    nc = get_num_columns(err);
    MX_ERROR_CHECK(err);

    nr = get_num_rows(err);
    MX_ERROR_CHECK(err);

    cell_oids = new uint32[nc * nr];
    cell_objs = new mx_text_area*[nc * nr];

    // object ids first
    tmp = buffer = new uint8[nc * nr * 4];

    get_blob_data(err, MX_TABLE_AREA_CELL_OIDS, 0, nc * nr * 4, buffer);
    if (err != MX_ERROR_OK) {
        MX_ERROR_CLEAR(err);

        const mx_attribute* a;

        a = get_readonly(err, MX_TABLE_AREA_CELL_OIDS);
        MX_ERROR_CHECK(err);

        memcpy(cell_oids, a->value.ida, nc * nr * 4);
    } else {
        unserialise_int32_a((int32*)cell_oids, nc * nr, &buffer);
    }

    delete[] tmp;

    // the real objects
    blob_size = get_blob_size(err, MX_TABLE_AREA_CELLS);
    if (err == MX_DB_CLIENT_CACHE_NO_SUCH_BLOB) {
        MX_ERROR_CLEAR(err);

        const mx_attribute* a;

        a = get_readonly(err, MX_TABLE_AREA_CELLS);
        MX_ERROR_CHECK(err);

        tmp = (uint8*)a->value.ca;

        mx_area::unserialise(err, &tmp);
        MX_ERROR_CHECK(err);

        unserialise_cells(err, &tmp, nc, nr);
        MX_ERROR_CHECK(err);
    } else {
        MX_ERROR_CHECK(err);

        tmp = buffer = new uint8[blob_size];

        get_blob_data(err, MX_TABLE_AREA_CELLS, 0, blob_size, buffer);
        MX_ERROR_CHECK(err);

        mx_area::unserialise(err, &buffer);
        MX_ERROR_CHECK(err);

        unserialise_cells(err, &buffer, nc, nr);
        MX_ERROR_CHECK(err);

        delete[] tmp;
    }

abort:;
}

void mx_table_area::unserialise_cells(int& err, uint8** buffer, int nc, int nr)
{
    int i;
    mx_document* doc;
    mx_paragraph* p;

    mx_sheet* sheet = get_sheet(err);
    MX_ERROR_CHECK(err);

    doc = get_document(err);
    MX_ERROR_CHECK(err);

    for (i = 0; i < (nc * nr); i++) {
        cell_objs[i] = (mx_text_area*)mx_db_cc_get_obj_from_oid(
            err, doc_id, cell_oids[i]);
        if (err == MX_DB_CLIENT_CACHE_NO_SUCH_OID) {
            MX_ERROR_CLEAR(err);

            cell_objs[i] = new mx_text_area(doc_id, cell_oids[i], TRUE);
            MX_ERROR_CHECK(cell_objs[i]->error);

            p = cell_objs[i]->get(err, 0);
            MX_ERROR_CHECK(err);

            p->move_to_document(err, doc);
            MX_ERROR_CHECK(err);
        } else {
            MX_ERROR_CHECK(err);
        }

        cell_objs[i]->set_owner_object(err, (mx_db_object*)sheet);
        MX_ERROR_CHECK(err);

        cell_objs[i]->unserialise(err, buffer);
        MX_ERROR_CHECK(err);
    }

    this->reposition_cells(err);
    MX_ERROR_CHECK(err);
abort:;
}

void mx_table_area::delete_cells(int& err)
{
    int nr, nc, x, y;

    nr = get_num_rows(err);
    MX_ERROR_CHECK(err);

    nc = get_num_columns(err);
    MX_ERROR_CHECK(err);

    for (x = 0; x < nc; x++) {
        for (y = 0; y < nr; y++) {
            cell_objs[x + y * nc]->delete_links(err);
            MX_ERROR_CHECK(err);

            delete cell_objs[x + y * nc];
        }
    }
abort:;
}

uint32 mx_table_area::memory_size(int& err)
{
    err = MX_ERROR_OK;
    return 0;
}

void mx_table_area::uncache(int& err)
{
    err = MX_ERROR_OK;
}

void mx_table_area::set_column_width(int& err, int column, float width)
{
    int i;
    int nr;
    mx_text_area* ta;

    nr = get_num_rows(err);
    MX_ERROR_CHECK(err);

    for (i = 0; i < nr; i++) {
        ta = get_cell(err, i, column);
        MX_ERROR_CHECK(err);

        ta->set_outline(err, width, ta->get_height(err));
        MX_ERROR_CHECK(err);
    }

    reposition_cells(err);
    MX_ERROR_CHECK(err);

    recalculate_outline(err);
    MX_ERROR_CHECK(err);

abort:;
}

float mx_table_area::get_column_width(int& err, int column)
{
    mx_text_area* ta;
    float res = 0.0;

    ta = get_cell(err, 0, column);
    MX_ERROR_CHECK(err);

    res = ta->get_width(err);
    MX_ERROR_CHECK(err);

abort:
    return res;
}

void mx_table_area::reformat(int& err,
    mx_table_area_pos& start,
    mx_table_area_pos& end,
    bool& height_changed)
{
    bool b;
    int i, j, nc;

    mx_text_area_pos area_pos;
    mx_text_area* ta;

    height_changed = FALSE;

    nc = get_num_columns(err);
    MX_ERROR_CHECK(err);

    (void)get_num_rows(err);
    MX_ERROR_CHECK(err);

    i = start.column;
    j = start.row;

    // reformat each cell in the range given
    while (TRUE) {
        ta = get_cell(err, j, i);
        MX_ERROR_CHECK(err);

        if (i == start.column && j == start.row) {
            // reformat from the start text area position
            area_pos.moveto_end(err, ta);
            MX_ERROR_CHECK(err);

            ta->reformat(err, start.text_pos, area_pos, b);
            MX_ERROR_CHECK(err);
        } else {
            if (i == end.column && j == end.row) {
                // reformat until the end text area position
                area_pos.moveto_start(err, ta);

                ta->reformat(err, area_pos, end.text_pos, b);
                MX_ERROR_CHECK(err);
            } else {
                // reformat the whole cell
                ta->reformat(err, b);
                MX_ERROR_CHECK(err);
            }
        }

        if (b) {
            height_changed = TRUE;
        }

        if (i == end.column && j == end.row) {
            break;
        }

        i++;

        if (i == nc) {
            i = 0;
            j++;
        }
    }

    if (height_changed) {
        reposition_cells(err);
        MX_ERROR_CHECK(err);

        recalculate_outline(err);
        MX_ERROR_CHECK(err);
    }

abort:;
}

void mx_table_area::reformat(int& err, bool& height_changed)
{
    mx_table_area_pos start;
    mx_table_area_pos end;

    start.moveto_start(err, this);
    MX_ERROR_CHECK(err);

    end.moveto_end(err, this);
    MX_ERROR_CHECK(err);

    reformat(err, start, end, height_changed);
    MX_ERROR_CHECK(err);

abort:;
}

void mx_table_area::serialise(int& err, uint8** buffer)
{
    // tables can't be serialised as a whole.....
}

void mx_table_area::unserialise(int& err, uint8** buffer)
{
    // tables can't be serialised as a whole.....
}

uint32 mx_table_area::get_serialised_size(int& err)
{
    return 0;
}

void mx_table_area::move_to_document(int& err, mx_document* doc)
{
    uint32 i, nc, nr;

    mx_area::move_to_document(err, doc);
    MX_ERROR_CHECK(err);

    nc = get_num_columns(err);
    MX_ERROR_CHECK(err);

    nr = get_num_rows(err);
    MX_ERROR_CHECK(err);

    for (i = 0; i < (nc * nr); i++) {
        cell_objs[i]->move_to_document(err, doc);
        MX_ERROR_CHECK(err);

        cell_oids[i] = cell_objs[i]->get_db_id(err);
        MX_ERROR_CHECK(err);
    }

abort:;
}

void mx_table_area::reposition_cells(int& err)
{
    int nc, nr, column, row;
    float temp, x = 0, y = 0, height;
    bool b;

    mx_text_area* ta = nullptr;

    nc = get_num_columns(err);
    MX_ERROR_CHECK(err);

    nr = get_num_rows(err);
    MX_ERROR_CHECK(err);

    // go down each column in turn setting the x position
    for (column = 0; column < nc; column++) {
        for (row = 0; row < nr; row++) {
            ta = get_cell(err, row, column);
            MX_ERROR_CHECK(err);

            ta->set_xorigin(err, x);
            MX_ERROR_CHECK(err);

            ta->recalculate_outline(err, b);
            MX_ERROR_CHECK(err);
        }

        // all cells in column must be same width, so this is safe
        x += ta->get_width(err);
        MX_ERROR_CHECK(err);
    }

    // go across each row setting the y position and looking for the
    // greatest height
    for (row = 0; row < nr; row++) {
        height = 0.0;

        // set the y-origin and find the greatest height
        for (column = 0; column < nc; column++) {
            ta = get_cell(err, row, column);
            MX_ERROR_CHECK(err);

            ta->set_yorigin(err, y);
            MX_ERROR_CHECK(err);

            temp = ta->get_height(err);
            MX_ERROR_CHECK(err);

            if (temp > height) {
                height = temp;
            }
        }

        // set the heights all the way across
        for (column = 0; column < nc; column++) {
            ta = get_cell(err, row, column);
            MX_ERROR_CHECK(err);

            temp = ta->get_width(err);
            MX_ERROR_CHECK(err);

            ta->set_outline(err, temp, height);
            MX_ERROR_CHECK(err);
        }

        y += height;
    }
abort:;
}

void mx_table_area::recalculate_outline(int& err)
{
    float width = 0.0, height = 0.0, temp_height;
    int i, nc, nr;

    mx_point o;

    mx_text_area* ta;

    nc = get_num_columns(err);
    MX_ERROR_CHECK(err);

    nr = get_num_rows(err);
    MX_ERROR_CHECK(err);

    ta = get_cell(err, 0, nc - 1);
    MX_ERROR_CHECK(err);

    o = ta->get_origin(err);
    MX_ERROR_CHECK(err);

    width = o.x + ta->get_width(err);
    MX_ERROR_CHECK(err);

    for (i = 0; i < nc; i++) {
        ta = get_cell(err, nr - 1, i);
        MX_ERROR_CHECK(err);

        o = ta->get_origin(err);
        MX_ERROR_CHECK(err);

        temp_height = o.y + ta->get_height(err);
        MX_ERROR_CHECK(err);

        if (temp_height > height) {
            height = temp_height;
        }
    }

    set_outline(err, width, height);
    MX_ERROR_CHECK(err);
abort:;
}

void mx_table_area::replace_style(int& err,
    mx_paragraph_style* style_to_replace,
    mx_paragraph_style* replacement_style)
{
    int i, j, r, c;
    mx_text_area* t = NULL;

    mx_area::replace_style(err, style_to_replace, replacement_style);
    MX_ERROR_CHECK(err);

    c = get_num_columns(err);
    MX_ERROR_CHECK(err);

    r = get_num_rows(err);
    MX_ERROR_CHECK(err);

    for (i = 0; i < r; i++) {
        for (j = 0; j < c; j++) {
            t = get_cell(err, i, j);
            MX_ERROR_CHECK(err);

            t->replace_style(err, style_to_replace, replacement_style);
            MX_ERROR_CHECK(err);
        }
    }
abort:;
}

mx_table_area::mx_table_type_t mx_table_area::get_type(int& err)
{
    mx_table_area::mx_table_type_t res;

    res = (mx_table_area::mx_table_type_t)get_default_int(err, MX_TABLE_TYPE, (int)mx_table_area::mx_table_whole_e);
    MX_ERROR_CHECK(err);

    return res;

abort:
    return mx_table_area::mx_table_whole_e;
}

void mx_table_area::set_type(int& err, mx_table_area::mx_table_type_t t)
{
    set_int(err, MX_TABLE_TYPE, (int)t);
    MX_ERROR_CHECK(err);
abort:;
}

void mx_table_area::set_cell(int& err, int row, int column, mx_text_area* cell)
{
    int nc, i;

    mx_sheet* sheet = get_sheet(err);
    MX_ERROR_CHECK(err);

    cc_unserialise_attributes(err);
    MX_ERROR_CHECK(err);

    nc = get_num_columns(err);
    MX_ERROR_CHECK(err);

    i = (row * nc) + column;

    delete cell_objs[i];
    cell_objs[i] = cell;

    cell_objs[i]->set_owner_object(err, (mx_db_object*)sheet);
    MX_ERROR_CHECK(err);

    cell_oids[i] = cell_objs[i]->get_db_id(err);
    MX_ERROR_CHECK(err);

abort:;
}

mx_paragraph_style* mx_table_area::get_style(int& err, mx_area_pos* p)
{
    mx_table_area_pos* ap = (mx_table_area_pos*)p;
    mx_text_area* ta;
    mx_paragraph_style* res;

    ta = (mx_text_area*)get_cell(err, ap->row, ap->column);
    MX_ERROR_CHECK(err);

    res = ta->get_style(err, &(ap->text_pos));
    MX_ERROR_CHECK(err);

    return res;

abort:
    return NULL;
}

void mx_table_area::insert_char(int& err, char c, mx_table_area_pos* p)
{
    mx_text_area* ta;

    ta = (mx_text_area*)get_cell(err, p->row, p->column);
    MX_ERROR_CHECK(err);

    ta->insert_char(err, c, &(p->text_pos));
    MX_ERROR_CHECK(err);
abort:;
}

void mx_table_area::insert_cw_item(int& err, mx_cw_t* item,
    mx_table_area_pos* p)
{
    mx_text_area* ta;

    ta = (mx_text_area*)get_cell(err, p->row, p->column);
    MX_ERROR_CHECK(err);

    ta->insert_cw_item(err, item, &(p->text_pos));
    MX_ERROR_CHECK(err);
abort:;
}

mx_cw_t* mx_table_area::delete_to_left(
    int& err,
    mx_table_area_pos* p,
    bool& deleted_paragraph)
{
    mx_text_area* ta;
    mx_cw_t* res = NULL;

    ta = (mx_text_area*)get_cell(err, p->row, p->column);
    MX_ERROR_CHECK(err);

    res = ta->delete_to_left(err, &(p->text_pos), deleted_paragraph);
    MX_ERROR_CHECK(err);

abort:
    return res;
}

void mx_table_area::insert_line_break(int& err, mx_table_area_pos* p)
{
    mx_text_area* ta;

    ta = (mx_text_area*)get_cell(err, p->row, p->column);
    MX_ERROR_CHECK(err);

    ta->insert_line_break(err, &(p->text_pos));
    MX_ERROR_CHECK(err);
abort:;
}

void mx_table_area::insert_para_break(int& err, mx_table_area_pos* p)
{
    mx_text_area* ta;

    ta = (mx_text_area*)get_cell(err, p->row, p->column);
    MX_ERROR_CHECK(err);

    ta->insert_para_break(err, &(p->text_pos));
    MX_ERROR_CHECK(err);
abort:;
}

void mx_table_area::set_vertical_border(int& err, int row, int column,
    const mx_line_style& new_border)
{
    bool last_column;
    int num_columns;
    mx_text_area* txta;
    mx_paragraph_style ps;

    num_columns = get_num_columns(err);
    MX_ERROR_CHECK(err);

    MX_ERROR_ASSERT(err, column <= num_columns);

    last_column = (column == num_columns);
    if (last_column)
        column--;

    txta = get_cell(err, row, column);
    MX_ERROR_CHECK(err);

    ps = *txta->get_paragraph_style();

    if (last_column) {
        ps.get_border_style()->right_style = new_border;
    } else {
        ps.get_border_style()->left_style = new_border;
    }
    txta->modify_paragraph_style(ps);
abort:;
}

void mx_table_area::set_horizontal_border(int& err, int row, int column,
    const mx_line_style& new_border)
{
    bool last_row;
    int num_rows;
    mx_text_area* txta;
    mx_paragraph_style ps;

    num_rows = get_num_rows(err);
    MX_ERROR_CHECK(err);

    MX_ERROR_ASSERT(err, row <= num_rows);

    last_row = (row == num_rows);
    if (last_row)
        row--;

    txta = get_cell(err, row, column);
    MX_ERROR_CHECK(err);

    ps = *txta->get_paragraph_style();

    if (last_row) {
        ps.get_border_style()->bottom_style = new_border;
    } else {
        ps.get_border_style()->top_style = new_border;
    }
    txta->modify_paragraph_style(ps);
abort:;
}

void mx_table_area::get_vertical_border(int& err, int row, int column,
    mx_line_style& border)
{
    bool last_column;
    int num_columns;
    mx_text_area* txta;
    mx_paragraph_style ps;

    num_columns = get_num_columns(err);
    MX_ERROR_CHECK(err);

    MX_ERROR_ASSERT(err, column <= num_columns);

    last_column = (column == num_columns);
    if (last_column)
        column--;

    txta = get_cell(err, row, column);
    MX_ERROR_CHECK(err);

    ps = *txta->get_paragraph_style();

    if (last_column) {
        border = ps.get_border_style()->right_style;
    } else {
        border = ps.get_border_style()->left_style;
    }
    txta->modify_paragraph_style(ps);
abort:;
}

void mx_table_area::get_horizontal_border(int& err, int row, int column,
    mx_line_style& border)
{
    bool last_row;
    int num_rows;
    mx_text_area* txta;
    mx_paragraph_style ps;

    num_rows = get_num_rows(err);
    MX_ERROR_CHECK(err);

    MX_ERROR_ASSERT(err, row <= num_rows);

    last_row = (row == num_rows);
    if (last_row)
        row--;

    txta = get_cell(err, row, column);
    MX_ERROR_CHECK(err);

    ps = *txta->get_paragraph_style();

    if (last_row) {
        border = ps.get_border_style()->bottom_style;
    } else {
        border = ps.get_border_style()->top_style;
    }
    txta->modify_paragraph_style(ps);
abort:;
}

void mx_table_area::get_cell_borders_for_selection(
    int& err,
    const mx_table_area_pos& start,
    const mx_table_area_pos& end,
    mx_nlist& border_array)
{
    uint32 start_row = end.row < start.row ? end.row : start.row;
    uint32 end_row = end.row > start.row ? end.row : start.row;
    uint32 start_column = end.column < start.column ? end.column : start.column;
    uint32 end_column = end.column > start.column ? end.column : start.column;
    uint32 r, c;

    border_array = mx_nlist();

    // get the basic border styles

    for (r = start_row; r <= end_row; r++) {
        for (c = start_column; c <= end_column; c++) {
            mx_border_style* bs;
            mx_text_area* txta = this->get_cell(err, r, c);
            MX_ERROR_CHECK(err);

            bs = txta->get_paragraph_style()->get_border_style();

            border_array.insert(err, new mx_border_style(*bs),
                r - start_row,
                c - start_column);
            MX_ERROR_CHECK(err);
        }
    }

    // fix the last border styles for the bottom and right edges

    {
        mx_line_style ls;
        mx_border_style* bs;

        for (r = start_row; r <= end_row; r++) {
            get_vertical_border(err, r, end_column + 1, ls);
            MX_ERROR_CHECK(err);

            bs = (mx_border_style*)border_array.item(
                err, r - start_row, end_column - start_column);
            MX_ERROR_CHECK(err);

            bs->right_style = ls;
        }

        for (c = start_column; c <= end_column; c++) {
            get_horizontal_border(err, end_row + 1, c, ls);
            MX_ERROR_CHECK(err);

            bs = (mx_border_style*)border_array.item(
                err, end_row - start_row, c - start_column);
            MX_ERROR_CHECK(err);

            bs->bottom_style = ls;
        }
    }
abort:;
}

void mx_table_area::apply_border_mod_to_selection(
    int& err, const mx_table_area_pos& start, const mx_table_area_pos& end,
    const mx_border_style_mod& mod)
{
    uint32 start_row = end.row < start.row ? end.row : start.row;
    uint32 end_row = end.row > start.row ? end.row : start.row;
    uint32 start_column = end.column < start.column ? end.column : start.column;
    uint32 end_column = end.column > start.column ? end.column : start.column;
    uint32 r, c;
    mx_paragraph_style_mod psm;
    mx_fill_style fs;
    mx_line_style ls;
    float f;

    if (mod.get_top_style_mod(ls)) {
        for (c = start_column; c <= end_column; c++) {
            set_horizontal_border(err, start_row, c, ls);
            MX_ERROR_CHECK(err);
        }
    }

    if (mod.get_bottom_style_mod(ls)) {
        for (c = start_column; c <= end_column; c++) {
            set_horizontal_border(err, end_row + 1, c, ls);
            MX_ERROR_CHECK(err);
        }
    }

    if (mod.get_horizontal_style_mod(ls)) {
        for (r = start_row + 1; r <= end_row; r++) {
            for (c = start_column; c <= end_column; c++) {
                set_horizontal_border(err, r, c, ls);
                MX_ERROR_CHECK(err);
            }
        }
    }

    if (mod.get_left_style_mod(ls)) {
        for (r = start_row; r <= end_row; r++) {
            set_vertical_border(err, r, start_column, ls);
            MX_ERROR_CHECK(err);
        }
    }

    if (mod.get_right_style_mod(ls)) {
        for (r = start_row; r <= end_row; r++) {
            set_vertical_border(err, r, end_column + 1, ls);
            MX_ERROR_CHECK(err);
        }
    }

    if (mod.get_vertical_style_mod(ls)) {
        for (c = start_column + 1; c <= end_column; c++) {
            for (r = start_row; r <= end_row; r++) {
                set_vertical_border(err, r, c, ls);
                MX_ERROR_CHECK(err);
            }
        }
    }

    if (mod.get_distance_from_contents_mod(f)) {
        psm.get_border_style_mod()->set_distance_from_contents_mod(f);
    }

    if (mod.get_fill_style_mod(fs)) {
        psm.get_border_style_mod()->set_fill_style_mod(fs);
    }

    if (!psm.is_null()) {
        mx_text_area* txta;
        mx_paragraph_style ps;
        for (r = start_row; r <= end_row; r++) {
            for (c = start_column; c <= end_column; c++) {
                txta = this->get_cell(err, r, c);
                MX_ERROR_CHECK(err);
                ps = *txta->get_paragraph_style();
                ps += psm;
                txta->modify_paragraph_style(ps);
            }
        }
    }
abort:;
}

mx_area* mx_table_area::duplicate()
{
    mx_table_area* res;

    res = new mx_table_area(*this);

    return (mx_table_area*)res;
}

int mx_table_area::get_break_row_for_height(int& err, float f)
{
    int nr;
    int i;
    float h, height = 0.0;
    mx_text_area* ta;

    nr = get_num_rows(err);
    MX_ERROR_CHECK(err);

    for (i = 0; i < nr; i++) {
        ta = get_cell(err, i, 0);
        MX_ERROR_CHECK(err);

        h = ta->get_height(err);
        MX_ERROR_CHECK(err);

        if (h + height > f) {
            return i;
        }
        height += h;
    }

    return nr - 1;

abort:
    return -1;
}

float mx_table_area::get_height_from_row(int& err, int row)
{
    mx_text_area* ta;

    int i;
    float h, height = 0.0;

    for (i = 0; i < row; i++) {
        ta = get_cell(err, i, 0);
        MX_ERROR_CHECK(err);

        h = ta->get_height(err);
        MX_ERROR_CHECK(err);

        height += h;
    }

    return height;

abort:
    return 0.0;
}

mx_table_area* mx_table_area::repaginate_split(int& err, int row_num)
{
    mx_table_area* res = this->aka_split(err, row_num);
    MX_ERROR_CHECK(err);

    this->set_split_table_types_repagination(err, res);
    MX_ERROR_CHECK(err);

    return res;
abort:
    return NULL;
}

mx_table_area* mx_table_area::normal_split(int& err, const mx_table_area_pos& tap)
{
    mx_table_area* res = this->aka_split(err, tap.row);
    MX_ERROR_CHECK(err);

    this->set_split_table_types_normal(err, res);
    MX_ERROR_CHECK(err);

    return res;
abort:
    return NULL;
}

mx_table_area* mx_table_area::aka_split(int& err, int row_num)
{
    mx_table_area* res;
    mx_sheet* s;
    int index_position, nc, nr;
    int i, j, num_rows_in_result;

    nc = get_num_columns(err);
    MX_ERROR_CHECK(err);

    nr = get_num_rows(err);
    MX_ERROR_CHECK(err);

    if (row_num < 1 || row_num >= nr) {
        MX_ERROR_THROW(err, MX_POSITION_RANGE_ERROR);
    }

    s = this->get_sheet(err);
    MX_ERROR_CHECK(err);

    index_position = s->get_index_position(err, this);
    MX_ERROR_CHECK(err);

    res = s->add_table_area(err, index_position + 1);
    MX_ERROR_CHECK(err);

    cc_unserialise_attributes(err);
    MX_ERROR_CHECK(err);

    set_int(err, MX_TABLE_NUM_ROWS, row_num);
    MX_ERROR_CHECK(err);

    num_rows_in_result = nr - row_num;

    res->set_int(err, MX_TABLE_NUM_ROWS, num_rows_in_result);
    MX_ERROR_CHECK(err);

    res->set_int(err, MX_TABLE_NUM_COLUMNS, nc);
    MX_ERROR_CHECK(err);

    res->cell_oids = new uint32[nc * num_rows_in_result];
    res->cell_objs = new mx_text_area*[nc * num_rows_in_result];

    for (i = 0; i < nc; i++) {
        for (j = row_num; j < nr; j++) {
            res->cell_objs[((j - row_num) * nc) + i] = cell_objs[(j * nc) + i];
            res->cell_oids[((j - row_num) * nc) + i] = cell_oids[(j * nc) + i];

            res->cell_objs[((j - row_num) * nc) + i]->set_owner_object(err, res);
            MX_ERROR_CHECK(err);
        }
    }

    res->base_style = base_style;
    res->style_mod = style_mod;
    if (res->style_mod != NULL) {
        res->style_mod = new mx_paragraph_style_mod(*res->style_mod);
    }

    recalculate_outline(err);
    MX_ERROR_CHECK(err);

    res->reposition_cells(err);
    MX_ERROR_CHECK(err);

    res->recalculate_outline(err);
    MX_ERROR_CHECK(err);

    return res;
abort:
    return NULL;
}

void mx_table_area::set_split_table_types_normal(int& err,
    mx_table_area* table2)
{
    mx_table_type_t this_type, other_type;

    other_type = this_type = this->get_type(err);
    MX_ERROR_CHECK(err);

    switch (this_type) {
    case mx_table_start_e:
        this_type = mx_table_whole_e;
        break;
    case mx_table_middle_e:
        this_type = mx_table_end_e;
        other_type = mx_table_start_e;
        break;
    case mx_table_end_e:
        other_type = mx_table_whole_e;
        break;
    case mx_table_whole_e:
        break;
    default:
        MX_ERROR_THROW(err, MX_ERROR_UNDEFINED);
    }

    this->set_type(err, this_type);
    MX_ERROR_CHECK(err);

    table2->set_type(err, other_type);
    MX_ERROR_CHECK(err);
abort:;
}

void mx_table_area::set_split_table_types_repagination(int& err,
    mx_table_area* table2)
{
    mx_table_type_t this_type, other_type;

    other_type = this_type = this->get_type(err);
    MX_ERROR_CHECK(err);

    switch (this_type) {
    case mx_table_start_e:
        other_type = mx_table_middle_e;
        break;
    case mx_table_middle_e:
        break;
    case mx_table_end_e:
        this_type = mx_table_middle_e;
        break;
    case mx_table_whole_e:
        this_type = mx_table_start_e;
        other_type = mx_table_end_e;
        break;
    default:
        MX_ERROR_THROW(err, MX_ERROR_UNDEFINED);
    }

    this->set_type(err, this_type);
    MX_ERROR_CHECK(err);

    table2->set_type(err, other_type);
    MX_ERROR_CHECK(err);
abort:;
}

bool mx_table_area::can_join(int& err, mx_area* a)
{
    int this_nc, other_nc, i;
    mx_table_area* taba = (mx_table_area*)a;

    if (!a->is_a(mx_table_area_class_e))
        return FALSE;

    this_nc = this->get_num_columns(err);
    MX_ERROR_CHECK(err);

    other_nc = taba->get_num_columns(err);
    MX_ERROR_CHECK(err);

    if (this_nc != other_nc)
        return FALSE;

    for (i = 0; i < this_nc; i++) {
        float this_w, other_w;

        this_w = this->get_column_width(err, i);
        MX_ERROR_CHECK(err);

        other_w = taba->get_column_width(err, i);
        MX_ERROR_CHECK(err);

        if (!MX_FLOAT_EQ(this_w, other_w))
            return FALSE;
    }
    return TRUE;

abort:
    return FALSE;
}

void mx_table_area::join(int& err, mx_area* a)
{
    mx_sheet* sheet;
    mx_table_area* other = (mx_table_area*)a;
    int this_nc, other_nc, this_nr, other_nr, new_nr, i, j;
    uint32* new_cell_oids = NULL;
    mx_text_area** new_cell_objs = NULL;
    mx_table_type_t this_type, other_type, new_type;

    MX_ERROR_ASSERT(err, a->is_a(mx_table_area_class_e));

    this_nc = get_num_columns(err);
    MX_ERROR_CHECK(err);

    other_nc = other->get_num_columns(err);
    MX_ERROR_CHECK(err);

    if (this_nc != other_nc) {
        MX_ERROR_THROW(err, MX_TABLE_AREA_COLUMN_INDEX);
    }

    this_nr = get_num_rows(err);
    MX_ERROR_CHECK(err);

    other_nr = other->get_num_rows(err);
    MX_ERROR_CHECK(err);

    new_nr = this_nr + other_nr;

    this_type = this->get_type(err);
    MX_ERROR_CHECK(err);

    other_type = other->get_type(err);
    MX_ERROR_CHECK(err);

    cc_unserialise_attributes(err);
    MX_ERROR_CHECK(err);

    other->cc_unserialise_attributes(err);
    MX_ERROR_CHECK(err);

    new_cell_oids = new uint32[new_nr * this_nc];
    new_cell_objs = new mx_text_area*[new_nr * this_nc];

    memcpy(new_cell_oids, this->cell_oids, (this_nr * this_nc * sizeof(uint32)));
    memcpy(new_cell_objs, this->cell_objs, (this_nr * this_nc * sizeof(mx_text_area*)));
    memcpy((new_cell_oids + (this_nr * this_nc)), other->cell_oids, (other_nr * other_nc * sizeof(uint32)));
    memcpy((new_cell_objs + (this_nr * this_nc)), other->cell_objs, (other_nr * other_nc * sizeof(mx_text_area*)));

    other->set_int(err, MX_TABLE_NUM_ROWS, 0);
    MX_ERROR_CHECK(err);

    other->set_int(err, MX_TABLE_NUM_COLUMNS, 0);
    MX_ERROR_CHECK(err);

    delete[] other->cell_oids;
    delete[] other->cell_objs;
    other->cell_oids = NULL;
    other->cell_objs = NULL;

    delete[] this->cell_oids;
    delete[] this->cell_objs;
    this->cell_oids = new_cell_oids;
    this->cell_objs = new_cell_objs;

    sheet = this->get_sheet(err);
    MX_ERROR_CHECK(err);

    for (i = 0; i < this_nc; i++) {
        for (j = 0; j < new_nr; j++) {
            this->cell_objs[(j * this_nc) + i]->set_owner_object(err, sheet);
            MX_ERROR_CHECK(err);
        }
    }

    this->set_int(err, MX_TABLE_NUM_ROWS, new_nr);
    MX_ERROR_CHECK(err);

    this->set_int(err, MX_TABLE_NUM_COLUMNS, this_nc);
    MX_ERROR_CHECK(err);

    switch (this_type) {
    case mx_table_whole_e:
    case mx_table_start_e:
        switch (other_type) {
        case mx_table_whole_e:
        case mx_table_end_e:
            new_type = mx_table_whole_e;
            break;
        case mx_table_start_e:
        case mx_table_middle_e:
            new_type = mx_table_start_e;
            break;
        default:
            MX_ERROR_THROW(err, MX_ERROR_UNDEFINED);
        }
        break;
    case mx_table_middle_e:
    case mx_table_end_e:
        switch (other_type) {
        case mx_table_whole_e:
        case mx_table_end_e:
            new_type = mx_table_end_e;
            break;
        case mx_table_start_e:
        case mx_table_middle_e:
            new_type = mx_table_middle_e;
            break;
        default:
            MX_ERROR_THROW(err, MX_ERROR_UNDEFINED);
        }
        break;
    default:
        MX_ERROR_THROW(err, MX_ERROR_UNDEFINED);
        break;
    }

    this->set_type(err, new_type);
    MX_ERROR_CHECK(err);

    this->reposition_cells(err);
    MX_ERROR_CHECK(err);

    this->recalculate_outline(err);
    MX_ERROR_CHECK(err);

abort:;
}

void mx_table_area::print_debug()
{
}

mx_db_mem_state_t mx_table_area::get_mem_state()
{
    int nr, nc, i, err = MX_ERROR_OK;

    if (mx_db_object::get_mem_state() == mx_in_mem_and_modified_e || mx_db_object::get_mem_state() == mx_on_disk_e) {
        return mx_db_object::get_mem_state();
    }

    nc = get_num_columns(err);
    MX_ERROR_CHECK(err);

    nr = get_num_rows(err);
    MX_ERROR_CHECK(err);

    for (i = 0; i < (nc * nr); i++) {
        if (cell_objs[i]->get_mem_state() == mx_in_mem_and_modified_e) {
            mx_db_object::set_mem_state(mx_in_mem_and_modified_e);
            return mx_in_mem_and_modified_e;
        }
    }

abort:
    return mx_in_mem_e;
}

void mx_table_area::set_mem_state(mx_db_mem_state_t s)
{
    int nr, nc, i, err = MX_ERROR_OK;

    mx_db_object::set_mem_state(s);

    if (cell_objs != NULL) {
        nc = get_num_columns(err);
        MX_ERROR_CHECK(err);

        nr = get_num_rows(err);
        MX_ERROR_CHECK(err);

        for (i = 0; i < (nc * nr); i++) {
            cell_objs[i]->set_mem_state(s);
        }
    }

abort:;
}
