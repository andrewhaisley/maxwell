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

#include <mx_rtf_importer.h>

#include <assert.h>
#include <mx.h>
#include <mx_break_w.h>
#include <mx_cw_t.h>
#include <mx_dialog_man.h>
#include <mx_file_type.h>
#include <mx_list_iter.h>
#include <mx_para_style.h>
#include <mx_paragraph.h>
#include <mx_prog_d.h>
#include <mx_simple_w.h>
#include <mx_ta_pos.h>
#include <mx_wp_cursor.h>
#include <mx_wp_doc.h>
#include <mx_wp_pos.h>
#include <rasdib.h>
#include <rtf.h>

// translation tables for character codes of different systems
static uint8 ansi_char_lut[256] = {
    0, 1, 2, 3, 4, 5, 6, 7, 8, 9,
    10, 11, 12, 13, 14, 15, 16, 17, 18, 19,
    20, 21, 22, 23, 24, 25, 26, 27, 28, 29,
    30, 31, 32, 33, 34, 35, 36, 37, 38, 39,
    40, 41, 42, 43, 44, 45, 46, 47, 48, 49,
    50, 51, 52, 53, 54, 55, 56, 57, 58, 59,
    60, 61, 62, 63, 64, 65, 66, 67, 68, 69,
    70, 71, 72, 73, 74, 75, 76, 77, 78, 79,
    80, 81, 82, 83, 84, 85, 86, 87, 88, 89,
    90, 91, 92, 93, 94, 95, 96, 97, 98, 99,
    100, 101, 102, 103, 104, 105, 106, 107, 108, 109,
    110, 111, 112, 113, 114, 115, 116, 117, 118, 119,
    120, 121, 122, 123, 124, 125, 126, 127, 128, 129,
    130, 131, 132, 133, 134, 135, 136, 137, 138, 139,
    140, 141, 142, 143, 144, 145, 146, 147, 148, 149,
    150, 151, 152, 153, 154, 155, 156, 157, 158, 159,
    160, 161, 162, 163, 164, 165, 166, 167, 168, 169,
    170, 171, 172, 173, 174, 175, 176, 177, 178, 179,
    180, 181, 182, 183, 184, 185, 186, 187, 188, 189,
    190, 191, 192, 193, 194, 195, 196, 197, 198, 199,
    200, 201, 202, 203, 204, 205, 206, 207, 208, 209,
    210, 211, 212, 213, 214, 215, 216, 217, 218, 219,
    220, 221, 222, 223, 224, 225, 226, 227, 228, 229,
    230, 231, 232, 233, 234, 235, 236, 237, 238, 239,
    240, 241, 242, 243, 244, 245, 246, 247, 248, 249,
    250, 251, 252, 253, 254, 255
};

static uint8 mac_char_lut[256] = {
    0, 1, 2, 3, 4, 5, 6, 7, 8, 9,
    10, 11, 12, 13, 14, 15, 16, 17, 18, 19,
    20, 21, 22, 23, 24, 25, 26, 27, 28, 29,
    30, 31, 32, 33, 34, 35, 36, 37, 38, 39,
    40, 41, 42, 43, 44, 45, 46, 47, 48, 49,
    50, 51, 52, 53, 54, 55, 56, 57, 58, 59,
    60, 61, 62, 63, 64, 65, 66, 67, 68, 69,
    70, 71, 72, 73, 74, 75, 76, 77, 78, 79,
    80, 81, 82, 83, 84, 85, 86, 87, 88, 89,
    90, 91, 92, 93, 94, 95, 96, 97, 98, 99,
    100, 101, 102, 103, 104, 105, 106, 107, 108, 109,
    110, 111, 112, 113, 114, 115, 116, 117, 118, 119,
    120, 121, 122, 123, 124, 125, 126, 32, 196, 197,
    199, 201, 209, 214, 220, 225, 224, 226, 228, 227,
    229, 231, 233, 232, 234, 235, 237, 236, 238, 239,
    241, 243, 242, 244, 246, 245, 250, 249, 251, 252,
    '*', 176, 162, 163, 167, 183, 182, 223, 174, 169,
    '*', 180, 168, '*', 198, 216, '*', 177, '*', '*',
    165, 181, '*', '*', '*', '*', '*', 170, 186, '*',
    230, 248, 191, 161, 172, '*', '*', '*', '*', 171,
    187, 46, 32, 192, 195, 213, '*', '*', 173, 173,
    34, 34, 96, 39, 247, '*', 255, 89, 47, 164,
    '<', '>', '*', '*', '*', 183, 39, 34, '*', 194,
    202, 193, 203, 200, 205, 206, 207, 204, 211, 212,
    '*', 210, 218, 219, 217, 105, 94, 126, '*', '*',
    183, 176, '*', 34, '*', '*'
};

mx_rtf_importer::mx_rtf_importer(int& err, const char* f, mx_wp_document* d,
    mx_wp_cursor* c, mx_dialog_manager* dm)
    : mx_importer(err, f)
    , mx_rtf(err, f)
{
    if (!mx_is_file_type(file_name, mx_rtf_file_e)) {
        MX_ERROR_THROW(err, MX_IMPORTER_BAD_FILE_TYPE);
    }

    file_length = mx_file_size(err, file_name);
    MX_ERROR_CHECK(err);

    if (dm == nullptr) {
        pd = nullptr;
    } else {
        pd = dm->get_prog_bar_d();
    }

    doc = d;
    doc_cursor = c;

    last_para_in_table = break_before_next_para = false;
    last_num_cells = 0;
    field_inst_was_inserted = false;
    styles_are_added = false;
    table_area = nullptr;
    do_full_import = true;
    inserted_header = inserted_footer = false;
abort:;
}

