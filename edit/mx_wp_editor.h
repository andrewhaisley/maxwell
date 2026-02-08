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
#ifndef MX_WP_EDITOR
#define MX_WP_EDITOR
/*
 * MODULE/CLASS : mx_wp_editor
 *
 * AUTHOR : Andrew Haisley
 *
 *
 *
 * DESCRIPTION:
 *
 *  The mx_wp_editor class provides a word_processing editing interface
 *  to the document.
 *
 *
 *
 *
 */

#include <vector>

#include "mx_editor.h"
#include <mx_painter.h>
#include <mx_para_style.h>
#include <mx_search_d.h>
#include <mx_wp_cursor.h>
#include <mx_wp_pos.h>

// forward declarations
class mx_app;
class mx_print_target;
class mx_word_iterator;
class mx_tabstop;
class mx_wp_menubar;
class mx_wp_toolbar;
class mx_wp_statusbar;
class mx_wp_document;
class mx_wp_layout;
class mx_paragraph;
class mx_scrollable_frame;
class mx_text_area;
class mx_undo_redo_buffer;

typedef enum {
    mx_wp_editor_body_e,
    mx_wp_editor_header_e,
    mx_wp_editor_footer_e
} mx_wp_editor_view_t;

#define MX_WP_EDITOR_MAX_FIND_WORDS 100

class mx_wp_editor : public mx_editor {
    // MX_RTTI(mx_wp_editor_class_e)
public:
    virtual ~mx_wp_editor();

    // at this point, it is assumed that the file is ready to go.
    mx_wp_editor(int& err, mx_app& a, mx_wp_document* doc);

    // forced exit
    virtual void window_is_closing();

    // get the status bar
    mx_wp_statusbar* get_status_bar();

    // set what it is the editor is editing
    void set_edit_component(int& err, mx_wp_editor_view_t c);

    // things called by the frame
    virtual void draw(int& err, mx_draw_event& event);
    virtual void buttonPress(int& err, mx_button_event& event);
    virtual void externalSelection(int& err, char* s);
    virtual const char* getExternalSelection(int& err);
    virtual void scroll(int& err, mx_scroll_event& event);
    virtual void buttonDoublePress(int& err, mx_button_event& event);
    virtual void buttonTriplePress(int& err, mx_button_event& event);
    virtual void buttonMotion(int& err, mx_button_event& event);
    virtual void buttonMotionEnd(int& err, mx_button_event& event);
    virtual void scrollDrag(int& err, mx_scroll_event& event);
    virtual void keyPress(int& err, mx_key_event& event);
    virtual void keyRelease(int& err, mx_key_event& event);
    virtual void preScroll(int& err, mx_scroll_event& e);
    virtual void postScroll(int& err, mx_scroll_event& e);
    virtual void preDraw(int& err, mx_draw_event& e);
    virtual void postDraw(int& err, mx_draw_event& e);

    // called by the app if the global user config file got changed
    virtual void config_changed();

    // called by super-class to save in different formats
    virtual void file_export(int& err, char* file_name, mx_file_type_t type);

