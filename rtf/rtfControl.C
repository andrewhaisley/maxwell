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
 * MODULE : rtfControl.C
 *
 * AUTHOR : David Miller
 *
 * This file
 *
 * DESCRIPTION:
 * Module rtfControl.C
 *
 *
 *
 *
 */

#include <rtf.h>
#include <rtflocal.h>

/*-------------------------------------------------
 * FUNCTION: mx_rtf::processFontControl
 *
 * DESCRIPTION:
 *
 *
 */

void mx_rtf::processFontControl(int& err,
    RTFSgroup* group,
    rtfDestinationType destination,
    rtfControl control,
    int controlValue)
{
    if (currentFont == NULL) {
        currentFont = new RTFSfont;
    }

    switch (control) {
    case RTFfnil:
    case RTFfroman:
    case RTFfswiss:
    case RTFfmodern:
    case RTFfscript:
    case RTFfdecor:
    case RTFftech:
    case RTFfbidi:
        currentFont->fontfamily = control;
        break;
    case RTFfcharset:
        currentFont->fcharset = (RTFEcharset)controlValue;
        break;
    case RTFfprq:
        currentFont->fprq = (RTFEpitch)controlValue;
        break;
    case RTFf:
        currentFont->f = controlValue;
        break;
    default:
        break;
    }
    return;
}

/*-------------------------------------------------
 * FUNCTION: mx_rtf::processColourControl
 *
 * DESCRIPTION:
 *
 *
 */

void mx_rtf::processColourControl(int& err,
    RTFSgroup* group,
    rtfDestinationType destination,
    rtfControl control,
    int controlValue)
{
    if (currentColour == NULL) {
        currentColour = new RTFScolour;

        currentColour->offset = currentColourCount;
        (currentColourCount)++;
    }

    switch (control) {
    case RTFred:
        currentColour->red = controlValue;
        break;
    case RTFgreen:
        currentColour->green = controlValue;
        break;
    case RTFblue:
        currentColour->blue = controlValue;
        break;
    default:
        break;
    }

    return;
}

/*-------------------------------------------------
 * FUNCTION: mx_rtf::processFileControl
 *
 * DESCRIPTION:
 *
 *
 */

void mx_rtf::processFileControl(int& err,
    RTFSgroup* group,
    rtfDestinationType destination,
    rtfControl control,
    int controlValue)
{
    if (currentFile == NULL) {
        currentFile = new RTFSfile;
    }

    switch (control) {
    case RTFfid:
        currentFile->fid = controlValue;
        break;
    case RTFfrelative:
        currentFile->frelative = controlValue;
        break;
    case RTFfvalidmac:
    case RTFfvaliddos:
    case RTFfvalidntfs:
    case RTFfvalidhpfs:
    case RTFfnetwork:
        currentFile->filesource = control;
        break;
    default:
        break;
    }

    return;
}

/*-------------------------------------------------
 * FUNCTION: mx_rtf::processFontControl
 *
 * DESCRIPTION:
 *
 *
 */

void mx_rtf::processStyleControl(int& err,
    RTFSgroup* group,
    rtfDestinationType destination,
    rtfControl control,
    int controlValue)
{
    RTFcontrol* storeControl;

    if (currentStyle == NULL) {
        currentStyle = new RTFSstyle(err);
        MX_ERROR_CHECK(err);
    }

    switch (control) {
    case RTFs:
        currentStyle->styleNumber = controlValue;
        currentStyle->styledef = control;
        currentStyle->styleType = RTFTparagraphStyle;
        currentStyle->formatting.paragraph.fmt.s = controlValue;
        currentStyle->formatting.paragraph.fmt.ss = currentStyle;
        break;
    case RTFcs:
        currentStyle->styleNumber = controlValue;
        currentStyle->styledef = control;
        currentStyle->styleType = RTFTcharacterStyle;
        currentStyle->formatting.character.fmt.cs = controlValue;
        currentStyle->formatting.character.fmt.css = currentStyle;
        break;
    case RTFds:
        currentStyle->styleNumber = controlValue;
        currentStyle->styledef = control;
        currentStyle->styleType = RTFTsectionStyle;
        currentStyle->formatting.section.fmt.ds = controlValue;
        currentStyle->formatting.section.fmt.dss = currentStyle;
        break;
    case RTFsbasedon:
        currentStyle->sbasedon = controlValue;
        break;
    case RTFadditive:
        currentStyle->additive = controlValue;
        break;
    case RTFsnext:
        currentStyle->snext = controlValue;
        break;
    default:

        storeControl = new RTFcontrol;

        storeControl->control = control;
        storeControl->destination = destination;
        storeControl->value = controlValue;

        currentStyle->setControls.append(storeControl);
        break;
    }
abort:;
}

/*-------------------------------------------------
 * FUNCTION: RTFSformatting::processControl
 *
 * DESCRIPTION:
 *
 *
 */

