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
 * MODULE/CLASS : mx_para_d
 *
 * AUTHOR : Andrew Haisley
 *
 * This file
 *
 * DESCRIPTION:
 *
 *  A paragraph style dialog
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

#include "mx_dialog.h"
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
#include <mx_nlist.h>
#include <mx_std.h>
#include <mx_ui_object.h>

#include "mx_border_d.h"
#include "mx_char_d.h"
#include "mx_para_d.h"
#include "mx_tab_d.h"

static void text_return(
    Widget widget,
    XtPointer client_data,
    XtPointer call_data)
{
    mx_para_d* d = (mx_para_d*)client_data;

    d->figure_options();
    d->modal_result = yes_e;
}

static void button_cb(
    Widget widget,
    XtPointer client_data,
    XtPointer call_data)
{
    mx_para_d* d = (mx_para_d*)client_data;

    if (widget == d->ok_button) {
        d->figure_options();
        d->modal_result = yes_e;
    } else {
        if (widget == d->apply_button) {
            d->figure_options();
            d->set_options();
            d->modal_result = apply_e;
        } else {
            if (widget == d->cancel_button) {
                d->modal_result = cancel_e;
            } else {
                d->handle_button(widget);
            }
        }
    }
}

void mx_para_d::handle_button(Widget w)
{
    int i;

    if (w == character_button) {
        mx_char_style_mod* mod = result->get_char_style_mod();
        mx_char_style** cs = new mx_char_style*[num_styles];

        for (i = 0; i < num_styles; i++) {
            cs[i] = selected_styles[i]->get_char_style();
        }

        if (char_d == NULL) {
            char_d = new mx_char_d(dialog, FALSE);
        }

        if (char_d->run(cs, num_styles, *mod) == cancel_e) {
            *mod = mx_char_style_mod();
        }
        delete[] cs;
    } else if (w == border_button) {
        mx_border_style_mod* mod = result->get_border_style_mod();
        mx_nlist bst;
        bst.append_item(selected_styles[0]->get_border_style());

        if (border_d == NULL) {
            border_d = new mx_border_d(dialog, FALSE);
        }

        if (border_d->run(bst, units, *mod) == cancel_e) {
            *mod = mx_border_style_mod();
        }
    } else if (w == tabs_button) {
        mx_ruler ruler;

        figure_options();

        if (tab_d == NULL) {
            tab_d = new mx_tab_d(dialog, FALSE);
        }

        if (!result->get_ruler_mod(ruler)) {
            ruler = *base_style->get_ruler();
        }

        if (tab_d->run(ruler, units) == yes_e) {
            result->set_ruler_mod(tab_d->ruler);
        }
        figure_options();
        set_options();
    }
}

