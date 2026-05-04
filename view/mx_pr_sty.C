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

void mx_print_device::setDashStyle(int& err, int ndashes, float* dashes, float dashOffset)
{
}

/*-------------------------------------------------
 * FUNCTION: mx_print_device::setDefaultLineStyle
 *
 * DESCRIPTION:
 *
 *
 */

void mx_print_device::setDefaultLineStyle(int& err, mx_line_style& s)
{
}

/*-------------------------------------------------
 * FUNCTION: mx_print_device::setFillStyle
 *
 * DESCRIPTION:
 *
 *
 */

void mx_print_device::setDefaultFillStyle(int& err, mx_fill_style& s)
{
}

// set the current postscript font to the one indicated by the lastCharStyle
void mx_print_device::setFont(int& err)
{
#if 0
    if (!isLastFontSet || !MX_FLOAT_EQ(lastFontSizeSet, f->get_size()) || strcmp(lastFontNameSet, "NONE") != 0) {
        float scale;
        const char *s, *str;

        isLastFontSet = TRUE;
        strcpy(lastFontNameSet, "NONE");
        lastFontSizeSet = f->get_size();

        mx_list_iterator iter(*document_fonts);

        scale = f->get_size() / (72.0 / (25.4 * getScreenResolution()));

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
#endif
}

void mx_print_device::setColour(int& err, const mx_colour& c)
{
    bool setColours;
    int i;

#define PDSC(x) ((x * 1.0) / 255.0)

    lastColourSet.setColourChanges(!isLastColourSet, c, setColours);
    isLastColourSet = TRUE;

    if (setColours) {
        if (c.name == NULL) {
            cairo_set_source_rgb(m_cairo, PDSC(c.red), PDSC(c.green), PDSC(c.blue));
        } else {
            for (i = 0; i < MX_NUM_COLOURS; i++) {
                if (strcmp(mx_colour_names[i], c.name) == 0) {
                    cairo_set_source_rgb(m_cairo, PDSC(mx_colour_values[i].r), PDSC(mx_colour_values[i].g), PDSC(mx_colour_values[i].b));
                    return;
                }
            }
            cairo_set_source_rgb(m_cairo, 0, 0, 0);
        }
    }
}

void mx_print_device::resetStyles()
{
    isLastColourSet = isLastLineStyleSet = isLastFillStyleSet = isLastFontSet = FALSE;
}
