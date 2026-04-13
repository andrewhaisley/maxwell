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

extern char* global_maxhome;

void dummy_sigproc()
{
}

static void set_io_timeout()
{
    // set the timeout to be quite large, in case of a highly loaded or low
    // performance machine
    signal(SIGALRM, (sighandler_t)dummy_sigproc);
    alarm(10);
}

static void clear_io_timeout()
{
    alarm(0);
}

static int read_with_timeout(int fd, char* buf, size_t count)
{
    int res;
    set_io_timeout();
    res = read(fd, buf, count);
    clear_io_timeout();
    return res;
}

static size_t write_with_timeout(int fd, const char* buf, size_t count)
{
    size_t res;
    set_io_timeout();
    res = write(fd, buf, count);
    clear_io_timeout();
    return res;
}

static void button_cb(
    Widget widget,
    XtPointer client_data,
    XtPointer call_data)
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
    strncpy(d->replacement, choice, 99);
    XtFree(choice);

    if (cbs->reason == XmCR_BROWSE_SELECT) {
        // single clicked on a replacement word
        XmTextSetString(d->replacement_text, d->replacement);
        return;
    }

    if (cbs->reason == XmCR_DEFAULT_ACTION) {
        // double clicked on a word
        d->modal_result = replace_e;
    }
}

static void text_return(
    Widget widget,
    XtPointer client_data,
    XtPointer call_data)
{
    mx_spell_d* d = (mx_spell_d*)client_data;

    d->handle_button(d->replace_button);
}

static void language_change(
    Widget widget,
    XtPointer client_data,
    XtPointer call_data)
{
    mx_spell_d* d = (mx_spell_d*)client_data;
    char s[30];

    strcpy(s, XtName(widget));
    d->set_language(s);
}

