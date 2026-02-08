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
 * MODULE : raspng.C
 *
 * AUTHOR : David Miller
 *
 * This file
 *
 * DESCRIPTION: A TIFF raster
 * Module raspng.C
 *
 *
 *
 *
 * $id$
 * $log$
 *
 */

#include "raspng.h"

/*-------------------------------------------------
 * FUNCTION: pngErrorFn
 *
 * DESCRIPTION: Horrible routine for png library
 *
 *
 */

#if 0
static void pngErrorFn(png_structp png_ptr, png_const_charp message)
{
  /* Return control to the setjmp point */
  //longjmp(png_ptr->jmpbuf, 1);  
}

/*-------------------------------------------------
 * FUNCTION: pngWarningFn
 *
 * DESCRIPTION: Horrible routine for png library 
 * 
 *
 */

static void pngWarningFn(png_structp png_ptr, png_const_charp message)
{
}
#endif

/*-------------------------------------------------
 * FUNCTION: raspng::reset
 *
 * DESCRIPTION:
 *
 *
 */

void raspng::reset(int& err)
{
#if 0
   /* Set the background color to draw transparent and alpha
      images over */
   png_color_16 my_background = {0,65535,65535,65535,65535} ;

   err = MX_ERROR_OK ;

   /* seek to beginning of file */

   fseek(fp,0L,0) ;


   /* read the file information */
   png_read_info(&pngStruct, &pngInfo);

   /* set up the transformations you want.  Note that these are
      all optional.  Only call them if you want them */

   /* expand grayscale images to the full 8 bits */
   if (pngInfo.color_type == PNG_COLOR_TYPE_GRAY &&
        (pngInfo.bit_depth > 1) && (pngInfo.bit_depth < 8))
      png_set_expand(&pngStruct);

   /* expand images with transparency to full alpha channels */
   if (pngInfo.valid & PNG_INFO_tRNS)
     png_set_expand(&pngStruct);

   if(pngInfo.valid & PNG_INFO_bKGD)
   {
     png_set_background(&pngStruct, &(pngInfo.background),
                        PNG_BACKGROUND_GAMMA_FILE, 1, 1.0);
   }
   else
   {
     png_set_background(&pngStruct, &my_background,
                        PNG_BACKGROUND_GAMMA_SCREEN, 0, 1.0);
   }

   /* tell libpng to handle the gamma conversion for you */
   if (pngInfo.valid & PNG_INFO_gAMA)
   {
     png_set_gamma(&pngStruct,2.5, pngInfo.gamma);
   }
   else
   {
     png_set_gamma(&pngStruct,2.5, 0.45);
   }
   
   /* tell libpng to strip 16 bit depth files down to 8 bits */
   if (pngInfo.bit_depth == 16)
      png_set_strip_16(&pngStruct);


/* FOR TESTING COLOUR

   if (pngInfo.color_type & PNG_COLOR_MASK_COLOR)
   {
      if (pngInfo.valid & PNG_INFO_PLTE)
         png_set_dither(&pngStruct, 
                        pngInfo.palette,
                        pngInfo.num_palette,
                        256,
                        pngInfo.hist,TRUE);
      else
      {
        png_color std_color_cube[27] =  { 
                                          {0,0,0},
                                          {0,0,127},
                                          {0,0,255},
                                          {0,127,0},
                                          {0,127,127},
                                          {0,127,255},
                                          {0,255,0},
                                          {0,255,127},
                                          {0,255,255},

                                          {127,0,0},
                                          {127,0,127},
                                          {127,0,255},
                                          {127,127,0},
                                          {127,127,127},
                                          {127,127,255},
                                          {127,255,0},
                                          {127,255,127},
                                          {127,255,255},

                                          {255,0,0},
                                          {255,0,127},
                                          {255,0,255},
                                          {255,127,0},
                                          {255,127,127},
                                          {255,127,255},
                                          {255,255,0},
                                          {255,255,127},
                                        {255,255,255} 
                                        } ;
        
         
        
        
          png_color *std_color_cube2 = new png_color[27] ;
          MX_ALLOC_CHECK(err,std_color_cube2) ;

          memcpy(std_color_cube2,std_color_cube,sizeof(std_color_cube)) ;
        
          png_set_dither(&pngStruct, std_color_cube2,27,27,NULL,TRUE) ;
      }
   }
*/


   /* shift the pixels down to their true bit depth */
   if (pngInfo.valid & PNG_INFO_sBIT) 
      png_set_shift(&pngStruct, &(pngInfo.sig_bit));

   /* pack pixels into bytes */
   if ((pngInfo.color_type == PNG_COLOR_TYPE_GRAY) && 
       (pngInfo.bit_depth > 1) && (pngInfo.bit_depth < 8))
      png_set_packing(&pngStruct);

   /* turn on interlace handling if you are not using png_read_image() */
   if(pngInfo.interlace_type)
   {
     numberPasses = png_set_interlace_handling(&pngStruct);
   }
   else
   {
     numberPasses = 1;
   }

   currentLine = 0 ;
   
   /* optional call to update the info structure */
   png_read_update_info(&pngStruct,&pngInfo);

   return ;
#endif
}

