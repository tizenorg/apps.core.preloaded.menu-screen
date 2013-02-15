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



#include <stdlib.h>
#include <Elementary.h>
#include <aul.h>
#include <ail.h>

#include "menu_screen.h"
#include "conf.h"
#include "item.h"
#include "item_event.h"
#include "layout.h"
#include "list.h"
#include "mapbuf.h"
#include "page.h"
#include "popup.h"
#include "util.h"

#define LINE_SIZE 10
#define LAYOUT_BLOCK_INTERVAL		1.0
#define ITEM_GROUP_NAME "icon"

#define STR_ATTRIBUTE_NAME "name"
#define STR_ATTRIBUTE_ICON "icon"
#define STR_ATTRIBUTE_PKG_NAME "package"
#define STR_ATTRIBUTE_REMOVABLE	"removable"
#define STR_ATTRIBUTE_DESKTOP "desktop"
#define STR_ATTRIBUTE_PAGE "pending,page"
#define STR_ICON_IMAGE_TYPE_OBJECT "object"
#define STR_ICON_IMAGE_TYPE_EDJE "edje"

#define BUFSZE 1024



#if 0 // Dual line is disabled temporarily.
static char *_space_to_new_line(const char *str)
{
	char *tmp;
	int len;
	int space_idx = 0;

	retv_if(NULL == str, NULL);
	len = strlen(str);

	if (len > LINE_SIZE) {
		for (space_idx = LINE_SIZE - 1; str[space_idx] != ' ' && space_idx > 0; space_idx--);
		if (!space_idx) {
			space_idx = strcspn(str, " ");
			space_idx = (space_idx == len ? 0 : space_idx);
		}
	}

	if (space_idx) {
		int i;
		retv_if(NULL == (tmp = calloc(1, len + 4)), NULL);

		for (i = 0; i < space_idx; i++) {
			tmp[i] = str[i];
		}
		strcat(tmp, "<br>");
		for (i = space_idx + 1; i < len; i++) {
			tmp[i + 3] = str[i];
		}
	} else {
		retv_if(NULL == (tmp = strdup(str)), NULL);
	}

	return tmp;
}
#endif



void item_set_icon(Evas_Object *edje, char *icon, int sync)
{
	char *tmp;
	int changed;

	tmp = evas_object_data_get(edje, STR_ATTRIBUTE_ICON);
	changed = (tmp && icon) ? strcmp(icon, tmp) : 1;

	if (!changed) {
		return;
	}

	free(tmp);
	evas_object_data_del(edje, STR_ATTRIBUTE_ICON);
	if (icon) {
		tmp = strdup(icon);
		if (tmp) {
			evas_object_data_set(edje, STR_ATTRIBUTE_ICON, tmp);
		}else {
			_E("No more memory for allocating space \"%s\"", icon);
		}
	}
}



inline char *item_get_icon(Evas_Object *edje)
{
	return evas_object_data_get(edje, STR_ATTRIBUTE_ICON);
}




void item_set_name(Evas_Object *edje, char *name, int sync)
{
	char *tmp;
	int changed;
	bool is_dual_line;

	tmp = evas_object_data_get(edje, STR_ATTRIBUTE_NAME);
	changed = (tmp && name) ? strcmp(name, tmp) : 1;

	if (!changed) {
		return;
	}

	free(tmp);
	evas_object_data_del(edje, STR_ATTRIBUTE_NAME);
	if (name) {
		tmp = strdup(name);
		if (tmp) {
			evas_object_data_set(edje, STR_ATTRIBUTE_NAME, tmp);
		}else {
			_E("No more memory for allocating space \"%s\"", name);
		}

		is_dual_line = (bool) evas_object_data_get(edje, "item_text_dual_line");
		if (is_dual_line) {
			char *dual_txt;

			# if 0 // Dual line is disabled temporarily.
			dual_txt = _space_to_new_line(name);
			#else
			dual_txt = strdup(name);
			#endif
			if (edje_object_part_text_set(_EDJ(edje), "txt", dual_txt) == EINA_FALSE){
				//_E("Failed to set text on the part");
			}
			free(dual_txt);
		} else {
			if (edje_object_part_text_set(_EDJ(edje), "txt", name) == EINA_FALSE){
				//_E("Failed to set text on the part");
			}
		}
	}
}



