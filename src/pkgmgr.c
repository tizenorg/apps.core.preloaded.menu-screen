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

#include <Elementary.h>
#include <package-manager.h>
#include <pkgmgr-info.h>

#include "conf.h"
#include "menu_screen.h"
#include "item.h"
#include "layout.h"
#include "list.h"
#include "page.h"
#include "page_scroller.h"
#include "pkgmgr.h"
#include "util.h"



struct pkgmgr_handler {
	const char *key;
	int (*func)(const char *package, const char *val, void *data);
};

typedef struct {
	char* package;
	char* key;
	char* val;
} pkgmgr_reserve_s;

typedef struct {
	char* package;
	char* status;
} pkgmgr_request_s;

typedef struct {
	char* pkg_id;
	char* app_id;
	Evas_Object *item;
} pkgmgr_install_s;

static struct {
	pkgmgr_client *listen_pc;
	Eina_List *reserve_list;
	Eina_List *request_list;
	Eina_List *item_list;
} pkg_mgr_info = {
	.listen_pc = NULL,
	.reserve_list = NULL,
	.request_list = NULL,
	.item_list = NULL,
};



static menu_screen_error_e _append_request_in_list(const char *package, const char *status)
{
	retv_if(NULL == package, MENU_SCREEN_ERROR_INVALID_PARAMETER);
	retv_if(NULL == status, MENU_SCREEN_ERROR_INVALID_PARAMETER);

	pkgmgr_request_s *rt = calloc(1, sizeof(pkgmgr_request_s));
	retv_if(NULL == rt, MENU_SCREEN_ERROR_FAIL);

	rt->package = strdup(package);
	goto_if(NULL == rt->package, ERROR);

	rt->status = strdup(status);
	goto_if(NULL == rt->status, ERROR);

	pkg_mgr_info.request_list = eina_list_append(pkg_mgr_info.request_list, rt);
	goto_if(NULL == pkg_mgr_info.request_list, ERROR);

	return MENU_SCREEN_ERROR_OK;

ERROR:
	if (rt->status) free(rt->status);
	if (rt->package) free(rt->package);
	if (rt) free(rt);

	return MENU_SCREEN_ERROR_FAIL;
}



static menu_screen_error_e _remove_request_in_list(const char *package)
{
	retv_if(NULL == package, MENU_SCREEN_ERROR_INVALID_PARAMETER);

	if (NULL == pkg_mgr_info.request_list) return MENU_SCREEN_ERROR_OK;

	const Eina_List *l = NULL;
	const Eina_List *ln = NULL;
	pkgmgr_request_s *rt = NULL;
	EINA_LIST_FOREACH_SAFE(pkg_mgr_info.request_list, l, ln, rt) {
		if (!rt) continue;
		if (!rt->package) continue;
		if (strcmp(rt->package, package)) continue;

		pkg_mgr_info.request_list = eina_list_remove(pkg_mgr_info.request_list, rt);
		free(rt->package);
		if (rt->status) free(rt->status);
		free(rt);
		return MENU_SCREEN_ERROR_OK;
	}

	return MENU_SCREEN_ERROR_FAIL;
}



static int _exist_request_in_list(const char *package)
{
	retv_if(NULL == package, 0);

	if (NULL == pkg_mgr_info.request_list) return 0;

	const Eina_List *l = NULL;
	const Eina_List *ln = NULL;
	pkgmgr_request_s *rt = NULL;
	EINA_LIST_FOREACH_SAFE(pkg_mgr_info.request_list, l, ln, rt) {
		if (!rt) continue;
		if (!rt->package) continue;
		if (strcmp(rt->package, package)) continue;
		return 1;
	}

	return 0;
}



static pkgmgr_request_s *_get_request_in_list(const char *package)
{
	retv_if(NULL == package, NULL);

	if (NULL == pkg_mgr_info.request_list) return NULL;

	const Eina_List *l = NULL;
	const Eina_List *ln = NULL;
	pkgmgr_request_s *rt = NULL;
	EINA_LIST_FOREACH_SAFE(pkg_mgr_info.request_list, l, ln, rt) {
		if (!rt) continue;
		if (!rt->package) continue;
		if (strcmp(rt->package, package)) continue;
		return rt;
	}

	return NULL;
}



