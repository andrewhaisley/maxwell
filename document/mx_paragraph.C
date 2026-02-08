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
 * MODULE : mx_paragraph.C
 *
 * AUTHOR : Andrew Haisley
 *
 * This file
 *
 * DESCRIPTION:
 *
 *
 *
 */

#include <mx.h>
#include <mx_db.h>

#include "mx_paragraph.h"
#include <mx_break_w.h>
#include <mx_complex_w.h>
#include <mx_cw_t.h>
#include <mx_doc.h>
#include <mx_field_w.h>
#include <mx_list.h>
#include <mx_list_iter.h>
#include <mx_nlist.h>
#include <mx_nlist_iter.h>
#include <mx_para_pos.h>
#include <mx_simple_w.h>
#include <mx_space_w.h>
#include <mx_word.h>

#define MX_SCALE 10000.0

static void set_word_operations(mx_para_pos& p)
{
    if (p.is_start()) {
        mx_word::set_operations_to_right_of_mods();
    } else {
        mx_word::set_operations_to_left_of_mods();
    }
}

static mx_char_style* temp_style = NULL;
static mx_char_style_mod* temp_style_mod = NULL;
static mx_paragraph_style* style_to_use = NULL;
static mx_paragraph_style* last_style = (mx_paragraph_style*)-1;
static mx_paragraph_style_mod* last_style_mod = (mx_paragraph_style_mod*)-1;
static bool style_mod_changed = TRUE;

mx_paragraph_style& mx_paragraph::get_style_to_use()
{
    if (!style_mod_changed && last_style_mod == style_mod && last_style == style) {
        // don't need to recalculate the style_to_use
        return *style_to_use;
    }

    if (style == NULL) {
        // get a default if there is no style
        *style_to_use = mx_paragraph_style();
    } else {
        *style_to_use = *style;
    }

    if (style_mod != NULL) {
        *style_to_use += *style_mod;
    }

    last_style_mod = style_mod;
    last_style = style;
    style_mod_changed = FALSE;
    return *style_to_use;
}

mx_paragraph::mx_paragraph()
{
    width = height = 0.0;
    mx_char_style* s;
    mx_break_word* bw;

    doc = NULL;
    style = NULL;
    style_mod = NULL;

    if (style_to_use == NULL) {
        style_to_use = new mx_paragraph_style;
        temp_style = new mx_char_style;
        temp_style_mod = new mx_char_style_mod;
    }

    s = new mx_char_style(*get_style_to_use().get_char_style());

    reformat_upper = INT_MAX;
    reformat_lower = -1;

    type = mx_paragraph_whole_e;
    bw = new mx_break_word(mx_paragraph_break_e);

    styles.append(s);
    words.append_item(bw);

    set_mem_state(mx_in_mem_and_modified_e);
}

mx_paragraph::mx_paragraph(mx_paragraph& para)
{
    mx_list_iterator row_iter(para.words.list);
    mx_word* w;

    int err = MX_ERROR_OK;

    doc = para.doc;
    style = para.style;
    style_mod = para.style_mod;
    if (style_mod != NULL) {
        style_mod = new mx_paragraph_style_mod(*style_mod);
        style_mod_changed = TRUE;
    }

    reformat_upper = para.reformat_upper;
    reformat_lower = para.reformat_lower;
    format_width = para.format_width;
    height = para.height;
    width = para.width;

    type = para.type;
    set_mem_state(mx_in_mem_and_modified_e);

    while (row_iter.more()) {
        mx_list* row = (mx_list*)row_iter.data();
        mx_list* new_row = words.append();

        mx_list_iterator word_iter(*row);

        while (word_iter.more()) {
            w = (mx_word*)word_iter.data();
            w = w->duplicate();
            new_row->append(w);
        }
    }

    MX_ERROR_ASSERT(err, doc != NULL);
    MX_ERROR_ASSERT(err, doc->get_named_style(err, style->get_name()) == style);
    MX_ERROR_CHECK(err);

    reformat_styles(err);
    MX_ERROR_CHECK(err);

    return;
abort:
    global_error_trace->print();
    MX_ERROR_CLEAR(err);
}

mx_paragraph::~mx_paragraph()
{
    delete_words();
    delete_styles();
    if (style_mod != NULL) {
        delete style_mod;
    }
}

void mx_paragraph::delete_words()
{
    mx_nlist_iterator iter(words);
    while (iter.more())
        delete (mx_word*)iter.data();
    words = mx_nlist();
}

void mx_paragraph::delete_styles()
{
    mx_list_iterator iter(styles);
    while (iter.more())
        delete (mx_char_style*)iter.data();
    styles = mx_list();
}

void mx_paragraph::set_paragraph_style(mx_paragraph_style* s)
{
    int err = MX_ERROR_OK;

    reformat_styles(err);
    MX_ERROR_CHECK(err);

    fix_styles_for_base_style_change(err, *s->get_char_style());
    MX_ERROR_CHECK(err);

    style = s;

    recalculate_format_width();
    return;
abort:
    global_error_trace->print();
    MX_ERROR_CLEAR(err);
}

void mx_paragraph::modify_paragraph_style(const mx_paragraph_style& s)
{
    mx_paragraph_style& stu = get_style_to_use();
    mx_paragraph_style_mod psm;

    if (style_mod != NULL) {
        psm = *style_mod;
    }

    psm += mx_paragraph_style_mod(stu, s);
    *psm.get_char_style_mod() = mx_char_style_mod();

    if (psm.is_null()) {
        if (style_mod != NULL) {
            delete style_mod;
            style_mod = NULL;
        }
    } else {
        if (style_mod == NULL) {
            style_mod = new mx_paragraph_style_mod(psm);
        } else {
            *style_mod = psm;
        }
    }
    style_mod_changed = TRUE;

    recalculate_format_width();
}

void mx_paragraph::modify_paragraph_style(const mx_paragraph_style_mod& mod)
{
    mx_paragraph_style ps = *this->get_paragraph_style();
    ps += mod;
    this->modify_paragraph_style(ps);
}

mx_paragraph_style* mx_paragraph::get_paragraph_style()
{
    return &get_style_to_use();
}

mx_paragraph_style* mx_paragraph::get_base_paragraph_style()
{
    return style;
}

float mx_paragraph::get_height()
{
    return height;
};

