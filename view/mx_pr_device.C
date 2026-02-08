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
 * MODULE : mx_pr_device.C
 *
 * AUTHOR : David Miller
 *
 * DESCRIPTION:
 * Module mx_pr_device.C
 *
 *
 */
#include <pwd.h>

#include <mx_list_iter.h>
#include <mx_pr_device.h>

/*-------------------------------------------------
 * FUNCTION: mx_print_device::mx_print_device
 *
 * DESCRIPTION:
 *
 *
 */

mx_print_device::mx_print_device(int& err,
    mx_frame& deviceFrame,
    mx_point& deviceSize,
    double deviceResolution)
    : mx_device(deviceFrame)
{
    err = MX_ERROR_OK;

    // postscript copes with RGB
    doesRGB = TRUE;

    // postscipt can join and fill any path

    drawsChord = TRUE;
    drawsSector = TRUE;
    fillsSector = TRUE;

    drawsBeziers = TRUE;
    fillsBeziers = TRUE;

    clipsArc = TRUE;
    clipsSector = TRUE;

    doesRotate = TRUE;
    doesAffine = TRUE;

    setScreenResolution(deviceResolution);

    document_fonts = new mx_list;

    // set the printer size in pixels
    printerSize = deviceSize * deviceResolution;
}

/*-------------------------------------------------
 * FUNCTION: mx_print_device::mx_print_device
 *
 * DESCRIPTION:
 *
 *
 */

mx_print_device::~mx_print_device()
{
    mx_list_iterator iter(*document_fonts);
    mx_font* f;

    while (iter.more()) {
        f = (mx_font*)iter.data();
        delete f;
    }

    delete document_fonts;

    return;
}

/*-------------------------------------------------
 * FUNCTION: mx_print_device:setShiftOrigin
 *
 * DESCRIPTION: Shift the contents of a window so
 * that the background pixmap is used where possible
 * and the new areas are redrawn.
 *
 * This is typically used by scrolling to save any
 * area which can be redrawn any then to redraw the
 * new areas.
 */

void mx_print_device::setShiftOrigin(mx_doc_coord_t& wantedShiftOrigin,
    mx_doc_coord_t& gotShiftOrigin,
    bool doScroll)
{
    gotShiftOrigin = pixelSnapFrame(wantedShiftOrigin);
    return;
}

/*-------------------------------------------------
 * FUNCTION: mx_print_device::closeGap
 *
 * DESCRIPTION: Close a gap in the device in a direction,
 * from a point. The gapSize must have either a zero
 * x or a zero y. The non-zero value may be negative
 * depending on the direction in which the gap is to
 * be closed. The startPoint pixel is the 'first' pixel
 * to disappear
 */

void mx_print_device::closeGap(int& err,
    bool doX,
    mx_point& frameStartPoint,
    double frameGapSize)
{
    err = MX_ERROR_OK;
}

/*-------------------------------------------------
 * FUNCTION: mx_print_device::makeGap
 *
 * DESCRIPTION: Make a gap in the device in a direction,
 * from a point. The gap size may be negative depending
 * on the direction in which the gap is to
 * be made. The startPoint pixel is the 'first' pixel
 * to be moved
 */

void mx_print_device::makeGap(int& err,
    bool doX,
    mx_point& frameStartPoint,
    double frameGapSize)
{
    err = MX_ERROR_OK;
}

/*-------------------------------------------------
 * FUNCTION: mx_print_device:shift
 *
 * DESCRIPTION: Shift the contents of a window so
 * that the background pixmap is used where possible
 * and the new areas are redrawn.
 *
 * This is typically used by scrolling to save any
 * area which can be redrawn any then to redraw the
 * new areas.
 */

void mx_print_device::shift(int& err)
{
    err = MX_ERROR_OK;
}

/*-------------------------------------------------
 * FUNCTION: mx_print_device:refresh
 *
 * DESCRIPTION: Shift the contents of a window so
 * that the background pixmap is used where possible
 * and the new areas are redrawn.
 *
 * This is typically used by scrolling to save any
 * area which can be redrawn any then to redraw the
 * new areas.
 *
 * A top left and bottom right can be defined in
 * frame coordinates
 */

