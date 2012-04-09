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



#include <Evas.h>

#include "util.h"



void _evas_object_resize_cb(void *data, Evas *e, Evas_Object *obj, void *event_info)
{
	Evas_Coord x;
	Evas_Coord y;
	Evas_Coord w;
	Evas_Coord h;

	evas_object_geometry_get(obj, &x, &y, &w, &h);
	_D("%s is resized to (%d, %d, %d, %d)", data, x, y, w, h);
}



void _evas_object_event_del_cb(void *data, Evas *e, Evas_Object *obj, void *event_info)
{
	_D("%s IS REMOVED!", (const char *) data);
}



void _evas_object_event_move_cb(void *data, Evas *e, Evas_Object *obj, void *event_info)
{
	Evas_Coord x;
	Evas_Coord y;
	Evas_Coord w;
	Evas_Coord h;

	evas_object_geometry_get(obj, &x, &y, &w, &h);
	_D("%s's GEOMETRY : [%d, %d, %d, %d]", (const char *) data, x, y, w, h);
}



void _evas_object_event_show_cb(void *data, Evas *e, Evas_Object *obj, void *event_info)
{
	Evas_Coord x;
	Evas_Coord y;
	Evas_Coord w;
	Evas_Coord h;

	evas_object_geometry_get(obj, &x, &y, &w, &h);
	_D("%s's GEOMETRY : [%d, %d, %d, %d]", (const char *) data, x, y, w, h);
}



// End of a file
