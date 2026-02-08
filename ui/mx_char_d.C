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
 * MODULE/CLASS : mx_char_d
 *
 * AUTHOR : Andrew Haisley
 *
 * This file
 *
 * DESCRIPTION:
 *
 *  A character style dialog
 *
 *
 *
 *
 */

#include <dirent.h>
#include <errno.h>
#include <pwd.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>

#include <Xm/ComboBox.h>
#include <Xm/DrawingA.h>
#include <Xm/Form.h>
#include <Xm/Frame.h>
#include <Xm/Label.h>
#include <Xm/LabelG.h>
#include <Xm/List.h>
#include <Xm/Protocols.h>
#include <Xm/PushBG.h>
#include <Xm/RowColumn.h>
#include <Xm/Separator.h>
#include <Xm/Text.h>
#include <Xm/ToggleB.h>
#include <Xm/ToggleBG.h>
#include <Xm/Xm.h>

#include <mx.h>
#include <mx_file_type.h>
#include <mx_std.h>
#include <mx_ui_object.h>
#include <mx_dialog.h>
#include <mx_font_family.h>
#include <mx_char_d.h>
#include <mx_sc_device.h>

void mx_char_d::drawing_area_cb(
    Widget widget,
    XtPointer client_data,
    XtPointer call_data)
{
    mx_char_d* d = (mx_char_d*)client_data;
    XmDrawingAreaCallbackStruct* cbs = (XmDrawingAreaCallbackStruct*)call_data;

    if (cbs->reason == XmCR_INPUT) {
        return;
    } else {
        d->draw_sample();
    }
}

void mx_char_d::alignment_offset_return_cb(
    Widget widget,
    XtPointer client_data,
    XtPointer call_data)
{
    mx_char_d* d = (mx_char_d*)client_data;

    d->set_alignment_offset();
    d->draw_sample();
}

void mx_char_d::size_return_cb(
    Widget widget,
    XtPointer client_data,
    XtPointer call_data)
{
    mx_char_d* d = (mx_char_d*)client_data;

    d->set_size();
    d->draw_sample();
}

void mx_char_d::spacing_return_cb(
    Widget widget,
    XtPointer client_data,
    XtPointer call_data)
{
    mx_char_d* d = (mx_char_d*)client_data;

    d->set_spacing();
    d->draw_sample();
}

void mx_char_d::style_change_cb(
    Widget widget,
    XtPointer client_data,
    XtPointer call_data)
{
    mx_char_d* d = (mx_char_d*)client_data;

    d->set_style();
    d->draw_sample();
}

void mx_char_d::align_change_cb(
    Widget widget,
    XtPointer client_data,
    XtPointer call_data)
{
    mx_char_d* d = (mx_char_d*)client_data;

    d->set_align();
    d->draw_sample();
}

void mx_char_d::colour_cb(
    Widget widget,
    XtPointer client_data,
    XtPointer call_data)
{
    mx_char_d* d = (mx_char_d*)client_data;

    d->set_colour();
    d->draw_sample();
}

void mx_char_d::button_cb(
    Widget widget,
    XtPointer client_data,
    XtPointer call_data)
{
    mx_char_d* d = (mx_char_d*)client_data;

    if (widget == d->ok_button) {
        d->modal_result = yes_e;
    } else if (widget == d->apply_button) {
        d->modal_result = apply_e;
    } else {
        d->modal_result = cancel_e;
    }
}

void mx_char_d::list_cb(Widget list, XtPointer client_data, XtPointer call_data)
{
    mx_char_d* d = (mx_char_d*)client_data;

    char* choice;

    XmListCallbackStruct* cbs = (XmListCallbackStruct*)call_data;

    if (cbs->reason == XmCR_BROWSE_SELECT) {
        // single clicked on a font name
        XmStringGetLtoR(cbs->item, XmFONTLIST_DEFAULT_TAG, &choice);
        d->set_font(choice);
        XtFree(choice);
        return;
    }

    if (cbs->reason == XmCR_DEFAULT_ACTION) {
        d->modal_result = yes_e;
    }
}

