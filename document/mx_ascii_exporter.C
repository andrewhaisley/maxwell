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
 * MODULE/CLASS : mx_ascii_exporter
 *
 * AUTHOR : Andrew Haisley
 *
 *
 *
 * DESCRIPTION:
 *
 * ASCII export for maxwell documents
 *
 *
 *
 */

#include <mx.h>
#include <mx_ascii_exporter.h>
#include <mx_break_w.h>
#include <mx_document.h>
#include <mx_sl_obj.h>
#include <mx_space_w.h>
#include <mx_word_iter.h>
#include <mx_wp_pos.h>

mx_ascii_exporter::mx_ascii_exporter(
    int& err,
    const char* f,
    mx_wp_document* doc,
    mx_dialog_manager* dm)
    : mx_exporter(err, f, doc, dm)
{
    MX_ERROR_CHECK(err);
abort:;
}

mx_ascii_exporter::~mx_ascii_exporter()
{
}

void mx_ascii_exporter::export_doc(int& err)
{
    mx_wp_doc_pos start, end;

    start.moveto_start(err, doc);
    MX_ERROR_CHECK(err);

    end.moveto_end(err, doc);
    MX_ERROR_CHECK(err);

    {
        mx_word_iterator iter(doc, start, end, TRUE);
        mx_word* w;
        mx_break_word* bw;
        mx_space_word* sw;

        mx_space_word::mx_space_word_item_t it;

        int i;

        while (iter.more(err)) {
            MX_ERROR_CHECK(err);

            w = iter.data(err);
            MX_ERROR_CHECK(err);

            switch (w->rtti_class()) {
            case mx_break_word_class_e:
                bw = (mx_break_word*)w;

                if ((bw->type() == mx_paragraph_break_e) || (bw->type() == mx_line_break_e)) {
                    mx_fprintf(err, file, "\n");
                    MX_ERROR_CHECK(err);
                }
                break;

            case mx_space_word_class_e:
                sw = (mx_space_word*)w;

                for (i = 0; i < sw->get_num_items(); i++) {
                    it = sw->item_type(err, i);
                    MX_ERROR_CHECK(err);

                    if (it == mx_space_word::mx_space_word_space_e) {
                        mx_fprintf(err, file, " ");
                        MX_ERROR_CHECK(err);
                    } else {
                        if (it == mx_space_word::mx_space_word_tab_e) {
                            mx_fprintf(err, file, "\t");
                            MX_ERROR_CHECK(err);
                        }
                    }
                }
                break;

            default:
            case mx_complex_word_class_e:
            case mx_simple_word_class_e:
                mx_fprintf(err, file, "%s", w->cstring());
                MX_ERROR_CHECK(err);
                break;
            }
        }
        MX_ERROR_CHECK(err);
    }
abort:;
}
