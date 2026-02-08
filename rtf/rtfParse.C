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
 * MODULE : rtfParse.C
 *
 * AUTHOR : David Miller
 *
 * This file
 *
 * DESCRIPTION:
 * Module rtfParse.C
 *
 *
 *
 */

#include "rtf.h"
#include "rtflocal.h"
#include <ctype.h>

/*-------------------------------------------------
 * FUNCTION: mx_rtf::putLast
 *
 * DESCRIPTION: Put back the last character read
 *
 *
 */

bool mx_rtf::putLast(int& err, char c)
{
    // An error
    if (ungetc(c, iif) == EOF) {
        MX_ERROR_THROW(err, MX_ERROR_RTF_FILE_ERROR);
    }

    file_offset--;
    return FALSE;
abort:
    return TRUE;
}

/*-------------------------------------------------
 * FUNCTION: mx_rtf::readNext
 *
 * DESCRIPTION: Read the next character of an RTF
 * file
 *
 */

bool mx_rtf::readNext(int& err,
    char& c)
{
    int val;

    err = MX_ERROR_OK;

    val = fgetc(iif);
    c = val;

    // Reached end of file
    if (val == EOF) {
        if (groups.get_num_items() != 0) {
            MX_ERROR_THROW(err, MX_INTERNAL_RTF_ERROR);
        }
        return TRUE;
    }

    file_offset++;
    return FALSE;
abort:
    return TRUE;
}

/*-------------------------------------------------
 * FUNCTION: mx_rtf::currentDestination
 *
 * DESCRIPTION: Read the rtf file
 *
 *
 */

rtfDestinationType mx_rtf::currentDestination(int& err)
{
    int numItems, iitem;
    RTFSgroup* group;

    // Clean out the groups
    numItems = groups.get_num_items() - 1;

    for (iitem = numItems; iitem >= 0; iitem--) {
        group = (RTFSgroup*)groups.get(err, iitem);
        MX_ERROR_CHECK(err);

        if (group->destination != RTFDunknownDestination)
            return group->destination;
    }

abort:
    return RTFDunknownDestination;
}

/*-------------------------------------------------
 * FUNCTION: mx_rtf::findControl
 *
 * DESCRIPTION: Read the rtf file
 *
 *
 */

void mx_rtf::findControl(int& err)
{
    //mx_attribute_value value, data;
    bool readValue = TRUE, gotControl = FALSE;
    RTFcontrol* thisControl = NULL;
    RTFSgroup* thisGroup = NULL;
    char c;
    int controlValue;

    controlBuffer[controlBufferSize] = 0;
    controlBufferSize = 0;

    readingType = RTFreadingText;

    if (m_controls.find(controlBuffer) != m_controls.end()) {
        gotControl = true;
        thisControl = m_controls[controlBuffer];
    }
    
    thisGroup = (RTFSgroup*)groups.get(err, -1);
    MX_ERROR_CHECK(err);

    if (!gotControl) {
        // If this is a new destination type - but is unknown
        // then set skipping all its data
        if (readingStarredDestination) {
            readingStarredDestination = FALSE;
            skippingGroup = TRUE;
            thisGroup->skippingRoot = TRUE;
        } else {
            // silently ignore unknown control words
            readValue = TRUE;
        }
    }

    if (readValue) {
        // Now read the value associated with the control

        controlValueSize = 0;

        if (gotControl && thisControl->control == RTFsymquote) {
            do {
                if (readNext(err, c))
                    return;

                if (isxdigit(c)) {
                    controlValueBuffer[controlValueSize] = c;
                    controlValueSize++;
                    if (controlValueSize == MAX_RTF_CONTROL_BUFFER) {
                        err = MX_ERROR_RTF_BUFFER_OVERFLOW;
                        goto abort;
                    }
                } else {
                    // Not a digit - put back if not a space
                    if (c != ' ') {
                        if (putLast(err, c))
                            return;
                    }
                    break;
                }
                if (controlValueSize == 2)
                    break;
            } while (1);

            controlValueBuffer[controlValueSize] = 0;
            sscanf(controlValueBuffer, "%x", &controlValue);
        } else {
            do {
                if (readNext(err, c))
                    return;

                if ((isdigit(c)) || ((c == '-') && (controlValueSize == 0))) {
                    controlValueBuffer[controlValueSize] = c;
                    controlValueSize++;
                    if (controlValueSize == MAX_RTF_CONTROL_BUFFER) {
                        err = MX_ERROR_RTF_BUFFER_OVERFLOW;
                        goto abort;
                    }
                } else {
                    // Not a digit - put back if not a space
                    if (c != ' ') {
                        if (putLast(err, c))
                            return;
                    }
                    break;
                }
            } while (1);

            if (controlValueSize == 0) {
                if (gotControl) {
                    if ((thisControl->controlType == RTFCtoggle) || (thisControl->controlType == RTFCflag)) {
                        controlValue = 1;
                    } else {
                        controlValue = thisControl->value;
                    }
                }
            } else {
                controlValueBuffer[controlValueSize] = 0;
                sscanf(controlValueBuffer, "%d", &controlValue);
            }
        }

        if (gotControl) {
            processControl(err, thisGroup, thisControl->control,
                thisControl->controlType, thisControl->destination,
                controlValue);
            MX_ERROR_CHECK(err);
        }
    }

    return;
abort:
    return;
}