mx_char_d::mx_char_d(Widget parent, bool has_apply) : mx_dialog("character", parent, TRUE, FALSE)
{
    Widget style_form, sample_frame;
    Widget frame, size_combo, line_spacing_combo, char_spacing_combo;
    Widget alignment_offset_combo;
    Arg args[20];
    int i, n;
    Widget weight_form, weight_frame, radio_frame;
    Widget combo_form, label_rc;

    base_style = new mx_char_style;
    base_diffs = new mx_char_style_mod;

    XGCValues gcv;

    XtVaSetValues(action_area, XmNfractionBase, has_apply ? 7 : 5, NULL);

    font_list = XmCreateScrolledList(control_area, const_cast<char*>("fontList"), NULL, 0);

    XtVaSetValues(font_list,
        XmNselectionPolicy, XmBROWSE_SELECT,
        NULL);

    XtVaSetValues(XtParent(font_list),
        XmNtopAttachment, XmATTACH_FORM,
        XmNleftAttachment, XmATTACH_FORM,
        XmNleftOffset, 5,
        XmNbottomOffset, 5,
        XmNrightOffset, 5,
        XmNtopOffset, 5,
        NULL);

    XtAddCallback(font_list, XmNdefaultActionCallback, list_cb, this);
    XtAddCallback(font_list, XmNbrowseSelectionCallback, list_cb, this);

    style_form = XtVaCreateWidget(
        "style",
        xmFormWidgetClass,
        control_area,
        XmNtopAttachment, XmATTACH_FORM,
        XmNleftAttachment, XmATTACH_WIDGET,
        XmNleftWidget, font_list,
        NULL);

    sample_frame = XtVaCreateWidget(
        "sampleFrame",
        xmFrameWidgetClass,
        control_area,
        XmNtopAttachment, XmATTACH_WIDGET,
        XmNtopWidget, font_list,
        XmNleftAttachment, XmATTACH_FORM,
        XmNrightAttachment, XmATTACH_FORM,
        XmNbottomAttachment, XmATTACH_FORM,
        NULL);

    (void)XtVaCreateManagedWidget(
        "sampleFrameLabel",
        xmLabelGadgetClass,
        sample_frame,
        XmNchildType, XmFRAME_TITLE_CHILD,
        NULL);

    drawing_area = XtVaCreateManagedWidget(
        "drawingArea",
        xmDrawingAreaWidgetClass, sample_frame,
        XmNchildType, XmFRAME_WORKAREA_CHILD,
        XmNleftAttachment, XmATTACH_WIDGET,
        XmNleftWidget, sample_frame,
        XmNrightAttachment, XmATTACH_WIDGET,
        XmNrightWidget, sample_frame,
        XmNtopAttachment, XmATTACH_WIDGET,
        XmNtopWidget, sample_frame,
        XmNbottomAttachment, XmATTACH_WIDGET,
        XmNbottomWidget, sample_frame,
        NULL);

    gcv.foreground = BlackPixelOfScreen(XtScreen(drawing_area));
    gc = XCreateGC(
        XtDisplay(drawing_area),
        RootWindowOfScreen(XtScreen(drawing_area)),
        GCForeground,
        &gcv);

    XtAddCallback(
        drawing_area,
        XmNexposeCallback,
        drawing_area_cb,
        (XtPointer)this);

    frame = XtVaCreateWidget(
        "frame1",
        xmFrameWidgetClass,
        control_area,
        XmNtopAttachment, XmATTACH_FORM,
        XmNleftAttachment, XmATTACH_WIDGET,
        XmNleftWidget, style_form,
        XmNrightAttachment, XmATTACH_FORM,
        NULL);

    colour_rc = XtVaCreateWidget(
        "colour",
        xmRowColumnWidgetClass,
        frame,
        XmNnumColumns, 2,
        XmNorientation, XmVERTICAL,
        XmNpacking, XmPACK_COLUMN,
        XmNradioBehavior, True,
        NULL);

    weight_frame = XtVaCreateWidget(
        "weightFrame",
        xmFrameWidgetClass,
        style_form,
        XmNtopAttachment, XmATTACH_FORM,
        XmNleftAttachment, XmATTACH_FORM,
        XmNrightAttachment, XmATTACH_FORM,
        NULL);

    weight_form = XtVaCreateWidget(
        "weightForm",
        xmFormWidgetClass,
        weight_frame,
        NULL);

    bold_toggle = XtVaCreateManagedWidget(
        "boldToggle",
        xmToggleButtonGadgetClass,
        weight_form,
        XmNtopAttachment, XmATTACH_FORM,
        XmNleftAttachment, XmATTACH_FORM,
        NULL);

    italic_toggle = XtVaCreateManagedWidget(
        "italicToggle",
        xmToggleButtonGadgetClass,
        weight_form,
        XmNtopAttachment, XmATTACH_FORM,
        XmNleftAttachment, XmATTACH_WIDGET,
        XmNleftWidget, bold_toggle,
        NULL);

    underline_toggle = XtVaCreateManagedWidget(
        "underlineToggle",
        xmToggleButtonGadgetClass,
        weight_form,
        XmNtopAttachment, XmATTACH_FORM,
        XmNleftAttachment, XmATTACH_WIDGET,
        XmNleftWidget, italic_toggle,
        NULL);

    XtAddCallback(bold_toggle, XmNvalueChangedCallback, style_change_cb, (XtPointer)this);
    XtAddCallback(italic_toggle, XmNvalueChangedCallback, style_change_cb, (XtPointer)this);
    XtAddCallback(underline_toggle, XmNvalueChangedCallback, style_change_cb, (XtPointer)this);

    radio_frame = XtVaCreateWidget(
        "radioFrame",
        xmFrameWidgetClass,
        style_form,
        XmNleftAttachment, XmATTACH_FORM,
        XmNrightAttachment, XmATTACH_FORM,
        XmNtopAttachment, XmATTACH_WIDGET,
        XmNtopWidget, weight_frame,
        NULL);

    align_radio_box = XtVaCreateManagedWidget(
        "alignmentRadio",
        xmRowColumnWidgetClass,
        radio_frame,
        XmNorientation, XmHORIZONTAL,
        XmNpacking, XmPACK_TIGHT,
        XmNradioBehavior, True,
        NULL);

    normal_toggle = XtVaCreateManagedWidget(
        "normal",
        xmToggleButtonGadgetClass, align_radio_box,
        NULL);

    superscript_toggle = XtVaCreateManagedWidget(
        "superscript",
        xmToggleButtonGadgetClass, align_radio_box,
        NULL);

    subscript_toggle = XtVaCreateManagedWidget(
        "subscript",
        xmToggleButtonGadgetClass, align_radio_box,
        NULL);

    XtAddCallback(normal_toggle, XmNvalueChangedCallback, align_change_cb, (XtPointer)this);
    XtAddCallback(superscript_toggle, XmNvalueChangedCallback, align_change_cb, (XtPointer)this);
    XtAddCallback(subscript_toggle, XmNvalueChangedCallback, align_change_cb, (XtPointer)this);

    combo_form = XtVaCreateWidget(
        "comboForm",
        xmRowColumnWidgetClass,
        style_form,
        XmNtopAttachment, XmATTACH_WIDGET,
        XmNtopWidget, radio_frame,
        XmNleftAttachment, XmATTACH_FORM,
        XmNleftWidget, font_list,
        XmNbottomAttachment, XmATTACH_WIDGET,
        XmNbottomWidget, sample_frame,
        XmNorientation, XmVERTICAL,
        XmNpacking, XmPACK_COLUMN,
        NULL);

    label_rc = XtVaCreateWidget(
        "labelRC",
        xmRowColumnWidgetClass,
        style_form,
        XmNtopAttachment, XmATTACH_WIDGET,
        XmNtopWidget, radio_frame,
        XmNleftAttachment, XmATTACH_WIDGET,
        XmNleftWidget, combo_form,
        XmNbottomAttachment, XmATTACH_WIDGET,
        XmNbottomWidget, sample_frame,
        XmNorientation, XmVERTICAL,
        XmNpacking, XmPACK_COLUMN,
        NULL);

    alignment_offset_combo = XmCreateDropDownComboBox(
        combo_form, const_cast<char*>("alignmentOffsetCombo"), NULL, 0);

    alignment_offset_text = XtNameToWidget(alignment_offset_combo, "*Text");
    alignment_offset_list = XtNameToWidget(alignment_offset_combo, "*List");

    XtAddCallback(alignment_offset_text, XmNactivateCallback,
        alignment_offset_return_cb, this);
    XtAddCallback(alignment_offset_combo, XmNselectionCallback,
        alignment_offset_return_cb, this);

    size_combo = XmCreateDropDownComboBox(combo_form, const_cast<char*>("sizeCombo"), NULL, 0);

    size_text = XtNameToWidget(size_combo, "*Text");
    size_list = XtNameToWidget(size_combo, "*List");

    XtAddCallback(size_text, XmNactivateCallback, size_return_cb, this);
    XtAddCallback(size_combo, XmNselectionCallback, size_return_cb, this);

    line_spacing_combo = XmCreateDropDownComboBox(combo_form, const_cast<char*>("lineSpacingCombo"), NULL, 0);

    line_spacing_text = XtNameToWidget(line_spacing_combo, "*Text");
    line_spacing_list = XtNameToWidget(line_spacing_combo, "*List");

    XtAddCallback(line_spacing_text, XmNactivateCallback, spacing_return_cb, this);
    XtAddCallback(line_spacing_combo, XmNselectionCallback, spacing_return_cb, this);

    n = 0;
    XtSetArg(args[n++], XmNcomboBoxType, XmDROP_DOWN_COMBO_BOX);

    char_spacing_combo = XmCreateDropDownComboBox(combo_form, const_cast<char*>("charSpacingCombo"), NULL, 0);

    char_spacing_text = XtNameToWidget(char_spacing_combo, "*Text");
    char_spacing_list = XtNameToWidget(char_spacing_combo, "*List");

    XtAddCallback(char_spacing_text, XmNactivateCallback, spacing_return_cb, this);
    XtAddCallback(char_spacing_combo, XmNselectionCallback, spacing_return_cb, this);

    (void)XtVaCreateManagedWidget(
        "alignmentOffsetLabel",
        xmLabelGadgetClass,
        label_rc,
        NULL);

    (void)XtVaCreateManagedWidget(
        "sizeLabel",
        xmLabelGadgetClass,
        label_rc,
        NULL);

    (void)XtVaCreateManagedWidget(
        "lineSpacingLabel",
        xmLabelGadgetClass,
        label_rc,
        NULL);

    (void)XtVaCreateManagedWidget(
        "charSpacingLabel",
        xmLabelGadgetClass,
        label_rc,
        NULL);

    // colour buttons
    for (i = 0; i < MX_NUM_COLOURS; i++) {
        colour_buttons[i] = XtVaCreateManagedWidget(
            mx_colour_label_names[i],
            xmToggleButtonGadgetClass, colour_rc,
            NULL);

        XtAddCallback(
            colour_buttons[i],
            XmNvalueChangedCallback,
            colour_cb,
            (XtPointer)this);
    }

    // now, the action buttons
    ok_button = XtVaCreateManagedWidget(
        "ok",
        xmPushButtonGadgetClass, action_area,
        XmNshowAsDefault, True,
        XmNdefaultButtonShadowThickness, 1,
        XmNleftAttachment, XmATTACH_POSITION,
        XmNleftPosition, 1,
        XmNrightAttachment, XmATTACH_POSITION,
        XmNrightPosition, 2,
        XmNbottomAttachment, XmATTACH_FORM,
        NULL);

    XtAddCallback(
        ok_button,
        XmNactivateCallback,
        button_cb,
        (XtPointer)this);

    apply_button = NULL;
    if (has_apply) {
        apply_button = XtVaCreateManagedWidget(
            "apply",
            xmPushButtonGadgetClass, action_area,
            XmNshowAsDefault, False,
            XmNdefaultButtonShadowThickness, 1,
            XmNleftAttachment, XmATTACH_POSITION,
            XmNleftPosition, 3,
            XmNrightAttachment, XmATTACH_POSITION,
            XmNrightPosition, 4,
            XmNbottomAttachment, XmATTACH_FORM,
            NULL);

        XtAddCallback(
            apply_button,
            XmNactivateCallback,
            button_cb,
            (XtPointer)this);
    }

    cancel_button = XtVaCreateManagedWidget(
        "cancel",
        xmPushButtonGadgetClass, action_area,
        XmNshowAsDefault, False,
        XmNdefaultButtonShadowThickness, 1,
        XmNleftAttachment, XmATTACH_POSITION,
        XmNleftPosition, has_apply ? 5 : 3,
        XmNrightAttachment, XmATTACH_POSITION,
        XmNrightPosition, has_apply ? 6 : 4,
        XmNbottomAttachment, XmATTACH_FORM,
        NULL);

    XtAddCallback(
        cancel_button,
        XmNactivateCallback,
        button_cb,
        (XtPointer)this);

    fill_list(font_list);
    set_combo_lists();

    XtManageChild(font_list);
    XtManageChild(sample_frame);
    XtManageChild(weight_frame);
    XtManageChild(radio_frame);
    XtManageChild(weight_form);
    XtManageChild(frame);
    XtManageChild(align_radio_box);
    XtManageChild(style_form);
    XtManageChild(label_rc);
    XtManageChild(combo_form);
    XtManageChild(colour_rc);
    XtManageChild(alignment_offset_combo);
    XtManageChild(size_combo);
    XtManageChild(line_spacing_combo);
    XtManageChild(char_spacing_combo);
    XtManageChild(action_area);
    XtManageChild(control_area);
    XtManageChild(pane);

    return;
}

