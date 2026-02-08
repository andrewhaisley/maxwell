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
 * MODULE : rtfConvert.C
 *
 * AUTHOR : Tom Newton
 *
 * This file
 *
 * DESCRIPTION: methods on the various RTF objects to convert themselves into
 *              the relevant maxwell objects
 *
 *
 *
 *
 *
 */

#include <rtflocal.h>
#include <mx_para_style.h>
#include <mx_rtf_importer.h>
#include <mx_table_area.h>
#include <mx_text_area.h>
#include <mx_wp_doc.h>
#include <rtf.h>

void RTFSparBorder::convertTo(int& err, mx_line_style& ls,
    mx_rtf_importer& lookups)
{
    if (brdrw == 0) {
        ls.width = 0.25f; // quarter of a millimeter is the default width
    } else {
        ls.width = MX_TWIPS_TO_MM(brdrw);
    }

    mx_colour* c = lookups.get_colour(err, brdrcf);
    MX_ERROR_CHECK(err);

    ls.colour = *c;

    switch (brdrk) {
    case RTFbrdrs:
        ls.line_type = mx_solid;
        break;
    case RTFbrdrth:
        ls.line_type = mx_solid;
        ls.width *= 2.0f;
        break;
    case RTFbrdrsh:
        ls.line_type = mx_solid;
        break;
    case RTFbrdrdb:
        ls.line_type = mx_solid;
        break;
    case RTFbrdrdot:
        ls.line_type = mx_dotted;
        break;
    case RTFbrdrdash:
        ls.line_type = mx_dashed;
        break;
    case RTFbrdrhair:
        ls.line_type = mx_dotted;
        ls.width /= 2.0f;
        break;
    default:
        ls.line_type = mx_no_line;
        break;
    }

    // int         brsp;       // Border space
abort:;
}

void RTFSfont::convertTo(int& err, mx_font& maxwell_font)
{
    // try to get the font as specified
    if (!fontname || !maxwell_font.set_family(err, fontname)) {
        MX_ERROR_CLEAR(err);

        // try to get the alternative font as specified
        if (!falt || !maxwell_font.set_family(err, falt)) {
            MX_ERROR_CLEAR(err);

            switch (fontfamily) {
            case RTFfroman:
            case RTFfscript:
            case RTFfdecor:
                maxwell_font.set_family(err, mx_font::get_default_roman_font());
                break;
            case RTFfswiss:
                maxwell_font.set_family(err, "URW-Frutiger");
                break;
            case RTFfmodern:
                maxwell_font.set_family(err, "Adobe-Courier");
                break;
            case RTFftech:
                maxwell_font.set_family(err, mx_font::get_default_symbol_font());
                break;
            default:
                maxwell_font.set_nearest_family(err, fontname);
                break;
            }

            if (err != MX_ERROR_OK) {
                MX_ERROR_CLEAR(err);
                maxwell_font.set_nearest_family(err, fontname);
                MX_ERROR_CHECK(err);
            }
        }
    }
abort:;
}

void RTFScolour::convertTo(int& err, mx_colour& maxwell_colour)
{
    maxwell_colour.red = red;
    maxwell_colour.green = green;
    maxwell_colour.blue = blue;
    maxwell_colour.name = NULL;
}

