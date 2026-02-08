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
 * MODULE/CLASS : mx_hsearch_d
 *
 * AUTHOR : Andrew Haisley
 *
 * This file
 *
 * DESCRIPTION:
 *
 *  A search dialog for the help system.
 *
 *
 *
 */

#include <errno.h>
#include <pwd.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>

#include "mx_dialog.h"
#include <Xm/ComboBox.h>
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
#include <mx_std.h>
#include <mx_ui_object.h>
// #include "mx_yes_no_d.h"
// #include "mx_open_d.h"
#include <mx_file_type.h>

#include "mx_hsearch_d.h"

extern char* global_maxhome;

static void button_cb(
    Widget widget,
    XtPointer client_data,
    XtPointer call_data)
{
    mx_hsearch_d* d = (mx_hsearch_d*)client_data;

    if (widget == d->goto_button) {
        d->add_pattern();
        d->modal_result = yes_e;
    } else {
        if (widget == d->search_button) {
            d->search();
        } else {
            d->modal_result = cancel_e;
        }
    }
}

static void pattern_return(
    Widget widget,
    XtPointer client_data,
    XtPointer call_data)
{
    mx_hsearch_d* d = (mx_hsearch_d*)client_data;

    d->add_pattern();
    d->search();
}

static void pattern_changed(
    Widget widget,
    XtPointer client_data,
    XtPointer call_data)
{
    mx_hsearch_d* d = (mx_hsearch_d*)client_data;
    char* s = XmTextGetString(d->pattern_text);
    if (s[0] == 0) {
        XtVaSetValues(d->search_button, XmNsensitive, False, NULL);
    } else {
        XtVaSetValues(d->search_button, XmNsensitive, True, NULL);
    }
    XtFree(s);
}

static void list_cb(Widget list, XtPointer client_data, XtPointer call_data)
{
    mx_hsearch_d* d = (mx_hsearch_d*)client_data;

    char* choice;

    XmListCallbackStruct* cbs = (XmListCallbackStruct*)call_data;

    XmStringGetLtoR(cbs->item, XmFONTLIST_DEFAULT_TAG, &choice);
    d->set_topic_file(choice);
    XtFree(choice);

    XtVaSetValues(d->goto_button, XmNsensitive, True, NULL);

    if (cbs->reason == XmCR_BROWSE_SELECT) {
        // single clicked on a template
        return;
    }

    if (cbs->reason == XmCR_DEFAULT_ACTION) {
        // double clicked on a template
        d->modal_result = yes_e;
    }
}

mx_hsearch_d::mx_hsearch_d(Widget parent)
    : mx_dialog("hsearch", parent, TRUE, FALSE)
{
    Widget combo, label;

    num_patterns = 0;
    XtVaSetValues(action_area, XmNfractionBase, 5, NULL);

    label = XtVaCreateManagedWidget(
        "findLabel",
        xmLabelGadgetClass,
        control_area,
        XmNtopAttachment, XmATTACH_FORM,
        XmNleftAttachment, XmATTACH_FORM,
        NULL);

    combo = XmCreateDropDownComboBox(control_area, const_cast<char*>("combo"), NULL, 0);
    XtVaSetValues(combo,
        XmNleftAttachment, XmATTACH_WIDGET,
        XmNleftWidget, label,
        XmNtopAttachment, XmATTACH_FORM,
        NULL);

    search_button = XtVaCreateManagedWidget(
        "search",
        xmPushButtonGadgetClass, control_area,
        XmNtopAttachment, XmATTACH_FORM,
        XmNleftAttachment, XmATTACH_WIDGET,
        XmNrightAttachment, XmATTACH_FORM,
        XmNleftWidget, combo,
        NULL);

    XtAddCallback(
        search_button,
        XmNactivateCallback,
        button_cb,
        (XtPointer)this);

    pattern_text = XtNameToWidget(combo, "*Text");
    pattern_list = XtNameToWidget(combo, "*List");

    XtAddCallback(pattern_text, XmNvalueChangedCallback, pattern_changed, this);
    XtAddCallback(pattern_text, XmNactivateCallback, pattern_return, this);

    XtVaSetValues(pattern_text,
        XmNeditable, True,
        XmNcursorPosition, True,
        NULL);

    XtVaSetValues(pattern_list, XmNvisibleItemCount, 0, NULL);

    topic_list = XmCreateScrolledList(control_area, const_cast<char*>("topicList"), NULL, 0);

    XtVaSetValues(topic_list,
        XmNselectionPolicy, XmBROWSE_SELECT,
        NULL);

    XtVaSetValues(XtParent(topic_list),
        XmNtopAttachment, XmATTACH_WIDGET,
        XmNtopWidget, combo,
        XmNleftAttachment, XmATTACH_FORM,
        XmNrightAttachment, XmATTACH_FORM,
        XmNleftOffset, 5,
        XmNbottomOffset, 5,
        XmNrightOffset, 5,
        XmNtopOffset, 5,
        NULL);

    XtAddCallback(topic_list, XmNdefaultActionCallback, list_cb, this);
    XtAddCallback(topic_list, XmNbrowseSelectionCallback, list_cb, this);

    // now, the action buttons
    goto_button = XtVaCreateManagedWidget(
        "goto",
        xmPushButtonGadgetClass, action_area,
        XmNshowAsDefault, True,
        XmNdefaultButtonShadowThickness, 1,
        XmNleftAttachment, XmATTACH_POSITION,
        XmNleftPosition, 1,
        XmNrightAttachment, XmATTACH_POSITION,
        XmNrightPosition, 2,
        XmNsensitive, False,
        NULL);

    XtAddCallback(
        goto_button,
        XmNactivateCallback,
        button_cb,
        (XtPointer)this);

    cancel_button = XtVaCreateManagedWidget(
        "cancel",
        xmPushButtonGadgetClass, action_area,
        XmNshowAsDefault, False,
        XmNdefaultButtonShadowThickness, 1,
        XmNleftAttachment, XmATTACH_POSITION,
        XmNleftPosition, 3,
        XmNrightAttachment, XmATTACH_POSITION,
        XmNrightPosition, 4,
        NULL);

    XtAddCallback(
        cancel_button,
        XmNactivateCallback,
        button_cb,
        (XtPointer)this);

    load_topic_map();

    XtManageChild(combo);
    XtManageChild(topic_list);
    XtManageChild(action_area);
    XtManageChild(control_area);
    XtManageChild(pane);
}

