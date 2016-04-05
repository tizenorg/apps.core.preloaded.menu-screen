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

	goto_if(0> pkgmgrinfo_appinfo_get_usr_appinfo(id, getuid(), &appinfo_h), ERROR);

	char *pkgid = NULL;
	goto_if(PMINFO_R_OK != pkgmgrinfo_appinfo_get_pkgid(appinfo_h, &pkgid), ERROR);
	goto_if (NULL == pkgid, ERROR);

	goto_if (0>pkgmgrinfo_pkginfo_get_usr_pkginfo(pkgid, getuid(), &handle), ERROR);

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



static int _all_apps_cb(pkgmgrinfo_appinfo_h handle, void *data)
{
	app_list *list = data;
	app_list_item *item;
	app_info_t *item_info;
	int installed_time = 0;
	char *appid = NULL;

	pkgmgrinfo_appinfo_get_appid(handle, &appid);
	retv_if(NULL == appid, 0);
	retv_if(NULL == (item = calloc(1, sizeof(struct _app_list_item))), 0);

	if (NULL == (item->package = strdup(appid))) {
		free(item);
		return 0;
	}

	if (NULL == (item->data = calloc(1, sizeof(app_info_t)))) {
		free(item->package);
		free(item);
		return 0;
	}

	if (MENU_SCREEN_ERROR_OK != list_get_values(item->package, item->data)) {
		free(item->data);
		free(item->package);
		free(item);
		return 0;
	}

	item_info = item->data;
	if (NULL == item_info) {
		free(item->data);
		free(item->package);
		free(item);
		return 0;
	}

	if (item_info->nodisplay || !item_info->enabled) {
		free(item->data);
		free(item->package);
		free(item);
		return 0;
	}

	installed_time = all_apps_list_find_installed_time(item->package);
	item->installed_time = installed_time;

	if (0 == item->installed_time) {
		free(item->data);
		free(item->package);
		free(item);
		return 0;
	}

	if (list_append_item(list, item) != MENU_SCREEN_ERROR_OK) {
		free(item->data);
		free(item->package);
		free(item);
		return 0;
	}

	_D("[ALL Apps] package=%s", item->package);

	return 1;
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
	pkgmgrinfo_appinfo_filter_h handle = NULL;

	list = calloc(1, sizeof(app_list));
	retv_if(NULL == list, NULL);

	if (PMINFO_R_OK != pkgmgrinfo_appinfo_filter_create(&handle))
	{
		free(list);
		return NULL;
	}
	goto_if(PMINFO_R_OK != pkgmgrinfo_appinfo_filter_add_bool(handle, PMINFO_APPINFO_PROP_APP_NODISPLAY, 0), ERROR);
	goto_if(PMINFO_R_OK != pkgmgrinfo_appinfo_usr_filter_foreach_appinfo(handle, _all_apps_cb, list, getuid()), ERROR);

	pkgmgrinfo_appinfo_filter_destroy(handle);

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

			if (!item) {
				_E("There's something wrong, can't remove the item");
				continue;
			} else if (item->package) {
				_D("%d: %s is removed", i, item->package);
			}

			retv_if(MENU_SCREEN_ERROR_FAIL == list_remove_item(list, item), NULL);
		}
	}

	if (MENU_SCREEN_ERROR_FAIL == list_sort(list, _all_apps_sort_cb)) {
		_D("Cannot sort apps.");
	}

	return list;

ERROR:
	if (handle) pkgmgrinfo_appinfo_filter_destroy(handle);

	return list;
}



// End of a file
