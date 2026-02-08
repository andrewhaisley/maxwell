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
 * MODULE/CLASS :  mx_wp_cursor
 *
 * AUTHOR : Andrew Haisley
 *
 *
 *
 * DESCRIPTION:
 *
 * Cursor for WP documents
 *
 *
 *
 */

#include "mx_aa.h"
#include "mx_wp_cursor.h"
#include <mx.h>
#include <mx_break_w.h>
#include <mx_cw_t.h>
#include <mx_event.h>
#include <mx_frame.h>
#include <mx_mod_iter.h>
#include <mx_op_pos.h>
#include <mx_paragraph.h>
#include <mx_rtti.h>
#include <mx_sc_device.h>
#include <mx_ta_pos.h>
#include <mx_txt_pos.h>
#include <mx_undo.h>
#include <mx_wp_area_iter.h>
#include <mx_wp_doc.h>
#include <mx_wp_lay.h>
#include <mx_wp_ob_buff.h>
#include <mx_wp_pos.h>
#include <mx_wp_style_iter.h>

void mx_wp_cursor::single_click(int& err, mx_button_event& event)
{
    mx_doc_coord_t cd;

    active_area->deactivate();

    // single press -> move the cursor position or extend selection if
    // shift pressed
    update_selection(err, event.getShift());
    MX_ERROR_CHECK(err);

    cd = event.getCoord();

    calculate_view_correction(err, cd.sheet_number);
    if (err == MX_DOCUMENT_SHEET_INDEX) {
        MX_ERROR_THROW(err, MX_POSITION_RANGE_ERROR);
    }
    MX_ERROR_CHECK(err);

    cd.p += view_correction;

    if (event.getShift() && is_selection) {
        mx_wp_doc_pos temp_pos = position;

        temp_pos.moveto(err, document, cd);
        MX_ERROR_CHECK(err);

        set_selection(
            err,
            temp_pos < start ? temp_pos : start,
            temp_pos > position ? temp_pos : position);
        MX_ERROR_CHECK(err);
    } else {
        position.moveto(err, document, cd);
        MX_ERROR_CHECK(err);
    }

    active_area->single_click(err, event);
    MX_ERROR_CHECK(err);

    // save the x_position on next up/down movement
    save_x_pos = TRUE;

    {
        mx_char_style* cs = this->char_style_at_position(err);
        MX_ERROR_CHECK(err);

        set_current_style(*cs);
    }

    fix_selection_positions(err);
    MX_ERROR_CHECK(err);

    update_caret(err);
    MX_ERROR_CLEAR(err);

abort:;
}

void mx_wp_cursor::double_click(int& err, mx_button_event& event)
{
    mx_doc_coord_t cd;

    if (active_area->is_active()) {
        // start an appropriate editor here
        return;
    } else {
        if (event.getShift() && is_selection) {
            mx_doc_coord_t cd;
            mx_wp_doc_pos temp_pos;
            mx_wp_doc_pos start_word, end_word;

            mx_para_pos* pp;
            mx_paragraph* p;

            cd = event.getCoord();

            calculate_view_correction(err, cd.sheet_number);
            if (err == MX_DOCUMENT_SHEET_INDEX) {
                MX_ERROR_THROW(err, MX_POSITION_RANGE_ERROR);
            }
            MX_ERROR_CHECK(err);

            cd.p += view_correction;

            temp_pos.moveto(err, document, cd);
            MX_ERROR_CHECK(err);

            start_word = temp_pos;
            end_word = temp_pos;

            pp = start_word.get_para_pos(err);
            MX_ERROR_CHECK(err);

            pp->moveto_start_word();

            pp = end_word.get_para_pos(err);
            MX_ERROR_CHECK(err);

            p = end_word.get_para(err, document);
            MX_ERROR_CHECK(err);

            pp->moveto_end_word(err, p);
            MX_ERROR_CHECK(err);

            if (start_word > start) {
                start_word = start;
            }

            if (end_word < position) {
                end_word = position;
            }

            if (start_word != end_word) {
                set_selection(err, start_word, end_word);
                MX_ERROR_CLEAR(err);
            }
        } else {
            mx_wp_doc_pos start_word = position, end_word = position;

            mx_para_pos* pp;
            mx_paragraph* p;

            pp = start_word.get_para_pos(err);
            MX_ERROR_CHECK(err);

            pp->moveto_start_word();

            pp = end_word.get_para_pos(err);
            MX_ERROR_CHECK(err);

            p = end_word.get_para(err, document);
            MX_ERROR_CHECK(err);

            pp->moveto_end_word(err, p);
            MX_ERROR_CHECK(err);

            // save the x_position on next up/down movement
            save_x_pos = TRUE;

            if (start_word != end_word) {
                set_selection(err, start_word, end_word);
                MX_ERROR_CLEAR(err);
            }
        }
    }

abort:;
}

