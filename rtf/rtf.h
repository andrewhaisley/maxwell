#pragma once 

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

#include <stdio.h>

#include <string>
#include <map>

#include <mx_list.h>

// forward definitions
class mx_rtf_importer;
class mx_font;
class mx_paragraph_style;
class mx_char_style;
class mx_colour;
class mx_table_area;
class mx_line_style;
class mx_fill_style;
class mx_tabstop;
class mx_wp_document;
class mx_wp_doc_pos;
class RTFcontrol;

// The types of tables

typedef enum {
    RTFTunknown,
    RTFTfonts,
    RTFTcolours,
    RTFTfiles,
    RTFTsectionStyle,
    RTFTparagraphStyle,
    RTFTcharacterStyle,
    RTFTnumberTables
} rtfTableType;

// The types of control word

typedef enum {
    RTFCflag,
    RTFCvalue,
    RTFCdestination,
    RTFCtoggle,
    RTFCsymbol
} rtfControlType;

// An enumeration for charsets

typedef enum {
    RTF_CHARSET_ANSI = 0,
    RTF_CHARSET_SYMBOL = 2,
    RTF_CHARSET_SHIFTJIS = 128,

    RTF_CHARSET_GREEK = 161,
    RTF_CHARSET_TURKISH,

    RTF_CHARSET_HEBREW = 177,
    RTF_CHARSET_ARABICSIMPLIFIED,
    RTF_CHARSET_ARABICTRDITIONAL,
    RTF_CHARSET_ARABICUSER,
    RTF_CHARSET_HEBREWUSER,

    RTF_CHARSET_CYRILLIC = 204,
    RTF_CHARSET_EASTERNEUROPE = 238,

    RTF_CHARSET_PC437 = 254,
    RTF_CHARSET_OEM
} RTFEcharset;

// An enumeration for pitches

typedef enum {
    RTF_DEFAULT_PITCH,
    RTF_FIXED_PITCH,
    RTF_VARIABLE_PITCH
} RTFEpitch;

// An enumeration for code page support types

typedef enum {
    RTF_CPG_USIBM = 437,

    RTF_CPG_ARABIC_ASMO708 = 708,
    RTF_CPG_ARABIC_ASMO449,
    RTF_CPG_ARABIC_TRANSPARENT,
    RTF_CPG_ARABIC_NAFITHA,

    RTF_CPG_ARABIC_TRANSPARENT_ASMO = 720,
    RTF_CPG_WINDOW31 = 819,
    RTF_CPG_IBM_MULTILINGUAL = 850,
    RTF_CPG_EASTERNEUROPE = 852,
    RTF_CPG_PORTUGESE = 860,
    RTF_CPG_HEBREW = 862,
    RTF_CPG_FRENCH_CANADIAN = 863,
    RTF_CPG_ARABIC = 864,
    RTF_CPG_NORWEGIAN = 865,
    RTF_CPG_SOVIETUNION = 866,
    RTF_CPG_JAPANESE = 932,
    RTF_CPG_WINDOWS31_EASTERNEUROPE = 1250,
    RTF_CPG_WINDOWS31_CYRILLIC = 1251
} RTFEcpg;

// Footnote type

typedef enum {
    RTF_FTN_NONE,
    RTF_FTN_ENDNOTES,
    RTF_FTN_BOTH
} RTFEfootnote;

// Revision marks

typedef enum {
    RTF_REV_NONE,
    RTF_REV_BOLD,
    RTF_REV_ITALIC,
    RTF_REV_UNDERLINE,
    RTF_REV_DUNDERLINE
} RTFErevision;

typedef enum {
    RTF_REVBAR_NONE,
    RTF_REVBAR_LEFT,
    RTF_REVBAR_RIGHT,
    RTF_REVBAR_OUTSIDE
} RTFErevisionBar;

// Dropcaps

typedef enum {
    RTF_DROPCAP_INTEXT,
    RTF_DROPCAP_MARGIN
} RTFEdropcap;

// Language

typedef enum {
    RTF_LANG_NONE = 0x0400,
    RTF_LANG_ALBANIAN = 0x041c,
    RTF_LANG_ARABIC = 0x0401,
    RTF_LANG_BAHASA = 0x0421,
    RTF_LANG_BELGIUM_DUTCH = 0x0813,
    RTF_LANG_BELGIAN_FRENCH = 0x080c,
    RTF_LANG_BRAZILIAN_PORTUGESE = 0x0416,
    RTF_LANG_BULGARIAN = 0x0402,
    RTF_LANG_CATALAN = 0x0403,
    RTF_LANG_CROATO_SERBIAN = 0x041a,
    RTF_LANG_CZECH = 0x0405,
    RTF_LANG_DANISH = 0x0406,
    RTF_LANG_ENGLISH_AUSTRALIAN = 0x0c09,
    RTF_LANG_ENGLISH_UK = 0x0809,
    RTF_LANG_ENGLISH_US = 0x0409,
    RTF_LANG_FINNISH = 0x040b,
    RTF_LANG_FRENCH = 0x040c,
    RTF_LANG_FRENCH_CANADIAN = 0x0c0c,
    RTF_LANG_GERMAN = 0x0407,
    RTF_LANG_GREEK = 0x0408,
    RTF_LANG_HEBREW = 0x040d,
    RTF_LANG_HUNGARIAN = 0x040e,
    RTF_LANG_ICELANDIC = 0x040f,
    RTF_LANG_ITALIAN = 0x0410,
    RTF_LANG_JAPANSES = 0x0411,
    RTF_LANG_KOREAN = 0x0412,
    RTF_LANG_NORWEGIAN_BOKMAL = 0x0414,
    RTF_LANG_NORWEGIAN_NYNORSK = 0x0814,
    RTF_LANG_POLISH = 0x0415,
    RTF_LANG_PORTUGESE = 0x0816,
    RTF_LANG_RHAETO_ROMANIC = 0x0417,
    RTF_LANG_ROMANIAN = 0x0418,
    RTF_LANG_RUSSIAN = 0x0419,
    RTF_LANG_SERBO_CROATIAN = 0x081a,
    RTF_LANG_SIMPLIFIED_CHINESE = 0x0804,
    RTF_LANG_SLOVAK = 0x041b,
    RTF_LANG_SPANISH_CASTILLIAN = 0x040a,
    RTF_LANG_SPANISH_MEXICAN = 0x080a,
    RTF_LANG_SWEDISH = 0x041d,
    RTF_LANG_SWISS_FRENCH = 0x100c,
    RTF_LANG_SWISS_GERMAN = 0x0807,
    RTF_LANG_SWISS_ITALIAN = 0x0810,
    RTF_LANG_THAI = 0x041e,
    RTF_LANG_TRADITIONAL_CHINESE = 0x0404,
    RTF_LANG_TURKISH = 0x041f,
    RTF_LANG_URDU = 0x0420
} RTFElang;

