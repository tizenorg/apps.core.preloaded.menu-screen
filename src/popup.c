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
#include "menu_screen.h"
#include "util.h"



static void _response_cb(void *data, Evas_Object *obj, void *event_info)
{
	Evas_Object *popup;

	ret_if(NULL == data);
	popup = data;
	evas_object_del(evas_object_data_del(popup, "button"));
	evas_object_del(popup);
}



Evas_Object *popup_create(Evas_Object *parent, const char *warning)
{
	Evas_Object *popup;
	Evas_Object *btn;

	retv_if(NULL == warning, NULL);

	popup = elm_popup_add(parent);
	retv_if(NULL == popup, NULL);

	btn = elm_button_add(popup);
	if (NULL == btn) {
		evas_object_del(popup);
		return NULL;
	}

	elm_object_text_set(btn, D_("IDS_COM_SK_OK"));
	evas_object_data_set(popup, "button", btn);

	elm_object_part_content_set(popup, "button1", btn);
	evas_object_smart_callback_add(btn, "clicked", _response_cb, popup);

	evas_object_size_hint_weight_set(popup, EVAS_HINT_EXPAND, EVAS_HINT_EXPAND);
	elm_object_part_text_set(popup, "title,text", D_("IDS_COM_POP_WARNING"));
	elm_object_text_set(popup, warning);
	evas_object_show(popup);

	return popup;
}



// End of a file
