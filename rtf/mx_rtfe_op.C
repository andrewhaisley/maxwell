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
 * MODULE : mx_rtfe_op.C
 *
 * AUTHOR : Tom Newton
 *
 * This file
 *
 * DESCRIPTION:
 * a load of classes for outputting maxwell objects into an rtf file
 *
 *
 *
 *
 */

#include "mx_rtfe_conv.h"
#include "mx_rtfe_lu.h"
#include "mx_rtfe_op.h"
#include <mx_break_w.h>
#include <mx_complex_w.h>
#include <mx_cw_t.h>
#include <mx_image_area.h>
#include <mx_para_pos.h>
#include <mx_paragraph.h>
#include <mx_sheet.h>
#include <mx_simple_w.h>
#include <mx_space_w.h>
#include <mx_stdio.h>
#include <mx_text_area.h>
#include <mx_wp_doc.h>

mx_rtfe_output::mx_rtfe_output(FILE* f, mx_rtfe_lookups* lu)
    : file(f)
    , lookups(lu)
    , brace_count(0)
{
}

mx_rtfe_output::~mx_rtfe_output() { }

// disable the copy constructor - link error if you try to copy an object
// mx_rtfe_output::mx_rtfe_output(const mx_rtfe_output &o);

void mx_rtfe_output::open_brace(int& err)
{
    write_char(err, '{');
    MX_ERROR_CHECK(err);
    brace_count++;
abort:;
}

void mx_rtfe_output::close_brace(int& err)
{
    if (brace_count > 0) {
        write_char(err, '}');
        MX_ERROR_CHECK(err);
        brace_count--;
    }
abort:;
}

void mx_rtfe_output::close_all_braces()
{
    int err = MX_ERROR_OK;

    while (brace_count > 0) {
        close_brace(err);
        MX_ERROR_CHECK(err);
    }
    return;
abort:
    global_error_trace->print();
    MX_ERROR_CLEAR(err);
}

void mx_rtfe_output::write_string(int& err, const char* s)
{
    if (EOF == fputs(s, file)) {
        MX_ERROR_THROW(err, mx_translate_file_error(errno));
    }
abort:;
}

void mx_rtfe_output::write_char(int& err, char c)
{
    if (EOF == fputc(c, file)) {
        MX_ERROR_THROW(err, mx_translate_file_error(errno));
    }
abort:;
}

void mx_rtfe_output::write_rtf_string(int& err, const char* s)
{
    while (*s != 0) {
        write_rtf_char(err, *s++);
        MX_ERROR_CHECK(err);
    }
abort:;
}

void mx_rtfe_output::write_rtf_char(int& err, char cc)
{
    uint8 c = cc;

    switch (c) {
    case '\\':
        write_string(err, "\\\\");
        MX_ERROR_CHECK(err);
        break;
    case '{':
        write_string(err, "\\{");
        MX_ERROR_CHECK(err);
        break;
    case '}':
        write_string(err, "\\}");
        MX_ERROR_CHECK(err);
        break;
    default:
        if (c >= ' ') {
            if (c > 126) {
                char str[50];
                sprintf(str, "\\'%2x", c);

                write_string(err, str);
                MX_ERROR_CHECK(err);
            } else {
                write_char(err, cc);
                MX_ERROR_CHECK(err);
            }
        }
        break;
    }
abort:;
}

mx_rtfe_paragraph_output::mx_rtfe_paragraph_output(FILE* file,
    mx_rtfe_lookups* lu)
    : mx_rtfe_output(file, lu)
    , para(NULL)
    , current_base_style(NULL)
    , current_para_style(NULL)
{
}

mx_rtfe_paragraph_output::~mx_rtfe_paragraph_output() { }

// disable the copy constructor - link error if you try to copy an object
// mx_rtfe_paragraph_output::mx_rtfe_paragraph_output(const mx_rtfe_paragraph_output &op);

void mx_rtfe_paragraph_output::set_paragraph(mx_paragraph* p,
    mx_rtfe_para_type pt)
{
    para = p;
    rtf_para_type = pt;
}

