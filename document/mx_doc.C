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
 */
#include <filesystem>

#include <mx.h>
#include <mx_db.h>
#include <mx_doc.h>

const uint32* mx_document::get_sheet_oids(int& err)
{
    const mx_attribute* d;

    d = this->get_readonly(err, MX_DOCUMENT_SHEETS);
    if (err == MX_DB_CLIENT_CACHE_NO_SUCH_ATTR) {
        MX_ERROR_CLEAR(err);
        return nullptr;
    } else {
        MX_ERROR_CHECK(err);
        return d->value.ida;
    }
abort:;
    return nullptr;
}

mx_document::~mx_document()
{
    int i;

    for (i = 0; i < MX_DOCUMENT_MAX_STYLES; i++) {
        if (styles[i] != nullptr) {
            delete styles[i];
        }
    }
}

mx_document::mx_document(const char* file, uint32 docid)
    : mx_paintable(docid, MX_DB_OBJECT_DOCUMENT_ID, false)
{
    int i;

    strcpy(file_name, file);
    temporary = false;

    for (i = 0; i < MX_DOCUMENT_MAX_STYLES; i++) {
        styles[i] = nullptr;
    }

    // get a handle on a suitable painter
    painter = mx_document_painter::painter();
}

char* mx_document::get_file_name()
{
    return file_name;
}

std::string mx_document::get_full_file_name()
{
    return std::filesystem::canonical(file_name);
}

void mx_document::commit(int& err)
{
    write_styles(err);
    MX_ERROR_CHECK(err);

    mx_db_cc_commit(err, doc_id);
    MX_ERROR_CHECK(err);
abort:;
}

int mx_document::get_num_sheets(int& err)
{
    int res = this->get_id_array_length(err, MX_DOCUMENT_SHEETS);
    MX_ERROR_CHECK(err);

    return res;
abort:;
    return -1;
}

mx_sheet* mx_document::sheet(int& err, int sheet_index)
{
    int ns;
    mx_sheet* t = nullptr;
    uint32 oid;

    ns = this->get_num_sheets(err);
    MX_ERROR_CHECK(err);

    if (sheet_index < 0 || sheet_index >= ns) {
        MX_ERROR_THROW(err, MX_DOCUMENT_SHEET_INDEX);
    } else {
        oid = this->get_id_array_item(err, MX_DOCUMENT_SHEETS, sheet_index);
        MX_ERROR_CHECK(err);

        t = (mx_sheet*)mx_db_cc_get_obj_from_oid(err, doc_id, oid);
        if (err == MX_DB_CLIENT_CACHE_NO_SUCH_OID) {
            // add it to the cache
            MX_ERROR_CLEAR(err);

            t = new mx_sheet(doc_id, oid);
            if (t->error != MX_ERROR_OK) {
                MX_ERROR_THROW(err, t->error);
            }

            t->set_owner_object(err, this);
            MX_ERROR_CHECK(err);

            return t;
        } else {
            MX_ERROR_CHECK(err);
            return t;
        }
    }

abort:
    return nullptr;
}

void mx_document::delete_sheet_index(int& err, int sheet_index)
{
    int ns;
    mx_sheet* t;

    ns = this->get_num_sheets(err);
    MX_ERROR_CHECK(err);

    if ((sheet_index < 0) || (sheet_index >= ns)) {
        MX_ERROR_THROW(err, MX_DOCUMENT_SHEET_INDEX);
    } else {
        t = sheet(err, sheet_index);
        MX_ERROR_CHECK(err);

        t->delete_areas(err);
        MX_ERROR_CHECK(err);
        delete t;

        del_id_array_item(err, MX_DOCUMENT_SHEETS, sheet_index);
        MX_ERROR_CHECK(err);
    }
abort:;
}

