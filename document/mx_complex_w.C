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
 * MODULE/CLASS :  mx_complex_word
 *
 * AUTHOR : Andrew Haisley
 *
 *
 *
 * DESCRIPTION:
 *
 * Words with character style modifications.
 *
 *
 *
 *
 */
#include "mx_complex_w.h"
#include "mx_cw_t.h"
#include "mx_simple_w.h"
#include <mx.h>
#include <mx_list.h>
#include <mx_list_iter.h>
#include <mx_rtti.h>
#include <mx_style.h>
#include <mx_word.h>

mx_complex_word::mx_complex_word()
    : mx_word()
{
    word_pointer = NULL;
}

mx_complex_word::mx_complex_word(const char* s)
    : mx_word()
{
    word_pointer = NULL;

    items = mx_list();

    if (s != NULL) {
        while (*s != 0)
            items.append(new mx_character_cw_t(*s++));
    }
}

mx_complex_word::mx_complex_word(mx_complex_word& o)
{
    mx_list_iterator iter(o.items);

    word_pointer = NULL;
    x = o.x;
    y = o.y;

    items = mx_list();

    while (iter.more()) {
        mx_cw_t* e = (mx_cw_t*)iter.data();
        items.append(e->deep_copy());
    }
}

mx_complex_word::~mx_complex_word()
{
    mx_list_iterator iter(items);

    while (iter.more()) {
        mx_cw_t* e = (mx_cw_t*)iter.data();
        delete e;
    }

    if (word_pointer != NULL) {
        delete[] word_pointer;
    }
}

// the width according to the underlying outline font(s).
float mx_complex_word::width()
{
    int err = MX_ERROR_OK;
    mx_cw_t* e;
    float res = 0.0;
    int i, index = items.get_num_items();
    mx_char_style temp_style = *style;

    for (i = 0; i < index; i++) {
        e = (mx_cw_t*)items.get(err, i);

        // there should be no errors here
        MX_ERROR_ASSERT(err, err == MX_ERROR_OK);

        e->mod_style(temp_style);
        res += e->width(temp_style);
    }
    return res;
abort:
    global_error_trace->print();
    MX_ERROR_CLEAR(err);
    return res;
}

// the width according to the underlying outline font(s).
float mx_complex_word::width(int index)
{
    int err = MX_ERROR_OK;
    mx_cw_t* e;
    float res = 0.0;
    int i;
    mx_char_style temp_style = *style;

    index = get_char_index(err, index);
    MX_ERROR_CHECK(err);

    for (i = 0; i < index; i++) {
        e = (mx_cw_t*)items.get(err, i);
        MX_ERROR_ASSERT(err, err == MX_ERROR_OK);

        e->mod_style(temp_style);
        res += e->width(temp_style);
    }
    return res;
abort:
    global_error_trace->print();
    MX_ERROR_CLEAR(err);
    return res;
}

// the metrics according to the underlying character style(s)
void mx_complex_word::metrics(
    int& err,
    float& spacing,
    float& ascender,
    float& descender)
{
    mx_word::metrics(err, spacing, ascender, descender);
    MX_ERROR_CHECK(err);

    {
        mx_char_style temp_style = *style;
        mx_list_iterator iter(items);
        mx_cw_t* e;
        float temp;

        while (iter.more()) {
            e = (mx_cw_t*)iter.data();

            if (e->line_difference()) {
                e->mod_style(temp_style);

                temp = e->ascender(temp_style);
                if (temp > ascender)
                    ascender = temp;

                temp = e->descender(temp_style);
                if (temp < descender)
                    descender = temp;

                temp = e->spacing(temp_style);
                if (temp > spacing)
                    spacing = temp;
            }
        }
    }
abort:;
}

// get the word as a C string
const char* mx_complex_word::cstring()
{
    if (word_pointer == NULL) {
        mx_character_cw_t* e;
        int i = 0;

        mx_list_iterator iter(items);

        while (iter.more()) {
            e = (mx_character_cw_t*)iter.data();
            if (e->is_a(mx_character_cw_t_class_e))
                i++;
        }

        word_pointer = new char[i + 1];

        iter.rewind();
        i = 0;

        while (iter.more()) {
            e = (mx_character_cw_t*)iter.data();
            if (e->is_a(mx_character_cw_t_class_e))
                word_pointer[i++] = e->c;
        }
        word_pointer[i] = 0;
    }
    return word_pointer;
}

