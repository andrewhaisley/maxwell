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
 * MODULE : rtf.C
 *
 * AUTHOR : David Miller
 *
 * This file
 *
 * DESCRIPTION:
 * Module rtf.C
 *
 *
 *
 *
 */

#define RTF_DEFINING_CONTROLS

#include <rtf.h>
#include <rtflocal.h>

// Global control list

std::map<std::string, RTFcontrol *> mx_rtf::m_controls; // Control dictionary

/*-------------------------------------------------
 * FUNCTION: mx_rtf
 *
 * DESCRIPTION:
 *
 *
 */

mx_rtf::mx_rtf(int& err, const char* filename)
{
    file_offset = 0;
    iif = fopen(filename, "r");

    if (iif == NULL) {
        MX_ERROR_THROW(err, MX_ERROR_RTF_FILE_ERROR);
    }

    textBuffer = NULL;
    textBufferSize = 0;
    allocTextBufferSize = 0;
    controlBufferSize = 0;
    readingType = RTFreadingText;
    readingStarredDestination = FALSE;
    skippingGroup = FALSE;

    currentFont = NULL;
    currentStyle = NULL;
    currentStyle = NULL;
    currentColour = NULL;
    currentColourCount = 0;

    for (int ictl = 0; ictl < nRTFcontrols; ictl++) {
        m_controls[Controls[ictl].name] = Controls + ictl;
    }

abort:
    return;
}

/*-------------------------------------------------
 * FUNCTION: mx_rtf::~mx_rtf
 *
 * DESCRIPTION:
 *
 *
 */

mx_rtf::~mx_rtf()
{
    int err = MX_ERROR_OK;
    RTFSgroup* group;
    RTFSfont* font;
    RTFScolour* colour;
    RTFSstyle* style;
    RTFSfile* file;
    int numItems, iitem;

    if (iif != NULL)
        fclose(iif);

    delete[] textBuffer;
    delete currentFont;
    delete currentStyle;
    delete currentStyle;
    delete currentColour;

    // Clean out the groups
    numItems = groups.get_num_items() - 1;
    for (iitem = numItems; iitem >= 0; iitem--) {
        group = (RTFSgroup*)groups.remove(err, iitem);
        MX_ERROR_CHECK(err);
        delete group;
    }

    for (auto i : m_tables) {
        switch (i.first % RTFTnumberTables) {
        case RTFTfonts:
            font = (RTFSfont*)i.second;
            delete font;
            break;
        case RTFTcolours:
            colour = (RTFScolour*)i.second;
            delete colour;
            break;
        case RTFTfiles:
            file = (RTFSfile*)i.second;
            delete file;
            break;
        case RTFTsectionStyle:
        case RTFTparagraphStyle:
        case RTFTcharacterStyle:
            style = (RTFSstyle*)i.second;
            delete style;
            break;
        default:
            break;
        }
    }
    return;
abort:
    return;
}

/*-------------------------------------------------
 * FUNCTION: RTFSfont::RTFSfont
 *
 * DESCRIPTION: Font constructor
 *
 *
 */

RTFSfont::RTFSfont()
{
    fontname = NULL;
    falt = NULL;
    fontfamily = RTFfnil;
    fcharset = RTF_CHARSET_ANSI;
    fprq = RTF_DEFAULT_PITCH;
    f = -1;
}

/*-------------------------------------------------
 * FUNCTION: RTFSfont::~RTFSfont
 *
 * DESCRIPTION: Font destructor
 *
 *
 */

RTFSfont::~RTFSfont()
{
    delete[] fontname;
    delete[] falt;
}

/*-------------------------------------------------
 * FUNCTION: RTScolour::RTFScolour
 *
 * DESCRIPTION: Colour constructor
 *
 *
 */

RTFScolour::RTFScolour()
{
    offset = red = green = blue = 0;
}

/*-------------------------------------------------
 * FUNCTION: RTFSfile::RTFSfile
 *
 * DESCRIPTION: File constructor
 *
 *
 */

RTFSfile::RTFSfile()
{
    filename = NULL;
    fid = -1;
    frelative = 0;
    filesource = RTFfvalidhpfs;
}

/*-------------------------------------------------
 * FUNCTION: RTFSfile::~RTFSfile
 *
 * DESCRIPTION: File constructor
 *
 *
 */

RTFSfile::~RTFSfile()
{
    delete[] filename;
}

