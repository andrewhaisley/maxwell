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
 * MODULE/CLASS :  mx_space_word
 *
 * AUTHOR : Andrew Haisley
 *
 *
 *
 * DESCRIPTION:
 *
 * Words containg spaces, tabs & character style modifications.
 *
 *
 *
 *
 */
#include <mx_cw_t.h>
#include <mx_list.h>
#include <mx_list_iter.h>
#include <mx_space_w.h>
#include <mx_style.h>

struct mx_sw_t {
    mx_sw_t()
    {
        pos = 0;
        type = mx_space_word::mx_space_word_space_e;
        mod = NULL;
    };

    mx_sw_t(const mx_char_style_mod& new_mod)
    {
        pos = 0;
        type = mx_space_word::mx_space_word_mod_e;
        mod = new mx_char_style_mod(new_mod);
    };

    mx_sw_t(const mx_sw_t& other)
    {
        pos = other.pos;
        type = other.type;
        if (type == mx_space_word::mx_space_word_mod_e) {
            mod = new mx_char_style_mod(*other.mod);
        } else {
            mod = NULL;
        }
    };

    ~mx_sw_t()
    {
        if (type == mx_space_word::mx_space_word_mod_e)
            delete mod;
    }

    // NOTE "pos" stores the position of the following item, so that we can
    // store the position of the character following the space word, in order
    // that we can use calculated width to get the width of the entire word

    float pos;
    mx_space_word::mx_space_word_item_t type;
    mx_char_style_mod* mod;
};

mx_space_word::mx_space_word()
    : mx_word()
{
}

mx_space_word::mx_space_word(mx_space_word& o)
    : mx_word()
{
    mx_sw_t* e;

    mx_list_iterator iter(o.items);
    x = o.x;
    y = o.y;

    while (iter.more()) {
        e = (mx_sw_t*)iter.data();
        items.append(new mx_sw_t(*e));
    }
}

mx_space_word::~mx_space_word()
{
    mx_sw_t* e;

    mx_list_iterator iter(items);

    while (iter.more()) {
        e = (mx_sw_t*)iter.data();
        delete e;
    }
}

void mx_space_word::serialise(int& err, uint8** buffer)
{
    mx_sw_t* e;

    mx_list_iterator iter(items);

    while (iter.more()) {
        e = (mx_sw_t*)iter.data();

        serialise_enum(e->type, buffer);
        serialise_float(e->pos, buffer);

        if (e->type == mx_space_word_mod_e) {
            e->mod->serialise(err, buffer);
            MX_ERROR_CHECK(err);
        }
    }

    serialise_enum(mx_space_word_end_e, buffer);
abort:;
}

void mx_space_word::unserialise(int& err, uint8** buffer, uint32 docid)
{
    mx_sw_t* e;
    uint16 tmp;

    while (TRUE) {
        unserialise_enum(tmp, buffer);
        if (((mx_space_word_item_t)tmp) == mx_space_word_end_e)
            return;

        e = new mx_sw_t;
        e->type = (mx_space_word_item_t)tmp;
        unserialise_float(e->pos, buffer);

        switch (e->type) {
        case mx_space_word_mod_e:
            e->mod = new mx_char_style_mod;
            e->mod->unserialise(err, buffer);
            MX_ERROR_CHECK(err);
            break;
        case mx_space_word_space_e:
        case mx_space_word_tab_e:
            break;
        default:
            // should not be here
            MX_ERROR_THROW(err, MX_ERROR_UNDEFINED);
        }
        items.append(e);
    }
abort:;
}

uint32 mx_space_word::get_serialised_size(int& err)
{
    mx_sw_t* e;
    int res = 0;

    mx_list_iterator iter(items);

    while (iter.more()) {
        e = (mx_sw_t*)iter.data();

        res += SLS_ENUM;
        res += SLS_FLOAT;

        if (e->type == mx_space_word_mod_e) {
            res += e->mod->get_serialised_size(err);
            MX_ERROR_CHECK(err);
        }
    }
    res += SLS_ENUM;
abort:
    return res;
}

// append another word
mx_space_word& mx_space_word::operator+=(mx_space_word& w)
{
    mx_list_iterator iter(w.items);
    mx_sw_t* e;

    while (iter.more()) {
        e = (mx_sw_t*)iter.data();

        switch (e->type) {
        case mx_space_word_space_e:
            append_space();
            break;
        case mx_space_word_tab_e:
            append_tab();
            break;
        case mx_space_word_mod_e:
            *this += *(e->mod);
            break;
        case mx_space_word_end_e:
        default:
            break;
        }
    }
    return *this;
}

