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


#ifndef __MENU_SCREEN_LIST_H__
#define __MENU_SCREEN_LIST_H__

#include <Evas.h>
#include <stdbool.h>

#include "util.h"

typedef struct
{
	char *package;
	char *exec;
	char *name;
	char *icon;
	char *desktop;
	bool nodisplay;
	bool x_slp_removable;
	bool x_slp_taskmanage;
	pid_t pid;
	Evas_Object *image;
} app_info_t;

typedef struct _app_list {
	Eina_List *list;
	unsigned int cur_idx;
} app_list;

typedef struct _app_list_item {
	char *package;
	pid_t pid;
	time_t launch_time;
	long long installed_time;
	void *data;
} app_list_item;

extern menu_screen_error_e list_count(app_list *list, int *count);
extern menu_screen_error_e list_first(app_list *list);
extern menu_screen_error_e list_next(app_list *list);
extern menu_screen_error_e list_is_ended(app_list *list, bool *flag);

extern menu_screen_error_e list_get_item(app_list *list, app_list_item **item);
extern menu_screen_error_e list_get_values(const char *package, app_info_t *ai);
extern void list_free_values(app_info_t *ai);

extern menu_screen_error_e list_append_item(app_list *list, app_list_item *item);
extern menu_screen_error_e list_remove_item(app_list *list, app_list_item *item);
extern menu_screen_error_e list_sort(app_list *list, int (*_sort_cb)(const void *d1, const void *d2));

#endif //__MENU_SCREEN_LIST_H__

// End of a file
