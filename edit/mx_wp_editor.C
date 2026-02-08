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
 * MODULE/CLASS : mx_wp_editor
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

#include "mx_wp_editor.h"
#include <maxwell.h>
#include <mx.h>
#include <mx_db.h>
#include <mx_ui.h>

#include <mx_app.h>
#include <mx_view.h>

#include <mx_ascii_importer.h>
#include <mx_cw_t.h>
#include <mx_rtf_importer.h>
#include <mx_ta_pos.h>
#include <mx_undo.h>
#include <mx_wp_area_iter.h>
#include <mx_wp_style_iter.h>

mx_wp_editor::mx_wp_editor(int& err, mx_app& a, mx_wp_document* doc)
    : mx_editor(a, NULL)
    , layout(NULL)
    , body_ur_buffer(new mx_undo_redo_buffer(50))
    , header_ur_buffer(new mx_undo_redo_buffer(50))
    , footer_ur_buffer(new mx_undo_redo_buffer(50))
    , menu(NULL)
    , toolbar(NULL)
    , statusbar(NULL)
    , frame(NULL)
{
    current_view_mode = mx_paint_wp_normal_e;
    current_edit_component = mx_wp_editor_body_e;
    draw_table_guides = TRUE;
    last_search_pattern[0] = 0;

    footer_document = NULL;
    header_document = NULL;
    ur_buffer = body_ur_buffer;

    body_document = document = doc;

    (void)set_sheet_layout(err, document);
    MX_ERROR_CHECK(err);

    setSheetLayout(layout);

    menu = new mx_wp_menubar(window);
    window->add_ui_object(menu);
    print_layout = NULL;
    print_target = NULL;
    print_frame = NULL;

    toolbar = new mx_wp_toolbar(window);
    toolbar->set_style_pulldown(doc);

    window->add_ui_object(toolbar);

    window->set_title(doc->get_file_name());

    create_frame(err);
    MX_ERROR_CHECK(err);

    window->add_ui_object(frame);

    statusbar = new mx_wp_statusbar(window);
    window->add_ui_object(statusbar);

    cursor.set_frame(frame);
    cursor.set_ur_buffer(ur_buffer);
    cursor.set_document(doc);
    cursor.set_view_mode(current_view_mode);
    cursor.set_layout(layout);

    statusbar->update_b(MX_MAXWELL_VERSION, FALSE);

    statusbar->update_b(
        err,
        *(cursor.get_position()),
        document,
        current_units,
        FALSE,
        current_zoom * 100.0);
    MX_ERROR_CHECK(err);

    window->initial_focus((mx_screen_device*)(frame->getDevice()));

    window->activate(err);
    MX_ERROR_CHECK(err);

    m_window = XtWindow(window->get_widget());
    m_editors[m_window] = this;

    ((mx_screen_device*)frame->getDevice())->addProcessMask(MX_EVENT_KEY_RELEASE | MX_EVENT_PREDRAW | MX_EVENT_POSTDRAW);
    ((mx_screen_device*)frame->getDevice())->take_focus();

    update_all_sensitivities(err);
    MX_ERROR_CHECK(err);

    menu->set_invisible(mx_edit_find_again_e, mx_end_item_e);
    toolbar->set_invisible(mx_edit_find_again_e, mx_end_item_e);

    just_received_external_selection = FALSE;
    update_toolbar_on_shift_release = FALSE;
    update_status_on_draw_event = TRUE;
    return;

abort:;
}

mx_wp_editor::~mx_wp_editor()
{
    int err = MX_ERROR_OK;

    if (menu)
        delete menu;
    if (toolbar)
        delete toolbar;
    if (statusbar)
        delete statusbar;
    if (frame)
        delete frame;
    if (window)
        delete window;
    if (layout)
        delete layout;

    delete body_ur_buffer;
    delete header_ur_buffer;
    delete footer_ur_buffer;

    if (footer_document != NULL) {
        mx_db_client_close_wp_doc(err, footer_document);
        MX_ERROR_CHECK(err);
    }

    if (header_document != NULL) {
        mx_db_client_close_wp_doc(err, header_document);
        MX_ERROR_CHECK(err);
    }

    mx_db_client_close_wp_doc(err, document);
    MX_ERROR_CHECK(err);

    return;
abort:
    global_error_trace->print();
    MX_ERROR_CLEAR(err);
}

void mx_wp_editor::window_is_closing()
{
    int err = MX_ERROR_OK;

    (void)file_force_close(err);
    MX_ERROR_CHECK(err);

    return;

abort:
    global_error_trace->print();
    MX_ERROR_CLEAR(err);
}

