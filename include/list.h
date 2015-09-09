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

#ifndef __MENU_SCREEN_LIST_H__
#define __MENU_SCREEN_LIST_H__

#include <Evas.h>
#include <stdbool.h>

#include "util.h"

typedef struct
{
	char *pkgid;
	char *package; /* appid */
	char *exec;
	char *name;
	char *icon;
	char *desktop;
	bool nodisplay;
	bool enabled;
	bool removable;
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
	int installed_time;
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
extern app_list_item *list_nth(app_list *parent, unsigned int nth);

#endif //__MENU_SCREEN_LIST_H__

// End of a file
