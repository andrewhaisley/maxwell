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
 * MODULE : mx_stydev.h
 *
 * AUTHOR : David Miller
 *
 * This file 
 *
 * DESCRIPTION: 
 * Module mx_stydev.h 
 *
 *
 * Revision 1.1  1996/10/06 13:15:45  andrew
 * Initial revision
 *
 *
 */

#include "mx_device.h"
#include "mx_gstyle.h"
#include "mx_doc_rect.h"

/*-------------------------------------------------
 * CLASS: mx_style_device
 *
 * DESCRIPTION: A collection of styles.
 * 
 *
 */
  
// for efficiency a style (fill,colour and line) 
// may be in a number of states

class mx_style_defaults
{
public:
  int32           solidFillStyleId ;
  int32           solidAreaStyleId ;
  int32           solidBorderStyleId ;
  int32           XORLineStyleId ;
  int32           blackColourId ;
} ;

class mx_style_device
{
 public:
  mx_style_device() {reset();};

  mx_style_device(mx_device     *idevice,
		  mx_all_styles *istyles) ;

  ~mx_style_device() ;

  void setDevice(mx_device *idevice){device=idevice;};
  void setStyles(mx_all_styles *istyles){styles=istyles;reset();};

  inline mx_device *getDevice(){return device;};
  void   draw(int &err,mx_geom *geom) ;

  inline bool setUseDefaultStyles(bool iuseDefault) 
                                      {bool tempVal=useDefaultStyles;
				            useDefaultStyles=iuseDefault;
				            return tempVal;};

  inline bool getUseDefaultStyles() {return useDefaultStyles;};

  inline bool setUseDefaultColours(bool iuseDefault) 
                                      {bool tempVal=useDefaultColours;
				            useDefaultColours=iuseDefault;
				            return tempVal;};
  inline bool getUseDefaultColours() {return useDefaultColours;};

  inline mx_style_defaults *getDefaults(){return &defaults;};

private:
  int32           currentColourId ;
  bool            currentColourDict ;
  mx_colour       *currentColour ;

  int32           currentPointStyleId ;
  bool            currentPointDict ;
  mx_point_style  *currentPointStyle ;

  int32           currentDashStyleId ;
  bool            currentDashDict ;
  mx_dash_style   *currentDashStyle ;

  int32           currentLineStyleId ;
  bool            currentLineDict ;
  mx_line_style   *currentLineStyle ;

  int             currentHatchStyleId ;
  bool            currentHatchDict ;
  mx_hatch_style  *currentHatchStyle ;

  int32           currentFillStyleId ;
  bool            currentFillDict ;
  mx_fill_style   *currentFillStyle ;

  int32           currentAreaStyleId ;
  bool            currentAreaDict ;
  mx_area_style   *currentAreaStyle ;

  int32           currentBorderStyleId ;
  bool            currentBorderDict ;
  mx_border_style *currentBorderStyle ;

  mx_device       *device ;
  mx_all_styles   *styles ;

  mx_style_defaults defaults ;

  // styles used internally by the style device
  // these are hard coded and do not need to be
  // stored
  static bool          defaultStylesSet ;				   
  static mx_all_styles defaultStyles ;

  mx_line_style   outputLineStyle ;
  mx_fill_style   outputFillStyle ;

  // stack of doc coord polypoint ;
  static  int              numPolypoints ;
  static  int              nextPolypoint ; 
  static  mx_doc_polypoint **docpps ;

  mx_doc_polypoint *pushPolypoint() ;
  inline void      popPolypoint(){nextPolypoint--;} ;

  // polypoint for fills
  static  mx_doc_polypoint docppf ;

  bool useDefaultStyles  ;
  bool useDefaultColours ;

  // ids of preloaded private styles 



  void drawPolypoint(int               &err,
		     mx_doc_polypoint  &idocpp,
		     int               lineStyleId,
		     int               lineColourId) ;

  void hatchArc(int         &err,
		mx_doc_arc  &doca,
		int         hatchStyleId) ;

  void hatchPolypoint(int &err,
		      mx_doc_polypoint  &idocpp,
		      int hatchStyleId) ;

  void hatchBox(int       &err,
		mx_rect   &hatchbox) ;

  bool changeStyleStyles(int           newStyleId,
			 int           oldStyleId,
			 bool          currentDict,
			 mx_all_styles **useDict) ;

  bool changeColourStyles(int           newStyleId,
			  int           oldStyleId,
			  bool          currentDict,
			  mx_all_styles **useDict) ;

  void setupDefaultStyles(int &err) ;

