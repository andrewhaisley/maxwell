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
 * MODULE/CLASS : mx_hash
 *
 * AUTHOR : Andrew Haisley
 *
 *
 *
 *
 *
 */
#include <math.h>
#include <mx.h>
#include <mx_hash.h>
#include <stdio.h>
#include <string.h>

#define DEFAULT_SIZE 1000

mx_hash::mx_hash(uint32 init_size)
{
    size = init_size;

    table = new entry_t[size];

    init();
}

mx_hash::mx_hash()
{
    size = DEFAULT_SIZE;

    table = new entry_t[size];
    init();
}

void mx_hash::init()
{
    uint32 i;

    for (i = 0; i < size; i++) {
        table[i].key.i = 0;
        table[i].item = NULL;
        table[i].continuation = NULL;
        table[i].usage = unused;
    }
    num_entries = 0;
    num_cont = 0;
}

void mx_hash::add(int& err, uint32 key, const void* item)
{
    uint32 s, table_index;

    table_index = key % size;
    if (table[table_index].usage == unused) {
        // slot free
        table[table_index].key.i = key;
        table[table_index].item = item;
        table[table_index].usage = int_key;
        num_entries++;
        err = MX_ERROR_OK;
    } else {
        // check for duplicate
        if (table[table_index].key.i == key) {
            err = MX_HASH_DUPLICATE;
            return;
        }

        // not free, sub-hash
        if (table[table_index].continuation == NULL) {
            // create a sub-table
            s = (uint32)sqrt(size * 1.0);
            if (s < 20)
                s = 20;
            if (size == s)
                s--;
            table[table_index].continuation = new mx_hash(s);
            num_cont++;
        }
        // now, add the entry to the sub-table
        table[table_index].continuation->add(err, key, item);
        if (err == MX_ERROR_OK) {
            num_entries++;
        }
    }
}

void mx_hash::aka_add(int& err, uint32 gen_key, const char* key, const void* item)
{
    uint32 s, table_index;

    table_index = gen_key % size;
    if (table[table_index].usage == unused) {
        // slot free
        table[table_index].key.s = mx_string_copy(key);

        table[table_index].item = item;
        table[table_index].usage = string_key;
        num_entries++;
        err = MX_ERROR_OK;
    } else {
        // check for duplicate
        if (strcmp(table[table_index].key.s, key) == 0) {
            err = MX_HASH_DUPLICATE;
            return;
        }

        // not free, sub-hash
        if (table[table_index].continuation == NULL) {
            // create a sub-table
            s = (uint32)sqrt(size * 1.0);
            if (s < 20) {
                s = 20;
            }
            if (size == s) {
                s--;
            }
            table[table_index].continuation = new mx_hash(s);
            num_cont++;
        }

        // now, add the entry to the sub-table
        table[table_index].continuation->aka_add(err, gen_key, key, item);
        if (err == MX_ERROR_OK) {
            num_entries++;
        }
    }
}

uint32 mx_hash::gen_key(const char* key) const
{
    uint32 res = 1, i, n;

    n = strlen(key);
    for (i = 0; i < n; i++) {
        res = res * key[i];
    }
    return res;
}

void mx_hash::add(int& err, const char* key, const void* item)
{
    aka_add(err, gen_key((char*)key), key, item);
}

void mx_hash::add_or_repl(int& err, uint32 key, const void* item)
{
    err = MX_ERROR_OK;
    add(err, key, item);
    if (err == MX_HASH_DUPLICATE) {
        MX_ERROR_CLEAR(err);

        del(err, key);
        MX_ERROR_CHECK(err);

        add_or_repl(err, key, item);
        MX_ERROR_CHECK(err);
    } else {
        MX_ERROR_CHECK(err);
    }

    return;
abort:
    return;
}

void mx_hash::add_or_repl(int& err, const char* key, const void* item)
{
    err = MX_ERROR_OK;
    add(err, key, item);
    if (err == MX_HASH_DUPLICATE) {
        MX_ERROR_CLEAR(err);

        del(err, key);
        MX_ERROR_CHECK(err);

        add_or_repl(err, key, item);
        MX_ERROR_CHECK(err);
    } else {
        MX_ERROR_CHECK(err);
    }

    return;
abort:
    return;
}

const void* mx_hash::get(int& err, uint32 key)
{
    uint32 table_index;

    table_index = key % size;

    if ((table[table_index].usage == unused) && (table[table_index].continuation == NULL)) {
        // item not found
        err = MX_HASH_NOT_FOUND;
        return NULL;
    }

    // is it this one ?
    if ((table[table_index].usage == int_key) && (table[table_index].key.i == key)) {
        // yes
        err = MX_ERROR_OK;
        return table[table_index].item;
    } else {
        // no, is there a sub table
        if (table[table_index].continuation == NULL) {
            // item not found
            err = MX_HASH_NOT_FOUND;
            return NULL;
        } else {
            return table[table_index].continuation->get(err, key);
        }
    }
}