// note the character format for this paragraph style should be set up before
// this is called
void RTFSparfmt::convertTo(int& err, mx_paragraph_style& ps, mx_rtf_importer& lookups)
{
    ps.keep_lines_together = keep;
    ps.keep_with_next = keepn;
    ps.space_before = MX_TWIPS_TO_MM(sb);
    ps.space_after = MX_TWIPS_TO_MM(sa);
    ps.page_break_before = pagebb;

    if (level != -1) {
        ps.is_toc = FALSE;
        ps.is_heading = TRUE;
        ps.heading_level = level;
    }

    if (sl > 0) {
        ps.line_spacing = (float)sl / (20.0f * ps.get_char_style()->get_font()->get_size());
    }

    ps.get_ruler()->default_tabstop_space = MX_TWIPS_TO_MM(lookups.documentData.deftab);
    ps.get_ruler()->left_indent = MX_TWIPS_TO_MM(li);
    ps.get_ruler()->right_indent = MX_TWIPS_TO_MM(ri);
    ps.get_ruler()->first_line_indent = MX_TWIPS_TO_MM(fi);

    switch (alignment) {
    case RTFqr:
        ps.justification = mx_right_aligned;
        break;
    case RTFqj:
        ps.justification = mx_justified;
        break;
    case RTFqc:
        ps.justification = mx_centred;
        break;
    case RTFql:
    default:
        ps.justification = mx_left_aligned;
        break;
    }
}

void RTFSparTab::convertTo(int& err, mx_tabstop& mx_t)
{

    if (tx != -1) {
        mx_t.position = MX_TWIPS_TO_MM(tx);
    } else if (tb != -1) {
        mx_t.position = MX_TWIPS_TO_MM(tb);
    } else {
        MX_ERROR_THROW(err, MX_INTERNAL_RTF_ERROR);
    }

    switch (tabkind) {
    case RTFtqr:
        mx_t.type = mx_right;
        break;
    case RTFtqc:
        mx_t.type = mx_centre;
        break;
    case RTFtqdec:
        mx_t.type = mx_centre_on_dp;
        break;
    default:
        mx_t.type = mx_left;
        break;
    }

    switch (tablead) {
    case RTFtldot:
        mx_t.leader_type = mx_leader_dot_e;
        break;
    case RTFtlhyph:
        mx_t.leader_type = mx_leader_hyphen_e;
        break;
    case RTFtlul:
        mx_t.leader_type = mx_leader_underline_e;
        break;
    case RTFtlth:
        mx_t.leader_type = mx_leader_thickline_e;
        break;
    case RTFtleq:
        mx_t.leader_type = mx_leader_equals_e;
        break;
    default:
        mx_t.leader_type = mx_leader_space_e;
        break;
    }
abort:;
}

void RTFSpar::convertTo(int& err, mx_paragraph_style& maxwell_para_style,
    mx_rtf_importer& lookups)
{
    mx_border_style* bd = NULL;
    mx_tabstop mx_t;
    mx_ruler* mx_r = maxwell_para_style.get_ruler();
    MX_ERROR_CHECK(err);

    // convert the paragraph style.

    fmt.convertTo(err, maxwell_para_style, lookups);
    MX_ERROR_CHECK(err);

    // convert the borders

    bd = maxwell_para_style.get_border_style();

    if (box.brdrk != RTFunknownControl) {
        bd->distance_from_contents = MX_TWIPS_TO_MM(box.brsp);

        box.convertTo(err, bd->top_style, lookups);
        MX_ERROR_CHECK(err);

        bd->bottom_style = bd->left_style = bd->right_style = bd->top_style;
    } else {
        // bit of a fudge, but the best we can do

        bd->distance_from_contents = MX_TWIPS_TO_MM(brdrl.brsp);

        brdrt.convertTo(err, bd->top_style, lookups);
        MX_ERROR_CHECK(err);
        brdrb.convertTo(err, bd->bottom_style, lookups);
        MX_ERROR_CHECK(err);
        brdrl.convertTo(err, bd->left_style, lookups);
        MX_ERROR_CHECK(err);
        brdrr.convertTo(err, bd->right_style, lookups);
        MX_ERROR_CHECK(err);
    }

    // convert the shading
    shading.convertTo(err, bd->fill_style, lookups);
    MX_ERROR_CHECK(err);

    // convert the tabs
    mx_r->clear_user_tabs();

    for (auto i : tabs) {
        i.second->convertTo(err, mx_t);
        MX_ERROR_CHECK(err);

        mx_r->add_tab(err, mx_t);
        MX_ERROR_CHECK(err);
    }

abort:;
}