bool mx_paragraph::is_empty(int& err)
{
    mx_break_word* bw = (mx_break_word*)get_word(err, 0, 0);
    MX_ERROR_CHECK(err);

    if (bw->is_a(mx_break_word_class_e)) {
        return bw->type() == mx_paragraph_break_e;
    }
abort:
    return FALSE;
}

/* This shouldn't be needed

void mx_paragraph::set_style_mod_revert_flags(int &err)
{
    mx_char_style *para_cs = named_style.get_char_style();
    mx_nlist_iterator iter(words);

    while (iter.more())
    {
        mx_word *w = (mx_word *)iter.data();

        w->set_style_mod_revert_flags(err, *para_cs);
        MX_ERROR_CHECK(err);
    }
abort:;
}
*/

void mx_paragraph::minimise_style_mods(int& err)
{
    mx_nlist_iterator i(words);
    while (i.more())
        ((mx_word*)i.data())->minimise_mods();
}

void mx_paragraph::delete_word(int& err, int row_index, int item_index)
{
    mx_word* w = remove_word(err, row_index, item_index);
    MX_ERROR_CHECK(err);

    delete w;

abort:;
}

mx_word* mx_paragraph::remove_word(int& err, int row_index, int item_index)
{
    mx_char_style *word_style, *next_word_style;
    mx_word *w, *next_w;
    int num_words;

    w = (mx_word*)words.delete_item(err, row_index, item_index);
    MX_ERROR_CHECK(err);

    num_words = this->num_words(err, row_index);
    MX_ERROR_CHECK(err);

    if (num_words == 0) {
        mx_list* l = words.delete_row(err, row_index);
        MX_ERROR_CHECK(err);
        delete l;
    }

    // get the item after the deleted one
    words.next(err, 0, row_index, item_index);
    if (err != MX_ERROR_OK) {
        // no next item so exit
        MX_ERROR_CLEAR(err);
        return w;
    }

    next_w = (mx_word*)words.item(err, row_index, item_index);
    MX_ERROR_CHECK(err);

    word_style = w->get_style();
    next_word_style = next_w->get_style();
    next_w->set_style(word_style);

    if (word_style != next_word_style) {
        mx_char_style_mod m(*word_style, *next_word_style);
        mx_para_pos pp(row_index, item_index, 0);

        mx_word::set_operations_to_left_of_mods();
        aka_insert_mod(err, m, pp);
        MX_ERROR_CHECK(err);
    }
    return w;
abort:
    return NULL;
}

void mx_paragraph::insert_word(int& err, mx_word* w, mx_para_pos& p)
{
    set_word_operations(p);
    aka_insert_word(err, w, p);
}

void mx_paragraph::aka_insert_word(int& err, mx_word* w, mx_para_pos& p)
{
    mx_char_style s;
    mx_word* right;

    split_word(err, p);
    MX_ERROR_CHECK(err);

    right = get_word(err, p);
    MX_ERROR_CHECK(err);

    w->set_style(right->get_style());

    words.insert(err, w, p.line_index, p.word_index);
    MX_ERROR_CHECK(err);

    join_words(err, p);
    MX_ERROR_CHECK(err);

abort:;
}

uint32 mx_paragraph::get_serialised_size(int& err)
{
    uint32 res;
    mx_word* word;

    mx_nlist_iterator iter(words);

    // serialise the paragraph style and each word in the list
    res = SLS_STRING(style->get_name()) + SLS_UINT32 + SLS_ENUM;
    MX_ERROR_CHECK(err);

    if (style_mod == NULL) {
        mx_paragraph_style_mod psm;
        res += psm.get_serialised_size(err);
    } else {
        res += style_mod->get_serialised_size(err);
    }
    MX_ERROR_CHECK(err);

    while (iter.more()) {
        word = (mx_word*)iter.data();
        res += word->get_serialised_size(err) + SLS_ENUM;
    }

    return res;
abort:
    return 0;
}

void mx_paragraph::serialise(int& err, uint8** buffer)
{
    mx_word* word;
    mx_nlist_iterator iter(words);

    // serialise the paragraph style name only
    serialise_string(style->get_name(), buffer);

    if (style_mod == NULL) {
        mx_paragraph_style_mod psm;
        psm.serialise(err, buffer);
    } else {
        style_mod->serialise(err, buffer);
    }
    MX_ERROR_CHECK(err);

    serialise_enum(type, buffer);
    serialise_uint32(total_num_words(), buffer);

    while (iter.more()) {
        word = (mx_word*)iter.data();

        if (word->rtti_class() == mx_complex_word_class_e) {
            if (((mx_complex_word*)word)->is_simple()) {
                mx_simple_word sw(word->cstring());

                serialise_enum(sw.rtti_class(), buffer);

                sw.serialise(err, buffer);
                MX_ERROR_CHECK(err);

                continue;
            }
        }

        serialise_enum(word->rtti_class(), buffer);

        word->serialise(err, buffer);
        MX_ERROR_CHECK(err);
    }

    return;
abort:
    return;
}

void mx_paragraph::unserialise(int& err, uint8** buffer)
{
    mx_word* word;
    uint32 num_words, i;
    uint16 type_num;
    mx_paragraph_style_mod temp_para_mod;
    mx_char_style* base_char_style;

    char temp[MX_PARA_STYLE_MAX_NAME];

    delete_words();

    // get the style name
    unserialise_string(temp, buffer);

    MX_ERROR_ASSERT(err, doc != NULL);

    style = doc->get_named_style(err, temp);
    MX_ERROR_CHECK(err);
    base_char_style = style->get_char_style();

    temp_para_mod.unserialise(err, buffer);
    MX_ERROR_CHECK(err);

    if (style_mod != NULL) {
        delete style_mod;
        style_mod = NULL;
    }

    if (!temp_para_mod.is_null()) {
        style_mod = new mx_paragraph_style_mod(temp_para_mod);
    }
    style_mod_changed = TRUE;

    unserialise_enum(type_num, buffer);
    type = (mx_paragraph_type_t)type_num;
    unserialise_uint32(num_words, buffer);

    for (i = 0; i < num_words; i++) {
        mx_rtti_class_e word_class;
        uint16 e;

        unserialise_enum(e, buffer);
        word_class = (mx_rtti_class_e)e;

        switch (word_class) {
        case mx_simple_word_class_e:
            word = new mx_simple_word();
            break;
        case mx_space_word_class_e:
            word = new mx_space_word();
            break;
        case mx_break_word_class_e:
            word = new mx_break_word();
            break;
        case mx_complex_word_class_e:
            word = new mx_complex_word();
            break;
        case mx_field_word_class_e:
            word = new mx_field_word();
            break;
        default:
            MX_ERROR_THROW(err, MX_PARAGRAPH_BAD_WORD_TYPE);
        }

        word->unserialise(err, buffer, doc->get_docid());
        MX_ERROR_CHECK(err);

        word->set_style_mod_revert_flags(err, *base_char_style);
        MX_ERROR_CHECK(err);

        words.append_item(word);
    }
abort:;
}

