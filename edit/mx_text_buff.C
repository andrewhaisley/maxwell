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
 * MODULE/CLASS : mx_text_buffer
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

#include "mx_text_buff.h"
#include <mx.h>
#include <mx_document.h>
#include <mx_list.h>
#include <mx_list_iter.h>
#include <mx_paragraph.h>
#include <mx_rtti.h>
#include <mx_text_area.h>
#include <mx_txt_pos.h>
#include <mx_word.h>

enum mx_text_buffer_item_type_t {
    mx_tbi_word_e,
    mx_tbi_paragraph_e,
};

struct mx_text_buffer_item_t {
    mx_text_buffer_item_t(mx_text_buffer_item_type_t nt, void* nthing = NULL);
    mx_text_buffer_item_t(const mx_text_buffer_item_t& item);
    ~mx_text_buffer_item_t();

    void move_to_document(int& err, mx_wp_document* d);

    mx_text_buffer_item_type_t t;
    void* thing;
};

mx_text_buffer_item_t::mx_text_buffer_item_t(mx_text_buffer_item_type_t nt,
    void* nthing)
    : t(nt)
    , thing(nthing)
{
}

mx_text_buffer_item_t::mx_text_buffer_item_t(const mx_text_buffer_item_t& item)
    : t(item.t)
    , thing(item.thing)
{
    if (thing != NULL) {
        switch (t) {
        case mx_tbi_word_e:
            thing = ((mx_word*)thing)->duplicate();
            break;
        case mx_tbi_paragraph_e:
            thing = new mx_paragraph(*(mx_paragraph*)thing);
            break;
        default:
            break;
        }
    }
}

mx_text_buffer_item_t::~mx_text_buffer_item_t()
{
    if (thing != NULL) {
        switch (t) {
        case mx_tbi_word_e:
            delete (mx_word*)thing;
            break;
        case mx_tbi_paragraph_e:
            delete (mx_paragraph*)thing;
            break;
        default:
            break;
        }
    }
}

void mx_text_buffer_item_t::move_to_document(int& err, mx_wp_document* d)
{
    if (thing != NULL) {
        switch (t) {
        case mx_tbi_word_e:
            ((mx_word*)thing)->move_to_document(err, d);
            break;
        case mx_tbi_paragraph_e:
            ((mx_paragraph*)thing)->move_to_document(err, d);
            break;
        default:
            break;
        }
        MX_ERROR_CHECK(err);
    }
abort:;
}

mx_text_buffer::mx_text_buffer(mx_wp_document* d)
    : mx_buffer(d)
{
    contents = new mx_list;
}

mx_text_buffer::mx_text_buffer(const mx_text_buffer& tb)
    : mx_buffer(tb)
{
    mx_list_iterator iter(*tb.contents);
    contents = new mx_list;

    while (iter.more()) {
        mx_text_buffer_item_t* item = (mx_text_buffer_item_t*)iter.data();
        item = new mx_text_buffer_item_t(*item);
        contents->append(item);
    }
}

mx_text_buffer::~mx_text_buffer()
{
    clear_old_contents();
    delete contents;
}

void mx_text_buffer::cut_paragraph(int& err, mx_text_area& a, int index,
    mx_edit_op edit_op)
{
    mx_paragraph* para;
    mx_document* d = this->get_document();

    if (edit_op != mx_copy_e) {
        para = a.remove(err, index);
        MX_ERROR_CHECK(err);

        if (edit_op == mx_remove_e) {
            delete para;
            return;
        }
    } else {
        para = a.get(err, index);
        MX_ERROR_CHECK(err);

        para = new mx_paragraph(*para);
    }

    para->move_to_document(err, d);
    MX_ERROR_CHECK(err);

    contents->append(new mx_text_buffer_item_t(mx_tbi_paragraph_e, para));

abort:;
}

