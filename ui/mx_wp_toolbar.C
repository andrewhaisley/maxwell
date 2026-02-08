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
 * MODULE/CLASS : mx_wp_toolbar
 *
 * AUTHOR : Andrew Hailsey
 *
 * This file
 *
 * DESCRIPTION:
 *
 *  Implementation for mx_wp_toolbar.h
 *
 *
 *
 *
 */

#include <stdarg.h>

#include <Xm/ComboBox.h>
#include <Xm/Form.h>
#include <Xm/List.h>
#include <Xm/PushB.h>
#include <Xm/PushBG.h>
#include <Xm/RowColumn.h>
#include <Xm/Text.h>
#include <Xm/ToggleB.h>
#include <Xm/ToggleBG.h>
#include <Xm/Xm.h>

#include <math.h>
#include <mx.h>
#include <mx_bar.h>
#include <mx_ruler_bar.h>
#include <mx_wp_editor.h>
#include <mx_wp_stsbar.h>
#include <mx_wp_toolbar.h>
#include <mx_font_family.h>

mx_wp_toolbar::mx_wp_toolbar(mx_window* window)
    : mx_bar("toolBar", window, xmFormWidgetClass)
{
    Arg args[10];
    int i, n;

    Widget char_style_checkbox;
    Widget ss_style_radio_box;
    Widget font_name_combo;
    Widget font_size_combo;
    Widget named_style_combo;

    Widget form1, form2, form3;

    current_ps = new mx_paragraph_style;
    update_ps = TRUE;

    num_style_names = 0;

    container_form = XtVaCreateManagedWidget(
        "containerForm", xmFormWidgetClass,
        widget,
        XmNleftAttachment, XmATTACH_FORM,
        XmNrightAttachment, XmATTACH_FORM,
        NULL);

    form1 = XtVaCreateManagedWidget(
        "form1", xmFormWidgetClass,
        container_form,
        XmNleftAttachment, XmATTACH_FORM,
        XmNrightAttachment, XmATTACH_FORM,
        XmNtopAttachment, XmATTACH_FORM,
        NULL);

    form2 = XtVaCreateManagedWidget(
        "form2", xmFormWidgetClass,
        container_form,
        XmNleftAttachment, XmATTACH_FORM,
        XmNrightAttachment, XmATTACH_FORM,
        XmNtopAttachment, XmATTACH_WIDGET,
        XmNtopWidget, form1,
        XmNtopOffset, 2,
        NULL);

    form3 = XtVaCreateManagedWidget(
        "form3", xmFormWidgetClass,
        container_form,
        XmNleftAttachment, XmATTACH_FORM,
        XmNrightAttachment, XmATTACH_FORM,
        XmNtopAttachment, XmATTACH_WIDGET,
        XmNtopWidget, form2,
        XmNbottomAttachment, XmATTACH_FORM,
        NULL);

    style_form = XtVaCreateManagedWidget(
        "styleForm", xmFormWidgetClass,
        form1,
        XmNleftAttachment, XmATTACH_FORM,
        XmNrightAttachment, XmATTACH_FORM,
        NULL);

    menu_form = XtVaCreateManagedWidget(
        "menuForm", xmFormWidgetClass,
        form2,
        XmNleftAttachment, XmATTACH_FORM,
        XmNrightAttachment, XmATTACH_FORM,
        NULL);

    ruler_bar = new mx_ruler_bar(form3, this);

    // style shortcuts first
    char_style_checkbox = XtVaCreateManagedWidget(
        "charStyleCheckbox",
        xmRowColumnWidgetClass,
        style_form,
        XmNorientation, XmHORIZONTAL,
        XmNleftAttachment, XmATTACH_FORM,
        XmNtopAttachment, XmATTACH_FORM,
        XmNbottomAttachment, XmATTACH_FORM,
        NULL);

    bold_button = XtVaCreateManagedWidget("boldButton", xmToggleButtonGadgetClass,
        char_style_checkbox,
        XmNleftAttachment, XmATTACH_FORM,
        XmNtopAttachment, XmATTACH_FORM,
        XmNtopOffset, 4,
        XmNmarginWidth, 0,
        XmNmarginHeight, 0,
        XmNtoggleMode, XmTOGGLE_INDETERMINATE,
        NULL);

    italic_button = XtVaCreateManagedWidget("italicButton", xmToggleButtonGadgetClass,
        char_style_checkbox,
        XmNleftAttachment, XmATTACH_WIDGET,
        XmNleftWidget, bold_button,
        XmNmarginWidth, 0,
        XmNmarginHeight, 0,
        XmNtopAttachment, XmATTACH_FORM,
        XmNtoggleMode, XmTOGGLE_INDETERMINATE,
        XmNtopOffset, 4,
        NULL);

    underline_button = XtVaCreateManagedWidget("underlineButton", xmToggleButtonGadgetClass,
        char_style_checkbox,
        XmNleftAttachment, XmATTACH_WIDGET,
        XmNleftWidget, italic_button,
        XmNmarginWidth, 0,
        XmNmarginHeight, 0,
        XmNtopAttachment, XmATTACH_FORM,
        XmNtoggleMode, XmTOGGLE_INDETERMINATE,
        XmNtopOffset, 4,
        NULL);

    XtAddCallback(bold_button, XmNvalueChangedCallback, &mx_wp_toolbar::activate_cb, this);
    XtAddCallback(italic_button, XmNvalueChangedCallback, &mx_wp_toolbar::activate_cb, this);
    XtAddCallback(underline_button, XmNvalueChangedCallback, &mx_wp_toolbar::activate_cb, this);

    ss_style_radio_box = XtVaCreateManagedWidget(
        "ssStyleRadiobox",
        xmRowColumnWidgetClass,
        style_form,
        XmNorientation, XmHORIZONTAL,
        XmNpacking, XmPACK_TIGHT,
        XmNradioBehavior, True,
        XmNleftAttachment, XmATTACH_WIDGET,
        XmNleftWidget, char_style_checkbox,
        XmNtopAttachment, XmATTACH_FORM,
        XmNbottomAttachment, XmATTACH_FORM,
        XmNradioAlwaysOne, False,
        NULL);

    subscript_button = XtVaCreateManagedWidget("subscriptButton", xmToggleButtonGadgetClass,
        ss_style_radio_box,
        XmNleftAttachment, XmATTACH_FORM,
        XmNmarginWidth, 0,
        XmNmarginHeight, 0,
        XmNtopAttachment, XmATTACH_FORM,
        XmNtopOffset, 4,
        NULL);

    superscript_button = XtVaCreateManagedWidget("superscriptButton", xmToggleButtonGadgetClass,
        ss_style_radio_box,
        XmNleftAttachment, XmATTACH_WIDGET,
        XmNleftWidget, subscript_button,
        XmNmarginWidth, 0,
        XmNmarginHeight, 0,
        XmNtopAttachment, XmATTACH_FORM,
        XmNtopOffset, 4,
        NULL);

    XtAddCallback(subscript_button, XmNvalueChangedCallback, &mx_wp_toolbar::activate_cb, this);
    XtAddCallback(superscript_button, XmNvalueChangedCallback, &mx_wp_toolbar::activate_cb, this);

    font_name_combo = XmCreateDropDownComboBox(style_form, const_cast<char*>("fontNameCombo"), NULL, 0);
    XtVaSetValues(font_name_combo,
        XmNtopAttachment, XmATTACH_FORM,
        XmNbottomAttachment, XmATTACH_FORM,
        XmNleftAttachment, XmATTACH_WIDGET,
        XmNleftWidget, ss_style_radio_box,
        NULL);

    font_size_combo = XmCreateDropDownComboBox(style_form, const_cast<char*>("fontSizeCombo"), NULL, 0);
    XtVaSetValues(font_size_combo,
        XmNtopAttachment, XmATTACH_FORM,
        XmNbottomAttachment, XmATTACH_FORM,
        XmNleftAttachment, XmATTACH_WIDGET,
        XmNleftWidget, font_name_combo,
        NULL);

    named_style_combo = XmCreateDropDownComboBox(style_form, const_cast<char*>("namedStyleCombo"), NULL, 0);
    XtVaSetValues(named_style_combo,
        XmNtopAttachment, XmATTACH_FORM,
        XmNbottomAttachment, XmATTACH_FORM,
        XmNleftAttachment, XmATTACH_WIDGET,
        XmNleftWidget, font_size_combo,
        NULL);

    font_list = XtNameToWidget(font_name_combo, "*List");
    font_text = XtNameToWidget(font_name_combo, "*Text");

    XtVaSetValues(font_text, XmNeditable, True, NULL);
    XtAddCallback(font_text, XmNactivateCallback,
        &mx_wp_toolbar::activate_cb, this);
    XtAddCallback(font_list, XmNbrowseSelectionCallback,
        &mx_wp_toolbar::activate_cb, this);

    size_list = XtNameToWidget(font_size_combo, "*List");
    size_text = XtNameToWidget(font_size_combo, "*Text");
    XtVaSetValues(size_text, XmNeditable, True, NULL);
    XtAddCallback(size_text, XmNactivateCallback,
        &mx_wp_toolbar::activate_cb, this);
    XtAddCallback(size_list, XmNbrowseSelectionCallback,
        &mx_wp_toolbar::activate_cb, this);

    style_list = XtNameToWidget(named_style_combo, "*List");
    style_text = XtNameToWidget(named_style_combo, "*Text");
    XtVaSetValues(style_text, XmNeditable, True, NULL);
    XtAddCallback(style_text, XmNactivateCallback,
        &mx_wp_toolbar::activate_cb, this);
    XtAddCallback(style_list, XmNbrowseSelectionCallback,
        &mx_wp_toolbar::activate_cb, this);

    fill_size_pulldown(size_list);
    fill_font_pulldown(font_list);

    n = 0;
    XtSetArg(args[n], XmNleftAttachment, XmATTACH_WIDGET);
    n++;
    XtSetArg(args[n], XmNleftWidget, named_style_combo);
    n++;
    XtSetArg(args[n], XmNtopAttachment, XmATTACH_FORM);
    n++;
    XtSetArg(args[n], XmNbottomAttachment, XmATTACH_FORM);
    n++;
    XtSetArg(args[n], XmNrightAttachment, XmATTACH_FORM);
    n++;
    XtSetArg(args[n], XmNorientation, XmHORIZONTAL);
    n++;
    para_style_radiobox = XmCreateRadioBox(style_form, const_cast<char*>("paraStyleRadiobox"), args, n);

    justify_button = XtVaCreateManagedWidget("justifyButton", xmToggleButtonGadgetClass,
        para_style_radiobox,
        XmNleftAttachment, XmATTACH_FORM,
        XmNmarginWidth, 0,
        XmNmarginHeight, 0,
        XmNtopAttachment, XmATTACH_FORM,
        XmNtopOffset, 4,
        NULL);

    left_justify_button = XtVaCreateManagedWidget("leftJustifyButton", xmToggleButtonGadgetClass,
        para_style_radiobox,
        XmNleftAttachment, XmATTACH_WIDGET,
        XmNleftWidget, justify_button,
        XmNmarginWidth, 0,
        XmNmarginHeight, 0,
        XmNtopAttachment, XmATTACH_FORM,
        XmNtopOffset, 4,
        NULL);

    right_justify_button = XtVaCreateManagedWidget("rightJustifyButton", xmToggleButtonGadgetClass,
        para_style_radiobox,
        XmNleftAttachment, XmATTACH_WIDGET,
        XmNleftWidget, left_justify_button,
        XmNmarginWidth, 0,
        XmNmarginHeight, 0,
        XmNtopAttachment, XmATTACH_FORM,
        XmNtopOffset, 4,
        NULL);

    centre_justify_button = XtVaCreateManagedWidget("centreJustifyButton", xmToggleButtonGadgetClass,
        para_style_radiobox,
        XmNleftAttachment, XmATTACH_WIDGET,
        XmNleftWidget, right_justify_button,
        XmNmarginWidth, 0,
        XmNmarginHeight, 0,
        XmNtopAttachment, XmATTACH_FORM,
        XmNtopOffset, 4,
        NULL);

    XtAddCallback(justify_button, XmNvalueChangedCallback, &mx_wp_toolbar::activate_cb, this);
    XtAddCallback(left_justify_button, XmNvalueChangedCallback, &mx_wp_toolbar::activate_cb, this);
    XtAddCallback(right_justify_button, XmNvalueChangedCallback, &mx_wp_toolbar::activate_cb, this);
    XtAddCallback(centre_justify_button, XmNvalueChangedCallback, &mx_wp_toolbar::activate_cb, this);

    XtManageChild(font_name_combo);
    XtManageChild(font_size_combo);
    XtManageChild(named_style_combo);
    XtManageChild(para_style_radiobox);

    new_button = XtVaCreateManagedWidget("newButton", xmPushButtonGadgetClass,
        menu_form,
        XmNleftAttachment, XmATTACH_FORM,
        XmNmarginWidth, 0,
        XmNmarginHeight, 0,
        NULL);

    open_button = XtVaCreateManagedWidget("openButton", xmPushButtonGadgetClass,
        menu_form,
        XmNleftWidget, new_button,
        XmNleftAttachment, XmATTACH_WIDGET,
        XmNmarginWidth, 0,
        XmNmarginHeight, 0,
        NULL);

    save_button = XtVaCreateManagedWidget("saveButton", xmPushButtonGadgetClass,
        menu_form,
        XmNleftWidget, open_button,
        XmNleftAttachment, XmATTACH_WIDGET,
        XmNmarginWidth, 0,
        XmNmarginHeight, 0,
        NULL);

    quit_button = XtVaCreateManagedWidget("quitButton", xmPushButtonGadgetClass,
        menu_form,
        XmNleftWidget, save_button,
        XmNleftAttachment, XmATTACH_WIDGET,
        XmNmarginWidth, 0,
        XmNmarginHeight, 0,
        NULL);

    print_button = XtVaCreateManagedWidget("printButton", xmPushButtonGadgetClass,
        menu_form,
        XmNleftWidget, quit_button,
        XmNleftAttachment, XmATTACH_WIDGET,
        XmNmarginWidth, 0,
        XmNmarginHeight, 0,
        NULL);

    cut_button = XtVaCreateManagedWidget("cutButton", xmPushButtonGadgetClass,
        menu_form,
        XmNleftWidget, print_button,
        XmNleftAttachment, XmATTACH_WIDGET,
        XmNleftOffset, 15,
        XmNmarginWidth, 0,
        XmNmarginHeight, 0,
        NULL);

    copy_button = XtVaCreateManagedWidget("copyButton", xmPushButtonGadgetClass,
        menu_form,
        XmNleftWidget, cut_button,
        XmNleftAttachment, XmATTACH_WIDGET,
        XmNmarginWidth, 0,
        XmNmarginHeight, 0,
        NULL);

    paste_button = XtVaCreateManagedWidget("pasteButton", xmPushButtonGadgetClass,
        menu_form,
        XmNleftWidget, copy_button,
        XmNleftAttachment, XmATTACH_WIDGET,
        XmNmarginWidth, 0,
        XmNmarginHeight, 0,
        NULL);

    find_button = XtVaCreateManagedWidget("findButton", xmPushButtonGadgetClass,
        menu_form,
        XmNleftWidget, paste_button,
        XmNleftAttachment, XmATTACH_WIDGET,
        XmNleftOffset, 15,
        XmNmarginWidth, 0,
        XmNmarginHeight, 0,
        NULL);

    replace_button = XtVaCreateManagedWidget("replaceButton", xmPushButtonGadgetClass,
        menu_form,
        XmNleftWidget, find_button,
        XmNleftAttachment, XmATTACH_WIDGET,
        XmNmarginWidth, 0,
        XmNmarginHeight, 0,
        NULL);

    goto_button = XtVaCreateManagedWidget("gotoButton", xmPushButtonGadgetClass,
        menu_form,
        XmNleftWidget, replace_button,
        XmNleftAttachment, XmATTACH_WIDGET,
        XmNmarginWidth, 0,
        XmNmarginHeight, 0,
        NULL);

    header_button = XtVaCreateManagedWidget("headerButton", xmPushButtonGadgetClass,
        menu_form,
        XmNleftWidget, goto_button,
        XmNleftAttachment, XmATTACH_WIDGET,
        XmNleftOffset, 15,
        XmNmarginWidth, 0,
        XmNmarginHeight, 0,
        NULL);

    footer_button = XtVaCreateManagedWidget("footerButton", xmPushButtonGadgetClass,
        menu_form,
        XmNleftWidget, header_button,
        XmNleftAttachment, XmATTACH_WIDGET,
        XmNmarginWidth, 0,
        XmNmarginHeight, 0,
        NULL);

    insert_image_button = XtVaCreateManagedWidget("insertImageButton", xmPushButtonGadgetClass,
        menu_form,
        XmNleftWidget, footer_button,
        XmNleftAttachment, XmATTACH_WIDGET,
        XmNleftOffset, 15,
        XmNmarginWidth, 0,
        XmNmarginHeight, 0,
        NULL);

    insert_diagram_button = XtVaCreateManagedWidget("insertDiagramButton", xmPushButtonGadgetClass,
        menu_form,
        XmNleftWidget, insert_image_button,
        XmNleftAttachment, XmATTACH_WIDGET,
        XmNmarginWidth, 0,
        XmNmarginHeight, 0,
        NULL);

    insert_table_button = XtVaCreateManagedWidget("insertTableButton", xmPushButtonGadgetClass,
        menu_form,
        XmNleftWidget, insert_diagram_button,
        XmNleftAttachment, XmATTACH_WIDGET,
        XmNmarginWidth, 0,
        XmNmarginHeight, 0,
        NULL);

    insert_bullets_button = XtVaCreateManagedWidget("insertBulletsButton", xmPushButtonGadgetClass,
        menu_form,
        XmNleftWidget, insert_table_button,
        XmNleftAttachment, XmATTACH_WIDGET,
        XmNmarginWidth, 0,
        XmNmarginHeight, 0,
        NULL);

    insert_symbol_button = XtVaCreateManagedWidget("insertSymbolButton", xmPushButtonGadgetClass,
        menu_form,
        XmNleftWidget, insert_bullets_button,
        XmNleftAttachment, XmATTACH_WIDGET,
        XmNmarginWidth, 0,
        XmNmarginHeight, 0,
        NULL);

    insert_page_break_button = XtVaCreateManagedWidget("insertPageBreakButton", xmPushButtonGadgetClass,
        menu_form,
        XmNleftWidget, insert_symbol_button,
        XmNleftAttachment, XmATTACH_WIDGET,
        XmNmarginWidth, 0,
        XmNmarginHeight, 0,
        NULL);

    format_character_button = XtVaCreateManagedWidget("formatCharacterButton", xmPushButtonGadgetClass,
        menu_form,
        XmNleftWidget, insert_page_break_button,
        XmNleftAttachment, XmATTACH_WIDGET,
        XmNleftOffset, 15,
        XmNmarginWidth, 0,
        XmNmarginHeight, 0,
        NULL);

    format_paragraph_button = XtVaCreateManagedWidget("formatParagraphButton", xmPushButtonGadgetClass,
        menu_form,
        XmNleftWidget, format_character_button,
        XmNleftAttachment, XmATTACH_WIDGET,
        XmNmarginWidth, 0,
        XmNmarginHeight, 0,
        NULL);

#if 0
    format_columns_button = XtVaCreateManagedWidget("formatColumnsButton", xmPushButtonGadgetClass,
                    menu_form, 
                    XmNleftWidget, format_paragraph_button,
                    XmNleftAttachment, XmATTACH_WIDGET,
                    XmNmarginWidth, 0,
                    XmNmarginHeight, 0,
                    NULL);
#endif

    format_table_button = XtVaCreateManagedWidget("formatTableButton", xmPushButtonGadgetClass,
        menu_form,
#if 0
                    XmNleftWidget, format_columns_button,
#else
        XmNleftWidget, format_paragraph_button,
#endif
        XmNleftAttachment, XmATTACH_WIDGET,
        XmNmarginWidth, 0,
        XmNmarginHeight, 0,
        NULL);

    spelling_button = XtVaCreateManagedWidget("spellingButton", xmPushButtonGadgetClass,
        menu_form,
        XmNleftWidget, format_table_button,
        XmNleftAttachment, XmATTACH_WIDGET,
        XmNleftOffset, 15,
        XmNmarginWidth, 0,
        XmNmarginHeight, 0,
        NULL);

    envelope_button = XtVaCreateManagedWidget("envelopeButton", xmPushButtonGadgetClass,
        menu_form,
        XmNleftWidget, spelling_button,
        XmNleftAttachment, XmATTACH_WIDGET,
        XmNmarginWidth, 0,
        XmNmarginHeight, 0,
        NULL);

    // add the activate callbacks
    {
        Widget wa[25] = {
            new_button,
            open_button,
            save_button,
            quit_button,
            print_button,
            cut_button,
            copy_button,
            paste_button,
            find_button,
            replace_button,
            goto_button,
            header_button,
            footer_button,
            insert_image_button,
            insert_diagram_button,
            insert_table_button,
            insert_bullets_button,
            insert_symbol_button,
            insert_page_break_button,
            format_character_button,
            format_paragraph_button,
#if 0
        format_columns_button,
#endif
            format_table_button,
            spelling_button,
            envelope_button
        };

#if 0
    for (i = 0; i < 25; i++)
#else
        for (i = 0; i < 24; i++)
#endif
        {
            XtAddCallback(wa[i], XmNactivateCallback,
                &mx_wp_toolbar::activate_cb, this);

            XtAddCallback(wa[i], XmNarmCallback,
                (XtCallbackProc)&mx_wp_toolbar::arm_cb, (XtPointer)this);

            XtAddCallback(wa[i], XmNdisarmCallback,
                (XtCallbackProc)&mx_wp_toolbar::disarm_cb, (XtPointer)this);
        }
    }

    return;
}