void mx_document::delete_sheet_ref(int& err, mx_sheet* o)
{
    int i, ns;
    mx_db_object* t;
    const uint32* sheet_oids;

    ns = this->get_num_sheets(err);
    MX_ERROR_CHECK(err);

    sheet_oids = this->get_sheet_oids(err);
    MX_ERROR_CHECK(err);

    for (i = 0; i < ns; i++) {
        t = mx_db_cc_get_obj_from_oid(err, doc_id, sheet_oids[i]);
        if (err == MX_DB_CLIENT_CACHE_NO_SUCH_OID) {
            MX_ERROR_CLEAR(err);
        } else {
            MX_ERROR_CHECK(err);
            if (t == o) {
                this->delete_sheet_index(err, i);
                MX_ERROR_CHECK(err);
                return;
            }
        }
    }
abort:;
}

mx_sheet* mx_document::add_sheet(int& err)
{
    mx_sheet* sheet;
    uint32 oid;
    int ns;

    ns = this->get_num_sheets(err);
    MX_ERROR_CHECK(err);

    if (ns >= MX_ATTRIBUTE_MAX_ID_ARRAY_LENGTH) {
        MX_ERROR_THROW(err, MX_DOCUMENT_TOO_MANY_SHEETS);
    }

    sheet = new mx_sheet(doc_id);

    if (sheet->error != MX_ERROR_OK) {
        MX_ERROR_THROW(err, sheet->error);
    }

    sheet->set_owner_object(err, this);
    MX_ERROR_CHECK(err);

    oid = sheet->get_db_id(err);
    MX_ERROR_CHECK(err);

    add_id_array_item(err, MX_DOCUMENT_SHEETS, oid);
    MX_ERROR_CHECK(err);

    return sheet;

abort:
    return nullptr;
}

// sets description, author and version info
void mx_document::set_info(int& err, char* description, char* author, char* version)
{
    mx_db_cc_set_info(err, doc_id, description, author, version);
    MX_ERROR_CHECK(err);

abort:;
}

// gets description
const char* mx_document::get_description()
{
    return mx_db_cc_get_description(doc_id);
}

// gets author
const char* mx_document::get_author()
{
    return mx_db_cc_get_author(doc_id);
}

// gets version
const char* mx_document::get_version()
{
    return mx_db_cc_get_version(doc_id);
}

mx_db_object* mx_document::get_obj(int& err, uint32 oid)
{
    mx_db_object* res = mx_db_cc_get_obj_from_oid(err, doc_id, oid);
    MX_ERROR_CHECK(err);

    return res;
abort:;
    return nullptr;
}

mx_sheet* mx_document::add_sheet_index(int& err, int index)
{
    mx_sheet* sheet;
    uint32 oid;
    int ns, i;

    uint32* sheet_oids;

    ns = this->get_num_sheets(err);
    MX_ERROR_CHECK(err);

    if (ns >= MX_ATTRIBUTE_MAX_ID_ARRAY_LENGTH) {
        MX_ERROR_THROW(err, MX_DOCUMENT_TOO_MANY_SHEETS);
    }

    if (index > ns) {
        MX_ERROR_THROW(err, MX_DOCUMENT_SHEET_INDEX);
    }

    sheet = new mx_sheet(doc_id);

    if (sheet->error != MX_ERROR_OK) {
        MX_ERROR_THROW(err, sheet->error);
    }

    sheet->set_owner_object(err, this);
    MX_ERROR_CHECK(err);

    oid = sheet->get_db_id(err);
    MX_ERROR_CHECK(err);

    sheet_oids = (uint32*)this->get_sheet_oids(err);
    MX_ERROR_CHECK(err);

    for (i = ns; i > index; i--) {
        sheet_oids[i] = sheet_oids[i - 1];
    }

    sheet_oids[index] = oid;

    set_id_array(err, MX_DOCUMENT_SHEETS, sheet_oids, ns + 1);
    MX_ERROR_CHECK(err);

    return sheet;

abort:
    return nullptr;
}

void mx_document::swap_sheets(int& err, int index1, int index2)
{
    int ns;
    uint32 oid1, oid2;

    ns = this->get_num_sheets(err);
    MX_ERROR_CHECK(err);

    if ((index1 < 0) || (index1 >= ns) || (index2 < 0) || (index2 >= ns)) {
        MX_ERROR_THROW(err, MX_DOCUMENT_SHEET_INDEX);
    }

    oid1 = this->get_id_array_item(err, MX_DOCUMENT_SHEETS, index1);
    MX_ERROR_CHECK(err);

    oid2 = this->get_id_array_item(err, MX_DOCUMENT_SHEETS, index2);
    MX_ERROR_CHECK(err);

    set_id_array_item(err, MX_DOCUMENT_SHEETS, index1, oid2);
    MX_ERROR_CHECK(err);

    set_id_array_item(err, MX_DOCUMENT_SHEETS, index2, oid1);
    MX_ERROR_CHECK(err);

abort:;
}