void RTFSformatting::processControl(int& err, std::map<rtfTableType, void *> &tables, rtfDestinationType destination, rtfControl control, int controlValue)
{
    RTFSparBorder* pcurrentBorder;
    bool getStyle;
    rtfTableType styleType;
    RTFSstyle* style;

    getStyle = FALSE;
    switch (control) {
    case RTFs:
        styleType = RTFTparagraphStyle;
        getStyle = TRUE;
        break;
    case RTFcs:
        styleType = RTFTcharacterStyle;
        getStyle = TRUE;
        break;
    case RTFds:
        styleType = RTFTsectionStyle;
        getStyle = TRUE;
        break;
    default:
        break;
    }

    if (getStyle) {
        // get the style

        rtfTableType k = static_cast<rtfTableType>((controlValue * RTFTnumberTables) + styleType);

        if (tables.find(k) == tables.end()) {
            return;
        } else {
            style = static_cast<RTFSstyle *>(tables[k]);
        }

        style->update(err, tables);
        MX_ERROR_CHECK(err);

        switch (control) {
        case RTFs:
            paragraph.reset();
            MX_ERROR_CHECK(err);

            character.reset();
            paragraph.equate(err, style->formatting.paragraph);
            MX_ERROR_CHECK(err);
            paragraph.tabsAreStyleTabs = TRUE;

            character = style->formatting.character;
            break;
        case RTFcs:
            character.reset();
            character = style->formatting.character;
            break;
        case RTFds:
            section.reset();
            section = style->formatting.section;
            break;
        default:
            break;
        }
    } else {
        switch (destination) {
        case RTFDborder:

            paragraph.getCurrentBorder(err, &pcurrentBorder);
            MX_ERROR_CHECK(err);

            pcurrentBorder->processControl(err, control, controlValue);
            MX_ERROR_CHECK(err);
            break;
        case RTFDparfmt:
            paragraph.fmt.processControl(err, control, controlValue);
            MX_ERROR_CHECK(err);
            break;
        case RTFDpar:
            paragraph.processControl(err, control, controlValue);
            MX_ERROR_CHECK(err);
            break;
        case RTFDframe:
            paragraph.frame.processControl(err, control, controlValue);
            MX_ERROR_CHECK(err);
            break;
        case RTFDshading:
            paragraph.shading.processControl(err, control, controlValue);
            MX_ERROR_CHECK(err);
            break;
        case RTFDtab:
            paragraph.currentTab.processControl(err, control, controlValue);
            MX_ERROR_CHECK(err);

            paragraph.processEndTab(control, controlValue);
            break;

        case RTFDchrfmt:
            character.fmt.processControl(err, control, controlValue);
            MX_ERROR_CHECK(err);
            break;
        case RTFDsecfmt:
            section.fmt.processControl(err, control, controlValue);
            MX_ERROR_CHECK(err);
            break;
        case RTFDsectionpage:
            section.fmt.page.processControl(err, control, controlValue);
            MX_ERROR_CHECK(err);
            break;
        default:
            break;
        }
    }

abort:;
}

/*-------------------------------------------------
 * FUNCTION: mx_rtf::processNewParagraph
 *
 * DESCRIPTION:
 *
 *
 */

void mx_rtf::processNewParagraph(int& err,
    RTFSgroup* group,
    rtfDestinationType destination)
{
}

/*-------------------------------------------------
 * FUNCTION: mx_rtf::processNewTableCell
 *
 * DESCRIPTION:
 *
 *
 */

void mx_rtf::processNewTableCell(int& err,
    RTFSgroup* group,
    rtfDestinationType destination)
{
}

/*-------------------------------------------------
 * FUNCTION: mx_rtf::processNewTableRow
 *
 * DESCRIPTION:
 *
 *
 */

void mx_rtf::processNewTableRow(int& err,
    RTFSgroup* group,
    rtfDestinationType destination)
{
}

/*-------------------------------------------------
 * FUNCTION: mx_rtf::processNewTab
 *
 * DESCRIPTION:
 *
 *
 */

void mx_rtf::processNewTab(int& err,
    RTFSgroup* group,
    rtfDestinationType destination)
{
}

/*-------------------------------------------------
 * FUNCTION: mx_rtf::processSpecialCharacter
 *
 * DESCRIPTION:
 *
 *
 */

void mx_rtf::processSpecialCharacter(int& err,
    RTFSgroup* group,
    rtfDestinationType destination,
    rtfControl control,
    int controlValue)
{
}

/*-------------------------------------------------
 * FUNCTION: mx_rtf::processMainControl
 *
 * DESCRIPTION:
 *
 *
 */

