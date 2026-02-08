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
 * MODULE : mx_geoms.C
 *
 * AUTHOR : David Miller
 *
 * This file
 *
 * DESCRIPTION:
 * Module mx_geoms.C
 *
 *
 * Revision 1.1  1996/10/06 13:15:45  andrew
 * Initial revision
 *
 *
 */

#include "mx_gstyle.h"

/*-------------------------------------------------
 * FUNCTION: mx_geomstyle::mx_geomstyle
 *
 * DESCRIPTION:
 *
 *
 */

mx_geomstyle::mx_geomstyle(const char* iname,
    mx_gstyle_type_t itype,
    int iid)
{
    name = NULL;

    setId(iid);
    setName(iname);
    setType(itype);
    setPrivate(FALSE);
}

/*-------------------------------------------------
 * FUNCTION: mx_geomstyle::~mx_geomstyle
 *
 * DESCRIPTION:
 *
 *
 */

mx_geomstyle::~mx_geomstyle()
{
    delete[] name;
}

/*-------------------------------------------------
 * FUNCTION: mx_geomstyle::setName
 *
 * DESCRIPTION:
 *
 *
 */

void mx_geomstyle::setName(const char* iname)
{
    delete[] name;

    name = NULL;

    if (iname != NULL) {
        name = new char[strlen(iname) + 1];
        strcpy(name, iname);
    }
}

/*-------------------------------------------------
 * FUNCTION: mx_geomstyle::get_serialised_size
 *
 * DESCRIPTION:
 *
 *
 */

uint32 mx_geomstyle::get_serialised_size(int& err)
{
    err = MX_ERROR_OK;

    return (2 * SLS_INT32 + SLS_STRING(name));
}

/*-------------------------------------------------
 * FUNCTION: mx_geomstyle::serialise
 *
 * DESCRIPTION:
 *
 *
 */

void mx_geomstyle::serialise(int& err,
    unsigned char** buffer)
{
    int32 val = 0;

    err = MX_ERROR_OK;

    serialise_int32(id, buffer);
    serialise_int32(val, buffer);
    serialise_string(name, buffer);
}

/*-------------------------------------------------
 * FUNCTION: mx_geomstyle::unserialise
 *
 * DESCRIPTION:
 *
 *
 */

void mx_geomstyle::unserialise(int& err,
    unsigned char** buffer)
{
    char iname[1000];
    int32 val = styleType;

    err = MX_ERROR_OK;

    unserialise_int32(id, buffer);
    unserialise_int32(val, buffer);
    styleType = (mx_gstyle_type_t)val;

    unserialise_string(iname, buffer);

    if (strlen(iname) == 0) {
        name = NULL;
    } else {
        name = new char[strlen(iname) + 1];
        strcpy(name, iname);
    }
}