  mx_fill_style *getFillStyle(int &err,
			      int styleId,
			      bool &isSame);
  mx_line_style *getLineStyle(int &err,
			      int styleId,
			      bool &isSame) ;
  bool setupOutputLineStyle(int  &err,
			    int  styleId,
			    int  colourId) ;

  bool setupOutputFillStyle(int  &err,
			    int  styleId,
			    int  colourId) ;

  bool setupOutputHatchStyle(int  &err,
			     int  styleId) ;

  bool setupOutputAreaStyle(int  &err,
			    int  styleId) ;

  bool setupOutputBorderStyle(int  &err,
			      int  styleId) ;

  bool setupOutputColourStyle(int  &err,
			      int  styleId) ;

  bool setupOutputDashStyle(int  &err,
			    int  styleId,
			    bool setLineStyle);

  bool setupOutputPointStyle(int  &err,
			     int  styleId);

  void drawArrow(int                     &err,
		 mx_doc_coord_t          &head,
		 mx_point                &span,
		 mx_point                &uspan,
		 mx_cap_type_t           capType,
		 mx_arrow_pattern_type_t arrowPattern,
		 float                   lineWidth,
		 int                     colourId,
		 double                  &lineback) ;
  
  double backArc(mx_arc      &a,
		 double   lineBack,
		 mx_point &ps,
		 mx_point &sp,
		 mx_point &usp) ;

  double backLines(mx_polypoint   &pline,
		   double         lineBack,
		   bool           doingStart,
		   mx_point       &sp,
		   mx_point       &usp) ;

  bool drawSelectedSpan(int &err,
			gmx_span *gspan) ;

  void drawSpan(int            &err,
		gmx_span       *span) ;

  bool drawSelectedRect(int &err,
			gmx_rect *grect) ;

  void drawRect(int            &err,
		gmx_rect       *rect) ;

  bool drawSelectedArc(int &err,
		       gmx_arc *garc) ;

  void drawArc(int            &err,
	       gmx_arc        *arc) ;

  void drawArc(int        &err,
	       mx_doc_arc &doca,
	       int        styleId,
	       int        colourId) ;

  bool drawSelectedPolypoint(int &err,
			     gmx_polypoint *gpolypoint) ;

  void drawPolypoint(int &err,
		     gmx_polypoint *gpolypoint) ;

  void fillArc(int              &err,
	       mx_doc_arc       &doca,
	       int              fillStyleId,
	       int              fillColourId,
	       int              hatchStyleId,
	       int              lineId,
	       int              lineColourId) ;

  void fillArc(int            &err,
	       gmx_arc        *arc) ;

  void fillRect(int            &err,
		gmx_rect       *rect) ;

  void fillSimpleRect(int                  &err,
		      const mx_doc_coord_t &topLeft,
		      const mx_doc_coord_t &bottomRight,
		      const mx_angle       &angle,
		      int                  fillStyleId,
		      int                  fillColourId) ;

  void fillComplexRect(int      &err,
		       gmx_rect *grect) ;

  void fillRect(int              &err,
		mx_doc_rect_t    &irect,
		int              fillStyleId,
		int              fillColourId,
		int              hatchStyleId);

  void hatchRect(int                  &err,
		 const mx_doc_coord_t &topLeft,
		 const mx_doc_coord_t &bottomRight,
		 const mx_angle       &angle,
		 int                  hatchStyleId) ;

  void fillPolypoint(int            &err,
		     gmx_polypoint *gpolypoint) ;

  void reset() ;

  void outputRectLines(int         &err,
		       mx_doc_rect_t &rr,
		       int         startPoint,
		       int         npoints,
		       bool        isClosed,
		       int         lineStyleId,
		       int         colourId) ;

  void fillPolypoint(int              &err,
		     mx_doc_polypoint &docpp,
		     int              fillStyleId,
		     int              fillColourId,
		     int              hatchStyleId,
		     int              lineId,
		     int              lineColourId) ;

  void drawSimpleRect(int                  &err,
		      const mx_doc_coord_t &topLeft,
		      const mx_doc_coord_t &bottomRight,
		      const mx_angle       &angle,
		      int                  styleId,
		      int                  colourId) ;

  void drawRect(int            &err,
		mx_doc_rect_t  &rr,
		int            borderId) ;


  void fillIds(int              &err,
	       mx_area_style    *areaStyle,
	       int              &borderId,
	       int              &fillStyleId,
	       int              &hatchStyleId,
	       int              &lineId,
	       int              &lineColourId) ;

  void drawSelectMark(int            &err,
		      const mx_point &where) ;

  void drawSelectSpan(int            &err,
		      const mx_rect  &span) ; 

  void drawSelectArc(int            &err,
		     const mx_arc   &arc) ; 
} ;