void mx_wp_cursor::triple_click(int& err, mx_button_event& event)
{
    mx_doc_coord_t cd;

    if (active_area->is_active()) {
        // start an appropriate editor here
        return;
    } else {
        if (event.getShift() && is_selection) {
            mx_doc_coord_t cd;
            mx_wp_doc_pos temp_pos;
            mx_wp_doc_pos start_word, end_word;

            mx_para_pos* pp;
            mx_paragraph* p;

            cd = event.getCoord();

            calculate_view_correction(err, cd.sheet_number);
            if (err == MX_DOCUMENT_SHEET_INDEX) {
                MX_ERROR_THROW(err, MX_POSITION_RANGE_ERROR);
            }
            MX_ERROR_CHECK(err);

            cd.p += view_correction;

            temp_pos.moveto(err, document, cd);
            MX_ERROR_CHECK(err);

            start_word = temp_pos;
            end_word = temp_pos;

            pp = start_word.get_para_pos(err);
            MX_ERROR_CHECK(err);

            pp->word_index = 0;
            pp->letter_index = 0;

            pp = end_word.get_para_pos(err);
            MX_ERROR_CHECK(err);

            p = end_word.get_para(err, document);
            MX_ERROR_CHECK(err);

            pp->moveto_end(err, p, pp->line_index);
            MX_ERROR_CHECK(err);

            if (start_word > start) {
                start_word = start;
            }

            if (end_word < position) {
                end_word = position;
            }

            if (start_word != end_word) {
                set_selection(err, start_word, end_word);
                MX_ERROR_CLEAR(err);
            }
        } else {
            mx_wp_doc_pos start_word = position, end_word = position;

            mx_para_pos* pp;
            mx_paragraph* p;

            pp = start_word.get_para_pos(err);
            MX_ERROR_CHECK(err);

            pp->word_index = 0;
            pp->letter_index = 0;

            pp = end_word.get_para_pos(err);
            MX_ERROR_CHECK(err);

            p = end_word.get_para(err, document);
            MX_ERROR_CHECK(err);

            pp->moveto_end(err, p, pp->line_index);
            MX_ERROR_CHECK(err);

            // save the x_position on next up/down movement
            save_x_pos = TRUE;

            if (start_word != end_word) {
                set_selection(err, start_word, end_word);
                MX_ERROR_CLEAR(err);
            }
        }
    }

abort:;
}

void mx_wp_cursor::button_motion(int& err, mx_button_event& event)
{
    mx_doc_coord_t cd;

    active_area->button_motion(err, event);
    MX_ERROR_CHECK(err);

    if (!active_area->is_dragging()) {
        update_selection(err, TRUE);
        MX_ERROR_CHECK(err);

        cd = event.getCoord();

        calculate_view_correction(err, cd.sheet_number);
        if (err == MX_DOCUMENT_SHEET_INDEX) {
            MX_ERROR_THROW(err, MX_POSITION_RANGE_ERROR);
        }
        MX_ERROR_CHECK(err);

        cd.p += view_correction;

        position.moveto(err, document, cd);
        MX_ERROR_CHECK(err);

        update_caret(err);
        MX_ERROR_CHECK(err);
    }

abort:;
}