void mx_complex_word::serialise(int& err, uint8** buffer)
{
    mx_cw_t* e;
    mx_list_iterator iter(items);

    serialise_uint32(items.get_num_items(), buffer);

    while (iter.more()) {
        e = (mx_cw_t*)iter.data();
        serialise_enum(e->rtti_class(), buffer);
        e->serialise(err, buffer);
        MX_ERROR_CHECK(err);
    }

abort:;
}

void mx_complex_word::unserialise(int& err, uint8** buffer, uint32 docid)
{
    mx_cw_t* e = NULL;
    uint16 en;
    mx_rtti_class_e item_type;
    uint32 i, n_items = 0;

    unserialise_uint32(n_items, buffer);

    for (i = 0; i < n_items; i++) {
        unserialise_enum(en, buffer);
        item_type = (mx_rtti_class_e)en;

        switch (item_type) {
        case mx_character_cw_t_class_e:
            e = new mx_character_cw_t;
            break;
        case mx_style_mod_cw_t_class_e:
            e = new mx_style_mod_cw_t;
            break;
        case mx_field_word_cw_t_class_e:
            e = new mx_field_word_cw_t;
            break;
        case mx_area_cw_t_class_e:
            e = new mx_area_cw_t;
            break;
        default:
            // should not be here
            MX_ERROR_THROW(err, MX_ERROR_UNDEFINED);
            break;
        }
        e->unserialise(err, buffer, docid);
        MX_ERROR_CHECK(err);
        items.append(e);
    }

    return;
abort:
    if (e)
        delete e;
}

uint32 mx_complex_word::get_serialised_size(int& err)
{
    uint32 res = SLS_UINT32;
    mx_cw_t* e;

    mx_list_iterator iter(items);

    while (iter.more()) {
        e = (mx_cw_t*)iter.data();

        res += e->get_serialised_size(err) + SLS_ENUM;
        MX_ERROR_CHECK(err);
    }
    return res;
abort:
    return 0;
}

mx_cw_t* mx_complex_word::get_item(int& err, int32 i)
{
    return (mx_cw_t*)items.get(err, i);
}

uint32 mx_complex_word::get_num_items()
{
    return items.get_num_items();
}

// append another word
mx_complex_word& mx_complex_word::operator+=(mx_simple_word& w)
{
    char* s = (char*)w.cstring();
    *this += s;
    return *this;
}

mx_complex_word& mx_complex_word::operator+=(char* s)
{
    if (word_pointer != NULL) {
        delete[] word_pointer;
        word_pointer = NULL;
    }

    while (*s != '\0')
        items.append(new mx_character_cw_t(*s++));

    return *this;
}

mx_complex_word& mx_complex_word::operator+=(mx_field_word& fw)
{
    append(new mx_field_word_cw_t(fw));
    return *this;
}

mx_complex_word& mx_complex_word::operator+=(mx_area& na)
{
    append(new mx_area_cw_t(&na));
    return *this;
}

mx_complex_word& mx_complex_word::operator+=(mx_complex_word& w)
{
    if (word_pointer != NULL) {
        delete[] word_pointer;
        word_pointer = NULL;
    }

    mx_cw_t* e;

    mx_list_iterator iter(w.items);

    while (iter.more()) {
        e = (mx_cw_t*)iter.data();
        items.append(e->deep_copy());
    }
    return *this;
}

// append a character
mx_complex_word& mx_complex_word::operator+=(char c)
{
    append(new mx_character_cw_t(c));
    return *this;
}

mx_complex_word& mx_complex_word::operator+=(mx_char_style_mod& s)
{
    append(new mx_style_mod_cw_t(s));
    return *this;
}

// insert a character to the left of the given character index - note
// that character indexes are just that (i.e. they ignore style changes)
void mx_complex_word::insert(int& err, char c, int index)
{
    insert(err, new mx_character_cw_t(c), index);
}

