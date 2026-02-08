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

#ifndef MX_DB_CLIENTX_H
#define MX_DB_CLIENTX_H

#include <mx.h>
#include <mx_db_object.h>
#include <mx_doc_file.h>

class mx_wp_document;
class mx_document;

bool mx_db_client_login(int& err);
/*
 * PARAMS  :
 *   OUT   - err       what went wrong
 *
 * ACTION  : logs in - may need to connect to server too
 * RETURNS : TRUE if everything worked, FALSE otherwise. On
 *            : failure, err indicates what went wrong:
 *         :
 *           :    MX_DB_CLIENT_COULDNT_CONNECT
 *           :    MX_DB_CLIENT_AUTHENTICATION_FAILURE
 *
 */

mx_wp_document* mx_db_client_open_wp_doc(int& err, char* file_name, bool recover, mx_file_create_type_t t = mx_file_simple_e);
/*
 * PARAMS  :
 *   OUT   - err          what went wrong
 *   IN    - cid          database connection id
 *   IN    - file_name       name of file document
 *   IN    - recover      whether to attempt to recover a damaged document
 *
 * ACTION  : opens the document creating it if necessary.
 * RETURNS : a pointer to the document
 *         :    MX_FILE_NEEDS_RECOVER
 *         :    MX_FILE_UNRECOVERABLE
 *
 */

mx_document* mx_db_client_open_temporary_doc(int& err);
/*
 * PARAMS  :
 *   OUT   - err  what went wrong
 *
 * ACTION  : creates a new document in a temporary file (/tmp e.g.)
 * RETURNS : a pointer to the document. There should be no erros except in
 *         : a disaster.
 *
 */

mx_wp_document* mx_db_client_open_temporary_wp_doc(int& err, const char* page_size = "A4");
/*
 * PARAMS  :
 *   OUT   - err  what went wrong
 *   IN    - page_size
 *
 * ACTION  : creates a new document in a temporary file (/tmp e.g.)
 * RETURNS : a pointer to the document. There should be no erros except in
 *         : a disaster.
 *
 */

void mx_db_client_close_wp_doc(int& err, mx_wp_document* doc);
/*
 * PARAMS  :
 *   OUT   - err  what went wrong
 *   IN    - doc  the document to close
 *
 * ACTION  : closes the document
 *
 * Errors that can be handled by the caller:
 *
 *          MX_MALLOC_FAILED
 *
 *          MX_FILE_EFAULT
 *          MX_FILE_EACCES
 *          MX_FILE_ENOMEM
 *          MX_FILE_ENOSPC
 *          MX_FILE_UNKNOWN
 *
 *
 */

#endif