mx_wp_statusbar* mx_wp_editor::get_status_bar()
{
    return statusbar;
}

mx_document* mx_wp_editor::open_derived_doc_class(int& err, char* name, bool recover)
{
    mx_wp_document* res;

    switch (mx_guess_file_type(name)) {
    case mx_document_file_e:
        res = mx_db_client_open_wp_doc(err, name, recover);
        MX_ERROR_CHECK(err);
        return res;

    case mx_rtf_file_e:
        res = open_derived_import_rtf(err, name);
        MX_ERROR_CHECK(err);
        return res;

    case mx_ascii_file_e:
        res = open_derived_import_ascii(err, name);
        MX_ERROR_CHECK(err);
        return res;

    default:
        MX_ERROR_THROW(err, MX_FILE_UNIMPORTABLE);
        break;
    }
abort:
    return NULL;
}

mx_wp_document* mx_wp_editor::open_derived_import_rtf(int& err, char* name)
{
    mx_wp_document* res;
    const char* s;
    mx_rtf_importer* i;

    mx_wp_cursor temp_cursor;

    s = global_user_config->get_default_string(err, "page", "A4");
    MX_ERROR_CHECK(err);

    res = mx_db_client_open_temporary_wp_doc(err, (char*)s);
    MX_ERROR_CHECK(err);

    temp_cursor.set_document(res);

    i = new mx_rtf_importer(err, name, res, &temp_cursor, &dialog_man);
    MX_ERROR_CHECK(err);

    i->load(err);
    if (err == MX_ERROR_OK) {
        bool b;
        mx_cw_t* p = res->delete_to_left(err, temp_cursor.get_position(), b);
        MX_ERROR_CHECK(err);
        delete p;

        res->reformat(err);
        MX_ERROR_CHECK(err);

        res->repaginate(err, 0);
        MX_ERROR_CHECK(err);

        delete i;
        return res;
    } else {
        int the_err = err;

        global_error_trace->print();
        MX_ERROR_CLEAR(err);

        // try to close the document
        mx_db_client_close_wp_doc(err, res);
        MX_ERROR_CLEAR(err);

        // rethrow the error
        MX_ERROR_THROW(err, the_err);
    }

abort:
    return NULL;
}

mx_wp_document* mx_wp_editor::open_derived_import_ascii(int& err, char* name)
{
    mx_wp_document* res;
    const char* s;
    mx_ascii_importer* i;

    mx_wp_cursor temp_cursor;

    s = global_user_config->get_default_string(err, "page", "A4");
    MX_ERROR_CHECK(err);

    res = mx_db_client_open_temporary_wp_doc(err, (char*)s);
    MX_ERROR_CHECK(err);

    temp_cursor.set_document(res);

    temp_cursor.insert_paragraph(err);
    MX_ERROR_CHECK(err);

    i = new mx_ascii_importer(err, name, res, &temp_cursor, &dialog_man);
    MX_ERROR_CHECK(err);

    i->import(err);
    if (err == MX_ERROR_OK) {
        delete i;

        res->reformat(err);
        MX_ERROR_CHECK(err);

        res->repaginate(err, 0);
        MX_ERROR_CHECK(err);

        return res;
    } else {
        int the_err = err;

        global_error_trace->print();
        MX_ERROR_CLEAR(err);

        // try to close the document
        mx_db_client_close_wp_doc(err, res);
        MX_ERROR_CLEAR(err);

        // rethrow the error
        MX_ERROR_THROW(err, the_err);
    }
abort:
    return NULL;
}

mx_wp_document* mx_wp_editor::open_derived_import_msword(int& err, char* name)
{
    err = MX_ERROR_NOT_IMPLEMENTED;
    return NULL;
}

mx_wp_layout& mx_wp_editor::set_sheet_layout(int& err, mx_wp_document* doc)
{
    mx_sheet_size* sheets;
    int i, n;

    if (layout == NULL) {
        layout = new mx_wp_layout;
    } else {
        n = layout->getNumberSheets();

        layout->removeSheets(err, 0, n);
        MX_ERROR_CHECK(err);
    }

    n = doc->get_num_sheets(err);
    MX_ERROR_CHECK(err);

    sheets = new mx_sheet_size[n];

    for (i = 0; i < n; i++) {
        sheets[i] = doc->get_sheet_visible_size(err, i, current_view_mode);
        MX_ERROR_CHECK(err);
    }

    layout->setSnapTolerance(0.00001);

    layout->addSheets(err, 0, n, sheets);
    MX_ERROR_CHECK(err);

    delete[] sheets;

    return *layout;
abort:
    return *layout;
}