mx_rtf_importer::~mx_rtf_importer()
{
    mx_list_iterator li(text_fifo);
    while (li.more())
        delete (text_fifo_elem_t*)li.data();

    for (auto i : m_font_lookup) {
        delete i.second;
    }
    m_font_lookup.clear();

    for (auto i : m_colour_lookup) {
        delete i.second;
    }

    m_colour_lookup.clear();
}

static void remove_last_para(int& err, mx_text_area* a)
{
    int np;
    mx_paragraph* p;

    np = a->num_paragraphs(err);
    MX_ERROR_CHECK(err);

    p = a->remove(err, np - 1);
    MX_ERROR_CHECK(err);

    delete p;
abort:;
}

void mx_rtf_importer::load(int& err)
{
    do_full_import = true;
    do_import(err);
    MX_ERROR_CHECK(err);

    documentData.convertTo(err, *doc, *doc_cursor->get_position(), *this);
    MX_ERROR_CHECK(err);

    // invalidate the view correction for the cursor
    doc_cursor->sheet_size_changed(err);
    MX_ERROR_CHECK(err);

    if (inserted_header) {
        mx_text_area* hd = doc->header(err);
        MX_ERROR_CHECK(err);
        MX_ERROR_ASSERT(err, hd != nullptr);

        remove_last_para(err, hd);
        MX_ERROR_CHECK(err);
    }

    if (inserted_footer) {
        mx_text_area* ft = doc->footer(err);
        MX_ERROR_CHECK(err);
        MX_ERROR_ASSERT(err, ft != nullptr);

        remove_last_para(err, ft);
        MX_ERROR_CHECK(err);
    }

abort:;
}

void mx_rtf_importer::import(int& err)
{
    do_full_import = false;
    do_import(err);
    MX_ERROR_CHECK(err);
abort:;
}

void mx_rtf_importer::do_import(int& err)
{
    char message_str[2000] = "Importing file: ";

    strcat(message_str, file_name);

    if (pd != nullptr) {
        pd->centre();
        pd->activate_d(message_str);
    }

    file_offset = 0;
    character_set_lookup = nullptr;
    styles_are_added = false;
    inserted_header = inserted_footer = false;

    update_progress();

    this->read(err);
    MX_ERROR_CHECK(err);

abort:
    if (pd != nullptr) {
        pd->deactivate();
    }
}

static inline void set_up_character_set_lut(rtfControl charset,
    uint8** charset_lookup_ptr)
{
    if (*charset_lookup_ptr == nullptr) {
        switch (charset) {
        case RTFmac:
            *charset_lookup_ptr = mac_char_lut;
            break;
        case RTFpc:
        case RTFpca:
        case RTFansi:
        default:
            *charset_lookup_ptr = ansi_char_lut;
            break;
        }
    }
}

static inline void convert_string(char* string, uint8* charset_lut)
{
    uint8* s = (uint8*)string;
    while (*s != 0) {
        *s = charset_lut[*s];
        s++;
    }
}

void mx_rtf_importer::processMainOutput(int& err, RTFSgroup* group)
{
    const char* ptr = textBuffer;
    mx_word* w = nullptr;

    textBuffer[textBufferSize] = '\0';

    set_up_character_set_lut(documentData.charset, &character_set_lookup);
    convert_string(textBuffer, character_set_lookup);

    // stack up the character style and the words
    text_fifo_elem_t* elem = nullptr;

    if (not_field_text_group(group)) {
        // stack up the character style and the words
        elem = new text_fifo_elem_t;

        // convert the RTF character style into a maxwell one.
        group->formatting.character.convertTo(err, elem->mx_cs, *this);
        MX_ERROR_CHECK(err);

        // insert the words into the list
        while (*ptr != '\0') {
            w = mx_paragraph::get_word_from_string(err, ptr);
            MX_ERROR_CHECK(err);

            if (w == nullptr) {
                // if we're not at the end of the string, skip characters which
                // could not be converted
                if (*ptr != '\0')
                    ptr++;
            } else {
                elem->mx_words.append(w);
            }
        }

        // append this chunk of text to the fifo
        text_fifo.append(elem);
        elem = nullptr;
    }

abort:
    if (elem)
        delete elem;
    return;
}

mx_paragraph_style* mx_rtf_importer ::get_named_paragraph_style(int& err, const RTFSpar& rtf_ps)
{
    mx_paragraph_style* named_ps = nullptr;

    // if we have not used the style sheet yet, add all the styles in it
    if (!styles_are_added) {
        add_all_styles(err);
        MX_ERROR_CHECK(err);
    }

    if (rtf_ps.fmt.s == 0) {
        named_ps = doc->get_default_style(err);
        MX_ERROR_CHECK(err);
    } else {
        MX_ERROR_ASSERT(err, rtf_ps.fmt.ss->stylename != nullptr);
        named_ps = doc->get_named_style(err, rtf_ps.fmt.ss->stylename);
        MX_ERROR_CHECK(err);
    }

    return named_ps;
abort:
    return nullptr;
}