void mx_print_device::refresh(int& err,
    mx_point* topLeft,
    mx_point* bottomRight)
{
    mx_doc_coord_t devTopLeft;
    mx_doc_coord_t devBottomRight;
    mx_doc_coord_t frameTopLeft;
    mx_doc_coord_t frameBottomRight;
    mx_draw_event drawEvent;

    err = MX_ERROR_OK;

    // convert top left coordinate to device coordinates
    if (topLeft != NULL) {
        frameTopLeft.sheet_number = 0;
        frameTopLeft.p = *topLeft;

        devTopLeft = frame2dev(frameTopLeft);

        devTopLeft.p.x = GMAX(devTopLeft.p.x, 0);
        devTopLeft.p.y = GMAX(devTopLeft.p.y, 0);
    }

    // convert bottom right coordinate to device coordinates
    if (bottomRight != NULL) {
        frameBottomRight.sheet_number = 0;
        frameBottomRight.p = *bottomRight;

        devBottomRight = frame2dev(frameBottomRight);

        devBottomRight.p.x = GMIN(bottomRight->x, printerSize.x - 1);
        devBottomRight.p.y = GMIN(bottomRight->y, printerSize.y - 1);
    } else {
        devBottomRight.p.x = printerSize.x - 1;
        devBottomRight.p.y = printerSize.y - 1;
    }

    drawEvent.setTL(devTopLeft);
    drawEvent.setBR(devBottomRight);
    drawEvent.setCount(1);
    drawEvent.setType(MX_EVENT_DRAW);

    draw(err, drawEvent);
    MX_ERROR_CHECK(err);

    return;
abort:
    return;
}

void mx_print_device::sendPreamble(
    int& err,
    int numPages,
    int numCopies,
    const char* paperType,
    float pageWidth,
    float pageHeight,
    const char* title,
    bool includeFonts)
{
    time_t t;
    char hn[100];

    struct passwd* pwd;

    err = MX_ERROR_OK;

    t = time(NULL);
    gethostname(hn, 99);
    hn[99] = 0;

    pwd = getpwuid(getuid());

    mx_print_device::includeFonts = includeFonts;

    if (fprintf(outputFile, "%%!PS-Adobe-3.0\n") == EOF) {
        MX_ERROR_THROW(err, MX_PS_FILE_ERROR);
    }
    if (fprintf(outputFile, "%%%%Creator: Maxwell Postscript Device\n") == EOF) {
        MX_ERROR_THROW(err, MX_PS_FILE_ERROR);
    }
    if (fprintf(outputFile, "%%%%CreationDate: %s", ctime(&t)) == EOF) {
        MX_ERROR_THROW(err, MX_PS_FILE_ERROR);
    }
    if (fprintf(outputFile, "%%%%DocumentMedia: %s %.0f %.0f 0 white ()\n",
            paperType,
            MX_MM_TO_POINTS(pageWidth),
            MX_MM_TO_POINTS(pageHeight))
        == EOF) {
        MX_ERROR_THROW(err, MX_PS_FILE_ERROR);
    }
    if (fprintf(outputFile, "%%%%DocumentPrinterRequired: () ()\n") == EOF) {
        MX_ERROR_THROW(err, MX_PS_FILE_ERROR);
    }
    if (fprintf(outputFile, "%%%%DocumentData: Clean7Bit\n") == EOF) {
        MX_ERROR_THROW(err, MX_PS_FILE_ERROR);
    }
    if (fprintf(outputFile, "%%%%For: %s\n", pwd->pw_name) == EOF) {
        MX_ERROR_THROW(err, MX_PS_FILE_ERROR);
    }
    if (fprintf(outputFile, "%%%%Title: %s\n", title) == EOF) {
        MX_ERROR_THROW(err, MX_PS_FILE_ERROR);
    }
    if (fprintf(outputFile, "%%%%Routing: %s@%s\n", pwd->pw_name, hn) == EOF) {
        MX_ERROR_THROW(err, MX_PS_FILE_ERROR);
    }
    if (fprintf(outputFile, "%%%%Pages: %d\n", numPages) == EOF) {
        MX_ERROR_THROW(err, MX_PS_FILE_ERROR);
    }
    if (fprintf(outputFile, "%%%%Copies: 1\n") == EOF) {
        MX_ERROR_THROW(err, MX_PS_FILE_ERROR);
    }

    delete document_fonts;
    document_fonts = new mx_list;

abort:;
}