bool mx_document::is_modified()
{
    int err = MX_ERROR_OK;

    // temporarily always return true
    return true;

    bool res = mx_db_cc_doc_modified(err, doc_id);
    if (err != MX_ERROR_OK) {
        MX_ERROR_CLEAR(err);
        return false;
    } else {
        return res;
    }
}

void mx_document::commit_as(int& err, char* s)
{
    write_styles(err);
    MX_ERROR_CHECK(err);

    mx_db_cc_commit_as(err, doc_id, s);
    MX_ERROR_CHECK(err);

    strcpy(file_name, s);
abort:;
}

int mx_document::get_num_styles(int& err)
{
    int res;

    res = get_default_int(err, MX_DOCUMENT_NUM_STYLES, 0);
    MX_ERROR_CHECK(err);

    return res;

abort:
    return -1;
}

mx_paragraph_style* mx_document::get_style(int& err, int index)
{
    int n;

    n = get_num_styles(err);
    MX_ERROR_CHECK(err);

    if (index < 0 || index >= n) {
        MX_ERROR_THROW(err, MX_DOCUMENT_STYLE_INDEX_RANGE);
    }

    if (styles[index] == nullptr) {
        cache_styles(err);
        MX_ERROR_CHECK(err);
    }

    return styles[index];

abort:
    return nullptr;
}

void mx_document::add_style(int& err, mx_paragraph_style* s)
{
    int n;

    n = get_num_styles(err);
    MX_ERROR_CHECK(err);

    if (n == MX_DOCUMENT_MAX_STYLES) {
        MX_ERROR_THROW(err, MX_DOCUMENT_STYLE_INDEX_RANGE);
    }

    styles[n] = new mx_paragraph_style(*s);

    set_int(err, MX_DOCUMENT_NUM_STYLES, n + 1);
    MX_ERROR_CHECK(err);
abort:;
}

void mx_document::add_or_replace_style(int& err, mx_paragraph_style* s)
{
    int named_style_index = this->get_named_style_index(err, s->get_name());
    MX_ERROR_CHECK(err);

    if (named_style_index == -1) {
        this->add_style(err, s);
        MX_ERROR_CHECK(err);
    } else {
        mx_paragraph_style* old_style = styles[named_style_index];
        mx_paragraph_style* new_style = new mx_paragraph_style(*s);

        this->replace_style(err, old_style, new_style);
        MX_ERROR_CHECK(err);

        styles[named_style_index] = new_style;
        delete old_style;
    }

    write_styles(err);
    MX_ERROR_CHECK(err);
abort:;
}

mx_paragraph_style* mx_document::add_or_get_style(int& err,
    mx_paragraph_style* s)
{
    bool need_to_add_style = false;
    mx_paragraph_style* new_ps = this->get_named_style(err, s->get_name());

    if (err != MX_ERROR_OK) {
        MX_ERROR_CLEAR(err);
        need_to_add_style = true;
    } else if (strcasecmp(new_ps->get_name(), s->get_name()) != 0) {
        need_to_add_style = true;
    }

    if (need_to_add_style) {
        this->add_style(err, s);
        MX_ERROR_CHECK(err);

        this->write_styles(err);
        MX_ERROR_CHECK(err);

        new_ps = this->get_named_style(err, s->get_name());
        MX_ERROR_CHECK(err);
    }

    return new_ps;
abort:
    return nullptr;
}

void mx_document::delete_all_styles(int& err)
{
    int i, n = this->get_num_styles(err);
    MX_ERROR_CHECK(err);

    set_int(err, MX_DOCUMENT_NUM_STYLES, 0);
    MX_ERROR_CHECK(err);

    for (i = 0; i < n; i++) {
        delete styles[i];
        styles[i] = nullptr;
    }

    write_styles(err);
    MX_ERROR_CHECK(err);
abort:;
}