mx_wp_toolbar::~mx_wp_toolbar()
{
    for (int i = 0; i < num_style_names; i++)
        delete[] style_names[i];
    delete ruler_bar;
    delete current_ps;
}

void mx_wp_toolbar::activate_cb(
    Widget w,
    XtPointer ptr,
    XtPointer cd)
{
    mx_wp_toolbar* bar = (mx_wp_toolbar*)ptr;

    bar->handle_button(w);
}

void mx_wp_toolbar::handle_button(Widget w)
{
    mx_window* window = get_window();
    mx_wp_editor* ed = (mx_wp_editor*)window->get_editor();

    int err = MX_ERROR_OK;

    window->set_busy();

    if (w == new_button)
        ed->file_new(err);
    else if (w == open_button)
        ed->file_open(err);
    else if (w == save_button)
        ed->file_save(err);
    else if (w == quit_button) {
        if (ed->file_close(err)) {
            // return immediately as the window has been destroyed -
            // so we can't set it not-busy
            return;
        }
    } else if (w == print_button)
        ed->file_print(err);
    else if (w == cut_button)
        ed->edit_cut(err);
    else if (w == copy_button)
        ed->edit_copy(err);
    else if (w == paste_button)
        ed->edit_paste(err);
    else if (w == find_button)
        ed->edit_find(err);
    else if (w == replace_button)
        ed->edit_find_and_replace(err);
    else if (w == goto_button)
        ed->edit_goto_page(err);
    else if (w == header_button)
        ed->view_header(err);
    else if (w == footer_button)
        ed->view_footer(err);
    else if (w == insert_image_button)
        ed->insert_image(err);
    else if (w == insert_diagram_button)
        ed->insert_diagram(err);
    else if (w == insert_table_button)
        ed->insert_table(err);
    else if (w == insert_bullets_button)
        ed->insert_bullets(err);
    else if (w == insert_symbol_button)
        ed->insert_symbol(err);
    else if (w == insert_page_break_button)
        ed->insert_page_break(err);
    else if (w == format_character_button)
        ed->format_character(err);
    else if (w == format_paragraph_button)
        ed->format_paragraph(err);
#if 0
    else if (w == format_columns_button)
    ed->format_columns(err);
#endif
    else if (w == format_table_button)
        ed->format_table(err);
    else if (w == spelling_button)
        ed->tools_spelling(err);
    else if (w == envelope_button)
        ed->tools_envelope(err);
    else if (w == font_text)
        font_family_change(err, ed);
    else if (w == style_text)
        style_change(err, ed);
    else if (w == size_text)
        font_size_change(err, ed);
    else if (w == justify_button) {
        if (XmToggleButtonGetState(w)) {
            justify_change(err, ed, w);
        }
    } else if (w == left_justify_button) {
        if (XmToggleButtonGetState(w)) {
            justify_change(err, ed, w);
        }
    } else if (w == right_justify_button) {
        if (XmToggleButtonGetState(w)) {
            justify_change(err, ed, w);
        }
    } else if (w == centre_justify_button) {
        if (XmToggleButtonGetState(w)) {
            justify_change(err, ed, w);
        }
    } else if (w == superscript_button) {
        // ignore button coming up when other button pressed
        // so only take notice of this if, the other button is now off

        if (XmToggleButtonGetState(superscript_button) || !XmToggleButtonGetState(subscript_button)) {
            align_change(err, ed, w);
        }
    } else if (w == subscript_button) {
        if (XmToggleButtonGetState(subscript_button) || !XmToggleButtonGetState(superscript_button)) {
            align_change(err, ed, w);
        }
    } else if (w == bold_button)
        bold_change(err, ed);
    else if (w == italic_button)
        italic_change(err, ed);
    else if (w == underline_button)
        underline_change(err, ed);
    else if (w == style_list)
        style_change(err, ed);
    else if (w == size_list)
        font_size_change(err, ed);
    else if (w == font_list)
        font_family_change(err, ed);

    window->clear_busy();
    MX_ERROR_CHECK(err);

    return;

abort:
    global_error_trace->print();
}

