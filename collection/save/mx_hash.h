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
 *
 */
#ifndef MX_HASH_H
#define MX_HASH_H

#include <mx.h>
#include <stdio.h>

class mx_hash {

public:
    typedef union {
        char* s;
        uint32 i;
    } key_t;

    mx_hash(uint32 init_size);
    mx_hash();

    ~mx_hash();
    void clear(int& err);

    void add(int& err, uint32 key, const void* item);
    void add(int& err, const char* key, const void* item);

    // adds the item if it doesn't exist or replaces an existing item
    void add_or_repl(int& err, uint32 key, const void* item);
    void add_or_repl(int& err, const char* key, const void* item);

    void del(int& err, uint32 key);
    void del_safe(int& err, uint32 key);
    void del(int& err, const char* key);
    void del_safe(int& err, const char* key);

    const void* get(int& err, uint32 key);
    const void* get(int& err, const char* key);

    const void* find_smallest(int& err, int (*comp_func)(const void* one, const void* two));
    const void* find_largest(int& err, int (*comp_func)(const void* one, const void* two));

    uint32 get_num_entries(int& err);

    void iterate_keys(int& err, void (*iter_func)(int& err, key_t key, const void* item));

    // These functions can be used to iterate through inserted objects
    // you call iterate_start and then repeatedly call iterate_next.
    // iterate_next returns NULL when you have completed the iteration.
    void iterate_start(int& err);
    const void* iterate_next(int& err);
    key_t* iterate_next_key(int& err);
    const void* iterate_next_key_data(int& err, void* key);

private:
    void aka_add(int& err, uint32 gen_key, const char* key, const void* item);
    void aka_del_safe(int& err, uint32 gen_key, const char* key);
    const void* aka_get(int& err, uint32 gen_key, const char* key);
    void aka_del(int& err, uint32 gen_key, const char* key);

    uint32 gen_key(const char* key) const;
    uint32 num_entries;
    uint32 num_cont;

    typedef enum { string_key,
        int_key,
        unused } usage_t;

    typedef struct
    {
        usage_t usage;
        key_t key;
        const void* item;
        mx_hash* continuation;
    } entry_t;

    uint32 size;
    entry_t* table;
    void init();

    uint32 current_index; // Used by iterate_start, iterate_next
};

#endif
