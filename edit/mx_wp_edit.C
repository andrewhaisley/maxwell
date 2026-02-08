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
 * MODULE/CLASS :  mx_wp_editor
 *
 * AUTHOR : Andrew Haisley
 *
 *
 *
 * DESCRIPTION:
 *
 *  routines for the wp edit menu
 *
 *
 *
 *
 */

#include "mx_wp_editor.h"
#include <mx_cw_iter.h>
#include <mx_document.h>
#include <mx_goto_d.h>
#include <mx_new_d.h>
#include <mx_open_d.h>
#include <mx_para_pos.h>
#include <mx_replace_d.h>
#include <mx_undo.h>
#include <mx_window.h>
#include <mx_word_iter.h>
#include <mx_wp_menubar.h>
#include <mx_wp_ob_buff.h>
#include <mx_wp_toolbar.h>
#include <mx_xframe.h>
#include <mx_yes_no_d.h>

void mx_wp_editor::edit_goto_page(int& err)
{
    mx_document* doc = get_document();
    mx_goto_d* d;
    int sheet, n;
    mx_doc_coord_t f;

    frame->getTopLeftFrame(err, f);
    MX_ERROR_CHECK(err);

    d = dialog_man.get_goto_d();
    if (d->run() == yes_e) {
        sheet = d->page_num - 1;

        n = doc->get_num_sheets(err);
        MX_ERROR_CHECK(err);

        if (sheet > n) {
            sheet = n;
        }
        f.p.y = 0;
        f.sheet_number = sheet;

        frame->setTopLeftFrame(err, f);
        MX_ERROR_CHECK(err);
    }
abort:;
}

void mx_wp_editor::edit_goto_start(int& err)
{
    mx_doc_coord_t f;
    (void)get_document();

    frame->getTopLeftFrame(err, f);
    MX_ERROR_CHECK(err);

    f.p.y = 0;
    f.sheet_number = 0;

    frame->setTopLeftFrame(err, f);
    MX_ERROR_CHECK(err);

abort:;
}

void mx_wp_editor::edit_goto_end(int& err)
{
    mx_document* doc = get_document();
    int n;

    mx_doc_coord_t f;

    frame->getTopLeftFrame(err, f);
    MX_ERROR_CHECK(err);

    n = doc->get_num_sheets(err);
    MX_ERROR_CHECK(err);

    f.p.y = 0;
    f.sheet_number = n - 1;

    frame->setTopLeftFrame(err, f);
    MX_ERROR_CHECK(err);

abort:;
}

void mx_wp_editor::edit_copy(int& err)
{
    cursor.copy(err);
    MX_ERROR_CHECK(err);

    update_all_sensitivities(err);
    MX_ERROR_CHECK(err);
abort:;
}

void mx_wp_editor::edit_cut(int& err)
{
    if (cursor.selection()) {
        cursor.cut(err);
        MX_ERROR_CHECK(err);

        cleanup_after_cursor(err);
        MX_ERROR_CHECK(err);

        update_all_sensitivities(err);
        MX_ERROR_CHECK(err);
    }

abort:;
}

void mx_wp_editor::edit_paste(int& err)
{

    cursor.paste(err);
    if (err == MX_POSITION_RANGE_ERROR) {
        MX_ERROR_CLEAR(err);
        dialog_man.run_inform_d(inform_e, "cantPasteThatThere");
    } else {
        if (err == MX_TABLE_BUFFER_NOT_BIG_ENOUGH) {
            MX_ERROR_CLEAR(err);
            dialog_man.run_inform_d(inform_e, "tableNotBigEnough");
        } else {
            MX_ERROR_CHECK(err);

            cleanup_after_cursor(err);
            MX_ERROR_CHECK(err);
        }
    }

    update_all_sensitivities(err);
    MX_ERROR_CHECK(err);

    update_toolbar(err);
    MX_ERROR_CHECK(err);

abort:;
}

void mx_wp_editor::edit_select_all(int& err)
{
    cursor.select_all(err);
    MX_ERROR_CHECK(err);

    update_all_sensitivities(err);
    MX_ERROR_CHECK(err);

    update_toolbar(err);
    MX_ERROR_CHECK(err);
abort:;
}