void mx_wp_cursor::button_motion_end(int& err, mx_button_event& event)
{
    mx_doc_coord_t cd;
    bool area_was_resized = active_area->is_dragging();

    active_area->button_motion_end(err, event);
    MX_ERROR_CHECK(err);

    if (area_was_resized) {
        mx_wp_doc_pos sheet_start = position, sheet_end = position;

        sheet_start.moveto_start_sheet(err, document);
        MX_ERROR_CHECK(err);

        sheet_end.moveto_end_sheet(err, document);
        MX_ERROR_CHECK(err);

        // the area has just been dragged to resize/crop, so reformat and
        // redraw
        reformat_and_restore_position(err, sheet_start, sheet_end, TRUE);
        MX_ERROR_CHECK(err);

        active_area->get_area(err, get_position(), get_document());
        MX_ERROR_CHECK(err);

        frame->refresh(err);
        MX_ERROR_CHECK(err);
    } else {
        update_selection(err, TRUE);
        MX_ERROR_CHECK(err);

        cd = event.getCoord();

        calculate_view_correction(err, cd.sheet_number);
        if (err == MX_DOCUMENT_SHEET_INDEX) {
            MX_ERROR_THROW(err, MX_POSITION_RANGE_ERROR);
        }
        MX_ERROR_CHECK(err);

        cd.p += view_correction;

        position.moveto(err, document, cd);
        MX_ERROR_CHECK(err);

        // save the x_position on next up/down movement
        save_x_pos = TRUE;

        is_selection = (position != start);
        if (!is_selection) {
            frame->refresh(err);
            MX_ERROR_CHECK(err);
        }
    }

    {
        mx_char_style* cs = this->char_style_at_position(err);
        MX_ERROR_CHECK(err);

        set_current_style(*cs);
    }

    fix_selection_positions(err);
    MX_ERROR_CHECK(err);

    update_caret(err);
    MX_ERROR_CHECK(err);

abort:;
}

void mx_wp_cursor::move_vertical(int& err, mx_vertical_direction d,
    bool is_shift)
{
    mx_doc_coord_t p;

    update_selection(err, is_shift);
    MX_ERROR_CHECK(err);

    if (save_x_pos) {
        p = position.position(err, document);
        MX_ERROR_CHECK(err);
        saved_x_pos = p.p.x;
    }

    bool b;

    b = is_shift && !position.is_same_cell_as(err, &start);
    MX_ERROR_CHECK(err);

    if (b) {
        if (d == up_e) {
            position.prev_row(err, document);
            MX_ERROR_CHECK(err);
        } else {
            position.next_row(err, document);
            MX_ERROR_CHECK(err);
        }
    } else {
        if (d == up_e) {
            position.up(err, document);
            MX_ERROR_CHECK(err);
        } else {
            position.down(err, document);
            MX_ERROR_CHECK(err);
        }
    }

    // restore the x position as best we can
    p = position.position(err, document);
    MX_ERROR_CHECK(err);

    p.p.x = saved_x_pos;
    position.moveto(err, document, p);
    MX_ERROR_CHECK(err);

    // don't want to save the position on the next up/down movement
    save_x_pos = FALSE;

    fix_selection_positions(err);
    MX_ERROR_CHECK(err);
abort:;
}

void mx_wp_cursor::up(int& err, bool is_shift)
{
    move_vertical(err, up_e, is_shift);
}

void mx_wp_cursor::down(int& err, bool is_shift)
{
    move_vertical(err, down_e, is_shift);
}

void mx_wp_cursor::left(int& err, bool is_shift)
{
    bool b = is_shift;

    update_selection(err, is_shift);
    MX_ERROR_CHECK(err);

    b = b && !start.is_same_cell_as(err, &position);
    MX_ERROR_CHECK(err);

    b = b && position.is_table(err);
    MX_ERROR_CHECK(err);

    if (b) {
        if (start.is_same_area_as(&position)) {
            position.prev_cell(err, document);
            MX_ERROR_CHECK(err);
        } else {
            this->up(err, is_shift);
            MX_ERROR_CHECK(err);

            return;
        }
    } else {
        position.left(err, document);
        MX_ERROR_CHECK(err);
    }

    fix_selection_positions(err);
    MX_ERROR_CHECK(err);
abort:;
}