void mx_rtfe_paragraph_output::output_rtf(int& err)
{
    mx_rtfe_para_style_conv psc(lookups);
    char buf[4000] = "";
    uint32 base_style_num;
    mx_paragraph_style *base_ps, *ps;
    MX_ERROR_ASSERT(err, para != NULL);

    para->minimise_style_mods(err);
    MX_ERROR_CHECK(err);

    base_ps = para->get_base_paragraph_style();
    ps = para->get_paragraph_style();

    base_style_num = lookups->styles()->get_para_style_num(err, base_ps->get_name());
    MX_ERROR_CHECK(err);

    // output any style changes

    if (current_base_style == NULL || strcmp(current_base_style, base_ps->get_name()) != 0 || *current_para_style != *ps) {
        sprintf(buf, "\\pard\\plain\\s%d", (int)base_style_num);
        write_string(err, buf);
        MX_ERROR_CHECK(err);

        psc.convert_from(err, *ps);
        MX_ERROR_CHECK(err);

        psc.output_rtf(err, file);
        MX_ERROR_CHECK(err);
    }

    if (rtf_para_type != normal_e) {
        write_string(err, "\\intbl ");
        MX_ERROR_CHECK(err);
    }

    current_base_style = base_ps->get_name();
    current_para_style = ps;

    // now output all the words

    {
        mx_word* w;
        mx_para_pos pp;
        pp.moveto_start();

        do {
            MX_ERROR_CHECK(err);

            w = para->get_word(err, pp);
            MX_ERROR_CHECK(err);

            switch (w->rtti_class()) {
            case mx_simple_word_class_e:
                write_rtf_string(err, ((mx_simple_word*)w)->cstring());
                break;
            case mx_complex_word_class_e:
                output_word_rtf(err, (mx_complex_word*)w);
                break;
            case mx_space_word_class_e:
                output_word_rtf(err, (mx_space_word*)w);
                break;
            case mx_break_word_class_e:
                output_word_rtf(err, (mx_break_word*)w);
                break;
            default:
                MX_ERROR_THROW(err, MX_ERROR_UNDEFINED);
                break;
            }
            MX_ERROR_CHECK(err);

            pp.right_word(err, para);
        } while (err != MX_POSITION_RANGE_ERROR);

        if (err == MX_POSITION_RANGE_ERROR) {
            MX_ERROR_CLEAR(err);
        } else {
            MX_ERROR_CHECK(err);
        }
    }

    close_all_braces();

abort:;
}

void mx_rtfe_paragraph_output::invalidate_current_style()
{
    current_base_style = NULL;
}

void mx_rtfe_paragraph_output::output_word_rtf(int& err, mx_complex_word* w)
{
    mx_rtfe_char_style_conv csc(lookups);
    mx_cw_t* e;
    uint32 i, num_items = w->get_num_items();

    for (i = 0; i < num_items; i++) {
        e = (mx_cw_t*)w->get_item(err, i);
        MX_ERROR_CHECK(err);

        switch (e->rtti_class()) {
        case mx_character_cw_t_class_e:
            write_rtf_char(err, ((mx_character_cw_t*)e)->c);
            MX_ERROR_CHECK(err);
            break;
        case mx_style_mod_cw_t_class_e:
            open_brace(err);
            MX_ERROR_CHECK(err);

            csc.convert_from(err, ((mx_style_mod_cw_t*)e)->m);
            MX_ERROR_CHECK(err);

            csc.output_rtf(err, file);
            MX_ERROR_CHECK(err);
            break;
        case mx_field_word_cw_t_class_e:
            this->output_word_rtf(err, &((mx_field_word_cw_t*)e)->w);
            MX_ERROR_CHECK(err);
            break;
        case mx_area_cw_t_class_e:
            this->output_area_rtf(err, ((mx_area_cw_t*)e)->a);
            MX_ERROR_CHECK(err);
            break;
        default:
            // should not be here
            MX_ERROR_THROW(err, MX_ERROR_UNDEFINED);
            break;
        }
    }
abort:;
}