void mx_wp_editor::setup_frame_target_params()
{
    addProcessMask(MX_EVENT_KEY_PRESS | MX_EVENT_KEY_RELEASE | MX_EVENT_PREDRAW | MX_EVENT_POSTDRAW | MX_EVENT_ALL_SCROLL | MX_EVENT_BUTTON_PRESSES);
}

void mx_wp_editor::create_frame(int& err)
{
    mx_doc_coord_t top_left(-MX_INCHES_TO_MM(1), 0, 0);
    mx_point size(210, 192);
    mx_point increment(5, 5);
    mx_rect scroll_add(
        MX_INCHES_TO_MM(1), 0,
        MX_INCHES_TO_MM(1), MX_INCHES_TO_MM(1));

    frame = new mx_scrollable_frame(
        err,
        top_left,
        size,
        1,
        *this,
        window->get_form(),
        TRUE,
        increment,
        FALSE,
        &scroll_add);
    MX_ERROR_CHECK(err);

    frame->setApplyScrollLimitToTarget(TRUE);
    frame->setMouseScrolling(TRUE);
    frame->setDrawPerSheet(TRUE);
    frame->setTargetLimitAdd(scroll_add);
    frame->addProcessMask(MX_EVENT_KEY_PRESS | MX_EVENT_KEY_RELEASE | MX_EVENT_PREDRAW | MX_EVENT_POSTDRAW);

    setup_frame_target_params();
abort:;
}

void mx_wp_editor::draw(int& err, mx_draw_event& event)
{
    mx_device* dev;
    mx_doc_rect_t r(event.getTL(), event.getBR());

    dev = event.getDevice();

    mx_painter::set_draw_mode(current_view_mode);
    mx_table_area_painter::set_guide_draw(draw_table_guides);
    mx_painter::set_device(*dev);
    if (dev->is_a(mx_screen_device_class_e)) {
        ((mx_screen_device*)dev)->setDrawToWindowOnly(FALSE);
    }

    if (cursor.selection()) {
        document->set_highlight_range(*cursor.selection_start(),
            *cursor.selection_end());
    } else {
        document->clear_highlight();
    }

    document->redraw(err, r);
    MX_ERROR_CHECK(err);

    if (update_status_on_draw_event) {
        mx_char_style* cs = cursor.char_style_at_position(err);
        MX_ERROR_CHECK(err);

        cursor.set_current_style(*cs);

        update_toolbar(err);
        MX_ERROR_CHECK(err);

        update_all_sensitivities(err);
        MX_ERROR_CHECK(err);

        update_status_on_draw_event = FALSE;
    }
abort:;
}

void mx_wp_editor::buttonPress(int& err, mx_button_event& event)
{
    ((mx_screen_device*)frame->getDevice())->take_focus();

    just_received_external_selection = FALSE;

    if (event.getButtonNumber() == 2) {
        // paste the current X selection - if there is one
        frame->requestSelection(err);
        MX_ERROR_CHECK(err);
    } else {
        cursor.single_click(err, event);
        if (err == MX_POSITION_RANGE_ERROR) {
            MX_ERROR_CLEAR(err);
            window->bell();

            cursor.get_position()->make_valid(err, document);
            MX_ERROR_CHECK(err);
        } else {
            MX_ERROR_CHECK(err);

            statusbar->update_b(
                err,
                *(cursor.get_position()),
                document,
                current_units,
                FALSE,
                current_zoom * 100.0);
            MX_ERROR_CHECK(err);

            update_toolbar(err);
            MX_ERROR_CHECK(err);
        }
    }

    update_all_sensitivities(err);
    MX_ERROR_CHECK(err);

abort:;
}

void mx_wp_editor::buttonDoublePress(int& err, mx_button_event& event)
{
    cursor.double_click(err, event);
    MX_ERROR_CHECK(err);

    statusbar->update_b(
        err,
        *(cursor.get_position()),
        document,
        current_units,
        FALSE,
        current_zoom * 100.0);
    MX_ERROR_CHECK(err);

    update_edit_sensitivities(err);
    MX_ERROR_CHECK(err);

abort:;
}

void mx_wp_editor::buttonTriplePress(int& err, mx_button_event& event)
{
    cursor.triple_click(err, event);
    MX_ERROR_CHECK(err);

    statusbar->update_b(
        err,
        *(cursor.get_position()),
        document,
        current_units,
        FALSE,
        current_zoom * 100.0);
    MX_ERROR_CHECK(err);

    update_edit_sensitivities(err);
    MX_ERROR_CHECK(err);

abort:;
}