inline static bool find_substring(const char* haystack, const char* needle,
    int& needle_index, bool match_case, bool search_backwards)
{
    if (search_backwards) {
        // reverse the search strings

        int i;
        bool found;
        int haystack_length = strlen(haystack);
        int needle_length = strlen(needle);
        char rev_haystack[haystack_length + 1];
        char rev_needle[needle_length + 1];

        for (i = 0; i < haystack_length; i++) {
            rev_haystack[haystack_length - i - 1] = haystack[i];
        }

        for (i = 0; i < needle_length; i++) {
            rev_needle[needle_length - i - 1] = needle[i];
        }

        rev_haystack[haystack_length] = 0;
        rev_needle[needle_length] = 0;

        found = mx_str_find(rev_haystack, rev_needle, needle_index,
            match_case);

        needle_index = haystack_length - needle_index - needle_length;
        return found;
    } else {
        return mx_str_find((char*)haystack, (char*)needle, needle_index,
            match_case);
    }
}

bool mx_wp_editor::find_words(
    int& err,
    mx_dialog* d,
    char* patterns[],
    int num_words,
    bool match_case,
    mx_wp_doc_pos& start,
    mx_wp_doc_pos& end,
    mx_wp_doc_pos& word_start,
    mx_wp_doc_pos& word_end)
{
    mx_word_iterator iter1(document, start, end);
    mx_word_iterator iter2(iter1);
    mx_word_iterator iter3(iter1);

    mx_word* w;
    bool b;
    bool reverse = (start > end);
    int i, n, first_index;

    mx_wp_doc_pos first_word_start;

    if (d != NULL)
        d->set_busy();

    while (iter1.more(err)) {
        MX_ERROR_CHECK(err);

        iter2 = iter1;
        first_index = -1;
        n = 0;

        while (n < num_words) {
            w = (mx_word*)iter2.data(err);
            MX_ERROR_CHECK(err);

            if (w->rtti_class() == mx_simple_word_class_e || w->rtti_class() == mx_complex_word_class_e) {
                bool matched_string = false;
                int32 pattern_index = reverse ? (num_words - 1 - n) : n;
                char* word_string = (char*)w->cstring();
                char* pattern_string = patterns[pattern_index];
                int32 word_length = w->get_num_visible_items();
                int32 pattern_length = strlen(pattern_string);

                // three cases - first, middle or last words
                // we must make sure that if this is the first word we get a
                // string match at the appropriate place in the word. The match
                // should be at the end for the first word, at the beginning
                // for the last word and a complete match for middle words.

                if (num_words == 1) {
                    // one word in pattern - we match it anywhere
                    matched_string = find_substring(word_string,
                        pattern_string, i,
                        match_case, reverse);
                } else if (pattern_index > 0 && pattern_index < (num_words - 1)) {
                    // either it is a middle word in the pattern, or we have a
                    // single word in the pattern- in either case we need an
                    // entire string match

                    matched_string = find_substring(word_string,
                        pattern_string, i,
                        match_case, FALSE);

                    matched_string = (matched_string && (i == 0 && word_length == pattern_length));
                } else if (pattern_index == 0) {
                    // first word - match last characters by doing a reverse
                    // string match
                    matched_string = find_substring(word_string,
                        pattern_string, i,
                        match_case, TRUE);
                    matched_string = (matched_string && ((i + pattern_length) == word_length));
                } else if (pattern_index == (num_words - 1)) {
                    // last word - match first characters by always doing a
                    // normal string match
                    matched_string = find_substring(word_string,
                        pattern_string, i,
                        match_case, FALSE);
                    matched_string = matched_string && (i == 0);
                }

                if (matched_string) {
                    if (first_index == -1) {
                        first_index = i;
                        first_word_start = iter2.word_start;
                    }

                    n++;

                    iter3 = iter2;

                    b = iter2.more(err);
                    MX_ERROR_CHECK(err);

                    if (!b) {
                        break;
                    }
                } else {
                    break;
                }
            } else {
                b = iter2.more(err);
                MX_ERROR_CHECK(err);

                if (b) {
                    continue;
                }
            }
        }

        if (n == num_words) {
            int last_pattern_index = reverse ? 0 : (num_words - 1);
            int first_pattern_index = num_words - 1 - last_pattern_index;
            int chars_matched_at_start = first_index, chars_matched_at_end = i;
            int index;

            word_start = first_word_start;
            word_end = iter3.word_start;

            if (reverse) {
                chars_matched_at_start += strlen(patterns[first_pattern_index]);
            } else {
                chars_matched_at_end += strlen(patterns[last_pattern_index]);
            }

            for (index = 0; index < chars_matched_at_start; index++) {
                word_start.move_right_ignoring_false_breaks(err, document);
                MX_ERROR_CHECK(err);
            }

            for (index = 0; index < chars_matched_at_end; index++) {
                word_end.move_right_ignoring_false_breaks(err, document);
                MX_ERROR_CHECK(err);
            }

            if ((reverse && word_start >= end && word_start <= start) || (!reverse && word_start >= start && word_start <= end)) {
                if (d != NULL)
                    d->clear_busy();
                return TRUE;
            }
        }
    }
    MX_ERROR_CHECK(err);
abort:
    if (d != NULL)
        d->clear_busy();
    return FALSE;
}

