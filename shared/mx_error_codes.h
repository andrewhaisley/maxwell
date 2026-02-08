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
 * MODULE/CLASS : mx_error_codes
 *
 * AUTHOR : Andrew Haisley
 *
 *
 *
 *
 *
 */
#ifndef MX_ERROR_CODES_H
#define MX_ERROR_CODES_H

/* all routines can return these four. MX_ERROR_TRACEBACK doesn't need
 * to be handled explicitly by the calling routine, MX_ERROR_CHECK
 * deals with it.
 */
#define MX_ERROR_OK 0
#define MX_ERROR_TRACEBACK 1
#define MX_ERROR_UNDEFINED 1000000
#define MX_ERROR_ASSERTION_FAILED 1000001
#define MX_ERROR_NOT_IMPLEMENTED 1000002

/* general errors */

/* MX DB client errors */
#define MX_DB_CLIENT_COULDNT_CONNECT 3
#define MX_DB_CLIENT_NO_SUCH_SERVER_HOST 4
#define MX_DB_CLIENT_AUTHENTICATION_FAILURE 5
#define MX_DB_CLIENT_TOO_MANY_USERS 6
#define MX_DB_CLIENT_CANT_OPEN_DOC 7
#define MX_DB_CLIENT_NOT_CONNECTED 8
#define MX_DB_CLIENT_NOT_LOGGED_IN 9
#define MX_DB_CLIENT_FILE_LOCKED 10
#define MX_DB_CLIENT_FILE_ALREADY_OPEN 11

/* MX_DB_OBJECT class errors */
#define MX_DB_OBJECT_LOST_CONNECTION 100
#define MX_DB_OBJECT_OUT_OF_DISK_SPACE 101
#define MX_DB_OBJECT_NO_SUCH_ATTRIBUTE 102
#define MX_DB_OBJECT_NO_SUCH_BLOB 103
#define MX_DB_OBJECT_BLOB_INDEX_RANGE 104
#define MX_DB_OBJECT_BLOB_SLICE_SIZE 105

#define MX_DB_ATTRIBUTE_NAME_TOO_LONG 150
#define MX_DB_ATTRIBUTE_TOO_BIG 151

/* MX_DOCUMENT class errors */
#define MX_DOCUMENT_SHEET_INDEX 200
#define MX_DOCUMENT_TOO_MANY_SHEETS 201
#define MX_DOCUMENT_STYLE_INDEX_RANGE 202

/* MX_SHEET class errors */
#define MX_SHEET_AREA_INDEX 300
#define MX_SHEET_NO_SUCH_AREA 301
#define MX_SHEET_TOO_MANY_TEXT_AREAS 302
#define MX_SHEET_TOO_MANY_PAGE_AREAS 303
#define MX_SHEET_TOO_MANY_IMAGE_AREAS 304
#define MX_SHEET_TOO_MANY_DIAGRAM_AREAS 305
#define MX_SHEET_TOO_MANY_GRAPH_AREAS 306
#define MX_SHEET_TOO_MANY_TABLE_AREAS 307
#define MX_SHEET_BAD_AREA_CLASS 308

/* MX DB CLIENT CACHE */
#define MX_DB_CLIENT_CACHE_NO_SPACE 400
#define MX_DB_CLIENT_CACHE_NO_SUCH_OID 401
#define MX_DB_CLIENT_CACHE_ATTR_TOO_BIG 402
#define MX_DB_CLIENT_CACHE_FUCKED 403
#define MX_DB_CLIENT_CACHE_NO_SUCH_ATTR 404
#define MX_DB_CLIENT_CACHE_NO_SUCH_BLOB 405
#define MX_DB_CLIENT_CACHE_DUPLICATE_ATTR 406
#define MX_DB_CLIENT_CACHE_DUPLICATE_BLOB 407
#define MX_DB_CLIENT_CACHE_NO_SUCH_DOC 408

/* MX_DISK_HEAP */
#define MX_DISK_HEAP_CANT_OPEN 500
#define MX_DISK_HEAP_DISK_FULL 501
#define MX_DISK_HEAP_FILE_ERROR 502
#define MX_DISK_HEAP_FULL 503

/* MX_HASH */
#define MX_HASH_DUPLICATE 600
#define MX_HASH_NOT_FOUND 601

/* MX_CONFIG */
#define MX_CONFIG_CANT_OPEN 700
#define MX_CONFIG_NO_SUCH_NAME 701
#define MX_CONFIG_IO_ERROR 702

