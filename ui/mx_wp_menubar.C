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
 * MODULE/CLASS : mx_wp_menubar
 *
 * AUTHOR : Tom Newton
 *
 * This file
 *
 * DESCRIPTION:
 *
 *
 *
 *
 */

#include <Xm/Xm.h>
#include <stdarg.h>

#include <mx.h>
#include <mx_menubar.h>
#include <mx_wp_editor.h>
#include <mx_wp_menubar.h>

mx_wp_menubar::mx_wp_menubar(mx_window* w) : mx_menubar("wpMenuBar", w)
{
    mx_menu* menu;

    menu = add_sub_menu("fileItem");
    menu->add_item("newItem", &mx_wp_menubar::file_cb, this);
    menu->add_item("openItem", &mx_wp_menubar::file_cb, this);
    menu->add_item("closeItem", &mx_wp_menubar::file_cb, this);
    menu->add_separator();
    menu->add_item("saveItem", &mx_wp_menubar::file_cb, this);
    menu->add_item("saveAsItem", &mx_wp_menubar::file_cb, this);
    menu->add_separator();
    menu->add_item("printItem", &mx_wp_menubar::file_cb, this);
    menu->add_item("printCopyItem", &mx_wp_menubar::file_cb, this);

    menu = add_sub_menu("editItem");
    menu->add_item("undoItem", &mx_wp_menubar::edit_cb, this);
    menu->add_item("redoItem", &mx_wp_menubar::edit_cb, this);
    menu->add_separator();
    menu->add_item("cutItem", &mx_wp_menubar::edit_cb, this);
    menu->add_item("copyItem", &mx_wp_menubar::edit_cb, this);
    menu->add_item("pasteItem", &mx_wp_menubar::edit_cb, this);
    menu->add_item("selectAllItem", &mx_wp_menubar::edit_cb, this);
    menu->add_separator();
    menu->add_item("findItem", &mx_wp_menubar::edit_cb, this);
    menu->add_item("findAgainItem", &mx_wp_menubar::edit_cb, this);
    menu->add_item("replaceItem", &mx_wp_menubar::edit_cb, this);
    menu->add_separator();
    menu->add_item("gotoItem", &mx_wp_menubar::edit_cb, this);
    menu->add_item("gotoStartItem", &mx_wp_menubar::edit_cb, this);
    menu->add_item("gotoEndItem", &mx_wp_menubar::edit_cb, this);

    menu = add_sub_menu("tableMenuItem");
    menu->add_item("deleteRowsItem", &mx_wp_menubar::table_cb, this);
    menu->add_item("deleteColumnsItem", &mx_wp_menubar::table_cb, this);
    menu->add_separator();
    menu->add_item("rowsItem", &mx_wp_menubar::table_cb, this);
    menu->add_item("columnsItem", &mx_wp_menubar::table_cb, this);

    menu = add_sub_menu("viewItem");
    menu->add_checkable_item("outlineItem", FALSE, &mx_wp_menubar::view_cb, this);
    menu->add_separator();
    menu->add_checkable_item("headerItem", FALSE, &mx_wp_menubar::view_cb, this);
    menu->add_checkable_item("footerItem", FALSE, &mx_wp_menubar::view_cb, this);
    menu->add_separator();
    menu->add_checkable_item("viewStylesItem", TRUE, &mx_wp_menubar::view_cb, this);
    menu->add_checkable_item("viewToolsItem", TRUE, &mx_wp_menubar::view_cb, this);
    menu->add_checkable_item("viewRulerItem", TRUE, &mx_wp_menubar::view_cb, this);
    menu->add_checkable_item("viewTableGuidesItem", TRUE, &mx_wp_menubar::view_cb, this);
    menu->add_separator();
    menu->add_item("zoomItem", &mx_wp_menubar::view_cb, this);

    menu = add_sub_menu("insertItem");
    menu->add_item("breakItem", &mx_wp_menubar::insert_cb, this);
    menu->add_item("pageNumberItem", &mx_wp_menubar::insert_cb, this);
    menu->add_separator();
    menu->add_item("dateTimeItem", &mx_wp_menubar::insert_cb, this);
    menu->add_item("symbolItem", &mx_wp_menubar::insert_cb, this);
    menu->add_item("bulletsItem", &mx_wp_menubar::insert_cb, this);
    menu->add_separator();
    menu->add_item("imageItem", &mx_wp_menubar::insert_cb, this);
    menu->add_item("diagramItem", &mx_wp_menubar::insert_cb, this);
    menu->add_item("tableItem", &mx_wp_menubar::insert_cb, this);
    menu->add_item("insertFileItem", &mx_wp_menubar::insert_cb, this);
    menu->add_separator();
    menu->add_item("insertTOCItem", &mx_wp_menubar::insert_cb, this);

    menu = add_sub_menu("formatItem");
    menu->add_item("paragraphItem", &mx_wp_menubar::format_cb, this);
    menu->add_item("characterItem", &mx_wp_menubar::format_cb, this);
    menu->add_item("borderItem", &mx_wp_menubar::format_cb, this);
    menu->add_item("tabsItem", &mx_wp_menubar::format_cb, this);
    menu->add_separator();
    menu->add_item("styleItem", &mx_wp_menubar::format_cb, this);
    menu->add_separator();
    menu->add_item("pageSetupItem", &mx_wp_menubar::format_cb, this);
    menu->add_separator();
    menu->add_item("tableFormatItem", &mx_wp_menubar::format_cb, this);
    menu->add_item("tableStyleFormatItem", &mx_wp_menubar::format_cb, this);
    menu->add_separator();
    menu->add_item("makeNormalItem", &mx_wp_menubar::format_cb, this);
    menu->add_item("makeBoldItem", &mx_wp_menubar::format_cb, this);
    menu->add_item("makeItalicItem", &mx_wp_menubar::format_cb, this);
    menu->add_item("makeUnderlineItem", &mx_wp_menubar::format_cb, this);

    menu = add_sub_menu("toolsItem");
    menu->add_item("spellingItem", &mx_wp_menubar::tools_cb, this);
    menu->add_item("optionsItem", &mx_wp_menubar::tools_cb, this);
    menu->add_item("envelopeItem", &mx_wp_menubar::tools_cb, this);
    menu->add_item("repaginateItem", &mx_wp_menubar::tools_cb, this);

    menu = add_sub_menu("helpItem", TRUE);
    menu->add_item("aboutItem", &mx_wp_menubar::help_cb, this);
}

