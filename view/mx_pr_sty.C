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
 * MODULE : mx_pr_sty.C
 *
 * AUTHOR : David Miller
 *
 * DESCRIPTION:
 * Module mx_pr_sty.C
 *
 *
 */

#include <mx_list_iter.h>
#include <mx_pr_device.h>

/*-------------------------------------------------
 * FUNCTION: mx_print_device::setDashStyle
 *
 * DESCRIPTION:
 *
 *
 */

void mx_print_device::setDashStyle(int& err,
    int ndashes,
    float* dashes,
    float dashOffset)
{
    int i;

    err = MX_ERROR_OK;

    if (fprintf(tempFile, "[") == EOF) {
        MX_ERROR_THROW(err, MX_PS_FILE_ERROR);
    }

    for (i = 0; i < ndashes; i++) {
        if (fprintf(tempFile, "%.2f ", frame2dev(dashes[i])) == EOF) {
            MX_ERROR_THROW(err, MX_PS_FILE_ERROR);
        }
    }

    if (fprintf(tempFile, "] ") == EOF) {
        MX_ERROR_THROW(err, MX_PS_FILE_ERROR);
    }

    if (fprintf(tempFile, "%.2f ", frame2dev(dashOffset)) == EOF) {
        MX_ERROR_THROW(err, MX_PS_FILE_ERROR);
    }

    if (fprintf(tempFile, "sd\n") == EOF) {
        MX_ERROR_THROW(err, MX_PS_FILE_ERROR);
    }
abort:;
}

/*-------------------------------------------------
 * FUNCTION: mx_print_device::setDefaultLineStyle
 *
 * DESCRIPTION:
 *
 *
 */

void mx_print_device::setDefaultLineStyle(int& err,
    mx_line_style& s)
{
    float dashes[2];
    bool setWidth, setJoin, setCap, setLineType;

    err = MX_ERROR_OK;

    lastLineStyle.setStyleChanges(!isLastLineStyleSet,
        s, setWidth, setJoin, setCap, setLineType);
    MX_ERROR_CHECK(err);

    isLastLineStyleSet = TRUE;

    setColour(err, s.colour);
    MX_ERROR_CHECK(err);

    if (setWidth) {
        if (fprintf(tempFile, "%.2f slw\n", frame2dev(s.width)) == EOF) {
            MX_ERROR_THROW(err, MX_PS_FILE_ERROR);
        }
    }

    if (setJoin) {
        switch (s.join_type) {
        case mx_join_bevel:
            if (fprintf(tempFile, "2 slj\n") == EOF) {
                MX_ERROR_THROW(err, MX_PS_FILE_ERROR);
            }
            break;
        case mx_join_round:
            if (fprintf(tempFile, "1 slj\n") == EOF) {
                MX_ERROR_THROW(err, MX_PS_FILE_ERROR);
            }
            break;
        default:
        case mx_join_miter:
            if (fprintf(tempFile, "0 slj\n") == EOF) {
                MX_ERROR_THROW(err, MX_PS_FILE_ERROR);
            }
            break;
        }
    }

    if (setCap) {
        switch (s.cap_type) {
        default:
        case mx_cap_butt:
        case mx_cap_butt_not_last:
            if (fprintf(tempFile, "0 slc\n") == EOF) {
                MX_ERROR_THROW(err, MX_PS_FILE_ERROR);
            }
            break;
        case mx_cap_round:
            if (fprintf(tempFile, "1 slc\n") == EOF) {
                MX_ERROR_THROW(err, MX_PS_FILE_ERROR);
            }
            break;
        case mx_cap_projecting:
            if (fprintf(tempFile, "2 slc\n") == EOF) {
                MX_ERROR_THROW(err, MX_PS_FILE_ERROR);
            }
            break;
        }
    }

    if (setLineType) {
        switch (lastLineStyle.line_type) {
        default:
        case mx_solid:
            setDashStyle(err, 0, NULL, 0);
            MX_ERROR_CHECK(err);
            break;
        case mx_dotted:
            dashes[0] = 0.5;
            setDashStyle(err, 1, dashes, 0);
            MX_ERROR_CHECK(err);
            break;
        case mx_dashed:
            dashes[0] = 3;
            dashes[1] = 1;
            setDashStyle(err, 2, dashes, 0);
            MX_ERROR_CHECK(err);
            break;
        case mx_custom_line:
            setDashStyle(err,
                s.dash_style.get_ndashes(),
                s.dash_style.get_dashes(),
                s.dash_style.get_dash_offset());
            MX_ERROR_CHECK(err);
            break;
        }
    }
abort:;
}

