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
 * MODULE : mx_sdarc.C
 *
 * AUTHOR : David Miller
 *
 * This file 
 *
 * DESCRIPTION: 
 * Module mx_sdarc.C 
 *
 *
 * Revision 1.1  1996/10/06 13:15:45  andrew
 * Initial revision
 *
 *
 */

#include <mx_stydev.h>


/*-------------------------------------------------
 * FUNCTION: mx_style_device::drawArc
 *
 * DESCRIPTION: 
 * 
 *
 */

void mx_style_device::drawArc(int        &err,
			      mx_doc_arc &doca,
			      int        styleId,
			      int        colourId) 
{
  bool startArrow ;
  bool endArrow ;
  double angleBack1 = 0 ;
  double angleBack2 = 0 ;
  bool   doDraw = TRUE ;
  mx_line_style *thisLineStyle ;
  bool isSame ;

  err = MX_ERROR_OK ;
  
  if( (styleId == -1) || (colourId == -1)) return ;
  
  // the span has a line style 
  // get the line style - but do not set it yet 
  thisLineStyle = getLineStyle(err,styleId,isSame) ;
  MX_ERROR_CHECK(err);

  if( (doca.arc.arcType == ARC_ARC) && 
      (thisLineStyle->getArrows(startArrow,endArrow)))
  {
    double lineBack ;

    if(startArrow) 
    {
      mx_point ps = doca.arc.ac(doca.arc.gsa()) ;
      mx_doc_coord_t pps(ps) ;

      mx_point sp  = doca.arc.tangent(doca.arc.gsa()) ;
      mx_point usp = sp.flip() ;
        
      usp = -usp ;
      
      drawArrow(err,pps,sp,usp,
		thisLineStyle->cap_type,
		thisLineStyle->arrow_pattern,
		thisLineStyle->width,
		colourId,
		lineBack) ;
      MX_ERROR_CHECK(err);

      angleBack1 = backArc(doca.arc,lineBack,ps,sp,usp) ;

      if(angleBack1 < doca.arc.gsa()) angleBack1 += PITWO ;
      angleBack1 -= doca.arc.gsa() ;
      
    }

    if(endArrow) 
    {
      mx_point ps = doca.arc.ac(doca.arc.gea()) ;
      mx_doc_coord_t pps(ps) ;

      mx_point sp = -doca.arc.tangent(doca.arc.gea()) ;
      mx_point usp = sp.flip() ;

      drawArrow(err,pps,sp,usp,
		thisLineStyle->cap_type,
		thisLineStyle->arrow_pattern,
		thisLineStyle->width,
		colourId,
		lineBack) ;
      MX_ERROR_CHECK(err);

      angleBack2 = backArc(doca.arc,lineBack,ps,sp,usp) ;

      if(angleBack2 > doca.arc.gea()) angleBack2 -= PITWO ;
      angleBack2 = doca.arc.gea() - angleBack2 ;
    }
  }

  if(doDraw) 
  {
    setupOutputLineStyle(err,
			 styleId,
			 colourId) ;
    MX_ERROR_CHECK(err) ;

    angleBack1 = doca.arc.gsa() + angleBack1 ;
    doca.arc.ssa(angleBack1) ;

    angleBack2 = doca.arc.gea() - angleBack2 ;
    doca.arc.sea(angleBack2) ;

    switch(doca.arc.arcType) 
    {
    case ARC_ARC:
      device->drawArc(err,doca,&outputLineStyle) ;
      break ;
    case ARC_CHORD:
      device->drawChord(err,doca,&outputLineStyle) ;
      break ;
    case ARC_SECTOR:
      device->drawSector(err,doca,&outputLineStyle) ;
      break ;
    default:
      break ;
    }
    MX_ERROR_CHECK(err) ;
  }
  
  return ;
abort:
  return ;
}

/*-------------------------------------------------
 * FUNCTION: mx_style_device::fillArc
 *
 * DESCRIPTION: 
 * 
 *
 */

void mx_style_device::fillArc(int              &err,
			      mx_doc_arc       &doca,
			      int              fillStyleId,
			      int              fillColourId,
			      int              hatchStyleId,
			      int              lineId,
			      int              lineColourId) 
{
  mx_doc_polypoint *docpp = pushPolypoint() ;

  err = MX_ERROR_OK ;

  // use the colour on the geometry
  if( (fillStyleId != -1) && (fillColourId != -1)) 
  {
    setupOutputFillStyle(err,
			 fillStyleId,
			 fillColourId) ;
    MX_ERROR_CHECK(err) ;

    switch(doca.arc.arcType) 
    {
    case ARC_ARC:
    case ARC_CHORD:
      device->fillArc(err,doca,&outputFillStyle) ;
      break ;
    case ARC_SECTOR:
      device->fillSector(err,doca,&outputFillStyle) ;
      break ;
    default:
      break ;
    }
    MX_ERROR_CHECK(err) ;
  }

  hatchArc(err,doca,hatchStyleId);
  MX_ERROR_CHECK(err);

  drawArc(err,
	  doca,
	  lineId,
	  lineColourId) ;
  MX_ERROR_CHECK(err) ;

abort:
  popPolypoint();
}
/*-------------------------------------------------
 * FUNCTION: mx_style_device::fillArc
 *
 * DESCRIPTION: 
 * 
 *
 */