void mx_wp_toolbar::arm_cb(Widget w, XtPointer ptr, XtPointer cd)
{
    mx_wp_toolbar* bar = (mx_wp_toolbar*)ptr;

    bar->handle_arm(w);
}

void mx_wp_toolbar::handle_arm(Widget w)
{
    mx_window* window = get_window();

    mx_wp_editor* ed = (mx_wp_editor*)window->get_editor();
    mx_wp_statusbar* st = (mx_wp_statusbar*)ed->get_status_bar();

    if (w == new_button)
        st->update_b("Creates a new document.", TRUE);
    else if (w == open_button)
        st->update_b("Opens an existing document.", TRUE);
    else if (w == save_button)
        st->update_b("Saves changes to this document.", TRUE);
    else if (w == quit_button)
        st->update_b("Close this window.", TRUE);
    else if (w == print_button)
        st->update_b("Prints this document.", TRUE);
    else if (w == cut_button)
        st->update_b("Prints this document.", TRUE);
    else if (w == copy_button)
        st->update_b("Copy current selection to clip-board.", TRUE);
    else if (w == paste_button)
        st->update_b("Paste current clip-board into cursor position.", TRUE);
    else if (w == find_button)
        st->update_b("Search for text in document.", TRUE);
    else if (w == replace_button)
        st->update_b("Search and replace text in document.", TRUE);
    else if (w == goto_button)
        st->update_b("Move to a specified page in this document.", TRUE);
    else if (w == header_button)
        st->update_b("Edit the page header.", TRUE);
    else if (w == footer_button)
        st->update_b("Edit the page footer.", TRUE);
    else if (w == insert_image_button)
        st->update_b("Insert an image into the document.", TRUE);
    else if (w == insert_diagram_button)
        st->update_b("Insert a diagram into the document.", TRUE);
    else if (w == insert_table_button)
        st->update_b("Insert a table into the document.", TRUE);
    else if (w == insert_bullets_button)
        st->update_b("Reformat selected paragraphs as bullet points.", TRUE);
    else if (w == insert_symbol_button)
        st->update_b("Insert special character (e.g. © symbol).", TRUE);
    else if (w == insert_page_break_button)
        st->update_b("Insert a page break.", TRUE);
    else if (w == format_character_button)
        st->update_b("Change format of characters (font, size etc).", TRUE);
    else if (w == format_paragraph_button)
        st->update_b("Change format of paragraph.", TRUE);
#if 0
    else if (w == format_columns_button)
    st->update("Change number and layout of columns.", TRUE);
#endif
    else if (w == format_table_button)
        st->update_b("Change layout of table.", TRUE);
    else if (w == spelling_button)
        st->update_b("Check spellings.", TRUE);
    else if (w == envelope_button)
        st->update_b("Print an envelope.", TRUE);
}