void mx_document::set_style_list(int& err, mx_paragraph_style* s[], int num_new_styles)
{
    int i = 0, j = 0, num_old_styles = 0, num_styles_to_delete = 0;
    mx_paragraph_style* ds = nullptr;
    bool *create_style = nullptr, *delete_style = nullptr;
    int* change_style = nullptr;

    num_old_styles = get_num_styles(err);
    MX_ERROR_CHECK(err);

    create_style = new bool[num_new_styles];
    delete_style = new bool[num_old_styles];
    change_style = new int[num_old_styles];

    ds = get_default_style(err);
    MX_ERROR_CHECK(err);

    // find the new styles which correspond to the old styles, and the old
    // styles which need to be deleted, and the new styles which need to be
    // created, and the styles which have been modified

    for (i = 0; i < num_new_styles; i++)
        create_style[i] = true;
    for (j = 0; j < num_old_styles; j++)
        delete_style[j] = true;
    for (j = 0; j < num_old_styles; j++)
        change_style[j] = -1;

    for (i = 0; i < num_new_styles; i++) {
        for (j = 0; j < num_old_styles; j++) {
            if (strcasecmp(styles[j]->get_name(), s[i]->get_name()) == 0) {
                create_style[i] = delete_style[j] = false;
                if (*styles[j] != *s[i]) {
                    change_style[j] = i;
                }
            }
        }
    }

    // replace all the styles which need to be changed with themselves, to
    // tell document objects that they have changed

    for (j = 0; j < num_old_styles; j++) {
        if (change_style[j] != -1) {
            mx_paragraph_style* new_style = s[change_style[j]];
            new_style = new mx_paragraph_style(*new_style);

            this->replace_style(err, styles[j], new_style);
            MX_ERROR_CHECK(err);

            delete styles[j];
            styles[j] = new_style;
        }
    }

    // replace all the styles which are about to be deleted, and delete them

    for (j = 0; j < num_old_styles; j++) {
        if (delete_style[j]) {
            if (styles[j] == ds) {
                mx_printf_warning("You cannot delete the default style");
            } else {
                this->replace_style(err, styles[j], ds);
                MX_ERROR_CHECK(err);
                delete styles[j];
                styles[j] = nullptr;
                num_styles_to_delete++;
            }
        }
    }

    // reshuffle all the styles to get rid of the ones we deleted

    for (j = (num_old_styles - 1); j >= 0; j--) {
        for (i = 0; i < j; i++) {
            if (styles[i] == nullptr) {
                styles[i] = styles[i + 1];
                styles[i + 1] = nullptr;
            }
        }
    }

    // reset the number of styles

    set_int(err, MX_DOCUMENT_NUM_STYLES,
        num_old_styles - num_styles_to_delete);
    MX_ERROR_CHECK(err);

    // add all the new styles

    for (i = 0; i < num_new_styles; i++) {
        if (create_style[i]) {
            add_style(err, s[i]);
            MX_ERROR_CHECK(err);
        }
    }

    // sort and write the styles
    write_styles(err);
    MX_ERROR_CHECK(err);

abort:
    if (create_style)
        delete[] create_style;
    if (delete_style)
        delete[] delete_style;
    if (change_style)
        delete[] change_style;
}

void mx_document::replace_style(int& err, mx_paragraph_style* style_to_replace,
    mx_paragraph_style* replacement_style)
{
    int i = 0, j = 0, num_sheets = 0, num_areas = 0;
    mx_sheet* s = nullptr;
    mx_area* a = nullptr;

    num_sheets = get_num_sheets(err);
    MX_ERROR_CHECK(err);

    for (i = 0; i < num_sheets; i++) {
        s = sheet(err, i);
        MX_ERROR_CHECK(err);

        num_areas = s->get_num_areas();

        for (j = 0; j < num_areas; j++) {
            a = s->get_area(err, j);
            MX_ERROR_CHECK(err);

            a->replace_style(err, style_to_replace, replacement_style);
            MX_ERROR_CHECK(err);
        }
    }
abort:;
}

