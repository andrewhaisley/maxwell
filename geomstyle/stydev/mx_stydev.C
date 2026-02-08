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
/* MODULE : mx_stydev.C
 *
 * AUTHOR : David Miller
 *
 * This file 
 *
 * DESCRIPTION: 
 * Module mx_stydev.C 
 * 
 * 
 * 
 *
 * $id$
 * $log$
 *
 */

#include <mx_stydev.h>

mx_doc_polypoint **mx_style_device::docpps = NULL ;
int                mx_style_device::numPolypoints = 0 ;
int                mx_style_device::nextPolypoint = 0 ;
bool               mx_style_device::defaultStylesSet = FALSE ;
mx_all_styles      mx_style_device::defaultStyles ;

/*-------------------------------------------------
 * FUNCTION: mx_style_device::~mx_style_device
 *
 * DESCRIPTION: Delete a style device
 * 
 *
 */

mx_style_device::~mx_style_device()
{
  int i ;

  // get rid of the static data when a style device is
  // destroyed
  for(i=0;i<numPolypoints;i++) 
  {
    delete docpps[i] ;
  }
  delete [] docpps ;

  docpps = NULL ;
  numPolypoints = 0 ;
  nextPolypoint = 0 ;
}

/*-------------------------------------------------
 * FUNCTION: mx_style_device::setupDefaultStyles
 *
 * DESCRIPTION: 
 * 
 *
 */

void mx_style_device::setupDefaultStyles(int &err)
{
  // first create a solid fill for use with arrows 

  if(defaultStylesSet) return ;
    
  mx_gfill_style   *solidFillStyle  = new mx_gfill_style("solidFill");
  mx_gborder_style *borderFillStyle = new mx_gborder_style("solidFill");
  mx_garea_style   *areaFillStyle   = new mx_garea_style("solidFill");
  mx_gline_style   *lineXORStyle    = new mx_gline_style("xorLine") ;
  mx_colour        *whiteColour     = new mx_colour(0,0,0,"white") ;
  
  solidFillStyle->style.type   = mx_fill_colour ;
  lineXORStyle->style.cap_type = mx_cap_butt_not_last ;

  defaults.solidFillStyleId = defaultStyles.insert(err,solidFillStyle);
  MX_ERROR_CHECK(err) ;

  defaults.solidBorderStyleId = defaultStyles.insert(err,borderFillStyle);
  MX_ERROR_CHECK(err) ;

  defaults.solidAreaStyleId = defaultStyles.insert(err,areaFillStyle);
  MX_ERROR_CHECK(err) ;

  defaults.XORLineStyleId = defaultStyles.insert(err,lineXORStyle);
  MX_ERROR_CHECK(err) ;

  // add black as a colour for Xor-ing
  defaults.blackColourId  = defaultStyles.insertColour(err,whiteColour);
  MX_ERROR_CHECK(err) ;

  borderFillStyle->style.fill_style.fill_id   = defaults.solidFillStyleId ;
  areaFillStyle->style.border_style.border_id = defaults.solidBorderStyleId ;

  defaultStylesSet = TRUE ;
  
  return ;
abort:;
}

/*-------------------------------------------------
 * FUNCTION: mx_style_device::mx_style_device
 *
 * DESCRIPTION: 
 * 
 *
 */

mx_style_device::mx_style_device(mx_device     *idevice,
				 mx_all_styles *istyles) 
{

    setDevice(idevice) ;
    setStyles(istyles) ;
  
abort:;
  
}  

/*-------------------------------------------------
 * FUNCTION: mx_style_device::reset
 *
 * DESCRIPTION: 
 * 
 *
 */

void mx_style_device::reset() 
{
    currentColourId       = -1 ;
    currentColour         = NULL ;

    currentPointStyleId   = -1 ;
    currentPointStyle     = NULL;

    currentDashStyleId    = -1 ;
    currentDashStyle      = NULL;

    currentLineStyleId    = -1 ;
    currentLineStyle      = NULL;

    currentHatchStyleId   = -1 ;
    currentHatchStyle     = NULL;

    currentFillStyleId    = -1 ;
    currentFillStyle      = NULL;

    currentAreaStyleId    = -1 ;
    currentAreaStyle      = NULL;

    currentBorderStyleId  = -1 ;
    currentBorderStyle    = NULL;

    setUseDefaultStyles(FALSE) ;
    setUseDefaultColours(FALSE) ;
} 

