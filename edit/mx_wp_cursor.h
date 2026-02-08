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
 *
 */
#ifndef MX_WP_CURSOR_H
#define MX_WP_CURSOR_H

#include <mx.h>
#include <mx_event.h>
#include <mx_painter.h>
#include <mx_rtti.h>
#include <mx_sc_device.h>
#include <mx_wp_pos.h>

class mx_wp_document;
class mx_frame;
class mx_paragraph_style;
class mx_paragraph;
class mx_image_area;
class mx_diagram_area;
class mx_table_area;
class mx_text_area;
class mx_table_area_pos;
class mx_text_area_pos;
class mx_wp_layout;
class mx_cw_t;
class mx_active_area;
class mx_wp_object_buffer;
class mx_undo_redo_buffer;
class mx_nlist;
class mx_word;

class mx_wp_cursor //: public mx_cursor
{
    MX_RTTI(mx_wp_cursor_class_e)

public:
    mx_wp_cursor();
    mx_wp_cursor(mx_wp_document* doc, mx_frame* f);
    virtual ~mx_wp_cursor();

    void set_document(mx_wp_document* doc);
    void set_ur_buffer(mx_undo_redo_buffer* buf);
    void set_frame(mx_frame* f);
    void set_layout(mx_wp_layout* layout);

    // unfortunately, the view doesn't always represent the
    // actual shape of the sheets.....
    void set_view_mode(mx_painter_mode_t current_view_mode);

    void single_click(int& err, mx_button_event& event);
    void double_click(int& err, mx_button_event& event);
    void triple_click(int& err, mx_button_event& event);
    void button_motion(int& err, mx_button_event& event);
    void button_motion_end(int& err, mx_button_event& event);

    void key_down(int& err,
        mx_key_event& event,
        bool& something_changed);

    // go to a given sheet
    void moveto_sheet(int& err, int sheet);

    // moveto start of current paragraph (this does nothing if
    // the cursor is not in a text area/table cell.
    void moveto_para_start(int& err);

    // ... and end
    void moveto_para_end(int& err);

    // get style at current position
    mx_paragraph_style* style_at_position(int& err);

    // get character style at current position
    mx_char_style* char_style_at_position(int& err);

    // get/set the cursors notion of the "current character style" ie, the
    // style which will be inserted on the next keypress
    void set_current_style(const mx_char_style& cs);
    mx_char_style* get_current_style();

    // get a list of all the styles in a selection (a list containing one
    // entry if there is no selection). All the list items (pointers to
    // mx_paragraph_styles) must be deleted by the caller
    void styles_in_selection(int& err, mx_list& style_list);

    // get a list of all the table area paragraph styles in a selection
    void table_styles_in_selection(int& err, mx_list& style_list);

    // get an array of all the border styles in a selection. All the list items
    // (pointers to mx_border_styles) must be deleted by the caller. If a
    // selection is inside a table - gets the table cells border styles, else
    // gets paragraphs border styles. If a selection covers tables and text an
    // error is thrown
    void border_styles_in_selection(int& err, mx_nlist& style_array);

    // is there a selection
    bool selection() { return is_selection; };

    // set selection - takes care of making the selection visible as
    // far as possible
    void set_selection(int& err, mx_wp_doc_pos& start, mx_wp_doc_pos& end);

    // clear any selection
    void clear_selection(int& err);

    // set the current position
    void set_position(int& err, mx_wp_doc_pos& pos);

    // get start/end of selection
    mx_wp_doc_pos* selection_start();
    mx_wp_doc_pos* selection_end();

    // does the current selection/position have a table in it
    bool selection_has_table();
    bool selection_has_text();
    bool selection_has_diagram();
    bool selection_has_image();
    bool selection_has_multi_column();

    // does the selection only cover text, or a range of table cells in one
    // table only
    bool selection_only_covers_text();
    bool selection_only_covers_table();

    // based on current range or - error if there isn't one
    void cut(int& err);
    void copy(int& err);

    // based on current object buffer - error if empty
    void paste(int& err);

    // current document
    mx_wp_document* get_document() { return document; };

    // current position
    mx_wp_doc_pos* get_position() { return &position; };

    // insert a new paragraph. position is left pointing at first position
    // in the new paragraph
    mx_paragraph* insert_paragraph(int& err);

    // insert after the current position. This only makes a difference for
    // blank paragraphs where there is no logical difference start and end.
    mx_paragraph* insert_paragraph_after(int& err);

    // insert a new table area
    mx_table_area* insert_table(int& err);

    // insert a complex word item of some sort. The item is not copied so do
    // not delete it. Use this to insert images/diagrams
    void insert_cw_item(int& err, mx_cw_t* item);

    // call these before/after the view scrolls
    void pre_scroll();
    void post_scroll();

