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
 * MODULE : mx_styles.C
 *
 * AUTHOR : David Miller
 *
 * This file 
 *
 * DESCRIPTION: 
 * Module mx_styles.C 
 *
 *
 * Revision 1.1  1996/10/06 13:15:45  andrew
 * Initial revision
 *
 *
 */

#include "mx_gstyle.h"

mx_gpoint_style::mx_gpoint_style(char *name) 
    :mx_geomstyle(name,  mx_gstyle_point) 
{
}

mx_gdash_style::mx_gdash_style(char *name) 
    :mx_geomstyle(name,  mx_gstyle_dash) 
{
}

mx_gline_style::mx_gline_style(char *name) 
    :mx_geomstyle(name,  mx_gstyle_line) 
{
}


mx_ghatch_style::mx_ghatch_style(char *name) 
    :mx_geomstyle(name,  mx_gstyle_hatch) 
{
}


mx_gfill_style::mx_gfill_style(char *name) 
    :mx_geomstyle(name,  mx_gstyle_fill) 
{
}

mx_gborder_style::mx_gborder_style(char *name) 
    :mx_geomstyle(name,  mx_gstyle_border) 
{
}

mx_garea_style::mx_garea_style(char *name) 
    :mx_geomstyle(name,  mx_gstyle_area) 
{
}



void mx_gpoint_style::serialise(int           &err,
				unsigned char **buffer) 
{
    mx_geomstyle::serialise(err,buffer) ;
    MX_ERROR_CHECK(err) ;
    
    style.serialise(err,buffer) ;
    MX_ERROR_CHECK(err) ;

abort:
    return ;
}

void mx_gpoint_style::unserialise(int           &err,
				  unsigned char **buffer) 
{
    mx_geomstyle::unserialise(err,buffer) ;
    MX_ERROR_CHECK(err) ;
    
    style.unserialise(err,buffer) ;
    MX_ERROR_CHECK(err) ;

abort:
    return ;
}

uint32 mx_gpoint_style::get_serialised_size(int           &err) 
{
    uint32 size ;
    
    size = mx_geomstyle::get_serialised_size(err) ;
    MX_ERROR_CHECK(err) ;
    
    size += style.get_serialised_size(err) ;
    MX_ERROR_CHECK(err) ;

abort:
    return size ;
}



void mx_gline_style::serialise(int           &err,
				unsigned char **buffer) 
{
    mx_geomstyle::serialise(err,buffer) ;
    MX_ERROR_CHECK(err) ;
    
    style.serialise(err,buffer) ;
    MX_ERROR_CHECK(err) ;

abort:
    return ;
}

void mx_gline_style::unserialise(int           &err,
				  unsigned char **buffer) 
{
    mx_geomstyle::unserialise(err,buffer) ;
    MX_ERROR_CHECK(err) ;
    
    style.unserialise(err,buffer) ;
    MX_ERROR_CHECK(err) ;

abort:
    return ;
}

uint32 mx_gline_style::get_serialised_size(int           &err) 
{
    uint32 size ;
    
    size = mx_geomstyle::get_serialised_size(err) ;
    MX_ERROR_CHECK(err) ;
    
    size += style.get_serialised_size(err) ;
    MX_ERROR_CHECK(err) ;

abort:
    return size ;
}


void mx_gdash_style::serialise(int           &err,
				unsigned char **buffer) 
{
    mx_geomstyle::serialise(err,buffer) ;
    MX_ERROR_CHECK(err) ;
    
    style.serialise(err,buffer) ;
    MX_ERROR_CHECK(err) ;

abort:
    return ;
}

void mx_gdash_style::unserialise(int           &err,
				  unsigned char **buffer) 
{
    mx_geomstyle::unserialise(err,buffer) ;
    MX_ERROR_CHECK(err) ;
    
    style.unserialise(err,buffer) ;
    MX_ERROR_CHECK(err) ;

abort:
    return ;
}

uint32 mx_gdash_style::get_serialised_size(int           &err) 
{
    uint32 size ;
    
    size = mx_geomstyle::get_serialised_size(err) ;
    MX_ERROR_CHECK(err) ;
    
    size += style.get_serialised_size(err) ;
    MX_ERROR_CHECK(err) ;

abort:
    return size;
}