void mx_char_d::fill_list(Widget w)
{
    XmString str[MAX_FONTS];
    mx_font f;
    int i, num_fonts = 0;

    for (auto i : f.get_font_family_names()) {
        str[num_fonts++] = XmStringCreate(const_cast<char *>(i.c_str()), XmFONTLIST_DEFAULT_TAG);
    }

    XtVaSetValues(w, XmNitemCount, num_fonts, XmNitems, str, NULL);

    for (i = 0; i < num_fonts; i++) {
        XmStringFree(str[i]);
    }
}

int mx_char_d::run(mx_char_style* initial_styles[], int num_styles, mx_char_style_mod& result)
{
    int res = 0;
    centre();
    activate_d(initial_styles, num_styles, result);
    res = run_modal();
    deactivate();
    return res;
}

void mx_char_d::activate_d(mx_char_style* initial_styles[], int num_styles, mx_char_style_mod& out_mod)
{
    int i;

    result = &out_mod;
    *base_diffs = *result = mx_char_style_mod();

    if (num_styles < 1) {
        *base_style = mx_char_style();
    } else {
        *base_style = *initial_styles[0];

        for (i = 0; i < num_styles; i++) {
            *base_diffs += mx_char_style_mod(*base_style, *initial_styles[i]);
        }
    }

    set_options(*base_diffs);
    mx_dialog::activate();
    return;
}

