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
 * MODULE/CLASS : mx_editor
 *
 * AUTHOR : Andrew Haisley
 *
 *
 *
 * DESCRIPTION:
 *
 * Implementation for mx_editor class
 *
 *
 *
 *
 */

#include <cups/cups.h>

#include "mx_editor.h"
#include <mx.h>
#include <mx_ui.h>

#include <mx_app.h>
#include <mx_pframe.h>
#include <mx_pr_device.h>
#include <mx_print_d.h>

using namespace std;

std::map<Window, mx_editor *> mx_editor::m_editors;


mx_editor *mx_editor::get_editor_for_window(Window w)
{
    if (m_editors.find(w) == m_editors.end()) {
        return nullptr;
    } else {
        return m_editors[w];
    }
}

mx_editor::mx_editor(mx_app& a, mx_sheet_layout* layout) : mx_frame_target(layout)
{
    init(a);
}

mx_editor::~mx_editor()
{
    if (current_printer_name)
        delete[] current_printer_name;
    if (current_page_size)
        delete[] current_page_size;
    if (current_envelope_size)
        delete[] current_envelope_size;
    if (current_language)
        delete[] current_language;

    m_editors.erase(m_window);
}

void mx_editor::init(mx_app& a)
{
    window = new mx_window(this, "maxwell");
    app = &a;
    dialog_man.set_parent(window->get_widget());

    current_printer_name = nullptr;
    current_page_size = nullptr;
    current_envelope_size = nullptr;
    current_language = nullptr;
    current_zoom = 1.0;

    read_config();
}

void mx_editor::window_is_closing()
{
}

void mx_editor::activate()
{
    window->deiconise();
}

// attempts to open a document - can fail for many reasons. Returns nullptr if
// it does.
mx_document* mx_editor::open_document(int& err, char* file_name)
{
    mx_document* res;

    // is it already open within this application?
    if (app->is_doc_open(file_name)) {
        dialog_man.run_inform_d(inform_e, "openAlready");

        app->promote_doc_editor(file_name);
        return nullptr;
    }

    // try to open without recover
    res = open_derived_doc_class(err, file_name, false);

    while (true) {
        switch (err) {
        case MX_FILE_UNRECOVERABLE:
            dialog_man.run_inform_d(inform_e, "fileUnrecoverable");
            MX_ERROR_CLEAR(err);
            return nullptr;

        case MX_FILE_UNIMPORTABLE:
            dialog_man.run_inform_d(inform_e, "cantImportFileType");
            MX_ERROR_CLEAR(err);
            return nullptr;

        case MX_FILE_NEEDS_RECOVER:
            MX_ERROR_CLEAR(err);
            if (dialog_man.run_yes_no_d("damaged", false, yes_e) == yes_e) {
                res = open_derived_doc_class(err, file_name, true);
                break;
            } else {
                return nullptr;
            }

        case MX_ERROR_OK:
            return res;

        case MX_USER_CANCEL:
            MX_ERROR_CLEAR(err);
            return nullptr;

        default:
            MX_ERROR_CHECK(err);
        }
    }

abort:;
    return nullptr;
}

void mx_editor::file_new(int& err)
{
}

void mx_editor::file_save(int& err)
{
    if (get_document()->get_temp_flag()) {
        file_save_as(err);
        MX_ERROR_CHECK(err);
    } else {
        get_document()->commit(err);
        MX_ERROR_CHECK(err);
    }
abort:;
}

void mx_editor::file_save_as(int& err)
{
    char template_dir[MAX_PATH_LEN];
    mx_save_d* d;

    sprintf(template_dir, "%s/template", global_maxhome);

    d = dialog_man.get_save_d(template_dir);

    if (d->run() == yes_e) {
        if (d->save_file_type == mx_document_file_e) {
            get_document()->commit_as(err, d->file_name());
            MX_ERROR_CHECK(err);

            // not temporary any more
            get_document()->set_temp_flag(false);
        } else {
            if (d->save_file_type == mx_template_e) {
                sprintf(template_dir, "%s/template/%s",
                    global_maxhome,
                    d->file_name());

                get_document()->commit_as(err, template_dir);
                MX_ERROR_CHECK(err);

                // not temporary any more
                get_document()->set_temp_flag(false);
            } else {
                file_export(err, d->file_name(), d->save_file_type);
                MX_ERROR_CHECK(err);
            }
        }
    }
abort:;
}