void mx_print_device::sendProcset(int& err)
{
    if (fprintf(outputFile,

            "/Maxwell10Dict 18 dict def\n"
            "Maxwell10Dict begin\n"
            "/ds { moveto show } def\n"
            "/sf { findfont exch scalefont setfont } def\n"
            "/sc { setcolor } def\n"
            "/scs { /DeviceRGB setcolorspace } def\n"
            "/mt { moveto } def\n"
            "/lt { lineto } def\n"
            "/st { stroke } def\n"
            "/slw { setlinewidth } def\n"
            "/slj { setlinejoin } def\n"
            "/slc { setlinecap } def\n"
            "/sd { setdash } def\n"
            "/rf { rectfill } def\n"
            "/cp { closepath } def\n"
            "/ef { eofill } def\n"
            "/ec { eoclip } def\n"
            "/np { newpath } def\n"
            "/rc { rectclip } def\n"
            "/rs { rectstroke } def\n")
        == EOF) {
        MX_ERROR_THROW(err, MX_PS_FILE_ERROR);
    }
abort:;
}

void mx_print_device::startPage(int& err, int n, float page_width, float page_height, bool is_landscape)
{
    float scale;
    float translate;

    err = MX_ERROR_OK;

    charStyleSet = FALSE;

    scale = 72.0 / (25.4 * getScreenResolution());

    if (fprintf(tempFile, "%%%%Page: %d\n", n) == EOF) {
        MX_ERROR_THROW(err, MX_PS_FILE_ERROR);
    }
    if (fprintf(tempFile, "%%%%BeginPageSetup\n") == EOF) {
        MX_ERROR_THROW(err, MX_PS_FILE_ERROR);
    }
    if (fprintf(tempFile, "/pgsave save def\n") == EOF) {
        MX_ERROR_THROW(err, MX_PS_FILE_ERROR);
    }
    if (fprintf(tempFile, "%%%%EndPageSetup\n") == EOF) {
        MX_ERROR_THROW(err, MX_PS_FILE_ERROR);
    }
    if (is_landscape) {
        if (fprintf(tempFile, "90 rotate\n") == EOF) {
            MX_ERROR_THROW(err, MX_PS_FILE_ERROR);
        }
    } else {
        translate = MX_MM_TO_POINTS(page_height);
        if (fprintf(tempFile, "0 %.2f translate\n", translate) == EOF) {
            MX_ERROR_THROW(err, MX_PS_FILE_ERROR);
        }
    }
    if (fprintf(tempFile, "%.2f %.2f scale\n", scale, scale) == EOF) {
        MX_ERROR_THROW(err, MX_PS_FILE_ERROR);
    }
    if (fprintf(tempFile, "scs\n") == EOF) {
        MX_ERROR_THROW(err, MX_PS_FILE_ERROR);
    }

    resetStyles();
abort:;
}

void mx_print_device::endPage(int& err)
{
    err = MX_ERROR_OK;
    if (fprintf(tempFile, "showpage\n") == EOF) {
        MX_ERROR_THROW(err, MX_PS_FILE_ERROR);
    }
    if (fprintf(tempFile, "pgsave restore\n") == EOF) {
        MX_ERROR_THROW(err, MX_PS_FILE_ERROR);
    }
abort:;
}

