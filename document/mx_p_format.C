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

#include <vector>

#include <mx.h>
#include <mx_db.h>

#include "mx_paragraph.h"
#include <mx_break_w.h>
#include <mx_complex_w.h>
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

void mx_paragraph::reformat(int& err, bool& height_changed)
{
    mx_para_pos start_pp(0, 0, 0);
    mx_para_pos end_pp(0, 0, 0);

    end_pp.moveto_end(err, this);
    MX_ERROR_CHECK(err);

    reformat(err, start_pp, end_pp, height_changed);
    MX_ERROR_CHECK(err);
abort:;
}

static int reformat_page_number = 1;

void mx_paragraph::set_page_number(uint32 page_num)
{
    reformat_page_number = page_num;
}

void mx_paragraph::metrics_for_line(
    int& err,
    int line_index,
    float& ascender,
    float& descender,
    float& spacing)
{
    mx_list* row = (mx_list*)words.row(err, line_index);
    MX_ERROR_CHECK(err);

    {
        mx_word* word;
        float s, a, d;
        mx_list_iterator iter(*row);

        ascender = spacing = 0.0;
        descender = 99999999.0;

        while (iter.more()) {
            word = (mx_word*)iter.data();

            word->metrics(err, s, a, d);
            MX_ERROR_CHECK(err);

            if (s > spacing) {
                spacing = s;
            }

            if (d < descender) {
                descender = d;
            }

            if (a > ascender) {
                ascender = a;
            }
        }
    }
abort:;
}

float mx_paragraph::height_for_line(int& err, int line_index)
{
    float s, a, d, prev_descender;

    if (line_index == 0) {
        metrics_for_line(err, 0, a, d, s);
        MX_ERROR_CHECK(err);

        return a + MX_POINTS_TO_MM(s);
    } else {
        metrics_for_line(err, line_index - 1, a, prev_descender, s);
        MX_ERROR_CHECK(err);

        metrics_for_line(err, line_index, a, d, s);
        MX_ERROR_CHECK(err);

        return (a + MX_POINTS_TO_MM(s) - prev_descender) * get_style_to_use().line_spacing;
    }

abort:
    return 0.0;
}

float mx_paragraph::ascender_for_line(int& err, int line_index)
{
    float a, d, s;

    metrics_for_line(err, line_index, a, d, s);
    MX_ERROR_CHECK(err);

    return a;
abort:;
    return 0.0;
}

float mx_paragraph::descender_for_line(int& err, int line_index)
{
    float a, d, s;

    metrics_for_line(err, line_index, a, d, s);
    MX_ERROR_CHECK(err);

    return d;
abort:;
    return 0.0;
}

float mx_paragraph::figure_ypos_for_line(
    int& err,
    int line_index,
    bool& moved)
{
    // figure the y position. This is the position of the
    // baseline in mm below the top of the paragraph. In the case of
    // the first line, this is found as follows:
    //   y pos = width_of_border + border_distance_from_contents +
    //           max(ascender for char style in this line) +
    //           space before
    // for subsequent lines, this is found as:
    //   y pos = y pos of previous line +
    //           max(line height for char style in this line).

    // moved is set if the figured ypos is different from the
    // current one for the line given or if words on the line were
    // at different y positions (after overflow for example)

    mx_border_style* bs;

    float res = 0.0;
    mx_list* prev_row;
    mx_word* word;

    (void)words.row(err, line_index);
    MX_ERROR_CHECK(err);

    bs = get_style_to_use().get_border_style();

    if (line_index == 0) {
        if (type == mx_paragraph_start_e || type == mx_paragraph_whole_e) {
            if (bs->top_style.line_type != mx_no_line) {
                res += bs->top_style.width;
            }
            res += get_style_to_use().space_before + bs->distance_from_contents;
        }

        res += height_for_line(err, 0);
        MX_ERROR_CHECK(err);
    } else {
        prev_row = words.row(err, line_index - 1);
        MX_ERROR_CHECK(err);

        word = (mx_word*)prev_row->get(err, 0);
        MX_ERROR_CHECK(err);

        res = word->y;

        res += height_for_line(err, line_index);
        MX_ERROR_CHECK(err);
    }

    set_line_ypos(err, line_index, res, moved);
    MX_ERROR_CHECK(err);

abort:
    return res;
}

void mx_paragraph::set_line_ypos(int& err, int line, float pos, bool& moved)
{
    mx_list* row = (mx_list*)words.row(err, line);
    MX_ERROR_CHECK(err);

    moved = FALSE;

    {
        mx_word* word = NULL;
        mx_list_iterator iter(*row);

        // set the y-positions - where necessary
        while (iter.more()) {
            word = (mx_word*)iter.data();
            if (!MX_FLOAT_EQ(word->y, pos)) {
                moved = TRUE;
                word->y = pos;
            }
        }
    }

    if (moved) {
        set_mem_state(mx_in_mem_and_modified_e);

        set_reformat_range(err, line);
        MX_ERROR_CHECK(err);
    }

abort:;
}

