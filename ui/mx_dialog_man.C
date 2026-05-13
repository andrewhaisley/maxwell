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
 * MODULE/CLASS : mx_dialog_manager
 *
 * AUTHOR : Andrew Haisley
 *
 * This file
 *
 * DESCRIPTION:
 *
 *
 *  A class for holding onto lists of dialogs and re-using them
 *  as necessary
 *
 *
 *
 */


#include <Xm/DialogS.h>
#include <Xm/Form.h>
#include <Xm/Label.h>
#include <Xm/LabelG.h>
#include <Xm/MwmUtil.h>
#include <Xm/PanedW.h>
#include <Xm/PushB.h>
#include <Xm/PushBG.h>
#include <Xm/Separator.h>
#include <Xm/VendorS.h>
#include <Xm/Xm.h>

#include <maxwell.h>
#include <mx.h>
#include <mx_font.h>
#include <mx_dialog_man.h>

#include <mx_about_d.h>
#include <mx_border_d.h>
#include <mx_cell_d.h>
#include <mx_char_d.h>
#include <mx_column_d.h>
#include <mx_date_d.h>
#include <mx_dialog.h>
#include <mx_env_d.h>
#include <mx_goto_d.h>
#include <mx_hsearch_d.h>
#include <mx_index_d.h>
#include <mx_inform_d.h>
#include <mx_ins_c_d.h>
#include <mx_ins_r_d.h>
#include <mx_new_d.h>
#include <mx_open_d.h>
#include <mx_opt_d.h>
#include <mx_page_d.h>
#include <mx_para_d.h>
#include <mx_print_d.h>
#include <mx_prog_d.h>
#include <mx_quit_d.h>
#include <mx_replace_d.h>
#include <mx_save_d.h>
#include <mx_search_d.h>
#include <mx_spell_d.h>
#include <mx_style_d.h>
#include <mx_support_d.h>
#include <mx_symbol_d.h>
#include <mx_tab_d.h>
#include <mx_tabf_d.h>
#include <mx_table_d.h>
#include <mx_toc_d.h>
#include <mx_yes_no_d.h>
#include <mx_zoom_d.h>

mx_dialog_result_t mx_dialog_manager::mess_result = no_e;

mx_dialog_manager::mx_dialog_manager()
    : mx_ui_object()
{
    open_d = NULL;
    save_d = NULL;
    quit_d = NULL;
    about_d = NULL;
    new_d = NULL;
    print_d = NULL;
    search_d = NULL;
    replace_d = NULL;
    goto_d = NULL;
    zoom_d = NULL;
    symbol_d = NULL;
    table_d = NULL;
    toc_d = NULL;
    index_d = NULL;
    char_d = NULL;
    para_d = NULL;
    tabf_d = NULL;
    border_d = NULL;
    tab_d = NULL;
    style_d = NULL;
    date_d = NULL;
    page_d = NULL;
    column_d = NULL;
    ins_c_d = NULL;
    ins_r_d = NULL;
    cell_d = NULL;
    spell_d = NULL;
    env_d = NULL;
    opt_d = NULL;
    support_d = NULL;
    prog_d = NULL;
    prog_bar_d = NULL;
}

