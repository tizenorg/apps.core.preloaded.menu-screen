/*
 * MENU-SCREEN
 *
 * Copyright (c) 2009-2014 Samsung Electronics Co., Ltd All Rights Reserved
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
#include <pkgmgr-info.h>
#include <ail.h>

#include "menu_screen.h"
#include "conf.h"
#include "list.h"
#include "all_apps/layout.h"


HAPI int all_apps_list_find_installed_time(const char *id)
{
	pkgmgrinfo_pkginfo_h handle = NULL;
	pkgmgrinfo_appinfo_h appinfo_h = NULL;

	int ret = 0;
	int installed_time = 0;

	goto_if(0> pkgmgrinfo_appinfo_get_appinfo(id, &appinfo_h), ERROR);

	char *pkgid = NULL;
	goto_if(PMINFO_R_OK != pkgmgrinfo_appinfo_get_pkgid(appinfo_h, &pkgid), ERROR);
	goto_if (NULL == pkgid, ERROR);

	goto_if (0>pkgmgrinfo_pkginfo_get_pkginfo(pkgid, &handle), ERROR);

	ret = pkgmgrinfo_pkginfo_get_installed_time(handle, &installed_time);
	goto_if (ret != PMINFO_R_OK, ERROR);

	pkgmgrinfo_appinfo_destroy_appinfo(appinfo_h);
	pkgmgrinfo_pkginfo_destroy_pkginfo(handle);

	return installed_time;

ERROR:
	if (appinfo_h) pkgmgrinfo_appinfo_destroy_appinfo(appinfo_h);
	if (handle) pkgmgrinfo_pkginfo_destroy_pkginfo(handle);
	return 0;
}



static ail_cb_ret_e _all_apps_cb(ail_appinfo_h ai, void *data)
{
	app_list *list = data;
	app_list_item *item;
	app_info_t *item_info;
	int installed_time = 0;
	char *value;

	retv_if(ail_appinfo_get_str(ai, AIL_PROP_PACKAGE_STR, &value) == AIL_ERROR_FAIL, AIL_CB_RET_CANCEL);
	retv_if(NULL == (item = calloc(1, sizeof(struct _app_list_item))), AIL_CB_RET_CANCEL);

	if (NULL == (item->package = strdup(value))) {
		free(item);
		return AIL_CB_RET_CANCEL;
	}

	if (NULL == (item->data = calloc(1, sizeof(app_info_t)))) {
		free(item->package);
		free(item);
		return AIL_CB_RET_CANCEL;
	}

	if (MENU_SCREEN_ERROR_OK != list_get_values(item->package, item->data)) {
		free(item->data);
		free(item->package);
		free(item);
		return AIL_CB_RET_CANCEL;
	}

	item_info = item->data;
	if (NULL == item_info) {
		free(item->data);
		free(item->package);
		free(item);
		return AIL_CB_RET_CANCEL;
	}

	if (item_info->nodisplay || !item_info->enabled) {
		free(item->data);
		free(item->package);
		free(item);
		return AIL_CB_RET_CONTINUE;
	}

	installed_time = all_apps_list_find_installed_time(item->package);
	item->installed_time = installed_time;

	if (0 == item->installed_time) {
		free(item->data);
		free(item->package);
		free(item);
		return AIL_CB_RET_CANCEL;
	}

	if (list_append_item(list, item) != MENU_SCREEN_ERROR_OK) {
		free(item->data);
		free(item->package);
		free(item);
		return AIL_CB_RET_CANCEL;
	}

	_D("[ALL Apps] package=%s", item->package);

	return AIL_CB_RET_CONTINUE;
}



static int _all_apps_installed_time_sort_cb(const void *d1, const void *d2)
{
	app_list_item *item[2];

	retv_if(NULL == d1, 0);
	retv_if(NULL == d2, 0);

	item[0] = (app_list_item *) d1;
	item[1] = (app_list_item *) d2;


	if (item[0]->installed_time > item[1]->installed_time) return (1);
	if (item[0]->installed_time < item[1]->installed_time) return (-1);
	else return (0);
}



static int _all_apps_sort_cb(const void *d1, const void *d2)
{
	app_info_t *ai[2];
	app_list_item *item[2];

	retv_if(NULL == d1, 0);
	retv_if(NULL == d2, 0);

	item[0] = (app_list_item *) d1;
	item[1] = (app_list_item *) d2;

	ai[0] = item[0]->data;
	ai[1] = item[1]->data;

	return strcmp(ai[0]->name, ai[1]->name);
}



HAPI void all_apps_list_destroy(app_list *list)
{
	app_list_item *item;

	ret_if(NULL == list);
	ret_if(NULL == list->list);

	EINA_LIST_FREE(list->list, item) {
		if (NULL == item) break;
		if (item->package) free(item->package);
		if (item->data) {
			list_free_values(item->data);
			free(item->data);
		}
		free(item);
	}

	eina_list_free(list->list);
	free(list);
}



HAPI app_list *all_apps_list_create(void)
{
	int count = 0;
	app_list *list;
	ail_filter_h f;

	list = calloc(1, sizeof(app_list));
	retv_if(NULL == list, NULL);

	if (ail_filter_new(&f) != AIL_ERROR_OK) {
		free(list);
		return NULL;
	}

	if (ail_filter_add_bool(f, AIL_PROP_NODISPLAY_BOOL, false) != AIL_ERROR_OK){
		_E("Failed to add filter");
		ail_filter_destroy(f);
		free(list);
		return NULL;
	}

	ail_filter_list_appinfo_foreach(f, _all_apps_cb, list);
	ail_filter_destroy(f);

	if (MENU_SCREEN_ERROR_FAIL == list_count(list, &count)) {
		_D("Cannot count apps.");
	}

	_D("Total package count = %d", count);

	if (count > (MAX_PAGE_NO * PAGE_MAX_APP)) {

		int i;

		if (MENU_SCREEN_ERROR_FAIL == list_sort(list, _all_apps_installed_time_sort_cb)) {
		_E("Cannot sort installed_time");
		}

		for(i = (MAX_PAGE_NO * PAGE_MAX_APP); i<count; i++) {
			app_list_item *item = list_nth(list, (MAX_PAGE_NO * PAGE_MAX_APP));
			if(!item)
			_E("Cannot remove an item(%s)", item->package);

			_D("%d: %s is removed", i, item->package);

			retv_if(MENU_SCREEN_ERROR_FAIL == list_remove_item(list, item), NULL);
		}
	}

	if (MENU_SCREEN_ERROR_FAIL == list_sort(list, _all_apps_sort_cb)) {
		_D("Cannot sort apps.");
	}

	return list;
}



// End of a file