/* MX_FILE */
#define MX_FILE_EEXIST 800
#define MX_FILE_EISDIR 801
#define MX_FILE_ETXTBSY 802
#define MX_FILE_EFAULT 804
#define MX_FILE_EACCES 805
#define MX_FILE_ENAMETOOLONG 806
#define MX_FILE_ENOENT 807
#define MX_FILE_ENOTDIR 808
#define MX_FILE_EMFILE 809
#define MX_FILE_ENFILE 810
#define MX_FILE_ENOMEM 811
#define MX_FILE_EROFS 812
#define MX_FILE_ELOOP 813
#define MX_FILE_ENOSPC 814
#define MX_FILE_SEGMENT_RANGE 815
#define MX_FILE_UNKNOWN 816
#define MX_FILE_NO_SUCH_ENTRY 817
#define MX_FILE_NO_SUCH_ATTR 818
#define MX_FILE_NO_SUCH_BLOB 819
#define MX_FILE_NO_SUCH_OID 820
#define MX_FILE_CANT_GET_PWD 821
#define MX_FILE_CANT_GET_GRP 822
#define MX_FILE_NEEDS_RECOVER 823
#define MX_FILE_UNRECOVERABLE 824
#define MX_FILE_UNIMPORTABLE 825

/* MX_BLOCK */
#define MX_BLOCK_NOT_MX_FILE 900
#define MX_BLOCK_FILE_ERROR 901
#define MX_BLOCK_NO_SUCH_BLOCK 902
#define MX_BLOCK_CHAIN_ERROR 903
#define MX_BLOCK_OUT_OF_BLOCKS 904
#define MX_BLOCK_CONCURRENT_ACCESS 905

/* MX_AREA */
#define MX_AREA_NO_CHAIN 1000
#define MX_AREA_LINK_INDEX 1001
#define MX_AREA_NOT_LINKED 1002
#define MX_AREA_TOO_MANY_COORDS 1003

/* MX_WP_TOOLBAR */
#define MX_WP_TOOLBAR_NO_SUCH_MENU_ITEM 1100

/* MX_WP_MENUBAR */
#define MX_WP_MENUBAR_NO_SUCH_MENU_ITEM 1200

/* MX_TABLE_AREA class errors */
#define MX_TABLE_AREA_ROW_INDEX 1400
#define MX_TABLE_AREA_COLUMN_INDEX 1401

/* MX_APP */
#define MX_APP_ALREADY_OPEN 1500

/* MX_ATTRIBUTE */
#define MX_ATTRIBUTE_BAD_TYPE 1600

/* MX_UNIT */
#define MX_UNIT_INVALID_TYPE 1700
#define MX_UNIT_INVALID_STRING 1701

/* MX_PAINTABLE_OBJECT class errors */
#define MX_PAINTABLE_OBJECT_NO_PAINTER 1800

/* MX_NLIST class errors */
#define MX_NLIST_ROW_RANGE 1900
#define MX_NLIST_NITEMS_RANGE 1901

/* ITERATOR */
#define MX_ITERATOR_NO_COLLECTION 10000
#define MX_ITERATOR_NULL_START_VALUE 10001

/* JPEG */

#define MX_ERROR_NO_JPEG_FILE 10002
#define MX_ERROR_INTERNAL_JPEG_ERROR 10003

/* RASTER */

#define MX_TILE_DICT_ERROR 10004
#define MX_ERROR_NULL_RASTER 10005
#define MX_COMPRESS_ERROR_RASTER 10006

/* TIFF */

#define MX_ERROR_TIFF_ORIENTATION 10006
#define MX_ERROR_TIFF_PLANARCONFIG 10007
#define MX_TIFF_NO_TAG 10008
#define MX_ERROR_INTERNAL_TIFF_ERROR 10009
#define MX_ERROR_TIFF_READ 10010
#define MX_TIFF_OPEN_FAILED 10011

/* PNG errors */
#define MX_ERROR_NO_PNG_FILE 10040
#define MX_ERROR_INTERNAL_PNG_ERROR 10041

/* XBM errors */
#define MX_ERROR_NO_XBM_FILE 10045
#define MX_ERROR_INTERNAL_XBM_ERROR 10046

/* RTF */

#define MX_INTERNAL_RTF_ERROR 10012
#define MX_ERROR_RTF_BUFFER_OVERFLOW 10013
#define MX_ERROR_RTF_FILE_ERROR 10014

/* IMPORTER errors */

#define MX_IMPORTER_BAD_FILE_TYPE 10015

/* USER errors */

#define MX_USER_CANCEL 10016

/* Transfrom raster errors */