void mx_char_d::set_font(char* name)
{
    int err = MX_ERROR_OK;
    bool b = false, it = false;

    mx_font f(err, name, 12, mx_normal);
    MX_ERROR_CHECK(err);

    f.get_font_family()->family_has_B_or_I(b, it);

    result->get_font_mod()->set_family_mod(name);

    XtSetSensitive(bold_toggle, b);
    XtSetSensitive(italic_toggle, it);
    if (!b)
        XmToggleButtonSetState(bold_toggle, False, False);
    if (!it)
        XmToggleButtonSetState(italic_toggle, False, False);

    draw_sample();

    return;
abort:
    global_error_trace->print();
    MX_ERROR_CLEAR(err);
}

void mx_char_d::set_combo_lists()
{
    int alignment_offsets[14] = { 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 16, 24,
        36 };
    int sizes[19] = {
        72, 48, 32, 24, 22, 20, 18, 16, 14, 13, 12, 11, 10, 9, 8, 7, 6,
        5, 4
    };

    int char_spacings[11] = { 0, 1, 2, 5, 10, 50, -1, -2, -5, -10, -50 };
    int line_spacings[12] = { 1, 2, 3, 4, 5, 6, -1, -2, -3, -4, -5, -6 };

    int i;
    char s[30];
    XmString item;

    for (i = 0; i < 14; i++) {
        sprintf(s, "%d %s", alignment_offsets[i], mx_unit_name(mx_points));
        item = XmStringCreateLocalized(s);
        XmListAddItem(alignment_offset_list, item, 0);
        XmStringFree(item);
    }

    for (i = 0; i < 19; i++) {
        sprintf(s, "%d %s", sizes[i], mx_unit_name(mx_points));
        item = XmStringCreateLocalized(s);
        XmListAddItem(size_list, item, 1);
        XmStringFree(item);
    }

    item = XmStringCreateLocalized(const_cast<char*>("Auto"));
    XmListAddItem(line_spacing_list, item, 1);
    XmListAddItem(char_spacing_list, item, 1);
    XmStringFree(item);

    for (i = 0; i < 11; i++) {
        if (char_spacings[i] > 0) {
            sprintf(s, "+%d%%", char_spacings[i]);
        } else {
            sprintf(s, "%d%%", char_spacings[i]);
        }
        item = XmStringCreateLocalized(s);
        XmListAddItem(char_spacing_list, item, 0);
        XmStringFree(item);
    }

    for (i = 0; i < 12; i++) {
        if (line_spacings[i] > 0) {
            sprintf(s, "+%d %s", line_spacings[i], mx_unit_name(mx_points));
        } else {
            sprintf(s, "%d %s", line_spacings[i], mx_unit_name(mx_points));
        }

        item = XmStringCreateLocalized(s);
        XmListAddItem(line_spacing_list, item, 0);
        XmStringFree(item);
    }
}

