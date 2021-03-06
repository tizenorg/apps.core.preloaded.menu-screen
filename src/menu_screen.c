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
#include <Ecore_X.h>
#include <ail.h>
#include <app.h>
#include <stdbool.h>
#include <vconf.h>
#include <utilX.h>
#include <aul.h>

#include "conf.h"
#include "item.h"
#include "layout.h"
#include "mapbuf.h"
#include "menu_screen.h"
#include "mouse.h"
#include "page.h"
#include "page_scroller.h"
#include "util.h"

#define STR_ENV_ENGINE "LAUNCHER_ENGINE"
#define STR_ENV_FPS "LAUNCHER_FPS"

#define LAYOUT_EDJE_PORTRAIT EDJEDIR"/layout_portrait.edj"
#define LAYOUT_GROUP_NAME "layout"



// Define prototype of the "hidden API of AUL"
extern int aul_listen_app_dead_signal(int (*func)(int signal, void *data), void *data);
static struct {
	int state;
	int root_width;
	int root_height;
	double xscale;
	double yscale;
	Evas *evas;
	Ecore_Evas *ee;
	Evas_Object *win;
	bool is_done;
} menu_screen_info = {
	.state = APP_STATE_PAUSE,
	.evas = NULL,
	.ee = NULL,
	.win = NULL,
	.is_done = false,
};



Evas *menu_screen_get_evas(void)
{
	return menu_screen_info.evas;
}



int menu_screen_get_root_width(void)
{
	return menu_screen_info.root_width;
}



int menu_screen_get_root_height(void)
{
	return menu_screen_info.root_height;
}



double menu_screen_get_yscale(void)
{
	return menu_screen_info.yscale;
}



Evas_Object *menu_screen_get_win(void)
{
	return menu_screen_info.win;
}



bool menu_screen_get_done(void)
{
	return menu_screen_info.is_done;
}



void menu_screen_set_done(bool is_done)
{
	menu_screen_info.is_done = is_done;
}



static menu_screen_error_e _create_canvas(char *name, char *title)
{
	Ecore_X_Atom ATOM_WM_WINDOW_ROLE;

	menu_screen_info.win = elm_win_add(NULL, name, ELM_WIN_BASIC);
	retv_if(NULL == menu_screen_info.win, MENU_SCREEN_ERROR_FAIL);

	if (title) {
		elm_win_title_set(menu_screen_info.win, title);
	}
	elm_win_borderless_set(menu_screen_info.win, EINA_TRUE);

	ecore_x_icccm_name_class_set(elm_win_xwindow_get(menu_screen_info.win), "MENU_SCREEN", "MENU_SCREEN");
	ATOM_WM_WINDOW_ROLE = ecore_x_atom_get("WM_WINDOW_ROLE");
	if (ATOM_WM_WINDOW_ROLE) {
		ecore_x_window_prop_string_set(elm_win_xwindow_get(menu_screen_info.win), ATOM_WM_WINDOW_ROLE, "MENU_SCREEN");
	} else {
		_D("Failed to set the window role as MENU_SCREEN");
	}

	menu_screen_info.evas = evas_object_evas_get(menu_screen_info.win);
	if (!menu_screen_info.evas) {
		_E("[%s] Failed to get the evas object", __func__);
	}

	menu_screen_info.ee = ecore_evas_ecore_evas_get(menu_screen_info.evas);
	if (!menu_screen_info.ee) {
		_E("[%s] Failed to get ecore_evas object", __func__);
	}

	evas_object_move(menu_screen_info.win, 0, 0);
	evas_object_resize(menu_screen_info.win, menu_screen_info.root_width, menu_screen_info.root_height);
	evas_object_show(menu_screen_info.win);

	return MENU_SCREEN_ERROR_OK;
}



static void _destroy_canvas(void)
{
	evas_object_del(menu_screen_info.win);
}



static int _dead_cb(int pid, void *data)
{
	utilx_hide_fake_effect(
		ecore_x_display_get(),
		ecore_x_window_root_get(ecore_evas_window_get(menu_screen_info.ee))
	);

	return EXIT_SUCCESS;
}



static void _set_scale(void)
{
	double scale;
	Ecore_X_Window focus_win;
	Ecore_X_Window root_win;

	focus_win = ecore_x_window_focus_get();
	root_win = ecore_x_window_root_get(focus_win);
	ecore_x_window_size_get(root_win, &menu_screen_info.root_width, &menu_screen_info.root_height);

	menu_screen_info.xscale = (double) menu_screen_info.root_width / (double) BASE_WIDTH;
	menu_screen_info.yscale = (double) menu_screen_info.root_height / (double) BASE_HEIGHT;
	scale = menu_screen_info.xscale < menu_screen_info.yscale ? menu_screen_info.xscale : menu_screen_info.yscale;
	_D("width:%d, height:%d, scale:%f", menu_screen_info.root_width, menu_screen_info.root_height, scale);
	elm_config_scale_set(scale);
}