void mx_rtfe_paragraph_output::output_area_rtf(int& err,
    mx_area* a)
{
    err = MX_ERROR_OK;

    switch (a->rtti_class()) {
    case mx_image_area_class_e:
        output_image_area_rtf(err,
            (mx_image_area*)a);
        MX_ERROR_CHECK(err);
        break;
    default:
        // should not be here
        MX_ERROR_THROW(err, MX_ERROR_UNDEFINED);
        break;
    }

abort:;
}

void mx_rtfe_paragraph_output::output_image_area_rtf(int& err,
    mx_image_area* a)
{
    char buffer[1000];
    mx_point originalSize;
    mx_point topLeftCrop;
    int mmWidth, mmHeight, twipWidth, twipHeight, xscale, yscale;
    int cropxt, cropyt, cropxb, cropyb;
    mx_point bottomRightCrop;
    float areaWidth, areaHeight;
    mx_mxmetafile_writer writer(a);
    mx_point scales;

    err = MX_ERROR_OK;

    open_brace(err);
    MX_ERROR_CHECK(err);

    /*
       The RTF spec isnt very clear here but playing with
       Word suggests that the sizes are the original size of the image,
       cropping values are relative to the original size. Finally any scaling
       is set independently of the original size
       */

    a->get_original_size(err, originalSize);
    MX_ERROR_CHECK(err);

    /*
      in maxwell top left cropping is measured in terms of the
      scaled raster
      */

    a->get_top_left_crop(err, topLeftCrop);
    MX_ERROR_CHECK(err);

    areaWidth = a->get_width(err);
    MX_ERROR_CHECK(err);

    areaHeight = a->get_height(err);
    MX_ERROR_CHECK(err);

    a->get_scale(err, scales);
    MX_ERROR_CHECK(err);

    /* convert area and crop to original scale */
    areaWidth /= scales.x;
    areaHeight /= scales.y;

    topLeftCrop.x /= scales.x;
    topLeftCrop.y /= scales.y;

    xscale = (int)((scales.x * 100) + 0.5);
    yscale = (int)((scales.y * 100) + 0.5);

    mmWidth = (int)((originalSize.x * 100) + 0.5);
    mmHeight = (int)((originalSize.y * 100) + 0.5);

    twipWidth = (int)(MX_MM_TO_TWIPS(originalSize.x) + 0.5);
    twipHeight = (int)(MX_MM_TO_TWIPS(originalSize.y) + 0.5);

    bottomRightCrop.x = originalSize.x + topLeftCrop.x - areaWidth;
    bottomRightCrop.y = originalSize.y + topLeftCrop.y - areaHeight;

    cropxb = (int)GNINT(MX_MM_TO_TWIPS(-topLeftCrop.x));
    cropyb = (int)GNINT(MX_MM_TO_TWIPS(-topLeftCrop.y));
    cropxt = (int)GNINT(MX_MM_TO_TWIPS(bottomRightCrop.x));
    cropyt = (int)GNINT(MX_MM_TO_TWIPS(bottomRightCrop.y));

    sprintf(buffer, "\\pict\\picscalex%d\\picscaley%d\\piccropl%d\\piccropr%d\\piccropt%d\\piccropb%d\\picw%d\\pich%d\\picwgoal%d\\pichgoal%d\\wmetafile8 \n",
        xscale,
        yscale,
        cropxb,
        cropxt,
        cropyb,
        cropyt,
        mmWidth,
        mmHeight,
        twipWidth,
        twipHeight);

    write_string(err, buffer);
    MX_ERROR_CHECK(err);

    // now we need to output the metafile

    writer.output(err);
    MX_ERROR_CHECK(err);

    write_string(err, writer.getBuffer());
    MX_ERROR_CHECK(err);

    close_brace(err);
    MX_ERROR_CHECK(err);

abort:;
}

