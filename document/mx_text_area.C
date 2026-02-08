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
 * MODULE/CLASS : mx_text_area
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
 */

#include <mx.h>
#include <mx_cw_t.h>
#include <mx_doc.h>
#include <mx_list.h>
#include <mx_list_iter.h>
#include <mx_paragraph.h>
#include <mx_text_area.h>
#include <mx_txt_paint.h>
#include <mx_txt_pos.h>

mx_text_area::mx_text_area(uint32 docid, mx_paragraph_style* style, bool is_controlled)
    : mx_area(docid, style, is_controlled)
{
    int err = MX_ERROR_OK;
    mx_document* doc;
    mx_paragraph* p;

    // get a handle on a suitable painter
    painter = mx_text_area_painter::painter();

    doc = this->get_document(err);
    MX_ERROR_CHECK(err);

    // totally new area, so add a paragraph
    p = new mx_paragraph;
    p->set_paragraph_style(style);
    p->set_document(doc);

    paragraphs.append(p);
    return;
abort:
    global_error_trace->print();
    MX_ERROR_CLEAR(err);
}

mx_text_area::mx_text_area(uint32 docid, uint32 oid, bool is_controlled)
    : mx_area(docid, oid, is_controlled)
{
    // get a handle on a suitable painter
    painter = mx_text_area_painter::painter();
}

mx_text_area::mx_text_area(mx_text_area& a)
    : mx_area(*((mx_area*)&a))
{
    mx_paragraph* p;
    mx_list_iterator iter(a.paragraphs);

    // get a handle on a suitable painter
    painter = mx_text_area_painter::painter();

    while (iter.more()) {
        p = (mx_paragraph*)iter.data();
        paragraphs.append(new mx_paragraph(*p));
    }
}

void mx_text_area::clear_contents(int& err)
{
    mx_document* doc;
    mx_paragraph* p;

    cc_unserialise_attributes(err);
    MX_ERROR_CHECK(err);

    set_mem_state(mx_in_mem_and_modified_e);

    while (paragraphs.get_num_items() > 0) {
        p = (mx_paragraph*)paragraphs.remove(err, 0);
        MX_ERROR_CHECK(err);

        delete p;
    }

    doc = this->get_document(err);
    MX_ERROR_CHECK(err);

    p = new mx_paragraph;
    p->set_paragraph_style(base_style);
    p->set_document(doc);

    paragraphs.append(p);
abort:;
}

mx_text_area::~mx_text_area()
{
    mx_list_iterator iter(paragraphs);
    mx_paragraph* a_para;

    while (iter.more()) {
        a_para = (mx_paragraph*)iter.data();
        delete a_para;
    }
}

// called by the client cache when it wants to free
// some real memory. It serialises each paragraph and
// stores them in a blob. The other attributes are :
// the number of paragraphs
// the outline_cache               } from mx_area
// the background/foreground_cache }

void mx_text_area::uncache(int& err)
{
    // this needs writing properly at some point.....
}

// called by the client cache when committing data
// should serialise each paragraph
void mx_text_area::serialise_attributes(int& err)
{
    int32 blob_size, n_paras;
    uint8 *buffer, *tmp;

    n_paras = num_paragraphs(err);
    MX_ERROR_CHECK(err);

    if (n_paras == 0) {
        return;
    }

    blob_size = get_serialised_size(err);
    MX_ERROR_CHECK(err);

    if (blob_size < MX_ATTRIBUTE_MAX_SIZE) {
        mx_attribute a;

        delete_blob(err, MX_TEXT_AREA_PARAS);
        MX_ERROR_CLEAR(err);

        strcpy(a.name, MX_TEXT_AREA_PARAS);
        a.type = mx_char_array;

        tmp = buffer = new uint8[blob_size];
        a.length = blob_size;
        a.value.ca = (char*)tmp;

        serialise(err, &buffer);
        MX_ERROR_CHECK(err);

        set(err, &a);
        MX_ERROR_CHECK(err);

        delete[] tmp;
    } else {
        delete_attribute(err, MX_TEXT_AREA_PARAS);
        MX_ERROR_CLEAR(err);

        create_blob(err, MX_TEXT_AREA_PARAS);
        if (err == MX_DB_CLIENT_CACHE_DUPLICATE_BLOB) {
            MX_ERROR_CLEAR(err);
        } else {
            MX_ERROR_CHECK(err);
        }

        tmp = buffer = new uint8[blob_size];

        set_blob_size(err, MX_TEXT_AREA_PARAS, blob_size);
        MX_ERROR_CHECK(err);

        serialise(err, &buffer);
        MX_ERROR_CHECK(err);

        set_blob_data(err, MX_TEXT_AREA_PARAS, 0, blob_size, (uint8*)tmp);
        MX_ERROR_CHECK(err);

        delete[] tmp;
    }

abort:
    return;
}