void mx_wp_toolbar::disarm_cb(Widget w, XtPointer ptr, XtPointer cd)
{
    mx_wp_toolbar* bar = (mx_wp_toolbar*)ptr;

    bar->handle_disarm();
}

void mx_wp_toolbar::handle_disarm()
{
    mx_window* window = get_window();

    mx_wp_editor* ed = (mx_wp_editor*)window->get_editor();
    mx_wp_statusbar* st = (mx_wp_statusbar*)ed->get_status_bar();

    st->update_b(MX_MAXWELL_VERSION, FALSE);
}

void mx_wp_toolbar::view_actions(bool s)
{
    if (s) {
        XtUnmanageChild(menu_form);
    } else {
        XtManageChild(menu_form);
    }
}

void mx_wp_toolbar::view_styles(bool s)
{
    if (s) {
        XtUnmanageChild(style_form);
    } else {
        XtManageChild(style_form);
    }
}

void mx_wp_toolbar::view_ruler(bool s)
{
    if (s) {
        XtUnmanageChild(ruler_bar->get_widget());
    } else {
        XtManageChild(ruler_bar->get_widget());
    }
}

void mx_wp_toolbar::fill_size_pulldown(Widget list)
{
    int sizes[17] = {
        72, 48, 32, 24,
        18, 17, 16, 15,
        14, 13, 12, 11,
        10, 9, 8, 7,
        6
    };
    int i;

    char s[10];
    XmString str;

    for (i = 0; i < 17; i++) {
        sprintf(s, "%d", sizes[i]);
        str = XmStringCreate(s, XmFONTLIST_DEFAULT_TAG);
        XmListAddItem(list, str, 1);
        XmStringFree(str);
    }
    XtVaSetValues(list, XmNvisibleItemCount, 17, NULL);
}

