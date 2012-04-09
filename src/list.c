/**
 * Copyright (c) 2000 - 2012 Samsung Electronics Co., Ltd All Rights Reserved
 *
 * This file is part of Menu-screen
 * Written by Jin Yoon <jinny.yoon@samsung.com>, Youngjoo Park <yjoo93.park@samsung.com>
 *
 * PROPRIETARY/CONFIDENTIAL
 *
 * This software is the confidential and proprietary information of SAMSUNG ELECTRONICS ("Confidential Information").
 * You shall not disclose such Confidential Information and shall use it only in accordance with the terms of the license agreement you entered into with SAMSUNG ELECTRONICS.
 * SAMSUNG make no representations or warranties about the suitability of the software, either express or implied, including but not limited to the implied warranties of merchantability, fitness for a particular purpose, or non-infringement.
 * SAMSUNG shall not be liable for any damages suffered by licensee as a result of using, modifying or distributing this software or its derivatives.
 */

#include <stdbool.h>
#include <stdlib.h>
#include <Elementary.h>
#include <ail.h>

#include "list.h"
#include "util.h"
#include "all_apps/list.h"



menu_screen_error_e list_count(app_list *list, int *count)
{
	retv_if(NULL == list, MENU_SCREEN_ERROR_INVALID_PARAMETER);
	retv_if(NULL == list->list, MENU_SCREEN_ERROR_INVALID_PARAMETER);
	retv_if(NULL == count, MENU_SCREEN_ERROR_INVALID_PARAMETER);

	*count = eina_list_count(list->list);

	return MENU_SCREEN_ERROR_OK;
}



menu_screen_error_e list_first(app_list *list)
{
	retv_if(NULL == list, MENU_SCREEN_ERROR_INVALID_PARAMETER);

	list->cur_idx = 0;

	return MENU_SCREEN_ERROR_OK;
}



menu_screen_error_e list_next(app_list *list)
{
	int count;

	retv_if(NULL == list, MENU_SCREEN_ERROR_INVALID_PARAMETER);
	retv_if(NULL == list->list, MENU_SCREEN_ERROR_INVALID_PARAMETER);

	count = eina_list_count(list->list);
	if (list->cur_idx + 1 == count) return MENU_SCREEN_ERROR_NO_DATA;

	list->cur_idx ++;

	return MENU_SCREEN_ERROR_OK;
}



menu_screen_error_e list_is_ended(app_list *list, bool *flag)
{
	int count;

	retv_if(NULL == list, MENU_SCREEN_ERROR_INVALID_PARAMETER);
	retv_if(NULL == list->list, MENU_SCREEN_ERROR_INVALID_PARAMETER);

	count = eina_list_count(list->list);
	*flag = (list->cur_idx == count) ? true : false;

	return MENU_SCREEN_ERROR_OK;
}



menu_screen_error_e list_get_item(app_list *list, app_list_item **item)
{
	retv_if(NULL == list, MENU_SCREEN_ERROR_INVALID_PARAMETER);
	retv_if(NULL == list->list, MENU_SCREEN_ERROR_INVALID_PARAMETER);

	*item = eina_list_nth(list->list, list->cur_idx);

	return MENU_SCREEN_ERROR_OK;
}




menu_screen_error_e list_get_values(const char *package, app_info_t *ai)
{
	ail_appinfo_h appinfo_h;
	char *exec;
	char *name;
	char *icon;
	ail_error_e ret;

	retv_if(NULL == package, MENU_SCREEN_ERROR_FAIL);
	retv_if(NULL == ai, MENU_SCREEN_ERROR_FAIL);
	retv_if(NULL == (ai->package = strdup(package)), MENU_SCREEN_ERROR_FAIL);

	ret = ail_package_get_appinfo(ai->package, &appinfo_h);
	if (AIL_ERROR_OK == ret) {
		do {
			break_if(ail_appinfo_get_str(appinfo_h, AIL_PROP_EXEC_STR, &exec) < 0);
			break_if(ail_appinfo_get_str(appinfo_h, AIL_PROP_NAME_STR, &name) < 0);
			break_if(ail_appinfo_get_str(appinfo_h, AIL_PROP_ICON_STR, &icon) < 0);
			break_if(ail_appinfo_get_bool(appinfo_h, AIL_PROP_NODISPLAY_BOOL, &ai->nodisplay) < 0);
			break_if(ail_appinfo_get_bool(appinfo_h, AIL_PROP_X_SLP_REMOVABLE_BOOL, &ai->x_slp_removable) < 0);
			break_if(ail_appinfo_get_bool(appinfo_h, AIL_PROP_X_SLP_TASKMANAGE_BOOL, &ai->x_slp_taskmanage) < 0);

			break_if(NULL == exec || NULL == (ai->exec = strdup(exec)));
			break_if(NULL == name || NULL == (ai->name = strdup(name)));
			break_if(NULL == icon || NULL == (ai->icon = strdup(icon)));

			ail_package_destroy_appinfo(appinfo_h);

			return MENU_SCREEN_ERROR_OK;
		} while(0);

		ail_package_destroy_appinfo(appinfo_h);
		list_free_values(ai);
		return MENU_SCREEN_ERROR_FAIL;
	} else if (AIL_ERROR_NO_DATA == ret) {
		return MENU_SCREEN_ERROR_OK;
	}

	return MENU_SCREEN_ERROR_FAIL;
}



void list_free_values(app_info_t *ai)
{
	ret_if(NULL == ai);

	/* Origin field */
	if (ai->package) free(ai->package);
	if (ai->exec) free(ai->exec);
	if (ai->name) free(ai->name);
	if (ai->icon) free(ai->icon);
}



menu_screen_error_e list_append_item(app_list *list, app_list_item *item)
{
	retv_if(NULL == list, MENU_SCREEN_ERROR_INVALID_PARAMETER);
	retv_if(NULL == item, MENU_SCREEN_ERROR_INVALID_PARAMETER);

	list->list = eina_list_append(list->list, item);
	retv_if(NULL == list->list, MENU_SCREEN_ERROR_FAIL);

	return MENU_SCREEN_ERROR_OK;
}



menu_screen_error_e list_remove_item(app_list *list, app_list_item *item)
{
	retv_if(NULL == list, MENU_SCREEN_ERROR_INVALID_PARAMETER);
	retv_if(NULL == list->list, MENU_SCREEN_ERROR_INVALID_PARAMETER);
	retv_if(NULL == item, MENU_SCREEN_ERROR_INVALID_PARAMETER);

	list->list = eina_list_remove(list->list, item);

	return MENU_SCREEN_ERROR_OK;
}



menu_screen_error_e list_sort(app_list *list, int (*_sort_cb)(const void *d1, const void *d2))
{
	retv_if(NULL == list, MENU_SCREEN_ERROR_INVALID_PARAMETER);

	list->list = eina_list_sort(list->list, eina_list_count(list->list), _sort_cb);
	retv_if(NULL == list->list, MENU_SCREEN_ERROR_FAIL);

	return MENU_SCREEN_ERROR_OK;
}



// End of a file
