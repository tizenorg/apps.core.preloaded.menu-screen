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

#ifndef __MENU_SCREEN_ITEM_H__
#define __MENU_SCREEN_ITEM_H__

#include <Evas.h>

#include "list.h"
#include "util.h"

extern char *item_get_icon(Evas_Object *item);
extern void item_set_icon(Evas_Object *item, char *icon, int sync);

extern char *item_get_name(Evas_Object *item);
extern void item_set_name(Evas_Object *item, char *name, int sync);

extern char *item_get_package(Evas_Object *item);
extern void item_set_package(Evas_Object *item, char *package, int sync);

extern int item_get_removable(Evas_Object *item);
extern void item_set_removable(Evas_Object *item, int removable, int sync);

extern Evas_Object *item_get_page(Evas_Object *edje);
extern void item_set_page(Evas_Object *edje, Evas_Object *page, int sync);

extern char *item_get_desktop(Evas_Object *item);
extern void item_set_desktop(Evas_Object *item, char *name, int sync);

extern void item_enable_delete(Evas_Object *obj);
extern void item_disable_delete(Evas_Object *item);

extern Evas_Object *item_create(Evas_Object *scroller, app_info_t* ai);
extern void item_update(Evas_Object *item, app_info_t *ai);
extern void item_destroy(Evas_Object *obj);

extern void item_launch(Evas_Object *obj);

extern void item_enable_progress(Evas_Object *obj);
extern int item_is_enabled_progress(Evas_Object *obj);
extern void item_update_progress(Evas_Object *obj, int value);
extern void item_disable_progress(Evas_Object *obj);

extern menu_screen_error_e item_is_edje_icon(const char *icon);
extern int item_get_position(Evas_Object *item);

extern void item_mark_dirty(Evas_Object *item);
extern void item_unmark_dirty(Evas_Object *item);
extern int item_is_dirty(Evas_Object *item);

#endif //__MENU_SCREEN_ITEM_H__

// End of a file
