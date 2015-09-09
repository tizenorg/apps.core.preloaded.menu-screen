/*
 * MENU-SCREEN
 *
 * Copyright (c) 2009-2015 Samsung Electronics Co., Ltd All Rights Reserved
 *
 * Contact: Jin Yoon <jinny.yoon@samsung.com>
 *          Junkyu Han <junkyu.han@samsung.com>

 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 *
 */

#include <db-util.h>
#include <sqlite3.h>
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include "util.h"
#include "db.h"



#define retv_with_dbmsg_if(expr, val) do { \
	if (expr) { \
		_E("%s", sqlite3_errmsg(db_info.db)); \
		return (val); \
	} \
} while (0)



static struct {
	sqlite3 *db;
} db_info = {
	.db = NULL,
};

struct stmt {
	sqlite3_stmt *stmt;
};



HAPI menu_screen_error_e db_open(const char *db_file)
{
	int ret;

	retv_if(NULL == db_file, MENU_SCREEN_ERROR_INVALID_PARAMETER);
	if (db_info.db) {
		return MENU_SCREEN_ERROR_OK;
	}

	ret = db_util_open(db_file, &db_info.db, DB_UTIL_REGISTER_HOOK_METHOD);
	retv_with_dbmsg_if(ret != SQLITE_OK, MENU_SCREEN_ERROR_FAIL);

	return MENU_SCREEN_ERROR_OK;
}



HAPI stmt_h *db_prepare(const char *query)
{
	int ret;
	stmt_h *handle;

	retv_if(NULL == query, NULL);

	handle = calloc(1, sizeof(stmt_h));
	retv_if(NULL == handle, NULL);

	ret = sqlite3_prepare_v2(db_info.db, query, strlen(query), &(handle->stmt), NULL);
	if (ret != SQLITE_OK) {
		free(handle);
		_E("%s", sqlite3_errmsg(db_info.db));
		return NULL;
	}

	return handle;
}



HAPI menu_screen_error_e db_bind_bool(stmt_h *handle, int idx, bool value)
{
	int ret;

	retv_if(NULL == handle, MENU_SCREEN_ERROR_FAIL);

	ret = sqlite3_bind_int(handle->stmt, idx, (int) value);
	retv_with_dbmsg_if(ret != SQLITE_OK, MENU_SCREEN_ERROR_FAIL);

	return MENU_SCREEN_ERROR_OK;
}



HAPI menu_screen_error_e db_bind_int(stmt_h *handle, int idx, int value)
{
	int ret;

	retv_if(NULL == handle, MENU_SCREEN_ERROR_FAIL);

	ret = sqlite3_bind_int(handle->stmt, idx, value);
	retv_with_dbmsg_if(ret != SQLITE_OK, MENU_SCREEN_ERROR_FAIL);

	return MENU_SCREEN_ERROR_OK;
}



HAPI menu_screen_error_e db_bind_str(stmt_h *handle, int idx, const char *str)
{
	int ret;

	retv_if(NULL == handle, MENU_SCREEN_ERROR_FAIL);

	ret = sqlite3_bind_text(handle->stmt, idx, str, strlen(str), SQLITE_TRANSIENT);
	retv_with_dbmsg_if(ret != SQLITE_OK, MENU_SCREEN_ERROR_FAIL);

	return MENU_SCREEN_ERROR_OK;
}



HAPI menu_screen_error_e db_next(stmt_h *handle)
{
	int ret;

	retv_if(NULL == handle, MENU_SCREEN_ERROR_FAIL);

	ret = sqlite3_step(handle->stmt);
	switch (ret) {
		case SQLITE_ROW:
			return MENU_SCREEN_ERROR_OK;
		case SQLITE_DONE:
			return MENU_SCREEN_ERROR_NO_DATA;
		default:
			retv_with_dbmsg_if(1, MENU_SCREEN_ERROR_FAIL);
	}
}



HAPI bool db_get_bool(stmt_h *handle, int index)
{
	retv_if(NULL == handle, false);
	return (bool) sqlite3_column_int(handle->stmt, index);
}



HAPI int db_get_int(stmt_h *handle, int index)
{
	retv_if(NULL == handle, 0);
	return sqlite3_column_int(handle->stmt, index);
}



HAPI long long db_get_long_long(stmt_h *handle, int index)
{
	retv_if(NULL == handle, 0l);
	return sqlite3_column_int64(handle->stmt, index);
}


HAPI const char *db_get_str(stmt_h *handle, int index)
{
	retv_if(NULL == handle, NULL);
	return (const char *) sqlite3_column_text(handle->stmt, index);
}