void mx_text_buffer::cut_word(int& err, mx_paragraph* para, mx_para_pos& pos,
    int& line, int& word, mx_edit_op edit_op)
{
    mx_document* d = this->get_document();
    mx_para_pos p1(line, word, 0);
    mx_word* w;

    if (edit_op == mx_copy_e) {
        w = para->get_word(err, line, word);
        MX_ERROR_CHECK(err);

        w = w->duplicate();
    } else {
        int num_lines_diff = para->num_lines();

        w = para->remove_word(err, line, word);
        MX_ERROR_CHECK(err);

        num_lines_diff -= para->num_lines();

        if (pos.line_index == line) {
            // if we're on the same line, then move the end position back a
            // word
            pos.word_index--;

            if (num_lines_diff != 0) {
                // if we removed all the words on this line, then move the
                // position to the end of the previous line. If we've deleted
                // all the words on the row of the same line as the end, then
                // we must have finished, so force the end condition
                pos.moveto_end(err, para, pos.line_index - 1);
                MX_ERROR_CHECK(err);

                line = pos.line_index;
                word = pos.word_index + 1;
            }
        } else if (num_lines_diff != 0) {
            // if we removed a line after the current line move the end line
            // index back
            pos.line_index--;
        }

        word--;

        if (edit_op == mx_remove_e) {
            delete w;
            return;
        }
    }

    w->move_to_document(err, d);
    MX_ERROR_CHECK(err);

    contents->append(new mx_text_buffer_item_t(mx_tbi_word_e, w));
abort:;
}

void mx_text_buffer::cut_from_word(
    int& err,
    mx_paragraph* para,
    int line,
    int word,
    int start_letter,
    int end_letter,
    mx_edit_op edit_op)
{
    mx_document* d = this->get_document();
    mx_word *w, *nw;
    mx_para_pos p1(line, word, start_letter);

    if (start_letter == end_letter) {
        return;
    }

    w = para->get_word(err, line, word);
    MX_ERROR_CHECK(err);

    if (edit_op == mx_copy_e)
        w = w->duplicate();

    nw = w->cut(err, start_letter, end_letter);
    MX_ERROR_CHECK(err);

    if (edit_op == mx_copy_e)
        delete w;
    if (edit_op == mx_remove_e) {
        delete nw;
        return;
    }

    nw->move_to_document(err, d);
    MX_ERROR_CHECK(err);

    contents->append(new mx_text_buffer_item_t(mx_tbi_word_e, nw));
abort:;
}

void mx_text_buffer::cut_from_paragraph(
    int& err,
    mx_paragraph* para,
    mx_para_pos p1,
    mx_para_pos p2,
    mx_edit_op edit_op)
{
    if (p1 == p2)
        return;
    int line = p1.line_index, word = p1.word_index;

    if (!p2.is_after_end()) {
        // if the selection is not past the end of the paragraph, then don't
        // cut/copy any break word that the second position might be on

        mx_word* w = para->get_word(err, p2);
        MX_ERROR_CHECK(err);

        if (w->is_a(mx_break_word_class_e)) {
            p2.left(err, para);
            MX_ERROR_CHECK(err);

            w = para->get_word(err, p2);
            MX_ERROR_CHECK(err);

            p2.letter_index = w->get_num_visible_items();
        }
    }

    while (TRUE) {
        if (line == p1.line_index && word == p1.word_index) {
            if (line == p2.line_index && word == p2.word_index) {
                // covers a portion of one word
                if (covers_whole_word(err, para, line, word, p1.letter_index, p2.letter_index)) {
                    cut_word(err, para, p2, line, word, edit_op);
                    MX_ERROR_CHECK(err);
                } else {
                    if (p2.letter_index > 0) {
                        cut_from_word(err, para, line, word, p1.letter_index,
                            p2.letter_index, edit_op);
                        MX_ERROR_CHECK(err);
                    }
                }
            } else {
                // covers part way through this word to the end of it
                if (p1.letter_index == 0) {
                    cut_word(err, para, p2, line, word, edit_op);
                    MX_ERROR_CHECK(err);
                } else {
                    cut_from_word(err, para, line, word, p1.letter_index, -1, edit_op);
                    MX_ERROR_CHECK(err);
                }
            }
        } else {
            if (line == p2.line_index && word == p2.word_index) {
                // covers from start to part way through this word
                if (covers_whole_word(err, para, line, word, 0, p2.letter_index)) {
                    cut_word(err, para, p2, line, word, edit_op);
                    MX_ERROR_CHECK(err);
                } else {
                    cut_from_word(err, para, line, word, 0, p2.letter_index, edit_op);
                    MX_ERROR_CHECK(err);
                }
            } else {
                // covers whole word
                cut_word(err, para, p2, line, word, edit_op);
                MX_ERROR_CHECK(err);
            }
        }

        if (line == p2.line_index && word == p2.word_index) {
            break;
        }

        word++;
        if (word >= para->num_words(err, line)) {
            line++;
            word = 0;
        }
    }

    if (edit_op != mx_copy_e) {
        // get rid of any blank lines
        para->delete_blank_lines(err, p1.line_index);
        MX_ERROR_CHECK(err);

        // make sure it still has a paragraph break at the end
        fix_break(err, para);
        MX_ERROR_CHECK(err);
    }

abort:;
}

