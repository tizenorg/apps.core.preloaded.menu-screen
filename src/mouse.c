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

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <assert.h>
#include <Elementary.h>

#include "menu_screen.h"
#include "conf.h"
#include "item.h"
#include "mouse.h"
#include "page.h"
#include "page_scroller.h"
#include "util.h"

#define CHAGNE_POSITION_TIME 0.1f
#define CHANGE_POSITION_COUNT 5
#define MOVE_THRESHOLD 10



static struct {
	Ecore_Event_Handler *mouse_down;
	Ecore_Event_Handler *mouse_up;
	Ecore_Event_Handler *mouse_move;
	bool pressed;

	Evas_Coord pre_x;
	Evas_Coord pre_y;
	bool is_initialized;

	Evas_Coord down_x;
	Evas_Coord down_y;
	Evas_Coord move_x;
	Evas_Coord move_y;
} mouse_info = {
	.mouse_down = NULL,
	.mouse_up = NULL,
	.mouse_move = NULL,
	.pressed = false,

	.pre_x = 0,
	.pre_y = 0,
	.is_initialized = false,

	.down_x = 0,
	.down_y = 0,
	.move_x = 0,
	.move_y = 0,
};



HAPI bool mouse_is_scrolling(void)
{
	bool scroll_x = false;
	bool scroll_y = false;

	if (mouse_info.move_x > mouse_info.down_x + MOVE_THRESHOLD
		|| mouse_info.move_x < mouse_info.down_x - MOVE_THRESHOLD)
	{
		scroll_x = true;
	}

	if (mouse_info.move_y > mouse_info.down_y + MOVE_THRESHOLD
		|| mouse_info.move_y < mouse_info.down_y - MOVE_THRESHOLD)
	{
		scroll_y = true;
	}

	return scroll_x || scroll_y;
}



static Eina_Bool _down_cb(void *data, int type, void *event)
{
	Ecore_Event_Mouse_Button *move = event;

	retv_if(true == mouse_info.pressed, ECORE_CALLBACK_RENEW);

	_D("Mouse down (%d,%d)", move->root.x, move->root.y);

	mouse_info.pressed = true;
	mouse_info.is_initialized = false;

	mouse_info.down_x = move->root.x;
	mouse_info.down_y = move->root.y;

	return ECORE_CALLBACK_RENEW;
}



static Eina_Bool _up_cb(void *data, int type, void *event)
{
	Ecore_Event_Mouse_Button *move = event;

	_D("Mouse up (%d,%d)", move->root.x, move->root.y);

	retv_if(mouse_info.pressed == false, ECORE_CALLBACK_RENEW);
	mouse_info.pressed = false;
	mouse_info.pre_x = 0;
	mouse_info.pre_y = 0;

	return ECORE_CALLBACK_RENEW;
}



static Eina_Bool _move_cb(void *data, int type, void *event)
{
	Ecore_Event_Mouse_Move *move = event;

	mouse_info.move_x = move->root.x;
	mouse_info.move_y = move->root.y;

	if (mouse_info.pressed == false) {
		return ECORE_CALLBACK_RENEW;
	}

	return ECORE_CALLBACK_RENEW;
}



HAPI void mouse_register(void)
{
	mouse_info.mouse_down = ecore_event_handler_add(ECORE_EVENT_MOUSE_BUTTON_DOWN, _down_cb, NULL);
	if (!mouse_info.mouse_down) {
		_D("Failed to register the mouse down event callback");
	}

	mouse_info.mouse_move = ecore_event_handler_add(ECORE_EVENT_MOUSE_MOVE, _move_cb, NULL);
	if (!mouse_info.mouse_move) {
		_D("Failed to register the mouse move event callback");
		ecore_event_handler_del(mouse_info.mouse_down);
		mouse_info.mouse_down = NULL;
	}

	mouse_info.mouse_up = ecore_event_handler_add(ECORE_EVENT_MOUSE_BUTTON_UP, _up_cb, NULL);
	if (!mouse_info.mouse_up) {
		_D("Failed to register the mouse up event callback");
		ecore_event_handler_del(mouse_info.mouse_down);
		ecore_event_handler_del(mouse_info.mouse_move);

		mouse_info.mouse_down = NULL;
		mouse_info.mouse_move = NULL;
	}
}



HAPI void mouse_unregister(void)
{
	if (mouse_info.mouse_down) {
		ecore_event_handler_del(mouse_info.mouse_down);
		mouse_info.mouse_down = NULL;
	}

	if (mouse_info.mouse_up) {
		ecore_event_handler_del(mouse_info.mouse_up);
		mouse_info.mouse_up = NULL;
	}

	if (mouse_info.mouse_move) {
		ecore_event_handler_del(mouse_info.mouse_move);
		mouse_info.mouse_move = NULL;
	}
}



// End of a file
