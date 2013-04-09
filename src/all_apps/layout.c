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
#include "all_apps/shortcut.h"

#define ALL_APPS_EDJE_PORTRAIT EDJEDIR"/all_apps_portrait.edj"
#define ALL_APPS_GROUP "all_apps"

#define GROUP_4X4_EDJE_PORTRAIT EDJEDIR"/group_4x4_portrait.edj";

#define ALL_APPS_EDIT_BUTTON_EDJE EDJEDIR"/button_edit.edj"
#define ALL_APPS_EDIT_BUTTON_GROUP "button_edit"
#define ALL_APPS_EDIT_BUTTON_PART "button_edit"

#define IDS_HS_BUTTON_DONE "IDS_HS_BUTTON_DONE"


HAPI void all_apps_layout_edit(Evas_Object *all_apps)
{
	Evas_Object *edit_button;
	Evas_Object *scroller;

	edit_button = elm_object_part_content_get(all_apps, ALL_APPS_EDIT_BUTTON_PART);
	ret_if(NULL == edit_button);
	edje_object_signal_emit(_EDJ(edit_button), "edit_button,enable", "menu");
	if (edje_object_part_text_set(_EDJ(edit_button), "edit_button_text", _(IDS_HS_BUTTON_DONE)) == EINA_FALSE) {
		_E("Failed to set text on the part, edje:%p, part:%s, text:%s", _EDJ(edit_button), "edit_button_text", _(IDS_HS_BUTTON_DONE));
	}

	scroller = elm_object_part_content_get(all_apps, "content");
	ret_if(NULL == scroller);
	page_scroller_edit(scroller);
}



HAPI void all_apps_layout_unedit(Evas_Object *all_apps)
{
	Evas_Object *edit_button;

	ret_if(NULL == all_apps);

	do {
		edit_button = elm_object_part_content_get(all_apps, ALL_APPS_EDIT_BUTTON_PART);
		ret_if(NULL == edit_button);

		edje_object_signal_emit(_EDJ(edit_button), "edit_button,disable", "menu");
		if (edje_object_part_text_set(_EDJ(edit_button), "edit_button_text", "") == EINA_FALSE) {
			_E("Failed to set text on the part, edje:%p, part:%s, text:(null)", _EDJ(edit_button), "edit_button_text");
		}
	} while (0);

	do {
		Evas_Object *scroller;

		scroller = elm_object_part_content_get(all_apps, "content");
		ret_if(NULL == scroller);

		page_scroller_unedit(scroller);
	} while (0);
}



static void _press_edit_button(Evas_Object *edje)
{
	edje_object_signal_emit(edje, "edit_button,enable,press", "menu");
}



static void _release_edit_button(Evas_Object *edje)
{
	edje_object_signal_emit(edje, "edit_button,enable,release", "menu");
}



static void _edit_button_down_cb(void *data, Evas_Object *obj, const char* emission, const char* source)
{
	Evas_Object *scroller = data;

	_D("Edit button is down");

	if (page_scroller_is_edited(scroller)) {
		_press_edit_button(obj);
	}
}



static void _edit_button_up_cb(void *data, Evas_Object *obj, const char* emission, const char* source)
{
	Evas_Object *scroller = data;
	Evas_Object *all_apps;

	_D("Edit button is up");

	all_apps = evas_object_data_get(scroller, "tab");
	if (page_scroller_is_edited(scroller)) {
		Evas_Object *eo;
		bool pressed = false;

		eo = evas_object_data_get(obj, "evas_object");
		pressed = (bool) evas_object_data_get(eo, "pressed");
		if (pressed) {
			all_apps_layout_unedit(all_apps);
		} else {
			_release_edit_button(obj);
		}
	} else {
		all_apps_layout_edit(all_apps);
	}
}



static void _button_out_cb(void *data, Evas *e, Evas_Object *obj, void *event_info)
{
	_D("Edit button is out");
	evas_object_data_set(obj, "pressed", (void *) false);
}