void mx_paragraph::delete_paragraph_break(int& err)
{
    int n_rows, n_items;
    mx_break_word* bw;

    n_rows = words.get_num_rows();
    if (n_rows != 0) {
        n_items = words.get_num_items(err, n_rows - 1);
        MX_ERROR_CHECK(err);

        bw = (mx_break_word*)words.item(err, n_rows - 1, n_items - 1);
        MX_ERROR_CHECK(err);

        if (bw->rtti_class() == mx_break_word_class_e) {
            if (((mx_break_word*)bw)->type() == mx_paragraph_break_e) {
                bw = (mx_break_word*)words.delete_item(err, n_rows - 1, n_items - 1);
                MX_ERROR_CHECK(err);

                delete bw;
            }
        }
    }
abort:;
}

void mx_paragraph::append_word(int& err, mx_word* word)
{
    mx_para_pos pp;

    pp.moveto_end(err, this);
    MX_ERROR_CHECK(err);

    mx_word::set_operations_to_right_of_mods();

    aka_insert_word(err, word, pp);
    MX_ERROR_CHECK(err);

abort:;
}

uint32 mx_paragraph::total_num_words()
{
    return words.get_num_items();
}

mx_word* mx_paragraph::get_word(int& err, int line_index, int word_index)
{
    return (mx_word*)words.item(err, line_index, word_index);
}

int mx_paragraph::num_lines()
{
    return words.list.get_num_items();
}

int mx_paragraph::num_words(int& err, int line_index)
{
    mx_list* row = words.row(err, line_index);
    MX_ERROR_CHECK(err);

    return row->get_num_items();
abort:
    return -1;
}

void mx_paragraph::set_width(float f)
{
    set_mem_state(mx_in_mem_and_modified_e);
    width = f;
}

float mx_paragraph::get_format_width()
{
    return format_width;
};

mx_paragraph::mx_paragraph_type_t mx_paragraph::get_type()
{
    return type;
}

void mx_paragraph::set_type(mx_paragraph::mx_paragraph_type_t t)
{
    set_mem_state(mx_in_mem_and_modified_e);
    type = t;
}

void mx_paragraph::set_document(mx_document* d)
{
    set_mem_state(mx_in_mem_and_modified_e);
    doc = d;
}

mx_char_style* mx_paragraph::get_style(int& err, mx_para_pos& pp)
{
    set_word_operations(pp);
    return aka_get_style(err, pp);
}

mx_char_style* mx_paragraph::aka_get_style(int& err, mx_para_pos& pp)
{
    mx_word* w = get_word(err, pp.line_index, pp.word_index);
    MX_ERROR_CHECK(err);

    w->get_index_style(err, pp.letter_index, *temp_style);
    MX_ERROR_CHECK(err);

    return temp_style;
abort:
    return NULL;
}

mx_paragraph* mx_paragraph::split(int& err, mx_para_pos& p)
{
    mx_paragraph* new_para = new mx_paragraph();
    mx_break_word* bw;
    int n;
    mx_char_style_mod *csm, mod_to_insert;

    set_mem_state(mx_in_mem_and_modified_e);
    new_para->doc = this->doc;

    this->reformat_styles(err);
    MX_ERROR_CHECK(err);

    mx_word::set_operations_to_right_of_mods();
    csm = aka_get_style_mod(err, p);
    MX_ERROR_CHECK(err);
    mod_to_insert = *csm;

    new_para->set_paragraph_style(style);
    new_para->style_mod = style_mod;
    if (new_para->style_mod != NULL) {
        new_para->style_mod = new mx_paragraph_style_mod(*new_para->style_mod);
        style_mod_changed = TRUE;
    }

    switch (this->type) {
    case mx_paragraph_start_e:
        this->type = mx_paragraph_whole_e;
        new_para->type = mx_paragraph_start_e;
        break;
    case mx_paragraph_middle_e:
        this->type = mx_paragraph_end_e;
        new_para->type = mx_paragraph_start_e;
        break;
    case mx_paragraph_whole_e:
    case mx_paragraph_end_e:
        new_para->type = mx_paragraph_whole_e;
        break;
    default:
        MX_ERROR_THROW(err, MX_ERROR_UNDEFINED);
    }

    mx_word::set_operations_to_right_of_mods();
    split_word(err, p);
    MX_ERROR_CHECK(err);

    // is the word the first on the line?
    if (p.word_index > 0) {
        // no - break the line up first

        n = words.get_num_items(err, p.line_index);
        MX_ERROR_CHECK(err);

        words.move_items_out_end(err, p.line_index, n - p.word_index);
        MX_ERROR_CHECK(err);

        p.line_index++;
    }

    // get rid of the last break word
    n = words.get_num_items(err, num_lines() - 1);
    MX_ERROR_CHECK(err);

    bw = (mx_break_word*)words.delete_item(err, num_lines() - 1, n - 1);
    MX_ERROR_CHECK(err);

    words.move_rows_out_end(err, num_lines() - p.line_index, new_para->words);
    MX_ERROR_CHECK(err);

    // put break word at the end of this paragraph
    words.append_item(bw);

    new_para->delete_blank_lines(err, 0);
    MX_ERROR_CHECK(err);

    this->reformat_styles(err);
    MX_ERROR_CHECK(err);

    {
        mx_para_pos start_pos;
        start_pos.moveto_start();

        mx_word::set_operations_to_left_of_mods();
        new_para->aka_insert_mod(err, mod_to_insert, start_pos);
        MX_ERROR_CHECK(err);
    }

    new_para->reformat_styles(err);
    MX_ERROR_CHECK(err);

    p.moveto_end(err, this);
    MX_ERROR_CHECK(err);

    return new_para;

abort:
    return NULL;
}