void mx_wp_menubar::file_cb(Widget w, XtPointer cd, XtPointer call_data)
{
    mx_wp_menubar* bar = (mx_wp_menubar*)cd;
    mx_window* window = bar->get_window();
    mx_wp_editor* ed = (mx_wp_editor*)window->get_editor();
    char* name = XtName(w);

    int err = MX_ERROR_OK;

    window->set_busy();

    if (strcmp(name, "newItem") == 0)
        ed->file_new(err);
    else if (strcmp(name, "openItem") == 0)
        ed->file_open(err);
    else if (strcmp(name, "closeItem") == 0) {
        if (ed->file_close(err)) {
            // return immediately as the window has been destroyed -
            // so we can't set it not-busy
            return;
        }
    } else if (strcmp(name, "saveItem") == 0)
        ed->file_save(err);
    else if (strcmp(name, "saveAsItem") == 0)
        ed->file_save_as(err);
    else if (strcmp(name, "printItem") == 0)
        ed->file_print(err);
    else if (strcmp(name, "printCopyItem") == 0)
        ed->file_print_one_copy(err);
    else if (strcmp(name, "closeItem") != 0)
        ed->file_close(err);

    window->clear_busy();
    MX_ERROR_CHECK(err);
    return;

abort:
    global_error_trace->print();
}

void mx_wp_menubar::help_cb(Widget w, XtPointer cd, XtPointer call_data)
{
    mx_wp_menubar* bar = (mx_wp_menubar*)cd;
    mx_window* window = bar->get_window();
    mx_wp_editor* ed = (mx_wp_editor*)window->get_editor();

    int err = MX_ERROR_OK;

    window->set_busy();

    ed->help_support(err);
    window->clear_busy();

    MX_ERROR_CHECK(err);
    return;

abort:
    global_error_trace->print();
}