mx_dialog_manager::~mx_dialog_manager()
{
    if (open_d != NULL)
        delete open_d;
    if (save_d != NULL)
        delete save_d;
    if (quit_d != NULL)
        delete quit_d;
    if (about_d != NULL)
        delete about_d;
    if (print_d != NULL)
        delete print_d;
    if (new_d != NULL)
        delete new_d;
    if (search_d != NULL)
        delete search_d;
    if (replace_d != NULL)
        delete replace_d;
    if (goto_d != NULL)
        delete goto_d;
    if (zoom_d != NULL)
        delete zoom_d;
    if (symbol_d != NULL)
        delete symbol_d;
    if (table_d != NULL)
        delete table_d;
    if (toc_d != NULL)
        delete toc_d;
    if (index_d != NULL)
        delete index_d;
    if (char_d != NULL)
        delete char_d;
    if (para_d != NULL)
        delete para_d;
    if (tabf_d != NULL)
        delete tabf_d;
    if (border_d != NULL)
        delete border_d;
    if (tab_d != NULL)
        delete tab_d;
    if (style_d != NULL)
        delete style_d;
    if (date_d != NULL)
        delete date_d;
    if (page_d != NULL)
        delete page_d;
    if (column_d != NULL)
        delete column_d;
    if (ins_c_d != NULL)
        delete ins_c_d;
    if (ins_r_d != NULL)
        delete ins_r_d;
    if (cell_d != NULL)
        delete cell_d;
    if (spell_d != NULL)
        delete spell_d;
    if (env_d != NULL)
        delete env_d;
    if (opt_d != NULL)
        delete opt_d;
    if (support_d != NULL)
        delete support_d;
    if (prog_d != NULL)
        delete prog_d;
    if (prog_bar_d != NULL)
        delete prog_bar_d;

    {
        for (auto i : m_yes_no_d_hash)
            delete i.second;

        for (auto i : m_inform_d_hash)
            delete i.second;

    }
}

mx_quit_d* mx_dialog_manager::get_quit_d()
{
    if (quit_d == NULL) {
        quit_d = new mx_quit_d(parent);
    }
    return quit_d;
}

mx_about_d* mx_dialog_manager::get_about_d()
{
    if (about_d == NULL) {
        about_d = new mx_about_d(parent);
    }
    return about_d;
}

mx_inform_d* mx_dialog_manager::get_inform_d(mx_inform_d_t type, const char* name)
{
    if (m_inform_d_hash.find(name) == m_inform_d_hash.end()) {
        m_inform_d_hash[name] = new mx_inform_d(name, parent, type);
    }

    return m_inform_d_hash[name];
}

mx_yes_no_d* mx_dialog_manager::get_yes_no_d(
    const char* name,
    bool cancel_button,
    mx_dialog_result_t default_button)
{
    if (m_yes_no_d_hash.find(name) == m_yes_no_d_hash.end()) {
        m_yes_no_d_hash[name] = new mx_yes_no_d(name, parent, cancel_button, default_button);
    } 

    return m_yes_no_d_hash[name];
}

mx_open_d* mx_dialog_manager::get_open_d()
{
    if (open_d == NULL) {
        open_d = new mx_open_d("open", parent, mx_document_file_e, mx_document_file_class_e, TRUE);
    }
    return open_d;
}

mx_save_d* mx_dialog_manager::get_save_d(const char* template_dir)
{
    if (save_d == NULL) {
        save_d = new mx_save_d("save", parent, mx_document_file_e, mx_save_file_class_e, template_dir);
    }
    return save_d;
}

mx_new_d* mx_dialog_manager::get_new_d(const char* template_dir)
{
    if (new_d == NULL) {
        new_d = new mx_new_d(parent, template_dir);
    }
    return new_d;
}

mx_print_d* mx_dialog_manager::get_print_d()
{
    if (print_d == NULL) {
        print_d = new mx_print_d(parent);
    }
    return print_d;
}

void mx_dialog_manager::run_inform_d(mx_inform_d_t type, const char* name)
{
    mx_inform_d* d;

    d = get_inform_d(type, name);
    d->centre();
    d->activate();
    d->run_modal();
    d->deactivate();
}

mx_dialog_result_t mx_dialog_manager::run_yes_no_d(
    const char* name,
    bool cancel_button,
    mx_dialog_result_t default_button)
{
    int res;
    mx_yes_no_d* d;

    d = get_yes_no_d(name, cancel_button, default_button);
    d->centre();
    d->activate();
    res = d->run_modal();
    d->deactivate();
    return (mx_dialog_result_t)res;
}

mx_search_d* mx_dialog_manager::get_search_d()
{
    if (search_d == NULL) {
        search_d = new mx_search_d(parent);
    }
    return search_d;
}

mx_replace_d* mx_dialog_manager::get_replace_d()
{
    if (replace_d == NULL) {
        replace_d = new mx_replace_d(parent);
    }
    return replace_d;
}

mx_goto_d* mx_dialog_manager::get_goto_d()
{
    if (goto_d == NULL) {
        goto_d = new mx_goto_d(parent);
    }
    return goto_d;
}