void mx_paragraph::reposition_lines(int& err)
{
    int i;
    bool b;

    float f = 0.0;
    set_mem_state(mx_in_mem_and_modified_e);

    for (i = 0; i < words.get_num_rows(); i++) {
        f += height_for_line(err, i);
        MX_ERROR_CHECK(err);

        set_line_ypos(err, i, f, b);
        MX_ERROR_CHECK(err);
    }
abort:;
}

mx_paragraph* mx_paragraph::split(int& err, int line_index)
{
    mx_paragraph* new_para;
    mx_break_word* bw;
    int n;
    mx_word* w;
    mx_list* l;
    mx_char_style_mod *csm, mod_to_insert;
    mx_para_pos pp(line_index, 0, 0);
    float x, y;

    set_mem_state(mx_in_mem_and_modified_e);
    new_para = new mx_paragraph();
    new_para->doc = this->doc;

    new_para->style_mod = style_mod;
    if (new_para->style_mod != NULL) {
        new_para->style_mod = new mx_paragraph_style_mod(*new_para->style_mod);
        style_mod_changed = TRUE;
    }
    new_para->set_paragraph_style(style);

    csm = this->get_style_mod(err, pp);
    MX_ERROR_CHECK(err);
    mod_to_insert = *csm;

    switch (type) {
    case mx_paragraph_start_e:
        new_para->type = mx_paragraph_middle_e;
        break;
    case mx_paragraph_middle_e:
        new_para->type = mx_paragraph_middle_e;
        break;
    case mx_paragraph_end_e:
        type = mx_paragraph_middle_e;
        new_para->type = mx_paragraph_end_e;
        break;
    case mx_paragraph_whole_e:
        type = mx_paragraph_start_e;
        new_para->type = mx_paragraph_end_e;
    }

    // get rid of the break word in the new paragraph
    bw = (mx_break_word*)new_para->words.delete_item(err, 0, 0);
    MX_ERROR_CHECK(err);

    l = new_para->words.delete_row(err, 0);
    MX_ERROR_CHECK(err);

    delete l;

    // move rows out of this paragraph into the new one
    words.move_rows_out_end(err, num_lines() - line_index, new_para->words);
    MX_ERROR_CHECK(err);

    // put break word at the end of this paragraph
    n = words.get_num_items(err, num_lines() - 1);
    MX_ERROR_CHECK(err);

    w = (mx_word*)words.item(err, num_lines() - 1, n - 1);
    MX_ERROR_CHECK(err);

    bw->x = w->x + w->width();
    bw->y = w->y;

    words.append_item(bw);

    this->reformat_styles(err);
    MX_ERROR_CHECK(err);

    pp.moveto_start();

    w = (mx_word*)(new_para->words.item(err, 0, 0));
    MX_ERROR_CHECK(err);

    x = w->x;
    y = w->y;

    new_para->insert_mod(err, mod_to_insert, pp);
    MX_ERROR_CHECK(err);

    w = (mx_word*)(new_para->words.item(err, 0, 0));
    MX_ERROR_CHECK(err);

    w->x = x;
    w->y = y;

    new_para->reformat_styles(err);
    MX_ERROR_CHECK(err);

    // reposition the lines
    new_para->reposition_lines(err);
    MX_ERROR_CHECK(err);

    this->recalculate_height(err);
    MX_ERROR_CHECK(err);

    new_para->recalculate_height(err);
    MX_ERROR_CHECK(err);

    return new_para;

abort:
    return NULL;
}

void mx_paragraph::insert_mod(int& err, mx_char_style_mod& m, mx_para_pos& p)
{
    set_word_operations(p);
    aka_insert_mod(err, m, p);
}

void mx_paragraph::aka_insert_mod(int& err, mx_char_style_mod& m, mx_para_pos& pos)
{
    mx_paragraph_style& stu = get_style_to_use();
    mx_para_pos p = pos;
    mx_word* w;

    set_mem_state(mx_in_mem_and_modified_e);
    if (m.is_null()) {
        return;
    }

    m.set_revert_to_default_flags(*stu.get_char_style());

    // get the word at the insert position
    w = (mx_word*)words.item(err, p.line_index, p.word_index);
    MX_ERROR_CHECK(err);

    w->insert(err, m, p.letter_index);
    if (err != MX_WORD_OP_NOT_AVAILABLE_ON_SUBCLASS) {
        MX_ERROR_CHECK(err);
    } else {
        mx_complex_word* new_word = new mx_complex_word;

        MX_ERROR_CLEAR(err);
        *new_word += m;

        aka_insert_word(err, new_word, p);
        MX_ERROR_CHECK(err);
    }
abort:;
}

mx_para_pos mx_paragraph::join(int& err, mx_paragraph* para)
{
    mx_para_pos join_pos, end_pos, start_pos;
    mx_word* w;
    int i = 0;
    int num_words_on_line = 0, num_rows = 0;
    mx_char_style *temp_cs, cs;
    mx_char_style_mod mod, *temp_mod;

    reformat_styles(err);
    MX_ERROR_CHECK(err);

    para->set_paragraph_style(this->get_base_paragraph_style());

    start_pos.moveto_start();
    end_pos.moveto_end(err, this);
    MX_ERROR_CHECK(err);

    // get the style mod from the style at the end of this paragraph back to
    // the base style, and then concatenate the style mod from the base style
    // of the other paragraph to the style at the start of the other paragraph

    mx_word::set_operations_to_left_of_mods();
    temp_cs = this->aka_get_style(err, end_pos);
    MX_ERROR_CHECK(err);
    cs = *temp_cs;
    temp_cs = this->get_base_paragraph_style()->get_char_style();
    mod = mx_char_style_mod(cs, *temp_cs);

    temp_mod = para->get_style_mod(err, start_pos);
    MX_ERROR_CHECK(err);
    mod += *temp_mod;

    set_mem_state(mx_in_mem_and_modified_e);
    delete_paragraph_break(err);
    MX_ERROR_CHECK(err);

    if (type == mx_paragraph_start_e) {
        if (para->type == mx_paragraph_end_e) {
            type = mx_paragraph_whole_e;
        }
    } else {
        if (type == mx_paragraph_middle_e) {
            if (para->type == mx_paragraph_end_e) {
                type = mx_paragraph_end_e;
            }
        }
    }

    num_rows = words.get_num_rows();

    if (num_rows > 0) {
        num_words_on_line = words.get_num_items(err, num_rows - 1);
        MX_ERROR_CHECK(err);

        if (num_words_on_line > 0) {
            join_pos.line_index = num_rows - 1;
            join_pos.word_index = num_words_on_line - 1;

            w = this->get_word(err, join_pos);
            MX_ERROR_CHECK(err);
            join_pos.letter_index = w->get_num_visible_items();
        }
    }

    words.append_remove(para->words);
    delete para;

    if (num_rows > 0 && num_words_on_line > 0 && !mod.is_null()) {
        // sort all the styles out before we insert a mod
        reformat_styles(err);
        MX_ERROR_CHECK(err);

        this->join_words(err, join_pos);
        MX_ERROR_CHECK(err);

        this->insert_mod(err, mod, join_pos);
        MX_ERROR_CHECK(err);
    }

    delete_blank_lines(err, i);
    MX_ERROR_CHECK(err);

    reformat_styles(err);
    MX_ERROR_CHECK(err);
abort:
    return join_pos;
}