// append a style change - the change is copied so you can delete your
// copy when finished with it
mx_space_word& mx_space_word::operator+=(mx_char_style_mod& s)
{
    if (items.get_num_items() > 0) {
        mx_sw_t* last = (mx_sw_t*)items.last();
        if (last->type == mx_space_word_mod_e) {
            *(last->mod) += s;
            return *this;
        }
    }

    items.append(new mx_sw_t(s));
    return *this;
}

void mx_space_word::append_space()
{
    mx_sw_t* e = new mx_sw_t;

    e->type = mx_space_word_space_e;
    items.append(e);

    // invalidate the words position so that it gets reformatted
    this->x = this->y = -30.0f;
}

void mx_space_word::append_tab()
{
    mx_sw_t* e = new mx_sw_t;

    e->type = mx_space_word_tab_e;
    items.append(e);

    // invalidate the words position so that it gets reformatted
    this->x = this->y = -30.0f;
}

// insert a space to the left of the given character index - note
// that character indexes are just that (i.e. they ignore style changes)
void mx_space_word::insert_space(int& err, int index)
{
    mx_sw_t* e;
    index = get_char_index(err, index);
    MX_ERROR_CHECK(err);

    e = new mx_sw_t;
    e->type = mx_space_word_space_e;
    items.insert(err, index, e);
    MX_ERROR_CHECK(err);

    // invalidate the words position so that it gets reformatted
    this->x = this->y = -30.0f;
abort:;
}

// insert a tab to the left of the given character index - note
// that character indexes are just that (i.e. they ignore style changes)
void mx_space_word::insert_tab(int& err, int index)
{
    mx_sw_t* e;
    index = get_char_index(err, index);
    MX_ERROR_CHECK(err);

    e = new mx_sw_t;
    e->type = mx_space_word_tab_e;
    items.insert(err, index, e);
    MX_ERROR_CHECK(err);

    // invalidate the words position so that it gets reformatted
    this->x = this->y = -30.0f;
abort:;
}

// insert a style change to the left of the given character index - note
// that character indexes are just that (i.e. they ignore style changes)
void mx_space_word::insert(int& err, mx_char_style_mod& s, int index)
{
    int real_index = get_char_index(err, index);
    MX_ERROR_CHECK(err);

    if (operations_to_left_of_mods) {
        if (real_index < items.get_num_items()) {
            mx_sw_t* next = (mx_sw_t*)items.get(err, real_index);
            MX_ERROR_CHECK(err);

            if (next->type == mx_space_word_mod_e) {
                s += *next->mod;
                *next->mod = s;

                if (next->mod->is_null()) {
                    items.remove(err, real_index);
                    MX_ERROR_CHECK(err);
                    delete next;
                }
                return;
            }
        }
    } else {
        if (real_index > 0) {
            mx_sw_t* prev = (mx_sw_t*)items.get(err, real_index - 1);
            MX_ERROR_CHECK(err);

            if (prev->type == mx_space_word_mod_e) {
                *prev->mod += s;
                if (prev->mod->is_null()) {
                    items.remove(err, real_index - 1);
                    MX_ERROR_CHECK(err);
                    delete prev;
                }
                return;
            }
        }
    }

    items.insert(err, real_index, new mx_sw_t(s));
    MX_ERROR_CHECK(err);

    // invalidate the words position so that it gets reformatted
    this->x = this->y = -30.0f;
abort:;
}

// how many spaces are there in the word?
int mx_space_word::num_spaces()
{
    mx_list_iterator iter(items);
    mx_sw_t* sw;
    int res = 0;

    while (iter.more()) {
        sw = (mx_sw_t*)iter.data();
        if (sw->type == mx_space_word_space_e) {
            res++;
        }
    }
    return res;
}

int mx_space_word::get_num_visible_items()
{
    mx_list_iterator iter(items);
    mx_sw_t* sw;
    int res = 0;

    while (iter.more()) {
        sw = (mx_sw_t*)iter.data();
        if (sw->type != mx_space_word_mod_e) {
            res++;
        }
    }
    return res;
}

