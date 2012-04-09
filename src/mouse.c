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



bool mouse_is_scrolling(void)
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



void mouse_register(void)
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



void mouse_unregister(void)
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