// called by the client cache
// returns amount of real memory used.
uint32 mx_text_area::memory_size(int& err)
{
    return 0;
}

void mx_text_area::unserialise_attributes(int& err)
{
    int32 size;

    const uint8* buffer;
    uint8 *temp_buffer, *tmp;
    bool b;

    size = get_blob_size(err, MX_TEXT_AREA_PARAS);
    if (err != MX_ERROR_OK) {
        MX_ERROR_CLEAR(err);

        const mx_attribute* a;
        uint8* tmp;

        a = get_readonly(err, MX_TEXT_AREA_PARAS);
        MX_ERROR_CHECK(err);

        tmp = (uint8*)a->value.ca;

        unserialise(err, &tmp);
        MX_ERROR_CHECK(err);
    } else {
        if (size > MX_DB_OBJECT_MAX_READONLY_BLOB_SIZE) {
            tmp = temp_buffer = new uint8[size];

            get_blob_data(err, MX_TEXT_AREA_PARAS, 0, size, temp_buffer);
            MX_ERROR_CHECK(err);

            unserialise(err, &temp_buffer);
            MX_ERROR_CHECK(err);

            delete[] tmp;
        } else {
            buffer = get_blob_readonly(err, MX_TEXT_AREA_PARAS, 0, size);
            MX_ERROR_CHECK(err);

            unserialise(err, (uint8**)&buffer);
            MX_ERROR_CHECK(err);
        }
    }

    reformat(err, b);
    MX_ERROR_CHECK(err);

abort:
    return;
}

int mx_text_area::num_paragraphs(int& err)
{
    cc_unserialise_attributes(err);
    MX_ERROR_CHECK(err);

    return paragraphs.get_num_items();

abort:
    return -1;
}

void mx_text_area::serialise(int& err, uint8** buffer)
{
    uint32 n_paras;
    uint32 i;
    mx_paragraph* para;

    mx_area::serialise(err, buffer);
    MX_ERROR_CHECK(err);

    n_paras = num_paragraphs(err);
    MX_ERROR_CHECK(err);

    serialise_uint32(n_paras, buffer);

    // put the paragraphs in
    for (i = 0; i < n_paras; i++) {
        para = get(err, i);
        MX_ERROR_CHECK(err);

        para->minimise_style_mods(err);
        MX_ERROR_CHECK(err);

        para->serialise(err, buffer);
        MX_ERROR_CHECK(err);
    }

abort:
    return;
}

uint32 mx_text_area::get_serialised_size(int& err)
{
    uint32 n_paras;
    uint32 i;
    uint32 res;
    mx_paragraph* para;

    res = mx_area::get_serialised_size(err);
    MX_ERROR_CHECK(err);

    res += SLS_UINT32;

    n_paras = num_paragraphs(err);
    MX_ERROR_CHECK(err);

    // get the size of the paragraphs
    for (i = 0; i < n_paras; i++) {
        para = get(err, i);
        MX_ERROR_CHECK(err);

        res += para->get_serialised_size(err);
        MX_ERROR_CHECK(err);
    }

    return res;
abort:
    return 0;
}

