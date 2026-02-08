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
 * MODULE : mx_dash_style.C
 *
 * AUTHOR : David Miller
 *
 * This file
 *
 * DESCRIPTION:
 * Module mx_dash_style.C
 *
 *
 * Revision 1.1  1996/10/06 13:15:45  andrew
 * Initial revision
 *
 *
 */

#include "mx_dash_style.h"

/*-------------------------------------------------
 * FUNCTION: mx_dash_style::init
 *
 * DESCRIPTION:
 *
 *
 */

void mx_dash_style::init()
{
    dashes = NULL;
    ndashes = 0;
    dash_id = -1;
    dash_offset = 0.0f;
}

/*-------------------------------------------------
 * FUNCTION: mx_dash_style::mx_dash_style
 *
 * DESCRIPTION:
 *
 *
 */

mx_dash_style::mx_dash_style()
{
    init();
}

/*-------------------------------------------------
 * FUNCTION: mx_dash_style::~mx_dash_style
 *
 * DESCRIPTION:
 *
 *
 */

mx_dash_style::~mx_dash_style()
{
    delete[] dashes;
}

/*-------------------------------------------------
 * FUNCTION: mx_dash_style::set
 *
 * DESCRIPTION:
 *
 *
 */

void mx_dash_style::set(int32 indashes,
    const float* idashes,
    float idash_offset,
    int32 idash_id)
{
    delete[] dashes;
    dashes = NULL;

    if (idashes == NULL) {
        init();
    } else {
        dashes = new float[indashes];
        ndashes = indashes;
        dash_offset = idash_offset;
        memcpy(dashes, idashes, sizeof(float) * indashes);
        dash_id = idash_id;
    }

    return;
}

/*-------------------------------------------------
 * FUNCTION: mx_dash_style::get_serialised_size
 *
 * DESCRIPTION:
 *
 *
 */

uint32 mx_dash_style::get_serialised_size(int& err)
{
    uint32 size = SLS_INT32;

    err = MX_ERROR_OK;

    if (ndashes != 0)
        size += (SLS_INT32 + SLS_FLOAT + SLS_FLOAT_A(ndashes));

    return size;
}

/*-------------------------------------------------
 * FUNCTION: mx_dash_style::serialise
 *
 * DESCRIPTION:
 *
 *
 */

void mx_dash_style::serialise(int& err,
    unsigned char** buffer)
{
    err = MX_ERROR_OK;

    serialise_int32(ndashes, buffer);

    if (ndashes != 0) {
        serialise_float(dash_offset, buffer);
        serialise_float_a(dashes, ndashes, buffer);
        serialise_int32(dash_id, buffer);
    }
}

/*-------------------------------------------------
 * FUNCTION: mx_dash_style::unserialise
 *
 * DESCRIPTION:
 *
 *
 */

void mx_dash_style::unserialise(int& err,
    unsigned char** buffer)
{
    err = MX_ERROR_OK;

    unserialise_int32(ndashes, buffer);

    if (ndashes == 0) {
        init();
    } else {
        unserialise_float(dash_offset, buffer);

        dashes = new float[ndashes];
        unserialise_float_a(dashes, ndashes, buffer);
        unserialise_int32(dash_id, buffer);
    }
}

bool operator==(const mx_dash_style& s1, const mx_dash_style& s2)
{
    if (s1.ndashes == s2.ndashes) {
        if (s1.ndashes == 0) {
            return TRUE;
        } else {
            return (s1.dash_offset == s2.dash_offset) && (memcmp(s1.dashes, s2.dashes, s1.ndashes * sizeof(float)) == 0);
        }
    } else {
        return FALSE;
    }
}

bool operator!=(const mx_dash_style& s1, const mx_dash_style& s2)
{
    return !(s1 == s2);
}

mx_dash_style& mx_dash_style::operator=(const mx_dash_style& idash_style)
{
    set(idash_style.ndashes,
        idash_style.dashes,
        idash_style.dash_offset,
        idash_style.dash_id);
    return *this;
}