void mx_rtf_importer::insertParagraphWords(int& err, RTFSgroup* group,
    mx_paragraph* paragraph)
{
    text_fifo_elem_t* e = nullptr;
    mx_paragraph_style group_ps, *named_ps = nullptr;
    mx_para_pos end_pos;
    mx_char_style* current_style;
    mx_char_style_mod mod;

    // set the paragraph style

    named_ps = get_named_paragraph_style(err, group->formatting.paragraph);
    MX_ERROR_CHECK(err);
    paragraph->set_paragraph_style(named_ps);

    // set up the changes to the paragraph style
    group->formatting.convertTo(err, group_ps, *this);
    MX_ERROR_CHECK(err);

    if (break_before_next_para) {
        group_ps.page_break_before = true;
        break_before_next_para = false;
    }

    paragraph->modify_paragraph_style(group_ps);

    // set up the character styles for each chunk of text in the fifo.

    while (text_fifo.get_num_items() > 0) {
        e = (text_fifo_elem_t*)text_fifo.remove(err, 0);
        MX_ERROR_CHECK(err);

        // set the style for this chunk of text

        end_pos.moveto_end(err, paragraph);
        MX_ERROR_CHECK(err);

        mx_word::set_operations_to_right_of_mods();
        current_style = paragraph->aka_get_style(err, end_pos);
        MX_ERROR_CHECK(err);

        mod = mx_char_style_mod(*current_style, e->mx_cs);
        paragraph->append_mod(err, mod);
        MX_ERROR_CHECK(err);

        // insert the words

        mx_list_iterator i(e->mx_words);

        while (i.more()) {
            mx_word* w = (mx_word*)i.data();
            paragraph->append_word(err, w);
            MX_ERROR_CHECK(err);
        }

        // set the style back to the original style

        mod = mx_char_style_mod(e->mx_cs, *current_style);
        paragraph->append_mod(err, mod);
        MX_ERROR_CHECK(err);

        delete e;
    }
abort:;
}

void mx_rtf_importer::insertNewParagraph(int& err, RTFSgroup* group,
    rtfDestinationType destination)
{
    mx_paragraph_style group_ps;
    mx_paragraph* paragraph = nullptr;
    mx_para_pos end_pos;
    // get a new paragraph and a reference to the paragraph position

    paragraph = doc_cursor->insert_paragraph(err);
    MX_ERROR_CHECK(err);

    insertParagraphWords(err, group, paragraph);
    MX_ERROR_CHECK(err);

    // move the cursor to the end of the paragraph
    doc_cursor->moveto_para_end(err);
    MX_ERROR_CHECK(err);

    // move the cursor to the start of the next paragraph
    doc_cursor->get_position()->right(err, doc);
    MX_ERROR_CHECK(err);
abort:;
}

#if 0
static mx_table_area *join_table_with_previous(int &err, mx_wp_document *doc,
                                               mx_wp_doc_pos &tab_pos)
{
    bool b;
    mx_wp_doc_pos prev_tab_pos = tab_pos;
    mx_table_area *prev_tab, *tab;

    tab = (mx_table_area *)tab_pos.get_area(err, doc);
    MX_ERROR_CHECK(err);

    prev_tab_pos.prev_area(err, doc);
    if (err == MX_POSITION_RANGE_ERROR)
    {
        MX_ERROR_CLEAR(err);
        return tab;
    }
    else
    {
        MX_ERROR_CHECK(err);
    }

    prev_tab = (mx_table_area *)prev_tab_pos.get_area(err, doc);
    MX_ERROR_CHECK(err);

    b = prev_tab->can_join(err, tab);
    MX_ERROR_CHECK(err);

    if (b)
    {
        mx_table_area_pos *tap, table_pos;
        uint32 prev_num_rows;
        mx_sheet *sheet;
        mx_wp_sheet_pos *sp;

        MX_ERROR_ASSERT(err, prev_tab->is_a(mx_table_area_class_e));
        MX_ERROR_ASSERT(err, tab->is_a(mx_table_area_class_e));

        prev_num_rows = prev_tab->get_num_rows(err);
        MX_ERROR_CHECK(err);

        sheet = doc->sheet(err, tab_pos.get_sheet());
        MX_ERROR_CHECK(err);

        sp = tab_pos.get_sheet_pos();

        tap = (mx_table_area_pos *)sp->get_area_pos(err);
        MX_ERROR_CHECK(err);
        MX_ERROR_ASSERT(err, tap->is_a(mx_table_area_pos_class_e));
        table_pos = *tap;

        tab = (mx_table_area *)sheet->remove_area(err, sp->get_area_index());
        MX_ERROR_CHECK(err);
        MX_ERROR_ASSERT(err, tab->is_a(mx_table_area_class_e));

        prev_tab->join(err, tab);
        MX_ERROR_CHECK(err);

        table_pos.row += prev_num_rows;

        sp->set_area_index(err, sp->get_area_index() - 1, sheet);
        MX_ERROR_CHECK(err);

        tap = (mx_table_area_pos *)sp->get_area_pos(err);
        MX_ERROR_CHECK(err);
        MX_ERROR_ASSERT(err, tap->is_a(mx_table_area_pos_class_e));
        *tap = table_pos;

        return prev_tab;
    }
    else
    {
        return tab;
    }
abort:
    return nullptr;
}
#endif