bool mx_paragraph::join_words(int& err, mx_para_pos& p)
{
    mx_para_pos p1;
    mx_word* w;
    bool right_join = FALSE;
    bool left_join = FALSE;
    int n;

    set_mem_state(mx_in_mem_and_modified_e);
    delete_blank_lines(err, p.line_index);
    MX_ERROR_CHECK(err);

    p1 = p;
    p1.left_word(err, this);
    if (err == MX_POSITION_RANGE_ERROR) {
        MX_ERROR_CLEAR(err);
    } else {
        MX_ERROR_CHECK(err);

        w = get_word(err, p1);
        MX_ERROR_CHECK(err);

        n = w->get_num_visible_items();

        left_join = join_words(err, p1, p);
        MX_ERROR_CHECK(err);

        if (left_join) {
            p1.letter_index = n + p.letter_index;
            p = p1;
        }
    }

    p1 = p;
    p1.right_word(err, this);
    if (err == MX_POSITION_RANGE_ERROR) {
        MX_ERROR_CLEAR(err);
    } else {
        MX_ERROR_CHECK(err);

        right_join = join_words(err, p, p1);
        MX_ERROR_CHECK(err);
    }

    return left_join || right_join;

abort:
    return FALSE;
}

mx_word* mx_paragraph::get_word(int& err, const mx_para_pos& pp)
{
    return get_word(err, pp.line_index, pp.word_index);
}

bool mx_paragraph::join_words(int& err, const mx_para_pos& p1,
    const mx_para_pos& p2)
{
    bool b;
    mx_word *one, *two;

    one = get_word(err, p1);
    MX_ERROR_CHECK(err);

    two = get_word(err, p2);
    MX_ERROR_CHECK(err);

    if (one->rtti_class() == mx_space_word_class_e) {
        if (two->rtti_class() == mx_space_word_class_e) {
            // two space words -> yes
            mx_space_word* sw1 = (mx_space_word*)one;
            mx_space_word* sw2 = (mx_space_word*)two;

            *sw1 += *sw2;

            delete_word(err, p2);
            MX_ERROR_CHECK(err);

            return TRUE;
        }
    } else if (one->rtti_class() == mx_complex_word_class_e) {
        mx_complex_word* cw = (mx_complex_word*)one;

        if (two->rtti_class() == mx_simple_word_class_e) {
            mx_simple_word* sw = (mx_simple_word*)two;
            *cw += *sw;

            delete_word(err, p2);
            MX_ERROR_CHECK(err);

            return TRUE;
        } else if (two->rtti_class() == mx_complex_word_class_e) {
            mx_para_pos pos = p2;
            mx_complex_word* cw2 = (mx_complex_word*)remove_word(err, pos);
            MX_ERROR_ASSERT(err, cw2->is_a(mx_complex_word_class_e));

            cw->join(err, cw2);
            MX_ERROR_CHECK(err);

            return TRUE;
        }
    } else if (one->rtti_class() == mx_simple_word_class_e) {
        mx_simple_word* sw = (mx_simple_word*)one;

        if (two->rtti_class() == mx_simple_word_class_e) {
            mx_simple_word* sw2 = (mx_simple_word*)two;
            *sw += *sw2;

            delete_word(err, p2);
            MX_ERROR_CHECK(err);

            return TRUE;
        } else if (two->rtti_class() == mx_complex_word_class_e) {
            mx_complex_word* cw = (mx_complex_word*)two;

            // swap the words over and prepend the simple word to the
            // complex word

            words.overwrite(err, cw, p1.line_index, p1.word_index);
            MX_ERROR_CHECK(err);

            words.overwrite(err, sw, p2.line_index, p2.word_index);
            MX_ERROR_CHECK(err);

            cw->move_last_in_start(err, sw->get_num_visible_items(), sw);
            MX_ERROR_CHECK(err);

            delete_word(err, p2);
            MX_ERROR_CHECK(err);

            return TRUE;
        }
    }

    b = fix_trailing_style_mods(err, p1, p2);
    MX_ERROR_CHECK(err);

    return b;
abort:
    return FALSE;
}

bool mx_paragraph::fix_trailing_style_mods(int& err, const mx_para_pos& p1,
    const mx_para_pos& p2)
{
    mx_word *one, *two;

    set_mem_state(mx_in_mem_and_modified_e);
    one = get_word(err, p1);
    MX_ERROR_CHECK(err);

    two = get_word(err, p2);
    MX_ERROR_CHECK(err);

    one->move_style_mod_out_end(err, two);
    if (err != MX_WORD_OP_NOT_AVAILABLE_ON_SUBCLASS) {
        MX_ERROR_CHECK(err);
    } else {
        MX_ERROR_CLEAR(err);
        if (two->is_a(mx_simple_word_class_e)) {
            mx_complex_word* cw = new mx_complex_word(two->cstring());
            cw->set_style(two->get_style());

            words.overwrite(err, cw, p2.line_index, p2.word_index);
            MX_ERROR_CHECK(err);
            delete two;

            one->move_style_mod_out_end(err, cw);
            MX_ERROR_CHECK(err);
        }
    }

    if (one->is_empty()) {
        delete_word(err, p1);
        MX_ERROR_CHECK(err);
        return TRUE;
    }

abort:
    return FALSE;
}

void mx_paragraph::delete_word(int& err, const mx_para_pos& p)
{
    set_mem_state(mx_in_mem_and_modified_e);
    delete_word(err, p.line_index, p.word_index);
}