void mx_wp_toolbar::fill_font_pulldown(Widget list)
{
    mx_font f;

    int num_fonts = 0;

    XmString str;

    for (auto i : f.get_font_family_names()) {
        str = XmStringCreate(const_cast<char *>(i.c_str()), XmFONTLIST_DEFAULT_TAG);
        XmListAddItem(list, str, 1);
        XmStringFree(str);
        num_fonts++;
    }

    XtVaSetValues(list,
        XmNvisibleItemCount, num_fonts < 25 ? num_fonts : 25,
        NULL);
}

void mx_wp_toolbar::set_style_pulldown(mx_document* d)
{
    int i, n, err = MX_ERROR_OK;
    mx_paragraph_style* s;
    XmString str[MX_DOCUMENT_MAX_STYLES];

    n = d->get_num_styles(err);
    MX_ERROR_CHECK(err);

    XtVaSetValues(style_list,
        XmNvisibleItemCount, n < 25 ? n : 25,
        NULL);

    for (i = 0; i < num_style_names; i++) {
        delete[] style_names[i];
    }
    num_style_names = n;

    for (i = 0; i < n; i++) {
        s = d->get_style(err, i);
        MX_ERROR_CHECK(err);
        str[i] = XmStringCreate(s->get_name(), XmFONTLIST_DEFAULT_TAG);

        style_names[i] = new char[strlen(s->get_name()) + 1];
        strcpy(style_names[i], s->get_name());
    }

    XtVaSetValues(style_list, XmNitemCount, n, XmNitems, str, NULL);

    for (i = 0; i < n; i++) {
        XmStringFree(str[i]);
    }

    return;

abort:
    global_error_trace->print();
    MX_ERROR_CLEAR(err);
}