mx_spell_d::mx_spell_d(Widget parent, const char* language)
    : mx_dialog("spell", parent, TRUE, FALSE)
{
    Widget label1, label2, language_menu, language_sub_menu;
    Arg args[15];
    int n;

    if (language == NULL || mx_is_blank(language)) {
        language = "english";
    }

    child_pid = 0;
    strcpy(current_language, language);

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

    set_language_menu(language_sub_menu, language);

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

void mx_spell_d::ispell_fail()
{
    static mx_inform_d* d = NULL;

    if (d == NULL) {
        d = new mx_inform_d("ispellFail", dialog, error_e);
    }

    d->centre();
    d->run_modal();
    d->deactivate();

    return;
}

void mx_spell_d::fill_list()
{
    int i = 0, j, n = 0;
    char buffer[1000], word[100];
    XmString str[100];

    i = read_with_timeout(from_ispell_fd[0], buffer, 999);
    if (i < 0) {
        ispell_fail();
        return;
    } else {
        buffer[i] = 0;
    }

    i = 0;
    while (buffer[i] != 0 && buffer[i] != ':') {
        i++;
    }

    if (buffer[i] == 0) {
        XmListDeleteAllItems(list);
        XmTextSetString(replacement_text, const_cast<char*>(""));
        return;
    }

    i += 2;

    while (TRUE) {
        j = 0;
        while (buffer[i] != ',' && buffer[i] != 0 && buffer[i] != '\n') {
            word[j++] = buffer[i++];
        }
        word[j] = 0;
        if (n == 0) {
            XmTextSetString(replacement_text, word);
        }
        str[n++] = XmStringCreate(word, XmFONTLIST_DEFAULT_TAG);
        if (n == 100 || buffer[i] == 0 || buffer[i] == '\n') {
            break;
        } else {
            i += 2;
        }
    }

    XtVaSetValues(list,
        XmNitemCount, n,
        XmNitems, str,
        NULL);

    for (i = 0; i < n; i++) {
        XmStringFree(str[i]);
    }
}

void mx_spell_d::handle_button(Widget w)
{
    char* s;
    char buffer[201];

    if (w == ignore_button) {
        XtVaSetValues(back_button, XmNsensitive, True, NULL);

        modal_result = ignore_e;
        return;
    }

    if (w == ignore_all_button) {
        // tell ispell about it
        sprintf(buffer, "@%s\n", original_word);
        write_with_timeout(to_ispell_fd[1], buffer, strlen(buffer));

        XtVaSetValues(back_button, XmNsensitive, False, NULL);

        modal_result = ignore_e;
        return;
    }

    if (w == replace_button) {
        modal_result = replace_e;

        s = XmTextGetString(replacement_text);
        strcpy(replacement, s);
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
        // tell ispell about it
        sprintf(buffer, "*%s\n", original_word);
        write_with_timeout(to_ispell_fd[1], buffer, strlen(buffer));

        XtVaSetValues(back_button, XmNsensitive, False, NULL);

        modal_result = ignore_e;
        return;
    }
}

int mx_spell_d::run_modal(const char* word)
{
    char buffer[202], *s, tmpbuf[1000];
    strcpy(original_word, word);

    // send the word to ispell
    if (strlen(word) > 100) {
        return ignore_e;
    }

    sprintf(buffer, "^%s\n", word);
    write_with_timeout(to_ispell_fd[1], buffer, strlen(buffer));

    if (read_with_timeout(from_ispell_fd[0], buffer, 1) < 0) {
        ispell_fail();
        return none_e;
    }

    if (buffer[0] == '*' || buffer[0] == '+' || buffer[0] == '-' || buffer[0] == '\n') {
        // Either 1 information line and a blank line
        // or just a blank line

        if (buffer[0] == '+') {
            read_with_timeout(from_ispell_fd[0], tmpbuf, 100);
        } else if (buffer[0] != '\n') {
            read_with_timeout(from_ispell_fd[0], tmpbuf, 2);
        }
        return correct_e;
    }

    if (buffer[0] == '&' || buffer[0] == '?') {
        fill_list();
    } else {
        XmListDeleteAllItems(list);
        XmTextSetString(replacement_text, const_cast<char*>(""));
        read_with_timeout(from_ispell_fd[0], tmpbuf, 999);
    }

    XmTextSetString(word_text, const_cast<char*>(word));
    s = XmTextGetString(replacement_text);
    XmListSelectPos(list, 1, False);

    strcpy(replacement, s);

    XmProcessTraversal(replacement_text, XmTRAVERSE_CURRENT);
    if (s[0] != 0) {
        XmTextSetSelection(replacement_text, 0, strlen(s), CurrentTime);
    }

    XtFree(s);
    return mx_dialog::run_modal();
}

void mx_spell_d::spawn_ispell(const char* language)
{
    int i;

    static mx_inform_d* d = NULL;

    char buffer[100], file_name[MAX_PATH_LEN];
    char dict_file_name[MAX_PATH_LEN];

    finish_ispell();

    signal(SIGCHLD, SIG_IGN);

    if (pipe(to_ispell_fd)) {
        return;
    }
    if (pipe(from_ispell_fd)) {
        return;
    }

    if ((child_pid = vfork()) == 0) {
        /* child */
        dup2(to_ispell_fd[0], fileno(stdin));
        dup2(from_ispell_fd[1], fileno(stdout));
        close(to_ispell_fd[1]);
        close(from_ispell_fd[0]);

        sprintf(file_name, "/usr/bin/ispell");
        //sprintf(dict_file_name, "%s/dict/%s", global_maxhome, language);
        //execl(file_name, "ispell", "-a", "-d", dict_file_name, NULL);
        execl(file_name, "ispell", NULL);

        // exec failed if we got here
        exit(1);
    } else {
        close(to_ispell_fd[0]);
        close(from_ispell_fd[1]);

        i = read_with_timeout(from_ispell_fd[0], buffer, 100);

        spawn_ispell_failed = (i < 0);
        if (spawn_ispell_failed) {
            finish_ispell();
            if (d == NULL) {
                d = new mx_inform_d("ispellStartFail", dialog, error_e);
            }
            d->centre();
            d->run_modal();
            d->deactivate();

            buffer[0] = 0;
        } else {
            buffer[i] = 0;
        }
    }
    return;
}

void mx_spell_d::finish_ispell()
{
    if (child_pid > 0) {
        int res;
        char tmpbuf[1000];
        char buffer[] = "#\n\n\nburble burble\n\n";

        // send a string to tell ispell to save its dictionary
        res = write_with_timeout(to_ispell_fd[1], buffer, strlen(buffer));

        if (res != -1) {
            // read a string from ispell to make sure its saved its dictionary
            read_with_timeout(from_ispell_fd[0], tmpbuf, 999);
        }

        // close the pipes we were writing to
        close(to_ispell_fd[1]);
        close(from_ispell_fd[0]);

        // kill ispell
        kill(child_pid, SIGTERM);

        set_io_timeout();
        waitpid(child_pid, NULL, 0);
        clear_io_timeout();

        child_pid = 0;
    }
}

void mx_spell_d::activate()
{
    mx_dialog::activate();
}

void mx_spell_d::set_language(const char* s)
{
    finish_ispell();
    spawn_ispell(s);
}

void mx_spell_d::set_language_menu(Widget w, const char* def)
{
    int i;
    Widget b;

    b = XtVaCreateManagedWidget(
        def,
        xmPushButtonGadgetClass,
        w,
        NULL);

    XtAddCallback(b, XmNactivateCallback, language_change, this);

    for (i = 0; i < MX_NUM_LANGUAGES; i++) {
        if (strcmp(mx_language_names[i], def) != 0) {
            b = XtVaCreateManagedWidget(
                mx_language_names[i],
                xmPushButtonGadgetClass,
                w,
                NULL);

            XtAddCallback(b, XmNactivateCallback, language_change, this);
        }
    }
}

bool mx_spell_d::wrong_spelling(const char* word)
{
    char buffer[202], tmpbuf[1000];

    // send the word to ispell
    if (strlen(word) > 100) {
        return FALSE;
    }

    sprintf(buffer, "^%s\n", word);
    write_with_timeout(to_ispell_fd[1], buffer, strlen(buffer));

    if (read_with_timeout(from_ispell_fd[0], buffer, 1) < 0) {
        return FALSE;
    }

    if (buffer[0] == '*' || buffer[0] == '+' || buffer[0] == '-' || buffer[0] == '\n') {
        // Either 1 information line and a blank line
        // or just a blank line
        if (buffer[0] == '+') {
            read_with_timeout(from_ispell_fd[0], tmpbuf, 100);
        } else {
            if (buffer[0] != '\n') {
                read_with_timeout(from_ispell_fd[0], tmpbuf, 2);
            }
        }
        return FALSE;
    }

    // clear the input
    read_with_timeout(from_ispell_fd[0], tmpbuf, 999);

    return buffer[0] == '&' || buffer[0] == '?' || buffer[0] == '#';
}
