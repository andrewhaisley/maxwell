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
 * MODULE/CLASS :   mx_wp_editor
 *
 * AUTHOR : Andrew Haisley
 *
 *
 *
 * DESCRIPTION:
 *
 *  insert menu routines for wp editor.
 *
 *
 *
 *
 *
 */
#include <mx_ap_iter.h>
#include <mx_list_iter.h>
#include <mx_ascii_importer.h>
#include <mx_cw_t.h>
#include <mx_paragraph.h>
#include <mx_rtf_importer.h>
#include <mx_ta_pos.h>
#include <mx_word_6_importer.h>
#include <mx_wp_editor.h>
#include <mx_wp_ob_buff.h>

#include <mx_date_d.h>
#include <mx_open_d.h>
#include <mx_symbol_d.h>
#include <mx_table_d.h>

#include <mx_undo.h>
#include <mx_wp_menubar.h>
#include <mx_wp_toolbar.h>
#include <mx_xframe.h>

#include <mx_dg_editor.h>

void mx_wp_editor::insert_index(int& err) { }
void mx_wp_editor::insert_index_entry(int& err) { }

void mx_wp_editor::insert_page_break(int& err)
{
    cursor.insert_page_break(err);
    MX_ERROR_CHECK(err);

    ur_buffer->mark_cant_undo(err);
    MX_ERROR_CHECK(err);

    update_all_sensitivities(err);
    MX_ERROR_CHECK(err);
abort:;
}

void mx_wp_editor::insert_page_number(int& err)
{
    mx_field_word_cw_t* f;

    f = new mx_field_word_cw_t(
        mx_field_word(mx_field_word::mx_page_num_field_e));

    cursor.insert_cw_item(err, f);
    MX_ERROR_CHECK(err);

    cleanup_after_cursor(err);
    MX_ERROR_CHECK(err);

    update_file_sensitivities(err);
    MX_ERROR_CHECK(err);

abort:;
}

void mx_wp_editor::insert_date_time(int& err)
{
    mx_date_d* d;

    d = dialog_man.get_date_d();

    if (d->run() == yes_e) {
        mx_field_word_cw_t* f;
        f = new mx_field_word_cw_t(mx_field_word(mx_field_word::mx_date_time_field_e,
            d->time_format,
            d->date_format));
        cursor.insert_cw_item(err, f);
        MX_ERROR_CHECK(err);

        cleanup_after_cursor(err);
        MX_ERROR_CHECK(err);

        update_file_sensitivities(err);
        MX_ERROR_CHECK(err);
    }
abort:;
}

void mx_wp_editor::insert_image(int& err)
{
    static mx_file_type_t file_type = mx_all_file_e;
    mx_open_d* d;
    mx_image_area* a = NULL;
    mx_point areaSize;
    mx_point paraSize;
    double pixelResolution;
    mx_paragraph_style* cps;
    bool b;

    cps = cursor.style_at_position(err);
    MX_ERROR_CHECK(err);

    d = dialog_man.get_open_d();

    if (d->run(mx_image_file_class_e, file_type, false) == yes_e) {
        file_type = mx_guess_file_type(d->file_name(b));

        a = new mx_image_area(document->get_docid(), cps);
        MX_ERROR_CHECK(err);

        switch (file_type) {
        case mx_tiff_file_e:
            a->createTIFFarea(err, d->file_name(b));
            break;
        case mx_jpg_file_e:
            a->createJPEGarea(err, d->file_name(b), false, 16);
            break;
        case mx_xpm_file_e:
            a->createXPMarea(err, d->file_name(b));
            break;
        case mx_xbm_file_e:
            a->createXBMarea(err, d->file_name(b));
            break;
        case mx_png_file_e:
            a->createPNGarea(err, d->file_name(b));
            break;
        default:
            dialog_man.run_inform_d(inform_e, "cantImportFileType");
            file_type = mx_all_file_e;
            return;
        }
        MX_ERROR_CHECK(err);

        // calculate a size - based on current paragraph size and the
        // display pixel size the general rules are
        //
        //
        // if( raster hasnt a  defined size)
        // {
        //      define raster size to be 1-1 with pixel
        //      display size now
        //
        //  }
        //
        //  if(raster fits on page)
        //  {
        //      use the raster size
        //  }
        //  else
        //  {
        //      scale raster so it fits on this page
        //   }
        // }

        // these values need to be set correctly
        // they are the sheet size in mm and
        // the number of pixels per mm on the display

        b = cursor.get_position()->is_paragraph(err);
        MX_ERROR_CHECK(err);

        if (b) {
            mx_paragraph* p
                = cursor.get_position()->get_para(err, document);
            MX_ERROR_CHECK(err);

            paraSize.x = p->get_format_width();
        } else {
            paraSize.x = cursor.get_sheet_width(err);
            MX_ERROR_CHECK(err);
        }

        paraSize.y = cursor.get_sheet_height(err);
        MX_ERROR_CHECK(err);

        pixelResolution = cursor.get_device()->getScreenResolution();

        areaSize = a->areaSize(err,
            paraSize,
            pixelResolution);
        MX_ERROR_CHECK(err);

        // now create a stored raster this is easier a full import or a
        // subsampled raster

        a->set_outline(err, areaSize.x, areaSize.y);
        MX_ERROR_CHECK(err);

        a->createStoredRaster(err, areaSize, pixelResolution);
        MX_ERROR_CHECK(err);

        a->deleteInputRaster();

        mx_ur_item* ur_item = new mx_ur_item(mx_undo_typing_e,
            *(cursor.get_position()));
        ur_buffer->insert(err, ur_item, document);
        MX_ERROR_CHECK(err);

        // now insert the image
        cursor.insert_cw_item(err, new mx_area_cw_t(a));
        MX_ERROR_CHECK(err);

        a = NULL;

        cleanup_after_cursor(err);
        MX_ERROR_CHECK(err);

        update_file_sensitivities(err);
        MX_ERROR_CHECK(err);
    }

    return;
abort:
    a->deleteInputRaster();
    if (a)
        delete a;
    return;
}

