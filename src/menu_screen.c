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

#include <app.h>
#include <appcore-efl.h>
#include <aul.h>
#include <Elementary.h>
#include <stdbool.h>
#include <system_info.h>
#include <vconf.h>
#include <app_preference.h>
#include <system_settings.h>
#include <pkgmgr-info.h>

#include "conf.h"
#include "item.h"
#include "key.h"
#include "layout.h"
#include "mapbuf.h"
#include "menu_screen.h"
#include "mouse.h"
#include "page.h"
#include "page_scroller.h"
#include "pkgmgr.h"
#include "util.h"

#define MENU_SCREEN_ENGINE "file/private/org.tizen.menu-screen/engine"

#define LAYOUT_EDJE_PORTRAIT EDJEDIR"/layout_portrait.edj"
#define LAYOUT_GROUP_NAME "layout"



// Define prototype of the "hidden API of AUL"
extern int aul_listen_app_dead_signal(int (*func)(int signal, void *data), void *data);
static struct {
	int state;
	int root_width;
	int root_height;
	int is_tts;
	int booting_state;
	Evas *evas;
	Ecore_Evas *ee;
	Evas_Object *win;
	Elm_Theme *theme;
	bool is_done;
} menu_screen_info = {
	.state = APP_STATE_PAUSE,
	.is_tts = false,
	.booting_state = 0,
	.evas = NULL,
	.ee = NULL,
	.win = NULL,
	.theme = NULL,
	.is_done = false,
};



HAPI Evas *menu_screen_get_evas(void)
{
	return menu_screen_info.evas;
}



HAPI int menu_screen_get_root_width(void)
{
	return menu_screen_info.root_width;
}



HAPI int menu_screen_get_root_height(void)
{
	return menu_screen_info.root_height;
}



HAPI Evas_Object *menu_screen_get_win(void)
{
	return menu_screen_info.win;
}



HAPI Elm_Theme *menu_screen_get_theme(void)
{
	return menu_screen_info.theme;
}



HAPI bool menu_screen_get_done(void)
{
	return menu_screen_info.is_done;
}



HAPI void menu_screen_set_done(bool is_done)
{
	menu_screen_info.is_done = is_done;
}



HAPI int menu_screen_get_state(void)
{
	return menu_screen_info.state;
}



HAPI int menu_screen_is_tts(void)
{
	return menu_screen_info.is_tts;
}


static Eina_Bool _appcore_flush_cb(void *data)
{
	if (APP_STATE_PAUSE != menu_screen_info.state) return ECORE_CALLBACK_CANCEL;
	if (0 != appcore_flush_memory()) _E("Cannot flush memory");
	return ECORE_CALLBACK_CANCEL;
}



HAPI void menu_screen_inc_booting_state(void)
{
	menu_screen_info.booting_state++;
	if (BOOTING_STATE_DONE > menu_screen_info.booting_state) return;

	menu_screen_error_e ret = MENU_SCREEN_ERROR_OK;
	do {
		ret = pkgmgr_reserve_list_pop_request();
	} while (MENU_SCREEN_ERROR_OK == ret);

	/*  Cache memory is cleared when the application paused (every time, after 5 seconds (in appcore)),
	*  but after running in a minimized mode (HIDE_LAUNCH) application have status AS_RUNNING.
	*  Application have status AS_PAUSED only after change of visibility to hidden condition by user (on hiding window)
	*  Cleaning must be performed only once after application loading in hidden condition
	*  (and stay in a hidden condition at time of cleaning).
	*/
	if (APP_STATE_PAUSE == menu_screen_info.state)
		ecore_timer_add(5, _appcore_flush_cb, NULL);
}



HAPI void menu_screen_dec_booting_state(void)
{
	menu_screen_info.booting_state --;
}



HAPI int menu_screen_get_booting_state(void)
{
	return menu_screen_info.booting_state;
}



static bool _is_emulator_on(void)
{
	int ret;
	char *model = NULL;

	ret = system_info_get_platform_string("tizen.org/system/model_name", &model);
	if (SYSTEM_INFO_ERROR_NONE != ret) {
		if (model) {
			free(model);
		}
		return false;
	}

	if (!strncmp(model, "Emulator", strlen(model))) {
		_D("This model is on Emulator");
		free(model);
		return true;
	}

	_D("This model is NOT on Emulator");
	free(model);
	return false;
}



static void _window_resize_cb(void *data, Evas *e, Evas_Object *obj, void *event_info)
{
	int x, y, w, h;

	evas_object_geometry_get(obj, &x, &y, &w, &h);
	_D("window is resized : x[%d], y[%d], w[%d], h[%d]", x, y, w, h);
}