mx_word* mx_paragraph::remove_word(int& err, mx_para_pos& p)
{
    set_mem_state(mx_in_mem_and_modified_e);
    return remove_word(err, p.line_index, p.word_index);
}

mx_char_style_mod* mx_paragraph::get_style_mod(int& err, mx_para_pos& pp)
{
    set_word_operations(pp);
    return aka_get_style_mod(err, pp);
}

mx_char_style_mod* mx_paragraph::aka_get_style_mod(int& err, mx_para_pos& pp)
{
    mx_word* w;
    mx_char_style temp_cs;

    w = get_word(err, pp.line_index, pp.word_index);
    MX_ERROR_CHECK(err);

    w->get_index_style(err, pp.letter_index, temp_cs);
    MX_ERROR_CHECK(err);

    *temp_style_mod = mx_char_style_mod(
        *(get_style_to_use().get_char_style()),
        temp_cs);

    return temp_style_mod;
abort:
    return NULL;
}

float mx_paragraph::line_ypos(int& err, int line_index)
{
    mx_word* w;

    w = (mx_word*)words.item(err, line_index, 0);
    MX_ERROR_CHECK(err);

    return w->y;

abort:
    return 0.0;
}

void mx_paragraph::append_mod(int& err, mx_char_style_mod& a_mod)
{
    mx_para_pos pp;

    pp.moveto_end(err, this);
    MX_ERROR_CHECK(err);

    mx_word::set_operations_to_right_of_mods();

    aka_insert_mod(err, a_mod, pp);
    MX_ERROR_CHECK(err);
abort:;
}

void mx_paragraph::delete_blank_lines(int& err, int i)
{
    mx_list* l;

    set_mem_state(mx_in_mem_and_modified_e);
    while (i < words.get_num_rows()) {
        int nw = num_words(err, i);
        MX_ERROR_CHECK(err);

        if (nw == 0) {
            l = words.delete_row(err, i);
            MX_ERROR_CHECK(err);

            delete l;
        } else {
            i++;
        }
    }
abort:;
}

mx_cw_t* mx_paragraph::delete_to_left(int& err, mx_para_pos* p)
{
    mx_para_pos pos = *p, left;
    int n, num_items;
    mx_word *w, *left_word;
    mx_cw_t* res = NULL;

    if (p->is_start()) {
        MX_ERROR_THROW(err, MX_POSITION_RANGE_ERROR);
    }

    set_mem_state(mx_in_mem_and_modified_e);
    mx_word::set_operations_to_right_of_mods();

    if (pos.letter_index != 0) {
        p->left(err, this);
        MX_ERROR_CHECK(err);

        // delete a single thing from this word
        w = get_word(err, pos);
        MX_ERROR_CHECK(err);

        res = w->remove(err, p->letter_index, p->letter_index + 1);
        MX_ERROR_CHECK(err);

        if (w->get_num_visible_items() == 0) {
            w = remove_word(err, pos);
            MX_ERROR_CHECK(err);

            delete w;
        }
    } else {
        mx_para_pos p2 = *p;

        // delete the last thing from the previous word
        p->left(err, this);
        MX_ERROR_CHECK(err);

        w = get_word(err, *p);
        MX_ERROR_CHECK(err);

        if (w->is_a(mx_break_word_class_e)) {
            mx_break_word* bw = (mx_break_word*)w;

            fix_trailing_style_mods(err, *p, p2);
            MX_ERROR_CHECK(err);

            if (bw->type() == mx_long_word_break_e) {
                res = delete_to_left(err, p);
                MX_ERROR_CHECK(err);

                return res;
            }
        }

        num_items = w->get_num_visible_items();
        if (num_items <= 1) {
            // delete previous word
            w = remove_word(err, *p);
            MX_ERROR_CHECK(err);

            if (w->rtti_class() == mx_break_word_class_e) {
                mx_break_word* bw = (mx_break_word*)w;

                switch (bw->type()) {
                case mx_paragraph_break_e:
                    res = new mx_character_cw_t(MX_CW_T_PARA_BREAK_CHAR);
                    break;

                case mx_line_break_e:
                    res = new mx_character_cw_t(MX_CW_T_LINE_BREAK_CHAR);
                    break;

                default:
                    res = NULL;
                    break;
                }
            } else {
                if (num_items == 1) {
                    res = w->remove(err, 0, 1);
                    MX_ERROR_CHECK(err);
                } else {
                    res = NULL;
                }
            }

            delete w;

        } else {
            // delete last letter of previous word
            res = w->remove(err, num_items - 1, num_items);
            MX_ERROR_CHECK(err);
        }
    }

    pos = *p;
    left = *p;

    left.left(err, this);
    if (err == MX_POSITION_RANGE_ERROR) {
        MX_ERROR_CLEAR(err);
    } else {
        bool b;
        left_word = get_word(err, left);
        MX_ERROR_CHECK(err);

        n = left_word->get_num_visible_items();

        b = join_words(err, left);
        MX_ERROR_CHECK(err);

        if (b) {
            // as this is delete to the left, it must the left word we just
            // joined up with
            *p = left;
            p->letter_index = n;
        }
    }

    delete_blank_lines(err, pos.line_index);
    MX_ERROR_CHECK(err);

    MX_ERROR_ASSERT(err, res != NULL);
abort:
    return res;
}

void mx_paragraph::insert_char(int& err, char c, mx_para_pos& p)
{
    set_word_operations(p);

    switch (c) {
    case ' ':
        aka_insert_space(err, p);
        break;
    case '\t':
        aka_insert_tab(err, p);
        break;
    default:
        aka_insert_char(err, c, p);
    }
    MX_ERROR_CHECK(err);
abort:;
}

void mx_paragraph::aka_insert_char(int& err, char c, mx_para_pos& p)
{
    mx_simple_word* new_word = new mx_simple_word(&c, 1);

    aka_insert_word(err, new_word, p);
    MX_ERROR_CHECK(err);

abort:;
}

void mx_paragraph::aka_insert_space(int& err, mx_para_pos& p)
{
    mx_space_word* new_word = new mx_space_word;
    new_word->append_space();

    aka_insert_word(err, new_word, p);
    MX_ERROR_CHECK(err);

abort:;
}