mx_zoom_d* mx_dialog_manager::get_zoom_d()
{
    if (zoom_d == NULL) {
        zoom_d = new mx_zoom_d(parent);
    }
    return zoom_d;
}

mx_symbol_d* mx_dialog_manager::get_symbol_d(mx_font* f)
{
    if (symbol_d == NULL) {
        symbol_d = new mx_symbol_d(parent, f);
    }
    return symbol_d;
}

mx_table_d* mx_dialog_manager::get_table_d(float width_mm, mx_unit_e display_units)
{
    if (table_d == NULL) {
        table_d = new mx_table_d(parent, width_mm, display_units);
    }
    return table_d;
}

mx_toc_d* mx_dialog_manager::get_toc_d()
{
    if (toc_d == NULL) {
        toc_d = new mx_toc_d(parent);
    }
    return toc_d;
}

mx_index_d* mx_dialog_manager::get_index_d()
{
    if (index_d == NULL) {
        index_d = new mx_index_d(parent);
    }
    return index_d;
}

mx_char_d* mx_dialog_manager::get_char_d()
{
    if (char_d == NULL) {
        char_d = new mx_char_d(parent);
    }
    return char_d;
}

mx_para_d* mx_dialog_manager::get_para_d()
{
    if (para_d == NULL) {
        para_d = new mx_para_d(parent);
    }
    return para_d;
}

mx_tabf_d* mx_dialog_manager::get_tabf_d()
{
    if (tabf_d == NULL) {
        tabf_d = new mx_tabf_d(parent);
    }
    return tabf_d;
}

mx_border_d* mx_dialog_manager::get_border_d()
{
    if (border_d == NULL) {
        border_d = new mx_border_d(parent);
    }
    return border_d;
}

mx_tab_d* mx_dialog_manager::get_tab_d()
{
    if (tab_d == NULL) {
        tab_d = new mx_tab_d(parent);
    }
    return tab_d;
}

mx_style_d* mx_dialog_manager::get_style_d()
{
    if (style_d == NULL) {
        style_d = new mx_style_d(parent);
    }
    return style_d;
}

mx_date_d* mx_dialog_manager::get_date_d()
{
    if (date_d == NULL) {
        date_d = new mx_date_d(parent);
    }
    return date_d;
}

mx_page_d* mx_dialog_manager::get_page_d()
{
    if (page_d == NULL) {
        page_d = new mx_page_d(parent);
    }
    return page_d;
}

mx_column_d* mx_dialog_manager::get_column_d()
{
    if (column_d == NULL) {
        column_d = new mx_column_d(parent);
    }
    return column_d;
}

mx_ins_c_d* mx_dialog_manager::get_ins_c_d()
{
    if (ins_c_d == NULL) {
        ins_c_d = new mx_ins_c_d(parent);
    }
    return ins_c_d;
}

mx_ins_r_d* mx_dialog_manager::get_ins_r_d()
{
    if (ins_r_d == NULL) {
        ins_r_d = new mx_ins_r_d(parent);
    }
    return ins_r_d;
}

mx_cell_d* mx_dialog_manager::get_cell_d()
{
    if (cell_d == NULL) {
        cell_d = new mx_cell_d(parent);
    }
    return cell_d;
}

mx_spell_d* mx_dialog_manager::get_spell_d()
{
    if (spell_d == NULL) {
        spell_d = new mx_spell_d(parent);
    }
    return spell_d;
}

mx_env_d* mx_dialog_manager::get_env_d()
{
    if (env_d == NULL) {
        env_d = new mx_env_d(parent);
    }
    return env_d;
}

mx_opt_d* mx_dialog_manager::get_opt_d()
{
    if (opt_d == NULL) {
        opt_d = new mx_opt_d(parent);
    }
    return opt_d;
}

mx_support_d* mx_dialog_manager::get_support_d()
{
    if (support_d == NULL) {
        support_d = new mx_support_d(parent);
    }
    return support_d;
}

// progress dialog with no bar
mx_prog_d* mx_dialog_manager::get_prog_d()
{
    if (prog_d == NULL) {
        prog_d = new mx_prog_d(parent, FALSE);
    }
    return prog_d;
}

