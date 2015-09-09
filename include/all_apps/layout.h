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

#ifndef _MENU_SCREEN_CONTROLBAR_ALL_APPS_H_
#define _MENU_SCREEN_CONTROLBAR_ALL_APPS_H_

#include <Evas.h>

#define PAGE_MAX_APP 16

extern bool all_apps_layout_is_edited(Evas_Object *all_apps);
extern void all_apps_layout_edit(Evas_Object *all_apps);
extern void all_apps_layout_unedit(Evas_Object *all_apps);

extern Evas_Object *all_apps_layout_create(Evas_Object *controlbar, int rotate);
extern void all_apps_layout_destroy(Evas_Object *all_apps);

#endif // _MENU_SCREEN_CONTROLBAR_ALL_APPS_H_

// End of a file
