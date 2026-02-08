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
#include <mx_ole.h>

mx_ole::mx_ole(int& err, const char* file_name, const char* mode)
{
    int i;

    file = NULL;
    big_depot_nums = NULL;
    bigblock = NULL;
    touch = 0;
    last_start = -1;
    last_index = -1;

    for (i = 0; i < MX_OLE_CACHE_SIZE; i++) {
        bigblock_nums[i] = -2;
        bigblock_touch[i] = 0;
    }

    if (strcmp(mode, "r") == 0) {
        is_write = FALSE;
        file = fopen(file_name, "r");
        if (file == NULL) {
            MX_ERROR_THROW(err, mx_translate_file_error(errno));
        } else {
            read_header(err);
            MX_ERROR_CHECK(err);

            root_pps = find_pps(err, "Root Entry");
            MX_ERROR_CHECK(err);
        }
    } else {
        if (strcmp(mode, "w") == 0) {
            is_write = TRUE;
            file = fopen(file_name, "w");
            if (file == NULL) {
                MX_ERROR_THROW(err, mx_translate_file_error(errno));
            }
        } else {
            MX_ERROR_THROW(err, MX_OLE_BAD_MODE);
        }
    }
abort:;
}

mx_ole::~mx_ole()
{
    if (file != NULL) {
        fclose(file);
    }
    if (big_depot_nums != NULL) {
        delete[] big_depot_nums;
    }
}

void mx_ole::read_bigblock(int& err, int bn)
{
    int i, oldest, oldest_index;

    for (i = 0; i < MX_OLE_CACHE_SIZE; i++) {
        if (bigblock_nums[i] == bn) {
            bigblock = bigblock_cache[i];
            bigblock_touch[i] = touch++;
            return;
        }
    }

    oldest = 200000;
    oldest_index = 0;

    for (i = 0; i < MX_OLE_CACHE_SIZE; i++) {
        if (bigblock_nums[i] == -2) {
            break;
        }
        if (bigblock_touch[i] < oldest) {
            oldest = bigblock_touch[i];
            oldest_index = i;
        }
    }

    if (i == MX_OLE_CACHE_SIZE) {
        i = oldest_index;
    }

    bigblock_nums[i] = bn;
    bigblock_touch[i] = touch++;

    // block numbers start from -1
    fseek(file, (bn + 1) * MX_BIGBLOCK_SIZE, SEEK_SET);

    if (fread(bigblock_cache[i], MX_BIGBLOCK_SIZE, 1, file) != 1) {
        MX_ERROR_THROW(err, MX_OLE_FAILED_TO_READ_BIGBLOCK);
    } else {
        bigblock = bigblock_cache[i];
    }
abort:;
}

void mx_ole::read_header(int& err)
{
    read_bigblock(err, -1);
    MX_ERROR_CHECK(err);

    if (bigblock[0] == 0xd0 && bigblock[1] == 0xcf && bigblock[2] == 0x11 && bigblock[3] == 0xe0 && bigblock[4] == 0xa1 && bigblock[5] == 0xb1 && bigblock[6] == 0x1a && bigblock[7] == 0xe1) {
        num_big_depots = read_int32(err, 0x2c);
        MX_ERROR_CHECK(err);

        root_block = read_int32(err, 0x30);
        MX_ERROR_CHECK(err);

        small_block_depot_num = read_int32(err, 0x3c);
        MX_ERROR_CHECK(err);

        big_depot_nums = new int32[num_big_depots];

        for (int i = 0; i < num_big_depots; i++) {
            big_depot_nums[i] = read_int32(err, 0x4c + i * 4);
            MX_ERROR_CHECK(err);
        }
    } else {
        MX_ERROR_THROW(err, MX_OLE_NOT_OLE_FILE);
    }
abort:;
}

int32 mx_ole::read_int32(int& err, int i)
{
    int32 result;

    if (i < 0 || i > (MX_BIGBLOCK_SIZE - 4)) {
        MX_ERROR_THROW(err, MX_OLE_INDEX_RANGE);
    } else {
        result = bigblock[i];
        result |= bigblock[i + 1] << 8;
        result |= bigblock[i + 2] << 16;
        result |= bigblock[i + 3] << 24;

        return result;
    }

abort:
    return 0;
}