void mx_complex_word::insert(int& err, char* s, int index)
{
    int real_index = get_char_index(err, index);
    MX_ERROR_CHECK(err);

    if (word_pointer != NULL) {
        delete[] word_pointer;
        word_pointer = NULL;
    }

    while (*s != '\0') {
        mx_cw_t* e = new mx_character_cw_t(*s++);
        items.insert(err, real_index, e);
        MX_ERROR_CHECK(err);
        real_index++;
    }

abort:;
}

// insert a style change to the left of the given character index - note
// that character indexes are just that (i.e. they ignore style changes)
void mx_complex_word::insert(int& err, mx_char_style_mod& s, int index)
{
    insert(err, new mx_style_mod_cw_t(s), index);
    MX_ERROR_CHECK(err);
abort:;
}

void mx_complex_word::insert(int& err, mx_cw_t* item, int index)
{
    int real_index = get_char_index(err, index);
    MX_ERROR_CHECK(err);

    if (word_pointer != NULL) {
        delete[] word_pointer;
        word_pointer = NULL;
    }

    if (item->is_a(mx_style_mod_cw_t_class_e)) {
        mx_style_mod_cw_t* e = (mx_style_mod_cw_t*)item;

        if (operations_to_left_of_mods) {
            if (real_index < items.get_num_items()) {
                mx_style_mod_cw_t* next = (mx_style_mod_cw_t*)items.get(err, real_index);
                MX_ERROR_CHECK(err);

                if (next->is_a(mx_style_mod_cw_t_class_e)) {
                    e->m += next->m;
                    next->m = e->m;
                    delete e;
                    if (next->m.is_null()) {
                        items.remove(err, real_index);
                        MX_ERROR_CHECK(err);
                        delete next;
                    }
                    return;
                }
            }
        } else {
            if (real_index > 0) {
                mx_style_mod_cw_t* prev = (mx_style_mod_cw_t*)items.get(err, real_index - 1);
                MX_ERROR_CHECK(err);

                if (prev->is_a(mx_style_mod_cw_t_class_e)) {
                    prev->m += e->m;
                    delete e;
                    if (prev->m.is_null()) {
                        items.remove(err, real_index - 1);
                        MX_ERROR_CHECK(err);
                        delete prev;
                    }
                    return;
                }
            }
        }
    }

    items.insert(err, real_index, item);
    MX_ERROR_CHECK(err);

abort:;
}

void mx_complex_word::append(mx_cw_t* item)
{
    if (word_pointer != NULL) {
        delete[] word_pointer;
        word_pointer = NULL;
    }

    if (items.get_num_items() > 0 && item->is_a(mx_style_mod_cw_t_class_e)) {
        mx_style_mod_cw_t* e1 = (mx_style_mod_cw_t*)item;
        mx_style_mod_cw_t* e2 = (mx_style_mod_cw_t*)items.last();
        if (e2->is_a(mx_style_mod_cw_t_class_e)) {
            e2->m += e1->m;
            delete e1;
            return;
        }
    }
    items.append(item);
}

int mx_complex_word::get_char_index(int& err, int index)
{
    mx_cw_t* e;
    mx_list_iterator iter(items);
    int i = 0, res = 0;
    bool got_index = FALSE;

    if (index < 0)
        index = this->get_num_visible_items();

    while (iter.more()) {
        e = (mx_cw_t*)iter.data();
        got_index = (i == index);

        if (operations_to_left_of_mods && got_index)
            break;

        if (!e->is_a(mx_style_mod_cw_t_class_e)) {
            if (got_index)
                break;
            i++;
        }
        res++;
    }
    if (i == index) {
        return res;
    } else {
        MX_ERROR_THROW(err, MX_WORD_CHAR_INDEX_RANGE);
    }
abort:;
    return -1;
}

