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
#include <mx_cw_iter.h>
#include <mx_cw_t.h>

mx_cw_iter::mx_cw_iter(mx_complex_word* w)
{
    start_index = 0;
    end_index = 0;
    first = TRUE;
    word = w;
}

bool mx_cw_iter::more(int& err)
{
    mx_cw_t* item;

    if (first) {
        first = FALSE;

        // find the first character item
        while (start_index < word->get_num_items()) {
            item = word->get_item(err, start_index);
            MX_ERROR_CHECK(err);

            if (item->rtti_class() == mx_character_cw_t_class_e) {
                end_index = start_index;
                while (end_index < word->get_num_items()) {
                    item = word->get_item(err, end_index);
                    MX_ERROR_CHECK(err);

                    if (item->rtti_class() == mx_character_cw_t_class_e) {
                        end_index++;
                    } else {
                        break;
                    }
                }
                return TRUE;
            } else {
                start_index++;
            }
        }

        // no characters at all
        return FALSE;
    } else {
        increment(err);
        MX_ERROR_CHECK(err);

        return start_index < end_index;
    }
abort:
    return FALSE;
}

void mx_cw_iter::increment(int& err)
{
    mx_cw_t* item;

    start_index = end_index;

    while (start_index < word->get_num_items()) {
        item = word->get_item(err, start_index);
        MX_ERROR_CHECK(err);

        if (item->rtti_class() == mx_character_cw_t_class_e) {
            end_index = start_index;

            while (end_index < word->get_num_items()) {
                item = word->get_item(err, end_index);
                MX_ERROR_CHECK(err);

                if (item->rtti_class() == mx_character_cw_t_class_e) {
                    end_index++;
                } else {
                    break;
                }
            }
            return;
        } else {
            start_index++;
        }
    }

abort:;
}

char* mx_cw_iter::data(int& err)
{
    uint32 n = 0, i = start_index;
    ;
    mx_cw_t* item;

    while ((i < end_index) && (n < MX_CW_ITER_MAX_STRING)) {
        item = word->get_item(err, i);
        MX_ERROR_CHECK(err);

        if (item->rtti_class() == mx_character_cw_t_class_e) {
            substring[n++] = ((mx_character_cw_t*)item)->c;
        }
        i++;
    }
    substring[n] = 0;
    return substring;

abort:;
    return NULL;
}
