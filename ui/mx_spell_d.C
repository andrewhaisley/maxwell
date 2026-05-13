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
 * MODULE/CLASS : mx_spell_d
 *
 * AUTHOR : Andrew Haisley
 *
 * This file
 *
 * DESCRIPTION:
 *
 *  A spell checker dialog
 *
 *
 *
 *
 */

#include <ctype.h>
#include <dirent.h>
#include <errno.h>
#include <pwd.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>

#include "mx_dialog.h"
#include "mx_inform_d.h"
#include <Xm/Label.h>
#include <Xm/LabelG.h>
#include <Xm/List.h>
#include <Xm/Protocols.h>
#include <Xm/PushBG.h>
#include <Xm/RowColumn.h>
#include <Xm/Separator.h>
#include <Xm/Text.h>
#include <Xm/Xm.h>
#include <mx.h>
#include <mx_file_type.h>
#include <mx_std.h>
#include <mx_ui_object.h>

#include "mx_spell_d.h"

using namespace std;

static void button_cb(Widget widget, XtPointer client_data, XtPointer call_data)
{
    mx_spell_d* d = (mx_spell_d*)client_data;

    if (widget == d->quit_button) {
        d->modal_result = cancel_e;
    } else {
        d->handle_button(widget);
    }
}

static void list_cb(Widget list, XtPointer client_data, XtPointer call_data)
{
    mx_spell_d* d = (mx_spell_d*)client_data;

    char* choice;

    XmListCallbackStruct* cbs = (XmListCallbackStruct*)call_data;

    XmStringGetLtoR(cbs->item, XmFONTLIST_DEFAULT_TAG, &choice);
    d->replacement = choice;
    XtFree(choice);

    if (cbs->reason == XmCR_BROWSE_SELECT) {
        // single clicked on a replacement word
        XmTextSetString(d->replacement_text, const_cast<char *>(d->replacement.c_str()));
        return;
    }

    if (cbs->reason == XmCR_DEFAULT_ACTION) {
        // double clicked on a word
        d->modal_result = replace_e;
    }
}

static void text_return(Widget widget, XtPointer client_data, XtPointer call_data)
{
    mx_spell_d* d = (mx_spell_d*)client_data;

    d->handle_button(d->replace_button);
}

static void language_change(Widget widget, XtPointer client_data, XtPointer call_data)
{
    mx_spell_d* d = (mx_spell_d*)client_data;
    char s[100];

    strcpy(s, XtName(widget));
    d->set_language(s);
}

mx_spell_d::~mx_spell_d()
{
    for (const auto &[name, dict] : m_dictionaries) {
        enchant_broker_free_dict(m_broker, dict);
    }
    enchant_broker_free(m_broker);
}

