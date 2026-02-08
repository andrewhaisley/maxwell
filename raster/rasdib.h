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
 * MODULE : rasdib.h
 *
 * AUTHOR : David Miller
 *
 * This file
 *
 * DESCRIPTION: A DIB raster
 * Module rasdib.h
 *
 *
 *
 *
 * $id$
 * $log$
 *
 */

#ifndef RASDIB_H
#define RASDIB_H

#include "mx_mxmetafile.h"
#include "raster.h"

/*-------------------------------------------------
 * CLASS: rasdib
 *
 * DESCRIPTION: A dib based raster
 *
 *
 */

class rasdib : public raster {
public:
    rasdib(int& err,
        mx_mxmetafile_output& metafileData,
        RAST_CACHE cacheType = RCACHE_NONE);
    ~rasdib();
    unsigned char* getTile(int& err,
        unsigned char* thisBuffer,
        bool getForEdit,
        bool& unknown,
        mx_ipoint& tile);

protected:
private:
    unsigned char hexStr(char str);
    unsigned char* dibbuffer;
};

#endif