void mx_garea_style::serialise(int           &err,
				unsigned char **buffer) 
{
    mx_geomstyle::serialise(err,buffer) ;
    MX_ERROR_CHECK(err) ;
    
    style.serialise(err,buffer) ;
    MX_ERROR_CHECK(err) ;

abort:
    return ;
}

void mx_garea_style::unserialise(int           &err,
				  unsigned char **buffer) 
{
    mx_geomstyle::unserialise(err,buffer) ;
    MX_ERROR_CHECK(err) ;
    
    style.unserialise(err,buffer) ;
    MX_ERROR_CHECK(err) ;

abort:
    return ;
}

uint32 mx_garea_style::get_serialised_size(int           &err) 
{
    uint32 size ;
    
    size = mx_geomstyle::get_serialised_size(err) ;
    MX_ERROR_CHECK(err) ;
    
    size += style.get_serialised_size(err) ;
    MX_ERROR_CHECK(err) ;

abort:
    return size ;
}


void mx_ghatch_style::serialise(int           &err,
				unsigned char **buffer) 
{
    mx_geomstyle::serialise(err,buffer) ;
    MX_ERROR_CHECK(err) ;
    
    style.serialise(err,buffer) ;
    MX_ERROR_CHECK(err) ;

abort:
    return ;
}

void mx_ghatch_style::unserialise(int           &err,
				  unsigned char **buffer) 
{
    mx_geomstyle::unserialise(err,buffer) ;
    MX_ERROR_CHECK(err) ;
    
    style.unserialise(err,buffer) ;
    MX_ERROR_CHECK(err) ;

abort:
    return ;
}

uint32 mx_ghatch_style::get_serialised_size(int           &err) 
{
    uint32 size ;
    
    size = mx_geomstyle::get_serialised_size(err) ;
    MX_ERROR_CHECK(err) ;
    
    size += style.get_serialised_size(err) ;
    MX_ERROR_CHECK(err) ;

abort:
    return size ;
}


void mx_gborder_style::serialise(int           &err,
				unsigned char **buffer) 
{
    mx_geomstyle::serialise(err,buffer) ;
    MX_ERROR_CHECK(err) ;
    
    style.serialise(err,buffer) ;
    MX_ERROR_CHECK(err) ;

abort:
    return ;
}

void mx_gborder_style::unserialise(int           &err,
				  unsigned char **buffer) 
{
    mx_geomstyle::unserialise(err,buffer) ;
    MX_ERROR_CHECK(err) ;
    
    style.unserialise(err,buffer) ;
    MX_ERROR_CHECK(err) ;

abort:
    return ;
}

uint32 mx_gborder_style::get_serialised_size(int           &err) 
{
    uint32 size ;
    
    size = mx_geomstyle::get_serialised_size(err) ;
    MX_ERROR_CHECK(err) ;
    
    size += style.get_serialised_size(err) ;
    MX_ERROR_CHECK(err) ;

abort:
    return size ;
}


void mx_gfill_style::serialise(int           &err,
				unsigned char **buffer) 
{
    mx_geomstyle::serialise(err,buffer) ;
    MX_ERROR_CHECK(err) ;
    
    style.serialise(err,buffer) ;
    MX_ERROR_CHECK(err) ;

abort:
    return ;
}

void mx_gfill_style::unserialise(int           &err,
				  unsigned char **buffer) 
{
    mx_geomstyle::unserialise(err,buffer) ;
    MX_ERROR_CHECK(err) ;
    
    style.unserialise(err,buffer) ;
    MX_ERROR_CHECK(err) ;

abort:
    return ;
}

uint32 mx_gfill_style::get_serialised_size(int           &err) 
{
    uint32 size ;
    
    size = mx_geomstyle::get_serialised_size(err) ;
    MX_ERROR_CHECK(err) ;
    
    size += style.get_serialised_size(err) ;
    MX_ERROR_CHECK(err) ;

abort:
    return size ;
}

