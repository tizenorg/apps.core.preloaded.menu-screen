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
#include <utilX.h>
#include <vconf.h>

#include "menu_screen.h"
#include "conf.h"
#include "item_event.h"
#include "layout.h"
#include "mapbuf.h"
#include "page.h"
#include "item.h"
#include "util.h"
#include "all_apps/layout.h"



Evas_Object *layout_create(Evas_Object *win, const char *file, const char *group, int rotate)
{
	Evas_Object *layout;
	Evas_Object *all_apps;
	int width;
	int height;

	layout = layout_load_edj(win, file, group);
	retv_if(NULL == layout, NULL);

	evas_object_size_hint_weight_set(layout, EVAS_HINT_EXPAND, EVAS_HINT_EXPAND);
	elm_win_resize_object_add(win, layout);

	evas_object_data_set(layout, "win", win);
	evas_object_data_set(layout, "rotate", (void *) rotate);

	width = menu_screen_get_root_width();
	height = menu_screen_get_root_height();

	evas_object_data_set(layout, "width", (void *) width);
	evas_object_data_set(layout, "height", (void *) height);

	all_apps = all_apps_layout_create(layout, rotate);
	if (NULL == all_apps) {
		_E("Failed to create scroller");
		layout_destroy(layout);
		return NULL;
	}
	evas_object_data_set(layout, "all_apps", all_apps);
	elm_object_part_content_set(layout, "content", all_apps);

	return layout;
}



void layout_destroy(Evas_Object *layout)
{
	Evas_Object *all_apps;

	all_apps = evas_object_data_get(layout, "all_apps");
	all_apps_layout_destroy(all_apps);

	evas_object_data_del(layout, "win");
	evas_object_data_del(layout, "all_apps");
	evas_object_data_del(layout, "rotate");
	evas_object_data_del(layout, "width");
	evas_object_data_del(layout, "height");

	layout_unload_edj(layout);
}



void layout_enable_block(Evas_Object *layout)
{
	ret_if(NULL == layout);

	_D("Enable layout blocker");
	edje_object_signal_emit(_EDJ(layout), "block", "layout");
}



void layout_disable_block(Evas_Object *layout)
{
	ret_if(NULL == layout);

	_D("Disable layout blocker");
	edje_object_signal_emit(_EDJ(layout), "unblock", "layout");
}



Evas_Object* layout_load_edj(Evas_Object *parent, const char *edjname, const char *grpname)
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



void layout_unload_edj(Evas_Object *layout)
{
	Evas_Object *evas_object;

	ret_if(NULL == layout);

	evas_object = evas_object_data_get(_EDJ(layout), "evas_object");
	if (evas_object) {
		evas_object_data_del(_EDJ(layout), "evas_object");
	}

	evas_object_del(layout);
}



// End of file
