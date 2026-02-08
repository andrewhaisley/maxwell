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
 * MODULE/CLASS : mx_buffer
 *
 * AUTHOR : Andrew Haisley
 *
 *
 *
 * DESCRIPTION:
 *
 * The base class for buffers of all types. This hold the clipboard
 * temporary document that is used to hold bits and prices that have been
 * cut or copied.
 *
 *
 *
 *
 *
 */
#ifndef MX_BUFFER_H
#define MX_BUFFER_H

#include <mx.h>
#include <mx_rtti.h>

class mx_wp_document;

class mx_buffer : public mx_rtti {
    MX_RTTI(mx_buffer_class_e)

    void set_document(int& err, mx_wp_document* d);

protected:
    mx_buffer(mx_wp_document*);
    mx_buffer(const mx_buffer&);
    virtual ~mx_buffer();

    mx_wp_document* get_document();
    int get_docid();

    virtual void move_to_document(int& err, mx_wp_document* d) = 0;

    enum mx_edit_op {
        mx_cut_e,
        mx_copy_e,
        mx_remove_e
    };

private:
    mx_wp_document* doc;
};

#endif