void RTFScharacter::convertTo(int& err, mx_char_style& maxwell_st,
    mx_rtf_importer& lookups)
{
    fmt.convertTo(err, maxwell_st, lookups);
}

void RTFSchrfmt::convertTo(int& err, mx_char_style& maxwell_st,
    mx_rtf_importer& lookups)
{
    mx_font* mf = NULL;

    if (this->f != -1) {
        mf = lookups.get_font(err, this->f);
        MX_ERROR_CHECK(err);

        maxwell_st.set_font(*mf);
    }

    if (this->cf != -1) {
        maxwell_st.colour = *lookups.get_colour(err, this->cf);
        MX_ERROR_CHECK(err);
    }

    mf = maxwell_st.get_font();

    mf->set_bold(err, this->b);
    MX_ERROR_CLEAR(err); // ignore no bold errors
    mf->set_italic(err, this->i);
    MX_ERROR_CLEAR(err); // ignore no italic errors

    maxwell_st.effects = ul ? mx_underline : mx_no_effects;
    maxwell_st.alignment = sub ? mx_subscript : (super ? mx_superscript : mx_normal_align);

    maxwell_st.alignment_offset = sub ? dn : super ? up
                                                   : 0;

    if (fs > 0)
        mf->set_size(0.5 * fs);

    if (expnd != -1) {
        // expnd is quarter points additional space
        maxwell_st.set_width_adjust = (char)(0.5 + (100.0 * (double)expnd / (4.0 * mf->get_size())));
    }

    if (expndtw != -1) {
        // expndtw is twentieth points additional space
        maxwell_st.set_width_adjust = (char)(0.5 + (100.0 * (double)expndtw / (20.0 * mf->get_size())));
    }

abort:;
}

inline static void set_heading_toc_level(mx_paragraph_style& ps, const char* sn)
{
    if (strncasecmp(sn, "toc ", 4) == 0 && isdigit(sn[4])) {
        int level = atoi(sn + 4);
        if (level >= 0 && level < 255) {
            ps.is_toc = TRUE;
            ps.is_heading = FALSE;
            ps.heading_level = level;
        }
    } else if (strncasecmp(sn, "heading ", 8) == 0 && isdigit(sn[8])) {
        int level = atoi(sn + 8);
        if (level >= 0 && level < 255) {
            ps.is_toc = FALSE;
            ps.is_heading = TRUE;
            ps.heading_level = level;
        }
    } else {
        ps.is_toc = ps.is_heading = FALSE;
        ps.heading_level = 0;
    }
}

void RTFSstyle::convertTo(int& err, mx_paragraph_style& maxwell_para_style,
    mx_rtf_importer& lookups)
{
    MX_ERROR_ASSERT(err, stylename != NULL);

    lookups.update_style(err, this);
    MX_ERROR_CHECK(err);

    MX_ERROR_ASSERT(err, updated);

    // if the style name is "toc ?" or "heading ?", then set the toc or
    // heading flags and levels respectively
    set_heading_toc_level(maxwell_para_style, stylename);

    // only set the name if this is not the default paragraph style
    if (styleNumber != 0)
        maxwell_para_style.set_name(stylename);
    formatting.convertTo(err, maxwell_para_style, lookups);
    MX_ERROR_CHECK(err);
abort:;
}

void RTFSformatting::convertTo(int& err,
    mx_paragraph_style& maxwell_para_style,
    mx_rtf_importer& lookups)
{
    character.convertTo(err, *maxwell_para_style.get_char_style(), lookups);
    MX_ERROR_CHECK(err);

    paragraph.convertTo(err, maxwell_para_style, lookups);
    MX_ERROR_CHECK(err);

abort:;
}