void mx_editor::set_print_device_size(int& err, mx_print_frame* print_frame)
{
    mx_sheet* s;

    float width, height;
    mx_point size(1, 1);

    s = get_document()->sheet(err, 0);
    MX_ERROR_CHECK(err);

    width = s->get_width(err);
    MX_ERROR_CHECK(err);

    height = s->get_height(err);
    MX_ERROR_CHECK(err);

    size.x = width;
    size.y = height;

    print_frame->setSize(err, size);
    MX_ERROR_CHECK(err);

abort:;
}

void mx_editor::print_page(int& err, int i, mx_print_frame* print_frame)
{
    mx_print_device* dev;
    mx_sheet* s;

    float w, h;
    bool b;

    dev = (mx_print_device*)print_frame->getDevice();

    s = get_document()->sheet(err, i);
    MX_ERROR_CHECK(err);

    w = s->get_width(err);
    MX_ERROR_CHECK(err);

    h = s->get_height(err);
    MX_ERROR_CHECK(err);

    b = s->get_landscape(err);
    MX_ERROR_CHECK(err);

    dev->startPage(err, i + 1, w, h, b);
    MX_ERROR_CHECK(err);

    print_frame->printSheet(err, i);
    MX_ERROR_CHECK(err);

    dev->endPage(err);
    MX_ERROR_CHECK(err);

abort:;
}

void mx_editor::print_copies(int& err, const char* file_name, const char* printer_name, int num_copies)
{
    if (cupsPrintFile(printer_name, file_name, "maxwell document", 0, nullptr) == 0) {
        fprintf(stderr, "print job failed %s, %s: %s\n", printer_name, file_name, cupsLastErrorString());
    }
}

void mx_editor::file_print(int& err, int current_page, mx_print_frame* print_frame)
{
    mx_print_device* dev;

    mx_print_d* d;
    char out_file_name[MAX_PATH_LEN];
    int i, num_pages, num_copies = 0;

    dev = (mx_print_device*)print_frame->getDevice();
    dev->startPrint();

    d = dialog_man.get_print_d();

    num_pages = get_document()->get_num_sheets(err);
    MX_ERROR_CHECK(err);

    if (d->run(num_pages, global_user_config, true) == yes_e) {
        if (d->to_file) {
            strcpy(out_file_name, d->selected_file);
        } else {
            mx_tmpnam(out_file_name);
        }

        dev->setOutputFile(out_file_name);

        set_print_device_size(err, print_frame);
        MX_ERROR_CHECK(err);

        num_pages = get_num_pages_to_print(err, d);
        MX_ERROR_CHECK(err);

        if (d->collate) {
            num_pages *= num_copies;
            num_copies = 1;
        } else {
            num_copies = d->num_copies;
        }

        switch (d->range) {
        default:
        case mx_print_d::print_all_e:
            print_range(err, 1, num_pages, d, print_frame);
            MX_ERROR_CHECK(err);
            break;

        case mx_print_d::print_range_e:
            print_range(err, d->first_page, d->last_page, d, print_frame);
            MX_ERROR_CHECK(err);
            break;

        case mx_print_d::print_current_e:
            print_page(err, current_page, print_frame);
            MX_ERROR_CHECK(err);
            break;

        case mx_print_d::print_list_e:
            for (i = 0; i < d->num_pages; i++) {
                print_page(err, d->pages_to_print[i] - 1, print_frame);
                MX_ERROR_CHECK(err);
            }
            break;
        }

        dev->endPrint();

        if (!d->to_file) {
            print_copies(err, out_file_name, d->selected_printer.c_str(), num_copies);
            MX_ERROR_CHECK(err);
        }
    }

abort:;
    dev->endPrint();
}

bool mx_editor::file_close(int& err)
{
    return false;
}

bool mx_editor::file_force_close(int& err)
{
    return false;
}