int mx_wp_editor::parse_pattern(int& err, char* pattern, char* patterns[])
{
    int res = 0, i = 0, j;

    while (TRUE) {
        while (pattern[i] == ' ' || pattern[i] == '\t') {
            i++;
        }

        if (pattern[i] == 0) {
            return res;
        } else {
            j = i + 1;
            while (pattern[j] != ' ' && pattern[j] != '\t' && pattern[j] != 0) {
                j++;
            }

            patterns[res++] = pattern + i;
            if (pattern[j] == 0) {
                return res;
            } else {
                pattern[j] = 0;
                i = j + 1;
            }
        }
    }
}

void mx_wp_editor::do_word_search(
    int& err,
    mx_dialog* d,
    mx_wp_doc_pos& pos,
    mx_search_dir_t dir,
    char* pattern,
    bool match_case,
    bool& found)
{
    mx_wp_doc_pos start, end;
    mx_wp_doc_pos word_start, word_end;
    char* patterns[MX_WP_EDITOR_MAX_FIND_WORDS];
    int num_words;

    num_words = parse_pattern(err, pattern, patterns);
    MX_ERROR_CHECK(err);

    start = pos;

    if (dir == mx_search_forward_e) {
        end.moveto_end(err, document);
        MX_ERROR_CHECK(err);
    } else {
        end.moveto_start(err, document);
        MX_ERROR_CHECK(err);
    }

    found = find_words(err, d, patterns, num_words, match_case, start, end,
        word_start, word_end);
    MX_ERROR_CHECK(err);

    if (!found) {
        int dialogue_result;

        if (dir == mx_search_forward_e) {
            dialogue_result = dialog_man.run_yes_no_d("startFromTop", FALSE,
                yes_e);
        } else {
            dialogue_result = dialog_man.run_yes_no_d("startFromBottom", FALSE,
                yes_e);
        }

        if (dialogue_result == yes_e) {
            if (dir == mx_search_forward_e) {
                start.moveto_start(err, document);
                MX_ERROR_CHECK(err);
            } else {
                start.moveto_end(err, document);
                MX_ERROR_CHECK(err);
            }
            end = pos;

            found = find_words(err, d, patterns, num_words, match_case, start,
                end, word_start, word_end);
            MX_ERROR_CHECK(err);
        }
    }

    if (found) {
        cursor.set_selection(err, word_start, word_end);
        MX_ERROR_CHECK(err);

        pos = word_end;
    } else {
        cursor.set_position(err, end);
        MX_ERROR_CHECK(err);
    }

    cursor.make_visible(err);
    MX_ERROR_CHECK(err);

    cursor.update_caret(err);
    MX_ERROR_CHECK(err);

abort:;
}

