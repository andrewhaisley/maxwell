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
#ifndef MX_EDITOR
#define MX_EDITOR
/*
 * MODULE/CLASS : mx_editor
 *
 * AUTHOR : Andrew Haisley
 *
 *
 *
 * DESCRIPTION:
 *
 * The mx_editor class provides very basic behaviour that all editors will
 * need.
 *
 *
 *
 *
 */

#include <map>
#include <mx.h>
#include <mx_dialog_man.h>
#include <mx_ftarget.h>

// forward declarations
class mx_app;
class mx_document;
class mx_dialog_manager;
class mx_print_frame;
class mx_print_device;
class mx_window;

class mx_editor : public mx_frame_target {
    // MX_RTTI(mx_editor_class_e)
public:
    // creates a window with toolbars and the like and manages it
    mx_editor(mx_app& owner, mx_sheet_layout* layout);
    virtual ~mx_editor();

    // called by the editors window when the user selects the close option
    // on the window menu - can also be used to force an immediate exit when
    // the whole app is closing down.
    virtual void window_is_closing();

    // de-iconise, bring to the front etc
    virtual void activate();

    // must be overidden by derived class as that will hold ont a derived
    // class of mx_document which we don't know about here. The reason for
    // this routine is to allow common document handling code (printing,
    // etc) to sit in the editor rather than get duplicated all over the shot.
    virtual mx_document* get_document() = 0;

    // must be overriden to open a document of the appropriate class for the
    // edit derived class.
    virtual mx_document* open_derived_doc_class(int& err, char* name, bool recover) = 0;

    // opens a document with all the appropriate dialogs if things go wrong.
    // uses open_derived_doc_class. Returns NULL if the document couldn't be
    // opened. If there was an unexpected error, err is set also.
    mx_document* open_document(int& err, char* file_name);

    // this is called by the application instance when the user configuration
    // is changed. The default is to do nothing, but you may need to do a
    // for example if the unit type changes.
    virtual void config_changed();

    // derived class must handle file exporting
    virtual void file_export(int& err, char* file_name, mx_file_type_t type) = 0;

    // these are needed by most editor sub-classes - you don't have to use them
    // if you want to do things differently.
    virtual void file_new(int& err);
    virtual bool file_close(int& err);
    virtual bool file_force_close(int& err);
    virtual void file_save(int& err);
    virtual void file_save_as(int& err);
    virtual void help_support(int& err);
    virtual void tools_options(int& err);

    void file_print(int& err, int page_number, mx_print_frame* frame);
    void file_print_one_copy(int& err, mx_print_frame* frame);

    // get the editor for a given window
    static mx_editor *get_editor_for_window(Window w);

protected:
    mx_window* window;
    mx_app* app;
    Window m_window; // set after the app window is realized

    mx_dialog_manager dialog_man;

    // read the user config information
    void read_config();

    // these are read from the user config file
    mx_unit_e current_units;
    char* current_printer_name;
    char* current_page_size;
    char* current_envelope_size;
    char* current_language;
    int current_printer_x_res;
    int current_printer_y_res;

    float current_zoom;

    // routines for printing
    virtual void print_page(int& err, int i, mx_print_frame* print_frame);

    void set_print_device_size(int& err, mx_print_frame* print_frame);
    void send_print_preamble(
        int& err,
        mx_print_device* dev,
        int num_pages,
        int num_copies,
        bool include_fonts);
    void send_print_postamble(int& err, mx_print_device* dev);
    void print_copies(int& err, char* name, char* printer, int num_copies);

    int get_num_pages_to_print(int& err, mx_print_d* d);
    int modify_for_parity(mx_print_d* d, int start, int end);
    void print_range(int& err, int start, int end, mx_print_d* d, mx_print_frame* print_frame);

    static std::map<Window, mx_editor *> m_editors;


private:
    void init(mx_app& a);

};

#endif // MX_EDITOR
