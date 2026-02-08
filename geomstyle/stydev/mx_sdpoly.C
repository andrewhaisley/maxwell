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
/* MODULE : mx_sdpoly.C
 *
 * AUTHOR : David Miller
 *
 * This file 
 *
 * DESCRIPTION: 
 * Module mx_sdpoly.C 
 *
 *
 * Revision 1.1  1996/10/06 13:15:45  andrew
 * Initial revision
 *
 *
 */

#include "mx_stydev.h"

/*-------------------------------------------------
 * Function: mx_style_device::pushPolypoint
 *
 * DESCRIPTION: Push a stack of static polypoints
 * 
 *
 */

mx_doc_polypoint *mx_style_device::pushPolypoint() 
{
  mx_doc_polypoint **oldpps ;
  mx_doc_polypoint *retpp ;

  if(nextPolypoint==numPolypoints) 
  {
    // need a new polypoint

    oldpps = docpps ;
    numPolypoints++ ;

    docpps = new (mx_doc_polypoint *)[numPolypoints] ;
    
    // restore old values 
    memcpy(docpps,oldpps,nextPolypoint*sizeof(mx_doc_polypoint *)) ;

    delete [] oldpps ;

    retpp = new mx_doc_polypoint ;
    docpps[nextPolypoint] = retpp ;
  }
  else
  {
    retpp = *(docpps+nextPolypoint);
  }

  nextPolypoint++ ;

  return retpp ;
}

/*-------------------------------------------------
 * FUNCTION: mx_style_device::fillPolypoint
 *
 * DESCRIPTION: 
 * 
 *
 */

void mx_style_device::fillPolypoint(int              &err,
				    mx_doc_polypoint &docpp,
				    int              fillStyleId,
				    int              fillColourId,
				    int              hatchStyleId,
				    int              lineId,
				    int              lineColourId) 
{
  err = MX_ERROR_OK ;

  // use the colour on the geometry
  if( (fillStyleId != -1) && (fillColourId != -1)) 
  {
    setupOutputFillStyle(err,
			 fillStyleId,
			 fillColourId) ;
    MX_ERROR_CHECK(err) ;

    // fill the polypoint
    device->fillPolypoint(err,docpp,&outputFillStyle) ;
    MX_ERROR_CHECK(err) ;
  }
  
  hatchPolypoint(err,
		 docpp,
		 hatchStyleId) ;
  MX_ERROR_CHECK(err) ;

  drawPolypoint(err,
		docpp,
		lineId,
		lineColourId) ;
  MX_ERROR_CHECK(err) ;

abort:;
  return ;
}
/*-------------------------------------------------
 * FUNCTION: mx_style_device::fillPolypoint
 *
 * DESCRIPTION: 
 * 
 *
 */

void mx_style_device::fillPolypoint(int &err,
				    gmx_polypoint *gpolypoint)
{
  int lineStyleId ;
  int lineColourId ;
  int fillId ;
  int hatchId ;
  int borderId ;
  
  mx_doc_polypoint *docpp = pushPolypoint() ;
  
  err = MX_ERROR_OK ;

  if(drawSelectedPolypoint(err,gpolypoint)) return ;
  MX_ERROR_CHECK(err);

  // the style on the geometry is the area style -
  // this holds a border style and an
  // hatch style in addition to the proper fill style 

  setupOutputAreaStyle(err,gpolypoint->styleId) ;
  MX_ERROR_CHECK(err) ;    

  // create the polypoint
  docpp->pp.copy(gpolypoint->pl) ;

  fillIds(err,
	  currentAreaStyle,
	  borderId,
	  fillId,
	  hatchId,
	  lineStyleId,
	  lineColourId) ;
  MX_ERROR_CHECK(err) ;

  fillPolypoint(err,
		*docpp,
		fillId,
		gpolypoint->colourId,
		hatchId,
		lineStyleId,
		lineColourId) ;
  MX_ERROR_CHECK(err) ;

abort:;
  popPolypoint();
  return ;
}

/*-------------------------------------------------
 * FUNCTION: mx_style_device::drawPolypoint
 *
 * DESCRIPTION: 
 * 
 *
 */

void mx_style_device::drawPolypoint(int &err,
				    gmx_polypoint *gpolypoint)
{
  // the span has a line style 
  mx_doc_polypoint *docpp = pushPolypoint() ;

  err = MX_ERROR_OK ;

  if(drawSelectedPolypoint(err,gpolypoint)) return ;
  MX_ERROR_CHECK(err);

  docpp->pp.copy(gpolypoint->pl) ;

  drawPolypoint(err,
		*docpp,
		gpolypoint->styleId,
		gpolypoint->colourId) ;
  MX_ERROR_CHECK(err) ;
    
abort:
  popPolypoint() ;
  return ;
}

/*-------------------------------------------------
 * FUNCTION: mx_style_device::drawPolypoint
 *
 * DESCRIPTION: Draw the static polypoint
 * 
 *
 */