/*-------------------------------------------------
 * FUNCTION: RTFSdocfootnote::RTFSdocfootnote
 *
 * DESCRIPTION: Footnote constructor
 *
 *
 */

RTFSdocfootnote::RTFSdocfootnote()
{
    footnote = RTF_FTN_NONE;
    ftnsep = NULL;
    ftnsepc = NULL;
    ftncn = NULL;
    aftnsep = NULL;
    aftnsepc = NULL;
    aftncn = NULL;
    endnotes = RTFendnotes;
    aendnotes = RTFaendnotes;
    ftnstart = 1;
    aftnstart = 1;
    ftnr = RTFftnrstcont;
    aftnr = RTFaftnrstcont;
    ftnn = RTFftnnar;
    aftnn = RTFaftnnar;
}

/*-------------------------------------------------
 * FUNCTION: RTFSdocfootnote::~RTFSdocfootnote
 *
 * DESCRIPTION: Footnote destructor
 *
 *
 */

RTFSdocfootnote::~RTFSdocfootnote()
{
    delete[] ftnsep;
    delete[] ftnsepc;
    delete[] ftncn;
    delete[] aftnsep;
    delete[] aftnsepc;
    delete[] aftncn;
}

/*-------------------------------------------------
 * FUNCTION: RTFSpage::RTFSpage
 *
 * DESCRIPTION: RTF page constructor
 *
 *
 */

RTFSpage::RTFSpage()
{
    reset();
}

/*-------------------------------------------------
 * FUNCTION: RTFSpage::reset
 *
 * DESCRIPTION: RTF page constructor
 *
 *
 */

void RTFSpage::reset()
{
    paperw = 12240;
    paperh = 15840;
    margl = 1800;
    margr = 1800;
    margt = 1440;
    margb = 1440;
    facingp = FALSE;
    gutter = 0;
    margmirror = FALSE;
    landscape = FALSE;
    pgnstart = 1;
    titlepg = FALSE;
    headery = 720;
    footery = 720;
}

/*-------------------------------------------------
 * FUNCTION: RTFSdocfmt::RTFSdocfmt
 *
 * DESCRIPTION:
 *
 *
 */

RTFSdocfmt::RTFSdocfmt()
{
    charset = RTFansi;
    deff = 0;
    deftab = 720;
    hyphhotz = 0;
    hyphconsec = 0;
    hyphcaps = TRUE;
    hyphauto = FALSE;
    linestart = 1;
    deflang = RTF_LANG_NONE;
}

/*-------------------------------------------------
 * FUNCTION: RTFSsecFmt::RTFSsecFmt
 *
 * DESCRIPTION: RTF section formatting properties
 *
 *
 */

RTFSsecfmt::RTFSsecfmt(RTFSpage* documentPage)
{
    reset(documentPage);
}

/*-------------------------------------------------
 * FUNCTION: RTFSsecfmt::reset
 *
 * DESCRIPTION: RTF section formatting properties
 *
 *
 */

void RTFSsecfmt::reset(RTFSpage* documentPage)
{
    endnhere = FALSE;
    sectionBreak = RTFsbknone;
    cols = 1;
    colsx = 720;
    colno = 0;
    colsr = 0;
    colw = 0;
    linebetcol = FALSE;
    linemod = 1;
    linex = 360;
    linestarts = 1;
    linerestart = FALSE;
    linerestartType = RTFunknownControl;
    pgnrestart = FALSE;
    pgnrestartType = RTFpgncont;
    pgnx = 720;
    pgny = 720;
    pgntype = RTFpgndec;
    pgnhn = 0;
    pgnhns = RTFpgnhnsh;
    verta = RTFvertalt;
    ds = -1;
    dss = NULL;

    if (documentPage == NULL) {
        // Set all the page data
        page.reset();
    } else {
        // Get everything from the document page
        page = *documentPage;
    }
}

/*-------------------------------------------------
 * FUNCTION: RTFSsection::RTFSsection
 *
 * DESCRIPTION: RTF section formatting properties
 *
 *
 */

RTFSsection::RTFSsection(RTFSpage* documentPage)
{
    reset(documentPage);
}

/*-------------------------------------------------
 * FUNCTION: RTFSsection::reset
 *
 * DESCRIPTION: RTF section formatting properties
 *
 *
 */

void RTFSsection::reset(RTFSpage* documentPage)
{
    fmt.reset(documentPage);
}

/*-------------------------------------------------
 * FUNCTION: RTFSparBorder::RTFSparborder
 *
 * DESCRIPTION: RTF border
 *
 *
 */

