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
 * MODULE : rasxbm.h
 *
 * AUTHOR : David Miller
 *
 * This file
 *
 * DESCRIPTION: A XBM raster
 * Module rasxbm.h
 *
 *
 *
 *
 * $id$
 * $log$
 *
 */

#ifndef RASXBM_H
#define RASXBM_H

#include "raster.h"

/*-------------------------------------------------
 * CLASS: rasxbm
 *
 * DESCRIPTION: A xbm based raster
 *
 *
 */

class rasxbm : public raster {
public:
    rasxbm(int& err,
        char* filename,
        bool isBitmap,
        RAST_CACHE cacheType = RCACHE_NONE);
    ~rasxbm();
    unsigned char* getTile(int& err,
        unsigned char* thisBuffer,
        bool getForEdit,
        bool& unknown,
        mx_ipoint& tile);

protected:
private:
    void xpm(int& err, char* filename);
    void xbm(int& err, char* filename);
    unsigned char* xbmbuffer;
};

#endif
