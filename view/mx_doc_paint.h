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
 * MODULE/CLASS : mx_document_painter
 *
 * AUTHOR : Andrew Haisley
 *
 *
 *
 * DESCRIPTION:
 *
 * A class to draw documents onto devices
 *
 *
 *
 *
 *
 */
#ifndef MX_DOC_PAINTER_H
#define MX_DOC_PAINTER_H

#include <mx.h>
#include <mx_doc_coord.h>
#include <mx_doc_rect.h>
#include <mx_paintable.h>
#include <mx_painter.h>
#include <mx_spline.h>
#include <mx_wp_pos.h>

class mx_device;

class mx_document_painter : public mx_painter {
    MX_RTTI(mx_document_painter_class_e)

public:
    // these routines are needed by the document to perform drawing

    // use this to get a painter. There is in fact only one
    // instance in the system.....
    static mx_document_painter* painter();

    // draw a bit of the document
    virtual void draw(int& err, mx_doc_rect_t& r, mx_paintable& o);

    virtual ~mx_document_painter();

    // set the highlight range
    void set_highlight_range(mx_wp_doc_pos& start, mx_wp_doc_pos& end);

    // clear the highlight
    void clear_highlight();

private:
    // the constructor is private to stop instances being created. use the
    // painter() routine to get an instance to use.
    mx_document_painter();

    void draw_highlight(int& err, int sheet_number, mx_sheet* s);

    static mx_document_painter* the_painter;

    static void init();

    mx_wp_doc_pos highlight_start;
    mx_wp_doc_pos highlight_end;

    bool is_highlight;
};

#endif