void mx_style_device::fillArc(int &err,
			      gmx_arc *garc)
{
  int lineStyleId ;
  int lineColourId ;
  int fillId ;
  int hatchId ;
  int borderId ;

  mx_doc_arc doca ;

  doca.arc = garc->a ;

  // the style on the geometry is the area style -
  // this holds a border style and an
  // hatch style in addition to the proper fill style 

  setupOutputAreaStyle(err,garc->styleId) ;
  MX_ERROR_CHECK(err) ;    

  fillIds(err,
	  currentAreaStyle,
	  borderId,
	  fillId,
	  hatchId,
	  lineStyleId,
	  lineColourId) ;
  MX_ERROR_CHECK(err) ;

  fillArc(err,
	  doca,
	  fillId,
	  garc->colourId,
	  hatchId,
	  lineStyleId,
	  lineColourId) ;
  MX_ERROR_CHECK(err) ;

abort:;
}

/*-------------------------------------------------
 * FUNCTION: mx_style_device::drawArc
 *
 * DESCRIPTION: 
 * 
 *
 */

void mx_style_device::drawArc(int &err,
			      gmx_arc *garc)
{
  mx_doc_arc doca ;

  doca.arc = garc->a ;

  drawArc(err,
	  doca,
	  garc->styleId,
	  garc->colourId) ;
  MX_ERROR_CHECK(err);
  
  return ;
abort:
  return ;
}

/*-------------------------------------------------
 * FUNCTION: mx_style_device::backArc
 *
 * DESCRIPTION: 
 * 
 *
 */

double mx_style_device::backArc(mx_arc    &a,
				double    lineBack,
				mx_point  &ps,
				mx_point  &sp,
				mx_point  &usp) 
{
  double par1,par2,par3,par4,par5,par6,par7 ;
  double root,deter ;
  bool foundEndRoot ;
  mx_point backPoint ;
  double angle ;
  mx_point s1 = a.gs() ;
  mx_point c  = a.gc() ;

  // here ps is the head point,sp points back along
  // the arc and usp is pointing out from the arc

  // we are looking for the point where
  //
  // backPoint + k*usp lies on the arc
  // 

  backPoint = ps + 0.8*lineBack*sp - c ;

  par1 = usp.x/s1.x ;
  par2 = backPoint.x/s1.x ;
  par3 = usp.y/s1.y ;
  par4 = backPoint.y/s1.y ;
  
  par5 = par1*par1 + par3*par3 ;
  par6 = (par1*par2 + par3*par4)/par5 ;
  par7 = (par2*par2 + par4*par4 - 1)/par5 ;

  deter = par6*par6 - par7 ;
  
  foundEndRoot = FALSE ;
      
  if(deter >= 0) 
  {
    // if there is a root there are 2 negative
    // ones - choose the biggest
    root = -par6 + sqrt(deter)  ;
    
    if(root >= -lineBack) 
    {
      // intersects back line of arrow 
      
      foundEndRoot = TRUE ;
      
      backPoint += root*usp ;
	  
      angle = atan2(backPoint.y*a.gs().x,backPoint.x*a.gs().y) ;
    }
  }
  
  if(!foundEndRoot) 
  {
    mx_point arrowEdge = -0.5*usp + sp ;
    mx_point c = a.gc() ;  
    // must have gone through the side of the arrow 
    
    backPoint = ps - c ;
    
    par1 = arrowEdge.x/s1.x ;
    par2 = backPoint.x/s1.x ;
    par3 = arrowEdge.y/s1.y ;
    par4 = backPoint.y/s1.y ;
    
    par5 = par1*par1 + par3*par3 ;
    par6 = (par1*par2 + par3*par4)/par5 ;
    
    // zero is always a solution 
    root = -2*par6 ;
    
    backPoint += root*arrowEdge ;
    
    angle = atan2(backPoint.y*a.gs().x,backPoint.x*a.gs().y) ;
  }
  
  if(angle < 0) angle += PITWO ;    

  return angle ;
}

/*-------------------------------------------------
 * FUNCTION: mx_style_device::hatchArc
 *
 * DESCRIPTION: Hatch the current polypoint
 * 
 *
 */

