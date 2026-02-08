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
 *
 * AUTHOR : Andrew Haisley
 *
 * This file
 *
 * DESCRIPTION:
 *
 *  Implementation for mx_dialog.h
 *
 *
 *
 *
 */

#include <X11/Intrinsic.h>
#include <X11/IntrinsicP.h>
#include <X11/CoreP.h>
#include <X11/Core.h>
#include <X11/Shell.h>
#include <X11/keysymdef.h>
#include <Xm/Xm.h>
#include <Xm/MwmUtil.h>
#include <Xm/Label.h>
#include <Xm/LabelG.h>
#include <Xm/Form.h>
#include <Xm/RowColumn.h>
#include <Xm/PushB.h>
#include <Xm/PushBG.h>
#include <Xm/DialogS.h>
#include <Xm/PanedW.h>
#include <Xm/VendorS.h>
#include <Xm/Separator.h>
#include <Xm/Xm.h>
#include <X11/Shell.h>
#include <X11/cursorfont.h>
#include <Xm/ToggleB.h>

#include "mx_dialog.h"
#include <ctype.h>
#include <mx_ui_object.h>
#include <stdio.h>

mx_dialog::mx_dialog(
    const char* name,
    Widget parent,
    bool do_modal,
    bool resizable)
    : mx_ui_object()
{
    int func, decor;
    Widget sep, label_area;

    m_resizable = resizable;

    // climb up the widget tree to get the application top
    // shell
    active = FALSE;
    dialog_parent = parent;
    while (dialog_parent && !XtIsWMShell(dialog_parent)) {
        dialog_parent = XtParent(dialog_parent);
    }
    parent = dialog_parent;

    if (resizable) {
        func = MWM_FUNC_MOVE | MWM_FUNC_RESIZE | MWM_FUNC_CLOSE;
        decor = MWM_DECOR_BORDER | MWM_DECOR_TITLE | MWM_DECOR_MENU | MWM_DECOR_RESIZEH;
    } else {
        func = MWM_FUNC_MOVE | MWM_FUNC_CLOSE;
        decor = MWM_DECOR_BORDER | MWM_DECOR_TITLE | MWM_DECOR_MENU;
    }

    dialog = XtVaCreatePopupShell(
        const_cast<char*>(name),
        xmDialogShellWidgetClass,
        parent,
        XmNdeleteResponse, XmUNMAP,
        XmNnoResize, !resizable,
        XmNdeleteResponse, XmDO_NOTHING,
        XmNmappedWhenManaged, False,
        XmNmwmDecorations, decor,
        XmNmwmFunctions, func,
        NULL);

    pane = XtVaCreateWidget("pane",
        xmFormWidgetClass,
        dialog,
        NULL);

    control_area = XtVaCreateWidget(
        "control",
        xmFormWidgetClass,
        pane,
        XmNleftAttachment, XmATTACH_FORM,
        XmNtopAttachment, XmATTACH_FORM,
        XmNrightAttachment, XmATTACH_FORM,
        XmNtopOffset, 5,
        XmNleftOffset, 5,
        XmNrightOffset, 5,
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

    label_area = XtVaCreateManagedWidget(
        "label",
        xmFormWidgetClass,
        pane,
        XmNtopOffset, 5,
        XmNleftOffset, 5,
        XmNbottomOffset, 8,
        XmNleftAttachment, XmATTACH_FORM,
        XmNbottomAttachment, XmATTACH_FORM,
        XmNtopAttachment, XmATTACH_WIDGET,
        XmNtopWidget, sep,
        NULL);

    (void)XtVaCreateManagedWidget(
        "maxwellIcon",
        xmLabelGadgetClass,
        label_area,
        XmNleftAttachment, XmATTACH_FORM,
        XmNrightAttachment, XmATTACH_FORM,
        XmNtopAttachment, XmATTACH_FORM,
        XmNbottomAttachment, XmATTACH_FORM,
        NULL);

    action_area = XtVaCreateWidget(
        "action",
        xmFormWidgetClass,
        pane,
        XmNtopOffset, 5,
        XmNbottomOffset, 8,
        XmNleftOffset, 5,
        XmNrightOffset, 5,
        XmNleftAttachment, XmATTACH_WIDGET,
        XmNleftWidget, label_area,
        XmNbottomAttachment, XmATTACH_FORM,
        XmNrightAttachment, XmATTACH_FORM,
        XmNtopAttachment, XmATTACH_WIDGET,
        XmNtopWidget, sep,
        NULL);

    if (do_modal) {
        XtVaSetValues(dialog, XmNdialogStyle,
            XmDIALOG_FULL_APPLICATION_MODAL, NULL);
    } else {
        XtVaSetValues(dialog, XmNdialogStyle,
            XmDIALOG_MODELESS, NULL);
    }

    widget = dialog;
}

void mx_dialog::centre()
{
    Position parent_x, parent_y;
    Position dialog_x, dialog_y;
    Dimension parent_width, parent_height;
    Dimension dialog_width, dialog_height;

    XtVaGetValues(
        dialog_parent,
        XmNx, &parent_x,
        XmNy, &parent_y,
        XmNwidth, &parent_width,
        XmNheight, &parent_height,
        NULL);

    XtVaGetValues(
        dialog,
        XmNwidth, &dialog_width,
        XmNheight, &dialog_height,
        NULL);

    dialog_x = parent_x + (parent_width / 2) - (dialog_width / 2);
    dialog_y = parent_y + (parent_height / 2) - (dialog_height / 2);

    XtVaSetValues(
        dialog,
        XmNx, dialog_x,
        XmNy, dialog_y,
        NULL);

    XRaiseWindow(XtDisplay(dialog), XtWindow(dialog));
}

mx_dialog::~mx_dialog()
{
}

bool mx_dialog::is_active()
{
    return active;
}

void mx_dialog::activate()
{
    active = TRUE;
    XtPopup(dialog, XtGrabExclusive);
    XmUpdateDisplay(dialog);

    Dimension width, height;

    // nasty hack to make modern window managers respect non-resizable dialogs
    if (!m_resizable)
    {
        XtVaGetValues(
                dialog,
                XtNwidth, &width,
                XtNheight, &height,
                NULL);

        XtVaSetValues(
            dialog,
            XtNminWidth,  width,
            XtNmaxWidth,  width,
            XtNminHeight, height,
            XtNmaxHeight, height,
            NULL);
    }
}

void mx_dialog::deactivate()
{
    active = FALSE;
    XtPopdown(dialog);
    XmUpdateDisplay(dialog);
}

int mx_dialog::run_modal()
{
    modal_result = none_e;
    XEvent next_event;

    if (!is_active()) {
        activate();
    }

    while (modal_result == none_e) {
        XtAppNextEvent(global_app_context, &next_event);
        if (next_event.type == KeyPress) {
            if (next_event.xkey.state & Mod1Mask) {
                if (handle_acc(dialog, &next_event)) {
                    continue;
                }
            }
        }
        XtDispatchEvent(&next_event);
    }

    return modal_result;
}

XmToggleButtonState mx_dialog::XmToggleButtonGetValue(Widget w)
{
    if (XmToggleButtonGetState(w) == True) {
        return True;
    } else {
        if (XmToggleButtonGetState(w) == False) {
            return False;
        } else {
            return XmINDETERMINATE;
        }
    }
}

bool mx_dialog::traverse_widget_tree(Widget w, char* mnemonic, XKeyEvent* key_event)
{
    WidgetList child_list;
    int num_children, child;
    KeySym LabelMnemonic;
    char* mnemonic_string;

    if (XtClass(w) == xmPushButtonGadgetClass || XtClass(w) == xmPushButtonWidgetClass) {
        XtVaGetValues(w, XmNmnemonic, &LabelMnemonic, NULL);
        mnemonic_string = XKeysymToString(LabelMnemonic);

        if (mnemonic_string && (strcasecmp(mnemonic_string, mnemonic) == 0)) {
            XmProcessTraversal(w, XmTRAVERSE_CURRENT);

            XtCallCallbacks(w, XmNactivateCallback, this);

            return True;
        } else {
            return False;
        }
    } else {
        if (XtIsComposite(w)) {
            XtVaGetValues(w,
                XmNchildren, &child_list,
                XmNnumChildren, &num_children,
                NULL);

            for (child = 0; child < num_children; child++) {
                if (traverse_widget_tree(child_list[child], mnemonic, key_event)) {
                    return True;
                }
            }
        }
        return False;
    }
}

bool mx_dialog::handle_acc(Widget w, XEvent* event)
{
    static char keybuffer[10];
    int CharCount;
    static XComposeStatus composeStatus;

    CharCount = XLookupString((XKeyEvent*)event,
        keybuffer, sizeof(keybuffer),
        NULL, &composeStatus);
    keybuffer[CharCount] = 0;

    if (CharCount == 1) {
        keybuffer[0] = tolower(keybuffer[0]);
        return traverse_widget_tree(w, keybuffer, (XKeyEvent*)event);
    }

    return False;
}

int mx_dialog::run()
{
    int res;

    centre();
    activate();
    res = run_modal();
    deactivate();

    return res;
}
