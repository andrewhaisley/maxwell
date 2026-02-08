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
 * MODULE : rtflocal.h
 *
 * AUTHOR : David Miller
 *
 * This file
 *
 * DESCRIPTION:
 * Module rtflocal.h
 *
 *
 *
 *
 */

#ifndef _RTFLOCAL_H
#define _RTFLOCAL_H

#include <rtf.h>

#ifdef RTF_DEFINING_CONTROLS

RTFcontrol Controls[] = {
    // Version
    { "rtf", RTFrtf, RTFDrtf, RTFCdestination, 1 },

    // Default font
    { "deff", RTFdeff, RTFDdocfmt, RTFCvalue, 0 },

    // Charset
    { "ansi", RTFansi, RTFDdocfmt, RTFCflag, 0 },
    { "mac", RTFmac, RTFDdocfmt, RTFCflag, 0 },
    { "pc", RTFpc, RTFDdocfmt, RTFCflag, 0 },
    { "pca", RTFpca, RTFDdocfmt, RTFCflag, 0 },

    // Font attributes
    { "fonttbl", RTFfonttbl, RTFDfonttbl, RTFCdestination, 0 },
    { "f", RTFf, RTFDchrfmt, RTFCvalue, 0 },
    { "fnil", RTFfnil, RTFDfonttbl, RTFCflag, 0 },
    { "froman", RTFfroman, RTFDfonttbl, RTFCflag, 0 },
    { "fswiss", RTFfswiss, RTFDfonttbl, RTFCflag, 0 },
    { "fmodern", RTFfmodern, RTFDfonttbl, RTFCflag, 0 },
    { "fscript", RTFfscript, RTFDfonttbl, RTFCflag, 0 },
    { "fdecor", RTFfdecor, RTFDfonttbl, RTFCflag, 0 },
    { "ftech", RTFftech, RTFDfonttbl, RTFCflag, 0 },
    { "fbidi", RTFfbidi, RTFDfonttbl, RTFCflag, 0 },
    { "fcharset", RTFfcharset, RTFDfonttbl, RTFCvalue, 0 },
    { "fprq", RTFfprq, RTFDfonttbl, RTFCvalue, 0 },
    { "cpg", RTFcpg, RTFDfonttbl, RTFCvalue, 0 },

    // Alternative font name
    { "falt", RTFfalt, RTFDfalt, RTFCdestination, 0 },

    // Font embedding
    { "fontemb", RTFfontemb, RTFDfontemb, RTFCdestination, 0 },
    { "ftnil", RTFftnil, RTFDfontemb, RTFCflag, 0 },
    { "fttruetype", RTFfttruetype, RTFDfontemb, RTFCflag, 0 },

    // Font file
    { "fontfile", RTFfontfile, RTFDfontfile, RTFCdestination, 0 },

    // File attributes
    { "filetbl", RTFfiletbl, RTFDfiletbl, RTFCdestination, 0 },
    { "file", RTFfile, RTFDfile, RTFCdestination, 0 },
    { "fid", RTFfid, RTFDfile, RTFCvalue, 0 },
    { "frelative", RTFfrelative, RTFDfile, RTFCvalue, 0 },
    { "fosnum", RTFfosnum, RTFDfile, RTFCvalue, 0 },
    { "fvalidmac", RTFfvalidmac, RTFDfile, RTFCflag, 0 },
    { "fvaliddos", RTFfvaliddos, RTFDfile, RTFCflag, 0 },
    { "fvalidntfs", RTFfvalidntfs, RTFDfile, RTFCflag, 0 },
    { "fvalidhpfs", RTFfvalidhpfs, RTFDfile, RTFCflag, 0 },
    { "fnetwork", RTFfnetwork, RTFDfile, RTFCflag, 0 },

    // Colour table
    { "colortbl", RTFcolortbl, RTFDcolortbl, RTFCdestination, 0 },
    { "red", RTFred, RTFDcolortbl, RTFCvalue, 0 },
    { "green", RTFgreen, RTFDcolortbl, RTFCvalue, 0 },
    { "blue", RTFblue, RTFDcolortbl, RTFCvalue, 0 },

    // Stylesheet
    { "stylesheet", RTFstylesheet, RTFDstylesheet, RTFCdestination, 0 },
    { "s", RTFs, RTFDparfmt, RTFCvalue, 0 },
    { "cs", RTFcs, RTFDchrfmt, RTFCvalue, 0 },
    { "ds", RTFds, RTFDsecfmt, RTFCvalue, 0 },
    { "additive", RTFadditive, RTFDstylesheet, RTFCflag, 0 },
    { "sbasedon", RTFsbasedon, RTFDstylesheet, RTFCvalue, 222 },
    { "snext", RTFsnext, RTFDstylesheet, RTFCvalue, 0 },

    // Keycodes
    { "keycode", RTFkeycode, RTFDkeycode, RTFCdestination, 0 },
    { "shift", RTFshift, RTFDkeycode, RTFCflag, 0 },
    { "ctrl", RTFctrl, RTFDkeycode, RTFCflag, 0 },
    { "alt", RTFalt, RTFDkeycode, RTFCflag, 0 },
    { "fn", RTFfn, RTFDkeycode, RTFCvalue, 0 },

    // Revtbl
    { "revtbl", RTFrevtbl, RTFDrevtbl, RTFCdestination, 0 },

    // Info
    { "info", RTFinfo, RTFDinfo, RTFCdestination, 0 },
    { "title", RTFtitle, RTFDtitle, RTFCdestination, 0 },
    { "subject", RTFsubject, RTFDsubject, RTFCdestination, 0 },
    { "author", RTFauthor, RTFDauthor, RTFCdestination, 0 },
    { "operator", RTFoperator, RTFDoperator, RTFCdestination, 0 },
    { "keywords", RTFkeywords, RTFDkeywords, RTFCdestination, 0 },
    { "comment", RTFcomment, RTFDcomment, RTFCdestination, 0 },
    { "doccomm", RTFdoccomm, RTFDdoccomm, RTFCdestination, 0 },
    { "creatim", RTFcreatim, RTFDcreatim, RTFCdestination, 0 },
    { "revtim", RTFrevtim, RTFDrevtim, RTFCdestination, 0 },
    { "printim", RTFprintim, RTFDprintim, RTFCdestination, 0 },
    { "buptim", RTFbuptim, RTFDbuptim, RTFCdestination, 0 },

    { "version", RTFversion, RTFDinfo, RTFCvalue, 0 },
    { "vern", RTFvern, RTFDinfo, RTFCvalue, 0 },
    { "edmins", RTFedmins, RTFDinfo, RTFCvalue, 0 },
    { "nofpages", RTFnofpages, RTFDinfo, RTFCvalue, 0 },
    { "nofwords", RTFnofwords, RTFDinfo, RTFCvalue, 0 },
    { "nofchars", RTFnofchars, RTFDinfo, RTFCvalue, 0 },
    { "id", RTFid, RTFDinfo, RTFCvalue, 0 },
    { "yr", RTFyr, RTFDinfo, RTFCvalue, 0 },
    { "mo", RTFmo, RTFDinfo, RTFCvalue, 0 },
    { "dy", RTFdy, RTFDinfo, RTFCvalue, 0 },
    { "hr", RTFhr, RTFDinfo, RTFCvalue, 0 },
    { "min", RTFmin, RTFDinfo, RTFCvalue, 0 },
    { "sec", RTFsec, RTFDinfo, RTFCvalue, 0 },

    // Document formatting
    { "deftab", RTFdeftab, RTFDdocfmt, RTFCvalue, 720 },
    { "hyphhotz", RTFhyphhotz, RTFDdocfmt, RTFCvalue, 0 },
    { "hyphconsec", RTFhyphconsec, RTFDdocfmt, RTFCvalue, 0 },
    { "linestart", RTFlinestart, RTFDdocfmt, RTFCvalue, 1 },
    { "deflang", RTFdeflang, RTFDdocfmt, RTFCvalue, 0 },

    { "hyphcaps", RTFhyphcaps, RTFDdocfmt, RTFCtoggle, 1 },
    { "hyphauto", RTFhyphauto, RTFDdocfmt, RTFCtoggle, 0 },

    { "fracwidth", RTFfracwidth, RTFDdocfmt, RTFCflag, 0 },
    { "makebackup", RTFmakebackup, RTFDdocfmt, RTFCflag, 0 },
    { "defformat", RTFdefformat, RTFDdocfmt, RTFCflag, 0 },
    { "psover", RTFpsover, RTFDdocfmt, RTFCflag, 0 },
    { "doctemp", RTFdoctemp, RTFDdocfmt, RTFCflag, 0 },

    { "nextfile", RTFnextfile, RTFDnextfile, RTFCdestination, 0 },
    { "template", RTFtemplate, RTFDtemplate, RTFCdestination, 0 },

    // Footnotes

    { "fet", RTFfet, RTFDdocfootnote, RTFCvalue, 0 },
    { "ftnstart", RTFftnstart, RTFDdocfootnote, RTFCvalue, 1 },
    { "aftnstart", RTFaftnstart, RTFDdocfootnote, RTFCvalue, 1 },

    { "ftnsep", RTFftnsep, RTFDftnsep, RTFCdestination, 0 },
    { "ftnsepc", RTFftnsepc, RTFDftnsepc, RTFCdestination, 0 },
    { "ftncn", RTFftncn, RTFDftncn, RTFCdestination, 0 },
    { "aftnsepc", RTFaftnsepc, RTFDaftnsepc, RTFCdestination, 0 },
    { "aftncn", RTFaftncn, RTFDaftncn, RTFCdestination, 0 },

    { "endnotes", RTFendnotes, RTFDdocfootnote, RTFCflag, 0 },
    { "enddoc", RTFenddoc, RTFDdocfootnote, RTFCflag, 0 },
    { "ftntj", RTFftntj, RTFDdocfootnote, RTFCflag, 0 },
    { "ftnbj", RTFftnbj, RTFDdocfootnote, RTFCflag, 0 },
    { "aendnotes", RTFaendnotes, RTFDdocfootnote, RTFCflag, 0 },
    { "aenddoc", RTFaenddoc, RTFDdocfootnote, RTFCflag, 0 },
    { "aftntj", RTFaftntj, RTFDdocfootnote, RTFCflag, 0 },
    { "aftnbj", RTFaftnbj, RTFDdocfootnote, RTFCflag, 0 },
    { "ftnrstpg", RTFftnrstpg, RTFDdocfootnote, RTFCflag, 0 },
    { "ftnrestart", RTFftnrestart, RTFDdocfootnote, RTFCflag, 0 },
    { "ftnrstcont", RTFftnrstcont, RTFDdocfootnote, RTFCflag, 0 },
    { "aftnrestart", RTFaftnrestart, RTFDdocfootnote, RTFCflag, 0 },
    { "aftnrstcont", RTFaftnrstcont, RTFDdocfootnote, RTFCflag, 0 },
    { "ftnnar", RTFftnnar, RTFDdocfootnote, RTFCflag, 0 },
    { "ftnnalc", RTFftnnalc, RTFDdocfootnote, RTFCflag, 0 },
    { "ftnnauc", RTFftnnauc, RTFDdocfootnote, RTFCflag, 0 },
    { "ftnnrlc", RTFftnnrlc, RTFDdocfootnote, RTFCflag, 0 },
    { "ftnnruc", RTFftnnruc, RTFDdocfootnote, RTFCflag, 0 },
    { "ftnnchi", RTFftnnchi, RTFDdocfootnote, RTFCflag, 0 },
    { "aftnnar", RTFaftnnar, RTFDdocfootnote, RTFCflag, 0 },
    { "aftnnalc", RTFaftnnalc, RTFDdocfootnote, RTFCflag, 0 },
    { "aftnnauc", RTFaftnnauc, RTFDdocfootnote, RTFCflag, 0 },
    { "aftnnrlc", RTFaftnnrlc, RTFDdocfootnote, RTFCflag, 0 },
    { "aftnnruc", RTFaftnnruc, RTFDdocfootnote, RTFCflag, 0 },
    { "aftnnchi", RTFaftnnchi, RTFDdocfootnote, RTFCflag, 0 },

    // Page info

    { "paperw", RTFpaperw, RTFDdocpage, RTFCvalue, 12240 },
    { "paperh", RTFpaperh, RTFDdocpage, RTFCvalue, 15840 },
    { "psz", RTFpsz, RTFDdocpage, RTFCvalue, 0 },
    { "margl", RTFmargl, RTFDdocpage, RTFCvalue, 1800 },
    { "margr", RTFmargr, RTFDdocpage, RTFCvalue, 1800 },
    { "margt", RTFmargt, RTFDdocpage, RTFCvalue, 1440 },
    { "margb", RTFmargb, RTFDdocpage, RTFCvalue, 1440 },
    { "gutter", RTFgutter, RTFDdocpage, RTFCvalue, 0 },
    { "pgnstart", RTFpgnstart, RTFDdocpage, RTFCvalue, 1 },
    { "facingp", RTFfacingp, RTFDdocpage, RTFCflag, 0 },
    { "margmirror", RTFmargmirror, RTFDdocpage, RTFCflag, 0 },
    { "landscape", RTFlandscape, RTFDdocpage, RTFCflag, 0 },
    { "widowctrl", RTFwidowctrl, RTFDdocpage, RTFCflag, 0 },

    // linked styles,compatibility options, forms,
    // revision marks, annotatons, bidirectional controls

    { "linkstyles", RTFlinkstyles, RTFDdocfmt, RTFCflag, 0 },
    { "notabind", RTFnotabind, RTFDdocfmt, RTFCflag, 0 },
    { "wraptrsp", RTFwraptrsp, RTFDdocfmt, RTFCflag, 0 },
    { "prcolbl", RTFprcolbl, RTFDdocfmt, RTFCflag, 0 },
    { "noextrasprl", RTFnoextrasprl, RTFDdocfmt, RTFCflag, 0 },
    { "nocolbal", RTFnocolbal, RTFDdocfmt, RTFCflag, 0 },
    { "cvmme", RTFcvmme, RTFDdocfmt, RTFCflag, 0 },
    { "sprstsp", RTFsprstsp, RTFDdocfmt, RTFCflag, 0 },
    { "sprsspbf", RTFsprsspbf, RTFDdocfmt, RTFCflag, 0 },
    { "otblrul", RTFotblrul, RTFDdocfmt, RTFCflag, 0 },
    { "transmf", RTFtransmf, RTFDdocfmt, RTFCflag, 0 },
    { "swpbdr", RTFswpbdr, RTFDdocfmt, RTFCflag, 0 },
    { "brkfrm", RTFbrkfrm, RTFDdocfmt, RTFCflag, 0 },

    // Forms
    { "formprot", RTFformprot, RTFDdocfmt, RTFCflag },
    { "allprot", RTFallprot, RTFDdocfmt, RTFCflag },
    { "formshade", RTFformshade, RTFDdocfmt, RTFCflag },
    { "formdisp", RTFformdisp, RTFDdocfmt, RTFCflag },
    { "printdata", RTFprintdata, RTFDdocfmt, RTFCflag },

    // revisions
    { "revprot", RTFrevprot, RTFDdocfmt, RTFCflag },
    { "revisions", RTFrevisions, RTFDdocfmt, RTFCflag },
    { "revprop", RTFrevprop, RTFDdocfmt, RTFCvalue, 0 },
    { "revbar", RTFrevbar, RTFDdocfmt, RTFCvalue, 0 },

    // Annotations
    { "annotprot", RTFannotprot, RTFDdocfmt, RTFCflag, 0 },

    // Bidirectional controls
    { "rtldoc", RTFrtldoc, RTFDdocfmt, RTFCflag, 0 },
    { "ltrdoc", RTFltrdoc, RTFDdocfmt, RTFCflag, 0 },

    // section formatting
    { "sectd", RTFsectd, RTFDsecfmt, RTFCflag, 0 },
    { "endnhere", RTFendnhere, RTFDsecfmt, RTFCflag, 0 },
    { "setunlocked", RTFsetunlocked, RTFDsecfmt, RTFCflag, 0 },
    { "binfsxn", RTFbinfsxn, RTFDsecfmt, RTFCvalue, 0 },
    { "binsxn", RTFbinsxn, RTFDsecfmt, RTFCvalue, 0 },
    { "pnseclvl", RTFpnseclvl, RTFDpnseclvl, RTFCdestination, 0 },

    // Section break
    { "sbknone", RTFsbknone, RTFDsecfmt, RTFCflag, 0 },
    { "sbkcol", RTFsbkcol, RTFDsecfmt, RTFCflag, 0 },
    { "sbkpage", RTFsbkpage, RTFDsecfmt, RTFCflag, 0 },
    { "sbkeven", RTFsbkeven, RTFDsecfmt, RTFCflag, 0 },
    { "sbkodd", RTFsbkodd, RTFDsecfmt, RTFCflag, 0 },

    // Line numbering
    { "linebetcol", RTFlinebetcol, RTFDsecfmt, RTFCflag, 0 },
    { "linerestart", RTFlinerestart, RTFDsecfmt, RTFCflag, 0 },
    { "lineppage", RTFlineppage, RTFDsecfmt, RTFCflag, 0 },
    { "linecont", RTFlinecont, RTFDsecfmt, RTFCflag, 0 },
    { "cols", RTFcols, RTFDsecfmt, RTFCvalue, 1 },
    { "colsx", RTFcolsx, RTFDsecfmt, RTFCvalue, 720 },
    { "colno", RTFcolno, RTFDsecfmt, RTFCvalue, 0 },
    { "colsr", RTFcolsr, RTFDsecfmt, RTFCvalue, 0 },
    { "colw", RTFcolw, RTFDsecfmt, RTFCvalue, 0 },
    { "linemod", RTFlinemod, RTFDsecfmt, RTFCvalue, 1 },
    { "linex", RTFlinex, RTFDsecfmt, RTFCvalue, 360 },
    { "linestarts", RTFlinestarts, RTFDsecfmt, RTFCvalue, 1 },

    // Page information
    { "pgwsxn", RTFpgwsxn, RTFDsectionpage, RTFCvalue, 0 },
    { "pghsxn", RTFpghsxn, RTFDsectionpage, RTFCvalue, 0 },
    { "marglsxn", RTFmarglsxn, RTFDsectionpage, RTFCvalue, 0 },
    { "margrsxn", RTFmargrsxn, RTFDsectionpage, RTFCvalue, 0 },
    { "margtsxn", RTFmargtsxn, RTFDsectionpage, RTFCvalue, 0 },
    { "margbsxn", RTFmargbsxn, RTFDsectionpage, RTFCvalue, 0 },
    { "guttersxn", RTFguttersxn, RTFDsectionpage, RTFCvalue, 0 },
    { "lndscpsxn", RTFlndscpsxn, RTFDsectionpage, RTFCflag, 0 },
    { "titlepg", RTFtitlepg, RTFDsectionpage, RTFCflag, 0 },
    { "headery", RTFheadery, RTFDsectionpage, RTFCvalue, 720 },
    { "footery", RTFfootery, RTFDsectionpage, RTFCvalue, 720 },
    { "pgnstarts", RTFpgnstarts, RTFDsectionpage, RTFCvalue, 1 },

    // Page numbers
    { "pgnx", RTFpgnx, RTFDsecfmt, RTFCvalue, 720 },
    { "pgny", RTFpgny, RTFDsecfmt, RTFCvalue, 720 },
    { "pgnhn", RTFpgnhn, RTFDsecfmt, RTFCvalue, 0 },
    { "pgncont", RTFpgncont, RTFDsecfmt, RTFCflag, 0 },
    { "pgnrestart", RTFpgnrestart, RTFDsecfmt, RTFCflag, 0 },
    { "pgndec", RTFpgndec, RTFDsecfmt, RTFCflag, 0 },
    { "pgnucrm", RTFpgnucrm, RTFDsecfmt, RTFCflag, 0 },
    { "pgnlcrm", RTFpgnlcrm, RTFDsecfmt, RTFCflag, 0 },
    { "pgnucltr", RTFpgnucltr, RTFDsecfmt, RTFCflag, 0 },
    { "pgnlcltr", RTFpgnlcltr, RTFDsecfmt, RTFCflag, 0 },
    { "pgnhnsh", RTFpgnhnsh, RTFDsecfmt, RTFCflag, 0 },
    { "pgnhnsp", RTFpgnhnsp, RTFDsecfmt, RTFCflag, 0 },
    { "pgnhnsc", RTFpgnhnsc, RTFDsecfmt, RTFCflag, 0 },
    { "pgnhnsm", RTFpgnhnsm, RTFDsecfmt, RTFCflag, 0 },
    { "pgnhnsn", RTFpgnhnsn, RTFDsecfmt, RTFCflag, 0 },

    // Vertical alignment
    { "vertalb", RTFvertalb, RTFDsecfmt, RTFCflag, 0 },
    { "vertalt", RTFvertalt, RTFDsecfmt, RTFCflag, 0 },
    { "vertalc", RTFvertalc, RTFDsecfmt, RTFCflag, 0 },
    { "vertalj", RTFvertalj, RTFDsecfmt, RTFCflag, 0 },
    { "rtlsect", RTFrtlsect, RTFDsecfmt, RTFCflag, 0 },
    { "ltrsect", RTFltrsect, RTFDsecfmt, RTFCflag, 0 },

    // Headers and footers
    { "header", RTFheader, RTFDheader, RTFCdestination, 0 },
    { "footer", RTFfooter, RTFDfooter, RTFCdestination, 0 },
    { "headerl", RTFheaderl, RTFDheaderl, RTFCdestination, 0 },
    { "footerl", RTFfooterl, RTFDfooterl, RTFCdestination, 0 },
    { "headerr", RTFheaderr, RTFDheaderr, RTFCdestination, 0 },
    { "footerr", RTFfooterr, RTFDfooterr, RTFCdestination, 0 },
    { "headerf", RTFheaderf, RTFDheaderf, RTFCdestination, 0 },
    { "footerf", RTFfooterf, RTFDfooterf, RTFCdestination, 0 },

    // Paragraph formatting
    { "pard", RTFpard, RTFDparfmt, RTFCflag, 0 },
    { "hyphpar", RTFhyphpar, RTFDparfmt, RTFCtoggle, 0 },
    { "intbl", RTFintbl, RTFDparfmt, RTFCflag, 0 },
    { "keep", RTFkeep, RTFDparfmt, RTFCflag, 0 },
    { "widctlpar", RTFwidctlpar, RTFDparfmt, RTFCflag, 0 },
    { "nowidctlpar", RTFnowidctlpar, RTFDparfmt, RTFCflag, 0 },
    { "keepn", RTFkeepn, RTFDparfmt, RTFCflag, 0 },
    { "noline", RTFnoline, RTFDparfmt, RTFCflag, 0 },
    { "pagebb", RTFpagebb, RTFDparfmt, RTFCflag, 0 },
    { "sbys", RTFsbys, RTFDparfmt, RTFCflag, 0 },
    { "level", RTFlevel, RTFDparfmt, RTFCvalue, 0 },

    // Paragraph Indentation
    { "fi", RTFfi, RTFDparfmt, RTFCvalue, 0 },
    { "li", RTFli, RTFDparfmt, RTFCvalue, 0 },
    { "ri", RTFri, RTFDparfmt, RTFCvalue, 0 },

    // Paragraph Alignment
    { "ql", RTFql, RTFDparfmt, RTFCflag, 0 },
    { "qr", RTFqr, RTFDparfmt, RTFCflag, 0 },
    { "qj", RTFqj, RTFDparfmt, RTFCflag, 0 },
    { "qc", RTFqc, RTFDparfmt, RTFCflag, 0 },

    // Paragraph Spacing
    { "sb", RTFsb, RTFDparfmt, RTFCvalue, 0 },
    { "sa", RTFsa, RTFDparfmt, RTFCvalue, 0 },
    { "sl", RTFsl, RTFDparfmt, RTFCvalue, 0 },
    { "slmult", RTFslmult, RTFDparfmt, RTFCvalue, 0 },

    // Paragraph Subdocument
    { "subdocument", RTFsubdocument, RTFDparfmt, RTFCvalue, 0 },

    // Paragraph Bidirectional controls
    { "rltpar", RTFrltpar, RTFDparfmt, RTFCflag, 0 },
    { "ltrpar", RTFltrpar, RTFDparfmt, RTFCflag, 0 },

    // Tabs
    { "tx", RTFtx, RTFDtab, RTFCvalue, 0 },
    { "tb", RTFtb, RTFDtab, RTFCvalue, 0 },
    { "tqr", RTFtqr, RTFDtab, RTFCflag, 0 },
    { "tqc", RTFtqc, RTFDtab, RTFCflag, 0 },
    { "tqdec", RTFtqdec, RTFDtab, RTFCflag, 0 },
    { "tldot", RTFtldot, RTFDtab, RTFCflag, 0 },
    { "tlhyph", RTFtlhyph, RTFDtab, RTFCflag, 0 },
    { "tlul", RTFtlul, RTFDtab, RTFCflag, 0 },
    { "tlth", RTFtlth, RTFDtab, RTFCflag, 0 },
    { "tleq", RTFtleq, RTFDtab, RTFCflag, 0 },

    // Bullets and numbering
    { "pntext", RTFpntext, RTFDpntext, RTFCdestination, 0 },
    { "pn", RTFpn, RTFDpn, RTFCdestination, 0 },
    { "pnlvl", RTFpnlvl, RTFDpn, RTFCvalue, 0 },
    { "pnlvbit", RTFpnlvbit, RTFDpn, RTFCflag, 0 },
    { "pnlvlbody", RTFpnlvlbody, RTFDpn, RTFCflag, 0 },
    { "pnlvlcont", RTFpnlvlcont, RTFDpn, RTFCflag, 0 },
    { "pncard", RTFpncard, RTFDpn, RTFCflag, 0 },
    { "pndec", RTFpndec, RTFDpn, RTFCflag, 0 },
    { "pnucltr", RTFpnucltr, RTFDpn, RTFCflag, 0 },
    { "pnucrm", RTFpnucrm, RTFDpn, RTFCflag, 0 },
    { "pnlctr", RTFpnlctr, RTFDpn, RTFCflag, 0 },
    { "pnlcrm", RTFpnlcrm, RTFDpn, RTFCflag, 0 },
    { "pnord", RTFpnord, RTFDpn, RTFCflag, 0 },
    { "pnordt", RTFpnordt, RTFDpn, RTFCflag, 0 },
    { "pnuld", RTFpnuld, RTFDpn, RTFCflag, 0 },
    { "pnuldb", RTFpnuldb, RTFDpn, RTFCflag, 0 },
    { "pnulnone", RTFpnulnone, RTFDpn, RTFCflag, 0 },
    { "pnulw", RTFpnulw, RTFDpn, RTFCflag, 0 },
    { "pnnumonce", RTFpnnumonce, RTFDpn, RTFCflag, 0 },
    { "pnacross", RTFpnacross, RTFDpn, RTFCflag, 0 },
    { "pnprev", RTFpnprev, RTFDpn, RTFCflag, 0 },
    { "pnhang", RTFpnhang, RTFDpn, RTFCflag, 0 },
    { "pnrestart", RTFpnrestart, RTFDpn, RTFCflag, 0 },
    { "pnqc", RTFpnqc, RTFDpn, RTFCflag, 0 },
    { "pnql", RTFpnql, RTFDpn, RTFCflag, 0 },
    { "pnqr", RTFpnqr, RTFDpn, RTFCflag, 0 },
    { "pnf", RTFpnf, RTFDpn, RTFCvalue, 0 },
    { "pnfs", RTFpnfs, RTFDpn, RTFCvalue, 0 },
    { "pncf", RTFpncf, RTFDpn, RTFCvalue, 0 },
    { "pnindent", RTFpnindent, RTFDpn, RTFCvalue, 0 },
    { "pnsp", RTFpnsp, RTFDpn, RTFCvalue, 0 },
    { "pnstart", RTFpnstart, RTFDpn, RTFCvalue, 0 },
    { "pnb", RTFpnb, RTFDpn, RTFCtoggle, 0 },
    { "pni", RTFpni, RTFDpn, RTFCtoggle, 0 },
    { "pncaps", RTFpncaps, RTFDpn, RTFCtoggle, 0 },
    { "pnscaps", RTFpnscaps, RTFDpn, RTFCtoggle, 0 },
    { "pnul", RTFpnul, RTFDpn, RTFCtoggle, 0 },
    { "pnstrike", RTFpnstrike, RTFDpn, RTFCtoggle, 0 },
    { "pntxta", RTFpntxta, RTFDpntxta, RTFCdestination, 0 },
    { "pntxtb", RTFpntxtb, RTFDpntxtb, RTFCdestination, 0 },

    // Paragraph borders
    { "brdrt", RTFbrdrt, RTFDpar, RTFCflag, 0 },
    { "brdrb", RTFbrdrb, RTFDpar, RTFCflag, 0 },
    { "brdrl", RTFbrdrl, RTFDpar, RTFCflag, 0 },
    { "brdrr", RTFbrdrr, RTFDpar, RTFCflag, 0 },
    { "brdrbtw", RTFbrdrbtw, RTFDpar, RTFCflag, 0 },
    { "brdrbar", RTFbrdrbar, RTFDpar, RTFCflag, 0 },
    { "box", RTFbox, RTFDpar, RTFCflag, 0 },
    { "brdrs", RTFbrdrs, RTFDborder, RTFCflag, 0 },
    { "brdrth", RTFbrdrth, RTFDborder, RTFCflag, 0 },
    { "brdrsh", RTFbrdrsh, RTFDborder, RTFCflag, 0 },
    { "brdrdb", RTFbrdrdb, RTFDborder, RTFCflag, 0 },
    { "brdrdot", RTFbrdrdot, RTFDborder, RTFCflag, 0 },
    { "brdrdash", RTFbrdrdash, RTFDborder, RTFCflag, 0 },
    { "brdrhair", RTFbrdrhair, RTFDborder, RTFCflag, 0 },
    { "brdrw", RTFbrdrw, RTFDborder, RTFCvalue, 0 },
    { "brsp", RTFbrsp, RTFDborder, RTFCvalue, 0 },
    { "brdrcf", RTFbrdrcf, RTFDborder, RTFCvalue, 0 },

    // Paragraph shading
    { "shading", RTFshading, RTFDshading, RTFCvalue, 0 },
    { "cfpat", RTFcfpat, RTFDshading, RTFCvalue, 0 },
    { "cbpat", RTFcbpat, RTFDshading, RTFCvalue, 0 },
    { "bghoriz", RTFbghoriz, RTFDshading, RTFCflag, 0 },
    { "bgvert", RTFbgvert, RTFDshading, RTFCflag, 0 },
    { "bgfdiag", RTFbgfdiag, RTFDshading, RTFCflag, 0 },
    { "bgbdiag", RTFbgbdiag, RTFDshading, RTFCflag, 0 },
    { "bgcross", RTFbgcross, RTFDshading, RTFCflag, 0 },
    { "bgdcross", RTFbgdcross, RTFDshading, RTFCflag, 0 },
    { "bgdkhoriz", RTFbgdkhoriz, RTFDshading, RTFCflag, 0 },
    { "bgdkvert", RTFbgdkvert, RTFDshading, RTFCflag, 0 },
    { "bgdkfdiag", RTFbgdkfdiag, RTFDshading, RTFCflag, 0 },
    { "bgdkbdiag", RTFbgdkbdiag, RTFDshading, RTFCflag, 0 },
    { "bgdkcross", RTFbgdkcross, RTFDshading, RTFCflag, 0 },
    { "bgdkdcross", RTFbgdkdcross, RTFDshading, RTFCflag, 0 },

    // Positioned objects and frames
    { "absw", RTFabsw, RTFDframe, RTFCvalue, 0 },
    { "absh", RTFabsh, RTFDframe, RTFCvalue, 0 },
    { "posx", RTFposx, RTFDframe, RTFCvalue, 0 },
    { "posnegx", RTFposnegx, RTFDframe, RTFCvalue, 0 },
    { "posy", RTFposy, RTFDframe, RTFCvalue, 0 },
    { "posnegy", RTFposnegy, RTFDframe, RTFCvalue, 0 },
    { "dxfrtext", RTFdxfrtext, RTFDframe, RTFCvalue, 0 },
    { "dfrmtxtx", RTFdfrmtxtx, RTFDframe, RTFCvalue, 0 },
    { "dfrmtxty", RTFdfrmtxty, RTFDframe, RTFCvalue, 0 },
    { "dropcapli", RTFdropcapli, RTFDframe, RTFCvalue, 0 },
    { "dropcapt", RTFdropcapt, RTFDframe, RTFCvalue, 0 },
    { "phmrg", RTFphmrg, RTFDframe, RTFCflag, 0 },
    { "phpg", RTFphpg, RTFDframe, RTFCflag, 0 },
    { "phcol", RTFphcol, RTFDframe, RTFCflag, 0 },
    { "posxc", RTFposxc, RTFDframe, RTFCflag, 0 },
    { "posxi", RTFposxi, RTFDframe, RTFCflag, 0 },
    { "posxo", RTFposxo, RTFDframe, RTFCflag, 0 },
    { "posxr", RTFposxr, RTFDframe, RTFCflag, 0 },
    { "posxl", RTFposxl, RTFDframe, RTFCflag, 0 },
    { "pvmrg", RTFpvmrg, RTFDframe, RTFCflag, 0 },
    { "pvpg", RTFpvpg, RTFDframe, RTFCflag, 0 },
    { "pvpara", RTFpvpara, RTFDframe, RTFCflag, 0 },
    { "posyc", RTFposyc, RTFDframe, RTFCflag, 0 },
    { "posyil", RTFposyil, RTFDframe, RTFCflag, 0 },
    { "posyt", RTFposyt, RTFDframe, RTFCflag, 0 },
    { "posyb", RTFposyb, RTFDframe, RTFCflag, 0 },
    { "nowrap", RTFnowrap, RTFDframe, RTFCflag, 0 },

    // Tables
    { "trgaph", RTFtrgaph, RTFDtable, RTFCvalue, 0 },
    { "trleft", RTFtrleft, RTFDtable, RTFCvalue, 0 },
    { "trrh", RTFtrrh, RTFDtable, RTFCvalue, 0 },
    { "cellx", RTFcellx, RTFDtable, RTFCvalue, 0 },
    { "clcfpat", RTFclcfpat, RTFDcell, RTFCvalue, 0 },
    { "clcbpat", RTFclcbpat, RTFDcell, RTFCvalue, 0 },
    { "clshdng", RTFclshdng, RTFDcell, RTFCvalue, 0 },
    { "trowd", RTFtrowd, RTFDtable, RTFCflag, 0 },
    { "trql", RTFtrql, RTFDtable, RTFCflag, 0 },
    { "trqr", RTFtrqr, RTFDtable, RTFCflag, 0 },
    { "trqc", RTFtrqc, RTFDtable, RTFCflag, 0 },
    { "trhdr", RTFtrhdr, RTFDtable, RTFCflag, 0 },
    { "trkeep", RTFtrkeep, RTFDtable, RTFCflag, 0 },
    { "ltrrow", RTFltrrow, RTFDtable, RTFCflag, 0 },
    { "rtlrow", RTFrtlrow, RTFDtable, RTFCflag, 0 },
    { "trbrdrt", RTFtrbrdrt, RTFDtable, RTFCflag, 0 },
    { "trbrdrl", RTFtrbrdrl, RTFDtable, RTFCflag, 0 },
    { "trbrdrb", RTFtrbrdrb, RTFDtable, RTFCflag, 0 },
    { "trbrdrr", RTFtrbrdrr, RTFDtable, RTFCflag, 0 },
    { "trbrdrh", RTFtrbrdrh, RTFDtable, RTFCflag, 0 },
    { "trbrdrv", RTFtrbrdrv, RTFDtable, RTFCflag, 0 },
    { "clmgf", RTFclmgf, RTFDcell, RTFCflag, 0 },
    { "clmrg", RTFclmrg, RTFDcell, RTFCflag, 0 },
    { "clbrdrt", RTFclbrdrt, RTFDcell, RTFCflag, 0 },
    { "clbrdrl", RTFclbrdrl, RTFDcell, RTFCflag, 0 },
    { "clbrdrb", RTFclbrdrb, RTFDcell, RTFCflag, 0 },
    { "clbrdrr", RTFclbrdrr, RTFDcell, RTFCflag, 0 },
    { "clbghoriz", RTFclbghoriz, RTFDcell, RTFCflag, 0 },
    { "clbgvert", RTFclbgvert, RTFDcell, RTFCflag, 0 },
    { "clbgfdiag", RTFclbgfdiag, RTFDcell, RTFCflag, 0 },
    { "clbgbdiag", RTFclbgbdiag, RTFDcell, RTFCflag, 0 },
    { "clbgcross", RTFclbgcross, RTFDcell, RTFCflag, 0 },
    { "clbgdcross", RTFclbgdcross, RTFDcell, RTFCflag, 0 },
    { "clbgdkhor", RTFclbgdkhor, RTFDcell, RTFCflag, 0 },
    { "clbgdkvert", RTFclbgdkvert, RTFDcell, RTFCflag, 0 },
    { "clbgdkfdiag", RTFclbgdkfdiag, RTFDcell, RTFCflag, 0 },
    { "clbgdkbdiag", RTFclbgdkbdiag, RTFDcell, RTFCflag, 0 },
    { "clbgdkcross", RTFclbgdkcross, RTFDcell, RTFCflag, 0 },
    { "clbgdkdcross", RTFclbgdkdcross, RTFDcell, RTFCflag, 0 },

    // Characters
    { "dn", RTFdn, RTFDchrfmt, RTFCvalue, 6 },
    { "expnd", RTFexpnd, RTFDchrfmt, RTFCvalue, 0 },
    { "expndtw", RTFexpndtw, RTFDchrfmt, RTFCvalue, 0 },
    { "fs", RTFfs, RTFDchrfmt, RTFCvalue, 24 },
    { "revauth", RTFrevauth, RTFDchrfmt, RTFCvalue, 0 },
    { "revdttm", RTFrevdttm, RTFDchrfmt, RTFCvalue, 0 },
    { "kerning", RTFkerning, RTFDchrfmt, RTFCvalue, 0 },
    { "up", RTFup, RTFDchrfmt, RTFCvalue, 6 },
    { "cf", RTFcf, RTFDchrfmt, RTFCvalue, 0 },
    { "cb", RTFcb, RTFDchrfmt, RTFCvalue, 0 },
    { "cchs", RTFcchs, RTFDchrfmt, RTFCvalue, 0 },
    { "lang", RTFlang, RTFDchrfmt, RTFCvalue, 0 },
    { "plain", RTFplain, RTFDchrfmt, RTFCflag, 0 },
    { "sub", RTFsub, RTFDchrfmt, RTFCflag, 0 },
    { "super", RTFsuper, RTFDchrfmt, RTFCflag, 0 },
    { "nosupersub", RTFnosupersub, RTFDchrfmt, RTFCflag, 0 },
    { "revised", RTFrevised, RTFDchrfmt, RTFCflag, 0 },
    { "uld", RTFuld, RTFDchrfmt, RTFCflag, 0 },
    { "ulnone", RTFulnone, RTFDchrfmt, RTFCflag, 0 },
    { "uldb", RTFuldb, RTFDchrfmt, RTFCflag, 0 },
    { "ulw", RTFulw, RTFDchrfmt, RTFCflag, 0 },
    { "rtlch", RTFrtlch, RTFDchrfmt, RTFCflag, 0 },
    { "ltrch", RTFltrch, RTFDchrfmt, RTFCflag, 0 },
    { "b", RTFb, RTFDchrfmt, RTFCtoggle, 0 },
    { "caps", RTFcaps, RTFDchrfmt, RTFCtoggle, 0 },
    { "deleted", RTFdeleted, RTFDchrfmt, RTFCtoggle, 0 },
    { "i", RTFi, RTFDchrfmt, RTFCtoggle, 0 },
    { "outl", RTFoutl, RTFDchrfmt, RTFCtoggle, 0 },
    { "scaps", RTFscaps, RTFDchrfmt, RTFCtoggle, 0 },
    { "shad", RTFshad, RTFDchrfmt, RTFCtoggle, 0 },
    { "strike", RTFstrike, RTFDchrfmt, RTFCtoggle, 0 },
    { "ul", RTFul, RTFDchrfmt, RTFCtoggle, 0 },
    { "v", RTFv, RTFDchrfmt, RTFCtoggle, 0 },

    // Associated character properties
    { "adn", RTFadn, RTFDachrfmt, RTFCvalue, 6 },
    { "aexpnd", RTFaexpnd, RTFDachrfmt, RTFCvalue, 0 },
    { "af", RTFaf, RTFDachrfmt, RTFCvalue, 0 },
    { "afs", RTFafs, RTFDachrfmt, RTFCvalue, 24 },
    { "aup", RTFaup, RTFDachrfmt, RTFCvalue, 6 },
    { "acf", RTFacf, RTFDachrfmt, RTFCvalue, 0 },
    { "alang", RTFalang, RTFDachrfmt, RTFCvalue, 0 },
    { "auld", RTFauld, RTFDachrfmt, RTFCflag, 0 },
    { "aulnone", RTFaulnone, RTFDachrfmt, RTFCflag, 0 },
    { "auldb", RTFauldb, RTFDachrfmt, RTFCflag, 0 },
    { "aulw", RTFaulw, RTFDachrfmt, RTFCflag, 0 },
    { "ab", RTFab, RTFDachrfmt, RTFCtoggle, 0 },
    { "acaps", RTFacaps, RTFDachrfmt, RTFCtoggle, 0 },
    { "ai", RTFai, RTFDachrfmt, RTFCtoggle, 0 },
    { "aoutl", RTFaoutl, RTFDachrfmt, RTFCtoggle, 0 },
    { "ascaps", RTFascaps, RTFDachrfmt, RTFCtoggle, 0 },
    { "ashad", RTFashad, RTFDachrfmt, RTFCtoggle, 0 },
    { "astrike", RTFastrike, RTFDachrfmt, RTFCtoggle, 0 },
    { "aul", RTFaul, RTFDachrfmt, RTFCtoggle, 0 },

    // Symbols
    { "par", RTFpar, RTFDrtf, RTFCsymbol, 0 },
    { "row", RTFrow, RTFDrtf, RTFCsymbol, 0 },
    { "cell", RTFcell, RTFDrtf, RTFCsymbol, 0 },
    { "sect", RTFsect, RTFDrtf, RTFCsymbol, 0 },
    { "chdate", RTFchdate, RTFDrtf, RTFCsymbol, 0 },
    { "chdpl", RTFchdpl, RTFDrtf, RTFCsymbol, 0 },
    { "chdpa", RTFchdpa, RTFDrtf, RTFCsymbol, 0 },
    { "chtime", RTFchtime, RTFDrtf, RTFCsymbol, 0 },
    { "chpgn", RTFchpgn, RTFDrtf, RTFCsymbol, 0 },
    { "sectnum", RTFsectnum, RTFDrtf, RTFCsymbol, 0 },
    { "chftn", RTFchftn, RTFDrtf, RTFCsymbol, 0 },
    { "chatn", RTFchatn, RTFDrtf, RTFCsymbol, 0 },
    { "chftnsep", RTFchftnsep, RTFDrtf, RTFCsymbol, 0 },
    { "chftnsepc", RTFchftnsepc, RTFDrtf, RTFCsymbol, 0 },
    { "page", RTFpage, RTFDrtf, RTFCsymbol, 0 },
    { "column", RTFcolumn, RTFDrtf, RTFCsymbol, 0 },
    { "line", RTFline, RTFDrtf, RTFCsymbol, 0 },
    { "tab", RTFtab, RTFDrtf, RTFCsymbol, 0 },
    { "emdash", RTFemdash, RTFDrtf, RTFCsymbol, 0 },
    { "endash", RTFendash, RTFDrtf, RTFCsymbol, 0 },
    { "emspace", RTFemspace, RTFDrtf, RTFCsymbol, 0 },
    { "enspace", RTFenspace, RTFDrtf, RTFCsymbol, 0 },
    { "bullet", RTFbullet, RTFDrtf, RTFCsymbol, 0 },
    { "lquote", RTFlquote, RTFDrtf, RTFCsymbol, 0 },
    { "rquote", RTFrquote, RTFDrtf, RTFCsymbol, 0 },
    { "ldblquote", RTFldblquote, RTFDrtf, RTFCsymbol, 0 },
    { "rdblquote", RTFrdblquote, RTFDrtf, RTFCsymbol, 0 },
    { "|", RTFsymbar, RTFDrtf, RTFCsymbol, 0 },
    { "~", RTFsymtilda, RTFDrtf, RTFCsymbol, 0 },
    { "-", RTFsymhyphen, RTFDrtf, RTFCsymbol, 0 },
    { "_", RTFsymunderbar, RTFDrtf, RTFCsymbol, 0 },
    { ":", RTFsymcolon, RTFDrtf, RTFCsymbol, 0 },
    { "*", RTFsymstar, RTFDrtf, RTFCsymbol, 0 },
    { "'", RTFsymquote, RTFDrtf, RTFCsymbol, 0 },
    { "\n", RTFnewline, RTFDrtf, RTFCsymbol, 0 },
    { "\r", RTFcarriageReturn, RTFDrtf, RTFCsymbol, 0 },
    { "ltrmark", RTFltrmark, RTFDrtf, RTFCsymbol, 0 },
    { "rtlmark", RTFrtlmark, RTFDrtf, RTFCsymbol, 0 },
    { "zwj", RTFzwj, RTFDrtf, RTFCsymbol, 0 },
    { "zwnj", RTFzwnj, RTFDrtf, RTFCsymbol, 0 },
    { "softpage", RTFsoftpage, RTFDrtf, RTFCflag, 0 },
    { "softcol", RTFsoftcol, RTFDrtf, RTFCflag, 0 },
    { "softline", RTFsoftline, RTFDrtf, RTFCflag, 0 },
    { "softlheight", RTFsoftlheight, RTFDrtf, RTFCvalue, 0 },

    // Book marks
    { "bkmkstart", RTFbkmkstart, RTFDbkmkstart, RTFCdestination, 0 },
    { "bkmkend", RTFbkmkend, RTFDbkmkend, RTFCdestination, 0 },
    { "bkmkcolf", RTFbkmkcolf, RTFDbkmkstart, RTFCvalue, 0 },
    { "bkmkcoll", RTFbkmkcoll, RTFDbkmkstart, RTFCvalue, 0 },

    // Picture
    { "pict", RTFpict, RTFDpict, RTFCdestination, 0 },
    { "macpict", RTFmacpict, RTFDpict, RTFCflag, 0 },
    { "pmmetafile", RTFpmmetafile, RTFDpict, RTFCvalue, 0 },
    { "wmetafile", RTFwmetafile, RTFDpict, RTFCvalue, 1 },
    { "dibitmap", RTFdibitmap, RTFDpict, RTFCvalue, 0 },
    { "wbitmap", RTFwbitmap, RTFDpict, RTFCvalue, 0 },
    { "wbmbitspixel", RTFwbmbitspixel, RTFDpict, RTFCvalue, 1 },
    { "wbmplanes", RTFwbmplanes, RTFDpict, RTFCvalue, 1 },
    { "wbmwidthbytes", RTFwbmwidthbytes, RTFDpict, RTFCvalue, 0 },
    { "picw", RTFpicw, RTFDpict, RTFCvalue, 0 },
    { "pich", RTFpich, RTFDpict, RTFCvalue, 0 },
    { "picwgoal", RTFpicwgoal, RTFDpict, RTFCvalue, 0 },
    { "pichgoal", RTFpichgoal, RTFDpict, RTFCvalue, 0 },
    { "picscalex", RTFpicscalex, RTFDpict, RTFCvalue, 100 },
    { "picscaley", RTFpicscaley, RTFDpict, RTFCvalue, 100 },
    { "picscaled", RTFpicscaled, RTFDpict, RTFCflag, 0 },
    { "piccropt", RTFpiccropt, RTFDpict, RTFCvalue, 0 },
    { "piccropb", RTFpiccropb, RTFDpict, RTFCvalue, 0 },
    { "piccropr", RTFpiccropr, RTFDpict, RTFCvalue, 0 },
    { "piccropl", RTFpiccropl, RTFDpict, RTFCvalue, 0 },
    { "picbmp", RTFpicbmp, RTFDpict, RTFCflag, 0 },
    { "picbpp", RTFpicbpp, RTFDpict, RTFCvalue, 0 },
    { "bin", RTFbin, RTFDpict, RTFCvalue, 0 },

    // OLE objects
    { "object", RTFobject, RTFDobject, RTFCdestination, 0 },

    // Drawings
    { "do", RTFdo, RTFDdo, RTFCdestination, 0 },

    // Footnote
    { "footnote", RTFfootnote, RTFDfootnote, RTFCdestination, 0 },
    { "ftnalt", RTFftnalt, RTFDfootnote, RTFCflag, 0 },

    // Annotations
    { "atrfstart", RTFatrfstart, RTFDatrfstart, RTFCdestination, 0 },
    { "atrfend", RTFatrfend, RTFDatrfend, RTFCdestination, 0 },
    { "atnid", RTFatnid, RTFDatnid, RTFCdestination, 0 },
    { "atnauthor", RTFatnauthor, RTFDatnauthor, RTFCdestination, 0 },
    { "annotation", RTFannotation, RTFDannotation, RTFCdestination, 0 },
    { "atnref", RTFatnref, RTFDatnref, RTFCdestination, 0 },
    { "atntime", RTFatntime, RTFDatntime, RTFCdestination, 0 },
    { "atnicn", RTFatnicn, RTFDatnicn, RTFCdestination, 0 },

    // Fields
    { "field", RTFfield, RTFDfield, RTFCdestination, 0 },
    { "fldinst", RTFfldinst, RTFDfldinst, RTFCdestination, 0 },
    { "fldrslt", RTFfldrslt, RTFDfldrslt, RTFCdestination, 0 },
    { "datafield", RTFdatafield, RTFDdatafield, RTFCdestination, 0 },
    { "flddirty", RTFflddirty, RTFDfield, RTFCflag, 0 },
    { "fldedit", RTFfldedit, RTFDfield, RTFCflag, 0 },
    { "fldlock", RTFfldlock, RTFDfield, RTFCflag, 0 },
    { "fldpriv", RTFfldpriv, RTFDfield, RTFCflag, 0 },
    { "fldalt", RTFfldalt, RTFDfield, RTFCflag, 0 },

    // Indices
    { "xe", RTFxe, RTFDxe, RTFCdestination, 0 },
    { "txe", RTFtxe, RTFDtxe, RTFCdestination, 0 },
    { "rxe", RTFrxe, RTFDrxe, RTFCdestination, 0 },
    { "xef", RTFxef, RTFDxe, RTFCvalue, 0 },
    { "bxe", RTFbxe, RTFDxe, RTFCflag, 0 },
    { "ixe", RTFixe, RTFDxe, RTFCflag, 0 },

    // Table of contents
    { "tc", RTFtc, RTFDtc, RTFCdestination, 0 },
    { "tcf", RTFtcf, RTFDtc, RTFCvalue, 67 },
    { "tcl", RTFtcl, RTFDtc, RTFCvalue, 1 },
    { "tcn", RTFtcn, RTFDtc, RTFCflag, 0 },
};

int nRTFcontrols = sizeof(Controls) / sizeof(RTFcontrol);

#else

extern RTFcontrol Controls[];
extern int nRTFcontrols;

#endif

#endif