int16 mx_ole::read_int16(int& err, int i)
{
    int16 result;

    if (i < 0 || i > (MX_BIGBLOCK_SIZE - 2)) {
        MX_ERROR_THROW(err, MX_OLE_INDEX_RANGE);
    } else {
        result = bigblock[i];
        result |= bigblock[i + 1] << 8;

        return result;
    }

abort:
    return 0;
}

void mx_ole::read_root_chain(int& err, int index)
{
    read_chain(err, root_block, index);
    MX_ERROR_CHECK(err);

abort:;
}

void mx_ole::read_chain(int& err, int start, int index)
{
    int x, n = 0, i = start;

    if (last_start == start && last_index == index) {
        return;
    }
    last_start = start;
    last_index = index;

    while (TRUE) {
        read_bigblock(err, big_depot_nums[i / MX_BIGDEPOT_ENTRIES]);
        MX_ERROR_CHECK(err);

        x = i;

        i = read_int32(err, (i % MX_BIGDEPOT_ENTRIES) * 4);
        MX_ERROR_CHECK(err);

        if (n == index) {
            read_bigblock(err, x);
            MX_ERROR_CHECK(err);

            return;
        }

        if (i == -2) {
            MX_ERROR_THROW(err, MX_OLE_ROOT_BLOCK_RANGE);
        } else {
            n++;
        }
    }
abort:
    return;
}

int mx_ole::get_num_root_blocks(int& err)
{
    int n = 1, i = root_block;

    while (TRUE) {
        read_bigblock(err, big_depot_nums[i / MX_BIGDEPOT_ENTRIES]);
        MX_ERROR_CHECK(err);

        i = read_int32(err, (i % MX_BIGDEPOT_ENTRIES) * 4);
        MX_ERROR_CHECK(err);

        if (i == -2) {
            return n;
        } else {
            n++;
        }
    }
abort:
    return 0;
}

void mx_ole::read_pps(int& err, int i, mx_ole_pps& pps)
{
    int bi, j, n, offset;

    bi = i / MX_NUM_PPS_PER_BLOCK;
    offset = i % MX_NUM_PPS_PER_BLOCK;
    offset *= MX_PPS_SIZE;

    read_root_chain(err, bi);
    MX_ERROR_CHECK(err);

    for (j = 0; j < MX_PPS_NAME_LEN; j += 2) {
        pps.name[j / 2] = bigblock[j + offset];
    }

    n = read_int16(err, 0x40 + offset);
    MX_ERROR_CHECK(err);

    pps.name[n] = 0;
    pps.type = bigblock[0x42 + offset];

    pps.prev = read_int32(err, 0x44 + offset);
    MX_ERROR_CHECK(err);

    pps.next = read_int32(err, 0x48 + offset);
    MX_ERROR_CHECK(err);

    pps.dir = read_int32(err, 0x4c + offset);
    MX_ERROR_CHECK(err);

    pps.start_block = read_int32(err, 0x74 + offset);
    MX_ERROR_CHECK(err);

    pps.size = read_int32(err, 0x78 + offset);
    MX_ERROR_CHECK(err);

abort:;
}

uint32 mx_ole::size(int& err, const char* name)
{
    mx_ole_pps pps;

    pps = find_pps(err, name);
    MX_ERROR_CHECK(err);

    return pps.size;

abort:
    return 0;
}

mx_ole_pps mx_ole::find_pps(int& err, const char* name, int i)
{
    mx_ole_pps pps;

    while (TRUE) {
        read_pps(err, i, pps);
        MX_ERROR_CHECK(err);

        if (pps.type == 2 || pps.type == 5) {
            if (strcmp(pps.name, name) == 0) {
                return pps;
            }
        }

        if ((pps.type == 1 || pps.type == 5) && pps.dir != -1) {
            pps = find_pps(err, name, pps.dir);
            if (err == MX_ERROR_OK) {
                return pps;
            } else {
                if (err == MX_OLE_STREAM_NOT_FOUND) {
                    MX_ERROR_CLEAR(err);
                } else {
                    MX_ERROR_CHECK(err);
                }
            }
        }

        if (pps.next == -1) {
            MX_ERROR_THROW(err, MX_OLE_STREAM_NOT_FOUND);
        } else {
            i = pps.next;
        }
    }
abort:
    return pps;
}

void mx_ole::open(int& err, const char* name)
{
    mx_ole_pps pps;

    pps = find_pps(err, name);
    MX_ERROR_CHECK(err);

    current_stream = pps;
    current_position = 0;

abort:;
}

