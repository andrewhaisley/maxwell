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
#ifndef MX_LIST_H
#define MX_LIST_H
/*
 * MODULE/CLASS : mx_list
 *
 * AUTHOR : Tom Newton
 *
 *
 *
 *
 *
 *
 */

#include <mx.h>
#include <mx_rtti.h>

/*-------------------------------------------------
 * CLASS: mx_list
 *
 * DESCRIPTION:
 *
 *  This list class is designed to be reasonably space efficient and also
 *  reasonably fast. You can basically insert and delete items at indexs
 *  and there are also methods to iterate throught the list.
 *
 *  It is optimized such that repeated operations at a given index are
 *  really quick and so that if you get iterate up through the list it is
 *  quick too.
 *
 */
class mx_list {
    MX_RTTI(mx_list_class_e)

public:
    mx_list();
    mx_list(const mx_list& list);
    virtual ~mx_list();

    mx_list& operator=(const mx_list& rvalue) { return equate(rvalue); };

    mx_list& equate(const mx_list& rvalue);

    inline int32 get_num_items() const { return num_items; };

    void* get(int& err, int32 index) const;
    void* last();

    void insert(int& err, int32 index, void* item);
    void prepend(void* item);
    void append(void* item);

    // these return a pointer to the object that has just been overwritten
    // or removed.
    void* overwrite(int& err, int32 index, void* item);
    void* remove(int& err, int32 index);

    void* find_smallest(int& err, int (*comp_func)(void* one, void* two));
    void* find_largest(int& err, int (*comp_func)(void* one, void* two));
    inline int32 find_smallest_index(int& err, int (*comp_func)(void* one, void* two));
    inline int32 find_largest_index(int& err, int (*comp_func)(void* one, void* two));

    // returns an index from a pointer if the pointer is in the list.
    int32 find_index(int& err, void* item);

    void iterate(int& err, void (*iter_func)(int& err, uint32 index, void* item));

    // These functions can be used to iterate through inserted objects
    // you call iterate_start and then repeatedly call iterate_next.
    // iterate_next returns NULL when you have completed the iteration.
    // Having said that, it is much easier to use a list iterator object.
    void iterate_start();
    void* iterate_next();
    void* iterate_next_no_increment();
    bool iterate_more();

    // move the last n items from this list into the start of another list
    void move_items_out_end(int& err, int n, mx_list& other);

    // move the first n items of another list into the end of this one
    void move_items_in_end(int& err, int n, mx_list& other);

    // move the first n items from this list into the end of another list
    void move_items_out_start(int& err, int n, mx_list& other);

    // move the last n items of another list into the start of this one
    void move_items_in_start(int& err, int n, mx_list& other);

    struct elem_t {
        // overridden new/delete operators to cache nodes so they don't have to be
        // allocated and deleted
        void* operator new(size_t s);
        void operator delete(void* n);

        void* item;
        elem_t* next;
    };

private:
    int32 num_items;
    mutable int32 current_index;

    mutable elem_t* prev_list_elem;
    mutable elem_t* current_list_elem;
    mutable elem_t* first_list_elem;
    mutable elem_t* last_list_elem;

    void goto_index(int& err, int32 index) const;
    void range_check(int& err, int32& index) const;
    void delete_list(elem_t* list);
    elem_t* copy_list(elem_t* list, mx_list& other);

    // private stuff for moving chunks of lists about

    struct list_segment_t {
        elem_t* first_elem;
        elem_t* last_elem;
        int32 num_elems;
    };

    void remove_all_elems(list_segment_t& seg);
    void remove_last_n_elems(int& err, int32 n, list_segment_t& seg);
    void remove_first_n_elems(int& err, int32 n, list_segment_t& seg);

    void prepend_segment(const list_segment_t& seg);
    void append_segment(const list_segment_t& seg);
};

/*-------------------------------------------------
 * FUNCTION: find_smallest_index
 *
 * DESCRIPTION:
 *
 * You give this function a comparision function for the objects
 * and it will return the index of the smallest one (according to
 * the comparison function. find_largest_index does the opposite.
 *
 */
inline int32 mx_list::
    find_smallest_index(int& err, int (*comp_func)(void* one, void* two))
{
    find_smallest(err, comp_func);
    return current_index;
}

inline int32 mx_list::
    find_largest_index(int& err, int (*comp_func)(void* one, void* two))
{
    find_largest(err, comp_func);
    return current_index;
}

#endif // MX_LIST_H