void mx_rtf::processMainControl(int& err,
    RTFSgroup* group,
    rtfDestinationType destination,
    rtfControl control,
    int controlValue)
{
    RTFSparBorder* pcurrentBorder;

    switch (control) {
    case RTFsectd:
        group->formatting.section.reset(&(documentData.page));
        break;
    case RTFpard:
        group->formatting.paragraph.reset();
        break;
    case RTFplain:
        group->formatting.character.reset();
        break;
    case RTFrow:
        processNewTableRow(err, group, destination);
        MX_ERROR_CHECK(err);
        tableData.cellCount = 0;
        break;
    case RTFcell:
        processNewTableCell(err, group, destination);
        MX_ERROR_CHECK(err);
        (tableData.cellCount)++;
        break;
    case RTFpar:
    case RTFnewline:
    case RTFcarriageReturn:
        processNewParagraph(err, group, destination);
        MX_ERROR_CHECK(err);
        break;
    case RTFtab:
        processNewTab(err, group, destination);
        MX_ERROR_CHECK(err);
        break;
    case RTFsect:
    case RTFchdate:
    case RTFchdpl:
    case RTFchdpa:
    case RTFchtime:
    case RTFchpgn:
    case RTFsectnum:
    case RTFchftn:
    case RTFchatn:
    case RTFchftnsep:
    case RTFchftnsepc:
    case RTFpage:
    case RTFcolumn:
    case RTFline:
    case RTFemdash:
    case RTFendash:
    case RTFemspace:
    case RTFenspace:
    case RTFbullet:
    case RTFlquote:
    case RTFrquote:
    case RTFldblquote:
    case RTFrdblquote:
    case RTFsymbar:
    case RTFsymtilda:
    case RTFsymhyphen:
    case RTFsymunderbar:
    case RTFsymcolon:
    case RTFsymstar:
    case RTFsymquote:
    case RTFltrmark:
    case RTFrtlmark:
    case RTFzwj:
    case RTFzwnj:
        processSpecialCharacter(err, group, destination, control,
            controlValue);
        MX_ERROR_CHECK(err);
        break;
    default:
        switch (destination) {
        case RTFDborder:
            if (tableData.inCell) {
                tableData.currentCell.getCurrentBorder(err, &pcurrentBorder);
                if (err == MX_INTERNAL_RTF_ERROR) {
                    MX_ERROR_CLEAR(err);
                } else {
                    MX_ERROR_CHECK(err);
                    pcurrentBorder->processControl(err, control, controlValue);
                    MX_ERROR_CHECK(err);
                }
            } else if (tableData.inTable) {
                tableData.getCurrentBorder(err, &pcurrentBorder);
                if (err == MX_INTERNAL_RTF_ERROR) {
                    MX_ERROR_CLEAR(err);
                } else {
                    MX_ERROR_CHECK(err);
                    pcurrentBorder->processControl(err, control, controlValue);
                    MX_ERROR_CHECK(err);
                }
            } else if (pictData.inPict) {
                pictData.border.processControl(err, control, controlValue);
                MX_ERROR_CHECK(err);
            } else {
                group->formatting.paragraph.getCurrentBorder(err,
                    &pcurrentBorder);
                MX_ERROR_CHECK(err);
                pcurrentBorder->processControl(err, control, controlValue);
                MX_ERROR_CHECK(err);
            }
            break;
        case RTFDshading:
            if (tableData.inCell) {
                tableData.currentCell.cellshad.processControl(err, control, controlValue);
                MX_ERROR_CHECK(err);
            } else if (pictData.inPict) {
                pictData.shading.processControl(err, control,
                    controlValue);
                MX_ERROR_CHECK(err);
            } else {
                group->formatting.processControl(err, m_tables, destination, control, controlValue);
                MX_ERROR_CHECK(err);
            }
            break;
        case RTFDparfmt:
        case RTFDframe:
        case RTFDtab:
        case RTFDchrfmt:
        case RTFDsecfmt:
        case RTFDsectionpage:
        case RTFDpar:
            group->formatting.processControl(err, m_tables, destination, control, controlValue);
            MX_ERROR_CHECK(err);
            break;
        case RTFDdocfmt:
            documentData.processControl(err, control, controlValue);
            MX_ERROR_CHECK(err);
            break;
        case RTFDdocpage:
            documentData.page.processControl(err, control, controlValue);
            MX_ERROR_CHECK(err);
            break;
        case RTFDdocfootnote:
            documentData.footnote.processControl(err, control, controlValue);
            MX_ERROR_CHECK(err);
            break;
        case RTFDtable:
            tableData.processControl(err, control, controlValue);
            MX_ERROR_CHECK(err);
            break;
        case RTFDcell:
            tableData.inCell = TRUE;
            tableData.currentCell.processControl(err, control, controlValue);
            MX_ERROR_CHECK(err);
            break;
        case RTFDpict:
            pictData.processControl(err, control, controlValue);
            MX_ERROR_CHECK(err);
            break;
        case RTFDfield:
        case RTFDfldinst:
            fieldData.processControl(err, control, controlValue);
            MX_ERROR_CHECK(err);
        default:
            break;
        }
        break;
    }

    return;
abort:
    return;
}

/*-------------------------------------------------
 * FUNCTION: mx_rtf::processControl
 *
 * DESCRIPTION:
 *
 *
 */