void RTFScell::convertTo(int& err, mx_table_area& ta,
    mx_rtf_importer& lookups, int row, int column)
{
    mx_text_area* cell;
    mx_paragraph_style ps;
    mx_border_style* bd;

    cell = ta.get_cell(err, row, column);
    MX_ERROR_CHECK(err);

    ps = *cell->get_paragraph_style();
    bd = ps.get_border_style();
    clbrdrt.convertTo(err, bd->top_style, lookups);
    MX_ERROR_CHECK(err);
    clbrdrl.convertTo(err, bd->left_style, lookups);
    MX_ERROR_CHECK(err);
    clbrdrb.convertTo(err, bd->bottom_style, lookups);
    MX_ERROR_CHECK(err);
    clbrdrr.convertTo(err, bd->right_style, lookups);
    MX_ERROR_CHECK(err);
    cellshad.convertTo(err, bd->fill_style, lookups);
    MX_ERROR_CHECK(err);

    cell->modify_paragraph_style(ps);

abort:;
}

void RTFSparShading::convertTo(int& err, mx_fill_style& fs,
    mx_rtf_importer& lookups)
{
    float fg_bg_ratio, bg_fg_ratio;
    mx_colour fg_col, bg_col;

    if (cfpat != -1) {
        fg_col = *lookups.get_colour(err, cfpat);
        MX_ERROR_CHECK(err);
    } else {
        fg_col = mx_colour(0, 0, 0, NULL);
    }

    if (cbpat != -1) {
        bg_col = *lookups.get_colour(err, cbpat);
        MX_ERROR_CHECK(err);
    } else {
        bg_col = mx_colour(255, 255, 255, NULL);
    }

    // for all patterns other than shaded ones assume a a %50 shading
    if (pat != RTFunknownControl)
        shading = 5000;

    // default shading is clear(ie 0)
    if (shading == -1)
        shading = 0;

    fg_bg_ratio = (float)shading / 10000.0;
    bg_fg_ratio = 1.0 - (float)shading / 10000.0;
    fs.colour.red = (uint8)(0.5 + bg_fg_ratio * bg_col.red + fg_bg_ratio * fg_col.red);
    fs.colour.green = (uint8)(0.5 + bg_fg_ratio * bg_col.green + fg_bg_ratio * fg_col.green);
    fs.colour.blue = (uint8)(0.5 + bg_fg_ratio * bg_col.blue + fg_bg_ratio * fg_col.blue);
    fs.colour.name = NULL;

    if (fs.colour.red == fs.colour.green && fs.colour.green == fs.colour.blue && fs.colour.blue == 255) {
        fs.type = mx_fill_transparent;
    } else {
        fs.type = mx_fill_colour;
    }

abort:;
}

// merges the formatting properties of the cells which have the \clmgf and
// \clmrg flags set. Does not delete the merged cells, but creates a list of
// booleans which describes which cells have been merged
static int merge_cells(int& err, mx_list& cells_to_merge, mx_list& merged_cells)
{
    RTFScell *c = NULL, *first_c = NULL;
    int i = 0, num_cells = 0, num_merged_cells = 0;

    num_cells = cells_to_merge.get_num_items();

    merged_cells = mx_list();

    for (i = 0; i < num_cells; i++) {
        c = (RTFScell*)cells_to_merge.get(err, i);
        MX_ERROR_CHECK(err);

        merged_cells.append((void*)c->clmrg);
        if (!c->clmrg) {
            if (c->clmgf) {
                first_c = c;
            } else {
                first_c = NULL;
            }
            num_merged_cells++;
        } else {
            if (first_c == NULL) {
                MX_ERROR_THROW(err, MX_INTERNAL_RTF_ERROR);
            } else {
                first_c->merge(err, *c);
                MX_ERROR_CHECK(err);
            }
        }
    }

abort:
    return num_merged_cells;
}