// how many items in total
int mx_space_word::get_num_items()
{
    return items.get_num_items();
}

// get the type of a given item
mx_space_word::mx_space_word_item_t
mx_space_word::item_type(int& err, int index)
{
    mx_sw_t* e = (mx_sw_t*)items.get(err, index);
    MX_ERROR_CHECK(err);

    return e->type;
abort:
    return mx_space_word_end_e;
}

// get the position of the given item. Used by the
// reformatting/painters
float mx_space_word::item_pos(int& err, int index)
{
    if (index < 1)
        return this->x;

    mx_sw_t* e = (mx_sw_t*)items.get(err, index - 1);
    MX_ERROR_CHECK(err);

    return e->pos;
abort:
    return 0;
}

// set the position of the given item. Used by the
// reformatting/painters
void mx_space_word::item_pos(int& err, int index, float new_pos)
{
    if (index < 1) {
        this->x = new_pos;
    } else {
        mx_sw_t* e = (mx_sw_t*)items.get(err, index - 1);
        MX_ERROR_CHECK(err);

        e->pos = new_pos;
    }
abort:;
}

// get the style change at the given index
mx_char_style_mod* mx_space_word::get(int& err, int index)
{
    mx_sw_t* e;

    e = (mx_sw_t*)items.get(err, index);
    MX_ERROR_CHECK(err);

    return e->mod;
abort:;
    return NULL;
}