void mx_char_d::set_alignment_offset()
{
    char* s;
    int i;

    s = XmTextGetString(alignment_offset_text);

    if (strcmp(s, "") == 0) {
        XtFree(s);
        return;
    }
    i = atoi(s);
    XtFree(s);

    if (i >= 1)
        result->set_alignment_offset_mod(i);
}

void mx_char_d::set_size()
{
    char* s;
    int i;

    s = XmTextGetString(size_text);

    if (strcmp(s, "") == 0) {
        XtFree(s);
        return;
    }
    i = atoi(s);
    XtFree(s);

    if (i >= 1)
        result->get_font_mod()->set_size_mod(i);
}

void mx_char_d::set_style()
{
    mx_font_mod* fm = result->get_font_mod();

    if (XmToggleButtonGetValue(italic_toggle) == XmINDETERMINATE) {
        fm->clear_italic_mod();
    } else {
        fm->set_italic_mod(XmToggleButtonGetState(italic_toggle));
    }

    if (XmToggleButtonGetValue(bold_toggle) == XmINDETERMINATE) {
        fm->clear_bold_mod();
    } else {
        fm->set_bold_mod(XmToggleButtonGetState(bold_toggle));
    }

    if (XmToggleButtonGetValue(underline_toggle) == XmINDETERMINATE) {
        result->clear_effects_mod();
    } else {
        result->set_effects_mod(XmToggleButtonGetState(underline_toggle) ? mx_underline : mx_no_effects);
    }
}