inline char *item_get_name(Evas_Object *edje)
{
	return evas_object_data_get(edje, STR_ATTRIBUTE_NAME);
}



void item_set_desktop(Evas_Object *edje, char *name, int sync)
{
	char *tmp;
	int changed;

	tmp = evas_object_data_get(edje, STR_ATTRIBUTE_DESKTOP);
	changed = (tmp && name) ? strcmp(name, tmp) : 1;	// We have to do sync when an attribute is created

	if (!changed) {
		return;
	}

	free(tmp);	// NOTE: I can accept this, "free" will not do anything for NULL
	evas_object_data_del(edje, STR_ATTRIBUTE_DESKTOP);
	if (name) {
		tmp = strdup(name);
		if (!tmp) {
			_E("No more memory for allocating space \"%s\"", name);
		} else {
			evas_object_data_set(edje, STR_ATTRIBUTE_DESKTOP, tmp);
		}
	}
}



inline char *item_get_desktop(Evas_Object *edje)
{
	return evas_object_data_get(edje, STR_ATTRIBUTE_DESKTOP);
}



void item_set_package(Evas_Object *edje, char *package, int sync)
{
	char *tmp;
	int changed;

	tmp = evas_object_data_get(edje, STR_ATTRIBUTE_PKG_NAME);
	changed = (package && tmp) ? strcmp(tmp, package) : 1;

	if (!changed) {
		return;
	}

	free(tmp);
	evas_object_data_del(edje, STR_ATTRIBUTE_PKG_NAME);

	if (package) {
		tmp = strdup(package);
		if (!tmp) {
			_E("No more space for string \"%s\"", package);
		} else {
			evas_object_data_set(edje, STR_ATTRIBUTE_PKG_NAME, tmp);
		}
	}
}



inline char *item_get_package(Evas_Object *edje)
{
	return evas_object_data_get(edje, STR_ATTRIBUTE_PKG_NAME);
}



void item_set_removable(Evas_Object *edje, int removable, int sync)
{
	int value;
	int changed;

	value = (int)evas_object_data_get(edje, STR_ATTRIBUTE_REMOVABLE);
	changed = (int)(value != removable);

	if (!changed) {
		return;
	}

	evas_object_data_del(edje, STR_ATTRIBUTE_REMOVABLE);

	if (removable >= 0) {
		evas_object_data_set(edje, STR_ATTRIBUTE_REMOVABLE, (void*) removable);
	}
}



inline int item_get_removable(Evas_Object *edje)
{
	return (int) evas_object_data_get(edje, STR_ATTRIBUTE_REMOVABLE);
}



void item_set_page(Evas_Object *edje, Evas_Object *page, int sync)
{
	Evas_Object *value;
	int changed;

	value = evas_object_data_get(edje, STR_ATTRIBUTE_PAGE);
	changed = (int)(value != page);

	if (!changed) {
		return;
	}

	evas_object_data_del(edje, STR_ATTRIBUTE_PAGE);

	if (page) {
		evas_object_data_set(edje, STR_ATTRIBUTE_PAGE, page);
	}
}



inline Evas_Object *item_get_page(Evas_Object *edje)
{
	return evas_object_data_get(edje, STR_ATTRIBUTE_PAGE);
}




inline void item_enable_delete(Evas_Object *item)
{
	if (item_get_removable(item) > 0) {
		edje_object_signal_emit(_EDJ(item), "delete,on", "menu");
	}
}



inline void item_disable_delete(Evas_Object *item)
{
	if (item_get_removable(item) > 0) {
		edje_object_signal_emit(_EDJ(item), "delete,off", "menu");
	}
}


void item_enable_progress(Evas_Object *obj)
{
	Evas_Object *progress;

	ret_if(evas_object_data_get(obj, "progress,enabled"));

	progress = elm_object_part_content_unset(obj, "progress,swallow");
	if (progress) {
		_D("Progress bar is already registered... Hmm.. just remove it");
		evas_object_del(progress);
	}

	progress = elm_progressbar_add(obj);
	ret_if(NULL == progress);

	elm_object_part_content_set(obj, "progress,swallow", progress);

	evas_object_size_hint_weight_set(progress, EVAS_HINT_EXPAND, EVAS_HINT_EXPAND);
	evas_object_size_hint_align_set(progress, EVAS_HINT_FILL, EVAS_HINT_FILL);

	elm_progressbar_value_set(progress, 0.0f);
	elm_progressbar_horizontal_set(progress, EINA_TRUE);
	evas_object_show(progress);

	edje_object_signal_emit(_EDJ(obj), "progress,enable", "item");

	evas_object_data_set(obj, "progress,enabled", "true");
}