RTFSparBorder::RTFSparBorder()
{
    reset();
}

/*-------------------------------------------------
 * FUNCTION: RTFSparBorder::reset
 *
 * DESCRIPTION: RTF border
 *
 *
 */

void RTFSparBorder::reset()
{
    brdrk = RTFunknownControl;
    brdrw = 0;
    brdrcf = 0;
    brsp = 0;
}

/*-------------------------------------------------
 * FUNCTION: RTFSparfmt
 *
 * DESCRIPTION: Paragraph format constructor
 *
 *
 */

RTFSparfmt::RTFSparfmt()
{
    reset();
}

/*-------------------------------------------------
 * FUNCTION: RTFSparFmt
 *
 * DESCRIPTION: Paragraph format constructor
 *
 *
 */

void RTFSparfmt::reset()
{
    hyphpar = FALSE;
    intbl = FALSE;
    keep = FALSE;
    keepn = FALSE;
    level = -1;
    noline = FALSE;
    pagebb = FALSE;
    sbys = FALSE;
    alignment = RTFql;
    fi = 0;
    li = 0;
    ri = 0;
    sb = 0;
    sa = 0;
    sl = 0;
    slmult = 0;
    s = 0;
    ss = NULL;
}

/*-------------------------------------------------
 * FUNCTION: RTFSparFrame::RTFSparFrame
 *
 * DESCRIPTION:
 *
 *
 */

RTFSparFrame::RTFSparFrame()
{
    reset();
}

/*-------------------------------------------------
 * FUNCTION: RTFSparFrame::reset
 *
 * DESCRIPTION:
 *
 *
 */

void RTFSparFrame::reset()
{
    absw = 0;
    absh = 0;
    nowrap = 0;
    dxfrtext = 0;
    dfrmtxtx = 0;
    dfrmtxty = 0;
    dropcapli = 0;
    dropcapt = RTF_DROPCAP_INTEXT;
    posx = 0;
    posy = 0;
    hframe = RTFphcol;
    hdist = RTFposxl;
    vframe = RTFpvmrg;
    vdist = RTFposyt;
}

/*-------------------------------------------------
 * FUNCTION: RTFSparTab::RTFSparTab
 *
 * DESCRIPTION:
 *
 *
 */

RTFSparTab::RTFSparTab()
{
    reset();
}

/*-------------------------------------------------
 * FUNCTION: RTFSparTab::reset
 *
 * DESCRIPTION:
 *
 *
 */

void RTFSparTab::reset()
{
    tx = -1;
    tb = -1;
    tabkind = RTFunknownControl;
    tablead = RTFunknownControl;
}

/*-------------------------------------------------
 * FUNCTION: RTFSparShading::RTFSparShading
 *
 * DESCRIPTION:
 *
 *
 */

RTFSparShading::RTFSparShading()
{
    reset();
}

/*-------------------------------------------------
 * FUNCTION: RTFSparShading::reset
 *
 * DESCRIPTION:
 *
 *
 */

void RTFSparShading::reset()
{
    shading = -1;
    cfpat = -1;
    cbpat = -1;
    pat = RTFunknownControl;
}

/*-------------------------------------------------
 * FUNCTION: RTFSpar::RTFSpar
 *
 * DESCRIPTION:
 *
 *
 */

RTFSpar::RTFSpar(int& err)
{
    tabsAreStyleTabs = FALSE;
    currentBorder = RTFunknownControl;
}

/*-------------------------------------------------
 * FUNCTION: RTFSpar::RTFSpar
 *
 * DESCRIPTION:
 *
 *
 */

RTFSpar::RTFSpar(int& err, RTFSpar& par)
{
    this->equate(err, par);
    MX_ERROR_CHECK(err);

    return;
abort:
    return;
}
/*-------------------------------------------------
 * FUNCTION: RTFSpar::=
 *
 * DESCRIPTION:
 *
 *
 */

RTFSpar& RTFSpar::equate(int& err, RTFSpar& par)
{
    RTFSparTab *newTab;

    MX_ERROR_CHECK(err);

    tabsAreStyleTabs = par.tabsAreStyleTabs;
    currentBorder = RTFunknownControl;

    deleteTabs();

    /* Copy the tabs */
    for (auto i : par.tabs)
    {
        newTab = new RTFSparTab;
        *newTab = *i.second;
        tabs[i.first] = newTab;
    }

    brdrt = par.brdrt;
    brdrb = par.brdrb;
    brdrl = par.brdrl;
    brdrr = par.brdrr;
    brdrbtw = par.brdrbtw;
    brdrbar = par.brdrbar;
    box = par.box;

    fmt = par.fmt;
    frame = par.frame;
    shading = par.shading;
abort:
    return *this;
}