void mx_wp_editor::buttonMotion(int& err, mx_button_event& event)
{
    if (just_received_external_selection)
        return;
    cursor.button_motion(err, event);
    if (err == MX_POSITION_RANGE_ERROR) {
        MX_ERROR_CLEAR(err);
        window->bell();

        cursor.get_position()->make_valid(err, document);
        MX_ERROR_CHECK(err);
    } else {
        MX_ERROR_CHECK(err);

        statusbar->update_b(
            err,
            *(cursor.get_position()),
            document,
            current_units,
            FALSE,
            current_zoom * 100.0);
        MX_ERROR_CHECK(err);
    }

abort:;
}

void mx_wp_editor::buttonMotionEnd(int& err, mx_button_event& event)
{
    if (just_received_external_selection) {
        just_received_external_selection = FALSE;
        return;
    }

    cursor.button_motion_end(err, event);
    if (err == MX_POSITION_RANGE_ERROR) {
        MX_ERROR_CLEAR(err);
        window->bell();

        cursor.get_position()->make_valid(err, document);
        MX_ERROR_CHECK(err);
    } else {
        MX_ERROR_CHECK(err);

        statusbar->update_b(
            err,
            *(cursor.get_position()),
            document,
            current_units,
            FALSE,
            current_zoom * 100.0);
        MX_ERROR_CHECK(err);

        update_toolbar(err);
        MX_ERROR_CHECK(err);
    }

    update_all_sensitivities(err);
    MX_ERROR_CHECK(err);

abort:;
}

void mx_wp_editor::scroll(int& err, mx_scroll_event& event)
{
}

void mx_wp_editor::scrollDrag(int& err, mx_scroll_event& event)
{
    float x_offset;

    frame->forceScroll(err, event);
    MX_ERROR_CHECK(err);

    x_offset = event.getOldTopLeft().p.x - event.getNewTopLeft().p.x;

    if (x_offset != 0.0) {
        mx_ruler_bar* ruler = toolbar->get_ruler_bar();

        ruler->set_left_origin(err, ruler->get_left_origin() + x_offset);
        MX_ERROR_CHECK(err);
    }
abort:;
}

void mx_wp_editor::config_changed()
{
    mx_unit_e old_units = current_units;

    int err = MX_ERROR_OK;

    // re-read config
    mx_editor::config_changed();

    if (old_units != current_units) {
        toolbar->get_ruler_bar()->display(err, current_zoom, current_units);
        MX_ERROR_CHECK(err);

        frame->refresh(err);
        MX_ERROR_CHECK(err);
    }
    return;

abort:
    global_error_trace->print();
    MX_ERROR_CLEAR(err);
}

void mx_wp_editor::keyPress(int& err, mx_key_event& event)
{
    int n;
    bool was_selection;
    bool something_changed;

    mx_paragraph* cursor_paragraph;

    int old_num_sheets = document->get_num_sheets(err);
    MX_ERROR_CHECK(err);

    cursor_paragraph = cursor.get_position()->get_para(err, document);
    MX_ERROR_CHECK(err);

    was_selection = cursor.selection();

    cursor.key_down(err, event, something_changed);
    if (err == MX_POSITION_RANGE_ERROR) {
        MX_ERROR_CLEAR(err);
        window->bell();
        return;
    } else {
        MX_ERROR_CHECK(err);
    }

    if (something_changed) {
        n = document->get_num_sheets(err);
        MX_ERROR_CHECK(err);

        if (n == old_num_sheets && !cursor.need_to_refresh_all()) {
            refresh_changed_section(err, cursor_paragraph);
            MX_ERROR_CHECK(err);
        } else {
            (void)set_sheet_layout(err, document);
            MX_ERROR_CHECK(err);

            frame->refresh(err);
            MX_ERROR_CHECK(err);
        }
    }

    if (something_changed || cursor.last_key_event_was_cursor_movement()) {
        cursor.make_visible(err);
        MX_ERROR_CHECK(err);
    }

    cursor.update_caret(err);
    MX_ERROR_CHECK(err);

    if (cursor.last_key_event_was_cursor_movement() && event.getShift()) {
        update_toolbar_on_shift_release = TRUE;
    } else {
        update_toolbar_on_shift_release = FALSE;
        update_toolbar(err);
        MX_ERROR_CHECK(err);
    }

    statusbar->update_b(err, *cursor.get_position(), document, current_units,
        FALSE, current_zoom * 100.0);
    MX_ERROR_CHECK(err);

    if (event.is_return() || event.is_arrow() || was_selection) {
        update_all_sensitivities(err);
        MX_ERROR_CHECK(err);
    } else {
        update_undo(err);
        MX_ERROR_CHECK(err);

        update_file_sensitivities(err);
        MX_ERROR_CHECK(err);
    }

abort:;
}