void mx_editor::help_support(int& err)
{
    dialog_man.get_support_d()->run();
}

void mx_editor::tools_options(int& err)
{
    mx_opt_d* d = dialog_man.get_opt_d();

    if (d->run(global_user_config) == yes_e) {
        app->config_changed();
    }
}

void mx_editor::config_changed()
{
    // re-read the user config
    read_config();
}

void mx_editor::read_config()
{
    string s;

    int err = MX_ERROR_OK;

    if (current_printer_name != nullptr) {
        delete[] current_printer_name;
    }

    if (current_page_size != nullptr) {
        delete[] current_page_size;
    }

    if (current_envelope_size != nullptr) {
        delete[] current_envelope_size;
    }

    if (current_language != nullptr) {
        delete[] current_language;
    }

    s = global_user_config->get_default_string(err, "units", "mm");
    MX_ERROR_CHECK(err);

    current_units = mx_unit_type(s.c_str());

    s = global_user_config->get_default_string(err, "printer", "lp");
    MX_ERROR_CHECK(err);

    current_printer_name = mx_string_copy(s.c_str());

    s = global_user_config->get_default_string(err, "page", "A4");
    MX_ERROR_CHECK(err);

    current_page_size = mx_string_copy(s.c_str());

    s = global_user_config->get_default_string(err, "envelope", "DL");
    MX_ERROR_CHECK(err);

    current_envelope_size = mx_string_copy(s.c_str());

    s = global_user_config->get_default_string(err, "language", "english");
    MX_ERROR_CHECK(err);

    current_language = mx_string_copy(s.c_str());

    return;

abort:
    global_error_trace->print();
    MX_ERROR_CLEAR(err);
}

int mx_editor::get_num_pages_to_print(int& err, mx_print_d* d)
{
    int n;

    n = get_document()->get_num_sheets(err);
    MX_ERROR_CHECK(err);

    switch (d->range) {
    default:
    case mx_print_d::print_all_e:
        return modify_for_parity(d, 1, n);
    case mx_print_d::print_range_e:
        return modify_for_parity(d, d->first_page, d->last_page);
    case mx_print_d::print_current_e:
        return 1;
    case mx_print_d::print_list_e:
        return d->num_pages;
    }
abort:
    return 0;
}

int mx_editor::modify_for_parity(mx_print_d* d, int start, int end)
{
    int i, n = 0;
    switch (d->parity) {
    default:
    case mx_print_d::print_both_e:
        return end - start + 1;
    case mx_print_d::print_odd_e:
        // ok, this is crap but at least it's simple to get
        // right
        for (i = start; i <= end; i++) {
            if ((i & 0x1) != 0) {
                n++;
            }
        }
        return n;
    case mx_print_d::print_even_e:
        for (i = start; i <= end; i++) {
            if ((i & 0x1) == 0) {
                n++;
            }
        }
        return n;
    }
}

void mx_editor::print_range(int& err, int start, int end, mx_print_d* d, mx_print_frame* print_frame)
{
    int i;
    mx_prog_d* prog = dialog_man.get_prog_d();
    char message[MAX_PATH_LEN];

    prog->centre();
    prog->activate_d("Printing....");

    for (i = start; i <= end; i++) {
        if (prog->peek_cancel()) {
            dialog_man.reset_prog_d();
            return;
        }

        switch (d->parity) {
        default:
        case mx_print_d::print_both_e:
            sprintf(message, "Printing page %d", i);
            prog->set_message(message);
            print_page(err, i - 1, print_frame);
            MX_ERROR_CHECK(err);
            break;
        case mx_print_d::print_odd_e:
            if ((i & 0x1) != 0) {
                sprintf(message, "Printing page %d", i);
                prog->set_message(message);

                print_page(err, i - 1, print_frame);
                MX_ERROR_CHECK(err);
            }
            break;
        case mx_print_d::print_even_e:
            if ((i & 0x1) == 0) {
                sprintf(message, "Printing page %d", i);
                prog->set_message(message);

                print_page(err, i - 1, print_frame);
                MX_ERROR_CHECK(err);
            }
            break;
        }
    }

abort:
    dialog_man.reset_prog_d();
}
