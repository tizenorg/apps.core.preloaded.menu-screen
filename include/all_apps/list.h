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

#ifndef __MENU_SCREEN_LIST_ALL_APPS_H__
#define __MENU_SCREEN_LIST_ALL_APPS_H__

extern app_list *all_apps_list_create(void);
extern int all_apps_list_find_installed_time(const char *id);
extern void all_apps_list_destroy(app_list *list);

#endif //__MENU_SCREEN_LIST_ALL_APPS_H__

// End of a file
