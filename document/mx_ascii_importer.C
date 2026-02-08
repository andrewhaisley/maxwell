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

#include <assert.h>
#include <mx.h>
#include <mx_ascii_importer.h>
#include <mx_dialog_man.h>
#include <mx_file_type.h>
#include <mx_prog_d.h>
#include <mx_wp_cursor.h>
#include <mx_wp_doc.h>
#include <mx_wp_pos.h>

mx_ascii_importer::mx_ascii_importer(
    int& err,
    const char* f,
    mx_wp_document* d,
    mx_wp_cursor* c,
    mx_dialog_manager* dm)
    : mx_importer(err, f)
{
    if (!mx_is_file_type(file_name, mx_ascii_file_e)) {
        MX_ERROR_THROW(err, MX_IMPORTER_BAD_FILE_TYPE);
    }

    file_length = mx_file_size(err, file_name);
    MX_ERROR_CHECK(err);

    in = fopen(f, "r");
    if (in == NULL) {
        err = mx_translate_file_error(errno);
        goto abort;
    }

    if (dm == NULL) {
        pd = NULL;
    } else {
        pd = dm->get_prog_bar_d();
    }

    doc = d;
    doc_cursor = c;

abort:;
}

mx_ascii_importer::~mx_ascii_importer()
{
    fclose(in);
}

void mx_ascii_importer::import(int& err)
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

    import_paragraphs(err);
    MX_ERROR_CHECK(err);

abort:
    if (pd != NULL) {
        pd->deactivate();
    }
}

bool mx_ascii_importer::update_progress()
{
    float percent;
    int file_offset;

    file_offset = ftell(in);

    percent = 100.0 * (float)file_offset / (float)file_length;

    if (pd == NULL) {
        return FALSE;
    } else {
        pd->set_progress((int)percent);
        return pd->peek_cancel();
    }
}

void mx_ascii_importer::import_paragraphs(int& err)
{
    char buffer[1001];
    int i = 0, c;

    while (TRUE) {
        c = fgetc(in);

        if (c == EOF) {
            buffer[i] = 0;

            doc_cursor->insert_text(err, buffer);
            MX_ERROR_CHECK(err);

            if (update_progress()) {
                MX_ERROR_THROW(err, MX_USER_CANCEL);
            }

            return;
        }

        if (c == 13) {
            c = fgetc(in);
            if (c != 10) {
                ungetc(c, in);
            }
            buffer[i] = 0;

            doc_cursor->insert_text(err, buffer);
            MX_ERROR_CHECK(err);

            doc_cursor->insert_para_break(err);
            MX_ERROR_CHECK(err);

            if (update_progress()) {
                MX_ERROR_THROW(err, MX_USER_CANCEL);
            }

            i = 0;
        } else {
            if (c == 10) {
                buffer[i] = 0;

                doc_cursor->insert_text(err, buffer);
                MX_ERROR_CHECK(err);

                doc_cursor->insert_para_break(err);
                MX_ERROR_CHECK(err);

                if (update_progress()) {
                    MX_ERROR_THROW(err, MX_USER_CANCEL);
                }

                i = 0;
            } else {
                if (isprint(c)) {
                    buffer[i++] = c;
                    if (i == 1000) {
                        buffer[i] = 0;

                        doc_cursor->insert_text(err, buffer);
                        MX_ERROR_CHECK(err);

                        i = 0;

                        if (update_progress()) {
                            MX_ERROR_THROW(err, MX_USER_CANCEL);
                        }
                    }
                }
            }
        }
    }
abort:;
}
