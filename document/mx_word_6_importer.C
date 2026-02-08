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
 * MODULE/CLASS : mx_word_6_importer
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
 *
 */

#include <assert.h>
#include <mx.h>
#include <mx_dialog_man.h>
#include <mx_file_type.h>
#include <mx_prog_d.h>
#include <mx_word_6.h>
#include <mx_word_6_importer.h>
#include <mx_wp_cursor.h>
#include <mx_wp_doc.h>
#include <mx_wp_pos.h>

mx_word_6_importer::mx_word_6_importer(
    int& err,
    const char* f,
    mx_wp_document* d,
    mx_wp_cursor* c,
    mx_dialog_manager* dm)
    : mx_importer(err, f)
{
    if (!mx_is_file_type(file_name, mx_word_6_file_e)) {
        MX_ERROR_THROW(err, MX_IMPORTER_BAD_FILE_TYPE);
    }

    in = new mx_word_6(err, (char*)f);
    MX_ERROR_CHECK(err);

    stream_length = in->ole_file->size(err, "WordDocument");
    MX_ERROR_CHECK(err);

    if (dm == NULL) {
        pd = NULL;
    } else {
        pd = dm->get_prog_bar_d();
    }

    doc = d;
    doc_cursor = c;

abort:;
}

mx_word_6_importer::~mx_word_6_importer()
{
    delete in;
}

void mx_word_6_importer::import(int& err)
{
    char message_str[2000] = "Importing file: ";

    strcat(message_str, file_name);

    if (pd != NULL) {
        pd->centre();
        pd->activate_d(message_str);
    }

    if (update_progress()) {
        MX_ERROR_THROW(err, MX_USER_CANCEL);
    }

    import_words(err);
    MX_ERROR_CHECK(err);

abort:
    if (pd != NULL) {
        pd->deactivate();
    }
}

bool mx_word_6_importer::update_progress()
{
    float percent;
    int file_offset, err = MX_ERROR_OK;

    file_offset = in->ole_file->tell(err);
    MX_ERROR_CLEAR(err);

    percent = 100.0 * (float)file_offset / (float)stream_length;

    if (pd == NULL) {
        return FALSE;
    } else {
        pd->set_progress((int)percent);
        return pd->peek_cancel();
    }
}

void mx_word_6_importer::import_words(int& err)
{
    in->import_into_document(err, doc, doc_cursor, pd);
    MX_ERROR_CHECK(err);
abort:;
}