// deal with a left justfied line that has overflowed over the right hand side. The
// line/word indexes point to the word that is hanging over. There are three
// cases to deal with:
// 1. the first word on the line is longer than the line length. The word must
//    be split up and a long word break inserted.
// 2. the word is a space word
// 3. the word is a complex or simple word and must be moved on to the
//    next line
void mx_paragraph::reformat_line_left_justify_overflow(int& err, mx_word* word, int line_index, int word_index)
{
    mx_rtti_class_e word_type = word->rtti_class();
    int n_items;

    n_items = words.get_num_items(err, line_index);
    MX_ERROR_CHECK(err);

    switch (word_type) {
    case mx_space_word_class_e: {
        // if we've got a space word then we only want to move the words
        // following the space if they are not break words
        if (word_index < (n_items - 1)) {
            mx_break_word* w = (mx_break_word*)words.item(err, line_index, word_index + 1);
            MX_ERROR_CHECK(err);

            if (w->is_a(mx_break_word_class_e) && w->type() != mx_long_word_break_e) {
                w->x = (word->x + left_justify_space_width(err, (mx_space_word*)word, line_index, word_index));
                MX_ERROR_CHECK(err);

                if (word_index < (n_items - 2)) {
                    words.move_items_out_end(err, line_index, n_items - word_index - 2);
                    MX_ERROR_CHECK(err);
                }
            } else {
                words.move_items_out_end(err, line_index, n_items - word_index - 1);
                MX_ERROR_CHECK(err);
            }
        }
        break;
    }
    case mx_break_word_class_e:
        // case 2 : leave the break word in place, but move whatever
        // comes next (if anything)
        if (word_index < (n_items - 1)) {
            words.move_items_out_end(err, line_index, n_items - word_index - 1);
            MX_ERROR_CHECK(err);
        }
        break;
    case mx_simple_word_class_e:
    case mx_field_word_class_e:
    case mx_complex_word_class_e:
        // case 3 :
        // move this word and anything that follows it on this line onto
        // the next line
        words.move_items_out_end(err, line_index, n_items - word_index);
        MX_ERROR_CHECK(err);
        break;
    default:
        // this means we've got a garbled word type
        MX_ERROR_THROW(err, MX_ERROR_UNDEFINED);
        break;
    }
abort:;
}

float mx_paragraph::left_indent_to_text()
{
    return get_style_to_use().get_ruler()->left_indent;
}

// returns true if a word contains a . (decimal point) character
static bool word_has_dp(mx_word* w, int& index)
{
    char* s = (char*)w->cstring();
    char* t = strchr(s, '.');

    index = t - s;

    return t != NULL;
}

// this gets the width of all the words/spaces following the one pointed to by
// line_index, word_index and item_index, until the next space word which
// contains a tab or until the end of the paragraph. If find_dp is TRUE, the
// routine also stops at the first decimal point character it encounters
static float width_until_tab_or_dp(int& err, mx_nlist* some_words, int line_index,
    int word_index, int item_index, bool find_dp)
{
    float group_width = 0.0;

    // first get the width of all the bits of this space word after the tab
    {
        mx_space_word* w = (mx_space_word*)some_words->item(err, line_index,
            word_index);
        MX_ERROR_CHECK(err);

        MX_ERROR_ASSERT(err, w->is_a(mx_space_word_class_e));
        MX_ERROR_ASSERT(err, w->get_last_tab_index() == item_index);

        group_width += w->width_of_trailing_spaces();
    }

    word_index++;

    while (TRUE) {
        mx_word* w;

        // reajust line/word index
        some_words->next(err, 0, line_index, word_index);
        MX_ERROR_CHECK(err);

        w = (mx_word*)some_words->item(err, line_index, word_index);
        MX_ERROR_CHECK(err);

        switch (w->rtti_class()) {
        case mx_break_word_class_e:
            return group_width;
            break;
        case mx_simple_word_class_e:
        case mx_complex_word_class_e:
        case mx_field_word_class_e: {
            int dp_index = 0;

            if (find_dp && word_has_dp(w, dp_index)) {
                group_width += w->width(dp_index);
                return group_width;
            } else {
                group_width += w->width();
            }
            break;
        }
        case mx_space_word_class_e: {
            int tab_ind = ((mx_space_word*)w)->get_first_tab_index();

            if (tab_ind == -1) {
                group_width += w->width();
            } else {
                group_width += w->width(tab_ind);
                return group_width;
            }
            break;
        }
        default:
            MX_ERROR_THROW(err, MX_ERROR_UNDEFINED);
        }

        word_index++;
    }
abort:;
    return 0.0;
}

void mx_paragraph::space_to_centre_dp_tabify(
    int& err,
    float& x,
    float& tab_position,
    int item_index,
    int line_index,
    int word_index)
{
    float width = width_until_tab_or_dp(err, &words, line_index, word_index,
        item_index, TRUE);
    MX_ERROR_CHECK(err);

    if ((x + width) > tab_position) {
        x = 0;
    } else {
        x = tab_position - width - x;
    }
abort:;
}

void mx_paragraph::space_to_centre_tabify(
    int& err,
    float& x,
    float& tab_position,
    int item_index,
    int line_index,
    int word_index)
{
    float width = 0.5 * width_until_tab_or_dp(err, &words, line_index, word_index, item_index, FALSE);
    MX_ERROR_CHECK(err);

    if ((x + width) > tab_position) {
        x = 0;
    } else {
        x = tab_position - width - x;
    }
abort:;
}

void mx_paragraph::space_to_right_tabify(int& err,
    float& x,
    float& tab_position,
    int item_index,
    int line_index,
    int word_index)
{
    float width = width_until_tab_or_dp(err, &words, line_index, word_index,
        item_index, FALSE);
    MX_ERROR_CHECK(err);

    if ((x + width) > tab_position) {
        x = 0;
    } else {
        x = tab_position - width - x;
    }
abort:;
}