void mx_rtf_importer::insertNewTableParagraph(int& err, RTFSgroup* group,
    rtfDestinationType destination)
{
    bool want_to_insert = true;

    // if we are at the first cell in a table, work out whether we need to
    // create a new table or continue an old one and then convert the RTF table
    // formatting
    if (tableData.cellCount == 0) {
        int row = 0;
        bool insert_new_table = (!last_para_in_table || tableData.cells.get_num_items() != last_num_cells);

        last_num_cells = tableData.cells.get_num_items();

        // if the last paragraph we inserted was a table, and the formatting of
        // the next row of the table is the same as the last row, then continue
        // that table, otherwise create a new table.
        if (insert_new_table) {
            table_area = doc_cursor->insert_table(err);
            MX_ERROR_CHECK(err);

            table_trgaph = tableData.trgaph;

            MX_ERROR_ASSERT(err, tableData.cells.get_num_items() != 0);
        } else {
            row = table_area->get_num_rows(err);
            MX_ERROR_CHECK(err);

            // after we have inserted a row, we should be in the first position
            // after the table, so move back into the table, and extend it by
            // one row, then move into the first position in the new row

            doc_cursor->get_position()->left(err, doc);
            MX_ERROR_CHECK(err);

            table_area->insert_rows_no_reposition(err, row, 1);
            MX_ERROR_CHECK(err);

            doc_cursor->get_position()->right(err, doc);
            MX_ERROR_CHECK(err);
        }

        tableData.convertTo(err, *table_area, *this, row, merged_cells);
        MX_ERROR_CHECK(err);

        if (insert_new_table) {
            // if we've just inserted a new table, then try to join this table
            // with the previous one

            // table_area = join_table_with_previous(err, doc, *doc_cursor->get_position());
            // MX_ERROR_CHECK(err);
        }
    }

    // check to see if this is a merged cell. If it is then do not bother
    // inserting the paragraph for it.

    want_to_insert = !(bool)merged_cells.get(err, tableData.cellCount);
    MX_ERROR_CHECK(err);

    if (want_to_insert) {
        mx_paragraph* p;
        mx_paragraph_style ps;

        insertNewParagraph(err, group, destination);
        MX_ERROR_CHECK(err);

        // now we need to set up the horizontal table gaps. We are currently
        // doing this by modifying left and right indents. Ug.

        doc_cursor->get_position()->left(err, doc);
        MX_ERROR_CHECK(err);

        p = doc_cursor->get_position()->get_para(err, doc);
        MX_ERROR_CHECK(err);

        ps = *p->get_paragraph_style();
        ps.get_ruler()->left_indent += MX_TWIPS_TO_MM(table_trgaph);
        ps.get_ruler()->right_indent += MX_TWIPS_TO_MM(table_trgaph);
        p->modify_paragraph_style(ps);

        doc_cursor->get_position()->right(err, doc);
        MX_ERROR_CHECK(err);
    }
abort:;
}

void mx_rtf_importer::insertHeaderFooterText(int& err, RTFSgroup* group,
    mx_text_area* txta)
{
    int np;
    mx_paragraph* p;

    np = txta->num_paragraphs(err);
    MX_ERROR_CHECK(err);

    p = txta->get(err, np - 1);
    MX_ERROR_CHECK(err);

    this->insertParagraphWords(err, group, p);
    MX_ERROR_CHECK(err);

    p = new mx_paragraph;
    p->set_paragraph_style(txta->get_base_paragraph_style());

    txta->append(err, p);
    MX_ERROR_CHECK(err);
abort:;
}

inline static mx_text_area* mx_rtf_get_header(int& err, mx_wp_document* doc)
{
    mx_text_area* res = doc->header(err);
    MX_ERROR_CHECK(err);

    if (res == nullptr) {
        mx_paragraph_style* def_style = doc->get_default_style(err);
        MX_ERROR_CHECK(err);

        res = new mx_text_area(doc->get_docid(), def_style);

        doc->set_header(err, res);
        MX_ERROR_CHECK(err);
    }
abort:
    return res;
}

inline static mx_text_area* mx_rtf_get_footer(int& err, mx_wp_document* doc)
{
    mx_text_area* res = doc->footer(err);
    MX_ERROR_CHECK(err);

    if (res == nullptr) {
        mx_paragraph_style* def_style = doc->get_default_style(err);
        MX_ERROR_CHECK(err);

        res = new mx_text_area(doc->get_docid(), def_style);

        doc->set_footer(err, res);
        MX_ERROR_CHECK(err);
    }
abort:
    return res;
}

void mx_rtf_importer::processNewParagraph(int& err, RTFSgroup* group,
    rtfDestinationType destination)
{
    mx_text_area* hdft_txt;
    switch (group->destination) {
    case RTFDheader:
        if (do_full_import) {
            hdft_txt = mx_rtf_get_header(err, doc);
            MX_ERROR_CHECK(err);

            insertHeaderFooterText(err, group, hdft_txt);
            MX_ERROR_CHECK(err);
            inserted_header = true;
        }
        break;
    case RTFDfooter:
        if (do_full_import) {
            hdft_txt = mx_rtf_get_footer(err, doc);
            MX_ERROR_CHECK(err);

            insertHeaderFooterText(err, group, hdft_txt);
            MX_ERROR_CHECK(err);
            inserted_footer = true;
        }
        break;
    default:
        if (group->formatting.paragraph.fmt.intbl) {
            insertNewTableParagraph(err, group, destination);
            MX_ERROR_CHECK(err);

            last_para_in_table = true;
        } else {
            insertNewParagraph(err, group, destination);
            MX_ERROR_CHECK(err);

            last_para_in_table = false;
            table_area = nullptr;
        }
        break;
    }

    if (update_progress()) {
        MX_ERROR_THROW(err, MX_USER_CANCEL);
    }

abort:;
}

void mx_rtf_importer::processNewTableCell(int& err, RTFSgroup* group,
    rtfDestinationType destination)
{
    mx_table_area_pos* tab_ap = nullptr;
    mx_text_area* ta = nullptr;
    mx_table_area* tab_a = nullptr;
    int p = 0;
    bool want_to_insert = true;

    processNewParagraph(err, group, destination);
    MX_ERROR_CHECK(err);

    // don't do table stuff in headers or footers
    switch (group->destination) {
    case RTFDheader:
    case RTFDfooter:
        return;
    default:
        break;
    }

    // check to see if this is a merged cell. If it is then do not bother
    // inserting the paragraph for it.

    want_to_insert = !(bool)merged_cells.get(err, tableData.cellCount);
    MX_ERROR_CHECK(err);

    if (want_to_insert) {
        // if this cell has more than 1 paragraph, delete the last one in this
        // cell because it is the empty one that all text areas start off with,
        // and which we do not want

        tab_ap = (mx_table_area_pos*)doc_cursor->get_position()->get_area_pos(err);
        MX_ERROR_CHECK(err);

        tab_a = (mx_table_area*)doc_cursor->get_position()->get_area(err, doc);
        MX_ERROR_CHECK(err);
        MX_ERROR_ASSERT(err, tab_a->is_a(mx_table_area_class_e));

        ta = (mx_text_area*)tab_a->get_cell(err, tab_ap->row, tab_ap->column);
        MX_ERROR_CHECK(err);
        MX_ERROR_ASSERT(err, ta->is_a(mx_text_area_class_e));

        p = ta->num_paragraphs(err);
        MX_ERROR_CHECK(err);

        // leave the cursor at the start of the next cell

        doc_cursor->get_position()->right(err, doc);
        MX_ERROR_CHECK(err);

        // delete the first paragraph if necessary

        if (p > 1) {
            mx_paragraph* para = ta->remove(err, p - 1);
            MX_ERROR_CHECK(err);
            delete para;
        }
    }
abort:;
}

