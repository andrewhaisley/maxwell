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
#ifndef MX_ASCII_IMPORTER_H
#define MX_ASCII_IMPORTER_H
/*
 * MODULE/CLASS : mx_ascii_importer
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
 */

#include <mx.h>
#include <mx_importer.h>

class mx_wp_document;
class mx_wp_cursor;
class mx_dialog_manager;
class mx_prog_d;

class mx_ascii_importer : public mx_importer {
    // MX_RTTI(mx_ascii_importer_class_e)

public:
    mx_ascii_importer(
        int& err,
        const char* f,
        mx_wp_document* d,
        mx_wp_cursor* c,
        mx_dialog_manager* dm = NULL);

    mx_ascii_importer(const mx_ascii_importer&);
    virtual ~mx_ascii_importer();

    void import(int& err);

private:
    // stuff for the progress bar.
    uint32 file_length;
    mx_prog_d* pd;

    bool update_progress();

    // current document and cursor
    mx_wp_document* doc;
    mx_wp_cursor* doc_cursor;

    void import_paragraphs(int& err);

    FILE* in;
};

#endif