void mx_paragraph::split_long_word(
    int& err,
    mx_word* word,
    int line_index,
    int word_index)
{
    mx_rtti_class_e word_class = word->rtti_class();
    mx_word* new_word;
    int num_items;
    mx_break_word* br;

    // we should only split simple and complex words
    MX_ERROR_ASSERT(err, (word_class == mx_simple_word_class_e || word_class == mx_complex_word_class_e));

    mx_word::set_operations_to_left_of_mods();
    new_word = word->split(err, format_width);
    MX_ERROR_CHECK(err);

    // if the word refuses to split then we've got a BUG
    MX_ERROR_ASSERT(err, new_word != NULL);

    // complex words have style changes so we need to insert the new
    // style in the styles list

    if (new_word->is_a(mx_complex_word_class_e)) {
        mx_char_style s = *word->get_style();
        word->get_end_style(s);

        if (s != *new_word->get_style()) {
            mx_char_style* ns = new mx_char_style(s);
            styles.append(ns);
            new_word->set_style(ns);
        }
    }

    br = new mx_break_word(mx_long_word_break_e);
    br->set_style(new_word->get_style());
    br->x = word->x + word->width();

    words.insert(err, br, line_index, word_index + 1);
    MX_ERROR_CHECK(err);

    words.insert(err, new_word, line_index, word_index + 2);
    MX_ERROR_CHECK(err);

    num_items = words.get_num_items(err, line_index);
    MX_ERROR_CHECK(err);

    // move all the words but the first part of the long word and the break
    // word onto the next line
    words.move_items_out_end(err, line_index, num_items - word_index - 2);
    MX_ERROR_CHECK(err);
abort:;
}

bool mx_paragraph::left_justify_underflow(int& err, int line_index, float x)
{
    mx_word* w;
    mx_rtti_class_e word_type;

    if (line_index >= (words.get_num_rows() - 1)) {
        return FALSE;
    }

    // get the next item and check its width
    w = (mx_word*)words.item(err, line_index + 1, 0);
    MX_ERROR_CHECK(err);

    word_type = w->rtti_class();

    switch (word_type) {
    case mx_space_word_class_e: {
        int word_index = words.get_num_items(err, line_index) - 1;
        MX_ERROR_CHECK(err);

        // move the item up
        words.move_items_in_end(err, line_index, 1);
        MX_ERROR_CHECK(err);

        w->x = x;
        x += left_justify_space_width(err, (mx_space_word*)w, line_index, word_index);
        MX_ERROR_CHECK(err);

        left_justify_underflow(err, line_index, x);
        MX_ERROR_CHECK(err);

        return TRUE;
    }
    case mx_break_word_class_e: {
        // move the item up
        words.move_items_in_end(err, line_index, 1);
        MX_ERROR_CHECK(err);

        w->x = x;

        if (((mx_break_word*)w)->type() == mx_long_word_break_e) {
            // if this is a long word break then maybe we need to do a rejoin
            // and split
            bool b;
            int word_index = this->num_words(err, line_index) - 1;
            MX_ERROR_CHECK(err);

            rejoin_and_split_long_word(err, line_index, word_index, b);
            MX_ERROR_CHECK(err);
        }
        return TRUE;
    }
    case mx_complex_word_class_e:
    case mx_simple_word_class_e: {
        float width = w->width();

        if ((width + x) > format_width) {
            return FALSE;
        } else {
            (void)words.get_num_items(err, line_index + 1);
            MX_ERROR_CHECK(err);

            // move the item up
            words.move_items_in_end(err, line_index, 1);
            MX_ERROR_CHECK(err);

            w->x = x;
            x += width;
            ;

            left_justify_underflow(err, line_index, x);
            MX_ERROR_CHECK(err);

            return TRUE;
        }
    }
    default:
        // shouldn't be here
        MX_ERROR_THROW(err, MX_ERROR_UNDEFINED);
    }
abort:
    return FALSE;
}

float mx_paragraph::left_justify_tab_width(int& err,
    float x,
    mx_space_word* word,
    int item_index,
    int line_index,
    int word_index)
{
    mx_tabstop tab;

    float actual_x = x + left_indent_to_text();

    get_style_to_use().get_ruler()->get_right_tab(err, actual_x, tab);
    MX_ERROR_CHECK(err);

    tab.position -= left_indent_to_text();

    // Are there any more tabs in the word? If there are then treat this tab as
    // an automatic tab.
    if (item_index < word->get_last_tab_index()) {
        tab.type = mx_automatic;
    }

    switch (tab.type) {
    default:
    case mx_automatic:
    case mx_left:
        x = tab.position - x;
        break;
    case mx_right:
        // if the next word is small enough to fit lined up to the
        // left of this tab, do it. Otherwise, ignore the tab.
        space_to_right_tabify(err, x, tab.position, item_index,
            line_index, word_index);
        MX_ERROR_CHECK(err);
        break;
    case mx_centre:
        space_to_centre_tabify(err, x, tab.position, item_index,
            line_index, word_index);
        MX_ERROR_CHECK(err);
        break;
    case mx_centre_on_dp:
        space_to_centre_dp_tabify(err, x, tab.position, item_index,
            line_index, word_index);
        MX_ERROR_CHECK(err);
        break;
    }

    return x;
abort:;
    return 0.0;
}

float mx_paragraph::left_justify_space_width(
    int& err,
    mx_space_word* word,
    int line_index,
    int word_index)

{
    // several cases to deal with here
    // 1. spaces
    // 2. tabs
    // 3. spaces and tabs

    float x = word->x;
    int n = word->get_num_items();
    int i;

    mx_char_style style(*(word->get_style()));
    mx_char_style_mod* temp_mod;

    mx_space_word::mx_space_word_item_t t;

    for (i = 0; i < n; i++) {
        t = word->item_type(err, i);
        MX_ERROR_CHECK(err);

        word->item_pos(err, i, x);
        MX_ERROR_CHECK(err);

        switch (t) {
        case mx_space_word::mx_space_word_space_e:
            x += style.width(' ');
            break;
        case mx_space_word::mx_space_word_tab_e:
            x += left_justify_tab_width(err, x, word, i, line_index, word_index);
            MX_ERROR_CHECK(err);
            break;
        case mx_space_word::mx_space_word_mod_e:
            temp_mod = word->get(err, i);
            MX_ERROR_CHECK(err);
            style += *temp_mod;
            break;
        default:
        case mx_space_word::mx_space_word_end_e:
            break;
        }
    }
    word->item_pos(err, n, x);
    MX_ERROR_CHECK(err);

    return x - word->x;

abort:;
    return 0.0;
}