void mx_rtfe_paragraph_output::output_word_rtf(int& err, mx_space_word* w)
{
    mx_rtfe_char_style_conv csc(lookups);
    int i, n = w->get_num_items();
    mx_space_word::mx_space_word_item_t t;

    for (i = 0; i < n; i++) {
        t = w->item_type(err, i);
        MX_ERROR_CHECK(err);

        switch (t) {
        case mx_space_word::mx_space_word_space_e:
            write_char(err, ' ');
            MX_ERROR_CHECK(err);
            break;
        case mx_space_word::mx_space_word_tab_e:
            write_string(err, "\\tab ");
            MX_ERROR_CHECK(err);
            break;
        case mx_space_word::mx_space_word_mod_e: {
            mx_char_style_mod* m = w->get(err, i);
            MX_ERROR_CHECK(err);

            open_brace(err);
            MX_ERROR_CHECK(err);

            csc.convert_from(err, *m);
            MX_ERROR_CHECK(err);

            csc.output_rtf(err, file);
            MX_ERROR_CHECK(err);
            break;
        }
        default:
            break;
        }
    }
abort:;
}

void mx_rtfe_paragraph_output::output_word_rtf(int& err, mx_break_word* w)
{
    switch (w->type()) {
    case mx_paragraph_break_e:
        if (para->get_type() == mx_paragraph::mx_paragraph_whole_e || para->get_type() == mx_paragraph::mx_paragraph_end_e) {
            const char* end_mark_type;

            switch (rtf_para_type) {
            case last_in_cell_e:
                end_mark_type = "\\cell \n";
                break;
            case last_in_row_e:
                end_mark_type = "\\cell \\row \n";
                break;
            default:
                end_mark_type = "\\par \n";
                break;
            }
            write_string(err, end_mark_type);
            MX_ERROR_CHECK(err);
        }
        break;
    case mx_line_break_e:
        write_string(err, "\\line \n");
        MX_ERROR_CHECK(err);
        break;
    case mx_long_word_break_e:
        break;
    default:
        MX_ERROR_THROW(err, MX_ERROR_UNDEFINED);
    }
abort:;
}

void mx_rtfe_paragraph_output::output_word_rtf(int& err, mx_field_word* w)
{
    char buf[500];

    switch (w->type()) {
    case mx_field_word::mx_page_num_field_e:
        sprintf(buf, "{\\field\\flddirty{\\*\\fldinst page }{\\fldrslt %s}}",
            w->cstring());
        break;
    case mx_field_word::mx_date_time_field_e: {
        const char* dtfmts[] = { "", "dd/MM/yyyy", "MM/dd/yyyy", "d MMMM, yyyy",
            "HH:mm", "hh:mm", "HH:mm", "" };
        sprintf(buf, "{\\field\\flddirty{\\*\\fldinst time \\@ \"%s %s\" }{\\fldrslt %s}}",
            dtfmts[w->date_format()],
            dtfmts[w->hour_format()],
            w->cstring());
        break;
    }
    default:
        MX_ERROR_THROW(err, MX_ERROR_UNDEFINED);
        break;
    }

    write_string(err, buf);
    MX_ERROR_CHECK(err);
abort:;
}

mx_rtfe_table_output::mx_rtfe_table_output(FILE* file, mx_rtfe_lookups* lu,
    mx_rtfe_paragraph_output* a_para_op)
    : mx_rtfe_output(file, lu)
{
    para_op = a_para_op;
    table = NULL;
}

mx_rtfe_table_output::~mx_rtfe_table_output() { }

// disable the copy constructor - link error if you try to copy an object
// mx_rtfe_table_output::mx_rtfe_table_output(const mx_rtfe_table_output &);

void mx_rtfe_table_output::set_table(mx_table_area* taba)
{
    table = taba;
}

void mx_rtfe_table_output::output_rtf(int& err)
{
    uint32 r;
    MX_ERROR_ASSERT(err, table != NULL);

    num_rows = table->get_num_rows(err);
    MX_ERROR_CHECK(err);

    num_columns = table->get_num_columns(err);
    MX_ERROR_CHECK(err);

    for (r = 0; r < num_rows; r++) {
        output_row(err, r);
        MX_ERROR_CHECK(err);
    }

abort:;
}

