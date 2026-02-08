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
 * MODULE/CLASS : mx_tab_d
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
#include "mx_yes_no_d.h"
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

#include "mx_tab_d.h"
#include <mx_list_iter.h>

static void position_return(
    Widget widget,
    XtPointer client_data,
    XtPointer call_data)
{
    mx_tab_d* d = (mx_tab_d*)client_data;
    char* str = XmTextGetString(widget);
    float f = atof(str);
    XtFree(str);
    f = mx_unit_to_mm(f, d->units);
    d->update_toggles(f);
}

static void button_cb(
    Widget widget,
    XtPointer client_data,
    XtPointer call_data)
{
    mx_tab_d* d = (mx_tab_d*)client_data;

    if (widget == d->ok_button) {
        d->figure_options();
        d->modal_result = yes_e;
    } else {
        if (widget == d->apply_button) {
            d->figure_options();
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

static void toggle_cb(
    Widget widget,
    XtPointer client_data,
    XtPointer call_data)
{
    mx_tab_d* d = (mx_tab_d*)client_data;

    d->handle_toggle(widget);
}

static void tab_toggle_cb(
    Widget widget,
    XtPointer client_data,
    XtPointer call_data)
{
    mx_tab_d* d = (mx_tab_d*)client_data;

    d->handle_tab_toggle(widget);
}

void mx_tab_d::handle_toggle(Widget w)
{
    if (XmToggleButtonGetState(w)) {
        if (w == user_space_toggle) {
            XtVaSetValues(tab_type_rc, XmNsensitive, True, NULL);
            XtVaSetValues(tab_lead_type_rc, XmNsensitive, True, NULL);
            XtVaSetValues(default_space_text, XmNsensitive, False, NULL);
            XtVaSetValues(position_text, XmNsensitive, True, NULL);
            XtVaSetValues(tab_list, XmNsensitive, True, NULL);

            update_toggles_and_position();
        } else if (w == default_space_toggle) {
            /*          code which only allows mutually exclusive editing of user OR
                        default tabs
                        if (ruler.get_num_user_tabs() != 0)
                        {
                        if (confirm_d == NULL)
                        {
                        confirm_d = new mx_yes_no_d("loseTabs", dialog, FALSE, yes_e);
                        }

                        if (confirm_d->run() != yes_e)
                        {
                        XmToggleButtonSetState(user_space_toggle, True, True);
                        return;
                        }
                        }

                        ruler.clear_user_tabs();
                        XmListDeleteAllItems(tab_list);
                        */
            XtVaSetValues(add_button, XmNsensitive, False, NULL);
            XtVaSetValues(delete_button, XmNsensitive, False, NULL);
            XtVaSetValues(tab_type_rc, XmNsensitive, False, NULL);
            XtVaSetValues(tab_lead_type_rc, XmNsensitive, False, NULL);
            XtVaSetValues(default_space_text, XmNsensitive, True, NULL);
            XtVaSetValues(position_text, XmNsensitive, False, NULL);
            XtVaSetValues(tab_list, XmNsensitive, False, NULL);
        }
    }
}

void mx_tab_d::handle_button(Widget w)
{
    if (w == add_button) {
        add_tab();
    } else if (w == delete_button) {
        delete_tab();
    }
}

static void select_tab(Widget list, XtPointer client_data, XtPointer call_data)
{
    mx_tab_d* d = (mx_tab_d*)client_data;
    int num_sels = 0;

    XtVaGetValues(list, XmNselectedPositionCount, &num_sels, NULL);

    if (num_sels == 1) {
        unsigned int* selected_pos = NULL;
        XtVaGetValues(list, XmNselectedPositions, &selected_pos, NULL);

        selected_pos[0]--;

        // always update all the buttons with the tab at position f
        d->update_toggles_and_position((uint32)selected_pos[0]);
    } else {
        d->update_toggles_and_position((uint32)0);
    }
}

mx_tab_d::mx_tab_d(Widget parent, bool has_apply)
    : mx_dialog("tab", parent, TRUE, FALSE)
{
    Widget label1, label3, label4, tab_form1, tab_form2, tab_form3,
        tab_frame1, tab_frame2;

    confirm_d = NULL;
    XtVaSetValues(action_area, XmNfractionBase, has_apply ? 7 : 5, NULL);

    tab_list = XmCreateScrolledList(control_area, const_cast<char*>("tabList"), NULL, 0);

    XtVaSetValues(tab_list,
        XmNselectionPolicy, XmBROWSE_SELECT,
        NULL);

    XtAddCallback(tab_list, XmNdefaultActionCallback, select_tab, this);
    XtAddCallback(tab_list, XmNbrowseSelectionCallback, select_tab, this);

    type_rc = XtVaCreateWidget(
        "type",
        xmRowColumnWidgetClass,
        control_area,
        XmNorientation, XmHORIZONTAL,
        XmNpacking, XmPACK_COLUMN,
        XmNradioBehavior, True,
        XmNleftAttachment, XmATTACH_WIDGET,
        XmNleftWidget, XtParent(tab_list),
        XmNtopAttachment, XmATTACH_FORM,
        NULL);

    user_space_toggle = XtVaCreateManagedWidget(
        "userSpaceToggle",
        xmToggleButtonGadgetClass, type_rc,
        NULL);

    default_space_toggle = XtVaCreateManagedWidget(
        "defaultSpaceToggle",
        xmToggleButtonGadgetClass, type_rc,
        NULL);

    XtAddCallback(user_space_toggle, XmNvalueChangedCallback, toggle_cb, (XtPointer)this);
    XtAddCallback(default_space_toggle, XmNvalueChangedCallback, toggle_cb, (XtPointer)this);

    label1 = XtVaCreateManagedWidget(
        "defaultSpacingLabel",
        xmLabelGadgetClass,
        control_area,
        XmNtopAttachment, XmATTACH_WIDGET,
        XmNtopWidget, type_rc,
        XmNleftAttachment, XmATTACH_WIDGET,
        XmNleftWidget, XtParent(tab_list),
        NULL);

    default_space_text = XtVaCreateManagedWidget("spaceText",
        xmTextWidgetClass, control_area,
        XmNtopAttachment, XmATTACH_WIDGET,
        XmNtopWidget, type_rc,
        XmNleftAttachment, XmATTACH_WIDGET,
        XmNleftWidget, label1,
        NULL);

    tab_form3 = XtVaCreateWidget("tabform3",
        xmFormWidgetClass, control_area,
        XmNleftAttachment, XmATTACH_WIDGET,
        XmNleftWidget, XtParent(tab_list),
        XmNtopAttachment, XmATTACH_WIDGET,
        XmNtopWidget, default_space_text,
        XmNrightAttachment, XmATTACH_FORM,
        NULL);

    tab_frame1 = XtVaCreateWidget("tabframe1",
        xmFrameWidgetClass, tab_form3,
        XmNleftAttachment, XmATTACH_FORM,
        XmNtopAttachment, XmATTACH_FORM,
        XmNbottomAttachment, XmATTACH_FORM,
        NULL);

    tab_form1 = XtVaCreateWidget("tabform1", xmFormWidgetClass, tab_frame1,
        NULL);

    tab_frame2 = XtVaCreateWidget("tabframe2",
        xmFrameWidgetClass, tab_form3,
        XmNleftAttachment, XmATTACH_WIDGET,
        XmNleftWidget, tab_frame1,
        XmNtopAttachment, XmATTACH_FORM,
        XmNbottomAttachment, XmATTACH_FORM,
        XmNrightAttachment, XmATTACH_FORM,
        NULL);

    tab_form2 = XtVaCreateWidget("tabform2", xmFormWidgetClass, tab_frame2,
        NULL);

    label3 = XtVaCreateManagedWidget("justificationTypeLabel",
        xmLabelGadgetClass,
        tab_form1,
        XmNtopAttachment, XmATTACH_FORM,
        XmNleftAttachment, XmATTACH_FORM,
        XmNrightAttachment, XmATTACH_FORM,
        NULL);

    tab_type_rc = XtVaCreateWidget("tabType",
        xmRowColumnWidgetClass,
        tab_form1,
        XmNorientation, XmVERTICAL,
        XmNpacking, XmPACK_COLUMN,
        XmNradioBehavior, True,
        XmNleftAttachment, XmATTACH_FORM,
        XmNbottomAttachment, XmATTACH_FORM,
        XmNtopAttachment, XmATTACH_WIDGET,
        XmNtopWidget, label3,
        XmNrightAttachment, XmATTACH_FORM,
        NULL);

    left_toggle = XtVaCreateManagedWidget(
        "leftToggle",
        xmToggleButtonGadgetClass, tab_type_rc,
        NULL);

    right_toggle = XtVaCreateManagedWidget(
        "rightToggle",
        xmToggleButtonGadgetClass, tab_type_rc,
        NULL);

    centre_toggle = XtVaCreateManagedWidget(
        "centreToggle",
        xmToggleButtonGadgetClass, tab_type_rc,
        NULL);

    dp_toggle = XtVaCreateManagedWidget(
        "dpToggle",
        xmToggleButtonGadgetClass, tab_type_rc,
        NULL);

    XtAddCallback(left_toggle, XmNvalueChangedCallback, tab_toggle_cb, (XtPointer)this);
    XtAddCallback(right_toggle, XmNvalueChangedCallback, tab_toggle_cb, (XtPointer)this);
    XtAddCallback(centre_toggle, XmNvalueChangedCallback, tab_toggle_cb, (XtPointer)this);
    XtAddCallback(dp_toggle, XmNvalueChangedCallback, tab_toggle_cb, (XtPointer)this);

    label4 = XtVaCreateManagedWidget("tabLeadTypeLabel",
        xmLabelGadgetClass,
        tab_form2,
        XmNtopAttachment, XmATTACH_FORM,
        XmNleftAttachment, XmATTACH_FORM,
        XmNrightAttachment, XmATTACH_FORM,
        NULL);

    tab_lead_type_rc = XtVaCreateWidget("tabLeadType",
        xmRowColumnWidgetClass,
        tab_form2,
        XmNorientation, XmVERTICAL,
        XmNpacking, XmPACK_COLUMN,
        XmNradioBehavior, True,
        XmNleftAttachment, XmATTACH_FORM,
        XmNbottomAttachment, XmATTACH_FORM,
        XmNtopAttachment, XmATTACH_WIDGET,
        XmNtopWidget, label4,
        XmNrightAttachment, XmATTACH_FORM,
        NULL);

    no_lead_toggle = XtVaCreateManagedWidget(
        "noTabLeadToggle",
        xmToggleButtonGadgetClass, tab_lead_type_rc,
        NULL);

    dot_lead_toggle = XtVaCreateManagedWidget(
        "dotTabLeadToggle",
        xmToggleButtonGadgetClass, tab_lead_type_rc,
        NULL);

    ul_lead_toggle = XtVaCreateManagedWidget(
        "ulTabLeadToggle",
        xmToggleButtonGadgetClass, tab_lead_type_rc,
        NULL);

    eq_lead_toggle = XtVaCreateManagedWidget(
        "eqTabLeadToggle",
        xmToggleButtonGadgetClass, tab_lead_type_rc,
        NULL);

    hyp_lead_toggle = XtVaCreateManagedWidget(
        "hypTabLeadToggle",
        xmToggleButtonGadgetClass, tab_lead_type_rc,
        NULL);

    XtAddCallback(no_lead_toggle, XmNvalueChangedCallback, tab_toggle_cb, (XtPointer)this);
    XtAddCallback(dot_lead_toggle, XmNvalueChangedCallback, tab_toggle_cb, (XtPointer)this);
    XtAddCallback(ul_lead_toggle, XmNvalueChangedCallback, tab_toggle_cb, (XtPointer)this);
    XtAddCallback(eq_lead_toggle, XmNvalueChangedCallback, tab_toggle_cb, (XtPointer)this);
    XtAddCallback(hyp_lead_toggle, XmNvalueChangedCallback, tab_toggle_cb, (XtPointer)this);

    (void)XtVaCreateManagedWidget(
        "positionLabel",
        xmLabelGadgetClass,
        control_area,
        XmNtopAttachment, XmATTACH_WIDGET,
        XmNtopWidget, tab_form3,
        XmNleftAttachment, XmATTACH_WIDGET,
        XmNleftWidget, XtParent(tab_list),
        NULL);

    position_text = XtVaCreateManagedWidget("positionText",
        xmTextWidgetClass, control_area,
        XmNleftAttachment, XmATTACH_WIDGET,
        XmNleftWidget, label1,
        XmNtopAttachment, XmATTACH_WIDGET,
        XmNtopWidget, tab_form3,
        NULL);

    XtAddCallback(position_text, XmNactivateCallback, position_return, this);
    XtAddCallback(position_text, XmNvalueChangedCallback, position_return, this);

    add_button = XtVaCreateManagedWidget(
        "add",
        xmPushButtonGadgetClass, control_area,
        XmNleftAttachment, XmATTACH_FORM,
        XmNbottomAttachment, XmATTACH_FORM,
        NULL);

    delete_button = XtVaCreateManagedWidget(
        "delete",
        xmPushButtonGadgetClass, control_area,
        XmNleftAttachment, XmATTACH_WIDGET,
        XmNleftWidget, add_button,
        XmNbottomAttachment, XmATTACH_FORM,
        NULL);

    XtVaSetValues(XtParent(tab_list),
        XmNtopAttachment, XmATTACH_FORM,
        XmNleftAttachment, XmATTACH_FORM,
        XmNbottomAttachment, XmATTACH_WIDGET,
        XmNbottomWidget, add_button,
        NULL);

    XtAddCallback(
        add_button,
        XmNactivateCallback,
        button_cb,
        (XtPointer)this);

    XtAddCallback(
        delete_button,
        XmNactivateCallback,
        button_cb,
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

    XtManageChild(type_rc);
    XtManageChild(tab_type_rc);
    XtManageChild(tab_lead_type_rc);
    XtManageChild(tab_form2);
    XtManageChild(tab_form1);
    XtManageChild(tab_frame2);
    XtManageChild(tab_frame1);
    XtManageChild(tab_form3);
    XtManageChild(tab_list);
    XtManageChild(action_area);
    XtManageChild(control_area);
    XtManageChild(pane);

    return;
}

void mx_tab_d::activate_d(mx_ruler& r, mx_unit_e u)
{
    ruler = r;
    units = u;

    set_options();

    mx_dialog::activate();
}

void mx_tab_d::set_options()
{
    float f;
    char s[40];

    current_tab = &new_tab;

    f = mx_mm_to_unit(ruler.default_tabstop_space, units);
    sprintf(s, "%.2f %s", f, mx_unit_name(units));
    XmTextSetString(default_space_text, s);

    if (ruler.get_num_user_tabs() == 0) {
        update_toggles_and_position();

        XmToggleButtonSetState(default_space_toggle, True, True);
        XtVaSetValues(add_button, XmNsensitive, False, NULL);
        XtVaSetValues(delete_button, XmNsensitive, False, NULL);
        XtVaSetValues(tab_type_rc, XmNsensitive, False, NULL);
        XtVaSetValues(tab_lead_type_rc, XmNsensitive, False, NULL);
        XtVaSetValues(default_space_text, XmNsensitive, True, NULL);
        XtVaSetValues(position_text, XmNsensitive, False, NULL);
        XtVaSetValues(tab_list, XmNsensitive, False, NULL);
    } else {
        XmToggleButtonSetState(user_space_toggle, True, True);
        XtVaSetValues(tab_type_rc, XmNsensitive, True, NULL);
        XtVaSetValues(tab_lead_type_rc, XmNsensitive, True, NULL);
        XtVaSetValues(default_space_text, XmNsensitive, False, NULL);
        XtVaSetValues(position_text, XmNsensitive, True, NULL);
        XtVaSetValues(tab_list, XmNsensitive, True, NULL);

        update_toggles_and_position((uint32)0);
    }
}

void mx_tab_d::fill_list()
{
    XmString str[50];
    int i, j;
    mx_tabstop* tmp_tab;
    float f;
    char s[40];

    ruler.user_tabs.iterate_start();

    i = 0;
    while (TRUE) {
        tmp_tab = (mx_tabstop*)ruler.user_tabs.iterate_next();

        if (tmp_tab == NULL) {
            break;
        } else {
            if (i == 50) {
                break;
            }
        }

        f = mx_mm_to_unit(tmp_tab->position, units);
        sprintf(s, "%.2f %s - ", f, mx_unit_name(units));
        switch (tmp_tab->type) {
        case mx_left:
            strcat(s, "left");
            break;
        case mx_right:
            strcat(s, "right");
            break;
        case mx_centre:
            strcat(s, "centre");
            break;
        case mx_centre_on_dp:
            strcat(s, "d.p.");
            break;
        default:
            break;
        }

        switch (tmp_tab->leader_type) {
        case mx_leader_space_e:
            break;
        case mx_leader_dot_e:
            strcat(s, " ...");
            break;
        case mx_leader_hyphen_e:
            strcat(s, " --");
            break;
        case mx_leader_underline_e:
        case mx_leader_thickline_e:
            strcat(s, " __");
            break;
        case mx_leader_equals_e:
            strcat(s, " ==");
            break;
        default:
            break;
        }

        str[i] = XmStringCreate(s, XmFONTLIST_DEFAULT_TAG);
        i++;
    }

    XtVaSetValues(tab_list,
        XmNitemCount, i,
        XmNitems, str,
        NULL);

    for (j = 0; j < i; j++) {
        XmStringFree(str[j]);
    }
}

mx_tab_d::~mx_tab_d()
{
    if (confirm_d != NULL) {
        delete confirm_d;
    }
}

void mx_tab_d::add_tab()
{
    int err = MX_ERROR_OK;

    ruler.add_tab(err, new_tab);
    if (err != MX_ERROR_OK) {
        MX_ERROR_CLEAR(err);
    }
    update_toggles_and_position(new_tab.position);
}

void mx_tab_d::delete_tab()
{
    int num_selected = 0;
    int err = MX_ERROR_OK;

    XtVaGetValues(tab_list, XmNselectedPositionCount, &num_selected, NULL);

    if (num_selected == 1) {
        mx_tabstop* t;
        unsigned int* sel_pos = NULL;
        XtVaGetValues(tab_list, XmNselectedPositions, &sel_pos, NULL);

        //        if (sel_pos[0] == 0) sel_pos[0] = ruler.user_tabs.get_num_items();
        sel_pos[0]--;

        t = (mx_tabstop*)ruler.user_tabs.remove(err, sel_pos[0]);
        if (err != MX_ERROR_OK) {
            MX_ERROR_CLEAR(err);
            t = &new_tab;
        } else {
            new_tab = *t;
            delete t;
        }

        current_tab = &new_tab;
        update_toggles_and_position();
    }
}

void mx_tab_d::figure_options()
{
    char* s = XmTextGetString(default_space_text);
    float f = atof(s);
    XtFree(s);
    f = mx_unit_to_mm(f, units);
    if (f >= 1.0) {
        ruler.default_tabstop_space = f;
    }
}

int mx_tab_d::run(mx_ruler& r, mx_unit_e u)
{
    int res = 0;
    centre();
    activate_d(r, u);
    res = run_modal();
    deactivate();
    return res;
}

// returns whether two mm measurements will print the same in a given
// measurement unit
static bool floats_print_equal_to_2dp_in_units(float f1, float f2, mx_unit_e units)
{
    char s1[50];
    char s2[50];

    f1 = mx_mm_to_unit(f1, units);
    f2 = mx_mm_to_unit(f2, units);

    sprintf(s1, "%.2f", f1);
    sprintf(s2, "%.2f", f2);

    return !strcmp(s1, s2);
}

void mx_tab_d::find_tab(float tab_pos)
{
    mx_tabstop* nearest_tab = ruler.get_nearest_tab(tab_pos);

    if (nearest_tab != NULL && floats_print_equal_to_2dp_in_units(tab_pos, nearest_tab->position, units)) {
        current_tab = nearest_tab;
    } else {
        new_tab = *current_tab;
        new_tab.position = tab_pos;
        current_tab = &new_tab;
    }
}

// updates all the toggle and list fields and also the position text widget
void mx_tab_d::update_toggles_and_position(float tab_pos)
{
    if (position_set) {
        position_set = FALSE;
    } else {
        find_tab(tab_pos);
        update_toggles_and_position();
    }
}

void mx_tab_d::update_toggles(float tab_pos)
{
    if (position_set) {
        position_set = FALSE;
    } else {
        find_tab(tab_pos);
        update_toggles();
    }
}

void mx_tab_d::update_toggles_and_position(uint32 list_item_num)
{
    int err = MX_ERROR_OK;
    current_tab = (mx_tabstop*)ruler.user_tabs.get(err, list_item_num);

    if (err != MX_ERROR_OK) {
        MX_ERROR_CLEAR(err);
        current_tab = &new_tab;
    }

    update_toggles_and_position();
}

// updates all the toggle and list fields and also the position text widget
void mx_tab_d::update_toggles_and_position()
{
    float f;
    char s[20];

    update_toggles();

    // setting this text will cause the position_return() callback to be
    // called. We do not want this to result in any change and this boolean
    // ensures that this occurs
    position_set = TRUE;

    f = mx_mm_to_unit(current_tab->position, units);
    sprintf(s, "%.2f", f);
    XmTextSetString(position_text, s);
}

// updates all the fields of the tabs dialogue from a tab position which has
// been retrieved from the list of tabs. tab_pos is in mm.
void mx_tab_d::update_toggles()
{
    fill_list();

    if (current_tab == NULL)
        current_tab = &new_tab;

    if (current_tab == &new_tab) {
        // then we're using a new tab. We can add it, but not delete it
        XtVaSetValues(add_button, XmNsensitive, True, NULL);
        XtVaSetValues(delete_button, XmNsensitive, False, NULL);
    } else {
        // we're using an existing tab, we can delete it, but not add it
        int sel_pos = 0;
        mx_list_iterator iter(ruler.user_tabs);

        XtVaSetValues(add_button, XmNsensitive, False, NULL);
        XtVaSetValues(delete_button, XmNsensitive, True, NULL);

        while (iter.more()) {
            mx_tabstop* tab = (mx_tabstop*)iter.data();

            if (tab == current_tab)
                break;
            sel_pos++;
        }

        if (sel_pos < ruler.user_tabs.get_num_items()) {
            XmListSelectPos(tab_list, sel_pos + 1, False);
        }
    }

    switch (current_tab->type) {
    case mx_right:
        XmToggleButtonSetState(right_toggle, True, False);
        XmToggleButtonSetState(centre_toggle, False, False);
        XmToggleButtonSetState(dp_toggle, False, False);
        XmToggleButtonSetState(left_toggle, False, False);
        break;
    case mx_centre:
        XmToggleButtonSetState(right_toggle, False, False);
        XmToggleButtonSetState(centre_toggle, True, False);
        XmToggleButtonSetState(dp_toggle, False, False);
        XmToggleButtonSetState(left_toggle, False, False);
        break;
    case mx_centre_on_dp:
        XmToggleButtonSetState(right_toggle, False, False);
        XmToggleButtonSetState(centre_toggle, False, False);
        XmToggleButtonSetState(dp_toggle, True, False);
        XmToggleButtonSetState(left_toggle, False, False);
        break;
    default:
        XmToggleButtonSetState(right_toggle, False, False);
        XmToggleButtonSetState(centre_toggle, False, False);
        XmToggleButtonSetState(dp_toggle, False, False);
        XmToggleButtonSetState(left_toggle, True, False);
        break;
    }

    switch (current_tab->leader_type) {
    case mx_leader_dot_e:
        XmToggleButtonSetState(dot_lead_toggle, True, False);
        XmToggleButtonSetState(ul_lead_toggle, False, False);
        XmToggleButtonSetState(eq_lead_toggle, False, False);
        XmToggleButtonSetState(hyp_lead_toggle, False, False);
        XmToggleButtonSetState(no_lead_toggle, False, False);
        break;
    case mx_leader_underline_e:
    case mx_leader_thickline_e:
        XmToggleButtonSetState(dot_lead_toggle, False, False);
        XmToggleButtonSetState(ul_lead_toggle, True, False);
        XmToggleButtonSetState(eq_lead_toggle, False, False);
        XmToggleButtonSetState(hyp_lead_toggle, False, False);
        XmToggleButtonSetState(no_lead_toggle, False, False);
        break;
    case mx_leader_equals_e:
        XmToggleButtonSetState(dot_lead_toggle, False, False);
        XmToggleButtonSetState(ul_lead_toggle, False, False);
        XmToggleButtonSetState(eq_lead_toggle, True, False);
        XmToggleButtonSetState(hyp_lead_toggle, False, False);
        XmToggleButtonSetState(no_lead_toggle, False, False);
        break;
    case mx_leader_hyphen_e:
        XmToggleButtonSetState(dot_lead_toggle, False, False);
        XmToggleButtonSetState(ul_lead_toggle, False, False);
        XmToggleButtonSetState(eq_lead_toggle, False, False);
        XmToggleButtonSetState(hyp_lead_toggle, True, False);
        XmToggleButtonSetState(no_lead_toggle, False, False);
        break;
    case mx_leader_space_e:
    default:
        XmToggleButtonSetState(dot_lead_toggle, False, False);
        XmToggleButtonSetState(ul_lead_toggle, False, False);
        XmToggleButtonSetState(eq_lead_toggle, False, False);
        XmToggleButtonSetState(hyp_lead_toggle, False, False);
        XmToggleButtonSetState(no_lead_toggle, True, False);
        break;
    }
}

void mx_tab_d::handle_tab_toggle(Widget w)
{
    if (XmToggleButtonGetState(w)) {
        if (w == dp_toggle) {
            current_tab->type = mx_centre_on_dp;
        } else if (w == right_toggle) {
            current_tab->type = mx_right;
        } else if (w == centre_toggle) {
            current_tab->type = mx_centre;
        } else if (w == left_toggle) {
            current_tab->type = mx_left;
        } else if (w == dot_lead_toggle) {
            current_tab->leader_type = mx_leader_dot_e;
        } else if (w == ul_lead_toggle) {
            current_tab->leader_type = mx_leader_underline_e;
        } else if (w == eq_lead_toggle) {
            current_tab->leader_type = mx_leader_equals_e;
        } else if (w == hyp_lead_toggle) {
            current_tab->leader_type = mx_leader_hyphen_e;
        } else if (w == no_lead_toggle) {
            current_tab->leader_type = mx_leader_space_e;
        }

        update_toggles_and_position();
    }
}
