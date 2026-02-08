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
 * MODULE : mx_dg_undo.C
 *
 * AUTHOR : David Miller
 *
 * This file
 *
 * DESCRIPTION:
 * Module mx_dg_undo.C
 *
 *
 * Revision 1.1  1996/10/06 13:15:45  andrew
 * Initial revision
 *
 *
 */

#include "mx_dg_editor.h"

mx_dg_undo::mx_dg_undo(mx_dg_editor* ieditor,
    int imaxLevels)
{
    editor = ieditor;

    maxLevels = imaxLevels + 1;

    // the first action is a dud marker
    currentAction = 0;

    // always put a dud transaction in as an end marker
    actions = new mx_dg_undo_action[maxLevels];
}

mx_dg_undo::~mx_dg_undo()
{
    delete[] actions;
}

void mx_dg_undo::startTransaction()
{
    // any undo actions left from previous
    // undo's - also always clear the current action
    // and the next action - which is an end marker

    currentAction++;

    int forwardAction = currentAction;
    int count = 0;

    while ((!actions[forwardAction].isClear()) || (count < 2)) {
        actions[forwardAction].clear();
        forwardAction++;
        if (forwardAction == maxLevels)
            forwardAction = 0;
        count++;
    }
}

void mx_dg_undo::undo(int& err)
{
    err = MX_ERROR_OK;

    if (actions[currentAction].isClear())
        return;

    actions[currentAction].undo(err, editor);
    MX_ERROR_CHECK(err);

    currentAction--;
    if (currentAction == -1)
        currentAction = maxLevels - 1;
abort:;
}

void mx_dg_undo::redo(int& err)
{
    int nextAction = currentAction + 1;

    err = MX_ERROR_OK;

    if (nextAction == maxLevels)
        nextAction = 0;

    if (actions[nextAction].isClear())
        return;

    currentAction = nextAction;

    actions[currentAction].redo(err, editor);
    MX_ERROR_CHECK(err);

abort:;
}

void mx_dg_undo::addUndoItem(mx_dg_ur_item* uitem)
{
    actions[currentAction].addUndoItem(uitem);
}

mx_dg_undo_action::mx_dg_undo_action()
{
    nallocUndos = 0;
    nundos = 0;
    undoItems = NULL;
}

mx_dg_undo_action::~mx_dg_undo_action()
{
    clear();
}

void mx_dg_undo_action::clear()
{
    for (int iu = 0; iu < nundos; iu++) {
        delete undoItems[iu];
        undoItems[iu] = NULL;
    }

    delete[] undoItems;

    undoItems = NULL;
    nallocUndos = 0;
    nundos = 0;
}

void mx_dg_undo_action::addUndoItem(mx_dg_ur_item* uitem)
{
    if (nundos == nallocUndos) {
        mx_dg_ur_item** oldUndoItems = undoItems;

        nallocUndos += 16;

        // undoItems = new (mx_dg_ur_item *)[nallocUndos] ;

        memcpy(undoItems, oldUndoItems,
            nundos * sizeof(mx_dg_ur_item*));

        delete[] oldUndoItems;
    }

    undoItems[nundos] = uitem;
    nundos++;
}

void mx_dg_undo_action::undo(int& err,
    mx_dg_editor* editor)
{
    err = MX_ERROR_OK;

    for (int iu = nundos - 1; iu >= 0; iu--) {
        undoItems[iu]->undo(err, editor);
        MX_ERROR_CHECK(err);
    }

    editor->refresh(err, areaOfInterest);
    MX_ERROR_CHECK(err);

abort:;
}

void mx_dg_undo_action::redo(int& err,
    mx_dg_editor* editor)
{
    err = MX_ERROR_OK;

    for (int iu = 0; iu < nundos; iu++) {
        undoItems[iu]->redo(err, editor);
        MX_ERROR_CHECK(err);
    }

    editor->refresh(err, areaOfInterest);
    MX_ERROR_CHECK(err);

abort:;
}

void mx_dg_add_ur_item::undo(int& err,
    mx_dg_editor* editor)
{
    err = MX_ERROR_OK;

    editor->deleteGeometry(err,
        levelKey,
        nextKey,
        prevKey);
    MX_ERROR_CHECK(err);
abort:;
}

void mx_dg_add_ur_item::redo(int& err,
    mx_dg_editor* editor)
{
    err = MX_ERROR_OK;

    editor->getArea()->undeleteGeometry(err,
        levelKey,
        nextKey,
        prevKey);
    MX_ERROR_CHECK(err);
abort:;
}