void mx_text_area::unserialise(int& err, uint8** buffer)
{
    uint32 n_paras, i;
    bool b = FALSE;
    mx_document* doc;
    float w = 0;

    doc = get_document(err);
    MX_ERROR_CHECK(err);

    mx_area::unserialise(err, buffer);
    MX_ERROR_CHECK(err);

    unserialise_uint32(n_paras, buffer);

    w = width_minus_border(err);
    MX_ERROR_CHECK(err);

    for (i = 0; i < n_paras; i++) {
        mx_paragraph* p = new mx_paragraph;
        MX_ERROR_CHECK(err);

        p->set_document(doc);

        p->unserialise(err, buffer);
        MX_ERROR_CHECK(err);

        p->set_width(w);
        MX_ERROR_CHECK(err);

        p->reformat(err, b);
        MX_ERROR_CHECK(err);

        insert(err, i, p);
        MX_ERROR_CHECK(err);
    }

    b = FALSE;

    reformat(err, b);
    MX_ERROR_CHECK(err);

    recalculate_outline(err, b);
    MX_ERROR_CHECK(err);

abort:
    return;
}

mx_paragraph* mx_text_area::get(int& err, int index)
{
    cc_unserialise_attributes(err);
    MX_ERROR_CHECK(err);

    return (mx_paragraph*)paragraphs.get(err, index);

abort:
    return NULL;
}

mx_paragraph* mx_text_area::remove(int& err, int index)
{
    bool b;
    mx_paragraph* res;

    cc_unserialise_attributes(err);
    MX_ERROR_CHECK(err);

    set_mem_state(mx_in_mem_and_modified_e);

    res = (mx_paragraph*)paragraphs.remove(err, index);
    MX_ERROR_CHECK(err);

    recalculate_outline(err, b);
    MX_ERROR_CHECK(err);

    return res;

abort:
    return NULL;
}

void mx_text_area::insert(int& err, int index, mx_paragraph* p)
{
    mx_document* doc;

    doc = get_document(err);
    MX_ERROR_CHECK(err);

    cc_unserialise_attributes(err);
    MX_ERROR_CHECK(err);

    set_mem_state(mx_in_mem_and_modified_e);

    paragraphs.insert(err, index, p);
    MX_ERROR_CHECK(err);

    p->set_width(get_width(err));
    MX_ERROR_CHECK(err);

    p->set_document(doc);

    return;

abort:
    return;
}

mx_object_buffer* mx_text_area::force_height(int& err, float requested, float& actual)
{
    return NULL;
}

void mx_text_area::append(int& err, mx_paragraph* p)
{
    bool b;
    mx_paragraph* last;

    cc_unserialise_attributes(err);
    MX_ERROR_CHECK(err);

    set_mem_state(mx_in_mem_and_modified_e);

    p->set_width(get_width(err));
    MX_ERROR_CHECK(err);

    if (paragraphs.get_num_items() > 0) {
        last = (mx_paragraph*)paragraphs.last();

        if (last->get_type() == mx_paragraph::mx_paragraph_start_e || last->get_type() == mx_paragraph::mx_paragraph_middle_e) {
            last->join(err, p);
            MX_ERROR_CHECK(err);
        } else {
            paragraphs.append(p);
        }
    } else {
        paragraphs.append(p);
    }

    recalculate_outline(err, b);
    MX_ERROR_CHECK(err);

abort:;
}

void mx_text_area::reformat(
    int& err,
    mx_text_area_pos& start,
    mx_text_area_pos& end,
    bool& height_changed)
{
    mx_paragraph* a_para;
    mx_para_pos pp_start;
    mx_para_pos pp_end;
    int i = 0;
    bool b = FALSE;
    float w;

    cc_unserialise_attributes(err);
    MX_ERROR_CHECK(err);

    set_mem_state(mx_in_mem_and_modified_e);

    for (i = start.paragraph_index; i <= end.paragraph_index; i++) {
        a_para = (mx_paragraph*)paragraphs.get(err, i);
        MX_ERROR_CHECK(err);

        // is this paragraph the one to start from
        if (start.paragraph_index == i) {
            pp_start = start.para_pos;
        } else {
            pp_start = mx_para_pos(0, 0, 0);
        }

        // is this paragraph the one to end in
        if (end.paragraph_index == i) {
            pp_end = end.para_pos;
        } else {
            pp_end.moveto_end(err, a_para);
            MX_ERROR_CHECK(err);
        }

        w = width_minus_border(err);
        MX_ERROR_CHECK(err);

        a_para->set_width(w);
        MX_ERROR_CHECK(err);

        a_para->reformat(err, pp_start, pp_end, b);
        MX_ERROR_CHECK(err);

        if (start.paragraph_index == i) {
            start.para_pos = pp_start;
        }
    }

    recalculate_outline(err, height_changed);
    MX_ERROR_CHECK(err);

abort:;
}