void mx_rtfe_table_output::output_row_formatting_rtf(int& err, uint32 row_num)
{
    char buf[4000] = "";
    char* buf_ptr = buf;
    const char* row_just;
    uint32 i;
    mx_paragraph_style* table_ps = table->get_paragraph_style();
    mx_rtfe_table_bs_conv bs_conv(lookups);

    switch (table_ps->justification) {
    case mx_right_aligned:
        row_just = "\\trqr";
        break;
    case mx_centred:
        row_just = "\\trqc";
        break;
    default:
        row_just = "\\trql";
        break;
    }

    buf_ptr += sprintf(buf_ptr, "\\trowd\\trgaph0%s\\trleft%d", row_just,
        (int)(0.5 + MX_MM_TO_TWIPS(table_ps->get_ruler()->left_indent)));

    float cellx = 0.0f;
    for (i = 0; i < num_columns; i++) {
        mx_text_area* cell = table->get_cell(err, row_num, i);
        MX_ERROR_CHECK(err);

        bs_conv.convert_from(err,
            *cell->get_paragraph_style()->get_border_style());
        MX_ERROR_CHECK(err);

        cellx += cell->get_width(err);
        MX_ERROR_CHECK(err);

        buf_ptr += sprintf(buf_ptr, "%s\\cellx%d ",
            bs_conv.get_rtf_string(),
            (int)(0.5 + MX_MM_TO_TWIPS(cellx)));
    }

    write_string(err, buf);
    MX_ERROR_CHECK(err);

abort:;
}

static bool row_formatting_same_as_previous(int& err, mx_table_area* table,
    uint32 row_num, uint32 num_columns)
{
    if (row_num == 0)
        return FALSE;

    uint32 i;
    mx_text_area *prev_cell, *cell;

    for (i = 0; i < num_columns; i++) {
        prev_cell = table->get_cell(err, row_num - 1, i);
        MX_ERROR_CHECK(err);

        cell = table->get_cell(err, row_num, i);
        MX_ERROR_CHECK(err);

        if (*prev_cell->get_paragraph_style()->get_border_style() != *cell->get_paragraph_style()->get_border_style()) {
            return FALSE;
        }
    }
    return TRUE;
abort:
    return FALSE;
}

void mx_rtfe_table_output::output_row(int& err, uint32 row_num)
{
    uint32 i;
    bool b = row_formatting_same_as_previous(err, table, row_num, num_columns);
    MX_ERROR_CHECK(err);

    if (!b) {
        this->output_row_formatting_rtf(err, row_num);
        MX_ERROR_CHECK(err);
    }

    for (i = 0; i < num_columns; i++) {
        output_cell(err, row_num, i);
        MX_ERROR_CHECK(err);
    }

abort:;
}

void mx_rtfe_table_output::output_cell(int& err, uint32 row_num,
    uint32 col_num)
{
    uint32 i, num_paras;
    mx_paragraph* para;
    mx_text_area* text;

    text = table->get_cell(err, row_num, col_num);
    MX_ERROR_CHECK(err);

    num_paras = text->num_paragraphs(err);
    MX_ERROR_CHECK(err);

    for (i = 0; i < num_paras - 1; i++) {
        para = text->get(err, i);
        MX_ERROR_CHECK(err);

        para_op->set_paragraph(para, mx_rtfe_paragraph_output::in_table_e);

        para_op->output_rtf(err);
        MX_ERROR_CHECK(err);
    }

    para = text->get(err, num_paras - 1);
    MX_ERROR_CHECK(err);

    if (col_num == num_columns - 1) {
        para_op->set_paragraph(para, mx_rtfe_paragraph_output::last_in_row_e);
    } else {
        para_op->set_paragraph(para, mx_rtfe_paragraph_output::last_in_cell_e);
    }

    para_op->output_rtf(err);
    MX_ERROR_CHECK(err);

abort:;
}

mx_rtfe_document_output::mx_rtfe_document_output(const char* output_file)
    : mx_rtfe_output(NULL, NULL)
{
    maxwell_doc = NULL;
    file_name = output_file;
}