/*-------------------------------------------------
 * FUNCTION: raspng::~raspng
 *
 * DESCRIPTION: Destructor of a png raster
 *
 *
 */

raspng::~raspng()
{
#if 0
    // Set the error jump for this routine

    delete imageData ;
    
    if(fp != NULL) 
    {
        fclose(fp);
        fp = NULL ;
    }
#endif
}

/*-------------------------------------------------
 * FUNCTION: rasjpeg::rasjpeg
 *
 * DESCRIPTION: Constructor of a jpeg raster
 *
 *
 */

raspng::raspng(int& err,
    char* filename,
    RAST_CACHE cacheType)
    : raster(err)
{
#if 0
   int bytesPerRow ;
   int imageSize ;
   png_bytep *row_pointers ;
   unsigned char *startPtr ;
   int irow ;
   
   fp = NULL ;
   imageData = NULL ;
   
   MX_ERROR_CHECK(err) ;
   
   /* open the file */
   fp = fopen(filename,"rb")  ;
   if(fp == NULL) 
   {
       MX_ERROR_THROW(err,MX_ERROR_NO_PNG_FILE) ;
   }

   /* set error handling */
   if (setjmp(pngStruct.jmpbuf))
   {
       MX_ERROR_THROW(err,MX_ERROR_INTERNAL_PNG_ERROR) ;
   }

   /* if you are using replacement message functions, here you would call */
   png_set_message_fn(&pngStruct,NULL, pngErrorFn, pngWarningFn);

   /* initialize the structures, info first for error handling */
   png_info_init(&pngInfo);
   png_read_init(&pngStruct);

   /* set up the input control if you are using standard C streams */
   png_init_io(&pngStruct, fp);

   // Set up the jpeg internal structures
   reset(err) ;
   MX_ERROR_CHECK(err) ;


   if(pngInfo.color_type == PNG_COLOR_TYPE_GRAY)
   {
      nplanes = 1 ;
      pack = (pngInfo.bit_depth == 1) ? RPACK_BIT : RPACK_BYTE ;
      data = (pngInfo.bit_depth == 1) ? RDATA_BINARY : RDATA_GREYSCALE ;
      numColours = 0 ;
      colours    = NULL ;
   }
   else if(pngInfo.color_type == PNG_COLOR_TYPE_PALETTE)
   {
     data = RDATA_COLOUR ;
     pack = RPACK_BYTE ;
     nplanes = 1 ;
   }
   else
   {
     nplanes = 3 ;
     data = RDATA_RGB ;
     pack = RPACK_BYTE ;
     numColours = 0 ;
     colours    = NULL ;
   }

   if(data == RDATA_COLOUR) 
   {
     if(pngInfo.num_palette > 0) 
     {
       numColours = pngInfo.num_palette ;
     }
     else
     {
       numColours = pngStruct.num_palette ;
     }
       
     sigbits[0] = sigbitsFromColours() ;

     // Allocate space for the colours
     colours = new mx_colour[numColours] ;

      // Initialise colours
     for(int icol=0;icol<numColours;icol++) 
     {
       if(pngInfo.num_palette > 0) 
       {
         colours[icol].red    = ((pngInfo.palette[icol]).red) ;
         colours[icol].green  = ((pngInfo.palette[icol]).green) ;
         colours[icol].blue   = ((pngInfo.palette[icol]).blue) ;
       }
       else
       {
         colours[icol].red    = ((pngStruct.palette[icol]).red) ;
         colours[icol].green  = ((pngStruct.palette[icol]).green) ;
         colours[icol].blue   = ((pngStruct.palette[icol]).blue) ;
       }         
       colours[icol].name   = NULL ;
     }
   }
   else if(data == RDATA_BINARY) 
   {
     sigbits[0] = 1 ;     
   }
   else
   {
      // All planes have the same number of bits 
      for(int iplane=0;iplane<nplanes;iplane++) 
      {
         sigbits[iplane] = 8 ;
      }
   }

   size.x = pngInfo.width;
   size.y = pngInfo.height;
   tileSize.x = size.x ;

   bytesPerRow = pngInfo.rowbytes ;
   
   tileSize.y = 16384/bytesPerRow ;
   if(tileSize.y == 0) tileSize.y = 1 ;
   if(tileSize.y > size.y) tileSize.y = size.y ;

   defaultMMsize() ;
   
   if (pngInfo.valid & PNG_INFO_pHYs)
   {
       double convFactor ;
       
       if(pngInfo.phys_unit_type == 1) 
       {
           convFactor      = 0.001 ;
       }
       else
       {
           convFactor      = 1.0 ;
       }
       
       // pixels per meter
       
       hasStoredMMsize = (convFactor != 1.0);
       storedMMsize.x = size.x/(pngInfo.x_pixels_per_unit*0.001) ;
       storedMMsize.y = size.y/(pngInfo.y_pixels_per_unit*0.001) ;
   }
   
   // Interlaced - read whole image in 

   if(numberPasses > 1) 
   {
     imageSize = bytesPerRow*getTotalSize().y ;
     imageData = new unsigned char[imageSize] ;

     memset(imageData,0,imageSize) ;

     /* the easiest way to read the image */
   
     row_pointers = new png_bytep[pngInfo.height] ;
     
     startPtr = imageData ;
     for(irow=0;irow<(int)pngInfo.height;irow++) 
     {
       row_pointers[irow] = startPtr ;
       startPtr += bytesPerRow ;
     }
     
     png_read_image(&pngStruct, row_pointers);

     /* read the rest of the file, getting any additional chunks
        in info_ptr */
     png_read_end(&pngStruct,&pngInfo);
     
     /* clean up after the read, and free any memory allocated */
     png_read_destroy(&pngStruct,&pngInfo,NULL);
     
     delete row_pointers ;
   }
   
   createCache(err,cacheType) ;
   MX_ERROR_CHECK(err) ;

   return ;
  abort:
   return ;
#endif
}

