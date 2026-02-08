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
 * MODULE/CLASS :  mx_simple_word
 *
 * AUTHOR : Andrew Haisley
 *
 *
 *
 * DESCRIPTION:
 *
 * Simple words with 8 bit characters and no style changes
 *
 *
 *
 *
 *
 */

#include <mx_char_style.h>
#include <mx_complex_w.h>
#include <mx_cw_t.h>
#include <mx_simple_w.h>

void mx_simple_word::init()
{
    word_length = 0;
    buffer_length = 0;
    word_pointer = NULL;
    word_array[0] = '\0';
}

mx_simple_word::mx_simple_word()
    : mx_word()
{
    init();
}

mx_simple_word::mx_simple_word(const char* string)
    : mx_word()
{
    init();
    set((char*)string);
}

mx_simple_word::mx_simple_word(const char* string, int len)
    : mx_word()
{
    init();
    set((char*)string, len);
}

mx_simple_word::mx_simple_word(mx_simple_word& s)
    : mx_word()
{
    init();

    x = s.x;
    y = s.y;

    set((char*)s.cstring());
}

void mx_simple_word::set(const char* s)
{
    set(s, strlen(s));
}

void mx_simple_word::set(const char* s, int length)
{
    word_length = length;

    if (word_pointer != NULL || word_length > MX_SIMPLE_WORD_MAX_STATIC_LENGTH) {
        // use dynamically allocated space if either we are already using it or
        // if there is not enough room in the static space.

        if (word_length > buffer_length) {
            // allocate more space if we have not got enough

            if (word_pointer != NULL)
                delete[] word_pointer;
            buffer_length = word_length * 2;
            word_pointer = new char[buffer_length + 1];
        }

        strncpy(word_pointer, s, word_length);
        word_pointer[word_length] = 0;
    } else {
        // in this case, use static space
        memcpy(word_array, s, word_length);
        word_array[length] = 0;
    }
}

mx_simple_word::~mx_simple_word()
{
    if (word_pointer != NULL) {
        delete[] word_pointer;
    }
}

// the width according to the underlying outline font.
float mx_simple_word::width()
{
    return style->width((char*)cstring());
}

float mx_simple_word::width(int index)
{
    int i, err = MX_ERROR_OK;
    float res = 0.0f;
    const char* s = cstring();

    MX_ERROR_ASSERT(err, index <= this->length() && index >= 0);

    for (i = 0; i < index; i++) {
        res += style->width(s[i]);
    }
    return res;
abort:
    global_error_trace->print();
    MX_ERROR_CLEAR(err);
    return 0.0f;
}

// get the word as a C string
const char* mx_simple_word::cstring()
{
    if (word_pointer != NULL) {
        return word_pointer;
    } else {
        return word_array;
    }
}

void mx_simple_word::serialise(int& err, uint8** buffer)
{
    const char* s = cstring();

    err = MX_ERROR_OK;

    if (length() > (MX_SL_OBJ_TEMP_BUFFER_SIZE - 1)) {
        serialise_uint8(1, buffer);
        serialise_uint32(length(), buffer);
        serialise_string((char*)s, buffer);
    } else {
        serialise_uint8(0, buffer);
        serialise_string((char*)s, buffer);
    }
}

void mx_simple_word::unserialise(int& err, uint8** buffer, uint32 docid)
{
    uint8 c;
    unserialise_uint8(c, buffer);

    if (c == 0) {
        unserialise_string((char*)temp_buffer, buffer);

        set((char*)temp_buffer);
    } else {
        uint32 l = 0;
        char* new_buffer = NULL;

        unserialise_uint32(l, buffer);
        new_buffer = new char[l + 1];

        unserialise_string(new_buffer, buffer);

        set(new_buffer);
        delete[] new_buffer;
    }
}

uint32 mx_simple_word::get_serialised_size(int& err)
{
    const char* s = cstring();

    err = MX_ERROR_OK;

    if (length() > (MX_SL_OBJ_TEMP_BUFFER_SIZE - 1)) {
        return SLS_UINT8 + SLS_UINT32 + SLS_STRING(s);
    } else {
        return SLS_UINT8 + SLS_STRING(s);
    }
}

// get/set a particular character
char& mx_simple_word::operator[](int i)
{
    if (i < 0 || i > word_length) {
        mx_printf_warning("word letter index out of bounds");
        return word_array[0];
    }

    if (word_pointer == NULL) {
        return word_array[i];
    } else {
        return word_pointer[i];
    }
}