/*-------------------------------------------------
 * FUNCTION: mx_style_device::draw
 *
 * DESCRIPTION: 
 * 
 *
 */

void mx_style_device::draw(int &err,mx_geom *geom)
{
    int32 oldStyleId,oldColourId;

    // in select mode set up the styles 
    if(geom->selectCount != MX_GEOM_SNAP_NONE) 
    {
      geom->getDetails(oldStyleId,oldColourId);

      geom->colourId = defaults.blackColourId ;

      if(geom->dimension() == 2) 
      {
	geom->styleId  = defaults.solidAreaStyleId ;
      }
      else
      {
	geom->styleId  =   defaults.XORLineStyleId ;
      }
    }

    // always return if style is undefined or has no
    // colour

    if( (geom->styleId == -1) || (geom->colourId == -1)) return ;

    switch(geom->type) 
    {
    case MX_GEOM_SPAN:
	drawSpan(err,(gmx_span *)geom) ;
	MX_ERROR_CHECK(err) ;
	break ;
    case MX_GEOM_RECT:
        if(geom->dimension() == 2) 
	{
	  fillRect(err,(gmx_rect *)geom) ;
	  MX_ERROR_CHECK(err) ;
	}
	else
	{
	  drawRect(err,(gmx_rect *)geom) ;
	  MX_ERROR_CHECK(err) ;
	}
	break ;
    case MX_GEOM_ARC:
        if(geom->dimension() == 2) 
	{
	    fillArc(err,(gmx_arc *)geom) ;
	    MX_ERROR_CHECK(err) ;
	}
	else
	{
	    drawArc(err,(gmx_arc *)geom) ;
	    MX_ERROR_CHECK(err) ;
	}
	
	break ;
    case MX_GEOM_POLYPOINT:
        if(geom->dimension() == 2) 
	{
	  fillPolypoint(err,(gmx_polypoint *)geom) ;
	  MX_ERROR_CHECK(err) ;
	}
	else
	{
	  drawPolypoint(err,(gmx_polypoint *)geom) ;
	  MX_ERROR_CHECK(err) ;
	}
	break ;
    default:
	break ;
    }
abort:
    if(geom->selectCount != MX_GEOM_SNAP_NONE) 
    {
      geom->setDetails(oldStyleId,oldColourId);
    }
    return ;
}

/*-------------------------------------------------
 * FUNCTION: mx_style_device::fillIds
 *
 * DESCRIPTION: 
 * 
 *
 */

void mx_style_device::fillIds(int              &err,
			      mx_area_style    *areaStyle,
			      int              &borderId,
			      int              &fillStyleId,
			      int              &hatchStyleId,
			      int              &lineId,
			      int              &lineColourId) 
{
    err = MX_ERROR_OK ;
    
    fillStyleId  = -1 ;
    hatchStyleId = -1 ;
    lineId       = -1 ;
    lineColourId = -1 ;
    borderId     = -1 ;

    if(areaStyle == NULL) return ;
    
    // get the border style
    borderId     = areaStyle->border_style.border_id ;
    hatchStyleId = areaStyle->hatch_style.hatch_id ;
    
    if(borderId == -1) return ;
    
    setupOutputBorderStyle(err,borderId) ;
    MX_ERROR_CHECK(err) ;

    fillStyleId  = currentBorderStyle->fill_style.fill_id ;
    lineId       = currentBorderStyle->top_style.line_id ;
    lineColourId = currentBorderStyle->top_style.colour_id ;

abort:
    return ;
}

/*-------------------------------------------------
 * FUNCTION: mx_style_device::changeStyleDevice
 *
 * DESCRIPTION: 
 * 
 *
 */

bool mx_style_device::changeStyleStyles(int           newStyleId,
					int           oldStyleId,
					bool          currentDict,
					mx_all_styles **useDict) 
{
  bool doChange = FALSE ;

  if(useDefaultStyles != currentDict) 
  {
    // different styles dictionaries 
    doChange = TRUE ;
  }
  else
  {
    doChange = (newStyleId != oldStyleId) ;
  }

  *useDict = useDefaultStyles ? &defaultStyles : styles ;

  return doChange ;
}

/*-------------------------------------------------
 * FUNCTION: mx_style_device::changeStyleDevice
 *
 * DESCRIPTION: 
 * 
 *
 */