/*-------------------------------------------------
 * FUNCTION: mx_print_device::setFillStyle
 *
 * DESCRIPTION:
 *
 *
 */

void mx_print_device::setDefaultFillStyle(int& err,
    mx_fill_style& s)
{
    bool setFill;

    err = MX_ERROR_OK;

    lastFillStyle.setStyleChanges(!isLastFillStyleSet,
        s, setFill);
    MX_ERROR_CHECK(err);

    isLastFillStyleSet = TRUE;

    setColour(err, s.colour);
    MX_ERROR_CHECK(err);
abort:;
}

// set the current postscript font to the one indicated by the
// lastCharStyle
void mx_print_device::setFont(int& err)
{
    const mx_font* f;
    f = lastCharStyle.get_font();

    //if (!isLastFontSet || !MX_FLOAT_EQ(lastFontSizeSet, f->get_size()) || strcmp(lastFontNameSet, f->get_ps_fontname()) != 0) {
    if (!isLastFontSet || !MX_FLOAT_EQ(lastFontSizeSet, f->get_size()) || strcmp(lastFontNameSet, "NONE") != 0) {
        float scale;
        const char *s, *str;

        isLastFontSet = TRUE;
        //strcpy(lastFontNameSet, f->get_ps_fontname());
        strcpy(lastFontNameSet, "NONE");
        lastFontSizeSet = f->get_size();

        mx_list_iterator iter(*document_fonts);

        scale = f->get_size() / (72.0 / (25.4 * getScreenResolution()));

        //if (fprintf(tempFile, "%.2f %s-Maxwell sf\n", scale, s = f->get_ps_fontname()) == EOF) {
        if (fprintf(tempFile, "%.2f %s-Maxwell sf\n", scale, s = strdup("NONE")) == EOF) {
            MX_ERROR_THROW(err, MX_PS_FILE_ERROR);
        }

        while (iter.more()) {
            //str = ((mx_font*)iter.data())->get_ps_fontname();
            str = strdup("NONE");

            if (strcmp(str, s) == 0) {
                return;
            }
        }
        document_fonts->append(new mx_font(*f));
    }
abort:;
}

void mx_print_device::setColour(int& err, const mx_colour& c)
{
    bool setColours;
    int i;

#define PDSC(x) ((x * 1.0) / 255.0)

    lastColourSet.setColourChanges(!isLastColourSet,
        c, setColours);
    isLastColourSet = TRUE;

    if (setColours) {
        if (c.name == NULL) {
            if (fprintf(tempFile, "%.3f %.3f %.3f sc\n",
                    PDSC(c.red),
                    PDSC(c.green),
                    PDSC(c.blue))
                == EOF) {
                MX_ERROR_THROW(err, MX_PS_FILE_ERROR);
            }
        } else {
            for (i = 0; i < MX_NUM_COLOURS; i++) {
                if (strcmp(mx_colour_names[i], c.name) == 0) {
                    if (fprintf(tempFile, "%.3f %.3f %.3f sc\n",
                            PDSC(mx_colour_values[i].r),
                            PDSC(mx_colour_values[i].g),
                            PDSC(mx_colour_values[i].b))
                        == EOF) {
                        MX_ERROR_THROW(err, MX_PS_FILE_ERROR);
                    }
                    return;
                }
            }
            if (fprintf(tempFile, "0 0 0 sc\n") == EOF) {
                MX_ERROR_THROW(err, MX_PS_FILE_ERROR);
            }
        }
    }
abort:;
}

void mx_print_device::resetStyles()
{
    isLastColourSet = isLastLineStyleSet = isLastFillStyleSet = isLastFontSet = FALSE;
}
