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



#include <Elementary.h>
#include <string.h>
#include <aul.h>
#include <stdbool.h>

#include "menu_screen.h"
#include "conf.h"
#include "item.h"
#include "item_event.h"
#include "mapbuf.h"
#include "mouse.h"
#include "page.h"
#include "page_scroller.h"
#include "util.h"
#include "all_apps/layout.h"

#define LONG_PRESS_TIME 1.0f
#define BUFSZE 1024



static struct {
	Evas_Object *pressed_item;
} item_event_info = {
	.pressed_item = NULL,
};



static void _item_down_cb(void *data, Evas_Object *obj, const char* emission, const char* source)
{
	Evas_Object *icon_image;
	Evas_Object *item;
	bool item_enable_long_press;

	item = evas_object_data_get(obj, "item");
	item_enable_long_press = (bool) evas_object_data_get(item, "item_enable_long_press");

	_D("ITEM: mouse down event callback is invoked for %p", item);

	item_event_info.pressed_item = item;

	icon_image = evas_object_data_get(item, "icon_image");
	evas_object_color_set(icon_image, 100, 100, 100, 100);

	if (!item_enable_long_press) {
		return;
	}
}



static void _item_up_cb(void *data, Evas_Object *obj, const char* emission, const char* source)
{
	Evas_Object *icon_image;
	Evas_Object *item;

	item = evas_object_data_get(obj, "item");
	ret_if(NULL == item);

	_D("ITEM: mouse up event callback is invoked for %p", item);
	PRINT_APPFWK();

	icon_image = evas_object_data_get(item, "icon_image");
	evas_object_color_set(icon_image, 255, 255, 255, 255);

	ret_if(NULL == item_event_info.pressed_item);

	if (item_event_info.pressed_item == item) {
		bool item_enable_long_press;

		item_enable_long_press = (bool) evas_object_data_get(item, "item_enable_long_press");
		_D("Not Edit Mode");

		if (mouse_is_scrolling()) {
			return;
		}

		item_launch(item);
	}

	item_event_info.pressed_item = NULL;
}



void item_event_register(Evas_Object *item)
{
	Evas_Object *item_edje;
	item_edje = _EDJ(item);
	evas_object_data_set(item_edje, "item", item);

	edje_object_signal_callback_add(item_edje, "item,down", "menu", _item_down_cb, NULL);
	edje_object_signal_callback_add(item_edje, "item,up", "menu", _item_up_cb, NULL);
}



void item_event_unregister(Evas_Object *item)
{
	Evas_Object *item_edje;
	item_edje = _EDJ(item);

	edje_object_signal_callback_del(item_edje, "item,down", "menu", _item_down_cb);
	edje_object_signal_callback_del(item_edje, "item,up", "menu", _item_up_cb);

	evas_object_data_del(item_edje, "item");
}



// End of a file