void mx_wp_cursor::right(int& err, bool is_shift)
{
    bool b = is_shift;

    update_selection(err, is_shift);
    MX_ERROR_CHECK(err);

    b = b && !start.is_same_cell_as(err, &position);
    MX_ERROR_CHECK(err);

    b = b && position.is_table(err);
    MX_ERROR_CHECK(err);

    if (b) {
        if (start.is_same_area_as(&position)) {
            position.next_cell(err, document);
            MX_ERROR_CHECK(err);
        } else {
            this->down(err, is_shift);
            MX_ERROR_CHECK(err);

            return;
        }
    } else {
        position.right(err, document);
        MX_ERROR_CHECK(err);
    }

    fix_selection_positions(err);
    MX_ERROR_CHECK(err);
abort:;
}

void mx_wp_cursor::backspace(int& err)
{
    int ns_before = 0, ns_after = 0;
    bool deleted_para = FALSE;

    mx_cw_t* cwi;
    mx_ur_item* ur_item;

    if (is_selection) {
        force_refresh_whole_screen();

        remove_selection(err);
        MX_ERROR_CHECK(err);

        deleted_para = TRUE;
    } else {
        mx_char_style *cs_ptr, cs;
        mx_paragraph_style style_at_pos, *base_style;
        mx_paragraph* para;

        para = position.get_para(err, document);
        MX_ERROR_CHECK(err);

        base_style = para->get_base_paragraph_style();
        style_at_pos = *para->get_paragraph_style();

        cs_ptr = char_style_at_position(err);
        MX_ERROR_CHECK(err);
        cs = *cs_ptr;

        ns_before = document->get_num_sheets(err);
        MX_ERROR_CHECK(err);

        fix_split_paragraph(err);
        MX_ERROR_CHECK(err);

        cwi = document->delete_to_left(err, &position, deleted_para);
        MX_ERROR_CHECK(err);

        // revert the current style back to previous
        set_current_style(cs);

        if (deleted_para) {
            force_refresh_whole_screen();
            ur_item = new mx_ur_item(mx_undo_delete_para_break_e, document,
                base_style->get_name(), style_at_pos,
                position);
            delete cwi;
        } else {
            mx_char_style* ur_cs;
            ur_cs = new mx_char_style(*this->get_current_style());
            ur_item = new mx_ur_item(mx_undo_delete_e, cwi, ur_cs, position);
        }

        ur_buffer->insert(err, ur_item, document);
        MX_ERROR_CHECK(err);

        ns_after = document->get_num_sheets(err);
        MX_ERROR_CHECK(err);

        cs_ptr = char_style_at_position(err);
        MX_ERROR_CHECK(err);

        deleted_para = deleted_para || (cs != *cs_ptr);
    }

    {
        mx_wp_doc_pos start_pos = position, end_pos = position;

        if (deleted_para) {
            start_pos.moveto_start_para(err, document);
            MX_ERROR_CLEAR(err);

            end_pos.moveto_end_sheet(err, document);
            MX_ERROR_CHECK(err);
        }

        reformat_and_restore_position(err, start_pos, end_pos, ns_before != ns_after);
        MX_ERROR_CHECK(err);
    }

abort:
    return;
}

void mx_wp_cursor::handle_delete(int& err)
{
    if (is_selection) {
        force_refresh_whole_screen();

        remove_selection(err);
        MX_ERROR_CHECK(err);
    } else {
        mx_word* w = document->get_word(err, position);
        MX_ERROR_CHECK(err);

        if (w->is_a(mx_break_word_class_e) && ((mx_break_word*)w)->type() == mx_long_word_break_e) {
            position.right(err, document);
            MX_ERROR_CHECK(err);
        }

        position.right(err, document);
        MX_ERROR_CHECK(err);

        backspace(err);
        if (err == MX_POSITION_RANGE_ERROR) {
            MX_ERROR_CLEAR(err);

            position.left(err, document);
            MX_ERROR_CHECK(err);

            MX_ERROR_THROW(err, MX_POSITION_RANGE_ERROR);
        }
        MX_ERROR_CHECK(err);
    }

abort:;
}

void mx_wp_cursor::handle_special_key(int& err, mx_key_event& event)
{
}

void mx_wp_cursor::handle_normal_key(int& err, mx_key_event& event)
{
}