void mx_text_buffer::cut(int& err, mx_text_area& a, mx_text_area_pos p1,
    mx_text_area_pos p2, mx_edit_op edit_op)
{
    // four types of things to cut here:
    //  1. whole paragraphs
    //  2. bits from the middle of paragraphs
    //  3. bits from the start to the middle of a paragraph
    //  4. bits from the middle to the end of a paragraph
    mx_paragraph* para;
    mx_para_pos pp1, pp2;
    int i;

    if (edit_op != mx_remove_e)
        clear_old_contents();

    p1.para_pos.set_inside();
    p2.para_pos.set_inside();

    i = p1.paragraph_index;
    while (i <= p2.paragraph_index) {
        para = a.get(err, i);
        MX_ERROR_CHECK(err);

        if (i == p1.paragraph_index && !p1.para_pos.is_start()) {
            if (i == p2.paragraph_index && !p2.is_after_end()) {
                // range is entirely within one paragraph
                cut_from_paragraph(err, para, p1.para_pos, p2.para_pos, edit_op);
                MX_ERROR_CHECK(err);
            } else {
                // range starts in the middle of this paragraph and ends in
                // a later paragraph
                pp1.moveto_end(err, para);
                MX_ERROR_CHECK(err);

                pp1.set_after_end();

                cut_from_paragraph(err, para, p1.para_pos, pp1, edit_op);
                MX_ERROR_CHECK(err);
            }
        } else {
            if (i == p2.paragraph_index && !p2.is_after_end()) {
                // range is to second paragraph position
                pp1.moveto_start();
                pp1.set_before_start();

                cut_from_paragraph(err, para, pp1, p2.para_pos, edit_op);
                MX_ERROR_CHECK(err);
            } else {
                // whole paragraph is covered
                cut_paragraph(err, a, i, edit_op);
                MX_ERROR_CHECK(err);

                if (edit_op != mx_copy_e) {
                    p2.paragraph_index--;
                    i--;
                }
            }
        }
        i++;
    }
abort:;
}

void mx_text_buffer::insert_word(int& err, mx_text_area& a, mx_word* w, mx_text_area_pos& p)
{
    mx_paragraph* para;
    mx_paragraph_style* ps;
    int letter, i;
    mx_document* d = a.get_document(err);
    MX_ERROR_CHECK(err);

    para = a.get(err, p.paragraph_index);
    MX_ERROR_CHECK(err);

    letter = w->get_num_visible_items();

    if (w->rtti_class() == mx_break_word_class_e) {
        switch (((mx_break_word*)w)->type()) {
        case mx_paragraph_break_e:
            para = para->split(err, p.para_pos);
            MX_ERROR_CHECK(err);

            p.paragraph_index++;

            a.insert(err, p.paragraph_index, para);
            MX_ERROR_CHECK(err);

            p.para_pos.moveto_start();

            delete w;
            return;
        case mx_line_break_e:
            // line breaks count as one letter
            letter++;
            break;
        case mx_long_word_break_e:
            // long word breaks have no visible items
            break;
        default:
            MX_ERROR_THROW(err, MX_ERROR_UNDEFINED);
        }
    }

    w->move_to_document(err, d);
    MX_ERROR_CHECK(err);

    ps = para->get_paragraph_style();

    w->replace_base_style(err, *ps->get_char_style());
    MX_ERROR_CHECK(err);

    mx_word::set_operations_to_left_of_mods();
    para->aka_insert_word(err, w, p.para_pos);
    MX_ERROR_CHECK(err);

    for (i = 0; i < letter; i++) {
        p.para_pos.right(err, para);
        MX_ERROR_CHECK(err);
    }

abort:;
}