void item_update_progress(Evas_Object *obj, int value)
{
	Evas_Object *progress;

	ret_if(NULL == evas_object_data_get(obj, "progress,enabled"));

	progress = edje_object_part_swallow_get(_EDJ(obj), "progress,swallow");
	ret_if(NULL == progress);

	elm_progressbar_value_set(progress, (float)value/100.0f);

	_D("progress is updated to %d", value);
}



void item_disable_progress(Evas_Object *obj)
{
	Evas_Object *progress;

	ret_if(NULL == evas_object_data_get(obj, "progress,enabled"));

	edje_object_signal_emit(_EDJ(obj), "progress,disable", "item");

	progress = elm_object_part_content_unset(obj, "progress,swallow");
	if (progress) {
		evas_object_del(progress);
	}

	evas_object_data_del(obj, "progress,enabled");
}



int item_is_enabled_progress(Evas_Object *obj)
{
	return evas_object_data_get(obj, "progress,enabled") != NULL;
}



static Evas_Object *_add_icon_image(Evas_Object *item, const char *icon_file)
{
	Evas_Object *icon;

	icon = elm_icon_add(item);
	if (elm_image_file_set(icon, icon_file, NULL) == EINA_FALSE) {
		_E("Icon file is not accessible (%s)", icon_file);
		evas_object_del(icon);
		icon = NULL;
	}

	elm_image_resizable_set(icon, EINA_TRUE, EINA_TRUE);

	if (menu_screen_get_root_height() > BASE_HEIGHT || menu_screen_get_root_width() > BASE_WIDTH) {
		elm_image_no_scale_set(icon, EINA_TRUE);
	}

	return icon;
}



static Evas_Object *_add_edje_icon(Evas_Object *item, const char *icon_file)
{
	Evas_Object *icon;
	if (access(icon_file, R_OK) != 0) {
		_E("Failed to get an icon");
		return NULL;
	}

	icon = layout_load_edj(item, (char*)icon_file, ITEM_GROUP_NAME);
	if (!icon) {
		_E("Failed to load an edje, [%s] group icon", icon_file);
		evas_object_del(icon);
		icon = NULL;
	}

	return icon;
}



menu_screen_error_e item_is_edje_icon(const char *icon)
{
	int len;
	const char *ext = "jde.";

	retv_if(!icon, MENU_SCREEN_ERROR_FAIL);

	len = strlen(icon) - 1;

	while (len >= 0 && *ext && icon[len] == *ext) {
		len --;
		ext ++;
	}

	return *ext ? MENU_SCREEN_ERROR_FAIL : MENU_SCREEN_ERROR_OK;
}



void item_update(Evas_Object *item, app_info_t *ai)
{
	Evas_Object *icon;

	if (!ai->image) {
		if (item_is_edje_icon(ai->icon) == MENU_SCREEN_ERROR_OK) {
			icon = _add_edje_icon(item, ai->icon);
			evas_object_data_set(item, "icon_image_type", STR_ICON_IMAGE_TYPE_EDJE);
		} else {
			icon = _add_icon_image(item, ai->icon);
			evas_object_data_set(item, "icon_image_type", STR_ICON_IMAGE_TYPE_OBJECT);
		}
	} else {
		icon = ai->image;
		evas_object_data_set(item, "icon_image_type", STR_ICON_IMAGE_TYPE_OBJECT);
	}

	if (icon) {
		Evas_Object *temp_item;
		temp_item = elm_object_part_content_unset(item, "icon_image");
		if (temp_item) {
			evas_object_del(temp_item);
		}

		elm_object_part_content_set(item, "icon_image", icon);
		evas_object_data_set(item, "icon_image", icon);
	}

	item_set_package(item, ai->package, 0);
	item_set_desktop(item, ai->desktop, 0);
	item_set_name(item, ai->name, 0);
	item_set_icon(item, ai->icon, 0);
	evas_object_data_set(item, STR_ATTRIBUTE_REMOVABLE, (void*) ai->x_slp_removable);
	evas_object_data_set(item, "pid", (void *) ai->pid);
}