/*-------------------------------------------------
 * FUNCTION: RTFSpar::RTFSpar
 *
 * DESCRIPTION:
 *
 *
 */

RTFSpar::~RTFSpar()
{
    reset();
}

/*-------------------------------------------------
 * FUNCTION: RTFSpar::RTFSpar
 *
 * DESCRIPTION:
 *
 *
 */

void RTFSpar::reset()
{
    deleteTabs();

    fmt.reset();
    frame.reset();
    shading.reset();

    currentBorder = RTFunknownControl;
    brdrt.reset();
    brdrb.reset();
    brdrl.reset();
    brdrr.reset();
    brdrbtw.reset();
    brdrbar.reset();
    box.reset();
}

/*-------------------------------------------------
 * FUNCTION: RTFSpar::deleteTabs
 *
 * DESCRIPTION: delete the tabs from this formatting
 *
 *
 */

void RTFSpar::deleteTabs()
{
    for (auto i : tabs)
    {
        delete i.second;
    }

    tabs.clear();
}

/*-------------------------------------------------
 * FUNCTION: RTFScell::reset
 *
 * DESCRIPTION:
 *
 *
 */

void RTFScell::reset()
{
    clmgf = FALSE;
    clmrg = FALSE;
    cellx = 0;
    currentBorder = RTFunknownControl;
    clbrdrt.reset();
    clbrdrl.reset();
    clbrdrb.reset();
    clbrdrr.reset();
    cellshad.reset();
}

/*-------------------------------------------------
 * FUNCTION: RTFScell::RTFScell
 *
 * DESCRIPTION:
 *
 *
 */

RTFScell::RTFScell()
{
    reset();
}

/*-------------------------------------------------
 * FUNCTION: RTFStable::RTFStable
 *
 * DESCRIPTION:
 *
 *
 */

RTFStable::RTFStable()
{
    reset();
}

/*-------------------------------------------------
 * FUNCTION: RTFStable::RTFStable
 *
 * DESCRIPTION:
 *
 *
 */

RTFStable::~RTFStable()
{
    reset();
}

/*-------------------------------------------------
 * FUNCTION: RTFStable::RTFStable
 *
 * DESCRIPTION:
 *
 *
 */

void RTFStable::reset()
{
    int err = MX_ERROR_OK;
    int iitem, numItems;
    RTFScell* cell;

    trgaph = 0;
    rowjust = RTFunknownControl;
    trleft = 0;
    trrh = 0;
    trhdr = FALSE;
    trkeep = FALSE;
    currentBorder = RTFunknownControl;
    inTable = FALSE;
    inCell = FALSE;
    cellCount = 0;

    trbrdrt.reset();
    trbrdrl.reset();
    trbrdrb.reset();
    trbrdrr.reset();
    trbrdrh.reset();
    trbrdrv.reset();
    currentCell.reset();

    // Clean out the cells
    numItems = cells.get_num_items() - 1;
    for (iitem = numItems; iitem >= 0; iitem--) {
        cell = (RTFScell*)cells.remove(err, iitem);
        MX_ERROR_CHECK(err);
        delete cell;
    }
abort:
    return;
}

/*-------------------------------------------------
 * FUNCTION: RTFSfield::RTFSfield
 *
 * DESCRIPTION:
 *
 *
 */

RTFSfield::RTFSfield()
{
    reset();
}

/*-------------------------------------------------
 * FUNCTION: RTFSfield::reset
 *
 * DESCRIPTION: resets the object to its default state
 *
 *
 */

void RTFSfield::reset()
{
    dirty = edit = lock = priv = FALSE;
}

/*-------------------------------------------------
 * FUNCTION: RTFSpict::RTFSpict
 *
 * DESCRIPTION:
 *
 *
 */

RTFSpict::RTFSpict()
{
    reset();
}

/*-------------------------------------------------
 * FUNCTION: RTFSpict::reset()
 *
 * DESCRIPTION: resets the object to its default state
 *
 *
 */