    // menubar callbacks
    virtual void file_new(int& err);
    virtual void file_open(int& err);
    virtual bool file_close(int& err);
    virtual bool file_force_close(int& err);
    virtual void file_save(int& err);
    virtual void file_save_as(int& err);
    virtual void file_print(int& err);
    virtual void file_print_one_copy(int& err);
    void help_about(int& err);
    virtual void help_support(int& err);
    void help_index(int& err);
    void view_outline(int& err, bool s);
    void view_zoom(int& err);
    void view_header(int& err);
    void view_footer(int& err);
    void view_document(int& err);
    void view_actions(int& err, bool s);
    void view_styles(int& err, bool s);
    void view_table_guides(int& err, bool s);
    void view_ruler(int& err, bool s);
    void edit_undo(int& err);
    void edit_redo(int& err);
    void edit_copy(int& err);
    void edit_cut(int& err);
    void edit_paste(int& err);
    void edit_select_all(int& err);
    void edit_find(int& err);
    void edit_find_again(int& err);
    void edit_find_and_replace(int& err);
    void edit_goto_page(int& err);
    void edit_goto_start(int& err);
    void edit_goto_end(int& err);
    void insert_page_break(int& err);
    void insert_page_number(int& err);
    void insert_date_time(int& err);
    void insert_symbol(int& err);
    void insert_image(int& err);
    void insert_diagram(int& err);
    void insert_table(int& err);
    void insert_file(int& err);
    void insert_toc(int& err);
    void insert_index(int& err);
    void insert_index_entry(int& err);
    void format_character(int& err);
    void format_paragraph(int& err);
    void format_border(int& err);
    void format_tabs(int& err);
    void format_style(int& err);
    void format_page_setup(int& err);
    void format_columns(int& err);
    void format_image(int& err);
    void format_diagram(int& err);
    void format_table(int& err);
    void format_table_style(int& err);
    void format_set_underline(int& err, bool s);
    void format_set_bold(int& err, bool s);
    void format_set_italic(int& err, bool s);
    void table_cut_rows(int& err);
    void table_cut_columns(int& err);
    void table_insert_rows(int& err);
    void table_insert_columns(int& err);
    void tools_spelling(int& err);
    void tools_repaginate(int& err);
    virtual void tools_options(int& err);
    void tools_envelope(int& err);

    // these come straight off the toolbar and have no menubar equivalent
    void set_bold(int& err, bool s);
    void set_italic(int& err, bool s);
    void set_underline(int& err, bool s);
    void set_font_size(int& err, float s);
    void set_font_family(int& err, const char* name);
    void set_justification(int& err, mx_justification_t j);
    void set_alignment(int& err, mx_align_t a);
    void set_style(int& err, char* name);
    void insert_bullets(int& err);

    // add/remove a tab at a given position. This is called from the ruler bar
    void add_tab(int& err, const mx_tabstop& tab, float position_tolerance);
    void remove_tab(int& err, float position, float position_tolerance);

    // get a pointer to the current document
    virtual mx_document* get_document();

    virtual mx_document* open_derived_doc_class(int& err, char* name, bool recover);

private:
    mx_wp_layout* layout;
    mx_painter_mode_t current_view_mode;

    // undo/redo buffer
    mx_undo_redo_buffer* ur_buffer;

    mx_undo_redo_buffer* body_ur_buffer;
    mx_undo_redo_buffer* header_ur_buffer;
    mx_undo_redo_buffer* footer_ur_buffer;

    // attempts to open a document - can fail for many reasons. Returns NULL if
    // it does. It only returns errors that it can't handle.
    mx_wp_document* open_document(int& err, char* file_name);

    // sets up a new sheet layout for the given document and return a
    // reference to it
    mx_wp_layout& set_sheet_layout(int& err, mx_wp_document* doc);

    // create the frame
    void create_frame(int& err);

    // set frame target parameters
    void setup_frame_target_params();

    // reformat etc after a cursor operation
    void cleanup_after_cursor(int& err);

    // set the contents of the toolbar
    void update_toolbar(int& err);

    // open a new temporary file and import an rtf file into it
    mx_wp_document* open_derived_import_rtf(int& err, char* name);
    mx_wp_document* open_derived_import_ascii(int& err, char* name);
    mx_wp_document* open_derived_import_msword(int& err, char* name);

    // find the width of the table cells in the given range. If there is more
    // than one column in range, returns -1.0
    float get_column_width(int& err, mx_wp_doc_pos* start, mx_wp_doc_pos* end);

    // set the width of all columns in the given range to that specified
    void set_table_column_widths(
        int& err,
        mx_wp_doc_pos* start,
        mx_wp_doc_pos* end,
        float width);

    // update all sensitivities
    void update_all_sensitivities(int& err);

    // initialise the printer frame
    void init_print_frame(int& err);

    // print a single page
    virtual void print_page(int& err, int i, mx_print_frame* print_frame);

    // update the sensitivities of the edit menu items
    void update_edit_sensitivities(int& err);

    // update the sensitivities of the file menu items
    void update_file_sensitivities(int& err);