void mx_wp_editor::replace_all(int& err,
    char* pattern_to_replace,
    char* replacement_text,
    bool match_case)
{
    mx_wp_doc_pos start, end, word_start, word_end;
    mx_wp_object_buffer delete_buf(NULL);
    char* patterns[MX_WP_EDITOR_MAX_FIND_WORDS];
    int num_words;
    bool replaced_something = FALSE;

    num_words = parse_pattern(err, pattern_to_replace, patterns);
    MX_ERROR_CHECK(err);

    if (num_words > 0) {
        bool found;

        start.moveto_start(err, document);
        MX_ERROR_CHECK(err);

        end.moveto_end(err, document);
        MX_ERROR_CHECK(err);

        found = find_words(err, NULL, patterns, num_words, match_case,
            start, end, word_start, word_end);
        MX_ERROR_CHECK(err);

        if (found) {
            replaced_something = TRUE;

            cursor.set_position(err, start);
            MX_ERROR_CHECK(err);
        }

        while (found) {
            delete_buf.remove(err, *document, word_start, word_end);
            MX_ERROR_CHECK(err);

            document->insert_text(err, word_end, replacement_text);
            MX_ERROR_CHECK(err);

            start = word_end;

            found = find_words(err, NULL, patterns, num_words, match_case,
                start, end, word_start, word_end);
            MX_ERROR_CHECK(err);
        }
    }

    if (replaced_something) {
        start.moveto_start(err, document);
        MX_ERROR_CHECK(err);

        end.moveto_end(err, document);
        MX_ERROR_CHECK(err);

        cursor.reformat_and_restore_position(err, start, end, TRUE);
        MX_ERROR_CHECK(err);

        frame->refresh(err);
        MX_ERROR_CHECK(err);

        cursor.make_visible(err);
        MX_ERROR_CHECK(err);

        cursor.update_caret(err);
        MX_ERROR_CHECK(err);

        update_toolbar(err);
        MX_ERROR_CHECK(err);

        update_all_sensitivities(err);
        MX_ERROR_CHECK(err);
    }

    dialog_man.run_inform_d(inform_e, "allChecked");
abort:;
}

void mx_wp_editor::edit_find(int& err)
{
    bool found;
    mx_wp_doc_pos pos;
    mx_search_d* d = NULL;

    while (TRUE) {
        if (d == NULL) {
            d = dialog_man.get_search_d();
            d->centre();
            d->activate();
        }

        if (d->run_modal() == yes_e) {
            strcpy(last_search_pattern, d->search_pattern);
            last_search_dir = d->direction;
            last_search_case_sensitive = d->match_case;

            pos = (last_search_dir == mx_search_forward_e) ? *cursor.selection_end() : *cursor.selection_start();

            do_word_search(
                err,
                d,
                pos,
                d->direction,
                d->search_pattern,
                d->match_case,
                found);
            MX_ERROR_CHECK(err);

            if (!found) {
                dialog_man.run_inform_d(inform_e, "allChecked");
                pos = *cursor.get_position();
            }
        } else {
            d->deactivate();
            break;
        }
    }

    update_all_sensitivities(err);
    MX_ERROR_CHECK(err);

    update_toolbar(err);
    MX_ERROR_CHECK(err);

    return;

abort:;
    if (d != NULL) {
        d->deactivate();
    }
}

void mx_wp_editor::edit_find_and_replace(int& err)
{
    bool found, cut_something = FALSE, want_to_replace_text = FALSE;
    mx_wp_doc_pos pos;
    mx_replace_d* d = NULL;

    while (TRUE) {
        if (d == NULL) {
            d = dialog_man.get_replace_d();
            d->centre();
            d->activate();
        }

        if (d->run_modal() == yes_e) {
            strcpy(last_search_pattern, d->search_pattern);
            last_search_dir = d->direction;
            last_search_case_sensitive = d->match_case;

            if (d->replace_all) {
                d->set_busy();
                this->replace_all(err, d->search_pattern, d->replace_string,
                    d->match_case);
                d->clear_busy();
                MX_ERROR_CHECK(err);

                ur_buffer->mark_cant_undo(err);
                MX_ERROR_CHECK(err);

                d->deactivate();
                return;
            }

            if (want_to_replace_text) {
                replace_highlight_text(err, d->replace_string);
                MX_ERROR_CHECK(err);

                cut_something = TRUE;
            }

            pos = (last_search_dir == mx_search_forward_e) ? *cursor.selection_end() : *cursor.selection_start();

            do_word_search(
                err,
                d,
                pos,
                d->direction,
                d->search_pattern,
                d->match_case,
                found);
            MX_ERROR_CHECK(err);

            if (found) {
                want_to_replace_text = TRUE;
            } else {
                dialog_man.run_inform_d(inform_e, "allChecked");
                break;
            }
        } else {
            break;
        }
    }

    if (cut_something) {
        cursor.get_position()->moveto_start(err, document);
        MX_ERROR_CHECK(err);

        cursor.clear_selection(err);
        MX_ERROR_CHECK(err);

        cursor.make_visible(err);
        MX_ERROR_CHECK(err);

        cursor.update_caret(err);
        MX_ERROR_CHECK(err);

        ur_buffer->mark_cant_undo(err);
        MX_ERROR_CHECK(err);
    }

    update_all_sensitivities(err);
    MX_ERROR_CHECK(err);

    update_toolbar(err);
    MX_ERROR_CHECK(err);

abort:;
    if (d != NULL) {
        d->deactivate();
    }
}