void mx_wp_editor::keyRelease(int& err, mx_key_event& event)
{
    if (update_toolbar_on_shift_release && strncmp("Shift", event.getKeyName(), 5) == 0) {
        window->set_busy();

        update_toolbar_on_shift_release = FALSE;

        update_toolbar(err);
        MX_ERROR_CHECK(err);

        window->clear_busy();
    }
    return;
abort:
    window->clear_busy();
}

void mx_wp_editor::preScroll(int& err, mx_scroll_event& e)
{
    cursor.pre_scroll();
}

void mx_wp_editor::postScroll(int& err, mx_scroll_event& e)
{
    cursor.post_scroll();
}

void mx_wp_editor::preDraw(int& err, mx_draw_event& e)
{
    cursor.pre_redraw();
}

void mx_wp_editor::postDraw(int& err, mx_draw_event& e)
{
    cursor.post_redraw();
}

void mx_wp_editor::cleanup_after_cursor(int& err)
{
    mx_wp_doc_pos start_of_sheet = *cursor.selection_start();
    mx_wp_doc_pos end_of_sheet = *cursor.selection_end();

    start_of_sheet.moveto_start_sheet(err, document);
    MX_ERROR_CHECK(err);

    // go forward one sheet if we can, since a cut may affect the sheet after
    // the current one. Clear errors since there may be no more sheets in the
    // document
    end_of_sheet.forward_sheet(err, document);
    MX_ERROR_CLEAR(err);

    end_of_sheet.moveto_end_sheet(err, document);
    MX_ERROR_CHECK(err);

    cursor.reformat_and_restore_position(err, start_of_sheet, end_of_sheet,
        TRUE);
    MX_ERROR_CHECK(err);

    frame->refresh(err);
    MX_ERROR_CHECK(err);

    cursor.make_visible(err);
    MX_ERROR_CHECK(err);

    cursor.update_caret(err);
    MX_ERROR_CHECK(err);

    update_toolbar(err);
    MX_ERROR_CHECK(err);
abort:;
}

void mx_wp_editor::update_toolbar(int& err)
{
    int n, i;
    mx_list style_list;
    mx_paragraph_style** style_array = NULL;
    mx_ruler** ruler_array = NULL;

    cursor.styles_in_selection(err, style_list);
    MX_ERROR_CHECK(err);

    n = style_list.get_num_items();

    style_array = new mx_paragraph_style*[n];
    ruler_array = new mx_ruler*[n];

    for (i = 0; i < n; i++) {
        mx_paragraph_style* ps = (mx_paragraph_style*)style_list.get(err, i);
        MX_ERROR_CHECK(err);

        style_array[i] = ps;
        ruler_array[i] = ps->get_ruler();
    }

    if (!cursor.selection()) {
        MX_ERROR_ASSERT(err, n == 1);
        style_array[0]->set_char_style(*cursor.get_current_style());
    }

    toolbar->set_current_style(style_array, n);

    update_ruler(err, ruler_array, n);
    MX_ERROR_CHECK(err);

    for (i = 0; i < n; i++)
        delete style_array[i];
    delete[] style_array;
    delete[] ruler_array;
abort:;
}

void mx_wp_editor::update_all_sensitivities(int& err)
{
    update_edit_sensitivities(err);
    MX_ERROR_CHECK(err);

    update_file_sensitivities(err);
    MX_ERROR_CHECK(err);

    update_table_sensitivities(err);
    MX_ERROR_CHECK(err);

    update_insert_sensitivities(err);
    MX_ERROR_CHECK(err);

    update_format_sensitivities(err);
    MX_ERROR_CHECK(err);

    update_tool_sensitivities(err);
    MX_ERROR_CHECK(err);

    update_undo(err);
    MX_ERROR_CHECK(err);
abort:;
}

