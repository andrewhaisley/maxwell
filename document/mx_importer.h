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
#ifndef MX_IMPORTER_H
#define MX_IMPORTER_H
/*
 * MODULE/CLASS : mx_importer
 *
 * AUTHOR : Tom Newton
 *
 *
 *
 * DESCRIPTION:
 *
 * An importer object is one which imports other things into documents. An
 * example is the RTF importer which is used to import RTF files. Has a very
 * simple interface - here is a document, here is a position, here is a file
 * name of the document to import - do it.
 *
 *
 *
 *
 *
 */

#include <mx.h>
#include <mx_rtti.h>

class mx_importer : public mx_rtti {
    MX_RTTI(mx_importer_class_e)

public:
    mx_importer(int& err, const char* f) { file_name = f; };
    virtual ~mx_importer() { };

    virtual void import(int& err) = 0;

protected:
    const char* file_name;
};

#endif
