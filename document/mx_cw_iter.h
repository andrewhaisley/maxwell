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
 * MODULE/CLASS :  mx_cw_iter
 *
 * AUTHOR : Andrew Haisley
 *
 *
 *
 * DESCRIPTION:
 *
 * A class to iterate over the alpha numeric sections of a complex word
 *
 *
 *
 */
#ifndef MX_CW_ITER_H
#define MX_CW_ITER_H

#include <mx_complex_w.h>
#include <mx_list.h>

#define MX_CW_ITER_MAX_STRING 100

class mx_cw_iter // : public mx_rtti
{
    // MX_RTTI(mx_cw_iter_class_e)

public:
    mx_cw_iter(mx_complex_word* w);

    bool more(int& err);
    char* data(int& err);

    uint32 start_index;
    uint32 end_index;

private:
    bool first;
    char substring[MX_CW_ITER_MAX_STRING + 1];

    mx_complex_word* word;

    void increment(int& err);
};

#endif