Evas_Object *item_create(Evas_Object *scroller, app_info_t* ai)
{
	Evas_Object *item;
	Evas_Object *bg;

	char *item_edje;
	int item_width;
	int item_height;

	item_edje = evas_object_data_get(scroller, "item_edje");
	item = layout_load_edj(scroller, item_edje, ITEM_GROUP_NAME);
	if (!item) {
		_E("Failed to load an item object");
		return NULL;
	}

	bg = evas_object_rectangle_add(menu_screen_get_evas());
	if (!bg) {
		evas_object_del(item);
		return NULL;
	}

	item_width = (int) evas_object_data_get(scroller, "item_width");
	item_height = (int) evas_object_data_get(scroller, "item_height");

	evas_object_color_set(bg, 0, 0, 0, 0);
	evas_object_resize(bg, item_width, item_height);
	evas_object_size_hint_min_set(bg, item_width, item_height);
	evas_object_size_hint_max_set(bg, item_width, item_height);
	elm_object_part_content_set(item, "bg", bg);

	edje_object_text_class_set(_EDJ(item), "menu_item", "TIZEN:style=medium", 24);

	evas_object_data_set(item, "win", evas_object_data_get(scroller, "win"));
	evas_object_data_set(item, "layout", evas_object_data_get(scroller, "layout"));
	evas_object_data_set(item, "controlbar", evas_object_data_get(scroller, "controlbar"));
	evas_object_data_set(item, "tab", evas_object_data_get(scroller, "tab"));
	evas_object_data_set(item, "scroller", scroller);
	evas_object_data_set(item, "item_enable_long_press", evas_object_data_get(scroller, "item_enable_long_press"));
	evas_object_data_set(item, "item_text_dual_line", evas_object_data_get(scroller, "item_text_dual_line"));
	evas_object_data_set(item, "pending,idx", (void *) 0);
	evas_object_data_set(item, "x", (void *) 0);
	evas_object_data_set(item, "y", (void *) 0);
	evas_object_data_set(item, "dirty", (void *) 0);

	item_update(item, ai);
	item_event_register(item);

	return item;
}



void item_destroy(Evas_Object *item)
{
	Evas_Object *icon;
	Evas_Object *bg;
	Evas_Object *page;
	Eina_List *pending_list;
	Eina_List *n;
	Eina_List *t;
	Evas_Object *pend_item;
	int pending_idx;
	const char *icon_image_type;

	ret_if(NULL == item);

	page = item_get_page(item);
	pending_list = evas_object_data_get(page, "pending,list");
	EINA_LIST_FOREACH_SAFE(pending_list, n, t, pend_item) {
		if (pend_item == item) {
			pending_idx = (int)evas_object_data_get(pend_item, "pending,idx");
			pending_list = eina_list_remove(pending_list, pend_item);
			evas_object_data_set(page, "pending,list", pending_list);
			evas_object_data_del(pend_item, "pending,idx");
		}
	}

	item_event_unregister(item);

	item_set_package(item, NULL, 1);
	item_set_desktop(item, NULL, 1);
	item_set_name(item, NULL, 1);
	item_set_icon(item, NULL, 1);
	item_set_page(item, NULL, 1);
	evas_object_data_del(item, STR_ATTRIBUTE_REMOVABLE);
	evas_object_data_del(item, "pid");
	evas_object_data_del(_EDJ(item), "evas_object");
	bg = elm_object_part_content_unset(item, "bg");
	if (bg) {
		evas_object_del(bg);
	}

	elm_object_part_content_unset(item, "icon_image");
	icon = evas_object_data_del(item, "icon_image");
	icon_image_type = evas_object_data_get(item, "icon_image_type");
	if (icon_image_type) {
		if (!strcmp(icon_image_type, STR_ICON_IMAGE_TYPE_OBJECT)) {
			evas_object_del(icon);
		} else if (!strcmp(icon_image_type, STR_ICON_IMAGE_TYPE_EDJE)) {
			layout_unload_edj(icon);
		}
	}

	evas_object_data_del(item, "win");
	evas_object_data_del(item, "layout");
	evas_object_data_del(item, "controlbar");
	evas_object_data_del(item, "tab");
	evas_object_data_del(item, "scroller");
	evas_object_data_del(item, "item_enable_long_press");
	evas_object_data_del(item, "item_text_dual_line");
	evas_object_data_del(item, "pending,idx");
	evas_object_data_del(item, "x");
	evas_object_data_del(item, "y");
	evas_object_data_del(item, "dirty");
	layout_unload_edj(item);
}



