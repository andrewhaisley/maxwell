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
#ifndef MX_APP
#define MX_APP
/*
 * MODULE/CLASS : mx_app
 *
 * AUTHOR : Andrew Haisley
 *
 *
 *
 * DESCRIPTION:
 *
 * This module defines a class which will handle all common aspects of
 * initializing and running an application. For example saving the document
 * etc.
 *
 * One app owns many editors. Each editor has it's own window menus.
 * Editors may in turn have editors of their own - the app knows nothing
 * about these sub-editors.
 *
 * There can only ever be one app instance.
 *
 *
 *
 */

// #include <mx.h>
#include <mx_ui_object.h>

// forward declarations
class mx_wp_editor;
class mx_wp_document;
class mx_list;

class mx_app : public mx_ui_object {
public:
    // creates the instance of the app - reads config files, connects to
    // the lock manager etc
    static mx_app* init();

    // runs the app - only returns on the quit button being pressed
    void run(int& err);

    // closes things down - can't call run again after this
    void stop(int& err);

    // explict call to open a document in a new editor window - call this
    // to open up command line option documents. Is also called from the
    // editor if the user asks to opens a document.
    void new_wp_editor(int& err, mx_wp_document* doc);

    // relocks any files opened by editors that the app knows about
    void relock();

    // unlocks any files opened by editors that the app knows about
    void unlock();

    // called by a wp editor when it is exiting
    void wp_editor_is_exiting(int& err, mx_wp_editor* ed);

    // called by anyone who has changed items in the global user
    // configuration. This allows the app to tell all it's editors
    // to refresh themselves (redraw rulers in new units etc).
    void config_changed();

    // is a named document currently being edited by one of the editors the
    // app knows about?
    bool is_doc_open(char* file_name);

    // if the doc is open, bring the editor dealing with it to the front
    void promote_doc_editor(char* file_name);

    ~mx_app();

private:
    mx_app();

    static mx_app* the_app;

    void build_main_window(Widget main_window);
    void clear_busy();
    void set_busy();

    static void relock_cb(XtPointer cd, XtIntervalId* id);
    static void quit_cb(Widget w, XtPointer client_data, XtPointer call_data);
    static void start_wp_cb(Widget w, XtPointer client_data, XtPointer call_data);

    mx_list* wp_editors;
};

#endif
