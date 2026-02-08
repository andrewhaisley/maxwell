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
 * MODULE/CLASS : mx_image_area
 *
 * AUTHOR : Dave Miller
 *
 *
 *
 * DESCRIPTION:
 *
 * Areas specifically for holding images
 *
 *
 *
 */
#ifndef MX_IMAGE_AREA_H
#define MX_IMAGE_AREA_H

#include <mx.h>
#include <mx_db.h>

#include <mx_area.h>
#include <mx_mxmetafile.h>
#include <mxraster.h>

typedef enum {
    MX_RASTER_SUBSAMPLED,
    MX_RASTER_FULL
} mx_stored_raster_t;

typedef enum {
    MX_IMAGE_AREA_UNDEFINED,
    MX_IMAGE_AREA_TIFF,
    MX_IMAGE_AREA_JPEG,
    MX_IMAGE_AREA_PNG,
    MX_IMAGE_AREA_XBM,
    MX_IMAGE_AREA_XPM,
    MX_IMAGE_AREA_METAFILE,
    MX_IMAGE_AREA_MAXRAST
} mx_image_area_t;

class mx_image_area : public mx_area {
    MX_RTTI(mx_image_area_class_e)

public:
    mx_image_area(uint32 docid, mx_paragraph_style* s, bool is_controlled = FALSE);
    mx_image_area(uint32 docid, uint32 oid, bool is_controlled = FALSE);
    mx_image_area(const mx_image_area& a);
    virtual ~mx_image_area();

    virtual mx_area* duplicate();

    virtual void serialise(int& err, uint8** buffer);
    virtual void unserialise(int& err, uint8** buffer);
    virtual uint32 get_serialised_size(int& err);

    // move the object to another document - subclass must make sure that
    // all it's attributes and blobs are in the cache before calling this.
    virtual void move_to_document(int& err, mx_document* doc);

    // image areas are not flowable
    virtual bool is_flowable() { return FALSE; };

    // they are croppable
    virtual bool is_croppable() const { return TRUE; };

    // and scalable
    virtual bool is_scalable() const { return TRUE; };

    virtual void uncache(int& err);
    virtual uint32 memory_size(int& err);

    void init();

    void createMETAFILEarea(int& err,
        mx_mxmetafile_output& metafileData);

    void createTIFFarea(int& err,
        char* TIFFfile);

    void createXBMarea(int& err,
        char* XBMfile);

    void createXPMarea(int& err,
        char* XPMfile);

    void createPNGarea(int& err,
        char* PNGfile);

    void createJPEGarea(int& err,
        char* jpegfile,
        bool get24bits,
        int ncolours);

    inline mxraster* getSubsampledRaster() { return subsampledRaster; };
    inline raster* getInputRaster() { return inputRaster; };
    inline mxraster* getFullImportRaster() { return fullImportRaster; };
    inline raster* getDisplayRaster() { return displayRaster; };
    inline raster* getPrintRaster() { return printRaster; };
    raster* getSourceRaster();

    bool newDisplayRaster(int& err,
        double displayResolution,
        mx_point& totalSize,
        mx_ipoint& newRasterSize);

    raster* getBestRaster(int& err,
        bool& needsDeleting);

    raster* createDisplayRaster(int& err,
        double displayResolution,
        LUT_VALUE* lut,
        int lutSize,
        int32 ndisplayBits,
        RAST_DATA data,
        RAST_PACK pack,
        int ncolours,
        mx_colour* colours,
        mx_ipoint& newRasterSize);

    raster* createPrintRaster(int& err,
        double printResolution);

    mx_point areaSize(int& err,
        mx_point& pageSize,
        double pixelResolution);

    void createStoredRaster(int& err,
        mx_point& pageSize,
        double pixelResolution);

    void deleteInputRaster();
    void deleteDisplayRaster();
    void deletePrintRaster();

    void defaultColours(int* ndefColours,
        mx_colour** defColours);

    inline void setLastRasterSize(mx_ipoint& newRasterSize)
    {
        lastRasterSize = newRasterSize;
    };
    inline mx_ipoint getLastRasterSize() { return lastRasterSize; };

private:
    void createBasics(int& err,
        char* ifilename,
        mx_image_area_t itype,
        bool icreateSubsampled,
        bool icreateFullImport,
        int inumColours);

    void readBasics(int& err);

    void createSubsampled(int& err,
        mx_point& areaSize,
        mx_ipoint& pixelAreaSize);
    void createCopy(int& err);

    mx_image_area_t type;
    raster* inputRaster; // the raster based on the file
    mxraster* fullImportRaster; // a full import mxraster of the file
    mxraster* subsampledRaster; // a subsampled view of the file
    raster* displayRaster; // a raster used in display
    raster* printRaster; // a raster used in printing
    char* filename;
    int numColours;
    bool fullImportExists;
    bool subsampledExists;
    mx_ipoint lastRasterSize;
};

#endif