// Windows metafile type

typedef enum {
    RTF_MM_TEXT = 1,
    RTF_MM_LOMETRIC = 2,
    RTF_MM_HIMETRIC = 3,
    RTF_MM_LOENGLISH = 4,
    RTF_MM_HIENGLISH = 5,
    RTF_MM_TWIPS = 6,
    RTF_MM_ISOTROPIC = 7,
    RTF_MM_ANISOTROPIC = 8
} RTFEmetafile;

// OS/2 metafile type

typedef enum {
    RTF_PU_ARBITRARY = 0x0004,
    RTF_PU_PELS = 0x0008,
    RTF_PU_LOMETRIC = 0x000c,
    RTF_PU_HIMETRIC = 0x0010,
    RTF_PU_LOENGLISH = 0x0014,
    RTF_PU_HIENGLISH = 0x0018,
    RTF_PU_TWIPS = 0x001c
} RTFEos2metafile;

// An enumeration for all the RTF keywords recognised

typedef enum {
    RTFunknownControl,
    RTFrtf,
    RTFdeff,
    RTFansi,
    RTFmac,
    RTFpc,
    RTFpca,
    RTFfonttbl,
    RTFf,
    RTFfnil,
    RTFfroman,
    RTFfswiss,
    RTFfmodern,
    RTFfscript,
    RTFfdecor,
    RTFftech,
    RTFfbidi,
    RTFfcharset,
    RTFfprq,
    RTFfalt,
    RTFfontemb,
    RTFftnil,
    RTFfttruetype,
    RTFfontfile,
    RTFcpg,
    RTFfiletbl,
    RTFfile,
    RTFfid,
    RTFfrelative,
    RTFfosnum,
    RTFfvalidmac,
    RTFfvaliddos,
    RTFfvalidntfs,
    RTFfvalidhpfs,
    RTFfnetwork,
    RTFcolortbl,
    RTFred,
    RTFgreen,
    RTFblue,
    RTFstylesheet,
    RTFs,
    RTFcs,
    RTFds,
    RTFkeycode,
    RTFadditive,
    RTFsbasedon,
    RTFsnext,
    RTFshift,
    RTFctrl,
    RTFalt,
    RTFfn,
    RTFrevtbl,
    RTFinfo,
    RTFtitle,
    RTFsubject,
    RTFauthor,
    RTFoperator,
    RTFkeywords,
    RTFcomment,
    RTFdoccomm,
    RTFcreatim,
    RTFrevtim,
    RTFprintim,
    RTFbuptim,
    RTFversion,
    RTFvern,
    RTFedmins,
    RTFnofpages,
    RTFnofwords,
    RTFnofchars,
    RTFid,
    RTFyr,
    RTFmo,
    RTFdy,
    RTFhr,
    RTFmin,
    RTFsec,
    RTFdeftab,
    RTFhyphhotz,
    RTFhyphconsec,
    RTFlinestart,
    RTFdeflang,
    RTFhyphcaps,
    RTFhyphauto,
    RTFfracwidth,
    RTFmakebackup,
    RTFdefformat,
    RTFpsover,
    RTFdoctemp,
    RTFnextfile,
    RTFtemplate,
    RTFfet,
    RTFftnstart,
    RTFaftnstart,
    RTFftnsep,
    RTFftnsepc,
    RTFftncn,
    RTFaftnsepc,
    RTFaftncn,
    RTFendnotes,
    RTFenddoc,
    RTFftntj,
    RTFftnbj,
    RTFaendnotes,
    RTFaenddoc,
    RTFaftntj,
    RTFaftnbj,
    RTFftnrstpg,
    RTFftnrestart,
    RTFftnrstcont,
    RTFaftnrestart,
    RTFaftnrstcont,
    RTFftnnar,
    RTFftnnalc,
    RTFftnnauc,
    RTFftnnrlc,
    RTFftnnruc,
    RTFftnnchi,
    RTFaftnnar,
    RTFaftnnalc,
    RTFaftnnauc,
    RTFaftnnrlc,
    RTFaftnnruc,
    RTFaftnnchi,
    RTFpaperw,
    RTFpaperh,
    RTFpsz,
    RTFmargl,
    RTFmargr,
    RTFmargt,
    RTFmargb,
    RTFfacingp,
    RTFgutter,
    RTFpgnstart,
    RTFmargmirror,
    RTFlandscape,
    RTFwidowctrl,
    RTFlinkstyles,
    RTFnotabind,
    RTFwraptrsp,
    RTFprcolbl,
    RTFnoextrasprl,
    RTFnocolbal,
    RTFcvmme,
    RTFsprstsp,
    RTFsprsspbf,
    RTFotblrul,
    RTFtransmf,
    RTFswpbdr,
    RTFbrkfrm,
    RTFformprot,
    RTFallprot,
    RTFformshade,
    RTFformdisp,
    RTFprintdata,
    RTFrevprot,
    RTFrevisions,
    RTFrevprop,
    RTFrevbar,
    RTFannotprot,
    RTFrtldoc,
    RTFltrdoc,
    RTFsect,
    RTFsectd,
    RTFendnhere,
    RTFsetunlocked,
    RTFsbknone,
    RTFsbkcol,
    RTFsbkpage,
    RTFsbkeven,
    RTFsbkodd,
    RTFlinebetcol,
    RTFlinerestart,
    RTFlineppage,
    RTFlinecont,
    RTFlndscpsxn,
    RTFtitlepg,
    RTFpgncont,
    RTFpgnrestart,
    RTFpgndec,
    RTFpgnucrm,
    RTFpgnlcrm,
    RTFpgnucltr,
    RTFpgnlcltr,
    RTFpgnhnsh,
    RTFpgnhnsp,
    RTFpgnhnsc,
    RTFpgnhnsm,
    RTFpgnhnsn,
    RTFvertalb,
    RTFvertalt,
    RTFvertalc,
    RTFvertalj,
    RTFrtlsect,
    RTFltrsect,
    RTFbinfsxn,
    RTFbinsxn,
    RTFpnseclvl,
    RTFcols,
    RTFcolsx,
    RTFcolno,
    RTFcolsr,
    RTFcolw,
    RTFlinemod,
    RTFlinex,
    RTFlinestarts,
    RTFpgwsxn,
    RTFpghsxn,
    RTFmarglsxn,
    RTFmargrsxn,
    RTFmargtsxn,
    RTFmargbsxn,
    RTFguttersxn,
    RTFheadery,
    RTFfootery,
    RTFpgnstarts,
    RTFpgnx,
    RTFpgny,
    RTFpgnhn,
    RTFheader,
    RTFfooter,
    RTFheaderl,
    RTFfooterl,
    RTFheaderr,
    RTFfooterr,
    RTFheaderf,
    RTFfooterf,
    RTFpar,
    RTFcell,
    RTFrow,
    RTFpard,
    RTFhyphpar,
    RTFintbl,
    RTFkeep,
    RTFwidctlpar,
    RTFnowidctlpar,
    RTFkeepn,
    RTFnoline,
    RTFpagebb,
    RTFsbys,
    RTFql,
    RTFqr,
    RTFqj,
    RTFqc,
    RTFrltpar,
    RTFltrpar,
    RTFlevel,
    RTFfi,
    RTFli,
    RTFri,
    RTFsb,
    RTFsa,
    RTFsl,
    RTFslmult,
    RTFsubdocument,
    RTFtx,
    RTFtb,
    RTFtqr,
    RTFtqc,
    RTFtqdec,
    RTFtldot,
    RTFtlhyph,
    RTFtlul,
    RTFtlth,
    RTFtleq,
    RTFpntext,
    RTFpn,
    RTFpnlvl,
    RTFpnlvbit,
    RTFpnlvlbody,
    RTFpnlvlcont,
    RTFpncard,
    RTFpndec,
    RTFpnucltr,
    RTFpnucrm,
    RTFpnlctr,
    RTFpnlcrm,
    RTFpnord,
    RTFpnordt,
    RTFpnuld,
    RTFpnuldb,
    RTFpnulnone,
    RTFpnulw,
    RTFpnnumonce,
    RTFpnacross,
    RTFpnprev,
    RTFpnhang,
    RTFpnrestart,
    RTFpnqc,
    RTFpnql,
    RTFpnqr,
    RTFpnf,
    RTFpnfs,
    RTFpncf,
    RTFpnindent,
    RTFpnsp,
    RTFpnstart,
    RTFpnb,
    RTFpni,
    RTFpncaps,
    RTFpnscaps,
    RTFpnul,
    RTFpnstrike,
    RTFpntxta,
    RTFpntxtb,
    RTFbrdrt,
    RTFbrdrb,
    RTFbrdrl,
    RTFbrdrr,
    RTFbrdrbtw,
    RTFbrdrbar,
    RTFbox,
    RTFbrdrs,
    RTFbrdrth,
    RTFbrdrsh,
    RTFbrdrdb,
    RTFbrdrdot,
    RTFbrdrdash,
    RTFbrdrhair,
    RTFbrdrw,
    RTFbrsp,
    RTFbrdrcf,
    RTFshading,
    RTFcfpat,
    RTFcbpat,
    RTFbghoriz,
    RTFbgvert,
    RTFbgfdiag,
    RTFbgbdiag,
    RTFbgcross,
    RTFbgdcross,
    RTFbgdkhoriz,
    RTFbgdkvert,
    RTFbgdkfdiag,
    RTFbgdkbdiag,
    RTFbgdkcross,
    RTFbgdkdcross,
    RTFabsw,
    RTFabsh,
    RTFposx,
    RTFposnegx,
    RTFposy,
    RTFposnegy,
    RTFdxfrtext,
    RTFdfrmtxtx,
    RTFdfrmtxty,
    RTFdropcapli,
    RTFdropcapt,
    RTFphmrg,
    RTFphpg,
    RTFphcol,
    RTFposxc,
    RTFposxi,
    RTFposxo,
    RTFposxr,
    RTFposxl,
    RTFpvmrg,
    RTFpvpg,
    RTFpvpara,
    RTFposyc,
    RTFposyil,
    RTFposyt,
    RTFposyb,
    RTFnowrap,
    RTFtrgaph,
    RTFtrleft,
    RTFtrrh,
    RTFcellx,
    RTFclcfpat,
    RTFclcbpat,
    RTFclshdng,
    RTFtrowd,
    RTFtrql,
    RTFtrqr,
    RTFtrqc,
    RTFtrhdr,
    RTFtrkeep,
    RTFltrrow,
    RTFrtlrow,
    RTFtrbrdrt,
    RTFtrbrdrl,
    RTFtrbrdrb,
    RTFtrbrdrr,
    RTFtrbrdrh,
    RTFtrbrdrv,
    RTFclmgf,
    RTFclmrg,
    RTFclbrdrt,
    RTFclbrdrl,
    RTFclbrdrb,
    RTFclbrdrr,
    RTFclbghoriz,
    RTFclbgvert,
    RTFclbgfdiag,
    RTFclbgbdiag,
    RTFclbgcross,
    RTFclbgdcross,
    RTFclbgdkhor,
    RTFclbgdkvert,
    RTFclbgdkfdiag,
    RTFclbgdkbdiag,
    RTFclbgdkcross,
    RTFclbgdkdcross,
    RTFdn,
    RTFexpnd,
    RTFexpndtw,
    RTFfs,
    RTFrevauth,
    RTFrevdttm,
    RTFkerning,
    RTFup,
    RTFcf,
    RTFcb,
    RTFcchs,
    RTFlang,
    RTFplain,
    RTFsub,
    RTFsuper,
    RTFnosupersub,
    RTFrevised,
    RTFuld,
    RTFulnone,
    RTFuldb,
    RTFulw,
    RTFrtlch,
    RTFltrch,
    RTFb,
    RTFcaps,
    RTFdeleted,
    RTFi,
    RTFoutl,
    RTFscaps,
    RTFshad,
    RTFstrike,
    RTFul,
    RTFv,
    RTFadn,
    RTFaexpnd,
    RTFaf,
    RTFafs,
    RTFaup,
    RTFacf,
    RTFalang,
    RTFauld,
    RTFaulnone,
    RTFauldb,
    RTFaulw,
    RTFab,
    RTFacaps,
    RTFai,
    RTFaoutl,
    RTFascaps,
    RTFashad,
    RTFastrike,
    RTFaul,
    RTFchdate,
    RTFchdpl,
    RTFchdpa,
    RTFchtime,
    RTFchpgn,
    RTFsectnum,
    RTFchftn,
    RTFchatn,
    RTFchftnsep,
    RTFchftnsepc,
    RTFpage,
    RTFcolumn,
    RTFline,
    RTFtab,
    RTFemdash,
    RTFendash,
    RTFemspace,
    RTFenspace,
    RTFbullet,
    RTFlquote,
    RTFrquote,
    RTFldblquote,
    RTFrdblquote,
    RTFsymbar,
    RTFsymtilda,
    RTFsymhyphen,
    RTFsymunderbar,
    RTFsymcolon,
    RTFsymstar,
    RTFsymquote,
    RTFnewline,
    RTFcarriageReturn,
    RTFltrmark,
    RTFrtlmark,
    RTFzwj,
    RTFzwnj,
    RTFsoftpage,
    RTFsoftcol,
    RTFsoftline,
    RTFsoftlheight,
    RTFbkmkstart,
    RTFbkmkend,
    RTFbkmkcolf,
    RTFbkmkcoll,
    RTFpict,
    RTFmacpict,
    RTFpmmetafile,
    RTFwmetafile,
    RTFdibitmap,
    RTFwbitmap,
    RTFwbmbitspixel,
    RTFwbmplanes,
    RTFwbmwidthbytes,
    RTFpicw,
    RTFpich,
    RTFpicwgoal,
    RTFpichgoal,
    RTFpicscalex,
    RTFpicscaley,
    RTFpicscaled,
    RTFpiccropt,
    RTFpiccropb,
    RTFpiccropr,
    RTFpiccropl,
    RTFpicbmp,
    RTFpicbpp,
    RTFbin,
    RTFobject,
    RTFdo,
    RTFfootnote,
    RTFatrfstart,
    RTFatrfend,
    RTFatnid,
    RTFatnauthor,
    RTFannotation,
    RTFatnref,
    RTFatntime,
    RTFatnicn,
    RTFfield,
    RTFfldinst,
    RTFfldrslt,
    RTFdatafield,
    RTFxe,
    RTFtxe,
    RTFrxe,
    RTFtc,
    RTFftnalt,
    RTFflddirty,
    RTFfldedit,
    RTFfldlock,
    RTFfldpriv,
    RTFfldalt,
    RTFxef,
    RTFbxe,
    RTFixe,
    RTFtcf,
    RTFtcl,
    RTFtcn
} rtfControl;