void mx_paragraph::aka_insert_tab(int& err, mx_para_pos& p)
{
    mx_space_word* new_word = new mx_space_word;
    new_word->append_tab();

    aka_insert_word(err, new_word, p);
    MX_ERROR_CHECK(err);

abort:;
}

void mx_paragraph::insert_cw_item(int& err, mx_cw_t* item, mx_para_pos& p)
{
    set_word_operations(p);

    switch (item->rtti_class()) {
    case mx_style_mod_cw_t_class_e:
        insert_mod(err, ((mx_style_mod_cw_t*)item)->m, p);
        MX_ERROR_CHECK(err);
        delete item;
        break;
    case mx_character_cw_t_class_e:
        insert_char(err, ((mx_character_cw_t*)item)->c, p);
        MX_ERROR_CHECK(err);
        delete item;
        break;
    default: {
        mx_complex_word* new_word = new mx_complex_word;
        new_word->append(item);

        insert_word(err, new_word, p);
        MX_ERROR_CHECK(err);
        break;
    }
    }
abort:;
}

void mx_paragraph::insert_line_break(int& err, mx_para_pos& p)
{
    int n;
    mx_break_word* new_word = new mx_break_word(mx_line_break_e);

    insert_word(err, new_word, p);
    MX_ERROR_CHECK(err);

    new_word = (mx_break_word*)get_word(err, p);
    MX_ERROR_CHECK(err);

    MX_ERROR_ASSERT(err, (new_word->is_a(mx_break_word_class_e) && new_word->type() == mx_line_break_e));

    n = num_words(err, p.line_index);
    MX_ERROR_CHECK(err);

    words.move_items_out_end(err, p.line_index, n - p.word_index - 1);
    MX_ERROR_CHECK(err);

abort:;
}

int mx_paragraph::get_num_non_space_words(int& err, int line_index,
    int word_index)
{
    mx_list* word_line;
    int i, num_words = 0, num_non_space_words = 0;

    word_line = words.row(err, line_index);
    MX_ERROR_CHECK(err);

    num_words = word_line->get_num_items();

    for (i = word_index; i < num_words; i++) {
        mx_word* w = (mx_word*)word_line->get(err, i);
        MX_ERROR_CHECK(err);

        if (w->is_printable())
            num_non_space_words++;
    }

abort:
    return num_non_space_words;
}

void mx_paragraph::move_to_document(int& err, mx_document* new_doc)
{
    mx_nlist_iterator iter(words);
    mx_paragraph_style *old_ps = this->get_base_paragraph_style(), *new_ps;

    set_mem_state(mx_in_mem_and_modified_e);

    new_ps = new_doc->add_or_get_style(err, old_ps);
    MX_ERROR_CHECK(err);

    set_paragraph_style(new_ps);

    while (iter.more()) {
        mx_word* w = (mx_word*)iter.data();

        w->move_to_document(err, new_doc);
        MX_ERROR_CHECK(err);
    }

    set_document(new_doc);
abort:;
}

int mx_paragraph::get_break_line_for_height(int& err, float f)
{
    int i, n;
    float height;
    float h = 0.0;

    mx_border_style* bs;
    mx_paragraph_style& stu = get_style_to_use();

    bs = stu.get_border_style();

    if (type == mx_paragraph_start_e || type == mx_paragraph_whole_e) {
        if (bs->top_style.line_type != mx_no_line) {
            h += bs->top_style.width;
            h += bs->distance_from_contents;
        }
        h += stu.space_before;
    }

    n = words.get_num_rows();

    for (i = 0; i < n; i++) {
        height = height_for_line(err, i);
        MX_ERROR_CHECK(err);

        if ((h + height) > f) {
            return i;
        } else {
            h += height;
        }
    }

    return n - 1;

abort:
    return -1;
}

float mx_paragraph::get_height_from_line(int& err, int line_num)
{
    mx_word* w;
    float ascender, descender, height;

    if (line_num == 0) {
        return 0.0;
    } else {
        w = (mx_word*)words.item(err, line_num - 1, 0);
        MX_ERROR_CHECK(err);

        metrics_for_line(err, line_num - 1, ascender, descender, height);
        MX_ERROR_CHECK(err);

        return w->y - descender;
    }

abort:
    return 0.0;
}

bool mx_paragraph::get_height_changed()
{
    return height_changed;
}

void mx_paragraph::reformat_range(int& err, int& upper, int& lower)
{
    int n = words.get_num_rows();

    if (reformat_upper > reformat_lower || reformat_upper < 0 || reformat_lower >= n) {
        // if a reformat has not been done, then return the whole paragraph
        reformat_upper = 0;
        reformat_lower = n - 1;
    }

    upper = reformat_upper;
    lower = reformat_lower;
}

void mx_paragraph::reformat_range(int& err, float& top, float& bottom)
{
    float ascender, descender, height;
    int n = words.get_num_rows();
    mx_word* w;

    if (reformat_upper > reformat_lower || reformat_upper < 0 || reformat_lower >= n) {
        // if a reformat has not been done, then return the whole paragraph
        reformat_upper = 0;
        reformat_lower = n - 1;
    }

    if (reformat_upper == 0) {
        top = 0.0;
    } else {
        w = (mx_word*)words.item(err, reformat_upper - 1, 0);
        MX_ERROR_CHECK(err);

        top = w->y;
    }

    metrics_for_line(err, reformat_lower, ascender, descender, height);
    MX_ERROR_CHECK(err);

    w = (mx_word*)words.item(err, reformat_lower, 0);
    MX_ERROR_CHECK(err);

    bottom = w->y - descender;
abort:;
}