void mx_char_d::set_spacing()
{
    char* s;

    s = XmTextGetString(line_spacing_text);
    if ((strcmp(s, "") != 0) && (strcmp(s, "Auto") != 0)) {
        result->set_line_space_mod(atoi(s));
    }
    XtFree(s);

    s = XmTextGetString(char_spacing_text);
    if ((strcmp(s, "") != 0) && (strcmp(s, "Auto") != 0)) {
        result->set_set_width_adjust_mod(atoi(s));
    }
    XtFree(s);
}

void mx_char_d::draw_sample()
{
#define S1 "the quick brown fox jumped over the lazy dogs"
#define S2 "JACKDAWS STOLE MY BIG SPHINX OF QUARTZ"
#define S3 "0123456789 !\"£$%^&*()_-=+#~[]{}@';:,.<>/?\\|"
#define S4 "Can't draw sample for multiple selection"
#define S5 "Unable to load the selected font"

    Dimension width, height;
    Display* display;
    Drawable drawable;

    display = XtDisplay(drawing_area);
    drawable = XtWindow(drawing_area);

    mx_colour c;
    result->get_colour_mod(c);

    auto xft_colour = mx_screen_device::getXftColour(display, c);

    mx_font* f;

    XColor screen_colour, exact_colour;
    float temp;
    const char* sample_strings[] = { S1, S2, S3 };
    const char* s;

    int err = MX_ERROR_OK;

    int i, n, str_offset, y = 0;

    XftFont *xft_fs;

    auto draw = XftDrawCreate(display, drawable, DefaultVisual(display, DefaultScreen(display)), DefaultColormap(display, DefaultScreen(display)));

    XtVaGetValues(
        drawing_area,
        XmNwidth, &width,
        XmNheight, &height,
        NULL);

    err = MX_ERROR_OK;

    *base_style += *result;
    base_diffs->clear_mods_in(*result);

    if (!base_diffs->is_null()) {
        mx_font* font;

        font = new mx_font(err, "", 14, mx_normal);
        MX_ERROR_CHECK(err);

        xft_fs = font->get_xft_font(err, display, 1.0);
        MX_ERROR_CHECK(err);

        XSetForeground(display, gc, BlackPixel(display, 0));

        XClearWindow(display, drawable);

        i = mx_font::get_xft_width(xft_fs, display, S4, strlen(S4));
        i = (width - i) / 2;

        XftDrawStringUtf8(draw, &xft_colour, xft_fs, i, (height / 2) - 10, (const FcChar8 *)S4, strlen(S4));

        return;
    }

    // Try by colour name
    if (base_style->colour.name != NULL) {
        if (XLookupColor(display,
                DefaultColormap(display, DefaultScreen(display)),
                base_style->colour.name, &screen_colour, &exact_colour)) {
            if (XAllocNamedColor(display,
                    DefaultColormap(display, DefaultScreen(display)),
                    base_style->colour.name, &screen_colour, &exact_colour)) {
                XSetForeground(display, gc, exact_colour.pixel);
            }
        }
    }

    XClearWindow(display, drawable);

    f = base_style->get_font();
    xft_fs = f->get_xft_font(err, display, 1.0);
    MX_ERROR_CHECK(err);

    if (xft_fs == NULL) {
        mx_font f(err, "", 14, mx_normal);
        MX_ERROR_CHECK(err);

        xft_fs = f.get_xft_font(err, display, 1.0);

        XSetForeground(display, gc, BlackPixel(display, 0));

        XClearWindow(display, drawable);

        i = mx_font::get_xft_width(xft_fs, display, S5, strlen(S5));
        i = (width - i) / 2;

        XftDrawStringUtf8(draw, &xft_colour, xft_fs, i, (height / 2) - 10, (const FcChar8 *)S5, strlen(S5));

        return;
    }

    y += xft_fs->ascent + xft_fs->descent + 4;

    for (n = 0; n < 3; n++) {
        str_offset = 10;
        i = 0;
        s = sample_strings[n];

        while (s[i] != 0) {
            XftDrawStringUtf8(draw, &xft_colour, xft_fs, str_offset, y, (const FcChar8*)(s + i), 1);
            temp = mx_font::get_xft_width(xft_fs, display, s + i, 1);
            temp *= base_style->set_width_adjust;
            temp /= 100;
            str_offset += (int)temp + mx_font::get_xft_width(xft_fs, display, s + i, 1);
            i++;
        }
        if (base_style->effects == mx_underline) {
            XDrawLine(display, drawable, gc, 10, y, str_offset, y);
        }

        y += xft_fs->ascent + xft_fs->descent + (int)base_style->line_space;
    }

    return;

abort:
    global_error_trace->print();
    MX_ERROR_CLEAR(err);
}