// An enumeration for all the RTF destinations recognised
typedef enum {
    RTFDunknownDestination,
    RTFDrtf,
    RTFDfonttbl,
    RTFDfalt,
    RTFDfontemb,
    RTFDfontfile,
    RTFDfiletbl,
    RTFDfile,
    RTFDcolortbl,
    RTFDstylesheet,
    RTFDtitle,
    RTFDsubject,
    RTFDauthor,
    RTFDoperator,
    RTFDkeywords,
    RTFDcomment,
    RTFDdoccomm,
    RTFDcreatim,
    RTFDrevtim,
    RTFDprintim,
    RTFDbuptim,
    RTFDkeycode,
    RTFDnextfile,
    RTFDtemplate,
    RTFDftnsep,
    RTFDftnsepc,
    RTFDftncn,
    RTFDaftnsepc,
    RTFDaftncn,
    RTFDrevtbl,
    RTFDinfo,
    RTFDpn,
    RTFDpnseclvl,
    RTFDpntext,
    RTFDheader,
    RTFDfooter,
    RTFDheaderl,
    RTFDfooterl,
    RTFDheaderr,
    RTFDfooterr,
    RTFDheaderf,
    RTFDfooterf,
    RTFDpntxta,
    RTFDpntxtb,
    RTFDbkmkstart,
    RTFDbkmkend,
    RTFDpict,
    RTFDobject,
    RTFDdo,
    RTFDfootnote,
    RTFDatrfstart,
    RTFDatrfend,
    RTFDatnid,
    RTFDatnauthor,
    RTFDannotation,
    RTFDatnref,
    RTFDatntime,
    RTFDatnicn,
    RTFDfield,
    RTFDfldinst,
    RTFDfldrslt,
    RTFDdatafield,
    RTFDxe,
    RTFDtxe,
    RTFDrxe,
    RTFDtc,

    // Now use destinations which are not destinations
    // in the RTF sense but are destinations in a broader
    // sense. e.g. borders are part of the paragraph formatting
    // and certain keywords relate to borders. These values
    // are used as further qualifiers for controls which are not
    // themseleves destinations

    RTFDborder,
    RTFDshading,
    RTFDframe,
    RTFDtable,
    RTFDcell,
    RTFDchrfmt,
    RTFDachrfmt,
    RTFDtab,
    RTFDdocfmt,
    RTFDsecfmt,
    RTFDparfmt,
    RTFDdocfootnote,
    RTFDsectionpage,
    RTFDdocpage,
    RTFDpar
} rtfDestinationType;