// routine which adjusts borders on columns of tables to ensure they have the
// standard "maxwell" configuration. The document position should be
// referencing a cell in the row which needs to be fixed
static void fix_table_column_borders(int& err, mx_wp_document* doc,
    mx_wp_doc_pos& doc_pos)
{
    uint32 num_cols, i;
    mx_table_area_pos* tabp;
    mx_table_area* taba;

    tabp = (mx_table_area_pos*)doc_pos.get_area_pos(err);
    MX_ERROR_CHECK(err);
    MX_ERROR_ASSERT(err, tabp->is_a(mx_table_area_pos_class_e));

    taba = (mx_table_area*)doc_pos.get_area(err, doc);
    MX_ERROR_CHECK(err);
    MX_ERROR_ASSERT(err, taba->is_a(mx_table_area_class_e));

    num_cols = taba->get_num_columns(err);
    MX_ERROR_CHECK(err);

    for (i = 1; i < num_cols; i++) {
        mx_text_area *left_cell, *right_cell;
        mx_line_style *right_cell_left_bd, *left_cell_right_bd;

        left_cell = taba->get_cell(err, tabp->row, i - 1);
        MX_ERROR_CHECK(err);

        right_cell = taba->get_cell(err, tabp->row, i);
        MX_ERROR_CHECK(err);

        left_cell_right_bd = &left_cell->get_paragraph_style()->get_border_style()->right_style;

        if (left_cell_right_bd->line_type != mx_no_line) {
            right_cell_left_bd = &right_cell->get_paragraph_style()->get_border_style()->left_style;

            *right_cell_left_bd = *left_cell_right_bd;

            left_cell_right_bd->line_type = mx_no_line;
        }
    }
abort:;
}

// routine which adjusts borders on rows of tables to ensure they have the
// standard "maxwell" configuration. The document position should be
// referencing a cell in the bottom row of the two rows to be fixed
static void fix_table_row_borders(int& err, mx_wp_document* doc,
    mx_wp_doc_pos docp_bot)
{
    uint32 top_num_cols, bot_num_cols, i;
    mx_wp_doc_pos docp_top = docp_bot;
    mx_table_area_pos *tabp_top, *tabp_bot;
    mx_table_area *tab_top, *tab_bot;

    // first we set up our two table areas and our two table area positions

    tabp_bot = (mx_table_area_pos*)docp_bot.get_area_pos(err);
    MX_ERROR_CHECK(err);
    MX_ERROR_ASSERT(err, tabp_bot->is_a(mx_table_area_pos_class_e));

    tab_top = tab_bot = (mx_table_area*)docp_bot.get_area(err, doc);
    MX_ERROR_CHECK(err);
    MX_ERROR_ASSERT(err, tab_top->is_a(mx_table_area_class_e));

    // are the two rows we're looking at part of the same table?
    if (tabp_bot->row > 0) {
        tabp_top = (mx_table_area_pos*)docp_top.get_area_pos(err);
        MX_ERROR_CHECK(err);
        MX_ERROR_ASSERT(err, tabp_top->is_a(mx_table_area_pos_class_e));
        tabp_top->row--;
    } else {
        uint32 num_rows;
        docp_top.prev_area(err, doc);
        if (err != MX_ERROR_OK) {
            MX_ERROR_CLEAR(err);
            return;
        }

        tabp_top = (mx_table_area_pos*)docp_top.get_area_pos(err);
        MX_ERROR_CHECK(err);

        if (!tabp_top->is_a(mx_table_area_pos_class_e))
            return;

        tab_top = (mx_table_area*)docp_top.get_area(err, doc);
        MX_ERROR_CHECK(err);
        MX_ERROR_ASSERT(err, tab_top->is_a(mx_table_area_class_e));

        num_rows = tab_top->get_num_rows(err);
        MX_ERROR_CHECK(err);

        tabp_top->row = num_rows - 1;
    }

    // now go through the two rows and check that they have the same
    // dimensions, otherwise we do not do anything

    top_num_cols = tab_top->get_num_columns(err);
    MX_ERROR_CHECK(err);

    bot_num_cols = tab_bot->get_num_columns(err);
    MX_ERROR_CHECK(err);

    if (top_num_cols != bot_num_cols)
        return;

    for (i = 0; i < top_num_cols; i++) {
        float top_w, bot_w;

        top_w = tab_top->get_column_width(err, i);
        MX_ERROR_CHECK(err);

        bot_w = tab_bot->get_column_width(err, i);
        MX_ERROR_CHECK(err);

        if (!MX_FLOAT_EQ(top_w, bot_w))
            return;
    }

    // now go throught the two rows moving the bottom border of the top rows
    // cells onto the top border of the bottom rows cells

    for (i = 0; i < top_num_cols; i++) {
        mx_text_area *top_cell, *bot_cell;
        mx_line_style *bot_cell_top_bd, *top_cell_bot_bd;

        top_cell = tab_top->get_cell(err, tabp_top->row, i);
        MX_ERROR_CHECK(err);

        bot_cell = tab_bot->get_cell(err, tabp_bot->row, i);
        MX_ERROR_CHECK(err);

        top_cell_bot_bd = &top_cell->get_paragraph_style()->get_border_style()->bottom_style;

        if (top_cell_bot_bd->line_type != mx_no_line) {
            bot_cell_top_bd = &bot_cell->get_paragraph_style()->get_border_style()->top_style;

            *bot_cell_top_bd = *top_cell_bot_bd;

            top_cell_bot_bd->line_type = mx_no_line;
        }
    }
abort:;
}

