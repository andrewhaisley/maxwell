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

#include <string.h>

#include <iostream>
#include <filesystem>

#include <X11/Intrinsic.h>
#include <X11/Shell.h>
#include <Xm/DialogS.h>
#include <Xm/LabelG.h>
#include <Xm/MwmUtil.h>
#include <Xm/PanedW.h>
#include <Xm/Protocols.h>
#include <Xm/PushBG.h>
#include <Xm/RowColumn.h>
#include <Xm/VendorS.h>
#include <Xm/Xm.h>

#include <mx_app.h>
#include <maxwell.h>
#include <mx.h>
#include <mx_db.h>
#include <mx_doc.h>
#include <mx_list_iter.h>

#include <mx_editor.h>
#include <mx_sc_device.h>
#include <mx_wp_editor.h>
#include <mx_window.h>

mx_app* mx_app::the_app = NULL;

using namespace std;

// creates the instance of the app
mx_app* mx_app::init()
{
    if (the_app == NULL) {
        the_app = new mx_app();
    }

    the_app->build_main_window(global_top_level);
    XtRealizeWidget(global_top_level);
    return the_app;
}

mx_app::mx_app() : mx_ui_object()
{
    widget = global_top_level;
    wp_editors = new mx_list;
}

mx_app::~mx_app()
{
    delete wp_editors;
}

// runs the app - only returns on the quit button being pressed
void mx_app::run(int& err)
{
    Atom wmDeleteMessage = XInternAtom(XtDisplay(global_top_level), "WM_DELETE_WINDOW", false);

    while (true) {
        XEvent event;
        XtAppNextEvent(global_app_context, &event);

        if (event.type == ClientMessage) {
            if ((Atom)event.xclient.data.l[0] == wmDeleteMessage) {
                auto e = mx_editor::get_editor_for_window(event.xclient.window);
                if (e)  {
                    e->window_is_closing();
                }
            }
        }

        XtDispatchEvent(&event);

        if (XtAppGetExitFlag(global_app_context)) {
            return;
        }
    }
}

// explict call to edit a document in a new editor window - call this
// to edit command line option documents or documents selected from
// another wp_editor
void mx_app::new_wp_editor(int& err, mx_wp_document* doc)
{
    mx_wp_editor* ed;

    mx_ui_object::set_busy();
    ed = new mx_wp_editor(err, *this, doc);
    MX_ERROR_CHECK(err);

    wp_editors->append((void*)ed);
abort:
    mx_ui_object::clear_busy();
}

// is a named document currently being edited by one of the editors the
// app knows about?
bool mx_app::is_doc_open(char* file_name)
{
    mx_list_iterator iter(*wp_editors);
    mx_wp_editor* ed;

    std::string s = std::filesystem::canonical(file_name);

    while (iter.more()) {
        ed = (mx_wp_editor*)iter.data();

        if (ed->get_document() != NULL) {
            if (ed->get_document()->get_full_file_name() == s) {
                return true;
            }
        }
    }
    return false;
}

// if the doc is open, bring the editor dealing with it to the front
void mx_app::promote_doc_editor(char* file_name)
{
    mx_list_iterator iter(*wp_editors);
    mx_wp_editor* ed;

    std::string s = std::filesystem::canonical(file_name);

    while (iter.more()) {
        ed = (mx_wp_editor*)iter.data();

        if (ed->get_document()->get_full_file_name() == s) {
            ed->activate();
            return;
        }
    }
}