HAPI menu_screen_error_e pkgmgr_item_list_append_item(const char *pkg_id, const char *app_id, Evas_Object *item)
{
	retv_if(NULL == pkg_id, MENU_SCREEN_ERROR_INVALID_PARAMETER);
	retv_if(NULL == app_id, MENU_SCREEN_ERROR_INVALID_PARAMETER);
	retv_if(NULL == item, MENU_SCREEN_ERROR_INVALID_PARAMETER);

	char *tmp_pkg_id = NULL;
	char *tmp_app_id = NULL;

	pkgmgr_install_s *pi = calloc(1, sizeof(pkgmgr_install_s));
	goto_if(NULL == pi, ERROR);

	tmp_pkg_id = strdup(pkg_id);
	goto_if(NULL == tmp_pkg_id, ERROR);

	tmp_app_id = strdup(app_id);
	goto_if(NULL == tmp_app_id, ERROR);

	pi->pkg_id = tmp_pkg_id;
	pi->app_id = tmp_app_id;
	pi->item = item;

	pkg_mgr_info.item_list = eina_list_append(pkg_mgr_info.item_list, pi);
	goto_if(NULL == pkg_mgr_info.item_list, ERROR);

	return MENU_SCREEN_ERROR_OK;

ERROR:
	if (tmp_app_id) free(tmp_app_id);
	if (tmp_pkg_id) free(tmp_pkg_id);
	free(pi);

	return MENU_SCREEN_ERROR_FAIL;
}



HAPI menu_screen_error_e pkgmgr_item_list_remove_item(const char *pkg_id, const char *app_id, Evas_Object *item)
{
	retv_if(NULL == pkg_id, MENU_SCREEN_ERROR_INVALID_PARAMETER);
	retv_if(NULL == app_id, MENU_SCREEN_ERROR_INVALID_PARAMETER);
	retv_if(NULL == item, MENU_SCREEN_ERROR_INVALID_PARAMETER);

	const Eina_List *l = NULL;
	const Eina_List *ln = NULL;
	pkgmgr_install_s *pi = NULL;
	EINA_LIST_FOREACH_SAFE(pkg_mgr_info.item_list, l, ln, pi) {
		continue_if(NULL == pi);
		continue_if(NULL == pi->pkg_id);
		continue_if(NULL == pi->app_id);
		continue_if(NULL == pi->item);

		if (strcmp(pi->pkg_id, pkg_id)) continue;
		if (strcmp(pi->app_id, app_id)) continue;
		if (pi->item != item) continue;

		pkg_mgr_info.item_list = eina_list_remove(pkg_mgr_info.item_list, pi);

		free(pi->app_id);
		free(pi->pkg_id);
		free(pi);

		return MENU_SCREEN_ERROR_OK;
	}

	return MENU_SCREEN_ERROR_FAIL;
}



HAPI void pkgmgr_item_list_affect_pkgid(const char *pkg_id, Eina_Bool (*_affected_cb)(const char *, Evas_Object *, void *), void *data)
{
	ret_if(NULL == pkg_mgr_info.item_list);
	ret_if(NULL == pkg_id);
	ret_if(NULL == _affected_cb);

	const Eina_List *l;
	const Eina_List *ln;
	pkgmgr_install_s *pi;
	EINA_LIST_FOREACH_SAFE(pkg_mgr_info.item_list, l, ln, pi) {
		continue_if(NULL == pi);
		continue_if(NULL == pi->app_id);
		continue_if(NULL == pi->item);

		if (strcmp(pkg_id, pi->pkg_id)) continue;
		/* It's possible that many items with the same package name are in the install list */
		continue_if(EINA_TRUE != _affected_cb(pi->app_id, pi->item, data));
	}
}



HAPI void pkgmgr_item_list_affect_appid(const char *app_id, Eina_Bool (*_affected_cb)(const char *, Evas_Object *, void *), void *data)
{
	ret_if(NULL == pkg_mgr_info.item_list);
	ret_if(NULL == app_id);
	ret_if(NULL == _affected_cb);

	const Eina_List *l;
	const Eina_List *ln;
	pkgmgr_install_s *pi;
	EINA_LIST_FOREACH_SAFE(pkg_mgr_info.item_list, l, ln, pi) {
		continue_if(NULL == pi);
		continue_if(NULL == pi->app_id);
		continue_if(NULL == pi->item);

		if (strcmp(app_id, pi->app_id)) continue;
		/* It's possible that many items with the same package name are in the install list */
		if (EINA_FALSE == _affected_cb(pi->app_id, pi->item, data)) break;
	}
}



