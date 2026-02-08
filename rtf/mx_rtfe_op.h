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
#ifndef MX_RTFE_OP_H
#define MX_RTFE_OP_H
/*
 * MODULE : mx_rtfe_op.h
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

#include <mx_std.h>
#include <stdio.h>

// forward definitions
struct mx_rtfe_lookups;
class mx_paragraph;
class mx_table_area;
class mx_complex_word;
class mx_space_word;
class mx_break_word;
class mx_field_word;
class mx_paragraph_style;
class mx_wp_document;
class mx_area;
class mx_image_area;

class mx_rtfe_output {
public:
    mx_rtfe_output(FILE* file, mx_rtfe_lookups* lu);
    virtual ~mx_rtfe_output();

    // can't copy these objects so copy constructor not implemented
    mx_rtfe_output(const mx_rtfe_output&);

    // output a brace into the stream and remember how many braces are
    // opened/closed
    void open_brace(int& err);

    // close a brace and
    void close_brace(int& err);
    void close_all_braces();

    // convenience functions which convert stdio errors to maxwell errors
    void write_string(int& err, const char* s);
    void write_char(int& err, char c);

    // write an rtf string, using rtf tokens for non-standard characters
    void write_rtf_string(int& err, const char* s);
    void write_rtf_char(int& err, char c);

    // override this to output the things rtf
    virtual void output_rtf(int& err) = 0;

protected:
    FILE* file;
    mx_rtfe_lookups* lookups;

private:
    uint32 brace_count;
};

class mx_rtfe_paragraph_output : public mx_rtfe_output {
public:
    mx_rtfe_paragraph_output(FILE* file, mx_rtfe_lookups* lu);
    ~mx_rtfe_paragraph_output();
    mx_rtfe_paragraph_output(const mx_rtfe_paragraph_output&);

    enum mx_rtfe_para_type {
        normal_e,
        in_table_e,
        last_in_cell_e,
        last_in_row_e
    };

    void set_paragraph(mx_paragraph* p, mx_rtfe_para_type = normal_e);
    void output_rtf(int& err);

    // makes sure the full paragraph style description is output for the next
    // paragraph
    void invalidate_current_style();

private:
    void output_word_rtf(int& err, mx_complex_word* w);
    void output_word_rtf(int& err, mx_space_word* w);
    void output_word_rtf(int& err, mx_break_word* w);
    void output_word_rtf(int& err, mx_field_word* w);
    void output_area_rtf(int& err, mx_area* a);
    void output_image_area_rtf(int& err, mx_image_area* a);

    mx_paragraph* para;
    const char* current_base_style;
    mx_paragraph_style* current_para_style;
    mx_rtfe_para_type rtf_para_type;
};

class mx_rtfe_table_output : public mx_rtfe_output {
public:
    mx_rtfe_table_output(FILE* file, mx_rtfe_lookups* lu,
        mx_rtfe_paragraph_output* a_para_op);
    ~mx_rtfe_table_output();
    mx_rtfe_table_output(const mx_rtfe_table_output&);

    void set_table(mx_table_area* taba);
    void output_rtf(int& err);

private:
    void output_row_formatting_rtf(int& err, uint32 row_num);
    void output_row(int& err, uint32 row_num);
    void output_cell(int& err, uint32 row_num, uint32 col_num);

    mx_table_area* table;

    // stored for convenience
    uint32 num_rows, num_columns;

    // used to output paragraphs
    mx_rtfe_paragraph_output* para_op;
};

class mx_rtfe_document_output : public mx_rtfe_output {
public:
    mx_rtfe_document_output(const char* output_file);
    ~mx_rtfe_document_output();
    mx_rtfe_document_output(const mx_rtfe_document_output&);

    void set_document(int& err, mx_wp_document* d);
    void output_rtf(int& err);

    // method which is called at intervals during export notifying progress, in
    // terms of an length integer value and a progress value specifying a
    // proportion of that length
    virtual void notify_export_progress(int& err,
        uint32 length, uint32 progress);

private:
    void output_docheader_rtf(int& err);
    void output_document_rtf(int& err);
    void output_doc_info_rtf(int& err);
    void output_doc_fmt_rtf(int& err);
    void output_page_info_rtf(int& err);

    void output_header_rtf(int& err);
    void output_footer_rtf(int& err);
    void output_header_footer_rtf(int& err, bool do_header);

    void output_main_text_rtf(int& err);

    // stuff for progress notification. document_length is the number of areas
    // in a document. During export the area counter is incremented the
    // notification methods are called with the relevant values
    uint32 document_length, current_area_num;
    mx_area* last_area;

    mx_wp_document* maxwell_doc;
    const char* file_name;
};

#endif // MX_RTFE_OP_H