void mx_text_area::recalculate_outline(int& err, bool& height_changed)
{
    // only the height can change after a reformat.
    float old_height, new_height = 0.0;
    float width;

    mx_list_iterator iter(paragraphs);
    mx_paragraph* para;
    mx_border_style* bs;

    cc_unserialise_attributes(err);
    MX_ERROR_CHECK(err);

    old_height = get_height(err);
    MX_ERROR_CHECK(err);

    while (iter.more()) {
        para = (mx_paragraph*)iter.data();
        new_height += para->get_height();
    }

    width = get_width(err);
    MX_ERROR_CHECK(err);

    bs = this->get_area_style()->get_border_style();

    if (bs->top_style.line_type != mx_no_line) {
        new_height += bs->top_style.width;
    }

    if (bs->bottom_style.line_type != mx_no_line) {
        new_height += bs->bottom_style.width;
    }
    new_height += 2 * bs->distance_from_contents;

    if (!MX_FLOAT_EQ(old_height, new_height)) {
        set_mem_state(mx_in_mem_and_modified_e);

        height_changed = TRUE;
        set_outline(err, width, new_height);
        MX_ERROR_CHECK(err);
    } else {
        height_changed = FALSE;
    }

abort:;
}

// reformat the whole area
void mx_text_area::reformat(int& err, bool& height_changed)
{
    cc_unserialise_attributes(err);
    MX_ERROR_CHECK(err);

    {
        mx_text_area_pos start(0, 0, 0, 0);
        mx_text_area_pos end;

        end.moveto_end(err, this);

        reformat(err, start, end, height_changed);
        MX_ERROR_CHECK(err);
    }
abort:;
}

void mx_text_area::insert(int& err, mx_paragraph* para, mx_text_area_pos& p)
{
    mx_paragraph *temp1, *temp2;

    cc_unserialise_attributes(err);
    MX_ERROR_CHECK(err);

    set_mem_state(mx_in_mem_and_modified_e);

    // if the position is at the end or start of an existing paragraph, just
    // move it up or down
    if (p.para_pos.is_start()) {
        // at start - move existing paragraph down
        insert(err, p.paragraph_index, para);
        MX_ERROR_CHECK(err);
    } else {
        temp1 = get(err, p.paragraph_index);
        MX_ERROR_CHECK(err);

        if (p.para_pos.is_end(err, temp1)) {
            MX_ERROR_CHECK(err);

            // at end
            insert(err, ++p.paragraph_index, para);
            MX_ERROR_CHECK(err);
        } else {
            MX_ERROR_CHECK(err);

            // in middle, have to split the paragraph
            temp2 = temp1->split(err, p.para_pos);
            MX_ERROR_CHECK(err);

            insert(err, ++p.paragraph_index, temp2);
            MX_ERROR_CHECK(err);

            insert(err, p.paragraph_index, para);
            MX_ERROR_CHECK(err);
        }
    }
abort:;
}

void mx_text_area::insert_adjust_position(int& err, mx_paragraph* para, mx_text_area_pos& p)
{
    mx_paragraph *temp1, *temp2;

    cc_unserialise_attributes(err);
    MX_ERROR_CHECK(err);

    set_mem_state(mx_in_mem_and_modified_e);

    // if the position is at the end or start of an existing paragraph, just
    // move it up or down
    if (p.para_pos.is_start()) {
        // at start - move existing paragraph down
        insert(err, p.paragraph_index++, para);
        MX_ERROR_CHECK(err);

        p.para_pos.moveto_start();
    } else {
        temp1 = get(err, p.paragraph_index);
        MX_ERROR_CHECK(err);

        if (p.para_pos.is_end(err, temp1)) {
            MX_ERROR_CHECK(err);

            // at end
            insert(err, ++p.paragraph_index, para);
            MX_ERROR_CHECK(err);

            p.para_pos.moveto_end(err, para);
            MX_ERROR_CHECK(err);
        } else {
            MX_ERROR_CHECK(err);

            // in middle, have to split the paragraph
            temp2 = temp1->split(err, p.para_pos);
            MX_ERROR_CHECK(err);

            insert(err, ++p.paragraph_index, temp2);
            MX_ERROR_CHECK(err);

            insert(err, p.paragraph_index, para);
            MX_ERROR_CHECK(err);

            p.paragraph_index++;
            p.para_pos.moveto_start();
        }
    }
abort:;
}