// split the word at a given index returning a new word.
mx_word* mx_complex_word::split(int& err, int index)
{
    mx_complex_word* res = NULL;
    index = get_char_index(err, index);
    MX_ERROR_CHECK(err);

    if (index == 0)
        MX_ERROR_THROW(err, MX_WORD_CANT_SPLIT_WORD);

    res = new mx_complex_word;
    res->style = style;
    items.move_items_out_end(err, items.get_num_items() - index, res->items);
    MX_ERROR_CHECK(err);

    if (word_pointer != NULL) {
        delete[] word_pointer;
        word_pointer = NULL;
    }

    return res;
abort:
    if (res)
        delete res;
    return NULL;
}

void mx_complex_word::join(int& err, mx_complex_word* w)
{
    mx_cw_t* e;
    uint32 i, n;

    n = w->items.get_num_items();
    for (i = 0; i < n; i++) {
        e = (mx_cw_t*)w->items.remove(err, 0);
        MX_ERROR_CHECK(err);

        this->append(e);
    }

    delete w;
abort:;
}

bool mx_complex_word::can_be_part_of_long_word()
{
    return TRUE;
}

bool mx_complex_word::is_simple()
{
    mx_list_iterator iter(items);
    mx_cw_t* e;

    while (iter.more()) {
        e = (mx_cw_t*)iter.data();
        if (!e->is_a(mx_character_cw_t_class_e)) {
            return FALSE;
        }
    }
    return TRUE;
}

void mx_complex_word::get_end_style(mx_char_style& s)
{
    mx_cw_t* e;
    mx_list_iterator iter(items);

    while (iter.more()) {
        e = (mx_cw_t*)iter.data();
        e->mod_style(s);
    }
}

mx_word* mx_complex_word::move_last_in_start(int& err, int n, mx_word* w)
{
    // what is the class of the other thing?
    if (w->rtti_class() == mx_complex_word_class_e) {
        mx_complex_word* cw = (mx_complex_word*)w;

        while (n > 0) {
            mx_cw_t* e = (mx_cw_t*)cw->items.remove(err, cw->items.get_num_items() - 1);
            MX_ERROR_CHECK(err);

            if (!e->is_a(mx_style_mod_cw_t_class_e)) {
                n--;
            }

            this->insert(err, e, 0);
            MX_ERROR_CHECK(err);
        }
        return this;
    } else {
        if (w->rtti_class() == mx_simple_word_class_e) {
            mx_simple_word* sw = (mx_simple_word*)w;
            mx_cw_t* c;

            while (n > 0) {
                c = sw->remove(err, sw->length() - 1);
                MX_ERROR_CHECK(err);

                items.insert(err, 0, c);
                MX_ERROR_CHECK(err);

                n--;
            }
            return this;
        } else {
            MX_ERROR_THROW(err, MX_WORD_OP_NOT_AVAILABLE_ON_SUBCLASS);
        }
    }
abort:;
    return this;
}

mx_word* mx_complex_word::move_first_in_end(int& err, int n, mx_word* w)
{
    // cannot move more items than there are in the word
    MX_ERROR_ASSERT(err, n <= w->get_num_visible_items());

    if (n > 0) {
        // what is the class of the other thing?
        if (w->rtti_class() == mx_complex_word_class_e) {
            mx_complex_word* cw = (mx_complex_word*)w;

            while (n > 0) {
                mx_cw_t* e = (mx_cw_t*)cw->items.remove(err, 0);
                MX_ERROR_CHECK(err);

                if (!e->is_a(mx_style_mod_cw_t_class_e)) {
                    n--;
                }

                this->append(e);
            }

            if (cw->items.get_num_items() == 1) {
                // if we have removed all the visible items and there is a
                // style mod left in the other word then get that too

                mx_cw_t* e = (mx_cw_t*)cw->items.last();

                if (e->is_a(mx_style_mod_cw_t_class_e)) {
                    this->append(e);

                    cw->items.remove(err, 0);
                    MX_ERROR_CHECK(err);
                }
            }

            return this;
        } else if (w->rtti_class() == mx_simple_word_class_e) {
            mx_simple_word* sw = (mx_simple_word*)w;
            char* s = mx_string_copy(sw->cstring());
            char c = s[n];

            s[n] = '\0';
            *this += s;

            s[n] = c;

            sw->set(s + n, sw->length() - n);
            delete s;

            return this;
        } else {
            MX_ERROR_THROW(err, MX_WORD_OP_NOT_AVAILABLE_ON_SUBCLASS);
        }
    }
abort:
    return this;
}