class RTFSstyle;

class RTFSfont {
public:
    RTFSfont();
    ~RTFSfont();

    // convert this rtf font spec into a maxwell font
    void convertTo(int& err, mx_font& maxwell_font);

    char* fontname; // Font name
    char* falt; // Alternative font name
    rtfControl fontfamily; // Font family
    RTFEcharset fcharset; // Font character set
    RTFEpitch fprq; // Pitch
    int f; // Font number
};

class RTFScolour {
public:
    RTFScolour();

    // convert this rtf paragraph style into a maxwell paragraph style
    void convertTo(int& err, mx_colour& maxwell_colour);

    int red;
    int green;
    int blue;
    int offset;
};

class RTFSfile {
public:
    RTFSfile();
    ~RTFSfile();
    char* filename; // Filename
    int fid; // fileid
    int frelative; // relative offset
    rtfControl filesource; // File source
};

class RTFSdocfootnote {
public:
    RTFSdocfootnote();
    ~RTFSdocfootnote();
    void processControl(int& err, rtfControl control, int controlValue);

    // convert this rtf thing into a maxwell document info
    void convertTo(int& err, mx_wp_document& doc, mx_wp_doc_pos& doc_pos);

    RTFEfootnote footnote; // 0, 1, or 2 depending on presence or
                           // otherwise of footnotes
    char* ftnsep; // Text seperator for footnotes
    char* ftnsepc; // Text seperator for footnotes
    char* ftncn; // Text seperator for footnotes
    char* aftnsep; // Text seperator for endnotes
    char* aftnsepc; // Text seperator for endnotes
    char* aftncn; // Text seperator for endnotes
    rtfControl endnotes; // Location of footnotes
    rtfControl aendnotes; // Location of endnotes
    int ftnstart; // Start footnote number
    int aftnstart; // Start endnote number
    rtfControl ftnr; // Restart footnote condition
    rtfControl aftnr; // Restart endnote condition
    rtfControl ftnn; // Footnote numbering type
    rtfControl aftnn; // Endnote numbering type
};

