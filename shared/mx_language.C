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
 * MODULE/CLASS : mx_language
 *
 * AUTHOR : Andrew Haisley
 *
 *
 *
 *
 *
 */

#include <cstring>
#include <iostream>
#include <unicode/uloc.h>
#include <unicode/ustring.h>
#include <enchant-2/enchant.h>
#include <mx_language.h>

using namespace std;

std::vector<std::string> mx_language::codes;
string mx_language::default_language;

string mx_language::get_language_name(string tag) {

    const char *locale = uloc_getDefault();

    if (strcmp(locale, "C") == 0) {
        locale = MX_DEFAULT_LANGUAGE;
    }

    UChar display_name[256];
    char display_utf8[256];

    UErrorCode status = U_ZERO_ERROR;

    uloc_getDisplayName(tag.c_str(), locale, display_name, 256, &status);

    if (U_FAILURE(status)) {
        return u_errorName(status);
    }

    status = U_ZERO_ERROR;
    u_strToUTF8(display_utf8, sizeof(display_utf8), NULL, display_name, -1, &status);

    if (U_FAILURE(status)) {
        return u_errorName(status);
    }

    return display_utf8;
}

void list_dicts_cb(const char * const lang_tag,
                   const char * const provider_name,
                   const char * const provider_desc,
                   const char * const provider_file,
                   void * user_data)
{
    mx_language::codes.push_back(lang_tag);
}

void mx_language::init()
{
    EnchantBroker *broker = enchant_broker_init();

    if (!broker) {
        return;
    }

    enchant_broker_list_dicts(broker, list_dicts_cb, NULL);
    enchant_broker_free(broker);

    set_default_language(MX_DEFAULT_LANGUAGE);
}

string mx_language::get_default_language()
{
    return default_language;
}

void mx_language::set_default_language(string name)
{
    default_language = name;
}