void mx_style_device::drawPolypoint(int               &err,
				    mx_doc_polypoint  &idocpp,
				    int               lineStyleId,
				    int               lineColourId) 
{
  bool   startArrow ;
  bool   endArrow ;
  double lineBack1 = 0 ;
  double lineBack2 = 0 ;
  int    numPoints ;
  mx_line_style *thisLineStyle ;
  bool    isSame ;

  err = MX_ERROR_OK ;

  if( (lineStyleId == -1) || (lineColourId == -1)) return ;

  thisLineStyle = getLineStyle(err,lineStyleId,isSame) ;
  MX_ERROR_CHECK(err);

  numPoints = idocpp.pp.get_num_points() ;

  if((!idocpp.pp.get_closed()) && 
     (numPoints > 1) && (thisLineStyle->getArrows(startArrow,endArrow)))
  {
    double lineBack ;

    if(startArrow) 
    {
      mx_rect  rp(idocpp.pp[0],idocpp.pp[1],TRUE) ;
      mx_point sp = rp.uspan();
      mx_point usp = sp.flip() ;
      mx_doc_coord_t tip(idocpp.pp[0]) ;

      drawArrow(err,tip,sp,usp,
		thisLineStyle->cap_type,
		thisLineStyle->arrow_pattern,
		thisLineStyle->width,
		lineColourId,
		lineBack1) ;
      MX_ERROR_CHECK(err);

      lineBack1 = backLines(idocpp.pp,lineBack1,TRUE,
			    sp,usp) ;
    }

    if(endArrow) 
    {
      mx_rect rp(idocpp.pp[numPoints-2],idocpp.pp[numPoints-1],TRUE) ;
      mx_point sp = rp.uspan();
      mx_point usp = sp.flip() ;
      mx_point msp = -sp ;
      mx_doc_coord_t tip(idocpp.pp[numPoints-1]) ;

      drawArrow(err,tip,msp,usp,
		thisLineStyle->cap_type,
		thisLineStyle->arrow_pattern,
		thisLineStyle->width,
		lineColourId,
		lineBack2) ;
      MX_ERROR_CHECK(err);

      lineBack2 = backLines(idocpp.pp,lineBack2,FALSE,
			    msp,usp) ;
    }
  }

  // set the masks and draw the polypoint

  idocpp.pp.setStartMask(lineBack1) ;
  idocpp.pp.setEndMask(lineBack2) ;

  // the span has a line style 
  setupOutputLineStyle(err,
		       lineStyleId,
		       lineColourId) ;
  MX_ERROR_CHECK(err) ;

  switch(idocpp.pp.get_type())
  {
  case POLY_LINE:
      device->drawPolypoint(err,idocpp,&outputLineStyle) ;
      MX_ERROR_CHECK(err) ;
      break ;
  case POLY_SEGMENT:
      device->drawSegments(err,idocpp,FALSE,&outputLineStyle) ;
      MX_ERROR_CHECK(err) ;
      break ;
  case POLY_CSEGMENT:
      device->drawSegments(err,idocpp,TRUE,&outputLineStyle) ;
      MX_ERROR_CHECK(err) ;
      break ;
  default:
      break ;
  }
    
abort:
    return ;
}

/*-------------------------------------------------
 * FUNCTION: mx_style_device::hatchPolypoint
 *
 * DESCRIPTION: Hatch the current polypoint
 * 
 *
 */

void mx_style_device::hatchPolypoint(int &err,
				     mx_doc_polypoint  &idocpp,
				     int hatchStyleId) 
{
  err = MX_ERROR_OK ;

  // the span has a line style 

  if(hatchStyleId == -1) return ;
  
  mx_rect hatchbox = idocpp.pp.bounding_box() ;
  
  setupOutputHatchStyle(err,
			hatchStyleId) ;
  MX_ERROR_CHECK(err) ;
  
  // hatching is required 
  // set clip mask 
  device->pushClipPolypoint(err,idocpp,TRUE) ;
  MX_ERROR_CHECK(err) ;
  
  hatchBox(err,hatchbox);
  MX_ERROR_CHECK(err) ;
  
  device->popClipMask(err);
  MX_ERROR_CHECK(err) ;
  
  // to be safe reset the style device here 
  reset() ;
  
abort:
    return ;
}

/*-------------------------------------------------
 * FUNCTION: mx_style_device::backLines
 *
 * DESCRIPTION: 
 * 
 *
 */