static void get_width_to_refresh(int& err,
    mx_wp_document* doc,
    mx_wp_doc_pos* pos,
    mx_doc_coord_t& tl,
    mx_doc_coord_t& br)
{
    bool b = pos->is_table(err);
    MX_ERROR_CHECK(err);

    if (b) {
        mx_text_area* txta;
        mx_table_area* taba;
        mx_table_area_pos* tabp;
        mx_point origin;

        taba = (mx_table_area*)pos->get_area(err, doc);
        MX_ERROR_CHECK(err);
        MX_ERROR_ASSERT(err, taba->is_a(mx_table_area_class_e));

        tabp = (mx_table_area_pos*)pos->get_area_pos(err);
        MX_ERROR_CHECK(err);
        MX_ERROR_ASSERT(err, tabp->is_a(mx_table_area_pos_class_e));

        origin = taba->get_origin(err);
        MX_ERROR_CHECK(err);

        txta = taba->get_cell(err, tabp->row, tabp->column);
        MX_ERROR_CHECK(err);

        origin += txta->get_origin(err);
        MX_ERROR_CHECK(err);

        tl.p.x = br.p.x = origin.x;
        br.p.x += txta->get_width(err);
        MX_ERROR_CHECK(err);
    } else {
        mx_sheet* s = doc->sheet(err, pos->get_sheet());
        MX_ERROR_CHECK(err);

        tl.p.x = 0;
        br.p.x = s->get_width(err);
        MX_ERROR_CHECK(err);
    }
abort:;
}

void mx_wp_editor::refresh_changed_section(
    int& err,
    mx_paragraph* para)
{
    if (para->get_height_changed()) {
        (void)set_sheet_layout(err, document);
        MX_ERROR_CHECK(err);

        frame->refresh(err);
        MX_ERROR_CHECK(err);
    } else {
        float upper_range, lower_range;
        mx_doc_coord_t tl, br;
        mx_para_pos* para_pos;
        mx_point offset;
        double pixel_size = frame->getDevice()->dev2frame(1.0);

        tl = br = cursor.get_position()->position(err, document);
        MX_ERROR_CHECK(err);

        para_pos = cursor.get_position()->get_para_pos(err);
        MX_ERROR_CHECK(err);

        offset = tl.p - para_pos->position(err, para);
        MX_ERROR_CHECK(err);

        para->reformat_range(err, upper_range, lower_range);
        MX_ERROR_CHECK(err);

        get_width_to_refresh(err, document, cursor.get_position(), tl, br);
        MX_ERROR_CHECK(err);

        tl.p.y = upper_range + offset.y - pixel_size;
        br.p.y = lower_range + offset.y + pixel_size;

        tl.p -= cursor.get_view_correction();
        br.p -= cursor.get_view_correction();

        frame->refresh(err, &tl, &br);
        MX_ERROR_CHECK(err);
    }
abort:;
}

void mx_wp_editor::set_edit_component(int& err, mx_wp_editor_view_t c)
{
    char temp[MAX_PATH_LEN];
    mx_wp_document* doc_to_view;
    mx_undo_redo_buffer* ur_buffer_to_use;

    mx_wp_doc_pos pos_to_use;

    if (current_edit_component == c) {
        return;
    }

    // store the cursor position
    switch (current_edit_component) {
    default:
    case mx_wp_editor_body_e:
        body_pos = *(cursor.get_position());
        break;
    case mx_wp_editor_header_e:
        header_pos = *(cursor.get_position());

        update_headers_and_footers(err);
        MX_ERROR_CHECK(err);

        break;
    case mx_wp_editor_footer_e:
        footer_pos = *(cursor.get_position());

        update_headers_and_footers(err);
        MX_ERROR_CHECK(err);

        break;
    }

    switch (c) {
    default:
    case mx_wp_editor_body_e:
        menu->set_item_checked(err, mx_view_header_e, FALSE);
        MX_ERROR_CHECK(err);
        menu->set_item_checked(err, mx_view_footer_e, FALSE);
        MX_ERROR_CHECK(err);

        strcpy(temp, document->get_file_name());
        doc_to_view = body_document;
        pos_to_use = body_pos;
        ur_buffer_to_use = body_ur_buffer;
        break;
    case mx_wp_editor_header_e:
        menu->set_item_checked(err, mx_view_header_e, TRUE);
        MX_ERROR_CHECK(err);
        menu->set_item_checked(err, mx_view_footer_e, FALSE);
        MX_ERROR_CHECK(err);

        sprintf(temp, "header - %s", document->get_file_name());
        if (header_document == NULL) {
            header_document = mx_db_client_open_temporary_wp_doc(err, "A4");
            MX_ERROR_CHECK(err);

            duplicate_sheet_size(err, body_document, header_document);
            MX_ERROR_CHECK(err);

            header_pos.moveto_start(err, header_document);
            MX_ERROR_CHECK(err);
        }

        copy_header_to_doc(err);
        MX_ERROR_CHECK(err);

        doc_to_view = header_document;
        pos_to_use = header_pos;
        ur_buffer_to_use = header_ur_buffer;
        break;
    case mx_wp_editor_footer_e:
        menu->set_item_checked(err, mx_view_header_e, FALSE);
        MX_ERROR_CHECK(err);
        menu->set_item_checked(err, mx_view_footer_e, TRUE);
        MX_ERROR_CHECK(err);

        sprintf(temp, "footer - %s", document->get_file_name());
        if (footer_document == NULL) {
            footer_document = mx_db_client_open_temporary_wp_doc(err, "A4");
            MX_ERROR_CHECK(err);

            duplicate_sheet_size(err, body_document, footer_document);
            MX_ERROR_CHECK(err);

            footer_pos.moveto_start(err, footer_document);
            MX_ERROR_CHECK(err);
        }

        copy_footer_to_doc(err);
        MX_ERROR_CHECK(err);

        doc_to_view = footer_document;
        pos_to_use = footer_pos;
        ur_buffer_to_use = footer_ur_buffer;
        break;
    }

    cursor.set_document(doc_to_view);
    cursor.set_ur_buffer(ur_buffer_to_use);
    document = doc_to_view;
    ur_buffer = ur_buffer_to_use;

    set_sheet_layout(err, doc_to_view);
    MX_ERROR_CHECK(err);

    frame->resetSheetLayout(err);
    MX_ERROR_CHECK(err);

    cursor.set_position(err, pos_to_use);
    MX_ERROR_CHECK(err);

    cursor.update_caret(err);
    MX_ERROR_CHECK(err);

    cursor.make_visible(err);
    MX_ERROR_CHECK(err);

    frame->refresh(err);
    MX_ERROR_CHECK(err);

    current_edit_component = c;
    window->set_title(temp);

    update_toolbar(err);
    MX_ERROR_CHECK(err);

    update_all_sensitivities(err);
    MX_ERROR_CHECK(err);

abort:;
}