static void _button_in_cb(void *data, Evas *e, Evas_Object *obj, void *event_info)
{
	_D("Edit button is in");
	evas_object_data_set(obj, "pressed", (void *) true);
}



static Evas_Object *_add_edit_button(Evas_Object *parent, Evas_Object *scroller)
{
	Evas_Object *button;

	retv_if(NULL == parent, NULL);
	retv_if(NULL == scroller, NULL);

	button = layout_load_edj(parent, ALL_APPS_EDIT_BUTTON_EDJE, ALL_APPS_EDIT_BUTTON_GROUP);
	retv_if(NULL == button, NULL);

	edje_object_signal_callback_add(_EDJ(button), "edit_button,down", "menu", _edit_button_down_cb, scroller);
	edje_object_signal_callback_add(_EDJ(button), "edit_button,up", "menu", _edit_button_up_cb, scroller);
	evas_object_event_callback_add(button, EVAS_CALLBACK_MOUSE_OUT, _button_out_cb, NULL);
	evas_object_event_callback_add(button, EVAS_CALLBACK_MOUSE_IN, _button_in_cb, NULL);
	evas_object_data_set(button, "pressed", (void *) false);

	return button;
}



static void _remove_edit_button(Evas_Object *button)
{
	ret_if(NULL == button);

	edje_object_signal_callback_del(_EDJ(button), "edit_button,down", "menu", _edit_button_down_cb);
	edje_object_signal_callback_del(_EDJ(button), "edit_button,up", "menu", _edit_button_up_cb);
	evas_object_event_callback_del(button, EVAS_CALLBACK_MOUSE_OUT, _button_out_cb);
	evas_object_event_callback_del(button, EVAS_CALLBACK_MOUSE_IN, _button_in_cb);
	evas_object_data_del(button, "pressed");

	layout_unload_edj(button);
}



static menu_screen_error_e _load_item(Evas_Object *scroller, app_list_item *item)
{
	app_info_t *ai;

	retv_if(NULL == item, MENU_SCREEN_ERROR_FAIL);
	ai = item->data;
	if (ai->nodisplay || !ai->enabled) {
		return MENU_SCREEN_ERROR_OK;
	}

	retv_if(NULL == page_scroller_push_item(scroller, ai), MENU_SCREEN_ERROR_FAIL);

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

	Evas_Object *scroller;
	scroller = evas_object_data_get(all_apps, "scroller");
	if (MENU_SCREEN_ERROR_OK != all_apps_shortcut_add_all(scroller))
		_E("Cannot add shortcuts");

	do {
		Evas_Object *button;
		button = _add_edit_button(all_apps, scroller);
		if (NULL == button) {
			_D("cannot make the edit button");
		}
		elm_object_part_content_set(all_apps, ALL_APPS_EDIT_BUTTON_PART, button);
	} while (0);

	return MENU_SCREEN_ERROR_OK;
}



HAPI Evas_Object *all_apps_layout_create(Evas_Object *controlbar, int rotate)
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
	height = menu_screen_get_root_height() - INDEX_HEIGHT;

	if (menu_screen_get_root_width() > menu_screen_get_root_height()) item_width = height / 5.0f;
	else item_width = width / 4.0f;

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

	index = index_create(all_apps, 0,0);
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
	if (!all_apps_shortcut_init(all_apps)) _E("Cannot initialize shortcut");
	retv_if(MENU_SCREEN_ERROR_FAIL == _push_items(all_apps), NULL);

	return all_apps;
}



HAPI void all_apps_layout_destroy(Evas_Object *all_apps)
{
	Evas_Object *index;
	Evas_Object *scroller;

	ret_if(NULL == all_apps);

	all_apps_shortcut_fini();

	do {
		Evas_Object *button;
		if(all_apps) {
			button = elm_object_part_content_unset(all_apps, ALL_APPS_EDIT_BUTTON_PART);
			_remove_edit_button(button);
		} else {
			_D("cannot find the all_apps object");
		}
	} while (0);

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