// progress dialog with bar
mx_prog_d* mx_dialog_manager::get_prog_bar_d()
{
    if (prog_bar_d == NULL) {
        prog_bar_d = new mx_prog_d(parent, TRUE);
    }
    return prog_bar_d;
}

// reset the progress bar dialogs
void mx_dialog_manager::reset_prog_d()
{
    if (prog_d != NULL) {
        prog_d->deactivate();
        delete prog_d;
        prog_d = NULL;
    }

    if (prog_bar_d != NULL) {
        prog_bar_d->deactivate();
        delete prog_bar_d;
        prog_bar_d = NULL;
    }
}

void mx_dialog_manager::button_cb(
    Widget widget,
    XtPointer client_data,
    XtPointer call_data)
{
    // mess_result = (mx_dialog_result_t)client_data;
    mess_result = (mx_dialog_result_t)((int64_t)client_data);
}

void mx_dialog_manager::centre_widget(Widget w)
{
    Position parent_x, parent_y;
    Position dialog_x, dialog_y;
    Dimension parent_width, parent_height;
    Dimension dialog_width, dialog_height;

    XtVaGetValues(
        parent,
        XmNx, &parent_x,
        XmNy, &parent_y,
        XmNwidth, &parent_width,
        XmNheight, &parent_height,
        NULL);

    XtVaGetValues(
        w,
        XmNwidth, &dialog_width,
        XmNheight, &dialog_height,
        NULL);

    dialog_x = parent_x + (parent_width / 2) - (dialog_width / 2);
    dialog_y = parent_y + (parent_height / 2) - (dialog_height / 2);

    XtVaSetValues(
        w,
        XmNx, dialog_x,
        XmNy, dialog_y,
        NULL);

    XRaiseWindow(XtDisplay(w), XtWindow(w));
}