mx_spell_d::mx_spell_d(Widget parent) : mx_dialog("spell", parent, TRUE, FALSE)
{
    Widget label1, label2, language_sub_menu;
    Arg args[15];
    int n;

    m_broker = enchant_broker_init();

    XtVaSetValues(action_area, XmNfractionBase, 3, NULL);

    list = XmCreateScrolledList(control_area, const_cast<char*>("wordList"), NULL, 0);

    XtVaSetValues(list,
        XmNselectionPolicy, XmBROWSE_SELECT,
        NULL);

    XtVaSetValues(XtParent(list),
        XmNtopAttachment, XmATTACH_FORM,
        XmNleftAttachment, XmATTACH_FORM,
        XmNbottomAttachment, XmATTACH_FORM,
        XmNleftOffset, 5,
        XmNbottomOffset, 5,
        XmNrightOffset, 5,
        XmNtopOffset, 5,
        NULL);

    XtAddCallback(list, XmNdefaultActionCallback, list_cb, this);
    XtAddCallback(list, XmNbrowseSelectionCallback, list_cb, this);

    label1 = XtVaCreateManagedWidget(
        "wordLabel",
        xmLabelGadgetClass,
        control_area,
        XmNtopAttachment, XmATTACH_FORM,
        XmNleftAttachment, XmATTACH_WIDGET,
        XmNleftWidget, list,
        NULL);

    label2 = XtVaCreateManagedWidget(
        "replaceLabel",
        xmLabelGadgetClass,
        control_area,
        XmNtopAttachment, XmATTACH_WIDGET,
        XmNtopWidget, label1,
        XmNleftAttachment, XmATTACH_WIDGET,
        XmNleftWidget, list,
        NULL);

    word_text = XtVaCreateManagedWidget("wordText",
        xmTextWidgetClass, control_area,
        XmNleftAttachment, XmATTACH_WIDGET,
        XmNleftWidget, label2,
        XmNtopAttachment, XmATTACH_FORM,
        XmNeditable, False,
        NULL);

    language_sub_menu = XmCreatePulldownMenu(control_area, const_cast<char*>("languagePulldown"), NULL, 0);

    // language menu
    n = 0;
    XtSetArg(args[n], XmNsubMenuId, language_sub_menu);
    n++;
    XtSetArg(args[n], XmNtopAttachment, XmATTACH_FORM);
    n++;
    XtSetArg(args[n], XmNleftAttachment, XmATTACH_WIDGET);
    n++;
    XtSetArg(args[n], XmNleftWidget, word_text);
    n++;

    language_menu = XmCreateOptionMenu(control_area, const_cast<char*>("languageMenu"), args, n);

    create_language_menu(language_sub_menu);

    replacement_text = XtVaCreateManagedWidget("replacementText",
        xmTextWidgetClass, control_area,
        XmNleftAttachment, XmATTACH_WIDGET,
        XmNleftWidget, label2,
        XmNtopAttachment, XmATTACH_WIDGET,
        XmNtopWidget, word_text,
        NULL);

    XtAddCallback(replacement_text, XmNactivateCallback, text_return, this);

    ignore_button = XtVaCreateManagedWidget(
        "ignore",
        xmPushButtonGadgetClass, control_area,
        XmNshowAsDefault, True,
        XmNdefaultButtonShadowThickness, 1,
        XmNleftAttachment, XmATTACH_WIDGET,
        XmNleftWidget, list,
        XmNtopAttachment, XmATTACH_WIDGET,
        XmNtopWidget, replacement_text,
        NULL);

    ignore_all_button = XtVaCreateManagedWidget(
        "ignoreAll",
        xmPushButtonGadgetClass, control_area,
        XmNleftAttachment, XmATTACH_WIDGET,
        XmNleftWidget, ignore_button,
        XmNtopAttachment, XmATTACH_WIDGET,
        XmNtopWidget, replacement_text,
        NULL);

    replace_button = XtVaCreateManagedWidget(
        "replace",
        xmPushButtonGadgetClass, control_area,
        XmNleftAttachment, XmATTACH_WIDGET,
        XmNleftWidget, ignore_all_button,
        XmNtopAttachment, XmATTACH_WIDGET,
        XmNtopWidget, replacement_text,
        NULL);

    back_button = XtVaCreateManagedWidget(
        "back",
        xmPushButtonGadgetClass, control_area,
        XmNleftAttachment, XmATTACH_WIDGET,
        XmNleftWidget, replace_button,
        XmNtopAttachment, XmATTACH_WIDGET,
        XmNtopWidget, replacement_text,
        XmNsensitive, False,
        NULL);

    add_dict_button = XtVaCreateManagedWidget(
        "addDict",
        xmPushButtonGadgetClass, control_area,
        XmNleftAttachment, XmATTACH_WIDGET,
        XmNleftWidget, back_button,
        XmNtopAttachment, XmATTACH_WIDGET,
        XmNtopWidget, replacement_text,
        NULL);

    XtAddCallback(
        add_dict_button,
        XmNactivateCallback,
        button_cb,
        (XtPointer)this);

    XtAddCallback(
        back_button,
        XmNactivateCallback,
        button_cb,
        (XtPointer)this);

    XtAddCallback(
        replace_button,
        XmNactivateCallback,
        button_cb,
        (XtPointer)this);

    XtAddCallback(
        ignore_all_button,
        XmNactivateCallback,
        button_cb,
        (XtPointer)this);

    XtAddCallback(
        ignore_button,
        XmNactivateCallback,
        button_cb,
        (XtPointer)this);

    // now, the action buttons
    quit_button = XtVaCreateManagedWidget(
        "quit",
        xmPushButtonGadgetClass, action_area,
        XmNshowAsDefault, True,
        XmNdefaultButtonShadowThickness, 1,
        XmNleftAttachment, XmATTACH_POSITION,
        XmNleftPosition, 1,
        XmNrightAttachment, XmATTACH_POSITION,
        XmNrightPosition, 2,
        NULL);

    XtAddCallback(
        quit_button,
        XmNactivateCallback,
        button_cb,
        (XtPointer)this);

    XtManageChild(list);
    XtManageChild(language_menu);
    XtManageChild(action_area);
    XtManageChild(control_area);
    XtManageChild(pane);
}

