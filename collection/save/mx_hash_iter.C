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
#include <mx.h>
#include <mx_hash.h>
#include <mx_hash_iter.h>
#include <mx_rtti.h>

mx_hash_iterator::mx_hash_iterator(mx_hash& hash)
    : the_hash(hash)
{
    rewind();
}

const void* mx_hash_iterator::data()
{
    return last_value;
}

int mx_hash_iterator::int_key()
{
    return reinterpret_cast<long>(last_key);
}

const char* mx_hash_iterator::string_key()
{
    return (char*)last_key;
}

void mx_hash_iterator::rewind()
{
    int err = MX_ERROR_OK;
    the_hash.iterate_start(err);
    MX_ERROR_CLEAR(err);
}

bool mx_hash_iterator::more()
{
    int err = MX_ERROR_OK;

    last_value = the_hash.iterate_next_key_data(err, &last_key);
    MX_ERROR_CHECK(err);

    return last_value != NULL;

abort:
    MX_ERROR_CLEAR(err);
    return FALSE;
}