static void _create_bg(void)
{
	char *buf;
	Evas_Coord w;
	Evas_Coord h;
	Evas_Object *bg;
	double f, wf, hf;
	static int trigger = 0;
	const char *key;
	int width;
	int height;

	buf = vconf_get_str(VCONFKEY_BGSET);
	ret_if(NULL == buf);

	width = menu_screen_get_root_width();
	height = menu_screen_get_root_height();

	bg = evas_object_data_get(menu_screen_get_win(), "bg");
	if (NULL == bg) {
		Evas_Object *rect;

		rect = evas_object_rectangle_add(menu_screen_get_evas());
		ret_if(NULL == rect);
		evas_object_data_set(menu_screen_get_win(), "rect", rect);
		evas_object_color_set(rect, 0, 0, 0, 255);
		evas_object_size_hint_min_set(rect, width, height);
		evas_object_size_hint_max_set(rect, width, height);
		evas_object_resize(rect, width, height);
		evas_object_show(rect);

		bg = evas_object_image_add(menu_screen_get_evas());
		if (NULL == bg) {
			free(buf);
			return;
		}
		evas_object_data_set(menu_screen_get_win(), "bg", bg);
	}

	if (trigger == 0) {
		key = "/";
		trigger = 1;
	} else {
		key = NULL;
		trigger = 0;
	}

	evas_object_image_file_set(bg, buf, key);
	evas_object_image_size_get(bg, &w, &h);
	evas_object_image_filled_set(bg, 1);

	wf = (double) width / (double) w;
	hf = (double) height / (double) h;

	f = wf > hf ? hf : wf;

	w = (int) ((double) f * (double) w);
	h = (int) ((double) f * (double) h);

	evas_object_image_load_size_set(bg, w, h);
	evas_object_image_fill_set(bg, 0, 0, w, h);
	evas_object_move(bg, (width - w) / 2, (height - h) / 2);
	evas_object_resize(bg, w, h);
	evas_object_show(bg);

	free(buf);
}




static void _destroy_bg()
{
	Evas_Object *rect;
	Evas_Object *bg;

	rect = evas_object_data_del(menu_screen_get_win(), "rect");
	evas_object_del(rect);

	bg = evas_object_data_del(menu_screen_get_win(), "bg");
	evas_object_del(bg);
}



static void _change_bg_cb(keynode_t *node, void *data)
{
	_D("Background image is changed.");
	_create_bg();
}



static bool _create_cb(void *data)
{
	Evas_Object *layout;

	_set_scale();
	retv_if(MENU_SCREEN_ERROR_FAIL == _create_canvas(PACKAGE, PACKAGE), EXIT_FAILURE);
	elm_win_indicator_mode_set(menu_screen_info.win, ELM_WIN_INDICATOR_SHOW);

	if (vconf_notify_key_changed(VCONFKEY_BGSET, _change_bg_cb, NULL) < 0) {
		_E("Failed to register a vconf cb for %s\n", VCONFKEY_BGSET);
	}
	_create_bg();

	layout = layout_create(menu_screen_info.win, LAYOUT_EDJE_PORTRAIT,
				LAYOUT_GROUP_NAME, MENU_SCREEN_ROTATE_PORTRAIT);
	if (NULL == layout) {
		_E("Faield to load an edje object");
		evas_object_del(menu_screen_info.win);
		return EXIT_FAILURE;
	}
	evas_object_data_set(menu_screen_info.win, "layout", layout);
	evas_object_show(layout);

	mouse_register();
	aul_listen_app_dead_signal(_dead_cb, NULL);

	return true;
}



static void _terminate_cb(void *data)
{
	Evas_Object *layout;

	if (vconf_ignore_key_changed(VCONFKEY_BGSET, _change_bg_cb) < 0) {
		_E("Failed to remove bgset %s\n", VCONFKEY_BGSET);
	}

	evas_object_hide(menu_screen_info.win);

	mouse_unregister();

	layout = evas_object_data_del(menu_screen_info.win, "layout");
	layout_destroy(layout);

	_destroy_bg();
	_destroy_canvas();
	evas_object_del(menu_screen_info.win);
}