HAPI inline menu_screen_error_e pkgmgr_uninstall(Evas_Object *item)
{
	char *pkgid = NULL;
	char *appid = NULL;
	pkgmgr_client *req_pc = NULL;
	pkgmgrinfo_appinfo_h handle = NULL;
	int ret = MENU_SCREEN_ERROR_OK;

	retv_if(NULL == item, MENU_SCREEN_ERROR_FAIL);

	pkgid = item_get_pkgid(item);
	retv_if(NULL == pkgid, MENU_SCREEN_ERROR_FAIL);

	appid = item_get_package(item);
	retv_if(NULL == appid, MENU_SCREEN_ERROR_FAIL);

	req_pc = pkgmgr_client_new(PC_REQUEST);
	retv_if(NULL == req_pc, MENU_SCREEN_ERROR_FAIL);

	if (PMINFO_R_OK != pkgmgrinfo_appinfo_get_usr_appinfo(appid, getuid(), &handle)) {
		if (PKGMGR_R_OK != pkgmgr_client_free(req_pc)) {
			_E("cannot free pkgmgr_client for request.");
		}
		return MENU_SCREEN_ERROR_FAIL;
	}

	if (PMINFO_R_OK != pkgmgrinfo_appinfo_destroy_appinfo(handle)) {
		_E("cannot destroy the appinfo");
		ret = MENU_SCREEN_ERROR_FAIL;
	}

	_D("Uninstall a package[%s] from an app[%s]", pkgid, appid);
	if (pkgmgr_client_usr_uninstall(req_pc, NULL, pkgid, PM_QUIET, NULL, NULL, getuid()) < 0) {
		_E("cannot uninstall %s.", item_get_package(item));
		ret = MENU_SCREEN_ERROR_FAIL;
	}

	if (PMINFO_R_OK != pkgmgr_client_free(req_pc)) {
		_E("cannot free pkgmgr_client");
		ret = MENU_SCREEN_ERROR_FAIL;
	}

	return ret;
}



static menu_screen_error_e _start_download(const char *package, void *scroller)
{
	_D("Start downloading for the package(%s)", package);
	return MENU_SCREEN_ERROR_OK;
}



static menu_screen_error_e _start_uninstall(const char *package, void *scroller)
{
	_D("Start uninstalling for the package(%s)", package);
	return MENU_SCREEN_ERROR_OK;
}



static menu_screen_error_e _start_update(const char *package, void *scroller)
{
	_D("Start updating for the package(%s)", package);
	return MENU_SCREEN_ERROR_OK;
}



static menu_screen_error_e _start_recover(const char *package, void *scroller)
{
	_D("Start recovering for the package(%s)", package);
	return MENU_SCREEN_ERROR_OK;
}



static menu_screen_error_e _start_install(const char *package, void *scroller)
{
	_D("Start installing for the package(%s)", package);
	return MENU_SCREEN_ERROR_OK;
}



static menu_screen_error_e _start(const char *package, const char *val, void *scroller)
{
	struct start_cb_set {
		const char *name;
		int (*handler)(const char *package, void *scroller);
	} start_cb[] = {
		{
			.name = "download",
			.handler = _start_download,
		},
		{
			.name = "uninstall",
			.handler = _start_uninstall,
		},
		{
			.name = "install",
			.handler = _start_install,
		},
		{
			.name = "update",
			.handler = _start_update,
		},
		{
			.name = "recover",
			.handler = _start_recover,
		},
		{
			.name = NULL,
			.handler = NULL,
		},
	};

	_D("package [%s], val [%s]", package, val);
	retv_if(_exist_request_in_list(package), MENU_SCREEN_ERROR_FAIL);
	retv_if(MENU_SCREEN_ERROR_OK != _append_request_in_list(package, val), MENU_SCREEN_ERROR_FAIL);

	register unsigned int i;
	for (i = 0; start_cb[i].name; i ++) {
		if (strcasecmp(val, start_cb[i].name)) continue;
		break_if(NULL == start_cb[i].handler);
		return start_cb[i].handler(package, scroller);
	}

	_E("Unknown status for starting phase signal'd from package manager");
	return MENU_SCREEN_ERROR_OK;
}



static menu_screen_error_e _icon_path(const char *package, const char *val, void *scroller)
{
	_D("package(%s) with %s", package, val);
	return MENU_SCREEN_ERROR_OK;
}



static menu_screen_error_e _download_percent(const char *package, const char *val, void *scroller)
{
	_D("package(%s) with %s", package, val);
	return MENU_SCREEN_ERROR_OK;
}



static menu_screen_error_e _install_percent(const char *package, const char *val, void *scroller)
{
	_D("package(%s) with %s", package, val);
	if (_exist_request_in_list(package)) return MENU_SCREEN_ERROR_OK;
	retv_if(MENU_SCREEN_ERROR_OK != _append_request_in_list(package, "install"), MENU_SCREEN_ERROR_FAIL);
	return MENU_SCREEN_ERROR_OK;
}