void mx_complex_word::move_style_mod_out_end(int& err, mx_word* w)
{
    int n = items.get_num_items();
    if (n > 0) {
        mx_style_mod_cw_t* e = (mx_style_mod_cw_t*)items.last();

        if (e->is_a(mx_style_mod_cw_t_class_e)) {
            set_operations_to_left_of_mods();

            w->insert(err, e->m, 0);
            MX_ERROR_CHECK(err);

            e = (mx_style_mod_cw_t*)items.remove(err, n - 1);
            MX_ERROR_CHECK(err);
            delete e;
        }
    }

abort:;
}

bool mx_complex_word::has_style_mod()
{
    mx_list_iterator iter(items);

    while (iter.more()) {
        mx_cw_t* item = (mx_cw_t*)iter.data();

        if (item->is_a(mx_style_mod_cw_t_class_e))
            return TRUE;
    }
    return FALSE;
}

mx_char_style_mod* mx_complex_word::get_style_mod(int& err, int index)
{
    mx_style_mod_cw_t* item;
    set_operations_to_left_of_mods();

    index = get_char_index(err, index);
    MX_ERROR_CHECK(err);

    item = (mx_style_mod_cw_t*)items.get(err, index);
    MX_ERROR_CHECK(err);

    if (item->is_a(mx_style_mod_cw_t_class_e))
        return &item->m;

abort:
    return NULL;
}

bool mx_complex_word::is_empty()
{
    return items.get_num_items() == 0;
}

mx_word* mx_complex_word::split(int& err, float f)
{
    mx_cw_t* e;
    float res = 0.0;
    mx_list_iterator iter(items);
    int i = 0;

    mx_char_style temp_style = *style;

    while (iter.more()) {
        e = (mx_cw_t*)iter.data();

        if (e->is_a(mx_style_mod_cw_t_class_e)) {
            e->mod_style(temp_style);
        } else {
            res += e->width(temp_style);

            if (res > f) {
                break;
            } else {
                i++;
            }
        }
    }
    if (i == 0) {
        if (this->get_num_visible_items() < 2) {
            return NULL;
        } else {
            return split(err, 1);
        }
    } else {
        return split(err, i);
    }
}

// get the style at given character index
void mx_complex_word::get_index_style(
    int& err,
    int index,
    mx_char_style& s)
{
    mx_cw_t* e;
    mx_list_iterator iter(items);
    int i = 0;

    MX_ERROR_ASSERT(err, style != NULL);
    s = *style;

    while (iter.more()) {
        e = (mx_cw_t*)iter.data();

        if (operations_to_left_of_mods && i == index)
            break;
        e->mod_style(s);

        if (!e->is_a(mx_style_mod_cw_t_class_e)) {
            if (i == index)
                break;
            i++;
        }
    }
abort:;
}

int mx_complex_word::get_num_visible_items()
{
    mx_cw_t* e;
    int res = 0;
    mx_list_iterator iter(items);

    while (iter.more()) {
        e = (mx_cw_t*)iter.data();
        if (!e->is_a(mx_style_mod_cw_t_class_e))
            res++;
    }
    return res;
}