void mx_wp_editor::insert_diagram(int& err)
{
    mx_diagram_area* a;
    mx_paragraph_style* cps;
    mx_point padd;
    mx_point centre;
    mx_point size;
    mx_point qsize(100, 100);

    cps = cursor.style_at_position(err);
    MX_ERROR_CHECK(err);

    a = new mx_diagram_area(document->get_docid(), cps);
    MX_ERROR_CHECK(err);

    // set a dummy size
    a->set_outline(err, 100, 100);
    MX_ERROR_CHECK(err);

    a->initialiseQuadSize(qsize);

    {
        mx_rect r(0, 0, 100, 100);

        mx_all_styles* styles = a->getAllStyles();
        mx_point c(20, 20);
        mx_point s(10, 10);

        mx_arc ar(c, s, 0, 1);

        gmx_arc gar(ar);
        gmx_polypoint gppoint;
        mx_rect rpp(10, 20, 70, 80);

        gmx_rect rr(rpp);

        bool doThis = false;

        gmx_span span(r);
        int borderStyleId;
        int currentId[4];
        int colourId[27];

        int defCol[27][3] = {
            { 0, 0, 0 },
            { 0, 0, 127 },
            { 0, 0, 255 },
            { 0, 127, 0 },
            { 0, 127, 127 },
            { 0, 127, 255 },
            { 0, 255, 0 },
            { 0, 255, 127 },
            { 0, 255, 255 },
            { 127, 0, 0 },
            { 127, 0, 127 },
            { 127, 0, 255 },
            { 127, 127, 0 },
            { 127, 127, 127 },
            { 127, 127, 255 },
            { 127, 255, 0 },
            { 127, 255, 127 },
            { 127, 255, 255 },
            { 255, 0, 0 },
            { 255, 0, 127 },
            { 255, 0, 255 },
            { 255, 127, 0 },
            { 255, 127, 127 },
            { 255, 127, 255 },
            { 255, 255, 0 },
            { 255, 255, 127 },
            { 255, 255, 255 }
        };

        for (int i = 0; i < 27; i++) {
            mx_colour* colour = new mx_colour;
            colour->red = defCol[i][0];
            colour->green = defCol[i][1];
            colour->blue = defCol[i][2];
            colour->name = NULL;
            colourId[i] = styles->insertColour(err, colour);
        }

        float dashes[3] = { 3, 4, 7 };
        mx_dg_level_key newLevel1 = a->addLevel(err, "1");
        mx_dg_level_key newLevel2 = a->addLevel(err, "2");

        mx_gborder_style* borderStyle = new mx_gborder_style("ffadf4");
        mx_garea_style* areaStyle = new mx_garea_style("ffadf4");
        mx_garea_style* areaStyle2 = new mx_garea_style("ffadf4");
        mx_gfill_style* fillStyle = new mx_gfill_style("ffadf4");
        int areaStyleId2, fillStyleId;

        mx_gdash_style* dashStyle1 = new mx_gdash_style("ffadf4");
        currentId[3] = styles->insert(err, dashStyle1);

        dashStyle1->style.set(3, dashes, 0, currentId[3]);

        mx_gline_style* lineStyle1 = new mx_gline_style("ffadf1");

        lineStyle1->style.width = 1;
        //       	lineStyle1->style.setDashStyle(dashStyle1->style) ;
        lineStyle1->style.arrow_type = mx_arrow_both;
        lineStyle1->style.arrow_pattern = mx_arrow_triangle;

        currentId[0] = styles->insert(err, lineStyle1);
        lineStyle1->style.line_id = currentId[0];

        fillStyle->style.type = mx_fill_colour;
        fillStyle->style.colour_id = colourId[7];
        fillStyleId = styles->insert(err, fillStyle);

        areaStyle->style.border_style.fill_style.fill_id = fillStyleId;
        areaStyle->style.border_style.top_style.line_id = currentId[0];
        areaStyle->style.border_style.top_style.colour_id = colourId[5];

        (void)styles->insert(err, areaStyle);

        mx_gline_style* lineStyle2 = new mx_gline_style("ffadf2");
        mx_gline_style* lineStyle3 = new mx_gline_style("ffadf3");

        lineStyle2->style.width = 1;
        lineStyle2->style.cap_type = mx_cap_round;

        currentId[1] = styles->insert(err, lineStyle2);
        lineStyle2->style.line_id = currentId[1];

        currentId[2] = styles->insert(err, lineStyle3);
        lineStyle3->style.line_id = currentId[2];

        mx_ghatch_style* hatchStyle = new mx_ghatch_style("ffadf3");

        hatchStyle->style.angle[0] = 0;
        hatchStyle->style.angle[1] = PI / 2;

        hatchStyle->style.spacing[0] = 15;
        hatchStyle->style.spacing[1] = 20;

        hatchStyle->style.line_style[0].line_id = currentId[1];
        hatchStyle->style.line_style[1].line_id = currentId[2];
        hatchStyle->style.line_style[0].colour_id = colourId[12];
        hatchStyle->style.line_style[1].colour_id = colourId[21];
        hatchStyle->style.hatch_id = styles->insert(err, hatchStyle);

        borderStyle->style.top_style.line_id = currentId[0];
        borderStyle->style.top_style.colour_id = colourId[5];

        borderStyle->style.left_style.line_id = currentId[1];
        borderStyle->style.left_style.colour_id = colourId[7];

        borderStyle->style.bottom_style.line_id = currentId[0];
        borderStyle->style.bottom_style.colour_id = colourId[10];

        borderStyle->style.right_style.line_id = currentId[1];
        borderStyle->style.right_style.colour_id = colourId[3];

        borderStyleId = styles->insert(err, borderStyle);

        areaStyle2->style.border_style.border_id = borderStyleId;
        areaStyle2->style.hatch_style.hatch_id = hatchStyle->style.hatch_id;

        areaStyleId2 = styles->insert(err, areaStyle2);

        gppoint.colourId = colourId[7];
        gppoint.styleId = currentId[0];

        rr.styleId = borderStyleId;
        rr.colourId = colourId[7];

        doThis = 1;
        if (doThis) {
            rr.r.setArea(true);
            rr.styleId = areaStyleId2;
            a->addGeometry(err, newLevel1, rr);
        }
        doThis = 0;

        if (doThis) {
            for (int i = 0; i < 100; i++) {
                padd.x = rand() % 100;
                padd.y = rand() % 100;
                gppoint.pl.addPoint(padd);
            }
            a->addGeometry(err, newLevel1, gppoint);
        }

        gar.colourId = colourId[0];
        gar.styleId = currentId[0];

        doThis = false;
        if (doThis) {
            //	  a->addGeometry(err,newLevel1,gar) ;

            centre.x = 50;
            centre.y = 50;
            size.x = 10;
            size.y = 10;

            gar.a.setArc(centre, size, 0.0, PI, ARC_ARC);
            a->addGeometry(err, newLevel1, gar);

            size.x = 20;
            size.y = 20;

            gar.a.setArc(centre, size, PI, PITWO, ARC_ARC);
            a->addGeometry(err, newLevel1, gar);

            size.x = 30;
            size.y = 30;

            gar.a.setArc(centre, size, PI / 4, PI / 2, ARC_ARC);
            a->addGeometry(err, newLevel1, gar);

            size.x = 40;
            size.y = 40;

            gar.a.setArc(centre, size, PI / 4 + PI, PI / 2 + PI, ARC_ARC);
            a->addGeometry(err, newLevel1, gar);
        }

        doThis = true;
        if (doThis) {
            padd.x = 20;
            padd.y = 20;
            gppoint.pl.addPoint(padd);

            gppoint.styleId = areaStyleId2;
            gppoint.pl.set_area(true);

            padd.x = 20;
            padd.y = 20.01;
            gppoint.pl.addPoint(padd);

            padd.x = 20;
            padd.y = 80;
            gppoint.pl.addPoint(padd);

            padd.x = 79.99;
            padd.y = 80;
            gppoint.pl.addPoint(padd);

            padd.x = 80;
            padd.y = 80;
            gppoint.pl.addPoint(padd);

            a->addGeometry(err, newLevel1, gppoint);
        }

        doThis = false;
        if (doThis) {
            span.s.xt = rand() % 100;
            span.s.xb = rand() % 100;
            span.s.yt = rand() % 100;
            span.s.yb = rand() % 100;

            span.colourId = colourId[0];
            span.styleId = currentId[0];

            a->addGeometry(err, newLevel1, span);
        }

        doThis = false;

        if (doThis) {
            for (int i = 0; i < 10000; i++) {

                span.s.xt = rand() % 100;
                span.s.xb = rand() % 100;
                span.s.yt = rand() % 100;
                span.s.yb = rand() % 100;

                span.colourId = colourId[i % 27];
                span.styleId = currentId[i % 3];

                if (i % 2) {
                    a->addGeometry(err, newLevel1, span);
                } else {
                    a->addGeometry(err, newLevel2, span);
                }
            }
        }
        (void)new mx_dg_editor(err, *app, a);
    }

    mx_ur_item* ur_item;
    ur_item = new mx_ur_item(mx_undo_typing_e, *(cursor.get_position()));

    ur_buffer->insert(err, ur_item, document);
    MX_ERROR_CHECK(err);

    // now insert the diagram
    cursor.insert_cw_item(err, new mx_area_cw_t(a));
    MX_ERROR_CHECK(err);

    cleanup_after_cursor(err);
    MX_ERROR_CHECK(err);

    update_file_sensitivities(err);
    MX_ERROR_CHECK(err);

abort:;
}