mx_para_d::mx_para_d(Widget parent, bool has_apply)
    : mx_dialog("paragraph", parent, TRUE, FALSE)
{
    Arg args[20];

    Widget form1, heading_form;
    Widget form1_labels, form1_controls;
    Widget keep_rc;
    Widget label1, label2, label3, label4, label5, label6, label7;
    Widget frame, combo1, combo2;

    int n;

    base_style = new mx_paragraph_style;
    style_mod = new mx_paragraph_style_mod;

    char_d = NULL;
    tab_d = NULL;
    border_d = NULL;
    num_styles = 0;

    XtVaSetValues(action_area, XmNfractionBase, has_apply ? 7 : 5, NULL);

    form1 = XtVaCreateWidget(
        "form1",
        xmFormWidgetClass,
        control_area,
        XmNleftAttachment, XmATTACH_FORM,
        XmNtopAttachment, XmATTACH_FORM,
        XmNbottomAttachment, XmATTACH_FORM,
        NULL);

    form1_labels = XtVaCreateWidget(
        "form1Labels",
        xmFormWidgetClass,
        form1,
        XmNleftAttachment, XmATTACH_FORM,
        XmNtopAttachment, XmATTACH_FORM,
        XmNbottomAttachment, XmATTACH_FORM,
        NULL);

    label1 = XtVaCreateManagedWidget(
        "justifyLabel",
        xmLabelGadgetClass,
        form1_labels,
        XmNtopAttachment, XmATTACH_FORM,
        XmNleftAttachment, XmATTACH_FORM,
        NULL);

    label2 = XtVaCreateManagedWidget(
        "keepLabel",
        xmLabelGadgetClass,
        form1_labels,
        XmNtopAttachment, XmATTACH_WIDGET,
        XmNtopWidget, label1,
        XmNleftAttachment, XmATTACH_FORM,
        NULL);

    label3 = XtVaCreateManagedWidget(
        "spaceBeforeLabel",
        xmLabelGadgetClass,
        form1_labels,
        XmNtopAttachment, XmATTACH_WIDGET,
        XmNtopWidget, label2,
        XmNleftAttachment, XmATTACH_FORM,
        NULL);

    label4 = XtVaCreateManagedWidget(
        "spaceAfterLabel",
        xmLabelGadgetClass,
        form1_labels,
        XmNtopAttachment, XmATTACH_WIDGET,
        XmNtopWidget, label3,
        XmNleftAttachment, XmATTACH_FORM,
        NULL);

    label5 = XtVaCreateManagedWidget(
        "leftIndentLabel",
        xmLabelGadgetClass,
        form1_labels,
        XmNtopAttachment, XmATTACH_WIDGET,
        XmNtopWidget, label4,
        XmNleftAttachment, XmATTACH_FORM,
        NULL);

    label6 = XtVaCreateManagedWidget(
        "rightIndentLabel",
        xmLabelGadgetClass,
        form1_labels,
        XmNtopAttachment, XmATTACH_WIDGET,
        XmNtopWidget, label5,
        XmNleftAttachment, XmATTACH_FORM,
        NULL);

    label7 = XtVaCreateManagedWidget(
        "firstLineIndentLabel",
        xmLabelGadgetClass,
        form1_labels,
        XmNtopAttachment, XmATTACH_WIDGET,
        XmNtopWidget, label6,
        XmNleftAttachment, XmATTACH_FORM,
        NULL);

    (void)XtVaCreateManagedWidget(
        "lineSpacingLabel",
        xmLabelGadgetClass,
        form1_labels,
        XmNtopAttachment, XmATTACH_WIDGET,
        XmNtopWidget, label7,
        XmNleftAttachment, XmATTACH_FORM,
        NULL);

    form1_controls = XtVaCreateWidget(
        "form1Controls",
        xmFormWidgetClass,
        form1,
        XmNleftAttachment, XmATTACH_WIDGET,
        XmNleftWidget, form1_labels,
        XmNtopAttachment, XmATTACH_FORM,
        XmNbottomAttachment, XmATTACH_FORM,
        NULL);

    frame = XtVaCreateManagedWidget(
        "frame1",
        xmFrameWidgetClass,
        form1_controls,
        XmNtopAttachment, XmATTACH_FORM,
        XmNleftAttachment, XmATTACH_FORM,
        XmNrightAttachment, XmATTACH_FORM,
        NULL);

    justification_rc = XtVaCreateWidget(
        "justification",
        xmRowColumnWidgetClass,
        frame,
        XmNorientation, XmHORIZONTAL,
        XmNpacking, XmPACK_TIGHT,
        XmNradioBehavior, True,
        NULL);

    left_justify_toggle = XtVaCreateManagedWidget(
        "left",
        xmToggleButtonGadgetClass, justification_rc,
        NULL);

    right_justify_toggle = XtVaCreateManagedWidget(
        "right",
        xmToggleButtonGadgetClass, justification_rc,
        NULL);

    full_justify_toggle = XtVaCreateManagedWidget(
        "full",
        xmToggleButtonGadgetClass, justification_rc,
        NULL);

    centre_justify_toggle = XtVaCreateManagedWidget(
        "centre",
        xmToggleButtonGadgetClass, justification_rc,
        NULL);

    frame = XtVaCreateManagedWidget(
        "frame2",
        xmFrameWidgetClass,
        form1_controls,
        XmNtopAttachment, XmATTACH_WIDGET,
        XmNtopWidget, justification_rc,
        XmNleftAttachment, XmATTACH_FORM,
        XmNrightAttachment, XmATTACH_FORM,
        NULL);

    keep_rc = XtVaCreateWidget(
        "keep",
        xmRowColumnWidgetClass,
        frame,
        XmNorientation, XmHORIZONTAL,
        XmNpacking, XmPACK_TIGHT,
        XmNradioBehavior, False,
        NULL);

    keep_together_toggle = XtVaCreateManagedWidget(
        "together",
        xmToggleButtonGadgetClass, keep_rc,
        NULL);

    keep_with_next_toggle = XtVaCreateManagedWidget(
        "withNext",
        xmToggleButtonGadgetClass, keep_rc,
        NULL);

    space_before_text = XtVaCreateManagedWidget("spaceBeforeText",
        xmTextWidgetClass, form1_controls,
        XmNleftAttachment, XmATTACH_FORM,
        XmNtopAttachment, XmATTACH_WIDGET,
        XmNtopWidget, keep_rc,
        NULL);

    XtAddCallback(
        space_before_text,
        XmNactivateCallback,
        text_return,
        (XtPointer)this);

    space_after_text = XtVaCreateManagedWidget("spaceAfterText",
        xmTextWidgetClass, form1_controls,
        XmNleftAttachment, XmATTACH_FORM,
        XmNtopAttachment, XmATTACH_WIDGET,
        XmNtopWidget, space_before_text,
        NULL);

    XtAddCallback(
        space_after_text,
        XmNactivateCallback,
        text_return,
        (XtPointer)this);

    left_indent_text = XtVaCreateManagedWidget("leftIndentText",
        xmTextWidgetClass, form1_controls,
        XmNleftAttachment, XmATTACH_FORM,
        XmNtopAttachment, XmATTACH_WIDGET,
        XmNtopWidget, space_after_text,
        NULL);

    XtAddCallback(
        left_indent_text,
        XmNactivateCallback,
        text_return,
        (XtPointer)this);

    right_indent_text = XtVaCreateManagedWidget("rightIndentText",
        xmTextWidgetClass, form1_controls,
        XmNleftAttachment, XmATTACH_FORM,
        XmNtopAttachment, XmATTACH_WIDGET,
        XmNtopWidget, left_indent_text,
        NULL);

    XtAddCallback(
        right_indent_text,
        XmNactivateCallback,
        text_return,
        (XtPointer)this);

    first_line_indent_text = XtVaCreateManagedWidget("firstLineIndentText",
        xmTextWidgetClass, form1_controls,
        XmNleftAttachment, XmATTACH_FORM,
        XmNtopAttachment, XmATTACH_WIDGET,
        XmNtopWidget, right_indent_text,
        NULL);

    XtAddCallback(
        first_line_indent_text,
        XmNactivateCallback,
        text_return,
        (XtPointer)this);

    combo1 = XmCreateDropDownComboBox(form1_controls, const_cast<char*>("combo1"), NULL, 0);
    XtVaSetValues(combo1,
        XmNleftAttachment, XmATTACH_FORM,
        XmNtopAttachment, XmATTACH_WIDGET,
        XmNtopWidget, first_line_indent_text,
        NULL);

    line_space_text = XtNameToWidget(combo1, "*Text");
    line_space_list = XtNameToWidget(combo1, "*List");
    fill_space_list(line_space_list);

    XtAddCallback(
        line_space_text,
        XmNactivateCallback,
        text_return,
        (XtPointer)this);

    frame = XtVaCreateManagedWidget("frame3", xmFrameWidgetClass, form1,
        XmNtopAttachment, XmATTACH_FORM,
        XmNleftAttachment, XmATTACH_WIDGET,
        XmNleftWidget, form1_controls,
        XmNrightAttachment, XmATTACH_FORM,
        NULL);

    heading_form = XtVaCreateManagedWidget("headingForm", xmFormWidgetClass,
        frame, NULL);

    heading_toggle = XtVaCreateManagedWidget(
        "heading",
        xmToggleButtonGadgetClass, heading_form,
        XmNleftAttachment, XmATTACH_FORM,
        XmNtopAttachment, XmATTACH_FORM,
        NULL);

    toc_toggle = XtVaCreateManagedWidget(
        "toc",
        xmToggleButtonGadgetClass, heading_form,
        XmNleftAttachment, XmATTACH_FORM,
        XmNtopAttachment, XmATTACH_WIDGET,
        XmNtopWidget, heading_toggle,
        NULL);

    (void)XtVaCreateManagedWidget(
        "levelLabel",
        xmLabelGadgetClass,
        heading_form,
        XmNtopAttachment, XmATTACH_WIDGET,
        XmNtopWidget, toc_toggle,
        XmNleftAttachment, XmATTACH_FORM,
        XmNbottomAttachment, XmATTACH_FORM,
        NULL);

    n = 0;
    XtSetArg(args[n++], XmNcomboBoxType, XmDROP_DOWN_LIST);
    combo2 = XmCreateDropDownComboBox(heading_form, const_cast<char*>("combo"), NULL, 0);
    XtVaSetValues(combo2,
        XmNrightAttachment, XmATTACH_FORM,
        XmNbottomAttachment, XmATTACH_FORM,
        XmNtopAttachment, XmATTACH_WIDGET,
        XmNtopWidget, toc_toggle,
        NULL);

    indent_level_text = XtNameToWidget(combo2, "*Text");
    indent_level_list = XtNameToWidget(combo2, "*List");
    fill_list(indent_level_list);

    XtAddCallback(
        indent_level_text,
        XmNactivateCallback,
        text_return,
        (XtPointer)this);

    break_before_toggle = XtVaCreateManagedWidget(
        "breakBefore",
        xmToggleButtonGadgetClass, form1,
        XmNtopAttachment, XmATTACH_WIDGET,
        XmNtopWidget, frame,
        XmNleftAttachment, XmATTACH_WIDGET,
        XmNleftWidget, form1_controls,
        NULL);

    border_button = XtVaCreateManagedWidget(
        "border",
        xmPushButtonGadgetClass, form1_controls,
        XmNtopAttachment, XmATTACH_WIDGET,
        XmNtopWidget, combo1,
        XmNleftAttachment, XmATTACH_FORM,
        NULL);

    character_button = XtVaCreateManagedWidget(
        "character",
        xmPushButtonGadgetClass, form1_controls,
        XmNtopAttachment, XmATTACH_WIDGET,
        XmNtopWidget, combo1,
        XmNleftAttachment, XmATTACH_WIDGET,
        XmNleftWidget, border_button,
        NULL);

    tabs_button = XtVaCreateManagedWidget(
        "tabs",
        xmPushButtonGadgetClass, form1_controls,
        XmNtopAttachment, XmATTACH_WIDGET,
        XmNtopWidget, combo1,
        XmNleftAttachment, XmATTACH_WIDGET,
        XmNleftWidget, character_button,
        NULL);

    XtAddCallback(border_button, XmNactivateCallback, button_cb,
        (XtPointer)this);
    XtAddCallback(character_button, XmNactivateCallback, button_cb,
        (XtPointer)this);
    XtAddCallback(tabs_button, XmNactivateCallback, button_cb,
        (XtPointer)this);

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
        NULL);

    XtAddCallback(
        cancel_button,
        XmNactivateCallback,
        button_cb,
        (XtPointer)this);

    XtManageChild(combo1);
    XtManageChild(combo2);
    XtManageChild(justification_rc);
    XtManageChild(keep_rc);
    XtManageChild(form1_labels);
    XtManageChild(form1_controls);
    XtManageChild(form1);
    XtManageChild(action_area);
    XtManageChild(control_area);
    XtManageChild(pane);

    return;
}