mx_paragraph* mx_text_area::insert_new_paragraph(
    int& err,
    mx_text_area_pos& p)
{
    mx_paragraph* para;
    mx_document* doc;
    mx_paragraph_style* s;

    cc_unserialise_attributes(err);
    MX_ERROR_CHECK(err);

    set_mem_state(mx_in_mem_and_modified_e);

    doc = get_document(err);
    MX_ERROR_CHECK(err);

    s = doc->get_default_style(err);
    MX_ERROR_CHECK(err);

    para = new mx_paragraph;
    para->set_paragraph_style(s);
    para->set_document(doc);

    insert(err, para, p);
    MX_ERROR_CHECK(err);

    return para;
abort:
    return NULL;
}

void mx_text_area::move_to_document(int& err, mx_document* doc)
{
    mx_list_iterator i(paragraphs);
    mx_paragraph* p = NULL;

    cc_unserialise_attributes(err);
    MX_ERROR_CHECK(err);

    mx_area::move_to_document(err, doc);
    MX_ERROR_CHECK(err);

    while (i.more()) {
        p = (mx_paragraph*)i.data();

        p->move_to_document(err, doc);
        MX_ERROR_CHECK(err);
    }
abort:;
}

void mx_text_area::replace_style(int& err,
    mx_paragraph_style* style_to_replace,
    mx_paragraph_style* replacement_style)
{
    mx_list_iterator i(paragraphs);
    mx_paragraph* p = NULL;

    cc_unserialise_attributes(err);
    MX_ERROR_CHECK(err);

    set_mem_state(mx_in_mem_and_modified_e);

    mx_area::replace_style(err, style_to_replace, replacement_style);
    MX_ERROR_CHECK(err);

    while (i.more()) {
        p = (mx_paragraph*)i.data();

        if (p->get_base_paragraph_style() == style_to_replace) {
            p->set_paragraph_style(replacement_style);
        }
    }
abort:;
}

float mx_text_area::paragraph_y_offset(int& err, int index)
{
    int i;
    float res = 0.0;
    mx_paragraph* p;

    cc_unserialise_attributes(err);
    MX_ERROR_CHECK(err);

    for (i = 0; i < index; i++) {
        p = (mx_paragraph*)paragraphs.get(err, i);
        MX_ERROR_CHECK(err);

        res += p->get_height();
    }

abort:
    return res;
}