int mx_space_word::get_char_index(int& err, int index)
{
    mx_sw_t* e;
    mx_list_iterator iter(items);
    int i = 0, res = 0;
    bool got_index = FALSE;

    if (index < 0)
        index = this->get_num_visible_items();

    while (iter.more()) {
        e = (mx_sw_t*)iter.data();
        got_index = (i == index);

        if (operations_to_left_of_mods && got_index)
            break;

        if (e->type != mx_space_word_mod_e) {
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
mx_word* mx_space_word::split(int& err, int index)
{
    mx_space_word* res = NULL;
    index = get_char_index(err, index);
    MX_ERROR_CHECK(err);

    if (index == 0)
        MX_ERROR_THROW(err, MX_WORD_CANT_SPLIT_WORD);

    res = new mx_space_word();
    res->style = style;
    items.move_items_out_end(err, items.get_num_items() - index, res->items);
    MX_ERROR_CHECK(err);

    return res;
abort:
    if (res)
        delete res;
    return NULL;
}

float mx_space_word::calculated_width(int index)
{
    int err = MX_ERROR_OK;
    float res;

    index = get_char_index(err, index);
    MX_ERROR_CHECK(err);

    res = item_pos(err, index);
    MX_ERROR_CHECK(err);

    return res - x;
abort:
    global_error_trace->print();
    MX_ERROR_CLEAR(err);
    return 0.0f;
}

const char* mx_space_word::cstring()
{
    mx_list_iterator iter(items);
    mx_sw_t* e;
    uint32 i = 0;
    static const uint32 max_length = 10000;
    static char string[max_length + 1];

    while (iter.more() && i < max_length) {
        e = (mx_sw_t*)iter.data();

        switch (e->type) {
        case mx_space_word_space_e:
            string[i++] = ' ';
            break;
        case mx_space_word_tab_e:
            string[i++] = '\t';
            break;
        default:
            break;
        }
    }
    string[i] = 0;
    return string;
}

void mx_space_word::metrics(
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
        mx_sw_t* e;
        float temp;

        while (iter.more()) {
            e = (mx_sw_t*)iter.data();

            if (e->type == mx_space_word_mod_e)
                temp_style += *(e->mod);

            temp = temp_style.line_space;
            if (temp > spacing)
                spacing = temp;

            temp = temp_style.get_ascender();
            if (temp > ascender)
                ascender = temp;

            temp = temp_style.get_descender();
            if (temp < descender)
                descender = temp;
        }
    }
abort:;
}

void mx_space_word::get_end_style(mx_char_style& s)
{
    mx_sw_t* e;
    mx_list_iterator iter(items);

    while (iter.more()) {
        e = (mx_sw_t*)iter.data();

        if (e->type == mx_space_word_mod_e) {
            s += *(e->mod);
        }
    }
}

void mx_space_word::move_style_mod_out_end(int& err, mx_word* w)
{
    int n = items.get_num_items();
    if (n > 0) {
        mx_sw_t* e = (mx_sw_t*)items.last();

        if (e->type == mx_space_word_mod_e) {
            set_operations_to_left_of_mods();

            w->insert(err, *e->mod, 0);
            MX_ERROR_CHECK(err);

            e = (mx_sw_t*)items.remove(err, n - 1);
            MX_ERROR_CHECK(err);
            delete e;
        }
    }
abort:;
}

bool mx_space_word::has_style_mod()
{
    mx_list_iterator iter(items);

    while (iter.more()) {
        mx_sw_t* item = (mx_sw_t*)iter.data();

        if (item->type == mx_space_word_mod_e)
            return TRUE;
    }
    return FALSE;
}

mx_char_style_mod* mx_space_word::get_style_mod(int& err, int index)
{
    mx_sw_t* item;
    set_operations_to_left_of_mods();

    index = get_char_index(err, index);
    MX_ERROR_CHECK(err);

    item = (mx_sw_t*)items.get(err, index);
    MX_ERROR_CHECK(err);

    if (item->type == mx_space_word_mod_e)
        return item->mod;

abort:
    return NULL;
}

bool mx_space_word::is_empty()
{
    return items.get_num_items() == 0;
}

void mx_space_word::get_index_style(int& err, int index, mx_char_style& s)
{
    mx_sw_t* e;
    mx_list_iterator iter(items);
    int i = 0;

    MX_ERROR_ASSERT(err, style != NULL);
    s = *style;

    while (iter.more()) {
        e = (mx_sw_t*)iter.data();

        if (operations_to_left_of_mods && i == index)
            break;
        if (e->type == mx_space_word_mod_e) {
            s += *e->mod;
        } else {
            if (i == index)
                break;
            i++;
        }
    }
abort:;
}

mx_word* mx_space_word::cut(int& err, int start, int end)
{
    mx_sw_t* e;
    mx_char_style start_cs, end_cs;
    mx_char_style_mod cs_mod;
    mx_space_word* res = new mx_space_word;
    int i, real_start_index, real_end_index;

    res->style = style;

    // get the real indices of the items

    real_start_index = get_char_index(err, start);
    MX_ERROR_CHECK(err);

    real_end_index = get_char_index(err, end);
    MX_ERROR_CHECK(err);

    // get the style modification up to the start position and insert it
    // into the new word

    cs_mod = mx_char_style_mod();
    for (i = 0; i < real_start_index; i++) {
        e = (mx_sw_t*)items.get(err, i);
        MX_ERROR_CHECK(err);

        if (e->type == mx_space_word_mod_e)
            cs_mod += *e->mod;
    }
    if (!cs_mod.is_null())
        *res += cs_mod;

    // now remove the items in the range building up a style modification as
    // we go.

    cs_mod = mx_char_style_mod();
    for (i = real_start_index; i < real_end_index; i++) {
        e = (mx_sw_t*)items.remove(err, real_start_index);
        MX_ERROR_CHECK(err);

        if (e->type == mx_space_word_mod_e) {
            cs_mod += *e->mod;
            *res += *e->mod;
            delete e;
        } else {
            res->items.append(e);
        }
    }

    // perhaps insert a mod into the original word
    if (!cs_mod.is_null()) {
        this->insert(err, cs_mod, start);
        MX_ERROR_CHECK(err);
    }

    // invalidate the words position so that it gets reformatted
    this->x = this->y = -30.0f;

    return res;
abort:
    return NULL;
}

float mx_space_word::width()
{
    return width_of_trailing_spaces();
}

float mx_space_word::width(int index)
{
    int i, err = MX_ERROR_OK;
    mx_char_style s = *get_style();
    mx_sw_t* e;
    float res = 0.0;

    MX_ERROR_ASSERT(err, index >= 0 && index < items.get_num_items());

    for (i = 0; i < index; i++) {
        e = (mx_sw_t*)items.get(err, i);
        MX_ERROR_CHECK(err);

        switch (e->type) {
        case mx_space_word_space_e:
            res += s.width(' ');
            break;
        case mx_space_word_mod_e:
            s += *(e->mod);
            break;
        case mx_space_word_tab_e:
            MX_ERROR_THROW(err, MX_ERROR_UNDEFINED);
        case mx_space_word_end_e:
        default:
            break;
        }
    }
    return res;
abort:
    global_error_trace->print();
    MX_ERROR_CLEAR(err);
    return 0;
}

mx_cw_t* mx_space_word::remove(int& err, int start, int end)
{
    mx_space_word_item_t t;
    mx_cw_t* res = NULL;

    mx_space_word* w = (mx_space_word*)this->cut(err, start, end);
    MX_ERROR_CHECK(err);
    MX_ERROR_ASSERT(err, w->is_a(mx_space_word_class_e));

    t = w->item_type(err, 0);
    MX_ERROR_CHECK(err);

    while (t == mx_space_word_mod_e) {
        mx_sw_t* item = (mx_sw_t*)w->items.remove(err, 0);
        MX_ERROR_CHECK(err);

        delete item;

        t = w->item_type(err, 0);
        MX_ERROR_CHECK(err);
    }

    if (t == mx_space_word_space_e) {
        res = new mx_character_cw_t(MX_CW_T_SPACE_CHAR);
    } else {
        if (t == mx_space_word_tab_e) {
            res = new mx_character_cw_t(MX_CW_T_TAB_CHAR);
        }
    }

    delete w;

abort:
    return res;
}

int mx_space_word::get_first_tab_index()
{
    int err = MX_ERROR_OK;
    int i, num_items = items.get_num_items();

    for (i = 0; i < num_items; i++) {
        mx_space_word_item_t i_t = item_type(err, i);
        MX_ERROR_CHECK(err);

        if (i_t == mx_space_word_tab_e) {
            return i;
        }
    }

    return -1;
abort:
    global_error_trace->print();
    return -1;
}

int mx_space_word::get_last_tab_index()
{
    int err = MX_ERROR_OK;
    int i, tab_index = -1, num_items = items.get_num_items();

    for (i = 0; i < num_items; i++) {
        mx_space_word_item_t i_t = item_type(err, i);
        MX_ERROR_CHECK(err);

        if (i_t == mx_space_word_tab_e) {
            tab_index = i;
        }
    }

    return tab_index;
abort:
    global_error_trace->print();
    return -1;
}

// get the width of all the spaces (if any) after the last tab in the word
float mx_space_word::width_of_trailing_spaces()
{
    mx_char_style s = *get_style();
    mx_sw_t* e;
    mx_list_iterator iter(items);
    float res = 0.0;

    while (iter.more()) {
        e = (mx_sw_t*)iter.data();

        switch (e->type) {
        case mx_space_word_space_e:
            res += s.width(' ');
            break;
        case mx_space_word_tab_e:
            res = 0.0;
            break;
        case mx_space_word_mod_e:
            s += *(e->mod);
            break;
        case mx_space_word_end_e:
        default:
            break;
        }
    }
    return res;
}

void mx_space_word::move_x(float x_offset)
{
    mx_sw_t* e;
    mx_list_iterator iter(items);

    while (iter.more()) {
        e = (mx_sw_t*)iter.data();
        e->pos += x_offset;
    }

    x += x_offset;
}

void mx_space_word::minimise_mods()
{
    int err = MX_ERROR_OK;
    int32 i = 0;
    mx_char_style s = *get_style();
    mx_sw_t* e;

    while (i < items.get_num_items()) {
        e = (mx_sw_t*)items.get(err, i++);
        MX_ERROR_CHECK(err);

        if (e->type == mx_space_word_mod_e) {
            if (!e->mod->modifies(s) || e->mod->is_null()) {
                items.remove(err, --i);
                MX_ERROR_CHECK(err);
                delete e;
            } else {
                s += *e->mod;
            }
        }
    }
    return;
abort:
    global_error_trace->print();
    MX_ERROR_CLEAR(err);
}

void mx_space_word::replace_base_style(int& err,
    mx_char_style& replacement_style)
{
    mx_list_iterator iter(items);
    mx_sw_t* mod_item;

    while (iter.more()) {
        mod_item = (mx_sw_t*)iter.data();
        if (mod_item->type == mx_space_word_mod_e) {
            mod_item->mod->set_new_default_cs(replacement_style);
        }
    }
}

void mx_space_word::set_style_mod_revert_flags(int& err,
    mx_char_style& default_style)
{
    mx_list_iterator iter(items);
    mx_sw_t* mod_item;

    while (iter.more()) {
        mod_item = (mx_sw_t*)iter.data();
        if (mod_item->type == mx_space_word_mod_e) {
            mod_item->mod->set_revert_to_default_flags(default_style);
        }
    }
}