void mx_document::write_styles(int& err)
{
    int i, n, total;
    uint8 *buffer, *temp;

    sort_styles();

    n = get_num_styles(err);
    MX_ERROR_CHECK(err);

    total = 0;
    for (i = 0; i < n; i++) {
        total += styles[i]->get_serialised_size(err);
        MX_ERROR_CHECK(err);
    }

    if (total == 0) {
        return;
    }

    temp = buffer = new uint8[total];

    for (i = 0; i < n; i++) {
        styles[i]->serialise(err, &buffer);
        MX_ERROR_CHECK(err);
    }

    create_blob(err, MX_DOCUMENT_STYLES);
    if (err == MX_DB_CLIENT_CACHE_DUPLICATE_BLOB) {
        MX_ERROR_CLEAR(err);
    } else {
        MX_ERROR_CHECK(err);
    }

    set_blob_size(err, MX_DOCUMENT_STYLES, total);
    MX_ERROR_CHECK(err);

    set_blob_data(err, MX_DOCUMENT_STYLES, 0, total, temp);
    MX_ERROR_CHECK(err);

    delete[] temp;
abort:;
}

void mx_document::cache_styles(int& err)
{
    int i, n;
    uint8 *buffer, *temp;

    n = get_blob_size(err, MX_DOCUMENT_STYLES);
    if (err == MX_DB_OBJECT_NO_SUCH_BLOB) {
        MX_ERROR_CLEAR(err);
        return;
    }
    MX_ERROR_CHECK(err);

    temp = buffer = new uint8[n];

    get_blob_data(err, MX_DOCUMENT_STYLES, 0, n, buffer);
    MX_ERROR_CHECK(err);

    n = get_num_styles(err);
    MX_ERROR_CHECK(err);

    for (i = 0; i < n; i++) {
        styles[i] = new mx_paragraph_style();

        styles[i]->unserialise(err, &buffer);
        MX_ERROR_CHECK(err);
    }

    delete[] temp;

abort:;
}

void mx_document::sort_styles()
{
    int n, err = MX_ERROR_OK;
    int i, j, smallest_index;
    mx_paragraph_style* temp;

    n = get_num_styles(err);
    MX_ERROR_CHECK(err);

    for (i = 0; i < n; i++) {
        smallest_index = -1;
        for (j = i + 1; j < n; j++) {
            if (strcasecmp(styles[j]->get_name(), styles[i]->get_name()) < 0) {
                smallest_index = j;
            }
        }
        if (smallest_index != -1) {
            temp = styles[i];
            styles[i] = styles[smallest_index];
            styles[smallest_index] = temp;
        }
    }

    return;

abort:
    MX_ERROR_CLEAR(err);
}

mx_paragraph_style* mx_document::get_default_style(int& err)
{
    mx_paragraph_style* result;
    int n, i;

    n = get_num_styles(err);
    MX_ERROR_CHECK(err);

    for (i = 0; i < n; i++) {
        result = get_style(err, i);
        MX_ERROR_CHECK(err);

        if (strcasecmp(result->get_name(), "default") == 0) {
            return (mx_paragraph_style*)result;
        }
    }

    MX_ERROR_THROW(err, MX_ERROR_UNDEFINED);
abort:
    return nullptr;
}

void mx_document::add_default_styles(int& err)
{
}

mx_paragraph_style* mx_document::get_named_style(int& err, const char* name)
{
    mx_paragraph_style* res;

    int i = get_named_style_index(err, name);
    MX_ERROR_CHECK(err);

    if (i == -1) {
        res = mx_document::get_default_style(err);
        MX_ERROR_CHECK(err);
    } else {
        res = styles[i];
    }

    return res;
abort:;
    return nullptr;
}

int mx_document::get_named_style_index(int& err, const char* name)
{
    int n, i;

    n = get_num_styles(err);
    MX_ERROR_CHECK(err);

    if (styles[0] == nullptr) {
        cache_styles(err);
        MX_ERROR_CHECK(err);
    }

    for (i = 0; i < n; i++) {
        if (strcasecmp(name, styles[i]->get_name()) == 0) {
            return i;
        }
    }

abort:
    return -1;
}