    // these are called before and after the document has been redrawn
    void pre_redraw();
    void post_redraw();

    // make sure that the caret is displayed in the right place - use
    // after a reformat to get the cursor going again
    void update_caret(int& err);

    void up(int& err, bool is_shift);
    void down(int& err, bool is_shift);
    void left(int& err, bool is_shift);
    void right(int& err, bool is_shift);
    void handle_return(int& err, bool is_shift);
    void backspace(int& err);
    void handle_delete(int& err);

    // routines for inserting things.  these don't do any reformatting or
    // similar - reformat the document when you've finished inserting stuff
    void insert_char(int& err, char c);
    void insert_text(int& err, char* s);
    void insert_space(int& err);
    void insert_tab(int& err);

    void insert_para_break(int& err);

    // does the whole screen need to be refreshed after inserting something?
    bool need_to_refresh_all();

    // was the last key event a cursor movement event
    bool last_key_event_was_cursor_movement();

    // make the current cursor position visible within the frame
    void make_visible(int& err);

    // the usable width (sheet width - margins)
    float get_sheet_width(int& err);

    // the usable height (sheet height - margins)
    float get_sheet_height(int& err);

    // get the frame
    mx_frame* get_frame();

    // get the device
    mx_device* get_device();

    // the area the cursor is currently in
    mx_area* current_area(int& err);

    // called after the sheet size has changed to tell the cursor to
    // recalculate its view correction
    void sheet_size_changed(int& err);

    // insert bullets into the current selection
    void insert_bullets(int& err);

    // get the width of table columns in the current selection. If columns of
    // more than one width are selected, -1.0 is returned.
    float get_selected_column_width(int& err);

    // set the width of table columns in the current selection
    void set_selected_column_width(int& err, float w);

    // select the whole document
    void select_all(int& err);

    // is the selection within one area?
    bool selection_within_area(int& err);

    // get the area pointed at by the start of the selection
    mx_area* get_start_area(int& err);

    // get the area pointed at by the end of the selection
    mx_area* get_end_area(int& err);

    // get the area position pointed at by the start of the selection
    mx_area_pos* get_start_area_pos(int& err);

    // get the area position pointed at by the end of the selection
    mx_area_pos* get_end_area_pos(int& err);

    // cut table columns from current position/selection
    void cut_table_columns(int& err);

    // cut table rows from current position/selection
    void cut_table_rows(int& err);

    // insert n table rows before the current position
    void insert_table_rows_before(int& err, int n);

    // insert n table rows after the current position
    void insert_table_rows_after(int& err, int n);

    // same for columns
    void insert_table_columns_before(int& err, int n);
    void insert_table_columns_after(int& err, int n);

    // find the view correction
    const mx_point& get_view_correction();

    // repaginate the document
    void repaginate(int& err);

    // last cut or copy
    // split the table at the current row
    mx_table_area* split_table(int& err);

    // insert a hard page break
    void insert_page_break(int& err);

    // this method applies a paragraph style mod to a selection, or if there is
    // no selection, applies the char_style mod to the current style ready for
    // the next character to be inserted
    void apply_mod_to_current_style(int& err, const mx_paragraph_style_mod& mod);

    // same again but for table paraghraph styles
    void apply_mod_to_current_table_style(int& err, const mx_paragraph_style_mod& mod);

    // add or remove a tab at the given position in the current selection
    enum mx_tab_change { mx_add_tab_e,
        mx_remove_tab_e };
    void apply_tab_change_to_current_style(int& err, mx_tab_change op,
        const mx_tabstop& tab,
        float tolerance);

    // change the default style in the current selection
    void apply_style_change_to_current_style(int& err,
        const char* new_style_name);

    // delete the class variable buffer, used for cut/paste. Only used just
    // before shutdown.
    static void delete_buffer();

    // if there is a selection, remove it
    void remove_selection(int& err);

    // get the buffer for this cursor - do not delete it..
    mx_wp_object_buffer* get_buffer();

    // get the selection as text - used for external selections
    const char* get_selection_as_text(int& err);

    // reformat the document, saving and restoring the document positions
    void reformat_and_restore_position(int& err,
        mx_wp_doc_pos& start_pos,
        mx_wp_doc_pos& end_pos,
        bool sheets_deleted = FALSE);

    // scale or crop the currently active area
    void scale_active_area(int& err,
        mx_area* active_area,
        const mx_point& new_size);
    void crop_active_area(int& err,
        mx_area* active_area,
        const mx_rect& crop_rectangle);

private:
    // start/end of any selection
    mx_wp_doc_pos start;

    // end is a temporary variable used in incremental selections
    mx_wp_doc_pos end;

    // current position
    mx_wp_doc_pos position;

    // is there a selection?
    bool is_selection;

