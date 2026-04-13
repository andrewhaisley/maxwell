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
 * MODULE/CLASS : mx_db_client
 *
 * AUTHOR : Andrew Haisley
 *
 *
 *
 *
 *
 */

#include <stdio.h>

#include <mx.h>

#include <mx_db_cc.h>
#include <mx_db_client.h>
#include <mx_db_object.h>
#include <mx_wp_doc.h>

static uint32 next_docid = 0;

bool mx_db_client_login(int& err)
/*
 * PARAMS  :
 *   OUT   - err       what went wrong
 *
 * ACTION  : logs in - may need to connect to server too
 * RETURNS : true if everything worked, false otherwise. On
 *            : failure, err indicates what went wrong:
 *         :
 *           :    MX_DB_CLIENT_COULDNT_CONNECT
 *           :    MX_DB_CLIENT_AUTHENTICATION_FAILURE
 *
 */
{
    mx_db_cc_init(err);
    MX_ERROR_CHECK(err);
    return true;
abort:
    return false;
}

mx_wp_document* mx_db_client_open_wp_doc(int& err, char* file_name, bool recover, mx_file_create_type_t t)
/*
 * PARAMS  :
 *   OUT   - err        what went wrong
 *   IN    - cid        database connection id
 *   IN    - file_name  name of file document
 *   IN    - recover    whether to attempt to recover a damaged document
 *
 * ACTION  : opens the document creating it if necessary
 * RETURNS : a pointer to the document
 *
 */
{
    mx_wp_document* res;

    err = MX_ERROR_OK;

    mx_db_cc_open_doc(err, next_docid, file_name, recover, t);
    MX_ERROR_CHECK(err);

    res = new mx_wp_document(file_name, next_docid);

    next_docid++;
    return res;

abort:
    return nullptr;
}

void mx_db_client_close_wp_doc(int& err, mx_wp_document* doc)
/*
 * PARAMS  :
 *   OUT   - err  what went wrong
 *   IN    - doc  the document to close
 *
 * ACTION  : closes the document
 *
 */
{
    int docid;

    docid = doc->get_docid();

    mx_db_cc_close_doc(err, docid);
    MX_ERROR_CHECK(err);

abort:;
}

mx_wp_document* mx_db_client_open_temporary_wp_doc(int& err, const char* page_size)
/*
 * PARAMS  :
 *   OUT   - err  what went wrong
 *
 * ACTION  : creates a new document in a temporary file (/tmp e.g.)
 * RETURNS : a pointer to the document. There should be no erros except in
 *         : a disaster.
 *
 */
{
    mx_wp_document* res;

    const char* file_name = mx_tmpnam(nullptr);

    err = MX_ERROR_OK;

    mx_db_cc_open_doc(err, next_docid, file_name, true);
    MX_ERROR_CHECK(err);

    res = new mx_wp_document(file_name, next_docid, page_size);

    res->commit(err);
    MX_ERROR_CHECK(err);

    res->set_temp_flag(true);

    next_docid++;

    return res;

abort:
    return nullptr;
}

mx_document* mx_db_client_open_temporary_doc(int& err)
/*
 * PARAMS  :
 *   OUT   - err  what went wrong
 *
 * ACTION  : creates a new document in a temporary file (/tmp e.g.)
 * RETURNS : a pointer to the document. There should be no erros except in
 *         : a disaster.
 *
 */
{
    mx_document* res;

    const char* file_name = mx_tmpnam(nullptr);

    err = MX_ERROR_OK;

    mx_db_cc_open_doc(err, next_docid, file_name, true);
    MX_ERROR_CHECK(err);

    res = new mx_document(file_name, next_docid);
    res->set_temp_flag(true);

    next_docid++;

    return res;

abort:
    return nullptr;
}