void mx_text_buffer::insert_mod(int& err, mx_text_area& a, mx_char_style_mod* m, mx_text_area_pos& p)
{
    mx_paragraph* para = a.get(err, p.paragraph_index);
    MX_ERROR_CHECK(err);

    para->insert_mod(err, *m, p.para_pos);
    MX_ERROR_CHECK(err);

abort:;
}

void mx_text_buffer::insert(int& err, mx_text_area& a, mx_text_area_pos& p)
{
    mx_text_buffer_item_t* item;
    mx_list_iterator iter(*contents);
    mx_document* d = a.get_document(err);
    MX_ERROR_CHECK(err);

    while (iter.more()) {
        item = (mx_text_buffer_item_t*)iter.data();

        switch (item->t) {
        case mx_tbi_word_e: {
            mx_paragraph* para;
            mx_paragraph_style* ps;

            para = a.get(err, p.paragraph_index);
            MX_ERROR_CHECK(err);

            ps = para->get_base_paragraph_style();

            mx_word* w = ((mx_word*)item->thing)->duplicate();
            w->set_style(((mx_word*)item->thing)->get_style());

            w->replace_base_style(err, *ps->get_char_style());
            MX_ERROR_CHECK(err);

            insert_word(err, a, w, p);
            MX_ERROR_CHECK(err);
            break;
        }
        case mx_tbi_paragraph_e: {
            mx_paragraph* para = new mx_paragraph(*(mx_paragraph*)item->thing);

            para->move_to_document(err, d);
            MX_ERROR_CHECK(err);

            a.insert_adjust_position(err, para, p);
            MX_ERROR_CHECK(err);
            break;
        }
        default:
            MX_ERROR_THROW(err, MX_ERROR_UNDEFINED);
        }
    }

abort:;
}

bool mx_text_buffer::contains_words()
{
    mx_text_buffer_item_t* thing;
    mx_list_iterator iter(*contents);

    while (iter.more()) {
        thing = (mx_text_buffer_item_t*)iter.data();
        if (thing->t == mx_tbi_word_e)
            return TRUE;
    }
    return FALSE;
}

void mx_text_buffer::clear_old_contents()
{
    mx_list_iterator iter(*contents);

    while (iter.more())
        delete (mx_text_buffer_item_t*)iter.data();

    delete contents;
    contents = new mx_list;
}

bool mx_text_buffer::covers_whole_word(
    int& err,
    mx_paragraph* para,
    int line,
    int word,
    int start_letter,
    int end_letter)
{
    mx_word* w;

    if (start_letter != 0) {
        return FALSE;
    }

    w = para->get_word(err, line, word);
    MX_ERROR_CHECK(err);

    switch (w->rtti_class()) {
    case mx_simple_word_class_e:
    case mx_space_word_class_e:
    case mx_complex_word_class_e:
        return end_letter == w->get_num_visible_items();

    default:
    case mx_break_word_class_e:
    case mx_field_word_class_e:
        // can't have partial selections of these
        return TRUE;
    }

abort:
    return FALSE;
}

void mx_text_buffer::fix_break(int& err, mx_paragraph* para)
{
    mx_break_word* bw;
    mx_word* w;
    int nw;

    // make sure last word is still a paragraph break
    if (para->num_lines() > 0) {
        nw = para->num_words(err, para->num_lines() - 1);
        MX_ERROR_CHECK(err);

        w = para->get_word(err, para->num_lines() - 1, nw - 1);
        MX_ERROR_CHECK(err);

        if (w->rtti_class() == mx_break_word_class_e) {
            if (((mx_break_word*)w)->type() == mx_paragraph_break_e) {
                return;
            }
        }
    }

    bw = new mx_break_word(mx_paragraph_break_e);
    para->words.append_item(bw);

abort:;
}

void mx_text_buffer::move_to_document(int& err, mx_wp_document* d)
{
    mx_list_iterator iter(*contents);
    while (iter.more()) {
        ((mx_text_buffer_item_t*)iter.data())->move_to_document(err, d);
        MX_ERROR_CHECK(err);
    }
abort:;
}