void mx_style_device::hatchArc(int         &err,
			       mx_doc_arc  &doca,
			       int         hatchStyleId) 
{
  err = MX_ERROR_OK ;

  // the span has a line style 

  if(hatchStyleId == -1) return ;
  
  mx_rect hatchbox = doca.arc.box() ;
  
  setupOutputHatchStyle(err,
			hatchStyleId) ;
  MX_ERROR_CHECK(err) ;
  
  // hatching is required 
  // set clip mask 

  switch(doca.arc.arcType) 
  {
  case ARC_ARC:
  case ARC_CHORD:
      device->pushClipArc(err,doca,TRUE) ;
      break ;
  case ARC_SECTOR:
      device->pushClipSector(err,doca,TRUE) ;
      break ;
    default:
      break ;
  }  
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
 * FUNCTION: mx_style_device::drawSelectedArc
 *
 * DESCRIPTION: Hatch the current polypoint
 * 
 *
 */

bool mx_style_device::drawSelectedArc(int &err,
				      gmx_arc *garc)
{
  bool drawStart  = FALSE ;
  bool drawEnd    = FALSE ;
  bool drawCentre = FALSE ;
  bool drawArc    = FALSE ;
  bool drawChord  = FALSE ;
  bool drawSpan1  = FALSE ;
  bool drawSpan2  = FALSE ;

  err = MX_ERROR_OK ;

  switch(garc->a.arcType)
  {
  case ARC_ARC:
    switch(garc->selectCount) 
    {
    case MX_ARC_SNAP_NONE:
      return FALSE ;
      break ;
    case MX_ARC_SNAP_START:
      drawStart = TRUE ;
      break ;
    case MX_ARC_SNAP_END:
      drawEnd = TRUE ;
      break ;
    case MX_ARC_SNAP_CENTRE:
      drawCentre = TRUE ;
      break ;
    case MX_ARC_SNAP_ARC:
      drawArc = TRUE ;
      break ;
    case MX_ARC_SNAP_WHOLE:
    default:
      drawStart = TRUE ;
      drawEnd   = TRUE ;
      drawCentre = TRUE ;
      break ;
    }
    break ;
  case ARC_CHORD:
    switch(garc->selectCount) 
    {
    case MX_CHORD_SNAP_NONE:
      return FALSE ;
      break ;
    case MX_CHORD_SNAP_START:
      drawStart = TRUE ;
      break ;
    case MX_CHORD_SNAP_END:
      drawEnd = TRUE ;
      break ;
    case MX_CHORD_SNAP_CENTRE:
      drawCentre = TRUE ;
      break ;
    case MX_CHORD_SNAP_ARC:
      drawArc = TRUE ;
      break ;
    case MX_CHORD_SNAP_CHORD:
      drawChord = TRUE ;
      break ;
    case MX_CHORD_SNAP_WHOLE:
    default:
      drawStart = TRUE ;
      drawEnd   = TRUE ;
      drawCentre = TRUE ;
      break ;
    }
    break ;
  case ARC_SECTOR:
    switch(garc->selectCount) 
    {
    case MX_SECTOR_SNAP_NONE:
      return FALSE ;
      break ;
    case MX_SECTOR_SNAP_START:
      drawStart = TRUE ;
      break ;
    case MX_SECTOR_SNAP_END:
      drawEnd = TRUE ;
      break ;
    case MX_SECTOR_SNAP_CENTRE:
      drawCentre = TRUE ;
      break ;
    case MX_SECTOR_SNAP_ARC:
      drawArc = TRUE ;
      break ;
    case MX_SECTOR_SNAP_SPAN1:
      drawSpan1 = TRUE ;
      break ;
    case MX_SECTOR_SNAP_SPAN2:
      drawSpan2 = TRUE ;
      break ;
    case MX_SECTOR_SNAP_WHOLE:
    default:
      drawStart = TRUE ;
      drawEnd   = TRUE ;
      drawCentre = TRUE ;
      break ;
    }
    break ;
  default:
    break ;
  }

  if(drawStart) 
  {
    drawSelectMark(err,garc->a.gsap());
    MX_ERROR_CHECK(err);
  }    

  if(drawEnd) 
  {
    drawSelectMark(err,garc->a.geap());
    MX_ERROR_CHECK(err);
  }    

  if(drawCentre) 
  {
    drawSelectMark(err,garc->a.gc());
    MX_ERROR_CHECK(err);
  }    

  if(drawArc) 
  {
    drawSelectArc(err,garc->a);
    MX_ERROR_CHECK(err);
  }    
  
  if(drawChord) 
  {
    mx_rect s(garc->a.gsap(),garc->a.geap(),TRUE) ;

    drawSelectSpan(err,s);
    MX_ERROR_CHECK(err);
  }

  if(drawSpan1) 
  {
    mx_rect s(garc->a.gc(),garc->a.gsap(),TRUE) ;

    drawSelectSpan(err,s);
    MX_ERROR_CHECK(err);
  }

  if(drawSpan2) 
  {
    mx_rect s(garc->a.gc(),garc->a.geap(),TRUE) ;

    drawSelectSpan(err,s);
    MX_ERROR_CHECK(err);
  }

  return TRUE ;
abort:
  return FALSE ;
}