void RTFSpict::reset()
{
    inPict = FALSE;

    border.reset();
    shading.reset();

    picttype = RTFwbitmap;
    picttypeQualifier = 0;
    wbmbitspixel = 0;
    wbmplanes = 0;
    wbmwidthbytes = 0;
    picw = 0;
    pich = 0;
    picwgoal = 0;
    pichgoal = 0;
    picscalex = 100;
    picscaley = 100;
    picscaled = FALSE;
    piccropt = 0;
    piccropb = 0;
    piccropl = 0;
    piccropr = 0;
    picbmp = FALSE;
    picbpp = 0;
    bin = FALSE;
}

/*-------------------------------------------------
 * FUNCTION: RTFSchrfmt::RTFSchrfmt
 *
 * DESCRIPTION:
 *
 *
 */

RTFSchrfmt::RTFSchrfmt()
{
    reset();
}

/*-------------------------------------------------
 * FUNCTION: RTFSchrfmt::reset
 *
 * DESCRIPTION:
 *
 *
 */

void RTFSchrfmt::reset()
{
    caps = FALSE;
    b = FALSE;
    deleted = FALSE;
    dn = 6;
    sub = FALSE;
    super = FALSE;
    nosupersub = FALSE;
    expnd = -1;
    expndtw = -1;
    kerning = -1;
    f = -1;
    font = NULL;
    fs = 24;
    i = FALSE;
    outl = FALSE;
    scaps = FALSE;
    shad = FALSE;
    strike = FALSE;
    ul = FALSE;
    ult = RTFunknownControl;
    up = 6;
    v = FALSE;
    cf = -1;
    cfs = NULL;
    cb = -1;
    cbs = NULL;
    cs = -1;
    css = NULL;
    lang = RTF_LANG_NONE;
}

/*-------------------------------------------------
 * FUNCTION: RTFScharacter::RTFScharacter
 *
 * DESCRIPTION:
 *
 *
 */

RTFScharacter::RTFScharacter()
{
}

/*-------------------------------------------------
 * FUNCTION: RTFScharacter::reset
 *
 * DESCRIPTION:
 *
 *
 */

void RTFScharacter::reset()
{
    fmt.reset();
}

/*-------------------------------------------------
 * FUNCTION: RTFSformatting::RTFSformatting
 *
 * DESCRIPTION:
 *
 *
 */

RTFSformatting::RTFSformatting(int& err, RTFSpage* documentPage)
    : section(documentPage)
    , paragraph(err)
{
    MX_ERROR_CHECK(err);
    return;
abort:
    return;
}

/*-------------------------------------------------
 * FUNCTION: RTFSformatting::RTFSformatting
 *
 * DESCRIPTION:
 *
 *
 */

RTFSformatting::RTFSformatting(int& err, const RTFSformatting& formatting)
    : paragraph(err)
{
    MX_ERROR_CHECK(err);

    this->equate(err, formatting);
    MX_ERROR_CHECK(err);

    return;
abort:
    return;
}

/*-------------------------------------------------
 * FUNCTION: RTFSformatting::=
 *
 * DESCRIPTION:
 *
 *
 */

RTFSformatting& RTFSformatting::equate(int& err, const RTFSformatting& formatting)
{
    paragraph.equate(err, (RTFSpar&)formatting.paragraph);
    MX_ERROR_CHECK(err);

    character = formatting.character;
    section = formatting.section;

abort:
    return *this;
}

/*-------------------------------------------------
 * FUNCTION: RTFSformatting::reset
 *
 * DESCRIPTION:
 *
 *
 */

void RTFSformatting::reset(RTFSpage* documentPage)
{
    paragraph.reset();
    section.reset(documentPage);
    character.reset();
}

/*-------------------------------------------------
 * FUNCTION: RTFSsytle::RTFSstyle
 *
 * DESCRIPTION:
 *
 *
 */

RTFSstyle::RTFSstyle(int& err, RTFSpage* documentPage)
    : formatting(err, documentPage)
{
    MX_ERROR_CHECK(err);

    stylename = NULL;
    styledef = RTFs;
    additive = FALSE;
    snext = -1;
    styleType = RTFTparagraphStyle;
    styleNumber = 0;
    sbasedon = 222;
    updated = FALSE;
abort:
    return;
}

/*-------------------------------------------------
 * FUNCTION: RTFSsytle::~RTFSstyle
 *
 * DESCRIPTION:
 *
 *
 */

RTFSstyle::~RTFSstyle()
{
    int iitem, numItems;
    int err = MX_ERROR_OK;
    RTFcontrol* control;

    delete[] stylename;

    // Clean out the groups
    numItems = setControls.get_num_items() - 1;
    for (iitem = numItems; iitem >= 0; iitem--) {
        control = (RTFcontrol*)setControls.remove(err, iitem);
        MX_ERROR_CHECK(err);
        delete control;
    }
    return;
abort:
    global_error_trace->print();
}

