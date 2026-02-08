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
#ifndef MX_STYLE_H
#define MX_STYLE_H
/*
 * MODULE/CLASS : styles of all sorts
 *
 * AUTHOR : Andrew Haisley/Tom Newton
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
 */

typedef enum {
    mx_gstyle_unknown,
    mx_gstyle_point,
    mx_gstyle_dash,
    mx_gstyle_line,
    mx_gstyle_hatch,
    mx_gstyle_fill,
    mx_gstyle_border,
    mx_gstyle_area
} mx_gstyle_type_t;

#include <mx_area_style.h>
#include <mx_bd_style.h>
#include <mx_char_style.h>
#include <mx_colour.h>
#include <mx_dash_style.h>
#include <mx_doc_coord.h>
#include <mx_doc_rect.h>
#include <mx_fill_style.h>
#include <mx_font.h>
#include <mx_hatch_style.h>
#include <mx_line_style.h>
#include <mx_para_style.h>
#include <mx_point_style.h>
#include <mx_ruler.h>
#include <mx_spline.h>

#endif // MX_STYLE_H