mx_text_area* mx_wp_editor::extract_first_text_area(
    int& err,
    mx_wp_document* doc)
{
    mx_sheet* s;
    mx_text_area* res;

    s = doc->sheet(err, 0);
    MX_ERROR_CHECK(err);

    res = (mx_text_area*)s->get_area(err, 0);
    MX_ERROR_CHECK(err);

    return res;

abort:
    return NULL;
}

bool mx_wp_editor::text_area_contains_something(mx_text_area* a)
{
    mx_paragraph* p;
    int n, err = MX_ERROR_OK;

    p = a->get(err, 0);
    MX_ERROR_CHECK(err);

    n = p->num_lines();
    if (n > 1) {
        return TRUE;
    } else {
        n = p->num_words(err, 0);
        MX_ERROR_CHECK(err);

        return n > 1;
    }

abort:
    return FALSE;
}

void mx_wp_editor::update_headers_and_footers(int& err)
{
    mx_text_area* a;

    if (header_document != NULL) {
        a = extract_first_text_area(err, header_document);
        MX_ERROR_CHECK(err);

        if (text_area_contains_something(a)) {
            a = (mx_text_area*)(a->duplicate());

            body_document->set_header(err, a);
            MX_ERROR_CHECK(err);
        } else {
            body_document->clear_header(err);
            MX_ERROR_CHECK(err);
        }
    }

    if (footer_document != NULL) {
        a = extract_first_text_area(err, footer_document);
        MX_ERROR_CHECK(err);

        if (text_area_contains_something(a)) {
            a = (mx_text_area*)(a->duplicate());

            body_document->set_footer(err, a);
            MX_ERROR_CHECK(err);
        } else {
            body_document->clear_footer(err);
            MX_ERROR_CHECK(err);
        }
    }

abort:;
}

void mx_wp_editor::duplicate_sheet_size(int& err, mx_wp_document* src, mx_wp_document* dst)
{
    float width, height, left, right, top, bottom;
    bool is_landscape;

    src->get_page_size(err, width, height, is_landscape);
    MX_ERROR_CHECK(err);

    dst->set_page_size(err, width, height, is_landscape);
    MX_ERROR_CHECK(err);

    src->get_page_margins(err, left, right, top, bottom);
    MX_ERROR_CHECK(err);

    dst->set_page_margins(err, left, right, top, bottom);
    MX_ERROR_CHECK(err);
abort:;
}

static void copy_head_foot_to_doc(int& err, mx_area* a,
    mx_wp_document* doc)
{
    if (a != NULL) {
        mx_sheet* s;
        mx_area* temp;
        a = a->duplicate();

        a->move_to_document(err, doc);
        MX_ERROR_CHECK(err);

        s = doc->sheet(err, 0);
        MX_ERROR_CHECK(err);

        temp = s->remove_area(err, 0);
        MX_ERROR_CHECK(err);

        delete temp;

        s->add_area(err, a, 0);
        MX_ERROR_CHECK(err);

        doc->reformat(err);
        MX_ERROR_CHECK(err);
    }
abort:;
}

