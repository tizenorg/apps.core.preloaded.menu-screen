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

#include "conf.h"
#include "util.h"
#include "page.h"
#include "layout.h"
#include "menu_screen.h"

#define PAGE_CONTROLLER_GROUP_NAME "index"



inline void index_bring_in(Evas_Object *index, int idx)
{
	Elm_Object_Item *idx_it;

	idx_it = elm_index_item_find(index, (void *) idx);
	ret_if(NULL == idx_it);
	elm_index_item_selected_set(idx_it, EINA_TRUE);
}



void index_destroy(Evas_Object *index)
{
	evas_object_data_del(index, "win");
	evas_object_data_del(index, "layout");
	evas_object_data_del(index, "controlbar");
	evas_object_data_del(index, "tab");
	evas_object_data_del(index, "count");
	elm_index_item_clear(index);
	evas_object_del(index);
}



Evas_Object *index_create(Evas_Object *tab, unsigned int count)
{
	Evas_Object *index;
	Evas_Object *scroller;
	register int i;
	char *number;

	retv_if(NULL == tab, NULL);
	index = elm_index_add(tab);
	retv_if(NULL == index, NULL);

	elm_object_style_set(index, "pagecontrol");
	elm_index_horizontal_set(index, EINA_TRUE);
	elm_index_autohide_disabled_set(index, EINA_TRUE);

	number = malloc(((size_t) log10((double) MAX_PAGE_NO)) + 2);
	retv_if(NULL == number, NULL);

	for (i = 0; i < count; i++) {
		sprintf(number, "%d", i + 1);
		elm_index_item_append(index, number, NULL, (void *) i);
	}
	elm_index_level_go(index, 0);

	if (count > 0) {
		index_bring_in(index, 0);
	}

	elm_object_part_content_set(tab, PAGE_CONTROLLER_GROUP_NAME, index);

	// FIXME : This part will be removed when fixing Elementary API for index_update.
	scroller = evas_object_data_get(tab, "scroller");
	if (scroller) {
		evas_object_data_set(scroller, "index", index);
	}

	evas_object_data_set(index, "win", evas_object_data_get(tab, "win"));
	evas_object_data_set(index, "layout", evas_object_data_get(tab, "layout"));
	evas_object_data_set(index, "controlbar", evas_object_data_get(tab, "controlbar"));
	evas_object_data_set(index, "tab", tab);
	evas_object_data_set(index, "count", (void *) count);

	evas_object_data_set(tab, "index", index);
	_D("COUNT:%d", count);

	evas_object_show(index);

	return index;
}



Evas_Object *index_update(Evas_Object *layout, Evas_Object *index, unsigned int count)
{

	index_destroy(index);
	return index_create(layout, count);
}



void index_hide(Evas_Object *index)
{
	edje_object_signal_emit(_EDJ(index), "hide", "index");
}



void index_show(Evas_Object *index)
{
	edje_object_signal_emit(_EDJ(index), "show", "index");
}



// End of the file
