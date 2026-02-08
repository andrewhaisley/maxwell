#pragma once

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
 * MODULE/CLASS : mx_rtf_importer
 *
 * AUTHOR : Tom Newton
 *
 *
 *
 * DESCRIPTION:
 *
 * This is the subclass of importer which is used to import rtf files into a
 * maxwell document. The simple interface is inherited from mx_importer.
 *
 *
 *
 *
 *
 */

#include <map>

#include <mx.h>
#include <mx_importer.h>
#include <mx_mxmetafile.h>
#include <rtf.h>

class mx_wp_document;
class mx_wp_cursor;
class mx_font;
class mx_colour;
class mx_char_style;
class mx_paragraph_style;
class mx_dialog_manager;
class mx_prog_d;
class mx_text_area;
class mx_paragraph;

class mx_rtf_importer : public mx_importer, public mx_rtf {
    MX_RTTI(mx_rtf_importer_class_e)

public:
    mx_rtf_importer(int& err, const char* f, mx_wp_document* d,
        mx_wp_cursor* c, mx_dialog_manager* dm = nullptr);
    mx_rtf_importer(const mx_rtf_importer&);
    virtual ~mx_rtf_importer();

    // the following two methods both read an rtf file into the maxwell
    // document structure. The difference is that import() does not read the
    // headers/footers, or page setup information into memory

    void load(int& err);
    void import(int& err);

    mx_font* get_font(int& err, int rtf_font_num);
    mx_colour* get_colour(int& err, int rtf_colour_num);
    mx_char_style* get_char_style(int& err, int rtf_char_style_num);
    mx_paragraph_style* get_para_style(int& err, int rtf_para_style_num);

    // build up a fifo of chunks of maxwell words with their character styles
    // with the following stucture
    struct text_fifo_elem_t {
        mx_char_style mx_cs;
        mx_list mx_words;
    };
    mx_list text_fifo;

private:
    // stuff for the progress bar.

    uint32 file_length;
    mx_prog_d* pd;
    bool update_progress();

    // get a maxwell paragraph style from an RTF paragraph style. Make sure the
    // document gets updated with any new styles correctly
    mx_paragraph_style* get_named_paragraph_style(int& err,
        const RTFSpar& rtf_ps);

    // whether the break before paragraph attribute should be set for the next
    // paragraph. Used to implement hard page breaks
    bool break_before_next_para;

    // if this is true, headers and footers get imported, and styles get
    // overwritten with the styles in the RTF document. Otherwise, headers and
    // footers are not loaded, and do we want to insert headers and footers
    bool do_full_import;

    // did we insert header/footer
    bool inserted_header;
    bool inserted_footer;

    // current document and position.
    mx_wp_document* doc;
    mx_wp_cursor* doc_cursor;

    // was the last paragraph inserted a table paragraph?
    bool last_para_in_table;

    // how many cells were there in the last table row?
    int32 last_num_cells;

    // used within table areas
    mx_table_area* table_area;
    mx_list merged_cells;

    // what is the trgaph gap (in twips) between cells for this table
    int table_trgaph;

    // translation table for the particular character set
    uint8* character_set_lookup;

    // lookup tables for rtf font numbers, colour numbers, character style
    // numbers and paragraph style numbers

    std::map<int, mx_font *> m_font_lookup;
    std::map<int, mx_colour *> m_colour_lookup;
    std::map<int, mx_paragraph_style *> m_para_style_lookup;

    // ensure all the styles in the rtf stylesheet are imported
    bool styles_are_added;
    void add_all_styles(int& err);

    void do_import(int& err);

    void processFieldInstOutput(int& err, RTFSgroup* group);
    void processMainOutput(int& err, RTFSgroup* group);

    void processNewParagraph(int& err, RTFSgroup* group,
        rtfDestinationType destination);
    void processNewTableCell(int& err, RTFSgroup* group,
        rtfDestinationType destination);
    void processNewTableRow(int& err, RTFSgroup* group,
        rtfDestinationType destination);
    void processNewTab(int& err, RTFSgroup* group,
        rtfDestinationType destination);
    void processSpecialCharacter(int& err, RTFSgroup* group,
        rtfDestinationType destination,
        rtfControl control, int controlValue);
    void processPictOutput(int& err,
        RTFSgroup* group);

    void insertNewParagraph(int& err, RTFSgroup* group,
        rtfDestinationType destination);
    void insertNewTableParagraph(int& err, RTFSgroup* group,
        rtfDestinationType destination);
    void insertHeaderFooterText(int& err, RTFSgroup* group,
        mx_text_area* txta);

    void insertParagraphWords(int& err, RTFSgroup* group, mx_paragraph* para);

    void userProcessFontOutput(int& err, RTFSfont* f);
    void userProcessFileOutput(int& err, RTFSfile* f);
    void userProcessColourOutput(int& err, RTFScolour* c);
    void userProcessStyleOutput(int& err, RTFSstyle* s);

    // was the last field instruction inserted? (pointer because of gcc bug)
    bool field_inst_was_inserted;

    // do we want to insert the current rtf "thing"?. Returns true if the
    // current group destination is RTFDrtf, OR if it is field text for which
    // we have not recognised the field instruction.
    bool not_field_text_group(RTFSgroup* group);
};

class mx_rtf_mxmetafile_output : public mx_mxmetafile_output {
public:
    mx_rtf_mxmetafile_output(mx_wp_document* doc,
        mx_list* text_fifo,
        mx_rtf_importer::text_fifo_elem_t* elem);

    // current document and position.
    mx_wp_document* doc;
    mx_list* text_fifo;
    mx_rtf_importer::text_fifo_elem_t* elem;

    void processStretchDIBits(int& err,
        mx_standard_metafile_record& record,
        char** buffer);
};
