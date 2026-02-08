#pragma once 

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
 * MODULE/CLASS : mx_document
 *
 * AUTHOR : Andrew Haisley
 *
 *
 *
 * DESCRIPTION:
 *
 * mx_document is the top-level class used by all parts of the
 * Maxwell application to access documents. Database connectivity,
 * access control etc is encapsulated in the mx_document_class
 * (actually, inherited from the mx_db_object class).
 *
 *
 *
 *
 */

#include <string>

#include <mx.h>
#include <mx_db.h>

#include <mx_area.h>
#include <mx_doc_paint.h>
#include <mx_paintable.h>
#include <mx_sheet.h>
#include <mx_style.h>

// names of attributes and blobs
#define MX_DOCUMENT_SHEETS "sheets"
#define MX_DOCUMENT_TYPE "type"
#define MX_DOCUMENT_NUM_STYLES "numstyles"
#define MX_DOCUMENT_STYLES "styles"

#define MX_DOCUMENT_MAX_STYLES 1024

// the primary type of the document
typedef enum { mx_wp = 100,
    mx_dtp,
    mx_image,
    mx_diagram,
    mx_graph,
    mx_signature } mx_primary_type_t;

class mx_document : public mx_paintable {
    MX_RTTI(mx_document_class_e)

public:
    mx_document(const char* file, uint32 docid);
    virtual ~mx_document();

    // get an object from the document by id
    mx_db_object* get_obj(int& err, uint32 oid);

    // what file it's stored in
    char* get_file_name();

    // the full expanded file it's stored in
    std::string get_full_file_name();

    // commit changes to disk - look in mx_error_codes for errors
    void commit(int& err);

    // commit changes to disk into a new file
    void commit_as(int& err, char* file_name);

    // retrieves the number of sheets in the document
    int get_num_sheets(int& err);

    // get a single sheet by index
    mx_sheet* sheet(int& err, int sheet_index);

    // remove a sheet - gets rid of all the areas in the
    // sheet as well
    void delete_sheet_index(int& err, int sheet_index);
    void delete_sheet_ref(int& err, mx_sheet* s);

    // create a new sheet and add it - it's up to you
    // to set all the attributes when you get it back
    mx_sheet* add_sheet(int& err);

    // add a new sheet at the given position
    mx_sheet* add_sheet_index(int& err, int index);

    // swap two sheets around in the document
    void swap_sheets(int& err, int index1, int index2);

    // sets description, author and version info
    void set_info(int& err, char* description, char* author, char* version);

    // gets description
    const char* get_description();

    // gets author
    const char* get_author();

    // gets version
    const char* get_version();

    // has this document been modified since it was opened
    bool is_modified();

    // style handling stuff
    int get_num_styles(int& err);

    mx_paragraph_style* get_style(int& err, int index);

    // gets the names style or, if it doesn't exist, the
    // default one
    mx_paragraph_style* get_named_style(int& err, const char* name);

    // returns the index of the named style, or returns -1 if it cannot be found
    int get_named_style_index(int& err, const char* name);

    void add_style(int& err, mx_paragraph_style* s);

    // if the style of the given name does not exist in this document, it adds
    // it. otherwise the style in the document is replaced. Note the whole
    // document must be reformatted after this to ensure that all parts know
    // about the style change
    void add_or_replace_style(int& err, mx_paragraph_style* s);

    // if the style of the given name does not exist in this document, it adds
    // it. Returns pointer to the named style.
    mx_paragraph_style* add_or_get_style(int& err, mx_paragraph_style* s);

    void delete_style(int& err, char* name);
    void delete_all_styles(int& err);

    void set_style_list(int& err, mx_paragraph_style* s[], int n);

    // update an individual style
    void update_style(int& err, mx_paragraph_style* s);

    mx_paragraph_style* get_default_style(int& err);

    // this depends on the document type
    virtual void add_default_styles(int& err);

    // set/get the temporary document flag
    void set_temp_flag(bool f) { temporary = f; };
    bool get_temp_flag() { return temporary; };

private:
    const uint32* get_sheet_oids(int& err);
    char file_name[MAX_PATH_LEN];

    mx_paragraph_style* styles[MX_DOCUMENT_MAX_STYLES];

    void sort_styles();
    void cache_styles(int& err);

    bool temporary;

protected:
    // iterates through all references to paragraph styles that exist in the
    // document and replaces any occurences of them with a new paragraph
    // style. Used just before a particular style is about to be deleted, or
    // replaced with a new one. document subclasses should override this if
    // they use any document objects which can reference paragraph styles,
    // which may need to be updated.
    virtual void replace_style(int& err, mx_paragraph_style* style_to_replace,
        mx_paragraph_style* replacement_style);

    void write_styles(int& err);
};