mx_text_area* mx_text_area::split(int& err, mx_text_area_pos& p)
{
    mx_paragraph* para;
    bool is_start_para, is_end_para;
    mx_sheet* s;
    mx_text_area* new_area = NULL;
    int index, np;
    float width;

    cc_unserialise_attributes(err);
    MX_ERROR_CHECK(err);

    set_mem_state(mx_in_mem_and_modified_e);

    // get the paragraph to split
    para = (mx_paragraph*)paragraphs.get(err, p.paragraph_index);
    MX_ERROR_CHECK(err);

    // get the number of paragraphs
    np = paragraphs.get_num_items();

    // is the position at the start or end (or both!) of a paragraph?
    is_start_para = p.para_pos.is_start();
    MX_ERROR_CHECK(err);

    is_end_para = p.para_pos.is_end(err, para);
    MX_ERROR_CHECK(err);

    if ((is_start_para && p.paragraph_index == 0) || (!is_start_para && is_end_para && p.paragraph_index == (np - 1))) {
        // It is an error to split a text area at the start of the first
        // paragraph or at the end of the last paragraph if its not empty
        MX_ERROR_THROW(err, MX_TEXT_AREA_INVALID_SPLIT_POS);
    } else {
        // create a new area
        s = get_sheet(err);
        MX_ERROR_CHECK(err);

        index = s->get_index_position(err, this);
        MX_ERROR_CHECK(err);

        new_area = s->add_text_area(err, index + 1);
        MX_ERROR_CHECK(err);

        width = get_width(err);
        MX_ERROR_CHECK(err);

        new_area->set_outline(err, width, 1);
        MX_ERROR_CHECK(err);

        // delete the paragraph in the new area
        para = (mx_paragraph*)new_area->paragraphs.remove(err, 0);
        MX_ERROR_CHECK(err);

        delete para;

        if (is_start_para) {
            // shift all the paragraphs including this one down to the new area
            paragraphs.move_items_out_end(err,
                np - p.paragraph_index,
                new_area->paragraphs);
            MX_ERROR_CHECK(err);
        } else if (is_end_para) {
            // shift all the paragraphs below this one down to the new area
            paragraphs.move_items_out_end(err,
                np - p.paragraph_index - 1,
                new_area->paragraphs);
            MX_ERROR_CHECK(err);
        } else {
            // we are in the middle of a paragraph, so split it and then split the
            // text area

            // split paragraph first
            mx_paragraph* new_para;
            mx_paragraph* para;

            para = (mx_paragraph*)paragraphs.get(err, p.paragraph_index);
            MX_ERROR_CHECK(err);

            new_para = para->split(err, p.para_pos);
            MX_ERROR_CHECK(err);

            paragraphs.insert(err, p.paragraph_index + 1, new_para);
            MX_ERROR_CHECK(err);

            // shift all the paragraphs including the new one down to the new area
            paragraphs.move_items_out_end(err,
                np - p.paragraph_index,
                new_area->paragraphs);
            MX_ERROR_CHECK(err);
        }
    }
abort:
    return NULL;
}

mx_cw_t* mx_text_area::delete_to_left(
    int& err,
    mx_text_area_pos* p,
    bool& paragraph_deleted)
{
    mx_paragraph *para, *prev;
    bool b;
    mx_cw_t* res = NULL;

    cc_unserialise_attributes(err);
    MX_ERROR_CHECK(err);

    set_mem_state(mx_in_mem_and_modified_e);

    para = get(err, p->paragraph_index);
    MX_ERROR_CHECK(err);

    b = p->para_pos.is_start();

    if (b) {
        if (p->paragraph_index == 0) {
            MX_ERROR_THROW(err, MX_POSITION_RANGE_ERROR);
        } else {
            mx_para_pos join_pos;

            prev = get(err, p->paragraph_index - 1);
            MX_ERROR_CHECK(err);

            paragraphs.remove(err, p->paragraph_index);
            MX_ERROR_CHECK(err);

            paragraph_deleted = TRUE;

            // join with previous paragraph
            join_pos = prev->join(err, para);
            MX_ERROR_CHECK(err);

            prev->reformat_styles(err);
            MX_ERROR_CHECK(err);

            p->para_pos = join_pos;
            p->paragraph_index--;

            return new mx_character_cw_t(MX_CW_T_PARA_BREAK_CHAR);
        }
    } else {
        res = para->delete_to_left(err, &(p->para_pos));
        MX_ERROR_CHECK(err);

        return res;
    }

abort:
    return NULL;
}

mx_paragraph_style* mx_text_area::get_style(int& err, mx_area_pos* p)
{
    mx_text_area_pos* ap = (mx_text_area_pos*)p;
    mx_paragraph* para;
    mx_char_style* cs;
    static mx_paragraph_style* temp_style = NULL;

    if (temp_style == NULL) {
        temp_style = new mx_paragraph_style;
    }

    cc_unserialise_attributes(err);
    MX_ERROR_CHECK(err);

    para = (mx_paragraph*)paragraphs.get(err, ap->paragraph_index);
    MX_ERROR_CHECK(err);

    *temp_style = *(para->get_paragraph_style());

    cs = para->get_style(err, ap->para_pos);
    MX_ERROR_CHECK(err);

    temp_style->set_char_style(*cs);

    return temp_style;
abort:
    return NULL;
}

void mx_text_area::insert_char(int& err, char c, mx_text_area_pos* p)
{
    mx_paragraph* para;

    cc_unserialise_attributes(err);
    MX_ERROR_CHECK(err);

    set_mem_state(mx_in_mem_and_modified_e);

    para = (mx_paragraph*)paragraphs.get(err, p->paragraph_index);
    MX_ERROR_CHECK(err);

    para->insert_char(err, c, p->para_pos);
    MX_ERROR_CHECK(err);

abort:;
}