void mx_wp_menubar::view_cb(
    Widget w,
    XtPointer cd,
    XtPointer call_data)
{
    mx_wp_menubar* bar = (mx_wp_menubar*)cd;
    mx_window* window = bar->get_window();
    char* name = XtName(w);
    mx_wp_editor* ed = (mx_wp_editor*)window->get_editor();

    int err = MX_ERROR_OK;

    window->set_busy();

    if (strcmp(name, "viewToolsItem") == 0)
        ed->view_actions(err, !bar->is_checked(w));
    else if (strcmp(name, "viewStylesItem") == 0)
        ed->view_styles(err, !bar->is_checked(w));
    else if (strcmp(name, "viewTableGuidesItem") == 0)
        ed->view_table_guides(err, !bar->is_checked(w));
    else if (strcmp(name, "viewRulerItem") == 0)
        ed->view_ruler(err, !bar->is_checked(w));
    else if (strcmp(name, "outlineItem") == 0)
        ed->view_outline(err, !bar->is_checked(w));
    else if (strcmp(name, "zoomItem") == 0)
        ed->view_zoom(err);
    else if ((strcmp(name, "headerItem") == 0) || (strcmp(name, "footerItem") == 0)) {
        bool b1, b2;
        mx_menu_item *i1, *i2;
        int err = MX_ERROR_OK;

        i1 = bar->get_item("viewItem", "headerItem");
        i2 = bar->get_item("viewItem", "footerItem");

        b1 = i1->is_checked();
        b2 = i2->is_checked();

        if (strcmp(name, "headerItem") == 0) {
            i2->set_checked(FALSE);

            if (b1) {
                ed->view_header(err);
            } else {
                // view normal
                ed->view_document(err);
            }
        } else {
            i1->set_checked(FALSE);

            if (b2) {
                ed->view_footer(err);
            } else {
                // view normal
                ed->view_document(err);
            }
        }
    }
    MX_ERROR_CHECK(err);

    window->clear_busy();
    return;

abort:
    window->clear_busy();
    global_error_trace->print();
}

void mx_wp_menubar::edit_cb(
    Widget w,
    XtPointer cd,
    XtPointer call_data)
{
    mx_wp_menubar* bar = (mx_wp_menubar*)cd;
    mx_window* window = bar->get_window();
    char* name = XtName(w);
    mx_wp_editor* ed = (mx_wp_editor*)window->get_editor();

    int err = MX_ERROR_OK;

    window->set_busy();

    if (strcmp(name, "undoItem") == 0)
        ed->edit_undo(err);
    else if (strcmp(name, "redoItem") == 0)
        ed->edit_redo(err);
    else if (strcmp(name, "copyItem") == 0)
        ed->edit_copy(err);
    else if (strcmp(name, "cutItem") == 0)
        ed->edit_cut(err);
    else if (strcmp(name, "pasteItem") == 0)
        ed->edit_paste(err);
    else if (strcmp(name, "selectAllItem") == 0)
        ed->edit_select_all(err);
    else if (strcmp(name, "findItem") == 0)
        ed->edit_find(err);
    else if (strcmp(name, "findAgainItem") == 0)
        ed->edit_find_again(err);
    else if (strcmp(name, "replaceItem") == 0)
        ed->edit_find_and_replace(err);
    else if (strcmp(name, "gotoItem") == 0)
        ed->edit_goto_page(err);
    else if (strcmp(name, "gotoStartItem") == 0)
        ed->edit_goto_start(err);
    else if (strcmp(name, "gotoEndItem") == 0)
        ed->edit_goto_end(err);

    window->clear_busy();
    MX_ERROR_CHECK(err);
    return;

abort:
    global_error_trace->print();
}

void mx_wp_menubar::insert_cb(
    Widget w,
    XtPointer cd,
    XtPointer call_data)
{
    mx_wp_menubar* bar = (mx_wp_menubar*)cd;
    mx_window* window = bar->get_window();
    mx_wp_editor* ed = (mx_wp_editor*)window->get_editor();
    char* name = XtName(w);

    int err = MX_ERROR_OK;

    window->set_busy();

    if (strcmp(name, "breakItem") == 0)
        ed->insert_page_break(err);
    else if (strcmp(name, "pageNumberItem") == 0)
        ed->insert_page_number(err);
    else if (strcmp(name, "dateTimeItem") == 0)
        ed->insert_date_time(err);
    else if (strcmp(name, "symbolItem") == 0)
        ed->insert_symbol(err);
    else if (strcmp(name, "bulletsItem") == 0)
        ed->insert_bullets(err);
    else if (strcmp(name, "imageItem") == 0)
        ed->insert_image(err);
    else if (strcmp(name, "diagramItem") == 0)
        ed->insert_diagram(err);
    else if (strcmp(name, "tableItem") == 0)
        ed->insert_table(err);
    else if (strcmp(name, "insertFileItem") == 0)
        ed->insert_file(err);
    else if (strcmp(name, "insertTOCItem") == 0)
        ed->insert_toc(err);
    else if (strcmp(name, "insertIndexEntryItem") == 0)
        ed->insert_index_entry(err);

    window->clear_busy();
    MX_ERROR_CHECK(err);
    return;

abort:
    global_error_trace->print();
}