void mx_wp_editor::insert_table(int& err)
{
    mx_table_d* d;
    mx_table_area* a;
    float w;
    int i;

    w = cursor.get_sheet_width(err);
    MX_ERROR_CHECK(err);

    d = dialog_man.get_table_d(w, current_units);
    MX_ERROR_CHECK(err);

    if (d->run(w, current_units) == yes_e) {
        mx_ur_item* ur_item;
        mx_para_pos* pp;

        pp = cursor.get_position()->get_para_pos(err);
        MX_ERROR_CHECK(err);

        if (!pp->is_start()) {
            cursor.handle_return(err, false);
            MX_ERROR_CHECK(err);
        }

        ur_item = new mx_ur_item(
            err,
            mx_undo_insert_table_e,
            document,
            *(cursor.get_position()),
            d->num_rows,
            d->num_columns,
            d->column_width_mm);
        MX_ERROR_CHECK(err);

        ur_buffer->insert(err, ur_item);
        MX_ERROR_CHECK(err);

        a = cursor.insert_table(err);
        MX_ERROR_CHECK(err);

        a->set_num_rows(err, d->num_rows);
        MX_ERROR_CHECK(err);

        a->set_num_columns(err, d->num_columns);
        MX_ERROR_CHECK(err);

        for (i = 0; i < d->num_columns; i++) {
            a->set_column_width(err, i, d->column_width_mm);
            MX_ERROR_CHECK(err);
        }

        cleanup_after_cursor(err);
        MX_ERROR_CHECK(err);

        update_all_sensitivities(err);
        MX_ERROR_CHECK(err);
    }
abort:;
}