void set_toggle_mode(Widget w, unsigned char mode)
{
    unsigned char was;

    XtVaGetValues(w, XmNtoggleMode, &was, NULL);

    if (was != mode) {
        XtVaSetValues(w, XmNtoggleMode, mode, NULL);
    }
}

void set_toggle(Widget toggle, Boolean value)
{
    switch (value) {
    case XmINDETERMINATE:
        set_toggle_mode(toggle, XmTOGGLE_INDETERMINATE);
        break;
    default:
        set_toggle_mode(toggle, XmTOGGLE_BOOLEAN);
        break;
    }
    XmToggleButtonSetState(toggle, value, False);
}

void mx_wp_toolbar::set_bold_toggle(Boolean b)
{
    set_toggle(bold_button, b);
}

void mx_wp_toolbar::set_italic_toggle(Boolean b)
{
    set_toggle(italic_button, b);
}

void mx_wp_toolbar::set_underline_toggle(Boolean b)
{
    set_toggle(underline_button, b);
}

void mx_wp_toolbar::set_string(Widget w, const char* s)
{
    char* was;

    was = XmTextGetString(w);

    if (strcmp(was, s) != 0) {
        XmTextSetString(w, (char*)s);
    }

    XtFree(was);
}

// family name may be NULL (for indeterminate)
void mx_wp_toolbar::set_current_font_family(const char* family_name)
{
    if (family_name == NULL) {
        set_string(font_text, "");
    } else {
        set_string(font_text, (char*)family_name);
    }
}

// size may be 0 (for indeterminate)
void mx_wp_toolbar::set_current_font_size(float s)
{
    char str[20];

    if (MX_FLOAT_EQ(s, 0.0)) {
        XmTextSetString(size_text, const_cast<char*>(""));
    } else {
        sprintf(str, "%.2f", s);
        set_string(size_text, str);
    }
}

void mx_wp_toolbar::set_current_style_name(const char* name)
{
    if (name == NULL) {
        set_string(style_text, "");
    } else {
        if (strcmp(name, "") == 0) {
            set_string(style_text, "default");
        } else {
            set_string(style_text, name);
        }
    }
}

// justification
void mx_wp_toolbar::set_justification(mx_justification_t j, bool is_indeterminate)
{
    Widget off[3], on = nullptr;

    XtVaSetValues(para_style_radiobox,
        XmNradioAlwaysOne, is_indeterminate ? True : False,
        NULL);

    off[0] = left_justify_button;
    off[1] = right_justify_button;
    off[2] = centre_justify_button;

    if (!is_indeterminate) {
        switch (j) {
        case mx_left_aligned:
            on = left_justify_button;
            off[0] = justify_button;
            break;

        case mx_right_aligned:
            on = right_justify_button;
            off[1] = justify_button;
            break;

        case mx_centred:
            on = centre_justify_button;
            off[2] = justify_button;
            break;

        case mx_justified:
            on = justify_button;
            break;
        }
        set_toggle(off[0], False);
        set_toggle(off[1], False);
        set_toggle(off[2], False);
        set_toggle(on, True);
    } else {
        set_toggle(justify_button, False);
        set_toggle(left_justify_button, False);
        set_toggle(right_justify_button, False);
        set_toggle(centre_justify_button, False);
    }
}