class RTFSpage {
public:
    RTFSpage();
    void reset();
    void processControl(int& err, rtfControl control, int controlValue);
    // convert this rtf thing into a maxwell document info
    void convertTo(int& err, mx_wp_document& doc, mx_wp_doc_pos& doc_pos);

    int paperw; // Paper width
    int paperh; // Paper height
    int margl; // Margins
    int margr;
    int margt;
    int margb;
    bool facingp; // Facing pages
    int gutter; // Gutter width
    bool margmirror; // Switches margin and gutter
    bool landscape; // Landscape
    int pgnstart; // Start page number
    bool titlepg; // First page has a special format
    int headery; // Distance of header from top of page
    int footery; // Distance of header from bottom of page
};

class RTFSdocfmt {
public:
    RTFSdocfmt();
    void processControl(int& err, rtfControl control, int controlValue);

    // convert this rtf thing into a maxwell document info
    void convertTo(int& err, mx_wp_document& doc, mx_wp_doc_pos& doc_pos,
        mx_rtf_importer& lookups);

    rtfControl charset; // ansi, mac, etc
    int deff; // default font
    int deftab; // default tab
    int hyphhotz; // Hypenation hot spot
    int hyphconsec; // Hypenation number of consecutive
                    // hypenated lines
    bool hyphcaps; // toggle hypenation of capitalised
                   // words
    bool hyphauto; // toggle automated hyphenation
    int linestart; // start line number
    RTFElang deflang; // Default language
    RTFSpage page; // Page information
    RTFSdocfootnote footnote; // Footnote information
};