/*-------------------------------------------------
 * FUNCTION: raspng::getTile
 *
 * DESCRIPTION: The virtual function for getting a
 * tile of raster from a TIFF file. The tile is
 * a TIFF strip
 */

unsigned char* raspng::getTile(int& err,
    unsigned char* buffer,
    bool getForEdit,
    bool& unknown,
    mx_ipoint& itile)
{

#if 0 
  err = MX_ERROR_OK ;
  unknown = FALSE ;
  
  if(numberPasses == 1) 
  {
    int lineWanted,irow ;
    png_bytep row_pointers ;

    /* set error handling */
    if (setjmp(pngStruct.jmpbuf))
    {
        MX_ERROR_THROW(err,MX_ERROR_INTERNAL_PNG_ERROR) ;
    }
    
    // Get the first scanline required 
    lineWanted = itile.y*tileSize.y ;

    // If we have already gone beyond the point
    // wanted then we need to start again
    if(currentLine > lineWanted)
    {
      /* read the rest of the file, getting any additional chunks
         in info_ptr */
      png_read_end(&pngStruct,&pngInfo);
      
      /* clean up after the read, and free any memory allocated */
      png_read_destroy(&pngStruct,&pngInfo,NULL);
      reset(err) ;
      MX_ERROR_CHECK(err) ;
    }

    // Now get up to the required line
    while(currentLine < lineWanted) 
    {
      row_pointers = buffer ;
      png_read_row(&pngStruct,row_pointers,NULL) ;
      currentLine++ ;
    }

    // Blat the buffer
    memset(buffer,0,tileByteSize()) ;   

    for(irow=0;irow<tileSize.y;irow++) 
    {
      // On the right line - do the read
      row_pointers = (buffer +irow*pngInfo.rowbytes) ;
      png_read_row(&pngStruct,row_pointers,NULL) ;
      currentLine++ ;
      if(currentLine == size.y) break ;
    }
  }
  else
  {
    mx_ipoint pt = getTotalSize() ;
    mx_irect totalSize(0,0,pt.x-1,pt.y-1) ;

    mx_irect tilePixels(itile.x*tileSize.x,
                     itile.y*tileSize.y, 
                     ((itile.x+1)*tileSize.x)-1,
                     ((itile.y+1)*tileSize.y)-1) ;
    

    copyBuffer(imageData, 
               totalSize,
               tilePixels,
               pixelBitSize(),
               buffer,
               tilePixels,
               tilePixels) ;
  }
  
   return buffer ;
 abort:
  return buffer ;
#endif
    buffer[0] = 0;
    return buffer;
}
