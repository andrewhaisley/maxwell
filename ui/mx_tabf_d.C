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
 * MODULE/CLASS : mx_tabf_d
 *
 * AUTHOR : Andrew Haisley
 *
 *
 *
 * DESCRIPTION:
 *
 *  A table paragraph style dialog
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
#include "mx_tab_d.h"
#include "mx_tabf_d.h"

static void text_return(
    Widget widget,
    XtPointer client_data,
    XtPointer call_data)
{
    mx_tabf_d* d = (mx_tabf_d*)client_data;

    d->figure_options();
    d->modal_result = yes_e;
}

static void button_cb(
    Widget widget,
    XtPointer client_data,
    XtPointer call_data)
{
    mx_tabf_d* d = (mx_tabf_d*)client_data;

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

void mx_tabf_d::handle_button(Widget w)
{
}

mx_tabf_d::mx_tabf_d(Widget parent, bool has_apply)
    : mx_dialog("tabf", parent, TRUE, FALSE)
{
    Widget form1;
    Widget form1_labels, form1_controls;
    Widget keep_rc;
    Widget label1, label2, label3, label4, label5;
    Widget frame;

    base_style = new mx_paragraph_style;
    style_mod = new mx_paragraph_style_mod;

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

    (void)XtVaCreateManagedWidget(
        "rightIndentLabel",
        xmLabelGadgetClass,
        form1_labels,
        XmNtopAttachment, XmATTACH_WIDGET,
        XmNtopWidget, label5,
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

    centre_justify_toggle = XtVaCreateManagedWidget(
        "centre",
        xmToggleButtonGadgetClass, justification_rc,
        NULL);

    frame = XtVaCreateManagedWidget(
        "frame2",
        xmFrameWidgetClass,
        form1_controls,
        XmNtopAttachment, XmATTACH_WIDGET,
        XmNtopWidget, frame,
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
        xmTextWidgetClass,
        form1_controls,
        XmNleftAttachment, XmATTACH_FORM,
        XmNtopAttachment, XmATTACH_WIDGET,
        XmNtopWidget, frame,
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

    break_before_toggle = XtVaCreateManagedWidget(
        "breakBefore",
        xmToggleButtonGadgetClass,
        form1_controls,
        XmNtopAttachment, XmATTACH_WIDGET,
        XmNtopWidget, right_indent_text,
        XmNleftAttachment, XmATTACH_FORM,
        NULL);

    // now, the action buttons
    ok_button = XtVaCreateManagedWidget(
        "ok",
        xmPushButtonGadgetClass, action_area,
        XmNshowAsDefault, True,
        XmNdefaultButtonShadowThickness, 1,
        XmNleftAttachment, XmATTACH_FORM,
        XmNtopAttachment, XmATTACH_FORM,
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
            XmNtopAttachment, XmATTACH_FORM,
            XmNleftAttachment, XmATTACH_WIDGET,
            XmNleftWidget, ok_button,
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
        XmNleftAttachment, XmATTACH_WIDGET,
        XmNleftWidget, has_apply ? apply_button : ok_button,
        XmNtopAttachment, XmATTACH_FORM,
        NULL);

    XtAddCallback(
        cancel_button,
        XmNactivateCallback,
        button_cb,
        (XtPointer)this);

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

mx_tabf_d::~mx_tabf_d()
{
    delete base_style;
    delete style_mod;
}

void mx_tabf_d::activate_d(mx_paragraph_style* s[], int s_num_styles, mx_unit_e u,
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

    mx_dialog::activate();
    set_options();
    return;
}

void mx_tabf_d::set_options()
{
    char s[20];
    mx_ruler new_ruler;
    mx_justification_t new_just;
    mx_fill_style new_fs;
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
    } else {
        mx_ruler* base_ruler = base_style->get_ruler();

        f = mx_mm_to_unit(base_ruler->left_indent, units);
        sprintf(s, "%.2f %s", f, mx_unit_name(units));
        XmTextSetString(left_indent_text, s);

        f = mx_mm_to_unit(base_ruler->right_indent, units);
        sprintf(s, "%.2f %s", f, mx_unit_name(units));
        XmTextSetString(right_indent_text, s);
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
}

void mx_tabf_d::figure_options()
{
    float f;
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

    if (ruler_changed) {
        result->set_ruler_mod(ruler);
    }

    if (XmToggleButtonGetValue(break_before_toggle) != XmINDETERMINATE) {
        result->set_page_break_before_mod(
            XmToggleButtonGetState(break_before_toggle));
    }

    *base_style += *result;
    style_mod->clear_mods_in(*result);
}

int mx_tabf_d::run(mx_paragraph_style* s[], int num_styles, mx_unit_e u,
    mx_paragraph_style_mod& out_mod)
{
    int res;

    centre();
    activate_d(s, num_styles, u, out_mod);
    res = run_modal();
    deactivate();

    return res;
}