// build the maxwell main window - icon + start button for each app.
void mx_app::build_main_window(Widget main_window)
{
    Widget button, rcw;

    XtVaSetValues(main_window,
        XmNnoResize, True,
        XmNmwmDecorations,
        MWM_DECOR_BORDER | MWM_DECOR_TITLE,
        XmNmwmFunctions,
        MWM_FUNC_MOVE | MWM_DECOR_MINIMIZE,
        NULL);

    rcw = XtVaCreateManagedWidget(
        "rowColumn",
        xmRowColumnWidgetClass,
        main_window,
        XmNentryAlignment, XmALIGNMENT_CENTER,
        NULL);

    (void)XtVaCreateManagedWidget("dummy1", xmLabelGadgetClass, rcw, NULL);
    (void)XtVaCreateManagedWidget("dummy2", xmLabelGadgetClass, rcw, NULL);
    (void)XtVaCreateManagedWidget("dummy3", xmLabelGadgetClass, rcw, NULL);
    (void)XtVaCreateManagedWidget("dummy4", xmLabelGadgetClass, rcw, NULL);
    (void)XtVaCreateManagedWidget("dummy5", xmLabelGadgetClass, rcw, NULL);
    (void)XtVaCreateManagedWidget("dummy6", xmLabelGadgetClass, rcw, NULL);
    (void)XtVaCreateManagedWidget("dummy7", xmLabelGadgetClass, rcw, NULL);
    (void)XtVaCreateManagedWidget("dummy8", xmLabelGadgetClass, rcw, NULL);
    (void)XtVaCreateManagedWidget("dummy9", xmLabelGadgetClass, rcw, NULL);
    (void)XtVaCreateManagedWidget("dummy10", xmLabelGadgetClass, rcw, NULL);

    button = XtVaCreateManagedWidget(
        "startWP",
        xmPushButtonGadgetClass,
        rcw,
        XmNshowAsDefault, True,
        XmNdefaultButtonShadowThickness, 1,
        NULL);

    XtAddCallback(button, XmNactivateCallback, start_wp_cb, this);

    button = XtVaCreateManagedWidget(
        "quitMaxwell",
        xmPushButtonGadgetClass,
        rcw,
        XmNshowAsDefault, False,
        NULL);

    XtAddCallback(button, XmNactivateCallback, quit_cb, this);
}

void mx_app::stop(int& err)
{
    mx_wp_editor* ed;

    // go over the editors shutting them down forceably.....
    while (wp_editors->get_num_items() > 0) {
        ed = (mx_wp_editor*)wp_editors->get(err, 0);

        // the editor destroys itself, so don't do need to do it here as well
        ed->window_is_closing();
    }

    // also make the screen device clear its memory allocations
    mx_screen_device::unloadFontsAndColours();

    // make the cursor deallocate its cut/paste buffer
    mx_wp_cursor::delete_buffer();

    delete global_user_config;
    global_user_config = NULL;
}

void mx_app::start_wp_cb(Widget w, XtPointer client_data, XtPointer call_data)
{
    int err = MX_ERROR_OK;
    mx_wp_document* doc;
    const char* s;

    mx_app* app = (mx_app*)client_data;

    app->mx_ui_object::set_busy();

    s = global_user_config->get_default_string(err, "page", "A4");
    MX_ERROR_CHECK(err);

    doc = mx_db_client_open_temporary_wp_doc(err, (char*)s);
    MX_ERROR_CHECK(err);

    app->new_wp_editor(err, doc);
    MX_ERROR_CHECK(err);

    return;

abort:;
    global_error_trace->print();
}

void mx_app::quit_cb(Widget w, XtPointer client_data, XtPointer call_data)
{
    XtAppSetExitFlag(global_app_context);
}

void mx_app::unlock()
{
}

void mx_app::wp_editor_is_exiting(int& err, mx_wp_editor* ed)
{
    int32 i;

    i = wp_editors->find_index(err, (void*)ed);
    MX_ERROR_CHECK(err);

    (void)wp_editors->remove(err, i);
    MX_ERROR_CHECK(err);

abort:;
}

void mx_app::config_changed()
{
    mx_list_iterator iter(*wp_editors);
    mx_wp_editor* ed;

    while (iter.more()) {
        ed = (mx_wp_editor*)iter.data();
        ed->config_changed();
    }
}