/*-------------------------------------------------
 * FUNCTION: RTFSsytle::update
 *
 * DESCRIPTION:
 *
 *
 */

void RTFSstyle::update(int& err, std::map<rtfTableType, void *> &tables)
{
    int numItems, iitem;
    RTFcontrol* rtfcontrol;
    RTFSstyle* basedOnStyle;
    static RTFSstyle* defaultStyle = NULL;

    // If already updated return
    if (updated)
        return;

    // Get the style we are based on
    if (sbasedon != 222) {

        rtfTableType k = static_cast<rtfTableType>((sbasedon * RTFTnumberTables) + styleType);

        if (tables.find(k) != tables.end()) {
            basedOnStyle = static_cast<RTFSstyle *>(tables[k]);
        } else {
            return;
        }
    
    } else {
        // get the default style
        if (defaultStyle == NULL) {
            defaultStyle = new RTFSstyle(err);
            MX_ERROR_CHECK(err);
            defaultStyle->updated = TRUE;
        }
        basedOnStyle = defaultStyle;
    }

    // Update the based on style if needed

    basedOnStyle->update(err, tables);
    MX_ERROR_CHECK(err);

    // Copy the style (though not paragraph properties or tabs which are held
    // in the parameters for this style (in Word anyway)). Also not the style
    // number or the pointer to this style
    switch (styleType) {
    case RTFTparagraphStyle: {
        RTFSparfmt oldParfmt = formatting.paragraph.fmt;

        formatting.paragraph.equate(err, basedOnStyle->formatting.paragraph);
        MX_ERROR_CHECK(err);

        formatting.paragraph.fmt = oldParfmt;
        formatting.paragraph.fmt.s = this->styleNumber;
        formatting.paragraph.fmt.ss = this;

        formatting.paragraph.deleteTabs();
        break;
    }
    case RTFTcharacterStyle:
        formatting.character = basedOnStyle->formatting.character;
        formatting.character.fmt.cs = this->styleNumber;
        formatting.character.fmt.css = this;
        break;
    case RTFTsectionStyle:
        formatting.section = basedOnStyle->formatting.section;
        formatting.section.fmt.ds = this->styleNumber;
        formatting.section.fmt.dss = this;
        break;
    default:
        break;
    }

    // Now loop around setting the values
    numItems = setControls.get_num_items();
    for (iitem = 0; iitem < numItems; iitem++) {
        rtfcontrol = (RTFcontrol*)setControls.get(err, iitem);
        MX_ERROR_CHECK(err);

        formatting.processControl(err,
            tables,
            rtfcontrol->destination,
            rtfcontrol->control,
            rtfcontrol->value);
        MX_ERROR_CHECK(err);
    }

    updated = TRUE;

    return;
abort:
    return;
}

/*-------------------------------------------------
 * FUNCTION: RTFSgroup::RTFSgroup
 *
 * DESCRIPTION:
 *
 *
 */

RTFSgroup::RTFSgroup(int& err, RTFSpage* documentPage)
    : formatting(err, documentPage)
{
    MX_ERROR_CHECK(err);
    destination = RTFDunknownDestination;
    skippingRoot = FALSE;

    return;
abort:
    return;
}

/*-------------------------------------------------
 * FUNCTION: RTFSgroup::RTFSgroup
 *
 * DESCRIPTION:
 *
 *
 */

RTFSgroup::RTFSgroup(int& err, RTFSgroup& group)
    : formatting(err)
{
    MX_ERROR_CHECK(err);

    this->equate(err, group);
    MX_ERROR_CHECK(err);

    return;
abort:
    return;
}

/*-------------------------------------------------
 * FUNCTION: RTFSgroup::=
 *
 * DESCRIPTION:
 *
 *
 */

RTFSgroup& RTFSgroup::equate(int& err, const RTFSgroup& group)
{
    destination = group.destination;
    skippingRoot = FALSE;

    formatting.equate(err, group.formatting);
    MX_ERROR_CHECK(err);

abort:
    return *this;
}

void RTFScell::merge(int& err, RTFScell& a_cell)
{
}

void mx_rtf::update_style(int& err, RTFSstyle* st)
{
    st->update(err, m_tables);
    MX_ERROR_CHECK(err);
abort:;
}
