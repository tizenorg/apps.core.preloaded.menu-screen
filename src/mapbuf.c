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

#include "menu_screen.h"
#include "conf.h"
#include "layout.h"
#include "mapbuf.h"
#include "util.h"



Evas_Object *mapbuf_get_mapbuf(Evas_Object *obj)
{
	Evas_Object *mapbuf;

	if (obj == NULL) return NULL;

	mapbuf = evas_object_data_get(obj, "mapbuf");
	if (!mapbuf && evas_object_data_get(obj, "page")) {
		mapbuf = obj;
	}

	return mapbuf;
}



Evas_Object *mapbuf_get_page(Evas_Object *obj)
{
	Evas_Object *page;

	if (obj == NULL) return NULL;

	page = evas_object_data_get(obj, "page");
	if (!page && evas_object_data_get(obj, "mapbuf")) {
		page = obj;
	}

	return page;
}



menu_screen_error_e mapbuf_enable(Evas_Object *obj, int force)
{
	Evas_Object *mapbuf;
	int cnt;

	mapbuf = mapbuf_get_mapbuf(obj);
	if (!mapbuf) {
		_D("Failed to get the mapbuf object");
		return MENU_SCREEN_ERROR_FAIL;
	}

	if (force) {
		evas_object_data_set(mapbuf, "mapbuf_enabled", (void*)0);
		elm_mapbuf_enabled_set(mapbuf, 1);
		return MENU_SCREEN_ERROR_OK;
	}

	cnt = (int)evas_object_data_get(mapbuf, "mapbuf_enabled");
	cnt ++;
	evas_object_data_set(mapbuf, "mapbuf_enabled", (void*)cnt);
	//_D("[%s] CNT = %d", __func__, cnt);

	if (cnt == 0) {
		if (!elm_mapbuf_enabled_get(mapbuf)) {
			elm_mapbuf_enabled_set(mapbuf, 1);
			//_D("[%s] mapbuf enabled", __func__);
		}
	}

	return MENU_SCREEN_ERROR_OK;
}



int mapbuf_is_enabled(Evas_Object *obj)
{
	Evas_Object *mapbuf;
	mapbuf = mapbuf_get_mapbuf(obj);
	if (!mapbuf) {
		return 0;
	}

	return elm_mapbuf_enabled_get(mapbuf);
}



int mapbuf_disable(Evas_Object *obj, int force)
{
	Evas_Object *mapbuf;
	int cnt;

	mapbuf = mapbuf_get_mapbuf(obj);
	if (!mapbuf) {
		_D("Failed to get the mapbuf object");
		return MENU_SCREEN_ERROR_FAIL;
	}

	if (force) {
		evas_object_data_set(mapbuf, "mapbuf_enabled", (void*)-1);
		elm_mapbuf_enabled_set(mapbuf, 0);
		return MENU_SCREEN_ERROR_OK;
	}

	cnt = (int)evas_object_data_get(mapbuf, "mapbuf_enabled");
	if (cnt == 0) {
		if (elm_mapbuf_enabled_get(mapbuf)) {
			elm_mapbuf_enabled_set(mapbuf, 0);
			//_D("[%s] disableld mapbuf", __func__);
		}
	}

	cnt --;
	evas_object_data_set(mapbuf, "mapbuf_enabled", (void*)cnt);
	//_D("[%s] CNT = %d", __func__, cnt);

	return MENU_SCREEN_ERROR_OK;
}



Evas_Object *mapbuf_bind(Evas_Object *box, Evas_Object *page)
{
	Evas_Object *mapbuf;

	mapbuf = elm_mapbuf_add(box);
	if (!mapbuf) {
		_D("Failed to create a new mapbuf");
		return NULL;
	}

	elm_mapbuf_smooth_set(mapbuf, EINA_TRUE);
	elm_mapbuf_alpha_set(mapbuf, EINA_TRUE);
	elm_object_content_set(mapbuf, page);

	evas_object_data_set(page, "mapbuf", mapbuf);
	evas_object_data_set(mapbuf, "page", page);
	mapbuf_disable(mapbuf, 1);
	evas_object_show(mapbuf);

	return mapbuf;
}



Evas_Object *mapbuf_unbind(Evas_Object *obj)
{
	Evas_Object *page;
	Evas_Object *mapbuf;

	page = evas_object_data_get(obj, "page");
	if (page) {
		mapbuf = obj;
	} else {
		page = obj;
		mapbuf = evas_object_data_get(obj, "mapbuf");
	}

	if (mapbuf) {
		elm_mapbuf_enabled_set(mapbuf, 0);
		evas_object_data_del(page, "mapbuf");
		evas_object_data_del(mapbuf, "page");
		evas_object_data_del(mapbuf, "mapbuf_enabled");
		page = elm_object_content_unset(mapbuf);
		evas_object_del(mapbuf);
	}
	return page;
}



// End of a file