void mx_wp_menubar::format_cb(
    Widget w,
    XtPointer cd,
    XtPointer call_data)
{
    mx_wp_menubar* bar = (mx_wp_menubar*)cd;
    mx_window* window = bar->get_window();
    mx_wp_editor* ed = (mx_wp_editor*)window->get_editor();
    char* name = XtName(w);

    int err = MX_ERROR_OK;

    window->set_busy();

    if (strcmp(name, "characterItem") == 0)
        ed->format_character(err);
    else if (strcmp(name, "paragraphItem") == 0)
        ed->format_paragraph(err);
    else if (strcmp(name, "borderItem") == 0)
        ed->format_border(err);
    else if (strcmp(name, "tabsItem") == 0)
        ed->format_tabs(err);
    else if (strcmp(name, "styleItem") == 0)
        ed->format_style(err);
    else if (strcmp(name, "pageSetupItem") == 0)
        ed->format_page_setup(err);
    else if (strcmp(name, "imageFormatItem") == 0)
        ed->format_image(err);
    else if (strcmp(name, "diagramFormatItem") == 0)
        ed->format_diagram(err);
    else if (strcmp(name, "tableFormatItem") == 0)
        ed->format_table(err);
    else if (strcmp(name, "tableStyleFormatItem") == 0)
        ed->format_table_style(err);
    else if (strcmp(name, "makeNormalItem") == 0) {
        ed->format_set_underline(err, FALSE);
        MX_ERROR_CHECK(err);
        ed->format_set_bold(err, FALSE);
        MX_ERROR_CHECK(err);
        ed->format_set_italic(err, FALSE);
        MX_ERROR_CHECK(err);
    } else if (strcmp(name, "makeBoldItem") == 0)
        ed->format_set_bold(err, TRUE);
    else if (strcmp(name, "makeItalicItem") == 0)
        ed->format_set_italic(err, TRUE);
    else if (strcmp(name, "makeUnderlineItem") == 0)
        ed->format_set_underline(err, TRUE);

    window->clear_busy();

    MX_ERROR_CHECK(err);
    return;

abort:
    global_error_trace->print();
}

void mx_wp_menubar::table_cb(
    Widget w,
    XtPointer cd,
    XtPointer call_data)
{
    mx_wp_menubar* bar = (mx_wp_menubar*)cd;
    mx_window* window = bar->get_window();
    mx_wp_editor* ed = (mx_wp_editor*)window->get_editor();
    char* name = XtName(w);

    int err = MX_ERROR_OK;

    window->set_busy();

    if (strcmp("deleteRowsItem", name) == 0)
        ed->table_cut_rows(err);
    else if (strcmp("deleteColumnsItem", name) == 0)
        ed->table_cut_columns(err);
    else if (strcmp("rowsItem", name) == 0)
        ed->table_insert_rows(err);
    else if (strcmp("columnsItem", name) == 0)
        ed->table_insert_columns(err);

    window->clear_busy();
    MX_ERROR_CHECK(err);
    return;

abort:
    global_error_trace->print();
}

void mx_wp_menubar::tools_cb(
    Widget w,
    XtPointer cd,
    XtPointer call_data)
{
    mx_wp_menubar* bar = (mx_wp_menubar*)cd;
    mx_window* window = bar->get_window();
    mx_wp_editor* ed = (mx_wp_editor*)window->get_editor();
    char* name = XtName(w);

    int err = MX_ERROR_OK;

    window->set_busy();

    if (strcmp(name, "spellingItem") == 0)
        ed->tools_spelling(err);
    else if (strcmp(name, "optionsItem") == 0)
        ed->tools_options(err);
    else if (strcmp(name, "envelopeItem") == 0)
        ed->tools_envelope(err);
    else if (strcmp(name, "repaginateItem") == 0)
        ed->tools_repaginate(err);

    window->clear_busy();
    MX_ERROR_CHECK(err);
    return;

abort:
    global_error_trace->print();
}