double mx_style_device::backLines(mx_polypoint   &pline,
				  double         lineBack,
				  bool           doingStart,
				  mx_point       &sp,
				  mx_point       &usp) 
{
  int thisPoint,nextPoint ;

  // backtrack until we intersect half way down the
  // arrow 
  mx_point arrowEdge1 = 0.5*usp + sp ;
  mx_point arrowEdge2 = -0.5*usp + sp ;

  double  backPoints = 0 ;
  double divFactor,factor;
  mx_point span,start,ps ;
  int      npoints = pline.get_num_points() ;
  mx_point backPoint ;
  bool     firstPoint = TRUE ;
  double   lowTest  = -GEOM_SMALL ;
  double   highTest = 1 + GEOM_SMALL ;
  
  if(doingStart) 
  {
    ps = pline[0] ;
  }
  else
  {
    ps = pline[npoints-1] ;
  }

  backPoint = ps + 0.8*lineBack*sp ;

  // get lines perpendicular to arrow edges
  arrowEdge1 = arrowEdge1.flip() ;
  arrowEdge2 = arrowEdge2.flip() ;
  
  nextPoint  = doingStart ? 0 : npoints-1 ; 
  
  do
  {
    // end of line 
    thisPoint = nextPoint ;
    
    if(doingStart)
    {
      if(thisPoint == (npoints-1)) return (npoints-1);
      nextPoint = thisPoint + 1 ;
    }
    else
    {
      if(thisPoint == 0) return (npoints-1) ;
      nextPoint = thisPoint - 1 ;
    }
    
    span = pline[nextPoint] - pline[thisPoint] ;

    // does the line intersect (or come near to) 
    // the back line
    
    divFactor =span*sp ;

    if(divFactor != 0) 
    {
      start = backPoint - pline[thisPoint] ;

      factor = (start*sp)/divFactor ;
      
      if( (factor >= lowTest)  && (factor <= highTest) ) 
      {
	// intersection with back point

	  if(factor < 0) 
	  {
	      factor = 0;
	  }
	  else if(factor > 1) 
	  {
	      factor =  1 ;
	  }
	  return (backPoints + factor) ;
      }
    }
    
    // does the span intersect with the first arrow edge

    divFactor =span*arrowEdge1 ;

    if(divFactor != 0) 
    {
      start = ps - pline[thisPoint] ;

      factor = (start*arrowEdge1)/divFactor ;

      // we expect zero as a solotion for the first span

      if( (factor >= lowTest && (!firstPoint))  && (factor <= highTest) ) 
      {
	  // intersection with back point
	  if(factor < 0) 
	  {
	      factor = 0;
	  }
	  else if(factor > 1) 
	  {
	      factor =  1 ;
	  }
	  return (backPoints + factor) ;
      } 
    }
    
    // does the span intersect with the first arrow edge

    divFactor =span*arrowEdge2 ;

    if(divFactor != 0) 
    {
      start = ps - pline[thisPoint] ;

      factor = (start*arrowEdge2)/divFactor ;
    
      if( (factor >= lowTest && (!firstPoint))  && (factor <= highTest) ) 
      {
	// intersection with back point
		  // intersection with back point
        if(factor < 0) 
	{
	    factor = 0;
	}
	else if(factor > 1) 
	{
	    factor =  1 ;
	}
	return (backPoints + factor) ;
      } 
    }
    
    backPoints++ ;
    firstPoint = FALSE ;

  } while(1) ;
  
  
}

/*-------------------------------------------------
 * FUNCTION: mx_style_device::drawSelectedPolypoint
 *
 * DESCRIPTION: 
 * 
 *
 */

bool mx_style_device::drawSelectedPolypoint(int &err,
					    gmx_polypoint *gpolypoint)
{
  int noComponentsCount,wholeCount ;
  int numPoints = gpolypoint->pl.get_num_points();

  gpolypoint->pl.components(noComponentsCount,wholeCount);

  err = MX_ERROR_OK ;

  if(gpolypoint->selectCount == MX_GEOM_SNAP_NONE) return FALSE ;

  // test if it is the whole count - if it is draw 
  // end points of polypoint

  if(gpolypoint->selectCount < 0)
  {
    if(numPoints > 0) 
    {
      drawSelectMark(err,gpolypoint->pl[0]) ;
      MX_ERROR_CHECK(err);
    }

    if(numPoints > 1) 
    {
      drawSelectMark(err,gpolypoint->pl[numPoints-1]) ;
      MX_ERROR_CHECK(err);
    }
    return ;
  }

  if(gpolypoint->selectCount <= numPoints)
  {
    // its a point
    
    drawSelectMark(err,gpolypoint->pl[gpolypoint->selectCount+1]) ;
    MX_ERROR_CHECK(err);

    return TRUE ;
  }

  if(gpolypoint->selectCount < noComponentsCount) 
  {
    // its a span

    int startSpan = gpolypoint->selectCount - numPoints + 1 ;
    int endSpan   = startSpan + 1 ;
    
    // it is closed
    if(endSpan == noComponentsCount) endSpan = 0; 

    mx_rect span(gpolypoint->pl[startSpan],
		 gpolypoint->pl[endSpan],TRUE) ;
    
    drawSelectSpan(err,span) ;
    MX_ERROR_CHECK(err);
    
    return TRUE ;	    
  }
    
  return FALSE ;

abort:;
  return FALSE ;
}

