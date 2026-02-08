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
 * MODULE/CLASS : mx_exporter
 *
 * AUTHOR : Andrew Haisley
 *
 *
 *
 * DESCRIPTION:
 *
 * Things which export maxwell documents
 *
 *
 *
 */

#include <mx_dialog_man.h>
#include <mx_exporter.h>
#include <mx_prog_d.h>

mx_exporter::mx_exporter(
    int& err,
    const char* f,
    mx_wp_document* doc,
    mx_dialog_manager* dm)
{
    file_name = f;
    file = mx_fopen(err, f, "w");
    MX_ERROR_CHECK(err);

    mx_exporter::doc = doc;
    mx_exporter::dm = dm;

    if (dm == NULL) {
        pd = NULL;
    } else {
        pd = dm->get_prog_bar_d();
    }
abort:;
}

mx_exporter::~mx_exporter()
{
    fclose(file);
}

void mx_exporter::set_output_length(int i)
{
    output_length = i;
}

bool mx_exporter::update_progress()
{
    if (pd == NULL) {
        return FALSE;
    } else {
        return pd->peek_cancel();
    }
}

bool mx_exporter::update_progress(int output_pos)
{
    if (pd == NULL) {
        return FALSE;
    } else {
        float percent;

        if (!pd->is_active()) {
            char message_str[2000] = "Exporting file: ";
            strcat(message_str, file_name);

            pd->centre();
            pd->activate_d(message_str);
        }

        percent = 100.0 * (float)output_pos / (float)output_length;

        pd->set_progress((int)percent);
        return pd->peek_cancel();
    }
}

void mx_exporter::finish_export()
{
    if (pd != NULL)
        pd->deactivate();
}