void mx_hsearch_d::load_topic_map()
{
    int i, c;
    FILE* f;
    char file_name[200];
    char line[200];

    num_help_files = 0;
    sprintf(file_name, "%s/help/help.map", global_maxhome);

    f = fopen(file_name, "r");
    if (f == NULL) {
        return;
    }

    c = 0;
    while (c != EOF) {
        line[0] = 0;
        i = 0;
        while (TRUE) {
            c = fgetc(f);
            if (c == EOF || c == '\n') {
                break;
            }
            line[i++] = c;
        }
        line[i] = 0;
        i = 0;
        while (line[i] != 0) {
            if (line[i] == ':') {
                line[i] = 0;
                help_files[num_help_files] = mx_string_copy(line);
                help_topics[num_help_files++] = mx_string_copy(line + i + 1);
                break;
            }
            i++;
        }
    }
    fclose(f);
}

void mx_hsearch_d::add_pattern()
{
    char* s;
    XmString item;

    s = XmTextGetString(pattern_text);
    item = XmStringCreateLocalized(s);
    XtFree(s);

    if (!XmListItemExists(pattern_list, item)) {
        XmListAddItem(pattern_list, item, 1);
        num_patterns++;
        XtVaSetValues(pattern_list,
            XmNvisibleItemCount, num_patterns < 25 ? num_patterns : 25,
            NULL);
    }

    XmStringFree(item);
}

void mx_hsearch_d::search()
{
    char* s;

    char command[300];
    XmString str[MAX_HSEARCH_FILES];
    int i, num_topics = 0, c;
    FILE* f;

    set_busy();

    s = XmTextGetString(pattern_text);
    if (strlen(s) > 150) {
        s[150] = 0;
    }

    for (i = 0; i < num_help_files; i++) {
        sprintf(command, "%s/bin/grep -i \'%s\' %s/help/%s",
            global_maxhome, s, global_maxhome, help_files[i]);

        f = popen(command, "r");
        if (f == NULL) {
            break;
        }
        c = fgetc(f);
        if (c != EOF) {
            str[num_topics++] = XmStringCreate(help_topics[i], XmFONTLIST_DEFAULT_TAG);
        }
        pclose(f);
    }

    XtVaSetValues(topic_list,
        XmNitemCount, num_topics,
        XmNitems, str,
        NULL);

    for (i = 0; i < num_topics; i++) {
        XmStringFree(str[i]);
    }
    clear_busy();
    XtFree(s);
}

void mx_hsearch_d::set_topic_file(char* topic)
{
    int i;

    for (i = 0; i < num_help_files; i++) {
        if (strcmp(help_topics[i], topic) == 0) {
            strcpy(selected_topic, help_files[i]);
            return;
        }
    }
}