void mx_print_device::sendPostamble(int& err)
{
    char buffer[1024];
    unsigned int n;

    err = MX_ERROR_OK;

    if (!includeFonts) {
        if (fprintf(outputFile, "%%%%DocumentNeededResources:\n") == EOF) {
            MX_ERROR_THROW(err, MX_PS_FILE_ERROR);
        }
        sendFontResourceList(err);
        if (fprintf(outputFile, "\n") == EOF) {
            MX_ERROR_THROW(err, MX_PS_FILE_ERROR);
        }
    }

    if (fprintf(outputFile, "%%%%DocumentSuppliedResources: procset Maxwell10Dict\n") == EOF) {
        MX_ERROR_THROW(err, MX_PS_FILE_ERROR);
    }
    if (includeFonts) {
        sendFontResourceList(err);
        MX_ERROR_CHECK(err);
    }

    if (fprintf(outputFile, "%%%%BeginResource: procset Maxwell10Dict\n") == EOF) {
        MX_ERROR_THROW(err, MX_PS_FILE_ERROR);
    }

    sendProcset(err);
    MX_ERROR_CHECK(err);

    if (fprintf(outputFile, "%%%%EndResource\n") == EOF) {
        MX_ERROR_THROW(err, MX_PS_FILE_ERROR);
    }

    if (includeFonts) {
        sendFonts(err);
        MX_ERROR_CHECK(err);
    }

    if (fprintf(outputFile, "%%%%BeginProlog\n") == EOF) {
        MX_ERROR_THROW(err, MX_PS_FILE_ERROR);
    }
    if (fprintf(outputFile, "%%%%EndProlog\n") == EOF) {
        MX_ERROR_THROW(err, MX_PS_FILE_ERROR);
    }

    if (fprintf(outputFile, "%%%%BeginSetup\n") == EOF) {
        MX_ERROR_THROW(err, MX_PS_FILE_ERROR);
    }
    if (fprintf(outputFile, "Maxwell10Dict begin\n") == EOF) {
        MX_ERROR_THROW(err, MX_PS_FILE_ERROR);
    }

    sendFontEncodings(err);
    MX_ERROR_CHECK(err);

    if (fprintf(outputFile, "%%%%EndSetup\n") == EOF) {
        MX_ERROR_THROW(err, MX_PS_FILE_ERROR);
    }

    if (fprintf(tempFile, "%%%%Trailer\n") == EOF) {
        MX_ERROR_THROW(err, MX_PS_FILE_ERROR);
    }
    if (fprintf(tempFile, "%%%%EOF\n") == EOF) {
        MX_ERROR_THROW(err, MX_PS_FILE_ERROR);
    }

    if (fseek(tempFile, 0, SEEK_SET) == EOF) {
        MX_ERROR_THROW(err, MX_PS_FILE_ERROR);
    }

    do {
        n = fread(buffer, 1, 1024, tempFile);
        if (n > 0) {
            if (fwrite(buffer, 1, n, outputFile) != n) {
                MX_ERROR_THROW(err, MX_PS_FILE_ERROR);
            }
        } else {
            if (ferror(tempFile)) {
                MX_ERROR_THROW(err, MX_PS_FILE_ERROR);
            }
        }
    } while (n == 1024);

    if (fclose(tempFile) == EOF) {
        MX_ERROR_THROW(err, MX_PS_FILE_ERROR);
    }
abort:;
}

void mx_print_device::setOutputFile(int& err, FILE* f)
{
    char file_name[MAX_PATH_LEN];

    (void)mx_tmpnam(file_name);
    tempFile = fopen(file_name, "w+");

    if (tempFile == NULL) {
        MX_ERROR_THROW(err, mx_translate_file_error(errno));
    }

    outputFile = f;

abort:;
}

void mx_print_device::sendFonts(int& err)
{
    mx_list_iterator iter(*document_fonts);

    mx_font* f;

    const char* file_name;
    FILE* file;
    char buffer[1024];
    unsigned int n;

    while (iter.more()) {
        f = (mx_font*)iter.data();
        file_name = f->get_filename().c_str();

        if (strcmp(file_name + strlen(file_name) - 4, ".pfb") == 0) {
            // it's a pfb - turn it into a temporary pfa
            const char* tmp = mx_tmpnam(NULL);
            //mx_pfb_2_pfa(err, file_name, tmp);
            MX_ERROR_CHECK(err);
            file_name = tmp;
        }

        file = fopen(file_name, "r");
        if (file == NULL) {
            continue;
        } else {
            //if (fprintf(outputFile, "%%%%BeginResource: font %s\n", f->get_ps_fontname() + 1) == EOF) {
            if (fprintf(outputFile, "%%%%BeginResource: font %s\n", "NONE" + 1) == EOF) {
                MX_ERROR_THROW(err, MX_PS_FILE_ERROR);
            }

            do {
                n = fread(buffer, 1, 1024, file);
                if (n > 0) {
                    if (fwrite(buffer, 1, n, outputFile) != n) {
                        MX_ERROR_THROW(err, MX_PS_FILE_ERROR);
                    }
                } else {
                    if (ferror(outputFile)) {
                        MX_ERROR_THROW(err, MX_PS_FILE_ERROR);
                    }
                }
            } while (n == 1024);

            if (fprintf(outputFile, "%%%%EndResource\n") == EOF) {
                MX_ERROR_THROW(err, MX_PS_FILE_ERROR);
            }
        }

        if (fclose(file) == EOF) {
            MX_ERROR_THROW(err, MX_PS_FILE_ERROR);
        }
    }
abort:;
}

