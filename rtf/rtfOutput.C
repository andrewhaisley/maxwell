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
 * MODULE : rtfOutput.C
 *
 * AUTHOR : David Miller
 *
 * This file
 *
 * DESCRIPTION:
 * Module rtfOutput.C
 *
 *
 *
 *
 *
 */

#include <rtf.h>
#include <string.h>

void mx_rtf::processOutput(int& err)
{
    RTFSgroup* group;
    rtfDestinationType destination;

    if ((textBufferSize == 0) || skippingGroup)
        goto abort;

    destination = currentDestination(err);
    MX_ERROR_CHECK(err);

    group = (RTFSgroup*)groups.get(err, -1);
    MX_ERROR_CHECK(err);

    switch (destination) {
    case RTFDfonttbl:
        processFontOutput(err, group);
        MX_ERROR_CHECK(err);
        break;
    case RTFDfiletbl:
        processFileOutput(err, group);
        MX_ERROR_CHECK(err);
        break;
    case RTFDcolortbl:
        processColourOutput(err, group);
        MX_ERROR_CHECK(err);
        break;
    case RTFDstylesheet:
        processStyleOutput(err, group);
        MX_ERROR_CHECK(err);
        break;
    case RTFDrtf:
    case RTFDheader:
    case RTFDfooter:
    case RTFDfldrslt:
        processMainOutput(err, group);
        MX_ERROR_CHECK(err);
        break;
    case RTFDpict:
        processPictOutput(err, group);
        MX_ERROR_CHECK(err);

        pictData.inPict = FALSE;
        break;
    case RTFDfldinst:
        processFieldInstOutput(err, group);
        MX_ERROR_CHECK(err);
        break;
    default:
        break;
    }
abort:
    textBufferSize = 0;
}

void mx_rtf::processFontOutput(int& err, RTFSgroup* group) {

    bool foundEnd;
    int fontNameLen;
#define MAX_FONT_OUTPUT_LENGTH 2048
    static char fontOutput[MAX_FONT_OUTPUT_LENGTH + 1];
    static int fontOutputLen = 0;
    static RTFSgroup* lastGroup = NULL;

    rtfTableType k;

    MX_ERROR_ASSERT(err, currentFont != NULL);
    MX_ERROR_ASSERT(err, (fontOutputLen + textBufferSize) < MAX_FONT_OUTPUT_LENGTH);

    if (group != lastGroup)
        fontOutputLen = 0;
    lastGroup = group;

    // add this output onto the font output
    memcpy(fontOutput + fontOutputLen, textBuffer, textBufferSize);
    fontOutputLen += textBufferSize;

    // This is either the font name or a ';'
    foundEnd = (textBuffer[textBufferSize - 1] == ';');

    if (foundEnd) {
        // if we've got the end of the font, then process the output
        fontOutput[fontOutputLen - 1] = 0;

        fontNameLen = fontOutputLen - 1;
        fontOutputLen = 0;

        if (fontNameLen > 0) {
            currentFont->fontname = new char[fontNameLen + 1];
            strncpy(currentFont->fontname, fontOutput, fontNameLen);
            (currentFont->fontname)[fontNameLen] = 0;

            if (currentFont->f == -1) {
                delete currentFont;
            } else {
                k = static_cast<rtfTableType>((currentFont->f * RTFTnumberTables) + RTFTfonts);
                m_tables[k] = currentFont;

                userProcessFontOutput(err, currentFont);
                MX_ERROR_CHECK(err);
            }
        }
        currentFont = NULL;
        fontOutputLen = 0;
    }
abort:;
}

void mx_rtf::processFileOutput(int& err, RTFSgroup* group)
{
    //mx_attribute_value data, key;
    int fileNameLen = textBufferSize;

    if (fileNameLen > 0) {
        currentFile->filename = new char[fileNameLen + 1];

        strncpy(currentFile->filename, textBuffer, fileNameLen);
        (currentFile->filename)[fileNameLen] = 0;
    }

    if (currentFile->fid == -1) {
        delete currentFile;
    } else {
        m_tables[static_cast<rtfTableType>((currentFile->fid * RTFTnumberTables) + RTFTfiles)] = currentFile;
        userProcessFileOutput(err, currentFile);
        MX_ERROR_CHECK(err);
    }

abort:
    currentFile = NULL;
}

void mx_rtf::processStyleOutput(int& err, RTFSgroup* group)
{
    bool foundEnd;
    int styleNameLen;

    // This is either the style name or a ';'

    foundEnd = (textBuffer[textBufferSize - 1] == ';');

    styleNameLen = foundEnd ? (textBufferSize - 1) : textBufferSize;

    if (currentStyle == NULL) {
        currentStyle = new RTFSstyle(err);
        MX_ERROR_CHECK(err);
    }

    if (styleNameLen > 0) {
        currentStyle->stylename = new char[styleNameLen + 1];

        strncpy(currentStyle->stylename, textBuffer, styleNameLen);
        (currentStyle->stylename)[styleNameLen] = 0;
    }

    if (foundEnd) {
        if (currentStyle->styleNumber == -1) {
            delete currentStyle;
        } else {
            m_tables[static_cast<rtfTableType>((currentStyle->styleNumber * RTFTnumberTables) + currentStyle->styleType)] = currentStyle;
            userProcessStyleOutput(err, currentStyle);
            MX_ERROR_CHECK(err);
        }
    }

abort:
    currentStyle = NULL;
}

void mx_rtf::processColourOutput(int& err, RTFSgroup* group)
{
    bool foundEnd;

    foundEnd = (textBuffer[textBufferSize - 1] == ';');

    if (foundEnd) {
        if (currentColour == NULL) {
            currentColour = new RTFScolour;

            currentColour->offset = currentColourCount;
            (currentColourCount)++;
        }

        m_tables[static_cast<rtfTableType>((currentColour->offset * RTFTnumberTables) + RTFTcolours)] = currentColour;
        userProcessColourOutput(err, currentColour);
        MX_ERROR_CHECK(err);
    }

abort:
    currentColour = NULL;
}

void mx_rtf::processMainOutput(int& err, RTFSgroup* group)
{
    addText(err, 0);
    MX_ERROR_CHECK(err);
abort:;
}

void mx_rtf::processPictOutput(int& err, RTFSgroup* group)
{
}

void mx_rtf::processFieldInstOutput(int& err, RTFSgroup* group)
{
}
