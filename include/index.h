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

#ifndef __MENU_SCREEN_PAGE_CONTROL_H__
#define __MENU_SCREEN_PAGE_CONTROL_H__

#include <Evas.h>

extern void index_bring_in(Evas_Object *index, int idx);

extern Evas_Object *index_create(Evas_Object *layout, unsigned int count, unsigned int current_idx);
extern Evas_Object *index_update(Evas_Object *layout, Evas_Object *index, unsigned int count);
extern void index_destroy(Evas_Object *index);

extern void index_show(Evas_Object *index);
extern void index_hide(Evas_Object *index);

#endif //__MENU_SCREEN_PAGE_CONTROL_H__

// End of the file
