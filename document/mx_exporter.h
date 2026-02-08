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
#ifndef MX_EXPORTER_H
#define MX_EXPORTER_H
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

#include <mx.h>
#include <mx_sl_obj.h>
#include <mx_wp_doc.h>

class mx_dialog_manager;
class mx_prog_d;

class mx_exporter : public mx_rtti {
    MX_RTTI(mx_exporter_class_e)

public:
    mx_exporter(int& err,
        const char* f,
        mx_wp_document* doc,
        mx_dialog_manager* dm = NULL);
    virtual ~mx_exporter();

    // do the actual export
    virtual void export_doc(int& err) = 0;

protected:
    FILE* file;
    mx_wp_document* doc;

    void set_output_length(int i);
    bool update_progress();
    bool update_progress(int output_pos);

    // do post export stuff, like deactivating the progress dialogue
    virtual void finish_export();

private:
    int output_length;

    const char* file_name;
    mx_dialog_manager* dm;
    mx_prog_d* pd;
};

#endif
