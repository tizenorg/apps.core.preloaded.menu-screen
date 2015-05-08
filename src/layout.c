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

#include <Elementary.h>
//#include <Ecore_X.h>
//#include <utilX.h>
#include <vconf.h>

#include "menu_screen.h"
#include "conf.h"
#include "item_event.h"
#include "layout.h"
#include "mapbuf.h"
#include "page.h"
#include "page_scroller.h"
#include "item.h"
#include "util.h"
#include "all_apps/layout.h"



HAPI Evas_Object *layout_create(Evas_Object *conformant, const char *file, const char *group, int rotate)
{
	Evas_Object *layout;
	Evas_Object *bg;
	Evas *evas;

	do {
		int width;
		int height;

		layout = layout_load_edj(conformant, file, group);
		retv_if(NULL == layout, NULL);

		width = menu_screen_get_root_width();
		height = menu_screen_get_root_height();

		evas_object_size_hint_weight_set(layout, EVAS_HINT_EXPAND, EVAS_HINT_EXPAND);
		evas_object_size_hint_min_set(layout, width, height);
		evas_object_size_hint_max_set(layout, width, height);
		evas_object_resize(layout, width, height);
		evas_object_show(layout);

		evas = evas_object_evas_get(layout);
		bg = evas_object_rectangle_add(evas);
		evas_object_size_hint_min_set(bg, width, height);
		evas_object_size_hint_max_set(bg, width, height);
		evas_object_resize(bg, width, height);
		elm_object_part_content_set(layout, "bg", bg);
		evas_object_show(bg);

		evas_object_data_set(layout, "win", menu_screen_get_win());
		evas_object_data_set(layout, "rotate", (void *) rotate);
		evas_object_data_set(layout, "width", (void *) width);
		evas_object_data_set(layout, "height", (void *) height);
	} while (0);

	do {
		Evas_Object *all_apps;

		all_apps = all_apps_layout_create(layout, rotate);
		if (NULL == all_apps) {
			_E("Failed to create scroller");
			layout_destroy(layout);
			return NULL;
		}
		evas_object_data_set(layout, "all_apps", all_apps);
		elm_object_part_content_set(layout, "content", all_apps);
	} while (0);

	return layout;
}



HAPI void layout_destroy(Evas_Object *layout)
{
	Evas_Object *all_apps;

	all_apps = evas_object_data_del(layout, "all_apps");
	all_apps_layout_destroy(all_apps);

	evas_object_data_del(layout, "win");
	evas_object_data_del(layout, "rotate");
	evas_object_data_del(layout, "width");
	evas_object_data_del(layout, "height");

	layout_unload_edj(layout);
}



HAPI void layout_enable_block(Evas_Object *layout)
{
	ret_if(NULL == layout);

	_D("Enable layout blocker");
	edje_object_signal_emit(_EDJ(layout), "block", "layout");
}



HAPI void layout_disable_block(Evas_Object *layout)
{
	ret_if(NULL == layout);

	_D("Disable layout blocker");
	edje_object_signal_emit(_EDJ(layout), "unblock", "layout");
}



HAPI Evas_Object* layout_load_edj(Evas_Object *parent, const char *edjname, const char *grpname)
{
	Evas_Object *eo;

	retv_if(NULL == parent, NULL);

	eo = elm_layout_add(parent);
	retv_if(NULL == eo, NULL);
	retv_if(EINA_FALSE == elm_layout_file_set(eo, edjname, grpname), NULL);

	evas_object_data_set(_EDJ(eo), "evas_object", eo);
	evas_object_show(eo);

	return eo;
}



HAPI void layout_unload_edj(Evas_Object *layout)
{
	Evas_Object *evas_object;

	ret_if(NULL == layout);

	evas_object = evas_object_data_get(_EDJ(layout), "evas_object");
	if (evas_object) {
		evas_object_data_del(_EDJ(layout), "evas_object");
	}

	evas_object_del(layout);
}