void mx_wp_editor::insert_symbol(int& err)
{
    mx_symbol_d* d = nullptr;
    mx_paragraph_style* s;
    mx_font f;

    s = document->get_para_style(err, cursor.get_position());
    MX_ERROR_CHECK(err);

    f = *s->get_char_style()->get_font();
    d = dialog_man.get_symbol_d(&f);

    d->centre();
    d->activate_d(&f);

    while (d->run_modal() == yes_e) {
        if (f.em_width(d->selected_character) != 0) {
            cursor.insert_char(err, d->selected_character);
            MX_ERROR_CHECK(err);

            frame->refresh(err);
            MX_ERROR_CHECK(err);

            cursor.update_caret(err);
            MX_ERROR_CHECK(err);

            cursor.make_visible(err);
            MX_ERROR_CHECK(err);

            update_toolbar(err);
            MX_ERROR_CHECK(err);

            update_all_sensitivities(err);
            MX_ERROR_CHECK(err);
        }
    }
abort:
    d->deactivate();
}

void mx_wp_editor::insert_file(int& err)
{
    static mx_file_type_t file_type = mx_document_file_e;

    mx_open_d* d;
    mx_ur_item* ur_item;

    bool b;

    d = dialog_man.get_open_d();
    mx_importer* i = NULL;
    mx_wp_doc_pos insert_start = (*cursor.get_position());
    mx_wp_doc_pos insert_end;

    if (d->run(mx_document_file_class_e, file_type, false) != yes_e)
        return;

    file_type = mx_guess_file_type(d->file_name(b));

    switch (file_type) {
    case mx_rtf_file_e:
        i = new mx_rtf_importer(err, d->file_name(b), document, &cursor, &dialog_man);
        MX_ERROR_CHECK(err);
        break;
    case mx_document_file_e:
        import_maxwell(err, d->file_name(b));
        MX_ERROR_CHECK(err);
        file_type = mx_document_file_e;
        return;
        break;
    case mx_ascii_file_e:
        i = new mx_ascii_importer(err, d->file_name(b), document, &cursor, &dialog_man);
        MX_ERROR_CHECK(err);
        break;
    case mx_word_6_file_e:
        i = new mx_word_6_importer(err, d->file_name(b), document, &cursor, &dialog_man);
        MX_ERROR_CHECK(err);
        break;
    case mx_png_file_e:
    case mx_jpg_file_e:
    case mx_xbm_file_e:
    case mx_xpm_file_e:
    case mx_ps_file_e:
    case mx_tiff_file_e:
        dialog_man.run_inform_d(inform_e, "tryInsertImage");
        break;
    default:
        dialog_man.run_inform_d(inform_e, "cantImportFileType");
        file_type = mx_document_file_e;
        break;
    }

    i->import(err);

    if (err == MX_USER_CANCEL) {
        MX_ERROR_CLEAR(err);
    } else if (err != MX_ERROR_OK) {
        dialog_man.run_inform_d(inform_e, "errorDuringImport");
        MX_ERROR_CLEAR(err);
    }

    toolbar->set_style_pulldown(document);

    insert_end = (*cursor.get_position());

    insert_start.save(err, document);
    MX_ERROR_CHECK(err);

    insert_end.save(err, document);
    MX_ERROR_CHECK(err);

    document->reformat(err);
    MX_ERROR_CHECK(err);

    set_sheet_layout(err, document);
    MX_ERROR_CHECK(err);

    frame->resetSheetLayout(err);
    MX_ERROR_CHECK(err);

    cursor.sheet_size_changed(err);
    MX_ERROR_CHECK(err);

    cursor.update_caret(err);
    MX_ERROR_CHECK(err);

    insert_start.restore(err, document);
    MX_ERROR_CHECK(err);

    insert_end.restore(err, document);
    MX_ERROR_CHECK(err);

    cursor.set_selection(err, insert_start, insert_end);
    MX_ERROR_CHECK(err);

    ur_item = new mx_ur_item(mx_undo_paste_e, document, insert_start, insert_end, false);

    ur_buffer->insert(err, ur_item);
    MX_ERROR_CHECK(err);

    frame->refresh(err);
    MX_ERROR_CHECK(err);

    update_toolbar(err);
    MX_ERROR_CHECK(err);

    update_all_sensitivities(err);
    MX_ERROR_CHECK(err);

abort:;
}