void mx_rtf::processControl(int& err,
    RTFSgroup* group,
    rtfControl control,
    rtfControlType controlType,
    rtfDestinationType controlDestination,
    int controlValue)
{
    rtfDestinationType currentdestination;

    // If skipping a group then return
    if (skippingGroup)
        return;

    // If the control is /* then return having noted that
    // the next control is a starred destination
    if (control == RTFsymstar) {
        readingStarredDestination = TRUE;
        return;
    }

    if (controlType == RTFCdestination) {
        currentdestination = group->destination = controlDestination;
        readingStarredDestination = FALSE;
    } else {
        currentdestination = currentDestination(err);
        MX_ERROR_CHECK(err);
    }

    switch (currentdestination) {
    case RTFDfonttbl:
        processFontControl(err, group, controlDestination, control,
            controlValue);
        MX_ERROR_CHECK(err);
        break;
    case RTFDfiletbl:
        processFileControl(err, group, controlDestination, control,
            controlValue);
        MX_ERROR_CHECK(err);
        break;
    case RTFDcolortbl:
        processColourControl(err, group, controlDestination, control,
            controlValue);
        MX_ERROR_CHECK(err);
        break;
    case RTFDstylesheet:
        processStyleControl(err, group, controlDestination, control,
            controlValue);
        MX_ERROR_CHECK(err);
        break;
    case RTFDfield:
    case RTFDfldrslt:
    case RTFDfldinst:
    case RTFDrtf:
    case RTFDheader:
    case RTFDfooter:
    case RTFDpict:
        processMainControl(err, group, controlDestination, control,
            controlValue);
        MX_ERROR_CHECK(err);
        break;
    default:
        group->skippingRoot = TRUE;
        skippingGroup = TRUE;
        break;
    }

abort:;
}

/*-------------------------------------------------
 * FUNCTION: RTFSpar::getCurrentBorder
 *
 * DESCRIPTION:
 *
 *
 */

void RTFSpar::getCurrentBorder(int& err,
    RTFSparBorder** pcurrentBorder)
{
    switch (currentBorder) {
    case RTFbrdrt:
        *pcurrentBorder = &brdrt;
        break;
    case RTFbrdrb:
        *pcurrentBorder = &brdrb;
        break;
    case RTFbrdrl:
        *pcurrentBorder = &brdrl;
        break;
    case RTFbrdrr:
        *pcurrentBorder = &brdrr;
        break;
    case RTFbrdrbtw:
        *pcurrentBorder = &brdrbtw;
        break;
    case RTFbrdrbar:
        *pcurrentBorder = &brdrbar;
        break;
    case RTFbox:
        *pcurrentBorder = &box;
        break;
    default:
        MX_ERROR_THROW(err, MX_INTERNAL_RTF_ERROR);
        break;
    }

abort:;
}

/*-------------------------------------------------
 * FUNCTION: RTFSpar::processControl
 *
 * DESCRIPTION:
 *
 *
 */

void RTFSpar::processControl(int& err,
    rtfControl control,
    int controlValue)
{
    RTFSparBorder* pcurrentBorder;
    bool setCurrentBorder = FALSE;

    switch (control) {
    case RTFbrdrt:
        setCurrentBorder = TRUE;
        pcurrentBorder = &brdrt;
        break;
    case RTFbrdrb:
        setCurrentBorder = TRUE;
        pcurrentBorder = &brdrb;
        break;
    case RTFbrdrl:
        setCurrentBorder = TRUE;
        pcurrentBorder = &brdrl;
        break;
    case RTFbrdrr:
        setCurrentBorder = TRUE;
        pcurrentBorder = &brdrr;
        break;
    case RTFbrdrbtw:
        setCurrentBorder = TRUE;
        pcurrentBorder = &brdrbtw;
        break;
    case RTFbrdrbar:
        setCurrentBorder = TRUE;
        pcurrentBorder = &brdrbar;
        break;
    case RTFbox:
        setCurrentBorder = TRUE;
        pcurrentBorder = &box;
        break;
    default:
        MX_ERROR_THROW(err, MX_INTERNAL_RTF_ERROR);
        break;
    }

    if (setCurrentBorder) {
        currentBorder = control;
        pcurrentBorder->reset();
    }
    return;
abort:
    return;
}

/*-------------------------------------------------
 * FUNCTION: RTFSparBorder::processControl
 *
 * DESCRIPTION:
 *
 *
 */

void RTFSparBorder::processControl(int& err,
    rtfControl control,
    int controlValue)
{
    switch (control) {
    case RTFbrdrs:
    case RTFbrdrth:
    case RTFbrdrsh:
    case RTFbrdrdb:
    case RTFbrdrdot:
    case RTFbrdrdash:
    case RTFbrdrhair:
        brdrk = control;
        break;
    case RTFbrdrw:
        brdrw = controlValue;
        break;
    case RTFbrdrcf:
        brdrcf = controlValue;
        break;
    case RTFbrsp:
        brsp = controlValue;
        break;
    default:
        break;
    }

    return;
}

/*-------------------------------------------------
 * FUNCTION: RTFSpar::processEndTab
 *
 * DESCRIPTION:
 *
 *
 */

void RTFSpar::processEndTab(rtfControl control, int controlValue)
{
    RTFSparTab* tab;
    bool newTab = false;
    int k;

    switch (control) {
    case RTFtx:
        k = currentTab.tx;
        newTab = true;
        break;
    case RTFtb:
        k = currentTab.tb;
        newTab = false;
        break;
    default:
        break;
    }

    if (newTab) {
        // delete the old tabs if they are from a style

        if (tabsAreStyleTabs) {
            deleteTabs();
            tabsAreStyleTabs = false;
        }

        if (tabs.find(k) != tabs.end()) {
            /* found the TAB - replace */
            tab = tabs[k];
            *tab = currentTab;
        } else {
            /* New Tab */
            tab = new RTFSparTab;

            *tab = currentTab;
            tabs[k] = tab;
        }
        currentTab.reset();
    }
}