    // update the sensitivities of the table menu items
    void update_table_sensitivities(int& err);

    // update the sensitivities of the insert menu items
    void update_insert_sensitivities(int& err);

    // update the sensitivities of the format menu items
    void update_format_sensitivities(int& err);

    // update the sensitivities of the tool menu items
    void update_tool_sensitivities(int& err);

    // set up the layout for printing stuff
    mx_wp_layout* set_print_layout(int& err);

    // refresh just the bit of the view affected by the last key press
    void refresh_changed_section(
        int& err,
        mx_paragraph* para);

    mx_wp_cursor cursor;

    mx_wp_menubar* menu;
    mx_wp_toolbar* toolbar;
    mx_wp_statusbar* statusbar;
    mx_scrollable_frame* frame;
    mx_wp_document* document;

    bool draw_table_guides;

    mx_wp_layout* print_layout;
    mx_print_target* print_target;
    mx_print_frame* print_frame;

    void do_word_search(
        int& err,
        mx_dialog* d,
        mx_wp_doc_pos& pos,
        mx_search_dir_t dir,
        char* pattern,
        bool match_case,
        bool& found);

    bool find_words(
        int& err,
        mx_dialog* d,
        char* pattern[],
        int num_words,
        bool match_case,
        mx_wp_doc_pos& start,
        mx_wp_doc_pos& end,
        mx_wp_doc_pos& word_start,
        mx_wp_doc_pos& word_end);

    int parse_pattern(int& err, char* pattern, char* patterns[]);

    void replace_all(int& err,
        char* pattern_to_replace,
        char* replacement_text,
        bool match_case);

    char last_search_pattern[MAX_SEARCH_PATTERN + 1];
    mx_search_dir_t last_search_dir;
    bool last_search_case_sensitive;

    mx_wp_editor_view_t current_edit_component;

    mx_wp_document* body_document;
    mx_wp_document* footer_document;
    mx_wp_document* header_document;

    mx_wp_doc_pos body_pos;
    mx_wp_doc_pos footer_pos;
    mx_wp_doc_pos header_pos;

    void update_headers_and_footers(int& err);
    void duplicate_sheet_size(int& err, mx_wp_document* src, mx_wp_document* dst);

    mx_text_area* extract_first_text_area(int& err, mx_wp_document* doc);
    bool text_area_contains_something(mx_text_area* a);
    void copy_footer_to_doc(int& err);
    void copy_header_to_doc(int& err);

    // this is set to true when externalSelection is called, and should be set
    // to false in the button release event which follows immediately
    bool just_received_external_selection;

    // do we want to update the toolbar on the next shift key release event
    bool update_toolbar_on_shift_release;

    // do we want to update the toolbar etc on the next draw event
    bool update_status_on_draw_event;

    void import_ascii(int& err, char* file_name);
    void import_word_6(int& err, char* file_name);
    void import_maxwell(int& err, char* file_name);

    void insert_address_text(
        int& err,
        mx_wp_document* doc,
        char* address);

    void get_address(int& err, char* address);

    int tools_spell_check_range(
        int& err,
        mx_spell_d* d,
        mx_wp_doc_pos& start,
        mx_wp_doc_pos& end);

    void replace_text(
        int& err,
        mx_wp_doc_pos& start,
        mx_wp_doc_pos& end,
        char* s);

    void replace_highlight_text(int& err, char* s);

    bool find_existing_toc(
        int& err,
        mx_wp_doc_pos& start,
        mx_wp_doc_pos& end);

    bool get_toc_para_list(int& err,
        mx_list& toc_paras,
        std::vector<int>& toc_pages);

    void insert_toc_para_list(
        int& err,
        mx_list& toc_paras,
        std::vector<int>& toc_pages,
        mx_wp_doc_pos& start,
        mx_wp_doc_pos& end);
    mx_paragraph_style* get_matching_toc_style(int& err, mx_paragraph* p);

    void update_ruler(
        int& err,
        mx_ruler** ruler_array,
        int n);

    void update_undo(int& err);
};

#endif
