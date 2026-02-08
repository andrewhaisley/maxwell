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
 * MODULE/CLASS : mx_table_buffer
 *
 * AUTHOR : Andrew Haisley
 *
 *
 *
 * DESCRIPTION:
 *
 *
 *
 *
 *
 */
#ifndef MX_TABLE_BUFFER_H
#define MX_TABLE_BUFFER_H

#include "mx_buffer.h"
#include <mx.h>
#include <mx_rtti.h>
#include <mx_text_area.h>

class mx_table_area;
class mx_table_area_pos;

class mx_table_buffer : public mx_buffer {
    MX_RTTI(mx_table_buffer_class_e)
public:
    mx_table_buffer(mx_wp_document*);
    mx_table_buffer(const mx_table_buffer&);
    virtual ~mx_table_buffer();

    void cut(int& err,
        mx_table_area& a,
        mx_table_area_pos& p1,
        mx_table_area_pos& p2,
        mx_edit_op cut_or_copy);

    void insert(int& err, mx_table_area& a, mx_table_area_pos& p);

    int num_rows;
    int num_columns;

private:
    void clear_old_contents();
    void cut_cells(int& err,
        mx_table_area& a,
        int top_row,
        int top_column,
        int bottom_row,
        int bottom_column,
        mx_edit_op cut_or_copy);

    virtual void move_to_document(int& err, mx_wp_document* d);

    mx_text_area** areas;
};

#endif