// rejoins a long word and the reformats to split it up again
// word_index references the long_word_break word
void mx_paragraph::rejoin_and_split_long_word(
    int& err,
    int line_index,
    int word_index,
    bool& overflow)
{
    float x;

    // previous and next words make up part (or maybe whole) of a long word
    mx_word *first, *second;
    mx_break_word* break_w;
    int num_items;

    num_items = words.get_num_items(err, line_index);
    MX_ERROR_CHECK(err);

    if (word_index == 0) {
        // we've just got a long word break on the line on its own, so delete
        // it and reformat
        delete_word(err, line_index, word_index);
        MX_ERROR_CHECK(err);

        num_items = words.get_num_items(err, line_index);
        MX_ERROR_CHECK(err);

        if (num_items == 0) {
            mx_list* r = words.delete_row(err, line_index);
            MX_ERROR_CHECK(err);
            delete r;
        }

        reformat_line_left_justify(err, line_index, word_index, overflow);
        MX_ERROR_CHECK(err);

        return;
    }

    if (word_index != num_items - 1) {
        // the long_word_break is not the last thing on the line so shift the
        // words after it onto the next line

        words.move_items_out_end(err, line_index, num_items - word_index - 1);
        MX_ERROR_CHECK(err);
    }

    // move any style mods that are in the long word break into the next word.
    fix_trailing_style_mods(err,
        mx_para_pos(line_index, word_index, 0),
        mx_para_pos(line_index + 1, 0, 0));
    MX_ERROR_CHECK(err);

    first = (mx_word*)words.item(err, line_index, word_index - 1);
    MX_ERROR_CHECK(err);

    break_w = (mx_break_word*)words.item(err, line_index, word_index);
    MX_ERROR_CHECK(err);
    MX_ERROR_ASSERT(err, break_w->is_a(mx_break_word_class_e));

    second = (mx_word*)words.item(err, line_index + 1, 0);
    MX_ERROR_CHECK(err);

    x = first->x;
    x += first->width();

    if (this->get_type() != mx_paragraph_whole_e && second->is_a(mx_break_word_class_e) && ((mx_break_word*)second)->type() == mx_paragraph_break_e) {
        // deal explicitly with the case where this long word break is at the
        // end of a paragraph split across pages

        left_justify_underflow(err, line_index, x);
        MX_ERROR_CHECK(err);
    } else if (!first->is_printable() || !second->is_printable()) {
        // if the first or second chunks of the long word are not printable
        // words then part of the long word has been deleted, so remove the
        // break word and reformat

        delete_word(err, line_index, word_index);
        MX_ERROR_CHECK(err);

        reformat_line_left_justify(err, line_index, 0, overflow);
        MX_ERROR_CHECK(err);
    } else if (x > format_width && first->get_num_visible_items() > 1) {
        // the first part of the break word needs splitting

        overflow = TRUE;

        split_long_word(err, first, line_index, word_index - 1);
        MX_ERROR_CHECK(err);
    } else if (x + second->width() < format_width || word_index > 1) {
        // in this case we can rejoin this bit of broken word with the next bit

        delete_word(err, line_index, word_index);
        MX_ERROR_CHECK(err);

        join_words(err, mx_para_pos(line_index, word_index - 1, 0),
            mx_para_pos(line_index + 1, 0, 0));
        MX_ERROR_CHECK(err);

        // then reformat the line again to maybe bring stuff from the next line
        // back onto this one.

        first = get_word(err, line_index, word_index - 1);
        MX_ERROR_CHECK(err);

        x = first->x + first->width();

        reformat_line_left_justify(err, line_index, 0, overflow);
        MX_ERROR_CHECK(err);

        overflow = TRUE;
    } else {
        // in this case we have got a bit of long word that fits onto the line
        // and we need to test whether we can flow any characters to/from the
        // first bit of word into/outof the second bit of word

        int i = 1, num_vis = second->get_num_visible_items();

        // find the number of characters which cause the word to overflow
        while (x + second->width(i) < format_width && i < num_vis) {
            i++;
        }

        // use 1 less than the overflowing number of characters
        i--;

        if (i == 0) {
            overflow = FALSE;
        } else {
            overflow = TRUE;

            first = first->move_first_in_end(err, i, second);
            MX_ERROR_CHECK(err);

            words.overwrite(err, first, line_index, word_index - 1);
            MX_ERROR_CHECK(err);

            break_w->x = first->x + first->width();

            // now check that the styles are OK

            {
                mx_char_style s = *first->get_style();
                first->get_end_style(s);

                if (s != *second->get_style()) {
                    mx_char_style* ns = new mx_char_style(s);
                    styles.append(ns);
                    break_w->set_style(ns);
                    second->set_style(ns);
                }
            }
        }
    }
abort:;
}

void mx_paragraph::reformat_line_left_justify_break(
    int& err,
    mx_break_word* word,
    int line_index,
    int word_index,
    bool& overflow)
{
    // is this a long word ?
    if (word->type() == mx_long_word_break_e) {
        // need to join the word back together - rest is on next line
        // good grief this is complicated. There must be an easier way
        // to do all this.
        rejoin_and_split_long_word(err, line_index, word_index, overflow);
        MX_ERROR_CHECK(err);
    } else {
        // if the break is the last thing in the line, that is fine.
        // if not, the rest of the line must be shoved down into the
        // next line
        int n = words.get_num_items(err, line_index);
        MX_ERROR_CHECK(err);

        if (word_index < (n - 1)) {
            overflow = TRUE;
            words.move_items_out_end(err, line_index, n - word_index - 1);
            MX_ERROR_CHECK(err);
        }
    }

abort:;
}