static menu_screen_error_e _create_canvas(char *name, char *title)
{
	char *buf;

	if (_is_emulator_on()) {
		_D("ELM_ENGINE is set as [software_x11]");
		elm_config_accel_preference_set("opengl");
	} else {
		buf = vconf_get_str(MENU_SCREEN_ENGINE);
		if (buf) {
			_D("ELM_ENGINE is set as [%s]", buf);
			elm_config_accel_preference_set(buf);
			free(buf);
		} else {
			_D("ELM_ENGINE is set as [gl]");
			elm_config_accel_preference_set("gl");
		}
	}

	menu_screen_info.win = elm_win_util_standard_add(name, name);
	retv_if(NULL == menu_screen_info.win, MENU_SCREEN_ERROR_FAIL);

	if (title) {
		elm_win_title_set(menu_screen_info.win, title);
	}
	_D("elm_scale: %f", elm_app_base_scale_get());
	_D("config_scale: %f", elm_config_scale_get());

	elm_win_borderless_set(menu_screen_info.win, EINA_TRUE);
	elm_win_screen_size_get(menu_screen_info.win, NULL, NULL, &menu_screen_info.root_width, &menu_screen_info.root_height);
	_D("menu-screen window size:: width: %d, height: %d", menu_screen_info.root_width, menu_screen_info.root_height);

#if 0
	ecore_x_icccm_name_class_set(elm_win_xwindow_get(menu_screen_info.win), "MENU_SCREEN", "MENU_SCREEN");
	ATOM_WM_WINDOW_ROLE = ecore_x_atom_get("WM_WINDOW_ROLE");
	if (ATOM_WM_WINDOW_ROLE) {
		ecore_x_window_prop_string_set(elm_win_xwindow_get(menu_screen_info.win), ATOM_WM_WINDOW_ROLE, "MENU_SCREEN");
	} else {
		_D("Failed to set the window role as MENU_SCREEN");
	}
#endif

	elm_win_role_set(menu_screen_info.win, "MENU_SCREEN");

	menu_screen_info.evas = evas_object_evas_get(menu_screen_info.win);
	if (!menu_screen_info.evas) {
		_E("[%s] Failed to get the evas object", __func__);
	}

	menu_screen_info.ee = ecore_evas_ecore_evas_get(menu_screen_info.evas);
	if (!menu_screen_info.ee) {
		_E("[%s] Failed to get ecore_evas object", __func__);
	}

	evas_object_show(menu_screen_info.win);

	evas_object_event_callback_add(menu_screen_info.win, EVAS_CALLBACK_RESIZE, _window_resize_cb, NULL);

	return MENU_SCREEN_ERROR_OK;
}



static void _destroy_canvas(void)
{
	evas_object_del(menu_screen_info.win);
}



static int _dead_cb(int pid, void *data)
{
	return EXIT_SUCCESS;
}



static void _bg_resize_cb(void *data, Evas *e, Evas_Object *obj, void *event_info)
{
	int x, y, w, h;

	evas_object_geometry_get(obj, &x, &y, &w, &h);
	_D("bg is resized : x[%d], y[%d], w[%d], h[%d]", x, y, w, h);
}