/*-------------------------------------------------
 * FUNCTION: RTFSparTab::processControl
 *
 * DESCRIPTION:
 *
 *
 */

void RTFSparTab::processControl(int& err,
    rtfControl control,
    int controlValue)
{
    switch (control) {
    case RTFtx:
        tx = controlValue;
        break;
    case RTFtb:
        tb = controlValue;
        break;
    case RTFtqr:
    case RTFtqc:
    case RTFtqdec:
        tabkind = control;
        break;
    case RTFtldot:
    case RTFtlhyph:
    case RTFtlul:
    case RTFtlth:
    case RTFtleq:
        tablead = control;
        break;
    default:
        break;
    }

    return;
}

/*-------------------------------------------------
 * FUNCTION: RTFSparfmt::processControl
 *
 * DESCRIPTION:
 *
 *
 */

void RTFSparfmt::processControl(int& err,
    rtfControl control,
    int controlValue)
{
    switch (control) {
    case RTFhyphpar:
        hyphpar = controlValue;
        break;
    case RTFintbl:
        intbl = controlValue;
        break;
    case RTFkeep:
        keep = controlValue;
        break;
    case RTFkeepn:
        keepn = controlValue;
        break;
    case RTFlevel:
        level = controlValue;
        break;
    case RTFnoline:
        noline = controlValue;
        break;
    case RTFpagebb:
        pagebb = controlValue;
        break;
    case RTFsbys:
        sbys = controlValue;
        break;
    case RTFql:
    case RTFqr:
    case RTFqj:
    case RTFqc:
        alignment = control;
        break;
    case RTFfi:
        fi = controlValue;
        break;
    case RTFli:
        li = controlValue;
        break;
    case RTFri:
        ri = controlValue;
        break;
    case RTFsb:
        sb = controlValue;
        break;
    case RTFsa:
        sa = controlValue;
        break;
    case RTFsl:
        sl = controlValue;
        break;
    case RTFslmult:
        slmult = controlValue;
        break;
    default:
        break;
    }
}

/*-------------------------------------------------
 * FUNCTION: RTFSparShading::processControl
 *
 * DESCRIPTION:
 *
 *
 */

void RTFSparShading::processControl(int& err,
    rtfControl control,
    int controlValue)
{
    switch (control) {
    case RTFshading:
    case RTFclshdng:
        shading = controlValue;
        break;
    case RTFcfpat:
    case RTFclcfpat:
        cfpat = controlValue;
        break;
    case RTFcbpat:
    case RTFclcbpat:
        cbpat = controlValue;
        break;
    case RTFbghoriz:
    case RTFbgvert:
    case RTFbgfdiag:
    case RTFbgbdiag:
    case RTFbgcross:
    case RTFbgdcross:
    case RTFbgdkhoriz:
    case RTFbgdkvert:
    case RTFbgdkfdiag:
    case RTFbgdkbdiag:
    case RTFbgdkcross:
    case RTFbgdkdcross:
    case RTFclbghoriz:
    case RTFclbgvert:
    case RTFclbgfdiag:
    case RTFclbgbdiag:
    case RTFclbgcross:
    case RTFclbgdcross:
    case RTFclbgdkhor:
    case RTFclbgdkvert:
    case RTFclbgdkfdiag:
    case RTFclbgdkbdiag:
    case RTFclbgdkcross:
    case RTFclbgdkdcross:
        pat = control;
        break;
    default:
        break;
    }
}

/*-------------------------------------------------
 * FUNCTION: RTFSpage::processControl
 *
 * DESCRIPTION:
 *
 *
 */

void RTFSpage::processControl(int& err,
    rtfControl control,
    int controlValue)
{
    switch (control) {
    case RTFpaperw:
    case RTFpgwsxn:
        paperw = controlValue;
        break;
    case RTFpaperh:
    case RTFpghsxn:
        paperh = controlValue;
        break;
    case RTFmargl:
    case RTFmarglsxn:
        margl = controlValue;
        break;
    case RTFmargr:
    case RTFmargrsxn:
        margr = controlValue;
        break;
    case RTFmargt:
    case RTFmargtsxn:
        margt = controlValue;
        break;
    case RTFmargb:
    case RTFmargbsxn:
        margb = controlValue;
        break;
    case RTFfacingp:
        facingp = controlValue;
        break;
    case RTFgutter:
    case RTFguttersxn:
        gutter = controlValue;
        break;
    case RTFmargmirror:
        margmirror = controlValue;
        break;
    case RTFlandscape:
    case RTFlndscpsxn:
        landscape = controlValue;
        break;
    case RTFpgnstart:
    case RTFpgnstarts:
        pgnstart = controlValue;
        break;
    case RTFtitlepg:
        titlepg = controlValue;
        break;
    case RTFheadery:
        headery = controlValue;
        break;
    case RTFfootery:
        footery = controlValue;
        break;
    default:
        break;
    }
}

