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
 * MODULE/CLASS :  mx_hash_iter
 *
 * AUTHOR : Andrew Haisley
 *
 *
 *
 * DESCRIPTION:
 *
 * A simple iterator for hash tables - not delete safe
 *
 *
 *
 *
 */
#ifndef MX_HASH_ITER_H
#define MX_HASH_ITER_H

#include <mx.h>
#include <mx_hash.h>
#include <mx_rtti.h>

class mx_hash_iterator // : public mx_rtti
{
    // MX_RTTI(mx_hash_iter_class_e)
public:
    mx_hash_iterator(mx_hash& hash);

    const void* data();
    int int_key();
    const char* string_key();
    void rewind();
    bool more();

private:
    const void* last_value;
    const void* last_key;

    mx_hash& the_hash;
};

#endif