void mx_wp_menubar::set_item_visible(menu_item_name_t item, bool s)
{
    const char* menu_name;
    const char* item_name;

    switch (item) {
    case mx_file_new_e:
        menu_name = "fileItem";
        item_name = "newItem";
        break;
    case mx_file_open_e:
        menu_name = "fileItem";
        item_name = "openItem";
        break;
    case mx_file_close_e:
        menu_name = "fileItem";
        item_name = "closeItem";
        break;
    case mx_file_save_e:
        menu_name = "fileItem";
        item_name = "saveItem";
        break;
    case mx_file_save_as_e:
        menu_name = "fileItem";
        item_name = "saveAsItem";
        break;
    case mx_file_print_e:
        menu_name = "fileItem";
        item_name = "printItem";
        break;
    case mx_file_print_one_e:
        menu_name = "fileItem";
        item_name = "printCopyItem";
        break;

    case mx_edit_undo_e:
        menu_name = "editItem";
        item_name = "undoItem";
        break;
    case mx_edit_redo_e:
        menu_name = "editItem";
        item_name = "redoItem";
        break;
    case mx_edit_cut_e:
        menu_name = "editItem";
        item_name = "cutItem";
        break;
    case mx_edit_copy_e:
        menu_name = "editItem";
        item_name = "copyItem";
        break;
    case mx_edit_paste_e:
        menu_name = "editItem";
        item_name = "pasteItem";
        break;
    case mx_edit_select_all_e:
        menu_name = "editItem";
        item_name = "selectAllItem";
        break;
    case mx_edit_find_e:
        menu_name = "editItem";
        item_name = "findItem";
        break;
    case mx_edit_find_again_e:
        menu_name = "editItem";
        item_name = "findAgainItem";
        break;
    case mx_edit_replace_e:
        menu_name = "editItem";
        item_name = "replaceItem";
        break;
    case mx_edit_goto_e:
        menu_name = "editItem";
        item_name = "gotoItem";
        break;
    case mx_edit_goto_start_e:
        menu_name = "editItem";
        item_name = "gotoStartItem";
        break;
    case mx_edit_goto_end_e:
        menu_name = "editItem";
        item_name = "gotoEndItem";
        break;

    case mx_table_delete_rows_e:
        menu_name = "tableMenuItem";
        item_name = "deleteRowsItem";
        break;
    case mx_table_delete_columns_e:
        menu_name = "tableMenuItem";
        item_name = "deleteColumnsItem";
        break;
    case mx_table_rows_e:
        menu_name = "tableMenuItem";
        item_name = "rowsItem";
        break;
    case mx_table_columns_e:
        menu_name = "tableMenuItem";
        item_name = "columnsItem";
        break;

    case mx_view_outline_e:
        menu_name = "viewItem";
        item_name = "outlineItem";
        break;
    case mx_view_header_e:
        menu_name = "viewItem";
        item_name = "headerItem";
        break;
    case mx_view_footer_e:
        menu_name = "viewItem";
        item_name = "footerItem";
        break;
    case mx_view_styles_e:
        menu_name = "viewItem";
        item_name = "viewStylesItem";
        break;
    case mx_view_table_guides_e:
        menu_name = "viewItem";
        item_name = "viewTableGuidesItem";
        break;
    case mx_view_tools_e:
        menu_name = "viewItem";
        item_name = "viewToolsItem";
        break;
    case mx_view_ruler_e:
        menu_name = "viewItem";
        item_name = "viewRulerItem";
        break;
    case mx_view_zoom_e:
        menu_name = "viewItem";
        item_name = "zoomItem";
        break;

    case mx_insert_break_e:
        menu_name = "insertItem";
        item_name = "breakItem";
        break;
    case mx_insert_page_number_e:
        menu_name = "insertItem";
        item_name = "pageNumberItem";
        break;
    case mx_insert_date_time_e:
        menu_name = "insertItem";
        item_name = "dateTimeItem";
        break;
    case mx_insert_symbol_e:
        menu_name = "insertItem";
        item_name = "symbolItem";
        break;
    case mx_insert_bullets_e:
        menu_name = "insertItem";
        item_name = "bulletsItem";
        break;
    case mx_insert_image_e:
        menu_name = "insertItem";
        item_name = "imageItem";
        break;
    case mx_insert_diagram_e:
        menu_name = "insertItem";
        item_name = "diagramItem";
        break;
    case mx_insert_table_e:
        menu_name = "insertItem";
        item_name = "tableItem";
        break;
    case mx_insert_file_e:
        menu_name = "insertItem";
        item_name = "insertFileItem";
        break;
    case mx_insert_toc_e:
        menu_name = "insertItem";
        item_name = "insertTOCItem";
        break;
    case mx_insert_index_e:
        menu_name = "insertItem";
        break;
    case mx_insert_index_entry_e:
        menu_name = "insertItem";
        item_name = "insertIndexEntryItem";
        break;

    case mx_format_paragraph_e:
        menu_name = "formatItem";
        item_name = "paragraphItem";
        break;
    case mx_format_character_e:
        menu_name = "formatItem";
        item_name = "characterItem";
        break;
    case mx_format_border_e:
        menu_name = "formatItem";
        item_name = "borderItem";
        break;
    case mx_format_tabs_e:
        menu_name = "formatItem";
        item_name = "tabsItem";
        break;
    case mx_format_style_e:
        menu_name = "formatItem";
        item_name = "styleItem";
        break;
    case mx_format_page_setup_e:
        menu_name = "formatItem";
        item_name = "pageSetupItem";
        break;
    case mx_format_table_e:
        menu_name = "formatItem";
        item_name = "tableFormatItem";
        break;
    case mx_format_table_style_e:
        menu_name = "formatItem";
        item_name = "tableStyleFormatItem";
        break;
    case mx_format_make_normal_e:
        menu_name = "formatItem";
        item_name = "makeNormalItem";
        break;
    case mx_format_make_bold_e:
        menu_name = "formatItem";
        item_name = "makeBoldItem";
        break;
    case mx_format_make_italic_e:
        menu_name = "formatItem";
        item_name = "makeItalicItem";
        break;
    case mx_format_make_underline_e:
        menu_name = "formatItem";
        item_name = "makeUnderlineItem";
        break;

    case mx_tools_spelling_e:
        menu_name = "toolsItem";
        item_name = "spellingItem";
        break;
    case mx_tools_options_e:
        menu_name = "toolsItem";
        item_name = "optionsItem";
        break;
    case mx_tools_envelope_e:
        menu_name = "toolsItem";
        item_name = "envelopeItem";
        break;
    case mx_tools_repaginate_e:
        menu_name = "toolsItem";
        item_name = "repaginateItem";
        break;

    default:
        return;
    }

    mx_menubar::set_item_visible(menu_name, item_name, s);
}