// append a character
mx_simple_word& mx_simple_word::operator+=(char c)
{
    int err = MX_ERROR_OK;

    insert(err, c, word_length);
    MX_ERROR_CLEAR(err);

    return *this;
}

mx_simple_word& mx_simple_word::operator+=(char* s)
{
    int err = MX_ERROR_OK;
    int i = 0;

    while (s[i] != 0) {
        insert(err, s[i++], word_length);
        MX_ERROR_CLEAR(err);
    }

    return *this;
}

// insert a character
void mx_simple_word::insert(int& err, char c, int index)
{
    int i = word_length + 1;

    char* str;
    int max_len;

    if (index > word_length || index < 0) {
        MX_ERROR_THROW(err, MX_WORD_CHAR_INDEX_RANGE);
    }

    if (word_pointer == NULL) {
        str = word_array;
        max_len = MX_SIMPLE_WORD_MAX_STATIC_LENGTH;
    } else {
        str = word_pointer;
        max_len = buffer_length;
    }

    if (word_length < max_len) {
        // if we don't need to extend our space, then just shuffle the existing
        // characters and insert the new one.

        while (i > index) {
            str[i] = str[i - 1];
            i--;
        }
        str[index] = c;
        word_length++;
    } else {
        // we need more space than we've got, so extend it, allocating some
        // heap memory, copying the old string.

        buffer_length = (word_length + 1) * 2;
        word_pointer = new char[buffer_length + 1];
        strcpy(word_pointer, str);

        // delete str if it was dynamically allocated

        if (str != word_array)
            delete[] str;

        insert(err, c, index);
    }
abort:;
}

// this may need optimising at some point
void mx_simple_word::insert(int& err, char* s, int index)
{
    int i = strlen(s);

    while (i > 0) {
        insert(err, s[--i], index);
    }
}

int mx_simple_word::length() const
{
    return word_length;
}

mx_cw_t* mx_simple_word::remove(int& err, int index)
{
    char* str = word_pointer ? word_pointer : word_array;
    mx_character_cw_t* res = NULL;

    if (index < 0 || index >= word_length) {
        MX_ERROR_THROW(err, MX_WORD_CHAR_INDEX_RANGE);
    }

    res = new mx_character_cw_t(str[index]);

    while (index < word_length) {
        str[index] = str[index + 1];
        index++;
    }
    word_length--;

abort:
    return res;
}

mx_simple_word& mx_simple_word::operator+=(mx_simple_word& w)
{
    if (word_pointer == NULL) {
        if ((word_length + w.length()) > MX_SIMPLE_WORD_MAX_STATIC_LENGTH) {
            buffer_length = (word_length + w.length()) * 2;
            word_pointer = new char[buffer_length + 1];
            strcpy(word_pointer, word_array);
            strcat(word_pointer, w.cstring());
        } else {
            strcat(word_array, w.cstring());
        }
    } else {
        if ((word_length + w.length()) > buffer_length) {
            buffer_length = (word_length + w.length()) * 2;
            char* s = new char[buffer_length + 1];

            strcpy(s, word_pointer);
            strcat(s, w.cstring());

            delete[] word_pointer;
            word_pointer = s;
        } else {
            strcat(word_pointer, w.cstring());
        }
    }
    word_length += w.length();
    return *this;
}

mx_complex_word& mx_simple_word::operator+=(mx_complex_word& w)
{
    int err = MX_ERROR_OK;

    w.insert(err, (char*)cstring(), 0);
    MX_ERROR_CLEAR(err);

    return w;
}

// split the word at a given index returning a new word.
mx_word* mx_simple_word::split(int& err, int index)
{
    mx_simple_word* res;

    if (index >= word_length || index <= 0) {
        MX_ERROR_THROW(err, MX_WORD_CHAR_INDEX_RANGE);
    }

    if (word_pointer == NULL) {
        res = new mx_simple_word(word_array + index);
        word_array[index] = 0;
    } else {
        res = new mx_simple_word(word_pointer + index);
        word_pointer[index] = 0;
    }
    res->style = style;
    word_length = index;
    return res;

abort:;
    return NULL;
}