void mx_wp_editor::update_edit_sensitivities(int& err)
{
    // can't goto page when editing header or footer
    if (current_edit_component == mx_wp_editor_body_e) {
        menu->set_visible(mx_edit_goto_e, mx_end_item_e);
        toolbar->set_visible(mx_edit_goto_e, mx_end_item_e);
    } else {
        menu->set_invisible(mx_edit_goto_e, mx_end_item_e);
        toolbar->set_invisible(mx_edit_goto_e, mx_end_item_e);
    }

    // if there is a selection, cut and copy should be sensitive
    if (cursor.selection()) {
        menu->set_visible(mx_edit_cut_e, mx_edit_copy_e, mx_end_item_e);
        toolbar->set_visible(mx_edit_cut_e, mx_edit_copy_e, mx_end_item_e);
    } else {
        menu->set_invisible(mx_edit_cut_e, mx_edit_copy_e, mx_end_item_e);
        toolbar->set_invisible(mx_edit_cut_e, mx_edit_copy_e, mx_end_item_e);
    }

    // if there is anything in the buffer, paste should be visible
    if (cursor.get_buffer()->is_empty()) {
        menu->set_item_visible(mx_edit_paste_e, FALSE);
        toolbar->set_item_visible(mx_edit_paste_e, FALSE);
    } else {
        // but, if the view is the header or footer and the buffer contains
        // table bits, you can't paste
        if ((current_edit_component != mx_wp_editor_body_e) && (cursor.get_buffer()->contains_cells() || cursor.get_buffer()->contains_tables())) {
            menu->set_item_visible(mx_edit_paste_e, FALSE);
            toolbar->set_item_visible(mx_edit_paste_e, FALSE);
        } else {
            menu->set_item_visible(mx_edit_paste_e, TRUE);
            toolbar->set_item_visible(mx_edit_paste_e, TRUE);
        }
    }

    toolbar->set_item_visible(
        mx_edit_find_again_e,
        last_search_pattern[0] != 0);
    menu->set_item_visible(
        mx_edit_find_again_e,
        last_search_pattern[0] != 0);
}

void mx_wp_editor::edit_find_again(int& err)
{
    bool found;
    char pattern[MAX_SEARCH_PATTERN + 1];

    mx_wp_doc_pos pos = (last_search_dir == mx_search_forward_e) ? *cursor.selection_end() : *cursor.selection_start();

    strcpy(pattern, last_search_pattern);

    do_word_search(
        err,
        NULL,
        pos,
        last_search_dir,
        pattern,
        last_search_case_sensitive,
        found);
    MX_ERROR_CHECK(err);

    if (!found) {
        dialog_man.run_inform_d(inform_e, "allChecked");
    }

abort:;
}