const void* mx_hash::aka_get(int& err, uint32 gen_key, const char* key)
{
    uint32 table_index;

    table_index = gen_key % size;

    if ((table[table_index].usage == unused) && (table[table_index].continuation == NULL)) {
        // item not found
        err = MX_HASH_NOT_FOUND;
        return NULL;
    }

    // is it this one ?
    if ((table[table_index].usage == string_key) && (strcmp(table[table_index].key.s, key) == 0)) {
        // yes
        err = MX_ERROR_OK;
        return table[table_index].item;
    } else {
        // no, is there a sub table
        if (table[table_index].continuation == NULL) {
            // item not found
            err = MX_HASH_NOT_FOUND;
            return NULL;
        } else {
            return table[table_index].continuation->aka_get(err, gen_key, key);
        }
    }
}

const void* mx_hash::get(int& err, const char* key)
{
    return aka_get(err, gen_key(key), key);
}

void mx_hash::del(int& err, uint32 key)
{
    uint32 table_index;

    table_index = key % size;

    if ((table[table_index].usage == unused) && (table[table_index].continuation == NULL)) {
        // item not found
        err = MX_HASH_NOT_FOUND;
        return;
    }

    // is it this one ?
    if ((table[table_index].usage == int_key) && (table[table_index].key.i == key)) {
        // yes
        err = MX_ERROR_OK;
        table[table_index].usage = unused;
        num_entries--;
        return;
    } else {
        // no, is there a sub table
        if (table[table_index].continuation == NULL) {
            // item not found
            err = MX_HASH_NOT_FOUND;
            return;
        } else {
            table[table_index].continuation->del(err, key);
            if ((table[table_index].continuation->num_entries == 0) && (table[table_index].continuation->num_cont == 0)) {
                num_cont--;
                delete table[table_index].continuation;
                table[table_index].continuation = NULL;
            }
        }
    }
}

void mx_hash::del_safe(int& err, uint32 key)
{
    uint32 table_index;

    table_index = key % size;

    if ((table[table_index].usage == unused) && (table[table_index].continuation == NULL)) {
        // item not found
        err = MX_HASH_NOT_FOUND;
        return;
    }

    // is it this one ?
    if ((table[table_index].usage == int_key) && (table[table_index].key.i == key)) {
        // yes
        err = MX_ERROR_OK;
        table[table_index].usage = unused;
        num_entries--;
        return;
    } else {
        // no, is there a sub table
        if (table[table_index].continuation == NULL) {
            // item not found
            err = MX_HASH_NOT_FOUND;
            return;
        } else {
            table[table_index].continuation->del_safe(err, key);
        }
    }
}

void mx_hash::aka_del(int& err, uint32 gen_key, const char* key)
{
    uint32 table_index;

    table_index = gen_key % size;

    if ((table[table_index].usage == unused) && (table[table_index].continuation == NULL)) {
        // item not found
        err = MX_HASH_NOT_FOUND;
        return;
    }

    // is it this one ?
    if ((table[table_index].usage == string_key) && (strcmp(table[table_index].key.s, key) == 0)) {
        // yes
        err = MX_ERROR_OK;
        table[table_index].usage = unused;
        delete[] table[table_index].key.s;
        num_entries--;
        return;
    } else {
        // no, is there a sub table
        if (table[table_index].continuation == NULL) {
            // item not found
            err = MX_HASH_NOT_FOUND;
            return;
        } else {
            table[table_index].continuation->aka_del(err, gen_key, key);
            if ((table[table_index].continuation->num_entries == 0) && (table[table_index].continuation->num_cont == 0)) {
                num_cont--;
                delete table[table_index].continuation;
                table[table_index].continuation = NULL;
            }
        }
    }
}

void mx_hash::aka_del_safe(int& err, uint32 gen_key, const char* key)
{
    uint32 table_index;

    table_index = gen_key % size;

    if ((table[table_index].usage == unused) && (table[table_index].continuation == NULL)) {
        // item not found
        err = MX_HASH_NOT_FOUND;
        return;
    }

    // is it this one ?
    if ((table[table_index].usage == string_key) && (strcmp(table[table_index].key.s, key) == 0)) {
        // yes
        err = MX_ERROR_OK;
        table[table_index].usage = unused;
        delete[] table[table_index].key.s;
        num_entries--;
        return;
    } else {
        // no, is there a sub table
        if (table[table_index].continuation == NULL) {
            // item not found
            err = MX_HASH_NOT_FOUND;
            return;
        } else {
            table[table_index].continuation->aka_del_safe(err, gen_key, key);
        }
    }
}

void mx_hash::del(int& err, const char* key)
{
    aka_del(err, gen_key(key), key);
}

void mx_hash::del_safe(int& err, const char* key)
{
    aka_del_safe(err, gen_key(key), key);
}

uint32 mx_hash::get_num_entries(int& err)
{
    err = MX_ERROR_OK;
    return num_entries;
}