/*-------------------------------------------------
 * FUNCTION: RTFStable::getCurrentBorder
 *
 * DESCRIPTION:
 *
 *
 */

void RTFStable::getCurrentBorder(int& err,
    RTFSparBorder** pcurrentBorder)
{
    switch (currentBorder) {
    case RTFtrbrdrt:
        *pcurrentBorder = &trbrdrt;
        break;
    case RTFtrbrdrl:
        *pcurrentBorder = &trbrdrl;
        break;
    case RTFtrbrdrb:
        *pcurrentBorder = &trbrdrb;
        break;
    case RTFtrbrdrr:
        *pcurrentBorder = &trbrdrr;
        break;
    case RTFtrbrdrh:
        *pcurrentBorder = &trbrdrh;
        break;
    case RTFtrbrdrv:
        *pcurrentBorder = &trbrdrv;
        break;
    default:
        MX_ERROR_THROW(err, MX_INTERNAL_RTF_ERROR);
        break;
    }
abort:;
}

/*-------------------------------------------------
 * FUNCTION: RTFStable::processControl
 *
 * DESCRIPTION:
 *
 *
 */

void RTFStable::processControl(int& err,
    rtfControl control,
    int controlValue)

{
    bool setCurrentBorder = FALSE;
    RTFSparBorder* pcurrentBorder;
    RTFScell* newCell;

    switch (control) {
    case RTFtrowd:
        reset();
        inTable = TRUE;
        break;
    case RTFtrgaph:
        trgaph = controlValue;
        break;
    case RTFtrleft:
        trleft = controlValue;
        break;
    case RTFtrrh:
        trrh = controlValue;
        break;
    case RTFtrhdr:
        trhdr = controlValue;
        break;
    case RTFtrkeep:
        trkeep = controlValue;
        break;
    case RTFcellx:
        currentCell.cellx = controlValue;

        newCell = new RTFScell;

        *newCell = currentCell;
        currentCell.reset();

        inCell = FALSE;
        cells.append(newCell);

        break;
    case RTFtrql:
    case RTFtrqr:
    case RTFtrqc:
        rowjust = control;
        break;
    case RTFtrbrdrt:
        setCurrentBorder = TRUE;
        pcurrentBorder = &trbrdrt;
        break;
    case RTFtrbrdrl:
        setCurrentBorder = TRUE;
        pcurrentBorder = &trbrdrl;
        break;
    case RTFtrbrdrb:
        setCurrentBorder = TRUE;
        pcurrentBorder = &trbrdrb;
        break;
    case RTFtrbrdrr:
        setCurrentBorder = TRUE;
        pcurrentBorder = &trbrdrr;
        break;
    case RTFtrbrdrh:
        setCurrentBorder = TRUE;
        pcurrentBorder = &trbrdrh;
        break;
    case RTFtrbrdrv:
        setCurrentBorder = TRUE;
        pcurrentBorder = &trbrdrv;
        break;
    default:
        MX_ERROR_THROW(err, MX_INTERNAL_RTF_ERROR);
        break;
    }

    if (setCurrentBorder) {
        currentBorder = control;
        pcurrentBorder->reset();
    }

abort:;
}

/*-------------------------------------------------
 * FUNCTION: RTFScell::processControl
 *
 * DESCRIPTION:
 *
 *
 */

void RTFScell::processControl(int& err,
    rtfControl control,
    int controlValue)

{
    bool setCurrentBorder = FALSE;
    RTFSparBorder* pcurrentBorder;

    switch (control) {
    case RTFclmgf:
        clmgf = controlValue;
        break;
    case RTFclmrg:
        clmrg = controlValue;
        break;
    case RTFclbrdrt:
        setCurrentBorder = TRUE;
        pcurrentBorder = &clbrdrt;
        break;
    case RTFclbrdrl:
        setCurrentBorder = TRUE;
        pcurrentBorder = &clbrdrl;
        break;
    case RTFclbrdrb:
        setCurrentBorder = TRUE;
        pcurrentBorder = &clbrdrb;
        break;
    case RTFclbrdrr:
        setCurrentBorder = TRUE;
        pcurrentBorder = &clbrdrr;
        break;
    default:
        cellshad.processControl(err, control, controlValue);
        MX_ERROR_CHECK(err);
        break;
    }

    if (setCurrentBorder) {
        pcurrentBorder->reset();
        currentBorder = control;
    }

abort:;
}

/*-------------------------------------------------
 * FUNCTION: RTFScell::getCurrentBorder
 *
 * DESCRIPTION:
 *
 *
 */

void RTFScell::getCurrentBorder(int& err,
    RTFSparBorder** pcurrentBorder)
{
    switch (currentBorder) {
    case RTFclbrdrt:
        *pcurrentBorder = &clbrdrt;
        break;
    case RTFclbrdrl:
        *pcurrentBorder = &clbrdrl;
        break;
    case RTFclbrdrb:
        *pcurrentBorder = &clbrdrb;
        break;
    case RTFclbrdrr:
        *pcurrentBorder = &clbrdrr;
        break;
    default:
        MX_ERROR_THROW(err, MX_INTERNAL_RTF_ERROR);
        break;
    }
abort:;
}