void mx_wp_editor::edit_undo(int& err)
{
    if (ur_buffer->anything_to_undo()) {
        mx_wp_doc_pos pos = *cursor.get_position();
        mx_ur_item_type_t undo_type;

        int n, old_num_sheets;

        cursor.clear_selection(err);
        MX_ERROR_CHECK(err);

        undo_type = ur_buffer->type_to_undo(err);
        MX_ERROR_CHECK(err);

        old_num_sheets = document->get_num_sheets(err);
        MX_ERROR_CHECK(err);

        ur_buffer->undo(err, document, &pos);
        MX_ERROR_CHECK(err);

        n = document->get_num_sheets(err);
        MX_ERROR_CHECK(err);

        if (undo_type == mx_undo_page_setup_e || undo_type == mx_undo_named_style_change_e || undo_type == mx_undo_cut_e || undo_type == mx_undo_paste_e) {
            (void)set_sheet_layout(err, document);
            MX_ERROR_CHECK(err);

            frame->refresh(err);
            MX_ERROR_CHECK(err);
        } else {
            if (n == old_num_sheets) {
                mx_sheet_size sheet_size;

                sheet_size = document->get_sheet_visible_size(err, pos.get_sheet(), current_view_mode);
                MX_ERROR_CHECK(err);

                layout->changeSheets(err, pos.get_sheet(), 1, &sheet_size);
                MX_ERROR_CHECK(err);

                frame->refresh(err);
                MX_ERROR_CHECK(err);
            } else {
                (void)set_sheet_layout(err, document);
                MX_ERROR_CHECK(err);

                frame->refresh(err);
                MX_ERROR_CHECK(err);
            }
        }

        cursor.set_position(err, pos);
        MX_ERROR_CHECK(err);

        cursor.make_visible(err);
        MX_ERROR_CHECK(err);

        update_toolbar(err);
        MX_ERROR_CHECK(err);

        update_undo(err);
        MX_ERROR_CHECK(err);
    }
    return;
abort:
    // on error, chances are that the undo is brokem, so
    // display the error and clear the buffer
    global_error_trace->print();
    MX_ERROR_CLEAR(err);
    ur_buffer->mark_cant_undo(err);
}

void mx_wp_editor::edit_redo(int& err)
{
    if (ur_buffer->anything_to_redo()) {
        mx_wp_doc_pos pos = *cursor.get_position();

        int n, old_num_sheets;

        cursor.clear_selection(err);
        MX_ERROR_CHECK(err);

        old_num_sheets = document->get_num_sheets(err);
        MX_ERROR_CHECK(err);

        ur_buffer->redo(err, document, &pos);
        MX_ERROR_CHECK(err);

        n = document->get_num_sheets(err);
        MX_ERROR_CHECK(err);

        if (n == old_num_sheets) {
            mx_sheet_size sheet_size;

            sheet_size = document->get_sheet_visible_size(err, pos.get_sheet(), current_view_mode);
            MX_ERROR_CHECK(err);

            layout->changeSheets(err, pos.get_sheet(), 1, &sheet_size);
            MX_ERROR_CHECK(err);

            frame->refresh(err);
            MX_ERROR_CHECK(err);
        } else {
            (void)set_sheet_layout(err, document);
            MX_ERROR_CHECK(err);

            frame->refresh(err);
            MX_ERROR_CHECK(err);
        }

        cursor.set_position(err, pos);
        MX_ERROR_CHECK(err);

        cursor.make_visible(err);
        MX_ERROR_CHECK(err);

        update_toolbar(err);
        MX_ERROR_CHECK(err);

        update_undo(err);
        MX_ERROR_CHECK(err);
    }
    return;
abort:
    // on error, chances are that the undo is brokem, so
    // display the error and clear the buffer
    global_error_trace->print();
    MX_ERROR_CLEAR(err);
    ur_buffer->mark_cant_undo(err);
}

void mx_wp_editor::update_undo(int& err)
{
    if (ur_buffer->anything_to_undo()) {
        menu->set_visible(mx_edit_undo_e, mx_end_item_e);
        toolbar->set_visible(mx_edit_undo_e, mx_end_item_e);
    } else {
        menu->set_invisible(mx_edit_undo_e, mx_end_item_e);
        toolbar->set_invisible(mx_edit_undo_e, mx_end_item_e);
    }

    if (ur_buffer->anything_to_redo()) {
        menu->set_visible(mx_edit_redo_e, mx_end_item_e);
        toolbar->set_visible(mx_edit_redo_e, mx_end_item_e);
    } else {
        menu->set_invisible(mx_edit_redo_e, mx_end_item_e);
        toolbar->set_invisible(mx_edit_redo_e, mx_end_item_e);
    }
}