bool mx_style_device::changeColourStyles(int           newStyleId,
					 int           oldStyleId,
					 bool          currentDict,
					 mx_all_styles **useDict) 
{
  bool doChange = FALSE ;

  if(useDefaultColours != currentDict) 
  {
    // different styles dictionaries 
    doChange = TRUE ;
  }
  else
  {
    doChange = (newStyleId != oldStyleId) ;
  }

  *useDict = useDefaultColours ? &defaultStyles : styles ;

  return doChange ;
}

/*-------------------------------------------------
 * FUNCTION: mx_style_device::setupOutputHatchStyle
 *
 * DESCRIPTION: 
 * 
 *
 */

bool mx_style_device::setupOutputHatchStyle(int &err,
					    int styleId) 
{
  mx_ghatch_style *gHatchStyle ;
  mx_all_styles   *styleDictionary ;

  bool isSame = TRUE ;

  err = MX_ERROR_OK ;

  if(changeStyleStyles(styleId,currentHatchStyleId,
		       currentHatchDict,&styleDictionary))
  {
    // need a new hatch style 
    gHatchStyle = (mx_ghatch_style *)styleDictionary->get(err,
							  mx_gstyle_hatch,
							  styleId) ;
    MX_ERROR_CHECK(err) ;    

    currentHatchStyle    = &(gHatchStyle->style) ;
    currentHatchDict     = useDefaultStyles ;
    currentHatchStyleId  = styleId ;
    isSame = FALSE ;
  }

abort:
  return isSame;
}

/*-------------------------------------------------
 * FUNCTION: mx_style_device::setupOutputAreaStyle
 *
 * DESCRIPTION: 
 * 
 *
 */

bool mx_style_device::setupOutputAreaStyle(int &err,
					   int styleId) 
{  
  mx_garea_style *gAreaStyle ;
  mx_all_styles   *styleDictionary ;

  bool isSame = TRUE ;

  err = MX_ERROR_OK ;

  if(changeStyleStyles(styleId,currentAreaStyleId,
		       currentAreaDict,&styleDictionary))
  {
    // need a new area style 
    gAreaStyle = (mx_garea_style *)styleDictionary->get(err,
							mx_gstyle_area,
							styleId) ;
    MX_ERROR_CHECK(err) ;    

    currentAreaStyle    = &(gAreaStyle->style) ;
    currentAreaDict     = useDefaultStyles ;
    currentAreaStyleId  = styleId ;
    isSame = FALSE ;
  }

abort:
  return isSame;
}

/*-------------------------------------------------
 * FUNCTION: mx_style_device::setupOutputBorderStyle
 *
 * DESCRIPTION: 
 * 
 *
 */

bool mx_style_device::setupOutputBorderStyle(int &err,
					   int styleId) 
{
  mx_gborder_style *gBorderStyle ;
  mx_all_styles   *styleDictionary ;

  bool isSame = TRUE ;

  err = MX_ERROR_OK ;

  if(changeStyleStyles(styleId,currentBorderStyleId,
		       currentBorderDict,&styleDictionary))
  {
    // need a new border style 
    gBorderStyle = (mx_gborder_style *)styleDictionary->get(err,
							    mx_gstyle_border,
							    styleId) ;
    MX_ERROR_CHECK(err) ;    

    currentBorderStyle    = &(gBorderStyle->style) ;
    currentBorderDict     = useDefaultStyles ;
    currentBorderStyleId  = styleId ;
    isSame = FALSE ;
  }

abort:
  return isSame;
}

/*-------------------------------------------------
 * FUNCTION: mx_style_device::setupOutputPointStyle
 *
 * DESCRIPTION: 
 * 
 *
 */

bool mx_style_device::setupOutputPointStyle(int &err,
					   int styleId) 
{
  mx_gpoint_style *gPointStyle ;
  mx_all_styles   *styleDictionary ;

  bool isSame = TRUE ;

  err = MX_ERROR_OK ;

  if(changeStyleStyles(styleId,currentPointStyleId,
		       currentPointDict,&styleDictionary))
  {
    // need a new point style 
    gPointStyle = (mx_gpoint_style *)styleDictionary->get(err,
							  mx_gstyle_point,
							  styleId) ;
    MX_ERROR_CHECK(err) ;    

    currentPointStyle    = &(gPointStyle->style) ;
    currentPointDict     = useDefaultStyles ;
    currentPointStyleId  = styleId ;
    isSame = FALSE ;
  }

abort:
  return isSame;
}