#define MX_RASTER_USED 10017
#define MX_RASTER_NOT_EDITABLE 10018

/* RESOURCE ERROR */

#define MX_NO_XTRESOURCE_DATABASE 10020

/* HELP FILE ERRORS */

#define MX_NULL_HELP_DIRECTORY 10025
#define MX_NULL_HELP_FONTINFO 10026
#define MX_FILE_LINK_TOO_LONG 10027
#define MX_LINK_FORMAT_ERROR 10028
#define MX_HELP_BUFFER_TOO_SMALL 10029
#define MX_UNSPECIFIED_HELP_PIXMAP 10030
#define MX_NO_XRESOURCE_HELP_FONT 10031
#define MX_NO_XRESOURCE_HELP_WINDOW 10032
#define MX_NO_XRESOURCE_HELP_PIXMAP 10033

/* MX_LIST */
#define MX_LIST_EMPTY 15000
#define MX_LIST_INDEX 15001
#define MX_LIST_NO_SUCH_POINTER 15002

/* MX_FONT */
#define MX_FONT_STYLE_NOT_AVAILABLE 15500
#define MX_XFT_FAILED 15501

/* MX_FONT_METRICS */
#define MX_FM_UNINITIALISED 15600
#define MX_FREETYPE_INIT_FAILED 15601
#define MX_FM_OUT_OF_RANGE 15602

//#define MX_FM_NO_EEXEC 15601
//#define MX_FM_NO_FAMILY_NAME 15602
//#define MX_FM_NO_FULL_NAME 15603
//#define MX_FM_NO_STYLE 15604
//#define MX_FM_NO_BOUNDING_BOX 15605
//#define MX_FM_BAD_FONT_ENCRYPTION 15606
//#define MX_FM_BAD_CHARSTRING 15607
//#define MX_FM_INVALID_MFM_FILE 15609
//#define MX_FM_BAD_INFO 15610
//#define MX_FM_DECRYPT_ERROR 15611

/* MX_FONT_METRICS_STORE */
#define MX_FMS_BAD_FONTSDIR_FILE 15700

/* MX_DEVICE */
#define MX_DEVICE_CANT_LOAD_X_FONT 16001
#define MX_DEVICE_NO_WINDOW 16002
#define MX_PS_FILE_ERROR 16003
#define MX_SCROLL_SIZE_CHANGE 16004
#define MX_TOO_MANY_PROPAGATE 16005

/* MX_SHEET_LAYOUT */
#define MX_SH_LAY_SHEET_RANGE_ERROR 16100

/* MX_TEXT_AREA */
#define MX_PARAGRAPH_INDEX 16500
#define MX_TEXT_AREA_INVALID_SPLIT_POS 16501

/* MX_WORD */
#define MX_WORD_CHAR_INDEX_RANGE 16600
#define MX_WORD_NOT_CHARACTER_NEXT 16601
#define MX_WORD_NOT_MOD_NEXT 16602
#define MX_WORD_NO_STYLE_SET 16603
#define MX_WORD_OP_NOT_AVAILABLE_ON_SUBCLASS 16604
#define MX_WORD_CANT_SPLIT_WORD 16605

/* MX_STYLE */
#define MX_DUPLICATE_TABSTOP 16700
#define MX_NO_SUCH_TABSTOP 16701

/* MX_GSTLYE */
#define MX_NULL_GSTYLE_NAME 17000

/* QUADING AND LEVEL */
#define MX_INVALID_LEVEL 18000

/* X errors */
#define MX_XDASHES_FAILED 19000
#define MX_XNPOINTS_FAILED 19001

/* PARAGRAPH errors */
#define MX_PARAGRAPH_BAD_WORD_TYPE 20000

/* GEOMETRY errors */
#define MX_SPLINE_TOO_FEW_POINTS 21000

/* POSITION errors */
#define MX_POSITION_RANGE_ERROR 22000

/* buffer class errors */
#define MX_TABLE_BUFFER_SINGLE_CELL 23000
#define MX_TABLE_BUFFER_NOT_BIG_ENOUGH 23001
#define MX_WP_BUFFER_BAD_CLASS 23002

/* CURSOR class errors */
#define MX_CURSOR_NO_SELECTION 23003
#define MX_CURSOR_CANT_SPLIT_TABLE 23004

/* OLE file errors */
#define MX_OLE_BAD_MODE 24000
#define MX_OLE_FAILED_TO_READ_BIGBLOCK 24001
#define MX_OLE_INDEX_RANGE 24002
#define MX_OLE_NOT_OLE_FILE 24003
#define MX_OLE_ROOT_BLOCK_RANGE 24004
#define MX_OLE_STREAM_EOF 24005
#define MX_OLE_STREAM_NOT_FOUND 24006