static void _pause_cb(void *data)
{
	_D("Pause start");

	if (vconf_set_int(VCONFKEY_IDLE_SCREEN_TOP, VCONFKEY_IDLE_SCREEN_TOP_FALSE) < 0) {
		_E("Failed to set memory/idle/top to 0");
	}

	menu_screen_info.state = APP_STATE_PAUSE;
}



static void _resume_cb(void *data)
{
	_D("START RESUME");

	if (vconf_set_int(VCONFKEY_IDLE_SCREEN_TOP, VCONFKEY_IDLE_SCREEN_TOP_TRUE) < 0) {
		_E("Failed to set memory/idle/top to 1");
	}

	utilx_hide_fake_effect(
		ecore_x_display_get(),
		ecore_x_window_root_get(ecore_evas_window_get(menu_screen_info.ee))
	);

	menu_screen_info.state = APP_STATE_RESUME;
}



static void _service_cb(service_h service, void *data)
{
	_D("START RESET : %d", menu_screen_info.state);

	if (vconf_set_int(VCONFKEY_IDLE_SCREEN_TOP, VCONFKEY_IDLE_SCREEN_TOP_TRUE) < 0) {
		_E("Failed to set memory/idle/top to 1");
	}

	utilx_hide_fake_effect(
		ecore_x_display_get(),
		ecore_x_window_root_get(ecore_evas_window_get(menu_screen_info.ee))
	);
}



static void _language_changed_cb(void *data)
{
	register unsigned int i;
	register unsigned int j;
	unsigned int count;
	Evas_Object *layout;
	Evas_Object *all_apps;
	Evas_Object *scroller;
	Evas_Object *page;
	Evas_Object *item;
	unsigned int page_max_app;

	_D("Language is changed");

	if (false == menu_screen_info.is_done) {
		elm_exit();
	}

	layout = evas_object_data_get(menu_screen_info.win, "layout");
	ret_if(NULL == layout);
	all_apps = evas_object_data_get(layout, "all_apps");
	ret_if(NULL == all_apps);
	scroller = elm_object_part_content_get(all_apps, "content");
	ret_if(NULL == scroller);

	count = page_scroller_count_page(scroller);
	page_max_app = (unsigned int) evas_object_data_get(scroller, "page_max_app");
	for (i = 0; i < count; i ++) {
		page = page_scroller_get_page_at(scroller, i);
		if (!page) continue;
		if (mapbuf_is_enabled(page)) {
			mapbuf_disable(page, 1);
		}

		for (j = 0; j < page_max_app; j ++) {
			ail_appinfo_h ai;
			char *name;

			item = page_get_item_at(page, j);
			if (!item) continue;

			if (ail_package_get_appinfo(item_get_package(item), &ai) < 0) continue;
			if (ail_appinfo_get_str(ai, AIL_PROP_NAME_STR, &name) < 0) {
				ail_package_destroy_appinfo(ai);
				continue;
			}

			if (!name) {
				_D("Faield to get name for %s", item_get_package(item));
				ail_package_destroy_appinfo(ai);
				continue;
			}

			item_set_name(item, name, 0);
			ail_package_destroy_appinfo(ai);
		}

		mapbuf_enable(page, 1);
	}
}



static void _init(app_event_callback_s *event_callback)
{
	event_callback->create = _create_cb;
	event_callback->terminate = _terminate_cb;
	event_callback->pause = _pause_cb;
	event_callback->resume = _resume_cb;
	event_callback->service = _service_cb;
	event_callback->low_memory = NULL;
	event_callback->low_battery = NULL;
	event_callback->device_orientation = NULL;
	event_callback->language_changed = _language_changed_cb;
	event_callback->region_format_changed = NULL;
}



static void _fini(void)
{
}



int main(int argc, char *argv[])
{
	const char *env;
	app_event_callback_s event_callback;

	env = getenv(STR_ENV_ENGINE);
	if (env) {
		_D("ELM_ENGINE is set as [%s]", env);
		setenv("ELM_ENGINE", env, 1);
	} else {
		_D("ELM_ENGINE is set as [%s]", "gl");
		setenv("ELM_ENGINE", "gl", 1);
	}

	env = getenv(STR_ENV_FPS);
	if (env) {
		_D("ELM_FPS is set as [%s]", env);
		setenv("ELM_FPS", env, 1);
	} else {
		_D("ELM_FPS is set as [%s]", "6000");
		setenv("ELM_FPS", "6000", 1);
	}

	_init(&event_callback);
	app_efl_main(&argc, &argv, &event_callback, NULL);
	_fini();

	return EXIT_SUCCESS;
}





// End of a file