void mx_wp_cursor::key_down(
    int& err,
    mx_key_event& event,
    bool& something_changed)
{
    bool was_selection = is_selection, get_the_current_style = TRUE;
    const char* s = event.getKeyName();
    char* str = event.getString();

    key_event_was_cursor_movement = FALSE;
    need_to_refresh_whole_screen = FALSE;
    something_changed = TRUE;

    // deactivate the active area
    active_area->deactivate();

    if ((strncmp(s, "Shift", 5) == 0) || (strncmp(s, "Control", 7) == 0) || (strncmp(s, "Caps", 4) == 0)) {
        something_changed = FALSE;
        return;
    }

    if (strlen(s) == 1) {
        insert_char(err, s[0]);
    } else if (strcmp(s, "Down") == 0) {
        key_event_was_cursor_movement = TRUE;
        something_changed = FALSE;
        down(err, event.getShift());
    } else if (strcmp(s, "Up") == 0) {
        key_event_was_cursor_movement = TRUE;
        something_changed = FALSE;
        up(err, event.getShift());
    } else if (strcmp(s, "Left") == 0) {
        key_event_was_cursor_movement = TRUE;
        something_changed = FALSE;
        left(err, event.getShift());

        // save the x_position on next up/down movement
        save_x_pos = TRUE;
    } else if (strcmp(s, "Right") == 0) {
        key_event_was_cursor_movement = TRUE;
        something_changed = FALSE;
        right(err, event.getShift());

        // save the x_position on next up/down movement
        save_x_pos = TRUE;
    } else if (strcmp(s, "BackSpace") == 0) {
        get_the_current_style = position.is_at_start_of_para(err, document);
        MX_ERROR_CHECK(err);

        backspace(err);
    } else if (strcmp(s, "Delete") == 0) {
        handle_delete(err);
    } else if (strcmp(s, "space") == 0) {
        insert_char(err, ' ');
    } else if (strcmp(s, "Tab") == 0) {
        insert_char(err, '\t');
    } else if (strcmp(s, "Return") == 0) {
        get_the_current_style = event.getShift();
        handle_return(err, event.getShift());
    } else {
        if (get_current_style()->get_font()->em_width(str[0]) != 0) {
            insert_char(err, str[0]);
        } else {
            something_changed = FALSE;
            return;
        }
    }
    MX_ERROR_CHECK(err);

    if (something_changed) {
        // save the x_position on next up/down movement
        save_x_pos = TRUE;

        // if we had a selection, it was removed so force a full redraw
        if (was_selection)
            force_refresh_whole_screen();
    }

    if (get_the_current_style) {
        mx_char_style* cs = this->char_style_at_position(err);
        MX_ERROR_CHECK(err);

        set_current_style(*cs);
    }

abort:;
}

bool mx_wp_cursor::need_to_refresh_all()
{
    return need_to_refresh_whole_screen;
}

void mx_wp_cursor::pre_scroll()
{
    device->switchCaretOff();
    active_area->pre_scroll();
}

void mx_wp_cursor::post_scroll()
{
    active_area->post_scroll();
    if (!active_area->is_active())
        device->switchCaretOn();
}

void mx_wp_cursor::insert_char(int& err, char c)
{
    mx_char_style *position_cs, *current_cs;
    mx_cw_t *mod_on = NULL, *mod_off = NULL;
    mx_ur_item* item;

    remove_selection(err);
    MX_ERROR_CHECK(err);

    this->fix_split_paragraph(err);
    MX_ERROR_CHECK(err);

    position_cs = this->char_style_at_position(err);
    MX_ERROR_CHECK(err);

    current_cs = this->get_current_style();

    if (*position_cs != *current_cs) {
        // need to insert character style mods around the character
        mx_char_style_mod new_cs_on(*position_cs, *current_cs);
        mx_char_style_mod new_cs_off(*current_cs, *position_cs);

        mod_on = new mx_style_mod_cw_t(new_cs_on);
        mod_off = new mx_style_mod_cw_t(new_cs_off);

        this->force_refresh_whole_screen();
    }

    item = new mx_ur_item(mx_undo_typing_e, position);

    ur_buffer->insert(err, item, document);
    MX_ERROR_CHECK(err);

    document->insert_char(err, c, position);
    MX_ERROR_CHECK(err);

    if (mod_on != NULL) {
        document->insert_cw_item(err, mod_on, position);
        MX_ERROR_CHECK(err);
    }

    position.right(err, document);
    MX_ERROR_CHECK(err);

    if (mod_off != NULL) {
        mx_wp_doc_pos start_pos, end_pos;

        document->insert_cw_item(err, mod_off, position);
        MX_ERROR_CHECK(err);

        start_pos = end_pos = position;

        start_pos.moveto_start_para(err, document);
        MX_ERROR_CHECK(err);

        end_pos.moveto_end_sheet(err, document);
        MX_ERROR_CHECK(err);

        reformat_and_restore_position(err, start_pos, end_pos, FALSE);
        MX_ERROR_CHECK(err);
    } else {
        mx_wp_doc_pos p = position;

        p.left(err, document);
        MX_ERROR_CHECK(err);

        reformat_and_restore_position(err, p, p, FALSE);
        MX_ERROR_CHECK(err);
    }
abort:;
}