void mx_wp_editor::update_insert_sensitivities(int& err)
{
    bool b;

    if (current_edit_component == mx_wp_editor_body_e) {
        menu->set_visible(
            mx_insert_toc_e,
            mx_insert_index_e,
            mx_insert_index_entry_e,
            mx_insert_file_e,
            mx_end_item_e);
        toolbar->set_visible(
            mx_insert_toc_e,
            mx_insert_index_e,
            mx_insert_index_entry_e,
            mx_insert_file_e,
            mx_end_item_e);

        // can't insert a break at the start of the document
        if (cursor.get_position()->is_start_line()) {
            menu->set_invisible(mx_insert_break_e, mx_end_item_e);
            toolbar->set_invisible(mx_insert_break_e, mx_end_item_e);
        } else {
            menu->set_visible(mx_insert_break_e, mx_end_item_e);
            toolbar->set_visible(mx_insert_break_e, mx_end_item_e);
        }
    } else {
        menu->set_invisible(
            mx_insert_toc_e,
            mx_insert_index_e,
            mx_insert_index_entry_e,
            mx_insert_break_e,
            mx_insert_file_e,
            mx_end_item_e);
        toolbar->set_invisible(
            mx_insert_toc_e,
            mx_insert_index_e,
            mx_insert_index_entry_e,
            mx_insert_break_e,
            mx_insert_file_e,
            mx_end_item_e);
    }

    b = cursor.get_position()->is_table(err);
    MX_ERROR_CHECK(err);

    if (b) {
        menu->set_invisible(mx_insert_table_e, mx_insert_file_e,
            mx_end_item_e);
        toolbar->set_invisible(mx_insert_table_e, mx_insert_file_e,
            mx_end_item_e);
    } else {
        menu->set_visible(mx_insert_table_e, mx_insert_file_e,
            mx_end_item_e);
        toolbar->set_visible(mx_insert_table_e, mx_insert_file_e,
            mx_end_item_e);
    }
abort:;
}