class RTFSsecfmt {
public:
    RTFSsecfmt(RTFSpage* documentPage = nullptr);
    void reset(RTFSpage* documentPage = nullptr);
    void processControl(int& err, rtfControl control, int controlValue);

    bool endnhere; // Section has footnotes
    rtfControl sectionBreak; // Section break type
    RTFSpage page; // Page data information for section
    int cols; // Number of columns
    int colsx; // Space betwenn columns
    int colno; // Column nunber to be formatted
    int colsr; // Space to right of columns
    int colw; // Width of column in twips
    bool linebetcol; // Line between columns
    int linemod; // Line number modulus
    int linex; // Line number distance offset
    int linestarts; // Start line number
    bool linerestart; // Line numbers restart at linestarts
    rtfControl linerestartType; // Line numbers start type
    int pgnrestart; // Page numbers restart at page restart
                    // value
    rtfControl pgnrestartType; // Page number start type
    int pgnx; // Page number position
    int pgny; // Page number position
    rtfControl pgntype; // Page number type
    int pgnhn; // Header number to go with page number
    rtfControl pgnhns; // Heading/page number seperator
    rtfControl verta; // Vertical alignment type
    int ds; // Style
    RTFSstyle* dss;
};

class RTFSsection {
public:
    RTFSsection(RTFSpage* documentPage = nullptr);
    void reset(RTFSpage* documentPage = nullptr);
    RTFSsecfmt fmt;
};

class RTFSparBorder {
public:
    RTFSparBorder();
    void reset();
    void processControl(int& err, rtfControl control, int controlValue);

    // convert this rtf thing into a maxwell style
    void convertTo(int& err, mx_line_style& maxwell_line_style,
        mx_rtf_importer& lookups);

    rtfControl brdrk; // Border type
    int brdrw; // Border width
    int brdrcf; // Border colour
    int brsp; // Border space
};

class RTFSparfmt {
public:
    RTFSparfmt();
    void reset();
    void processControl(int& err, rtfControl control, int controlValue);

    // convert this rtf paragraph style into a maxwell paragraph style
    void convertTo(int& err, mx_paragraph_style& maxwell_para_style,
        mx_rtf_importer& lookups);

    bool hyphpar; // Automatic hyphenation
    bool intbl; // Is a table paragraph
    bool keep; // Keep intact
    bool keepn; // keep with next
    int level; // Outline level
    bool noline; // No line numbering
    bool pagebb; // Break page before paragraph
    bool sbys; // Side by side paragraphs
    rtfControl alignment; // Alignment type
    int fi; // First line indent
    int li; // Left indent
    int ri; // Right indent
    int sb; // Space before
    int sa; // Space after
    int sl; // Space between lines
    int slmult; // Line spacing multiple
    int s; // Style
    RTFSstyle* ss;
};

class RTFSparFrame {
public:
    RTFSparFrame();
    void reset();
    void processControl(int& err, rtfControl control, int controlValue);

    int absw; // width
    int absh; // height
    bool nowrap; // outside text does not wrap around object
    int dxfrtext; // Distances between text in frame and outside
    int dfrmtxtx;
    int dfrmtxty;
    int dropcapli; // Number of lines capital drops
    RTFEdropcap dropcapt; // Type of dropped capital
    int posx; // Distance of frame from left edge of reference
    int posy; // Distance of frame from top edge of reference
    rtfControl hframe; // Horizontal Frame reference
    rtfControl hdist; // Horizontal position of paragraph
    rtfControl vframe; // Vertical frame reference
    rtfControl vdist; // Vertical position of paragraph
};

class RTFSparTab {
public:
    RTFSparTab();
    void reset();
    void processControl(int& err, rtfControl control, int controlValue);

    // convert this rtf tabstop into a maxwell tabstop
    void convertTo(int& err, mx_tabstop& mx_t);

    int tx; // Tab position
    int tb; // Bar tab poistion
    rtfControl tabkind; // Tab justification
    rtfControl tablead; // Tab leader
};

class RTFSparShading {
public:
    void reset();
    RTFSparShading();
    void processControl(int& err, rtfControl control, int controlValue);

    // convert this rtf thing into a maxwell fill style
    void convertTo(int& err, mx_fill_style& maxwell_fill_style,
        mx_rtf_importer& lookups);

    int shading; // Shading in hundredths of percent
    int cfpat; // Foreground colour
    int cbpat; // Background colour
    rtfControl pat; // Pattern type
};

class RTFSpar {
public:
    RTFSpar(int& err);
    RTFSpar(int& err, RTFSpar& par);
    RTFSpar& operator=(RTFSpar& par);
    RTFSpar& equate(int& err, RTFSpar& par);
    void reset();
    ~RTFSpar();
    RTFSparTab currentTab;
    void processControl(int& err, rtfControl control, int controlValue);
    void processEndTab(rtfControl control, int controlValue);
    void getCurrentBorder(int& err, RTFSparBorder** pcurrentBorder);

    void deleteTabs();

    // convert this rtf paragraph style into a maxwell paragraph style
    void convertTo(int& err, mx_paragraph_style& maxwell_para_style, mx_rtf_importer& lookups);

