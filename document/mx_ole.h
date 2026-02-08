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
 * MODULE/CLASS : mx_ole
 *
 * AUTHOR : Andrew Haisley
 *
 *
 *
 * Support for reading and writing OLE-2 data stream files. Data can
 * only be read from or written to the primary stream.
 *
 *
 *
 */
#ifndef MX_OLE_H
#define MX_OLE_H

#include <mx.h>

#define MX_BIGBLOCK_SIZE 512
#define MX_SMALLBLOCK_SIZE 64
#define MX_BIGDEPOT_ENTRIES 128
#define MX_PPS_NAME_LEN 64
#define MX_PPS_SIZE 128
#define MX_NUM_PPS_PER_BLOCK (MX_BIGBLOCK_SIZE / MX_PPS_SIZE)
#define MX_OLE_CACHE_SIZE 20
#define MX_NUM_SB_PER_BB (MX_BIGBLOCK_SIZE / MX_SMALLBLOCK_SIZE)

typedef struct
{
    char name[MX_PPS_NAME_LEN + 1];
    uint8 type;
    int32 prev;
    int32 next;
    int32 dir;
    uint32 start_block;
    uint32 size;
} mx_ole_pps;

class mx_ole {
public:
    // mode is either "r" for readonly or "w" for write only.
    mx_ole(int& err, const char* file_name, const char* mode);
    ~mx_ole();

    // find the size of a named stream - WordDocument is an example
    // of a suitable name
    uint32 size(int& err, const char* name);

    // open a named stream
    void open(int& err, const char* name);

    // read from the current position
    int read(int& err, uint8* buf, int n);

    // write to the current position
    void write(int& err, int8* buf, int n);

    // seek to absolute position
    void seek(int& err, uint32 pos);

    // get current absolute position
    uint32 tell(int& err);

private:
    FILE* file;
    bool is_write;

    uint8* bigblock;
    uint8 bigblock_cache[MX_OLE_CACHE_SIZE][MX_BIGBLOCK_SIZE];
    int32 bigblock_nums[MX_OLE_CACHE_SIZE];
    int32 bigblock_touch[MX_OLE_CACHE_SIZE];

    uint8 smallblock[MX_SMALLBLOCK_SIZE];

    int touch;

    int num_big_depots;
    int32* big_depot_nums;
    int root_block;
    int small_block_depot_num;

    void read_bigblock(int& err, int bn);
    void read_header(int& err);

    int32 read_int32(int& err, int i);
    int16 read_int16(int& err, int i);

    mx_ole_pps find_pps(int& err, const char* name, int i = 0);
    void read_chain(int& err, int start, int index);
    void read_small_chain(int& err, int start, int index);
    void read_smallblock(int& err, int n);

    mx_ole_pps current_stream;
    mx_ole_pps root_pps;
    uint32 current_position;

    // get the number of items in the root chain
    int get_num_root_blocks(int& err);

    // reads block root_chain[i]
    void read_root_chain(int& err, int i);

    // read an indexed pps block
    void read_pps(int& err, int i, mx_ole_pps& pps);

    int last_start;
    int last_index;
};

#endif