mx_rtfe_document_output::~mx_rtfe_document_output()
{
    if (lookups)
        delete lookups;
}

// disable the copy constructor - link error if you try to copy an object
// mx_rtfe_document_output::mx_rtfe_document_output(const mx_rtfe_document_output &);

static uint32 num_areas_in_document(int& err, mx_document* doc)
{
    uint32 i, num_sheets, res = 0;
    mx_sheet* s;

    num_sheets = doc->get_num_sheets(err);
    MX_ERROR_CHECK(err);

    for (i = 0; i < num_sheets; i++) {
        s = doc->sheet(err, i);
        MX_ERROR_CHECK(err);

        res += s->get_num_areas();
    }
abort:
    return res;
}

void mx_rtfe_document_output::set_document(int& err, mx_wp_document* d)
{
    maxwell_doc = d;

    // get rid of any old lookup tables
    if (lookups)
        delete lookups;
    lookups = new mx_rtfe_lookups();

    lookups->initialise_lookups(err, maxwell_doc);
    MX_ERROR_CHECK(err);

    document_length = num_areas_in_document(err, maxwell_doc);
    MX_ERROR_CHECK(err);

    current_area_num = 0;
    last_area = NULL;
abort:;
}

void mx_rtfe_document_output::output_rtf(int& err)
{
    file = fopen(file_name, "w");
    if (file == NULL)
        MX_ERROR_THROW(err, mx_translate_file_error(errno));

    open_brace(err);
    MX_ERROR_CHECK(err);

    output_docheader_rtf(err);
    MX_ERROR_CHECK(err);

    output_document_rtf(err);
    MX_ERROR_CHECK(err);

    close_brace(err);
    MX_ERROR_CHECK(err);

    fclose(file);
    file = NULL;
abort:;
}

void mx_rtfe_document_output::notify_export_progress(int& err,
    uint32 length,
    uint32 progress)
{
}

void mx_rtfe_document_output::output_docheader_rtf(int& err)
{
    char buf[100];
    int deff;

    deff = lookups->fonts()->get_default_font_number(err);
    MX_ERROR_CHECK(err);

    sprintf(buf, "\\rtf1\\ansi\\deff%d", deff);

    write_string(err, buf);
    MX_ERROR_CHECK(err);

    lookups->fonts()->output_font_table_rtf(err, file);
    MX_ERROR_CHECK(err);

    lookups->colours()->output_colour_table_rtf(err, file);
    MX_ERROR_CHECK(err);

    lookups->styles()->output_style_table_rtf(err, file);
    MX_ERROR_CHECK(err);
abort:;
}

void mx_rtfe_document_output::output_document_rtf(int& err)
{
    output_doc_info_rtf(err);
    MX_ERROR_CHECK(err);

    output_doc_fmt_rtf(err);
    MX_ERROR_CHECK(err);

    output_header_rtf(err);
    MX_ERROR_CHECK(err);

    output_footer_rtf(err);
    MX_ERROR_CHECK(err);

    output_main_text_rtf(err);
    MX_ERROR_CHECK(err);

abort:;
}

void mx_rtfe_document_output::output_doc_info_rtf(int& err)
{
    /*    char buf[2000];

        sprintf(buf, "{{\\author %s}{\\doccomm %s}}",
                maxwell_doc->get_author(),
                maxwell_doc->get_description());

        write_string(err, buf);
        MX_ERROR_CHECK(err);*/
}