mx_word* mx_complex_word::cut(int& err, int start, int end)
{
    mx_style_mod_cw_t* e;
    mx_char_style start_cs, end_cs;
    mx_char_style_mod cs_mod;
    mx_complex_word* res = new mx_complex_word;
    int i, real_start_index, real_end_index;

    res->style = style;

    if (word_pointer != NULL) {
        delete[] word_pointer;
        word_pointer = NULL;
    }

    // get the real indices of the items

    real_start_index = get_char_index(err, start);
    MX_ERROR_CHECK(err);

    real_end_index = get_char_index(err, end);
    MX_ERROR_CHECK(err);

    // get the style modification up to the start position and insert it
    // into the new word

    cs_mod = mx_char_style_mod();
    for (i = 0; i < real_start_index; i++) {
        e = (mx_style_mod_cw_t*)items.get(err, i);
        MX_ERROR_CHECK(err);

        if (e->is_a(mx_style_mod_cw_t_class_e))
            cs_mod += e->m;
    }
    if (!cs_mod.is_null())
        *res += cs_mod;

    // now remove the items in the range building up a style modification as
    // we go.

    cs_mod = mx_char_style_mod();
    for (i = real_start_index; i < real_end_index; i++) {
        e = (mx_style_mod_cw_t*)items.remove(err, real_start_index);
        MX_ERROR_CHECK(err);

        if (e->is_a(mx_style_mod_cw_t_class_e))
            cs_mod += e->m;
        res->append(e);
    }

    // perhaps insert a mod into the original word
    if (!cs_mod.is_null()) {
        this->insert(err, new mx_style_mod_cw_t(cs_mod), start);
        MX_ERROR_CHECK(err);
    }

    return res;
abort:
    return NULL;
}

mx_cw_t* mx_complex_word::remove(int& err, int start, int end)
{
    mx_cw_t* res = NULL;

    mx_complex_word* w = (mx_complex_word*)this->cut(err, start, end);
    MX_ERROR_CHECK(err);

    if (start == (end - 1)) {
        res = (mx_cw_t*)w->items.remove(err, 0);
        MX_ERROR_CHECK(err);

        while (res->is_a(mx_style_mod_cw_t_class_e)) {
            delete res;
            res = (mx_cw_t*)w->items.remove(err, 0);
            MX_ERROR_CHECK(err);
        }
    }

    delete w;

abort:
    return res;
}

void mx_complex_word::set_page_number(uint32 n)
{
    mx_list_iterator iter(items);

    while (iter.more()) {
        ((mx_cw_t*)iter.data())->set_page_number(n);
    }
}

void mx_complex_word::move_to_document(int& err, mx_document* doc)
{
    mx_cw_t* e;
    mx_list_iterator iter(items);

    while (iter.more()) {
        e = (mx_cw_t*)iter.data();
        e->move_to_document(err, doc);
        MX_ERROR_CHECK(err);
    }
abort:;
}

void mx_complex_word::minimise_mods()
{
    int err = MX_ERROR_OK;
    int32 i = 0;
    mx_char_style s = *get_style();
    mx_style_mod_cw_t* e;

    while (i < items.get_num_items()) {
        e = (mx_style_mod_cw_t*)items.get(err, i);
        MX_ERROR_CHECK(err);

        if (e->is_a(mx_style_mod_cw_t_class_e) && (!e->m.modifies(s) || e->m.is_null())) {
            items.remove(err, i);
            MX_ERROR_CHECK(err);
            delete e;
        } else {
            e->mod_style(s);
            i++;
        }
    }
    return;
abort:
    global_error_trace->print();
    MX_ERROR_CLEAR(err);
}

void mx_complex_word::replace_base_style(int& err,
    mx_char_style& replacement_style)
{
    mx_style_mod_cw_t* mod_item;
    mx_list_iterator iter(items);

    while (iter.more()) {
        mod_item = (mx_style_mod_cw_t*)iter.data();
        if (mod_item->is_a(mx_style_mod_cw_t_class_e)) {
            mod_item->m.set_new_default_cs(replacement_style);
        }
    }
}

void mx_complex_word::set_style_mod_revert_flags(int& err,
    mx_char_style& default_style)
{
    mx_list_iterator iter(items);
    mx_style_mod_cw_t* mod_item;

    while (iter.more()) {
        mod_item = (mx_style_mod_cw_t*)iter.data();
        if (mod_item->is_a(mx_style_mod_cw_t_class_e)) {
            mod_item->m.set_revert_to_default_flags(default_style);
        }
    }
}

int mx_complex_word::get_visible_index(int& err, int item)
{
    int i, res = 0;
    mx_cw_t* temp;

    for (i = 0; i < item; i++) {
        temp = (mx_cw_t*)items.get(err, i);
        MX_ERROR_CHECK(err);

        if (temp->rtti_class() != mx_style_mod_cw_t_class_e) {
            res++;
        }
    }
    return res;

abort:
    return -1;
}