static menu_screen_error_e _error(const char *package, const char *val, void *scroller)
{
	_D("package(%s) with %s", package, val);
	return MENU_SCREEN_ERROR_OK;
}



static int _end_cb(pkgmgrinfo_appinfo_h handle, void *user_data)
{
	retv_if(NULL == handle, -1);
	retv_if(NULL == user_data, -1);

	char *appid = NULL;
	pkgmgrinfo_appinfo_get_appid(handle, &appid);

	Evas_Object *layout = evas_object_data_get(menu_screen_get_win(), "layout");
	retv_if(NULL == layout, MENU_SCREEN_ERROR_FAIL);

	pkgmgr_request_s *rt = user_data;
	if (!strcmp(rt->status, "install")) {
		layout_create_package(layout, appid);
	} else if (!strcmp(rt->status, "update")) {
		layout_update_package(layout, appid);
	} else {
		_E("No routines for this status (%s:%s)", rt->package, rt->status);
	}

	return 0;
}



static Eina_Bool _uninstall_cb(const char *app_id, Evas_Object *item, void *data)
{
	retv_if(NULL == data, EINA_FALSE);

	Evas_Object *layout = data;
	layout_delete_package(layout, app_id);

	return EINA_TRUE;
}



static Eina_Bool _set_removable_cb(const char *app_id, Evas_Object *item, void *data)
{
	retv_if(NULL == item, EINA_FALSE);

	item_set_removable(item, 1, 0);
	elm_object_signal_emit(item, "uninstall,on", "menu");

	return EINA_TRUE;
}



static menu_screen_error_e _end(const char *package, const char *val, void *data)
{
	pkgmgrinfo_pkginfo_h handle = NULL;

	retv_if(!_exist_request_in_list(package), MENU_SCREEN_ERROR_FAIL);

	pkgmgr_request_s *rt = _get_request_in_list(package);
	retv_if(NULL == rt, MENU_SCREEN_ERROR_FAIL);
	if(strcasecmp(val, "ok")) {
		pkgmgr_item_list_affect_pkgid(package, _set_removable_cb, NULL);
		_E("cannot end that package(%s) is %s", rt->package, rt->status);

		goto ERROR;
	}

	_D("Package(%s) : key(%s) - val(%s)", package, rt->status, val);

	/* Criteria : pkgid */
	if (!strcasecmp("uninstall", rt->status)) {
		Evas_Object *layout = evas_object_data_get(menu_screen_get_win(), "layout");
		goto_if(NULL == layout, ERROR);

		pkgmgr_item_list_affect_pkgid(package, _uninstall_cb, layout);
		goto OUT;
	}

	goto_if(PMINFO_R_OK != pkgmgrinfo_pkginfo_get_usr_pkginfo(package, getuid(), &handle), ERROR);

	/* Criteria : appid */
	if (PMINFO_R_OK != pkgmgrinfo_appinfo_get_usr_list(handle, PMINFO_UI_APP, _end_cb, rt, getuid())) {
		goto ERROR;
	}

OUT:
	if (MENU_SCREEN_ERROR_OK != _remove_request_in_list(package))
		_E("cannot remove a request(%s:%s)", rt->package, rt->status);
	if (handle) pkgmgrinfo_pkginfo_destroy_pkginfo(handle);

	return MENU_SCREEN_ERROR_OK;

ERROR:
	if (MENU_SCREEN_ERROR_OK != _remove_request_in_list(package))
		_E("cannot remove a request(%s:%s)", rt->package, rt->status);
	if (handle) pkgmgrinfo_pkginfo_destroy_pkginfo(handle);

	return MENU_SCREEN_ERROR_FAIL;
}



static menu_screen_error_e _change_pkg_name(const char *package, const char *val, void *scroller)
{
	_D("package(%s) with %s", package, val);
	return MENU_SCREEN_ERROR_OK;
}



static struct pkgmgr_handler pkgmgr_cbs[] = {
	{ "start", _start },
	{ "icon_path", _icon_path },
	{ "download_percent", _download_percent },
	{ "command", NULL },
	{ "install_percent", _install_percent },
	{ "error", _error },
	{ "end", _end },
	{ "change_pkg_name", _change_pkg_name },
};



