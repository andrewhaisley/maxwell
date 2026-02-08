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
 * MODULE/CLASS : mx_rtf_exporter
 *
 * AUTHOR : Andrew Haisley
 *
 *
 *
 * DESCRIPTION:
 *
 * RTF export for maxwell documents
 *
 *
 *
 */

#include <mx.h>
#include <mx_rtf_exporter.h>
#include <mx_sl_obj.h>
#include <mx_wp_doc.h>

mx_rtf_exporter::mx_rtf_exporter(
    int& err,
    const char* f,
    mx_wp_document* doc,
    mx_dialog_manager* dm)
    : mx_exporter(err, f, doc, dm)
    , mx_rtfe_document_output(f)
{
    MX_ERROR_CHECK(err);
    length_is_set = FALSE;
abort:;
}

mx_rtf_exporter::~mx_rtf_exporter()
{
}

void mx_rtf_exporter::export_doc(int& err)
{
    mx_rtfe_document_output::set_document(err, doc);
    MX_ERROR_CHECK(err);

    mx_rtfe_document_output::output_rtf(err);
    MX_ERROR_CHECK(err);
abort:
    finish_export();
}

void mx_rtf_exporter::notify_export_progress(int& err,
    uint32 length, uint32 progress)
{
    if (!length_is_set) {
        set_output_length(length);
        length_is_set = TRUE;
    }

    if (update_progress(progress)) {
        MX_ERROR_THROW(err, MX_USER_CANCEL);
    }
abort:;
}