void mx_wp_cursor::handle_return(int& err, bool is_shift)
{
    mx_char_style *position_cs, *current_cs;
    mx_cw_t *mod_on = NULL, *mod_off = NULL;
    mx_ur_item* ur_item;

    remove_selection(err);
    MX_ERROR_CHECK(err);

    fix_split_paragraph(err);
    MX_ERROR_CHECK(err);

    position_cs = this->char_style_at_position(err);
    MX_ERROR_CHECK(err);

    if (is_shift) {
        current_cs = this->get_current_style();

        if (*position_cs != *current_cs) {
            // need to insert character style mods around the character
            mx_char_style_mod new_cs_on(*position_cs, *current_cs);
            mx_char_style_mod new_cs_off(*current_cs, *position_cs);

            mod_on = new mx_style_mod_cw_t(new_cs_on);
            mod_off = new mx_style_mod_cw_t(new_cs_off);

            this->force_refresh_whole_screen();
        }

        ur_item = new mx_ur_item(mx_undo_typing_e, position);
        ur_buffer->insert(err, ur_item, document);
        MX_ERROR_CHECK(err);

        document->insert_line_break(err, position);
        MX_ERROR_CHECK(err);
    } else {
        ur_item = new mx_ur_item(mx_undo_typing_e, position);

        ur_buffer->insert(err, ur_item, document);
        MX_ERROR_CHECK(err);

        document->insert_para_break(err, position);
        if (err == MX_POSITION_RANGE_ERROR) {
            MX_ERROR_CLEAR(err);

            (void)insert_paragraph(err);
        }
        force_refresh_whole_screen();
    }
    MX_ERROR_CHECK(err);

    if (mod_on) {
        document->insert_cw_item(err, mod_on, position);
        MX_ERROR_CHECK(err);
    }

    position.right(err, document);
    MX_ERROR_CHECK(err);

    if (mod_off) {
        document->insert_cw_item(err, mod_off, position);
        MX_ERROR_CHECK(err);
    }

    {
        mx_wp_doc_pos start_pos = position, end_pos = position;

        start_pos.moveto_start_sheet(err, document);
        MX_ERROR_CHECK(err);

        end_pos.moveto_end_sheet(err, document);
        MX_ERROR_CHECK(err);

        reformat_and_restore_position(err, start_pos, end_pos, FALSE);
        MX_ERROR_CHECK(err);
    }
abort:;
}

void mx_wp_cursor::fix_split_paragraph(int& err)
{
    mx_paragraph* p = position.get_para(err, document);
    MX_ERROR_CHECK(err);

    if (p->get_type() != mx_paragraph::mx_paragraph_whole_e) {
        force_refresh_whole_screen();

        position.save(err, document);
        MX_ERROR_CHECK(err);

        document->rejoin_split_paragraph(err, position);
        MX_ERROR_CHECK(err);

        frame->resetSheetLayout(err);
        MX_ERROR_CHECK(err);

        position.restore(err, document);
        MX_ERROR_CHECK(err);
    }
abort:;
}

void mx_wp_cursor::force_refresh_whole_screen()
{
    need_to_refresh_whole_screen = TRUE;
}

bool mx_wp_cursor::last_key_event_was_cursor_movement()
{
    return key_event_was_cursor_movement;
}

void mx_wp_cursor::pre_redraw()
{
    active_area->pre_redraw();
}

void mx_wp_cursor::post_redraw()
{
    active_area->post_redraw();
}