void mx_rtf_importer::processNewTableRow(int& err, RTFSgroup* group,
    rtfDestinationType destination)
{
    // don't do table stuff in headers or footers
    switch (group->destination) {
    case RTFDheader:
    case RTFDfooter:
        return;
    default:
        break;
    }

    if (not_field_text_group(group)) {
        // move back into the table
        doc_cursor->get_position()->left(err, doc);
        MX_ERROR_CHECK(err);

        // fix the row borders
        fix_table_row_borders(err, doc, *doc_cursor->get_position());
        MX_ERROR_CHECK(err);

        // fix the column borders
        fix_table_column_borders(err, doc, *doc_cursor->get_position());
        MX_ERROR_CHECK(err);

        // move the cursor to the first position after the table
        while (doc_cursor->get_position()->get_sheet_pos()->get_area_pos(err)->rtti_class() == mx_table_area_pos_class_e) {
            MX_ERROR_CHECK(err);
            doc_cursor->get_position()->right(err, doc);
            MX_ERROR_CHECK(err);
        }
        MX_ERROR_CHECK(err);
    }
abort:;
}

void mx_rtf_importer::processNewTab(int& err, RTFSgroup* group,
    rtfDestinationType destination)
{
    const char* tab_string = "\t";
    text_fifo_elem_t* elem = nullptr;

    if (not_field_text_group(group)) {
        // stack up a space word with a tab in it
        elem = new text_fifo_elem_t;
        mx_word* w;

        // convert the RTF character style into a maxwell one.
        group->formatting.character.convertTo(err, elem->mx_cs, *this);
        MX_ERROR_CHECK(err);

        w = mx_paragraph::get_word_from_string(err, tab_string);
        MX_ERROR_CHECK(err);

        // insert the tab word into the list
        elem->mx_words.append(w);

        // append this chunk of text to the fifo
        text_fifo.append(elem);
        elem = nullptr;
    }

abort:
    if (elem)
        delete elem;
    return;
}

void mx_rtf_importer::processSpecialCharacter(
    int& err, RTFSgroup* group,
    rtfDestinationType destination,
    rtfControl control, int controlValue)
{
    mx_field_word fw;
    static const char bullet_text[] = { (char)183, 0 };
    mx_word* w = nullptr;

    // new text element
    text_fifo_elem_t* elem = new text_fifo_elem_t;

    if (not_field_text_group(group)) {
        set_up_character_set_lut(documentData.charset, &character_set_lookup);

        // convert the RTF character style into a maxwell one.
        group->formatting.character.convertTo(err, elem->mx_cs, *this);
        MX_ERROR_CHECK(err);

        switch (control) {
        case RTFsect:
            processNewParagraph(err, group, destination);
            MX_ERROR_CHECK(err);
            break;
        case RTFchdate:
            w = new mx_complex_word;
            fw = mx_field_word(mx_field_word::mx_date_time_field_e,
                mx_field_word::dt_none_e,
                mx_field_word::dt_dd_mm_yyyy_e);
            *((mx_complex_word*)w) += fw;
            break;
        case RTFchdpl:
        case RTFchdpa:
        case RTFchtime:
        case RTFchpgn:
            break;
        case RTFsectnum:
            break;
        case RTFchftn:
        case RTFchatn:
        case RTFchftnsep:
        case RTFchftnsepc:
            break;
        case RTFpage:
            processNewParagraph(err, group, destination);
            MX_ERROR_CHECK(err);
            break_before_next_para = true;
            break;
        case RTFcolumn:
            break;
        case RTFline:
            w = new mx_break_word(mx_line_break_e);
            break;
        case RTFemspace:
        case RTFenspace:
        case RTFsymtilda:
            w = new mx_simple_word(" ");
            break;
        case RTFbullet:
            w = new mx_simple_word(bullet_text);
            break;
        case RTFlquote:
            w = new mx_simple_word("'");
            break;
        case RTFrquote:
            w = new mx_simple_word("'");
            break;
        case RTFldblquote:
            w = new mx_simple_word("\"");
            break;
        case RTFrdblquote:
            w = new mx_simple_word("\"");
            break;
        case RTFsymbar:
            break;
        case RTFemdash:
        case RTFendash:
        case RTFsymhyphen:
        case RTFsymunderbar:
            w = new mx_simple_word("-");
            break;
        case RTFsymcolon:
        case RTFsymstar:
            break;
        case RTFsymquote: {
            char s[2] = "?";
            const char* t = s;
            s[0] = character_set_lookup[(uint8)controlValue];
            w = mx_paragraph::get_word_from_string(err, t);
            MX_ERROR_CHECK(err);
            break;
        }
        case RTFltrmark:
        case RTFrtlmark:
        case RTFzwj:
        case RTFzwnj:
        default:
            break;
        }

        if (w != nullptr) {
            // insert the word into the list
            elem->mx_words.append(w);

            // append this chunk of text to the fifo
            text_fifo.append(elem);
        } else {
            goto abort;
        }
    }
    return;
abort:
    delete elem;
    return;
}

void mx_rtf_importer::userProcessFontOutput(int& err, RTFSfont* rtf_font)
{
    mx_font* maxwell_font = new mx_font;

    rtf_font->convertTo(err, *maxwell_font);
    MX_ERROR_CHECK(err);

    m_font_lookup[rtf_font->f] = maxwell_font;
abort:;
}