/*-------------------------------------------------
 * FUNCTION: mx_style_device::setupOutputColourStyle
 *
 * DESCRIPTION: 
 * 
 *
 */

bool mx_style_device::setupOutputColourStyle(int  &err,
					     int  styleId) 
{ 
  mx_all_styles   *styleDictionary ;

  bool isSame = TRUE ;

  if(changeColourStyles(styleId,currentColourId,
			currentColourDict,&styleDictionary))
  {
      currentColour = styleDictionary->getColour(err,styleId) ;
      MX_ERROR_CHECK(err) ;    
      
      outputLineStyle.colour   = *currentColour ;
      outputFillStyle.colour   = *currentColour ;

      currentColourId   = styleId ;
      currentColourDict = useDefaultColours ;
      isSame = FALSE ;
  }
abort:;
  return isSame ;
}

/*-------------------------------------------------
 * FUNCTION: mx_style_device::setupOutputDashStyle
 *
 * DESCRIPTION: 
 * 
 *
 */

bool mx_style_device::setupOutputDashStyle(int  &err,
					   int  styleId,
					   bool setOutputLine)
{ 
  mx_gdash_style *gDashStyle ;
  mx_all_styles   *styleDictionary ;

  bool isSame = TRUE ;

  if(changeStyleStyles(styleId,currentDashStyleId,
		       currentDashDict,&styleDictionary))
  {
    // need a new line style 
    if(styleId != -1) 
    {
      gDashStyle 
                = (mx_gdash_style *)
	            styleDictionary->get(err,
					 mx_gstyle_dash,
					 outputLineStyle.dash_style.get_dash_id()) ;
      MX_ERROR_CHECK(err) ;    

      currentDashStyle = &(gDashStyle->style) ;

      if(setOutputLine) outputLineStyle.dash_style = gDashStyle->style ;
    }
    else
    {
      currentDashStyle = NULL ;
      if(setOutputLine) outputLineStyle.dash_style.set(0,NULL,0,-1) ;
    }
    
    currentDashDict    = useDefaultStyles ;
    currentDashStyleId = styleId ;
    isSame = FALSE ;	
  }

abort:;
  return isSame ;
}

/*-------------------------------------------------
 * FUNCTION: mx_style_device::getLineStyle
 *
 * DESCRIPTION: Get a line style, but do not set to 
 *              the current line style
 *
 */

mx_line_style *mx_style_device::getLineStyle(int &err,
					     int styleId,
					     bool &isSame)
{
    mx_all_styles   *styleDictionary ;
    mx_gline_style *gLineStyle = NULL ;
    mx_line_style  *lineStyle ;

    isSame =  !changeStyleStyles(styleId,currentLineStyleId,
				 currentLineDict,&styleDictionary) ;
    
    if(!isSame)
    {
	// need a new line style 
      gLineStyle = (mx_gline_style *)styleDictionary->get(err,
							  mx_gstyle_line,
							  styleId) ;
      MX_ERROR_CHECK(err) ;    

      lineStyle = &(gLineStyle->style) ;
    }
    else
    {
      lineStyle = currentLineStyle ;
    }
    
abort:
    return lineStyle ;

}

/*-------------------------------------------------
 * FUNCTION: mx_style_device::getFillStyle
 *
 * DESCRIPTION: Get a line style, but do not set to 
 *              the current line style
 *
 */

mx_fill_style *mx_style_device::getFillStyle(int &err,
					     int styleId,
					     bool &isSame)
{
    mx_all_styles   *styleDictionary ;
    mx_gfill_style *gFillStyle = NULL ;
    mx_fill_style  *fillStyle ;

    isSame =  !changeStyleStyles(styleId,currentFillStyleId,
				 currentFillDict,&styleDictionary) ;
    
    if(!isSame)
    {
	// need a new fill style 
      gFillStyle = (mx_gfill_style *)styleDictionary->get(err,
							  mx_gstyle_fill,
							  styleId) ;
      MX_ERROR_CHECK(err) ;    

      fillStyle = &(gFillStyle->style) ;
    }
    else
    {
      fillStyle = currentFillStyle ;
    }
    
abort:
    return fillStyle ;

}

