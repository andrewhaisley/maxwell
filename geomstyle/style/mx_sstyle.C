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
 * MODULE : mx_sstyle.C
 *
 * AUTHOR : David Miller
 *
 * This file 
 *
 * DESCRIPTION: 
 * Module mx_sstyle.C 
 *
 *
 * Revision 1.1  1996/10/06 13:15:45  andrew
 * Initial revision
 *
 *
 */

#include "mx_gstyle.h"


/*-------------------------------------------------
 * FUNCTION: mx_single_styles::mx_single_styles
 *
 * DESCRIPTION: 
 * 
 *
 */

mx_single_styles::mx_single_styles(mx_gstyle_type_t istyleType)
               :stylesById(10),stylesByName(mx_string,mx_data)
{
  currentId = 0 ;
  styleType = istyleType ;
  
 abort:
  return ;
}

/*-------------------------------------------------
 * FUNCTION: mx_single_styles::~mx_single_styles
 *
 * DESCRIPTION: 
 * 
 *
 */

mx_single_styles::~mx_single_styles()
{
  int                err ;
  iterator           it(err,&stylesByName) ;
  mx_attribute_value data,key ;
  mx_geomstyle          *gs ;

  MX_ERROR_CHECK(err);
  
  it.start(err) ;
  MX_ERROR_CHECK(err) ;
  
  /* Delete the mx_geomstyles */
  
  while(it.next(err,&key,&data)) 
  {
    gs = (mx_geomstyle *) data.data ;
    delete gs ;
  }
  MX_ERROR_CHECK(err) ;
abort:
  return ;
}

/*-------------------------------------------------
 * FUNCTION: mx_single_styles::add
 *
 * DESCRIPTION: Insert a new style into global collections
 * 
 *
 */

void  mx_single_styles::add(int &err,mx_geomstyle *gstyle)
{
    mx_attribute_value key,data ;
    int dataCount ;
  
    err = MX_ERROR_OK ;
    
    // Add to hash table
    stylesById.add(err,gstyle->getId(),gstyle) ;
    MX_ERROR_CHECK(err) ;
  
    // add to dictionary - first get storage name
    key.s = gstyle->getName() ;
    data.data  = gstyle ;
  
    stylesByName.add(err,key,data,dataCount);
    MX_ERROR_CHECK(err) ;

abort:
    return ;
}

/*-------------------------------------------------
 * FUNCTION: mx_single_styles::insert
 *
 * DESCRIPTION: Insert a new style into global collections
 * 
 *
 */

int mx_single_styles::insert(int &err,mx_geomstyle *gstyle)
{
  err = MX_ERROR_OK ;
  
  // add a totally new style 
  if(gstyle->getId() != -1) return gstyle->getId();

  // get next id
  gstyle->setId(currentId) ;
  currentId++ ;
  
  add(err,gstyle) ;
  MX_ERROR_CHECK(err) ;

 abort:
  return (currentId-1);
}

/*-------------------------------------------------
 * FUNCTION: mx_single_styles::getStyleById
 *
 * DESCRIPTION: 
 * 
 *
 */

mx_geomstyle *mx_single_styles::getStyleById(int &err,int id)
{
  mx_geomstyle *gs ;
  
  err = MX_ERROR_OK ;
  
  // get from hash table
  gs = (mx_geomstyle *) stylesById.get(err,id) ;
  MX_ERROR_CHECK(err) ;
  
  return gs ;
  
 abort:
  return NULL ;
  
}

/*-------------------------------------------------
 * FUNCTION: mx_single_styles::getStyleByName
 *
 * DESCRIPTION: 
 * 
 *
 */

mx_geomstyle *mx_single_styles::getStyleByName(int              &err,
					       char             *iname) 
{
  mx_geomstyle *gs ;
  mx_attribute_value key,data ; 

  err = MX_ERROR_OK ;
  
  if(iname == NULL) 
  {
      MX_ERROR_THROW(err,MX_NULL_GSTYLE_NAME) ;
  }
  
  // create name
  key.s = iname ;
  
  // get from collection
  if(stylesByName.get(err,key,data)) 
  {
    gs = (mx_geomstyle *) data.data ;
  }
  else
  {
    MX_ERROR_CHECK(err) ;
    gs = NULL ;
  }

abort:
  return gs ;
}

/*-------------------------------------------------
 * FUNCTION: mx_single_styles::get_serialised_size
 *
 * DESCRIPTION: 
 * 
 *
 */

