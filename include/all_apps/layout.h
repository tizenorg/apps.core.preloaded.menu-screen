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

#ifndef _MENU_SCREEN_CONTROLBAR_ALL_APPS_H_
#define _MENU_SCREEN_CONTROLBAR_ALL_APPS_H_

#include <Evas.h>

#define PAGE_MAX_APP 20

extern Evas_Object *all_apps_layout_create(Evas_Object *controlbar, int rotate);
extern void all_apps_layout_destroy(Evas_Object *all_apps);

#endif // _MENU_SCREEN_CONTROLBAR_ALL_APPS_H_

// End of a file