mx_para_d::~mx_para_d()
{
    delete base_style;
    delete style_mod;
}

void mx_para_d::activate_d(mx_paragraph_style* s[], int s_num_styles, mx_unit_e u,
    mx_paragraph_style_mod& out_mod)
{
    mx_ruler ruler;
    int i;

    result = &out_mod;

    selected_styles = s;
    num_styles = s_num_styles;

    units = u;

    *base_style = (num_styles > 0) ? *selected_styles[0] : mx_paragraph_style();
    *result = *style_mod = mx_paragraph_style_mod();

    for (i = 1; i < num_styles; i++) {
        *style_mod += mx_paragraph_style_mod(*base_style, *selected_styles[i]);
    }

    XtVaSetValues(border_button, XmNsensitive, (num_styles == 1), NULL);
    mx_dialog::activate();
    set_options();
    return;
}

void mx_para_d::fill_list(Widget w)
{
    XmString str[10];
    int i;
    char s[3];

    XtVaSetValues(w, XmNvisibleItemCount, 10, NULL);

    for (i = 0; i < 10; i++) {
        sprintf(s, "%d", i + 1);
        str[i] = XmStringCreate(s, XmFONTLIST_DEFAULT_TAG);
    }

    XtVaSetValues(w,
        XmNitemCount, 10,
        XmNitems, str,
        NULL);

    for (i = 0; i < 10; i++) {
        XmStringFree(str[i]);
    }
}