float mx_paragraph::get_line_start_pos(
    int& err,
    int line_index,
    int word_index)
{
    mx_word* word;

    // find the x position of the first word. Ignore paragraph left indent for
    // this purpose as the painter is the thing that deals with drawing in
    // the right place. All we are worried about here is the width.
    if (word_index == 0) {
        // can't ignore the first line indent though
        if (line_index == 0) {
            return get_style_to_use().get_ruler()->first_line_indent;
        } else {
            return 0.0;
        }
    } else {
        word = (mx_word*)words.item(err, line_index, word_index);
        MX_ERROR_CHECK(err);

        return word->x;
    }
abort:;
    return 0.0;
}

float mx_paragraph::get_left_border_width()
{
    mx_border_style* bs;

    bs = get_style_to_use().get_border_style();

    if (bs->left_style.line_type != mx_no_line) {
        return bs->left_style.width + bs->distance_from_contents;
    } else {
        return bs->distance_from_contents;
    }
}

void mx_paragraph::reformat_line_left_justify(
    int& err,
    int line_index,
    int word_index,
    bool& overflow)
{
    mx_list* row;
    mx_word* word;
    float border_width, x;
    bool b, moved = FALSE;

    overflow = FALSE;

    x = get_line_start_pos(err, line_index, word_index);
    MX_ERROR_CHECK(err);

    border_width = get_left_border_width();

    row = words.row(err, line_index);
    MX_ERROR_CHECK(err);

    while (word_index < row->get_num_items()) {
        word = (mx_word*)row->get(err, word_index);
        MX_ERROR_CHECK(err);

        if (!MX_FLOAT_EQ(word->x, x + border_width))
            moved = TRUE;

        word->x = x + border_width;

        switch (word->rtti_class()) {
        case mx_complex_word_class_e:
            ((mx_complex_word*)word)->set_page_number(reformat_page_number);
            // now drop through to simple word case
        case mx_simple_word_class_e:
            x += word->width();
            break;
        case mx_space_word_class_e:
            x += left_justify_space_width(
                err,
                (mx_space_word*)word,
                line_index,
                word_index);
            MX_ERROR_CHECK(err);
            break;
        case mx_break_word_class_e:
            reformat_line_left_justify_break(err, (mx_break_word*)word, line_index, word_index, overflow);
            MX_ERROR_CHECK(err);
            return;
        default:
            break;
        }

        if (x > format_width) {
            if (word_index == 0 && !word->is_a(mx_space_word_class_e)) {
                // long word - must split it up, but lets first make sure there
                // is something to split

                if (word->get_num_visible_items() > 1) {
                    split_long_word(err, word, line_index, word_index);
                    MX_ERROR_CHECK(err);

                    overflow = moved = TRUE;
                    goto check_underflow;
                } else {
                    // drop back into loop and format the next word as if it
                    // had fit onto the line
                }
            } else {
                reformat_line_left_justify_overflow(err, word, line_index, word_index);
                MX_ERROR_CHECK(err);

                overflow = moved = TRUE;
                goto check_underflow;
            }
        }

        word_index++;
    }
check_underflow:
    // if the first thing on the next line is narrow enough to fit on the
    // end of this, there is an underflow
    b = left_justify_underflow(err, line_index, x);
    MX_ERROR_CHECK(err);

    if (b) {
        overflow = moved = TRUE;
    }
abort:;
}

void mx_paragraph::reformat_line_justify(int& err, int line_index,
    int word_index, bool& overflow,
    float gap_adjust)
{
    mx_word* last_word = NULL;
    int i, num_words_on_line = 0;
    float gap_width = 0.0;
    mx_list* row = NULL;

    // reformat this line for left justification from the beginning of the
    // line, and then shift all the words across by the width of the gap at the
    // right hand edge of the line.

    reformat_line_left_justify(err, line_index, 0, overflow);
    MX_ERROR_CHECK(err);

    // find the width of the gap

    row = words.row(err, line_index);
    MX_ERROR_CHECK(err);

    num_words_on_line = row->get_num_items();

    last_word = (mx_word*)row->get(err, num_words_on_line - 1);
    MX_ERROR_CHECK(err);

    gap_width = gap_adjust * (this->format_width - last_word->x - last_word->width());

    if (gap_width > 0.0) {
        for (i = 0; i < num_words_on_line; i++) {
            mx_word* w = (mx_word*)row->get(err, i);
            MX_ERROR_CHECK(err);
            w->move_x(gap_width);
        }
    }

    overflow = TRUE;
abort:;
}

void mx_paragraph::reformat_line_right_justify(
    int& err,
    int line_index,
    int word_index,
    bool& overflow)
{
    reformat_line_justify(err, line_index, word_index, overflow, 1);
}

void mx_paragraph::reformat_line_centre_justify(
    int& err,
    int line_index,
    int word_index,
    bool& overflow)
{
    reformat_line_justify(err, line_index, word_index, overflow, 0.5);
}

static int find_last_tab_word_index(int& err, mx_list* word_list)
{
    int tab_index = -1, i, num_words = word_list->get_num_items();
    mx_word* w;

    for (i = 0; i < num_words; i++) {
        w = (mx_word*)word_list->get(err, i);
        MX_ERROR_CHECK(err);

        if (w->is_a(mx_space_word_class_e)) {
            if (((mx_space_word*)w)->get_first_tab_index() != -1) {
                tab_index = i;
            }
        }
    }
abort:
    return tab_index;
}

