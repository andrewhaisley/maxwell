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
 * MODULE/CLASS : mx_sizes
 *
 * AUTHOR : Andrew Haisley
 *
 *
 *
 * DESCRIPTION:
 *
 * Styles for everything. Ranging from colours/fonts for characters
 * through to paragraph justification types......
 *
 *
 *
 *
 */
#include <mx.h>

#include <mx_sizes.h>

const char* mx_paper_size_names[MX_NUM_PAPER_SIZES] = {
    "A4",
    "A0", "A1", "A2", "A3", "A5", "A6", "A7",
    "Letter (8½ × 11 in)", "Legal (8½ × 14 in)", "Executive (7¼ × 10½ in)"
};

float mx_paper_widths[MX_NUM_PAPER_SIZES] = {
    210,
    841, 594, 420, 297, 148, 105, 74,
    215.9, 215.9, 184.15
};

float mx_paper_heights[MX_NUM_PAPER_SIZES] = {
    297,
    1189, 841, 594, 420, 210, 148, 105,
    279.4, 355.6, 266.7
};

const char* mx_envelope_size_names[MX_NUM_ENVELOPE_SIZES] = {
    "C5", "C6", "C65", "DL", "Italian",
    "Size 6¾", "Size 9", "Size 10", "Size 11", "Size 12"
};

float mx_envelope_widths[MX_NUM_ENVELOPE_SIZES] = {
    162, 114, 114, 110, 110,
    92.075, 94.825, 104.775, 114.3, 120.65
};

float mx_envelope_heights[MX_NUM_ENVELOPE_SIZES] = {
    229, 162, 229, 220, 230,
    165.1, 225.425, 241.3, 263.525, 279.4
};

const char* mx_paper_size_name(float w, float h)
{
    int i;

    for (i = 0; i < MX_NUM_PAPER_SIZES; i++) {
        if (MX_FLOAT_EQ(w, mx_paper_widths[i]) && MX_FLOAT_EQ(h, mx_paper_heights[i])) {
            return mx_paper_size_names[i];
        }
    }
    return NULL;
}

const char* mx_envelope_size_name(float w, float h)
{
    int i;

    for (i = 0; i < MX_NUM_ENVELOPE_SIZES; i++) {
        if (MX_FLOAT_EQ(w, mx_envelope_widths[i]) && MX_FLOAT_EQ(w, mx_envelope_heights[i])) {
            return mx_envelope_size_names[i];
        }
    }
    return NULL;
}