static Eina_Bool _unblock_cb(void *data)
{
	Evas_Object *layout;
	layout = evas_object_data_get(menu_screen_get_win(), "layout_current");
	layout_disable_block(layout);
	return EINA_FALSE;
}



void item_launch(Evas_Object *obj)
{
	char *package;
	char *name;
	int ret_aul;
	Evas_Object *layout;

	ret_if(NULL == obj);

	name = item_get_name(obj);
	package = item_get_package(obj);
	ret_if(NULL == package);

	layout = evas_object_data_get(menu_screen_get_win(), "layout_current");
	layout_enable_block(layout);

	ret_aul = aul_open_app(package);
	if (ret_aul == AUL_R_EINVAL) {
		char* sinform;
		int len;

		// IDS_IDLE_POP_UNABLE_TO_LAUNCH_PS : "Unable to launch %s"
		len = strlen(D_("IDS_IDLE_POP_UNABLE_TO_LAUNCH_PS")) + strlen(name) + 1;

		sinform = calloc(len, sizeof(char));
		if (!sinform) {
			_E("cannot calloc for popup.");
			return;
		}

		snprintf(sinform, len, D_("IDS_IDLE_POP_UNABLE_TO_LAUNCH_PS"), name);
		popup_create(obj, sinform);

		free(sinform);
		layout_disable_block(layout);
	} else {
		_D("Launch app's ret : [%d]", ret_aul);
		_T(package);
		ecore_timer_add(LAYOUT_BLOCK_INTERVAL, _unblock_cb, NULL);
	}
}



int item_get_position(Evas_Object *item)
{
	Evas_Object *scroller;
	Evas_Object *layout;

	Evas_Coord item_x;
	Evas_Coord item_y;

	Evas_Coord scroller_x;
	Evas_Coord scroller_y;
	Evas_Coord scroller_w;
	Evas_Coord scroller_h;

	int layout_width;

	int items_per_line;
	int nth_line;
	int nth_item;

	int item_width;
	int item_height;

	layout = evas_object_data_get(item, "layout");
	scroller = evas_object_data_get(item, "scroller");

	layout_width = (int) evas_object_data_get(layout, "width");
	item_width = (int) evas_object_data_get(scroller, "item_width");
	item_height = (int) evas_object_data_get(scroller, "item_height");

	evas_object_geometry_get(item, &item_x, &item_y, NULL, NULL);
	evas_object_geometry_get(scroller, &scroller_x, &scroller_y, &scroller_w, &scroller_h);

	if (item_x < ((float) layout_width) * 0.005f) {
		return -1;
	}

	if (item_x + ((float) item_width * 0.85f) > ((float) layout_width) * 0.98f) {
		return (int) evas_object_data_get(scroller, "page_max_app");
	}

	if (0 == item_width || 0 == item_height) {
		_E("item_widht or item_heiht is zero.");
		return 0;
	}

	items_per_line = scroller_w / item_width;
	nth_line = (item_y - (float) item_height * 0.3) / item_height;
	nth_line = nth_line < 0 ? 0 : nth_line;
	nth_item = (item_x + (float) item_width * 0.5) / item_width;

	return (nth_line * items_per_line) + nth_item;
}



void item_mark_dirty(Evas_Object *item)
{
	evas_object_data_set(item, "dirty", (void *) 1);
}



void item_unmark_dirty(Evas_Object *item)
{
	evas_object_data_set(item, "dirty", (void *) 0);
}



int item_is_dirty(Evas_Object *item)
{
	return (int) evas_object_data_get(item, "dirty");
}



// End of a file