    rtfControl currentBorder;
    RTFSparBorder brdrt;
    RTFSparBorder brdrb;
    RTFSparBorder brdrl;
    RTFSparBorder brdrr;
    RTFSparBorder brdrbtw;
    RTFSparBorder brdrbar;
    RTFSparBorder box;

    // are the tabs in this formatting from a style in the style table? If so
    // then before new tabs are added the old ones should be deleted and this
    // variable should be set to false;
    bool tabsAreStyleTabs;
    std::map<int, RTFSparTab *> tabs; // List of the paragraph tabs
    RTFSparfmt fmt; // Paragraph format
    RTFSparFrame frame; // Paragraph frame
    RTFSparShading shading; // Paragraph shading
};

class RTFScell {
public:
    RTFScell();
    void reset();
    void processControl(int& err, rtfControl control, int controlValue);

    // convert this rtf table cell format into a maxwell table cell
    void convertTo(int& err, mx_table_area& ta, mx_rtf_importer& lookups,
        int row, int column);

    // merge two cells contents
    void merge(int& err, RTFScell& a_cell);

    int cellx; // Right boundary of the cells
    bool clmgf; // Cell to be merged start
    bool clmrg; // Contents to be merged
    rtfControl currentBorder;
    void getCurrentBorder(int& err,
        RTFSparBorder** pcurrentBorder);
    RTFSparBorder clbrdrt; // Border
    RTFSparBorder clbrdrl;
    RTFSparBorder clbrdrb;
    RTFSparBorder clbrdrr;
    RTFSparShading cellshad; // Shading
};

class RTFStable {
public:
    RTFStable();
    ~RTFStable();
    void processControl(int& err, rtfControl control, int controlValue);
    void getCurrentBorder(int& err,
        RTFSparBorder** pcurrentBorder);
    void reset();

    // convert this rtf table format into a maxwell table
    void convertTo(int& err, mx_table_area& ta, mx_rtf_importer& lookups,
        int row, mx_list& merged_cells);

    bool inTable;
    bool inCell;
    int cellCount;
    RTFScell currentCell;
    rtfControl currentBorder; // The border currently being populated
    int trgaph; // Spacing between cells
    int trleft; // Postion of left row
    int trrh; // Row height
    bool trhdr; // Row header
    bool trkeep; // Keep row together
    rtfControl rowjust; // Row justification
    RTFSparBorder trbrdrt; // Table borders
    RTFSparBorder trbrdrl;
    RTFSparBorder trbrdrb;
    RTFSparBorder trbrdrr;
    RTFSparBorder trbrdrh;
    RTFSparBorder trbrdrv;
    mx_list cells;
};

class RTFSfield {
public:
    RTFSfield();

    void processControl(int& err, rtfControl control, int controlValue);
    void reset();

    bool dirty; // format change since last update
    bool edit; // text of field result has been modified
    bool lock; // field is locked, cannot be updated
    bool priv; // result cannot be displayed (eg binary data)
};

class RTFSchrfmt {
public:
    RTFSchrfmt();
    void reset();
    void processControl(int& err, rtfControl control, int controlValue);

    // convert this rtf character format into a maxwell character style
    void convertTo(int& err, mx_char_style& maxwell_style, mx_rtf_importer& lookups);

    bool caps; // Capitals
    bool b; // Bold
    bool deleted; // Deleted
    bool i; // Italic
    bool outl; // Outline
    bool scaps; // Small capitals
    bool shad; // Shadow
    bool strike; // Strike through
    bool v; // Hidden
    bool ul; // Underline on (standard)
    rtfControl ult; // Underline type
    int dn; // Subscript postiton
    int up; // Superscript position
    bool sub; // Subscript
    bool super; // Supercsript
    bool nosupersub; // Super, subscript off
    int expnd; // Expansion of spaces
    int expndtw;
    int kerning; // Kerning space
    int f; // Font number
    RTFSfont* font; // Font details
    int fs; // Font size
    int cf; // Foreground colour
    RTFScolour* cfs; // Character foreground colour
    int cb; // Background colour
    RTFScolour* cbs; // Character background colour
    RTFElang lang; // Language
    int cs; // Style
    RTFSstyle* css;
};

class RTFScharacter {
public:
    RTFScharacter();
    void reset();

    // convert this rtf character format into a maxwell character style
    void convertTo(int& err, mx_char_style& maxwell_style,
        mx_rtf_importer& lookups);

    RTFSchrfmt fmt;
};

class RTFSformatting {
public:
    RTFSformatting(int& err, RTFSpage* documentPage = nullptr);
    RTFSformatting(int& err, const RTFSformatting& formatting);
    RTFSformatting& operator=(RTFSformatting& formatting);
    RTFSformatting& equate(int& err, const RTFSformatting& formatting);
    void reset(RTFSpage* documentPage = nullptr);
    void processControl(int& err, std::map<rtfTableType, void *> &tables, rtfDestinationType destination, rtfControl control, int controlValue);

    // convert this rtf thing into a maxwell paragraph style
    void convertTo(int& err, mx_paragraph_style& maxwell_para_style, mx_rtf_importer& lookups);

    RTFSsection section; // Section formatting
    RTFSpar paragraph; // Paragraph formatting
    RTFScharacter character; // Character formatting
};

class RTFSstyle {
public:
    RTFSstyle(int& err, RTFSpage* documentPage = nullptr);
    ~RTFSstyle();
    void update(int& err, std::map<rtfTableType, void *> &tables);

    // convert this rtf style into a maxwell paragraph style
    void convertTo(int& err, mx_paragraph_style& maxwell_para_style,
        mx_rtf_importer& lookups);