void mx_para_d::set_options()
{
    char s[20];
    mx_ruler new_ruler;
    mx_justification_t new_just;
    mx_fill_style new_fs;
    uint8 new_hl;
    float f;
    bool b;

    if (style_mod->get_justification_mod(new_just)) {
        XtVaSetValues(justification_rc, XmNradioAlwaysOne, False, NULL);
        XmToggleButtonSetState(left_justify_toggle, False, False);
        XmToggleButtonSetState(right_justify_toggle, False, False);
        XmToggleButtonSetState(full_justify_toggle, False, False);
        XmToggleButtonSetState(centre_justify_toggle, False, False);
    } else {
        XtVaSetValues(justification_rc, XmNradioAlwaysOne, True, NULL);
        switch (base_style->justification) {
        case mx_left_aligned:
            XmToggleButtonSetState(left_justify_toggle, True, False);
            break;
        case mx_right_aligned:
            XmToggleButtonSetState(right_justify_toggle, True, False);
            break;
        case mx_justified:
            XmToggleButtonSetState(full_justify_toggle, True, False);
            break;
        case mx_centred:
            XmToggleButtonSetState(centre_justify_toggle, True, False);
            break;
        }
    }

    if (style_mod->get_keep_lines_together_mod(b)) {
        XtVaSetValues(keep_together_toggle, XmNtoggleMode,
            XmTOGGLE_INDETERMINATE, NULL);
        XmToggleButtonSetState(keep_together_toggle, XmINDETERMINATE, False);
    } else {
        XtVaSetValues(keep_together_toggle, XmNtoggleMode, XmTOGGLE_BOOLEAN,
            NULL);
        XmToggleButtonSetState(keep_together_toggle,
            base_style->keep_lines_together, False);
    }

    if (style_mod->get_keep_with_next_mod(b)) {
        XtVaSetValues(keep_with_next_toggle, XmNtoggleMode,
            XmTOGGLE_INDETERMINATE, NULL);
        XmToggleButtonSetState(keep_with_next_toggle, XmINDETERMINATE, False);
    } else {
        XtVaSetValues(keep_with_next_toggle, XmNtoggleMode, XmTOGGLE_BOOLEAN,
            NULL);
        XmToggleButtonSetState(keep_with_next_toggle,
            base_style->keep_with_next, False);
    }

    if (style_mod->get_space_before_mod(f)) {
        strcpy(s, "");
    } else {
        f = mx_mm_to_unit(base_style->space_before, units);
        sprintf(s, "%.2f %s", f, mx_unit_name(units));
    }
    XmTextSetString(space_before_text, s);

    if (style_mod->get_space_after_mod(f)) {
        strcpy(s, "");
    } else {
        f = mx_mm_to_unit(base_style->space_after, units);
        sprintf(s, "%.2f %s", f, mx_unit_name(units));
    }
    XmTextSetString(space_after_text, s);

    if (style_mod->get_ruler_mod(new_ruler)) {
        strcpy(s, "");
        XmTextSetString(left_indent_text, s);
        XmTextSetString(right_indent_text, s);
        XmTextSetString(first_line_indent_text, s);

        // can only edit tab positions for a single style
        XtVaSetValues(tabs_button, XmNsensitive, False, NULL);
    } else {
        mx_ruler* base_ruler = base_style->get_ruler();

        f = mx_mm_to_unit(base_ruler->left_indent, units);
        sprintf(s, "%.2f %s", f, mx_unit_name(units));
        XmTextSetString(left_indent_text, s);

        f = mx_mm_to_unit(base_ruler->right_indent, units);
        sprintf(s, "%.2f %s", f, mx_unit_name(units));
        XmTextSetString(right_indent_text, s);

        f = mx_mm_to_unit(base_ruler->first_line_indent, units);
        sprintf(s, "%.2f %s", f, mx_unit_name(units));
        XmTextSetString(first_line_indent_text, s);

        XtVaSetValues(tabs_button, XmNsensitive, True, NULL);
    }

    if (style_mod->get_is_heading_mod(b)) {
        XtVaSetValues(heading_toggle, XmNtoggleMode, XmTOGGLE_INDETERMINATE,
            NULL);
        XmToggleButtonSetState(heading_toggle, XmINDETERMINATE, False);
    } else {
        XtVaSetValues(heading_toggle, XmNtoggleMode, XmTOGGLE_BOOLEAN, NULL);
        XmToggleButtonSetState(heading_toggle, base_style->is_heading, False);
    }

    if (style_mod->get_is_toc_mod(b)) {
        XtVaSetValues(toc_toggle, XmNtoggleMode, XmTOGGLE_INDETERMINATE, NULL);
        XmToggleButtonSetState(toc_toggle, XmINDETERMINATE, False);
    } else {
        XtVaSetValues(toc_toggle, XmNtoggleMode, XmTOGGLE_BOOLEAN, NULL);
        XmToggleButtonSetState(toc_toggle, base_style->is_toc, False);
    }

    if (style_mod->get_page_break_before_mod(b)) {
        XtVaSetValues(break_before_toggle, XmNtoggleMode,
            XmTOGGLE_INDETERMINATE, NULL);
        XmToggleButtonSetState(break_before_toggle, XmINDETERMINATE, False);
    } else {
        XtVaSetValues(break_before_toggle, XmNtoggleMode, XmTOGGLE_BOOLEAN,
            NULL);
        XmToggleButtonSetState(break_before_toggle,
            base_style->page_break_before, False);
    }

    if (style_mod->get_heading_level_mod(new_hl)) {
        strcpy(s, "");
    } else {
        sprintf(s, "%d", base_style->heading_level);
    }
    XmTextSetString(indent_level_text, s);

    if (style_mod->get_line_spacing_mod(f)) {
        strcpy(s, "");
    } else {
        sprintf(s, "%.2f times", base_style->line_spacing);
    }
    XmTextSetString(line_space_text, s);
}