static void _create_bg(void)
{
	_D("Create BG");
	char *buf = NULL;
	Evas_Object *bg;
	int width;
	int height;

	if (system_settings_get_value_string(SYSTEM_SETTINGS_KEY_WALLPAPER_HOME_SCREEN, &buf) < 0) {
		_E("Failed to get a wallpaper: %d\n", SYSTEM_SETTINGS_KEY_WALLPAPER_HOME_SCREEN);
	}
	_D("Menu-screen bg's image : %s", buf);

	width = menu_screen_get_root_width();
	height = menu_screen_get_root_height();
	_D("width : %d, height : %d FOR BG", width, height);

	bg = evas_object_data_get(menu_screen_get_win(), "bg");
	if (!bg) {
		_D("BG is NULL, Create!!");
		Evas_Object *rect;

		rect = evas_object_rectangle_add(menu_screen_get_evas());
		ret_if(!rect);
		evas_object_data_set(menu_screen_get_win(), "rect", rect);
		evas_object_color_set(rect, 0, 0, 0, 255);
		evas_object_size_hint_min_set(rect, width, height);
		evas_object_size_hint_max_set(rect, width, height);
		elm_win_resize_object_add(menu_screen_get_win(), rect);
		evas_object_show(rect);

		bg = elm_image_add(menu_screen_get_win());
		if (NULL == bg) {
			free(buf);
			return;
		}
		evas_object_data_set(menu_screen_get_win(), "bg", bg);
	}

	elm_image_aspect_fixed_set(bg, EINA_TRUE);
	elm_image_fill_outside_set(bg, EINA_TRUE);
	elm_image_preload_disabled_set(bg, EINA_FALSE);

	evas_object_size_hint_min_set(bg, width, height);
	evas_object_size_hint_max_set(bg, width, height);
	if (!elm_image_file_set(bg, buf, NULL)) {
		_E("Failed to set image file : %s", buf);
	}

	elm_win_resize_object_add(menu_screen_get_win(), bg);
	evas_object_show(bg);

	evas_object_event_callback_add(bg, EVAS_CALLBACK_RESIZE, _bg_resize_cb, NULL);

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



static void _change_bg_cb(system_settings_key_e key, void *data)
{
	_D("Background image is changed.");
	_create_bg();
}



static void _init_theme(void)
{
	menu_screen_info.theme = elm_theme_new();
	elm_theme_ref_set(menu_screen_info.theme, NULL);
	elm_theme_extension_add(menu_screen_info.theme, EDJEDIR"/index.edj");
}



static void _fini_theme(void)
{
	elm_theme_extension_del(menu_screen_info.theme, EDJEDIR"/index.edj");
	elm_theme_free(menu_screen_info.theme);
	menu_screen_info.theme = NULL;

}



static void _conform_resize_cb(void *data, Evas *e, Evas_Object *obj, void *event_info)
{
	int x, y, w, h;

	evas_object_geometry_get(obj, &x, &y, &w, &h);
	_D("conformant is resized : x[%d], y[%d], w[%d], h[%d]", x, y, w, h);
}



static Evas_Object *_create_conformant(Evas_Object *win)
{
	Evas_Object *conformant;

	conformant = elm_conformant_add(win);
	retv_if(NULL == conformant, NULL);

	evas_object_size_hint_weight_set(conformant, EVAS_HINT_EXPAND, EVAS_HINT_EXPAND);
	elm_win_indicator_mode_set(menu_screen_info.win, ELM_WIN_INDICATOR_SHOW);
	elm_object_signal_emit(conformant, "elm,state,indicator,overlap", "elm");
	evas_object_data_set(conformant, "win", win);
	evas_object_show(conformant);

	elm_win_resize_object_add(win, conformant);
	elm_win_conformant_set(win, EINA_TRUE);

	evas_object_event_callback_add(conformant, EVAS_CALLBACK_RESIZE, _conform_resize_cb, NULL);

	return conformant;
}



static void _destroy_conformant(Evas_Object *conformant)
{
	evas_object_data_del(conformant, "win");
	evas_object_del(conformant);
}



static bool _create_cb(void *data)
{
	Evas_Object *conformant;

	_init_theme();
	retv_if(MENU_SCREEN_ERROR_FAIL == _create_canvas(PACKAGE, PACKAGE), false);

	if (system_settings_set_changed_cb(SYSTEM_SETTINGS_KEY_WALLPAPER_HOME_SCREEN, _change_bg_cb, NULL) < 0) {
		_E("Failed to register a settings change cb for %d\n", SYSTEM_SETTINGS_KEY_WALLPAPER_HOME_SCREEN);
	}
	_create_bg();

	conformant = _create_conformant(menu_screen_info.win);
	retv_if(NULL == conformant, false);
	evas_object_data_set(menu_screen_info.win, "conformant", conformant);

	Evas_Object *layout;
	layout = layout_create(conformant, LAYOUT_EDJE_PORTRAIT,
				LAYOUT_GROUP_NAME, MENU_SCREEN_ROTATE_PORTRAIT);
	if (NULL == layout) {
		_E("Failed to load an edje object");
		evas_object_del(menu_screen_info.win);
		return false;
	}
	evas_object_data_set(menu_screen_info.win, "layout", layout);

	elm_object_content_set(conformant, layout);

	mouse_register();
	aul_listen_app_dead_signal(_dead_cb, NULL);
	key_register();

	// FIXME : This will be enabled after rebuilding the routine for appid <-> pkgid.
	pkgmgr_init();

	return true;
}



static void _terminate_cb(void *data)
{
	Evas_Object *conformant;
	Evas_Object *layout;

	// FIXME : This will be enabled after rebuilding the routine for appid <-> pkgid.
	pkgmgr_fini();

	if (system_settings_unset_changed_cb(SYSTEM_SETTINGS_KEY_WALLPAPER_HOME_SCREEN) < 0) {
		_E("Failed to remove bgset [%s]\n", SYSTEM_SETTINGS_KEY_WALLPAPER_HOME_SCREEN);
	}

	evas_object_hide(menu_screen_info.win);

	key_unregister();
	mouse_unregister();

	layout = evas_object_data_del(menu_screen_info.win, "layout");
	if (layout) layout_destroy(layout);

	conformant = evas_object_data_del(menu_screen_info.win, "conformant");
	if (conformant) _destroy_conformant(conformant);

	_destroy_bg();
	_destroy_canvas();
	_fini_theme();
	evas_object_del(menu_screen_info.win);
}



static void _pause_cb(void *data)
{
	_D("Pause start");

	menu_screen_info.state = APP_STATE_PAUSE;
}



static void _resume_cb(void *data)
{
	_D("START RESUME");

	do { // Focus
		Evas_Object *layout = evas_object_data_get(menu_screen_info.win, "layout");
		break_if(NULL == layout);

		Evas_Object *all_apps = evas_object_data_get(layout, "all_apps");
		break_if(NULL == all_apps);

		Evas_Object *scroller = elm_object_part_content_get(all_apps, "content");
		break_if(NULL == scroller);

		page_scroller_focus(scroller);
	} while (0);

	menu_screen_info.state = APP_STATE_RESUME;
}



static void _app_control_cb(app_control_h service, void *data)
{
	_D("START RESET : %d", menu_screen_info.state);

	do { // Focus
		Evas_Object *layout = evas_object_data_get(menu_screen_info.win, "layout");
		break_if(NULL == layout);

		Evas_Object *all_apps = evas_object_data_get(layout, "all_apps");
		break_if(NULL == all_apps);

		Evas_Object *scroller = elm_object_part_content_get(all_apps, "content");
		break_if(NULL == scroller);

		page_scroller_focus(scroller);
	} while (0);
}



static void _language_changed_cb(app_event_info_h event_info, void *data)
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
			pkgmgrinfo_appinfo_h appinfo_h = NULL;
			char *name;

			item = page_get_item_at(page, j);
			if (!item) continue;

			if (pkgmgrinfo_appinfo_get_usr_appinfo(item_get_package(item), getuid(), &appinfo_h) < 0) {
				pkgmgrinfo_appinfo_destroy_appinfo(appinfo_h);
				continue;
			}
			if (pkgmgrinfo_appinfo_get_label(appinfo_h, &name) < 0) {
				pkgmgrinfo_appinfo_destroy_appinfo(appinfo_h);
				continue;
			}

			if (!name) {
				_D("Failed to get name for %s", item_get_package(item));
				pkgmgrinfo_appinfo_destroy_appinfo(appinfo_h);
				continue;
			}

			item_set_name(item, name, 0);
			pkgmgrinfo_appinfo_destroy_appinfo(appinfo_h);
		}

		mapbuf_enable(page, 1);
	}
}