const void* mx_hash::find_smallest(int& err, int (*comp_func)(const void* one, const void* two))
{
    const void *smallest = NULL, *temp;
    uint32 i;

    for (i = 0; i < size; i++) {
        if (table[i].usage != unused) {
            if (smallest == NULL) {
                smallest = table[i].item;
            } else {
                if (comp_func(table[i].item, smallest) < 0) {
                    smallest = table[i].item;
                }
            }
        }

        // check for sub-table
        if (table[i].continuation != NULL) {
            temp = table[i].continuation->find_smallest(err, comp_func);
            if (temp != NULL) {
                if (smallest == NULL) {
                    smallest = temp;
                } else {
                    if (comp_func(temp, smallest) < 0) {
                        smallest = temp;
                    }
                }
            }
        }
    }
    err = MX_ERROR_OK;
    return smallest;
}

const void* mx_hash::find_largest(int& err, int (*comp_func)(const void* one, const void* two))
{
    const void *largest = NULL, *temp;
    uint32 i;

    for (i = 0; i < size; i++) {
        if (table[i].usage != unused) {
            if (largest == NULL) {
                largest = table[i].item;
            } else {
                if (comp_func(table[i].item, largest) > 0) {
                    largest = table[i].item;
                }
            }
        }

        // check for sub-table
        if (table[i].continuation != NULL) {
            temp = table[i].continuation->find_largest(err, comp_func);
            if (temp != NULL) {
                if (largest == NULL) {
                    largest = temp;
                } else {
                    if (comp_func(temp, largest) > 0) {
                        largest = temp;
                    }
                }
            }
        }
    }
    err = MX_ERROR_OK;
    return largest;
}

void mx_hash::iterate_keys(int& err, void (*iter_func)(int& err, key_t key, const void* item))
{
    uint32 i;

    for (i = 0; i < size; i++) {
        if (table[i].usage != unused) {
            iter_func(err, table[i].key, table[i].item);
            MX_ERROR_CHECK(err);
        }
        if (table[i].continuation != NULL) {
            table[i].continuation->iterate_keys(err, iter_func);
            MX_ERROR_CHECK(err);
        }
    }
    err = MX_ERROR_OK;
    return;
abort:;
}

mx_hash::~mx_hash()
{
    uint32 i;

    for (i = 0; i < size; i++) {
        if (table[i].continuation != NULL)
            delete table[i].continuation;
        if (table[i].usage == string_key) {
            delete[] table[i].key.s;
        }
    }
    delete[] table;
}

void mx_hash::clear(int& err)
{
    uint32 i;

    for (i = 0; i < size; i++) {
        if (table[i].continuation != NULL)
            delete table[i].continuation;
        if (table[i].usage == string_key) {
            delete[] table[i].key.s;
        }
        table[i].usage = unused;
    }
    err = MX_ERROR_OK;
}

void mx_hash::iterate_start(int& err)
{
    uint32 i;

    for (i = 0; i < size; i++) {
        if (table[i].continuation != NULL) {
            table[i].continuation->iterate_start(err);
            MX_ERROR_CHECK(err);
        }
    }

    current_index = 0;
    err = MX_ERROR_OK;
abort:
    return;
}

const void* mx_hash::iterate_next_key_data(int& err, void* key)
{
    const void* an_item;

    err = MX_ERROR_OK;

    if (current_index >= size) {
        return NULL;
    } else {

        if (table[current_index].continuation != NULL) {
            an_item = table[current_index].continuation->iterate_next_key_data(err, key);
            if (an_item != NULL) {
                return an_item;
            }
        }
    }

    if (table[current_index].usage != unused) {
        memcpy(key, &(table[current_index].key), sizeof(key_t));
        return table[current_index++].item;
    } else {
        current_index++;
        return this->iterate_next_key_data(err, key);
    }
}

const void* mx_hash::iterate_next(int& err)
{
    const void* an_item;

    err = MX_ERROR_OK;

    if (current_index >= size) {
        return NULL;
    } else {
        if (table[current_index].continuation != NULL) {
            an_item = table[current_index].continuation->iterate_next(err);
            if (an_item != NULL) {
                return an_item;
            }
        }

        if (table[current_index].usage != unused) {
            return table[current_index++].item;
        } else {
            current_index++;
            return this->iterate_next(err);
        }
    }
}

mx_hash::key_t* mx_hash::iterate_next_key(int& err)
{
    key_t* a_key;

    err = MX_ERROR_OK;

    if (current_index >= size) {
        return NULL;
    } else {

        if (table[current_index].continuation != NULL) {
            a_key = table[current_index].continuation->iterate_next_key(err);
            if (a_key != NULL) {
                return a_key;
            }
        }

        if (table[current_index].usage != unused) {
            return &(table[current_index++].key);
        } else {
            current_index++;
            return this->iterate_next_key(err);
        }
    }
}
