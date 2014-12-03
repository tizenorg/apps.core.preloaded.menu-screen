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

#ifndef __MENU_SCREEN_H__
#define __MENU_SCREEN_H__

#include <Ecore_Evas.h>
#include <Evas.h>
#include <stdbool.h>

extern int menu_screen_get_root_width(void);
extern int menu_screen_get_root_height(void);
extern Evas *menu_screen_get_evas(void);
extern Evas_Object *menu_screen_get_win(void);
extern Elm_Theme *menu_screen_get_theme(void);
extern bool menu_screen_get_done(void);
extern void menu_screen_set_done(bool is_done);

#endif //__MENU_SCREEN_H__

// End of a file
