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

#include <stdbool.h>
#include <stdlib.h>
#include <Elementary.h>
#include <ail.h>
#include <pkgmgr-info.h>

#include "list.h"
#include "util.h"
#include "all_apps/list.h"



HAPI menu_screen_error_e list_count(app_list *list, int *count)
{
	retv_if(NULL == list, MENU_SCREEN_ERROR_INVALID_PARAMETER);
	retv_if(NULL == list->list, MENU_SCREEN_ERROR_INVALID_PARAMETER);
	retv_if(NULL == count, MENU_SCREEN_ERROR_INVALID_PARAMETER);

	*count = eina_list_count(list->list);

	return MENU_SCREEN_ERROR_OK;
}



HAPI menu_screen_error_e list_first(app_list *list)
{
	retv_if(NULL == list, MENU_SCREEN_ERROR_INVALID_PARAMETER);

	list->cur_idx = 0;

	return MENU_SCREEN_ERROR_OK;
}



HAPI menu_screen_error_e list_next(app_list *list)
{
	int count;

	retv_if(NULL == list, MENU_SCREEN_ERROR_INVALID_PARAMETER);
	retv_if(NULL == list->list, MENU_SCREEN_ERROR_INVALID_PARAMETER);

	count = eina_list_count(list->list);
	if (list->cur_idx + 1 == count) return MENU_SCREEN_ERROR_NO_DATA;

	list->cur_idx ++;

	return MENU_SCREEN_ERROR_OK;
}



HAPI menu_screen_error_e list_is_ended(app_list *list, bool *flag)
{
	int count;

	retv_if(NULL == list, MENU_SCREEN_ERROR_INVALID_PARAMETER);
	retv_if(NULL == list->list, MENU_SCREEN_ERROR_INVALID_PARAMETER);

	count = eina_list_count(list->list);
	*flag = (list->cur_idx == count) ? true : false;

	return MENU_SCREEN_ERROR_OK;
}



HAPI menu_screen_error_e list_get_item(app_list *list, app_list_item **item)
{
	retv_if(NULL == list, MENU_SCREEN_ERROR_INVALID_PARAMETER);
	retv_if(NULL == list->list, MENU_SCREEN_ERROR_INVALID_PARAMETER);

	*item = eina_list_nth(list->list, list->cur_idx);

	return MENU_SCREEN_ERROR_OK;
}



/* FIXME : package -> appid */
HAPI menu_screen_error_e list_get_values(const char *package, app_info_t *ai)
{
	pkgmgrinfo_appinfo_h appinfo_h = NULL;
	pkgmgrinfo_pkginfo_h pkghandle = NULL;
	char *pkgid;
	char *exec;
	char *name;
	char *icon;
	int ret;

	retv_if(NULL == package, MENU_SCREEN_ERROR_FAIL);
	retv_if(NULL == ai, MENU_SCREEN_ERROR_FAIL);
	retv_if(NULL == (ai->package = strdup(package)), MENU_SCREEN_ERROR_FAIL);

	goto_if(0 > pkgmgrinfo_appinfo_get_appinfo(ai->package, &appinfo_h), ERROR);

	goto_if(PMINFO_R_OK != pkgmgrinfo_appinfo_get_pkgid(appinfo_h, &pkgid), ERROR);
	_D("pkgmgrinfo pkgid : %s", pkgid);
	goto_if(PMINFO_R_OK != pkgmgrinfo_appinfo_get_label(appinfo_h, &name), ERROR);
	_D("pkgmgrinfo label : %s", name);
	goto_if(PMINFO_R_OK != pkgmgrinfo_appinfo_is_enabled(appinfo_h, &ai->enabled), ERROR);
	ret = pkgmgrinfo_appinfo_get_icon(appinfo_h, &icon);
	if (ret != PMINFO_R_OK) {
		_D("This package has something strange, icon: %s", icon);
		icon = DEFAULT_ICON;
	}
	_D("pkgmgrinfo icon : %s", icon);

	do {
		goto_if(NULL == pkgid || NULL == (ai->pkgid = strdup(pkgid)), ERROR);
		goto_if(NULL == name || NULL == (ai->name = strdup(name)), ERROR);
		goto_if(NULL == icon || NULL == (ai->icon = strdup(icon)), ERROR);
		goto_if(0 > pkgmgrinfo_pkginfo_get_pkginfo(pkgid, &pkghandle) || !pkghandle, ERROR);
	} while (0);

	goto_if(PMINFO_R_OK != pkgmgrinfo_pkginfo_is_removable(pkghandle, &ai->removable), ERROR);
	_D("This package is removable: %d", ai->removable);
	goto_if(PMINFO_R_OK != pkgmgrinfo_appinfo_is_nodisplay(appinfo_h, &ai->nodisplay), ERROR);
	_D("This package is nodisplay: %d", ai->nodisplay);
	if (ai->nodisplay) goto ERROR;

	pkgmgrinfo_appinfo_destroy_appinfo(appinfo_h);

	return MENU_SCREEN_ERROR_OK;

ERROR:
	if (appinfo_h) pkgmgrinfo_appinfo_destroy_appinfo(appinfo_h);
	list_free_values(ai);
	return MENU_SCREEN_ERROR_FAIL;
}



HAPI void list_free_values(app_info_t *ai)
{
	ret_if(NULL == ai);

	/* Origin field */
	if (ai->pkgid) {
		free(ai->pkgid);
		ai->pkgid = NULL;
	}

	if (ai->package) {
		free(ai->package);
		ai->package = NULL;
	}

	if (ai->exec) {
		free(ai->exec);
		ai->exec = NULL;
	}

	if (ai->name) {
		free(ai->name);
		ai->name = NULL;
	}

	if (ai->icon) {
		free(ai->icon);
		ai->icon = NULL;
	}
}



HAPI menu_screen_error_e list_append_item(app_list *list, app_list_item *item)
{
	retv_if(NULL == list, MENU_SCREEN_ERROR_INVALID_PARAMETER);
	retv_if(NULL == item, MENU_SCREEN_ERROR_INVALID_PARAMETER);

	list->list = eina_list_append(list->list, item);
	retv_if(NULL == list->list, MENU_SCREEN_ERROR_FAIL);

	return MENU_SCREEN_ERROR_OK;
}



HAPI menu_screen_error_e list_remove_item(app_list *list, app_list_item *item)
{
	retv_if(NULL == list, MENU_SCREEN_ERROR_INVALID_PARAMETER);
	retv_if(NULL == list->list, MENU_SCREEN_ERROR_INVALID_PARAMETER);
	retv_if(NULL == item, MENU_SCREEN_ERROR_INVALID_PARAMETER);

	list->list = eina_list_remove(list->list, item);

	return MENU_SCREEN_ERROR_OK;
}



HAPI menu_screen_error_e list_sort(app_list *list, int (*_sort_cb)(const void *d1, const void *d2))
{
	retv_if(NULL == list, MENU_SCREEN_ERROR_INVALID_PARAMETER);

	list->list = eina_list_sort(list->list, eina_list_count(list->list), _sort_cb);
	retv_if(NULL == list->list, MENU_SCREEN_ERROR_FAIL);

	return MENU_SCREEN_ERROR_OK;
}



HAPI app_list_item *list_nth(app_list *parent, unsigned int nth)
{
	Eina_List *list;

	retv_if(NULL == parent, NULL);

	list = parent->list;
	retv_if(NULL == list, NULL);

	return eina_list_nth(list, nth);
}



// End of a file