void mx_wp_toolbar::set_superscript_toggles(mx_align_t a)
{
    switch (a) {
    case mx_superscript:
        set_toggle(superscript_button, True);
        set_toggle(subscript_button, False);
        break;
    case mx_subscript:
        set_toggle(subscript_button, True);
        set_toggle(superscript_button, False);
        break;
    default:
        set_toggle(subscript_button, False);
        set_toggle(superscript_button, False);
        break;
    }
}

// this should get called whenever the current selection changes
void mx_wp_toolbar::set_current_style(mx_paragraph_style* styles[], int ns)
{
    int i;
    bool ind, bold_ind, italic_ind, bo, it;
    mx_font* fo;

    if (ns == 1) {
        if (update_ps) {
            update_ps = FALSE;
        } else {
            set_current_style(*styles[0]);
            return;
        }
    } else {
        update_ps = TRUE;
    }

    ind = FALSE;
    for (i = 1; i < ns; i++) {
        if (styles[0]->justification != styles[i]->justification) {
            ind = TRUE;
            break;
        }
    }

    set_justification(styles[0]->justification, ind);

    ind = FALSE;
    for (i = 1; i < ns; i++) {
        if (styles[0]->get_char_style()->get_font()->get_family() != styles[i]->get_char_style()->get_font()->get_family()) {
            ind = TRUE;
            break;
        }
    }

    set_current_font_family(ind ? (char*)NULL : (char*)(styles[0]->get_char_style()->get_font()->get_family().c_str()));

    // Set bold/italic to insensitive if this family can't do them
    if (!ind) {
        fo = styles[0]->get_char_style()->get_font();
        fo->get_font_family()->family_has_B_or_I(bo, it);
        XtSetSensitive(bold_button, bo);
        XtSetSensitive(italic_button, it);
    }

    ind = FALSE;
    for (i = 1; i < ns; i++) {
        if (styles[0]->get_char_style()->alignment != styles[i]->get_char_style()->alignment) {
            ind = TRUE;
            break;
        }
    }

    set_superscript_toggles(ind ? mx_normal_align : styles[0]->get_char_style()->alignment);

    ind = FALSE;
    for (i = 1; i < ns; i++) {
        if (styles[0]->get_char_style()->get_font()->get_size() != styles[i]->get_char_style()->get_font()->get_size()) {
            ind = TRUE;
            break;
        }
    }

    set_current_font_size(ind ? 0 : styles[0]->get_char_style()->get_font()->get_size());

    ind = FALSE;
    for (i = 1; i < ns; i++) {
        if (styles[0]->justification != styles[i]->justification) {
            ind = TRUE;
            break;
        }
    }

    set_justification(styles[0]->justification, ind);

    ind = FALSE;
    for (i = 1; i < ns; i++) {
        if (strcmp(styles[0]->get_name(), styles[i]->get_name()) != 0) {
            ind = TRUE;
            break;
        }
    }

    set_current_style_name(ind ? (char*)NULL : (char*)(styles[0]->get_name()));

    bold_ind = FALSE;
    italic_ind = FALSE;

    for (i = 1; i < ns; i++) {
        if (styles[0]->get_char_style()->get_font()->get_bold() != styles[i]->get_char_style()->get_font()->get_bold()) {
            bold_ind = TRUE;
        }
        if (styles[0]->get_char_style()->get_font()->get_italic() != styles[i]->get_char_style()->get_font()->get_italic()) {
            italic_ind = TRUE;
        }
    }

    set_bold_toggle(bold_ind ? XmINDETERMINATE : (styles[0]->get_char_style()->get_font()->get_bold() ? True : False));
    set_italic_toggle(italic_ind ? XmINDETERMINATE : (styles[0]->get_char_style()->get_font()->get_italic() ? True : False));

    ind = FALSE;
    for (i = 1; i < ns; i++) {
        if (styles[0]->get_char_style()->effects != styles[i]->get_char_style()->effects) {
            ind = TRUE;
            break;
        }
    }

    set_underline_toggle(ind ? XmINDETERMINATE : (styles[0]->get_char_style()->effects == mx_underline) ? True
                                                                                                        : False);
}

void mx_wp_toolbar::set_current_style(mx_paragraph_style& ps)
{
    mx_char_style_mod cs_mod(*current_ps->get_char_style(),
        *ps.get_char_style());
    mx_font_mod* font_mod = cs_mod.get_font_mod();
    mx_align_t new_alignment;
    mx_char_effects_t new_effects;
    bool toggle;
    float new_size;
    std::string new_family;

    if (font_mod->get_bold_mod(toggle)) {
        set_bold_toggle(toggle ? True : False);
    }

    if (font_mod->get_italic_mod(toggle)) {
        set_italic_toggle(toggle ? True : False);
    }

    if (cs_mod.get_effects_mod(new_effects)) {
        set_underline_toggle((new_effects == mx_underline) ? True : False);
    }

    if (cs_mod.get_alignment_mod(new_alignment)) {
        set_superscript_toggles(new_alignment);
    }

    if (font_mod->get_family_mod(new_family)) {
        set_current_font_family(new_family.c_str());
    }

    if (font_mod->get_size_mod(new_size)) {
        set_current_font_size(new_size);
    }

    if (strcmp(current_ps->get_name(), ps.get_name()) != 0) {
        set_current_style_name(ps.get_name());
    }

    if (current_ps->justification != ps.justification) {
        set_justification(ps.justification, FALSE);
    }

    *current_ps = ps;
}