// returns the index at which to start moving words in a fully justified line
// if there are tabs in the line
static int find_full_justify_start_index_tabs(int& err, mx_list* word_list,
    int tab_index, mx_ruler* ruler)
{
    int num_words = word_list->get_num_items();
    int justify_index = -1;

    if (num_words - tab_index < 5) {
        // In this case there are 1 or less words after the tab so we do not
        // need to try to justify the line
        justify_index = -1;
    } else {
        // Otherwise the behaviour is dependant on the type of tab we have. For
        // left and automatic tabs we justify from the next word, for other
        // types we do not justify
        int word_tab_index;
        float tab_pos;
        mx_tabstop tab;

        // get the last tab in the last tab space word
        mx_space_word* sp_w = (mx_space_word*)word_list->get(err, tab_index);
        MX_ERROR_CHECK(err);
        MX_ERROR_ASSERT(err, sp_w->is_a(mx_space_word_class_e));

        // now find the tab on the ruler to see what sort it is
        word_tab_index = sp_w->get_last_tab_index();

        // use the calculated width because the line has been reformatted at
        // this point
        tab_pos = sp_w->x + sp_w->calculated_width(word_tab_index);

        ruler->get_right_tab(err, tab_pos, tab);

        switch (tab.type) {
        case mx_automatic:
        case mx_left:
            justify_index = tab_index + 3;
            break;
        case mx_right:
        case mx_centre:
        case mx_centre_on_dp:
            justify_index = -1;
            break;
        default:
            // should not be here
            MX_ERROR_THROW(err, MX_ERROR_UNDEFINED);
            break;
        }
    }

abort:
    return justify_index;
}

// returns the word index at which to start moving words assuming there are no
// tabs on this line
static int find_full_justify_start_index_no_tabs(int& err, mx_list* word_list)
{
    mx_word* w = (mx_word*)word_list->get(err, 0);
    MX_ERROR_CHECK(err);

    if (w->is_a(mx_space_word_class_e)) {
        return 3;
    } else {
        return 2;
    }
abort:
    return -1;
}

// returns the index at which to begin fully justifying a line of text. That
// is, the index of the first word on the line which we want to move. If the
// line should not be fully justified (for example, because there is a right
// tab at the end of the line, or because this is the last line in a paragraph
// then the routine returns -1
static int find_full_justify_index(int& err, mx_list* word_list,
    mx_ruler* ruler,
    mx_paragraph::mx_paragraph_type_t type)
{
    int tab_index = -1;
    mx_break_word* last_word;

    // is the last word a break word?
    last_word = (mx_break_word*)word_list->last();
    if (last_word->is_a(mx_break_word_class_e)) {
        if (last_word->type() == mx_paragraph_break_e && (type == mx_paragraph::mx_paragraph_end_e || type == mx_paragraph::mx_paragraph_whole_e)) {
            // if its a paragraph break, and this paragraph is not a split one
            // then we do not want to justify this line
            return -1;
        }
    }

    // find the last word on the line which has a tab
    tab_index = find_last_tab_word_index(err, word_list);
    MX_ERROR_CHECK(err);

    if (tab_index == -1) {
        return find_full_justify_start_index_no_tabs(err, word_list);
    } else {
        return find_full_justify_start_index_tabs(err, word_list, tab_index,
            ruler);
    }

abort:
    return -1;
}

void mx_paragraph::reformat_line_full_justify(
    int& err,
    int line_index,
    int word_index,
    bool& overflow)
{
    mx_word* last_word = NULL;
    int i, justify_index, num_words_on_line = 0;
    float gap_width = 0.0;
    mx_list* row = NULL;

    // reformat this line for left justification from the beginning of the
    // line, and then shift all the words across various amounts to extend the
    // spaces. One problem is that this should be done only from the last tab
    // in the line (if there are any), and in addition only if the tab is a
    // left or automatic tab

    reformat_line_left_justify(err, line_index, 0, overflow);
    MX_ERROR_CHECK(err);

    // find the width of the gap

    row = words.row(err, line_index);
    MX_ERROR_CHECK(err);

    num_words_on_line = row->get_num_items();

    last_word = (mx_word*)row->get(err, num_words_on_line - 1);
    MX_ERROR_CHECK(err);

    gap_width = this->format_width - last_word->x - last_word->width();

    if (gap_width > 0.0) {
        float space_per_word = 0.0;
        int num_words_to_move = 0;
        int move_multiplier;

        // find which word index to justify from

        justify_index = find_full_justify_index(err, row,
            get_style_to_use().get_ruler(),
            get_type());
        MX_ERROR_CHECK(err);

        if (justify_index == -1) {
            return;
        }

        num_words_to_move = get_num_non_space_words(err, line_index, justify_index);
        MX_ERROR_CHECK(err);

        if (num_words_to_move < 1) {
            return;
        }

        space_per_word = gap_width / (float)num_words_to_move;
        move_multiplier = 0;

        for (i = justify_index; i < num_words_on_line; i++) {
            mx_word* w = (mx_word*)row->get(err, i);
            MX_ERROR_CHECK(err);

            if (w->is_printable())
                move_multiplier++;

            w->move_x(space_per_word * (float)move_multiplier);
        }
    }
abort:;
}

