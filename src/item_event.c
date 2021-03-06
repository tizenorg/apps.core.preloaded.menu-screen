 /*
  * Copyright 2012  Samsung Electronics Co., Ltd
  *
  * Licensed under the Flora License, Version 1.0 (the "License");
  * you may not use this file except in compliance with the License.
  * You may obtain a copy of the License at
  *
  *     http://www.tizenopensource.org/license
  *
  * Unless required by applicable law or agreed to in writing, software
  * distributed under the License is distributed on an "AS IS" BASIS,
  * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
  * See the License for the specific language governing permissions and
  * limitations under the License.
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