void mx_wp_toolbar::font_family_change(int& err, mx_wp_editor* ed)
{
    mx_font f;

    bool b, i;
    char* s;
    mx_list name_list;

    s = XmTextGetString(font_text);

    for (auto font_name : f.get_font_family_names()) {

        if (strncasecmp(s, font_name.c_str(), strlen(s)) == 0) {
            f.get_font_family()->family_has_B_or_I(b, i);

            XtSetSensitive(bold_button, b);
            XtSetSensitive(italic_button, i);

            if (!b) {
                set_bold_toggle(FALSE);
                ed->set_bold(err, FALSE);
                MX_ERROR_CHECK(err);
            }

            if (!i) {
                set_italic_toggle(FALSE);
                ed->set_italic(err, FALSE);
                MX_ERROR_CHECK(err);
            }

            XmTextSetString(font_text, const_cast<char *>(font_name.c_str()));
            ed->set_font_family(err, font_name.c_str());
            MX_ERROR_CHECK(err);

            XtFree(s);
            return;
        }
    }
    XtFree(s);
    bell();

abort:;
}

void mx_wp_toolbar::style_change(int& err, mx_wp_editor* ed)
{
    int i;
    char* s;

    s = XmTextGetString(style_text);

    for (i = 0; i < num_style_names; i++) {
        if (strncasecmp(s, style_names[i], strlen(s)) == 0) {
            ed->set_style(err, style_names[i]);
            MX_ERROR_CHECK(err);

            XmTextSetString(style_text, style_names[i]);
            XtFree(s);
            return;
        }
    }
    XtFree(s);
    bell();
abort:;
}

void mx_wp_toolbar::font_size_change(int& err, mx_wp_editor* ed)
{
    char *s, t[20];
    float f;

    s = XmTextGetString(size_text);
    sscanf(s, "%f", &f);
    XtFree(s);
    if (f < 1.0) {
        bell();
        return;
    }

    sprintf(t, "%.2f", f);
    XmTextSetString(size_text, t);

    ed->set_font_size(err, f);
    MX_ERROR_CHECK(err);
abort:;
}

void mx_wp_toolbar::justify_change(int& err, mx_wp_editor* ed, Widget w)
{
    if (w == justify_button)
        ed->set_justification(err, mx_justified);
    else if (w == left_justify_button)
        ed->set_justification(err, mx_left_aligned);
    else if (w == right_justify_button)
        ed->set_justification(err, mx_right_aligned);
    else if (w == centre_justify_button)
        ed->set_justification(err, mx_centred);
    MX_ERROR_CHECK(err);
abort:;
}

void mx_wp_toolbar::align_change(int& err, mx_wp_editor* ed, Widget which)
{
    if (XmToggleButtonGetState(which)) {
        if (which == subscript_button) {
            ed->set_alignment(err, mx_subscript);
        } else {
            ed->set_alignment(err, mx_superscript);
        }
    } else {
        ed->set_alignment(err, mx_normal_align);
    }
    MX_ERROR_CHECK(err);
abort:;
}

void mx_wp_toolbar::bold_change(int& err, mx_wp_editor* ed)
{
    ed->set_bold(err, XmToggleButtonGetState(bold_button));
    MX_ERROR_CHECK(err);
abort:;
}

void mx_wp_toolbar::italic_change(int& err, mx_wp_editor* ed)
{
    ed->set_italic(err, XmToggleButtonGetState(italic_button));
    MX_ERROR_CHECK(err);
abort:;
}

void mx_wp_toolbar::underline_change(int& err, mx_wp_editor* ed)
{
    ed->set_underline(err, XmToggleButtonGetState(underline_button));
    MX_ERROR_CHECK(err);
abort:;
}

void mx_wp_toolbar::set_visible(menu_item_name_t item, ...)
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

void mx_wp_toolbar::set_invisible(menu_item_name_t item, ...)
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

void mx_wp_toolbar::set_item_visible(menu_item_name_t item, bool s)
{
    Widget w;

    switch (item) {
    case mx_file_new_e:
        w = new_button;
        break;
    case mx_file_open_e:
        w = open_button;
        break;
    case mx_file_close_e:
        w = quit_button;
        break;
    case mx_file_save_e:
        w = save_button;
        break;
    case mx_file_print_e:
        w = print_button;
        break;
    case mx_edit_cut_e:
        w = cut_button;
        break;
    case mx_edit_copy_e:
        w = copy_button;
        break;
    case mx_edit_paste_e:
        w = paste_button;
        break;
    case mx_edit_find_e:
        w = find_button;
        break;
    case mx_edit_replace_e:
        w = replace_button;
        break;
    case mx_edit_goto_e:
        w = goto_button;
        break;
    case mx_insert_break_e:
        w = insert_page_break_button;
        break;
    case mx_insert_symbol_e:
        w = insert_symbol_button;
        break;
    case mx_insert_bullets_e:
        w = insert_bullets_button;
        break;
    case mx_insert_image_e:
        w = insert_image_button;
        break;
    case mx_insert_diagram_e:
        w = insert_diagram_button;
        break;
    case mx_insert_table_e:
        w = insert_table_button;
        break;
    case mx_format_paragraph_e:
        w = format_paragraph_button;
        break;
    case mx_format_character_e:
        w = format_character_button;
        break;
#if 0
        case mx_format_columns_e :
                w = format_columns_button; break;
#endif
    case mx_format_table_e:
        w = format_table_button;
        break;
    case mx_format_make_bold_e:
        w = bold_button;
        break;
    case mx_format_make_italic_e:
        w = italic_button;
        break;
    case mx_format_make_underline_e:
        w = underline_button;
        break;
    case mx_tools_spelling_e:
        w = spelling_button;
        break;
    case mx_tools_envelope_e:
        w = envelope_button;
        break;
    default:
        return;
    }
    XtVaSetValues(w, XmNsensitive, s ? True : False, NULL);
}