void mx_rtf::addText(int& err, char c)
{
    char* newBuffer;

    err = MX_ERROR_OK;

    // If a newline or a carriage return - ignore it
    if ((c == '\n') || (c == '\r'))
        return;

    if (textBufferSize == allocTextBufferSize) {
        allocTextBufferSize += 1024;
        newBuffer = new char[allocTextBufferSize];

        memcpy(newBuffer, textBuffer, textBufferSize);

        delete[] textBuffer;
        textBuffer = newBuffer;
    }

    textBuffer[textBufferSize] = c;
    textBufferSize++;

    if (textBufferSize > 1000000) {
        err = MX_ERROR_RTF_BUFFER_OVERFLOW;
        goto abort;
    }

    goto exit;
abort:
exit:
    return;
}

void mx_rtf::openGroup(int& err)
{
    int numItems;

    RTFSgroup *group, *oldGroup;

    err = MX_ERROR_OK;

    group = new RTFSgroup(err);
    MX_ERROR_CHECK(err);

    // Clean out the groups
    numItems = groups.get_num_items() - 1;

    if (numItems >= 0) {
        oldGroup = (RTFSgroup*)groups.get(err, numItems);
        MX_ERROR_CHECK(err);
        group->equate(err, *oldGroup);
        MX_ERROR_CHECK(err);
    }

    groups.append(group);

    goto exit;
abort:
exit:
    return;
}

void mx_rtf::closeGroup(int& err)
{
    RTFSgroup* group;

    err = MX_ERROR_OK;

    group = (RTFSgroup*)groups.remove(err, -1);
    MX_ERROR_CHECK(err);

    if (group->skippingRoot)
        skippingGroup = FALSE;

    delete group;

    goto exit;
abort:
exit:
    return;
}

/*-------------------------------------------------
 * Function: mx_rtf::read
 *
 * DESCRIPTION: Read the rtf file
 *
 *
 */

void mx_rtf::read(int& err)
{
    char c;

    err = MX_ERROR_OK;

    do {

        if (readNext(err, c))
            return;

        switch (readingType) {
        case RTFreadingControl:
            if (islower(c)) {
                // Continue the control word
                controlBuffer[controlBufferSize] = c;
                controlBufferSize++;
                if (controlBufferSize == MAX_RTF_CONTROL_BUFFER) {
                    err = MX_ERROR_RTF_BUFFER_OVERFLOW;
                    goto abort;
                }
            } else {
                // The control word is over - is it a
                // special character - or the start of
                // a new control or the control argument
                switch (c) {
                case '|':
                case '~':
                case '_':
                case ':':
                case '*':
                case '\'':
                case '\n':
                case '\r':
                case '-':
                    if (controlBufferSize != 0) {
                        if (putLast(err, c))
                            return;
                    } else {
                        controlBuffer[controlBufferSize] = c;
                        controlBufferSize++;
                        if (controlBufferSize == MAX_RTF_CONTROL_BUFFER) {
                            err = MX_ERROR_RTF_BUFFER_OVERFLOW;
                            goto abort;
                        }
                    }
                    break;
                default:
                    // Some other end to the control - put back
                    // the last character as it is part of the
                    // next control (e.g. \) or part of the
                    // plain text or part of the control
                    // argument or a space

                    if (putLast(err, c))
                        return;
                }

                findControl(err);
                MX_ERROR_CHECK(err);
            }

            break;
        case RTFreadingText:
            if (c == '\\') {
                /* might be a new control */
                if (readNext(err, c))
                    return;

                if ((c == '\\') || (c == '{') || (c == '}')) {
                    // Add this c to the text
                    addText(err, c);
                } else {
                    processOutput(err);
                    MX_ERROR_CHECK(err);

                    // Its a new control
                    // Put back the character and set up for
                    // reading the control data
                    if (putLast(err, c))
                        return;
                    readingType = RTFreadingControl;
                }
            } else if (c == '{') {
                processOutput(err);
                MX_ERROR_CHECK(err);

                /* new group */
                openGroup(err);
                MX_ERROR_CHECK(err);
            } else if (c == '}') {
                processOutput(err);
                MX_ERROR_CHECK(err);

                /* close group */
                closeGroup(err);
                MX_ERROR_CHECK(err);
            } else {
                addText(err, c);
                MX_ERROR_CHECK(err);
            }

            break;
        }
    } while (1);

    return;
abort:
    return;
}