static menu_screen_error_e _pkgmgr_cb(uid_t target_uid, int req_id, const char *pkg_type, const char *package, const char *key, const char *val, const void *pmsg, void *data)
{
	register unsigned int i;

	_D("pkgmgr request [%s:%s] for %s", key, val, package);

	if (BOOTING_STATE_DONE > menu_screen_get_booting_state()) {
		pkgmgr_reserve_list_push_request(package, key, val);
		return MENU_SCREEN_ERROR_OK;
	}

	for (i = 0; i < sizeof(pkgmgr_cbs) / sizeof(struct pkgmgr_handler); i++) {
		if (strcasecmp(pkgmgr_cbs[i].key, key)) continue;
		break_if(!pkgmgr_cbs[i].func);

		if (MENU_SCREEN_ERROR_OK != pkgmgr_cbs[i].func(package, val, NULL)) {
			_E("pkgmgr_cbs[%u].func has errors.", i);
		}

		return MENU_SCREEN_ERROR_OK;
	}

	return MENU_SCREEN_ERROR_FAIL;
}



HAPI menu_screen_error_e pkgmgr_reserve_list_push_request(const char *package, const char *key, const char *val)
{
	retv_if(NULL == package, MENU_SCREEN_ERROR_INVALID_PARAMETER);
	retv_if(NULL == key, MENU_SCREEN_ERROR_INVALID_PARAMETER);
	retv_if(NULL == val, MENU_SCREEN_ERROR_INVALID_PARAMETER);

	char *tmp_package = NULL;
	char *tmp_key = NULL;
	char *tmp_val = NULL;

	pkgmgr_reserve_s *pr = calloc(1, sizeof(pkgmgr_reserve_s));
	retv_if(NULL == pr, MENU_SCREEN_ERROR_FAIL);

	tmp_package = strdup(package);
	goto_if(NULL == tmp_package, ERROR);
	pr->package = tmp_package;

	tmp_key = strdup(key);
	goto_if(NULL == tmp_key, ERROR);
	pr->key = tmp_key;

	tmp_val = strdup(val);
	goto_if(NULL == tmp_val, ERROR);
	pr->val = tmp_val;

	pkg_mgr_info.reserve_list = eina_list_append(pkg_mgr_info.reserve_list, pr);
	goto_if(NULL == pkg_mgr_info.reserve_list, ERROR);

	return MENU_SCREEN_ERROR_OK;

ERROR:
	if (tmp_val) free(tmp_val);
	if (tmp_key) free(tmp_key);
	if (tmp_package) free(tmp_package);
	free(pr);

	return MENU_SCREEN_ERROR_FAIL;
}



HAPI menu_screen_error_e pkgmgr_reserve_list_pop_request(void)
{
	if (!pkg_mgr_info.reserve_list) return MENU_SCREEN_ERROR_NO_DATA;

	pkgmgr_reserve_s *pr = eina_list_nth(pkg_mgr_info.reserve_list, 0);
	if (!pr) return MENU_SCREEN_ERROR_NO_DATA;
	pkg_mgr_info.reserve_list = eina_list_remove(pkg_mgr_info.reserve_list, pr);

	goto_if(MENU_SCREEN_ERROR_OK != _pkgmgr_cb(getuid(), 0, NULL, pr->package, pr->key, pr->val, NULL, NULL), ERROR);

	if (pr->package) free(pr->package);
	if (pr->key) free(pr->key);
	if (pr->val) free(pr->val);
	free(pr);

	return MENU_SCREEN_ERROR_OK;

ERROR:
	if (pr->package) free(pr->package);
	if (pr->key) free(pr->key);
	if (pr->val) free(pr->val);
	free(pr);

	return MENU_SCREEN_ERROR_FAIL;
}



HAPI menu_screen_error_e pkgmgr_init(void)
{
	if (NULL != pkg_mgr_info.listen_pc) {
		return MENU_SCREEN_ERROR_OK;
	}

	pkg_mgr_info.listen_pc = pkgmgr_client_new(PC_LISTENING);
	retv_if(NULL == pkg_mgr_info.listen_pc, MENU_SCREEN_ERROR_FAIL);
	retv_if(pkgmgr_client_listen_status(pkg_mgr_info.listen_pc,
			_pkgmgr_cb, NULL) != PKGMGR_R_OK, MENU_SCREEN_ERROR_FAIL);

	return MENU_SCREEN_ERROR_OK;
}



HAPI void pkgmgr_fini(void)
{
	ret_if(NULL == pkg_mgr_info.listen_pc);
	if (pkgmgr_client_free(pkg_mgr_info.listen_pc) != PKGMGR_R_OK) {
		_E("cannot free pkgmgr_client for listen.");
	}
	pkg_mgr_info.listen_pc = NULL;
}



// End of a file