void mx_paragraph::reformat(
    int& err,
    mx_para_pos& start,
    mx_para_pos& end,
    bool& height_changed)
{
    int line = start.line_index, word_index;
    bool overflow = TRUE, moved = FALSE;
    bool b, format_whole_para;
    float old_height = height;
    std::vector<float> saved_word_positions;

    reformat_lower = -10000000;
    reformat_upper = 10000000;

    format_whole_para = start.is_start() && end.is_end(err, this);
    MX_ERROR_CHECK(err);

    recalculate_format_width();
    reformat_styles(err);
    MX_ERROR_CHECK(err);

    // always start formatting from the line before
    if (line > 0) {
        line--;
    }

    // stop reformatting as soon as a line doesn't overflow and we are
    // outside of the reformat line range. Note that reformatting a line
    // may move lines below but doesn't necessarily reformat them
    while (line < words.get_num_rows() && (format_whole_para || overflow || (line <= end.line_index))) {
        // for the first line in the range, start at the given word. For the last
        // ignore the end word index - the extra code isn't worth the trouble.
        // word_index = (line == start.line_index) ? start.word_index : 0;
        word_index = 0;

        // save the x positions to see if the words move after reformatting
        save_word_positions(err, line, saved_word_positions);
        MX_ERROR_CHECK(err);

        // first off, figure the x positions
        switch (get_style_to_use().justification) {
        case mx_left_aligned:
            reformat_line_left_justify(err, line, word_index, overflow);
            break;
        case mx_right_aligned:
            reformat_line_right_justify(err, line, word_index, overflow);
            break;
        case mx_justified:
            reformat_line_full_justify(err, line, word_index, overflow);
            break;
        case mx_centred:
            reformat_line_centre_justify(err, line, word_index, overflow);
            break;
        }
        MX_ERROR_CHECK(err);

        b = word_positions_changed(err, line, saved_word_positions);
        MX_ERROR_CHECK(err);

        if (b) {
            overflow = TRUE;
            set_reformat_range(err, line);
            MX_ERROR_CHECK(err);
        }

        // onto the next line down
        line++;
    }

    // now we have some x-wise formatted lines, figure the y position for them
    if (reformat_upper <= reformat_lower) {
        // if we moved any words x wise, then only move them
        for (line = reformat_upper; line <= reformat_lower; line++) {
            figure_ypos_for_line(err, line, moved);
            MX_ERROR_CHECK(err);
        }
    } else {
        // otherwise move all the rows from where we started formatting to
        // where we stopped formatting.
        int n = line;
        for (line = start.line_index; line < n; line++) {
            figure_ypos_for_line(err, line, moved);
            MX_ERROR_CHECK(err);
        }
    }

    // only need to worry about repositioning the following lines if there are
    // any
    if (line < words.get_num_rows()) {
        // if the last line moved at all up or down, reposition the following lines
        // as they don't need to be reformatted in any other way
        if (moved) {
            adjust_line_ypositions(err, line);
            MX_ERROR_CHECK(err);
        }
    }

    // check for trailing paragraph break following a line break in a split
    // paragraph
    check_trailing_para_break(err);
    MX_ERROR_CHECK(err);

    recalculate_height(err);
    MX_ERROR_CHECK(err);

    mx_paragraph::height_changed = height_changed = !MX_FLOAT_EQ(old_height, height);

    MX_ERROR_CHECK(err);

    if (height_changed || reformat_lower > -10000000 || reformat_upper < 10000000) {
        set_mem_state(mx_in_mem_and_modified_e);
    }

abort:;
}

void mx_paragraph::adjust_line_ypositions(int& err, int line)
{
    float height_for_this_line;
    mx_word *word_from_prev, *word_from_this;
    bool moved;

    float new_ypos, old_ypos;
    float move_by;

    int n = words.get_num_rows();

    if (n == 0) {
        return;
    }

    height_for_this_line = height_for_line(err, line);
    MX_ERROR_CHECK(err);

    word_from_prev = (mx_word*)words.item(err, line - 1, 0);
    MX_ERROR_CHECK(err);

    word_from_this = (mx_word*)words.item(err, line, 0);
    MX_ERROR_CHECK(err);

    old_ypos = word_from_this->y;

    set_line_ypos(err, line, word_from_prev->y + height_for_this_line, moved);
    MX_ERROR_CHECK(err);

    new_ypos = word_from_this->y;

    move_by = new_ypos - old_ypos;

    line++;

    while (line < n) {
        word_from_this = (mx_word*)words.item(err, line, 0);
        MX_ERROR_CHECK(err);

        set_line_ypos(err, line, word_from_this->y + move_by, moved);
        MX_ERROR_CHECK(err);

        line++;
    }

abort:;
}

void mx_paragraph::recalculate_height(int& err)
{
    mx_word* w;
    mx_border_style* bs = get_style_to_use().get_border_style();

    float ascender, descender, spacing;

    int i = words.get_num_rows();

    if (i < 1) {
        height = 0;
        return;
    }

    w = (mx_word*)words.item(err, i - 1, 0);
    MX_ERROR_CHECK(err);

    height = w->y;

    metrics_for_line(err, i - 1, ascender, descender, spacing);
    MX_ERROR_CHECK(err);

    height -= descender;
    height += MX_POINTS_TO_MM(spacing);

    if (type == mx_paragraph_end_e || type == mx_paragraph_whole_e) {
        if (bs->bottom_style.line_type != mx_no_line) {
            height += bs->bottom_style.width;
        }
        height += get_style_to_use().space_after + bs->distance_from_contents;
    }

abort:;
}

void mx_paragraph::recalculate_format_width()
{
    mx_border_style* bs = get_style_to_use().get_border_style();

    format_width = width;

    if (bs->left_style.line_type != mx_no_line) {
        format_width -= bs->left_style.width;
    }

    if (bs->left_style.line_type != mx_no_line) {
        format_width -= bs->right_style.width;
    }

    format_width -= 2 * bs->distance_from_contents;
    format_width -= get_style_to_use().get_ruler()->left_indent;
    format_width -= get_style_to_use().get_ruler()->right_indent;
}