/*-------------------------------------------------
 * FUNCTION: mx_style_device::setupOutputLineStyle
 *
 * DESCRIPTION: 
 * 
 *
 */

bool mx_style_device::setupOutputLineStyle(int &err,
					   int styleId,
					   int colourId) 
{
    mx_line_style *lineStyle ;
    int newDashId ;
    bool isSame2 ;
    bool isSame ;
    
    lineStyle = getLineStyle(err,styleId,isSame) ;
    MX_ERROR_CHECK(err) ;

    if(!isSame) 
    {
      outputLineStyle        = *lineStyle ;
      if(currentColour != NULL) outputLineStyle.colour = *currentColour ;
      currentLineStyle       = lineStyle ;
      currentLineDict        = useDefaultStyles ;
      currentLineStyleId     = styleId ;
    }
    else
    {
	// outputLineStyle may have been set to mx_use_lstyle ;
	outputLineStyle.line_type = lineStyle->line_type ;
    }
    
    isSame2 = setupOutputColourStyle(err,colourId) ;
    MX_ERROR_CHECK(err) ;

    if(isSame) isSame = isSame2 ;
    
    
    /* may need to refresh the dash style of the 
       line style */

    newDashId = outputLineStyle.dash_style.get_dash_id() ;

    isSame2 = setupOutputDashStyle(err,newDashId,TRUE) ;
    MX_ERROR_CHECK(err) ;

    if(isSame) isSame = isSame2 ;

    // if the same style no need to reset the device
    if(isSame) outputLineStyle.line_type = mx_use_lstyle ;
abort:	
    return isSame ;
}

/*-------------------------------------------------
 * FUNCTION: mx_style_device::setupOutputFillStyle
 *
 * DESCRIPTION: 
 * 
 *
 */

bool mx_style_device::setupOutputFillStyle(int &err,
					   int styleId,
					   int colourId) 
{
  mx_fill_style   *fillStyle ;
  mx_all_styles   *styleDictionary ;
  bool isSame2 ;
  bool isSame ;
    
  fillStyle = getFillStyle(err,styleId,isSame) ;
  MX_ERROR_CHECK(err) ;

  if(!isSame) 
  {
    outputFillStyle        = *fillStyle ;
    if(currentColour != NULL) outputFillStyle.colour = *currentColour ;
    currentFillStyle       = fillStyle ;
    currentFillDict        = useDefaultStyles ;
    currentFillStyleId     = styleId ;
  }
  else
  {
      // outputFillStyle may have been set to mx_use_fstyle ;
      outputFillStyle.type = fillStyle->type ;
  } 
   
  isSame2 = setupOutputColourStyle(err,colourId) ;
  MX_ERROR_CHECK(err) ;

  if(isSame) isSame = isSame2 ;

  // if the same style no need to reset the device
  if(isSame) outputFillStyle.type = mx_use_fstyle ;
abort:	
    return isSame;
}

/*-------------------------------------------------
 * FUNCTION: mx_style_device::drawArrow
 *
 * DESCRIPTION: 
 * 
 *
 */