void mx_rtfe_document_output::output_doc_fmt_rtf(int& err)
{
    char buf[1000];
    mx_paragraph_style* def_ps;
    float width, height, left, right, top, bottom;
    bool is_landscape;

    def_ps = maxwell_doc->get_default_style(err);
    MX_ERROR_CHECK(err);

    maxwell_doc->get_page_size(err, width, height, is_landscape);
    MX_ERROR_CHECK(err);

    maxwell_doc->get_page_margins(err, left, right, top, bottom);
    MX_ERROR_CHECK(err);

    sprintf(buf,
        "\\deftab%d\\paperw%d\\paperh%d\\margl%d\\margr%d\\margt%d\\margb%d%s",
        (int)(0.5 + MX_MM_TO_TWIPS(def_ps->get_ruler()->default_tabstop_space)),
        (int)(0.5 + MX_MM_TO_TWIPS(width)),
        (int)(0.5 + MX_MM_TO_TWIPS(height)),
        (int)(0.5 + MX_MM_TO_TWIPS(left)),
        (int)(0.5 + MX_MM_TO_TWIPS(right)),
        (int)(0.5 + MX_MM_TO_TWIPS(top)),
        (int)(0.5 + MX_MM_TO_TWIPS(bottom)),
        is_landscape ? "\\landscape" : "");

    write_string(err, buf);
    MX_ERROR_CHECK(err);
abort:;
}

void mx_rtfe_document_output::output_header_rtf(int& err)
{
    output_header_footer_rtf(err, TRUE);
}

void mx_rtfe_document_output::output_footer_rtf(int& err)
{
    output_header_footer_rtf(err, FALSE);
}

void mx_rtfe_document_output::output_header_footer_rtf(int& err,
    bool do_header)
{
    const char* rtf_str = do_header ? "\\header " : "\\footer ";
    mx_text_area* txta = do_header ? maxwell_doc->header(err) : maxwell_doc->footer(err);
    MX_ERROR_CHECK(err);

    if (txta != NULL) {
        int num_paras, i;
        mx_paragraph* p;
        mx_rtfe_paragraph_output para_output(file, lookups);

        this->open_brace(err);
        MX_ERROR_CHECK(err);

        this->write_string(err, rtf_str);
        MX_ERROR_CHECK(err);

        num_paras = txta->num_paragraphs(err);
        MX_ERROR_CHECK(err);

        for (i = 0; i < num_paras; i++) {
            p = txta->get(err, i);
            MX_ERROR_CHECK(err);

            para_output.set_paragraph(p);

            para_output.output_rtf(err);
            MX_ERROR_CHECK(err);
        }

        this->close_brace(err);
        MX_ERROR_CHECK(err);
    }
abort:;
}

void mx_rtfe_document_output::output_main_text_rtf(int& err)
{
    // the biggie. spout the whole document text

    mx_area* area;
    mx_wp_doc_pos doc_pos;
    mx_paragraph* para;
    mx_rtfe_paragraph_output para_output(file, lookups);
    mx_rtfe_table_output table_output(file, lookups, &para_output);

    doc_pos.moveto_start(err, maxwell_doc);

    do {
        MX_ERROR_CHECK(err);

        area = doc_pos.get_area(err, maxwell_doc);
        MX_ERROR_CHECK(err);

        if (area != last_area) {
            if (last_area != NULL && last_area->is_a(mx_table_area_class_e) && !area->is_a(mx_table_area_class_e)) {
                // reset the paragraph style when going from table areas to
                // text areas.

                para_output.invalidate_current_style();
            }

            last_area = area;
            this->notify_export_progress(err, document_length, current_area_num++);
            MX_ERROR_CHECK(err);
        }

        if (area->is_a(mx_table_area_class_e)) {
            table_output.set_table((mx_table_area*)area);

            table_output.output_rtf(err);
            MX_ERROR_CHECK(err);

            doc_pos.next_area(err, maxwell_doc);
        } else {
            MX_ERROR_ASSERT(err, area->is_a(mx_text_area_class_e));

            para = doc_pos.get_para(err, maxwell_doc);
            MX_ERROR_CHECK(err);

            para_output.set_paragraph(para);

            para_output.output_rtf(err);
            MX_ERROR_CHECK(err);

            doc_pos.next_para(err, maxwell_doc);
        }
    } while (err != MX_POSITION_RANGE_ERROR);

    if (err == MX_POSITION_RANGE_ERROR) {
        MX_ERROR_CLEAR(err);
    } else {
        MX_ERROR_CHECK(err);
    }

abort:;
}