void mx_para_d::figure_options()
{
    float f;
    int j;
    char* s;
    mx_ruler ruler;
    bool ruler_changed = FALSE;

    if (!result->get_ruler_mod(ruler)) {
        ruler = *base_style->get_ruler();
    }

    if (XmToggleButtonGetState(left_justify_toggle)) {
        result->set_justification_mod(mx_left_aligned);
    }

    if (XmToggleButtonGetState(right_justify_toggle)) {
        result->set_justification_mod(mx_right_aligned);
    }

    if (XmToggleButtonGetState(full_justify_toggle)) {
        result->set_justification_mod(mx_justified);
    }

    if (XmToggleButtonGetState(centre_justify_toggle)) {
        result->set_justification_mod(mx_centred);
    }

    if (XmToggleButtonGetValue(keep_together_toggle) != XmINDETERMINATE) {
        result->set_keep_lines_together_mod(
            XmToggleButtonGetState(keep_together_toggle));
    }

    if (XmToggleButtonGetValue(keep_with_next_toggle) != XmINDETERMINATE) {
        result->set_keep_with_next_mod(
            XmToggleButtonGetState(keep_with_next_toggle));
    }

    s = XmTextGetString(space_before_text);
    if (strcmp(s, "") != 0) {
        sscanf(s, "%f", &f);
        result->set_space_before_mod(mx_unit_to_mm(f, units));
    }
    XtFree(s);

    s = XmTextGetString(space_after_text);
    if (strcmp(s, "") != 0) {
        sscanf(s, "%f", &f);
        result->set_space_after_mod(mx_unit_to_mm(f, units));
    }
    XtFree(s);

    s = XmTextGetString(left_indent_text);
    if (strcmp(s, "") != 0) {
        sscanf(s, "%f", &f);
        ruler.left_indent = mx_unit_to_mm(f, units);
        ruler_changed = TRUE;
    }
    XtFree(s);

    s = XmTextGetString(right_indent_text);
    if (strcmp(s, "") != 0) {
        sscanf(s, "%f", &f);
        ruler.right_indent = mx_unit_to_mm(f, units);
        ruler_changed = TRUE;
    }
    XtFree(s);

    s = XmTextGetString(first_line_indent_text);
    if (strcmp(s, "") != 0) {
        sscanf(s, "%f", &f);
        ruler.first_line_indent = mx_unit_to_mm(f, units);
        ruler_changed = TRUE;
    }
    XtFree(s);

    if (ruler_changed) {
        result->set_ruler_mod(ruler);
    }

    if (XmToggleButtonGetValue(heading_toggle) != XmINDETERMINATE) {
        result->set_is_heading_mod(XmToggleButtonGetState(heading_toggle));
    }

    if (XmToggleButtonGetValue(toc_toggle) != XmINDETERMINATE) {
        result->set_is_toc_mod(XmToggleButtonGetState(toc_toggle));
    }

    if (XmToggleButtonGetValue(break_before_toggle) != XmINDETERMINATE) {
        result->set_page_break_before_mod(
            XmToggleButtonGetState(break_before_toggle));
    }

    s = XmTextGetString(indent_level_text);
    if (strcmp(s, "") != 0) {
        sscanf(s, "%d", &j);
        result->set_heading_level_mod(j);
    }
    XtFree(s);

    s = XmTextGetString(line_space_text);
    if (strcmp(s, "") != 0) {
        sscanf(s, "%f", &f);
        result->set_line_spacing_mod(f);
    }
    XtFree(s);

    *base_style += *result;
    style_mod->clear_mods_in(*result);
}

void mx_para_d::fill_space_list(Widget w)
{
    XmString str[10];
    int i;
    char s[20];

    XtVaSetValues(w, XmNvisibleItemCount, 4, NULL);

    for (i = 0; i < 4; i++) {
        sprintf(s, "%.2f times", (i + 1) * 1.0);
        str[i] = XmStringCreate(s, XmFONTLIST_DEFAULT_TAG);
    }

    XtVaSetValues(w,
        XmNitemCount, 4,
        XmNitems, str,
        NULL);

    for (i = 0; i < 4; i++) {
        XmStringFree(str[i]);
    }
}

int mx_para_d::run(mx_paragraph_style* s[], int num_styles, mx_unit_e u,
    mx_paragraph_style_mod& out_mod)
{
    int res;

    centre();
    activate_d(s, num_styles, u, out_mod);
    res = run_modal();
    deactivate();

    return res;
}