void mx_char_d::set_colour()
{
    int i;
    mx_colour c;

    for (i = 0; i < MX_NUM_COLOURS; i++) {
        if (XmToggleButtonGetState(colour_buttons[i])) {
            c = mx_colour(mx_colour_names[i]);
            result->set_colour_mod(c);
            return;
        }
    }
}

void mx_char_d::set_options(mx_char_style_mod& diffs)
{
    int i;
    mx_font_mod& font_diffs = *diffs.get_font_mod();
    int8 new_set_width_adjust;
    mx_colour new_colour;
    mx_char_effects_t new_effects;
    mx_align_t new_alignment;
    float new_offset;
    float new_line_space;
    std::string new_family;
    float new_size;
    bool bold_on;
    bool italic_on;

    char temp[40];

    XmString str;

    if (font_diffs.get_family_mod(new_family)) {
        XmListDeselectAllItems(font_list);
    } else {
        str = XmStringCreate((char*)base_style->get_font()->get_family().c_str(), XmFONTLIST_DEFAULT_TAG);
        XmListSelectItem(font_list, str, False);
        XmStringFree(str);
    }

    (void)base_style->get_font()->get_style();

    XmToggleButtonSetState(bold_toggle, False, False);
    XmToggleButtonSetState(italic_toggle, False, False);

    if (font_diffs.get_bold_mod(bold_on)) {
        XtVaSetValues(bold_toggle, XmNtoggleMode, XmTOGGLE_INDETERMINATE,
            NULL);
        XmToggleButtonSetState(bold_toggle, XmINDETERMINATE, False);
    } else {
        XtVaSetValues(bold_toggle, XmNtoggleMode, XmTOGGLE_BOOLEAN, NULL);
        XmToggleButtonSetState(bold_toggle, base_style->get_font()->get_bold(),
            False);
    }

    if (font_diffs.get_italic_mod(italic_on)) {
        XtVaSetValues(italic_toggle, XmNtoggleMode, XmTOGGLE_INDETERMINATE,
            NULL);
        XmToggleButtonSetState(italic_toggle, XmINDETERMINATE, False);
    } else {
        XtVaSetValues(italic_toggle, XmNtoggleMode, XmTOGGLE_BOOLEAN, NULL);
        XmToggleButtonSetState(italic_toggle,
            base_style->get_font()->get_italic(), False);
    }

    if (diffs.get_effects_mod(new_effects)) {
        XtVaSetValues(underline_toggle, XmNtoggleMode, XmTOGGLE_INDETERMINATE,
            NULL);
        XmToggleButtonSetState(underline_toggle, XmINDETERMINATE, False);
    } else {
        XtVaSetValues(underline_toggle, XmNtoggleMode, XmTOGGLE_BOOLEAN, NULL);
        XmToggleButtonSetState(underline_toggle,
            (base_style->effects == mx_underline),
            False);
    }

    if (font_diffs.get_family_mod(new_family)) {
        XtSetSensitive(bold_toggle, TRUE);
        XtSetSensitive(italic_toggle, TRUE);
    } else {
        bool b = false, it = false;
        base_style->get_font()->get_font_family()->family_has_B_or_I(b, it);

        XtSetSensitive(bold_toggle, b);
        if (!b)
            XmToggleButtonSetState(bold_toggle, False, False);

        XtSetSensitive(italic_toggle, it);
        if (!it)
            XmToggleButtonSetState(bold_toggle, False, False);
    }

    if (font_diffs.get_size_mod(new_size)) {
        XmTextSetString(size_text, const_cast<char*>(""));
    } else {
        i = (int)(0.5f + base_style->get_font()->get_size());

        sprintf(temp, "%d %s", i, mx_unit_name(mx_points));
        XmTextSetString(size_text, temp);
    }

    if (diffs.get_set_width_adjust_mod(new_set_width_adjust)) {
        XmTextSetString(char_spacing_text, const_cast<char*>(""));
    } else if (base_style->set_width_adjust == 0) {
        XmTextSetString(char_spacing_text, const_cast<char*>("Auto"));
    } else {
        if (base_style->set_width_adjust > 0) {
            sprintf(temp, "+%d%%", base_style->set_width_adjust);
        } else {
            sprintf(temp, "%d%%", base_style->set_width_adjust);
        }
        XmTextSetString(char_spacing_text, temp);
    }

    if (diffs.get_line_space_mod(new_line_space)) {
        XmTextSetString(line_spacing_text, const_cast<char*>(""));
    } else if (MX_FLOAT_EQ(base_style->line_space, 0.0f)) {
        XmTextSetString(line_spacing_text, const_cast<char*>("Auto"));
    } else {
        if (base_style->line_space > 0) {
            sprintf(temp, "+%.2f %s", base_style->line_space, mx_unit_name(mx_points));
        } else {
            sprintf(temp, "%.2f %s", base_style->line_space, mx_unit_name(mx_points));
        }
        XmTextSetString(line_spacing_text, temp);
    }

    if (diffs.get_colour_mod(new_colour) || base_style->colour.name == NULL) {
        XtVaSetValues(colour_rc, XmNradioAlwaysOne, False, NULL);
        for (i = 0; i < MX_NUM_COLOURS; i++) {
            XmToggleButtonSetState(colour_buttons[i], False, False);
        }
    } else {
        XtVaSetValues(colour_rc, XmNradioAlwaysOne, True, NULL);
        for (i = 0; i < MX_NUM_COLOURS; i++) {
            if (strcmp(base_style->colour.name, mx_colour_names[i]) == 0) {
                XmToggleButtonSetState(colour_buttons[i], True, False);
                break;
            }
        }
    }

    if (diffs.get_alignment_mod(new_alignment)) {
        XtVaSetValues(align_radio_box, XmNradioAlwaysOne, False, NULL);
        XmToggleButtonSetState(superscript_toggle, False, False);
        XmToggleButtonSetState(subscript_toggle, False, False);
        XmToggleButtonSetState(normal_toggle, False, False);
        XtSetSensitive(XtParent(alignment_offset_text), TRUE);
    } else {
        XtVaSetValues(align_radio_box, XmNradioAlwaysOne, True, NULL);
        switch (base_style->alignment) {
        case mx_superscript:
            XmToggleButtonSetState(superscript_toggle, True, False);
            XtSetSensitive(XtParent(alignment_offset_text), TRUE);
            break;
        case mx_subscript:
            XmToggleButtonSetState(subscript_toggle, True, False);
            XtSetSensitive(XtParent(alignment_offset_text), TRUE);
            break;
        default:
        case mx_normal_align:
            XmToggleButtonSetState(normal_toggle, True, False);
            XtSetSensitive(XtParent(alignment_offset_text), FALSE);
            break;
        }
    }

    if (diffs.get_alignment_offset_mod(new_offset)) {
        XmTextSetString(alignment_offset_text, const_cast<char*>(""));
    } else {
        sprintf(temp, "%.2f %s", base_style->alignment_offset, mx_unit_name(mx_points));
        XmTextSetString(alignment_offset_text, temp);
    }
}

void mx_char_d::set_align()
{
    if (XmToggleButtonGetState(superscript_toggle)) {
        result->set_alignment_mod(mx_superscript);
        XtSetSensitive(XtParent(alignment_offset_text), TRUE);
    } else if (XmToggleButtonGetState(subscript_toggle)) {
        result->set_alignment_mod(mx_subscript);
        XtSetSensitive(XtParent(alignment_offset_text), TRUE);
    } else if (XmToggleButtonGetState(normal_toggle)) {
        result->set_alignment_mod(mx_normal_align);
        XtSetSensitive(XtParent(alignment_offset_text), FALSE);
    }
}