    // current document
    mx_wp_document* document;

    // buffer for overwrite ops
    mx_wp_object_buffer* temp_cut_buffer;
    mx_wp_document* temp_cut_document;

    // layout in the current frame
    mx_wp_layout* layout;

    // current frame used to display the document
    mx_frame* frame;

    // the screen device
    mx_screen_device* device;

    // ui stuff for dealing with an active area
    mx_active_area* active_area;

    void update_selection(int& err, bool is_extend);
    void draw_selection(int& err);

    void handle_normal_key(int& err, mx_key_event& event);
    void handle_special_key(int& err, mx_key_event& event);
    void get_caret_offsets(int& err, float& height, float& depth);
    void calculate_view_correction(int& err, int sheet_number);

    void get_caret_height_for_text_area(int& err,
        float& depth,
        float& height,
        mx_text_area* area,
        mx_text_area_pos* ap);

    void get_caret_height_for_table_area(int& err,
        float& depth,
        float& height,
        mx_table_area* area,
        mx_table_area_pos* ap);

    mx_point view_correction;
    mx_painter_mode_t current_view_mode;

    int last_corrected_sheet_number;

    mx_paragraph* insert_paragraph_into_text_area(
        int& err,
        mx_text_area_pos* ap,
        mx_text_area* a);

    mx_paragraph* insert_paragraph_into_table_area(
        int& err,
        mx_table_area_pos* ap,
        mx_table_area* a);

    mx_paragraph* insert_paragraph_into_opaque_area(
        int& err,
        mx_wp_sheet_pos& sp,
        mx_sheet* s,
        mx_area* a);

    mx_text_area* add_text_area(int& err, mx_sheet* s, int i);

    mx_paragraph* insert_paragraph_right_of_opaque_area(
        int& err,
        mx_wp_sheet_pos& sp,
        mx_sheet* s);

    mx_paragraph* insert_paragraph_left_of_opaque_area(
        int& err,
        mx_wp_sheet_pos& sp,
        mx_sheet* s);

    void cut_into_buffer(int& err, mx_wp_object_buffer& buffer);

    void recalculate_layout(int& err);

    float cut_sheet_width;
    float cut_sheet_height;
    float cut_sheet_left_margin;
    float cut_sheet_right_margin;
    float cut_sheet_top_margin;
    float cut_sheet_bottom_margin;

    void check_for_empty_document(int& err);
    void store_sheet_dimensions(int& err);

    void update_selection_contents_flags(int& err);

    bool table_in_selection : 1;
    bool text_in_selection : 1;
    bool multi_column_in_selection : 1;
    bool diagram_in_selection : 1;
    bool image_in_selection : 1;
    bool text_only_selection : 1;
    bool table_only_selection : 1;

    // make sure start is before position
    void sort_range();

    void insert_text_area_before_table(int& err);

    // do we need to refresh the whole screen
    bool need_to_refresh_whole_screen;

    // force a refresh to occur by setting need_to_refresh_whole_screen
    void force_refresh_whole_screen();

    // after a key event, this variable indicates whether the last key event
    // was one which moves the cursor
    bool key_event_was_cursor_movement;

    // current style at the position, plus any modifications set via the
    // apply_mod_to_current_style method
    mx_char_style current_style;

    // applies the given border style mod to the current selection. The way
    // this works depends upon what is selected. It is called by the method of
    // the same name which works with paragraph style mods
    void apply_mod_to_current_style(int& err, const mx_border_style_mod& mod);

    static mx_wp_document* buffer_doc;
    static mx_wp_object_buffer* buffer;

    mx_undo_redo_buffer* ur_buffer;

    void remove_whole_table(int& err);

    // saves the whole of the current selection so that style changes can
    // be undone. This does not work for table borders which are handled
    // separately
    void save_selection_for_undo(int& err);

    // save the area styles in the current selection
    void save_area_styles_for_undo(int& err);

    // same thing for paragraph and table borders
    void save_table_borders_for_undo(int& err);
    void save_para_borders_for_undo(int& err);

    // variable which store the last x position of the cursor, so that when
    // up/down keys are pressed the cursor tries to go to the correct position
    float saved_x_pos;
    bool save_x_pos;

    // method for moving the cursor up or down. Called by up() and down()
    enum mx_vertical_direction { up_e,
        down_e };
    void move_vertical(int& err, mx_vertical_direction d, bool is_shift);

    // method which fixes a repaginated-split paragraph at the given position
    void fix_split_paragraph(int& err);

    // method which "fixes" the given selection positions, ie, so that
    // selections in tables select whole rows and columns
    void fix_selection_positions(int& err);

    void find_table_bounds(int& err,
        mx_wp_doc_pos& start_pos,
        mx_wp_doc_pos& end_pos);
};
#endif