void mx_wp_editor::insert_bullets(int& err)
{
    cursor.insert_bullets(err);
    MX_ERROR_CHECK(err);

    update_toolbar(err);
    MX_ERROR_CHECK(err);

abort:;
}

void mx_wp_editor::import_ascii(int& err, char* file_name)
{
}

void mx_wp_editor::insert_toc(int& err)
{
    bool b;

    mx_wp_doc_pos start, end, old_pos;
    mx_ur_item* ur_item;
    int dialogue_result = no_e;
    mx_list toc_paras;
    std::vector<int> toc_pages;

    set_edit_component(err, mx_wp_editor_body_e);
    MX_ERROR_CHECK(err);

    old_pos = *(cursor.get_position());

    b = find_existing_toc(err, start, end);
    MX_ERROR_CHECK(err);

    ur_buffer->mark_cant_undo(err);
    MX_ERROR_CHECK(err);

    if (b) {
        cursor.set_selection(err, start, end);
        MX_ERROR_CHECK(err);

        dialogue_result = dialog_man.run_yes_no_d("replaceExistingTOC",
            true, yes_e);

        if (dialogue_result == yes_e) {
            start.save(err, document);
            MX_ERROR_CHECK(err);

            cursor.cut(err);
            MX_ERROR_CHECK(err);
        }
    }

    if (dialogue_result == cancel_e) {
        return;
    }

    if (dialogue_result == no_e) {
        start = old_pos;

        start.moveto_start_para(err, document);
        MX_ERROR_CHECK(err);

        start.save(err, document);
        MX_ERROR_CHECK(err);
    }

    b = get_toc_para_list(err, toc_paras, toc_pages);
    MX_ERROR_CHECK(err);

    if (b) {
        mx_wp_object_buffer buff(NULL);

        insert_toc_para_list(err, toc_paras, toc_pages, start, end);
        MX_ERROR_CHECK(err);

        b = get_toc_para_list(err, toc_paras, toc_pages);
        MX_ERROR_CHECK(err);

        MX_ERROR_ASSERT(err, b);

        buff.remove(err, *document, start, end);
        MX_ERROR_CHECK(err);

        start = end;

        insert_toc_para_list(err, toc_paras, toc_pages, start, end);
        MX_ERROR_CHECK(err);

        set_sheet_layout(err, document);
        MX_ERROR_CHECK(err);

        cursor.set_selection(err, start, end);
        MX_ERROR_CHECK(err);

        ur_item = new mx_ur_item(mx_undo_paste_e, document, start, end, false);

        ur_buffer->insert(err, ur_item);
        MX_ERROR_CHECK(err);

        cursor.update_caret(err);
        MX_ERROR_CHECK(err);

        update_all_sensitivities(err);
        MX_ERROR_CHECK(err);

        frame->refresh(err);
        MX_ERROR_CHECK(err);
    } else {
        dialog_man.run_inform_d(inform_e, "noHeadingParas");
    }
abort:;
}

