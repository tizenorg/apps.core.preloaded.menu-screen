/*
 * Samsung API
 * Copyright (c) 2009-2015 Samsung Electronics Co., Ltd.
 *
 * Licensed under the Apache License, Version 2.0 (the License);
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 * http://www.apache.org/license/
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an AS IS BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#include <app_control.h>
#include <Elementary.h>
#include <ui-gadget-module.h>

#include "smart_menu.h"
#include "smart_menu_internal.h"
#include "layout.h"
#include "log.h"

#define EAPI __attribute__ ((visibility("default")))

#define MENU_SCREEN_CLASS_NAME "smart-menu"
#define MENU_SCREEN_DOMAIN "smart-menu"



static struct {
	Evas_Smart_Class smart_class;
	Evas_Smart *smart;
	Evas_Object *conformant;
} smart_menu_class_info = {
	.smart_class = EVAS_SMART_CLASS_INIT_NAME_VERSION(MENU_SCREEN_CLASS_NAME),
	.smart = NULL,
	.conformant = NULL,
};



static void __smart_menu_add(Evas_Object *smart_menu)
{
	smart_menu_s *smart_menu_info = NULL;

	ret_if(!smart_menu);

	smart_menu_info = calloc(1, sizeof(smart_menu_s));
	ret_if(!smart_menu_info);
	evas_object_smart_data_set(smart_menu, smart_menu_info);
	evas_object_size_hint_weight_set(smart_menu, EVAS_HINT_EXPAND, EVAS_HINT_EXPAND);
	//elm_object_part_content_set(smart_menu_class_info.conformant, "elm.swallow.smart_menu", smart_menu);

	smart_menu_info->smart_menu = smart_menu;
	smart_menu_info->conformant = smart_menu_class_info.conformant;
	smart_menu_info->layout = _layout_create(smart_menu, smart_menu_info);
	if (!smart_menu_info->layout) {
		free(smart_menu_info);
	}

	_layout_create(smart_menu_info);
	_smart_menu_show(smart_menu_info);
}



static void __smart_menu_del(Evas_Object *smart_menu)
{
	smart_menu_s *smart_menu_info = NULL;

	ret_if(!smart_menu);

	smart_menu_info = evas_object_smart_data_get(smart_menu);
	ret_if(!smart_menu_info);

	_layout_destroy(smart_menu_info);
	free(smart_menu_info);
}



static void __smart_menu_move(Evas_Object *smart_menu, Evas_Coord x, Evas_Coord y)
{
	smart_menu_s *smart_menu_info = NULL;

	ret_if(!smart_menu);

	smart_menu_info = evas_object_smart_data_get(smart_menu);
	ret_if(!smart_menu_info);
	ret_if(!smart_menu_info->layout);

	evas_object_move(smart_menu_info->layout, x, y);
}



static void __smart_menu_resize(Evas_Object *smart_menu, Evas_Coord w, Evas_Coord h)
{
	smart_menu_s *smart_menu_info = NULL;

	ret_if(!smart_menu);

	smart_menu_info = evas_object_smart_data_get(smart_menu);
	ret_if(!smart_menu_info);
	ret_if(!smart_menu_info->layout);

	evas_object_resize(smart_menu_info->layout, w, h);
}



static void __smart_menu_show(Evas_Object *smart_menu)
{
	smart_menu_s *smart_menu_info = NULL;

	ret_if(!smart_menu);

	smart_menu_info = evas_object_smart_data_get(smart_menu);
	ret_if(!smart_menu_info);
	ret_if(!smart_menu_info->layout);

	evas_object_show(smart_menu_info->layout);
}



static void __smart_menu_hide(Evas_Object *smart_menu)
{
	smart_menu_s *smart_menu_info = NULL;

	ret_if(!smart_menu);

	smart_menu_info = evas_object_smart_data_get(smart_menu);
	ret_if(!smart_menu_info);
	ret_if(!smart_menu_info->layout);

	evas_object_hide(smart_menu_info->layout);
}



static void __smart_menu_color_set(Evas_Object *smart_menu, int r, int g, int b, int a)
{
	smart_menu_s *smart_menu_info = NULL;

	ret_if(!smart_menu);

	smart_menu_info = evas_object_smart_data_get(smart_menu);
	ret_if(!smart_menu_info);
	ret_if(!smart_menu_info->layout);

	evas_object_color_set(smart_menu_info->layout, r, g, b, a);
}



static void __smart_menu_clip_set(Evas_Object *smart_menu, Evas_Object *clip)
{
	smart_menu_s *smart_menu_info = NULL;

	ret_if(!smart_menu);

	smart_menu_info = evas_object_smart_data_get(smart_menu);
	ret_if(!smart_menu_info);
	ret_if(!smart_menu_info->layout);

	evas_object_clip_set(smart_menu_info->layout, clip);
}



static void __smart_menu_clip_unset(Evas_Object *smart_menu)
{
	smart_menu_s *smart_menu_info = NULL;

	ret_if(!smart_menu);

	smart_menu_info = evas_object_smart_data_get(smart_menu);
	ret_if(!smart_menu_info);
	ret_if(!smart_menu_info->layout);

	evas_object_clip_unset(smart_menu_info->layout);
}



EAPI Evas_Object *smart_menu_add(Evas_Object *conformant)
{
	Evas_Object *smart_menu = NULL;
	Evas *e = NULL;

	retv_if(!conformant, NULL);

	bindtextdomain(MENU_SCREEN_DOMAIN, LOCALEDIR);

	e = evas_object_evas_get(conformant);
	retv_if(!e, NULL);

	smart_menu_class_info.conformant = conformant;

	/* We need only one attach panel class even if there are many attach panels */
	if (!smart_menu_class_info.smart) {
		smart_menu_class_info.smart_class.add = __smart_menu_add;
		smart_menu_class_info.smart_class.del = __smart_menu_del;
		smart_menu_class_info.smart_class.move = __smart_menu_move;
		smart_menu_class_info.smart_class.resize = __smart_menu_resize;
		smart_menu_class_info.smart_class.show = __smart_menu_show;
		smart_menu_class_info.smart_class.hide = __smart_menu_hide;
		smart_menu_class_info.smart_class.color_set = __smart_menu_color_set;
		smart_menu_class_info.smart_class.clip_set = __smart_menu_clip_set;
		smart_menu_class_info.smart_class.clip_unset = __smart_menu_clip_unset;
		smart_menu_class_info.smart = evas_smart_class_new(&smart_menu_class_info.smart_class);
	}

	smart_menu = evas_object_smart_add(e, smart_menu_class_info.smart);
	retv_if(!smart_menu, NULL);

	return smart_menu;
}



// End of this file