HAPI menu_screen_error_e db_reset(stmt_h *handle)
{
	int ret;

	retv_if(NULL == handle, MENU_SCREEN_ERROR_INVALID_PARAMETER);
	retv_if(NULL == handle->stmt, MENU_SCREEN_ERROR_INVALID_PARAMETER);

	ret = sqlite3_reset(handle->stmt);
	retv_with_dbmsg_if(ret != SQLITE_OK, MENU_SCREEN_ERROR_FAIL);

	sqlite3_clear_bindings(handle->stmt);

	return MENU_SCREEN_ERROR_OK;
}



HAPI menu_screen_error_e db_finalize(stmt_h *handle)
{
	int ret;

	retv_if(NULL == handle, MENU_SCREEN_ERROR_INVALID_PARAMETER);
	retv_if(NULL == handle->stmt, MENU_SCREEN_ERROR_INVALID_PARAMETER);

	ret = sqlite3_finalize(handle->stmt);
	if(ret != SQLITE_OK) {
		_E("Cannot finalize handle");
	}
	free(handle);

	return MENU_SCREEN_ERROR_OK;
}



HAPI long long db_last_insert_rowid(void)
{
	retv_if(NULL == db_info.db, -1l);

	long long rowid = sqlite3_last_insert_rowid(db_info.db);

	return rowid;
}



HAPI menu_screen_error_e db_exec(const char *query)
{
	retv_if(NULL == query, MENU_SCREEN_ERROR_INVALID_PARAMETER);
	retv_if(NULL == db_info.db, MENU_SCREEN_ERROR_FAIL);

	stmt_h *handle = db_prepare(query);
	retv_if(NULL == handle, MENU_SCREEN_ERROR_FAIL);

	goto_if(MENU_SCREEN_ERROR_FAIL == db_next(handle), ERROR);
	if (MENU_SCREEN_ERROR_OK != db_finalize(handle)) return MENU_SCREEN_ERROR_FAIL;

	return MENU_SCREEN_ERROR_OK;

ERROR:
	if (handle) db_finalize(handle);
	return MENU_SCREEN_ERROR_FAIL;
}



HAPI void db_close(void)
{
	ret_if(!db_info.db);
	sqlite3_close(db_info.db);
	db_info.db = NULL;
}



HAPI menu_screen_error_e db_begin_transaction(void)
{
	int ret = -1;

	ret = sqlite3_exec(db_info.db, "BEGIN IMMEDIATE TRANSACTION", NULL, NULL, NULL);

	while (SQLITE_BUSY == ret) {
		sleep(1);
		ret = sqlite3_exec(db_info.db, "BEGIN IMMEDIATE TRANSACTION", NULL, NULL, NULL);
	}

	if (SQLITE_OK != ret) {
		_E("sqlite3_exec() Failed(%d)", ret);
		return MENU_SCREEN_ERROR_FAIL;
	}

	return MENU_SCREEN_ERROR_OK;
}



#define MENU_SCREEN_COMMIT_TRY_MAX 3
HAPI menu_screen_error_e db_end_transaction(bool success)
{
	int ret = -1;
	int i = 0;
	char *errmsg = NULL;

	if (success) {
		ret = sqlite3_exec(db_info.db, "COMMIT TRANSACTION", NULL, NULL, &errmsg);
		if (SQLITE_OK != ret) {
			_E("sqlite3_exec(COMMIT) Failed(%d, %s)", ret, errmsg);
			sqlite3_free(errmsg);

			while (SQLITE_BUSY == ret && i < MENU_SCREEN_COMMIT_TRY_MAX) {
				i++;
				sleep(1);
				ret = sqlite3_exec(db_info.db, "COMMIT TRANSACTION", NULL, NULL, NULL);
			}

			if (SQLITE_OK != ret) {
				_E("sqlite3_exec() Failed(%d)", ret);
				ret = sqlite3_exec(db_info.db, "ROLLBACK TRANSACTION", NULL, NULL, NULL);
				if (SQLITE_OK != ret) {
					_E("sqlite3_exec() Failed(%d)", ret);
				}

				return MENU_SCREEN_ERROR_FAIL;
			}
		}
	} else {
		ret = sqlite3_exec(db_info.db, "ROLLBACK TRANSACTION", NULL, NULL, NULL);
		if (SQLITE_OK != ret) {
			_E("sqlite3_exec() Faild(%d)", ret);

			return MENU_SCREEN_ERROR_FAIL;
		}
	}

	return MENU_SCREEN_ERROR_OK;
}



// End of file.
