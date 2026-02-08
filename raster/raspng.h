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
 * MODULE : raspng.h
 *
 * AUTHOR : David Miller
 *
 * This file
 *
 * DESCRIPTION: A PNG raster
 * Module raspng.h
 *
 *
 *
 *
 * $id$
 * $log$
 *
 */

#ifndef RASPNG_H
#define RASPNG_H

#include "png.h"
#include "raster.h"
#include "setjmp.h"

/*-------------------------------------------------
 * CLASS: raspng
 *
 * DESCRIPTION: A jpeg based raster
 *
 *
 */

class raspng : public raster {
public:
    raspng(int& err, char* filename, RAST_CACHE cacheType = RCACHE_DICT);
    ~raspng();
    void reset(int& err);
    unsigned char* getTile(int& err,
        unsigned char* thisBuffer,
        bool getForEdit,
        bool& unknown,
        mx_ipoint& tile);

protected:
private:
    FILE* fp;
    // png_struct  pngStruct;
    // png_info    pngInfo ;
    int numberPasses;
    int currentLine;
    unsigned char* imageData;
};

#endif