void mx_paragraph::highlight_height_for_line(int& err,
    int line_index,
    float& height,
    float& descender)
{
    float t, prev_desc, prev_height;
    mx_word *w1, *w2;
    ;

    if (this->num_lines() == 1) {
        w1 = (mx_word*)this->words.item(err, 0, 0);
        MX_ERROR_CHECK(err);

        descender = w1->y - this->get_height();
        height = w1->y;
    } else if (line_index == 0) {
        this->metrics_for_line(err, 0, height, descender, t);
        MX_ERROR_CHECK(err);

        w1 = (mx_word*)this->words.item(err, 0, 0);
        MX_ERROR_CHECK(err);

        height = w1->y;
    } else if (line_index == (this->num_lines() - 1)) {
        w1 = (mx_word*)this->words.item(err, line_index, 0);
        MX_ERROR_CHECK(err);

        w2 = (mx_word*)this->words.item(err, line_index - 1, 0);
        MX_ERROR_CHECK(err);

        this->metrics_for_line(err, line_index - 1, prev_height, prev_desc, t);
        MX_ERROR_CHECK(err);

        descender = w1->y - this->get_height();
        height = w1->y - w2->y + prev_desc;
    } else {
        this->metrics_for_line(err, line_index, height, descender, t);
        MX_ERROR_CHECK(err);

        this->metrics_for_line(err, line_index - 1, prev_height, prev_desc, t);
        MX_ERROR_CHECK(err);

        w1 = (mx_word*)this->words.item(err, line_index, 0);
        MX_ERROR_CHECK(err);

        w2 = (mx_word*)this->words.item(err, line_index - 1, 0);
        MX_ERROR_CHECK(err);

        height = w1->y - w2->y + prev_desc;
    }

abort:;
}

void mx_paragraph::split_word(int& err, mx_para_pos& p)
{
    mx_word *w, *new_word;

    w = get_word(err, p);
    MX_ERROR_CHECK(err);

    new_word = w->split(err, p.letter_index);
    if (err != MX_ERROR_OK) {
        MX_ERROR_CLEAR(err);
        p.letter_index = 0;

        if (mx_word::get_operations_to_left_of_mods() || !w->is_a(mx_break_word_class_e) || !w->has_style_mod()) {
            return;
        } else {
            mx_char_style_mod* m;
            mx_break_word* bw = (mx_break_word*)w;
            mx_complex_word* cw = new mx_complex_word;

            m = w->get_style_mod(err, 0);
            MX_ERROR_CHECK(err);

            *cw += *m;
            cw->set_style(bw->get_style());
            *bw = mx_break_word(bw->type());

            new_word = bw;
            w = cw;

            words.overwrite(err, w, p.line_index, p.word_index);
            MX_ERROR_CHECK(err);

            {
                mx_para_pos left_p = p;
                left_p.left_word(err, this);
                if (err != MX_ERROR_OK) {
                    MX_ERROR_CLEAR(err);
                } else {
                    bool b = join_words(err, left_p, p);
                    MX_ERROR_CHECK(err);

                    if (b) {
                        p = left_p;
                        w = get_word(err, p);
                        MX_ERROR_CHECK(err);
                    }
                }
            }
        }
    }

    new_word->set_style(w->get_style());
    if (w->get_style() != NULL) {
        mx_char_style s = *w->get_style(), *ptr;
        w->get_end_style(s);

        if (s != *w->get_style()) {
            ptr = new mx_char_style(s);
            new_word->set_style(ptr);
            styles.append(ptr);
        }
    }

    p.word_index++;
    p.letter_index = 0;

    words.insert(err, new_word, p.line_index, p.word_index);
    MX_ERROR_CHECK(err);

abort:;
}

mx_db_mem_state_t mx_paragraph::get_mem_state()
{
    return mem_state;
}

void mx_paragraph::set_mem_state(mx_db_mem_state_t s)
{
    mem_state = s;
}

void mx_paragraph::insert_ascii_string(int& err, const char*& string,
    mx_para_pos& p)
{
    set_word_operations(p);
    aka_insert_ascii_string(err, string, p);
}

void mx_paragraph::aka_insert_ascii_string(int& err, const char*& string,
    mx_para_pos& p)
{
    mx_char_style* style_for_word;
    mx_word *w, *first_word;
    mx_para_pos start_pos, left_pos;
    int num_words_inserted = 0, end_letter_index;
    bool b;

    w = get_word_from_string(err, string);
    MX_ERROR_CHECK(err);

    if (w != NULL) {
        split_word(err, p);
        MX_ERROR_CHECK(err);

        start_pos = p;

        first_word = get_word(err, p);
        MX_ERROR_CHECK(err);
        style_for_word = first_word->get_style();

        while (w != NULL) {
            w->set_style(style_for_word);
            words.insert(err, w, p.line_index, p.word_index++);
            MX_ERROR_CHECK(err);

            num_words_inserted++;

            w = get_word_from_string(err, string);
            MX_ERROR_CHECK(err);
        }

        if (start_pos.word_index > 0) {
            // try a join at the start

            left_pos = start_pos;
            left_pos.word_index--;

            b = join_words(err, left_pos, start_pos);
            MX_ERROR_CHECK(err);

            if (b) {
                // they were joined, so shift the end pos back one word
                p.word_index--;
            }
        }

        // now do a join at the end

        left_pos = p;
        left_pos.word_index--;

        first_word = this->get_word(err, left_pos);
        MX_ERROR_CHECK(err);

        end_letter_index = first_word->get_num_visible_items();

        b = join_words(err, left_pos, p);
        MX_ERROR_CHECK(err);

        if (b) {
            p.word_index--;
            p.letter_index = end_letter_index;
        }
    }
abort:;
}

mx_word* mx_paragraph::get_word_from_string(int& err, const char*& input_string)
{
    uint8 real_char = *input_string;

    if (real_char == ' ' || real_char == '\t') {
        mx_space_word* w = new mx_space_word;
        while (TRUE) {
            if (real_char == ' ') {
                w->append_space();
            } else if (real_char == '\t') {
                w->append_tab();
            } else {
                return w;
            }
            real_char = *(++input_string);
        }
    } else if (real_char > ' ') {
        mx_simple_word* w = new mx_simple_word;

        do {
            *w += real_char;
            real_char = *(++input_string);
        } while (real_char > ' ');

        return w;
    }
    return NULL;
}

bool mx_paragraph::has_split_long_word_at_end(int& err)
{
    if (this->get_type() == mx_paragraph_start_e || this->get_type() == mx_paragraph_middle_e) {
        mx_para_pos p;

        p.moveto_end(err, this);
        MX_ERROR_CHECK(err);

        p.left_word(err, this);
        if (err == MX_POSITION_RANGE_ERROR) {
            MX_ERROR_CLEAR(err);
        } else {
            MX_ERROR_CHECK(err);

            mx_break_word* break_word;

            break_word = (mx_break_word*)this->get_word(err, p);
            MX_ERROR_CHECK(err);

            if (break_word->is_a(mx_break_word_class_e)) {
                if (break_word->type() == mx_long_word_break_e) {
                    return TRUE;
                }
            }
        }
    }
abort:
    return FALSE;
}
