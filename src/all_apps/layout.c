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

#include "menu_screen.h"
#include "conf.h"
#include "index.h"
#include "item_event.h"
#include "layout.h"
#include "list.h"
#include "page_scroller.h"
#include "util.h"
#include "all_apps/layout.h"
#include "all_apps/list.h"

#define ALL_APPS_EDJE_PORTRAIT EDJEDIR"/all_apps_portrait.edj"
#define ALL_APPS_GROUP "all_apps"

#define GROUP_4X4_EDJE_PORTRAIT EDJEDIR"/group_4x4_portrait.edj";



static menu_screen_error_e _load_item(Evas_Object *scroller, app_list_item *item)
{
	app_info_t *ai;

	retv_if(NULL == item, MENU_SCREEN_ERROR_FAIL);
	ai = item->data;
	if (ai->nodisplay == true) {
		return MENU_SCREEN_ERROR_FAIL;
	}

	retv_if(MENU_SCREEN_ERROR_FAIL == page_scroller_push_item(scroller, ai), MENU_SCREEN_ERROR_FAIL);

	return MENU_SCREEN_ERROR_OK;

}



static Eina_Bool _push_items_idler_cb(void *all_apps)
{
	bool flag = true;
	app_list *list;
	app_list_item *item;
	Evas_Object *scroller;

	list = evas_object_data_get(all_apps, "list");
	scroller = evas_object_data_get(all_apps, "scroller");

	retv_if(NULL == list, ECORE_CALLBACK_CANCEL);
	goto_if(NULL == scroller, ERROR);
	goto_if(MENU_SCREEN_ERROR_OK != list_is_ended(list, &flag), ERROR);
	goto_if(flag == true, ERROR);
	goto_if(MENU_SCREEN_ERROR_OK != list_get_item(list, &item), ERROR);
	goto_if(MENU_SCREEN_ERROR_OK != _load_item(scroller, item), ERROR);
	goto_if(MENU_SCREEN_ERROR_OK != list_next(list), ERROR);

	return ECORE_CALLBACK_RENEW;

ERROR:
	all_apps_list_destroy(list);
	evas_object_data_del(all_apps, "list");
	page_scroller_bring_in(scroller, 0);
	menu_screen_set_done(true);

	return ECORE_CALLBACK_CANCEL;
}



static menu_screen_error_e _push_items(Evas_Object *all_apps)
{
	app_list *list;
	Ecore_Idler *idle_timer;

	list = all_apps_list_create();
	retv_if(MENU_SCREEN_ERROR_OK != list_first(list), MENU_SCREEN_ERROR_FAIL);

	evas_object_data_set(all_apps, "list", list);
	idle_timer = ecore_idler_add(_push_items_idler_cb, all_apps);
	retv_if(NULL == idle_timer, MENU_SCREEN_ERROR_FAIL);

	return MENU_SCREEN_ERROR_OK;
}



Evas_Object *all_apps_layout_create(Evas_Object *controlbar, int rotate)
{
	Evas_Object *all_apps;
	Evas_Object *index;
	Evas_Object *scroller;

	static const char item_edje[] = EDJEDIR"/item_4x4.edj";
	static const char *page_edje;
	static const char *all_apps_edje;
	int item_width;
	int width;
	int height;

	all_apps = elm_layout_add(controlbar);
	retv_if(NULL == all_apps, NULL);

	all_apps_edje = ALL_APPS_EDJE_PORTRAIT;
	page_edje = GROUP_4X4_EDJE_PORTRAIT;
	width = menu_screen_get_root_width();
	height = menu_screen_get_root_height();
	item_width = width / 4.0f;

	if (EINA_FALSE == elm_layout_file_set(all_apps, all_apps_edje, ALL_APPS_GROUP)) {
		evas_object_del(all_apps);
		_E("cannot load layout");
		return NULL;
	}

	evas_object_data_set(all_apps, "win", evas_object_data_get(controlbar, "win"));
	evas_object_data_set(all_apps, "layout", evas_object_data_get(controlbar, "layout"));
	evas_object_data_set(all_apps, "controlbar", controlbar);
	evas_object_data_set(all_apps, "page_edje", (void *) page_edje);
	evas_object_data_set(all_apps, "page_max_app", (void *) PAGE_MAX_APP);
	evas_object_data_set(all_apps, "item_edje", (void *) item_edje);
	evas_object_data_set(all_apps, "item_width", (void *) item_width);
	evas_object_data_set(all_apps, "item_height", (void *) item_width);
	evas_object_data_set(all_apps, "item_enable_long_press", (void *) true);
	evas_object_data_set(all_apps, "item_text_dual_line", (void *) true);
	evas_object_data_set(all_apps, "is_under_pkgmgr", (void *) true);
	evas_object_data_set(all_apps, "rotate", (void *) rotate);
	evas_object_data_set(all_apps, "enable_bg_image", (void *) true);
	evas_object_size_hint_min_set(all_apps, width, height);
	evas_object_size_hint_max_set(all_apps, width, height);
	evas_object_resize(all_apps, width, height);

	index = index_create(all_apps, 0);
	if (NULL == index) {
		evas_object_del(all_apps);
		_E("cannot create index.");
		return NULL;
	}
	elm_object_part_content_set(all_apps, "index", index);
	evas_object_data_set(all_apps, "index", index);

	scroller = page_scroller_create(all_apps, index, PAGE_SCROLLER_SORT_BY_PACKAGE, rotate);
	if (NULL == scroller) {
		evas_object_del(all_apps);
		index_destroy(index);
		_E("cannot create scroller.");
		return NULL;
	}

	elm_object_part_content_set(all_apps, "content", scroller);
	evas_object_data_set(all_apps, "scroller", scroller);

	retv_if(MENU_SCREEN_ERROR_FAIL == _push_items(all_apps), NULL);

	return all_apps;
}



void all_apps_layout_destroy(Evas_Object *all_apps)
{
	Evas_Object *index;
	Evas_Object *scroller;

	index = evas_object_data_get(all_apps, "index");
	scroller = evas_object_data_get(all_apps, "scroller");

	if (scroller) page_scroller_destroy(scroller);
	if (index) index_destroy(index);

	evas_object_data_del(all_apps, "win");
	evas_object_data_del(all_apps, "layout");
	evas_object_data_del(all_apps, "controlbar");
	evas_object_data_del(all_apps, "index");
	evas_object_data_del(all_apps, "scroller");
	evas_object_data_del(all_apps, "page_edje");
	evas_object_data_del(all_apps, "page_max_app");
	evas_object_data_del(all_apps, "item_edje");
	evas_object_data_del(all_apps, "item_width");
	evas_object_data_del(all_apps, "item_height");
	evas_object_data_del(all_apps, "item_enable_long_press");
	evas_object_data_del(all_apps, "item_text_dual_line");
	evas_object_data_del(all_apps, "is_under_pkgmgr");
	evas_object_data_del(all_apps, "rotate");
	evas_object_data_del(all_apps, "enable_bg_image");

	evas_object_del(all_apps);
}



// End of a file