uint32 mx_single_styles::get_serialised_size(int           &err) 
{
    int      istyle ;
    int      size ;
    int      numberStyles ;
    mx_attribute_value key,data ;
    mx_geomstyle *gs ;

    iterator it(err,&stylesByName) ;    
    MX_ERROR_CHECK(err);
    
    // current id and number of styles 
    size = 3*SLS_INT32 ;

    numberStyles = stylesByName.count() ;
    
    it.start(err) ;
    MX_ERROR_CHECK(err) ;

    for(istyle=0;istyle<numberStyles;istyle++) 
    {
	while(it.next(err,&key,&data)) 
	{
	    gs = (mx_geomstyle *) data.data ;
	    
	    size += gs->get_serialised_size(err);
	    MX_ERROR_CHECK(err) ;	
	}
	MX_ERROR_CHECK(err) ;	
    }

abort:
    return size ;
}

/*-------------------------------------------------
 * FUNCTION: mx_single_styles::serialise
 *
 * DESCRIPTION: 
 * 
 *
 */

void mx_single_styles::serialise(int           &err,
				 unsigned char **buffer) 
{
    int32     istyle ;
    int32     numberStyles ;
    mx_attribute_value key,data ;
    mx_geomstyle *gs ;

    iterator  it(err,&stylesByName) ;    
    MX_ERROR_CHECK(err);
    
    numberStyles = stylesByName.count() ;
    istyle       = styleType ;
    
    serialise_int32(currentId,buffer) ;
    serialise_int32(numberStyles,buffer) ;
    serialise_int32(istyle,buffer) ;

    it.start(err) ;
    MX_ERROR_CHECK(err) ;

    for(istyle=0;istyle<numberStyles;istyle++) 
    {
	while(it.next(err,&key,&data)) 
	{
	    gs = (mx_geomstyle *) data.data ;
	    
	    gs->serialise(err,buffer);
	    MX_ERROR_CHECK(err) ;	
	}
	MX_ERROR_CHECK(err) ;	
    }
abort:
    return ;
}

/*-------------------------------------------------
 * FUNCTION: mx_single_styles::unserialise
 *
 * DESCRIPTION: 
 * 
 *
 */

void mx_single_styles::unserialise(int           &err,
				   unsigned char **buffer) 
{
    int32 numberStyles ;
    int32 istyle;
    mx_gline_style   *lineStyle ;
    mx_gdash_style   *dashStyle ;
    mx_gpoint_style  *pointStyle ;
    mx_garea_style   *areaStyle ;
    mx_gborder_style *borderStyle ;
    mx_ghatch_style  *hatchStyle ;
    mx_gfill_style   *fillStyle ;
    mx_geomstyle     *gstyle ;
    int iistyle ;
    
    unserialise_int32(currentId,buffer) ;
    unserialise_int32(numberStyles,buffer) ;
    unserialise_int32(istyle,buffer) ;
    
    styleType = (mx_gstyle_type_t) istyle ;
    
    for(iistyle=0;iistyle<numberStyles;iistyle++) 
    {
	switch(styleType)
        {
	case mx_gstyle_dash:
	    dashStyle   = new mx_gdash_style ;
	    gstyle      = dashStyle ;
	    break ;
	case mx_gstyle_line:
	    lineStyle   = new mx_gline_style ;
	    gstyle      = lineStyle ;
	    break ;
	case mx_gstyle_fill:
	    fillStyle   = new mx_gfill_style ;
	    gstyle      = fillStyle ;
	    break ;
	case mx_gstyle_point:
	    pointStyle  = new mx_gpoint_style ;
	    gstyle      = pointStyle ;
	    break ;
	case mx_gstyle_area:
	    areaStyle   = new mx_garea_style ;
	    gstyle      = areaStyle ;
	    break ;
	case mx_gstyle_border:
	    borderStyle = new mx_gborder_style ;
	    gstyle      = borderStyle ;
	    break ;
	case mx_gstyle_hatch:
	    hatchStyle  = new mx_ghatch_style ;
	    gstyle      = hatchStyle ;
	    break ;
	default:
	    break;
	}
    
	gstyle->unserialise(err,buffer) ;
	MX_ERROR_CHECK(err) ;
	
	add(err,gstyle) ;
	MX_ERROR_CHECK(err) ;
    }
    
abort:
    return ;
}