bool mx_wp_editor::find_existing_toc(
    int& err,
    mx_wp_doc_pos& start,
    mx_wp_doc_pos& end)
{
    mx_wp_doc_pos doc_start, doc_end;

    doc_start.moveto_start(err, document);
    MX_ERROR_CHECK(err);

    doc_end.moveto_end(err, document);
    MX_ERROR_CHECK(err);

    {
        mx_area_para_iterator iter(document, doc_start, doc_end);

        mx_rtti* o;

        do {
            o = iter.data(err);
            MX_ERROR_CHECK(err);

            if (o->rtti_class() == mx_paragraph_class_e) {
                if (((mx_paragraph*)o)->get_base_paragraph_style()->is_toc) {
                    start = iter.position;

                    // find first thing that isn't a toc para
                    do {
                        o = iter.data(err);
                        MX_ERROR_CHECK(err);

                        if (o->rtti_class() != mx_paragraph_class_e) {
                            break;
                        }

                        if (!(((mx_paragraph*)o)->get_base_paragraph_style()->is_toc)) {
                            break;
                        }
                    } while (iter.more(err));
                    MX_ERROR_CHECK(err);

                    end = iter.position;

                    start.moveto_start_para(err, document);
                    MX_ERROR_CHECK(err);

                    end.moveto_end_para(err, document);
                    MX_ERROR_CHECK(err);

                    return true;
                }
            }
        } while (iter.more(err));
        MX_ERROR_CHECK(err);

        return false;
    }

abort:
    return false;
}

bool mx_wp_editor::get_toc_para_list(int& err, mx_list& toc_paras, std::vector<int>& toc_pages)
{
    mx_wp_doc_pos doc_start, doc_end;

    doc_start.moveto_start(err, document);
    MX_ERROR_CHECK(err);

    doc_end.moveto_end(err, document);
    MX_ERROR_CHECK(err);

    toc_paras = mx_list();
    toc_pages.clear();

    {
        mx_area_para_iterator iter(document, doc_start, doc_end);

        mx_rtti* o;
        mx_paragraph* p;

        mx_paragraph_style* toc_style;

        do {
            o = iter.data(err);
            MX_ERROR_CHECK(err);

            if (o->rtti_class() == mx_paragraph_class_e) {
                p = (mx_paragraph*)o;

                if (p->get_base_paragraph_style()->is_heading) {
                    bool b = p->is_empty(err);
                    MX_ERROR_CHECK(err);

                    if (!b) {
                        toc_style = get_matching_toc_style(err, p);
                        MX_ERROR_CHECK(err);

                        if (toc_style != NULL) {
                            toc_paras.append(p);
                            toc_pages.push_back(iter.sheet_num() + 1);
                        }
                    }
                }
            }
        } while (iter.more(err));
        MX_ERROR_CHECK(err);

        return toc_paras.get_num_items() > 0;
    }

abort:
    return false;
}

void mx_wp_editor::insert_toc_para_list(
    int& err,
    mx_list& toc_paras,
    std::vector<int>& toc_pages,
    mx_wp_doc_pos& start,
    mx_wp_doc_pos& end)
{
    mx_list_iterator iter(toc_paras);
    std::vector<int>::iterator iter2 = toc_pages.begin();
    char s[100];
    int64_t i, sn;

    mx_paragraph *new_para, *para;
    mx_wp_doc_pos* pos = &start;

    start.save(err, document);
    MX_ERROR_CHECK(err);

    mx_paragraph_style* toc_style;

    while (iter.more()) {
        if (iter2 != toc_pages.end()) {
            iter2++;
        }

        para = (mx_paragraph*)iter.data();
        sn = *iter2;

        toc_style = get_matching_toc_style(err, para);
        MX_ERROR_CHECK(err);

        if (toc_style != NULL) {
            new_para = new mx_paragraph(*para);

            new_para->set_paragraph_style(toc_style);
            new_para->modify_paragraph_style(*toc_style);

            document->insert_next_para(err, *pos, new_para);
            MX_ERROR_CHECK(err);

            pos->moveto_end_para(err, document);
            MX_ERROR_CHECK(err);

            sprintf(s, "\t%ld", sn);

            i = 0;
            while (s[i] != 0) {
                document->insert_char(err, s[i], *pos);
                MX_ERROR_CHECK(err);

                pos->moveto_end_para(err, document);
                MX_ERROR_CHECK(err);

                i++;
            }

            pos->next_para(err, document);
            MX_ERROR_CHECK(err);
        }
    }

    end = *pos;
    end.save(err, document);
    MX_ERROR_CHECK(err);

    start.restore(err, document);
    MX_ERROR_CHECK(err);

    document->reformat(err);
    MX_ERROR_CHECK(err);

    document->repaginate(err, 0);
    MX_ERROR_CHECK(err);

    start.restore(err, document);
    MX_ERROR_CHECK(err);

    end.restore(err, document);
    MX_ERROR_CHECK(err);
abort:;
}