// this routine converts an rtf table definition into a maxwell table row, and
// returns a list of booleans indicating which cells have been merged.
void RTFStable::convertTo(int& err, mx_table_area& ta,
    mx_rtf_importer& lookups, int row,
    mx_list& merged_cells)
{
    int i = 0, num_rtf_cols = cells.get_num_items();

    // if the cells list has some objects then this row has a different format
    // to the previous row, and we are in a new table, so set up the row
    // format. Other wise copy the previous rows, format.

    if (num_rtf_cols > 0) {
        mx_paragraph_style new_style;
        int x_pos = 0, cell_width = 0, num_merged_rtf_cols = 0, j = 0;

        // merge any merged cells, and create the proper list of cells
        num_merged_rtf_cols = merge_cells(err, cells, merged_cells);
        MX_ERROR_CHECK(err);

        ta.set_num_columns(err, num_merged_rtf_cols);
        MX_ERROR_CHECK(err);

        for (i = 0; i < num_rtf_cols; i++) {
            bool merged;
            RTFScell* cell;

            cell = (RTFScell*)cells.get(err, i);
            MX_ERROR_CHECK(err);

            merged = (bool)merged_cells.get(err, i);
            MX_ERROR_CHECK(err);

            if (!merged) {
                float old_column_width, new_column_width;
                cell_width = cell->cellx - x_pos;

                new_column_width = MX_TWIPS_TO_MM(cell_width);
                old_column_width = ta.get_column_width(err, j);
                MX_ERROR_CHECK(err);

                if (new_column_width != old_column_width) {
                    ta.set_column_width(err, j, new_column_width);
                    MX_ERROR_CHECK(err);
                }

                x_pos += cell_width;

                cell->convertTo(err, ta, lookups, row, j);
                MX_ERROR_CHECK(err);
                j++;
            }
        }

        new_style = *ta.get_area_style();

        // set up the table justification

        switch (rowjust) {
        case RTFtrqr:
            new_style.justification = mx_right_aligned;
            break;
        case RTFtrqc:
            new_style.justification = mx_centred;
            break;
        case RTFtrql:
        default:
            new_style.justification = mx_left_aligned;
            break;
        }
        new_style.keep_with_next = trkeep;
        ta.modify_paragraph_style(new_style);
    } else {
        // there are no cell definitions for this row so use the paragraph
        // style from the cell above

        int num_table_cols = ta.get_num_columns(err);
        MX_ERROR_CHECK(err);

        for (i = 0; i < num_table_cols; i++) {
            mx_text_area *cell, *prev_cell;

            prev_cell = ta.get_cell(err, row - 1, i);
            MX_ERROR_CHECK(err);

            cell = ta.get_cell(err, row, i);
            MX_ERROR_CHECK(err);

            cell->modify_paragraph_style(*prev_cell->get_paragraph_style());
        }
    }

abort:;
}

void RTFSpage::convertTo(int& err, mx_wp_document& doc,
    mx_wp_doc_pos& doc_pos)
{
    doc.set_page_size(err, MX_TWIPS_TO_MM(paperw), MX_TWIPS_TO_MM(paperh),
        landscape);
    MX_ERROR_CHECK(err);

    doc.set_page_margins(err,
        MX_TWIPS_TO_MM(margl),
        MX_TWIPS_TO_MM(margr),
        MX_TWIPS_TO_MM(margt),
        MX_TWIPS_TO_MM(margb));
    MX_ERROR_CHECK(err);

abort:;
}

void RTFSdocfmt::convertTo(int& err, mx_wp_document& doc,
    mx_wp_doc_pos& doc_pos,
    mx_rtf_importer& lookups)
{
    mx_font* def_font;

    mx_paragraph_style* ps = NULL;

    page.convertTo(err, doc, doc_pos);
    MX_ERROR_CHECK(err);

    ps = doc.get_default_style(err);
    MX_ERROR_CHECK(err);

    def_font = lookups.get_font(err, deff);
    MX_ERROR_CHECK(err);

    ps->get_char_style()->get_font()->set_family(err, def_font->get_family().c_str());
    MX_ERROR_CHECK(err);
    ps->get_ruler()->default_tabstop_space = MX_TWIPS_TO_MM(deftab);

abort:;
}
