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
 * MODULE : rasjpeg.h
 *
 * AUTHOR : David Miller
 *
 * This file
 *
 * DESCRIPTION: A JPEG raster
 * Module rasjpeg.h
 *
 *
 *
 *
 * $id$
 * $log$
 *
 */

#ifndef RASJPEG_H
#define RASJPEG_H

#include "raster.h"

extern "C" {
#include "jerror.h"
#include "jpeglib.h"
#include "setjmp.h"
}

typedef struct
{
    struct jpeg_error_mgr pub; /* "public" fields */

    jmp_buf setjmp_buffer; /* for return to caller */
} MXerrorMgr;

/*-------------------------------------------------
 * CLASS: rasjpeg
 *
 * DESCRIPTION: A jpeg based raster
 *
 *
 */

class rasjpeg : public raster {
public:
    rasjpeg(int& err,
        char* filename,
        bool iget24bits,
        int incolours,
        RAST_CACHE cacheType = RCACHE_DICT);
    ~rasjpeg();
    void reset(int& err);
    unsigned char* getTile(int& err,
        unsigned char* thisBuffer,
        bool getForEdit,
        bool& unknown,
        mx_ipoint& tile);

protected:
private:
    FILE* fp;
    int ncolours;
    bool get24bits;
    struct jpeg_decompress_struct cinfo;
    MXerrorMgr jerr;
};

#endif