void mx_spell_d::fill_list()
{
    int n = 0;

    XmString str[100];

    for (const auto &s : m_suggestions) {
        if (n >= 100) {
            break;
        } else {
            str[n++] = XmStringCreate(const_cast<char *>(s.c_str()), XmFONTLIST_DEFAULT_TAG);
        }
    }
    
    XtVaSetValues(list, XmNitemCount, n, XmNitems, str, NULL);

    for (int i = 0; i < n; i++) {
        XmStringFree(str[i]);
    }
}

void mx_spell_d::handle_button(Widget w)
{
    char* s;

    if (w == ignore_button) {
        XtVaSetValues(back_button, XmNsensitive, True, NULL);
        modal_result = ignore_e;
        return;
    }

    if (w == ignore_all_button) {
        XtVaSetValues(back_button, XmNsensitive, False, NULL);
        modal_result = ignore_e;
        return;
    }

    if (w == replace_button) {
        modal_result = replace_e;
        s = XmTextGetString(replacement_text);
        replacement = s;
        XtFree(s);
        XtVaSetValues(back_button, XmNsensitive, False, NULL);
        return;
    }

    if (w == back_button) {
        XtVaSetValues(back_button, XmNsensitive, False, NULL);
        modal_result = back_e;
        return;
    }

    if (w == add_dict_button) {
        enchant_dict_add(m_dictionaries[m_current_language], original_word.c_str(), -1);
        XtVaSetValues(back_button, XmNsensitive, False, NULL);
        modal_result = ignore_e;
        return;
    }
}

int mx_spell_d::run_modal(mx_config *config, const string &word)
{
    m_config = config;
    original_word = word;

    XmTextSetString(word_text, const_cast<char *>(word.c_str()));

    XmListDeleteAllItems(list);
    fill_list();
    replacement = word;

    return mx_dialog::run_modal();
}

void mx_spell_d::activate()
{
    mx_dialog::activate();
}

void mx_spell_d::set_language(const string &language)
{
    m_current_language = language;

    if (m_dictionaries.find(language) == m_dictionaries.end()) {
        auto d = enchant_broker_request_dict(m_broker, language.c_str());
        if (d) {
            m_dictionaries[language] = d;
        } 
    }

    XtVaSetValues(language_menu, XmNmenuHistory, m_language_buttons[m_current_language], NULL);
}

void mx_spell_d::create_language_menu(Widget w)
{
    Widget b;

    for (auto l : mx_language::codes) {

        XmString label = XmStringCreateLocalized(const_cast<char *>(mx_language::get_language_name(l).c_str()));

        b = XtVaCreateManagedWidget(
            l.c_str(),
            xmPushButtonGadgetClass,
            w,
            XmNlabelString, label,
            NULL);

        XmStringFree(label);

        XtAddCallback(b, XmNactivateCallback, language_change, this);

        m_language_buttons[l] = b;
    }
}

bool mx_spell_d::wrong_spelling(mx_config *config, const string &word) 
{
    int err;
    m_config = config;
    m_suggestions.clear();
    set_language(config->get_string(err, "language"));

    if (m_dictionaries.find(m_current_language) == m_dictionaries.end()) {
        return false;
    }

    int result = enchant_dict_check(m_dictionaries[m_current_language], word.c_str(), -1);

    if (result == 0) {
        return false;
    } else {
        size_t suggestion_count = 0;
        char **suggestions = enchant_dict_suggest(m_dictionaries[m_current_language], word.c_str(), -1, &suggestion_count);

        if (suggestion_count > 0) {
            for (size_t i = 0; i < suggestion_count; i++) {
                m_suggestions.push_back(suggestions[i]);
            }

            enchant_dict_free_string_list(m_dictionaries[m_current_language], suggestions);
        }
        return true;
    }
}