void mx_style_device::drawArrow(int                     &err,
				mx_doc_coord_t          &head,
				mx_point                &span,
				mx_point                &uspan,
				mx_cap_type_t           capType,
				mx_arrow_pattern_type_t arrowPattern,
				float                   lineWidth,
				int                     colourId,
				double                  &lineBack) 
{
  static gmx_polypoint gpolypoint ;
  mx_point             testPoint ;
  mx_point             addOn,addOn1 ;
  bool                 oldDefVal ;

  err = MX_ERROR_OK ;
 
  // use the deafault style dictionary - but the
  // user style colours
  oldDefVal = setUseDefaultStyles(TRUE) ;

  setupDefaultStyles(err);
  MX_ERROR_CHECK(err);

  gpolypoint.pl.clear(6) ;
  gpolypoint.styleId = defaults.solidAreaStyleId ;
  gpolypoint.colourId = colourId ;

  lineBack = 0 ;
  
  switch(arrowPattern) 
  {
  case mx_arrow_triangle:

    gpolypoint.pl.addPoint(head.p) ;

    addOn = span+uspan ;
    addOn = head.p + 3.0*lineWidth*addOn ;
    gpolypoint.pl.addPoint(addOn) ;

    addOn = span-uspan ;
    addOn = head.p + 3.0*lineWidth*addOn ;
    gpolypoint.pl.addPoint(addOn) ;

    fillPolypoint(err,&gpolypoint);
    MX_ERROR_CHECK(err) ;

    lineBack = 3.0*lineWidth ;
    
    break ;
  case mx_arrow_lines:

    gpolypoint.pl.addPoint(head.p) ;

    addOn = span+uspan ;
    addOn = head.p + 3.0*lineWidth*addOn ;
    gpolypoint.pl.addPoint(addOn) ;

    addOn = addOn + (lineWidth*span) ;
    gpolypoint.pl.addPoint(addOn) ;

    addOn = head.p + (lineWidth*span) ;
    gpolypoint.pl.addPoint(addOn) ;

    addOn  = (span-uspan) ;
    addOn1 = head.p + 3.0*lineWidth*addOn ;

    addOn = addOn1 + lineWidth*span ;

    gpolypoint.pl.addPoint(addOn) ;
    gpolypoint.pl.addPoint(addOn1) ;

    /*
    points[1].p = head + 3.0*lineWidth*(span+uspan);
    points[2].p = points[1].p + (lineWidth*span) ;
    points[3].p = head + (lineWidth*span) ;
    points[5].p = 
    points[4].p = points[5].p + lineWidth*span;
    */

    fillPolypoint(err,&gpolypoint);
    MX_ERROR_CHECK(err) ;

    lineBack = lineWidth ;
    
    break ;
  default:
    break;
  }

  if(capType == mx_cap_projecting) lineBack -= 0.5*lineWidth ;

 abort:
  // revert to the user style 
  setUseDefaultStyles(oldDefVal) ;
  return ;
}

/*-------------------------------------------------
 * FUNCTION: mx_style_device::drawSelectMark
 *
 * DESCRIPTION: 
 * 
 *
 */

void mx_style_device::drawSelectMark(int            &err,
				     const mx_point &where) 
{
  err = MX_ERROR_OK ;

  // draw the mark as 6 pixels

  double size = 3.0/device->getScreenResolution() ;

  mx_doc_coord_t  topLeft(where.x-size,where.y-size);
  mx_doc_coord_t  bottomRight(where.x+size,where.y+size);

  printf("drawing select mark %f %f\n",
	 where.x,
	 where.y) ;

  fillSimpleRect(err,
		 topLeft,
		 bottomRight,
		 defaultAngle,
		 defaults.solidFillStyleId,
		 defaults.blackColourId) ;
  MX_ERROR_CHECK(err) ;
abort:;
}

/*-------------------------------------------------
 * FUNCTION: mx_style_device::drawSelectSpan
 *
 * DESCRIPTION: 
 * 
 *
 */

void mx_style_device::drawSelectSpan(int            &err,
				     const mx_rect  &span) 
{
  err = MX_ERROR_OK ;

  gmx_span s(span) ;

  printf("drawing select span %f %f %f %f\n",
	 span.xb,
	 span.yb,
	 span.xt,
	 span.yt);

  s.colourId    = defaults.blackColourId ;
  s.styleId     = defaults.solidAreaStyleId ;
  s.selectCount = MX_SPAN_SNAP_NONE ;

  drawSelectMark(err,s.s.start());
  MX_ERROR_CHECK(err) ;

  drawSelectMark(err,s.s.end());
  MX_ERROR_CHECK(err) ;

  drawSpan(err,&s) ;
  MX_ERROR_CHECK(err) ;
abort:;
}

/*-------------------------------------------------
 * FUNCTION: mx_style_device::drawSelectArc
 *
 * DESCRIPTION: 
 * 
 *
 */

void mx_style_device::drawSelectArc(int           &err,
				    const mx_arc  &arc) 
{
  err = MX_ERROR_OK ;

  gmx_arc a(arc) ;

  a.colourId    = defaults.blackColourId ;
  a.styleId     = defaults.solidAreaStyleId ;
  a.selectCount = MX_SPAN_SNAP_NONE ;
  a.a.arcType = ARC_ARC ;
  a.a.isArea  = FALSE ;

  drawSelectMark(err,a.a.gsap());
  MX_ERROR_CHECK(err) ;

  drawSelectMark(err,a.a.geap());
  MX_ERROR_CHECK(err) ;

  drawArc(err,&a) ;
  MX_ERROR_CHECK(err) ;
abort:;
}



