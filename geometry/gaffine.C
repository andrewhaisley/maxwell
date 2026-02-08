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
 * MODULE : gaffine.C
 *
 * AUTHOR : David Miller
 *
 * This file
 *
 * DESCRIPTION:
 * Module gaffine.C
 *
 *
 * Revision 1.1  1996/10/06 13:15:45  andrew
 * Initial revision
 *
 *
 */

#include "geometry.h"

mx_angle defaultAngle;

/*-------------------------------------------------
 * FUNCTION: mx_affine::mx_affine
 *
 * DESCRIPTION: Create a default zero affine transform
 *
 *
 */

mx_affine::mx_affine()
{
    a11 = a12 = a21 = a22 = b1 = b2 = 0;
}

mx_rotation::mx_rotation(double iangle)
{
    set(c, iangle);
}

mx_rotation::mx_rotation(const mx_angle& iangle)
{
    set(c, iangle);
}

mx_rotation::mx_rotation(const mx_point& ic, double iangle)
{
    set(ic, iangle);
}

mx_rotation::mx_rotation(const mx_point& ic, const mx_angle& iangle)
{
    set(ic, iangle);
}

void mx_rotation::set(const mx_point& ic, double iangle)
{
    c = ic;
    a.set(iangle);
}

void mx_rotation::set(const mx_point& ic, const mx_angle& iangle)
{
    c = ic;
    a.set(iangle);
}

mx_angle::mx_angle(double iangle)
{
    set(iangle);
}

mx_angle::mx_angle(const mx_angle& ia)
{
    set(ia);
}

void mx_angle::set(const mx_angle& iangle)
{
    *this = iangle;
}

void mx_angle::set(double iangle)
{
    angle = iangle;

    if (angle == 0) {
        cosang = 1;
        sinang = 0;
    } else {
        cosang = cos(angle);
        sinang = sin(angle);
    }
}

int mx_angle::quadrant()
{
    if (sinang > 0) {
        if (cosang > 0) {
            return 0;
        } else {
            return 1;
        }
    } else {
        if (cosang > 0) {
            return 3;
        } else {
            return 2;
        }
    }
}

mx_point mx_angle::quadrantRange()
{
    return squadrantRange(quadrant());
}

mx_point mx_angle::squadrantRange(int iquad)
{
    mx_point p;

    switch (iquad) {
    case 0:
        p.x = 0;
        p.y = M_PI_2;
        break;
    case 1:
        p.x = M_PI_2;
        p.y = M_PI;
        break;
    case 2:
        p.x = M_PI;
        p.y = M_PI + M_PI_2;
        break;
    case 3:
        p.x = M_PI + M_PI_2;
        p.y = M_PI + M_PI;
        break;
    }

    return p;
}