mx_paragraph_style* mx_wp_editor::get_matching_toc_style(int& err, mx_paragraph* p)
{
    int n, i;

    mx_paragraph_style *s, *bs;

    n = document->get_num_styles(err);
    MX_ERROR_CHECK(err);

    bs = p->get_base_paragraph_style();

    for (i = 0; i < n; i++) {
        s = document->get_style(err, i);
        MX_ERROR_CHECK(err);

        if (s->is_toc && (s->heading_level == bs->heading_level)) {
            return s;
        }
    }

abort:
    return NULL;
}

void mx_wp_editor::import_word_6(int& err, char* file_name)
{
}

void mx_wp_editor::import_maxwell(int& err, char* file_name)
{
    mx_wp_doc_pos insert_start = (*cursor.get_position());
    mx_wp_doc_pos insert_end;

    mx_ur_item* ur_item;

    mx_wp_document* source_doc;

    source_doc = mx_db_client_open_wp_doc(err, file_name, false);
check_result:

    switch (err) {
    case MX_FILE_UNRECOVERABLE:
        dialog_man.run_inform_d(inform_e, "fileUnrecoverable");
        MX_ERROR_CLEAR(err);
        return;

    case MX_FILE_UNIMPORTABLE:
        dialog_man.run_inform_d(inform_e, "cantImportFileType");
        MX_ERROR_CLEAR(err);
        return;

    case MX_FILE_NEEDS_RECOVER:
        MX_ERROR_CLEAR(err);
        if (dialog_man.run_yes_no_d("damaged", false, yes_e) == yes_e) {
            source_doc = mx_db_client_open_wp_doc(err, file_name, true);
            goto check_result;
        } else {
            return;
        }

    case MX_ERROR_OK:
        mx_wp_document* temp_doc;

        temp_doc = mx_db_client_open_temporary_wp_doc(err);
        MX_ERROR_CHECK(err);

        {
            mx_wp_doc_pos start;
            mx_wp_doc_pos end;

            mx_wp_object_buffer buf(temp_doc);

            start.moveto_start(err, source_doc);
            MX_ERROR_CHECK(err);

            end.moveto_end(err, source_doc);
            MX_ERROR_CHECK(err);

            buf.cut(err, *source_doc, start, end);
            MX_ERROR_CHECK(err);

            bool b;
            buf.insert(err, *document, *(cursor.get_position()), b);
            MX_ERROR_CHECK(err);

            insert_end = (*cursor.get_position());

            mx_db_client_close_wp_doc(err, source_doc);
            MX_ERROR_CHECK(err);

            insert_start.save(err, document);
            MX_ERROR_CHECK(err);

            insert_end.save(err, document);
            MX_ERROR_CHECK(err);

            document->reformat(err);
            MX_ERROR_CHECK(err);

            insert_start.restore(err, document);
            MX_ERROR_CHECK(err);

            insert_end.restore(err, document);
            MX_ERROR_CHECK(err);

            cursor.set_selection(err, insert_start, insert_end);
            MX_ERROR_CHECK(err);

            ur_item = new mx_ur_item(mx_undo_paste_e, document, insert_start, insert_end, false);

            ur_buffer->insert(err, ur_item);
            MX_ERROR_CHECK(err);

            set_sheet_layout(err, document);
            MX_ERROR_CHECK(err);

            frame->resetSheetLayout(err);
            MX_ERROR_CHECK(err);

            cursor.sheet_size_changed(err);
            MX_ERROR_CHECK(err);

            update_toolbar(err);
            MX_ERROR_CHECK(err);

            update_all_sensitivities(err);
            MX_ERROR_CHECK(err);

            frame->refresh(err);
            MX_ERROR_CHECK(err);

            cursor.update_caret(err);
            MX_ERROR_CHECK(err);
        }

        mx_db_client_close_wp_doc(err, temp_doc);
        MX_ERROR_CHECK(err);

        return;

    default:
        MX_ERROR_CHECK(err);
    }
abort:;
}