static void _init(ui_app_lifecycle_callback_s *lifecycle_callback, app_event_handler_h *event_handlers)
{
	lifecycle_callback->create = _create_cb;
	lifecycle_callback->terminate = _terminate_cb;
	lifecycle_callback->pause = _pause_cb;
	lifecycle_callback->resume = _resume_cb;
	lifecycle_callback->app_control = _app_control_cb;

	ui_app_add_event_handler(&event_handlers[APP_EVENT_LOW_BATTERY], APP_EVENT_LOW_BATTERY, NULL, NULL);
	ui_app_add_event_handler(&event_handlers[APP_EVENT_LOW_MEMORY], APP_EVENT_LOW_MEMORY, NULL, NULL);
	ui_app_add_event_handler(&event_handlers[APP_EVENT_DEVICE_ORIENTATION_CHANGED], APP_EVENT_DEVICE_ORIENTATION_CHANGED, NULL, NULL);
	ui_app_add_event_handler(&event_handlers[APP_EVENT_LANGUAGE_CHANGED], APP_EVENT_LANGUAGE_CHANGED, _language_changed_cb, NULL);
	ui_app_add_event_handler(&event_handlers[APP_EVENT_REGION_FORMAT_CHANGED], APP_EVENT_REGION_FORMAT_CHANGED, NULL, NULL);

}



static void _fini(app_event_handler_h *event_handlers)
{
	ui_app_remove_event_handler(event_handlers[APP_EVENT_LOW_BATTERY]);
	ui_app_remove_event_handler(event_handlers[APP_EVENT_LOW_MEMORY]);
	ui_app_remove_event_handler(event_handlers[APP_EVENT_DEVICE_ORIENTATION_CHANGED]);
	ui_app_remove_event_handler(event_handlers[APP_EVENT_LANGUAGE_CHANGED]);
	ui_app_remove_event_handler(event_handlers[APP_EVENT_REGION_FORMAT_CHANGED]);

}



int main(int argc, char *argv[])
{
	ui_app_lifecycle_callback_s lifecycle_callback = {0, };
	app_event_handler_h event_handlers[5] = {NULL, };

	_init(&lifecycle_callback, event_handlers);
	ui_app_main(argc, argv, &lifecycle_callback, NULL);
	_fini(event_handlers);

	return EXIT_SUCCESS;
}



// End of a file