void mx_dialog_manager::run_inform_now(mx_inform_d_t type, const char* s)
{
    Widget popup, text, label;
    Widget sep, pane, action_area, control_area, ok_button;
    Pixmap pixmap;
    Pixel fg, bg;

    XmString str;

    XEvent next_event;

    popup = XtVaCreatePopupShell(
        "messInform",
        xmDialogShellWidgetClass,
        parent,
        XmNdeleteResponse, XmUNMAP,
        XmNdeleteResponse, XmDO_NOTHING,
        XmNmappedWhenManaged, False,
        XmNmwmDecorations,
        MWM_DECOR_BORDER | MWM_DECOR_TITLE | MWM_DECOR_MENU,
        XmNmwmFunctions,
        MWM_FUNC_MOVE | MWM_FUNC_CLOSE,
        NULL);

    pane = XtVaCreateWidget("pane",
        xmFormWidgetClass,
        popup,
        NULL);

    control_area = XtVaCreateWidget(
        "control",
        xmFormWidgetClass,
        pane,
        XmNleftAttachment, XmATTACH_FORM,
        XmNtopAttachment, XmATTACH_FORM,
        XmNrightAttachment, XmATTACH_FORM,
        NULL);

    sep = XtVaCreateManagedWidget(
        "separator",
        xmSeparatorWidgetClass,
        pane,
        XmNseparatorType, XmSHADOW_ETCHED_IN,
        XmNleftAttachment, XmATTACH_FORM,
        XmNrightAttachment, XmATTACH_FORM,
        XmNtopAttachment, XmATTACH_WIDGET,
        XmNtopOffset, 5,
        XmNtopWidget, control_area,
        NULL);

    action_area = XtVaCreateWidget(
        "action",
        xmFormWidgetClass,
        pane,
        XmNtopOffset, 5,
        XmNbottomOffset, 8,
        XmNleftAttachment, XmATTACH_FORM,
        XmNbottomAttachment, XmATTACH_FORM,
        XmNrightAttachment, XmATTACH_FORM,
        XmNtopAttachment, XmATTACH_WIDGET,
        XmNtopWidget, sep,
        NULL);

    XtVaSetValues(action_area, XmNfractionBase, 5, NULL);

    XtVaSetValues(pane, XmNdialogStyle,
        XmDIALOG_FULL_APPLICATION_MODAL, NULL);

    XtVaGetValues(
        control_area,
        XmNforeground, &fg,
        XmNbackground, &bg,
        NULL);

    switch (type) {
    case inform_e:
        pixmap = XmGetPixmap(
            XtScreen(control_area),
            const_cast<char*>("mx_inform.xpm"),
            fg, bg);
        break;
    case warn_e:
        pixmap = XmGetPixmap(
            XtScreen(control_area),
            const_cast<char*>("mx_warn.xpm"),
            fg, bg);
        break;
    default:
    case error_e:
        pixmap = XmGetPixmap(
            XtScreen(control_area),
            const_cast<char*>("mx_error.xpm"),
            fg, bg);
        break;
    }

    label = XtVaCreateManagedWidget(
        "icon",
        xmLabelGadgetClass,
        control_area,
        XmNlabelType, XmPIXMAP,
        XmNlabelPixmap, pixmap,
        XmNleftAttachment, XmATTACH_FORM,
        XmNtopAttachment, XmATTACH_FORM,
        XmNbottomAttachment, XmATTACH_FORM,
        NULL);

    text = XtVaCreateManagedWidget("text",
        xmLabelWidgetClass, control_area,
        XmNleftAttachment, XmATTACH_WIDGET,
        XmNleftWidget, label,
        XmNrightAttachment, XmATTACH_FORM,
        XmNtopAttachment, XmATTACH_FORM,
        XmNbottomAttachment, XmATTACH_FORM,
        XmNrightOffset, 30,
        XmNleftOffset, 5,
        XmNtopOffset, 10,
        NULL);

    ok_button = XtVaCreateManagedWidget(
        "Ok",
        xmPushButtonGadgetClass, action_area,
        XmNshowAsDefault, True,
        XmNdefaultButtonShadowThickness, 1,
        XmNleftAttachment, XmATTACH_POSITION,
        XmNleftPosition, 2,
        XmNrightAttachment, XmATTACH_POSITION,
        XmNrightPosition, 3,
        NULL);

    XtAddCallback(ok_button, XmNactivateCallback, button_cb, (void*)1);

    str = XmStringCreate(const_cast<char*>(s), XmFONTLIST_DEFAULT_TAG);
    XtVaSetValues(text, XmNlabelString, str, NULL);

    XtManageChild(action_area);
    XtManageChild(control_area);
    XtManageChild(pane);

    centre_widget(popup);
    XtPopup(popup, XtGrabExclusive);

    mess_result = none_e;
    while (mess_result == none_e) {
        mx_ui_object::XtAppNextEvent(global_app_context, &next_event);
        XtDispatchEvent(&next_event);
    }

    XtPopdown(popup);
    XmStringFree(str);
    XtDestroyWidget(popup);
}