/* MS WORD file errors */
#define MX_WORD_COMPLEX 25000
#define MX_WORD_ENCRYPTED 25001

/* Diagram editor errors */
#define MX_DG_TOO_MANY_LEVELS 26000
#define MX_NO_GEOM_BOX 26001
#define MX_DG_STATE_ERROR 26002

/*  Metafile errors */

#define MX_ERROR_METAFILE_DATA_ERROR 27000
#define MX_ERROR_METAFILE_FILE_ERROR 27001
#define MX_ERROR_METAFILE_BUFFER_ERROR 27002
#define MX_ERROR_METAFILE_PARAM_ERROR 27003

#define MX_ERROR_FIRST 0
#define MX_ERROR_LAST 0xfffffff

#define MX_ERROR_TEXTS                                                                                \
    { { MX_ERROR_OK, "NO ERROR" },                                                                    \
        { MX_ERROR_TRACEBACK, "TRACEBACK" },                                                          \
        { MX_ERROR_UNDEFINED, "UNDEFINED ERROR" },                                                    \
        { MX_ERROR_ASSERTION_FAILED, "AN ASSERTION FAILED" },                                         \
        { MX_HASH_DUPLICATE, "HASH TABLE DUPLICATE KEY" },                                            \
        { MX_HASH_NOT_FOUND, "HASH TABLE ENTRY NOT FOUND" },                                          \
        { MX_DB_CLIENT_COULDNT_CONNECT, "FAILED TO CONNECT TO SERVER" },                              \
        { MX_DB_CLIENT_NO_SUCH_SERVER_HOST, "COULDNT RESOLVE SERVER HOST NAME" },                     \
        { MX_DB_CLIENT_AUTHENTICATION_FAILURE, "AUTHENTICATION FAILURE" },                            \
        { MX_DB_CLIENT_TOO_MANY_USERS, "TOO MANY USERS LOGGED ON" },                                  \
        { MX_DB_CLIENT_CANT_OPEN_DOC, "CANT OPEN DOCUMENT" },                                         \
        { MX_DB_CLIENT_NOT_CONNECTED, "NOT LOGGED IN TO SERVER" },                                    \
        { MX_DB_CLIENT_NOT_LOGGED_IN, "NOT LOGGED IN TO SERVER" },                                    \
        { MX_DB_OBJECT_LOST_CONNECTION, "LOST CONNECTION TO SERVER" },                                \
        { MX_DB_OBJECT_OUT_OF_DISK_SPACE, "OUT OF DISK SPACE" },                                      \
        { MX_DB_OBJECT_NO_SUCH_ATTRIBUTE, "NO SUCH ATTRIBUTE" },                                      \
        { MX_DB_OBJECT_NO_SUCH_BLOB, "NO SUCH BLOB" },                                                \
        { MX_DB_OBJECT_BLOB_INDEX_RANGE, "BLOB INDEX OUT OF RANGE" },                                 \
        { MX_DB_OBJECT_BLOB_SLICE_SIZE, "BLOB SLICE TO BIG TO READ" },                                \
        { MX_DB_CLIENT_CACHE_NO_SPACE, "CACHE FULL" },                                                \
        { MX_DB_CLIENT_CACHE_NO_SUCH_OID, "UNKNOWN OBJECT ID" },                                      \
        { MX_DB_CLIENT_CACHE_NO_SUCH_OID, "UNKNOWN DOC ID" },                                         \
        { MX_DB_CLIENT_CACHE_FUCKED, "FATAL UNSPECIFIED CLIENT CACHE ERROR" },                        \
        { MX_DB_CLIENT_CACHE_NO_SUCH_ATTR, "NO SUCH ATTRIBUTE" },                                     \
        { MX_DB_CLIENT_CACHE_NO_SUCH_BLOB, "NO SUCH BLOB" },                                          \
        { MX_DB_CLIENT_CACHE_DUPLICATE_ATTR, "DUPLICATED ATTRIBUTE NAME" },                           \
        { MX_DB_CLIENT_CACHE_DUPLICATE_BLOB, "DUPLICATED BLOB NAME" },                                \
        { MX_DB_CLIENT_CACHE_ATTR_TOO_BIG, "ATTRIBUTE TOO BIG" },                                     \
        { MX_DB_CLIENT_FILE_LOCKED, "FILE LOCKED BY SOMEONE ELSE" },                                  \
        { MX_DB_CLIENT_FILE_ALREADY_OPEN, "FILE ALREADY OPENED BY YOU" },                             \
        { MX_DB_ATTRIBUTE_NAME_TOO_LONG, "ATTRIBUTE NAME TOO LONG" },                                 \
        { MX_DB_ATTRIBUTE_TOO_BIG, "ATTRIBUTE IS TOO BIG" },                                          \
        { MX_DOCUMENT_SHEET_INDEX, "SHEET INDEX OUT OF RANGE" },                                      \
        { MX_DOCUMENT_TOO_MANY_SHEETS, "TOO MANY SHEETS" },                                           \
        { MX_DOCUMENT_STYLE_INDEX_RANGE, "STYLE INDEX OUT OF RANGE" },                                \
        { MX_SHEET_AREA_INDEX, "AREA INDEX OUT OF RANGE" },                                           \
        { MX_SHEET_TOO_MANY_TEXT_AREAS, "TOO MANY TEXT AREAS IN ONE SHEET" },                         \
        { MX_SHEET_TOO_MANY_TABLE_AREAS, "TOO MANY TABLE AREAS IN ONE SHEET" },                       \
        { MX_SHEET_TOO_MANY_PAGE_AREAS, "TOO MANY PAGE AREAS IN ONE SHEET" },                         \
        { MX_SHEET_TOO_MANY_IMAGE_AREAS, "TOO MANY IMAGE AREAS IN ONE SHEET" },                       \
        { MX_SHEET_TOO_MANY_DIAGRAM_AREAS, "TOO MANY DIAGRAM AREAS IN ONE SHEET" },                   \
        { MX_SHEET_TOO_MANY_GRAPH_AREAS, "TOO MANY GRAPH AREAS IN ONE SHEET" },                       \
        { MX_SHEET_NO_SUCH_AREA, "NO SUCH AREA" },                                                    \
        { MX_SHEET_BAD_AREA_CLASS, "BAD CLASS OF AREA" },                                             \
        { MX_FILE_EEXIST, "PATHNAME ALREADY EXISTS" },                                                \
        { MX_FILE_EISDIR, "PATHNAME IS A DIRECTORY" },                                                \
        { MX_FILE_ETXTBSY, "EXECUTABLE IS BUSY" },                                                    \
        { MX_FILE_EFAULT, "PATH POINTS OUTSIDE ADDRESS SPACE" },                                      \
        { MX_FILE_EACCES, "ACCESS DENIED" },                                                          \
        { MX_FILE_ENAMETOOLONG, "PATHNAME TOO LONG" },                                                \
        { MX_FILE_ENOENT, "DIRECTORY MISSING" },                                                      \
        { MX_FILE_ENOTDIR, "DIRECTORY IS FILE" },                                                     \
        { MX_FILE_EMFILE, "MAXIMUM NUMBER OF FILES OPEN" },                                           \
        { MX_FILE_ENFILE, "SYSTEM MAXIMUM OF FILES OPEN" },                                           \
        { MX_FILE_ENOMEM, "NO KERNEL MEMORY" },                                                       \
        { MX_FILE_EROFS, "WRITE PERMISSION DENIED" },                                                 \
        { MX_FILE_ELOOP, "SYMBOLIC LINK GIVES LOOP" },                                                \
        { MX_FILE_ENOSPC, "OUT OF DISK SPACE" },                                                      \
        { MX_FILE_SEGMENT_RANGE, "SEGMENT NUMBER RANGE" },                                            \
        { MX_FILE_UNKNOWN, "UNKNOWN FILE ERROR" },                                                    \
        { MX_FILE_NO_SUCH_ENTRY, "ENTRY NOT FOUND" },                                                 \
        { MX_FILE_NO_SUCH_ATTR, "NO SUCH ATTRIBUTE" },                                                \
        { MX_FILE_NO_SUCH_BLOB, "NO SUCH BLOB" },                                                     \
        { MX_FILE_NO_SUCH_OID, "NO SUCH OBJECT" },                                                    \
        { MX_FILE_CANT_GET_PWD, "CANT GET PASSWORD INFO" },                                           \
        { MX_FILE_CANT_GET_GRP, "CANT GET GROUP INFO" },                                              \
        { MX_FILE_NEEDS_RECOVER, "FILE IS DAMAGED - OPEN WITH RECOVER FLAG TRUE" },                   \
        { MX_FILE_UNRECOVERABLE, "FILE CANT BE RECOVERED" },                                          \
        { MX_FILE_UNIMPORTABLE, "FILE TYPE IS NOT IMPORTABLE" },                                      \
        { MX_BLOCK_NOT_MX_FILE, "NOT A MAXWELL DOCUMENT FILE" },                                      \
        { MX_BLOCK_FILE_ERROR, "FILE ERROR" },                                                        \
        { MX_BLOCK_NO_SUCH_BLOCK, "BLOCK OUT OF RANGE IN FILE" },                                     \
        { MX_BLOCK_CHAIN_ERROR, "FREE BLOCK WASNT FREE" },                                            \
        { MX_BLOCK_OUT_OF_BLOCKS, "NO UNLOCKED BLOCKS LEFT IN CACHE" },                               \
        { MX_BLOCK_CONCURRENT_ACCESS, "A CONCURRENT UPDATE HAS OCCURRED" },                           \
        { MX_ITERATOR_NO_COLLECTION, "ITERATOR HAS NO COLLECTION" },                                  \
        { MX_ITERATOR_NULL_START_VALUE, "ITERATOR GIVEN NULL START VALUE" },                          \
        { MX_LIST_EMPTY, "LIST IS EMPTY" },                                                           \
        { MX_LIST_INDEX, "LIST INDEX OUT OF BOUNDS" },                                                \
        { MX_LIST_NO_SUCH_POINTER, "NO SUCH POINTER IN LIST" },                                       \
        { MX_AREA_NO_CHAIN, "NO MORE AREAS IN CHAIN" },                                               \
        { MX_AREA_LINK_INDEX, "LINK INDEX OUT OF RANGE" },                                            \
        { MX_AREA_NOT_LINKED, "NOT LINKED TO THAT AREA" },                                            \
        { MX_AREA_TOO_MANY_COORDS, "TOO MANY COORDINATES IN AREA OUTLINE" },                          \
        { MX_FONT_STYLE_NOT_AVAILABLE, "STYLE IS NOT AVAILABLE FOR THIS FONT" },                      \
        { MX_XFT_FAILED, "XFT LIBRARY FAILURE" },                                                     \
        { MX_FM_UNINITIALISED, "FONT METRICS OBJECT IS UNINITIALISED" },                              \
        { MX_FREETYPE_INIT_FAILED, "FAILED TO INITIIALISE FREETYPE" },                                \
        { MX_FM_OUT_OF_RANGE, "CHARACTER INDEX FOR METRICS IS OUT OF RANGE" },                        \
        { MX_DEVICE_CANT_LOAD_X_FONT, "CANT LOAD X FONT" },                                           \
        { MX_DEVICE_NO_WINDOW, "X WINDOW HAS NOT BEEN CREATED" },                                     \
        { MX_PS_FILE_ERROR, "FILE ERROR CREATING POSTSCRIPT FILE" },                                  \
        { MX_SCROLL_SIZE_CHANGE, "CANNOT FORCE SIZE CHANGE ON SCROLLABLE FRAME" },                    \
        { MX_TOO_MANY_PROPAGATE, "TOO MANY TARGETS FOR PROPAGATION" },                                \
        { MX_SH_LAY_SHEET_RANGE_ERROR, "SHEET LAYOUT NUMBER OUT OF RANGE" },                          \
        { MX_PARAGRAPH_INDEX, "PARAGRAPH INDEX OUT OF RANGE" },                                       \
        { MX_TEXT_AREA_INVALID_SPLIT_POS, "INVALID SPLIT POSITION FOR A TEXT AREA" },                 \
        { MX_DUPLICATE_TABSTOP, "ATTEMPT TO DUPLICATE A TABSTOP" },                                   \
        { MX_NO_SUCH_TABSTOP, "NO TABSTOP AT THIS POSITION" },                                        \
        { MX_ERROR_NO_JPEG_FILE, "JPEG FILE CANNOT BE OPENED" },                                      \
        { MX_ERROR_INTERNAL_JPEG_ERROR, "ERROR IN PROCESSING JPEG FILE" },                            \
        { MX_TILE_DICT_ERROR, "INTERNAL RASTER CACHE ERROR" },                                        \
        { MX_ERROR_NULL_RASTER, "NULL RASTER COPIED OR ATTACHED TO ITERATOR" },                       \
        { MX_COMPRESS_ERROR_RASTER, "ERROR COMPRESSING/DECOMPRESSING RASTER" },                       \
        { MX_ERROR_TIFF_ORIENTATION, "CANNOT PROCESS TIFF FILE WITH THIS ORIENTATION" },              \
        { MX_ERROR_TIFF_PLANARCONFIG, "CANNOT PROCESS TIFF FILE WITH THIS PLANARCONFIG" },            \
        { MX_TIFF_NO_TAG, "TAG CANNOT BE FOUND IN TIFF TILE" },                                       \
        { MX_ERROR_INTERNAL_TIFF_ERROR, "INTERNAL ERROR IN TIFF FILE" },                              \
        { MX_ERROR_TIFF_READ, "INTERNAL ERROR IN READING TIFF FILE" },                                \
        { MX_TIFF_OPEN_FAILED, "FAILED TO OPEN TIFF FILE" },                                          \
        { MX_INTERNAL_RTF_ERROR, "INTERNAL ERROR IN READING RTF FILE" },                              \
        { MX_ERROR_RTF_BUFFER_OVERFLOW, "RTF BUFFER TOO SMALL" },                                     \
        { MX_ERROR_RTF_FILE_ERROR, "ERROR READING RTF FILE" },                                        \
        { MX_IMPORTER_BAD_FILE_TYPE, "BAD FILE TYPE FOR IMPORTER" },                                  \
        { MX_USER_CANCEL, "USER CANCELLED OPERATION" },                                               \
        { MX_NO_XTRESOURCE_DATABASE, "CANNOT GET THE X RESOURCE DATABASE" },                          \
        { MX_HELP_BUFFER_TOO_SMALL, "BUFFER TO HOLD HELP INFO IS TOO SMALL" },                        \
        { MX_NULL_HELP_DIRECTORY, "DIRECTORY FOR HELP FILES NOT DEFINED" },                           \
        { MX_FILE_LINK_TOO_LONG, "FILELINK NAME TOO LONG" },                                          \
        { MX_LINK_FORMAT_ERROR, "HELP LINK FORMAT ERROR" },                                           \
        { MX_NULL_HELP_FONTINFO, "FONT FOR HELP FILES NOT DEFINED" },                                 \
        { MX_UNSPECIFIED_HELP_PIXMAP, "PIXMAP FOR HELP FILES NOT DEFINED" },                          \
        { MX_NO_XRESOURCE_HELP_WINDOW, "NO X RESORUCE DEFINITION OF HELP WINDOW" },                   \
        { MX_NO_XRESOURCE_HELP_FONT, "NO X RESORUCE DEFINITION OF HELP FONT" },                       \
        { MX_NO_XRESOURCE_HELP_PIXMAP, "NO X RESORUCE DEFINITION OF HELP PIXMAP" },                   \
        { MX_RASTER_USED, "THE RASTER HAS ALREADY BEEN USED" },                                       \
        { MX_RASTER_NOT_EDITABLE, "THE RASTER CANNOT BE EDITED" },                                    \
        { MX_WP_TOOLBAR_NO_SUCH_MENU_ITEM, "NO SUCH TOOLBAR ENTRY" },                                 \
        { MX_WP_MENUBAR_NO_SUCH_MENU_ITEM, "NO SUCH MENUBAR ENTRY" },                                 \
        { MX_ERROR_NO_PNG_FILE, "FAILED TO OPEN PNG FILE" },                                          \
        { MX_ERROR_INTERNAL_PNG_ERROR, "INTERNAL ERROR READING PNG FILE" },                           \
        { MX_ERROR_NO_XBM_FILE, "FAILED TO OPEN X BITMAP FILE" },                                     \
        { MX_ERROR_INTERNAL_XBM_ERROR, "INTERNAL ERROR READING X BITMAP FILE" },                      \
        { MX_INVALID_LEVEL, "INVALID DIAGRAM AREA LEVEL" },                                           \
        { MX_NULL_GSTYLE_NAME, "NO NAME SPECIFIED FOR GRAPHICS STYLE" },                              \
        { MX_XDASHES_FAILED, "DASHES TO LONG FOR X SERVER" },                                         \
        { MX_XNPOINTS_FAILED, "TOO MANY XPOINTS FOR X SERVER" },                                      \
        { MX_TABLE_AREA_ROW_INDEX, "ROW INDEX OUT OF RANGE IN TABLE AREA" },                          \
        { MX_TABLE_AREA_COLUMN_INDEX, "COLUMN INDEX OUT OF RANGE IN TABLE AREA" },                    \
        { MX_APP_ALREADY_OPEN, "DOCUMENT ALREADY OPEN" },                                             \
        { MX_ATTRIBUTE_BAD_TYPE, "BAD TYPE FOR ATTRIBUTE" },                                          \
        { MX_UNIT_INVALID_TYPE, "UNRECOGNISED UNIT TYPE IN STRING" },                                 \
        { MX_UNIT_INVALID_STRING, "INVALID UNIT STRING" },                                            \
        { MX_PAINTABLE_OBJECT_NO_PAINTER, "NO PAINTER SET UP ON PAINTABLE OBJECT" },                  \
        { MX_NLIST_ROW_RANGE, "ROW OUT OF RANGE IN NLIST" },                                          \
        { MX_NLIST_NITEMS_RANGE, "NOT ENOUGHT ITEMS IN ROW" },                                        \
        { MX_WORD_CHAR_INDEX_RANGE, "INDEX FOR WORD LETTER OUT OF RANGE" },                           \
        { MX_WORD_NOT_CHARACTER_NEXT, "NEXT ITEM IN COMPLEX WORD IS NOT CHARACTER" },                 \
        { MX_WORD_NOT_MOD_NEXT, "NEXT ITEM IN COMPLEX WORD IS NOT STYLE MOD" },                       \
        { MX_WORD_NO_STYLE_SET, "WORD HAS NO STYLE SET" },                                            \
        { MX_WORD_OP_NOT_AVAILABLE_ON_SUBCLASS, "OPERATION NOT AVAILABLE ON THIS SUBLCASS OF WORD" }, \
        { MX_WORD_CANT_SPLIT_WORD, "CANT SPLIT WORD OF THIS CLASS" },                                 \
        { MX_PARAGRAPH_BAD_WORD_TYPE, "BAD WORD TYPE FOUND WHILST UNSERIALISING PARAGRAPH" },         \
        { MX_SPLINE_TOO_FEW_POINTS, "TOO FEW POINTS GIVEN TO SPLINE CONSTRUCTOR" },                   \
        { MX_POSITION_RANGE_ERROR, "POSITION OUT OF RANGE" },                                         \
        { MX_TABLE_BUFFER_SINGLE_CELL, "CAN'T CUT LESS THAN SINGLE CELL" },                           \
        { MX_TABLE_BUFFER_NOT_BIG_ENOUGH, "TABLE ISN'T BIG ENOUGH TO TAKE PASTE" },                   \
        { MX_WP_BUFFER_BAD_CLASS, "BAD CLASS PASSED INTO MEMBER FUNCTION" },                          \
        { MX_CURSOR_CANT_SPLIT_TABLE, "CAN ONLY INSERT PARAGRAPHS INTO A TABLE" },                    \
        { MX_CURSOR_NO_SELECTION, "NO SELECTION" },                                                   \
        { MX_ERROR_NOT_IMPLEMENTED, "FUNCTION NOT (YET) IMPLEMENTED" },                               \
        { MX_OLE_BAD_MODE, "OLE FILE OPEN MODE BAD - MUST BE r OR w" },                               \
        { MX_OLE_FAILED_TO_READ_BIGBLOCK, "FAILED TO READ AN OLE FILE BLOCK" },                       \
        { MX_OLE_INDEX_RANGE, "TRIED TO READ OUTSIDE OF STREAM SIZE" },                               \
        { MX_OLE_NOT_OLE_FILE, "NOT AN OLE FILE" },                                                   \
        { MX_OLE_ROOT_BLOCK_RANGE, "ASKED FOR BLOCK OUT OF RANGE OF CHAIN" },                         \
        { MX_OLE_STREAM_EOF, "EOF FOR STREAM" },                                                      \
        { MX_OLE_STREAM_NOT_FOUND, "NAMED STREAM NOT FOUND" },                                        \
        { MX_WORD_COMPLEX, "FILE IS QUICKSAVED" },                                                    \
        { MX_WORD_ENCRYPTED, "FILE IS ENCRYPTED" },                                                   \
        { MX_DG_TOO_MANY_LEVELS, "DIAGRAM HAS TOO MANY LEVELS" },                                     \
        { MX_NO_GEOM_BOX, "GEOMETRY HAS NO BOX" },                                                    \
        { MX_DG_STATE_ERROR, "INCONSISTENT STATE IN DIAGRAM EDITOR" },                                \
        { MX_ERROR_METAFILE_DATA_ERROR, "METAFILE DATA ERROR" },                                      \
        { MX_ERROR_METAFILE_FILE_ERROR, "METAFILE FILE ERROR" },                                      \
        { MX_ERROR_METAFILE_BUFFER_ERROR, "METAFILE BUFFER ERROR" },                                  \
        { MX_ERROR_METAFILE_PARAM_ERROR, "METAFILE PARAM ERROR" },                                    \
        { MX_ERROR_LAST, "" } }

#endif