void mx_print_device::sendFontEncodings(int& err)
{
    mx_list_iterator iter(*document_fonts);

    mx_font* f;
    const char* latin_encoding_string = "/Encoding ISOLatin1Encoding def";
    const char* no_change_encoding_string = "";
    const char* encoding_string;
    bool b;

    while (iter.more()) {
        f = (mx_font*)iter.data();

        b = f->has_latin_encoding(err);
        MX_ERROR_CHECK(err);

        if (b) {
            encoding_string = latin_encoding_string;
        } else {
            encoding_string = no_change_encoding_string;
        }

        // remap the font to maxwell font
        if (fprintf(outputFile,
                "%s findfont dup length dict begin\n"
                "{1 index /FID ne {def} {pop pop} ifelse} forall\n"
                "%s currentdict\n"
                "end %s-Maxwell exch definefont pop\n",
                //f->get_ps_fontname(),
                "NONE",
                encoding_string,
                //f->get_ps_fontname())
                "NONE") == EOF) {
            MX_ERROR_THROW(err, MX_PS_FILE_ERROR);
        }
    }
abort:;
}

void mx_print_device::sendFontResourceList(int& err)
{
    mx_list_iterator iter(*document_fonts);

    mx_font* f;

    while (iter.more()) {
        f = (mx_font*)iter.data();

        //if (fprintf(outputFile, "%%%%+ font %s\n", f->get_ps_fontname() + 1) == EOF) {
        if (fprintf(outputFile, "%%%%+ font %s\n", "NONE" + 1) == EOF) {
            MX_ERROR_THROW(err, MX_PS_FILE_ERROR);
        }
    }
abort:;
}

void mx_print_device::setPolypointPath(int& err,
    const mx_ipolypoint& pp,
    bool doNotClosePath)
{
    int i;

    err = MX_ERROR_OK;

    for (i = 0; i < pp.get_num_points(); i++) {
        if (i == 0) {
            if (fprintf(tempFile, "%d %d mt\n", pp[i].x, -pp[i].y) == EOF) {
                MX_ERROR_THROW(err, MX_PS_FILE_ERROR);
            }
        } else {
            if (fprintf(tempFile, "%d %d lt\n", pp[i].x, -pp[i].y) == EOF) {
                MX_ERROR_THROW(err, MX_PS_FILE_ERROR);
            }
        }
    }

    if ((!doNotClosePath) && (pp.get_closed())) {
        if (fprintf(tempFile, "cp\n") == EOF) {
            MX_ERROR_THROW(err, MX_PS_FILE_ERROR);
        }
    }

abort:;
}

void mx_print_device::setTransform(int& err,
    const char* saveName,
    bool doShift,
    const mx_point& shift,
    bool doScale,
    const mx_point& scaleFactor,
    bool doRotation,
    const mx_angle& angle)
{
    err = MX_ERROR_OK;

    // save the old state
    if (fprintf(tempFile, "/%s matrix currentmatrix def\n", saveName) == EOF) {
        MX_ERROR_THROW(err, MX_PS_FILE_ERROR);
    }

    if (doShift) {
        if (fprintf(tempFile, "%.2f %.2f translate\n", shift.x, -shift.y) == EOF) {
            MX_ERROR_THROW(err, MX_PS_FILE_ERROR);
        }
    }

    if (doScale) {
        if (fprintf(tempFile, "%.2f %.2f scale\n", scaleFactor.x, scaleFactor.y) == EOF) {
            MX_ERROR_THROW(err, MX_PS_FILE_ERROR);
        }
    }

    if (doRotation) {
        if (fprintf(tempFile, "%.2f rotate\n", -angle.a() * RADTODEG) == EOF) {
            MX_ERROR_THROW(err, MX_PS_FILE_ERROR);
        }
    }
abort:;
}

void mx_print_device::resetTransform(int& err,
    const char* saveName)
{
    err = MX_ERROR_OK;

    if (fprintf(tempFile, "%s setmatrix\n", saveName) == EOF) {
        MX_ERROR_THROW(err, MX_PS_FILE_ERROR);
    }
abort:;
}