void mx_paragraph::reformat_styles(int& err)
{
    mx_char_style* default_para_cs = get_style_to_use().get_char_style();
    mx_char_style temp = *default_para_cs;
    mx_char_style* s = new mx_char_style(temp);
    mx_word* w;

    set_mem_state(mx_in_mem_and_modified_e);
    delete_styles();
    styles.append(s);

    mx_nlist_iterator iter(words);
    while (iter.more()) {
        w = (mx_word*)iter.data();

        MX_ERROR_ASSERT(err, w != NULL);
        w->set_style(s);
        w->get_end_style(temp);

        if (temp != *s) {
            s = new mx_char_style(temp);
            styles.append(s);
        }
    }

    // now get the last word before the paragraph break and remove the last
    // style mod if there is one, because its a bit of a menace
    {
        mx_para_pos pp;

        pp.moveto_end(err, this);
        MX_ERROR_CHECK(err);

        pp.left(err, this);
        if (err != MX_ERROR_OK) {
            MX_ERROR_CLEAR(err);
        } else {
            mx_complex_word dummy;
            mx_word* w = get_word(err, pp);
            MX_ERROR_CHECK(err);

            w->move_style_mod_out_end(err, &dummy);
            MX_ERROR_CHECK(err);
        }
    }
abort:;
}

// this should be called just before any base style change, to fix all the
// revert_to_default flags in the style mods in the words
void mx_paragraph::fix_styles_for_base_style_change(int& err, mx_char_style& new_base_cs)
{
    mx_paragraph_style& stu = get_style_to_use();
    mx_char_style& old_base_cs = *stu.get_char_style();

    if (new_base_cs != old_base_cs) {
        mx_word *word, *prev_word = NULL;
        mx_char_style *word_style, *prev_word_style;
        mx_nlist_iterator iter(words);

        word_style = (mx_char_style*)styles.get(err, 0);
        MX_ERROR_CHECK(err);

        MX_ERROR_ASSERT(err, *word_style == old_base_cs);
        *word_style = new_base_cs;

        while (iter.more()) {
            word = (mx_word*)iter.data();

            word->replace_base_style(err, new_base_cs);
            MX_ERROR_CHECK(err);

            if (prev_word != NULL) {
                word_style = word->get_style();
                prev_word_style = prev_word->get_style();

                if (word_style != prev_word_style) {
                    *word_style = *prev_word_style;
                    prev_word->get_end_style(*word_style);
                }
            }
            prev_word = word;
        }
    }
abort:;
}

void mx_paragraph::check_trailing_para_break(int& err)
{
    mx_word* w1;
    mx_break_word *bw1, *bw2;
    int i, n;

    if (type == mx_paragraph::mx_paragraph_whole_e || type == mx_paragraph::mx_paragraph_end_e) {
        return;
    }

    n = words.get_num_rows();

    if (n > 1) {
        i = words.get_num_items(err, n - 2);
        MX_ERROR_CHECK(err);

        w1 = (mx_word*)words.item(err, n - 2, i - 1);
        MX_ERROR_CHECK(err);

        if (w1->rtti_class() == mx_break_word_class_e) {
            bw1 = (mx_break_word*)w1;

            i = words.get_num_items(err, n - 1);
            MX_ERROR_CHECK(err);

            if (i == 1) {
                bw2 = (mx_break_word*)words.item(err, n - 1, i - 1);
                MX_ERROR_CHECK(err);

                if (bw1->type() == mx_line_break_e && bw2->type() == mx_paragraph_break_e) {
                    words.move_items_in_end(err, n - 2, 1);
                    MX_ERROR_CHECK(err);

                    bw2->y = bw1->y;
                }
            }
        }
    }
abort:;
}

void mx_paragraph::save_word_positions(int& err, int line, std::vector<float>& list)
{
    mx_list* word_line = words.row(err, line);
    MX_ERROR_CHECK(err);
    MX_ERROR_ASSERT(err, sizeof(void*) >= sizeof(float));
    {
        mx_list_iterator i(*word_line);
        float tmp;
        mx_word* w;

        list.clear();

        while (i.more()) {
            w = (mx_word*)i.data();

            tmp = w->x;
            list.push_back(tmp);

            if (w->is_a(mx_space_word_class_e)) {
                tmp = ((mx_space_word*)w)->calculated_width(w->get_num_visible_items());
            } else {
                tmp = w->width();
            }
            list.push_back(tmp);
        }
    }
abort:;
}

bool mx_paragraph::word_positions_changed(int& err, int line, std::vector<float>& list)
{
    float tmpv;
    float tmpf;
    mx_word* w;
    mx_list* word_line = words.row(err, line);
    MX_ERROR_CHECK(err);

    if (list.size() != 2 * (size_t)word_line->get_num_items()) {
        return TRUE;
    }

    for (int i = 0; i < word_line->get_num_items(); i++) {
        w = (mx_word*)word_line->get(err, i);
        MX_ERROR_CHECK(err);

        tmpv = list[i * 2];
        MX_ERROR_CHECK(err);

        if (!MX_FLOAT_EQ(w->x, tmpv))
            return TRUE;

        tmpv = list[i * 2 + 1];
        MX_ERROR_CHECK(err);

        if (w->is_a(mx_space_word_class_e)) {
            tmpf = ((mx_space_word*)w)->calculated_width(w->get_num_visible_items());
        } else {
            tmpf = w->width();
        }
        if (!MX_FLOAT_EQ(tmpf, tmpv))
            return TRUE;
    }
    return FALSE;
abort:
    return TRUE;
}

void mx_paragraph::set_reformat_range(int& err, int line_index)
{
    if (line_index < reformat_upper) {
        reformat_upper = line_index;
    }

    if (line_index > reformat_lower) {
        reformat_lower = line_index;
    }
}