/*-------------------------------------------------
 * FUNCTION: RTFSparFrame::processControl
 *
 * DESCRIPTION:
 *
 *
 */

void RTFSparFrame::processControl(int& err,
    rtfControl control,
    int controlValue)
{
    switch (control) {
    case RTFabsw:
        absw = controlValue;
        break;
    case RTFabsh:
        absh = controlValue;
        break;
    case RTFnowrap:
        nowrap = controlValue;
        break;
    case RTFdxfrtext:
        dxfrtext = controlValue;
        break;
    case RTFdfrmtxtx:
        dfrmtxtx = controlValue;
        break;
    case RTFdfrmtxty:
        dfrmtxty = controlValue;
        break;
    case RTFdropcapli:
        dropcapli = controlValue;
        break;
    case RTFdropcapt:
        dropcapt = (RTFEdropcap)controlValue;
        break;
    case RTFposx:
        posx = controlValue;
        break;
    case RTFposy:
        posy = controlValue;
        break;
    case RTFphmrg:
    case RTFphpg:
    case RTFphcol:
        hframe = control;
        break;
    case RTFposxc:
    case RTFposxi:
    case RTFposxo:
    case RTFposxr:
    case RTFposxl:
        hdist = control;
        break;
    case RTFpvmrg:
    case RTFpvpg:
    case RTFpvpara:
        vframe = control;
        break;
    case RTFposyc:
    case RTFposyil:
    case RTFposyt:
    case RTFposyb:
        vdist = control;
        break;
    default:
        break;
    }
}

/*-------------------------------------------------
 * FUNCTION: RTFSfield::processControl
 *
 * DESCRIPTION: processes control words associated with a field
 *
 *
 */

void RTFSfield::processControl(int& err,
    rtfControl control,
    int controlValue)
{
    switch (control) {
    case RTFflddirty:
        dirty = TRUE;
        break;
    case RTFfldedit:
        edit = TRUE;
        break;
    case RTFfldlock:
        lock = TRUE;
        break;
    case RTFfldpriv:
        priv = TRUE;
        break;
    default:
        break;
    }
}

/*-------------------------------------------------
 * FUNCTION: RTFSchrfmt::processControl
 *
 * DESCRIPTION:
 *
 *
 */

void RTFSchrfmt::processControl(int& err,
    rtfControl control,
    int controlValue)
{
    switch (control) {
    case RTFcaps:
        caps = controlValue;
        break;
    case RTFb:
        b = controlValue;
        break;
    case RTFdeleted:
        deleted = controlValue;
        break;
    case RTFi:
        i = controlValue;
        break;
    case RTFoutl:
        outl = controlValue;
        break;
    case RTFscaps:
        scaps = controlValue;
        break;
    case RTFshad:
        shad = controlValue;
        break;
    case RTFstrike:
        strike = controlValue;
        break;
    case RTFv:
        v = controlValue;
        break;
    case RTFul:
        ul = controlValue;
        break;
    case RTFuld:
    case RTFulnone:
    case RTFuldb:
    case RTFulw:
        ult = control;
        break;
    case RTFdn:
        dn = controlValue;
        break;
    case RTFup:
        up = controlValue;
        break;
    case RTFsub:
        sub = controlValue;
        break;
    case RTFsuper:
        super = controlValue;
        break;
    case RTFnosupersub:
        nosupersub = controlValue;
        break;
    case RTFexpnd:
        expnd = controlValue;
        break;
    case RTFexpndtw:
        expndtw = controlValue;
        break;
    case RTFkerning:
        kerning = controlValue;
        break;
    case RTFf:
        f = controlValue;
        break;
    case RTFfs:
        fs = controlValue;
        break;
    case RTFcf:
        cf = controlValue;
        break;
    case RTFcb:
        cb = controlValue;
        break;
    case RTFlang:
        lang = (RTFElang)controlValue;
        break;
    default:
        break;
    }
}

/*-------------------------------------------------
 * FUNCTION: RTFSdocfmt::processControl
 *
 * DESCRIPTION:
 *
 *
 */

void RTFSdocfmt::processControl(int& err,
    rtfControl control,
    int controlValue)
{
    switch (control) {
    case RTFansi:
    case RTFmac:
    case RTFpc:
    case RTFpca:
        charset = control;
        break;
    case RTFdeff:
        deff = controlValue;
        break;
    case RTFdeftab:
        deftab = controlValue;
        break;
    case RTFhyphhotz:
        hyphhotz = controlValue;
        break;
    case RTFhyphconsec:
        hyphconsec = controlValue;
        break;
    case RTFhyphcaps:
        hyphcaps = controlValue;
        break;
    case RTFhyphauto:
        hyphauto = controlValue;
        break;
    case RTFlinestart:
        linestart = controlValue;
        break;
    case RTFdeflang:
        deflang = (RTFElang)controlValue;
        break;
    default:
        break;
    }
}

/*-------------------------------------------------
 * FUNCTION: RTFSsecfmt::processControl
 *
 * DESCRIPTION:
 *
 *
 */