int mx_ole::read(int& err, uint8* buf, int n)
{
    int offset, bn, start_bn, end_bn, start_offset, end_offset;
    int num_bytes;

    // is there any data left
    if (current_position >= current_stream.size) {
        MX_ERROR_THROW(err, MX_OLE_STREAM_EOF);
    }

    if (current_stream.size >= 4096) {
        if ((current_position + n) > current_stream.size) {
            n = current_stream.size - current_position;
        }

        start_bn = current_position / MX_BIGBLOCK_SIZE;
        end_bn = (current_position + n) / MX_BIGBLOCK_SIZE;
        start_offset = current_position % MX_BIGBLOCK_SIZE;
        end_offset = (current_position + n) % MX_BIGBLOCK_SIZE;
        offset = 0;

        for (bn = start_bn; bn <= end_bn; bn++) {
            read_chain(err, current_stream.start_block, bn);
            MX_ERROR_CHECK(err);

            if (bn == start_bn) {
                if (end_bn == start_bn) {
                    num_bytes = n;
                } else {
                    num_bytes = MX_BIGBLOCK_SIZE - start_offset;
                }

                memcpy(buf + offset, bigblock + start_offset, num_bytes);
                offset += num_bytes;
            } else {
                if (bn == end_bn) {
                    memcpy(buf + offset, bigblock, end_offset);
                } else {
                    memcpy(buf + offset, bigblock, MX_BIGBLOCK_SIZE);
                    offset += MX_BIGBLOCK_SIZE;
                }
            }
        }

        current_position += n;
        return n;
    } else {
        // data is in smallblocks
        if ((current_position + n) > current_stream.size) {
            n = current_stream.size - current_position;
        }

        start_bn = current_position / MX_SMALLBLOCK_SIZE;
        end_bn = (current_position + n) / MX_SMALLBLOCK_SIZE;
        start_offset = current_position % MX_SMALLBLOCK_SIZE;
        end_offset = (current_position + n) % MX_SMALLBLOCK_SIZE;
        offset = 0;

        for (bn = start_bn; bn <= end_bn; bn++) {
            read_small_chain(err, current_stream.start_block, bn);
            MX_ERROR_CHECK(err);

            if (bn == start_bn) {
                if (end_bn == start_bn) {
                    num_bytes = n;
                } else {
                    num_bytes = MX_SMALLBLOCK_SIZE - start_offset;
                }

                memcpy(buf + offset, smallblock + start_offset, num_bytes);
                offset += num_bytes;
            } else {
                if (bn == end_bn) {
                    memcpy(buf + offset, smallblock, end_offset);
                } else {
                    memcpy(buf + offset, smallblock, MX_SMALLBLOCK_SIZE);
                    offset += MX_SMALLBLOCK_SIZE;
                }
            }
        }

        current_position += n;
        return n;
    }

abort:
    return 0;
}

void mx_ole::read_small_chain(int& err, int start, int index)
{
    int bn;
    int x, n = 0;
    int i = start;

    while (TRUE) {
        // figure out the small block depot block index
        bn = i / MX_BIGDEPOT_ENTRIES;

        // get the small block depot block
        read_chain(err, small_block_depot_num, bn);
        MX_ERROR_CHECK(err);

        x = i;

        // get the next small block number
        i = read_int32(err, (i % MX_BIGDEPOT_ENTRIES) * 4);
        MX_ERROR_CHECK(err);

        if (n == index) {
            read_smallblock(err, x);
            MX_ERROR_CHECK(err);

            return;
        }

        if (i == -2) {
            MX_ERROR_THROW(err, MX_OLE_ROOT_BLOCK_RANGE);
        } else {
            n++;
        }
    }
abort:;
}

void mx_ole::read_smallblock(int& err, int n)
{
    int bbn;

    bbn = n / MX_NUM_SB_PER_BB;

    read_chain(err, root_pps.start_block, bbn);
    MX_ERROR_CHECK(err);

    memcpy(
        smallblock,
        bigblock + ((n % MX_NUM_SB_PER_BB) * MX_SMALLBLOCK_SIZE),
        MX_SMALLBLOCK_SIZE);
abort:;
}

uint32 mx_ole::tell(int& err)
{
    return current_position;
}

void mx_ole::seek(int& err, uint32 pos)
{
    if (pos >= current_stream.size) {
        MX_ERROR_THROW(err, MX_OLE_STREAM_EOF);
    } else {
        current_position = pos;
    }
abort:;
}