void mx_rtf_importer::userProcessFileOutput(int& err, RTFSfile* rtf_file)
{
}

void mx_rtf_importer::userProcessColourOutput(int& err, RTFScolour* rtf_colour)
{
    mx_colour* maxwell_colour = new mx_colour;

    rtf_colour->convertTo(err, *maxwell_colour);
    MX_ERROR_CHECK(err);

    m_colour_lookup[rtf_colour->offset] = maxwell_colour;
abort:;
}

void mx_rtf_importer::userProcessStyleOutput(int& err, RTFSstyle* rtf_style)
{
    MX_ERROR_ASSERT(err, rtf_style->stylename != nullptr);
    m_para_style_lookup[rtf_style->styleNumber] = nullptr;
abort:;
}

bool mx_rtf_importer::update_progress()
{
    float percent = 100.0 * (float)file_offset / (float)file_length;

    if (pd == nullptr) {
        return false;
    } else {
        pd->set_progress((int)percent);
        return pd->peek_cancel();
    }
}

mx_font* mx_rtf_importer::get_font(int& err, int rtf_font_num)
{
    if (m_font_lookup.find(rtf_font_num) == m_font_lookup.end()) {
        return nullptr;
    } else {
        return m_font_lookup[rtf_font_num];
    }
}

mx_colour* mx_rtf_importer::get_colour(int& err, int rtf_colour_num)
{
    if (m_colour_lookup.find(rtf_colour_num) == m_colour_lookup.end()) {
        return nullptr;
    } else {
        return m_colour_lookup[rtf_colour_num];
    }
}

mx_char_style* mx_rtf_importer::get_char_style(int& err, int rtf_char_style_num)
{
    return nullptr;
}

mx_paragraph_style* mx_rtf_importer::get_para_style(int& err, int rtf_para_style_num)
{
    if (m_para_style_lookup.find(rtf_para_style_num) == m_para_style_lookup.end()) {
        return nullptr;
    } else {
        return m_para_style_lookup[rtf_para_style_num];
    }
}

bool mx_rtf_importer::not_field_text_group(RTFSgroup* group)
{
    if (group->destination != RTFDfldrslt) {
        field_inst_was_inserted = false;
    }
    return !field_inst_was_inserted;
}

static inline void skip_until_white_space(const char*& string)
{
    while (*string != '\0' && !isspace(*string))
        string++;
}

static inline void skip_white_space(const char*& string)
{
    while (*string != '\0' && isspace(*string))
        string++;
}

static void parse_symbol_field_inst(const char* symbol_field_inst,
    uint8& character,
    uint32& size,
    char*& font)
{
    const char* arg = symbol_field_inst;

    while (*arg != '\0') {
        skip_white_space(arg);

        if (strncasecmp(arg, "symbol", 6) == 0) {
            // instruction name not needed
            arg += 6;
        } else if (strncasecmp(arg, "0x", 2) == 0) {
            // got hexadecimal value of character
            character = strtol(arg, nullptr, 16);
            skip_until_white_space(arg);
        } else if (isdigit(arg[0])) {
            // got decimal value of character
            character = atoi(arg);
            skip_until_white_space(arg);
        } else if (strncasecmp(arg, "\\f", 2) == 0) {
            // got a font specification

            arg += 2;
            skip_white_space(arg);

            if (*arg == '\"') {
                arg++;

                font = mx_string_copy(arg);
                char* s = font;
                while (*s != '\"' && s != 0)
                    s++;
                *s = '\0';

                while (*arg != '\"' && *arg != '\0')
                    arg++;
                skip_until_white_space(arg);
            }
        } else if (strncasecmp(arg, "\\s", 2) == 0) {
            // got a size specification
            arg += 2;
            skip_white_space(arg);

            size = atoi(arg);
            while (!isspace(*arg))
                arg++;
        } else {
            skip_until_white_space(arg);
        }
    }
}

void mx_rtf_importer::processFieldInstOutput(int& err, RTFSgroup* group)
{
    mx_complex_word* cw = nullptr;
    text_fifo_elem_t* elem = nullptr;

    textBuffer[textBufferSize] = '\0';

    if (strncasecmp(textBuffer, "time", 4) == 0) {
        mx_field_word fw(mx_field_word::mx_date_time_field_e);
        cw = new mx_complex_word;
        *cw += fw;
    } else if (strncasecmp(textBuffer, "page", 4) == 0) {
        mx_field_word fw(mx_field_word::mx_page_num_field_e);
        cw = new mx_complex_word;
        *cw += fw;
    } else if (strncasecmp(textBuffer, "symbol", 6) == 0) {
        elem = new text_fifo_elem_t;

        // convert the RTF character style into a maxwell one.
        group->formatting.character.convertTo(err, elem->mx_cs, *this);
        MX_ERROR_CHECK(err);

        {
            uint32 size = (uint32)elem->mx_cs.get_font()->get_size();
            uint8 character = 183;
            char* fontname = nullptr;
            parse_symbol_field_inst(textBuffer,
                character, size, fontname);
            elem->mx_cs.get_font()->set_size(size);
            if (fontname != nullptr) {
                elem->mx_cs.get_font()->set_nearest_family(err, fontname);
                MX_ERROR_CHECK(err);

                delete[] fontname;
            }

            elem->mx_words.append(new mx_simple_word((char*)&character, 1));
            text_fifo.append(elem);
            elem = nullptr;

            field_inst_was_inserted = true;
        }
    }

    if (cw != nullptr) {
        elem = new text_fifo_elem_t;

        // convert the RTF character style into a maxwell one.
        group->formatting.character.convertTo(err, elem->mx_cs, *this);
        MX_ERROR_CHECK(err);

        // insert the word into the list
        elem->mx_words.append(cw);

        // append this chunk of text to the fifo
        text_fifo.append(elem);

        elem = nullptr;
        cw = nullptr;
        field_inst_was_inserted = true;
    }

abort:
    if (cw)
        delete cw;
    if (elem)
        delete elem;
}