mx_word* mx_simple_word::move_last_in_start(int& err, int n, mx_word* w)
{
    // what is the class of the other thing?
    if (w->rtti_class() == mx_simple_word_class_e) {
        mx_simple_word* sw = (mx_simple_word*)w;
        char c;

        while (n > 0) {
            c = sw->get(err, sw->length() - 1);
            MX_ERROR_CHECK(err);

            sw->remove(err, sw->length() - 1);
            MX_ERROR_CHECK(err);

            insert(err, c);
            MX_ERROR_CHECK(err);

            n--;
        }
        return this;
    } else {
        if (w->rtti_class() == mx_complex_word_class_e) {
            mx_complex_word* res = new mx_complex_word(cstring());

            res->set_style(style);

            res->move_last_in_start(err, n, w);
            MX_ERROR_CHECK(err);

            delete this;

            return res;
        } else {
            MX_ERROR_THROW(err, MX_WORD_OP_NOT_AVAILABLE_ON_SUBCLASS);
        }
    }
abort:;
    return this;
}

mx_word* mx_simple_word::move_first_in_end(int& err, int n, mx_word* w)
{
    // what is the class of the other thing?
    if (w->rtti_class() == mx_simple_word_class_e) {
        mx_simple_word* sw = (mx_simple_word*)w;
        char c;

        while (n > 0) {
            c = sw->get(err, 0);
            MX_ERROR_CHECK(err);

            sw->remove(err, 0);
            MX_ERROR_CHECK(err);

            *this += c;
            n--;
        }
        return this;
    } else {
        if (w->rtti_class() == mx_complex_word_class_e) {
            mx_complex_word* res = new mx_complex_word(cstring());

            res->set_style(style);

            res->move_first_in_end(err, n, w);
            MX_ERROR_CHECK(err);

            delete this;

            return res;
        } else {
            MX_ERROR_THROW(err, MX_WORD_OP_NOT_AVAILABLE_ON_SUBCLASS);
        }
    }
abort:
    return this;
}

char mx_simple_word::get(int& err, int i)
{
    if (i < 0 || i >= word_length) {
        MX_ERROR_THROW(err, MX_WORD_CHAR_INDEX_RANGE);
    } else {
        if (word_pointer != NULL) {
            return word_pointer[i];
        } else {
            return word_array[i];
        }
    }
abort:
    return '\0';
}

bool mx_simple_word::is_empty()
{
    return word_length == 0;
}

// split the word at a given width
mx_word* mx_simple_word::split(int& err, float width)
{
    int i = 0;
    float f = 0;

    const char* s = cstring();

    while (s[i] != 0) {
        f += style->width(s[i]);
        if (f > width) {
            break;
        } else {
            i++;
        }
    }

    if (i == 0) {
        if (this->length() < 2) {
            return NULL;
        } else {
            return split(err, 1);
        }
    } else {
        return split(err, i);
    }
}

mx_word* mx_simple_word::cut(int& err, int start, int end)
{
    mx_simple_word* res;

    if (end == -1)
        end = word_length;

    if (end > word_length || end < 0 || start < 0 || start >= end) {
        MX_ERROR_THROW(err, MX_WORD_CHAR_INDEX_RANGE);
    }

    if (word_pointer == NULL) {
        res = new mx_simple_word(word_array + start, end - start);
        memmove(word_array + start, word_array + end, word_length - end + 1);
    } else {
        res = new mx_simple_word(word_pointer + start, end - start);
        memmove(word_pointer + start, word_pointer + end, word_length - end + 1);
    }

    res->style = style;
    word_length = word_length - (end - start);
    return res;

abort:
    return NULL;
}

int mx_simple_word::get_num_visible_items()
{
    return word_length;
}

bool mx_simple_word::can_be_part_of_long_word()
{
    return TRUE;
}

mx_cw_t* mx_simple_word::remove(int& err, int start, int end)
{
    mx_cw_t* res = NULL;
    if (end > word_length) {
        MX_ERROR_THROW(err, MX_WORD_CHAR_INDEX_RANGE);
    }

    if (end == -1) {
        end = word_length;
    }

    if (start == (end - 1)) {
        if (word_pointer == NULL) {
            res = new mx_character_cw_t(word_array[start]);
        } else {
            res = new mx_character_cw_t(word_pointer[start]);
        }
    }

    if (end < 1 || start < 0 || start >= word_length || start >= end) {
        MX_ERROR_THROW(err, MX_WORD_CHAR_INDEX_RANGE);
    }

    if (word_pointer == NULL) {
        memmove(word_array + start, word_array + end, word_length - end + 1);
    } else {
        memmove(word_pointer + start, word_pointer + end, word_length - end + 1);
    }
    word_length -= end - start;

abort:
    return res;
}