void mx_text_area::insert_cw_item(int& err, mx_cw_t* item,
    mx_text_area_pos* p)
{
    mx_paragraph* para;

    cc_unserialise_attributes(err);
    MX_ERROR_CHECK(err);

    set_mem_state(mx_in_mem_and_modified_e);

    para = (mx_paragraph*)paragraphs.get(err, p->paragraph_index);
    MX_ERROR_CHECK(err);

    para->insert_cw_item(err, item, p->para_pos);
    MX_ERROR_CHECK(err);

abort:;
}

void mx_text_area::insert_line_break(int& err, mx_text_area_pos* p)
{
    mx_paragraph* para;

    cc_unserialise_attributes(err);
    MX_ERROR_CHECK(err);

    set_mem_state(mx_in_mem_and_modified_e);

    para = (mx_paragraph*)paragraphs.get(err, p->paragraph_index);
    MX_ERROR_CHECK(err);

    para->insert_line_break(err, p->para_pos);
    MX_ERROR_CHECK(err);

abort:;
}

bool mx_text_area::can_join(int& err, mx_area* a)
{
    return a->is_a(mx_text_area_class_e);
}

void mx_text_area::join(int& err, mx_area* a)
{
    mx_text_area* another = (mx_text_area*)a;

    MX_ERROR_ASSERT(err, a->is_a(mx_text_area_class_e));

    cc_unserialise_attributes(err);
    MX_ERROR_CHECK(err);

    another->cc_unserialise_attributes(err);
    MX_ERROR_CHECK(err);

    set_mem_state(mx_in_mem_and_modified_e);
    another->set_mem_state(mx_in_mem_and_modified_e);

    paragraphs.move_items_in_end(err,
        another->paragraphs.get_num_items(),
        another->paragraphs);
    MX_ERROR_CHECK(err);

    delete another;
abort:;
}

void mx_text_area::insert_para_break(int& err, mx_text_area_pos* p)
{
    mx_paragraph *para, *new_para;
    mx_paragraph_style style, *new_style;
    mx_document* doc;

    bool break_before_current;

    cc_unserialise_attributes(err);
    MX_ERROR_CHECK(err);

    set_mem_state(mx_in_mem_and_modified_e);

    doc = get_document(err);
    MX_ERROR_CHECK(err);

    para = (mx_paragraph*)paragraphs.get(err, p->paragraph_index);
    MX_ERROR_CHECK(err);

    style = *(para->get_paragraph_style());
    break_before_current = style.page_break_before;

    if (p->para_pos.is_start()) {
        new_para = new mx_paragraph;
        new_para->set_paragraph_style(para->get_base_paragraph_style());
        new_para->set_document(doc);

        paragraphs.insert(err, p->paragraph_index, new_para);
        MX_ERROR_CHECK(err);

        style.page_break_before = FALSE;
        para->modify_paragraph_style(style);

        style.page_break_before = break_before_current;
        new_para->modify_paragraph_style(style);
    } else {
        new_para = para->split(err, p->para_pos);
        MX_ERROR_CHECK(err);

        style.page_break_before = FALSE;
        new_para->modify_paragraph_style(style);

        paragraphs.insert(err, p->paragraph_index + 1, new_para);
        MX_ERROR_CHECK(err);
    }

    if (style.get_next_style_name() != NULL) {
        new_style = doc->get_named_style(err, style.get_next_style_name());
        MX_ERROR_CHECK(err);

        new_para->set_paragraph_style(new_style);
    }

    new_para->set_width(get_width(err));
    MX_ERROR_CHECK(err);
abort:;
}

mx_area* mx_text_area::duplicate()
{
    mx_text_area* res;

    res = new mx_text_area(*this);

    return (mx_area*)res;
}

mx_paragraph* mx_text_area::remove(int& err, mx_paragraph* p)
{
    int i;

    cc_unserialise_attributes(err);
    MX_ERROR_CHECK(err);

    set_mem_state(mx_in_mem_and_modified_e);

    i = paragraphs.find_index(err, p);
    MX_ERROR_CHECK(err);

    (void)remove(err, i);
    MX_ERROR_CHECK(err);

abort:
    return p;
}