    char* stylename; // Stylename
    rtfControl styledef; // Styletype (paragraph, section or character)
    rtfTableType styleType; // The style table type
    bool additive; // Additive (for character sytles)
    int snext; // The style to be applied to the next paragraph
    int styleNumber; // The number of the sytle
    int sbasedon; // The style it is based on
    mx_list setControls; // Values set in the style
    bool updated; // Formatting is updated
    RTFSformatting formatting; // Formatting data for the style
};

class RTFSpict {
public:
    RTFSpict();
    void reset();

    bool inPict;

    void processControl(int& err, rtfControl control, int controlValue);

    RTFSparBorder border;
    RTFSparShading shading;
    rtfControl picttype;
    int picttypeQualifier;
    int wbmbitspixel;
    int wbmplanes;
    int wbmwidthbytes;
    int picw;
    int pich;
    int picwgoal;
    int pichgoal;
    int picscalex;
    int picscaley;
    bool picscaled;
    int piccropt;
    int piccropb;
    int piccropl;
    int piccropr;
    bool picbmp;
    int picbpp;
    bool bin;
};

class RTFSgroup {
public:
    RTFSgroup(int& err, RTFSpage* documentPage = nullptr);
    RTFSgroup(int& err, RTFSgroup& group);
    RTFSgroup& operator=(RTFSgroup& group);
    RTFSgroup& equate(int& err, const RTFSgroup& group);
    rtfDestinationType destination;
    bool skippingRoot;
    RTFSformatting formatting;
};

typedef enum {
    RTFreadingControl,
    RTFreadingText
} RTFreadingType;

#define MAX_RTF_CONTROL_BUFFER 128

class mx_rtf {
public:
    mx_rtf(int& err, const char* filename);
    void read(int& err);
    virtual ~mx_rtf();

    // update a given style with our style tables
    void update_style(int& err, RTFSstyle* st);

    RTFSdocfmt documentData; // Current document data

protected:
    char* textBuffer;
    int allocTextBufferSize;
    int textBufferSize;

    RTFStable tableData; // Current table data
    RTFSfield fieldData; // Current field data
    RTFSpict pictData; // Current picture data

    int file_offset; // how many characters of the file have
                     // been read
private:
    static std::map<std::string, RTFcontrol *> m_controls; // Control dictionary

    std::map<rtfTableType, void *> m_tables; // Fonts, colours, sytles, files
    FILE* iif; // datastream
    mx_list groups; // List of groups

    RTFreadingType readingType;
    bool readingStarredDestination;
    bool skippingGroup;

    int controlBufferSize;
    char controlBuffer[MAX_RTF_CONTROL_BUFFER];

    int controlValueSize;
    char controlValueBuffer[MAX_RTF_CONTROL_BUFFER];

    bool putLast(int& err, char c);
    bool readNext(int& err, char& c);

    void addText(int& err, char c);
    void findControl(int& err);
    void openGroup(int& err);
    void closeGroup(int& err);
    rtfDestinationType currentDestination(int& err);

    RTFSfont* currentFont;
    RTFSstyle* currentStyle;
    RTFSfile* currentFile;
    RTFScolour* currentColour;
    int currentColourCount;

    void processOutput(int& err);

    void processFontControl(int& err, RTFSgroup* group, rtfDestinationType destination, rtfControl control, int controlValue);
    void processFileControl(int& err, RTFSgroup* group, rtfDestinationType destination, rtfControl control, int controlValue);
    void processMainControl(int& err, RTFSgroup* group, rtfDestinationType destination, rtfControl control, int controlValue);
    void processStyleControl(int& err, RTFSgroup* group, rtfDestinationType destination, rtfControl control, int controlValue);
    void processColourControl(int& err, RTFSgroup* group, rtfDestinationType destination, rtfControl control, int controlValue);
    void processControl(int& err, RTFSgroup* group, rtfControl control, rtfControlType controlType, rtfDestinationType controlDestination, int controlValue);

    void processFontOutput(int& err, RTFSgroup* group); void processFileOutput(int& err, RTFSgroup* group);
    void processColourOutput(int& err, RTFSgroup* group); void processStyleOutput(int& err, RTFSgroup* group);

    // Virtual functions to process particular output

    virtual void processFieldInstOutput(int& err,
        RTFSgroup* group);
    virtual void processMainOutput(int& err,
        RTFSgroup* group);

    virtual void processPictOutput(int& err,
        RTFSgroup* group);

    virtual void processNewParagraph(int& err,
        RTFSgroup* group,
        rtfDestinationType destination);
    virtual void processNewTableCell(int& err,
        RTFSgroup* group,
        rtfDestinationType destination);
    virtual void processNewTableRow(int& err,
        RTFSgroup* group,
        rtfDestinationType destination);
    virtual void processNewTab(int& err,
        RTFSgroup* group,
        rtfDestinationType destination);
    virtual void processSpecialCharacter(int& err,
        RTFSgroup* group,
        rtfDestinationType destination,
        rtfControl control,
        int controlValue);

    virtual void userProcessFontOutput(int& err, RTFSfont* f) { };
    virtual void userProcessFileOutput(int& err, RTFSfile* f) { };
    virtual void userProcessColourOutput(int& err, RTFScolour* c) { };
    virtual void userProcessStyleOutput(int& err, RTFSstyle* s) { };
};

class RTFcontrol
{
public:
    const char* name;
    rtfControl control;
    rtfDestinationType destination;
    rtfControlType controlType;
    int value;
};