void mx_rtf_importer::add_all_styles(int& err)
{
    mx_paragraph_style* default_ps = doc->get_default_style(err);
    MX_ERROR_CHECK(err);

    styles_are_added = true;

    for (auto i : m_para_style_lookup) {
        RTFSstyle* rtf_ps = (RTFSstyle*)i.second;

        if (rtf_ps->styleType == RTFTparagraphStyle) {
            mx_paragraph_style style_to_add;

            rtf_ps->convertTo(err, style_to_add, *this);
            MX_ERROR_CHECK(err);

            if (rtf_ps->styleNumber == 0) {
                style_to_add.set_name(default_ps->get_name());
            }

            if (do_full_import) {
                // replace the style.

                doc->add_or_replace_style(err, &style_to_add);
                MX_ERROR_CHECK(err);
            } else {
                // add the style if it doesn't exist

                doc->add_or_get_style(err, &style_to_add);
                MX_ERROR_CHECK(err);
            }
        }
    }

abort:;
}

mx_rtf_mxmetafile_output::mx_rtf_mxmetafile_output(mx_wp_document* idoc,
    mx_list* itext_fifo,
    mx_rtf_importer::text_fifo_elem_t* ielem)
{
    doc = idoc;
    text_fifo = itext_fifo;
    elem = ielem;
}

void mx_rtf_mxmetafile_output::processStretchDIBits(int& err,
    mx_standard_metafile_record& record,
    char** buffer)
{
    mx_image_area* a = nullptr;
    mx_area_cw_t* cwi;
    mx_complex_word* cw;
    mx_point croppedSize;

    // stack up the character style and the words

    err = MX_ERROR_OK;

    // read the basics

    mx_mxmetafile_output::processStretchDIBits(err, record, buffer);
    MX_ERROR_CHECK(err);

    // now we need to do the rtf import specific stuff

    a = new mx_image_area(doc->get_docid(), nullptr);
    MX_ERROR_CHECK(err);

    a->createMETAFILEarea(err, *this);
    MX_ERROR_CHECK(err);

    // now create a stored raster this is either a full import or a
    // subsampled raster

    a->set_outline(err, size.x, size.y);
    MX_ERROR_CHECK(err);

    // set an unknown pixel scale so we force an import of the whole raster
    a->createStoredRaster(err, size, -1.0);
    MX_ERROR_CHECK(err);

    a->deleteInputRaster();

    // now crop

    croppedSize = size;

    if ((crop.xb != 0.0) || (crop.yb != 0.0) || (crop.xt != 0.0) || (crop.yt != 0.0)) {
        croppedSize.x -= (crop.xb + crop.xt);
        croppedSize.y -= (crop.yb + crop.yt);

        a->crop(err, crop);
        MX_ERROR_CHECK(err);

        a->set_outline(err, croppedSize.x, croppedSize.y);
        MX_ERROR_CHECK(err);
    }

    /* now do some scaling */

    if ((scale.x != 1.0) || (scale.y != 1.0)) {
        croppedSize.x *= scale.x;
        croppedSize.y *= scale.y;

        a->scale(err, croppedSize);
        MX_ERROR_CHECK(err);

        a->set_outline(err, croppedSize.x, croppedSize.y);
        MX_ERROR_CHECK(err);
    }

    cwi = new mx_area_cw_t(a);
    cw = new mx_complex_word;
    cw->append(cwi);
    elem->mx_words.append(cw);

    // append this chunk of text to the fifo
    text_fifo->append(elem);
    elem = nullptr;

abort:;
    if (elem != nullptr)
        delete elem;
}

void mx_rtf_importer::processPictOutput(int& err,
    RTFSgroup* group)
{
    mx_metafile_buffer_input* input = nullptr;
    mx_rtf_mxmetafile_output* output = nullptr;
    mx_metafile* metafile = nullptr;
    text_fifo_elem_t* elem = nullptr;

    mx_point size, scale;
    mx_rect crop;

    err = MX_ERROR_OK;

    if (not_field_text_group(group)) {
        elem = new text_fifo_elem_t;

        // convert the RTF character style into a maxwell one.
        group->formatting.character.convertTo(err, elem->mx_cs, *this);
        MX_ERROR_CHECK(err);

        input = new mx_metafile_buffer_input(err, textBufferSize, textBuffer);
        MX_ERROR_CHECK(err);

        output = new mx_rtf_mxmetafile_output(doc, &text_fifo, elem);
        elem = nullptr;

        size.x = MX_TWIPS_TO_MM(pictData.picwgoal);
        size.y = MX_TWIPS_TO_MM(pictData.pichgoal);

        scale.x = pictData.picscalex / 100.0;
        scale.y = pictData.picscaley / 100.0;

        crop.xt = MX_TWIPS_TO_MM(pictData.piccropr);
        crop.xb = MX_TWIPS_TO_MM(pictData.piccropl);
        crop.yb = MX_TWIPS_TO_MM(pictData.piccropt);
        crop.yt = MX_TWIPS_TO_MM(pictData.piccropb);

        output->setSize(size);
        output->setScale(scale);
        output->setCrop(crop);

        metafile = new mx_metafile(input, output);

        metafile->process(err);
        MX_ERROR_CHECK(err);
    }

abort:
    if (elem != nullptr)
        delete elem;
    if (input != nullptr)
        delete input;
    if (output != nullptr)
        delete output;
    if (metafile != nullptr)
        delete metafile;
    return;
}