int mx_text_area::get_index(int& err, mx_paragraph* p)
{
    int i;

    cc_unserialise_attributes(err);
    MX_ERROR_CHECK(err);

    i = paragraphs.find_index(err, p);
    MX_ERROR_CHECK(err);

    return i;

abort:
    return -1;
}

void mx_text_area::rejoin_from_position(int& err, int& para_index)
{
    mx_paragraph *para, *next;

    cc_unserialise_attributes(err);
    MX_ERROR_CHECK(err);

    set_mem_state(mx_in_mem_and_modified_e);

    para = (mx_paragraph*)paragraphs.get(err, para_index);
    MX_ERROR_CHECK(err);

    para_index++;

    while (para_index < (paragraphs.get_num_items())) {
        next = (mx_paragraph*)paragraphs.get(err, para_index);
        MX_ERROR_CHECK(err);

        if (next->get_type() == mx_paragraph::mx_paragraph_end_e || next->get_type() == mx_paragraph::mx_paragraph_middle_e) {
            para->join(err, next);
            MX_ERROR_CHECK(err);

            (void)paragraphs.remove(err, para_index);
            MX_ERROR_CHECK(err);
        } else {
            return;
        }
    }

abort:;
}

void mx_text_area::rejoin_logical_paras(int& err)
{
    int i = 0;
    mx_paragraph* para;

    cc_unserialise_attributes(err);
    MX_ERROR_CHECK(err);

    set_mem_state(mx_in_mem_and_modified_e);

    while (i < (paragraphs.get_num_items() - 1)) {
        para = (mx_paragraph*)paragraphs.get(err, i);
        MX_ERROR_CHECK(err);

        switch (para->get_type()) {
        case mx_paragraph::mx_paragraph_start_e:
        case mx_paragraph::mx_paragraph_middle_e:
            rejoin_from_position(err, i);
            MX_ERROR_CHECK(err);
            break;

        default:
        case mx_paragraph::mx_paragraph_end_e:
        case mx_paragraph::mx_paragraph_whole_e:
            i++;
            break;
        }
    }
abort:;
}

mx_paragraph* mx_text_area::swap(int& err, int index, mx_paragraph* p)
{
    mx_paragraph* res;
    mx_document* doc;

    doc = get_document(err);
    MX_ERROR_CHECK(err);

    cc_unserialise_attributes(err);
    MX_ERROR_CHECK(err);

    set_mem_state(mx_in_mem_and_modified_e);

    res = (mx_paragraph*)paragraphs.remove(err, index);
    MX_ERROR_CHECK(err);

    paragraphs.insert(err, index, p);
    MX_ERROR_CHECK(err);

    p->set_width(get_width(err));
    MX_ERROR_CHECK(err);

    p->set_document(doc);

    return res;

abort:
    return NULL;
}

void mx_text_area::set_mem_state(mx_db_mem_state_t s)
{
    mx_db_object::set_mem_state(s);

    mx_list_iterator it(paragraphs);

    while (it.more()) {
        ((mx_paragraph*)it.data())->set_mem_state(s);
    }
}

void mx_text_area::print_debug()
{
}

mx_db_mem_state_t mx_text_area::get_mem_state()
{
    if (mx_db_object::get_mem_state() == mx_in_mem_and_modified_e || mx_db_object::get_mem_state() == mx_on_disk_e) {
        return mx_db_object::get_mem_state();
    }

    mx_list_iterator it(paragraphs);

    while (it.more()) {
        mx_paragraph* para = (mx_paragraph*)it.data();

        if (para->get_mem_state() == mx_in_mem_and_modified_e) {
            mx_db_object::set_mem_state(mx_in_mem_and_modified_e);
            return mx_in_mem_and_modified_e;
        }
    }
    return mx_in_mem_e;
}

bool mx_text_area::has_page_break_before(int& err)
{
    mx_paragraph* para = this->get(err, 0);
    MX_ERROR_CHECK(err);

    return para->get_paragraph_style()->page_break_before;
abort:
    return FALSE;
}