void RTFSsecfmt::processControl(int& err,
    rtfControl control,
    int controlValue)
{
    switch (control) {
    case RTFsbknone:
    case RTFsbkcol:
    case RTFsbkpage:
    case RTFsbkeven:
    case RTFsbkodd:
        sectionBreak = control;
        break;
    case RTFlineppage:
    case RTFlinecont:
        linerestartType = control;
        break;
    case RTFpgncont:
        pgnrestartType = control;
        break;
    case RTFpgndec:
    case RTFpgnucrm:
    case RTFpgnlcrm:
    case RTFpgnucltr:
    case RTFpgnlcltr:
        pgntype = control;
        break;
    case RTFpgnhnsh:
    case RTFpgnhnsp:
    case RTFpgnhnsc:
    case RTFpgnhnsm:
    case RTFpgnhnsn:
        pgnhns = control;
        break;
    case RTFvertalb:
    case RTFvertalt:
    case RTFvertalc:
    case RTFvertalj:
        verta = control;
        break;
    case RTFendnhere:
        endnhere = controlValue;
        break;
    case RTFcols:
        cols = controlValue;
        break;
    case RTFcolsx:
        colsx = controlValue;
        break;
    case RTFcolno:
        colno = controlValue;
        break;
    case RTFcolsr:
        colsr = controlValue;
        break;
    case RTFcolw:
        colw = controlValue;
        break;
    case RTFlinebetcol:
        linebetcol = controlValue;
        break;
    case RTFlinemod:
        linemod = controlValue;
        break;
    case RTFlinex:
        linex = controlValue;
        break;
    case RTFlinestarts:
        linestarts = controlValue;
        break;
    case RTFpgnx:
        pgnx = controlValue;
        break;
    case RTFpgny:
        pgny = controlValue;
        break;
    case RTFpgnhn:
        pgnhn = controlValue;
        break;
    default:
        break;
    }
}

/*-------------------------------------------------
 * FUNCTION: RTFSdocfootnote::processControl
 *
 * DESCRIPTION:
 *
 *
 */

void RTFSdocfootnote::processControl(int& err,
    rtfControl control,
    int controlValue)
{
    switch (control) {
    case RTFendnotes:
    case RTFenddoc:
    case RTFftntj:
    case RTFftnbj:
        endnotes = control;
        break;
    case RTFaendnotes:
    case RTFaenddoc:
    case RTFaftntj:
    case RTFaftnbj:
        aendnotes = control;
        break;
    case RTFftnrstpg:
    case RTFftnrestart:
    case RTFftnrstcont:
        ftnr = control;
        break;
    case RTFaftnrestart:
    case RTFaftnrstcont:
        aftnr = control;
        break;
    case RTFftnnar:
    case RTFftnnalc:
    case RTFftnnauc:
    case RTFftnnrlc:
    case RTFftnnruc:
    case RTFftnnchi:
        ftnn = control;
        break;
    case RTFaftnnar:
    case RTFaftnnalc:
    case RTFaftnnauc:
    case RTFaftnnrlc:
    case RTFaftnnruc:
    case RTFaftnnchi:
        aftnn = control;
        break;
    case RTFftnstart:
        ftnstart = controlValue;
        break;
    case RTFaftnstart:
        aftnstart = controlValue;
        break;
    default:
        break;
    }
}

/*-------------------------------------------------
 * FUNCTION: RTFSpict::processControl
 *
 * DESCRIPTION:
 *
 *
 */

void RTFSpict::processControl(int& err,
    rtfControl control,
    int controlValue)

{
    err = MX_ERROR_OK;

    switch (control) {
    case RTFpict:
        reset();
        MX_ERROR_CHECK(err);
        inPict = TRUE;
        break;
    case RTFmacpict:
    case RTFpmmetafile:
    case RTFwmetafile:
    case RTFdibitmap:
    case RTFwbitmap:
        picttype = control;
        picttypeQualifier = controlValue;
        break;
    case RTFwbmbitspixel:
        wbmbitspixel = controlValue;
        break;
    case RTFwbmplanes:
        wbmplanes = controlValue;
        break;
    case RTFwbmwidthbytes:
        wbmwidthbytes = controlValue;
        break;
    case RTFpicw:
        picw = controlValue;
        break;
    case RTFpich:
        pich = controlValue;
        break;
    case RTFpicwgoal:
        picwgoal = controlValue;
        break;
    case RTFpichgoal:
        pichgoal = controlValue;
        break;
    case RTFpicscalex:
        picscalex = controlValue;
        break;
    case RTFpicscaley:
        picscaley = controlValue;
        break;
    case RTFpicscaled:
        picscaled = TRUE;
        break;
    case RTFpiccropt:
        piccropt = controlValue;
        break;
    case RTFpiccropb:
        piccropb = controlValue;
        break;
    case RTFpiccropr:
        piccropr = controlValue;
        break;
    case RTFpiccropl:
        piccropl = controlValue;
        break;
    case RTFpicbmp:
        picbmp = TRUE;
        break;
    case RTFpicbpp:
        picbpp = controlValue;
        break;
    case RTFbin:
        bin = TRUE;
        break;
    default:
        MX_ERROR_THROW(err, MX_INTERNAL_RTF_ERROR);
        break;
    }

abort:;
}