void mx_wp_menubar::set_visible(menu_item_name_t item, ...)
{
    va_list ap;
    menu_item_name_t i;

    set_item_visible(item, TRUE);

    va_start(ap, item);

    while (TRUE) {
        int x = va_arg(ap, int);
        i = (menu_item_name_t)x;
        if (i == mx_end_item_e) {
            va_end(ap);
            return;
        }

        set_item_visible(i, TRUE);
    }
}

void mx_wp_menubar::set_invisible(menu_item_name_t item, ...)
{
    va_list ap;
    menu_item_name_t i;

    set_item_visible(item, FALSE);

    va_start(ap, item);

    while (TRUE) {
        int x = va_arg(ap, int);
        i = (menu_item_name_t)x;
        if (i == mx_end_item_e) {
            va_end(ap);
            return;
        }

        set_item_visible(i, FALSE);
    }
}

void mx_wp_menubar::set_item_checked(int& err, menu_item_name_t item, bool s)
{
    mx_menu_item* i;

    switch (item) {
    case mx_view_header_e:
        i = get_item("viewItem", "headerItem");
        break;
    case mx_view_footer_e:
        i = get_item("viewItem", "footerItem");
        break;
    case mx_view_styles_e:
        i = get_item("viewItem", "viewStylesItem");
        break;
    case mx_view_table_guides_e:
        i = get_item("viewItem", "viewTableGuidesItem");
        break;
    case mx_view_tools_e:
        i = get_item("viewItem", "viewToolsItem");
        break;
    case mx_view_ruler_e:
        i = get_item("viewItem", "viewRulerItem");
        break;
    default:
        err = MX_WP_MENUBAR_NO_SUCH_MENU_ITEM;
        return;
    }
    i->set_checked(s);
}
