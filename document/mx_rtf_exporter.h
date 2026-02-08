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
#ifndef MX_RTF_EXPORTER_H
#define MX_RTF_EXPORTER_H
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
#include <mx_exporter.h>
#include <mx_rtfe_op.h>
#include <mx_sl_obj.h>

class mx_rtf_exporter : public mx_exporter, public mx_rtfe_document_output {
    MX_RTTI(mx_rtf_exporter_class_e)

public:
    mx_rtf_exporter(
        int& err,
        const char* f,
        mx_wp_document* doc,
        mx_dialog_manager* dm = NULL);
    virtual ~mx_rtf_exporter();

    virtual void export_doc(int& err);
    virtual void notify_export_progress(int& err,
        uint32 length, uint32 progress);
    bool length_is_set;
};

#endif