HAPI void layout_create_package(Evas_Object *layout, const char *package)
{
	app_info_t ai = {0,};
	Evas_Object *all_apps = NULL;
	Evas_Object *scroller = NULL;
	Evas_Object *item = NULL;

	if (MENU_SCREEN_ERROR_FAIL == list_get_values(package, &ai)) {
		list_free_values(&ai);
		return;
	}

	if (ai.nodisplay) {
		_D("This app(%s) is nodisplay", ai.package);
		list_free_values(&ai);
		return;
	}


	all_apps = evas_object_data_get(layout, "all_apps");
	goto_if(!all_apps, EXIT);

	scroller = evas_object_data_get(all_apps, "scroller");
	goto_if(!scroller, EXIT);

	/* Update the package */
	item = page_scroller_find_item_by_package(scroller, ai.package, NULL);
	if (item) {
		_D("Package %s is found, update it!", package);
		item_update(item, &ai);
		goto EXIT;
	}

	/* Create the package */
	_D("package %s is installed directly", package);
	item = page_scroller_push_item_by_order(scroller, &ai);
	if (!item) _E("item is NULL");

EXIT:
	list_free_values(&ai);
	return;
}



HAPI void layout_update_package(Evas_Object *layout, const char *package)
{
    layout_create_package(layout, package);
}



static Evas_Object *_animated_unpack_item(Evas_Object *scroller, Evas_Object *page, unsigned int pos)
{
	Evas_Object *out = NULL;
	Evas_Object *item;
	Evas_Object *next_page;

	char buf[32];
	unsigned int page_max_app;
	unsigned int page_no;
	page_scroller_sort_type_e sort_type;

	out = page_unpack_item_at(page, pos);
	retv_if(NULL == out, NULL);

	page_no = page_scroller_get_page_no(scroller, page);
	page_max_app = (unsigned int) evas_object_data_get(scroller, "page_max_app");
	sort_type = (page_scroller_sort_type_e) evas_object_data_get(scroller, "sort_type");

	pos ++;
	while (page && page_no < MAX_PAGE_NO) {
		if (0 == page_count_item(page)) {
			page_destroy(scroller, page);
			break;
		}

		for (; pos < page_max_app; pos ++) {
			item = page_unpack_item_at(page, pos);
			if (NULL == item) continue;

			page_pack_item(page, pos - 1, item);
			snprintf(buf, 32, "menu%d", pos - 1);
			edje_object_signal_emit(_EDJ(page), STR_MOVE_NEXT, buf);
			edje_object_signal_emit(_EDJ(page), STR_ANI_RETURN, buf);
		}

		if (sort_type == PAGE_SCROLLER_SORT_MAX) {
			return NULL;
		}

		page_no ++;
		next_page = page_scroller_get_page_at(scroller, page_no);
		if (next_page) {
			item = page_unpack_item_at(next_page, 0);
			if (NULL == item) continue;

			page_pack_item(page, page_max_app - 1, item);
		} else break;

		pos = 1;
		page = next_page;
	}

	return out;
}



HAPI void layout_delete_package(Evas_Object *layout, const char *package)
{
	Evas_Object *all_apps = NULL;
	Evas_Object *scroller = NULL;
	Evas_Object *page = NULL;
	Evas_Object *item = NULL;
	int page_no = 0;
	register unsigned int i;
	unsigned int page_max_app;

	all_apps = evas_object_data_get(layout, "all_apps");
	ret_if(!all_apps);

	scroller = evas_object_data_get(all_apps, "scroller");
	ret_if(!scroller);

	item = page_scroller_find_item_by_package(scroller, package, &page_no);
	if (!item) return;

	page = page_scroller_get_page_at(scroller, page_no);
	ret_if(!page);

	page_max_app = (unsigned int) evas_object_data_get(scroller, "page_max_app");
	for (i = 0; i < page_max_app; i++) {
		if (item == page_get_item_at(page, i)) {
			break;
		}
	}
	if (i == page_max_app) return;

	item = _animated_unpack_item(scroller, page, i);
	ret_if(!item);
	item_destroy(item);
}



// End of file