void mx_wp_editor::copy_footer_to_doc(int& err)
{
    mx_area* footer = body_document->footer(err);
    MX_ERROR_CHECK(err);

    copy_head_foot_to_doc(err, footer, footer_document);
    MX_ERROR_CHECK(err);
abort:;
}

void mx_wp_editor::copy_header_to_doc(int& err)
{
    mx_area* header = body_document->header(err);
    MX_ERROR_CHECK(err);

    copy_head_foot_to_doc(err, header, header_document);
    MX_ERROR_CHECK(err);
abort:;
}

void mx_wp_editor::update_ruler(
    int& err,
    mx_ruler** ruler_array,
    int n)
{
    float w = 0.0f;
    mx_doc_coord_t f;
    mx_ruler_bar* ruler = toolbar->get_ruler_bar();
    mx_wp_doc_pos* start = cursor.selection_start();
    mx_wp_doc_pos* end = cursor.selection_end();

    bool in_one_column = start->is_same_column_as(err, end);
    MX_ERROR_CHECK(err);

    if (cursor.selection_has_text() && !cursor.selection_has_table()) {
        w = cursor.get_sheet_width(err);
        MX_ERROR_CHECK(err);

        frame->getTopLeftFrame(err, f);
        MX_ERROR_CHECK(err);

        ruler->set_left_origin(err, -f.p.x);
        MX_ERROR_CHECK(err);

        ruler->set_sensitive(err, TRUE);
        MX_ERROR_CHECK(err);

        ruler->display(err, ruler_array, n, current_zoom, current_units, w);
        MX_ERROR_CHECK(err);
    } else if (in_one_column) {
        mx_point origin;
        mx_table_area* taba;
        mx_table_area_pos* tabp;
        mx_text_area* txta;

        tabp = (mx_table_area_pos*)start->get_area_pos(err);
        MX_ERROR_CHECK(err);

        taba = (mx_table_area*)start->get_area(err, document);
        MX_ERROR_CHECK(err);

        txta = taba->get_cell(err, tabp->row, tabp->column);
        MX_ERROR_CHECK(err);

        origin = taba->get_origin(err);
        MX_ERROR_CHECK(err);

        origin += txta->get_origin(err);
        MX_ERROR_CHECK(err);

        origin.x -= cursor.get_view_correction().x;

        w = txta->get_width(err);
        MX_ERROR_CHECK(err);

        frame->getTopLeftFrame(err, f);
        MX_ERROR_CHECK(err);

        ruler->set_left_origin(err, -f.p.x + origin.x);
        MX_ERROR_CHECK(err);

        ruler->set_sensitive(err, TRUE);
        MX_ERROR_CHECK(err);

        ruler->display(err, ruler_array, n, current_zoom, current_units, w);
        MX_ERROR_CHECK(err);
    } else {
        ruler->set_sensitive(err, FALSE);
        MX_ERROR_CHECK(err);

        ruler->display(err, ruler_array, n, current_zoom, current_units, w);
        MX_ERROR_CHECK(err);
    }
abort:;
}

void mx_wp_editor::externalSelection(int& err, char* s)
{
    mx_wp_doc_pos start, end;
    mx_ur_item* ur_item;

    start = *cursor.selection_start();

    just_received_external_selection = TRUE;

    start.save(err, document, start.get_sheet());
    MX_ERROR_CHECK(err);

    cursor.insert_text(err, s);
    MX_ERROR_CHECK(err);

    start.restore(err, document);
    MX_ERROR_CHECK(err);

    end = *cursor.get_position();

    cursor.reformat_and_restore_position(err, start, end, TRUE);
    MX_ERROR_CHECK(err);

    start.restore(err, document);
    MX_ERROR_CHECK(err);

    end = *cursor.get_position();

    cursor.set_selection(err, start, end);
    MX_ERROR_CHECK(err);

    ur_item = new mx_ur_item(mx_undo_paste_e, document, start, end, false);

    ur_buffer->insert(err, ur_item);
    MX_ERROR_CHECK(err);
abort:;
}

const char* mx_wp_editor::getExternalSelection(int& err)
{
    const char* res;

    res = cursor.get_selection_as_text(err);
    MX_ERROR_CHECK(err);

abort:
    return res;
}

mx_document* mx_wp_editor::get_document(void)
{
    return document;
}