mx_dialog_result_t mx_dialog_manager::run_confirm_now(const char* s)
{
    Widget popup;
    Widget sep, pane, action_area, control_area;
    Widget continue_button, cancel_button, text;
    Widget label;

    XmString str;

    XEvent next_event;

    Pixmap pixmap;
    Pixel fg, bg;

    popup = XtVaCreatePopupShell(
        "messInform",
        xmDialogShellWidgetClass,
        parent,
        XmNdeleteResponse, XmUNMAP,
        XmNnoResize, True,
        XmNdeleteResponse, XmDO_NOTHING,
        XmNmappedWhenManaged, False,
        XmNmwmDecorations,
        MWM_DECOR_BORDER | MWM_DECOR_TITLE | MWM_DECOR_MENU,
        XmNmwmFunctions,
        MWM_FUNC_MOVE | MWM_FUNC_CLOSE,
        NULL);

    pane = XtVaCreateWidget("pane",
        xmFormWidgetClass,
        popup,
        NULL);

    control_area = XtVaCreateWidget(
        "control",
        xmFormWidgetClass,
        pane,
        XmNleftAttachment, XmATTACH_FORM,
        XmNtopAttachment, XmATTACH_FORM,
        XmNrightAttachment, XmATTACH_FORM,
        NULL);

    sep = XtVaCreateManagedWidget(
        "separator",
        xmSeparatorWidgetClass,
        pane,
        XmNseparatorType, XmSHADOW_ETCHED_IN,
        XmNleftAttachment, XmATTACH_FORM,
        XmNrightAttachment, XmATTACH_FORM,
        XmNtopAttachment, XmATTACH_WIDGET,
        XmNtopOffset, 5,
        XmNtopWidget, control_area,
        NULL);

    action_area = XtVaCreateWidget(
        "action",
        xmFormWidgetClass,
        pane,
        XmNtopOffset, 5,
        XmNbottomOffset, 8,
        XmNleftAttachment, XmATTACH_FORM,
        XmNbottomAttachment, XmATTACH_FORM,
        XmNrightAttachment, XmATTACH_FORM,
        XmNtopAttachment, XmATTACH_WIDGET,
        XmNtopWidget, sep,
        NULL);

    XtVaSetValues(action_area, XmNfractionBase, 5, NULL);

    XtVaSetValues(pane, XmNdialogStyle,
        XmDIALOG_FULL_APPLICATION_MODAL, NULL);

    XtVaGetValues(
        control_area,
        XmNforeground, &fg,
        XmNbackground, &bg,
        NULL);

    pixmap = XmGetPixmap(
        XtScreen(control_area),
        const_cast<char*>("mx_question.xpm"),
        fg, bg);

    label = XtVaCreateManagedWidget(
        "icon",
        xmLabelGadgetClass,
        control_area,
        XmNlabelType, XmPIXMAP,
        XmNlabelPixmap, pixmap,
        XmNleftAttachment, XmATTACH_FORM,
        XmNtopAttachment, XmATTACH_FORM,
        XmNbottomAttachment, XmATTACH_FORM,
        NULL);

    text = XtVaCreateManagedWidget("text",
        xmLabelWidgetClass, control_area,
        XmNleftAttachment, XmATTACH_FORM,
        XmNleftAttachment, XmATTACH_WIDGET,
        XmNleftWidget, label,
        XmNtopAttachment, XmATTACH_FORM,
        XmNbottomAttachment, XmATTACH_FORM,
        XmNrightOffset, 30,
        XmNleftOffset, 5,
        XmNtopOffset, 10,
        NULL);

    continue_button = XtVaCreateManagedWidget(
        "Continue",
        xmPushButtonGadgetClass, action_area,
        XmNshowAsDefault, True,
        XmNdefaultButtonShadowThickness, 1,
        XmNleftAttachment, XmATTACH_POSITION,
        XmNleftPosition, 1,
        XmNrightAttachment, XmATTACH_POSITION,
        XmNrightPosition, 2,
        NULL);

    XtAddCallback(continue_button, XmNactivateCallback, button_cb,
        (void*)yes_e);

    cancel_button = XtVaCreateManagedWidget(
        "Cancel",
        xmPushButtonGadgetClass, action_area,
        XmNshowAsDefault, True,
        XmNdefaultButtonShadowThickness, 1,
        XmNleftAttachment, XmATTACH_POSITION,
        XmNleftPosition, 3,
        XmNrightAttachment, XmATTACH_POSITION,
        XmNrightPosition, 4,
        NULL);

    XtAddCallback(cancel_button, XmNactivateCallback, button_cb,
        (void*)no_e);

    str = XmStringCreate(const_cast<char*>(s), XmFONTLIST_DEFAULT_TAG);
    XtVaSetValues(text, XmNlabelString, str, NULL);

    XtManageChild(action_area);
    XtManageChild(control_area);
    XtManageChild(pane);

    XtPopup(popup, XtGrabExclusive);
    centre_widget(popup);

    mess_result = none_e;
    while (mess_result == none_e) {
        mx_ui_object::XtAppNextEvent(global_app_context, &next_event);
        XtDispatchEvent(&next_event);
    }

    XtPopdown(popup);
    XmStringFree(str);
    XtDestroyWidget(popup);

    return mess_result;
}

void mx_dialog_manager::set_parent(Widget w)
{
    parent = w;
}
